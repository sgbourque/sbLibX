#include <common/include/sb_hash.h>

#include <algorithm>
#include <array>
#include <cstdint>

namespace SB { namespace LibX
{
static inline constexpr size_t align_down(size_t size, size_t align) { return (size / align) * align; }
static inline constexpr size_t align_up(size_t size, size_t align) { return align_down(size + align-1, align); }

//	So the idea here is to have a key mapping to a hash;
//	That hash will be used through the data_key_array which maps it to its data_info.
struct key_info_t;
struct data_info_t;
//struct raw_data_t;
template<size_t _DIMENSION_> using key_info_array  = std::array<key_info_t, _DIMENSION_>;
template<size_t _DIMENSION_> using data_info_array = std::array<data_info_t, _DIMENSION_>;

struct key_info_t
{
	hashed_string_t name_hash;
	hashed_string_t type_hash;
	size_t          data_index;
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
static inline constexpr auto build_data_info(void) -> sbLibX::data_info_array<_IMPLEMENTATION_::kElementCount>;
	template<typename _IMPLEMENTATION_>
static inline constexpr auto build_key_info(void) -> sbLibX::key_info_array<_IMPLEMENTATION_::kKeyCount>;

#ifndef SB_STRUCTURED_BUFFER_MODIFIER
	#define SB_STRUCTURED_BUFFER_MODIFIER(...)
#else
	//#define SB_STRUCTURED_BUFFER_MODIFIER(...) __VA_ARGS__
#endif
#ifndef SB_MEMBER_ENTRY
	#define SB_MEMBER_ENTRY "."
#endif
#define SB_STRUCT_BEGIN( STRUCTURED_BUFFER_NAME, ... )\
	struct STRUCTURED_BUFFER_NAME {\
		using type_t = STRUCTURED_BUFFER_NAME;\
		using hashed_string_t = sbLibX::hashed_string_t;\
		using hash_t = typename hashed_string_t::hash_t;\
		static constexpr auto options = std::make_tuple(__VA_ARGS__);\
		inline static constexpr hashed_string_t struct_hash = #STRUCTURED_BUFFER_NAME " " #__VA_ARGS__ ""_xhash64;\
		\
		template<hash_t, typename IMPL = type_t> struct data_traits;\
		template<size_t, typename IMPL = type_t> struct key_traits;\
		static inline constexpr size_t kElementIdOffset = __COUNTER__;
#define SB_STRUCT_MEMBER( TYPE, NAME, ... ) /* note that TYPE can't be hashed by name */\
		static inline constexpr size_t NAME##_id = __COUNTER__ - kElementIdOffset - 1;\
		static_assert(NAME##_id < 64, "To much unique data members. Try using sub-buffers.");\
		static inline constexpr hashed_string_t NAME##_hash = SB_MEMBER_ENTRY #NAME ""_xhash64;\
		template<typename IMPL> struct data_traits<SB_MEMBER_ENTRY #NAME ""_xhash64, IMPL>\
		{\
			using type_t = decltype(IMPL::NAME);\
			static inline constexpr size_t offset = offsetof(IMPL, NAME);\
			static inline constexpr size_t size   = sizeof(type_t);\
			/*static inline constexpr size_t index  = NAME##_id;*/\
		};\
		template<typename IMPL> struct key_traits<NAME##_id, IMPL>\
		{\
			static inline constexpr hashed_string_t name_hash = IMPL::NAME##_hash;\
			using type_t = decltype(IMPL::NAME);/* TODO: type hash */\
			using data_traits_t = data_traits<name_hash, IMPL>;\
		};\
		using NAME##_base_t = TYPE; SB_STRUCTURED_BUFFER_MODIFIER(__VA_ARGS__) NAME##_base_t NAME /* Not supported (I will try to find a way) */
#define SB_STRUCT_END( STRUCTURED_BUFFER_NAME, ... ) /* STRUCTURED_BUFFER_NAME in case we need to initialize static data*/\
		static inline constexpr size_t kElementCount = __COUNTER__ - kElementIdOffset - 1;\
		static inline constexpr size_t size() { return kElementCount; };\
		static inline constexpr auto begin() { return data_info.begin(); };\
		static inline constexpr auto end() { return data_info.end(); };\
		using data_info_array_t = sbLibX::data_info_array<kElementCount>;\
		static const data_info_array_t data_info;\
		static inline constexpr size_t kKeyCount = /*nextprime*/(kElementCount + 1);/*sbLibX::StructuredBuffer::compute_optimal_key_count<type_t>();*/\
		using key_info_array_t = sbLibX::key_info_array<kKeyCount>;\
		static const key_info_array_t key_info;\
	};\
	const STRUCTURED_BUFFER_NAME::data_info_array_t STRUCTURED_BUFFER_NAME::data_info = sbLibX::StructuredBuffer::build_data_info<type_t>();\
	const STRUCTURED_BUFFER_NAME::key_info_array_t STRUCTURED_BUFFER_NAME::key_info = sbLibX::StructuredBuffer::build_key_info<type_t>();

//struct raw_data_t
//{
//	void* data;
//	size_t size;
//	hash_t type;
//};

////
	template<typename _IMPLEMENTATION_, typename _FACET_>
struct build_helper;
////
	template<typename _IMPLEMENTATION_>
struct build_helper<_IMPLEMENTATION_, data_info_t>
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
					.offset = data_traits::offset, // TODO: what to do with static data & members?
					.size = sizeof(typename data_traits::type_t),
					.align = alignof(typename data_traits::type_t),
					.key_index = END, // TODO
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
};
	template<typename _IMPLEMENTATION_>
static inline constexpr data_info_array<_IMPLEMENTATION_::kElementCount> build_data_info(void)
{
	return build_helper<_IMPLEMENTATION_, data_info_t>();
}
////
	template<typename _IMPLEMENTATION_>
struct build_helper<_IMPLEMENTATION_, key_info_t>
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

		template<size_t BEGIN, size_t END, size_t ...indices>
	struct range_t {
			template<typename ...TYPE_T>
		static constexpr key_info_array_t get_key_info(TYPE_T... key_info)
		{
			using key_traits = typename type_t::key_traits<BEGIN>;
			using data_traits = typename key_traits::data_traits_t;
			const std::type_info& type_info = typeid(data_traits::type_t);
			return range_t<BEGIN + 1, END, indices..., (key_traits::name_hash % kKeyCount)>::get_key_info(
				key_info...,
				key_info_t{
					.name_hash  = key_traits::name_hash,
					.type_hash  = hashed_string_t(std::string_view(type_info.name()), hash_t(type_info.hash_code())),
					.data_index = BEGIN,
				});
		}
	};
		template<size_t END, size_t ...indices>
	struct range_t<END, END, indices...> {
			template<typename ...TYPE_T>
		static constexpr auto get_key_info(TYPE_T... key_info)
		{ return sort_key_info<indices...>( key_info... ); }
	};
		template<size_t ...indices, typename ...TYPE_T>
	static constexpr key_info_array_t sort_key_info(TYPE_T... key_info)
	{
		return key_info_array_t{ key_info_t{}, key_info... };
	}
};
	template<typename _IMPLEMENTATION_>
static inline constexpr key_info_array<_IMPLEMENTATION_::kKeyCount> build_key_info(void)
{
	return build_helper<_IMPLEMENTATION_, key_info_t>();
}

} // StructuredBuffer


	template<hash_t HASH, typename _IMPLEMENTATION_>
auto sb_get(_IMPLEMENTATION_* buffer)
{
	using type_t = typename _IMPLEMENTATION_::template data_traits<HASH, _IMPLEMENTATION_>::type_t;
	return reinterpret_cast<type_t*>( reinterpret_cast<uint8_t*>(buffer) + _IMPLEMENTATION_::data_traits<HASH>::offset );
}
template<hash_t HASH, typename _IMPLEMENTATION_> auto& sb_get(_IMPLEMENTATION_& buffer) { return *sb_get<HASH>(&buffer); }
template<hash_t HASH, typename _IMPLEMENTATION_> auto&& sb_get(_IMPLEMENTATION_&& buffer) { return *sb_get<HASH>(&buffer); }


// Let's assume for now that we have the following :
// 1. We have an array of data_info on which we can iterate;
// 2. We have an acceleration table that maps a hash to either a corresponding entry of a buffer or the invalid entry
//    (which is unique up to equivalence in which );

//	template<typename RETURN_TYPE, typename _IMPLEMENTATION_>
//RETURN_TYPE sb_get([[maybe_unused]] _IMPLEMENTATION_* buffer, [[maybe_unused]] hash_t hash, RETURN_TYPE value = {})
//{
//	// TODO : don't forget to assert on struct size & alignment requirement
//	return value;
//}
//template<typename RETURN_TYPE, typename _IMPLEMENTATION_> RETURN_TYPE sb_get(_IMPLEMENTATION_& buffer, hash_t hash, RETURN_TYPE value = {}) { return sb_get(&buffer, hash, value); }
//
//	template<typename _IMPLEMENTATION_>
//raw_data_t sb_get([[maybe_unused]] _IMPLEMENTATION_* buffer, [[maybe_unused]] hash_t hash)
//{
//	return raw_data_t{};
//}
//template<typename _IMPLEMENTATION_> auto sb_get(_IMPLEMENTATION_& buffer, hash_t hash) { return sb_get(&buffer, hash); }

}} // namespace sbLibX


using sbLibX::operator "" _xhash64; \
using sbLibX::hash_t;
using sbLibX::hashed_string_t;

#include <xmmintrin.h>

////
SB_STRUCT_BEGIN(module_config_t, 0, 1, -1)
	SB_STRUCT_MEMBER(hashed_string_t, name)        = "(...)"_xhash64;
	SB_STRUCT_MEMBER(hashed_string_t, className)   = ""_xhash64;
	SB_STRUCT_MEMBER(uint64_t,        moduleFlags) = 0; // or whatever...
SB_STRUCT_END(module_config_t);


////
#include <iostream>
#pragma warning(disable:4324)
	// warning C4324: 'custom_class_t' : la structure a été remplie en raison du spécificateur d'alignement
	// because of that really weird __m128 in the middle of nowhere (ideally: try aligning your stuff)...
SB_STRUCT_BEGIN(custom_class_t, "my super hyper custom class!!", -1, "booo")
	SB_STRUCT_MEMBER(sbLibX::hashed_string_t, name)      = "hash";
	__m128 anonymous_vec4f; // "anonymous" data can be added anywhere
	SB_STRUCT_MEMBER(module_config_t, config) = {.name = "\0\0\0"};
	SB_STRUCT_MEMBER(uint8_t[64 * 1024], buffer) = {};
	SB_STRUCT_MEMBER(uint8_t, a_buffer_with_value_type_instead_of_array_type)[8 * 1024] = {};
	virtual ~custom_class_t() { /*std::cout << "bye from " << (config.className.name[0] ? std::string(config.className) + "/" : "") << config.name << "/" << name; "\n";*/ }

	// TODO: This will not work as I can't (yet) detect static data to process differently.
	// Also, I will have to deal with (virtual/non-virtual) functions.
	//SB_STRUCT_MEMBER(long const int unsigned volatile long, try_not_to, inline static constexpr)(int n) { return {n != 3}; }
	//SB_STRUCT_MEMBER(custom_class_t, do_weird_impl, virtual)(custom_class_t other) { other.config = {name, config.className, buffer[0] ^= 1}; return other; }
SB_STRUCT_END(custom_class_t);
#pragma warning(default:4324)


////
// for compilation performance check
#define COMPILATION_STRESS_MEMBERS	0
SB_STRUCT_BEGIN(compile_stress_t)
#if COMPILATION_STRESS_MEMBERS >= 1
	SB_STRUCT_MEMBER(int, n000); SB_STRUCT_MEMBER(int, n001); SB_STRUCT_MEMBER(int, n002); SB_STRUCT_MEMBER(int, n003); SB_STRUCT_MEMBER(int, n004); SB_STRUCT_MEMBER(int, n005); SB_STRUCT_MEMBER(int, n006); SB_STRUCT_MEMBER(int, n007); SB_STRUCT_MEMBER(int, n008); SB_STRUCT_MEMBER(int, n009); SB_STRUCT_MEMBER(int, n00a); SB_STRUCT_MEMBER(int, n00b); SB_STRUCT_MEMBER(int, n00c); SB_STRUCT_MEMBER(int, n00d); SB_STRUCT_MEMBER(int, n00e); SB_STRUCT_MEMBER(int, n00f);
#endif
#if COMPILATION_STRESS_MEMBERS >= 32
	SB_STRUCT_MEMBER(int, n010); SB_STRUCT_MEMBER(int, n011); SB_STRUCT_MEMBER(int, n012); SB_STRUCT_MEMBER(int, n013); SB_STRUCT_MEMBER(int, n014); SB_STRUCT_MEMBER(int, n015); SB_STRUCT_MEMBER(int, n016); SB_STRUCT_MEMBER(int, n017); SB_STRUCT_MEMBER(int, n018); SB_STRUCT_MEMBER(int, n019); SB_STRUCT_MEMBER(int, n01a); SB_STRUCT_MEMBER(int, n01b); SB_STRUCT_MEMBER(int, n01c); SB_STRUCT_MEMBER(int, n01d); SB_STRUCT_MEMBER(int, n01e); SB_STRUCT_MEMBER(int, n01f);
#endif
#if COMPILATION_STRESS_MEMBERS >= 64
	SB_STRUCT_MEMBER(int, n020); SB_STRUCT_MEMBER(int, n021); SB_STRUCT_MEMBER(int, n022); SB_STRUCT_MEMBER(int, n023); SB_STRUCT_MEMBER(int, n024); SB_STRUCT_MEMBER(int, n025); SB_STRUCT_MEMBER(int, n026); SB_STRUCT_MEMBER(int, n027); SB_STRUCT_MEMBER(int, n028); SB_STRUCT_MEMBER(int, n029); SB_STRUCT_MEMBER(int, n02a); SB_STRUCT_MEMBER(int, n02b); SB_STRUCT_MEMBER(int, n02c); SB_STRUCT_MEMBER(int, n02d); SB_STRUCT_MEMBER(int, n02e); SB_STRUCT_MEMBER(int, n02f);
	SB_STRUCT_MEMBER(int, n030); SB_STRUCT_MEMBER(int, n031); SB_STRUCT_MEMBER(int, n032); SB_STRUCT_MEMBER(int, n033); SB_STRUCT_MEMBER(int, n034); SB_STRUCT_MEMBER(int, n035); SB_STRUCT_MEMBER(int, n036); SB_STRUCT_MEMBER(int, n037); SB_STRUCT_MEMBER(int, n038); SB_STRUCT_MEMBER(int, n039); SB_STRUCT_MEMBER(int, n03a); SB_STRUCT_MEMBER(int, n03b); SB_STRUCT_MEMBER(int, n03c); SB_STRUCT_MEMBER(int, n03d); SB_STRUCT_MEMBER(int, n03e); SB_STRUCT_MEMBER(int, n03f);
#endif
#if COMPILATION_STRESS_MEMBERS >= 128
	SB_STRUCT_MEMBER(int, n040); SB_STRUCT_MEMBER(int, n041); SB_STRUCT_MEMBER(int, n042); SB_STRUCT_MEMBER(int, n043); SB_STRUCT_MEMBER(int, n044); SB_STRUCT_MEMBER(int, n045); SB_STRUCT_MEMBER(int, n046); SB_STRUCT_MEMBER(int, n047); SB_STRUCT_MEMBER(int, n048); SB_STRUCT_MEMBER(int, n049); SB_STRUCT_MEMBER(int, n04a); SB_STRUCT_MEMBER(int, n04b); SB_STRUCT_MEMBER(int, n04c); SB_STRUCT_MEMBER(int, n04d); SB_STRUCT_MEMBER(int, n04e); SB_STRUCT_MEMBER(int, n04f);
	SB_STRUCT_MEMBER(int, n050); SB_STRUCT_MEMBER(int, n051); SB_STRUCT_MEMBER(int, n052); SB_STRUCT_MEMBER(int, n053); SB_STRUCT_MEMBER(int, n054); SB_STRUCT_MEMBER(int, n055); SB_STRUCT_MEMBER(int, n056); SB_STRUCT_MEMBER(int, n057); SB_STRUCT_MEMBER(int, n058); SB_STRUCT_MEMBER(int, n059); SB_STRUCT_MEMBER(int, n05a); SB_STRUCT_MEMBER(int, n05b); SB_STRUCT_MEMBER(int, n05c); SB_STRUCT_MEMBER(int, n05d); SB_STRUCT_MEMBER(int, n05e); SB_STRUCT_MEMBER(int, n05f);
	SB_STRUCT_MEMBER(int, n060); SB_STRUCT_MEMBER(int, n061); SB_STRUCT_MEMBER(int, n062); SB_STRUCT_MEMBER(int, n063); SB_STRUCT_MEMBER(int, n064); SB_STRUCT_MEMBER(int, n065); SB_STRUCT_MEMBER(int, n066); SB_STRUCT_MEMBER(int, n067); SB_STRUCT_MEMBER(int, n068); SB_STRUCT_MEMBER(int, n069); SB_STRUCT_MEMBER(int, n06a); SB_STRUCT_MEMBER(int, n06b); SB_STRUCT_MEMBER(int, n06c); SB_STRUCT_MEMBER(int, n06d); SB_STRUCT_MEMBER(int, n06e); SB_STRUCT_MEMBER(int, n06f);
	SB_STRUCT_MEMBER(int, n070); SB_STRUCT_MEMBER(int, n071); SB_STRUCT_MEMBER(int, n072); SB_STRUCT_MEMBER(int, n073); SB_STRUCT_MEMBER(int, n074); SB_STRUCT_MEMBER(int, n075); SB_STRUCT_MEMBER(int, n076); SB_STRUCT_MEMBER(int, n077); SB_STRUCT_MEMBER(int, n078); SB_STRUCT_MEMBER(int, n079); SB_STRUCT_MEMBER(int, n07a); SB_STRUCT_MEMBER(int, n07b); SB_STRUCT_MEMBER(int, n07c); SB_STRUCT_MEMBER(int, n07d); SB_STRUCT_MEMBER(int, n07e); SB_STRUCT_MEMBER(int, n07f);
#endif
#if COMPILATION_STRESS_MEMBERS >= 256
	SB_STRUCT_MEMBER(int, n080); SB_STRUCT_MEMBER(int, n081); SB_STRUCT_MEMBER(int, n082); SB_STRUCT_MEMBER(int, n083); SB_STRUCT_MEMBER(int, n084); SB_STRUCT_MEMBER(int, n085); SB_STRUCT_MEMBER(int, n086); SB_STRUCT_MEMBER(int, n087); SB_STRUCT_MEMBER(int, n088); SB_STRUCT_MEMBER(int, n089); SB_STRUCT_MEMBER(int, n08a); SB_STRUCT_MEMBER(int, n08b); SB_STRUCT_MEMBER(int, n08c); SB_STRUCT_MEMBER(int, n08d); SB_STRUCT_MEMBER(int, n08e); SB_STRUCT_MEMBER(int, n08f);
	SB_STRUCT_MEMBER(int, n090); SB_STRUCT_MEMBER(int, n091); SB_STRUCT_MEMBER(int, n092); SB_STRUCT_MEMBER(int, n093); SB_STRUCT_MEMBER(int, n094); SB_STRUCT_MEMBER(int, n095); SB_STRUCT_MEMBER(int, n096); SB_STRUCT_MEMBER(int, n097); SB_STRUCT_MEMBER(int, n098); SB_STRUCT_MEMBER(int, n099); SB_STRUCT_MEMBER(int, n09a); SB_STRUCT_MEMBER(int, n09b); SB_STRUCT_MEMBER(int, n09c); SB_STRUCT_MEMBER(int, n09d); SB_STRUCT_MEMBER(int, n09e); SB_STRUCT_MEMBER(int, n09f);
	SB_STRUCT_MEMBER(int, n0a0); SB_STRUCT_MEMBER(int, n0a1); SB_STRUCT_MEMBER(int, n0a2); SB_STRUCT_MEMBER(int, n0a3); SB_STRUCT_MEMBER(int, n0a4); SB_STRUCT_MEMBER(int, n0a5); SB_STRUCT_MEMBER(int, n0a6); SB_STRUCT_MEMBER(int, n0a7); SB_STRUCT_MEMBER(int, n0a8); SB_STRUCT_MEMBER(int, n0a9); SB_STRUCT_MEMBER(int, n0aa); SB_STRUCT_MEMBER(int, n0ab); SB_STRUCT_MEMBER(int, n0ac); SB_STRUCT_MEMBER(int, n0ad); SB_STRUCT_MEMBER(int, n0ae); SB_STRUCT_MEMBER(int, n0af);
	SB_STRUCT_MEMBER(int, n0b0); SB_STRUCT_MEMBER(int, n0b1); SB_STRUCT_MEMBER(int, n0b2); SB_STRUCT_MEMBER(int, n0b3); SB_STRUCT_MEMBER(int, n0b4); SB_STRUCT_MEMBER(int, n0b5); SB_STRUCT_MEMBER(int, n0b6); SB_STRUCT_MEMBER(int, n0b7); SB_STRUCT_MEMBER(int, n0b8); SB_STRUCT_MEMBER(int, n0b9); SB_STRUCT_MEMBER(int, n0ba); SB_STRUCT_MEMBER(int, n0bb); SB_STRUCT_MEMBER(int, n0bc); SB_STRUCT_MEMBER(int, n0bd); SB_STRUCT_MEMBER(int, n0be); SB_STRUCT_MEMBER(int, n0bf);
	SB_STRUCT_MEMBER(int, n0c0); SB_STRUCT_MEMBER(int, n0c1); SB_STRUCT_MEMBER(int, n0c2); SB_STRUCT_MEMBER(int, n0c3); SB_STRUCT_MEMBER(int, n0c4); SB_STRUCT_MEMBER(int, n0c5); SB_STRUCT_MEMBER(int, n0c6); SB_STRUCT_MEMBER(int, n0c7); SB_STRUCT_MEMBER(int, n0c8); SB_STRUCT_MEMBER(int, n0c9); SB_STRUCT_MEMBER(int, n0ca); SB_STRUCT_MEMBER(int, n0cb); SB_STRUCT_MEMBER(int, n0cc); SB_STRUCT_MEMBER(int, n0cd); SB_STRUCT_MEMBER(int, n0ce); SB_STRUCT_MEMBER(int, n0cf);
	SB_STRUCT_MEMBER(int, n0d0); SB_STRUCT_MEMBER(int, n0d1); SB_STRUCT_MEMBER(int, n0d2); SB_STRUCT_MEMBER(int, n0d3); SB_STRUCT_MEMBER(int, n0d4); SB_STRUCT_MEMBER(int, n0d5); SB_STRUCT_MEMBER(int, n0d6); SB_STRUCT_MEMBER(int, n0d7); SB_STRUCT_MEMBER(int, n0d8); SB_STRUCT_MEMBER(int, n0d9); SB_STRUCT_MEMBER(int, n0da); SB_STRUCT_MEMBER(int, n0db); SB_STRUCT_MEMBER(int, n0dc); SB_STRUCT_MEMBER(int, n0dd); SB_STRUCT_MEMBER(int, n0de); SB_STRUCT_MEMBER(int, n0df);
	SB_STRUCT_MEMBER(int, n0e0); SB_STRUCT_MEMBER(int, n0e1); SB_STRUCT_MEMBER(int, n0e2); SB_STRUCT_MEMBER(int, n0e3); SB_STRUCT_MEMBER(int, n0e4); SB_STRUCT_MEMBER(int, n0e5); SB_STRUCT_MEMBER(int, n0e6); SB_STRUCT_MEMBER(int, n0e7); SB_STRUCT_MEMBER(int, n0e8); SB_STRUCT_MEMBER(int, n0e9); SB_STRUCT_MEMBER(int, n0ea); SB_STRUCT_MEMBER(int, n0eb); SB_STRUCT_MEMBER(int, n0ec); SB_STRUCT_MEMBER(int, n0ed); SB_STRUCT_MEMBER(int, n0ee); SB_STRUCT_MEMBER(int, n0ef);
	SB_STRUCT_MEMBER(int, n0f0); SB_STRUCT_MEMBER(int, n0f1); SB_STRUCT_MEMBER(int, n0f2); SB_STRUCT_MEMBER(int, n0f3); SB_STRUCT_MEMBER(int, n0f4); SB_STRUCT_MEMBER(int, n0f5); SB_STRUCT_MEMBER(int, n0f6); SB_STRUCT_MEMBER(int, n0f7); SB_STRUCT_MEMBER(int, n0f8); SB_STRUCT_MEMBER(int, n0f9); SB_STRUCT_MEMBER(int, n0fa); SB_STRUCT_MEMBER(int, n0fb); SB_STRUCT_MEMBER(int, n0fc); SB_STRUCT_MEMBER(int, n0fd); SB_STRUCT_MEMBER(int, n0fe); SB_STRUCT_MEMBER(int, n0ff);
#endif
SB_STRUCT_END(compile_stress_t);


////
#include <iostream>
#include <common/include/sb_common.h>
SB_EXPORT_TYPE int __stdcall test_hash([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	using sbLibX::sb_get;
	// This is not legal since custom_class_t has virtual members and custom weird stuffs:
	//	custom_class_t custom = {
	//		.name = "sbCustom",
	//		//.anonymous_vec4f = default; // uninitialized
	//		.config = {
	//			.name        = "sbCustomConfig",
	//			.className   = "sbCustomClass",
	//			//.moduleFlags = 0xF, // initialized to 0
	//		},
	//		//.which_was_my_config_again = default; // initialized to ".config";
	//	};
	// so we have to be less elegant...
	custom_class_t custom;
	custom.name = "sbCustom"_xhash64;
	//custom.anonymous_vec4f; // uninitialized
	// but module_config_t is a nice plain data struct, so
	custom.config = {
		.name        = "sbCustomConfig",
		.className   = "sbCustomClass",
		//.moduleFlags = 0xF, // initialized to 0
	};
	//custom.which_was_my_config_again = default; // initialized to ".config";

	std::cout << module_config_t::struct_hash
		<< " version " << get<0>(module_config_t::options)
		<< "." << get<1>(module_config_t::options)
		<< "." << get<2>(module_config_t::options)
		<< std::endl;
	auto& config = sb_get<".config"_xhash64>(custom);

	//std::cout << "Enter config name: ";
	std::array<char, 256> name = { "(...)" };
	//std::cin.getline(name.data(), name.size());
	sb_get<".name"_xhash64>(config) = std::string_view(name.data());

	std::cout << ".name"_xhash64 << ": " << sb_get<".name"_xhash64>(config) << "\n";
	std::cout << ".className"_xhash64 << ": " << sb_get<".className"_xhash64>(config) << "\n";
	//std::cout << ".anonymous_vec4f"_xhash64 << ": " << get<".anonymous_vec4f"_xhash64>(config) << "\n";
	//std::cout << ".anonymous_vec4f"_xhash64 << ": " << sb_get(config, ".anonymous_vec4f"_xhash64) << "\n";
	std::cout << ".moduleFlags"_xhash64 << ": " << sb_get<".moduleFlags"_xhash64>(config) << "\n";

	//std::cout << "Also, please " << std::string_view(custom.try_not_to_hash)
	//	<< "(do_weird_impl(custom).buffer) : "
	//	<< std::boolalpha << (bool)custom.try_not_to(
	//			sb_get<".buffer"_xhash64>(
	//				custom.do_weird_impl(custom))[0] |= 2 )
	//	<< "\n";
	//std::cout << "Also, please " << std::string_view(custom.try_not_to_hash)
	//	<< "(do_weird_impl(custom).buffer) : "
	//	<< std::boolalpha << (bool)custom.try_not_to(
	//			sb_get<".buffer"_xhash64>(
	//				custom.do_weird_impl(custom))[0] |= 2 )
	//	<< "\n";
	//std::cout.flush();

	//compile_stress_t test;
	auto& test = custom;
	std::cout << "\ndynamic data information for " << test.struct_hash << "\n";
	for (auto data_info : test)
	{
		auto key_info = custom.key_info[data_info.key_index];
		std::cout
			<< key_info.type_hash.name << " " << key_info.name_hash.name
			<< "\n\toffset=" << data_info.offset
			<< ";   size=" << data_info.size
			<< ";   align=" << data_info.align
			<< "\n";
	}
	std::cout.flush();
	return 0;
}
