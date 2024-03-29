#pragma once

/*///
#include <sb_platform.h>	// system-specific definitions & feature support
#include <sb_report.h>		// logging, checks and validation
#include <sb_terminal.h>	// terminal-type user interface
#include <sb_filesystem.h>	// filesystem (files, user/system/app paths)
#include <sb_memory.h>		// allocators
#include <sb_database.h>	// configuration & data
#include <sb_graphics.h>	// GPU interface
#include <sb_audio.h>		// audio interface
#include <sb_base_maths.h>	// base mathematical definitions (mostly linear and multilinear algebrae, FFTs, etc.)
///*/
//#include <sb_interface.h>	// main SBLibX entry points through commands
namespace SB { namespace LibX
{
}} // namespace SB::LibX

// Looks like it might create link errors in final target if cstdint is not included right into the precompile file
// I'm not sure why it's behaving badly with the vst/asio pattern but linker does not like it
#include <cstdint>

#define SB_LIBX_INTERNAL
