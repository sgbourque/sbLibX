#pragma once
///////////////////////////////////////////////////////////////////////////////
//SBCOMPILE_MESSAGE( "  " SBLIB_x86SPECIFIC_H )
//SBCOMPILE_MESSAGE( " --- SBLib © " __DATE__ " (" __TIME__ ") ---" )
//SBCOMPILE_MESSAGE( "| " )

///////////////////////////////////////////////////////////////////////////////
namespace SB
{

	struct ARCH_x86;
	#if !defined(_M_IX86)
		#define SBARCHx86_ID SB::ARCH_traits< SB::ARCH_x86 >::arch_t::arch_none
	#else
		#define SBARCHx86_ID static_cast< SB::ARCH_traits< SB::ARCH_x86 >::arch_t >(_M_IX86)
		#if !defined(SBARCH)
			#define SBARCH    SB::ARCH_x86
			#define SBARCH_ID SBARCHx86_ID
		#endif
		#if   _M_IX86 < 200
			SBCOMPILE_MESSAGE( "| \x7 Compiled for 8086/88 or 186 (" CSTR(_M_IX86) ")" )
		#elif _M_IX86 < 300
			SBCOMPILE_MESSAGE( "| \x7 Compiled for 286 (" CSTR(_M_IX86) ")" )
		#elif _M_IX86 < 400
			SBCOMPILE_MESSAGE( "| \x7 Compiled for 386 (" CSTR(_M_IX86) ")" )
		#elif _M_IX86 < 500
			SBCOMPILE_MESSAGE( "| \x7 Compiled for 486 (" CSTR(_M_IX86) ")" )
		#elif _M_IX86 < 600
			SBCOMPILE_MESSAGE( "| \x7 Compiled for pentium (" CSTR(_M_IX86) ")" )
		#elif _M_IX86 < 700
			SBCOMPILE_MESSAGE( "| \x7 Compiled for pentium pro/pentium II/pentium III or better CPU (" CSTR(_M_IX86) ")" )
		#else
			SBCOMPILE_MESSAGE( "| \x7 Compiled for x86 generation " CSTR(_M_IX86) )
		#endif
	#endif

	#if !defined(_M_IX86_FP)
		#define SBSIMDx86_ID SB::ARCH_traits< SB::ARCH_x86 >::simd_t::simd_none
	#else
		#define SBSIMDx86_ID static_cast< SB::ARCH_traits< SB::ARCH_x86 >::simd_t >(_M_IX86_FP)
		#if !defined(SBSIMD_ID)
			#define SBSIMD_ID SBSIMDx86_ID
		#endif
		#if   _M_IX86_FP < 1
			SBCOMPILE_MESSAGE( "| \x7 Compiled using no SIMD intrinsic (" CSTR(_M_IX86_FP) ")" )
		#elif _M_IX86_FP < 2
			SBCOMPILE_MESSAGE( "| \x7 Compiled using SSE intrinsic (" CSTR(_M_IX86_FP) ")" )
		#elif _M_IX86_FP < 3
			SBCOMPILE_MESSAGE( "| \x7 Compiled using SSE2 intrinsic (" CSTR(_M_IX86_FP) ")" )
		#else
			SBCOMPILE_MESSAGE( "| \x7 Compiled using SIMD intrinsic generation " CSTR(_M_IX86_FP) )
		#endif
	#endif

		template<>
	struct ARCH_traits< ARCH_x86 >
	{
		enum class arch_t
		{
			arch_none    =   0, // not supported
			arch_80x86   = 100, // 8086/88, 186
			arch_80286   = 200, // 286
			arch_80386   = 300, // 386
			arch_80486   = 400, // 486
			arch_pentium = 500, // pentium
			arch_default = 600, // pentium pro, pentium II, pentium III or better
		};
		static const arch_t arch_id = SBARCHx86_ID;
	
		enum class simd_t
		{
			simd_none    =         0,
			simd_SSE     =         1,
			simd_SSE2    =         2,
			simd_default = simd_SSE2,
		};
		static const simd_t simd_id = SBSIMDx86_ID;
	};

} // namespace SB
//SBCOMPILE_MESSAGE( "| " )

///////////////////////////////////////////////////////////////////////////////
