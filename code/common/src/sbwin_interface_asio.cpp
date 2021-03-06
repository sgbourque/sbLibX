#include <common/include/sb_interface_asio.h>
#include <common/include/sbwin_registry.h>

#include <unordered_map>
#include <iostream>

#include "Windows.h"

#include <system_error>
#include <cassert>

#pragma comment( lib, "Ole32.lib" )

namespace SB { namespace LibX { namespace ASIO {

using result_t = IUnknown::result_t;
using refcount_t = IUnknown::refcount_t;
using GUID = IUnknown::GUID;

using CLSID = ::CLSID;

static inline bool operator ==(const GUID& idA, const GUID& idB)
{
	return memcmp(idA.uid, idB.uid, sizeof(GUID)) == 0;
}
static inline bool operator ==(const CLSID& idA, const CLSID& idB)
{
	return memcmp(&idA, &idB, sizeof(CLSID)) == 0;
}
struct GUIDHasher
{
	inline size_t operator ()(const GUID& id) const
	{
		return id.uid64._0;
	}
	inline size_t operator ()(const CLSID& id) const
	{
		return *reinterpret_cast<const size_t*>(id.Data4);
	}
};

	template<typename base_t, typename derived_t>
result_t QueryInterface(base_t* base, derived_t** derived)
{
	return base->QueryInterface(derived_t::clsid, (void**)derived);
}

	template<typename _base_t_>
struct RefCountedImpl : _base_t_
{
	virtual refcount_t SB_STDCALL AddRef() override = 0;
	virtual refcount_t SB_STDCALL Release() override = 0;

protected:
	mutable volatile long ref_count = 0;
};

	template<typename _type_t_, typename _base_t_>
struct RefClassImpl : RefCountedImpl<_base_t_>
{
	virtual result_t SB_STDCALL QueryInterface(GUID riid, void** ppvObject) override
	{
		if( !ppvObject )
			return E_INVALIDARG;

		static_assert(sizeof(_type_t_::clsid) == 16);
		if ( memcmp(riid.uid, &_type_t_::clsid, sizeof(_type_t_::clsid)) == 0 )
			*ppvObject = static_cast<_type_t_*>( this );
		return *ppvObject ? S_OK : E_NOINTERFACE;
	}

	virtual refcount_t SB_STDCALL AddRef() override
	{
		// AddRef/Release can be called concurrently on multiple threads with init/shutdown called only once at a time on a single object
		refcount_t refCount;
		do 
		{
			refCount = _InterlockedExchange(&ref_count, -1);
			if (refCount == 0)
			{
				static_cast<_type_t_*>(this)->init();
				break;
			}
			else if( refCount < 0 )
			{
				_mm_pause();
			}
		} while ( refCount < 0 );
		_InterlockedExchange(&ref_count, refCount + 1);
		return refCount + 1;
	}
	virtual refcount_t SB_STDCALL Release() override
	{
		// AddRef/Release can be called concurrently on multiple threads with init/shutdown called only once at a time on a single object
		refcount_t refCount;
		do
		{
			refCount = _InterlockedExchange(&ref_count, -1);
			if (refCount == 1)
			{
				static_cast<_type_t_*>(this)->shutdown();
				break;
			}
			else if( refCount < 0 )
			{
				_mm_pause();
			}
		}
		while (refCount < 0);
		assert( refCount != 0 ); // this would mean it was already released
		_InterlockedExchange(&ref_count, refCount - 1);
		return refCount - 1;
	}
};

//////////////////////////////////////////////////////////////////////////
static std::wstring GetAsioDllPath( CLSID deviceId );
struct AdapterImpl1 : RefClassImpl<AdapterImpl1, Adapter>
{
	virtual result_t SB_STDCALL QueryInterface(GUID riid, void** ppvObject) override
	{
		result_t result = RefClassImpl::QueryInterface(riid, ppvObject);
		if (result < 0)
		{
			if ( memcmp(riid.uid, &deviceDesc.uid, sizeof(riid)) == 0 )
				*ppvObject = handle.Get();
			
		}
		return result;
	}

	bool init()
	{
		return true;
	}
	void shutdown()
	{
		handle = nullptr;
	}

	AdapterImpl1( CLSID deviceId, std::wstring_view desc )
	{
		memcpy( deviceDesc.uid, &deviceId, sizeof(DeviceDesc::uid) );
		WideCharToMultiByte( CP_UTF8, 0, desc.data(), (int)desc.size(), deviceDesc.name, sizeof(deviceDesc.name), nullptr, nullptr );
		auto path = GetAsioDllPath( deviceId );
		memcpy( driverPath, path.data(), path.size() * sizeof(*path.data()) );
	}

	virtual void GetDeviceDesc( DeviceDesc* desc ) const override
	{
		if( desc )
		{
			memcpy( desc, &deviceDesc, sizeof(deviceDesc) );
		}
	}

public:
	static inline GUID clsid = { 0x029ABF3D, 0x8B08, 0x4A4D, { 0x9F,0x0E,0xF5,0x41,0x72,0xB0,0x71,0x44, } };

	static constexpr size_t kDescSize = 256;
	static constexpr size_t kIIDSize = 16;
	DeviceDesc deviceDesc{};
	wchar_t  driverPath[MAX_PATH]{};
	DeviceHandle handle;
};

struct InstanceImpl1 : RefClassImpl<InstanceImpl1, Instance>
{
	virtual result_t SB_STDCALL QueryInterface( GUID riid, void** ppvObject ) override
	{
		result_t result = RefClassImpl::QueryInterface(riid, ppvObject);
		if( result < 0 )
		{
			// pass to children
			for( auto adapter : asioAdapters )
			{
				if (memcmp(riid.uid, &adapter.second.deviceDesc.uid, sizeof(riid)) == 0)
				{
					*ppvObject = adapter.second.handle.Get();
					break;
				}
			}
		}
		return result;
	}

	__declspec(noinline) bool init()
	{
		// initialize COM : ASIO requires appartment model
#if SB_TARGET_TYPE == SB_TARGET_TYPE_STATIC
		auto appartmentThreadedResult = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
#else
		constexpr HRESULT appartmentThreadedResult = S_FALSE;
#endif
		return SUCCEEDED( appartmentThreadedResult );
	}
	void shutdown()
	{
#if SB_TARGET_TYPE == SB_TARGET_TYPE_STATIC
		CoUninitialize();
#endif
	}

	AdapterHandle AllocateAdapter( CLSID deviceId, std::wstring_view keyname )
	{
		auto it = asioAdapters.emplace( std::make_pair( deviceId, AdapterImpl1{ deviceId, keyname } ) );
		return &it.first->second;
	}

public:
	static inline GUID clsid = { 0x029ABF3D, 0x8B08, 0x4A4D, { 0x9F,0x0E,0xF5,0x41,0x72,0xB0,0x71,0x44, } };

	friend SB_WIN_EXPORT InstanceHandle CreateInstance([[maybe_unused]] const Configuration* config);
	friend SB_WIN_EXPORT bool DestroyInstance([[maybe_unused]] InstanceHandle instance, [[maybe_unused]] const Configuration* config);
	static InstanceImpl1 sInstance;

public:
	std::unordered_map<CLSID, AdapterImpl1, GUIDHasher> asioAdapters;
};
InstanceImpl1 InstanceImpl1::sInstance{};

//////////////////////////////////////////////////////////////////////////
SB_WIN_EXPORT InstanceHandle CreateInstance([[maybe_unused]] const Configuration* config)
{
	return &InstanceImpl1::sInstance;
}
SB_WIN_EXPORT bool DestroyInstance([[maybe_unused]] InstanceHandle instance, [[maybe_unused]] const Configuration* config)
{
	instance = nullptr;
	return ( InstanceImpl1::sInstance.ref_count == 0 );
}


auto FindDeviceCLSID( HKEY hkey, std::wstring_view keyName )
{
	static constexpr wchar_t COM_CLSID[] = L"clsid";
	CLSID deviceId{};
	using SBWindows::RegistryKey;
	if (auto clsidKey = RegistryKey(hkey, RegistryKey::Query, keyName.data()))
	{
		auto deviceCLSID = clsidKey.getValue<std::wstring>(COM_CLSID);
		if (!deviceCLSID.empty())
		{
			CLSIDFromString(deviceCLSID.c_str(), reinterpret_cast<LPCLSID>(&deviceId));
		}
	}
	return deviceId;
}


using adapter_array_t = std::vector<AdapterHandle>;
SB_WIN_EXPORT adapter_array_t EnumerateAdapters(InstanceHandle instance, size_t maxCount)
{
	static constexpr wchar_t ASIO_PATH[] = L"software\\asio";
	using instance_t = InstanceImpl1;
	using SBWindows::RegistryKey;
	adapter_array_t result{};
	if (instance_t* ptr; QueryInterface(instance.Get(), &ptr) == S_OK)
	{
		if (maxCount < ~0u)
			result.reserve(maxCount);
		if (auto asioDeviceKey = RegistryKey(HKEY_LOCAL_MACHINE, RegistryKey::Query | RegistryKey::Enumerate, ASIO_PATH))
		{
			for (std::wstring keyName : asioDeviceKey)
			{
				if (!keyName.empty())
				{
					CLSID deviceId = FindDeviceCLSID( asioDeviceKey, keyName );
					result.emplace_back( ptr->AllocateAdapter( deviceId, keyName ) );
				}
			}
		}
	}
	return result;
}


static std::wstring GetAsioDllPath( CLSID deviceId )
{
	wchar_t idName[64] = {};
	StringFromGUID2( deviceId, idName, sizeof(idName)/sizeof(*idName) );

	static constexpr wchar_t COM_CLSID[] = L"clsid";
	static constexpr wchar_t INPROC_SERVER[] = L"InprocServer32";
	static constexpr wchar_t ASIODRV_DESC[] = L"description";
	using SBRegistryKey = SBWindows::RegistryKey;
	std::wstring path;
	if (auto clsidKey = SBRegistryKey(HKEY_CLASSES_ROOT, SBRegistryKey::Read | SBRegistryKey::Query, COM_CLSID))
	{
		if (auto subKey = SBRegistryKey(clsidKey, SBRegistryKey::Read, idName))
		{
			if (auto pathKey = SBRegistryKey(subKey, SBRegistryKey::Query, INPROC_SERVER))
			{
				auto value = pathKey.getValue();
				if (!value.empty())
				{
					HANDLE dll = CreateFileW(value.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (dll != INVALID_HANDLE_VALUE)
					{
						CloseHandle(dll);
						path = value;
					}
				}
			}
		}
	}
	return path;
}

SB_WIN_EXPORT DeviceDesc GetDeviceDesc( AdapterHandle adapter )
{
	DeviceDesc deviceDesc{};
	adapter->GetDeviceDesc( &deviceDesc );
	return deviceDesc;
}

SB_WIN_EXPORT DeviceHandle CreateDevice([[maybe_unused]] AdapterHandle adapter, [[maybe_unused]] const Configuration* config)
{
	DeviceHandle handle{};
	if( AdapterImpl1* ptr; QueryInterface(adapter.Get(), &ptr) == S_OK )
	{
		const CLSID& clsid = *reinterpret_cast<CLSID*>(ptr->deviceDesc.uid);
		[[maybe_unused]] auto result = CoCreateInstance( clsid, 0, CLSCTX_INPROC_SERVER, clsid, handle.ReleaseAndGetAddressOf<void>() );
		if(result != S_OK || !handle->init( GetCurrentProcess() ))
		{
			std::clog << "Error initializing device: code " << std::hex << result << " (error = " << GetLastError() << ")" << std::endl;
			handle = nullptr;
		}
		else
		{
			ptr->handle = handle;
		}
	}
	return handle;
}
SB_WIN_EXPORT bool DestroyDevice(DeviceHandle device, [[maybe_unused]] const Configuration* config)
{
	refcount_t refCount = ~0u;
	if( device )
	{
		// Make sure to stop the ASIO device before it is destroyed
		// Any open (weak) device handle will still be valid (until refCount reach 0) but audio processing will have been stopped.
		device->stop();
		device->disposeBuffers();
		refCount = device.Release();
	}
	return refCount == 0;
}

}}}
