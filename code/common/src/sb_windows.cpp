#include <common/include/sb_hash.h>
#include <common/include/sb_interface.h>

#include <algorithm>
#include <array>
#include <cstdint>

#define NOMINMAX
#include <windows.h>

//namespace sbLibX = SB::LibX;

//
// sbLibWin
//
namespace SB { namespace LibX
{
constexpr size_t alignup(size_t size, size_t align) { return ((size + align-1) / align) * align; }
#ifndef _offsetof
	#define _offsetof(s,m) ((::size_t) & reinterpret_cast<char const volatile&>((((s*)0)->m)))
#endif

using raw_data_t = uint8_t*;
class database_t;

///// generic
//	template<typename implementation_traits>
//constexpr size_t get_base_dimension( [[maybe_unused]] const typename implementation_traits::type_t* buffer = nullptr )
//{
//	constexpr bool isDynamic = (implementation_traits::kHashTableSize == 0);
//	static_assert(isDynamic || implementation_traits::kHashTableSize >= implementation_traits::kElementCount);
//	return implementation_traits::kHashTableSize;
//}
//	template<typename implementation_traits>
//constexpr size_t get_dimension( [[maybe_unused]] const typename implementation_traits::type_t* buffer = nullptr )
//{
//	return buffer ? buffer->size() : 0;
//}
//
///// This is the one you want to specialize
//	template<typename implementation_traits, typename HASH_TRAITS = typename implementation_traits::hash_traits>
//constexpr size_t get_index( const typename implementation_traits::type_t* buffer, typename HASH_TRAITS::value_t hash )
//{
//	constexpr size_t buffer_base_dimension = get_base_dimension(buffer);
//	if constexpr( buffer_base_dimension > 0 )
//		return (hash % buffer_base_dimension);
//	else
//		return get_dimension(buffer);
//}

struct data_info_t
{
	hashed_string_t name_hash;
	size_t   offset;
	size_t   size;
	size_t   align;
	hash_t   type_info;
};

	template<typename IMPLEMENTATION>
inline constexpr size_t getHashTableSize()
{
	return IMPLEMENTATION::kElementCount;
}

	template<typename IMPLEMENTATION>
struct build_hash_helper
{
	static inline constexpr size_t kHashCount = IMPLEMENTATION::kElementCount;
	static inline constexpr size_t kTableSize = IMPLEMENTATION::kHashTableSize;
	static_assert( kTableSize >= kHashCount, "Hash table size is too small" );
	using array_t = std::array<size_t, kTableSize>;

		template<size_t BEGIN = 0, size_t END = kTableSize>
	struct range_t
	{
			template<size_t... INDICES>
		static inline constexpr auto init()
		{
			constexpr hash_t data_hash = IMPLEMENTATION::get_hash<BEGIN>();
			using data_traits = typename IMPLEMENTATION::template data_traits<data_hash>;
			static_assert(data_traits::index == BEGIN, "error: data information is incompatible with the implementation.");
			constexpr size_t current = (data_hash % kTableSize);
			validate<current, INDICES...>();
			return range_t<BEGIN + 1, END>::init<INDICES..., current>();
		}
			template<size_t CURRENT, size_t FIRST, size_t... INDICES>
		static inline constexpr bool validate()
		{
			//static_assert(CURRENT != FIRST, "hash collision detected.");
			return CURRENT != FIRST && validate<CURRENT, INDICES...>();
		}
			template<size_t CURRENT>
		static inline constexpr bool validate()
		{
			return true;
		}
	};
		template<size_t END>
	struct range_t<END, END>
	{
			template<size_t... INDICES>
		static inline constexpr auto init()
		{
			return array_t{INDICES...};
		}
			template<size_t... INDICES>
		static inline constexpr bool validate()
		{
			return true;
		}
	};
};
	template<typename IMPLEMENTATION>
inline constexpr auto buildHashTable()
{
	return build_hash_helper<IMPLEMENTATION>::range_t<>::init();
}


//struct data_info_t
//{
//	size_t   offset;
//	hash_t   hash;
//	// flattened version of struct_info_t while keeping offset first in struct
//	size_t   size;
//	size_t   align;
//	hash_t   typedesc;
//};

//	template<typename _IMPLEMENTATION_>
//struct registry_traits {};
//
//	template<typename _STRUCTURE_TYPE_>
//_STRUCTURE_TYPE_ getData(const database_t* registry, uint64_t hash)
//{
//	return registry_traits<_STRUCTURE_TYPE_>::getData(registry, hash);
//}

//struct struct_info_t
//{
//	size_t   size;
//	size_t   align;
//	hash_t   typedesc;
//};
//
//struct data_info_t
//{
//	size_t   offset;
//	hash_t   hash;
//	// flattened version of struct_info_t while keeping offset first in struct
//	size_t   size;
//	size_t   align;
//	hash_t   typedesc;
//};
//
//	template<typename _IMPLEMENTATION_>
//struct data_type : data_info_t
//{
//	using type_t = _IMPLEMENTATION_;
//};
//
//
//	template<typename _IMPLEMENTATION_, typename traits>
//struct data_info_helper;
//
//struct validate_traits;


#if 1
//	template<typename _IMPLEMENTATION_, typename traits>
//struct structured_buffer_helper;


//struct init_traits;
//	template<typename _IMPLEMENTATION_>
//struct structured_buffer_helper<_IMPLEMENTATION_, init_traits>
//{
//	using type_t = _IMPLEMENTATION_;
//		template<typename ...TYPE_T>
//	static constexpr _IMPLEMENTATION_ init(TYPE_T... values) { return range_t<0, _IMPLEMENTATION_::kElementCount>::init(_IMPLEMENTATION_(), values...); }
//
//		template<size_t BEGIN, size_t END>
//	struct range_t {
//			template<typename FIRST, typename ...TYPE_T>
//		static constexpr _IMPLEMENTATION_ init(_IMPLEMENTATION_&& result, [[maybe_unused]] FIRST first, TYPE_T... values)
//		{ /*result.get<BEGIN>(first);*/ return range_t<BEGIN + 1, END>::init(result, values...); }
//			template<typename LAST>
//		static constexpr _IMPLEMENTATION_ init(_IMPLEMENTATION_&& result, [[maybe_unused]] LAST last)
//		{ /*;*/ return result; }
//	};
//		template<size_t END>
//	struct range_t<END, END> {
//		static constexpr _IMPLEMENTATION_ init(_IMPLEMENTATION_&& result)
//		{ return result; }
//	};
//};

// note that data_info[0] is just the invalid hash datainfo
#define SB_STRUCT_BEGIN( STRUCTURED_BUFFER_NAME, VERSION )\
	struct STRUCTURED_BUFFER_NAME {\
		using type_t = STRUCTURED_BUFFER_NAME;\
		using version_t = uint32_t;\
		static constexpr version_t version = VERSION;\
		inline static constexpr hashed_string_t struct_hash = #STRUCTURED_BUFFER_NAME " " #VERSION ""_xhash64;\
		\
		template<size_t index> static inline constexpr hash_t get_hash() { return 0; }\
		template<hash_t, typename IMPL = type_t> struct data_traits;\
		/*template<hash_t hash> static inline constexpr size_t get_index() { return data_traits<hash>::index; }*/\
		static inline constexpr size_t kElementIdOffset = __COUNTER__;
#define SB_STRUCT_MEMBER( TYPE, NAME )\
		static inline constexpr size_t NAME##_id = __COUNTER__ - kElementIdOffset - 1;\
		static inline constexpr hashed_string_t NAME##_hash = "." #NAME ""_xhash64;\
		template<> static inline constexpr hash_t get_hash<NAME##_id>() { return hash_t("." #NAME ""_xhash64); }\
		template<typename IMPL> struct data_traits<"." #NAME ""_xhash64, IMPL>\
		{\
			using type_t = TYPE;\
			static inline constexpr hashed_string_t name_hash = IMPL::NAME##_hash;\
			static inline constexpr size_t offset = offsetof(IMPL, NAME);\
			static inline constexpr size_t index = NAME##_id;\
		};\
		using NAME##_t = TYPE; NAME##_t NAME
#define SB_STRUCT_END( STRUCTURED_BUFFER_NAME )\
		static inline constexpr size_t kElementCount = __COUNTER__ - kElementIdOffset - 1;\
		static inline constexpr size_t kHashTableSize = kElementCount + kElementCount/2 - kElementCount/3 + kElementCount/5;/*getHashTableSize<type_t>();*/\
		static std::array<size_t, kHashTableSize> hash_table;/* = buildHashTable<type_t>();*/\
		static inline const data_info_t data_info[kElementCount];\
		static inline data_info_t get_info(hash_t hash) { size_t index = hash_table[(hash % kHashTableSize)]; data_info_t info = data_info[index]; return hash == info.name_hash ? info : data_info_t{}; }\
	};\
	std::array<size_t, STRUCTURED_BUFFER_NAME::kHashTableSize> STRUCTURED_BUFFER_NAME::hash_table = buildHashTable<STRUCTURED_BUFFER_NAME>();\

#else
// note that data_info[0] is just the invalid hash datainfo
#define SB_STRUCT_BEGIN( STRUCTURED_BUFFER_NAME, VERSION )\
	struct STRUCTURED_BUFFER_NAME : structured_buffer_impl<STRUCTURED_BUFFER_NAME, VERSION> {\
		using type_t = STRUCTURED_BUFFER_NAME;\
		using version_t = uint32_t; static constexpr version_t version = VERSION;\
		inline static constexpr hashed_string struct_hash = #STRUCTURED_BUFFER_NAME " " #VERSION ""_xhash64;\
		template<size_t> static constexpr data_info_t getDataInfo();\
		template<size_t> static constexpr bool validateDataInfo();\
		template<hash_t> static constexpr data_info_t getDataInfoHash();\
		template<> static constexpr data_info_t getDataInfo<0>() { return data_type<raw_data_t>{}; }\
		template<> static constexpr bool validateDataInfo<0>() { return false; }\
		template<> static constexpr data_info_t getDataInfoHash<hash_traits_t::invalid_hash>() { return getDataInfo<0>(); }\
		template<hash_t, typename IMPL = type_t> struct type_traits;\
		template<typename... TYPE> STRUCTURED_BUFFER_NAME(TYPE...) {}\
		static inline constexpr size_t kMemberIdOffset = __COUNTER__;
#define SB_STRUCT_MEMBER( TYPE, NAME )\
		static inline constexpr size_t NAME##_id = __COUNTER__ - kMemberIdOffset;\
		template<> static constexpr data_info_t getDataInfo<NAME##_id>()\
		{\
			constexpr data_info_t previousData = getDataInfo<NAME##_id - 1>();\
			constexpr size_t offset = offsetof(type_t, NAME);\
			return data_type<TYPE>{\
				/*.offset   =*/ offset,\
				/*.hash     =*/ hash_t("." #NAME ""_xhash64),\
				/*.size     =*/ sizeof(NAME##_t),\
				/*.align    =*/ alignof(NAME##_t),\
				/*.typedesc =*/ hash_t(#TYPE ""_xhash64),\
			};\
		};\
		template<> static constexpr bool validateDataInfo<NAME##_id>()\
		{\
			constexpr data_info_t info = getDataInfo<NAME##_id>();\
			constexpr bool isValid = /*getIndex(info.hash) == NAME##_id &&*/ offsetof( type_t, NAME ) == info.offset;\
			static_assert( isValid || (info.offset != 0), "cannot have virtual table with structured_buffer" );\
			static_assert( isValid || (info.offset == 0), "struct offset mismatch for var '" #NAME "' of type '" #TYPE "'." );\
			return	offsetof( type_t, NAME ) == info.offset;\
		}\
		template<> static constexpr data_info_t getDataInfoHash<hash_t("." #NAME ""_xhash64)>() { return getDataInfo<NAME##_id>(); }\
		template<typename IMPL> struct type_traits<hash_t("." #NAME ""_xhash64), IMPL> { using type_t = TYPE; static inline constexpr size_t offset = IMPL::getDataInfo<NAME##_id>().offset; };\
		using NAME##_t = TYPE; NAME##_t NAME
#define SB_STRUCT_END( STRUCTURED_BUFFER_NAME )\
		static inline constexpr size_t kMemberCount = __COUNTER__ - kMemberIdOffset - 1;\
		using data_info_array_t = std::array<data_info_t, kMemberCount + 1>;\
		static inline data_info_array_t data_info = data_info_helper<STRUCTURED_BUFFER_NAME, data_info_t>::getDataInfo();\
		/*hash_table / getIndex*/;\
		/*static constexpr size_t getIndex(hash_t hash) { return (hash % kMemberCount) + 1; }*/\
		/*static data_info_t getDataInfo(hash_t hash) { data_info_t info = data_info[getIndex(hash)]; return info.hash == hash ? info : data_info_t{}; }*/\
	};\
	static_assert( data_info_helper<STRUCTURED_BUFFER_NAME, validate_traits>::validateDataInfo(), "structured_buffer validation failed." );

	template<typename _IMPLEMENTATION_>
struct data_info_helper<_IMPLEMENTATION_, data_info_t>
{
	using type_t = _IMPLEMENTATION_;
	using data_info_array_t = typename type_t::data_info_array_t;

	static constexpr data_info_array_t getDataInfo() { return range_t<0, data_info_array_t().size()>::getDataInfo(); }

		template<size_t BEGIN, size_t END>
	struct range_t {
			template<typename ...TYPE_T>
		static constexpr data_info_array_t getDataInfo(TYPE_T... data_info)
		{ return range_t<BEGIN, END - 1>::getDataInfo( type_t::getDataInfo<END - 1>(), data_info... ); }
	};
		template<size_t BEGIN>
	struct range_t<BEGIN, BEGIN> {
			template<typename ...TYPE_T>
		static constexpr data_info_array_t getDataInfo(TYPE_T... data_info)
		{ return data_info_array_t{ data_info... }; }
	};
};
	template<>
struct data_info_helper<raw_data_t, data_info_t> {
	using type_t = raw_data_t;
	static constexpr void getDataInfo() {}
};

	template<typename _IMPLEMENTATION_>
struct data_info_helper<_IMPLEMENTATION_, validate_traits>
{
	using type_t = _IMPLEMENTATION_;
	using data_info_array_t = typename type_t::data_info_array_t;

	static constexpr bool validateDataInfo() { return range_t<0, data_info_array_t().size()>::validateDataInfo(); }

		template<size_t BEGIN, size_t END>
	struct range_t {
		static constexpr bool validateDataInfo()
		{ return type_t::validateDataInfo<END - 1>() && range_t<BEGIN, END - 1>::validateDataInfo(); }
	};
		template<>
	struct range_t<0, 1> { // ignore the invalid_hash data info
		static constexpr bool validateDataInfo()
		{ return true; }
	};
};
	template<>
struct data_info_helper<raw_data_t, validate_traits> {
	using type_t = raw_data_t;
	static constexpr void validateDataInfo() {}
};
#endif


	template<hash_t HASH, typename _IMPLEMENTATION_>
auto get(_IMPLEMENTATION_* buffer)
{
	using type_t = typename _IMPLEMENTATION_::template data_traits<HASH, _IMPLEMENTATION_>::type_t;
	return reinterpret_cast<const type_t*>( reinterpret_cast<const uint8_t*>(buffer) + _IMPLEMENTATION_::data_traits<HASH>::offset );
}
template<hash_t HASH, typename _IMPLEMENTATION_> auto get(_IMPLEMENTATION_& buffer) { return *get<HASH>(&buffer); }

	template<typename RETURN_TYPE, typename _IMPLEMENTATION_>
RETURN_TYPE get([[maybe_unused]] _IMPLEMENTATION_* buffer, [[maybe_unused]] hash_t hash, RETURN_TYPE value = {})
{
	return value;
}
template<typename RETURN_TYPE, typename _IMPLEMENTATION_> RETURN_TYPE get(_IMPLEMENTATION_& buffer, hash_t hash, RETURN_TYPE value = {}) { return get(&buffer, hash, value); }


	//// generic linear (slow) implementation (can be used without any acceleration structure)
	//	template<typename STRUCTURED_BUFFER = structured_buffer>
	//Chunk<uint8_t*> getData(const Chunk<STRUCTURED_BUFFER> bufferInfo, hash_t hash)
	//{
	//	// TODO : assert!
	//	// TODO : should not need to loop over all data_info!
	//	Chunk<uint8_t*> data;
	//	for ( const auto& data_info : STRUCTURED_BUFFER::data_info)
	//		if (data_info.hash == hash)
	//			data = {
	//				.data     = reinterpret_cast<uint8_t*>(bufferInfo.data) + data_info.offset,
	//				.dataSize = data_info.size,
	//				.info     = hash,
	//		}
	//	return data;
	//}



	//	template<typename IMPLEMENTATION, typename ExpectedType>
	//constexpr ExpectedType get(structured_buffer<IMPLEMENTATION>* data, hash_t hash)
	//{
	//	return reinterpret_cast<ExpectedType>(reinterpret_cast<uint8_t*>(data) + IMPLEMENTATION::data_info[IMPLEMENTATION::get_index(hash)].offset);
	//}

	//STRUCTURED_BUFFER_NAME::data_info_array_t STRUCTURED_BUFFER_NAME::data_info = {STRUCTURED_BUFFER_NAME::getDataInfo<0>(), STRUCTURED_BUFFER_NAME::getDataInfo<1>()};\


	//static STRUCTURED_BUFFER_NAME::data_info_array_t STRUCTURED_BUFFER_NAME::data_info = unpack_data_info<STRUCTURED_BUFFER_NAME>();

	//	template<typename STRUCTURE_TYPE, size_t MEMBER_ID, size_t MEMBER_END>
	//constexpr void unpack_data_info(const data_info_t* datainfo)
	//{
	//	hash_t hash = STRUCTURE_TYPE::getHash<MEMBER_ID>();
	//	hash_t table_index = (hash % sizeof());
	//	datainfo[ % ]
	//	return data;
	//}
	//	template<typename STRUCTURE_TYPE, size_t MEMBER_END>
	//constexpr void unpack_data_info<STRUCTURE_TYPE, 0, MEMBER_END>()
	//{
	//}

	//	template<>\
	//struct registry_traits<STRUCTURED_BUFFER_NAME>\
	//{\
	//};
	//	static inline uint32_t memberCount = 0/*count_members<STRUCTURED_BUFFER_NAME>::value;*/
		//static STRUCTURED_BUFFER_NAME getData([[maybe_unused]] database_t* registry, uint64_t hash )
		//{
		//	STRUCTURED_BUFFER_NAME data2{};
		//	return data2;
		//}

namespace Windows
{
#if defined(WIN32)
#define SBWIN SB_SUPPORTED
#endif

#if defined(SBWIN) && defined(UNICODE)
	using system_char_t = wchar_t;
	#define SYSTEM_STRING(X) L##X
#else
	using system_char_t = char;
	#define SYSTEM_STRING(X) X
#endif
using system_string_t = system_char_t*;
#define STR(X) SYSTEM_STRING(X)

SB_STRUCT_BEGIN(ApplicationConfiguration, 1)
	SB_STRUCT_MEMBER(system_char_t[128], name) = STR("sbApplication (anonymous)");
	SB_STRUCT_MEMBER(system_char_t[128], className) = STR("sbGenericWindow");
	SB_STRUCT_MEMBER(uint64_t,           windowFlags) = 0;
SB_STRUCT_END(ApplicationConfiguration);


//SB_STRUCT_BEGIN(MixedStruct, ~0u)
//	int someUnknownInt4[4];
//	SB_STRUCT_MEMBER(hashed_string_t, name) = "some hash/string pair"_xhash64;
//	char someUnknownChar2[2];
//	SB_STRUCT_MEMBER(uint64_t,        customDataFlags) = 0;
//	float someUnknownFloat[3];
//	explicit MixedStruct(int);
//	virtual ~MixedStruct() {}
//SB_STRUCT_END(MixedStruct);
//
//SB_STRUCT_BEGIN(PackedMixedData, ~0u)
//	// now data is packed
//	SB_STRUCT_MEMBER(hashed_string_t, name) = "some hash/string pair"_xhash64;
//	SB_STRUCT_MEMBER(uint64_t,        customDataFlags) = 0;
//	int someUnknownInt4[4]; // ok
//	char someUnknownChar2[2]; // ok
//	float someUnknownFloat[3]; // ok
//	explicit PackedMixedData(int); // ok
//	~PackedMixedData() {} // no virtual table = no offset for the 1st member
//SB_STRUCT_END(PackedMixedData);


//SB_STRUCT_BEGIN(testStruct, ~0u)
//	SB_STRUCT_MEMBER(int, _x);
//	SB_STRUCT_MEMBER(int, _y);
//	SB_STRUCT_MEMBER(int, _z);
//	SB_STRUCT_MEMBER(int, _w);
//	SB_STRUCT_MEMBER(int, _v0);
//	SB_STRUCT_MEMBER(int, _v1);
//	SB_STRUCT_MEMBER(int, _v2);
//	SB_STRUCT_MEMBER(int, _v3);
//	SB_STRUCT_MEMBER(int, _v4);
//	SB_STRUCT_MEMBER(int, _v5);
//	SB_STRUCT_MEMBER(int, _v6);
//	SB_STRUCT_MEMBER(int, _v7);
//	SB_STRUCT_MEMBER(int, _v8);
//	SB_STRUCT_MEMBER(int, _v9);
//	SB_STRUCT_MEMBER(int, _v10);
//	SB_STRUCT_MEMBER(int, _v11);
//	SB_STRUCT_MEMBER(int, _v12);
//SB_STRUCT_END(testStruct);

//
//
//// library exports
//int initialize(const database_t* registry)
//{
//	auto config = GetData<ApplicationConfiguration>(registry, "sbWinConfig"_xhash64);
//	const auto& name = config.name;
//	const auto& className = config.className;
//	HINSTANCE hinstance = GetModuleHandleW(NULL);
//
//	WNDCLASSEXW windowClassProperties = {
//		.cbSize = sizeof(WNDCLASSEXW),
//		.style = CS_HREDRAW | CS_VREDRAW,
//		.lpfnWndProc = MainWndProc,
//		.cbClsExtra = 0,
//		.cbWndExtra = 0,
//		.hInstance = GetModuleHandleW(NULL),
//		.hIcon = LoadIcon(NULL, IDI_WINLOGO),
//		.hCursor = LoadCursor(NULL, IDC_CROSS),
//		.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH),
//		.lpszMenuName = nullptr,
//		.lpszClassName = className,
//		hIconSm = (HICON)LoadImage(hinstance, MAKEINTRESOURCE(5), IMAGE_ICON,
//			GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR)
//};
//	if (!RegisterClassExW(&wcx))
//		return -1;
//
//	[[maybe_unused]] HWND hwnd = CreateWindowExW(
//		WS_EX_NOREDIRECTIONBITMAP,
//		className, name,
//		WS_OVERLAPPEDWINDOW /*WS_POPUP*/ | WS_VISIBLE,
//		CW_USEDEFAULT, CW_USEDEFAULT,
//		CW_USEDEFAULT, CW_USEDEFAULT,
//		nullptr, nullptr, hinstance, nullptr);
//
//	//const auto name  = get<system_string_t>(registry, "applicationName"_xhash64);
//	//const auto flags = get<WindowFlags>(registry, "mainWindowFlags"_xhash64);
//	//system_string name = get<system_string>(registry, "name"_xhash64);
//
//}
//#endif
//
//
//
//#if 0
//void register_class()
//{
//	const auto className = L"SBMainWinClass";
//	HINSTANCE hinstance = GetModuleHandleW(NULL);
//
//	WNDCLASSEXW wcx;
//
//	// Fill in the window class structure with parameters 
//	// that describe the main window. 
//
//	wcx.cbSize = sizeof(wcx);          // size of structure 
//	wcx.style = CS_HREDRAW | CS_VREDRAW; // redraw if size changes 
//	wcx.lpfnWndProc = MainWndProc;       // points to window procedure 
//	wcx.cbClsExtra = 0;                  // no extra class memory 
//	wcx.cbWndExtra = 0;                  // no extra window memory 
//	wcx.hInstance = hinstance;           // handle to instance 
//	wcx.hIcon = LoadIcon(NULL,
//		IDI_WINLOGO);               // predefined app. icon 
//	wcx.hCursor = LoadCursor(NULL,
//		IDC_CROSS);                     // predefined arrow 
//	wcx.hbrBackground = (HBRUSH)GetStockObject(
//		DKGRAY_BRUSH);                   // white background brush 
//	wcx.lpszMenuName = nullptr;       // name of menu resource 
//	wcx.lpszClassName = className;    // name of window class 
//	wcx.hIconSm = (HICON)LoadImage(hinstance,  // small class icon 
//		MAKEINTRESOURCE(5),
//		IMAGE_ICON,
//		GetSystemMetrics(SM_CXSMICON),
//		GetSystemMetrics(SM_CYSMICON),
//		LR_DEFAULTCOLOR);
//
//	if (!RegisterClassExW(&wcx))
//		return -1;
//
//
//	[[maybe_unused]] HWND hwnd = CreateWindowExW(
//		WS_EX_NOREDIRECTIONBITMAP,
//		className, L"SBLibX Test Win32",
//		WS_OVERLAPPEDWINDOW /*WS_POPUP*/ | WS_VISIBLE,
//		CW_USEDEFAULT, CW_USEDEFAULT,
//		CW_USEDEFAULT, CW_USEDEFAULT,
//		nullptr, nullptr, hinstance, nullptr);
//}
//#endif
}}} // namespace SB::LibX::Windows
using namespace SB::LibX;
#include <common/include/sb_common.h>

SB_EXPORT_TYPE int __stdcall sbInitialize([[maybe_unused]] const database_t* registry)
{
	return 0;
}
SB_EXPORT_TYPE int __stdcall sbRun([[maybe_unused]] const database_t* registry)
{
	return 0;
}
SB_EXPORT_TYPE int __stdcall sbShutdown([[maybe_unused]] const database_t* registry)
{
	return 0;
}

#include <dev/include/sb_dev_debug.h>
#include <iostream>
#include <cstddef>
//#ifndef _offsetof
//#define _offsetof(s,m) ((::size_t) & reinterpret_cast<char const volatile&>((((s*)0)->m)))
//#endif

std::ostream& operator << (std::ostream& os, hashed_string_t hashed)
{
	return os << "'" << hashed.name << "' (0x" << std::hex << hashed.hash << ")";
}
std::ostream& operator << (std::ostream& os, const wchar_t* wstr)
{
	// poor-man's implementation...
	while (wstr && *wstr)
		os << (char)*(wstr++);
	return os;
}

//	template<typename STRUCTURED_BUFFER, hash_t HASH_VALUE>
//auto get<HASH_VALUE>(const STRUCTURED_BUFFER* buffer)
//{
//	using type_t = typename STRUCTURED_BUFFER::template type_traits<HASH_VALUE, STRUCTURED_BUFFER>::type_t;
//	return reinterpret_cast<const type_t*>(nullptr);
//}

using config_t = sbLibX::Windows::ApplicationConfiguration;
template<hash_t HASH> using config_traits = config_t::data_traits<HASH>;

SB_EXPORT_TYPE int __stdcall main([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	//sbLibX::Debug::Console debugConsole; debugConsole.RedirectStdIO();

#if 0
	std::cout << "\nApplicationConfiguration:";
	std::cout << "\nstatic binding\n";
	enum : size_t
	{
		kNameHash   = hash_t(".name"_xhash64),
		kNameOffset = config_traits<".name"_xhash64>::offset,
		kNameSize   = sizeof(config_traits<kNameHash>::type_t),

		kClassNameHash   = hash_t(".className"_xhash64),
		kClassNameOffset = config_traits<kClassNameHash>::offset,
		kClassNameSize   = sizeof(config_traits<".className"_xhash64>::type_t),

		kWindowFlagsHash   = hash_t(".windowFlags"_xhash64),
		kWindowFlagsOffset = config_traits<".windowFlags"_xhash64>::offset,
		kWindowFlagsSize   = sizeof(config_traits<".windowFlags"_xhash64>::type_t),
	};
	std::cout << kNameHash << ": offset = " << kNameOffset << "; size = " << kNameSize << std::endl;
	std::cout << kClassNameHash << ": offset = " << kClassNameOffset << "; size = " << kClassNameSize << std::endl;
	std::cout << kWindowFlagsHash << ": offset = " << kWindowFlagsOffset << "; size = " << kWindowFlagsSize << std::endl;

	//std::cout << "\ndynamic binding\n";
	//for ( const auto& data_info : config_t::data_info )
	//	if (data_info.hash) std::cout << data_info.hash << ": offset = " << data_info.offset << "; size = " << data_info.size << std::endl;

	#define COUT_MEMBER(STRUCT_NAME, MEMBER) do { std::cout << "." #MEMBER ""_xhash64 << ": offset = " << _offsetof(STRUCT_NAME, MEMBER) << "; size = " << sizeof(STRUCT_NAME::MEMBER) << std::endl; } while(false)
	std::cout << "\ndefinition\n";
	COUT_MEMBER(config_t, name);
	COUT_MEMBER(config_t, className);
	COUT_MEMBER(config_t, windowFlags);

	//using mixed_t = sbLibX::Windows::MixedStruct;
	//std::cout << "\nMixedStruct:";
	//std::cout << "\ndynamic binding (incomplete)\n";
	//for (const auto& data_info : mixed_t::data_info)
	//	if (data_info.hash) std::cout << data_info.hash << ": offset = " << data_info.offset << "; size = " << data_info.size << std::endl;

	//std::cout << "\ndefinition\n";
	//COUT_MEMBER(mixed_t, someUnknownInt4);
	//COUT_MEMBER(mixed_t, name);
	//COUT_MEMBER(mixed_t, someUnknownChar2);
	//COUT_MEMBER(mixed_t, customDataFlags);
	//COUT_MEMBER(mixed_t, someUnknownFloat);

	////using _Ttype = typename tuple_element<_Index, tuple<_Types...>>::_Ttype;
	////return static_cast<const _Ttype&>(_Tuple)._Myfirst._Val;

	//std::tuple<int,float,const char*> test = { 1, 2.0f, "test" };
	//auto my_int = std::get<0>(test);
	//auto my_float = std::get<1>(test);
	//auto my_string = std::get<2>(test);
	//std::cout << "\n\n" << my_int << " " << my_float << " " << my_string << std::endl;
#endif

	std::cout << "\nTesting static bindings...\n";
	config_t local_config{ L"my application name", L"my class name", 0xFull };
	auto local_name = sbLibX::get<".name"_xhash64>(local_config);
	auto local_className = sbLibX::get(local_config, ".className"_xhash64, config_t::className_t{});
	////auto my_custom_flags = sbLibX::get<".customDataFlags"_xhash64>(local_config); // don't compile : customDataFlags is not defined
	//auto local_flags = sbLibX::get<".windowFlags"_xhash64>(local_config);
	std::cout << std::endl
		 << "\n" << config_traits<".name"_xhash64>::name_hash << ": " << local_name
		 << "\n" << config_traits<".className"_xhash64>::name_hash << ": " << local_className
	//	 << "\n" << config_traits<".windowFlags"_xhash64>::name << ": " << local_flags
		 << std::endl;
}
