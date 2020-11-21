#pragma once

namespace SB { namespace LibX
{

static inline constexpr size_t align_down( size_t size, size_t align ) { return ( size / align ) * align; }
static inline constexpr size_t align_up( size_t size, size_t align ) { return align_down( size + align-1, align ); }

}} // namespace SB:LibX
