#pragma once
#include <common/include/sb_type_hash.h>

#include <array>

namespace SB { namespace LibX
{
// designed initializer not supported before 201705
#if _MSVC_LANG < 201705
	#define SB_STRUCT_SET( X_eq )
#else
	#define SB_STRUCT_SET( X_eq ) X_eq
#endif

static inline constexpr size_t align_down(size_t size, size_t align) { return (size / align) * align; }
static inline constexpr size_t align_up(size_t size, size_t align) { return align_down(size + align-1, align); }

//	So the idea here is to have a key mapping to a hash;
//	That hash will be used through the key_info_array which maps it to its data_info.
//
//	We assume from now on that all structured buffer (including dynamic ones to be defined later)
//	have an array-like data_info member and a set-like structure key_info.
//	There is a 1:1 correspondence for all valid keys and ∞:0 for invalid keys from a key_info
//	to a data_info. Generically, neither data_info nor key_info are assumed to be ordered containers
//	as far as data_info[valid_key] is valid while data_info[invalid_key] is invalid.
//	It is however assumed that both data_info and key_info are iterable containers.
//	With begin() and end() as data_info iterators and key_begin() and key_end() as key_info
//	iterators.
	template<typename, typename> struct key_info;
struct data_info_t;

template<typename _HASH_TRAITS_, size_t _DIMENSION_> using key_info_array = std::array<key_info<_HASH_TRAITS_, type_info_hash_traits_t>, _DIMENSION_>;
template<size_t _DIMENSION_> using data_info_array = std::array<data_info_t, _DIMENSION_>;

	template<typename name_hash_traits, typename type_hash_traits>
struct key_info
{
	xhash_string_view<name_hash_traits> name_hash;
	xhash_string_view<type_hash_traits> type_hash;
	size_t data_index;
};
struct data_info_t
{
	size_t offset;
	size_t size;
	size_t align;
	size_t key_index;
};

namespace StructuredBuffer
{
	template<typename _IMPLEMENTATION_>
static inline constexpr auto build_data_info(void) -> sbLibX::data_info_array<_IMPLEMENTATION_::size()>;
	template<typename _IMPLEMENTATION_>
static inline constexpr auto build_key_info(void) -> sbLibX::key_info_array<typename _IMPLEMENTATION_::hash_traits_t, _IMPLEMENTATION_::key_size()>;

#ifndef SB_STRUCTURED_BUFFER_MODIFIER
	#define SB_STRUCTURED_BUFFER_MODIFIER(...)
#else
	#define SB_STRUCTURED_BUFFER_MODIFIER(...) __VA_ARGS__
#endif
#ifndef SB_MEMBER_ENTRY
	#define SB_MEMBER_ENTRY "."
#endif
#define SB_STRUCT_BEGIN( STRUCTURED_BUFFER_NAME, ... ) SB_STRUCT_BEGIN_TRAITS( STRUCTURED_BUFFER_NAME, sbLibX::xhash_traits_t, __VA_ARGS__ )
#define SB_STRUCT_BEGIN_TRAITS( STRUCTURED_BUFFER_NAME, _TRAITS_, ... )\
	struct STRUCTURED_BUFFER_NAME {\
		using type_t = STRUCTURED_BUFFER_NAME;\
		using hash_traits_t = _TRAITS_;/*sbLibX::xhash_traits_t;*/\
		using hash_string_view_t = sbLibX::xhash_string_view<hash_traits_t>;\
		using hash_t = typename hash_traits_t::hash_t;\
		static constexpr auto options = std::make_tuple(__VA_ARGS__);\
		inline static constexpr hash_string_view_t struct_hash = #STRUCTURED_BUFFER_NAME " " #__VA_ARGS__ "";\
		\
		template<hash_t, typename = type_t> struct data_traits;\
		template<size_t, typename = type_t> struct key_traits;\
		static inline constexpr size_t kElementIdOffset = __COUNTER__;
#define SB_STRUCT_MEMBER( TYPE, NAME, ... ) /* note that TYPE can't be hashed by name */\
		static inline constexpr size_t NAME##_id = __COUNTER__ - kElementIdOffset - 1;\
		static_assert(NAME##_id < 64, "Too many unique data members. Try using sub-buffers or split the buffer.");\
		static inline constexpr hash_string_view_t NAME##_hash = SB_MEMBER_ENTRY #NAME "";\
		template<typename IMPL> struct data_traits<hash_traits_t::hash(SB_MEMBER_ENTRY #NAME ""), IMPL>\
		{\
			using type_t = decltype(IMPL::NAME);\
			static inline constexpr size_t offset = offsetof(IMPL, NAME);\
			static inline constexpr size_t size   = sizeof(type_t);\
			using key_traits_t = key_traits<NAME##_id, IMPL>;\
		};\
		template<typename IMPL> struct key_traits<NAME##_id, IMPL>\
		{\
			static inline constexpr hash_string_view_t name_hash = IMPL::NAME##_hash;\
			using type_t = decltype(IMPL::NAME);\
			using data_traits_t = data_traits<name_hash, IMPL>;\
		};\
		using NAME##_base_t = TYPE; SB_STRUCTURED_BUFFER_MODIFIER(__VA_ARGS__) NAME##_base_t NAME /* modifiers NOT supported yet (I will try to find a way) */
#define SB_STRUCT_END( STRUCTURED_BUFFER_NAME, ... ) /* STRUCTURED_BUFFER_NAME in case we need to initialize static data*/\
		static inline constexpr size_t kElementCount = __COUNTER__ - kElementIdOffset - 1;\
		static inline constexpr size_t size() { return kElementCount; }\
		using data_info_array_t = sbLibX::data_info_array<kElementCount>;\
		static const data_info_array_t data_info;\
		static inline constexpr auto begin() { return data_info.begin(); }\
		static inline constexpr auto end() { return data_info.end(); }\
		\
		static inline constexpr size_t kKeyCount = (kElementCount + 1);/*sbLibX::StructuredBuffer::compute_optimal_key_count<type_t>();*/\
		static inline constexpr size_t key_size() { return kKeyCount; }\
		using key_info_array_t = sbLibX::key_info_array<hash_traits_t, kKeyCount>;\
		static const key_info_array_t key_info;\
		static inline constexpr auto key_begin() { return key_info.begin(); }\
		static inline constexpr auto key_end() { return key_info.end(); }\
	};\
	const typename STRUCTURED_BUFFER_NAME::key_info_array_t\
	STRUCTURED_BUFFER_NAME::key_info = sbLibX::StructuredBuffer::build_key_info<type_t>();\
	const typename STRUCTURED_BUFFER_NAME::data_info_array_t\
	STRUCTURED_BUFFER_NAME::data_info = sbLibX::StructuredBuffer::build_data_info<type_t>();

////
	template<typename _IMPLEMENTATION_, typename _FACET_>
struct build_helper;
////
	template<typename _IMPLEMENTATION_>
struct build_helper<_IMPLEMENTATION_, typename _IMPLEMENTATION_::data_info_array_t>
{
	using type_t = _IMPLEMENTATION_;
	using data_info_array_t = typename type_t::data_info_array_t;
	inline constexpr operator data_info_array_t() { return get_data_info(); }
	static inline constexpr auto get_data_info() { return range_t<0, data_info_array_t().size()>::get_data_info(); }

		template<size_t BEGIN, size_t END>
	struct range_t {
			template<typename ...TYPE_T>
		static constexpr data_info_array_t get_data_info(TYPE_T... data_info)
		{
			using key_traits = typename type_t::key_traits<END - 1>;
			using data_traits = typename key_traits::data_traits_t;
			return range_t<BEGIN, END - 1>::get_data_info(
				data_info_t{
					SB_STRUCT_SET(.offset    =) data_traits::offset, // TODO: what to do with static data & members? (might have to deal with more tables)
					SB_STRUCT_SET(.size      =) sizeof(typename data_traits::type_t),
					SB_STRUCT_SET(.align     =) alignof(typename data_traits::type_t),
					SB_STRUCT_SET(.key_index =) find_key_index(END - 1),
				},
				data_info... );
		}
	};
		template<size_t BEGIN>
	struct range_t<BEGIN, BEGIN> {
			template<typename ...TYPE_T>
		static constexpr data_info_array_t get_data_info(TYPE_T... data_info)
		{ return data_info_array_t{ data_info... }; }
	};

	static inline constexpr size_t find_key_index(size_t dataindex)
	{
		for (size_t index = 0; index < type_t::key_info.size(); ++index )
			if ( type_t::key_info[index].data_index == dataindex )
				return index;
		return ~0u;
	}
};
	template<typename _IMPLEMENTATION_>
static inline constexpr data_info_array<_IMPLEMENTATION_::size()> build_data_info(void)
{
	return build_helper<_IMPLEMENTATION_, _IMPLEMENTATION_::data_info_array_t>();
}
////
	template<typename _IMPLEMENTATION_>
struct build_helper<_IMPLEMENTATION_, typename _IMPLEMENTATION_::key_info_array_t>
{
	using type_t = _IMPLEMENTATION_;
	using data_info_array_t = typename type_t::data_info_array_t;
	using key_info_array_t = typename type_t::key_info_array_t;
	static constexpr size_t kElementCount = type_t::kElementCount;
	static constexpr size_t kKeyCount = type_t::kKeyCount;
	inline constexpr operator key_info_array_t() { return get_key_info(); }
	static inline constexpr auto get_key_info() {
		static_assert(kKeyCount > kElementCount);
		return range_t<0, data_info_array_t().size()>::get_key_info();
	}

		template<size_t BEGIN, size_t END, size_t ...HASH_VALUES>
	struct range_t {
			template<typename ...TYPE_T>
		static constexpr key_info_array_t get_key_info(TYPE_T... keys)
		{
			using key_traits  = typename type_t::key_traits<BEGIN>;
			using data_traits = typename key_traits::data_traits_t;
			using data_t      = typename data_traits::type_t;
			using type_hash_t = type_hash_string_t;
			using key_info_t  = key_info<typename type_t::hash_traits_t, type_info_hash_traits_t>;
			type_hash_t type_hash = get_type_hash<data_t>();
			return range_t<BEGIN + 1, END, HASH_VALUES..., key_traits::name_hash>::get_key_info(
				keys...,
				key_info_t{
					SB_STRUCT_SET(.name_hash  =) key_traits::name_hash,
					SB_STRUCT_SET(.type_hash  =) type_hash,
					SB_STRUCT_SET(.data_index =) BEGIN,
				});
		}
	};
		template<size_t END, size_t ...HASH_VALUES>
	struct range_t<END, END, HASH_VALUES...> {
			template<typename ...TYPE_T>
		static constexpr auto get_key_info(TYPE_T... keys)
		{ return sort_key_info<HASH_VALUES...>( keys... ); }
	};
	//	template<size_t ...HASH_VALUES, typename ...TYPE_T>
	//static constexpr key_info_array_t sort_key_info(TYPE_T... keys)
	//{
	//	// TODO : This is where I'm not sure it would be easy without C++ static reflection.
	//	// however, we could probably overload this function for specific types to get optimal
	//	// hash tables with a simple 1<->1 get_index_from_hash(hash) expression.
	//	//
	//	// Alternatively, with a hash_traits in the key_info,
	//	// we could define a optimal hashing traits for any given struct.
	//	return key_info_array_t{ key_info_t<typename type_t::hash_traits_t, type_info_hash_traits_t>{}, keys... };
	//}
		template<size_t ...HASH_VALUES, typename ...TYPE_T>
	static constexpr key_info_array_t sort_key_info(TYPE_T... keys)
	{
		using key_info_t = key_info<typename type_t::hash_traits_t, type_info_hash_traits_t>;
		key_info_array_t result{ key_info_t{}, keys... };
		// sort all valid indices
		std::sort(result.begin() + 1, result.end(), [&result](const key_info_t& a, const key_info_t& b) -> bool
		{
			size_t a_hint = (a.name_hash.get_key() % kKeyCount); const bool a_valid = a.name_hash.get_key() != type_t::hash_traits_t::invalid_hash;
			size_t b_hint = (b.name_hash.get_key() % kKeyCount); const bool b_valid = b.name_hash.get_key() != type_t::hash_traits_t::invalid_hash;
			return (a_hint < b_hint) && (a_valid && b_valid);
		});
		// place (unique) invalid data to the "optimal" spot
		for (size_t index = 1; index < result.size(); ++index)
		{
			auto& keyinfo = result[index];
			size_t keyinfo_hint = (keyinfo.name_hash.get_key() % kKeyCount);
			if (keyinfo_hint < index)
				std::swap(keyinfo, result[index - 1]);
			else
				break;
		}
		return result;
	}

};
	template<typename _IMPLEMENTATION_>
static inline constexpr key_info_array<typename _IMPLEMENTATION_::hash_traits_t, _IMPLEMENTATION_::key_size()> build_key_info(void)
{
	return build_helper<_IMPLEMENTATION_, typename _IMPLEMENTATION_::key_info_array_t>();
}

	template<xhash_t HASH, typename _IMPLEMENTATION_>
auto get(_IMPLEMENTATION_* buffer) noexcept
{
	using type_t = typename _IMPLEMENTATION_::template data_traits<HASH, _IMPLEMENTATION_>::type_t;
	return reinterpret_cast<type_t*>( reinterpret_cast<uint8_t*>(buffer) + _IMPLEMENTATION_::template data_traits<HASH>::offset );
}
template<xhash_t HASH, typename _IMPLEMENTATION_> auto& get(_IMPLEMENTATION_& buffer) noexcept { return *get<HASH>(&buffer); }
template<xhash_t HASH, typename _IMPLEMENTATION_> auto&& get(_IMPLEMENTATION_&& buffer) noexcept { return *get<HASH>(&buffer); }

} // StructuredBuffer

//#include <sb_dynamic_buffer.h>
// Let's assume for now that we have the following :
// 1. We have an array of data_info on which we can iterate;
// 2. We have an acceleration table that maps a hash to either a corresponding entry of a buffer or the invalid entry
//    (which is unique up to equivalence in which );

//	template<typename RETURN_TYPE, typename _IMPLEMENTATION_>
//RETURN_TYPE sb_get([[maybe_unused]] _IMPLEMENTATION_* buffer, [[maybe_unused]] xhash_t hash, RETURN_TYPE value = {})
//{
//	// TODO : don't forget to assert on struct size & alignment requirement
//	return value;
//}
//template<typename RETURN_TYPE, typename _IMPLEMENTATION_> RETURN_TYPE sb_get(_IMPLEMENTATION_& buffer, xhash_t hash, RETURN_TYPE value = {}) { return sb_get(&buffer, hash, value); }
//
//	template<typename _IMPLEMENTATION_>
//raw_data_t sb_get([[maybe_unused]] _IMPLEMENTATION_* buffer, [[maybe_unused]] xhash_t hash)
//{
//	return raw_data_t{};
//}
//template<typename _IMPLEMENTATION_> auto sb_get(_IMPLEMENTATION_& buffer, xhash_t hash) { return sb_get(&buffer, hash); }

}} // namespace sbLibX
