#include <common/include/sb_interface.h>
#include <string>
#include <variant>

#include <dev/include/sb_dev_debug.h>

namespace SB { namespace LibX
{
#if 0
// A graph key looks like
//		path/subkey
// where path can be composed of subpaths connected with '/'s and subkey is any end point.
//
// A generic graph might not be directed, making it possibly cyclic.
// Propagation along a graph line is a visiting pattern. To avoid potential cyclic loops,
// an object hash table is built to avoid any recursive call. This has the drawback of also
// discard possibly desirable re-entrency. To allow recursive calling, we can use a re-entrency count.

#define SB_HASH_TRAITS
#if defined( SB_HASH_TRAITS )
	template<typename hash_type>
struct base_hash_traits;

template<> struct base_hash_traits<uint32_t>
{
	using value_type = uint32_t;
	enum : value_type
	{
		hash_prime = 0xBEB9B485,
		hash_coprime = 0x34263E29,
	};
};
template<> struct base_hash_traits<uint64_t>
{
	using value_type = uint64_t;
	enum : value_type
	{
		hash_prime = 0x9EF3455AD47C9E31,
		hash_coprime = 0x03519CFFA7F0F405,
	};
};

template<typename hash_type>
struct default_hash_traits
{
	using value_type = hash_type;
	static constexpr value_type const_hash_append(value_type hash, const value_type cur_value)
	{
		return (hash + cur_value * base_hash_traits<hash_type>::hash_prime) * base_hash_traits<hash_type>::hash_coprime;
	}
};
template<typename hash_type>
struct case_insensitive_hash_traits
{
	using value_type = hash_type;
	static constexpr value_type const_hash_append(value_type hash, const value_type cur_value)
	{
		static_assert(('a' & ' ') != 0);
		const value_type fast_lower = (cur_value | ' ');
		return default_hash_traits<value_type>::const_hash_append(hash, cur_value < fast_lower ? fast_lower : cur_value);
	}
};
//	template<typename hash_type>
//struct case_asymetric_hash_traits
//{
//	using value_type = hash_type;
//	static constexpr value_type const_hash_append(value_type hash, const value_type cur_value)
//	{
//		static_assert(('a' & ' ') != 0 && ('A' & ' ') == 0 && ('A' < 'a'));
//		static_assert((' ' & ' ') != 0 && ('!' & ' ') != 0);
//		const value_type processed_value = (cur_value & ' ') == 0 ? cur_value : 0 -(cur_value ^ ' ');
//		return default_hash_traits<value_type>::const_hash_append(hash, processed_value);
//	}
//};



static constexpr char toggle_case( char n ) { return n ^ ' '; }

	template<typename int_type = uint32_t>
constexpr int_type static_log2(int_type n) { return n < 2 ? 0 : 1 + static_log2(n/2); }

	template<typename hash_type = uint32_t, template<typename> class traits >
constexpr hash_type const_hash(const char* s)
{
	hash_type hash = 0;
	while (*s)
		hash = traits<hash_type>::const_hash_append(hash, hash_type(*s++));
	return hash;
}

//	template<typename K, K ..._K>
//struct static_print_helper;
//static_assert(
//		(const_hash<uint64_t, case_asymetric_hash_traits>("test")
//		 + const_hash<uint64_t, case_asymetric_hash_traits>("TEST")) == 0
//	&&
//		(const_hash<uint64_t, case_asymetric_hash_traits>("test!")
//		 + const_hash<uint64_t, case_asymetric_hash_traits>("TEST!")) == 0
//	//&&
//	//static_print_helper< int64_t,
//	//	(const_hash<uint64_t, case_asymetric_hash_traits>("test")
//	//	 + const_hash<uint64_t, case_asymetric_hash_traits>("TEST")),
//	//	0
//	//>::value == 0
//);
#endif // #if defined( SB_HASH_TRAITS )
#endif

using half    = float;
//using char8_t = char;
using dynamic_void   = std::variant< std::monostate >;
using dynamic_char   = std::variant<           char,      wchar_t,     char8_t,     char16_t,      char32_t >;
using dynamic_int    = std::variant<         int8_t,      int16_t,     int32_t,      int64_t >;
using dynamic_uint   = std::variant<           bool,       uint8_t,     uint16_t,    uint32_t,     uint64_t >;
using dynamic_float  = std::variant<           half,        float,      double >;
using dynamic_native = std::variant<   dynamic_void, dynamic_char, dynamic_int, dynamic_uint, dynamic_float >;



//struct graph
//{
//	using char_t = char;
//	using hash_t = uint64_t;
//	using key_t  = std::basic_string<char_t>;
//	using key_view_t = std::variant<std::basic_string_view<char_t>, hash_t>;
//
//	bool propagate(key_view_t command, );
//
//private:
//	using array_t = std::unordered_map<graph>;
//	array_t ;
//};

//using sb_dynamic_type = std::variant<std::string, std::wstring, bool, uint8_t, int32_t, uint32_t, int64_t, uint64_t, float, double>;
//
//struct Configuration
//{
//	Configuration* get( const char* key );
//};

// SB::LibX::Configuration
// Imagine JSON I/O :
//	- read/write : JSON/compiled-JSON stream <-> configuration (sub)graph
// Nodes can be connected to each others through paths.
/*
// E.g., let's consider this JSON configuration file
{
	FileSystem:{
		"user save path":[
			"%USERPROFILE%\Documents\MyApp\save",
		],
		"user data path":[
			"%USERPROFILE%\Documents\MyApp\save",
		],
		"save history":[
			"autosave 0",
			"autosave 3",
			"autosave 2",
			"autosave 1",
		],
	},
	Graphics:{
		"adapter":["nVidia GTX", "{GUID}"],
		"output":["DELL", "{GUID}"],
		"resolution":"native",
		"texture quality":"high",
	},
	Audio:{
		"adapter":"",
		"resolution":"native",
		"texture quality":"high",
	},
	Inputs:{
		"mouse sensitivity":3,
		"use touch":true,
	},
};

*/


}} // namespace SB::LibX

#include <iostream>
#include <any>
#include <new>
#include <common/include/sb_interface.h>

constexpr size_t kCacheLineSize = std::hardware_constructive_interference_size;

struct data_chunk
{
	uint8_t data[kCacheLineSize];
};

namespace SB { namespace LibX { template<> inline data_chunk* get<data_chunk*>( BaseHandle resource ) { return reinterpret_cast<data_chunk*>( get_pointer( resource ) ); } }}

SB_EXPORT_TYPE int __stdcall main([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	using namespace SB::LibX;
	//Debug::Console debugConsole;
	//debugConsole.RedirectStdIO();

	{
		scoped_handle sblib_instance = AcquireResource();
		auto chunk = GetData<data_chunk>(sblib_instance, get_value(sblib_instance.handle));
	}

	xhash_t hashes[128]{};
	for( size_t n = 0; n < 128; ++n )
	{
		const char data[] = {'.', static_cast<char>(n), 0 };
		const auto hash = hashes[n] = xhash_traits_t::hash(data);
		std::cout << n << ": " << data << " -> "
			<< (hash & 0x1) << "\t" << (hash & 0x3) << "\t" << (hash & 0x7) << "\t" << (hash & 0xF) << "\t" << (hash & 0xFF) << "\t" << hash
			<< "\n";
	}

	constexpr size_t representation_size[] = { 4, 8, 16, 32, };
	size_t has_representation[_countof(representation_size)]{}; for(auto&i:has_representation)i=true;
	xhash_t hash_index[_countof(representation_size)][32]{};
	size_t base = 'X';
	for (size_t n = base; n < base + 32; ++n)
	{
		xhash_t hash = hashes[n];
		for ( size_t rank = 0; rank < _countof(representation_size); ++rank )
		{
			size_t size = representation_size[rank];
			if (n < base + size)
			{
				size_t index = (hash % size);
				if (hash_index[rank][index] == 0)
					hash_index[rank][index] = hash;
				else
					has_representation[rank] = false;
			}
		}
	}

	std::cout.flush();

#if 0
	// any type
	std::cout << std::boolalpha;
	std::any a = 1;
	std::cout << a.type().name() << ": " << std::any_cast<int>(a) << '\n';
	a = 3.14;
	std::cout << a.type().name() << ": " << std::any_cast<double>(a) << '\n';
	a = true;
	std::cout << a.type().name() << ": " << std::any_cast<bool>(a) << '\n';

	// bad cast
	try
	{
		a = 1;
		std::cout << std::any_cast<float>(a) << '\n';
	}
	catch (const std::bad_any_cast & e)
	{
		std::cout << e.what() << '\n';
	}

	// has value
	a = 1;
	if (a.has_value())
	{
		std::cout << a.type().name() << '\n';
	}

	// reset
	a.reset();
	if (!a.has_value())
	{
		std::cout << "no value\n";
	}

	// pointer to contained data
	a = 1;
	int* i = std::any_cast<int>(&a);
	std::cout << *i << "\n";

	//using namespace SB::LibX;
	//dynamic_uint dyn_var;
	//dyn_var = true;
	//dyn_var.index;
	//dyn_var.
	//std::cout << "dyn_var = " << std::get<bool>(dyn_var) << "\n size = " << sizeof(dyn_var) << std::endl;
#endif
}
