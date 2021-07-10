#pragma once

#if defined(_MSC_VER) && defined( _M_X64 )
#define SB_TARGET_X64_MSC 1
///////////////////////////////////////////////////////////////////////////////
//
//SBCOMPILE_INFO( " " )
//SBCOMPILE_INFO( " --- SBLib © " __DATE__ " (" __TIME__ ") ---" )
///////////////////////////////////////////////////////////////////////////////
#if defined(UNICODE) && !defined(_NATIVE_WCHAR_T_DEFINED)
	SBCOMPILE_INFO(" ********************************")
	SBCOMPILE_INFO(" * SB WARNING:                  *")
	SBCOMPILE_INFO(" * Type wchar_t is not defined. *")
	SBCOMPILE_INFO(" * Using char16_t instead       *")
	SBCOMPILE_INFO(" ********************************")
	typedef char16_t wchar_t;
#endif

#if defined( _WINDLL )
#define SB_TARGET_TYPE_BASE	SB_TARGET_TYPE_DYNAMIC
SBCOMPILE_INFO(" * Building dll *")
#else
#define SB_TARGET_TYPE_BASE	SB_TARGET_TYPE_STATIC
SBCOMPILE_INFO(" * Building static binaries *")
#endif
#if defined( SBSTANDALONE )
#define SB_TARGET_TYPE	( SB_TARGET_TYPE_STANDALONE | SB_TARGET_TYPE_BASE )
SBCOMPILE_INFO( " * Using standalone binaries *" )
#else
#define SB_TARGET_TYPE	( SB_TARGET_TYPE_BASE )
#endif

///////////////////////////////////////////////////////////////////////////////
#include <common/include/internal/arch/x64_specific.h>
#include <common/include/internal/arch/x86_specific.h>
//#include <common/include/internal/arch/Xenon_specific.h>    // X360 - need to put Xenon/WinCE elsewhere?
//#include <common/include/internal/arch/IA64_specific.h>     // _M_IA64
//#include <common/include/internal/arch/Alpha_specific.h>    // _M_ALPHA
//#include <common/include/internal/arch/PPC_specific.h>      // _M_PPC
//#include <common/include/internal/arch/MPPC_specific.h>     // _M_MPPC
//#include <common/include/internal/arch/CEE_specific.h>      // _M_CEE
//#include <common/include/internal/arch/CEE_PURE_specific.h> // _M_CEE_PURE
//#include <common/include/internal/arch/CEE_Safe_specific.h> // _M_CEE_SAFE
//#include <common/include/internal/arch/MRX000_specific.h>   // _M_MRX000

	#ifndef SBARCH
		#error "Architecture not supported"
	#endif


//SBCOMPILE_INFO( "  " SBLIB_VISUALCSPECIFIC_H )
//SBCOMPILE_INFO( " --- SBLib © " __DATE__ " (" __TIME__ ") ---" )
SBCOMPILE_INFO( "| " )
SBCOMPILE_INFO( "| *** Microsoft compiler version " CSTR(_MSC_VER) )
SBCOMPILE_INFO( "| *** " CSTR(_INTEGRAL_MAX_BITS) "-bits architecture support" )
SBCOMPILE_INFO( "| *** C++ std version: " CSTR(_MSVC_LANG) )
SBCOMPILE_INFO( "| " )

///////////////////////////////////////////////////////////////////////////////
// dynamic/run-time C++ support
#ifdef _CPPUNWIND
	SBCOMPILE_INFO( "| \x7 Exception support enabled." )
	#define SBEXCEPTIONS

	#if defined(_HAS_EXCEPTIONS)
		#undef _HAS_EXCEPTIONS
	#endif
	#define _HAS_EXCEPTIONS 1

	#if defined(_SECURE_SCL_THROWS)
		#undef _SECURE_SCL_THROWS
	#endif

	#if defined(SB_SECURE_VALIDATION)
	#if defined(_SECURE_SCL)
		#undef _SECURE_SCL
	#endif
	#define _SECURE_SCL        1
	#if !defined(_SCL_SECURE_NO_WARNINGS)
		#define _SCL_SECURE_NO_WARNINGS
	#endif
	#endif
#else // !_CPPUNWIND
	#if defined(_HAS_EXCEPTIONS)
		#undef _HAS_EXCEPTIONS
	#endif
	#define _HAS_EXCEPTIONS 0

	#if defined(_SECURE_SCL_THROWS)
		#undef _SECURE_SCL_THROWS
	#endif
	#define _SECURE_SCL_THROWS 0

	#if defined(_SECURE_SCL)
		#undef _SECURE_SCL
	#endif
	#define _SECURE_SCL        0
#endif
#ifdef _CPPRTTI
	SBCOMPILE_INFO( "| \x7 Run-time type information (RTTI) enabled." )
	#define SBRUNTIME
#endif


///////////////////////////////////////////////////////////////////////////////
//	Debug / Release compilation
#ifdef _DEBUG
	SBCOMPILE_INFO( "| \x7 Compiling DEBUG code." )
	#ifdef SBRELEASE
		#error "Release and Debug mode are incompatible. Please select only one..."
	#endif
	#ifndef SBDEBUG
		#define SBDEBUG
	#endif
#endif
#ifdef _NDEBUG
	SBCOMPILE_INFO( "| \x7 Compiling RELEASE code." )
	#ifdef SBDEBUG
		#error "Release and Debug mode are incompatible. Please select only one..."
	#endif
	#ifndef SBRELEASE
		#define SBRELEASE
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// Multithreading support
#ifdef _MT
	SBCOMPILE_INFO( "| \x7 Multithreaded CRT support." )
	#define SBMULTITHREADEDCRT
#endif
#ifdef _OPENMP
	SBCOMPILE_INFO( "| \x7 OpenMP extended pragma support." )
	#define SBOPENMP
#endif

///////////////////////////////////////////////////////////////////////////////
// Runtime library
#if defined(_DLL)
	SBCOMPILE_INFO( "| \x7 Dynamic (.dll) runtime library." )
	#define SBDLLRUNTIME
#else
	SBCOMPILE_INFO( "| \x7 Static runtime library." )
	#define SBSTATICRUNTIME
#endif

///////////////////////////////////////////////////////////////////////////////
// MS Windows-specific
#ifdef _WIN32
	SBCOMPILE_INFO( "| \x7 May use Win32-specific definitions." )
	#define SBLIB32
#endif
#ifdef _WIN64
	SBCOMPILE_INFO( "| \x7 May use Win64-specific definitions." )
	#define SBLIB64
#endif

// Partial or complete C++0x supports?
#if (_MSC_VER > 1500)
	#define SBLIB_NO_NULLPTR
#endif

#define __SB_FUNCTION__ __FUNCSIG__

///////////////////////////////////////////////////////////////////////////////
// Shall NEVER use min/max macro! Those two damned macros ;P ...
// In fact, even STL min/max shall be overriden for they're not permissive enough...
#ifndef NOMINMAX
	#define NOMINMAX
#endif

#if !defined(_NATIVE_WCHAR_T_DEFINED)
	SBCOMPILE_INFO("| \x7 wchar_t emulated")
#endif
#ifdef _ATL_VER
	#error "Not tested with ATL"
#endif
#ifdef _MANAGED
	#error "Not tested in managed C++"
#endif
#ifdef _MFC_VER
	#error "Not tested with MFC"
#endif
#ifdef _MSC_EXTENSIONS
	SBCOMPILE_INFO("| \x7 Using MSC extensions")
#endif
/*
	List of unstandard definition that may be defined by MS Visual C++ compiler

_CHAR_UNSIGNED
__cplusplus_cli
__COUNTER__
_CPPLIB_VER
_DLL
__FUNCDNAME__
__FUNCSIG__
__FUNCTION__
__MSVC_RUNTIME_CHECKS
_VC_NODEFAULTLIB
_Wp64
*/
#ifdef _DLL
	SBCOMPILE_INFO("| \x7 Building DLL")
#endif
#ifdef __MSVC_RUNTIME_CHECKS
	SBCOMPILE_INFO("| \x7 Using runtime checks")
#endif
#ifdef _CPPLIB_VER
	SBCOMPILE_INFO("| \x7 C++ lib compiler version: " CSTR(_CPPLIB_VER) )
#endif

namespace SB { namespace LibX {

	using ARCH_default = SBARCH;

}} // namespace SBLib

///////////////////////////////////////////////////////////////////////////////
#endif