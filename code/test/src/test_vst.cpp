#include <cstdint>
#include <cassert>

#include <algorithm>

////
#if defined(SBWIN64) || defined(SBWIN32)
#define NOMINMAX
//#pragma warning(disable:4668 5039 4339)
#include <windows.h>
//#pragma warning(default)
#ifndef WINAPI
	#define WINAPI SB_STDCALL
#endif
#endif

namespace SB { namespace LibX {

struct DosBinaryHeader
{
	union magicNumber_t {
		uint8_t  signature[2];
		uint16_t value;
	};
	static inline constexpr magicNumber_t signature = { 'M', 'Z' };

	uint16_t   magicNumber;
	uint16_t   lastPageSize;
	uint16_t   pageCount;
	uint16_t   relocationCount;
	uint16_t   headersSize;
	uint16_t   e_minExtraAlloc;
	uint16_t   e_maxExtraAlloc;
	uint16_t   initialStackSize;
	uint16_t   initialStackPointer;
	uint16_t   checksum;
	uint16_t   initialInstructionPointer;
	uint16_t   initialCodeSegment;
	uint16_t   relocationTableOffset;
	uint16_t   overlayNumber;
	uint16_t   reserved[4];
	uint16_t   oemId;
	uint16_t   oemInfo;
	uint16_t   reserved2[10];
	LONG   newHeaderOffset;
};


//typedef struct _IMAGE_FILE_HEADER {
//	WORD    Machine;
//	WORD    NumberOfSections;
//	DWORD   TimeDateStamp;
//	DWORD   PointerToSymbolTable;
//	DWORD   NumberOfSymbols;
//	WORD    SizeOfOptionalHeader;
//	WORD    Characteristics;
//} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;
//typedef struct _IMAGE_OPTIONAL_HEADER64 {
//	WORD        Magic;
//	BYTE        MajorLinkerVersion;
//	BYTE        MinorLinkerVersion;
//	DWORD       SizeOfCode;
//	DWORD       SizeOfInitializedData;
//	DWORD       SizeOfUninitializedData;
//	DWORD       AddressOfEntryPoint;
//	DWORD       BaseOfCode;
//	ULONGLONG   ImageBase;
//	DWORD       SectionAlignment;
//	DWORD       FileAlignment;
//	WORD        MajorOperatingSystemVersion;
//	WORD        MinorOperatingSystemVersion;
//	WORD        MajorImageVersion;
//	WORD        MinorImageVersion;
//	WORD        MajorSubsystemVersion;
//	WORD        MinorSubsystemVersion;
//	DWORD       Win32VersionValue;
//	DWORD       SizeOfImage;
//	DWORD       SizeOfHeaders;
//	DWORD       CheckSum;
//	WORD        Subsystem;
//	WORD        DllCharacteristics;
//	ULONGLONG   SizeOfStackReserve;
//	ULONGLONG   SizeOfStackCommit;
//	ULONGLONG   SizeOfHeapReserve;
//	ULONGLONG   SizeOfHeapCommit;
//	DWORD       LoaderFlags;
//	DWORD       NumberOfRvaAndSizes;
//	//#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16
//	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
//} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;
//typedef struct _IMAGE_NT_HEADERS64 {
//	DWORD Signature;
//	IMAGE_FILE_HEADER FileHeader;
//	IMAGE_OPTIONAL_HEADER64 OptionalHeader;
//} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

class export_function_iterator
{
public:
	export_function_iterator( const DosBinaryHeader* module, uint32_t hint = 0 )
	{
		if( module )
		{
			assert( !module || module->magicNumber == DosBinaryHeader::signature.value );
			rawModule = reinterpret_cast<const char*>(module);

			auto header = reinterpret_cast<const IMAGE_NT_HEADERS*>( rawModule + module->newHeaderOffset );
			assert( header->Signature == IMAGE_NT_SIGNATURE );
			assert( header->OptionalHeader.NumberOfRvaAndSizes > 0 );
			auto exports = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(
				rawModule + header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress
			);
			namesOffset = reinterpret_cast<const uint32_t*>( rawModule + exports->AddressOfNames );
			namesOffsetEnd = namesOffset + exports->NumberOfNames;

			hint = std::min<uint32_t>( hint, exports->NumberOfNames );
			namesOffset += hint;
		}
	}

	export_function_iterator operator ++() { ++namesOffset; return *this; }
	const char* operator *() const { return namesOffset && namesOffset < namesOffsetEnd ? rawModule + *namesOffset : nullptr; }

public:
	const uint32_t* namesOffset;
	const uint32_t* namesOffsetEnd;
	const char* rawModule;
};
bool operator ==(export_function_iterator& a, export_function_iterator& b)
{
	return a.rawModule == b.rawModule && a.namesOffset == b.namesOffset;
}

struct unique_library
{
	using result_t = int32_t;
	using handle_t = DosBinaryHeader*;

	explicit unique_library( const char* libName = nullptr ) { if( libName ) load( libName ); }
	explicit unique_library( const wchar_t* libName = nullptr ) { if( libName ) load( libName ); }
	explicit unique_library( unique_library&& other ) { handle = other.Detach(); }
	explicit unique_library( const unique_library& other ) = delete;
	~unique_library() { Release(); }

	handle_t Detach() { return std::move( handle ); }
	void Release() { if( handle ) unload( std::move( handle ) ); }

	result_t load( const char* moduleName );
	result_t load( const wchar_t* moduleName );
	result_t unload( handle_t module );
	export_function_iterator begin() const { return export_function_iterator{ handle }; }
	export_function_iterator end() const { return export_function_iterator{ handle, ~0u }; }

	operator handle_t() const
	{
		return handle;
	}
	operator bool() const
	{
		return handle;
	}
public:
	handle_t handle = nullptr;
};

unique_library::result_t unique_library::load( const char* moduleName )
{
	result_t result = handle ? unload( std::move( handle ) ) : 0;
	if( result < 0 )
	{
		return result;
	}

	// try exact name first
	handle = reinterpret_cast<DosBinaryHeader*>( LoadLibraryA( moduleName ) );
	assert( !handle || handle->magicNumber == DosBinaryHeader::signature.value );
	return handle ? 0 : GetLastError();
}

unique_library::result_t unique_library::unload( handle_t module )
{
	result_t result = 0;
	if( module )
	{
		result = FreeLibrary( reinterpret_cast<HMODULE>( module ) ) != 0 ? 0 : GetLastError();
		module = nullptr;
	}
	return 0;
}

}}

#include <string>
#include <iostream>

#include <common/include/sb_common.h>
SB_EXPORT_TYPE int SB_STDCALL vst( [[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[] )
{
	std::string moduleName;// e.g., "C:\\Program Files\\Steinberg\\VstPlugins\\dev\\Surge.vst3";
	if( argc > 1 )
	{
		const char* moduleNameBegin = argv[argc - 1];
		while( *moduleNameBegin == '"' )
			++moduleNameBegin;
		const char* moduleNameEnd = moduleNameBegin;
		while( *moduleNameEnd != '\0' && *moduleNameEnd != '"' )
			++moduleNameEnd;

		moduleName = std::basic_string_view{ moduleNameBegin, size_t(moduleNameEnd - moduleNameBegin) };
	}

	// Listing exported names seems to be the easiest way to find the VST version...
	// There's quite a big difference between VST 2.3 and VST 2.4 and even more at VST 3.
	sbLibX::unique_library lib( moduleName.c_str() );
	if( lib )
	{
		std::cout << moduleName << std::endl;
		for ( auto it : lib )
			std::cout << it << std::endl;
	}
	return 0;
}