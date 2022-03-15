#pragma once
#include <cstdint>
#if !defined(SBDEBUG) && _ITERATOR_DEBUG_LEVEL > 0
#error "Using <cstdint> with incompatible linker settings (linker error) : Iterator debug level shoud only be enabled with debug type targets."
#endif

namespace SB { namespace LibX {
// This must be binary-compatible with the COM interface, without requiring anything under "windows.h" and compiler's limitations involved
struct IUnknown
{
	using result_t   = int32_t;
	using refcount_t = int32_t;
	union GUID {
		struct CLSID { uint32_t _0; uint16_t _1; uint16_t _2; uint8_t _3[8]; } clsid;
		struct UID32 { uint32_t _0; uint32_t _1; uint32_t _2; uint32_t _3; } uid32;
		struct UID64 { uint64_t _0; uint64_t _1; } uid64;
		uint8_t uid[16];
	};
	virtual result_t SB_STDCALL QueryInterface(GUID riid, void** ppvObject) = 0;
	virtual int32_t  SB_STDCALL AddRef() = 0;
	virtual int32_t  SB_STDCALL Release() = 0;
};

// This is kind of half of a ComPtr (without the QueryInterface casting).
// If needing the interface casting, a new com_ptr template shall be inheriting from ref_ptr, completing the remaining half of ComPtr.
// However, the preferred approach for interface queries would be to create adaptor templates like:
//
//  ref_ptr<my_type> my_interface;
//	ref_ptr<query_type> new_interface = query_interface< query_type >( my_interface );
//
	template<typename Type>
struct ref_ptr
{
	//using type_t = std::remove_cvref_t<Type>; // <- should ideally use this but it might not be available on all compilers yet...
	using type_t = Type;

	type_t* Get() { return raw; }
	type_t* Get() const { return raw; }
	type_t** GetAddressOf() { return &raw; }
	type_t* const* GetAddressOf() const { return &raw; }

	operator bool() const { return Get() != nullptr; }

	auto Release() { auto refCount = raw ? raw->Release() : 0; raw = nullptr; return refCount; }
		template< typename interface_t = type_t >
	interface_t** ReleaseAndGetAddressOf() { Release(); return reinterpret_cast<interface_t**>( GetAddressOf() ); }

	type_t* operator ->() { return Get(); }
	type_t* operator ->() const { return Get(); }

	ref_ptr(type_t* ptr = nullptr) : raw(ptr) { if (raw) raw->AddRef(); }
	ref_ptr(const ref_ptr& ptr) : raw(ptr.raw) { if (raw) raw->AddRef(); }
	ref_ptr(ref_ptr&& ptr)  noexcept : raw(ptr.raw) { ptr.raw = nullptr; }

	const ref_ptr& operator =(type_t* ptr) { if (raw) raw->Release(); raw = ptr; if (raw) raw->AddRef(); return *this; }
	ref_ptr& operator =(ref_ptr&& refPtr) noexcept { raw = refPtr.raw; refPtr.raw = nullptr; return *this; }
	ref_ptr& operator =(const ref_ptr& refPtr) { if (raw) raw->Release(); raw = refPtr.Get(); if (raw) raw->AddRef(); return *this; }

	~ref_ptr() { if (raw) raw->Release(); raw = nullptr; }

protected:
	type_t* raw = nullptr;
};
}}
