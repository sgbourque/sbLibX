#include <common/include/sb_library.h>

#include <common/include/sb_common.h>
namespace SB { namespace LibX {
struct SystemBinaryHeader;
}}
SB_IMPORT_TYPE sbLibX::DLL::handle_t __stdcall LoadLibraryA( const char* moduleName );
SB_IMPORT_TYPE int32_t  SB_STDCALL GetLastError();
SB_IMPORT_TYPE uint32_t SB_STDCALL FreeLibrary( sbLibX::DLL::handle_t moduleHandle );


#include <cstdint>
#include <cassert>
#include <algorithm>

namespace SB { namespace LibX { namespace DLL
{

struct SystemBinaryHeader
{
	union magicNumber_t {
		uint8_t  signature[2] = { 'M', 'Z' };
		uint16_t value;
	};
	static inline constexpr magicNumber_t magicNumber{};

	magicNumber_t signature;
	uint16_t      lastPageSize;
	uint16_t      pageCount;
	uint16_t      relocationCount;
	uint16_t      headersSize;
	uint16_t      e_minExtraAlloc;
	uint16_t      e_maxExtraAlloc;
	uint16_t      initialStackSize;
	uint16_t      initialStackPointer;
	uint16_t      checksum;
	uint16_t      initialInstructionPointer;
	uint16_t      initialCodeSegment;
	uint16_t      relocationTableOffset;
	uint16_t      overlayNumber;
	uint16_t      reserved[4];
	uint16_t      oemId;
	uint16_t      oemInfo;
	uint16_t      reserved2[10];
	int32_t       fileHeaderOffset;
};
struct SystemFileHeader
{
	union magicNumber_t {
		uint8_t  signature[4] = { 'P', 'E', '\0', '\0' };
		uint32_t value;
	};
	static inline constexpr magicNumber_t magicNumber{};
	struct version_t
	{
		uint16_t major;
		uint16_t minor;
	};

	magicNumber_t signature;
	struct type_t
	{
		enum arch_t : uint16_t
		{
			x86  = 0x014c,
			IA64 = 0x0200,
			x64  = 0x8664,
		};
		enum flags_t : uint16_t
		{
			RelocsStripped       = ( 1 <<  0 ),	// Relocation information was stripped from the file. The file must be loaded at its preferred base address. If the base address is not available, the loader reports an error.
			Executable           = ( 1 <<  1 ),	// The file is executable (there are no unresolved external references).
			LineNumbersStripped  = ( 1 <<  2 ),	// COFF line numbers were stripped from the file.
			LocalSymbolsStripped = ( 1 <<  3 ),	// COFF symbol table entries were stripped from file.
			_16_deprecated       = ( 1 <<  4 ),	// Aggressively trim the working set. This value is obsolete.
			_32_unused           = ( 1 <<  5 ),
			LargeAddressSupport  = ( 1 <<  6 ),	// The application can handle addresses larger than 2 GB.
			_64_deprecated       = ( 1 <<  7 ),	// The bytes of the word are reversed. This flag is obsolete.
			Supports32bits       = ( 1 <<  8 ),	// The computer supports 32-bit words.
			DebugStripped        = ( 1 <<  9 ),	// Debugging information was removed and stored separately in another file.
			RemovableMedia       = ( 1 << 10 ),	// If the image is on removable media, copy it to and run it from the swap file.
			NetworkFile          = ( 1 << 11 ),	// If the image is on the network, copy it to and run it from the swap file.
			SystemFile           = ( 1 << 12 ),	// The image is a system file.
			DynamicLibrary       = ( 1 << 13 ),	// The image is a DLL file. While it is an executable file, it cannot be run directly.
			SingleProcessor      = ( 1 << 14 ),	// The file should be run only on a uniprocessor computer.
			_32768_deprecated    = ( 1 << 15 ),	// The bytes of the word are reversed.This flag is obsolete.
		};

		arch_t   architecture;
		uint16_t sectionCount;
		uint32_t timeDateStamp; // in seconds since midnight 01/01/1970, 00:00:00 UTC
		uint32_t symbolTableOffset;
		uint32_t symbolCount;
		uint16_t optionalHeaderSize;
		flags_t  flags;
	} header;
	struct optionalHeader_t
	{
		enum magicNumber_t : uint16_t
		{
			ROM          = 0x0107,
			Executable32 = 0x010b,
			Executable64 = 0x020b,
		#if defined( _WIN64 )
			Executable   = Executable64
		#else
			#error "32-bits platform not supported"
		#endif
		};
		enum subsystem_t : uint16_t
		{
			Unknown          =  0,	//Unknown subsystem.
			Native           =  1,	//No subsystem required( device drivers and native system processes ).
			WindowsGUI       =  2,	//Windows graphical user interface( GUI ) subsystem.
			WindowsConsole   =  3,	//Windows character-mode user interface( CUI ) subsystem.

			OS2Console       =  5,	//OS/2 CUI subsystem.

			POSIXConsole     =  7,	//POSIX CUI subsystem.

			WindowsCE        =  9,	//Windows CE system.
			EFI              = 10,	//Extensible Firmware Interface( EFI ) application.
			EFIBoot          = 11,	//EFI driver with boot services.
			EFIRuntimeDriver = 12,	//EFI driver with run-time services.
			EFIROM           = 13,	//EFI ROM image.
			XBOX             = 14,	//Xbox system.
			WindowsBoot      = 16,	//Boot application.
		};
		enum flags_t : uint16_t
		{
			_1_reserved             = ( 1 << 0 ),
			_2_reserved             = ( 1 << 1 ),
			_4_reserved             = ( 1 << 2 ),
			_8_reserved             = ( 1 << 3 ),
			_16_undefined           = ( 1 << 4 ),
			_32_undefined           = ( 1 << 5 ),
			DynamicBase             = ( 1 << 6 ),	// The DLL can be relocated at load time.
			ForceIntegrity          = ( 1 << 7 ),	// Code integrity checks are forced. If you set this flag and a section contains only uninitialized data, set the PointerToRawData member of IMAGE_SECTION_HEADER for that section to zero; otherwise, the image will fail to load because the digital signature cannot be verified.
			DataExecutionPrevention = ( 1 << 8 ),	// The image is compatible with data execution prevention (DEP).
			NoIsolation             = ( 1 << 9 ),	// The image is isolation aware, but should not be isolated.
			NoExceptionHandling     = ( 1 << 10 ),	// The image does not use structured exception handling (SEH). No handlers can be called in this image.
			NoBind                  = ( 1 << 11 ),	// Do not bind the image.
			_4096_reserved          = ( 1 << 12 ),
			WDMDriver               = ( 1 << 13 ),	// A WDM driver.
			_16384_reserved         = ( 1 << 14 ),
			TerminalServerAware     = ( 1 << 15 ),	// The image is terminal server aware.
		};
		enum data_type : uint32_t
		{
			Export,                 	// Export Directory
			Import,                 	// Import Directory
			Resource,               	// Resource Directory
			Exception,              	// Exception Directory
			Security,               	// Security Directory
			RelocationTable,        	// Base Relocation Table
			Debug,                  	// Debug Directory
			Architecture,           	// Architecture Specific Data
			GlobalPointer,          	// Relative Virtual Addresses (RVA) of global pointer register (GP)
			ThreadLocalStorageTable,	// TLS Directory
			LoadConfigurationTable, 	// Load Configuration Directory
			BoundImportTable,       	// Bound Import Directory in headers
			ImportAddressTable,     	// Import Address Table
			DelayImportDescriptors, 	// Delay Load Import Descriptors
			CLRDescriptor,          	// COM Runtime descriptor
			_15_reserved,

			DataEntryCount
		};

		magicNumber_t signature;
		struct linker_version_t 
		{
			uint8_t major;
			uint8_t minor;
		} linkerVersion;
		uint32_t      codeSegmentSize;
		uint32_t      initializedDataSize;
		uint32_t      uninitializedDataSize;
		uint32_t      entryPointOffset;
		uint32_t      codeSegmentOffset;
		uint64_t      imageBase;
		uint32_t      sectionAlignment;
		uint32_t      fileAlignment;
		version_t     operatingSystemVersion;
		version_t     imageVersion;
		version_t     subsystemVersion;
		version_t     win32Version;
		uint32_t      imageSize;
		uint32_t      headersSize;
		uint32_t      checksum;
		subsystem_t   subsystem;
		flags_t       flags;
		uint64_t      stackReserveSize;
		uint64_t      stackCommitSize;
		uint64_t      heapReserveSize;
		uint64_t      heapCommitSize;
		uint32_t      deprecated;
		uint32_t      dataEntryCount;
		struct data_entry_t {
			uint32_t virtualAddressOffset;
			uint32_t size;
		} dataEntry[DataEntryCount];
	} optionalHeader;

	struct data_entry_t
	{
		uint32_t  unused;
		uint32_t  timeDateStamp;
		version_t version;
		uint32_t  name;
		uint32_t  base;
		uint32_t  functionCount;
		uint32_t  nameCount;
		uint32_t  functionTable;
		uint32_t  nameTable;
		uint32_t  nameOrdinalTable;
	};
};

SB_WIN_EXPORT export_function_iterator::export_function_iterator( const handle_t module, uint32_t hint )
{
	if( module )
	{
		assert( !module || module->signature.value == module->magicNumber.value );
		rawModule = reinterpret_cast<const char*>( module );

		auto header = reinterpret_cast<const SystemFileHeader*>( rawModule + module->fileHeaderOffset );
		assert( header->signature.value == header->magicNumber.value );
		assert( header->optionalHeader.dataEntryCount > 0 );
		auto exports = reinterpret_cast<const SystemFileHeader::data_entry_t*>(
			rawModule + header->optionalHeader.dataEntry[SystemFileHeader::optionalHeader_t::Export].virtualAddressOffset
			);
		nameTable = reinterpret_cast<const uint32_t*>( rawModule + exports->nameTable );
		functionTable = reinterpret_cast<const uint32_t*>( rawModule + exports->functionTable );
		if( *functionTable == 0 )
			++functionTable; // clang will add the base module address there, so it seems. But why?

		nameTableEnd = nameTable + exports->nameCount;
		functionTableEnd = functionTable + exports->functionCount;

		hint = std::min<uint32_t>( hint, exports->nameCount );
		nameTable += hint;
		functionTable += hint;
	}
}
}

template<>
SB_WIN_EXPORT unique_library<DLL::handle_t>::result_t unique_library<DLL::handle_t>::load( const char* moduleName )
{
	result_t result = handle ? unload( std::move( handle ) ) : 0;
	if( result < 0 )
	{
		return result;
	}

	// try exact name first
	handle = LoadLibraryA( moduleName );
	assert( !handle || handle->signature.value == handle->magicNumber.value );
	return handle ? 0 : GetLastError();
}

	template<>
SB_WIN_EXPORT unique_library<DLL::handle_t>::result_t unique_library<DLL::handle_t>::unload( handle_t module )
{
	result_t result = 0;
	if( module )
	{
		result = FreeLibrary( module ) != 0 ? 0 : GetLastError();
		module = nullptr;
	}
	return result;
}

	template<>
SB_WIN_EXPORT function_helper unique_library<DLL::handle_t>::operator[]( std::string_view functionName ) const
{
	//Ideally, we should build a name -> function hashmap
	//function_helper helper{ GetProcAddress( handle, functionName.data() ) };
	for( auto it : *this )
		if( functionName.compare( it.first ) == 0 )
			return function_helper{ it.second };
	return function_helper{nullptr};
}


}}
