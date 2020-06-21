namespace SB { namespace LibX {
// This is kind of half of a ComPtr (without the QueryInterface casting).
// If needing the interface casting, a new com_ptr template shall be inheriting from ref_ptr, completing the remaining half of ComPtr.
	template<typename Type>
struct ref_ptr
{
	//using type_t = std::remove_cvref_t<Type>; // <- should ideally use this but it might not be available on all compilers yet...
	using type_t = Type;

	type_t* Get() { return raw; }
	type_t* Get() const { return raw; }
	type_t** GetAddressOf() { return &raw; }
	type_t* const* GetAddressOf() const { return &raw; }

	auto Release() { auto refCount = raw ? raw->Release() : 0; raw = nullptr; return refCount; }
	void** ReleaseAndGetAddressOf() { Release(); return reinterpret_cast<void**>( GetAddressOf() ); }

	type_t* operator ->() { return Get(); }
	type_t* operator ->() const { return Get(); }

	ref_ptr(type_t* ptr = nullptr) : raw(ptr) { if (raw) raw->AddRef(); }
	ref_ptr(const ref_ptr& ptr) : raw(ptr.raw) { if (raw) raw->AddRef(); }
	ref_ptr(ref_ptr&& ptr) : raw(ptr.raw) { ptr.raw = nullptr; }

	const ref_ptr& operator =(type_t* ptr) { if (raw) raw->Release(); raw = ptr; if (raw) raw->AddRef(); return *this; }
	ref_ptr& operator =(ref_ptr&& refPtr) { raw = refPtr.raw; refPtr.raw = nullptr; return *this; }
	ref_ptr& operator =(const ref_ptr& refPtr) { if (raw) raw->Release(); raw = refPtr.Get(); if (raw) raw->AddRef(); return *this; }

	~ref_ptr() { if (raw) raw->Release(); raw = nullptr; }
private:
	type_t* raw = nullptr;
};
}}
