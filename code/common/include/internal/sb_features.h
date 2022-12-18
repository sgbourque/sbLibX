#pragma once

#define SB_SUPPORT_MASK		( 0x8000000000000000ull )
#define SB_CATEGORY_BASE	( 0x0001000000000000ull ) // 32767 categories
static_assert( SB_CATEGORY_BASE != 0 && ( SB_CATEGORY_BASE & ( SB_CATEGORY_BASE - 1 ) ) == 0 );
#define SB_FEATURE_MASK		( SB_CATEGORY_BASE - 1 )
static_assert( SB_FEATURE_MASK );

#define SB_CATEGORY_FEATURE	( ( ~SB_SUPPORT_MASK & ( ~SB_SUPPORT_MASK * SB_CATEGORY_BASE ) ) / SB_CATEGORY_BASE )
#define SB_CATEGORY_NONE	( 0ull )

#define SB_FEATURE_CHECK( feature_mask )          ( ( SB_FEATURE_MASK & (feature_mask) ) / ( ( SB_FEATURE_MASK & (feature_mask) ) > 0 ? 1 : 0 ) )
#define SB_FEATURE_FLAG_NOCHECK( feature_bit )    ( SB_FEATURE_MASK & ( 1 << (feature_bit) ) )
#define SB_FEATURE_FLAG( feature_bit )            SB_FEATURE_CHECK( SB_FEATURE_FLAG_NOCHECK( feature_bit ) )
#define SB_FEATURE_CATEGORY( category )           ( ~SB_SUPPORT_MASK & ( (category) * SB_CATEGORY_BASE ) )

#define SB_FEATURE_BIT( category, feature_bit )      ( SB_FEATURE_CATEGORY( category ) | SB_FEATURE_FLAG( feature_bit ) )
#define SB_FEATURE( category, feature_mask )         ( SB_FEATURE_CATEGORY( category ) | SB_FEATURE_CHECK( feature_mask ) )
#define SB_SUPPORT_FEATURE( feature_mask )           ( SB_SUPPORT_MASK | (feature_mask) | SB_FEATURE( SB_CATEGORY_NONE, feature_mask ) )

#define SB_CATEGORY_MASK( feature )  ( ( feature ) & ( ~SB_SUPPORT_MASK & ~SB_FEATURE_MASK ) )
#define SB_SUPPORTS( feature )	     ( ( SB_SUPPORT_MASK | SB_FEATURE_CHECK( feature ) ) & ( SB_SUPPORT_MASK & (feature) ) )

#define SB_SUPPORT_ANY( feature_mask, feature_test ) (                                      \
		   ( ( SB_FEATURE_CHECK( feature_mask ) & SB_FEATURE_CHECK( feature_test ) ) != 0 ) \
		&& ( SB_CATEGORY_MASK( feature_mask ) == SB_CATEGORY_MASK( feature_test ) )         \
		&& SB_SUPPORT_FEATURE( feature_mask )                                               \
	)
#define SB_SUPPORT_ALL( feature_mask, feature_test ) (                                                                     \
		   ( ( SB_FEATURE_CHECK( feature_mask ) & SB_FEATURE_CHECK( feature_test ) ) == SB_FEATURE_CHECK( feature_test ) ) \
		&& ( SB_CATEGORY_MASK( feature_mask ) == SB_CATEGORY_MASK( feature_test ) )                                        \
		&& SB_SUPPORT_FEATURE( feature_mask )                                                                              \
	)

#define SB_UNSUPPORTED		SB_FEATURE( SB_CATEGORY_FEATURE, SB_FEATURE_MASK )
#define SB_SUPPORTED		SB_SUPPORT_FEATURE( SB_UNSUPPORTED )


#if defined(SBDEBUG)
//////////////////////////////////////////////////////////////////////////
//
// These are sanity compile checks
//
//////////////////////////////////////////////////////////////////////////
#if SB_SUPPORTS(SB_UNSUPPORTED)
#error "should not be supported"
#endif
#if SB_SUPPORTS(SB_SUPPORTED)
SBCOMPILE_INFO( "\x7 SB feature system supported" )
#else
#error "should be supported"
#endif


//#if SB_SUPPORTS(SB_TEST_FEATURE_UNDEFINED) || !SB_SUPPORTS(SB_TEST_FEATURE_UNDEFINED)
//#error "should not compile : division by zero"
//#endif

#define SB_TEST_FEATURE_INVALID           SB_FEATURE( SB_CATEGORY_FEATURE, 0 )
#define SB_TEST_FEATURE_SUPPORTED_INVALID SB_SUPPORT_FEATURE( SB_TEST_FEATURE_INVALID )
//#if SB_SUPPORTS( SB_TEST_FEATURE_SUPPORTED_INVALID ) || !SB_SUPPORTS( SB_TEST_FEATURE_SUPPORTED_INVALID )
//#error "should not compile : division by zero"
//#endif


#define SB_CATEGORY_TEST				( ~SB_SUPPORT_MASK / SB_CATEGORY_BASE - 1 )
static_assert( SB_CATEGORY_TEST * SB_CATEGORY_BASE == ( SB_SUPPORT_MASK - 2*SB_CATEGORY_BASE ) );
#define SB_TEST_FEATURE_0				SB_FEATURE_BIT( SB_CATEGORY_TEST, 0  )
#define SB_TEST_FEATURE_1				SB_FEATURE_BIT( SB_CATEGORY_TEST, 1 )
#if SB_SUPPORTS( SB_TEST_FEATURE_0 ) || SB_SUPPORTS( SB_TEST_FEATURE_1 )
#error "should not be supported"
#endif
static_assert( SB_CATEGORY_MASK( SB_TEST_FEATURE_0 ) == SB_CATEGORY_TEST * SB_CATEGORY_BASE );
static_assert( SB_CATEGORY_MASK( SB_TEST_FEATURE_1 ) == SB_CATEGORY_TEST * SB_CATEGORY_BASE );


#define SB_TEST_FEATURE_SUPPORTED_0           SB_SUPPORT_FEATURE( SB_TEST_FEATURE_0 )
#define SB_TEST_FEATURE_SUPPORTED_1           SB_SUPPORT_FEATURE( SB_TEST_FEATURE_1 )
#define SB_TEST_FEATURE_SUPPORTED_0_1         SB_SUPPORT_FEATURE( SB_TEST_FEATURE_0 | SB_TEST_FEATURE_1 )
#define SB_TEST_FEATURE_SUPPORTED_0_1_INVALID SB_SUPPORT_FEATURE( SB_TEST_FEATURE_0 & SB_TEST_FEATURE_1 )
static_assert( SB_CATEGORY_MASK( SB_TEST_FEATURE_SUPPORTED_0_1 ) == SB_CATEGORY_TEST * SB_CATEGORY_BASE );


#if !SB_SUPPORTS(SB_TEST_FEATURE_SUPPORTED_0)
#error "should be supported"
#endif
#if !SB_SUPPORTS(SB_TEST_FEATURE_SUPPORTED_1)
#error "should be supported"
#endif
#if !SB_SUPPORTS(SB_TEST_FEATURE_SUPPORTED_0_1)
#error "should be supported"
#endif
//#if SB_SUPPORTS( SB_TEST_FEATURE_SUPPORTED_0_1_INVALID ) || !SB_SUPPORTS( SB_TEST_FEATURE_SUPPORTED_0_1_INVALID )
//#error "should not compile : division by zero"
//#endif


#if !SB_SUPPORT_ANY( SB_TEST_FEATURE_SUPPORTED_0_1, SB_TEST_FEATURE_0 | SB_TEST_FEATURE_1 )
#error "should be supported"
#endif
#if !SB_SUPPORT_ALL( SB_TEST_FEATURE_SUPPORTED_0_1, SB_TEST_FEATURE_0 )
#error "should be supported"
#endif
#if !SB_SUPPORT_ALL( SB_TEST_FEATURE_SUPPORTED_0_1, SB_TEST_FEATURE_1 )
#error "should be supported"
#endif
#if !SB_SUPPORT_ALL( SB_TEST_FEATURE_SUPPORTED_0_1, SB_TEST_FEATURE_0 | SB_TEST_FEATURE_1 )
#error "should be supported"
#endif
#endif
