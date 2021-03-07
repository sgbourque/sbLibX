
#include "common/include/sb_utilities.h"

#if 1
#include <intrin.h>
#endif

#include <array>
#include <vector>

	template< size_t _ENCRYPT_BITS_ >
struct encrypt_key_t
{
	using type_t = uint64_t;
	static inline constexpr size_t encrypt_bits = _ENCRYPT_BITS_;
	static inline constexpr size_t split_count = ( encrypt_bits / (CHAR_BIT * sizeof(uint64_t)) );
	type_t value[split_count];
};

#if defined(SBDEBUG)
// Utility function to asserts that a value
	template< typename _TYPE_T >
static inline constexpr bool not_trivial_power( _TYPE_T value, size_t length, _TYPE_T base )
{
	using type_t = std::make_unsigned_t<_TYPE_T>;
	if( length == 0 )
		return true;
	//else if( base == 0 || ( base % 2 ) == 0 || ( value % base ) == 0 )
	//	return false;
	else if( length == 1 )
		return type_t( value ) * type_t( value ) > 1;
	else
		return type_t( value ) * type_t( value ) > 1 && not_trivial_power( type_t( base ) * type_t( value ), length - 1, type_t( base ) );
}
	template< typename type_t >
static inline constexpr bool not_trivial( type_t value, size_t length )
{
	return not_trivial_power( value, length - 1, value );
}
#endif
	template< typename _CHAR_T, size_t _LENGTH_, typename _KEY_T >
struct encrypted_data_traits
{
	using char_t = _CHAR_T;
	using uchar_t = std::make_unsigned_t<char_t>;
	using decrypted_t = std::array<char_t, _LENGTH_>;

	using key_t = _KEY_T;
	#ifndef SBPRIVATE_KEY_FILE
		#define SBPRIVATE_KEY_FILE "test/include/private_generated_512.key"
	#endif
	static inline constexpr key_t private_key = {
		#define KEY_PRIME(value) value ## ull,
		#include SBPRIVATE_KEY_FILE
		#define KEY_PRIME(value) sbLibX::modular_inverse(value ## ull),
		#include SBPRIVATE_KEY_FILE
	};
#if defined(SBDEBUG)
	#define KEY_PRIME(value) static_assert( not_trivial( value, _LENGTH_ > 0 ? _LENGTH_ : 64 ) );
	#include SBPRIVATE_KEY_FILE
#endif

	using type_t = typename key_t::type_t;
	static_assert( sizeof( type_t ) >= sizeof( char_t ) );
	static inline constexpr size_t packed_size = sizeof( type_t ) / sizeof( char_t );
	static inline constexpr size_t size() noexcept { return sbLibX::align_up( _LENGTH_, packed_size ) / packed_size; }
	using data_t = std::array< type_t, size() >;

	static inline constexpr size_t size( [[maybe_unused]] const data_t& data ) noexcept { return size(); }
	static inline constexpr size_t size( const decrypted_t& data ) noexcept { return _LENGTH_; }

	static inline constexpr void resize_encrypted_data( [[maybe_unused]] data_t& data, [[maybe_unused]] size_t length )
	{
	}
	static inline constexpr void resize_decrypted_data( [[maybe_unused]] decrypted_t& data, [[maybe_unused]] size_t encrypted_size )
	{
	}
};
	template< typename _CHAR_T, typename _KEY_T >
struct encrypted_data_traits<_CHAR_T, 0, _KEY_T>
{
	using char_t = _CHAR_T;
	using uchar_t = std::make_unsigned_t<char_t>;
	using decrypted_t = std::vector<char_t>;

	using key_t = _KEY_T;
	#ifndef SBPRIVATE_KEY_FILE_DYN
		#define SBPRIVATE_KEY_FILE_DYN "test/include/private_generated_512.key"
	#endif
	static inline constexpr key_t private_key = {
		#define KEY_PRIME(value) value ## ull,
		#include SBPRIVATE_KEY_FILE_DYN
		#define KEY_PRIME(value) sbLibX::modular_inverse(value ## ull),
		#include SBPRIVATE_KEY_FILE_DYN
	};
#if defined(SBDEBUG)
	#define KEY_PRIME(value) static_assert( not_trivial( value, 256 ) ); // can't push the compiler too far but that will garanties no cycle at least 256 steps long.
	#include SBPRIVATE_KEY_FILE_DYN
#endif

	using type_t = typename key_t::type_t;
	static_assert( sizeof( type_t ) >= sizeof( char_t ) );
	static inline constexpr size_t packed_size = sizeof( type_t ) / sizeof( char_t );
	using data_t = std::vector< type_t >;

	static inline constexpr size_t size( const data_t& data ) noexcept { return data.size(); }
	static inline constexpr size_t size( const decrypted_t& data ) noexcept { return packed_size * encrypted_size; }

	static inline constexpr void resize_encrypted_data( [[maybe_unused]] data_t& data, [[maybe_unused]] size_t length )
	{
		data.resize( sbLibX::align_up( length, packed_size ) / packed_size );
	}
	static inline constexpr void resize_decrypted_data( [[maybe_unused]] decrypted_t& data, [[maybe_unused]] size_t encrypted_size )
	{
		data.resize( packed_size * encrypted_size );
	}
};

	template< typename _CHAR_T, size_t _LENGTH_ = 0 >
struct encrypted_data
{
	using key_t = encrypt_key_t<512>;
	using traits_t = encrypted_data_traits< _CHAR_T, _LENGTH_, key_t >;
	using char_t = typename traits_t::char_t;
	using uchar_t = typename traits_t::uchar_t;
	using decrypted_t = typename traits_t::decrypted_t;
	using type_t = key_t::type_t;
	using data_t = typename traits_t::data_t;
	static inline constexpr key_t private_key = traits_t::private_key;

	static inline constexpr type_t private_encrypt_key = private_key.value[0] * private_key.value[1] * private_key.value[7];
	static inline constexpr type_t private_decrypt_key = private_key.value[4] * private_key.value[5] * private_key.value[3];
	static_assert( private_encrypt_key * private_decrypt_key == 1 );

	static inline constexpr type_t public_encrypt_key = private_key.value[1] * private_key.value[2] * private_key.value[4];
	static inline constexpr type_t public_decrypt_key = private_key.value[5] * private_key.value[6] * private_key.value[0];
	static_assert( public_encrypt_key * public_decrypt_key == 1 );


	static inline constexpr size_t packed_size = traits_t::packed_size;
	static inline constexpr size_t size( const decrypted_t& data ) noexcept { return traits_t::size( data ); }
	static inline constexpr size_t size( const data_t& data ) noexcept { return traits_t::size( data ); }
	type_t public_key;
	data_t data{};

	static inline constexpr void resize_encrypted_data( [[maybe_unused]] data_t& data, [[maybe_unused]] size_t length )
	{
		traits_t::resize_encrypted_data( data, length );
	}
	static inline constexpr void resize_decrypted_data( [[maybe_unused]] decrypted_t& data, [[maybe_unused]] size_t length )
	{
		traits_t::resize_decrypted_data( data, length );
	}

	constexpr encrypted_data( data_t&& _data, type_t _public_key ) noexcept : data( std::move( _data ) ), public_key( _public_key ) {}

	constexpr auto begin() const { return data.begin(); }
	constexpr auto end() const { return data.end(); }

	constexpr type_t operator []( size_t index ) const { return data[index]; }
};
//	template< typename _CHAR_T >
//struct encrypted_data<_CHAR_T, 0>
//{
//	using char_t = _CHAR_T;
//	using uchar_t = std::make_unsigned_t<char_t>;
//	using decrypted_t = std::vector<char_t>;
//
//	#ifndef SBPRIVATE_KEY_FILE
//		#define SBPRIVATE_KEY_FILE "test/include/private_generated_512.key"
//	#endif
//	using key_t = encrypt_key_t<512>;
//	static inline constexpr key_t private_key = {
//		#define KEY_PRIME(value) value ## ull,
//		#include SBPRIVATE_KEY_FILE
//		#define KEY_PRIME(value) sbLibX::modular_inverse(value ## ull),
//		#include SBPRIVATE_KEY_FILE
//	};
//
//	using type_t = key_t::type_t;
//	static inline constexpr bool not_trivial_power( type_t value, size_t length, type_t base )
//	{
//		if( length == 0 )
//			return true;
//		else if( length == 1 )
//			return std::make_unsigned_t<type_t>( value ) > 1;
//		else
//			return std::make_unsigned_t<type_t>( value ) > 1 && not_trivial_power( base * value, length - 1, base );
//	}
//	static inline constexpr bool not_trivial( type_t value, size_t length )
//	{
//		return not_trivial_power( value, length - 1, value );
//	}
//	#define KEY_PRIME(value) static_assert( not_trivial( value, 64 ) );
//	#include SBPRIVATE_KEY_FILE
//
//
//	static inline constexpr type_t private_encrypt_key = private_key.value[0] * private_key.value[1] * private_key.value[7];
//	static inline constexpr type_t private_decrypt_key = private_key.value[4] * private_key.value[5] * private_key.value[3];
//	static_assert( private_encrypt_key * private_decrypt_key == 1 );
//
//	static inline constexpr type_t public_encrypt_key = private_key.value[1] * private_key.value[2] * private_key.value[4];
//	static inline constexpr type_t public_decrypt_key = private_key.value[5] * private_key.value[6] * private_key.value[0];
//	static_assert( public_encrypt_key * public_decrypt_key == 1 );
//
//	static_assert( sizeof(type_t) >= sizeof(char_t) );
//	static inline constexpr size_t packed_size = sizeof(type_t) / sizeof(char_t);
//
//	inline size_t size() const noexcept { return data.size(); }
//	using data_t = std::vector< type_t >;
//	type_t public_key;
//	data_t data{};
//
//	static inline constexpr void resize_encrypted_data( [[maybe_unused]] data_t& data, [[maybe_unused]] size_t length )
//	{
//		data.resize( sbLibX::align_up( length, packed_size ) / packed_size );
//	}
//	static inline constexpr void resize_decrypted_data( [[maybe_unused]] decrypted_t& data, [[maybe_unused]] size_t encrypted_size )
//	{
//		data.resize( packed_size * encrypted_size );
//	}
//
//	constexpr encrypted_data( data_t&& _data, type_t _public_key ) noexcept : data( std::move( _data ) ), public_key( _public_key ) {}
//
//	constexpr auto begin() const { return data.begin(); }
//	constexpr auto end() const { return data.end(); }
//
//	constexpr type_t operator []( size_t index ) const { return data[index]; }
//};

	template< typename _TYPE_T, typename _EXP_T >
static inline constexpr auto power( _TYPE_T value, _EXP_T exponent, _TYPE_T characteristic = {} )
{
	// Note : 
	// This will not work if value is even as it does not have a modular inverse in given characteristic.
	// For this reason: return 0 instead, which is the limit point of any cycle.
	using type_t = std::make_unsigned_t<_TYPE_T>; type_t uvalue = value;
	using exp_t = std::make_unsigned_t<_EXP_T>;   exp_t  uexponent = exponent;
	if( uexponent == 0u )
		return type_t(1);
	else if( ( uexponent >> (CHAR_BIT * sizeof(_EXP_T) - 1) ) != 0 )
		return ( uvalue % (characteristic ? characteristic : 2) ) == 0 ? 0 : power<_TYPE_T, _EXP_T>( sbLibX::modular_inverse( uvalue, type_t(characteristic) ), ~uexponent + 1u );
	else
		return uvalue * power<_TYPE_T, _EXP_T>( value, uexponent - 1u );
}
static_assert( power( 2, -1 ) == 0 );
static_assert( power( 11, -5 ) * power( 11, 7 ) == 121 );

	template< typename _CHAR_T, size_t _LENGTH_ >
static inline constexpr auto encrypt_internal( const _CHAR_T* string, size_t length )
{
	using char_t = _CHAR_T;
	using uchar_t = std::make_unsigned_t<char_t>;
	using encrypt_data_t = encrypted_data<char_t, _LENGTH_>;
	using key_t = typename encrypt_data_t::key_t;
	using type_t = typename key_t::type_t;
	using data_t = encrypt_data_t::data_t;
	//SB_ENCODE_TYPE unique_key = hash<SB_ENCODE_TYPE, 0x9EF3455AD47C9E31ull, 0x03519CFFA7F0F405ull>( ""  __DATE__ __TIME__ );
	type_t public_key{ 1 };

	data_t result{};
	encrypt_data_t::resize_encrypted_data( result, length );
#if 1
	constexpr size_t packed_size = sizeof( type_t ) / sizeof( char_t );
	constexpr type_t private_encrypt_key = encrypt_data_t::private_encrypt_key;
	constexpr type_t public_encrypt_key =  encrypt_data_t::public_encrypt_key;
	constexpr type_t private_encrypt_key_power = power( private_encrypt_key, packed_size );
	type_t key_power = private_encrypt_key_power;
	for( size_t pack_index = 0; pack_index < result.size(); ++pack_index )
	{
		type_t packed_value = 0;
		for( size_t char_index = 0; char_index < packed_size; char_index++ )
		{
			const size_t next_char_offset = ( packed_size * result.size() - packed_size * pack_index - char_index - 1 );
			const type_t next_char = ( next_char_offset < length ) ? string[next_char_offset] : char_t(0);
			packed_value |= ( ( next_char & uchar_t(~0) ) << ( CHAR_BIT * sizeof(char_t) * char_index ) );
		}

		public_key = public_key * public_encrypt_key - private_encrypt_key;
		result[result.size() - pack_index - 1] = ( packed_value ^ ( public_key + private_encrypt_key - public_encrypt_key ) ) * key_power;
		key_power *= private_encrypt_key_power;
	}
	public_key = ( result[0] + public_key + private_encrypt_key ) ^ public_encrypt_key;
#else
	// unencrypted, packed
	for( size_t l = 0; l < _LENGTH_; ++l )
	{
		auto index = l / sizeof( type_t );
		result[index] <<= ( CHAR_BIT * sizeof( char_t ) );
		result[index] |= string[l] * public_key;
		public_key *= encrypted_data<_LENGTH_>::private_key.value[0];
	}
#endif
	return encrypted_data<char_t, _LENGTH_>( std::move( result ), public_key );
}
	template< typename _CHAR_T, size_t _LENGTH_ >
static inline auto decrypt_internal( const encrypted_data<_CHAR_T, _LENGTH_>& data )
{
	using char_t = _CHAR_T;
	using uchar_t = std::make_unsigned_t<char_t>;
	using encrypt_data_t = encrypted_data<char_t, _LENGTH_>;
	using key_t = typename encrypt_data_t::key_t;
	using type_t = typename key_t::type_t;
	using data_t = typename encrypt_data_t::data_t;
	using decrypted_t = typename encrypt_data_t::decrypted_t;

	constexpr size_t packed_size = sizeof( type_t ) / sizeof( char_t );
	const size_t data_size = encrypt_data_t::size( data.data );
	decrypted_t result{};
	encrypt_data_t::resize_decrypted_data( result, encrypt_data_t::size( data.data ) );
#if 1
	constexpr type_t private_encrypt_key = encrypt_data_t::private_encrypt_key;
	constexpr type_t private_decrypt_key = encrypt_data_t::private_decrypt_key;
	constexpr type_t public_encrypt_key = encrypt_data_t::public_encrypt_key;
	constexpr type_t public_decrypt_key = encrypt_data_t::public_decrypt_key;
	type_t public_key = ( data.public_key ^ public_encrypt_key ) - private_encrypt_key - data[0];

	constexpr type_t private_encrypt_key_power = power( private_encrypt_key, packed_size );
	type_t inverse_key_power = power( private_decrypt_key, ( data_size + 1 ) * packed_size );
	for( size_t pack_index = 0; pack_index < data_size; ++pack_index )
	{
		inverse_key_power *= private_encrypt_key_power;

		const type_t& encrypted_value = data[pack_index];
		const type_t packed_value = ( encrypted_value * inverse_key_power ) ^ ( public_key + private_encrypt_key - public_encrypt_key );

		public_key = ( public_key + private_encrypt_key ) * public_decrypt_key;

		for( size_t char_index = 0; char_index < packed_size; char_index++ )
		{
			const size_t offset = packed_size - char_index - 1;
			const char_t next_char = static_cast<char_t>( ( packed_value >> ( CHAR_BIT * sizeof(char_t) * offset ) ) & uchar_t(~0) );
			if( next_char )
				result[packed_size * pack_index + char_index] = next_char;
		}
	}
	// TODO : add validation
#else
	// unencrypted, packed
	for( const auto c : data )
	{
		for( size_t j = 0; j < sizeof( c ); ++j )
		{
			char_t next_unpacked =  static_cast<char_t>( ( c >> ( CHAR_BIT * sizeof(char_t) * ( sizeof( c )/sizeof(char_t) - j - 1 ) ) ) & 0xFF );
			result += next_unpacked;
		}
	}
#endif
	return result;
}


	template< typename _CHAR_T, size_t _LENGTH_ >
static inline constexpr auto encrypt( const _CHAR_T ( &string )[_LENGTH_] )
{
	return encrypt_internal<_CHAR_T, _LENGTH_>( string, _LENGTH_ );
}
	template< typename _CHAR_T >
static inline constexpr auto encrypt( const _CHAR_T* string, size_t length )
{
	return encrypt_internal<_CHAR_T, 0>( string, length );
}
	template< typename _CHAR_T, size_t _LENGTH_ >
static inline auto decrypt( const encrypted_data<_CHAR_T, _LENGTH_>& data )
{
	return decrypt_internal<_CHAR_T, _LENGTH_>( data );
}


	template< auto _DATA_ >
constexpr std::remove_all_extents_t<decltype( _DATA_ )> static_data = _DATA_;


#if 1 //def __clang__
SBCOMPILE_MESSAGE( "Warning: cannot encrypt at compile-time; non-type template parameter is not yet supported by clang" )
#define static_encrypt( ... ) encrypt( __VA_ARGS__ )
#else
#define static_encrypt( ... ) static_data< encrypt( __VA_ARGS__ ) >
#endif


// Inefficient but simple constexpr impl. which should be enough for normal compile-time use.
// Don't use that at runtime.
	template< typename type_t >
static inline constexpr type_t integer_log( type_t value, type_t base ) noexcept
{
	return value < base ? sbLibX::align_up( value, base ) / base - 1 : 1 + integer_log( value / base, base );
}
static_assert(
	   integer_log( 0ull, 10ull ) == ~0ull
	&& integer_log( 9ull, 10ull ) == 0ull
	&& integer_log( 99ull, 10ull ) == 1ull
	&& integer_log( 9999ull, 10ull ) == 3ull
);

#include <ostream>
	template< typename _STREAM_T, typename _CHAR_T, size_t _LENGTH_ >
static inline _STREAM_T& operator <<( _STREAM_T& out, const std::array<_CHAR_T, _LENGTH_>& data )
{
	for( auto c : data )
		out << c;
	return out;
}
	template< typename _STREAM_T, typename ..._Ty >
static inline _STREAM_T& operator <<( _STREAM_T& out, const std::vector<_Ty...>& data )
{
	for( auto c : data )
		out << c;
	return out;
}
	template< typename _STREAM_T, typename _CHAR_T, size_t _LENGTH_ >
static inline _STREAM_T& operator <<( _STREAM_T& out, const encrypted_data<_CHAR_T, _LENGTH_>& data )
{
	using char_t = _CHAR_T;
	using encrypted_data_t = encrypted_data<char_t, _LENGTH_>;
	using type_t = typename encrypted_data_t::type_t;
	constexpr char_t base = '~' - ' ' - 1;
	static_assert( base > 0 );

	constexpr size_t digit_count = integer_log<type_t>( type_t(-1), base ) + 1;
	for( type_t key = data.public_key, index = 0; index < digit_count; ++index )
	{
		type_t digit = ( key % base );
		key /= base;
		out << char_t( ' ' + 1 + digit );
	}
	for( auto packed : data )
	{
		for( ; packed > 0; packed /= base )
		{
			out << char_t( ' ' + 1 + ( packed % base ) );
		}
	}
	return out;
}


#include <iostream>
#include <common/include/sb_common.h>
SB_EXPORT_TYPE int SB_STDCALL test_encrypt2( [[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[] )
{
	const auto data = static_encrypt( "Test123" );
	std::cerr << "Encrypted: "
		<< data << std::endl;
	std::cerr << "Decrypted: "
		<< decrypt( data ) << std::endl;

	std::string input{};
	std::cout << "enter a word: ";
	std::cin >> input;
	auto test = encrypt( input.c_str(), input.size() + 1 );
	std::cerr << "Encrypted: "
		<< test << std::endl;
	std::cerr << "Decrypted: "
		<< decrypt( test ) << std::endl;
	return 0;
}
