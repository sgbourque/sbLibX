#pragma once
#include <cstdint>
#include <algorithm>
#include <string_view>
#include "sb_utilities.h"

namespace SB { namespace LibX
{

	template< typename _TYPE_T >
static inline constexpr _TYPE_T integer_log2( _TYPE_T n ) { static_assert( std::is_integral_v<_TYPE_T> ); return n > 1 ? 1 + integer_log2( n >> 1 ) : n > 0 ? 0 : ~_TYPE_T{}; }
static_assert( integer_log2( ~0ull ) == 63 && integer_log2( 0 ) == -1 && integer_log2( 1 ) == 0 && integer_log2( 2 ) == 1 && integer_log2( 3 ) == 1 && integer_log2( 4 ) == 2 && integer_log2( 8 ) == 3 && integer_log2( 16 ) == 4 && integer_log2( 31 ) == 4 );

	
	template< typename _TYPE_T >
static inline constexpr _TYPE_T ceil_pow2( _TYPE_T n )
{
	if (_TYPE_T{ 2 } *n > n)
		return _TYPE_T( 1 ) << integer_log2( _TYPE_T{ 2 } *n - _TYPE_T{ 1 } );
	else
		return _TYPE_T{};
}
static_assert( ceil_pow2( ~0ull ) == 0 );
static_assert( ceil_pow2( -31 )   == 0 );
static_assert( ceil_pow2( -1 )    == 0 );
static_assert( ceil_pow2( 0 )     == 0 );
static_assert( ceil_pow2( 1 )     == 1 );
static_assert( ceil_pow2( 2 )     == 2 );
static_assert( ceil_pow2( 3 )     == 4 );
static_assert( ceil_pow2( 8 )     == 8 );
static_assert( ceil_pow2( 16 )    == 16 );
static_assert( ceil_pow2( 31 )    == 32 );

	template< typename _TYPE_T >
static inline constexpr bool is_power_of_2( _TYPE_T n ) { static_assert( std::is_integral_v<_TYPE_T> ); return n > 0 && ( n & ( n - 1) ) == 0; }


////
// Some N-bits modular arithmetic (mostly used for static encryption)
// All algorithms below expects type_t to support the following operators : <=>, +, -, *, /, %
// and that 1 != 0. Characteristic 0 means 2^N for N the bit width of the given type.
	template<typename type_t>
static inline constexpr type_t gcd( type_t a, type_t b )
{
	type_t sign_value = std::conditional_t< std::is_signed_v<type_t>
		, std::integral_constant<type_t, type_t(-1)>
		, std::integral_constant<type_t, type_t(1ull)>
	>::value;
	if ( a < 0 )
		return sign_value * gcd( b, sign_value * a );
	else if ( b < a )
		return gcd(b, a);
	else if ( a == 0 )
		return b;
	else
		return gcd((b % a), a);
}
	template< typename type_t >
static inline constexpr auto quotient_remainder( type_t num, type_t denom )
{
	const type_t quotient = denom ? num / denom : 0;
	const type_t remainder = denom ? num % denom : 0;
	return std::make_pair( quotient, remainder );
}
	template< typename type_t >
static inline constexpr auto extended_gcd_modular_step( type_t a_rem, type_t a_bez, type_t b_rem, type_t b_bez, type_t a_r_offset = 0 )
{
	const auto [quotient, remainder] = quotient_remainder( a_rem, b_rem );
	type_t next_rem = a_r_offset + remainder;
	type_t next_bez = a_bez - quotient * b_bez;
	return std::make_tuple( b_rem, b_bez, next_rem, next_bez );
};
	template< typename type_t >
static inline constexpr type_t modular_inverse( type_t value, type_t characteristic = type_t{} )
{
	// If characteristic != 0 && gcd( value, characteristic ) != 1,
	// the modular inverse correspond to a stabilizer of the zero-divisor
	// ring generated from powers of gcd( value, characteristic ).
	//
	// Ex., let p = (5*7*11) and q = (7*13*17) such that
	// gcd(p,q) = 7. With p_inv_q = modular_inverse(p,q), we can find that
	// 1322 = 1/(5*11) mod q so that 1322 * p = 7 mod q. However, we also
	// find that 1543 is a 7-ring stabilizer on (p,q) and satisfies
	//		1543 * p = 7 mod q
	// More generally in this case, for any number N, we have
	//		p_inv_q_N = ( 13 * 17 * N - 4 );
	// that satisfies
	//		p_inv_q_N * p = 7 mod q;
	// The simplest solution is thus ( characteristic - 4 ).
	//
	// At the same time, we find that
	//		p_inv_q_N * ( 5 * 11 ) = 1326 * ( N + 1) + 1;
	// and that 1326 = 2*3*13*17 satisfies
	//		1326 * 7 = 2*3*7*13*17 = 0 mod 7 * 13 * 17
	// so that 1326 generates a zero-divisor 7-ring mod q, which
	// is a sub-algebra of characteristic 7.
	//
	// Note that 1326 satifies precisely 1/(5*11) mod q = ( 1326 - 4 ),
	// which gives 1/(5*11) mod q = p_inv_q_{7-1}.
	using modular_type_t = std::make_unsigned_t<std::remove_const_t<type_t>>;
	const modular_type_t zero_value = characteristic != 0 ? static_cast<modular_type_t>(characteristic) : std::numeric_limits<modular_type_t>::max();
	const modular_type_t offset = characteristic - zero_value;

	struct type_pair { modular_type_t remainder; modular_type_t bezout; };
	type_pair first  = { zero_value, 0 };
	type_pair second = { static_cast<modular_type_t>( value ), 1 };
	{
		const auto [ r1, b1, r2, b2 ] = extended_gcd_modular_step( first.remainder, first.bezout, second.remainder, second.bezout, offset );
		first  = { r1, b1 };
		second = { r2, b2 };
	}
	while( second.remainder != 0 )
	{
		const auto [ r1, b1, r2, b2 ] = extended_gcd_modular_step( first.remainder, first.bezout, second.remainder, second.bezout );
		first  = { r1, b1 };
		second = { r2, b2 };
	}
	if ( characteristic > 0 && first.bezout > modular_type_t(characteristic) )
	{
		// first.bezout really is negative, so need to add characteristic
		first.bezout += characteristic;
	}
	return static_cast<type_t>( first.bezout );
}

// some small non-trivial unit tests
sb_static_assert( 1                            ==   modular_inverse( 385ull                                ) * 385ull,                              "modular_inverse: bad implementation"                                                     , modular_inverse( 385ull                                ) );
sb_static_assert( 17284                        ==   modular_inverse( 34567            , 34569              ) % 34569,                               "modular_inverse: bad implementation"                                                     , modular_inverse( 34567            , 34569              ) );
sb_static_assert( 34568                        ==   modular_inverse( 34568            , 34569              ) % 34569,                               "modular_inverse: compiler does not support signed modular operations on signed integers.", modular_inverse( 34568            , 34569              ) );
sb_static_assert(  1                           == ( modular_inverse( 34567            , 34569              ) * 34567) % 34569,                      "modular_inverse: did the two previous static_assert really worked?"                      , modular_inverse( 34567            , 34569              ) );
sb_static_assert(  2                           ==   modular_inverse( 11u                                   ) * 22u,                                 "modular_inverse: 64-bits modular_inverse is incorrect."                                  , modular_inverse( 11u                                   ) );
sb_static_assert(  2                           ==   modular_inverse( 22u                                   ) * 22u,                                 "modular_inverse: implementation for zero-divisor is incorrect."                          , modular_inverse( 22u                                   ) );
sb_static_assert(  4                           ==   modular_inverse( 4 * (0ull - 7ull)                     ) * 4 * (0ull - 7ull),                   "modular_inverse: implementation for unsigned zero-divisor is incorrect."                 , modular_inverse( 4 * (0ull - 7ull)                     ) );
sb_static_assert( ( 7u * 13u * 17u ) - 4       ==   modular_inverse( ( 5u * 7u * 11u ), ( 7u * 13u * 17u ) ),                                       "modular_inverse: non-zero characteristic signed support is incorrect."                   , modular_inverse( ( 5u * 7u * 11u ), ( 7u * 13u * 17u ) ) );
sb_static_assert( ( (7u - 1) * 13u * 17u ) - 4 == ( modular_inverse( ( 5u      * 11u ), ( 7u * 13u * 17u ) ) ),                                     "modular_inverse: incorrect non-field calculation"                                        , modular_inverse( ( 5u      * 11u ), ( 7u * 13u * 17u ) ) );
sb_static_assert( 1                            == ( modular_inverse( ( 5u      * 11u ), ( 7u * 13u * 17u ) ) * ( 5u * 11u ) ) % ( 7u * 13u * 17u ), "modular_inverse: incorrect non-field support"                                            , modular_inverse( ( 5u      * 11u ), ( 7u * 13u * 17u ) ) );

// Least Common Multiple
	template<typename _TYPE_>
constexpr _TYPE_ lcm(_TYPE_ a, _TYPE_ b)
{
	return ( a / gcd(a, b) ) * b;
}



//////////////////////////////////////////////////////////////////////////
// half_bits_mask returns all first (log_shift + 1) bits sets :
//	0 -> 0x1
//	1 -> 0x3
//	2 -> 0xF
//	3 -> 0xFF
//  4 -> 0xFFFF
//  ...
template<class _TYPE_T> static inline constexpr std::enable_if_t<std::is_unsigned_v<_TYPE_T>, _TYPE_T> half_bits_mask( _TYPE_T log_shift ) noexcept { return (_TYPE_T(1) << (_TYPE_T(1) << log_shift)) - _TYPE_T(1); }
//////////////////////////////////////////////////////////////////////////
// parallel_bits_mask returns
// 0 -> 0x5...        = binary 01...
// 1 -> 0x33...       = binary 0011...
// 2 -> 0x0F...       = binary 00001111...
// 3 -> 0x00FF...
// 4 -> 0x0000FFFF...
// 5 -> 0x00000000FFFFFFFF...
// andconverges to _TYPE_T(-1) at log_shift >= log2( sizeof(_TYPE_T) * CHAR_BIT )
template<class _TYPE_T> static inline constexpr _TYPE_T parallel_bits_mask( _TYPE_T log_shift ) noexcept
{
	static_assert( std::is_unsigned_v<_TYPE_T>, "bit mask must be unsigned" );
	using type_t = _TYPE_T;
	constexpr type_t all_bits = type_mask<type_t>();
	constexpr size_t bit_count = sizeof(type_t) * CHAR_BIT;
	const type_t next_hi_bit = type_t(type_t(1) << (log_shift + 1));
	const type_t parallel_mask = next_hi_bit < bit_count ? type_t( all_bits / half_bits_mask(log_shift + 1) ) : type_t(1);
	return half_bits_mask(log_shift) * parallel_mask;
}
	template<class _TYPE_T>
static inline constexpr std::enable_if_t<std::is_unsigned_v<_TYPE_T>, _TYPE_T> parallel_bit_count(_TYPE_T mask, _TYPE_T log_shift) noexcept
{
	const auto parallel_mask = parallel_bits_mask(log_shift);
	const auto low_mask = ((mask >> (_TYPE_T(0) << log_shift)) & parallel_mask);
	const auto hi_mask = ((mask >> (_TYPE_T(1) << log_shift)) & parallel_mask);
	return hi_mask + low_mask;
}

// bit_count is O(log(N))
	template<class _TYPE_T>
static inline constexpr auto bit_count( _TYPE_T mask ) noexcept -> std::make_unsigned_t<_TYPE_T>
{
	using type_t = std::make_unsigned_t<_TYPE_T>;
	type_t bit_mask = static_cast<type_t>( mask );
	constexpr type_t max_log_shift = integer_log2<type_t>( sizeof(type_t) * CHAR_BIT );
	for ( type_t log_shift = 0; log_shift < max_log_shift; ++log_shift )
		bit_mask = parallel_bit_count( bit_mask, log_shift );
	return bit_mask;
}

static_assert( bit_count( 0 ) == 0 );
static_assert( bit_count( 1 ) == 1 );
static_assert( bit_count( 5 ) == 2 );
static_assert( bit_count( 0xEEEEEEEEEEEEEEEEull ) == 16 * 3 );

}} // namespace SB::LibX
namespace sbLibX = SB::LibX;
