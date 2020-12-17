#include <common/include/sb_range.h>
#include <common/include/sb_encrypted_string.h>
#include <common/include/sb_structured_buffer.h>

#include <compare>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <set>

//#include <cassert>
#define assert(X)\
	do{ if (!(X)) __debugbreak(); } while(false)


#define VALIDATE (false)


namespace SB { namespace LibX
{

namespace Character
{

// First let's support only basic properties of ASCII, just enough
// to parse unicode data files

// General_Category (gc)
enum class General_Category : uint32_t
{
#define SBDEF_GC_PROPERTY(property_name, category_alias, category_name, misc_alias, value) \
	category_name = value, category_alias = category_name,
#include "common/include/internal/unicode/property_value_aliases.h"
// @missing: 0000..10FFFF; General_Category; Unassigned
};
sb_enum_class_flags(General_Category);
static std::unordered_map<General_Category, const char*> general_category_name =
{
#define SBDEF_GC_PROPERTY(property_name, category_alias, category_name, misc_alias, value) \
	{General_Category :: category_name, ("" # category_name)},
#include "common/include/internal/unicode/property_value_aliases.h"
};

// Canonical_Combining_Class( ccc )
enum class Canonical_Combining_Class : uint8_t
{
#define SBDEF_CCC_PROPERTY(property_name, value, class_alias, class_name) \
	class_name = value, class_alias = class_name,
#include "common/include/internal/unicode/property_value_aliases.h"
};
static std::unordered_map<Canonical_Combining_Class, const char*> canonical_combining_class_name =
{
#define SBDEF_CCC_PROPERTY(property_name, value, class_alias, class_name) \
	{Canonical_Combining_Class :: class_name, ("" # class_name)},
#include "common/include/internal/unicode/property_value_aliases.h"
};

enum class Bidi_Class : uint32_t
{
#define SBDEF_BIDI_PROPERTY(property_name, bidi_alias, bidi_name, value) \
	bidi_name = value, bidi_alias = bidi_name,
#include "common/include/internal/unicode/property_value_aliases.h"
};
sb_enum_class_flags(Bidi_Class);
static std::unordered_map<Bidi_Class, const char*> bidi_class_name =
{
#define SBDEF_BIDI_PROPERTY(property_name, bidi_alias, bidi_name, value) \
	{Bidi_Class :: bidi_name, ("" # bidi_name)},
#include "common/include/internal/unicode/property_value_aliases.h"
};

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
	static inline constexpr unicode_subset code_point = unicode_subset{ - 1};
	Simple_Character_Mapping( underlying_type_t _code_point = code_point ) : value( unicode_subset{ _code_point } ) {}
	auto operator <=>( const Simple_Character_Mapping& ) const = default;

	unicode_subset value;
};


// based on Unicode 13.0.0: UCD / UnicodeData.txt
template< typename unicode_subset = char32_t >
struct unicode_data
{
	using unicode_subset_t = unicode_subset;
	using simple_character_mapping_t = Simple_Character_Mapping<unicode_subset_t>;
	
	unicode_subset_t code_point;
	const char* name;
	General_Category general_category;
	Canonical_Combining_Class canonical_combining_class;
	Bidi_Class bidi_class;
	const char* decomposition_type;
	DecimalDigit decimal_value;
	DecimalDigit digit_value;
	const char* numeric_value;
	Bidi_Mirrored bidi_mirrored;
	const char* unicode_1_name_obsolete;
	const char* unicode_comment_obsolete;
	simple_character_mapping_t simple_uppercase_mapping;
	simple_character_mapping_t simple_lowercase_mapping;
	simple_character_mapping_t simple_titlecase_mapping;
	
	unicode_data() = delete;
	
	unicode_data(
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
		simple_character_mapping_t _simple_uppercase_mapping = {},
		simple_character_mapping_t _simple_lowercase_mapping = {},
		simple_character_mapping_t _simple_titlecase_mapping = {}
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
		simple_uppercase_mapping ( _simple_uppercase_mapping != simple_character_mapping_t{} ? _simple_uppercase_mapping : _code_point ),
		simple_lowercase_mapping ( _simple_lowercase_mapping != simple_character_mapping_t{} ? _simple_lowercase_mapping : _code_point ),
		simple_titlecase_mapping ( _simple_titlecase_mapping != simple_character_mapping_t{} ? _simple_titlecase_mapping : _simple_uppercase_mapping != simple_character_mapping_t{} ? _simple_uppercase_mapping : _code_point )
	{}
};

// ASCII unicode subset (UnicodeData.txt)
// These ones deserve an enum since they are the most used ones in C++
enum basic_latin_t : int8_t
{
#define SBDEF_NAME_ALIAS( code_point, name, alias, tag ) \
	alias = code_point,
#include "common/include/internal/unicode/name_aliases_basic_latin.h"
};
using ascii_t = basic_latin_t;
static std::unordered_map<ascii_t, const char*> ascii_name =
{
#define SBDEF_NAME_ALIAS( code_point, name, alias, tag ) \
	{ascii_t :: alias, ("" # name)},
#include "common/include/internal/unicode/name_aliases_basic_latin.h"

#define SBDEF_UNICODE_DATA(	code_point, name, gc, ccc, bidi,\
							decomposition, decimal, digit, value, mirror, legacy, comment, upper, lower, title ) \
	{ascii_t{code_point}, ("" # name)},
#include "common/include/internal/unicode/unicode_data_basic_latin.h"
#undef SBDEF_UNICODE_DATA
};


/*static inline constexpr*/ unicode_data<ascii_t> ascii_data[] =
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

}

#if 1
// WIP: lexer (w/ basic_unicode transform + buffer_view) - phase 1 libX integration...
////
constexpr auto UTF8_to_UTF32(const char utf8_input[], size_t max_length)
{
	const char* utf8_iterator = utf8_input;
#if VALIDATE
	const char* const utf8_end = utf8_input + max_length;
	uint32_t length = 0;
#endif

	uint32_t partmask = 0x7f;
	uint32_t fullmask = 0;
	uint32_t utf32 = 0;

	uint32_t nextcode = static_cast<unsigned char>(*utf8_iterator);
	uint32_t nextpart = (nextcode & partmask);
	partmask = 0x3f;
	do
	{
		assert((utf32 == 0) || (nextcode & 0xC0) == 0x80);
		uint32_t shiftpart = ((utf32 & fullmask) << 6);
		assert(shiftpart + nextpart == (shiftpart | nextpart));
		utf32 = shiftpart | nextpart;

		++utf8_iterator;
		nextcode = (--max_length > 0) ? static_cast<unsigned char>(*utf8_iterator) : 0;
		nextpart = (nextcode & partmask);
		fullmask = (fullmask << 5) | partmask;
#if VALIDATE
		assert(utf8_iterator <= utf8_end);
		++length;
#endif
	}
	while ((nextcode & 0xC0u) == 0x80);

#if VALIDATE
	// Validate UTF-8 input :
	// Any valid sequence U crom from one of these masks :
	//	0xxxxxxx, 0x00 <= U <= 0x7F;
	//	0x110xxxxx 10xxxxxx, 0x0080 <= U <= 0x07FF;
	//	0x1110xxxx 10xxxxxx 10xxxxxx, 0x0800 <= U <= 0xFFFF,
	// or
	//	0x11110xxx 10xxxxxx 10xxxxxx 10xxxxxx, 0x10000 <= U <= 0x10FFFF.
	switch (length)
	{
	case 1:
		assert((utf8_input[0] & 0x80) == 0x00);
		break;
	case 2:
		assert((utf8_input[0] & 0xE0) == 0xC0 && (utf8_input[1] & 0xC0) == 0x80);
		break;
	case 3:
		assert((utf8_input[0] & 0xF0) == 0xE0 && (utf8_input[1] & 0xC0) == 0x80 && (utf8_input[2] & 0xC0) == 0x80);
		break;
	case 4:
		assert((utf8_input[0] & 0xF8) == 0xF0 && (utf8_input[1] & 0xC0) == 0x80 && (utf8_input[2] & 0xC0) == 0x80 && (utf8_input[3] & 0xC0) == 0x80);
		break;
	default:
		__debugbreak();
		break;
	}
	// And then validate it is not an UTF16 surrogate or an out-of-bound code.
	assert(utf32 < 0x110000 && (utf32 < 0xD800 || utf32 > 0xDFFF));
	assert(utf8_iterator == utf8_input + length);
#endif
	return std::make_tuple(static_cast<char32_t>(utf32), utf8_iterator);
}

std::string UTF32_to_UTF8(const char32_t utf32)
{
	std::string utf8{};
	utf8.reserve(4);
	//	0xxxxxxx, 0x00 <= U <= 0x7F;
	//	0x110xxxxx 10xxxxxx, 0x0080 <= U <= 0x07FF;
	//	0x1110xxxx 10xxxxxx 10xxxxxx, 0x0800 <= U <= 0xFFFF,
	// or
	//	0x11110xxx 10xxxxxx 10xxxxxx 10xxxxxx, 0x10000 <= U <= 0x10FFFF.
	if (utf32 < 0x80)
	{
		utf8.resize(1);
		utf8[0] = (utf32 & 0x7F);
	}
	else if (utf32 < 0x0800)
	{
		utf8.resize(2);
		utf8[0] = static_cast<char>(0xC0 | ((utf32 >> 6) & 0x3F));
		utf8[1] = static_cast<char>(0x80u | (utf32 & 0x3Fu));
	}
	else if (utf32 < 0x00010000)
	{
		utf8.resize(3);
		utf8[0] = static_cast<char>(0xE0 | ((utf32 >> 12) & 0x3F));
		utf8[1] = static_cast<char>(0x80 | ((utf32 >> 6) & 0x3F));
		utf8[2] = static_cast<char>(0x80 | (utf32 & 0x3F));
	}
	else
	{
		utf8.resize(4);
		utf8[0] = static_cast<char>(0xF0 | ((utf32 >> 18) & 0x3F));
		utf8[1] = static_cast<char>(0x80 | ((utf32 >> 12) & 0x3F));
		utf8[2] = static_cast<char>(0x80 | ((utf32 >> 6) & 0x3F));
		utf8[3] = static_cast<char>(0x80 | (utf32 & 0x3F));
	}
	assert(std::get<0>(UTF8_to_UTF32(&utf8[0], 4)) == utf32);
	return utf8;
}

////
	template< typename VIEW_TYPE, typename _CONTAINER_, typename TOKEN_TYPE >
auto token_to_view(const _CONTAINER_* container, const TOKEN_TYPE& token)
{
	return VIEW_TYPE(container->data() + token.begin(), token.end() - token.begin());
}

	template< typename _CONTAINER_, typename _HASH_TRAITS_ = sbLibX::xhash_traits<typename _CONTAINER_::value_type, sbLibX::xhash_t, 64, 3, 7> >
struct token_hash_t : std::hash< std::basic_string_view<typename _CONTAINER_::value_type> >
{
	using container_t = _CONTAINER_;
	using value_t = typename container_t::value_type;
	using string_view = std::basic_string_view<value_t>;
	using hash_traits = _HASH_TRAITS_;

	token_hash_t(const container_t* _container) : container(_container) {}

		template< typename token_type >
	auto operator()(const token_type& token) const
	{
		return hash_traits::hash(token_to_view<string_view>(container, token).data());
	}
private:
	const container_t* container;
};
	template< typename container_type >
struct token_equal_t
{
	using container_t = container_type;
	using value_t = typename container_t::value_type;
	using string_view = std::basic_string_view<value_t>;

	token_equal_t(const container_t* _container) : container(_container) {}

	template< typename token_type >
	bool operator ()(const token_type& a, const token_type& b) const
	{
		return token_to_view<string_view>(container, a) == token_to_view<string_view>(container, b);
	}
private:
	const container_t* container;
};

////
	template< typename char_type >
class symbol_lexer_t
{
public:
	using char_t = char_type;
	using view_t = std::basic_string_view<char_t>;
	using unicode_t = char32_t;
	using token_t = index_range_t<size_t>;

	symbol_lexer_t(
		size_t token_buffer_size = default_buffer_size,
		size_t sequence_length = default_sequence_length,
		size_t token_bucket_count = default_bucket_count
	) : token_buffer(token_buffer_size)
		, token_sequence(sequence_length)
		, token_collection(token_bucket_count, token_hash_t<token_buffer_t>(&token_buffer), token_equal_t<token_buffer_t>(&token_buffer))
	{
		token_buffer.resize(0);
		token_sequence.resize(0);
	}

	view_t input(const view_t module_name, const view_t file_name, view_t input);

	constexpr bool add_lexical_separator(unicode_t whitespace_char)
	{
		insert_token<token_type::SILENT>(whitespace_char);
		return whitespace_set.insert(whitespace_char).second;
	}
		template< typename container_type >
	constexpr bool add_lexical_separator(const container_type& whitespace_table)
	{
		bool success = true;
		for (auto whitespace : whitespace_table)
			success &= add_lexical_separator(whitespace);
		return success;
	}

	// raw token data
	auto data() const { return token_buffer.data(); }
	auto datasize() const { return token_buffer.size(); }

	// token sequence data
	auto begin() const { return token_sequence.begin(); }
	auto end() const { return token_sequence.end(); }
	auto size() const { return token_sequence.size(); }
	operator bool() { return !token_sequence.empty(); }

	// lexicon data
	auto count() const { return token_collection.size(); }

protected:
	using bounds = index_range_t<const char_t*>;
	bounds get_next_token(bounds bounds);
	auto get_next_char(bounds bounds);

private:
	using token_buffer_t = std::vector<unicode_t>;
	using token_sequence_t = std::vector<token_t>;
	using token_collection_t = std::unordered_set<token_t, token_hash_t<token_buffer_t>, token_equal_t<token_buffer_t>>;
	enum {
		default_buffer_size = 64 * 1024 / sizeof(unicode_t), // lexicon size (in dword) reserved at creation
		default_sequence_length = 32 * 1024, // word & control sequence length (in token) reserved at creation
		default_bucket_count = 16 * 1024, // hash size at creation
	};

	token_buffer_t     token_buffer;
	token_sequence_t   token_sequence;
	token_collection_t token_collection;

	std::set<unicode_t>	whitespace_set;

private:
	enum class token_type { ACTIVE, SILENT };
		template< token_type token_type = token_type::ACTIVE >
	void activate_token(token_t next_token)
	{
		assert(next_token.begin() < next_token.end());
		token_sequence.emplace_back(next_token);
	}
	template<> void activate_token<token_type::SILENT>(token_t) {}

		template< token_type token_type = token_type::ACTIVE >
	token_t insert_token(unicode_t single_char_token)
	{
		token_t token{ { token_buffer.size(), token_buffer.size() + 1 } };
		token_buffer.emplace_back(single_char_token);
		auto inserted = token_collection.insert(token);
		activate_token<token_type>(*inserted.first);
		if (!inserted.second)
		{
			token_buffer.resize(token.begin());
			token.end() = token.begin();
		}
		return token;
	}
		template< token_type token_type = token_type::ACTIVE >
	token_t insert_token(token_t token)
	{
		assert(token_sequence.size() == 0 || token.begin() >= token_sequence.back().end());
		if (token.size())
		{
			auto inserted = token_collection.insert(token);
			activate_token<token_type>(*inserted.first);
			if (!inserted.second)
			{
				std::fill(token_buffer.data() + token.begin(), token_buffer.data() + token.end(), 0);
				token_buffer.resize(token.begin());
				token.end() = token.begin();
			}
		}
		return token;
	}
};

	template< typename char_type >
auto symbol_lexer_t<char_type>::get_next_char(bounds bounds)
{
#if VALIDATE
	const auto begin = bounds.begin();
#endif
	auto next_uchar = reinterpret_cast<const std::make_unsigned_t<char_t>*>(bounds.data());
	char32_t next_char = bounds.size() ? static_cast<char32_t>(*next_uchar) : 0;
	size_t length = 1;
	if (next_char > 0x7f)
	{
		// > 1 Byte;
		static const size_t max_unicode_length = 4;
		uint32_t distance_to_max_length = (((next_char & 0xF0) ^ 0xF0) >> 4);
		length = std::max<size_t>((4u - distance_to_max_length), 2);
		if (length > max_unicode_length || bounds.size() < length)
		{
			length = 0;
			bounds.end() = bounds.begin();
			next_char = char32_t(-1);
		}
	}
	if (length > 0)
		std::tie(next_char, bounds.begin()) = UTF8_to_UTF32(bounds.data(), length);
	assert(bounds.begin() <= bounds.end());
#if VALIDATE
	assert(bounds.data() == begin + length);
#endif
	return std::make_tuple(next_char, bounds);
}

	template< typename char_type >
typename symbol_lexer_t<char_type>::bounds symbol_lexer_t<char_type>::get_next_token(typename symbol_lexer_t<char_type>::bounds bounds)
{
	token_t token{ { token_buffer.size(), token_buffer.size() } };
	token_buffer.reserve(token_buffer.size() + bounds.size());
	typename symbol_lexer_t<char_type>::bounds incompleteBounds = bounds;
	while (bounds.size())
	{
		char32_t next_char;
		std::tie(next_char, bounds) = get_next_char(bounds);
		auto found_whitespace = whitespace_set.find(next_char);
		if (found_whitespace != whitespace_set.end())
		{
			token = insert_token(token);
			token = insert_token(next_char);
			incompleteBounds = bounds;
		}
		else
		{
			token_buffer.emplace_back(next_char);
			++token.end();
		}
	}

	if (token.size())
	{
		//unfinished token
		bounds.begin() = bounds.end() = incompleteBounds.begin();
		token_buffer.resize(token_buffer.size() - token.size());
	}
	return bounds;
}

	template< typename char_type >
std::basic_string_view<char_type> symbol_lexer_t<char_type>::input(const view_t module_name, const view_t file_name, view_t input)
{
	(void)module_name;
	(void)file_name;

	assert(RangeAccessors::get<0>(input) <= RangeAccessors::get<1>(input));
	auto current = input.data();
	auto end = input.data() + input.size();
	auto next = end;
	while (current != next)
	{
		auto nextToken = get_next_token({ { current, end } });
		std::tie( current, next ) = nextToken.bounds();
	}

	return std::basic_string_view<char_type>(next, static_cast<size_t>(std::distance(next, end)));
}

////
	template<>
auto token_to_view< std::basic_string_view<char> >(const symbol_lexer_t<char>* container, const typename symbol_lexer_t<char>::token_t& token)
{
	static_assert(std::is_same_v<char32_t, typename symbol_lexer_t<char>::unicode_t>);
	const char32_t* token_it = container->data() + token.begin();
	const char32_t* const token_end = container->data() + token.end();

	std::basic_string<char> view;
	while (token_it < token_end)
		view += UTF32_to_UTF8(*(token_it++));
	return view;
}

	template< typename out_char_type >
std::basic_ostream<out_char_type>& operator <<(std::basic_ostream<out_char_type>& out, const symbol_lexer_t<out_char_type>& lexer)
{
	using view_type = typename symbol_lexer_t<out_char_type>::view_t;
	out << "unique token count: " << lexer.count() << "; "
		<< "sequence length: " << std::size(lexer) << "; "
		<< "token data size: " << lexer.datasize()
		<< std::endl;
	size_t last_token = 0;
	for (auto token : lexer)
	{
		const bool newToken = (token.end() > last_token);
		last_token = newToken ? token.end() : last_token;
		const auto token_view = token_to_view<view_type>(&lexer, token);
		if (!(out << token_view))
		{
			out.clear();
			out << '?';
		}
	}
	return out << "(eof)" << std::endl;
}
#endif
}} // namespace sbLibX



////
#include <iomanip>
#include <iostream>
#include <fstream>
//#include <common/include/sb_common.h>
SB_EXPORT_TYPE int SB_STDCALL test_parser([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
#if 1
	using sbLibX::operator "" _xhash64;
	using sbLibX::StructuredBuffer::get;
	using std::get;
	sbLibX::symbol_lexer_t<char> lexer;
	{
		// TODO : de-hardcode this and use Unicode Data (UCD) instead
		using General_Category = sbLibX::Character::General_Category;
		using Bidi_Class = sbLibX::Character::Bidi_Class;

		const auto& ascii_data = sbLibX::Character::ascii_data;
		constexpr General_Category separator_category = General_Category::Z;
		constexpr Bidi_Class       bidi_space = Bidi_Class::White_Space | Bidi_Class::Paragraph_Separator | Bidi_Class::Segment_Separator;
		static const char32_t whitespaces[] = { U'\0', U'\n', U'\t', U'\v', U'\f', U'\r', U' ', U'\uFEFF' };
		for( const auto& c : ascii_data )
		{
			const auto props = ascii_data[c.code_point];
			std::cout << "'" << props.code_point << "' ( " << sbLibX::Character::ascii_name[props.code_point] << " ): " << std::endl
				<< "\tcategory = " << sbLibX::Character::general_category_name[props.general_category] << std::endl
				<< "\tcombine  = " << sbLibX::Character::canonical_combining_class_name[props.canonical_combining_class] << std::endl
				<< "\tbidi     = " << sbLibX::Character::bidi_class_name[props.bidi_class] << std::endl
			;

			//const bool is_separator = anybit( props.general_category & separator_category );
			//const bool is_space = anybit( props.bidi_class & bidi_space );
			//if( is_space || is_separator )
			//{
			//	using code_point_t = std::underlying_type_t<decltype(c.code_point)>;
			//	std::cout << "U+" << std::setw(2) << std::hex << code_point_t{c.code_point} << " ("
			//		<< ( props.unicode_1_name_obsolete ? props.unicode_1_name_obsolete : props.name ) << ")\n";
			//
			//	auto categories = props.general_category;
			//	for( auto currentbit = firstbit( categories ); anybit( currentbit ); currentbit = firstbit( categories ) )
			//	{
			//		std::cout << "\t"  << std::underlying_type_t<General_Category>(currentbit) << std::endl;
			//		categories &= ~currentbit;
			//	}
			//}
		}

		lexer.add_lexical_separator(
			#if 0
				unicode_bidi_white_space
			#else
				whitespaces
			#endif
		);
	}
	{
		const std::string module_name = "";
		const std::string file_name = "ReadMe.md";
		std::ifstream input_file{ file_name };

		std::vector<char> content;
		while (input_file)
		{
			enum { read_size = 32 * 1024, };
			const auto current_size = content.size();
			content.resize(current_size + read_size);
			const auto read_count = input_file.read(content.data() + current_size, read_size).gcount();
			content.resize(current_size + static_cast<size_t>(read_count));
			std::string_view content_data(content.data(), content.size());
			std::string_view unprocessed_content = lexer.input(module_name, file_name, content_data);

			std::copy(unprocessed_content.begin(), unprocessed_content.end(), content.begin());
			content.resize(unprocessed_content.size());
		}
		if (!content.empty())
		{
			content.push_back('\0');
			std::cerr << "Error : Incomplete data :\n" << content.data() << std::endl;
		}
		if (!input_file.eof())
			std::cerr << "Error reading file";
	}
	std::cerr << lexer;

#endif
	std::cout.flush();
	return 0;
}
