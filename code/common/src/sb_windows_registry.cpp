#include <common/include/sb_windows_registry.h>
#include <common/include/sb_common.h>

#include <unordered_map>

#pragma comment( lib, "Advapi32.lib" )


#define assert(X)\
	do{ if (!(X)) __debugbreak(); } while(false)

namespace SB { namespace LibX { namespace Windows {

static constexpr wchar_t INPROC_SERVER[] = L"InprocServer32";
static constexpr wchar_t ASIODRV_DESC[] = L"description";


RegistryKey::RegistryKey(HKEY parentKey, AccessMask access, const char* keyname)
{
	if (ERROR_SUCCESS != RegOpenKeyExA(parentKey, keyname, 0, access, &hkey))
	{
		hkey = nullptr;
	}
}
RegistryKey::RegistryKey(const RegistryKey& parentKey, AccessMask access, const char* keyname)
{
	if (ERROR_SUCCESS != RegOpenKeyExA(parentKey, keyname, 0, access, &hkey))
	{
		hkey = nullptr;
	}
}
RegistryKey::RegistryKey(HKEY parentKey, AccessMask access, const wchar_t* keyname)
{
	if (ERROR_SUCCESS != RegOpenKeyExW(parentKey, keyname, 0, access, &hkey))
	{
		hkey = nullptr;
	}
}
RegistryKey::RegistryKey(const RegistryKey& parentKey, AccessMask access, const wchar_t* keyname)
{
	if (ERROR_SUCCESS != RegOpenKeyExW(parentKey, keyname, 0, access, &hkey))
	{
		hkey = nullptr;
	}
}

RegistryKey::~RegistryKey()
{
	if (hkey && (reinterpret_cast<uintptr_t>(hkey) & reinterpret_cast<uintptr_t>(HKEY_CLASSES_ROOT)) == 0)
	{
		RegCloseKey(hkey);
		hkey = nullptr;
	}
}

DWORD RegistryKey::getValue_internal( const wchar_t* keyname, DWORD dataType, void* data, DWORD dataSize )
{
	if( dataSize > 0 )
	{
		RegQueryValueExW( hkey, keyname, nullptr, &dataType, static_cast<uint8_t*>(data), &dataSize );
	}
	else
	{
		RegQueryValueExW( hkey, keyname, nullptr, nullptr, nullptr, &dataSize );
	}
	return dataSize;
}


RegistryKeyIterator::RegistryKeyIterator(HKEY key)
	: hkey(key), index(0), maxNameCharCount(0)
{
	RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL, &maxNameCharCount, NULL, NULL, NULL, NULL, NULL, NULL);
	++maxNameCharCount;
}

std::wstring RegistryKeyIterator::operator *() const
{
	DWORD nameCharCount = maxNameCharCount;
	std::wstring value;
	value.resize(maxNameCharCount);
	//if (ERROR_SUCCESS != RegEnumKeyExW(hkey, index, &value[0], &nameCharCount, NULL, NULL, NULL, NULL))
	if (ERROR_SUCCESS != RegEnumKeyExW(hkey, index, value.data(), &nameCharCount, NULL, NULL, NULL, NULL))
	{
		index = -1;
		nameCharCount = 0;
	}
	value.resize(nameCharCount);
	return value;
}


}}}
