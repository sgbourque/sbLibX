#include <common/include/sb_hash.h>
#include <common/include/sb_structured_buffer.h>

namespace SB { namespace LibX
{

}} // namespace sbLibX

// WIP: lexer (w/ basic_unicode transform + buffer_view) not yet ready for phase 1 libX integration...

////
#include <iostream>
//#include <common/include/sb_common.h>
SB_EXPORT_TYPE int __stdcall test_parser([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	using sbLibX::operator "" _xhash64;
	using sbLibX::get;
	using std::get;
	std::cout.flush();
	return 0;
}
