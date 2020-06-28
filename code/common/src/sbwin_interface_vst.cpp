#include <common/include/sb_interface_vst.h>
//#include <common/include/sb_windows_registry.h>
//
//#include <unordered_map>
//#include <iostream>
//
//#include "Windows.h"
//
//#include <cassert>
//
//#pragma comment( lib, "Ole32.lib" )

// For VST2
struct AEffect;
typedef intptr_t ( SB_STDCALL *audioMasterCallback ) ( AEffect* effect, int32_t opcode, int32_t index, intptr_t value, void* ptr, float opt );


namespace SB { namespace LibX
{

class DynamicLibary
{
	DynamicLibary( const char* );
	DynamicLibary( const wchar_t* );
};

}}

SB_EXPORT_TYPE int SB_STDCALL vst([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	
	return 1;
}