#pragma once
#include <common/include/sb_common.h>

#include <cstdint>
#include <algorithm>
#include <string_view>
#include <iosfwd>

namespace SB { namespace LibX
{
////
	template<typename _CHAR_TYPE_, typename _HASH_VALUE_T_, size_t _DEFAULT_LENGTH_ = 256, _HASH_VALUE_T_ _QUASI_PRIME_ = 0x9EF3455AD47C9E31ull, _HASH_VALUE_T_ _COPRIME_ = 0x03519CFFA7F0F405ull>
struct xhash_traits
{
	using char_t    = _CHAR_TYPE_;
	using value_t   = _HASH_VALUE_T_;
	using pointer_t = char_t*;
	using const_pointer_t = const char_t*;

	enum : value_t
	{
		invalid_hash = 0,
		quasi_prime = _QUASI_PRIME_,
		coprime = _COPRIME_,
	};
	enum : size_t
	{
		default_length = _DEFAULT_LENGTH_,
		char_size      = sizeof(char_t),
		max_data_size  = default_length * char_size,

		prime_count    = 2,
	};

	static inline constexpr typename value_t
	hash(const_pointer_t string, size_t length = default_length)
	{
		// TODO:get next unicode char from string instead of byte per byte
		// so that different encodings of the same string will give the same result.
		return string && *string ? (hash(++string, --length) + *string * quasi_prime) ^ coprime : 0;
	}
};


////
	template<typename _TRAITS_TYPE_>
struct xhash_string_view
{
	using traits_t = _TRAITS_TYPE_;
	using hash_t = typename traits_t::value_t;
	using char_t = typename traits_t::char_t;
	using key_t  = std::basic_string_view<char_t>;

	hash_t hash;
	key_t  name;

	constexpr xhash_string_view() noexcept = default;
	constexpr xhash_string_view( const xhash_string_view& ) noexcept = default;

	constexpr xhash_string_view(key_t _name) noexcept : hash( traits_t::hash(_name.data(), _name.size()) ), name(_name) {}
	constexpr xhash_string_view(hash_t _hash, key_t _name) noexcept : hash(_hash), name(_name) {}
	constexpr xhash_string_view(const char_t* _name) noexcept : hash(traits_t::hash(_name)), name(_name) {}

	constexpr operator hash_t() const { return hash; }
	constexpr operator key_t() const { return name; }
};


////
	template<typename _TRAITS_TYPE_>
inline std::ostream& operator << (std::ostream& os, xhash_string_view< _TRAITS_TYPE_> hash_string_view)
{
	return os << "{0x" << std::hex << hash_string_view.hash << ", \"" << hash_string_view.name << "\"}";
}



////
// Eventually, support for UTF-8/16/32
// with system-specific implementation for native <-> international (unicode) conversion.
// ... eventually... well, that's the plan...
using char_t = char; // lets focus on ascii -> UTF-8 first : system is ascii/native, user is UTF-8
using xhash_t = uint64_t;
using xhash_traits_t = xhash_traits<char_t, xhash_t>;
using xhash_string_view_t = xhash_string_view<xhash_traits_t>;

constexpr xhash_string_view_t operator "" _xhash64(xhash_traits_t::const_pointer_t string, size_t length)
{
	return xhash_string_view_t{ std::basic_string_view<xhash_traits_t::char_t>(string, length) };
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
