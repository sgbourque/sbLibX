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

//template<typename char_t, size_t _LENGHT_>
//constexpr size_t string_length( [[maybe_unused]] const char_t( &str )[_LENGHT_] ) {
//	return _LENGHT_;
//}
//template<typename Char, typename Arg, typename ... Args>
//constexpr auto concat( Arg&& arg, Args&& ... args ) {
//	return concat2( make( std::forward<Arg>( arg ) ), concat<Char>( std::forward<Args>( args ) ... ) );
//}
//
//

#if 1
////
	template <typename _CHAR_TYPE_, typename _HASH_TRAITS_, size_t _LENGHT_>
struct static_encrypted_string_info
{
	using char_t = _CHAR_TYPE_;
	using hash_trats_t = _HASH_TRAITS_;
	using const_pointer = const char_t*;
	using encrypt_type = typename hash_trats_t::hash_t;
	
	static inline constexpr size_t encrypted_size = (_LENGHT_ + sizeof(encrypt_type) - 1);
	using array_t = std::array<encrypt_type, encrypted_size / sizeof(encrypt_type)>;
};
	template <typename _CHAR_TYPE_, typename _HASH_TRAITS_, size_t _LENGHT_>
struct static_encrypted_string
{
	using info_t = static_encrypted_string_info<_CHAR_TYPE_, _HASH_TRAITS_, _LENGHT_>;
	using hash_trats_t = typename info_t::hash_trats_t;
	using char_t = typename info_t::char_t;
	using encrypt_type = typename info_t::encrypt_type;

	using encrypted_array_t = typename info_t::array_t;
	using decrypted_array_t = std::array<char_t, _LENGHT_>;

		template<typename ...CHAR_TYPE>
	inline constexpr static_encrypted_string( CHAR_TYPE... unencrypted )
		: encrypted_data( encrypt( std::forward<CHAR_TYPE>( unencrypted )... ) )
	{
	}

	inline constexpr const encrypt_type* data() const { return encrypted_data.data(); }
	inline constexpr size_t size() const { return encrypted_data.size(); }

		template<typename ...CHAR_TYPE>
	static inline constexpr encrypted_array_t encrypt( [[maybe_unused]] CHAR_TYPE... unencrypted )
	{
		decrypted_array_t unencrypted_array{ std::forward<CHAR_TYPE>( unencrypted )... };
		encrypted_array_t encrypted{};
		for( size_t index = 0; index < encrypted.size(); ++index )
		{
			constexpr size_t encrypt_size = sizeof(encrypt_type)/sizeof(char_t);
			encrypt_type next{};

			const int64_t remainder_lenght = static_cast<int64_t>(_LENGHT_) - static_cast<int64_t>( index * encrypt_size );
			const int64_t next_encrypt_size = std::min<int64_t>( encrypt_size, std::max<int64_t>( remainder_lenght, 0 ) );

			for( int64_t subindex = 0; subindex < next_encrypt_size; ++subindex )
			{
				constexpr int64_t char_t_bit = CHAR_BIT * sizeof( char_t );
				const size_t unencrypted_index = index * encrypt_size + static_cast<size_t>( subindex );
				const auto next_encrypted = static_cast<encrypt_type>( unencrypted_array[unencrypted_index] );
				const auto next_encrypted_pos = ( subindex * char_t_bit );
				next |= ( next_encrypted << next_encrypted_pos );
			}
			encrypted[index] = ( next * hash_trats_t::prime ) ^ hash_trats_t::coprime;
		}
		return encrypted;
	}

	inline constexpr decrypted_array_t decript() const
	{
		decrypted_array_t decrypted_array{};
		for( size_t index = 0; index < encrypted_data.size(); ++index )
		{
			constexpr size_t encrypt_size = sizeof(encrypt_type)/sizeof(char_t);
			const int64_t remainder_lenght = static_cast<int64_t>( _LENGHT_ ) - static_cast<int64_t>( index * encrypt_size );
			const int64_t next_encrypt_size = std::min<int64_t>( encrypt_size, std::max<int64_t>( remainder_lenght, 0 ) );
			encrypt_type next = ( encrypted_data[index] ^ hash_trats_t::coprime ) * hash_trats_t::inverse_prime;
			for( int64_t subindex = 0; subindex < next_encrypt_size; ++subindex )
			{
				using mask_t = std::conditional_t<std::is_signed_v<encrypt_type>,
					std::make_signed_t<char_t>,
					std::make_unsigned_t<char_t>
				>;
				constexpr mask_t char_mask = static_cast<mask_t>( ~0 );
				constexpr int64_t char_t_bit = CHAR_BIT * sizeof( char_t );
				const size_t unencrypted_index = index * encrypt_size + static_cast<size_t>( subindex );
				const auto next_encrypted_pos = ( subindex * char_t_bit );
				decrypted_array[unencrypted_index] = static_cast<char_t>( ( next >> next_encrypted_pos ) & char_mask );
			}
		}
		return decrypted_array;
	}
private:
	encrypted_array_t encrypted_data;
};

	template<typename _CHAR_TYPE_, typename _HASH_TRAITS_, size_t _LENGHT_>
struct encrypted_string_helper
{
	using type_t = static_encrypted_string_info<_CHAR_TYPE_, _HASH_TRAITS_, _LENGHT_>;
	using hash_trats_t = _HASH_TRAITS_;
	using char_t = typename type_t::char_t;
	using array_t = static_encrypted_string<char_t, hash_trats_t, _LENGHT_>;

		template<size_t BEGIN, size_t END>
	struct range_t {
			template<typename ...CHAR_TYPE>
		static constexpr array_t encrypt( [[maybe_unused]] const char_t* unencrypted_arrray, [[maybe_unused]] CHAR_TYPE... unencrypted )
		{
			if constexpr ( sizeof...(unencrypted) + 1 < END )
				return range_t<BEGIN + 1, END>::encrypt( unencrypted_arrray + 1, std::forward<CHAR_TYPE>( unencrypted )..., *unencrypted_arrray );
			else
				return range_t<END, END>::encrypt( std::forward<CHAR_TYPE>( unencrypted )..., *unencrypted_arrray );
		}

		static constexpr array_t encrypt( const char_t( &unencrypted_arrray )[_LENGHT_ - BEGIN] )
		{
			static_assert( BEGIN == 0 );
			if constexpr ( BEGIN + 1 < END )
				return range_t<BEGIN + 1, END>::encrypt( unencrypted_arrray + 1, *unencrypted_arrray );
			else
				return range_t<END, END>::encrypt( *unencrypted_arrray );
		}
	};
		template<size_t END>
	struct range_t<END, END> {
			template<typename ...CHAR_TYPE>
		static constexpr array_t encrypt( [[maybe_unused]] CHAR_TYPE... unencrypted )
		{
			return array_t{ std::forward<CHAR_TYPE>( unencrypted )... };
		}
	};

	static inline constexpr array_t encrypt( const char_t( &unencrypted )[_LENGHT_] )
	{
		return range_t<0, _LENGHT_>::encrypt( unencrypted );
	}
};
#endif

	template<typename char_t, size_t _LENGHT_>
static inline constexpr auto encrypted_string_impl( const char_t( &unencrypted )[_LENGHT_] )
{
	return encrypted_string_helper<char_t, sbLibX::xhash_traits_t, _LENGHT_>::encrypt( unencrypted );
}

// While possibly wasting space with duplicates, at least this garantees that 'unencrypted' will never
// be in the final binary data since it forces encrypted value evaluated as a constexpr.
// TODO: Find a way to prevent data duplication while still enforcing constexpr evaluation
// (and completely stripping out unencrypted data).
#define encrypted_string( unencrypted ) []() { constexpr auto encrypted = encrypted_string_impl( unencrypted ); return encrypted; }()

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
	virtual ~custom_class_t()
	{
		say( "bye!" );
	}

	virtual void say( std::string_view msg )
	{
		using namespace sbLibX;
		const bool hasClassName = config.className.get_value()[0];
		auto className = ( hasClassName ? config.className.get_value() : "" );

		[[maybe_unused]]
		auto test_encrypted1 = encrypted_string( "This text should be nowhere to be found in the optimized (constexpr) binary!" );
		auto test_encrypted2 = encrypted_string_impl( "This string is not encrypted properly (still present in bin)..." );
		std::cerr
			<< test_encrypted1.decript().data() << " "
			<< test_encrypted2.decript().data()
			<< std::endl;


		std::cerr
			<< "'" << className.decrypt() << ( hasClassName ? "/" : "" )
			<< config.name << "/" << name << "'"
			<< ": " << msg << "\n";
	}


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



#if 1 // Just an overview of a typical structured_buffer instance
	struct struct_buf {
		using type_t = struct_buf;
		using hash_traits_t = /*_TRAITS_;*//**/sbLibX::xhash_traits_t;/**/
		using hash_string_view_t = sbLibX::xhash_string_view<hash_traits_t>;
		using hash_t = typename hash_traits_t::hash_t;
		//static constexpr auto options = std::make_tuple({});
		inline static constexpr hash_string_view_t struct_hash = "struct_buf" " " "" "";
		
		template<hash_t, typename IMPL = type_t> struct data_traits;
		template<size_t, typename IMPL = type_t> struct key_traits;
		static inline constexpr size_t kElementIdOffset = __COUNTER__;

#if 0
	//SB_STRUCT_MEMBER(long const int unsigned volatile long, try_not_to, inline static constexpr)(int n) { return {n != 3}; }
		static inline constexpr size_t try_not_to_id = __COUNTER__ - kElementIdOffset - 1;
		static_assert(try_not_to_id < 64, "Too many unique data members. Try using sub-buffers or split the buffer.");
		static inline constexpr hash_string_view_t try_not_to_hash = SB_MEMBER_ENTRY "try_not_to";
		template<typename IMPL> struct data_traits<hash_traits_t::hash(SB_MEMBER_ENTRY "try_not_to"), IMPL>
		{
			using type_t = decltype(IMPL::try_not_to);
			static inline constexpr size_t offset = 
				std::conditional_t<true, std::integral_constant<size_t, ~0ull>
										, std::integral_constant<size_t, offsetof(IMPL, try_not_to)>>::value;
				//sb_offsetof(IMPL, try_not_to);
			static inline constexpr size_t size   = sizeof(type_t);
			using key_traits_t = key_traits<try_not_to_id, IMPL>;
		};
		template<typename IMPL> struct key_traits<try_not_to_id, IMPL>
		{
			static inline constexpr hash_string_view_t name_hash = IMPL::try_not_to_hash;
			using type_t = decltype(IMPL::try_not_to);/* TODO: type hash */
			using data_traits_t = data_traits<name_hash, IMPL>;
		};
		using try_not_to_base_t = long const int unsigned volatile long; /*inline static constexpr*/ try_not_to_base_t try_not_to /* Not supported (I will try to find a way) */ (int n) { return { n != 3 }; }
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
#endif

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
SB_EXPORT_TYPE int SB_STDCALL test_hash([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	using sbLibX::operator "" _xhash64;
	using sbLibX::StructuredBuffer::get;
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
	get<".name"_xhash64>(config) = sbLibX::xhash_string_view_t{name.data()};

	std::cout << "\t" << ".name"_xhash64 << ": " << get<".name"_xhash64>(config) << "\n";
	std::cout << "\t" << ".className"_xhash64 << ": " << get<".className"_xhash64>(config) << "\n";
	//std::cout << "\t" << ".anonymous_vec4f"_xhash64 << ": " << get<".anonymous_vec4f"_xhash64>(config) << "\n"; // won't compile
	//std::cout << "\t" << ".anonymous_vec4f"_xhash64 << ": " << get(config, ".anonymous_vec4f"_xhash64) << "\n"; // won't compile
	std::cout << "\t" << ".moduleFlags"_xhash64 << ": " << get<".moduleFlags"_xhash64>(config) << "\n";

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
	//struct_buf test = {};
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
