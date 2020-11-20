#pragma once
#include <cstdint>
#include <algorithm>
#include <string_view>

namespace SB { namespace LibX
{

	template<typename _CHAR_TYPE_, typename _HASH_VALUE_T_, size_t _MAX_LENGTH_ = 256, _HASH_VALUE_T_ _QUASI_PRIME_ = 0x9EF3455AD47C9E31ull, _HASH_VALUE_T_ _COPRIME_ = 0x03519CFFA7F0F405ull>
struct default_hash_traits // default are related to the _xhashXX algorithms
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
		max_length     = _MAX_LENGTH_,
		char_size      = sizeof(char_t),
		max_data_size  = max_length * char_size,

		prime_count    = 2,
	};
};

	template<typename hash_traits>
inline constexpr typename hash_traits::value_t
xhash(typename hash_traits::const_pointer_t string, size_t max_length = hash_traits::max_length)
{
	//constexpr prime_table_t = hash_traits::value_t[hash_traits::prime_count];
	return string && *string ? (xhash<hash_traits>(++string, --max_length) + *string * hash_traits::quasi_prime) * hash_traits::coprime : 0;
}

	template<typename _TRAITS_TYPE_>
struct hashed_string
{
	using hash_t = typename _TRAITS_TYPE_::value_t;
	using char_t = typename _TRAITS_TYPE_::char_t;
	using key_t  = std::basic_string_view<char_t>;
	static constexpr size_t max_length = _TRAITS_TYPE_::max_length;

	hash_t hash = 0;
	key_t  name = {}; // name is truncated to hash_traits_t::max_length characters, any less it will be null-terminated.
	constexpr operator hash_t() { return hash; }
	constexpr operator key_t() { return key_t(name, max_length); }
};

#if 0
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

	// Better primal testing (we really only requires that we are free of really factors).
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
