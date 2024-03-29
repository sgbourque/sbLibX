#pragma once

#define SB_TARGET_TYPE_STATIC    	(1 << 0)
#define SB_TARGET_TYPE_DYNAMIC   	(1 << 1)
#define SB_TARGET_TYPE_STANDALONE	(1 << 2)

#if defined(SBWIN64)
#define SB_TARGET_NAME_STATIC    	"lib"
#define SB_TARGET_NAME_DYNAMIC   	"dll"
#define SB_TARGET_NAME_STANDALONE	"exe"
#else
#error "Unknown platform"
#endif

#if defined( _M_X64 )
#define SB_TARGET_PLATFORM			"x64"
#else
#error "Undefined platform"
#endif
#if defined(SBDEBUG)
#define SB_TARGET "debug"
#elif defined(SBRELEASE)
#define SB_TARGET "release"
#else
#define SB_TARGET "final"
#endif

#define SB_ARCH_SUFFIX  	"_" SB_TARGET_PLATFORM
#define SB_TARGET_SUFFIX	"_" SB_TARGET

#if defined(SBWIN64)
	#define SB_FASTCALL    __fastcall
	#define SB_STDCALL     __stdcall
	#define SB_DEFAULTCALL __cdecl
#else
#error "unsupported platform"
#endif
#if defined(SBWIN64)
	#define SB_EXPORT_LIB __declspec(dllexport)
	#define SB_IMPORT_LIB __declspec(dllimport)
	#define SB_EXTERN_LIB extern "C"
	#define SB_LOCAL      
	#if defined(SBLOCAL)
		#define SB_EXPORT_TYPE SB_LOCAL
	#else
		#define SB_EXPORT_TYPE SB_EXTERN_LIB SB_EXPORT_LIB
		#define SB_IMPORT_TYPE SB_EXTERN_LIB SB_IMPORT_LIB
	#endif

	#if 0 // Expected main declaration. Arguments are optional, thanks to stdcall.
		SB_EXTERN_LIB int SB_STDCALL main(int _nArgs = 0, const char* const pArgs[] = nullptr);
	#endif
#else
#error "unsupported platform"
#endif

#if defined( SBWIN32 ) || defined( SBWIN64 )
	#define SB_LIB_EXTENSION	".lib"
#else
	#error "Undefined platform"
#endif
#if defined(SB_LIBX_INTERNAL)
	#define SB_COMMON_DEPENDS
	#define SB_COMMON_EXPORT SB_EXPORT_LIB
	#ifndef SB_LIBPLATFORM_INTERNAL
	#define SB_LIBPLATFORM_INTERNAL
	#endif
#else
	#define SB_COMMON_PROJECT	"sbLibX"
	#define SB_COMMON_DEPEND_NAME	SB_COMMON_PROJECT SB_ARCH_SUFFIX SB_TARGET_SUFFIX SB_LIB_EXTENSION
	#define SB_COMMON_DEPENDS	__pragma( comment( lib, SB_COMMON_DEPEND_NAME ) )
	#define SB_COMMON_EXPORT SB_IMPORT_LIB
#endif

#if !defined(SB_LIBPLATFORM_INTERNAL)
#if defined( SBWIN32 ) || defined( SBWIN64 )
	#define SB_PLATFORM_PROJECT	"sbWindows"
#else
	#error "Undefined platform"
#endif

	#define SB_PLATFORM_DEPEND_NAME	SB_PLATFORM_PROJECT SB_ARCH_SUFFIX SB_TARGET_SUFFIX SB_LIB_EXTENSION
	#define SB_PLATFORM_DEPENDS	__pragma( comment( lib, SB_PLATFORM_DEPEND_NAME ) )
#else
	#define SB_PLATFORM_DEPENDS
#endif


#if defined(SBDEBUG)
	#define SBCONSOLE SB_SUPPORTED // TODO : Fix SBCONSOLE !! It works really badly with unicode input, especially mixing cout and wcout
	#define SBDEBUG_OUTPUT SB_SUPPORTED
#endif
#if !defined(SBDEBUG_OUTPUT)
	#define SBCONSOLE SB_UNSUPPORTED
#if defined(SBRELEASE)
	#define SBCONSOLE_ATTACH SB_SUPPORTED
	#define SBDEBUG_OUTPUT SB_SUPPORTED
#else
	#define SBCONSOLE_ATTACH SB_UNSUPPORTED
	#define SBDEBUG_OUTPUT SB_UNSUPPORTED
#endif
#endif



////
	#define SB_UNEVAL(...) __VA_ARGS__
	#define SB_CONCAT(A, B) SB_UNEVAL(A) ## SB_UNEVAL(B)
////
	#define SB_CSTRING_WRAPPER(_X) "" #_X
	#define SB_WSTRING_WRAPPER(_X) L"" #_X
	#define SB_U8STRING_WRAPPER(_X) u8"" #_X
	#define SB_U16STRING_WRAPPER(_X) u"" #_X
	#define CSTR(_X) SB_CSTRING_WRAPPER(_X)
	#define WSTR(_X) SB_WSTRING_WRAPPER(_X)
	#define U8STR(_X) SB_U8STRING_WRAPPER(_X)
	#define U16STR(_X) SB_U16STRING_WRAPPER(_X)
#if defined(UNICODE)
	#define SB_SYSTEM_STRING   U16STR
	namespace SB { using system_char_t = char16_t; using const_system_string_t = const system_char_t*; }
#else
	#define SB_SYSTEM_STRING   CSTR
	#if _MSVC_LANG >= 201705
		namespace SB { using system_char_t = char8_t; }
	#else
		namespace SB { using system_char_t = char; }
	#endif
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

#include "common/include/internal/compiler_specific.h"

#include <cstdint>
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


#include "common/include/internal/sb_features.h"
