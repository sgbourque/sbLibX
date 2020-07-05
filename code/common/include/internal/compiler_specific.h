#pragma once

///////////////////////////////////////////////////////////////////////////////
namespace SB
{

		template< class _TARGET_ARCH_ = void >
	struct ARCH_traits;

} // namespace SB

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
//			static const arch_t arch_id; // (unique definition of current maximal supported architecture)
//
//			enum simd_t
//			{
//				... specifications of simd_t category
//			};
//			static const simd_t simd_id; // (unique definition of current maximal supported architecture)
//		};
//	} // namespace SB
//
//	#define SBARCH "real" supported architecture class
//
// May also define these :
//
//	#define SBARCH_ID maximal supported architecture subset id
//	#define SBSIMD_ID maximal supported SIMD instruction subset id
//
///////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
	#ifdef SBLOG
		#define SBCOMPILE_MESSAGE(...) __pragma( message(__VA_ARGS__) )
	#else
		#define SBCOMPILE_MESSAGE(...)
	#endif
	#include <SBLib/private/VisualC_specific.h>
#else
	#define SBCOMPILE_MESSAGE(...) __pragma( __VA_ARGS__ )
	#error "Compiler not supported yet."
#endif

///////////////////////////////////////////////////////////////////////////////
//	#pragma SBCOMPILE_MESSAGE( "| " )
//	#pragma SBCOMPILE_MESSAGE( "  " SBLIB_COMPILERSPECIFIC_H )
//	#pragma SBCOMPILE_MESSAGE( " --- SBLib © " __DATE__ " (" __TIME__ ") ---" )
#if !defined(__STDC__) || !__STDC__
	#if defined(__cplusplus)
	#pragma SBCOMPILE_MESSAGE( "| Using ANSI C++-compliant version " CSTRINGIFY(__cplusplus) )
	#else
		#error "\nNo C++ compiler found and not using ANSI-C compliant compiler. What are you trying to do, exactly?"
	#endif
#else
	#if defined(__cplusplus)
	#pragma SBCOMPILE_MESSAGE( "| Using full C/C++ ANSI compliant compiler." )
	#else
	#pragma SBCOMPILE_MESSAGE( "| Using ANSI C-compliant compiler" )
	#endif
	#if defined(__STDC_VERSION__ )
	#pragma SBCOMPILE_MESSAGE( "| C-compliance version : " CSTRINGIFY(__STDC_VERSION__) )
	#endif
	#if defined(__STDC_HOSTED__) && __STDC_HOSTED__
	#error "TODO : __STDC_HOSTED__"
	//#pragma SBCOMPILE_MESSAGE( "| C-compliance version : " CSTRINGIFY(__STDC_VERSION__) )
	#endif
#endif
	#pragma SBCOMPILE_MESSAGE( "| " )
	#pragma SBCOMPILE_MESSAGE( "| Architecture : " CSTRINGIFY( SBARCH ) )
	#if defined(SBARCH_ID)
	#pragma SBCOMPILE_MESSAGE( "| Architecture subset : " CSTRINGIFY( SBARCH_ID ) )
	#endif
	#if defined(SBSIMD_ID)
	#pragma SBCOMPILE_MESSAGE( "| SIMD subset         : " CSTRINGIFY( SBSIMD_ID ) )
	#endif
	#pragma SBCOMPILE_MESSAGE( " ------------------------- " )
	#pragma SBCOMPILE_MESSAGE( " " )

///////////////////////////////////////////////////////////////////////////////
#endif//SBLIB_COMPILERSPECIFIC_H
