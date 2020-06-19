#include <common/include/sb_interface_asio.h>
#if 0
enum COINIT
{
	COINIT_APARTMENTTHREADED = 0x2,      // Apartment model

#if  (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
  // These constants are only valid on Windows NT 4.0
  COINIT_MULTITHREADED = COINITBASE_MULTITHREADED,
  COINIT_DISABLE_OLE1DDE = 0x4,      // Don't use DDE for Ole1 support.
  COINIT_SPEED_OVER_MEMORY = 0x8,      // Trade memory for speed.
#endif // DCOM
};

#pragma comment( lib, "Ole32.lib" )
extern "C" long __stdcall CoInitializeEx( void* pvReserved, unsigned long dwCoInit );
extern "C" void __stdcall CoUninitialize();

namespace SB { namespace LibX { namespace ASIO {



class Instance_impl : public Instance
{
public:
	virtual HRESULT __stdcall QueryInterface(char riid[16], void** ppvObject);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
private:
	mutable volatile long refCount;
};
HRESULT __stdcall Instance_impl::QueryInterface( [[maybe_unused]] char riid[16], void** ppvObject )
{
	if( ppvObject )
	{
		*ppvObject = nullptr;
	}
	return 0;
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
		// TODO: Asio cleanup
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

//using adapter_array_t = std::vector<AdapterHandle>;
//adapter_array_t EnumerateAdapters(InstanceHandle instance, size_t maxCount);

}}}

//InstanceHandle CreateInstance(const Configuration* config = nullptr);
//bool DestroyInstance(InstanceHandle instance, const Configuration* config = nullptr);
//
//using adapter_array_t = std::vector<AdapterHandle>;
//adapter_array_t EnumerateAdapters(InstanceHandle instance, size_t maxCount = ~0u);
//
//struct DeviceInfo
//{
//	static constexpr size_t kDescSize = 256;
//	static constexpr size_t kIIDSize = 16;
//	char     description[kDescSize];
//	uint32_t vendorID;
//	uint32_t deviceID;
//	uint32_t apiID;
//	uint32_t driverVersion;
//	uint8_t  uid[16];
//};
//DeviceInfo GetDeviceInfo(AdapterHandle adapter);
//
//DeviceHandle CreateDevice(AdapterHandle adapter, const Configuration* config = nullptr);
//bool DestroyDevice(DeviceHandle device, const Configuration* config = nullptr);
//
//struct instance
//{
//	// TODO : should be ref counted
//	instance(const Configuration* config = nullptr) { handle = CreateInstance(config); }
//	instance(instance&& other) { handle = other.handle; other.handle = InstanceHandle{}; }
//	~instance() { DestroyInstance(handle); }
//
//	operator InstanceHandle() const { return handle; }
//public:
//	InstanceHandle handle;
//};
//
//struct device
//{
//	// TODO : should be ref counted
//	explicit device(AdapterHandle adapter = {}, const Configuration* config = nullptr) { handle = CreateDevice(adapter, config); }
//	explicit device(device&& other) { handle = other.handle; other.handle = DeviceHandle{}; }
//	~device() { DestroyDevice(handle); }
//
//	operator DeviceHandle() const { return handle; }
//public:
//	DeviceHandle handle;
//};



//#include "SBAsioDevice.h"
//
//#include "Windows.h"
//#include "malloc.h"
//
////#include "iasiodrv.h"
//
//#include <iostream>
//#include <algorithm>
//#include <unordered_map>
//
//#include <emmintrin.h>
//
//namespace SB { namespace Audio {
//
//static constexpr wchar_t ASIO_PATH[] = L"software\\asio";
//static constexpr wchar_t COM_CLSID[] = L"clsid";
//static constexpr wchar_t INPROC_SERVER[] = L"InprocServer32";
//static constexpr wchar_t ASIODRV_DESC[] = L"description";
//
//// This is an horrible macro...
//// At least it is as safe as it can be (can return nullptr if invalid)
//// since align will most probably be known at compile time, all of this should be well optimized in the end, cleaning up the mess.
//// Note that for _alloca to work, it can't be wrapped in a function so not much other choice to have something that will not corrupt the stack.
////#define SB_ALLOCA_BUFFER_ALIGN( type, size, align )	(align == 0 || (align & (align - 1)) != 0) ? nullptr : reinterpret_cast<type*>((reinterpret_cast<uintptr_t>(_alloca(size + (std::max<size_t>(align, alignof(type)) - 1))) + (std::max<uintptr_t>(align, alignof(type)) - 1)) & ~(std::max<uintptr_t>(align, alignof(type)) - 1))
////#define SB_ALLOCA_ARRAY_ALIGN( type, count, align )	SB_ALLOCA_BUFFER_ALIGN( type, count * sizeof(type), std::max<uintptr_t>(align, alignof(type)) )
////#define SB_ALLOCA_BUFFER( type, size )				SB_ALLOCA_BUFFER_ALIGN( type, size, alignof(type) )
////#define SB_ALLOCA_ARRAY( type, count )				SB_ALLOCA_ARRAY_ALIGN( type, count, alignof(type) )
//
//#ifndef assert
//#define assert(X) \
//	if (!(X)) \
//	{ \
//		__debugbreak(); \
//	}
//#endif // #ifndef assert
//
//static volatile long s_coInitializeCount = false;
//
////
//// CLSID
////
//bool operator ==(const AsioDeviceInfo::CLSID& clsidA, const AsioDeviceInfo::CLSID& clsidB)
//{
//	return memcmp(&clsidA, &clsidB, sizeof(clsidA)) == 0;
//}
//struct CLSIDHaser
//{
//	size_t operator ()(const AsioDeviceInfo::CLSID& clsid) const
//	{
//		static_assert(sizeof(AsioDeviceInfo::CLSID) == sizeof(__m128i), "");
//		return *reinterpret_cast<const size_t*>(&clsid);
//	}
//};
//static std::unordered_map<AsioDeviceInfo::CLSID, SBAsioHandle, CLSIDHaser> s_asioDevices;
//
////
//// Registry
////
//class SBRegistryKey
//{
//public:
//	enum AccessMask : DWORD
//	{
//		Query = KEY_QUERY_VALUE,
//		Read = KEY_READ,
//		Enumerate = KEY_ENUMERATE_SUB_KEYS,
//	};
//
//	SBRegistryKey(const SBRegistryKey& key) = delete;
//	SBRegistryKey(SBRegistryKey&& key)
//		: hkey(key)
//	{
//		key.hkey = nullptr;
//	}
//
//	SBRegistryKey(HKEY parentKey, AccessMask access, const wchar_t* keyname = nullptr)
//	{
//		if (ERROR_SUCCESS != RegOpenKeyExW(parentKey, keyname, 0, access, &hkey))
//		{
//			hkey = nullptr;
//		}
//	}
//	SBRegistryKey(const SBRegistryKey& parentKey, AccessMask access, const wchar_t* keyname = nullptr)
//	{
//		if (ERROR_SUCCESS != RegOpenKeyExW(parentKey, keyname, 0, access, &hkey))
//		{
//			hkey = nullptr;
//		}
//	}
//
//	~SBRegistryKey()
//	{
//		if (hkey && (reinterpret_cast<uintptr_t>(hkey) & reinterpret_cast<uintptr_t>(HKEY_CLASSES_ROOT)) == 0)
//		{
//			RegCloseKey(hkey);
//			hkey = nullptr;
//		}
//	}
//
//	operator bool() const { return hkey; }
//	operator HKEY() const { return hkey; }
//
//	template<typename type = std::wstring, DWORD regDataTypeId = REG_SZ>
//	type getValue(const wchar_t* keyname = nullptr)
//	{
//		DWORD dataType = regDataTypeId;
//		DWORD maxDataSize = 0;
//		RegQueryValueExW(hkey, keyname, nullptr, nullptr, nullptr, &maxDataSize);
//
//		type value;
//		if (maxDataSize > 0)
//		{
//			value.resize((maxDataSize + sizeof(wchar_t) - 1) / sizeof(wchar_t));
//			DWORD dataSize = maxDataSize;
//			unsigned char* data = reinterpret_cast<unsigned char*>(&value[0]);
//			RegQueryValueExW(hkey, keyname, nullptr, &dataType, data, &dataSize);
//			assert(dataSize <= maxDataSize);
//			assert(dataType != REG_SZ || value[(dataSize + sizeof(wchar_t) - 1) / sizeof(wchar_t) - 1] == L'\0');
//			value.resize((dataSize + sizeof(wchar_t) - 1) / sizeof(wchar_t));
//		}
//		return value;
//	}
//
//private:
//	HKEY hkey;
//};
//inline SBRegistryKey::AccessMask operator|(SBRegistryKey::AccessMask a, SBRegistryKey::AccessMask b) { return static_cast<SBRegistryKey::AccessMask>(static_cast<DWORD>(a) | static_cast<DWORD>(b)); }
//
//class SBRegistryKeyIterator
//{
//public:
//	SBRegistryKeyIterator(HKEY key)
//		: hkey(key), index(0), maxNameCharCount(0)
//	{
//		RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL, &maxNameCharCount, NULL, NULL, NULL, NULL, NULL, NULL);
//		++maxNameCharCount;
//	}
//
//	std::wstring operator *() const
//	{
//		DWORD nameCharCount = maxNameCharCount;
//		std::wstring value;
//		value.resize(maxNameCharCount);
//		if (ERROR_SUCCESS != RegEnumKeyExW(hkey, index, &value[0], &nameCharCount, NULL, NULL, NULL, NULL))
//		{
//			index = -1;
//			nameCharCount = 0;
//		}
//		value.resize(nameCharCount);
//		return value;
//	}
//
//	SBRegistryKeyIterator& operator ++()
//	{
//		if (index >= 0)
//			++index;
//		return *this;
//	}
//	bool operator ==(const SBRegistryKeyIterator & other) const
//	{
//		return hkey == other.hkey && index == other.index;
//	}
//	bool operator !=(const SBRegistryKeyIterator & other) const
//	{
//		return hkey != other.hkey || index != other.index;
//	}
//
//	static const SBRegistryKeyIterator end(HKEY key)
//	{
//		return SBRegistryKeyIterator(key, -1, 0);
//	}
//private:
//	explicit SBRegistryKeyIterator(HKEY key, int index, DWORD maxNameCharCount)
//		: hkey(key), index(index), maxNameCharCount(maxNameCharCount)
//	{
//	}
//
//	HKEY hkey;
//	DWORD maxNameCharCount = 0;
//	mutable int index = 0;
//};
//
//SBRegistryKeyIterator begin(const HKEY hkey)
//{
//	return SBRegistryKeyIterator(hkey);
//}
//SBRegistryKeyIterator end(const HKEY hkey)
//{
//	return SBRegistryKeyIterator::end(hkey);
//}
//
//
//std::wstring GetAsioDllPath(const wchar_t deviceCLSID[])
//{
//	std::wstring path;
//	if (auto clsidKey = SBRegistryKey(HKEY_CLASSES_ROOT, SBRegistryKey::Read | SBRegistryKey::Query, COM_CLSID))
//	{
//		if (auto subKey = SBRegistryKey(clsidKey, SBRegistryKey::Read, deviceCLSID))
//		{
//			if (auto pathKey = SBRegistryKey(subKey, SBRegistryKey::Query, INPROC_SERVER))
//			{
//				auto value = pathKey.getValue();
//				if (!value.empty())
//				{
//					HANDLE dll = CreateFileW(value.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//					if (dll != INVALID_HANDLE_VALUE)
//					{
//						CloseHandle(dll);
//						path = value;
//					}
//				}
//			}
//		}
//	}
//	return path;
//}
//
//AsioDeviceInfo ReadASIODevice(HKEY hkey, const wchar_t* keyname)
//{
//	AsioDeviceInfo device = {};
//	if (auto clsidKey = SBRegistryKey(hkey, SBRegistryKey::Query, keyname))
//	{
//		auto deviceCLSID = clsidKey.getValue(COM_CLSID);
//		if (!deviceCLSID.empty())
//		{
//			CLSIDFromString(deviceCLSID.c_str(), reinterpret_cast<CLSID*>(&device.classID));
//			device.path = GetAsioDllPath(deviceCLSID.c_str());
//			if (device)
//			{
//				auto desc = clsidKey.getValue(ASIODRV_DESC);
//				if (!desc.empty())
//				{
//					device.name = desc;
//				}
//				else
//				{
//					device.name = keyname;
//				}
//			}
//		}
//	}
//	return device;
//}
//
//
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
#endif
