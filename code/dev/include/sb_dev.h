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
#if defined(SBWIN64)
#define SB_TARGET_PLATFORM			"x64"
#endif
#if defined(_DLL) && (_DLL != 1)
#error "exe"
#endif

	template<typename ...T>
static inline constexpr int sb_noop([[maybe_unused]] T...) { return 0; }
