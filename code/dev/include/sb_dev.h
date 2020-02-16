#pragma once
#include <common/include/sb_common.h>

#if defined(SBDEBUG)
#define SB_TARGET_CONFIGURATION		"Debug"
#elif defined(SBRELEASE)
#define SB_TARGET_CONFIGURATION		"Release"
#elif defined(SBFINAL)
#define SB_TARGET_CONFIGURATION		"Final"
#else
#error "Unknown compilation configuration"
#endif
#if defined(SBWIN64)
#define SB_TARGET_PLATFORM			"x64"
#endif

	template<typename ...T>
static inline constexpr int sb_noop([[maybe_unused]] T...) { return 0; }
