#pragma once
#include <common/include/sb_common.h>
#include <common/include/sb_encrypted_string.h> // sb_hash and sb_encrypted_string are to be used together
#include <common/include/sb_utilities.h>

#include <cstdint>
#include <algorithm>
#include <string_view>
#include <iosfwd>

namespace SB { namespace LibX
{
////
// Some N-bits modular arithmetic used for encryption
	template< int a_r_offset = 0, typename type_t >
static inline constexpr auto extended_gcd_modular_step( type_t a_r, type_t a_t, type_t b_r, type_t b_t )
{
	const type_t quotient = a_r / b_r;
	type_t next_r = a_r + a_r_offset - quotient * b_r;
	type_t next_t = a_t - quotient * b_t;
	return std::make_tuple( b_r, b_t, next_r, next_t );
};
	template< typename type_t >
static inline constexpr auto modular_inverse( type_t prime64 )
{
	using modular_type_t = std::make_unsigned_t<type_t>;
	constexpr modular_type_t max_value = std::numeric_limits<modular_type_t>::max();
	constexpr modular_type_t offset_value = modular_type_t{} - max_value;

	struct type_pair { modular_type_t remainder; modular_type_t bezout; };
	type_pair first  = { max_value, 0 };
	type_pair second = { static_cast<modular_type_t>(prime64),   1 };
	{
		const auto iteration = extended_gcd_modular_step<offset_value>( first.remainder, first.bezout, second.remainder, second.bezout );
		first  ={ std::get<0>( iteration ), std::get<1>( iteration ) };
		second ={ std::get<2>( iteration ), std::get<3>( iteration ) };
	}
	while( second.remainder != 0 )
	{
		const auto iteration = extended_gcd_modular_step<0>( first.remainder, first.bezout, second.remainder, second.bezout );
		first  ={ std::get<0>( iteration ), std::get<1>( iteration ) };
		second ={ std::get<2>( iteration ), std::get<3>( iteration ) };
	}
	return static_cast<type_t>( first.bezout );
}
////
	template<typename _CHAR_TYPE_, typename _HASH_T_, size_t _DEFAULT_LENGTH_ = 256, _HASH_T_ _PRIME_ = 0x9EF3455AD47C9E31ull, _HASH_T_ _COPRIME_ = 0x03519CFFA7F0F405ull>
struct xhash_traits
{
	using hash_t           = _HASH_T_;
	using char_t           = _CHAR_TYPE_;
	using char_ptr_t       = char_t*;
	using const_char_ptr_t = const char_t*;

	enum : hash_t
	{
		invalid_hash = 0,
		prime = _PRIME_,
		coprime = _COPRIME_,

		inverse_prime = modular_inverse( prime ),
		inverse_coprime = modular_inverse( coprime ),
	};
	static_assert( prime * inverse_prime == 1, "TODO: Calculate modular inverse instead of forcing people to precompute it..." );
	enum : size_t
	{
		default_length = _DEFAULT_LENGTH_,
		char_size      = sizeof(char_t),
		max_data_size  = default_length * char_size,

		prime_count    = 2,
	};

	static inline constexpr typename hash_t
	hash(const_char_ptr_t string, size_t length = default_length)
	{
		// TODO: get next unicode32 char from string instead of byte per byte
		// so that different encodings of the same string would give the same result!!
		return string && *string && length > 0 ? (hash(++string, --length) + *string * prime) ^ coprime : 0;
	}
};

////
	template<typename _TRAITS_TYPE_>
struct xhash_string_view
{
	using traits_t = _TRAITS_TYPE_;
	using key_t = typename traits_t::hash_t;
	using char_t = typename traits_t::char_t;
	using const_char_ptr_t = typename traits_t::const_char_ptr_t;
	using value_t  = encrypted_string_view<char_t, traits_t>;

	key_t    key;
	value_t  value;

	constexpr xhash_string_view() noexcept = default;
	constexpr xhash_string_view( const xhash_string_view& ) noexcept = default;

	constexpr xhash_string_view( value_t _value ) noexcept : key( traits_t::hash(_value.data(), _value.size()) ), value(_value) {}
	constexpr xhash_string_view( key_t _key, value_t _value ) noexcept : key(_key), value(_value) {}
	constexpr xhash_string_view( const_char_ptr_t _value ) noexcept : key(traits_t::hash(_value)), value(_value) {}

	//constexpr xhash_string_view( value_t _value ) noexcept : key(), value( _value )
	//{
	//	const auto decrypted = _value.decrypt();
	//	key = traits_t::hash( decrypted.data(), decrypted.size() );
	//}
	//constexpr xhash_string_view( const_char_ptr_t _value, size_t lenght ) noexcept : key( traits_t::hash( _value, lenght ) ), value( _value ) {}

	constexpr operator key_t() const { return key; }
	constexpr operator value_t() const { return value; }

	constexpr key_t get_key() const { return key; }
	constexpr value_t get_value() const { return value; }
};


////
	template<typename _TRAITS_TYPE_>
inline std::ostream& operator << (std::ostream& os, xhash_string_view< _TRAITS_TYPE_> hash_string_view)
{
	return os << "{0x" << std::hex << hash_string_view.get_key() << ":'" << hash_string_view.get_value().decrypt() << "(" << hash_string_view.get_value().encrypted() << ")" << "'}";
}



////
// Eventually, support for UTF-8/16/32
// with system-specific implementation for native <-> international (unicode) conversion.
// ... eventually... well, that's the plan...
using char_t = char; // lets focus on ascii -> UTF-8 first : system is ascii/native, user is UTF-8
using xhash_t = uint64_t;
using xhash_traits_t = xhash_traits<char_t, xhash_t>;
using xhash_string_view_t = xhash_string_view<xhash_traits_t>;

constexpr xhash_string_view_t operator "" _xhash64([[maybe_unused]] xhash_traits_t::const_char_ptr_t string, [[maybe_unused]] size_t length)
{
	return xhash_string_view_t{ typename xhash_string_view_t::value_t(string, length) };
}


#if 0
// for sb_math_base.h
// Greatest Common Multiple
	template<typename _TYPE_>
constexpr _TYPE_ gcd(_TYPE_ a, _TYPE_ b) // expects <,>, +,-, *,/,% are all defined
{
	using type_t = _TYPE_;
	type_t sign_value = std::conditional_t< (a < 0)
		, std::integral_constant<type_t, 1>
		, std::integral_constant<type_t, -1> >::value;
	if ( a < 0 )
		return sign_value * gcd(b, sign_value*a);
	else if ( b < a )
		return gcd(b, a);
	else if ( a == 0 )
		return b;
	else
		return gcd((b % a), a);
}
// Smallest Common Multiple
	template<typename _TYPE_>
constexpr _TYPE_ scm(_TYPE_ a, _TYPE_ b); // expects <,>, +,-, *,/,% are all defined


	template<typename _CHAR_TYPE_, typename _HASH_VALUE_T_, _HASH_VALUE_T_ _QUASI_PRIME_ = 0x9EF3455AD47C9E31ull, _HASH_VALUE_T_ _COPRIME_ = 0x03519CFFA7F0F405ull>
struct hash_traits
{
	using char_t = _CHAR_TYPE_;
	using value_t = _HASH_VALUE_T_;
	enum : value_t
	{
		invalid_hash = 0,
		quasi_prime = _QUASI_PRIME_,
		coprime     = _COPRIME_,
	};
	value_t value;

	static_assert(_QUASI_PRIME_ > (1ull << (sizeof(value_t) - 1) * CHAR_BIT) + CHAR_BIT / 4, "please use a larger number");
	static_assert(_COPRIME_ > (1ull << (sizeof(value_t) - 1) * CHAR_BIT) + CHAR_BIT / 4, "please use a larger number");
	//static_assert(gcd(quasi_prime, coprime) == 1, "should use coprimes numbers");

	// Better primal testing (we really only requires that we are free of really small factors).
	// Here, we only require that there are no divisor smaller than 256.
	//enum : uint64_t { small_prime_product_0 = 614889782588491410ull, small_prime_product_1 = 1816798556036292277ull, small_prime_product_2 = 1969173919250648897ull, small_prime_product_3 = 239868713978954299ull, small_prime_product_4 = 1352708312947727201ull, small_prime_product_5 = 6532397423431938467ull, small_prime_product_6 = 82562146564818163ull, };
	//static_assert(gcd(quasi_prime, small_prime_product_0) == 1, "quasi_prime must be free of small integers factor");
	//static_assert(gcd(quasi_prime, small_prime_product_1) == 1, "quasi_prime must be free of small integers factor");
	//static_assert(gcd(quasi_prime, small_prime_product_2) == 1, "quasi_prime must be free of small integers factor");
	//static_assert(gcd(quasi_prime, small_prime_product_3) == 1, "quasi_prime must be free of small integers factor");
	//static_assert(gcd(quasi_prime, small_prime_product_4) == 1, "quasi_prime must be free of small integers factor");
	//static_assert(gcd(quasi_prime, small_prime_product_5) == 1, "quasi_prime must be free of small integers factor");
	//static_assert(gcd(quasi_prime, small_prime_product_6) == 1, "quasi_prime must be free of small integers factor");
	//static_assert(gcd(coprime, small_prime_product_0) == 1, "coprime must be free of small integers factor");
	//static_assert(gcd(coprime, small_prime_product_1) == 1, "coprime must be free of small integers factor");
	//static_assert(gcd(coprime, small_prime_product_2) == 1, "coprime must be free of small integers factor");
	//static_assert(gcd(coprime, small_prime_product_3) == 1, "coprime must be free of small integers factor");
	//static_assert(gcd(coprime, small_prime_product_4) == 1, "coprime must be free of small integers factor");
	//static_assert(gcd(coprime, small_prime_product_5) == 1, "coprime must be free of small integers factor");
	//static_assert(gcd(coprime, small_prime_product_6) == 1, "coprime must be free of small integers factor");
};

#endif

}} // namespace SB::LibX
namespace sbLibX = SB::LibX;
