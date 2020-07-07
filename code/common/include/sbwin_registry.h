#pragma once

#include <string>

#include "Windows.h"
#include <wrl/client.h>
#include <combaseapi.h>

namespace SB { namespace LibX { namespace Windows
{

class RegistryKey
{
public:
	enum AccessMask : DWORD
	{
		Query = KEY_QUERY_VALUE,
		Read = KEY_READ,
		Enumerate = KEY_ENUMERATE_SUB_KEYS,
	};
	enum KeyType : DWORD
	{
		KeyType_NONE                    = ( 0ul ), // No value type
		KeyType_SZ                      = ( 1ul ), // Unicode nul terminated string
		KeyType_EXPAND_SZ               = ( 2ul ), // Unicode nul terminated string
		// (with environment variable references)
		KeyType_BINARY                  = ( 3ul ), // Free form binary
		KeyType_DWORD                   = ( 4ul ), // 32-bit number
		KeyType_DWORD_LITTLE_ENDIAN     = ( 4ul ), // 32-bit number (same as REG_DWORD)
		KeyType_DWORD_BIG_ENDIAN        = ( 5ul ), // 32-bit number
		KeyType_LINK                    = ( 6ul ), // Symbolic Link (unicode)
		KeyType_MULTI_SZ                = ( 7ul ), // Multiple Unicode strings
		KeyType_RESOURCE_LIST           = ( 8ul ), // Resource list in the resource map
		KeyType_FULL_RESOURCE_DESCRIPTOR = ( 9ul ), // Resource list in the hardware description
		KeyType_RESOURCE_REQUIREMENTS_LIST = ( 10ul ),
		KeyType_QWORD                   = ( 11ul ), // 64-bit number
		KeyType_QWORD_LITTLE_ENDIAN     = ( 11ul ), // 64-bit number (same as REG_QWORD)
	};

	RegistryKey(const RegistryKey& key) = delete;
	RegistryKey(RegistryKey&& key) = default;

	RegistryKey(HKEY parentKey, AccessMask access, const char* keyname = nullptr);
	RegistryKey(const RegistryKey& parentKey, AccessMask access, const char* keyname = nullptr);
	RegistryKey(HKEY parentKey, AccessMask access, const wchar_t* keyname = nullptr);
	RegistryKey(const RegistryKey& parentKey, AccessMask access, const wchar_t* keyname = nullptr);

	~RegistryKey();

	operator bool() const { return hkey; }
	operator HKEY() const { return hkey; }

	template<typename type = std::wstring, KeyType regDataTypeId = KeyType_SZ>
	type getValue( const wchar_t* keyname = nullptr, KeyType dataType = KeyType_SZ )
	{
		DWORD maxDataSize = getValue_internal( keyname, static_cast<DWORD>(dataType) );

		type value;
		value.resize((maxDataSize + sizeof(wchar_t) - 1) / sizeof(wchar_t));
		if( maxDataSize > 0 )
		{
			DWORD dataSize = getValue_internal( keyname, static_cast<DWORD>(dataType), value.data(), maxDataSize );
			value.resize((dataSize + sizeof(wchar_t) - 1) / sizeof(wchar_t));
		}
		return value;
	}

private:
	DWORD getValue_internal( const wchar_t* keyname, DWORD dataType, void* data = nullptr, DWORD dataSize = 0u );

	HKEY hkey;
};
static inline constexpr RegistryKey::AccessMask operator|(RegistryKey::AccessMask a, RegistryKey::AccessMask b) { return static_cast<RegistryKey::AccessMask>(static_cast<DWORD>(a) | static_cast<DWORD>(b)); }

class RegistryKeyIterator
{
public:
	RegistryKeyIterator(HKEY key);

	std::wstring operator *() const;

	RegistryKeyIterator& operator ++()
	{
		if (index >= 0)
			++index;
		return *this;
	}
	const RegistryKeyIterator& operator ++() const
	{
		if (index >= 0)
			++index;
		return *this;
	}
	bool operator ==(const RegistryKeyIterator& other) const
	{
		return hkey == other.hkey && index == other.index;
	}
	bool operator !=(const RegistryKeyIterator& other) const
	{
		return hkey != other.hkey || index != other.index;
	}

	static inline RegistryKeyIterator end(HKEY key)
	{
		return RegistryKeyIterator(key, -1, 0);
	}
private:
	explicit RegistryKeyIterator(HKEY key, int index, DWORD maxNameCharCount)
		: hkey(key), index(index), maxNameCharCount(maxNameCharCount)
	{
	}

	HKEY hkey;
	DWORD maxNameCharCount = 0;
	mutable int index = 0;
};

static inline RegistryKeyIterator begin(const HKEY hkey)
{
	return RegistryKeyIterator(hkey);
}
static inline RegistryKeyIterator end(const HKEY hkey)
{
	return RegistryKeyIterator::end(hkey);
}

}
}}
namespace SBWindows = SB::LibX::Windows;
namespace SBWin32 = SBWindows;
