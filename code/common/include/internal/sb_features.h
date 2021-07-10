#pragma once

#define SB_FEATURE_MASK		(~0ull)

#define SB_FEATURE( feature )		((0 feature 0))
#define SB_SUPPORTS( feature )		(SB_FEATURE(feature) != 0)

#define SB_SUPPORT_FEATURE(feature)	|feature) | (feature|
#define SB_SUPPORTED		SB_SUPPORT_FEATURE(SB_FEATURE_MASK)
#define SB_UNSUPPORTED		SB_SUPPORT_FEATURE(0)


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
#if !SB_SUPPORTS(SB_SUPPORTED)
#error "should be supported"
#endif

#if 0
// None of these should compile. the message is cryptic through mismatching parenthesis though, sorry about that...
#define TEST_ERROR0  0
#define TEST_ERROR1  1
#if SB_SUPPORTS(TEST_UNDEFINED)
SBCOMPILE_INFO( "This should not compile: detected as supported" )
#else
SBCOMPILE_INFO( "This should not compile: detected as unsupported" )
#endif
#if SB_SUPPORTS(TEST_ERROR0)
SBCOMPILE_INFO( "This should not compile: detected as supported" )
#else
SBCOMPILE_INFO( "This should not compile: detected as unsupported" )
#endif
#if SB_SUPPORTS(TEST_ERROR1)
SBCOMPILE_INFO( "This should not compile: detected as supported" )
#else
SBCOMPILE_INFO( "This should not compile: detected as unsupported" )
#endif
#endif

#define SB_TEST_FEATURE_0 1
#define SB_TEST_FEATURE_1 2
#define SB_TEST_FEATURE_UNSUPPORTED     SB_UNSUPPORTED
#define SB_TEST_FEATURE_SUPPORTED_0     SB_SUPPORT_FEATURE(SB_TEST_FEATURE_0)
#define SB_TEST_FEATURE_SUPPORTED_1     SB_SUPPORT_FEATURE(SB_TEST_FEATURE_1)
#define SB_TEST_FEATURE_SUPPORTED_0_1   SB_SUPPORT_FEATURE(SB_TEST_FEATURE_0|SB_TEST_FEATURE_1)
#if SB_SUPPORTS(SB_TEST_FEATURE_UNSUPPORTED)
#error "should not be supported"
#endif
#if !SB_SUPPORTS(SB_TEST_FEATURE_SUPPORTED_0) || !SB_SUPPORTS(SB_TEST_FEATURE_SUPPORTED_1) || !SB_SUPPORTS(SB_TEST_FEATURE_SUPPORTED_0_1)
#error "should be supported"
#endif
#if ( SB_FEATURE(SB_TEST_FEATURE_SUPPORTED_0_1) & SB_TEST_FEATURE_0 ) == 0
#error "should be supported"
#endif
#if SB_FEATURE( SB_TEST_FEATURE_SUPPORTED_0 ) != SB_TEST_FEATURE_0
#error "should be 1"
#endif
#if SB_FEATURE( SB_TEST_FEATURE_SUPPORTED_1 ) != SB_TEST_FEATURE_1
#error "should be 2"
#endif
#if ( SB_FEATURE( SB_TEST_FEATURE_SUPPORTED_0_1 ) & SB_TEST_FEATURE_0 ) == 0
#error "should be supported"
#endif
#if ( SB_FEATURE( SB_TEST_FEATURE_SUPPORTED_0 ) & SB_TEST_FEATURE_1 ) != 0
#error "should not be supported"
#endif
#endif
