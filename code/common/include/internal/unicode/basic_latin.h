#pragma once

//should be included from "common/include/sb_unicode.h"

namespace SB {
namespace LibX {
namespace Unicode
{

// ASCII unicode subset (UnicodeData.txt)
// These ones deserve an enum since they are the most used ones in C++
enum basic_latin_t : uint8_t
{
	begin_value = 0,
#define SBDEF_NAME_ALIAS( code_point, name, alias, tag ) \
	alias = code_point,
#include "common/include/internal/unicode/name_aliases_basic_latin.h"
	end_value,
};
using ascii_t = basic_latin_t;


static inline constexpr unicode_data<ascii_t> ascii_data[] =
{
#define SBDEF_UNICODE_DATA(	code_point, name, gc, ccc, bidi,\
							decomposition, decimal, digit, value, mirror, legacy, comment, upper, lower, title ) \
		{\
			ascii_t{code_point}, {"" # name}, General_Category :: gc, Canonical_Combining_Class{ ccc }, Bidi_Class :: bidi,\
			{"" # decomposition}, {decimal}, {digit}, ("" # value), Bidi_Mirrored :: mirror, ("" # legacy), ("" # comment),\
			ascii_t{upper}, ascii_t{lower}, ascii_t{title}, \
		},

#include "common/include/internal/unicode/unicode_data_basic_latin.h"
#undef SBDEF_UNICODE_DATA
};
static inline auto get_data( ascii_t codepoint )
{
	if( ascii_t::begin_value <= codepoint && codepoint < ascii_t::end_value )
		return ascii_data[codepoint];

	return unicode_data<ascii_t>{};
}

SB_COMMON_EXPORT const char* get_name( ascii_t a );


std::partial_ordering compare( ascii_t a, ascii_t b )
{
	std::partial_ordering result{ std::partial_ordering::unordered };

	using unicode_subset_t = ascii_t;
	if( unicode_subset_t::begin_value <= std::min(a, b) && std::max(a, b) < unicode_subset_t::end_value )
	{
		const auto unicode_data_a = get_data(a);
		const auto unicode_data_b = get_data(b);

		result = compare( unicode_data_a, unicode_data_b );
	}

	return result;
}

bool is_less( const unicode_data<ascii_t>& unicode_data_a, const unicode_data<ascii_t>& unicode_data_b )
{
	if ( auto cmp = compare( unicode_data_a, unicode_data_b ); cmp != 0 )
		return cmp < 0;

	return unicode_data_a.code_point < unicode_data_b.code_point;
}
bool is_less( ascii_t a, ascii_t b )
{
	if ( auto cmp = compare( a, b ); cmp != 0 )
		return cmp < 0;

	return a < b;
}

}}} // sbUnicode
