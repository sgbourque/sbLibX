#pragma once

// TODO
#if defined(SBWIN64)
	#define SB_EXPORT_LIB extern "C" __declspec(dllexport)
	#define SB_IMPORT_LIB extern "C" __declspec(dllimport)
	#define SB_EXTERN_LIB extern "C"
	#define SB_LOCAL      
	#if defined(SBLOCAL)
		#define SB_EXPORT_TYPE SB_LOCAL
	#else
		#define SB_EXPORT_TYPE SB_EXPORT_LIB
	#endif

	#if 0 // expected main declaration
		SB_EXPORT_LIB int __stdcall main(int _nArgs = 0, const char* const pArgs[] = nullptr);
	#endif
#else
#error "unsupported platform"
#endif

#if defined(SBDEBUG)
	#define SBCONSOLE 1 // TODO : Fix SBCONSOLE !! It works really badly with unicode input, especially mixing cout and wcout
	#define SBDEBUG_OUTPUT 1
#endif
#if !defined(SBDEBUG_OUTPUT)
	#define SBCONSOLE 0
#if defined(SBRELEASE)
	#define SBCONSOLE_ATTACH 1
	#define SBDEBUG_OUTPUT 1
#else
	#define SBCONSOLE_ATTACH 0
	#define SBDEBUG_OUTPUT 0
#endif
#endif



////
	#define UNEVAL(...) __VA_ARGS__
	#define CONCAT(A, B) UNEVAL(A) ## UNEVAL(B)
////
	#define CSTRING_WRAPPER(_X)  #_X
	#define WSTRING_WRAPPER(_X) L#_X
	#define USTRING_WRAPPER(_X) u#_X
	#define CSTR(_X) CSTRING_WRAPPER(_X)
	#define WSTR(_X) WSTRING_WRAPPER(_X)
	#define USTR(_X) USTRING_WRAPPER(_X)
#if defined(UNICODE)
	#define STRINGIFY   WSTR
	#define SYSTEM_STRING(X) L##X
	namespace SBLib { using system_char_t = wchar_t; }
#else
	#define STRINGIFY   CSTR
	#define SYSTEM_STRING(X) X
	namespace SBLib { using system_char_t = char; }
#endif
	#define STR(X) SYSTEM_STRING(X)

// TODO: Wrap clog, cerr & cout into SBLog, SBErr & SBPrint
#if defined(SBDEBUG) || defined(SBRELEASE)
	// variadic template implementation into clog, cerr & cout
#else
	#define SBLog(...)
	#define SBErr(...)
	#define SBPrint(...)
#endif

namespace SB { namespace LibX{} }
namespace sbLibX = SB::LibX;
