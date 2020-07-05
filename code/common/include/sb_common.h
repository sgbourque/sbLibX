#pragma once

//#include <common/include/internal/compiler_specific.h>
#if defined(SBWIN64)
	#define SB_FASTCALL    __fastcall
	#define SB_STDCALL     __stdcall
	#define SB_DEFAULTCALL __cdecl
#else
#error "unsupported platform"
#endif
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
		SB_EXPORT_LIB int SB_STDCALL main(int _nArgs = 0, const char* const pArgs[] = nullptr);
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
	#define SB_UNEVAL(...) __VA_ARGS__
	#define SB_CONCAT(A, B) SB_UNEVAL(A) ## SB_UNEVAL(B)
////
	#define SB_CSTRING_WRAPPER(_X)  #_X
	#define SB_WSTRING_WRAPPER(_X) L#_X
	#define SB_U8STRING_WRAPPER(_X) u8#_X
	#define CSTR(_X) SB_CSTRING_WRAPPER(_X)
	#define WSTR(_X) SB_WSTRING_WRAPPER(_X)
	#define U8STR(_X) SB_U8STRING_WRAPPER(_X)
#if defined(UNICODE)
	#define SB_SYSTEM_STRING   WSTR
	namespace SB { using system_char_t = wchar_t; }
#else
	#define SB_SYSTEM_STRING   CSTR
	namespace SB { using system_char_t = char; }
#endif
	#define STR(X) SB_SYSTEM_STRING(X)

// TODO: Wrap clog, cerr & cout into SBLog, SBErr & SBPrint
#if defined(SBDEBUG) || defined(SBRELEASE)
	// variadic template implementation into clog, cerr & cout
#else
	#define SBLog(...)
	#define SBErr(...)
	#define SBPrint(...)
#endif

#include <type_traits>
// remove_cvref_t not part of std before 201705
#if defined(_MSVC_LANG) && _MSVC_LANG < 201705
	namespace SB { namespace LibX {
		template<typename _TYPE_> using remove_cvref_t = std::remove_cv_t< std::remove_reference_t<_TYPE_> >;
	}} // namespace SB:LibX
#else
	namespace SB { namespace LibX {
		template<typename _TYPE_> using remove_cvref_t = std::remove_cvref_t<_TYPE_>;
	}} // namespace SB:LibX
#endif
namespace sbLibX = SB::LibX;
