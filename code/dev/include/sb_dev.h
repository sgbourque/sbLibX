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

// most dll are also standalone but it is not defined systematically. Define it manually if needs be.
// exe files 

//#if defined(_DLL) && (_DLL != 1)
//#error "exe"
//#endif

	template<typename ...T>
static inline constexpr int sb_noop([[maybe_unused]] T...) { return 0; }
