#pragma once

#include "common/include/sb_utilities.h"

#include <cstdint>
#include <compare>

namespace SB { namespace LibX { namespace Unicode
{
// derived from Unicode 13.0.0
static inline constexpr char unicode_version[] = "13.0.0";

// General_Category (gc)
enum class General_Category : uint32_t
{
	invalid = 0,
#define SBDEF_GC_PROPERTY(property_name, category_alias, category_name, misc_alias, value) \
	category_name = value, category_alias = category_name,
#include "common/include/internal/unicode/unicode_property_value_aliases.h"
// @missing: 0000..10FFFF; General_Category; Unassigned
};
sb_enum_class_flags(General_Category);

static inline constexpr std::weak_ordering compare( General_Category a, General_Category b )
{
	enum
	{
		control_mask     = std::underlying_type_t<General_Category>(General_Category::C),
		letter_mask      = std::underlying_type_t<General_Category>(General_Category::L),
		mark_mask        = std::underlying_type_t<General_Category>(General_Category::Mark),
		number_mask      = std::underlying_type_t<General_Category>(General_Category::N),
		punctuation_mask = std::underlying_type_t<General_Category>(General_Category::P),
		symbol_mask      = std::underlying_type_t<General_Category>(General_Category::S),
		separator_mask   = std::underlying_type_t<General_Category>(General_Category::Z),
	};
	if( ( a & control_mask     ) != General_Category::invalid ) a |= control_mask;
	if( ( b & control_mask     ) != General_Category::invalid ) b |= control_mask;
	if( ( a & letter_mask      ) != General_Category::invalid ) a |= letter_mask;
	if( ( b & letter_mask      ) != General_Category::invalid ) b |= letter_mask;
	if( ( a & mark_mask        ) != General_Category::invalid ) a |= mark_mask;
	if( ( b & mark_mask        ) != General_Category::invalid ) b |= mark_mask;
	if( ( a & number_mask      ) != General_Category::invalid ) a |= number_mask;
	if( ( b & number_mask      ) != General_Category::invalid ) b |= number_mask;
	if( ( a & punctuation_mask ) != General_Category::invalid ) a |= punctuation_mask;
	if( ( b & punctuation_mask ) != General_Category::invalid ) b |= punctuation_mask;
	if( ( a & symbol_mask      ) != General_Category::invalid ) a |= symbol_mask;
	if( ( b & symbol_mask      ) != General_Category::invalid ) b |= symbol_mask;
	if( ( a & separator_mask   ) != General_Category::invalid ) a |= separator_mask;
	if( ( b & separator_mask   ) != General_Category::invalid ) b |= separator_mask;
	return a <=> b;
}
SB_COMMON_EXPORT const char* get_name( General_Category gc );

// Canonical_Combining_Class( ccc )
enum class Canonical_Combining_Class : uint8_t
{
#define SBDEF_CCC_PROPERTY(property_name, value, class_alias, class_name) \
	class_name = value, class_alias = class_name,
#include "common/include/internal/unicode/unicode_property_value_aliases.h"
	invalid,
};

static inline constexpr std::strong_ordering compare( Canonical_Combining_Class a, Canonical_Combining_Class b )
{
	return a <=> b;
}
SB_COMMON_EXPORT const char* get_name( Canonical_Combining_Class ccc );

enum class Bidi_Class : uint32_t
{
	invalid = 0,
#define SBDEF_BIDI_PROPERTY(property_name, bidi_alias, bidi_name, value) \
	bidi_name = value, bidi_alias = bidi_name,
#include "common/include/internal/unicode/unicode_property_value_aliases.h"
};
sb_enum_class_flags(Bidi_Class);

static inline constexpr std::strong_ordering compare( Bidi_Class a, Bidi_Class b )
{
	return a <=> b;
}
SB_COMMON_EXPORT const char* get_name( Bidi_Class bidi );

struct DecimalDigit
{
	enum value_t : int
	{
		undefined = -1,
		zero,
		one,
		two,
		three,
		four,
		five,
		six,
		seven,
		eight,
		nine
	};
	int value = undefined;
};


enum class Bidi_Mirrored : bool
{
	False = false, F = False, No = F,  N = No,
	True  = true,  T = True,  Yes = T, Y = Yes,
};


	template< typename unicode_subset >
struct Simple_Character_Mapping
{
	using underlying_type_t = std::underlying_type_t<unicode_subset>;
	static inline constexpr unicode_subset parent_code_point = unicode_subset{ - 1};
	constexpr Simple_Character_Mapping( underlying_type_t _code_point = parent_code_point ) : value( unicode_subset{ _code_point } ) {}
	//constexpr auto operator <=>( const Simple_Character_Mapping& ) const = default;
	constexpr operator bool() const { return value != parent_code_point; }

	unicode_subset value;
};


// based on Unicode 13.0.0: UCD / UnicodeData.txt
	template< typename unicode_subset = char32_t >
struct unicode_data
{
	using unicode_subset_t = unicode_subset;
	using simple_character_mapping_t = Simple_Character_Mapping<unicode_subset_t>;
	
	unicode_subset_t code_point = unicode_subset_t{-1};
	const char* name = nullptr;
	General_Category general_category = General_Category::invalid;
	Canonical_Combining_Class canonical_combining_class = Canonical_Combining_Class::invalid;
	Bidi_Class bidi_class = Bidi_Class::invalid;
	const char* decomposition_type = nullptr;
	DecimalDigit decimal_value{DecimalDigit::undefined};
	DecimalDigit digit_value{DecimalDigit::undefined};
	const char* numeric_value{nullptr};
	Bidi_Mirrored bidi_mirrored = Bidi_Mirrored::N;
	const char* unicode_1_name_obsolete = nullptr;
	const char* unicode_comment_obsolete = nullptr;
	simple_character_mapping_t simple_uppercase_mapping = simple_character_mapping_t{};
	simple_character_mapping_t simple_lowercase_mapping = simple_character_mapping_t{};
	simple_character_mapping_t simple_titlecase_mapping = simple_character_mapping_t{};
	
	unicode_data() = default;
	constexpr unicode_data( const unicode_data& ) = default;
	
	constexpr unicode_data(
		unicode_subset_t _code_point,
		const char* _name,
		General_Category _general_category,
		Canonical_Combining_Class _canonical_combining_class,
		Bidi_Class _bidi_class,
		const char* _decomposition_type = nullptr,
		DecimalDigit _decimal_value = DecimalDigit{DecimalDigit::undefined},
		DecimalDigit _digit_value = DecimalDigit{DecimalDigit::undefined},
		const char* _numeric_value = nullptr,
		Bidi_Mirrored _bidi_mirrored = Bidi_Mirrored::N,
		const char* _unicode_1_name_obsolete = nullptr,
		const char* _unicode_comment_obsolete = nullptr,
		[[maybe_unused]] simple_character_mapping_t _simple_uppercase_mapping = {},
		[[maybe_unused]] simple_character_mapping_t _simple_lowercase_mapping = {},
		[[maybe_unused]] simple_character_mapping_t _simple_titlecase_mapping = {}
	) :
		code_point               ( _code_point                ),
		name                     ( _name                      ),
		general_category         ( _general_category != General_Category{} ? _general_category : General_Category::Unassigned ),
		canonical_combining_class( _canonical_combining_class ),
		bidi_class               ( _bidi_class                ),
		decomposition_type       ( _decomposition_type        ),
		decimal_value            ( _decimal_value             ),
		digit_value              ( _digit_value               ),
		numeric_value            ( _numeric_value             ),
		bidi_mirrored            ( _bidi_mirrored             ),
		unicode_1_name_obsolete  ( _unicode_1_name_obsolete   ),
		unicode_comment_obsolete ( _unicode_comment_obsolete  ),
		simple_uppercase_mapping ( _simple_uppercase_mapping ? _simple_uppercase_mapping : _code_point ),
		simple_lowercase_mapping ( _simple_lowercase_mapping ? _simple_lowercase_mapping : _code_point ),
		simple_titlecase_mapping ( _simple_titlecase_mapping ? _simple_titlecase_mapping : _simple_uppercase_mapping != simple_character_mapping_t{} ? _simple_uppercase_mapping : _code_point )
	{}
};


//
	template< typename unicode_subset_t >
static inline constexpr std::strong_ordering compare( const unicode_data<unicode_subset_t>& props_a, const unicode_data<unicode_subset_t>& props_b )
{
	if( auto cmp = props_a.canonical_combining_class <=> props_b.canonical_combining_class; cmp != 0 )
		return cmp;
	if( auto cmp = props_a.general_category <=> props_b.general_category; cmp != 0 )
		return cmp;

	return props_a.bidi_class <=> props_b.bidi_class;
}

//	template< typename unicode_subset_t >
//static inline constexpr std::strong_ordering compare( unicode_subset_t a, unicode_subset_t b )
//{
//	return a <=> b;
//}
}}} // sbUnicode
namespace sbUnicode = SB::LibX::Unicode;

#include "common/include/internal/unicode/unicode_basic_latin.h"

//
namespace SB { namespace LibX { namespace Unicode
{
	template< typename unicode_subset_t >
static inline auto get_data( char32_t codepoint )
{
	if( unicode_subset_t::begin_value <= codepoint && codepoint < unicode_subset_t::end_value )
		return get_data( static_cast<unicode_subset_t>(codepoint) );

	return unicode_data<unicode_subset_t>{};
}
}}} // sbUnicode

SB_COMMON_DEPENDS
