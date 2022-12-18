#include "common/include/sb_utilities.h"
#include "common/include/sb_math_base.h"
#include "common/include/sb_hash.h" // for comparison

namespace SB { namespace LibX {

// At least as of end of 2022, both MSVC and Clang now supports non-type template arguments.
// static_apply is expected to be called as sbLibX::static_apply even if it is a macro.
#define SB_STATIC_DATA_PROCESSING		SB_SUPPORTED
#if SB_SUPPORTS( SB_STATIC_DATA_PROCESSING )
	template< auto _DATA_ >
[[maybe_unused]] static inline constexpr std::remove_all_extents_t<decltype( _DATA_ )> static_data_v = _DATA_;
#define static_apply( function, ... ) static_data_v< function( __VA_ARGS__ ) >
#else // #if SB_SUPPORTS( SB_STATIC_DATA_PROCESSING )
SBCOMPILE_MESSAGE( "Warning: compile-time data processing is disabled" )
#define static_assert( false, "Cannot use static_apply. The feature is not supported." );
#endif // #else // #if SB_SUPPORTS( SB_STATIC_DATA_PROCESSING )

// dynamic_apply is expected to be called as sbLibX::static_apply even if it is a macro.
	template< typename _DATA_TYPE_T >
static inline _DATA_TYPE_T&& dynamic_data( _DATA_TYPE_T&& data ) { return std::move( data ); }
#define dynamic_apply( function, ... ) dynamic_data( function( __VA_ARGS__ ) );
}}




// coprime_hash_traits_base_t is very good for string hashing and can be used for
// good in encryption system since it is a reversible process.
// However, is very unsafe precisely because it reversible.
// Since 
//( hash ^ coprime ) =
// For the moment, 
	template< typename _HASH_TYPE_, _HASH_TYPE_ _INTEGRAL_VALUE_, _HASH_TYPE_ _COPRIME_ >
struct coprime_hash_traits_base_t
{
	using hash_t = _HASH_TYPE_;
	static_assert( std::is_integral_v<hash_t>, "hash_traits: hash should first be decomposed down to integral type." );
	enum: hash_t
	{
		integral_value = _INTEGRAL_VALUE_,
		coprime = _COPRIME_,
	};
	static_assert( sbLibX::gcd<hash_t>( integral_value, coprime ) == 1, "coprime_traits: coprime pair is not a coprime pair! Choose wisely..." );
	static_assert( sbLibX::bit_count( integral_value ) > ( sizeof( hash_t ) - sizeof( hash_t ) / 2 + sizeof( hash_t ) / 4 ) * CHAR_BIT / 2, "coprime_traits: first value is too similar to 0." );
	static_assert( sbLibX::bit_count( integral_value ) < ( sizeof( hash_t ) + sizeof( hash_t ) / 2 - sizeof( hash_t ) / 4 ) * CHAR_BIT / 2, "coprime_traits: first value is too similar to -1." );
	static_assert( sbLibX::bit_count( coprime ) > ( sizeof( hash_t ) - sizeof( hash_t ) / 2 + sizeof( hash_t ) / 4 )* CHAR_BIT / 2, "coprime_traits: second value is too similar to 0." );
	static_assert( sbLibX::bit_count( coprime ) < ( sizeof( hash_t ) + sizeof( hash_t ) / 2 - sizeof( hash_t ) / 4 )* CHAR_BIT / 2, "coprime_traits: second value is too similar to -1." );
	static_assert( sbLibX::bit_count( integral_value ^ coprime ) > ( sizeof( hash_t ) - sizeof( hash_t ) / 2 + sizeof( hash_t ) / 4 ) * CHAR_BIT / 2, "coprime_traits: inputs are too similar." );
	static_assert( sbLibX::bit_count( integral_value ^ coprime ) < ( sizeof( hash_t ) + sizeof( hash_t ) / 2 - sizeof( hash_t ) / 4 )* CHAR_BIT / 2, "coprime_traits: inputs are too complementary." );
	static_assert( 0ull != ( ( integral_value ^ coprime ) >> ( ( sizeof( hash_t ) - 1 ) * CHAR_BIT ) ), "coprime_traits: coprime pair is suboptimal for hashing. Please choose large coprime integers." );
	static_assert( (( integral_value & coprime ) & 1) != 0, "Don't use even numbers" );

	template< typename _DATA_TYPE_ >
	static inline constexpr auto hash_combine( hash_t hash, _DATA_TYPE_ input )
	{
		static_assert( std::is_integral_v<_DATA_TYPE_>, "hash_traits: data should be divided in binary chunks of integral type." );
		return hash ^ ( ( hash * integral_value + 1 ) / 2 + input * ( coprime + 1 ) );
	};
	template< typename _DATA_TYPE_ >
	static inline constexpr auto hash_unapply( [[maybe_unused]] hash_t hash, [[maybe_unused]] _DATA_TYPE_ input )
	{
		static_assert( sizeof( _DATA_TYPE_ ) == 0, "hash_unapply does not have an inverse" );
		return hash - input;
	};
};

	template<typename hash_type, uint64_t _SEED_, template< typename _HASH_TYPE_, _HASH_TYPE_, _HASH_TYPE_ > typename _BASE_TRAITS_ = coprime_hash_traits_base_t >
struct coprime_hash_traits
{
	using native_t = uint64_t;
	using hash_t = hash_type;
	enum : native_t
	{
		hash_param1 = ( 2ull * _SEED_ ) + 1, // make sure it is odd
		hash_param2 = sbLibX::modular_inverse( hash_param1 ), // exists since param1 is odd
	};
	// These restrictions should minimally restrict trivial uninteresting cases.
	// Ideally, we would want to eliminate all periodicity (especially ideals).
	// If both hash_param1 and hash_param2 are distinct prime numbers
	static_assert( hash_param1 > 0x7FFFFFFFFFFFFFFFull, "Please choose an integer number with highest two bits 01..." );
	static_assert( hash_param2 > 0x3FFFFFFFFFFFFFFFull, "bad luck, try another large 64-bits integer... pretty sure there's a better one nearby." );
	static_assert( hash_param1 * hash_param2 == 1ull, "modular_inverse is bad" );
	template< native_t _INT1_, native_t _INT2_, native_t _GCD_ = sbLibX::gcd<native_t>( _INT1_, _INT2_ ) > struct coprime_check
	{
		static_assert( _GCD_ == 1ull, "Inputs are not coprimes. Please validate inputs." );
		static inline constexpr auto value = sbLibX::gcd<native_t>( _INT1_, _INT2_ );
	};
	static_assert( coprime_check< hash_param1, hash_param2 >::value == 1ull, "modular inverse did not generate a coprime value. Choose another seed." );
	static_assert( sbLibX::bit_count( hash_param1 ^ hash_param2 ) < 40ull && sbLibX::bit_count( hash_param1 ^ hash_param2 ) > 24ull, "coprimes are too similar/complementary." );
	using traits_base_t = _BASE_TRAITS_<native_t, hash_param1, hash_param2>;

	static inline constexpr hash_t combine( hash_t hash_value, [[maybe_unused]] native_t data )
	{
		if constexpr ( sizeof( hash_t ) > sizeof( native_t ) )
		{
			using sub_type = std::remove_cvref_t<decltype( *std::begin( hash_value ) )>;
			using sub_hash_type = coprime_hash_traits<sub_type, _SEED_, _BASE_TRAITS_>;
			sub_type residue{};
			for( auto& hash: hash_value )
			{
				residue = sub_hash_type::combine( residue, data );
				hash = sub_hash_type::combine( hash, residue );
			}
		}
		else
		{
			static_assert( std::is_constructible_v<hash_t, native_t> );
			hash_value = traits_base_t::hash_combine( hash_value, data );
		}
		return hash_value;
	}

		template< typename _DATA_TYPE_, size_t _SIZE_ >
	static inline constexpr
	hash_t combine( hash_t hash_value, const std::array<_DATA_TYPE_, _SIZE_>& data )
	{
		for ( auto& digit : data )
			hash_value = combine( hash_value, digit );
		return hash_value;
	}
		template< typename _DATA_TYPE_ >
	static inline constexpr
	hash_t combine( hash_t hash_value, const _DATA_TYPE_* data, size_t length )
	{
		for (; length > 0; --length, ++data)
			hash_value = combine( hash_value, *data );
		return hash_value;
	}
};
	template<typename hash_type, uint64_t _SEED_>
using default_coprime_hash_traits = coprime_hash_traits<hash_type, _SEED_>;



// Crypt functions usually are binary functions, taking two buffers as input :
// - the data buffer to be encrypted/decrypted;
// - the key buffer which is used a key;
// even though any function-like object can be used through dynamic_apply
// and, for constexpr objects, same holds for static_apply (when supported).
// With the helper define, programmer only have to define the 
//         template< size_t _MAX_LENGTH_, typename _VALUE_TYPE_T, typename _KEY_TYPE_T >
//     static inline constexpr auto encrypt( const _VALUE_TYPE_T* value_data, size_t value_length, const _KEY_TYPE_T* key_data, size_t key_length );
// implementation and use the defined crypt adapter functions through SB_DEFINE_CRYPT_ADAPTERS.
// and helper functions are created to be able to call either of
//     crypt(                 "msg", std::array{5,6,7,8,9} );
//     crypt( std::array{0,1,2,3,4}, "key" );
//     crypt( std::array{0,1,2,3,4}, std::array{5,6,7,8,9} );
//     crypt(                 "msg", "key" );
#define SB_DEFINE_CRYPT_ADAPTERS( crypt, crypt_impl ) \
		template< typename _VALUE_TYPE_T, size_t _VALUE_LENGTH_, typename _KEY_TYPE_T, size_t _KEY_LENGTH_ > \
	static inline constexpr auto crypt( const std::array< _VALUE_TYPE_T, _VALUE_LENGTH_ >& value_data, _KEY_TYPE_T ( &key )[_KEY_LENGTH_] ) \
	{ \
		static_assert( std::is_integral_v<_VALUE_TYPE_T> ); \
		static_assert( std::is_integral_v<_KEY_TYPE_T> ); \
		return crypt_impl<_VALUE_LENGTH_>( value_data.data(), _VALUE_LENGTH_, key, _KEY_LENGTH_ ); \
	} \
		template< typename _VALUE_TYPE_T, size_t _VALUE_LENGTH_, typename _KEY_TYPE_T, size_t _KEY_LENGTH_ > \
	static inline constexpr auto crypt( _VALUE_TYPE_T ( &value )[_VALUE_LENGTH_], const std::array< _KEY_TYPE_T, _KEY_LENGTH_ >& key_data ) \
	{ \
		static_assert( std::is_integral_v<_VALUE_TYPE_T> ); \
		static_assert( std::is_integral_v<_KEY_TYPE_T> ); \
		return crypt_impl<_VALUE_LENGTH_, _VALUE_TYPE_T, _KEY_TYPE_T>( value, _VALUE_LENGTH_, key_data.data(), _KEY_LENGTH_ ); \
	} \
		template< typename _VALUE_TYPE_T, size_t _VALUE_LENGTH_, typename _KEY_TYPE_T, size_t _KEY_LENGTH_ > \
	static inline constexpr auto crypt( _VALUE_TYPE_T ( &value )[_VALUE_LENGTH_], _KEY_TYPE_T ( &key )[_KEY_LENGTH_] ) \
	{ \
		static_assert( std::is_integral_v<_VALUE_TYPE_T> ); \
		static_assert( std::is_integral_v<_KEY_TYPE_T> ); \
		return crypt_impl<_VALUE_LENGTH_>( value, _VALUE_LENGTH_, key, _KEY_LENGTH_ ); \
	} \
		template< typename _VALUE_TYPE_T, size_t _VALUE_LENGTH_, typename _KEY_TYPE_T, size_t _KEY_LENGTH_ > \
	static inline constexpr auto crypt( const std::array< _VALUE_TYPE_T, _VALUE_LENGTH_ >& value_data, const std::array< _KEY_TYPE_T, _KEY_LENGTH_ >& key_data ) \
	{ \
		static_assert( std::is_integral_v<_VALUE_TYPE_T> ); \
		static_assert( std::is_integral_v<_KEY_TYPE_T> ); \
		return crypt_impl<_VALUE_LENGTH_>( value_data.data(), _VALUE_LENGTH_, key_data.data(), _KEY_LENGTH_ ); \
	}


	template< typename _CHAR_TYPE_, size_t _SEED_, size_t _PUBLIC_KEY_LENGTH_BITS_ = 512, template<typename, size_t> typename _KEY_HASH_TRAITS_ = default_coprime_hash_traits >
struct demo_crypt
{
	using char_t = _CHAR_TYPE_;
	using data_t = uintptr_t;
	using key_t = data_t;
	using key_hash_t = _KEY_HASH_TRAITS_<data_t, _SEED_>;

		template< typename _VALUE_TYPE_T, size_t _VALUE_LENGTH_ >
	static inline constexpr size_t encrypt_size = sbLibX::align_up( sizeof( key_t ) + _VALUE_LENGTH_ * sizeof( _VALUE_TYPE_T ), sizeof( data_t ) );

		template< typename _KEY_TYPE_T >
	static inline constexpr key_t generate_private_key_hash( const _KEY_TYPE_T* key_data, size_t key_length )
	{
		static_assert( std::is_integral_v<_KEY_TYPE_T> );
		data_t private_key{};
		for( ; key_length > 0; --key_length, ++key_data )
		{
			typename key_hash_t::native_t key_digit = *key_data;
			private_key = key_hash_t::combine( private_key, key_digit );
		}
		// make it odd so that it is has an inverse in 64 bits
		if (( private_key & 1 ) == 0)
		{
			private_key = 2 * private_key + 1;
		}
		return private_key;
	}
		template< typename _KEY_TYPE_T >
	static inline constexpr key_t generate_private_key_mask( const _KEY_TYPE_T* key_data, size_t key_length )
	{
		static_assert( std::is_integral_v<_KEY_TYPE_T> );
		data_t key_mask{};
		for( ; key_length > 0; --key_length, ++key_data )
		{
			typename key_hash_t::native_t key_digit = *key_data;
			key_mask = key_hash_t::combine( key_digit, key_mask );
		}
		return key_mask;
	}

		template< size_t _MAX_LENGTH_, typename _VALUE_TYPE_T, typename _KEY_TYPE_T >
	static inline constexpr auto encrypt( const _VALUE_TYPE_T* value_data, size_t value_length, const _KEY_TYPE_T* key_data, size_t key_length )
	{
		using value_t = std::remove_cvref_t<_VALUE_TYPE_T>;
		static_assert( std::is_integral_v<value_t> );
		constexpr size_t block_count = encrypt_size<value_t, _MAX_LENGTH_> / sizeof( data_t );
		using return_t = std::array<data_t, block_count>;

		// TODO next step :
		// generate key_hash/mask variations that can change for each block (instead of key power)
		key_t private_mask = generate_private_key_mask( key_data, key_length );
		key_t private_key = generate_private_key_hash( key_data, key_length );
		// encrypt uses the inverse of private_key; decrypt uses linear public_key.
		private_key = sbLibX::modular_inverse( private_key );

		enum: uint64_t
		{
			prime = 11565865254570073789ull,
			coprime = 14908050745593278309ull,
			prime_inverse = sbLibX::modular_inverse( prime ),
			coprime_inverse = sbLibX::modular_inverse( coprime ),
		};
		// if value_length == 0 && key_length == 0, encrypted should be 0.
		data_t residue = ( key_length * coprime + 1 ) / 2;
		data_t residue_factor = ( value_length * prime * private_key + residue ) ^ private_mask;
		return_t encrypted{ {residue_factor} };

		size_t encryption_mask = private_mask;
		size_t key_power = private_key;
		size_t index_begin = 0;
		for ( size_t block_index = 1; block_index < block_count; ++block_index )
		{
			data_t block{};
			if constexpr ( sizeof( value_t ) < sizeof( data_t ) )
			{
				const size_t next_end = std::min( value_length, sbLibX::align_up( ( index_begin + 1 ) * sizeof( value_t ), sizeof( data_t ) ) );
				const size_t next_count = next_end - index_begin;
				for ( size_t value_index = 0; value_index < next_count; ++value_index)
				{
					constexpr size_t value_bits = ( sizeof( value_t ) * CHAR_BIT );
					constexpr data_t mask = ( 1ull << value_bits ) - 1;
					const size_t value_offset = ( sizeof( value_t ) * CHAR_BIT * value_index );
					const data_t cur_data = ( mask & *value_data++ );
					block |= ( cur_data << value_offset );
				}
				index_begin = next_end;
			}
			else
			{
				block = *value_data++;
			}
			key_power *= private_key;
			encryption_mask = ( encryption_mask ^ key_power );
			data_t encrypted_block = ( block * prime + value_length * coprime ) * key_power + residue;
			encrypted[block_index] = ( encrypted_block ^ encryption_mask );
			residue = key_hash_t::combine( residue, block );
		}
		return encrypted;
	}
		template< size_t _MAX_LENGTH_, typename _VALUE_TYPE_T, typename _KEY_TYPE_T >
	static inline auto decrypt( const _VALUE_TYPE_T* encrypted_data, size_t block_count, const _KEY_TYPE_T* key_data, size_t key_length )
	{
		using value_t = char_t;
		static_assert( std::is_integral_v<value_t> );
		using return_t = std::array<value_t, ( _MAX_LENGTH_ - 1 ) * sizeof( data_t ) / sizeof( value_t ) >;

		return_t decrypted{};
		if( !encrypted_data || block_count < 2 )
		{
			return decrypted;
		}

		key_t private_mask = generate_private_key_mask( key_data, key_length );
		key_t private_key = generate_private_key_hash( key_data, key_length );
		// encrypt uses the inverse of private_key; decrypt uses linear public_key.

		enum: uint64_t
		{
			prime = 11565865254570073789ull,
			coprime = 14908050745593278309ull,
			prime_inverse = sbLibX::modular_inverse( prime ),
			coprime_inverse = sbLibX::modular_inverse( coprime ),
		};
		// if value_length == 0 && key_length == 0, encrypted should be 0.
		data_t residue_factor = ( encrypted_data[0] ^ private_mask );
		data_t residue = ( key_length * coprime + 1 ) / 2;
		size_t value_length = ( residue_factor - residue ) * prime_inverse * private_key;

		size_t key_power = private_key;
		size_t encryption_mask = private_mask;
		size_t value_index = 0;
		for ( size_t block_index = 1; block_index < block_count; ++block_index )
		{
			key_power *= private_key;
			encryption_mask = ( encryption_mask ^ key_power );
			data_t encrypted_block = ( encrypted_data[block_index] ^ encryption_mask );
			data_t block = ( ( encrypted_block - residue ) * key_power - value_length * coprime ) * prime_inverse;
			residue = key_hash_t::combine( residue, block );
			if constexpr ( sizeof( value_t ) < sizeof( data_t ) )
			{
				const size_t next_end = std::min( value_length, sbLibX::align_up( ( value_index + 1 ) * sizeof( value_t ), sizeof( data_t ) ) );
				for (; value_index < next_end; ++value_index)
				{
					constexpr size_t value_bits = ( sizeof( value_t ) * CHAR_BIT );
					constexpr data_t mask = ( 1ull << value_bits ) - 1;
					size_t value_offset = ( sizeof( value_t ) * CHAR_BIT * value_index );
					decrypted[value_index] = ( mask & ( block >> value_offset ) );
				}
			}
			else
			{
				decrypted[value_index++] = block;
			}
		}
		return decrypted;
	}
};



//////////////////////////////////////////////////////////////////////////
// Start of "unit" test
//#define SB_CRYPT_PRE_SEED	14331699416581894933ull
//#define SB_CRYPT_PRE_SEED	14331789426561893933ull
//#define SB_CRYPT_PRE_SEED	0x68B27C027DE16CD5ull
#define SB_CRYPT_PRE_SEED	7546228625962406389ull
#define SB_CRYPT_SEED_KEY	"- \\_/SBLib© - Seed Key - " CSTR( SB_CRYPT_PRE_SEED )
#pragma message( SB_CRYPT_SEED_KEY )
static inline constexpr uint64_t compile_encryption_seed =  0x4000000000000000ull | ( 0x7FFFFFFFFFFFFFFFull &
		default_coprime_hash_traits<uint64_t, SB_CRYPT_PRE_SEED>::combine( 0ull, SB_CRYPT_SEED_KEY, std::size( SB_CRYPT_SEED_KEY ))
	);
#define default_encrypt_impl demo_crypt<char, compile_encryption_seed>::encrypt
#define default_decrypt_impl demo_crypt<char, compile_encryption_seed>::decrypt
SB_DEFINE_CRYPT_ADAPTERS( demo_encrypt, default_encrypt_impl )
SB_DEFINE_CRYPT_ADAPTERS( demo_decrypt, default_decrypt_impl )


#define SB_STATIC_ENCRYPT	( SB_SUPPORTED & SB_STATIC_DATA_PROCESSING )

#include "common/include/sb_utilities.h"
#include <unordered_map>
#include <iostream>
#include <iomanip>


// poor-man proof-of-concept test encryption scheme
#include <array>
namespace SB { namespace LibX
{
//	template< typename _VALUE_TYPE_T, size_t _VALUE_LENGTH_, typename _KEY_TYPE_T, size_t _KEY_LENGTH_ >
//static inline constexpr auto xor_impl( const std::array< _VALUE_TYPE_T, _VALUE_LENGTH_ >& value_data, const std::array< _KEY_TYPE_T, _KEY_LENGTH_ >& key_data )
	template< size_t _VALUE_LENGTH_, typename _VALUE_TYPE_T, typename _KEY_TYPE_T >
static inline constexpr auto xor_impl( const _VALUE_TYPE_T* value, size_t length, const _KEY_TYPE_T* key_data, size_t key_length )
{
	using value_t = std::remove_cvref_t< _VALUE_TYPE_T >;
	std::array< value_t, _VALUE_LENGTH_ > output{};
	for ( size_t index = 0; index < length; ++index )
	{
		value_t xor_mask = key_data[index % key_length] & value_t( -1 );
		output.data()[index] = ( value[index] ^ xor_mask );
	}
	return output;
}
}} // sbLibX
#define xor_crypt_impl xor_impl
SB_DEFINE_CRYPT_ADAPTERS( xor_crypt, sbLibX::xor_crypt_impl )

#define test_encrypt_algorithm		demo_encrypt
#define test_decrypt_algorithm		demo_decrypt

SB_EXPORT_TYPE int SB_STDCALL test_encrypt3( [[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[] )
{
	#define RAW_ENCRYPTION_KEY	"" __TIME__ "-" __DATE__ "\\_/SBLib© - Encryption Key - "
#if 1
	//size_t test_non_invertible = sbLibX::modular_inverse( ( 5u * 7u * 11u ), ( 7u * 13u * 17u ) );
	//std::cout << test_non_invertible << std::endl;
	//size_t test_invertible = sbLibX::modular_inverse( ( 5u * 11u ), ( 7u * 13u * 17u ) );
	//std::cout << test_invertible << std::endl;

	//auto test_encrypt_0  = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", std::array{ 0ull } );
	//auto test_decrypt_0  = test_decrypt_algorithm( test_encrypt_0, "" );
	//std::cout << test_decrypt_0.data() << std::endl;
	//auto test_encrypt_00 = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", std::array{ 0, 0, 0, 0, 0 } );
	//auto test_encrypt_1a = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", std::array{ 1, 0, 0, 0, 0 } );
	//auto test_encrypt_1b = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", std::array{ 0, 1, 0, 0, 0 } );
	//auto test_encrypt_1c = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", std::array{ 0, 0, 1, 0, 0 } );
	//auto test_encrypt_1d = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", std::array{ 0, 0, 0, 1, 0 } );
	//auto test_encrypt_1e = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", std::array{ 0, 0, 0, 0, 1 } );
	//auto test_encrypt_2a = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", std::array{ 0, 1, 1 } );
	//auto test_encrypt_2b = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", std::array{ 0, 0, 0, 1, 1 } );
	//auto test_encrypt_3a = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", std::array{ 1, 0, 1, 0, 0 } );
	//auto test_encrypt_3b = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", std::array{ 0, 0, 1, 0, 1 } );

	auto test_encryption = test_encrypt_algorithm( std::array{ 1, 2, 3, 4, 5 }, RAW_ENCRYPTION_KEY "Safe - Should be kept hidden." );
	auto test_encrypted_key_0 = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", "" );
	auto test_decrypted_key_0 = test_decrypt_algorithm( test_encrypted_key_0, "" );
	std::cout << test_decrypted_key_0.data() << std::endl;
	auto test_encrypted_key_1 = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", "test" );
	auto test_decrypted_key_1 = test_decrypt_algorithm( test_encrypted_key_0, "test" );
	std::cout << test_decrypted_key_1.data() << std::endl;
	auto test_encrypted_key_2 = test_encrypt_algorithm( RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", std::array{ 1,2,3,4,5,6,7,8,9 } );
	auto test_decrypted_key_2 = test_decrypt_algorithm( test_encrypted_key_0, std::array{ 1,2,3,4,5,6,7,8,9 } );
	std::cout << test_decrypted_key_2.data() << std::endl;
#endif

#if 0
#if SB_SUPPORTS( SB_STATIC_ENCRYPT )
	#define SB_STATIC_ENCRYPT_CONSTEXPR	constexpr
	#define sb_apply static_apply
#else
	#define SB_STATIC_ENCRYPT_CONSTEXPR
	#define sb_apply dynamic_apply
#endif
	static constexpr auto safe_encrypted_key    = sbLibX::sb_apply( test_encrypt_algorithm, RAW_ENCRYPTION_KEY "Safe - Should be kept hidden.", std::array{ 1,2,3,4,5,6,7,8,9 } );
	static constexpr auto unsafe_encrypted_key1 = sbLibX::sb_apply( test_encrypt_algorithm, RAW_ENCRYPTION_KEY "Unsafe - Visible in plain text!", std::array{ 1,2,3,4,5,6,7,8,9 } );
	static constexpr auto unsafe_encrypted_key2 = sbLibX::sb_apply( test_encrypt_algorithm, RAW_ENCRYPTION_KEY "Unsafe - might be hidden or not...", std::array{ 1,2,3,4,5,6,7,8,9 } );
	std::cout << "safe_encrypt:    " << safe_encrypted_key.data() << std::endl;
	std::cout << "unsafe_encrypt1: " << unsafe_encrypted_key1.data() << std::endl;
	std::cout << "unsafe_encrypt2: " << unsafe_encrypted_key2.data() << std::endl;
	std::cout << std::endl;

	// constexpr won't compile here so it is not an issue so dynamic_apply offers some protection.
	// static is optionnal.
	static /*constexpr*/
	auto safe_decrypted_key2 = sbLibX::dynamic_apply( test_decrypt_algorithm, safe_encrypted_key, std::array{1,2,3,4,5,6,7,8,9} );
	std::cout << "safe_decrypt:    " << (char*)safe_decrypted_key2.data() << std::endl;

	//// This is very bad as it destroys all the benefit of compile-time encryption!!
	//// constexpr decryption is bad and should not be allowed at all (now how to prevent it?).
	//static constexpr auto unsafe_decrypted_key1 = test_decrypt_algorithm( unsafe_encrypted_key1, std::array{ 1,2,3,4,5,6,7,8,9 } );
	//std::cout << "unsafe_decrypt1: " << (char*)unsafe_decrypted_key1.data() << std::endl;

	// Now that is undefined behaviour since compiler could preprocess the input at compile-time or not.
	// dynamic_apply should definitively be preferred for all decryption.
	auto unsafe_decrypted_key3 = test_decrypt_algorithm( unsafe_encrypted_key2, std::array{ 1,2,3,4,5,6,7,8,9 } );
	std::cout << "unsafe_decrypt3: " << (char*)unsafe_decrypted_key3.data() << std::endl;
	std::cout << std::endl << "-------" << std::endl;

	// Only use a key based on safe_encrypted_key from now on.
	// That key could be very hard to recover since it's encrypted with a
	// compile-time varying encrypted data block.
	static constexpr auto very_safe_encryption_key = sbLibX::sb_apply( test_encrypt_algorithm, "Mangled Key: Could be made very safe with good asymetric algorithms (unlike the xor_crypt).", safe_encrypted_key );
	std::cout << (char*)very_safe_encryption_key.data() << std::endl;
	// While the key itself is very safe, the algorithm can be unsecured and expose themself
	auto safe_decrypted_mangled_key = sbLibX::dynamic_apply( test_decrypt_algorithm, very_safe_encryption_key, safe_encrypted_key );
	std::cout << (char*)safe_decrypted_mangled_key.data() << std::endl;
	std::cout << std::endl;
	auto safe_encrypted_data1 = sbLibX::static_apply( test_encrypt_algorithm, "Compile-time encryption (static)", very_safe_encryption_key );
	std::cout << (char*)safe_encrypted_data1.data() << std::endl;
	auto safe_encrypted_data2 = sbLibX::dynamic_apply( test_encrypt_algorithm, "Run-time encryption (dynamic)", very_safe_encryption_key );
	std::cout << (char*)safe_encrypted_data2.data() << std::endl;
	std::cout << std::endl;
	auto decrypted_data1 = test_decrypt_algorithm( safe_encrypted_data1, very_safe_encryption_key );
	std::cout << (char*)decrypted_data1.data() << std::endl;
	auto decrypted_data2 = test_decrypt_algorithm( safe_encrypted_data2, very_safe_encryption_key );
	std::cout << (char*)decrypted_data2.data() << std::endl;
	std::cout << std::endl << "-------" << std::endl;
	auto runtime_encrypted_data = test_decrypt_algorithm( "This string might be visible in plain text (might depends on compilers' options).", very_safe_encryption_key );
	std::cout << (char*)runtime_encrypted_data.data() << std::endl;
	auto runtime_decrypted_data = test_decrypt_algorithm( runtime_encrypted_data, very_safe_encryption_key );
	std::cout << (char*)runtime_decrypted_data.data() << std::endl;
#endif
	return 0;
}
