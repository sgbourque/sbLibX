#include <common/include/sb_hash.h>
#include <common/include/sb_math_base.h>


#ifdef SB_NEW_HASH
#undef SB_NEW_HASH
#endif
#define SB_NEW_HASH			SB_SUPPORTED


namespace SB { namespace LibX
{
#if SB_SUPPORTS( SB_NEW_HASH )
// toward constexpr character encoding support (Unicode will requires huge constexpr tables, not sure it will hold)
	template< typename _CHAR_TYPE_ >
struct default_encoding_traits_t
{
	using char_t = _CHAR_TYPE_;

	struct canonical_decomposition_size_t { size_t data_chunk_length; size_t decomposition_digits; };
	static inline constexpr canonical_decomposition_size_t canonical_decomposition_size( [[maybe_unused]] const char_t* string, [[maybe_unused]] size_t length )
	{
		return { 1, 1 };
	}
	static inline constexpr char_t canonical_decomposition_digit( const char_t* string, size_t length, size_t decomposition_index )
	{
		const auto [data_length, decomposition_length] = canonical_decomposition_size( string, length );
		return decomposition_index < decomposition_length && length >= data_length && string != nullptr ? *string : char_t{};
	}
};


// hash 2 (lending toward constexpr Unicode support)
using default_hash_type = uintptr_t;

enum: default_hash_type
{
	default_hash_prime   = 0x9EF3455AD47C9E31ull,
	default_hash_coprime = 0x03519CFFA7F0F405ull,
};

	template< typename _HASH_TYPE_, _HASH_TYPE_ _PRIME_, _HASH_TYPE_ _COPRIME_ >
struct coprime_hash_traits_base_t
{
	using hash_t = _HASH_TYPE_;
	enum: hash_t
	{
		prime = _PRIME_,
		coprime = _COPRIME_,
	};
	static_assert( sbLibX::gcd<hash_t>(prime, coprime) == 1, "coprime_traits: coprime pair is not a coprime pair! Choose wisely..." );
	static_assert( sbLibX::bit_count( prime ^ coprime ) > ( sizeof( hash_t ) - 1 ) * CHAR_BIT / 2, "coprime_traits: coprime pair are too similar for hashing." );
	static_assert( 0 != ( ( prime ^ coprime ) >> ( ( sizeof( hash_t ) - 1 ) * CHAR_BIT ) ), "coprime_traits: coprime pair is suboptimal for hashing. Please choose large coprime integers." );

	template< typename _DATA_TYPE_ >
	static inline constexpr auto hash_combine( hash_t hash, _DATA_TYPE_ input )
	{
		static_assert( std::is_constructible_v<hash_t, _DATA_TYPE_>, "hash_traits: data should be divided in binary chunks smaller than the hash size." );
		// could be hash * prime + input * coprime but this would be sub-optimal on x64 arch
		return input == _DATA_TYPE_{}  ? hash : ( ( hash + input * prime ) ^ coprime );
	};
};
	template< typename _HASH_TYPE_ >
using default_coprime_hash_traits_t = coprime_hash_traits_base_t<_HASH_TYPE_, _HASH_TYPE_( default_hash_prime & _HASH_TYPE_( ~0 ) ), _HASH_TYPE_( default_hash_coprime & _HASH_TYPE_( ~0 ) )>;

static_assert( default_coprime_hash_traits_t<default_hash_type>::hash_combine( 0, 0 ) == 0, "hash_combine: bad zero-termination support." );
static_assert( default_coprime_hash_traits_t<default_hash_type>::hash_combine( 127, 0 ) == 127, "hash_combine: bad zero-termination support." );
static_assert( default_coprime_hash_traits_t<default_hash_type>::hash_combine( 0, ' ' ) != 0, "hash_combine: bad white-space support" );


	template< typename _CHAR_TYPE_, typename _HASH_TYPE_, typename _CHAR_ENCODING_TRAITS_ = default_encoding_traits_t<_CHAR_TYPE_>, typename _HASH_FUNCTION_TRAITS_ = default_coprime_hash_traits_t<_HASH_TYPE_> >
struct hash_traits_base_t
{
	using char_t = _CHAR_TYPE_;
	using hash_t = _HASH_TYPE_;
	using encoding_traits_t = _CHAR_ENCODING_TRAITS_;
	using hash_function_traits_t = _HASH_FUNCTION_TRAITS_;

	// Assume null-terminated strings of max length.
	static inline constexpr hash_t hash_combine( hash_t hash, const char_t* string, size_t length )
	{
		if ( length == 0 || *string == char_t{} )
		{
			return hash;
		}
		else
		{
			const auto [data_length, decomposition_length] = encoding_traits_t::canonical_decomposition_size( string, length );
			for ( size_t decomposed_char = 0; decomposed_char < decomposition_length; ++decomposed_char )
			{
				hash = hash_combine( hash, encoding_traits_t::canonical_decomposition_digit( string, data_length, decomposed_char ) );
			}
			return hash_combine( hash, string + data_length, length - data_length );
		}
	}

	static inline constexpr hash_t hash_combine( hash_t hash, const char_t data )
	{
		return hash_function_traits_t::hash_combine( hash, data );
	}

	static inline constexpr hash_t hash_combine( hash_t hash, const hash_t data )
	{
		return hash_function_traits_t::hash_combine( hash, data );
	}

	template< size_t _DATA_COUNT_ >
	static inline constexpr hash_t hash_combine( hash_t hash, const char_t (&data)[_DATA_COUNT_] )
	{
		hash_t raw_hash[sbLibX::align_up( sizeof( data ), sizeof( hash_t ) ) / sizeof( hash_t )];
		for( size_t digit_index = 0; digit_index < _DATA_COUNT_; ++digit_index )
		{
			const size_t hash_digit   = ( digit_index * sizeof( char_t ) ) / sizeof( hash_t );
			const size_t digit_offset = digit_index - ( hash_digit * sizeof( hash_t ) / sizeof( char_t ) );
			raw_hash[hash_digit] |= ( data[digit_index] << hash_t{ digit_offset * sizeof( char_t ) * CHAR_BIT } );
		}
		for( const hash_t raw : raw_hash )
		{
			hash = hash_combine( hash, raw );
		}
		return hash;
	}
};
	template< typename _CHAR_TYPE_, typename _HASH_TYPE_ >
using hash_traits_t = hash_traits_base_t<_CHAR_TYPE_, _HASH_TYPE_>;

	template<typename _CHAR_T, size_t _LENGTH_, typename _HASH_TYPE_ = default_hash_type, template<typename, typename> typename _TRAITS_ = hash_traits_t >
static inline constexpr auto coprime_hash( const _CHAR_T( &string )[_LENGTH_] )
{
	return _TRAITS_<_CHAR_T, _HASH_TYPE_>::hash_combine( _HASH_TYPE_{}, string, _LENGTH_ );
}
	template<typename _CHAR_T, typename _HASH_TYPE_ = default_hash_type, template<typename, typename> typename _TRAITS_ = hash_traits_t >
static inline constexpr auto coprime_hash( const _CHAR_T* string, size_t length )
{
	return _TRAITS_<_CHAR_T, _HASH_TYPE_>::hash_combine( _HASH_TYPE_{}, string, length );
}

constexpr default_hash_type operator "" _hash( [[maybe_unused]] const char* string, [[maybe_unused]] size_t length )
{
	return coprime_hash( string, length );
}


static_assert( coprime_hash( " " ) != 0, "sb_hash: bad implementation" );
static_assert( coprime_hash( "abc" ) == coprime_hash( "abc\0" ), "sb_hash: bad zero-termination support" );
static_assert( hash_traits_t<char, default_hash_type>::hash_combine(
		hash_traits_t<char, default_hash_type>::hash_combine( 0, "ab", 3 ),
		"cd", 3
	) == hash_traits_t<char, default_hash_type>::hash_combine( 0, "abcd", 5 ), "hash_combine: hash is not associative." );

static_assert( coprime_hash( "\U000000C9" ) != 0, "sb_hash: unicode not supported" );
static_assert( coprime_hash( "\U000000B4E" ) != 0, "sb_hash: unicode not supported" );
static_assert( coprime_hash( "\U000000C9" ) == coprime_hash( "É" ), "sb_hash: locale does not support ascii extensions" ); // honestly, I'm surprised that this works even on my locale.
// following tests really requires Unicode decomposition recursive algorithm before passing unicode tests (yep, all that in constexpr, which should require less than 1MB RAM constexpr hashlike table during compiletime)
//static_assert( coprime_hash( "\U000000B4E" ) == coprime_hash( "É" ), "sb_hash: unicode not supported" );
//static_assert( unicode_hash( "\U000000B4E" ) == unicode_hash( L"\U000000B4E" ), "sb_hash: unicode not supported" );
//static_assert( unicode_hash( "\U000000C9" ) == unicode_hash( "\U000000B4E" ), "sb_hash: unicode not supported" );
#endif
}}

#include <string>
namespace SB { namespace LibX
{
#if SB_SUPPORTS( SB_NEW_HASH )
	template<typename _CHAR_T>
static inline constexpr auto coprime_hash( const _CHAR_T* string )
{
	return sbLibX::hash_traits_t<_CHAR_T, sbLibX::default_hash_type>::hash_combine( sbLibX::default_hash_type{}, string, string ? std::basic_string_view<_CHAR_T>( string ).size() : 0 );
}
#endif // #if SB_SUPPORTS( SB_NEW_HASH )
}}

//#include <common/include/sb_structured_buffer.h>

#if SB_SUPPORTS( SB_NEW_HASH )
using sbLibX::operator "" _hash;
#endif


#if SB_SUPPORTS( SB_LEGACY_HASH )
#include "common/include/sb_encrypted_string.h"
using sbLibX::operator "" _xhash64;
#endif // SB_SUPPORTS( SB_LEGACY_HASH )


//////////////////////////////////////////////////////////////////////////
// Start of "unit" test
using char_type = char8_t;


#if ( _MSC_VER > 1900 ) && !defined( __clang__ )
#define SB_STATIC_UNICODE_SUPPORT	SB_SUPPORTED
#else
#define SB_STATIC_UNICODE_SUPPORT	SB_UNSUPPORTED // cannot be supported
#endif

#if SB_SUPPORTS( SB_NEW_HASH )
#define SB_TEST_ENCRYPT_IMPORT_SIMPLE_DICT		( SB_SUPPORTED & SB_NEW_HASH )
#define SB_TEST_ENCRYPT_IMPORT_SIMPLE_HASH		( SB_SUPPORTED & SB_NEW_HASH )
// Following tests takes a while to compile, better do it only for unit tests
#define SB_TEST_ENCRYPT_IMPORT_ENGLISH_DICT		( SB_UNIT_TEST & SB_NEW_HASH )
#define SB_TEST_ENCRYPT_IMPORT_ENGLISH_HASH		( SB_UNIT_TEST & SB_NEW_HASH )
#define SB_TEST_ENCRYPT_IMPORT_FRENCH_DICT		( SB_UNIT_TEST & SB_STATIC_UNICODE_SUPPORT & SB_NEW_HASH )
#define SB_TEST_ENCRYPT_IMPORT_FRENCH_HASH		( SB_UNIT_TEST & SB_STATIC_UNICODE_SUPPORT & SB_NEW_HASH )


#define SB_ENTRY( str ) U8STR(str),
const char_type* raw_lexicon[] = {
	SB_ENTRY( "" ) // no 0-size array
#if SB_SUPPORTS(SB_TEST_ENCRYPT_IMPORT_SIMPLE_DICT)
	#include "../res/simple_lexicon.txt"
#endif
	//this is quite huge, slowing down compile time too much. Not worth doing it at compile-time ATM.
#if SB_SUPPORTS(SB_TEST_ENCRYPT_IMPORT_ENGLISH_DICT)
	#include "../res/english3.txt"
#endif
#if SB_SUPPORTS(SB_TEST_ENCRYPT_IMPORT_FRENCH_DICT)
	#include "../res/francais.txt"
#endif
};
#undef SB_ENTRY
#define SB_ENTRY( str ) str##_hash,
sbLibX::default_hash_type raw_lexicon_hash[] = {
	SB_ENTRY( "" ) // no 0-size array
#if SB_SUPPORTS(SB_TEST_ENCRYPT_IMPORT_SIMPLE_HASH)
	#include "../res/simple_lexicon.txt"
#endif
	//this is quite huge, slowing down compile time too much. Not worth doing it at compile-time ATM.
#if SB_SUPPORTS(SB_TEST_ENCRYPT_IMPORT_ENGLISH_HASH)
	#include "../res/english3.txt"
#endif
#if SB_SUPPORTS(SB_TEST_ENCRYPT_IMPORT_FRENCH_HASH)
	#include "../res/francais.txt"
#endif
};
#undef SB_ENTRY
#endif

#include <unordered_map>
#include <iostream>
#include <iomanip>

//#include <common/include/sb_common.h>
SB_EXPORT_TYPE int SB_STDCALL test_hash2([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
#if SB_SUPPORTS( SB_NEW_HASH )
	using hash_t = sbLibX::default_hash_type;
	std::unordered_map<hash_t, const char_type*> lexicon;
	lexicon.reserve( sbLibX::array_count(raw_lexicon) );
	auto check_duplicate = [&lexicon]( hash_t hash, const char_type* string ) -> bool
	{
		auto dict_entry = lexicon.insert( std::make_pair( hash, string ) );
		if (!dict_entry.second)
		{
			if (!string != !dict_entry.first->second || ( string && dict_entry.first->second && std::basic_string_view<char_type>( string ).compare( dict_entry.first->second ) != 0 ) )
			{
				std::cout << "Duplicate found"
					<< ": \"" << ( string ? reinterpret_cast<const char*>( string ) : "<nullptr>" ) << "\""
					<< " ~ \"" << ( dict_entry.first->second ? reinterpret_cast<const char*>( dict_entry.first->second ) : "<nullptr>" ) << "\""
					<< "; 0x" << std::setw( 16 ) << std::setfill( '0' ) << std::hex << sbLibX::coprime_hash( string )
					<< " = 0x" << std::setw( 16 ) << std::setfill( '0' ) << std::hex << sbLibX::coprime_hash( dict_entry.first->second )
					<< "\n";
			}
		}
		return !dict_entry.second;
	};

	for ( size_t index = 0; index < sbLibX::array_count(raw_lexicon); ++index )
	{
		const char_type* word = raw_lexicon[index];
		hash_t wordHash = index < sbLibX::array_count( raw_lexicon_hash ) ? raw_lexicon_hash[index] : sbLibX::coprime_hash( word );
		//assert( index >= sbLibX::array_count( raw_lexicon_hash ) || wordHash == sbLibX::coprime_hash( word ) );
		check_duplicate( wordHash, word );
	}
#endif
	std::cout.flush();
	return 0;
}

