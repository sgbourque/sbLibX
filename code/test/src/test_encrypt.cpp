#include "common/include/sb_common.h"
#include "common/include/sb_utilities.h"

#include <array>
#include <compare>

#include <string>
#include <iostream>



#ifdef __clang__
#define clang_pragma( ... ) __pragma( clang __VA_ARGS__ )
#define SB_CLANG_MESSAGE( ... ) __pragma( message( __VA_ARGS__ ) )
#else
#define clang_pragma( ... )
#define SB_CLANG_MESSAGE( ... )
#endif
SB_CLANG_MESSAGE( "non-type template parameter is not yet supported by clang" )


//clang_pragma( diagnostic push )
//clang_pragma( diagnostic ignored "-Wgnu-string-literal-operator-template" )
//	template< typename _T, _T... _UNENCRYPTED_STRING_ >
//constexpr auto operator "" _x64()
//{
//	return std::basic_string<_T>{_UNENCRYPTED_STRING_...};
//}
//clang_pragma( diagnostic pop )


//using char_t = char8_t; // default to UTF-8
using default_encode_t = uint64_t;
	template< typename encode_t, encode_t prime, encode_t coprime, typename char_t >
static inline constexpr encode_t hash( const char_t* string, size_t length )
{
	return string && *string && length > 0 ? ( hash<encode_t, prime, coprime>( string + 1, --length ) + std::make_unsigned_t<char_t>(*string) * prime ) ^ coprime : 0;
}
	template< typename encode_t, encode_t prime, encode_t coprime, typename char_t, size_t length >
static inline constexpr encode_t hash( const char_t( &string )[length] )
{
	return hash<encode_t, prime, coprime>( string, length );
}
//using sbLibX::operator "" _xhash64;


clang_pragma( diagnostic push )
clang_pragma( diagnostic ignored "-Wdate-time" )

#ifndef SB_ENCODE_TYPE
using SB_ENCODE_TYPE = default_encode_t;
#endif
#ifndef SB_ENCRYPT_BLOCK_COUNT
static inline constexpr size_t SB_ENCRYPT_BLOCK_COUNT = sbLibX::div_align_up( 32 * sizeof( uint8_t ), sizeof( SB_ENCODE_TYPE ) );
#endif
#ifndef SB_PRIME_0
static inline constexpr SB_ENCODE_TYPE sb_prime_0_factor = hash<SB_ENCODE_TYPE, 0x9EF3455AD47C9E31ull, 0x03519CFFA7F0F405ull>( ""  __DATE__ __TIME__ );
const SB_ENCODE_TYPE SB_PRIME_0 = SB_ENCODE_TYPE{ 0x9EF3455AD47C9E31ull * ( 2 * sb_prime_0_factor + 1 ) };
#endif
#ifndef SB_PRIME_1
static inline constexpr SB_ENCODE_TYPE sb_prime_1_factor = hash<SB_ENCODE_TYPE, 0x03519CFFA7F0F405ull, 281474976710677ull>( ""  __DATE__ __TIME__ );
const SB_ENCODE_TYPE SB_PRIME_1 = SB_ENCODE_TYPE{ 0x03519CFFA7F0F405ull * ( 2 * sb_prime_1_factor + 1 ) };
#endif
#ifndef SB_PRIME_2
static inline constexpr SB_ENCODE_TYPE sb_prime_2_factor = hash<SB_ENCODE_TYPE, 281474976710677ull, 17592186044423ull>( ""  __DATE__ __TIME__ );
const SB_ENCODE_TYPE SB_PRIME_2 = SB_ENCODE_TYPE{ 281474976710677ull * ( 2 * sb_prime_2_factor + 1 ) };
#endif
#ifndef SB_PRIME_3
static inline constexpr SB_ENCODE_TYPE sb_prime_3_factor = hash<SB_ENCODE_TYPE, 17592186044423ull, 17179869209ull>( ""  __DATE__ __TIME__ );
const SB_ENCODE_TYPE SB_PRIME_3 = SB_ENCODE_TYPE{ 17592186044423ull * ( 2 * sb_prime_3_factor + 1 ) };
#endif
#ifndef SB_PRIME_4
static inline constexpr SB_ENCODE_TYPE sb_prime_4_factor = hash<SB_ENCODE_TYPE, 17179869209ull, 0x9EF3455AD47C9E31ull>( ""  __DATE__ __TIME__ );
const SB_ENCODE_TYPE SB_PRIME_4 = SB_ENCODE_TYPE{ 17179869209ull * ( 2 * sb_prime_4_factor + 1 ) };
#endif
clang_pragma( diagnostic pop )


static inline constexpr SB_ENCODE_TYPE SB_PRIME_A = SB_PRIME_0;
static inline constexpr SB_ENCODE_TYPE SB_PRIME_B = sbLibX::gcd( SB_PRIME_0, SB_PRIME_1 ) == 1 ? SB_PRIME_1 : 
                                                    sbLibX::gcd( SB_PRIME_0, SB_PRIME_2 ) == 1 ? SB_PRIME_2 : 
                                                    sbLibX::gcd( SB_PRIME_0, SB_PRIME_3 ) == 1 ? SB_PRIME_3 : 
                                                    sbLibX::gcd( SB_PRIME_0, SB_PRIME_4 ) == 1 ? SB_PRIME_4 : 0;
static_assert( SB_PRIME_B != 0, "Bad luck generating coprime values, please try again... it is time dependent... I know, bad idea but I'm experimenting!" );



//SB_CLANG_MESSAGE( "sbLibX: Non-type template not yet supported in clang (error follows)" )
//	template< auto _DATA_ >
//struct static_data
//{
//	static_data() : data( _DATA_ ) {}
//
//	using data_t = std::remove_all_extents_t<decltype( _DATA_ )>;
//	data_t data;
//};
//SBCOMPILE_MESSAGE( "TODO: use a template constexpr for a use like" );
//SBCOMPILE_MESSAGE( "template< auto _DATA_ > constexpr std::remove_all_extents_t<decltype( _DATA_ )> encrypted_string = static_data<_DATA_>().data;" );
//SBCOMPILE_MESSAGE( "c.f., std::is_integral_v" );



#include <iostream>
#include <common/include/sb_common.h>
SB_EXPORT_TYPE int SB_STDCALL test_encrypt([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	return 0;
}
