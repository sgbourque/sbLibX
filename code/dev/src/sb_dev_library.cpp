#include <dev/include/sb_dev.h>
#include <dev/include/sb_dev_library.h>

//#include <cctype>
//#include <string>
////
//#pragma warning(disable:4081 4365 4472 4514 4571 4625 4626 4710 4774 4820 5026 5027)
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
//#pragma warning(default)

// c.f.,
// https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-symenumeratesymbols
// for how to enumerate symbols in non-retail env.

#define SBLIB_SUPPORT_EXE	(false)

static constexpr const char* const inverse_ordered_configuration_tokens[] = {
	"_" SB_TARGET_CONFIGURATION,
	"_" SB_TARGET_PLATFORM,
	"_" SB_TARGET_PLATFORM "-" SB_CLANG_SUFFIX_TARGET
#if SBLIB_SUPPORT_EXE
	"_" SB_TARGET_PLATFORM "-exec.exe",
	"_" SB_TARGET_PLATFORM "-" SB_CLANG_SUFFIX_TARGET "-exec.exe",
#endif
};



////
#if defined(SBWIN64) || defined(SBWIN32)
#define NOMINMAX
//#pragma warning(disable:4668 5039 4339)
#include <windows.h>
//#pragma warning(default)
#ifndef WINAPI
	#define WINAPI SB_STDCALL
#endif

namespace SB { namespace LibX { namespace Dev
{

bool iequal(const std::string name1, const std::string name2)
{
	return std::equal(	std::begin(name1), std::end(name1),
						std::begin(name2), std::end(name2),
						[](char c1, char c2) { return std::tolower(c1) == std::tolower(c2); } );
}


std::string sbDynLib_name_to_alias(std::string name)
{
	size_t lastSeparator = std::string::npos;
	for ( std::string token : inverse_ordered_configuration_tokens)
	{
		lastSeparator = name.find_last_of('_');
		if (lastSeparator != std::string::npos)
		{
			if (iequal(std::string(token), std::string(name.data() + lastSeparator, name.length() - lastSeparator)))
				name.resize(lastSeparator);
		}
	}
	return name;
}
std::string sbDynLib_alias_to_name(std::string name)
{
	name = sbDynLib_name_to_alias(name);
	name += std::string("_" SB_TARGET_PLATFORM);
	name += std::string("_" SB_TARGET_CONFIGURATION);
	return name;
}
std::string sbDynLib_alias_to_clang_name(std::string name)
{
	name = sbDynLib_name_to_alias(name);
	name += std::string("_" SB_TARGET_PLATFORM);
	name += std::string("_" SB_TARGET_CONFIGURATION "-" SB_CLANG_SUFFIX_TARGET);
	return name;
}


bool sbDynLib_equivalent(std::string name1, std::string name2)
{
	return iequal( sbDynLib_name_to_alias(name1), sbDynLib_name_to_alias(name2) );
}

bool library::load(const char* libName)
{
	if (sbDynLib_equivalent(alias(), libName))
	{
		std::cerr << "(library '" << name() << "' already loaded)" << std::endl;
		// should addref here
		return *this;
	}
	else if (*this && !unload())
	{
		std::cerr << "could not free library '" << name() << "'" << std::endl;
	}

	// critsec here

	// try exact name first
	std::string library_name = libName;
	std::string library_alias = sbDynLib_name_to_alias(libName);
	unique_dll::load( library_name.c_str() );
	if( !handle )
	{
		// try full name
		library_name = sbDynLib_alias_to_name(library_alias);
		unique_dll::load( library_name.c_str() );
	}
	if( !handle )
	{
		// try full name (w/ clang)
		library_name = sbDynLib_alias_to_clang_name(library_alias);
		unique_dll::load( library_name.c_str() );
	}
#if SBLIB_SUPPORT_EXE
	if( !handle )
	{
		// try exec
		library_name = sbDynLib_alias_to_name(library_alias) + "-exec.exe";
		unique_dll::load( library_name.c_str() );
	}
	if( !handle )
	{
		// try exec (w/ clang)
		library_name = sbDynLib_alias_to_clang_name(library_alias) + "-exec.exe";
		unique_dll::load( library_name.c_str() );
	}
#endif
	if( !handle )
	{
		// try alias
		library_name = library_alias;
		unique_dll::load( library_name.c_str() );
	}
	if( !handle )
	{
		library_name = library_alias = {};
		std::cerr << "Could not load library '" << libName << "'. Last error: " << GetLastError() << std::endl;
	}
	else
	{
		aliases.emplace_back( library_alias );
		aliases.emplace_back( library_name );
	}
	return *this;
}
//bool library::unload()
//{
//	// TODO : what if FreeLibrary fails?
//	while (data)
//	{
//		uint32_t error = 0;
//		HMODULE module = get_data<HMODULE>();
//		if (0 == FreeLibrary(module))
//		{
//			error = GetLastError();
//			if (error)
//				std::cerr << "could not unload library : Error Code 0x" << std::setw(8) << std::hex << error << ". Retrying..." << std::endl;
//		}
//
//		if (!error)
//		{
//			data = nullptr;
//			library_alias = library_name = {};
//		}
//	}
//	return data == nullptr;
//}

//typename library::raw_data_t library::get_internal(const char* fctName)
//{
//	std::string libraryName = library_name;
//	std::string functionName = fctName;
//	raw_data_t fct = nullptr;
//	if (!data)
//		return fct;
//
//	while (!fct && !functionName.empty())
//	{
//		HMODULE module_data = get_data<HMODULE>();
//		FARPROC proc_address = GetProcAddress(module_data, functionName.c_str());
//		fct = proc_address;
//		if (!fct)
//		{
//			if (!libraryName.empty())
//			{
//				functionName = libraryName + "_" + std::string(fctName);
//				auto lastSeparator = libraryName.find_last_of('_');
//				if (lastSeparator != std::string::npos)
//					libraryName = std::string(libraryName.data(), lastSeparator);
//				else
//					libraryName.clear();
//			}
//			else
//			{
//				functionName.clear();
//			}
//		}
//	}
//
//	if (fct == nullptr)
//	{
//		std::cerr << "Could not find entry point '" << library_alias << "/" << fctName << "'. Last error: " << GetLastError() << std::endl;
//	}
//	else
//	{
//		std::clog << "Entry point: '" << library_alias << "/" << fctName << "'" << std::endl;
//	}
//	return fct;
//}

#if 0
// To be used if linking to an executable file
// see https://www.codeproject.com/Articles/1045674/Load-EXE-as-DLL-Mission-Possible
// Note : This is not my code. It is an exerpt from the above article that give a good
// description of what needs to be done. Thanks to the author.
//
// It's worth a try some day, but I'm not there yet.
// For the moment, the exe will load, we can call init which start CRT initialization
// through either _DllWinMainCRTStartup (proceeding to init as a dll),
// or main/WinMainCRTStartup which will go through the whole main/WinMain execution process.
// however, I will usually crash at first function call most probably because of the
// missing part described here.
void ParseIAT(HINSTANCE h)
{
	// Find the IAT size
	DWORD ulsize = 0;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(h, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulsize);
	if (!pImportDesc)
		return;

	// Loop names
	for (; pImportDesc->Name; pImportDesc++)
	{
		PSTR pszModName = (PSTR)((PBYTE)h + pImportDesc->Name);
		if (!pszModName)
			break;

		HINSTANCE hImportDLL = LoadLibraryA(pszModName);
		if (!hImportDLL)
		{
			// ... (error)
		}

		// Get caller's import address table (IAT) for the callee's functions
		PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)
			((PBYTE)h + pImportDesc->FirstThunk);

		// Replace current function address with new function address
		for (; pThunk->u1.Function; pThunk++)
		{
			FARPROC pfnNew = 0;
			size_t rva = 0;
#ifdef _WIN64
			if (pThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG64)
#else
			if (pThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32)
#endif
			{
				// Ordinal
#ifdef _WIN64
				size_t ord = IMAGE_ORDINAL64(pThunk->u1.Ordinal);
#else
				size_t ord = IMAGE_ORDINAL32(pThunk->u1.Ordinal);
#endif

				PROC* ppfn = (PROC*)&pThunk->u1.Function;
				if (!ppfn)
				{
					// ... (error)
				}
				rva = (size_t)pThunk;

				char fe[100] = { 0 };
				sprintf_s(fe, 100, "#%u", ord);
				pfnNew = GetProcAddress(hImportDLL, (LPCSTR)ord);
				if (!pfnNew)
				{
					// ... (error)
				}
			}
			else
			{
				// Get the address of the function address
				PROC* ppfn = (PROC*)&pThunk->u1.Function;
				if (!ppfn)
				{
					// ... (error)
				}
				rva = (size_t)pThunk;
				PSTR fName = (PSTR)h;
				fName += pThunk->u1.Function;
				fName += 2;
				if (!fName)
					break;
				pfnNew = GetProcAddress(hImportDLL, fName);
				if (!pfnNew)
				{
					// ... (error)
				}
			}

			// Patch it now...
			auto hp = GetCurrentProcess();
			if (!WriteProcessMemory(hp, (LPVOID*)rva, &pfnNew, sizeof(pfnNew), NULL) && (ERROR_NOACCESS == GetLastError()))
			{
				DWORD dwOldProtect;
				if (VirtualProtect((LPVOID)rva, sizeof(pfnNew), PAGE_WRITECOPY, &dwOldProtect))
				{
					if (!WriteProcessMemory(GetCurrentProcess(), (LPVOID*)rva, &pfnNew, sizeof(pfnNew), NULL))
					{
						// ... (error)
					}
					if (!VirtualProtect((LPVOID)rva, sizeof(pfnNew), dwOldProtect, &dwOldProtect))
					{
						// ... (error)
					}
				}
			}
		}
	}
}
#endif

}}} // namespace SB::LibX::Dev

#endif
