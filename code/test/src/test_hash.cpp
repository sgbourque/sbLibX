#include <common/include/sb_hash.h>
#include <common/include/sb_structured_buffer.h>

namespace SB { namespace LibX
{
////	So the idea here is to have a key mapping to a hash;
////	That hash will be used through the key_info_array which maps it to its data_info.
//struct key_info_t;
//struct data_info_t;
//template<size_t _DIMENSION_> using key_info_array  = std::array<key_info_t, _DIMENSION_>;
//template<size_t _DIMENSION_> using data_info_array = std::array<data_info_t, _DIMENSION_>;

//struct key_info_t
//{		//	hashed_string_t name_hash;//	hashed_string_t	type_hash;//	size_t	data_index;		//};
//struct data_info_t
//{		//	size_t offset;//	size_t size;//	size_t align;//	size_t key_index;		//};

struct raw_data_t; // (currently unused: for future testing with dynamic data)
struct raw_data_t
{
	void* data;
	size_t size;
	xhash_t type;
};

namespace StructuredBuffer
{
//	const STRUCTURED_BUFFER_NAME::data_info_array_t STRUCTURED_BUFFER_NAME::data_info = sbLibX::StructuredBuffer::build_data_info<type_t>();\
//	const STRUCTURED_BUFFER_NAME::key_info_array_t STRUCTURED_BUFFER_NAME::key_info = sbLibX::StructuredBuffer::build_key_info<type_t>();
} // StructuredBuffer

//template<
//	typename info_traits_t,
//	struct _DATAINFO_CONTAINER_ = std::vector<>,
//	struct _KEYINFO_MAP_ = std::unordered_set<key_info<typename info_traits_t::hash_traits_t, type_info_hash_traits_t>, size_t>,
//>
//struct dynamic_buffer_descriptor
//{
//
//	//template<
//	//	class Key,
//	//	class Hash = std::hash<Key>,
//	//	class KeyEqual = std::equal_to<Key>,
//	//	class Allocator = std::allocator<Key>
//	//> class unordered_set;
//	using key_info_t = key_info<xhash_traits_t, type_info_hash_traits_t>;
//	std::unordered_set<key_info_t, []
////	using data_block_t = ;
////	//template<typename> using allocator = _ALLOCATOR_;
////	//std::vector<uint8_t,
//};

//	template<hash_t HASH, typename _IMPLEMENTATION_>
//auto sb_get(_IMPLEMENTATION_* buffer)
//{
//	using type_t = typename _IMPLEMENTATION_::template data_traits<HASH, _IMPLEMENTATION_>::type_t;
//	return reinterpret_cast<type_t*>( reinterpret_cast<uint8_t*>(buffer) + _IMPLEMENTATION_::data_traits<HASH>::offset );
//}
//template<hash_t HASH, typename _IMPLEMENTATION_> auto& sb_get(_IMPLEMENTATION_& buffer) { return *sb_get<HASH>(&buffer); }
//template<hash_t HASH, typename _IMPLEMENTATION_> auto&& sb_get(_IMPLEMENTATION_&& buffer) { return *sb_get<HASH>(&buffer); }
//
//
//// Let's assume for now that we have the following :
//// 1. We have an array of data_info on which we can iterate;
//// 2. We have an acceleration table that maps a hash to either a corresponding entry of a buffer or the invalid entry
////    (which is unique up to equivalence in which );
//
////	template<typename RETURN_TYPE, typename _IMPLEMENTATION_>
////RETURN_TYPE sb_get([[maybe_unused]] _IMPLEMENTATION_* buffer, [[maybe_unused]] hash_t hash, RETURN_TYPE value = {})
////{
////	// TODO : don't forget to assert on struct size & alignment requirement
////	return value;
////}
////template<typename RETURN_TYPE, typename _IMPLEMENTATION_> RETURN_TYPE sb_get(_IMPLEMENTATION_& buffer, hash_t hash, RETURN_TYPE value = {}) { return sb_get(&buffer, hash, value); }
////
////	template<typename _IMPLEMENTATION_>
////raw_data_t sb_get([[maybe_unused]] _IMPLEMENTATION_* buffer, [[maybe_unused]] hash_t hash)
////{
////	return raw_data_t{};
////}
////template<typename _IMPLEMENTATION_> auto sb_get(_IMPLEMENTATION_& buffer, hash_t hash) { return sb_get(&buffer, hash); }
//
}} // namespace sbLibX


#include <xmmintrin.h>

////
SB_STRUCT_BEGIN(module_config_t, 0, 1, -1)
	SB_STRUCT_MEMBER(sbLibX::xhash_string_view_t, name)        = "(...)";
	SB_STRUCT_MEMBER(sbLibX::xhash_string_view_t, className)   = "";
	SB_STRUCT_MEMBER(uint64_t,            moduleFlags) = 0; // or whatever...
SB_STRUCT_END(module_config_t)


////
#include <iostream>
#pragma warning(disable:4324)
	// warning C4324: 'custom_class_t' : la structure a été remplie en raison du spécificateur d'alignement
	// because of that really weird __m128 in the middle of nowhere (ideally: try aligning your stuff)...
	// (BTW: also tried to see what happens with template... possible but not easy to support).
SB_STRUCT_BEGIN(custom_class_t, "my super hyper custom class!!", -1, "booo")
	SB_STRUCT_MEMBER(sbLibX::xhash_string_view_t, name)      = "hash";
	__m128 anonymous_vec4f; // "anonymous" data can be added anywhere
	SB_STRUCT_MEMBER(module_config_t, config) = {.name = "\0\0\0"};
	SB_STRUCT_MEMBER(uint8_t[64 * 1024], buffer) = {};
	SB_STRUCT_MEMBER(uint8_t, a_buffer_with_byte_type_instead_of_carray_type)[0x78] = {};
	virtual ~custom_class_t() { /*std::cout << "bye from " << (config.className.name[0] ? std::string(config.className) + "/" : "") << config.name << "/" << name; "\n";*/ }

	// TODO: This will not work as I can't (yet) detect static data to process differently.
	// Also, I will have to deal with (virtual/non-virtual) functions (functions should be easier via enable_if).
	//SB_STRUCT_MEMBER(long const int unsigned volatile long, try_not_to, inline static constexpr)(int n) { return {n != 3}; }
	//SB_STRUCT_MEMBER(custom_class_t, do_weird_impl, virtual)(custom_class_t other) { other.config = {name, config.className, buffer[0] ^= 1}; return other; }
SB_STRUCT_END(custom_class_t)
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
SB_STRUCT_END(compile_stress_t)



#if 0 // Just an overview of a typical structured_buffer instance
	struct struct_buf {
		using type_t = struct_buf;
		using hash_traits_t = /*_TRAITS_;*//**/sbLibX::xhash_traits_t;/**/
		using hash_string_view_t = sbLibX::xhash_string_view<hash_traits_t>;
		using hash_t = typename hash_traits_t::value_t;
		//static constexpr auto options = std::make_tuple({});
		inline static constexpr hash_string_view_t struct_hash = "struct_buf" " " "" "";
		
		template<hash_t, typename IMPL = type_t> struct data_traits;
		template<size_t, typename IMPL = type_t> struct key_traits;
		static inline constexpr size_t kElementIdOffset = __COUNTER__;

//#define SB_STRUCT_MEMBER( TYPE, NAME, ... ) /* note that TYPE can't be hashed by name */
//		static inline constexpr size_t NAME##_id = __COUNTER__ - kElementIdOffset - 1;
//		static_assert(NAME##_id < 64, "Too many unique data members. Try using sub-buffers or split the buffer.");
//		static inline constexpr hash_string_view_t NAME##_hash = SB_MEMBER_ENTRY #NAME "";
//		template<typename IMPL> struct data_traits<hash_traits_t::hash(SB_MEMBER_ENTRY #NAME ""), IMPL>
//		{
//			using type_t = decltype(IMPL::NAME);
//			static inline constexpr size_t offset = offsetof(IMPL, NAME);
//			static inline constexpr size_t size   = sizeof(type_t);
//			using key_traits_t = key_traits<NAME##_id, IMPL>;
//		};
//		template<typename IMPL> struct key_traits<NAME##_id, IMPL>
//		{
//			static inline constexpr hash_string_view_t name_hash = IMPL::NAME##_hash;
//			using type_t = decltype(IMPL::NAME);/* TODO: type hash */
//			using data_traits_t = data_traits<name_hash, IMPL>;
//		};
//		using NAME##_base_t = TYPE; SB_STRUCTURED_BUFFER_MODIFIER(__VA_ARGS__) NAME##_base_t NAME /* Not supported (I will try to find a way) */

		static inline constexpr size_t kElementCount = __COUNTER__ - kElementIdOffset - 1;
		static inline constexpr size_t size() { return kElementCount; }
		using data_info_array_t = sbLibX::data_info_array<kElementCount>;
		static const data_info_array_t data_info;
		static inline constexpr auto begin() { return data_info.begin(); }
		static inline constexpr auto end() { return data_info.end(); }
		
		static inline constexpr size_t kKeyCount = (kElementCount + 1);/*sbLibX::StructuredBuffer::compute_optimal_key_count<type_t>();*/
		static inline constexpr size_t key_size() { return kKeyCount; }
		using key_info_array_t = sbLibX::key_info_array<hash_traits_t, kKeyCount>;
		static const key_info_array_t key_info;
		static inline constexpr auto key_begin() { return key_info.begin(); }
		static inline constexpr auto key_end() { return key_info.end(); }
	};
	const typename struct_buf::key_info_array_t struct_buf::key_info = sbLibX::StructuredBuffer::build_key_info<type_t>();
	const typename struct_buf::data_info_array_t struct_buf::data_info = sbLibX::StructuredBuffer::build_data_info<type_t>();
#endif


//#include <algorithm>
//#include <cstdint>


////
#include <iostream>
#include <common/include/sb_common.h>
SB_EXPORT_TYPE int __stdcall test_hash([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	using sbLibX::operator "" _xhash64;
	using sbLibX::get;
	using std::get;
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
	custom.name = "sbCustom";
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
	auto& config = get<".config"_xhash64>(custom);

	//std::cout << "Enter config name: ";
	std::array<char, 256> name = { "(...)" };
	//std::cin.getline(name.data(), name.size());
	get<".name"_xhash64>(config) = std::string_view(name.data());

	std::cout << ".name"_xhash64 << ": " << get<".name"_xhash64>(config) << "\n";
	std::cout << ".className"_xhash64 << ": " << get<".className"_xhash64>(config) << "\n";
	//std::cout << ".anonymous_vec4f"_xhash64 << ": " << get<".anonymous_vec4f"_xhash64>(config) << "\n";
	//std::cout << ".anonymous_vec4f"_xhash64 << ": " << get(config, ".anonymous_vec4f"_xhash64) << "\n";
	std::cout << ".moduleFlags"_xhash64 << ": " << get<".moduleFlags"_xhash64>(config) << "\n";

	//std::cout << "Also, please " << std::string_view(custom.try_not_to_hash)
	//	<< "(do_weird_impl(custom).buffer) : "
	//	<< std::boolalpha << (bool)custom.try_not_to(
	//			get<".buffer"_xhash64>(
	//				custom.do_weird_impl(custom))[0] |= 2 )
	//	<< "\n";
	//std::cout << "Also, please " << std::string_view(custom.try_not_to_hash)
	//	<< "(do_weird_impl(custom).buffer) : "
	//	<< std::boolalpha << (bool)custom.try_not_to(
	//			get<".buffer"_xhash64>(
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
			<< key_info.type_hash << " " << key_info.name_hash
			<< "\n\toffset=" << data_info.offset
			<< ";   size=" << data_info.size
			<< ";   align=" << data_info.align
			<< "\n";
	}
	std::cout.flush();
	return 0;
}
