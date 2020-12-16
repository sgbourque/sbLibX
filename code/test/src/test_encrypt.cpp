#include "common/include/sb_common.h"
#include "common/include/sb_utilities.h"

#include <array>
#include <compare>

#include <string>
#include <iostream>

#ifdef __clang__
SBCOMPILE_MESSAGE( "non-type template parameter is not yet supported by clang" )
#else
//	template< typename _CHAR_TYPE_, size_t _LENGHT_ >
//struct encoded_string
//{
////	constexpr encoded_string() : data() {}
////
////	std::basic_string<size_t> data;
//	std::basic_string<char> decrypt() const { return data.data(); }
//	std::array<_CHAR_TYPE_, _LENGHT_> data;
//};


//	template<int N>
//constexpr auto test_encrypt( const char(&str)[N] )
//{
//	encoded_string<char, sizeof...(_UNENCRYPTED_STRING_)> test{ std::forward<char>(_UNENCRYPTED_STRING_)... };
//	return test;//xhash_string_view_t{ typename xhash_string_view_t::value_t( string, length ) };
//}

//	template< typename _CHAR_TYPE_, _CHAR_TYPE_... _UNENCRYPTED_STRING_> 
//constexpr std::string operator "" _xencode()
//{
//	return std::string{};
//}
//	template< char... _UNENCRYPTED_STRING_ >
//constexpr auto operator "" _xencode()
//{
//	encoded_string<char, sizeof...(_UNENCRYPTED_STRING_)> test{ std::forward<char>(_UNENCRYPTED_STRING_)... };
//	return test;//xhash_string_view_t{ typename xhash_string_view_t::value_t( string, length ) };
//}
//
//	template< typename _CHAR_TYPE_, size_t _LENGHT_ >
//inline std::ostream& operator << ( std::ostream& os, encoded_string<_CHAR_TYPE_, _LENGHT_> encoded )
//{
//	return os << encoded.decrypt();
//}

//static inline constexpr std::array<size_t, 32> encrypt( [[maybe_unused]] const char* unencrypted, [[maybe_unused]] size_t lenght )
//{
//	return {};//encrypt(unencrypted, lenght);
//}
//
//
//static inline constexpr auto operator ""_x64 ( const char* unencrypted, [[maybe_unused]] size_t lenght )
//{
//	return encrypt(unencrypted, lenght);
//}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif

//	template< typename _T, _T... _UNENCRYPTED_STRING_ >
//constexpr auto operator "" _x64()
//{
//	return std::basic_string<_T>{_UNENCRYPTED_STRING_...};
//}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

	template<typename _CHAR_TYPE_/*, size_t _DEFAULT_UNENCRYPTED_LENGTH_*/, typename _ENCODE_BLOCK_TYPE_, _ENCODE_BLOCK_TYPE_ _PSEUDO_PRIME_, _ENCODE_BLOCK_TYPE_ _COPRIME_>
struct encryption_traits
{
	static_assert( _PSEUDO_PRIME_ > 0xFFFFFFFFull &&  _COPRIME_ > 0xFFFFFFFFull, "Please use large values" );
	using encode_t         = _ENCODE_BLOCK_TYPE_;
	static_assert( std::is_integral_v<encode_t> && std::is_unsigned_v<encode_t>, "Encode type should be an unsigned integer type" );

	using char_t           = _CHAR_TYPE_;
	using char_ptr_t       = char_t*;
	using const_char_ptr_t = const char_t*;

	enum : size_t
	{
		//unencrypted_length = _DEFAULT_UNENCRYPTED_LENGTH_,
		char_size          = sizeof(char_t),
		encode_size        = sizeof(encode_t),
		encode_block       = encode_size / char_size,

		//encoded_data_size  = sbLibX::align_up( unencrypted_length * char_size, encode_size ),
		//encode_lenght      = encoded_data_size / encode_size,

		char_mask          = encode_t{~0ull} >> ( ( encode_block - 1 ) * char_size * CHAR_BIT ),
	};
	static_assert( encode_block * char_size == encode_size );
	//using decrypted_string_t = std::array<encode_t, decode_lenght>;
	//using decrypted_string_t = std::basic_string<char_t>;
	//using encrypted_string_t = std::array<encode_t, encode_lenght>;

	enum : encode_t
	{
		prime           = _PSEUDO_PRIME_, // should be prime modulo 2^64, meaning sbLibX::modular_inverse won't panic
		coprime         = _COPRIME_, // should be prime modulo 2^64, meaning sbLibX::modular_inverse won't panic

		inverse_prime   = sbLibX::modular_inverse( prime ),
		inverse_coprime = sbLibX::modular_inverse( coprime ),
		jitter_constant = inverse_coprime * ( prime + 1 ) / 2,

		invalid_block   = 0,
		initial_block   = jitter_constant,
	};
	static_assert( sbLibX::gcd<encode_t>( prime, coprime ) == 1 && sbLibX::gcd<encode_t>( prime, inverse_coprime ), "Please use coprime numbers" );

	struct encryption_state
	{
		encode_t value = invalid_block;
		encode_t scrambled = coprime;
	};
	static inline constexpr auto encrypt( encode_t unencrypted, encryption_state state = encryption_state{} )
	{
		encode_t pre_scrambled = ( state.scrambled ^ unencrypted );
		return encryption_state{ ( pre_scrambled ^ coprime ) * prime, ( pre_scrambled * coprime ) };
	}
	static_assert( encrypt( invalid_block ).value == invalid_block, "Empty encode block should be invalid" );
	static inline constexpr auto decrypt( encode_t encrypted, encryption_state state = encryption_state{} )
	{
		encode_t pre_scrambled = ( encrypted * inverse_prime ) ^ coprime;
		return encryption_state{ ( state.scrambled ^ pre_scrambled ), ( pre_scrambled * coprime ) };
	}

	static inline constexpr size_t jitter_shift( size_t index )
	{
		const size_t jitter = ( (coprime + 1)/2 * (prime + 1)/2 + prime * index + ( index ) ^ ( coprime * index) ) % encode_block;
		const size_t shift = ( encode_block - jitter - 1 ) * sizeof( char_t ) * CHAR_BIT;
		return shift;
	}

		template< size_t... _INDICES_, typename... __CHAR_TYPE__ >
	static inline constexpr auto pack( [[maybe_unused]] std::index_sequence<_INDICES_...>&&, __CHAR_TYPE__... unencrypted )
	{
		static_assert( sizeof...(__CHAR_TYPE__) <= encode_block );
		const char_t unpacked[encode_block] = { static_cast<char_t>( std::forward<__CHAR_TYPE__>( unencrypted ) & char_mask )... };
		encode_t packed{};
		for( size_t index = 0; index < encode_block; ++index )
		{
			encode_t next_char = static_cast<encode_t>( unpacked[index] ) & char_mask;
			const size_t shift = jitter_shift( index );
			packed |= ( next_char << shift );
		}
		return packed;
	}
	static_assert( pack( std::make_index_sequence<256>{}, char_mask, char_mask, char_mask, char_mask, char_mask, char_mask, char_mask, char_mask ) == ~0ull );
		template< size_t... _INDICES_ >
	static inline constexpr auto unpack( encode_t packed, [[maybe_unused]] std::index_sequence<_INDICES_...>&& )
	{
		std::array<char_t, encode_block> unpacked{};
		for( size_t index = 0; index < encode_block; ++index )
		{
			const size_t shift = jitter_shift( index );
			unpacked[index] = static_cast<char_t>( ( packed >> shift ) & char_mask );
		}
		return unpacked;
	}
};

#if 0
	template< typename _ENCODING_TRAITS_ >
struct encoding_helper
{
	//	template< encode_t op( encode_t ), typename _SOURCE_TYPE_ >
	//static inline constexpr auto transform_block( encode_t initialValue, [[maybe_unused]] const _SOURCE_TYPE_* data, [[maybe_unused]] std::index_sequence<>&& )
	//{
	//	return op( initialValue );
	//}
	//	template< encode_t op( encode_t ), typename _SOURCE_TYPE_, size_t... _INDICES_ >
	//static inline constexpr auto transform_block( encode_t initialValue, [[maybe_unused]] const _SOURCE_TYPE_* data, [[maybe_unused]] std::index_sequence<_INDICES_...>&& )
	//	-> std::enable_if_t<(sizeof...(_INDICES_) > 0), encode_t>
	//{
	//	static_assert( sizeof...(_INDICES_) <= encode_block );
	//	constexpr size_t currentIndex = sizeof...(_INDICES_) - 1;
	//	constexpr size_t currentShift = std::conditional_t<sizeof(_SOURCE_TYPE_) < sizeof(encode_t),
	//		std::integral_constant< size_t, currentIndex * sizeof(char_t) * CHAR_BIT >,
	//		std::integral_constant< size_t, 0 > >::value;
	//	const encode_t currentValue = static_cast<encode_t>( data[currentIndex] );
	//	const encode_t currentMask = ( currentValue << currentShift );
	//	return transform_block<op>( initialValue ^ currentMask, data, std::make_index_sequence<currentIndex>{} );
	//}

	// Encryption
		template< typename... _BLOCKS_ >
	static inline constexpr auto encrypt( [[maybe_unused]] const char_t* unencrypted, [[maybe_unused]] std::index_sequence<>&&, [[maybe_unused]] _BLOCKS_... blocks )
	{
		return encrypted_string_t{ std::forward<_BLOCKS_>(blocks)... };
	}
		template< size_t... _INDICES_, typename... _BLOCKS_ >
	static inline constexpr auto encrypt( [[maybe_unused]] const char_t* unencrypted, [[maybe_unused]] std::index_sequence<_INDICES_...>&&, encode_t initialValue, [[maybe_unused]] _BLOCKS_... blocks )
		-> std::enable_if_t<(sizeof...(_INDICES_) > 0), encrypted_string_t>
	{
		constexpr size_t unencrypted_size = sizeof...(_INDICES_);
		constexpr size_t block_count      = sbLibX::align_up<size_t, size_t>( unencrypted_size, encode_block ) / encode_block;
		constexpr size_t last_block_index = (block_count - 1) * encode_block;
		constexpr size_t last_block_size  = unencrypted_size - last_block_index;
		constexpr size_t residue_size     = unencrypted_size > encode_block ? unencrypted_size - encode_block : 0;
		const char_t* last_block = unencrypted + last_block_index;
		const encode_t nextValue = encrypt_block( initialValue, last_block, std::make_index_sequence<last_block_size>{} );
		return encrypt( unencrypted, std::make_index_sequence<residue_size>{}, nextValue, initialValue, std::forward<_BLOCKS_>(blocks)... );
	}
		template< size_t... _INDICES_ >
	static inline constexpr auto encrypt( [[maybe_unused]] const char_t* unencrypted, [[maybe_unused]] std::index_sequence<_INDICES_...>&& )
	{
		constexpr size_t unencrypted_size = sizeof...(_INDICES_);
		constexpr size_t block_count      = sbLibX::align_up<size_t, size_t>( unencrypted_size, encode_block ) / encode_block;
		constexpr size_t last_block_index = (block_count - 1) * encode_block;
		constexpr size_t last_block_size  = unencrypted_size - last_block_index;
		constexpr size_t residue_size     = unencrypted_size > encode_block ? unencrypted_size - encode_block : 0;
		const char_t* last_block = unencrypted + last_block_index;
		const encode_t initialValue = encrypt_block( initial_block, last_block, std::make_index_sequence<last_block_size>{} );
		static_assert( unencrypted_size <= unencrypted_length, "Not enough capacity to store string. Use a larger encryption traits" );
		return encrypt( unencrypted, std::make_index_sequence<residue_size>{}, initialValue );
	}
		template< size_t _UNENCODED_LENGHT_ >
	static inline constexpr auto encrypt( [[maybe_unused]] const char_t( &unencrypted )[_UNENCODED_LENGHT_] )
	{
		return encrypt( unencrypted, std::make_index_sequence<_UNENCODED_LENGHT_>{} );
	}
};

	template< class _ENCODING_TRAITS_ >
struct decoding_helper
{
	// Decryption
		template< typename... _BLOCKS_ >
	static inline constexpr auto decrypt( [[maybe_unused]] const encode_t* encrypted, [[maybe_unused]] std::index_sequence<>&&, [[maybe_unused]] _BLOCKS_... blocks )
	{
		constexpr encode_t char_mask = sizeof( char_t ) < sizeof( encode_t ) ? ( encode_t{1} << ( sizeof( char_t ) * CHAR_BIT ) ) - 1 : ~encode_t{0};
		std::array<encode_t, sizeof...(_BLOCKS_)> decrypted_packed{ std::forward<_BLOCKS_>( blocks )... };
		decrypted_string_t decrypted{};
		decrypted.resize( sizeof...(_BLOCKS_) * encode_block );
		for( size_t index = 0; index < sizeof...(_BLOCKS_); ++index )
			for( size_t subindex = 0; subindex < encode_block; ++subindex )
				decrypted[index] = static_cast<char_t>( ( decrypted_packed[index] >> (subindex * sizeof(char_t) * CHAR_BIT) ) & char_mask );
		return decrypted;
	}
		template< size_t... _INDICES_, typename... _BLOCKS_ >
	static inline constexpr auto decrypt( [[maybe_unused]] const encode_t* encrypted, [[maybe_unused]] std::index_sequence<_INDICES_...>&&, [[maybe_unused]] encode_t initialValue, [[maybe_unused]] _BLOCKS_... blocks )
		-> std::enable_if_t<(sizeof...(_INDICES_) > 0), decrypted_string_t>
	{
		constexpr size_t encrypted_size = sizeof...( _INDICES_ );
		const encode_t* last_block = encrypted + encrypted_size - 1;
		const encode_t nextValue = decrypt_block( initialValue, last_block, std::make_index_sequence<std::min<size_t>(encrypted_size + 1, 2) - 1>{} );
		return decrypt( encrypted, std::make_index_sequence<encrypted_size - 1>{}, nextValue, initialValue, std::forward<_BLOCKS_>(blocks)... );
	}

		template< size_t... _INDICES_ >
	static inline constexpr auto decrypt( [[maybe_unused]] const encode_t* encrypted, [[maybe_unused]] std::index_sequence<_INDICES_...>&& )
	{
		constexpr size_t encrypted_size = sizeof...(_INDICES_);
		const encode_t* last_block = encrypted + encrypted_size - 1;
		const encode_t initialValue = decrypt_block( invalid_block, last_block, std::make_index_sequence<std::min<size_t>(encrypted_size + 1, 2) - 1>{} );
		return decrypt( encrypted, std::make_index_sequence<encrypted_size - 1>{}, initialValue );
	}
		template< size_t _ENCODED_LENGHT_ >
	static inline constexpr auto decrypt( [[maybe_unused]] const std::array<encode_t, _ENCODED_LENGHT_>& encrypted )
	{
		return decrypt( encrypted.data(), std::make_index_sequence<_ENCODED_LENGHT_>{} );
	}
};
#endif
//using char_t = char8_t; // default to UTF-8
using default_encode_t = uint64_t;

#ifndef SB_ENCODE_TYPE
using SB_ENCODE_TYPE = default_encode_t;
#endif
#ifndef SB_ENCRYPT_BLOCK_COUNT
static inline constexpr size_t SB_ENCRYPT_BLOCK_COUNT = sbLibX::align_up( 32 * sizeof( uint8_t ), sizeof( SB_ENCODE_TYPE ) ) / sizeof( SB_ENCODE_TYPE );
#endif
#ifndef SB_PRIME_0
const SB_ENCODE_TYPE SB_PRIME_0 = SB_ENCODE_TYPE{ 0x9EF3455AD47C9E31ull };
#endif
#ifndef SB_PRIME_1
const SB_ENCODE_TYPE SB_PRIME_1 = SB_ENCODE_TYPE{ 0x03519CFFA7F0F405ull };
#endif
#ifndef SB_PRIME_2
const SB_ENCODE_TYPE SB_PRIME_2 = SB_ENCODE_TYPE{ 281474976710677ull };
#endif
#ifndef SB_PRIME_3
const SB_ENCODE_TYPE SB_PRIME_3 = SB_ENCODE_TYPE{ 17592186044423ull };
#endif
#ifndef SB_PRIME_4
const SB_ENCODE_TYPE SB_PRIME_4 = SB_ENCODE_TYPE{ 17179869209ull };
#endif
// TODO: some unit tests

	template<typename _CHAR_TYPE_/*, size_t _LENGHT_= SB_ENCRYPT_BLOCK_COUNT*/, typename _ENCODE_TYPE_ = SB_ENCODE_TYPE >
using encryption_traits_base_t = encryption_traits<_CHAR_TYPE_/*, _LENGHT_*/, _ENCODE_TYPE_, SB_PRIME_0, SB_PRIME_1>;


	template< typename _CHAR_TYPE_, size_t _LENGHT_, typename _ENCODE_BLOCK_TYPE_ = SB_ENCODE_TYPE,
			template<typename /*_CHAR_TYPE_*/, size_t /*_DEFAULT_UNENCRYPTED_LENGTH_*/, typename /*_ENCODE_BLOCK_TYPE_*/, _ENCODE_BLOCK_TYPE_ /*_PRIME_*/, _ENCODE_BLOCK_TYPE_ /*_COPRIME_*/>
		typename _ENCRYPT_TRAITS_ = encryption_traits
	>
static inline constexpr auto encrypt( [[maybe_unused]] const _CHAR_TYPE_( &unencrypted )[_LENGHT_] )
{
	using encrypt_traits = _ENCRYPT_TRAITS_<_CHAR_TYPE_/*, _LENGHT_*/, _ENCODE_BLOCK_TYPE_, SB_PRIME_0, SB_PRIME_1>;
	const typename encrypt_traits::encrypted_string_t encrypted = encrypt_traits::encrypt( unencrypted );
	return encrypted;
}

	template< size_t _LENGHT_, typename _ENCRYPT_TRAITS_ = encryption_traits_base_t<char> >
static inline constexpr auto decrypt( [[maybe_unused]] const std::array<typename _ENCRYPT_TRAITS_::encode_t, _LENGHT_> encrypted )
{
	using encrypt_traits = _ENCRYPT_TRAITS_;// <_CHAR_TYPE_, _LENGHT_, _ENCODE_BLOCK_TYPE_, SB_PRIME_0, SB_PRIME_1>;
	const typename encrypt_traits::decrypted_string_t decrypted = encrypt_traits::decrypt( encrypted );
	return decrypted;
}

#ifdef __clang__
SBCOMPILE_MESSAGE( "sbLibX: Non-type template not yet supported in clang (error follows)" )
#endif
	template< auto _DATA_ >
struct static_data
{
	static_data() : data( _DATA_ ) {}

	using data_t = std::remove_all_extents_t<decltype( _DATA_ )>;
	data_t data;
};
SBCOMPILE_MESSAGE( "TODO: use a template constexpr for a use like" );
SBCOMPILE_MESSAGE( "template< auto _DATA_ > constexpr std::remove_all_extents_t<decltype( _DATA_ )> encrypted_string = static_data<_DATA_>().data;" );
SBCOMPILE_MESSAGE( "c.f., std::is_integral_v" );



#include <iostream>
#include <common/include/sb_common.h>
SB_EXPORT_TYPE int SB_STDCALL test_encrypt([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
#if 0
	auto static_encrypt = static_data<encrypt( "--- Compile-time encryption testing ---" )>().data;
	for( auto x : encrypted )
		std::cout << x << " ";
	auto decrypted = decrypt( encrypted );
	std::cout << " -> " << decrypted;

	auto empty_encrypt = encrypt("");7
	auto empty_decrypt = decrypt( empty_encrypt );
#endif

	using traits_t = encryption_traits_base_t<char>;

	auto packed0 = traits_t::pack( std::make_index_sequence<1>{}, '0', '1', '2', '3', '4', '5', '6', '7' );
	auto packed1 = traits_t::pack( std::make_index_sequence<1>{}, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' );
	//auto packed0 = traits_t::pack( std::make_index_sequence<1>{}, 's', 'c', 'r', 'a', 'm', 'b', 'l', 'e' );
	//auto packed1 = traits_t::pack( std::make_index_sequence<0>{}, ' ', 't', 'h', 'i', 's', '!', '?', '\0' );
	for( size_t index = 0; index < traits_t::encode_block; ++index )
		std::cout << static_cast<traits_t::char_t>( packed0 >> (index * ( sizeof(traits_t::char_t) * CHAR_BIT )) & traits_t::char_mask );
	for( size_t index = 0; index < traits_t::encode_block; ++index )
		std::cout << static_cast<traits_t::char_t>( packed1 >> ( index * ( sizeof( traits_t::char_t ) * CHAR_BIT ) ) & traits_t::char_mask );
	std::cout << std::endl;

	auto encrypted0 = traits_t::encrypt( packed0 );
	for( size_t index = 0; index < traits_t::encode_block; ++index )
		std::cout << static_cast<traits_t::char_t>( encrypted0.value >> ( index * ( sizeof( traits_t::char_t ) * CHAR_BIT ) ) & traits_t::char_mask );
	auto encrypted1 = traits_t::encrypt( packed1, encrypted0 );
	for( size_t index = 0; index < traits_t::encode_block; ++index )
		std::cout << static_cast<traits_t::char_t>( encrypted1.value >> ( index * ( sizeof( traits_t::char_t ) * CHAR_BIT ) ) & traits_t::char_mask );
	std::cout << std::endl;

	auto decrypted0 = traits_t::decrypt( encrypted0.value );
	auto unpacked0 = traits_t::unpack( decrypted0.value, std::make_index_sequence<0>{} );
	for( size_t index = 0; index < unpacked0.size(); ++index )
		std::cout << unpacked0[index];

	auto decrypted1 = traits_t::decrypt( encrypted1.value, decrypted0 );
	auto unpacked1 = traits_t::unpack( decrypted1.value, std::make_index_sequence<0>{} );
	for( size_t index = 0; index < unpacked1.size(); ++index )
		std::cout << unpacked1[index];
	std::cout << std::endl;

	std::cout.flush();
	return 0;
}

#endif
