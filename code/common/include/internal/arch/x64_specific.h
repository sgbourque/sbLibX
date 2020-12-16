#pragma once
///////////////////////////////////////////////////////////////////////////////
//SBCOMPILE_MESSAGE( "  " SBLIB_x64SPECIFIC_H )
//SBCOMPILE_MESSAGE( " --- SBLib © " __DATE__ " (" __TIME__ ") ---" )
//SBCOMPILE_MESSAGE( "| " )

/////////////////////////////////////////////////////////////////////////////////
namespace SB {

	struct ARCH_x64;

	#if !defined(_M_X64)
		#define SBARCHx64_ID SB::ARCH_traits< SB::ARCH_x64 >::arch_t::arch_none
		#error "unsupported yet"
		//#ifdef __AVX2__
		//	//AVX2
		//#elif defined ( __AVX__ )
		//	//AVX
		//#elif (defined(_M_AMD64) || defined(_M_X64))
		//	//SSE2 x64
		//#elif _M_IX86_FP == 2
		//	//SSE2 x32
		//#elif _M_IX86_FP == 1
		//	//SSE x32
		//#else
		//		//nothing
		//#endif

	#else
		#define SBARCHx64_ID ( SB::ARCH_traits< SB::ARCH_x64 >::arch_t{_M_X64} )
		#if !defined(SBARCH)
			#define SBARCH    SB::ARCH_x64
			#define SBARCH_ID SBARCHx64_ID
		#endif
		#if _M_X64 < 200
			SBCOMPILE_MESSAGE( "| • Compiled for generic x64 (" CSTR(_M_X64) ")" )
		#else
			SBCOMPILE_MESSAGE( "| • Compiled for x64 generation " CSTR(_M_X64) )
		#endif
	#endif

	#if !defined(_M_X64_FP)
		#define SBSIMDx64_ID SB::ARCH_traits< SB::ARCH_x64 >::simd_t::simd_default
		SBCOMPILE_MESSAGE( "| • Using default SIMD" )
	#else
		#define SBSIMDx64_ID static_cast< SB::ARCH_traits< SB::ARCH_x64 >::simd_t >(_M_X64_FP)
		#if !defined(SBSIMD_ID)
			#define SBSIMD_ID SBSIMDx64_ID
		#endif
		#if _M_X64 < 200
			SBCOMPILE_MESSAGE( "| • Compiled for generic x64 (" CSTR(_M_X64) ")" )
		#else
			SBCOMPILE_MESSAGE( "| • Compiled for x64 generation " CSTR(_M_X64) )
		#endif
	#endif

		template<>
	struct ARCH_traits< ARCH_x64 >
	{
		enum class arch_t
		{
			arch_none    =        0, // not supported
			arch_x64     =      100, // generic x64
			arch_default = arch_x64,
		};
		static inline constexpr arch_t arch_id = SBARCHx64_ID;
	
		enum class simd_t
		{
			simd_SSE2    =   0,
			simd_default =   1,
		};
		static inline constexpr simd_t simd_id = SBSIMDx64_ID;
	};

} // namespace SB
//SBCOMPILE_MESSAGE( "| " )

///////////////////////////////////////////////////////////////////////////////
