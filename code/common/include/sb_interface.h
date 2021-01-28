#pragma once
#include <cstdint>
#include <algorithm>
#include <tuple>

#include <common/include/sb_hash.h>
#include <common/include/sb_encrypted_string.h>

// Please note that this is to be renamed eventually.
// It is not clear what a generic HandleBase means.
// Here this is the resource (data) static interface (POD).
// Eventually, the static and dynamic (com-like) interface
// will join at some point (co-existing together with a common
// "interface").

namespace SB { namespace LibX
{

struct Configuration
{
	xhash_string_view_t name;
	xhash_string_view_t version;
};

// Each lib should make sure any resource handle it references is safe.
	template< typename traits = xhash_traits_t >
struct HandleBase { using hash_t = typename traits::hash_t; using char_ptr_t = typename traits::char_ptr_t; union { hash_t hash; char_ptr_t ptr; } key; };

	template< typename _HANDLE_TYPE_ >
inline constexpr _HANDLE_TYPE_ get_value(_HANDLE_TYPE_ handle);
	template< typename _HANDLE_TYPE_ >
inline constexpr _HANDLE_TYPE_ get_pointer(_HANDLE_TYPE_ handle);

	template< typename _TRAITS_ >
inline constexpr auto get_value(HandleBase<_TRAITS_> handle)	{ return handle.key.hash; }
	template< typename _TRAITS_ >
inline constexpr auto get_pointer(HandleBase<_TRAITS_> handle) { return handle.key.ptr; }

using BaseHandle   = HandleBase<>;
constexpr BaseHandle InvalidHandle = BaseHandle{};

struct scoped_handle;


// Main interface
BaseHandle AcquireResource(const char* name = nullptr); // name = nullptr correspond to the LibX::system interface
BaseHandle AcquireResource(BaseHandle handler);
BaseHandle AcquireResource(BaseHandle handler, const char* name); // if name = nullptr, same as BaseHandle AcquireResource(handler)
BaseHandle AcquireResource(BaseHandle handler, xhash_t hash); // if hash = invalid_hash, same as BaseHandle AcquireResource(handler)
BaseHandle AcquireResource(BaseHandle handler, BaseHandle resource); // if resource = InvalidHandle, same as BaseHandle AcquireResource(handler)

BaseHandle ReleaseResource(BaseHandle resource) noexcept;
//inline BaseHandle GetLastError(BaseHandle resource) { return AcquireResource(resource, InvalidHandle); }

////
struct scoped_handle
{
		template<typename HandleType>
	scoped_handle(HandleType handler) { handle = AcquireResource(handler); }
		template<typename HandleType, typename ResourceType> explicit
	scoped_handle(HandleType handler, ResourceType resource) { AcquireResource(handler, resource); }

	scoped_handle(const scoped_handle&) = delete;
	scoped_handle(scoped_handle&& temp) noexcept { handle = std::move(temp.handle); }

	virtual ~scoped_handle() noexcept { ReleaseResource(handle); }

	operator BaseHandle() const noexcept { return handle; }

public:
	BaseHandle handle;
};
//inline constexpr hash_t get_hash(const scoped_handle& handle) { return get_value(handle.handle); }
//	template<>
//inline constexpr auto get_value(const scoped_handle& handle)	{ return get_value(handle.handle); }
//	template<>
//inline constexpr auto get_pointer(const scoped_handle& handle) { return get_pointer(handle.handle); }

inline auto GetName( BaseHandle handle )
{
#if 0 // static
	enum : xhash_t {
		name_hash = xhash_t(".name"_xhash64),
	};
#else // dynamic
	xhash_t name_hash = ".name"_xhash64;
#endif
	scoped_handle name = AcquireResource(handle, name_hash);
	return get_pointer(name.handle);
}


// Expect any data 
	template<typename data_t>
struct Chunk
{
	size_t     datasize = 0;
	data_t*    data = nullptr;
	BaseHandle info = InvalidHandle;
};

template<typename _RETURN_T_, typename ...> _RETURN_T_ get( BaseHandle );
template<> inline constexpr size_t get<size_t>( BaseHandle handle ) { return static_cast<size_t>( get_value(handle) ); }

	template<typename data_t>
struct UniqueChunk : Chunk<data_t>
{
	UniqueChunk(const Chunk<data_t>& chunkData) : Chunk<data_t>(chunkData) {}
	UniqueChunk(UniqueChunk&& rref) { std::swap(*this = UniqueChunk<data_t>{}, rref); }
	virtual ~UniqueChunk() { ReleaseResource(Chunk<data_t>::info); }
};
	template<typename data_t>
UniqueChunk<data_t> GetData(BaseHandle resource)
{
	enum : xhash_t {
		datasize_hash = xhash_t(".datasize"_xhash64),
		data_hash = xhash_t(".data"_xhash64),
		info_hash = xhash_t(".info"_xhash64),
	};
	// generic implementation : expect much better performance with specialised implementations.
	Chunk<data_t> chunk = {
		.datasize = get<size_t>( AcquireResource(resource, datasize_hash) ),
		.data = get<data_t*>( AcquireResource(resource, data_hash) ),
		.info = AcquireResource(resource, info_hash),
	};
	return UniqueChunk<data_t>(chunk);
}

	template<typename data_t, typename _KEYTYPE_>
UniqueChunk<data_t> GetData(BaseHandle handler, _KEYTYPE_ key)
{
    BaseHandle resource = AcquireResource(handler, key);
    return GetData<data_t>(resource); // let UniqueChunk release the resource through the .info property
}

}} // namespace SB::LibX
namespace sbLibX = SB::LibX;
