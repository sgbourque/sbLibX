#if defined(SBWIN64) || defined(SBWIN32)
	#pragma warning(disable: 4472)

	#if !defined(SBDEBUG)
	#pragma warning(disable:4668 5039 4339)
	#define NOMINMAX
	#include <windows.h>
	#pragma warning(default:4668 5039)
	#ifndef WINAPI
		#define WINAPI SB_STDCALL
	#endif
	#endif
#endif

#if !defined(SBDEBUG)
	#pragma warning(disable:4081 4365 4472 4514 4571 4625 4626 4710 4774 4820 5026 5027)
	#include <iostream>
	#pragma warning(default)
#endif

//extern int main(int _nArgs = 0,  const char* const pArgs[] = nullptr);

#include <common/include/sb_common.h>
