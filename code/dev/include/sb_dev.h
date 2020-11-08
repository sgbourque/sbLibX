#pragma once
#include <common/include/sb_common.h>

#define SB_CLANG_PREFIX_TARGET		"clang-"
#if defined(SBDEBUG)
#define SB_TARGET_CONFIGURATION		"debug"
#elif defined(SBRELEASE)
#define SB_TARGET_CONFIGURATION		"release"
#elif defined(SBFINAL)
#define SB_TARGET_CONFIGURATION		"final"
#else
#error "Unknown compilation configuration"
#endif

#define SB_TARGET_TYPE_STATIC		(1 << 0)
#define SB_TARGET_TYPE_DYNAMIC		(1 << 1)
#define SB_TARGET_TYPE_STANDALONE	(1 << 2)
#if defined(SBWIN64)
#define SB_TARGET_PLATFORM			"x64"
#define SB_TARGET_NAME_STATIC		"lib"
#define SB_TARGET_NAME_DYNAMIC		"dll"
#define SB_TARGET_NAME_STANDALONE	"exe"
#else
#error "Unknown platform"
#endif

#if defined( _WINDLL )
#define SB_TARGET_TYPE	SB_TARGET_TYPE_DYNAMIC
#else
#define SB_TARGET_TYPE	SB_TARGET_TYPE_STATIC
#endif
// most dll are also standalone but it is not defined systematically. Define it manually if needs be.
// exe files 

//#if defined(_DLL) && (_DLL != 1)
//#error "exe"
//#endif

	template<typename ...T>
static inline constexpr int sb_noop([[maybe_unused]] T...) { return 0; }
