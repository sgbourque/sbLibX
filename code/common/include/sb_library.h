#pragma once
#include <common/include/sb_common.h>

#include <cstdint>
#include <string_view>

#ifndef SB_WIN_EXPORT
#define SB_WIN_EXPORT SB_IMPORT_LIB
#endif

namespace SB { namespace LibX {

	template< typename __handle_type >
struct library_traits;

namespace DLL
{
struct SystemBinaryHeader;
using handle_t = SystemBinaryHeader*;

class export_function_iterator
{
public:
	SB_WIN_EXPORT export_function_iterator( const handle_t module, uint32_t hint = 0 );

	export_function_iterator operator ++() { if( nameTable < nameTableEnd ) ++nameTable; if( functionTable < functionTableEnd ) ++functionTable; return *this; }

	std::pair<const char*, const void*> operator *() const
	{
		const char* name = nameTable && nameTable < nameTableEnd ? rawModule + *nameTable : "";
		const void* address = functionTable && functionTable < functionTableEnd ? rawModule + *functionTable : nullptr;
		return { name, address };
	}

	friend bool operator ==( const export_function_iterator& a, const export_function_iterator& b )
	{
		return a.rawModule == b.rawModule && a.nameTable == b.nameTable && a.functionTable == b.functionTable;
	}
	friend bool operator !=( const export_function_iterator& a, const export_function_iterator& b )
	{
		return a.rawModule != b.rawModule || a.nameTable != b.nameTable || a.functionTable != b.functionTable;
	}

private:
	const uint32_t* functionTable;
	const uint32_t* functionTableEnd;
	const uint32_t* nameTable;
	const uint32_t* nameTableEnd;
	const char* rawModule;
};
}
	template<>
struct library_traits<DLL::handle_t>
{
	using handle_t = DLL::handle_t;
	using result_t = int32_t;
	using iterator_t = DLL::export_function_iterator;
};


class function_helper
{
public:
	function_helper( const void* address ) : function_address( address ) {}

		template< typename fct_type/*, typename = std::enable_if_t<std::is_function_v<fct_type>>*/ >
	operator fct_type() const { return reinterpret_cast<fct_type>( function_address ); }
protected:
	const void* function_address;
};

	template< typename __handle_type, typename traits = library_traits<__handle_type> >
struct unique_library
{
	using result_t = typename traits::result_t;
	using handle_t = typename traits::handle_t;
	using iterator_t = typename traits::iterator_t;

	explicit unique_library( const char* libName = nullptr ) { if( libName ) load( libName ); }
	explicit unique_library( const wchar_t* libName ) { if( libName ) load( libName ); }
	explicit unique_library( unique_library&& other ) { handle = other.Detach(); }
	explicit unique_library( const unique_library& other ) = delete;
	~unique_library() { Release(); }

	handle_t Detach() { return std::move( handle ); }
	void Release() { if( handle ) unload( std::move( handle ) ); handle = nullptr; }

	SB_WIN_EXPORT result_t load( const char* moduleName );
	SB_WIN_EXPORT result_t load( const wchar_t* moduleName );
	SB_WIN_EXPORT result_t unload( handle_t module );
	iterator_t begin() const { return iterator_t{ handle }; }
	iterator_t end() const { return iterator_t{ handle, ~0u }; }

	operator handle_t() const { return handle; }
	operator bool() const { return handle; }

	SB_WIN_EXPORT function_helper operator[]( std::string_view functionName ) const;

protected:
	handle_t handle = nullptr;
};

using unique_dll = unique_library<DLL::handle_t>;

}}

SB_PLATFORM_DEPENDS

//#if !defined(SB_LIBPLATFORM_INTERNAL)
//SB_PLATFORM_DEPENDS()
//#endif

//#if !defined(SB_LIBPLATFORM_INTERNAL)
//#if defined(SBDEBUG)
//	#pragma comment(lib, "sbWindows_static_x64_debug.lib")
//#elif defined(SBRELEASE)
//	#pragma comment(lib, "sbWindows_static_x64_release.lib")
//#endif
//#endif
