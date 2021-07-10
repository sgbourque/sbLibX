#pragma once

///////////////////////////////////////////////////////////////////////////////
namespace SB
{

		template< class _TARGET_ARCH_ = void >
	struct ARCH_traits;

} // namespace SB

#define SBLIBX_COMPILERSPECIFIC_H
///////////////////////////////////////////////////////////////////////////////
// These compiler_specific files must at least defines these :
//
//	namespace SB
//	{
//			template<>
//		struct ARCH_traits<...for every arch type>
//		{
//			enum arch_t
//			{
//				... specifications of arch_t category
//			};
//			static inline constexpr arch_t arch_id = target_arch_id; // (unique definition of current maximal supported architecture)
//
//			enum simd_t
//			{
//				... specifications of simd_t category
//			};
//			static inline constexpr simd_t simd_id = target_simd_id; // (unique definition of current maximal supported architecture)
//		};
//	} // namespace SB
//
//	#define SBARCH my_arch_class
//
// May also define these :
//
//	#define SBARCH_ID max_arch_subset
//	#define SBSIMD_ID max_SIMD_subset
//
///////////////////////////////////////////////////////////////////////////////
#define SBCOMPILE_MESSAGE_INFO    0
#define SBCOMPILE_MESSAGE_WARNING 1
#define SBCOMPILE_MESSAGE_ERROR   2

#if defined(_MSC_VER) && defined( _M_X64 )
	#define SBCOMPILE_MESSAGE(...) __pragma( message(__VA_ARGS__) )
#else
	#define SBCOMPILE_MESSAGE(...) __pragma( __VA_ARGS__ )
	#error "Compiler/Platform not supported yet."
#endif


#define SBCOMPILE_MESSAGE_INFO    0
#define SBCOMPILE_MESSAGE_WARNING 1
#define SBCOMPILE_MESSAGE_ERROR   2
#ifndef SBCOMPILE_VERBOSE_MODE
	#if defined(SBDEBUG)
	#define SBCOMPILE_VERBOSE_MODE SBCOMPILE_MESSAGE_WARNING
	#else
	#define SBCOMPILE_VERBOSE_MODE SBCOMPILE_MESSAGE_WARNING
	#endif
#endif
#if SBCOMPILE_VERBOSE_MODE <= SBCOMPILE_MESSAGE_INFO
	#define SBCOMPILE_INFO(...) SBCOMPILE_MESSAGE(__VA_ARGS__)
#else
	#define SBCOMPILE_INFO(...)
#endif
#if SBCOMPILE_VERBOSE_MODE <= SBCOMPILE_MESSAGE_WARNING
	#define SBCOMPILE_WARNING(...) SBCOMPILE_MESSAGE(__VA_ARGS__)
#else
	#define SBCOMPILE_WARNING(...)
#endif
#if SBCOMPILE_VERBOSE_MODE <= SBCOMPILE_MESSAGE_ERROR
	#define SBCOMPILE_ERROR(...) SBCOMPILE_MESSAGE(__VA_ARGS__)
#else
	#define SBCOMPILE_ERROR(...)
#endif

#include <common/include/internal/arch/VisualC_specific.h>


#ifdef __clang__
	#define clang_pragma( ... ) __pragma( clang __VA_ARGS__ )
	#define SB_CLANG_MESSAGE( ... ) __pragma( message( __VA_ARGS__ ) )
#else
	#define clang_pragma( ... )
	#define SB_CLANG_MESSAGE( ... )
#endif

///////////////////////////////////////////////////////////////////////////////
//	SBCOMPILE_INFO( "| " )
//	SBCOMPILE_INFO( "  " __FILE__ " (" CSTR(__LINE__) ")" )
//	SBCOMPILE_INFO( " --- SBLib © " __DATE__ " (" __TIME__ ") ---" )
#if !defined(__STDC__) || !__STDC__
	#if defined(__cplusplus)
	SBCOMPILE_INFO( "| Using ANSI C++-compliant version " CSTR(__cplusplus) )
	#else
		#error "\nNo C++ compiler found and not using ANSI-C compliant compiler. What are you trying to do, exactly?"
	#endif
#else
	#if defined(__cplusplus)
	SBCOMPILE_INFO( "| Using full C/C++ ANSI compliant compiler." )
	#else
	SBCOMPILE_INFO( "| Using ANSI C-compliant compiler" )
	#endif
	#if defined(__STDC_VERSION__ )
	SBCOMPILE_INFO( "| C-compliance version : " CSTR(__STDC_VERSION__) )
	#endif
	#if defined(__STDC_HOSTED__) && __STDC_HOSTED__
	#error "TODO : __STDC_HOSTED__"
	//SBCOMPILE_INFO( "| C-compliance version : " CSTR(__STDC_VERSION__) )
	#endif
#endif
	SBCOMPILE_INFO( "| " )
	SBCOMPILE_INFO( "| Architecture : " CSTR( SBARCH ) )
	#if defined(SBARCH_ID)
	SBCOMPILE_INFO( "| Architecture subset : " CSTR( SBARCH_ID ) )
	#endif
	#if defined(SBSIMD_ID)
	SBCOMPILE_INFO( "| SIMD subset         : " CSTR( SBSIMD_ID ) )
	#endif
	SBCOMPILE_INFO( " ------------------------- " )
	SBCOMPILE_INFO( " " )

///////////////////////////////////////////////////////////////////////////////
#undef SBLIBX_COMPILERSPECIFIC_H
