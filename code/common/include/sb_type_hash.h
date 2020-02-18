#pragma once
#include "common/include/sb_hash.h"

namespace SB { namespace LibX
{
////
// Get type_info from std::type_info
struct type_info_hash_traits_t
{
	using char_t = char;
	using value_t = size_t;
	using pointer_t = char_t*;
	using const_pointer_t = const char_t*;
	enum { invalid_hash = 0, };
};
	template<typename _IMPLEMENTATION_>
struct type_info_hash_traits : type_info_hash_traits_t
{
	using type_t  = _IMPLEMENTATION_;

};

using type_hash_string_t = xhash_string_view<type_info_hash_traits_t>;
template<typename _IMPL_> using type_hash_string = xhash_string_view<type_info_hash_traits<_IMPL_>>;

////
	template<typename _IMPLEMENTATION_>
static inline constexpr type_hash_string_t get_type_hash()
{
	using type_t = std::decay_t<_IMPLEMENTATION_ >;
	return type_hash_string_t(typeid(type_t).hash_code(), typeid(type_t).name());
}

}} // namespace SB::LibX
namespace sbLibX = SB::LibX;
