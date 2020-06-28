#include <array>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#include "dev/include/sb_dev.h"

//#if !defined(SBDEBUG)
//	#pragma warning(disable:4081 4365 4472 4514 4571 4625 4626 4710 4774 4820 5026 5027)
//	#include <iostream>
//	#pragma warning(default)
//#endif

#if defined(SBWIN64) || defined(SBWIN32)
	//#pragma warning(disable: 4472)

	#if !defined(SBDEBUG)
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif

	//#pragma warning(disable:4668 5039 4339)
	#include <windows.h>
	//#pragma warning(default:4668 5039)

	#ifndef WINAPI
		#define WINAPI SB_STDCALL
	#endif
	#endif
#endif
