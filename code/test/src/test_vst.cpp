#include "common/include/sb_library.h"


#include <string>
#include <iostream>

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

		moduleName = std::basic_string_view<char>{ moduleNameBegin, size_t(moduleNameEnd - moduleNameBegin) };
	}

	// Listing exported names seems to be the easiest way to find the VST version...
	// There's quite a big difference between VST 2.3 and VST 2.4 and even more at VST 3.
	sbLibX::unique_dll lib( moduleName.c_str() );
	if( lib )
	{
		std::cout << "'" << moduleName << "':" << std::endl;
		for( auto it : lib )
			std::cout << "\t" << it.first << std::endl;
	}
	return 0;
}
