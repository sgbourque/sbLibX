#pragma once
#include <common/include/sb_hash.h>
#include <common/include/sb_utilities.h>

#include <cstdint>
#include <array>


namespace SB { namespace LibX
{
namespace Encryption
{

	template< typename _TYPE_T = uint64_t, size_t _KEY_LENGTH_ = 4 >
using private_key_t = std::array<_TYPE_T, _KEY_LENGTH_>;

// Essentially, this is 4 encryption keys with their modular inverse in characteristic 2^64
#ifndef SBPRIVATE_KEY_FILE
	#define SBPRIVATE_KEY_FILE "test/include/sb_default_256.key"
#endif
	template< typename _KEY_T = private_key_t<> >
struct key_pair_t
{
	static_assert( result.size() > 3, "key must have length at least 4" );

	using key_t = _KEY_T;
	using value_t = typename key_t::value_type;
	static inline constexpr key_t default_encrypt_key = {
		#define KEY_PRIME(value) type_t((value ## ull) & type_t(-1)),
		#include SBPRIVATE_KEY_FILE
	};
	static inline constexpr key_t default_decrypt_key = {
		#define KEY_PRIME(value) sbLibX::modular_inverse(type_t((value ## ull) & type_t(-1))),
		#include SBPRIVATE_KEY_FILE
	};
	static inline constexpr value_t public_encrypt_key( size_t seed = 0 ) noexcept {
		return default_encrypt_key[(seed + 0) % result.size()] * default_encrypt_key[(seed + 1) % result.size()] * default_decrypt_key[(seed + result.size() - 1) % result.size()];
	}
	static inline constexpr value_t public_decrypt_key( size_t seed = 0 ) noexcept {
		return default_decrypt_key[( seed + 0 ) % result.size()] * default_encrypt_key[( seed + 1 ) % result.size()] * default_encrypt_key[( seed + result.size() - 1 ) % result.size()];
	}

	key_t encrypt_key = public_encrypt_key();
	key_t decrypt_key = public_decrypt_key();
}


//	template< typename _TYPE_T = uint64_t, size_t _KEY_LENGTH_ = 8 >
//struct encrypted_key_traits
//{
//	static_assert( std::is_unsigned_v<type_t> );
//	#ifndef SBPRIVATE_KEY_FILE
//		#define SBPRIVATE_KEY_FILE "test/include/private_generated_512.key"
//	#endif
//	static inline constexpr key_t private_key = {
//		#define KEY_PRIME(value) type_t((value ## ull) & type_t(-1)), sbLibX::modular_inverse(type_t((value ## ull) & type_t(-1))),
//		#include SBPRIVATE_KEY_FILE
//	};
//	static_assert( private_key[0] * private_key[1] == 1ull
//	            && private_key[2] * private_key[3] == 1ull
//	            && private_key[4] * private_key[5] == 1ull
//	            && private_key[6] * private_key[7] == 1ull, "Keys must be paired" );
//	static_assert( private_key[0] * private_key[0] != 1ull
//	            && private_key[2] * private_key[2] != 1ull
//	            && private_key[4] * private_key[4] != 1ull
//	            && private_key[6] * private_key[6] != 1ull, "Keys must not be trivial" );
//};

	template< typename _CHAR_T, size_t _LENGTH_, typename _KEY_T >
struct encrypted_data_traits
{
	using char_t = _CHAR_T;
	using uchar_t = std::make_unsigned_t<char_t>;
	using decrypted_t = std::array<char_t, _LENGTH_>;

	using key_t = _KEY_T;
	using value_t = typename key_t::value_type;
	static_assert( sizeof( value_t ) >= sizeof( char_t ) );
	static inline constexpr size_t packed_size = sizeof( value_t ) / sizeof( char_t );
	static inline constexpr size_t encrypted_size = sbLibX::div_align_up( _LENGTH_, packed_size );
	using encrypted_t = std::array< value_t, encrypted_size >;

	static inline constexpr size_t size( [[maybe_unused]] const data_t& data ) noexcept { return encrypted_size; }
	static inline constexpr size_t size( [[maybe_unused]] const decrypted_t& data ) noexcept { return _LENGTH_; }

	static inline constexpr void resize_encrypted_data( [[maybe_unused]] data_t& data, [[maybe_unused]] size_t length ) {}
	static inline constexpr void resize_decrypted_data( [[maybe_unused]] decrypted_t& data, [[maybe_unused]] size_t encrypted_size ) {}
};
	template< typename _CHAR_T, typename _KEY_T >
struct encrypted_data_traits< _CHAR_T, 0, _KEY_T >
{
	using char_t = _CHAR_T;
	using uchar_t = std::make_unsigned_t<char_t>;
	using decrypted_t = std::vector<char_t>;

	using key_t = _KEY_T;
	using value_t = typename key_t::value_type;
	static_assert( sizeof( value_t ) >= sizeof( char_t ) );
	static inline constexpr size_t packed_size = sizeof( value_t ) / sizeof( char_t );
	using encrypted_t = std::vector< value_t >;

	static inline constexpr size_t size( const data_t& data ) noexcept { return data.size(); }
	static inline constexpr size_t size( const decrypted_t& data ) noexcept { return data.size(); }

	static inline constexpr void resize_encrypted_data( [[maybe_unused]] data_t& data, [[maybe_unused]] size_t length )
	{
		data.resize( sbLibX::div_align_up( length, packed_size ) );
	}
	static inline constexpr void resize_decrypted_data( [[maybe_unused]] decrypted_t& data, [[maybe_unused]] size_t encrypted_size )
	{
		data.resize( packed_size * encrypted_size );
	}
};

	template< typename _CHAR_T, size_t _LENGTH_ = 0 >
struct encrypted_data
{
	using key_t = encrypt_key_t<>;
	using traits_t = encrypted_data_traits< _CHAR_T, _LENGTH_, key_t >;
	using char_t = typename traits_t::char_t;
	using uchar_t = typename traits_t::uchar_t;
	using decrypted_t = typename traits_t::decrypted_t;
	using type_t = key_t::type_t;
	using data_t = typename traits_t::data_t;

	static inline constexpr key_t private_key = traits_t::private_key;
	static inline constexpr type_t private_encrypt_key = private_key.value[(_LENGTH_ + 0 ) % key_t::split_count] * private_key.value[(_LENGTH_ + 1 ) % key_t::split_count] * private_key.value[(_LENGTH_ + 7 ) % key_t::split_count];
	static inline constexpr type_t private_decrypt_key = private_key.value[(_LENGTH_ + 4 ) % key_t::split_count] * private_key.value[(_LENGTH_ + 5 ) % key_t::split_count] * private_key.value[(_LENGTH_ + 3 ) % key_t::split_count];
	static_assert( private_encrypt_key * private_decrypt_key == 1 );

	static inline constexpr type_t public_encrypt_key = private_key.value[(_LENGTH_ + 1 ) % key_t::split_count] * private_key.value[2] * private_key.value[(_LENGTH_ + 4 ) % key_t::split_count];
	static inline constexpr type_t public_decrypt_key = private_key.value[(_LENGTH_ + 5 ) % key_t::split_count] * private_key.value[6] * private_key.value[(_LENGTH_ + 0 ) % key_t::split_count];
	static_assert( public_encrypt_key * public_decrypt_key == 1 );


	static inline constexpr size_t packed_size = traits_t::packed_size;
	static inline constexpr size_t size( const decrypted_t& data ) noexcept { return traits_t::size( data ); }
	static inline constexpr size_t size( const data_t& data ) noexcept { return traits_t::size( data ); }
	type_t public_key;
	data_t data{};

	static inline constexpr void resize_encrypted_data( [[maybe_unused]] data_t& data, [[maybe_unused]] size_t length )
	{
		traits_t::resize_encrypted_data( data, length );
	}
	static inline constexpr void resize_decrypted_data( [[maybe_unused]] decrypted_t& data, [[maybe_unused]] size_t length )
	{
		traits_t::resize_decrypted_data( data, length );
	}

	constexpr encrypted_data( data_t&& _data, type_t _public_key ) noexcept : data( std::move( _data ) ), public_key( _public_key ) {}

	constexpr auto begin() const { return data.begin(); }
	constexpr auto end() const { return data.end(); }

	constexpr type_t operator []( size_t index ) const { return data[index]; }
};

}

#if 0 // deprecated : a new version should supercede this implementation
////
	template <typename _CHAR_TYPE_, typename _HASH_TRAITS_, size_t _LENGTH_>
struct encrypted_string_base
{
	using char_t = _CHAR_TYPE_;
	using hash_traits_t = _HASH_TRAITS_;
	using const_pointer = const char_t*;
	using encrypt_type = typename hash_traits_t::hash_t;
	enum : encrypt_type
	{
		prime = hash_traits_t::prime,
		inverse_prime = modular_inverse( hash_traits_t::prime ),

		coprime = hash_traits_t::coprime,
	};
	static_assert( prime * inverse_prime == 1, "TODO: Calculate modular inverse instead of forcing people to precompute it..." );

	static inline constexpr size_t encrypted_size = sbLibX::div_align_up( _LENGTH_ * sizeof( char_t ), sizeof( encrypt_type ) );
	using encrypted_array_t = std::array<encrypt_type, encrypted_size>;
	using decrypted_array_t = std::array<char_t, _LENGTH_>;

	inline constexpr encrypted_string_base( decrypted_array_t&& unencrypted_array )
		: encrypted_data( encrypt( unencrypted_array ) )
	{
	}

	inline constexpr const encrypt_type* data() const { return encrypted_data.data(); }
	inline constexpr size_t size() const { return encrypted_data.size(); }

	static inline constexpr encrypted_array_t encrypt( decrypted_array_t&& unencrypted_array )
	{
		encrypt_type next_scrambled{ coprime };
		encrypted_array_t encrypted{};
		for( size_t index = 0; index < encrypted.size(); ++index )
		{
			constexpr size_t encrypt_size = sizeof(encrypt_type)/sizeof(char_t);

			const int64_t remainder_length = static_cast<int64_t>(_LENGTH_) - static_cast<int64_t>( index * encrypt_size );
			const size_t next_unencrypted_length = std::min<size_t>( encrypt_size, std::max<int64_t>( remainder_length, 0 ) );

			for( size_t subindex = 0; subindex < encrypt_size; ++subindex )
			{
				constexpr size_t char_t_bit = CHAR_BIT * sizeof( char_t );
				const size_t unencrypted_index = index * encrypt_size + subindex;
				const auto next_unencrypted_char = subindex < next_unencrypted_length ? unencrypted_array[unencrypted_index] : encrypt_type{};
				const auto next_unencrypted_char_pos = ( subindex * char_t_bit );
				next_scrambled ^= ( next_unencrypted_char << next_unencrypted_char_pos );
			}
			encrypted[index] = ( next_scrambled * prime ) ^ coprime;
		}
		return encrypted;
	}
		template<typename ...CHAR_TYPE>
	static inline constexpr encrypted_array_t encrypt( [[maybe_unused]] CHAR_TYPE... unencrypted )
	{
		return encrypt( decrypted_array_t{ std::forward<CHAR_TYPE>( unencrypted )... } );
	}

	constexpr operator const encrypted_array_t& () const { return encrypted_data; }
private:
	encrypted_array_t encrypted_data;
};

	template<typename _HASH_TRAITS_, typename char_t, size_t _LENGTH_, size_t ... _INDICES_>
static inline constexpr auto encrypted_string_unsecured( const char_t* unencrypted, [[maybe_unused]] std::index_sequence<_INDICES_...> const& )
{
	using string_t = encrypted_string_base<char_t, _HASH_TRAITS_, _LENGTH_>;
	using array_t = typename string_t::decrypted_array_t;
	return string_t( array_t{ {unencrypted[_INDICES_]...} } );
}

	template<typename char_t, size_t _LENGTH_>
static inline constexpr auto encrypted_string_unsecured( const char_t( &unencrypted )[_LENGTH_] )
{
	return encrypted_string_unsecured<sbLibX::xhash_traits_t, char_t, align_up(_LENGTH_, /*sbLibX::xhash_traits_t::max_data_size*/32)>( unencrypted, std::make_index_sequence<_LENGTH_>{} );
}

//	template<typename char_t, typename _HASH_TRAITS_, size_t _LENGTH_>
//static inline constexpr auto encrypted_string_unsecured( const char_t* unencrypted, size_t max_length = size_t{-1} )
//{
//	return encrypted_string_unsecured<char_t, _HASH_TRAITS_, _LENGTH_>( unencrypted, max_length, std::make_index_sequence<_LENGTH_>{} );
//}


//
// DOES NOT WORKS! depends on compiler's settings
// 
// While possibly wasting space with duplicates, at least this garantees that 'unencrypted' will never
// be in the final binary data since it forces encrypted value evaluated as a constexpr.
// TODO: Find a way to prevent data duplication while still enforcing constexpr evaluation
// (and completely stripping out unencrypted data).
#define constexpr_encrypt( unencrypted ) []() { constexpr auto encrypted = sbLibX::encrypted_string_unsecured( unencrypted ); return encrypted; }()


	template <typename _CHAR_TYPE_, typename _HASH_TRAITS_, size_t _LENGTH_>
struct decrypted_string_base
{
	using char_t = _CHAR_TYPE_;
	using hash_traits_t = _HASH_TRAITS_;
	using const_pointer = const char_t*;
	using encrypt_type = typename hash_traits_t::hash_t;
	enum : encrypt_type
	{
		prime = hash_traits_t::prime,
		inverse_prime = modular_inverse( hash_traits_t::prime ),

		coprime = hash_traits_t::coprime,
	};

	static inline constexpr size_t encrypted_size = sbLibX::div_align_up( _LENGTH_ * sizeof( char_t ), sizeof( encrypt_type ) );
	using encrypted_array_t = std::array<encrypt_type, encrypted_size>;
	using decrypted_array_t = std::array<char_t, _LENGTH_>;

	inline constexpr const char_t* data() const { return decrypted_data.data(); }
	inline constexpr size_t size() const { return decrypted_data.size(); }

	inline constexpr decrypted_string_base( const encrypted_array_t& encrypted_array )
		: decrypted_data( decrypt( encrypted_array ) )
	{
	}

	static inline /*constexpr*/ decrypted_array_t decrypt( const encrypted_array_t& encrypted_data )
	{
		return decrypt( encrypted_data.data(), encrypted_data.data() + encrypted_data.size() );
	}
	static inline /*constexpr*/ decrypted_array_t decrypt( const encrypt_type* encrypted_data, const encrypt_type* encrypted_data_end )
	{
		encrypt_type prev_scrambled{ coprime };
		decrypted_array_t decrypted_array{};
		const size_t encrypted_data_size = static_cast<size_t>( encrypted_data_end - encrypted_data );
		for( size_t index = 0; index < encrypted_data_size; ++index )
		{
			constexpr size_t encrypt_size = sizeof(encrypt_type)/sizeof(char_t);
			const size_t max_decrypted_length = std::min( _LENGTH_, encrypted_data_size * encrypt_size );
			const int64_t remainder_length = static_cast<int64_t>( max_decrypted_length ) - static_cast<int64_t>( index * encrypt_size );
			const int64_t next_encrypt_size = std::min<int64_t>( encrypt_size, std::max<int64_t>( remainder_length, 0 ) );

			encrypt_type next_scrambled = ( encrypted_data[index] ^ coprime ) * inverse_prime;
			encrypt_type next_decrypted = ( next_scrambled ^ prev_scrambled );
			prev_scrambled = next_scrambled;
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
				decrypted_array[unencrypted_index] = static_cast<char_t>( ( next_decrypted >> next_encrypted_pos ) & char_mask );
			}
		}
		return decrypted_array;
	}

	operator const decrypted_array_t& () const { return decrypted_data; }
	operator std::basic_string<char_t> () const { return std::basic_string<char_t>( decrypted_data.data(), decrypted_data.data() + decrypted_data.size() - 1 ); }

private:
	decrypted_array_t decrypted_data;
};

	template<typename char_t, typename _HASH_TRAITS_, size_t _LENGTH_>
static inline constexpr auto decrypt( const encrypted_string_base<char_t, _HASH_TRAITS_, _LENGTH_>& encrypted )
{
	using string_t = decrypted_string_base<char_t, _HASH_TRAITS_, _LENGTH_>;
	return string_t{ encrypted };
}

	template<typename char_t, typename _HASH_TRAITS_, size_t _LENGTH_>
static inline std::ostream& operator <<( std::ostream& os, decrypted_string_base<char_t, _HASH_TRAITS_, _LENGTH_> decrypted )
{
	return os << decrypted.data();
}
	template<typename char_t, typename _HASH_TRAITS_, size_t _LENGTH_>
static inline std::ostream& operator <<( std::ostream& os, encrypted_string_base<char_t, _HASH_TRAITS_, _LENGTH_> encrypted )
{
	return os << decrypt( encrypted );
}


////
#if 0 // not yet ready for ""_xhash64 :/
	template <typename _CHAR_TYPE_, typename _HASH_TRAITS_, typename _CHAR_TRAITS_ = std::char_traits<_CHAR_TYPE_>>
struct encrypted_string_view
{
	using char_t = _CHAR_TYPE_;
	using hash_traits_t = _HASH_TRAITS_;
	using char_traits_t = _CHAR_TRAITS_;

	using encrypt_type = typename hash_traits_t::hash_t;
	using string_t = std::basic_string<char_t, char_traits_t>;

	constexpr encrypted_string_view() noexcept : string_view_t_value() {}
	constexpr encrypted_string_view( const encrypted_string_view& ) noexcept = default;
	constexpr encrypted_string_view& operator=( const encrypted_string_view& ) noexcept = default;

	constexpr encrypted_string_view( const char_t* unencrypted, size_t length ) noexcept
		: encrypted_begin(), encrypted_end()
	{
		encrypted_string_unsecured<char_t, hash_traits_t, hash_traits_t::default_length>( unencrypted, length );
	}

		template< size_t _LENGTH_ >
	constexpr encrypted_string_view( const encrypted_string_base<char_t, hash_traits_t, _LENGTH_>& encrypted ) noexcept
		: encrypted_begin( encrypted.data()), encrypted_end( encrypted.data() + encrypted.size() ) {}

	string_t decrypt() const
	{
		return sbLibX::decrypted_string_base<char_t, hash_traits_t, hash_traits_t::default_length>::decrypt( encrypted_begin, encrypted_end ).data();
	}
	
private:
	const encrypt_type* encrypted_begin;
	const encrypt_type* encrypted_end;
};

	template <typename _CHAR_TYPE_, typename _HASH_TRAITS_, typename _CHAR_TRAITS_ = std::char_traits<_CHAR_TYPE_>>
inline std::basic_ostream<_CHAR_TYPE_>& operator << ( std::basic_ostream<_CHAR_TYPE_>& os, encrypted_string_view< _CHAR_TYPE_, _HASH_TRAITS_, _CHAR_TRAITS_> encripted_string_view )
{
	return os << encripted_string_view.decrypt();
}

#else

	template <typename _CHAR_TYPE_, typename _HASH_TRAITS_, typename _CHAR_TRAITS_ = std::char_traits<_CHAR_TYPE_>>
struct encrypted_string_view
{
	using char_t = _CHAR_TYPE_;
	using hash_traits_t = _HASH_TRAITS_;
	using char_traits_t = _CHAR_TRAITS_;

	using string_view_t = std::basic_string_view<char_t, char_traits_t>;
	using const_pointer = typename string_view_t::const_pointer;
	using size_type = typename string_view_t::size_type;

	constexpr encrypted_string_view() noexcept : string_view_t_value() {}
	constexpr encrypted_string_view( const encrypted_string_view& ) noexcept = default;
	constexpr encrypted_string_view& operator=( const encrypted_string_view& ) noexcept = default;

	constexpr encrypted_string_view( const const_pointer _Ntcts ) noexcept
		: string_view_t_value( _Ntcts ) {}

	// TODO: encrypt here!
	constexpr encrypted_string_view( const const_pointer _Cts, const size_type _Count ) noexcept
		: string_view_t_value( /*encrypt(*/ _Cts, _Count /*)*/ ) {}

	constexpr auto data() const { return string_view_t_value.data(); }
	constexpr auto size() const { return string_view_t_value.size(); }
	constexpr auto operator []( size_type index ) const { return string_view_t_value[index]; }

	using string_t = std::basic_string<char_t, char_traits_t>;
	constexpr auto encrypted() const { return "!encrypted! " + string_t(string_view_t_value) + " !encrypted!"; }
	//constexpr string_view_t encrypted() const { return string_t(string_view_t_value) + "(TODO: should have been encrypted)"; }

	// TODO: encrypt/decrypt here
	//constexpr auto encrypt( ..._Types ) const { return encrypt( encode(first), ...rest ); }
	constexpr auto decrypt() const { return "?decrypted? " + string_t{ string_view_t_value } + " ?decrypted?"; }

	//static inline constexpr excrypt( const const_pointer _Cts, const size_type _Count ) noexcept
	//{
	//	return string_view_t( _Cts, _Count );
	//}

private:
	string_view_t string_view_t_value;
};

//	template <class _CHAR_TYPE_, class _HASH_TRAITS_ = std::char_traits<_CHAR_TYPE_>>
//inline std::ostream& operator << ( std::ostream& os, encrypted_string_view< _CHAR_TYPE_, _HASH_TRAITS_> encripted_string_view )
//{
//	return os << encripted_string_view;
//}
#endif

////
	template<typename _HASH_TRAITS_>
struct xhash_string_view
{
	using traits_t = _HASH_TRAITS_;
	using key_t = typename traits_t::hash_t;
	using char_t = typename traits_t::char_t;
	using const_char_ptr_t = typename traits_t::const_char_ptr_t;
	using value_t  = encrypted_string_view<char_t, traits_t>;
	static_assert( !std::is_same_v<key_t, value_t > );

	constexpr xhash_string_view() noexcept = default;
	constexpr xhash_string_view( const xhash_string_view& ) noexcept = default;

	//constexpr xhash_string_view( value_t _value ) noexcept : data( traits_t::hash(_value.data(), _value.size()), _value ) {}
	//constexpr xhash_string_view( key_t _key, value_t _value ) noexcept : data( _key, _value ) {}
	//constexpr xhash_string_view( const_char_ptr_t _value ) noexcept : data( traits_t::hash(_value), _value ) {}
	constexpr xhash_string_view( value_t _value ) noexcept : key( traits_t::hash(_value.data(), _value.size()) ), value( _value ) {}
	constexpr xhash_string_view( key_t _key, value_t _value ) noexcept : key( _key ), value( _value ) {}
	constexpr xhash_string_view( const_char_ptr_t _value ) noexcept : key( traits_t::hash(_value) ), value( _value ) {}

	//constexpr xhash_string_view( value_t _value ) noexcept : key(), value( _value )
	//{
	//	const auto decrypted = _value.decrypt();
	//	key = traits_t::hash( decrypted.data(), decrypted.size() );
	//}
	//constexpr xhash_string_view( const_char_ptr_t _value, size_t length ) noexcept : key( traits_t::hash( _value, length ) ), value( _value ) {}

	constexpr operator key_t() const { return get_key(); }
	constexpr operator value_t() const { return get_value(); }

	constexpr key_t get_key() const { return key; }
	constexpr value_t get_value() const { return value; }

	//key_value_pair<key_t, value_t> data;
	key_t    key;
	value_t  value;
};


////
//	template<typename _HASH_TRAITS_>
//struct xhash_string
//{
//	using traits_t = _HASH_TRAITS_;
//	using key_t = typename traits_t::hash_t;
//	using char_t = typename traits_t::char_t;
//	using const_char_ptr_t = typename traits_t::const_char_ptr_t;
//	using value_t  = encrypted_string_base<char_t, traits_t, traits_t::max_data_size>;
//
//	constexpr xhash_string() noexcept = default;
//	constexpr xhash_string( const xhash_string& ) noexcept = default;
//	constexpr xhash_string( xhash_string&& ) noexcept = default;
//
//	//constexpr xhash_string( value_t _value ) noexcept : key( traits_t::hash( _value.data(), _value.size() ) ), value( _value ) {}
//	constexpr xhash_string( key_t _key, const value_t& _encrypted ) noexcept : data( _key, _encrypted ) {}
//	constexpr xhash_string( const_char_ptr_t _unencrypted ) noexcept : key( traits_t::hash( _value ) ), value( _value ) {}
//
//	//constexpr xhash_string_view( value_t _value ) noexcept : key(), value( _value )
//	//{
//	//	const auto decrypted = _value.decrypt();
//	//	key = traits_t::hash( decrypted.data(), decrypted.size() );
//	//}
//	//constexpr xhash_string_view( const_char_ptr_t _value, size_t length ) noexcept : key( traits_t::hash( _value, length ) ), value( _value ) {}
//
//	constexpr key_t get_key() const { return data.get_key(); }
//	constexpr value_t get_value() const { return data.get_value(); }
//
//	key_value_pair<key_t, value_t> data;
//};


////
	template<typename _HASH_TRAITS_>
inline std::ostream& operator << (std::ostream& os, xhash_string_view< _HASH_TRAITS_> hash_string_view)
{
	return os << "{0x" << std::hex << hash_string_view.get_key() << ":'" << hash_string_view.get_value().decrypt() << "(" << hash_string_view.get_value().encrypted() << ")" << "'}";
}


////
// Eventually, support for UTF-8/16/32
// with system-specific implementation for native <-> international (unicode) conversion.
// ... eventually... well, that's the plan...
using xhash_string_view_t = xhash_string_view<xhash_traits_t>;


constexpr xhash_string_view_t operator "" _xhash64([[maybe_unused]] xhash_traits_t::const_char_ptr_t string, [[maybe_unused]] size_t length)
{
	return xhash_string_view_t{ typename xhash_string_view_t::value_t(string, length) };
}
#endif
}} // namespace SB::LibX
namespace sbLibX = SB::LibX;

