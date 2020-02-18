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


////
#if defined(SBWIN64) || defined(SBWIN32)
#define NOMINMAX
//#pragma warning(disable:4668 5039 4339)
#include <windows.h>
//#pragma warning(default)
#ifndef WINAPI
	#define WINAPI __stdcall
#endif

namespace SB { namespace LibX
{

bool iequal(const std::string name1, const std::string name2)
{
	return std::equal(	std::begin(name1), std::end(name1),
						std::begin(name2), std::end(name2),
						[](char c1, char c2) { return std::tolower(c1) == std::tolower(c2); } );
}

std::string sbDynLib_name_to_alias(std::string name)
{
	auto lastSeparator = name.find_last_of('_');
	if (lastSeparator != std::string::npos)
	{
		if ( iequal(std::string("_" SB_TARGET_CONFIGURATION), std::string(name.data() + lastSeparator, name.length() - lastSeparator)) )
			name.resize(lastSeparator);
	}
	lastSeparator = name.find_last_of('_');
	if (lastSeparator != std::string::npos)
	{
		if (iequal(std::string("_" SB_TARGET_PLATFORM), std::string(name.data() + lastSeparator, name.length() - lastSeparator)))
			name.resize(lastSeparator);
	}
	return name;
}
std::string sbDynLib_alias_to_name(std::string name)
{
	auto lastSeparator = name.find_last_of('_');
	if (lastSeparator != std::string::npos)
	{
		if (iequal(std::string("_" SB_TARGET_PLATFORM), std::string(name.data() + lastSeparator, name.length() - lastSeparator)))
			name.resize(lastSeparator);
	}
	lastSeparator = name.find_last_of('_');
	if (	lastSeparator == std::string::npos ||
			!iequal(std::string("_" SB_TARGET_PLATFORM), std::string(name.data() + lastSeparator, name.length() - lastSeparator)))
	{
		name += std::string("_" SB_TARGET_PLATFORM);
	}
	lastSeparator = name.find_last_of('_');
	if (	lastSeparator == std::string::npos ||
			!iequal(std::string("_" SB_TARGET_CONFIGURATION), std::string(name.data() + lastSeparator, name.length() - lastSeparator)))
	{
		name += std::string("_" SB_TARGET_CONFIGURATION);
	}
	return name;
}


bool sbDynLib_equivalent(std::string name1, std::string name2)
{
	return iequal( sbDynLib_name_to_alias(name1), sbDynLib_name_to_alias(name2) );
}

bool library::load(const char* libName)
{
	if (sbDynLib_equivalent(library_alias, libName))
	{
		return data;
	}
	else if (data && !free())
	{
		std::cerr << "could not free library '" << library_name << std::endl;
	}

	// try exact name first
	library_name = libName;
	library_alias = sbDynLib_name_to_alias(libName);
	data = LoadLibraryA(library_name.c_str());
	if (!data)
	{
		// try full name
		library_name = sbDynLib_alias_to_name(library_alias);
		data = LoadLibraryA(library_name.c_str());
	}
	if (!data)
	{
		// try alias
		library_name = library_alias;
		data = LoadLibraryA(library_alias.c_str());
	}
	if (!data)
	{
		library_name = library_alias = {};
		std::cerr << "Could not load library '" << libName << "'. Last error: " << GetLastError() << std::endl;
	}
	return data;
}
bool library::free()
{
	// TODO : what if FreeLibrary fails?
	while (data)
	{
		uint32_t error = 0;
		HMODULE module = get_data<HMODULE>();
		if (0 == FreeLibrary(module))
		{
			error = GetLastError();
			if (error)
				std::cerr << "could not unload library : Error Code 0x" << std::setw(8) << std::hex << error << ". Retrying..." << std::endl;
		}

		if (!error)
		{
			data = nullptr;
			library_alias = library_name = {};
		}
	}
	return data == nullptr;
}

typename library::raw_data_t library::get_internal(const char* fctName)
{
	std::string libraryName = library_name;
	std::string functionName = fctName;
	raw_data_t fct = nullptr;
	if (!data)
		return fct;

	while (!fct && !functionName.empty())
	{
		HMODULE module_data = get_data<HMODULE>();
		FARPROC proc_address = GetProcAddress(module_data, functionName.c_str());
		fct = proc_address;
		if (!fct)
		{
			if (!libraryName.empty())
			{
				functionName = libraryName + "_" + std::string(fctName);
				auto lastSeparator = libraryName.find_last_of('_');
				if (lastSeparator != std::string::npos)
					libraryName = std::string(libraryName.data(), lastSeparator);
				else
					libraryName.clear();
			}
			else
			{
				functionName.clear();
			}
		}
	}

	if (fct == nullptr)
	{
		std::cerr << "Could not find entry point '" << library_alias << "/" << fctName << "'. Last error: " << GetLastError() << std::endl;
	}
	else
	{
		std::clog << "Entry point: '" << library_alias << "/" << fctName << "'" << std::endl;
	}
	return fct;
}


}} // namespace SB::LibX
using namespace SB;

#endif
