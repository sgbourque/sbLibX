#pragma once
#include <common/include/sb_common.h>
//#include <common/include/sb_encrypted_string.h> // sb_hash and sb_encrypted_string are to be used together but won't use each other directly (only through traits)

#include <cstdint>
#include <algorithm>
#include <string_view>
#include <iosfwd>

namespace SB { namespace LibX
{
//	template<typename _CHAR_TYPE_, typename _HASH_T_, size_t _DEFAULT_LENGTH_ = 256, _HASH_T_ _PRIME_ = 0x9EF3455AD47C9E31ull, _HASH_T_ _COPRIME_ = 0x03519CFFA7F0F405ull>
//struct xhash_traits

////
	template<typename _CHAR_TYPE_, typename _HASH_T_, size_t _DEFAULT_LENGTH_, _HASH_T_ _PRIME_, _HASH_T_ _COPRIME_>
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
	};
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

using char_t = char; // lets focus on ascii -> UTF-8 first : system is ascii/native, user is UTF-8
using xhash_t = uint64_t;

	template<typename _CHAR_TYPE_, typename _HASH_TYPE_>
using xhash_traits_base_t = xhash_traits<_CHAR_TYPE_, _HASH_TYPE_, 256, 0x9EF3455AD47C9E31ull, 0x03519CFFA7F0F405ull>;
using xhash_traits_t = xhash_traits_base_t<char_t, xhash_t>;

}} // namespace SB::LibX
namespace sbLibX = SB::LibX;
