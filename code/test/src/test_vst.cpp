	//assert(((PIMAGE_DOS_HEADER)lib)->e_magic == IMAGE_DOS_SIGNATURE);
	//PIMAGE_NT_HEADERS header = (PIMAGE_NT_HEADERS)( (BYTE *)lib + ((PIMAGE_DOS_HEADER)lib)->e_lfanew );
	//assert(header->Signature == IMAGE_NT_SIGNATURE);
	//assert(header->OptionalHeader.NumberOfRvaAndSizes > 0);
	//PIMAGE_EXPORT_DIRECTORY exports = ( PIMAGE_EXPORT_DIRECTORY ) ( 
	//	(BYTE *)lib + header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress
	//) ;
	//PVOID names = (BYTE *)lib + exports->AddressOfNames;
	//std::cout << "EXPORTS" << std::endl ;
	//for (int i = 0; i < exports->NumberOfNames; i++)
	//	std::cout << "\t" << ( (BYTE *)lib + ((DWORD *)names)[i]) << std::endl ;

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

//class library
//{
//public:
//	library() {}
//	virtual ~library()
//	{
//		free();
//	}
//
//	//bool load( const char* libName );
//	//bool free();
//
//	//std::string name() const { return library_name; }
//	//std::string alias() const { return library_alias; }
//
//	//template<typename data_type>
//	//data_type get_data() const { return reinterpret_cast<data_type>( data ); };
//
//	//template<typename fct_type>
//	//fct_type get( const char* fctName ) { return reinterpret_cast<fct_type>( get_internal( fctName ) ); }
//
//	//operator bool() const { return data != 0; }
//
//private:
//	using handle_t = void*;
//	handle_t get_internal( const char* fctName );
//
//private:
//	//std::string library_name{};
//	//std::string library_alias{};
//	handle_t module = nullptr;
//};

#include <common/include/sb_common.h>
SB_EXPORT_TYPE int SB_STDCALL vst( [[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[] )
{
	return 0;
}