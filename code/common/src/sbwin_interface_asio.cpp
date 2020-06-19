#include <common/include/sb_interface_asio.h>
#include <common/include/sb_windows_registry.h>

#include <unordered_map>
#include <iostream>

#include "Windows.h"

#pragma comment( lib, "Ole32.lib" )

namespace SB { namespace LibX { namespace ASIO {

static constexpr wchar_t ASIO_PATH[] = L"software\\asio";
static constexpr wchar_t INPROC_SERVER[] = L"InprocServer32";
static constexpr wchar_t ASIODRV_DESC[] = L"description";


static std::wstring GetAsioDllPath(const wchar_t deviceCLSID[])
{
	using SBRegistryKey = SBWindows::RegistryKey;
	std::wstring path;
	if (auto clsidKey = SBRegistryKey(HKEY_CLASSES_ROOT, SBRegistryKey::Read | SBRegistryKey::Query, SBWindows::COM_CLSID))
	{
		if (auto subKey = SBRegistryKey(clsidKey, SBRegistryKey::Read, deviceCLSID))
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

class Adapter_impl : public Adapter
{
public:
	virtual HRESULT __stdcall QueryInterface(char riid[16], void** ppvObject);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

	Adapter_impl( SBWindows::RegistryKey asioDeviceKey, const wchar_t* keyName )
	{
		AddRef();
		GatherDeviceInfo( asioDeviceKey, keyName );
	}
	~Adapter_impl()
	{
		Release();
	}

	DeviceInfo* GetDeviceInfo() { return &deviceInfo; }
	const DeviceInfo* GetDeviceInfo() const { return &deviceInfo; }
	static inline char clsid[16] = { 'S','B',':',':','A','S','I','O',':',':','A','d','a','p','t', };

private:
	bool GatherDeviceInfo( HKEY hkey, const wchar_t* keyname )
	{
		using SBWindows::RegistryKey;
		if (auto clsidKey = RegistryKey(hkey, RegistryKey::Query, keyname))
		{
			auto deviceCLSID = clsidKey.getValue<std::wstring>(SBWindows::COM_CLSID);
			if (!deviceCLSID.empty())
			{
				static_assert( sizeof(CLSID) == sizeof(deviceInfo.uid) );
				CLSIDFromString(deviceCLSID.c_str(), reinterpret_cast<CLSID*>(&deviceInfo.uid));
				//device.path = GetAsioDllPath(deviceCLSID.c_str());
				//if (device)
				//{
				//	auto desc = clsidKey.getValue(ASIODRV_DESC);
				//	if (!desc.empty())
				//	{
				//		deviceInfo.name = desc;
				//	}
				//	else
				//	{
				//		//_wcstrcpy( deviceInfo.description, keyname, std::min(  ) );
				//	}
				//}
			}
		}
	}

private:
	mutable volatile long refCount;
	DeviceInfo deviceInfo;
	wchar_t driverPath[MAX_PATH];
};
HRESULT __stdcall Adapter_impl::QueryInterface([[maybe_unused]] char riid[16], void** ppvObject)
{
	if (ppvObject)
	{
		static_assert(sizeof(clsid) == 16);
		*ppvObject = memcmp(riid, clsid, sizeof(clsid)) == 0 ? this : nullptr;
	}
	return ppvObject && *ppvObject ? S_OK : E_NOINTERFACE;
}
ULONG __stdcall Adapter_impl::AddRef()
{
	auto resultRef = _InterlockedIncrement(&refCount);
	if (resultRef == 1)
	{

	}
	return resultRef;
}
ULONG __stdcall Adapter_impl::Release()
{
	auto resultRef = _InterlockedDecrement(&refCount);
	if (resultRef == 0)
	{
	}
	return resultRef;
}

DeviceInfo GetDeviceInfo(AdapterHandle adapter)
{
	Adapter_impl* ptr = nullptr;
	if( adapter.Get() && adapter->QueryInterface(Adapter_impl::clsid, (void**)&ptr) )
	{
	}
	return ptr ? *ptr->GetDeviceInfo() : DeviceInfo{};
}



class Instance_impl : public Instance
{
public:
	virtual HRESULT __stdcall QueryInterface(char riid[16], void** ppvObject);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

	//AdapterHandle adapter = ptr->CreateAdapter(asioDeviceKey, keyName.c_str());
	AdapterHandle FindAdapter( HKEY hkey, const wchar_t* keyName )
	{
		using SBWindows::RegistryKey;
		if (auto clsidKey = RegistryKey(hkey, RegistryKey::Query, keyName) )
		{
			auto deviceCLSID = clsidKey.getValue<std::wstring>(SBWindows::COM_CLSID);
			if (!deviceCLSID.empty())
			{
				CLSID deviceId;
				CLSIDFromString( deviceCLSID.c_str(), &deviceId );
				__debugbreak();
				//device.path = GetAsioDllPath(deviceCLSID.c_str());
				//if (device)
				//{
				//	auto desc = clsidKey.getValue(ASIODRV_DESC);
				//	if (!desc.empty())
				//	{
				//		deviceInfo.name = desc;
				//	}
				//	else
				//	{
				//		//_wcstrcpy( deviceInfo.description, keyname, std::min(  ) );
				//	}
				//}
			}
		}
		return nullptr;
	}

	static inline char clsid[16] = { 'S','B',':',':','L','i','b','X',':',':','A','S','I','O',':',':', };
private:
	mutable volatile long refCount;

	using CLSID = SBWindows::CLSID;
	using CLSIDHaser = SBWindows::CLSIDHaser;
	std::unordered_map<CLSID, ASIO::Adapter_impl, CLSIDHaser> asioDevices;
};
HRESULT __stdcall Instance_impl::QueryInterface( [[maybe_unused]] char riid[16], void** ppvObject )
{
	if( ppvObject )
	{
		static_assert( sizeof(clsid) == 16 );
		*ppvObject = memcmp( riid, clsid, sizeof(clsid) ) == 0 ? this : nullptr;
	}
	return ppvObject && *ppvObject ? S_OK : E_NOINTERFACE;
}
ULONG __stdcall Instance_impl::AddRef()
{
	auto resultRef = _InterlockedIncrement( &refCount );
	if( resultRef == 1 )
	{
		// initialize COM
		// TODO: once threaded, try using concurrency mode (COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY)
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	}
	return resultRef;
}
ULONG __stdcall Instance_impl::Release()
{
	auto resultRef = _InterlockedDecrement( &refCount );
	if ( resultRef == 0 )
	{
		asioDevices.clear();
		CoUninitialize();
	}
	return resultRef;
}

static Instance_impl s_comInstance;
InstanceHandle CreateInstance( [[maybe_unused]] const Configuration* config )
{
	return InstanceHandle{ &s_comInstance };
}
bool DestroyInstance( [[maybe_unused]] InstanceHandle instance, [[maybe_unused]] const Configuration* config )
{
	// nothing to do
	return true;
}


using adapter_array_t = std::vector<AdapterHandle>;
adapter_array_t EnumerateAdapters( InstanceHandle instance, size_t maxCount )
{
	using SBWindows::RegistryKey;
	adapter_array_t result{};
	if ( Instance_impl* ptr; instance->QueryInterface(Instance_impl::clsid, (void**)&ptr) == S_OK )
	{
		if( maxCount < ~0u )
			result.reserve(maxCount);
		if (auto asioDeviceKey = RegistryKey(HKEY_LOCAL_MACHINE, RegistryKey::Query | RegistryKey::Enumerate, ASIO_PATH))
		{
			for (std::wstring keyName : asioDeviceKey)
			{
				if (!keyName.empty())
				{
					AdapterHandle adapter = ptr->FindAdapter( asioDeviceKey, keyName.c_str() );
					auto deviceInfo = GetDeviceInfo( adapter );
					//if (deviceInfo.deviceID)
					//{
					//	result.emplace_back(deviceInfo);
					//}
					//else
					//{
					//	std::wcerr << "Could not read ASIO device: " << keyName << std::endl;
					//}
				}
			}
		}
	}
	return result;
}
//std::vector<AsioDeviceInfo> EnumerateASIODevices()
//{
//	std::vector<AsioDeviceInfo> deviceList;
//	deviceList.reserve(16);
//	if (auto asioDeviceKey = SBRegistryKey(HKEY_LOCAL_MACHINE, SBRegistryKey::Query | SBRegistryKey::Enumerate, ASIO_PATH))
//	{
//		for (std::wstring keyName : asioDeviceKey)
//		{
//			if (!keyName.empty())
//			{
//				auto device = ReadASIODevice(asioDeviceKey, keyName.c_str());
//				if (device)
//				{
//					deviceList.emplace_back(device);
//				}
//				else
//				{
//					std::wcerr << "Could not read ASIO device: " << keyName << std::endl;
//				}
//			}
//		}
//	}
//	return deviceList;
//}

}}}


//
//
//void ASIOInitialize()
//{
//	// initialize COM
//	// TODO: once threaded, try using concurrency mode (COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY)
//	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
//	_InterlockedIncrement( &s_coInitializeCount );
//}
//
//void ASIOShutdown()
//{
//	assert( s_coInitializeCount > 0 );
//	{
//		for( auto& it : s_asioDevices )
//		{
//			if( it.second )
//			{
//				it.second->stop();
//				it.second.Reset();
//			}
//		}
//		s_asioDevices.clear();
//	}
//	_InterlockedDecrement( &s_coInitializeCount );
//	CoUninitialize();
//}
//
//AsioDriverResult CreateAsioDriver(const AsioDeviceInfo::CLSID& deviceId)
//{
//	if (s_coInitializeCount > 0)
//	{
//		auto it = s_asioDevices.find(deviceId);
//		if (it != s_asioDevices.end())
//		{
//			return AsioDriverResult::AlreadyExists;
//		}
//		else
//		{
//			// TODO: Support CoCreateInstanceEx for remote audio rendering?
//			CLSID clsid = *reinterpret_cast<const CLSID*>(&deviceId);
//			SBAsioHandle handle;
//			long result = CoCreateInstance( clsid, 0, CLSCTX_INPROC_SERVER, clsid, (void**)handle.ReleaseAndGetAddressOf() );
//			if (S_OK == result)
//			{
//				s_asioDevices.insert({deviceId, handle});
//				return AsioDriverResult::Success;
//			}
//			else
//			{
//				return AsioDriverResult::Error_Failed;
//			}
//		}
//	}
//	else
//	{
//		return AsioDriverResult::Error_Unitialized;
//	}
//}
//
//AsioDriverResult ReleaseAsioDriver(const AsioDeviceInfo::CLSID& deviceId)
//{
//	if (s_coInitializeCount > 0)
//	{
//		auto it = s_asioDevices.find(deviceId);
//		if( it != s_asioDevices.end() )
//		{
//			const bool release = it->second.Reset() == 0;
//			s_asioDevices.erase(it);
//			return release ? AsioDriverResult::Success : AsioDriverResult::AlreadyExists;
//		}
//		else
//		{
//			return AsioDriverResult::Error_Failed;
//		}
//	}
//	else
//	{
//		return AsioDriverResult::Error_Unitialized;
//	}
//}
//
//SBAsioHandle QueryInterface(const AsioDeviceInfo& device)
//{
//	SBAsioHandle handle = nullptr;
//	auto it = s_asioDevices.find(device.classID);
//	if (it == s_asioDevices.end())
//	{
//		CreateAsioDriver(device.classID);
//		it = s_asioDevices.find(device.classID);
//	}
//
//	if (it != s_asioDevices.end())
//	{
//		handle = it->second;
//	}
//	return handle;
//}
//
//}}

#include <common/include/sb_common.h>
SB_EXPORT_TYPE int __stdcall asio([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	using namespace SB::LibX::ASIO;
	auto asioInstance = CreateInstance();
	auto deviceInfoArray = EnumerateAdapters(asioInstance);
	return 0;
}