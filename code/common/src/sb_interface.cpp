#include <common/include/sb_interface.h>
#include <string>
#include <variant>

#include <iostream>

namespace SB { namespace LibX
{

//	template< typename _VALUE_TYPE_, typename _HASH_TRAITS = hash_traits_t >
//struct hashed_key_value
//{
//	using value_t = _VALUE_TYPE_;
//	_VALUE_TYPE_
//};

	template<typename Implementation>
struct StructuredBuffer
{
	static UniqueChunk<uint8_t> GetData(Implementation& obj, hash_t hash_value);
};

#define SB_STRUCT_BEGIN(STRUCT_NAME)\
	struct STRUCT_NAME : StructuredBuffer<STRUCT_NAME> \
	{\
		template<hash_t _KEY_HASH_> struct traits;\
		template<hash_t _KEY_HASH_> static constexpr typename traits<_KEY_HASH_>::return_t get(STRUCT_NAME& obj);

#define SB_STRUCT_MEMBER(TYPE, NAME, RETURN_TYPE)\
	using NAME##_t = TYPE;\
	NAME##_t NAME;\
	template<> struct traits<"." #NAME ""_xhash64> { using data_t = NAME##_t; using return_t = RETURN_TYPE; };\
	template<> static constexpr typename traits<"." #NAME ""_xhash64>::return_t get<"." #NAME ""_xhash64>(my_type& obj) { return obj.##NAME; }
	//template<> static constexpr typename traits<"." #NAME ""_xhash64>::return_t offset("." #NAME ""_xhash64) { return obj.##NAME; }

#define SB_STRUCT_END(STRUCT_NAME)\
	};\
		template<hash_t _HASH_VALUE_>\
	auto get(STRUCT_NAME& obj) { return STRUCT_NAME::get<_HASH_VALUE_>(obj); }

	template<typename STRUCT_NAME>
auto get(STRUCT_NAME& obj, hash_t hash_value)
{
	return STRUCT_NAME::GetData(obj, hash_value);
}




SB_STRUCT_BEGIN(my_type)
SB_STRUCT_MEMBER(char[256], name, char*);
SB_STRUCT_MEMBER(uint32_t, version, uint32_t);
// unstructured properties below
// ...
SB_STRUCT_END(my_type);

template<>
UniqueChunk<uint8_t> StructuredBuffer<my_type>::GetData(my_type& obj, hash_t hash_value)
{
	// loose implementation for demonstration
	uint8_t* data = nullptr;
	size_t   dataSize = 0;
	switch (hash_value)
	{
	case ".name"_xhash64:
		data = reinterpret_cast<uint8_t*>(&obj.name[0]);
		dataSize = sizeof(typename my_type::name_t);
		break;
	case ".version"_xhash64:
		data = reinterpret_cast<uint8_t*>(&obj.version);
		dataSize = sizeof(my_type::version_t);
		break;
	}
	Chunk<uint8_t> chunk = {
		.datasize = dataSize,
		.data = data,
		.info = hash_value,
	};
	return chunk;
}



#define CSTRING_WRAPPER(_X)  #_X
#define WSTRING_WRAPPER(_X) L#_X
#define USTRING_WRAPPER(_X) u#_X
#define CSTR(_X) CSTRING_WRAPPER(_X)
#define WSTR(_X) WSTRING_WRAPPER(_X)
#define USTR(_X) USTRING_WRAPPER(_X)

#define SBLIBX_VERSION_MAJOR     0
#define SBLIBX_VERSION_MINOR     1
#define SBLIBX_VERSION_REVISION  (-1)
#define SBLIBX_VERSION_STRING CSTR(SBLIBX_VERSION_MAJOR) "." CSTR(SBLIBX_VERSION_MINOR) "." CSTR(SBLIBX_VERSION_REVISION) 

constexpr hashed_string_t SBLibX_MainInstance_ref = "SBLibX " SBLIBX_VERSION_STRING ""_xhash64;
constexpr BaseHandle SBLibX_MainInstance_handle{ SBLibX_MainInstance_ref.hash, };

auto GetNameString(BaseHandle resource)
{
	return std::string( GetName(resource) );
}

// Main interface
BaseHandle AcquireResource([[maybe_unused]] const char* name)
{
	// name = nullptr correspond to the LibX::system interface
	return name == nullptr ? SBLibX_MainInstance_handle : AcquireResource(SBLibX_MainInstance_handle, name);
}
BaseHandle AcquireResource([[maybe_unused]] BaseHandle handler)
{
	// increment handler refcount
	return handler;
}
BaseHandle AcquireResource([[maybe_unused]] BaseHandle handler, [[maybe_unused]] const char* name)
{
	// if name = nullptr, same as BaseHandle AcquireResource(handler)
	BaseHandle resource = AcquireResource( handler, xhash<hash_traits_t>(name) );
	// assert(resource == InvalidHandle || GetName(resource) == name );
	return resource;
}
BaseHandle AcquireResource([[maybe_unused]] BaseHandle handler, [[maybe_unused]] hash_t hash)
{
	// if hash = invalid_hash, same as BaseHandle AcquireResource(handler)
	my_type obj{
		.name = "test object",
	};
	auto name = get<".name"_xhash64>(obj);
	std::cout << name << std::endl;
	auto dynamic_value = get(obj, ".name"_xhash64);
	std::cout << dynamic_value.data << std::endl;
	return InvalidHandle;
}
BaseHandle AcquireResource([[maybe_unused]] BaseHandle handler, [[maybe_unused]] BaseHandle ref)
{
	// if resource = InvalidHandle, same as BaseHandle AcquireResource(handler)
	BaseHandle resource = AcquireResource(handler, get_value(ref));
	// assert(resource == InvalidHandle || GetName(resource) == name );
	return resource;
}

BaseHandle ReleaseResource([[maybe_unused]] BaseHandle resource) noexcept
{
	//
	return InvalidHandle;
}


}} // namespace SB::LibX
