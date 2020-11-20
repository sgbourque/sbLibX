#pragma once
#include <common/include/sb_common.h>

#include <cstdint>
#include <algorithm>
#include <string_view>
#include <iosfwd>

namespace SB { namespace LibX
{
////
	template<typename _CHAR_TYPE_, typename _HASH_T_, size_t _DEFAULT_LENGTH_ = 256, _HASH_T_ _PRIME_ = 0x9EF3455AD47C9E31ull, _HASH_T_ _INVERSE_PRIME_ = 0xDC649D343B77FAD1ull, _HASH_T_ _COPRIME_ = 0x03519CFFA7F0F405ull>
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
		inverse_prime = _INVERSE_PRIME_,
		coprime = _COPRIME_,
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


// TODO: constexpr encryption here
//	template <typename _CHAR_TYPE_, typename _HASH_TRAITS_, _CHAR_TYPE_... _UNENCRYPTED_STRING_>
//struct encrypted_string
//{
//	using char_t = _CHAR_TYPE_;
//	using hash_traits_t = _HASH_TRAITS_;
//	using const_pointer = const char_t*;
//	using encrypt_type = uint64_t;
//
//	using container_t = std::array<encrypt_type, (sizeof...(_UNENCRYPTED_STRING_) + sizeof(encrypt_type) - 1)/sizeof(encrypt_type)>;
//	container_t encrypted;
//
//	constexpr encrypted_string( char_t value... )
//		: encrypted( container_t{} ) {}
//
//	//constexpr auto encrypt( char_t value3, char_t value2, char_t value1, char_t value0, char_t extra... )
//	//{
//	//	return container_t{};
//	//	//return std::array<encrypt_type, 1 + ( (sizeof...(extra) + sizeof(encrypt_type)) / sizeof(encrypt_type) ) >{
//	//	//	encrypt( value3, value2, value1, value0 ),
//	//	//	encrypt( extra... ),
//	//	//};
//	//}
//
//	constexpr auto encrypt( char_t value3, char_t value2, char_t value1, char_t value0 )
//	{
//		const uint32_t swizzled = ( value0 << 24 ) | ( value2<< 16 ) | ( value3 << 8 ) | ( value1 << 0 );
//		
//	}
//	constexpr auto encrypt( char_t value2, char_t value1, char_t value0 )
//	{
//		return encrypt( value2, value1, value0, 0 );
//	}
//	constexpr auto encrypt( char_t value1, char_t value0 )
//	{
//		return encrypt( value1, value0, 0, 0 );
//	}
//	constexpr auto encrypt( char_t value )
//	{
//		return encrypt( value0, 0, 0, 0 );
//	}
//};
//
//static inline constexpr auto encrypt( const const_pointer _nextChar, const uint32_t* _encrypted, size_t _encrypted_size ) noexcept
//{
//}
//
//static inline constexpr auto encrypt( const const_pointer _Cts, const size_type _Count ) noexcept
//{
//	if ( _Count > 0 )
//		return encrypt( _Cts, encrypt( _Cts + 1, _Count - 1 ) );
//	else
//		return encrypt( "" );
//}

	template <typename _CHAR_TYPE_, typename _HASH_TRAITS_, typename _CHAR_TRAITS_ = std::char_traits<_CHAR_TYPE_>>
struct encrypted_string_view
{
	using char_t = _CHAR_TYPE_;
	using hash_traits_t = _HASH_TRAITS_;
	using char_traits_t = _CHAR_TRAITS_;
	using string_view_t = std::basic_string_view<char_t, char_traits_t>;
	using const_pointer = typename string_view_t::const_pointer;
	using size_type = typename string_view_t::size_type;

	constexpr encrypted_string_view() noexcept : string_view_t_value() {}
	constexpr encrypted_string_view( const encrypted_string_view& ) noexcept = default;
	constexpr encrypted_string_view& operator=( const encrypted_string_view& ) noexcept = default;

	constexpr encrypted_string_view( const const_pointer _Ntcts ) noexcept
		: string_view_t_value( _Ntcts ) {}

	// TODO: encrypt here!
	constexpr encrypted_string_view( const const_pointer _Cts, const size_type _Count ) noexcept
		: string_view_t_value( /*encrypt(*/ _Cts, _Count /*)*/ ) {}

	constexpr auto data() const { return string_view_t_value.data(); }
	constexpr auto size() const { return string_view_t_value.size(); }
	constexpr auto operator []( size_type index ) const { return string_view_t_value[index]; }

	using string_t = std::basic_string<char_t, char_traits_t>;
	constexpr auto encrypted() const { return "!encrypted! " + string_t(string_view_t_value) + " !encrypted!"; }
	//constexpr string_view_t encrypted() const { return string_t(string_view_t_value) + "(TODO: should have been encrypted)"; }

	// TODO: encrypt/decrypt here
	//constexpr auto encrypt( ..._Types ) const { return encrypt( encode(first), ...rest ); }
	constexpr auto decrypt() const { return "?decrypted? " + string_t{ string_view_t_value } + " ?decrypted?"; }

	//static inline constexpr excrypt( const const_pointer _Cts, const size_type _Count ) noexcept
	//{
	//	return string_view_t( _Cts, _Count );
	//}

private:
	string_view_t string_view_t_value;
};

//	template <class _CHAR_TYPE_, class _TRAITS_TYPE_ = std::char_traits<_CHAR_TYPE_>>
//inline std::ostream& operator << ( std::ostream& os, encrypted_string_view< _CHAR_TYPE_, _TRAITS_TYPE_> encripted_string_view )
//{
//	return os << encripted_string_view;
//}


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

	constexpr xhash_string_view(value_t _value) noexcept : key( traits_t::hash(_value.data(), _value.size()) ), value(_value) {}
	constexpr xhash_string_view(key_t _key, value_t _value) noexcept : key(_key), value(_value) {}
	constexpr xhash_string_view(const_char_ptr_t _value) noexcept : key(traits_t::hash(_value)), value(_value) {}

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
