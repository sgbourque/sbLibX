#pragma once
#include <common/include/sb_hash.h> // sb_hash and sb_encrypted_string are to be used together

#include <cstdint>
#include <array>
#include <iosfwd>
#include <string>

namespace SB { namespace LibX
{
////
	template <typename _CHAR_TYPE_, typename _HASH_TRAITS_, size_t _LENGHT_>
struct encrypted_string_base
{
	using char_t = _CHAR_TYPE_;
	using hash_traits_t = _HASH_TRAITS_;
	using const_pointer = const char_t*;
	using encrypt_type = typename hash_traits_t::hash_t;

	static inline constexpr size_t encrypted_size = sbLibX::align_up( _LENGHT_ * sizeof( char_t ), sizeof( encrypt_type ) ) / sizeof( encrypt_type );
	using encrypted_array_t = std::array<encrypt_type, encrypted_size>;
	using decrypted_array_t = std::array<char_t, _LENGHT_>;

	inline constexpr encrypted_string_base( decrypted_array_t&& unencrypted_array )
		: encrypted_data( encrypt( unencrypted_array ) )
	{
	}

	inline constexpr const encrypt_type* data() const { return encrypted_data.data(); }
	inline constexpr size_t size() const { return encrypted_data.size(); }

	static inline constexpr encrypted_array_t encrypt( decrypted_array_t&& unencrypted_array )
	{
		encrypt_type next_scrambled{ hash_traits_t::coprime };
		encrypted_array_t encrypted{};
		for( size_t index = 0; index < encrypted.size(); ++index )
		{
			constexpr size_t encrypt_size = sizeof(encrypt_type)/sizeof(char_t);

			const int64_t remainder_lenght = static_cast<int64_t>(_LENGHT_) - static_cast<int64_t>( index * encrypt_size );
			const size_t next_unencrypted_length = std::min<size_t>( encrypt_size, std::max<int64_t>( remainder_lenght, 0 ) );

			for( size_t subindex = 0; subindex < encrypt_size; ++subindex )
			{
				constexpr size_t char_t_bit = CHAR_BIT * sizeof( char_t );
				const size_t unencrypted_index = index * encrypt_size + subindex;
				const auto next_unencrypted_char = subindex < next_unencrypted_length ? unencrypted_array[unencrypted_index] : encrypt_type{};
				const auto next_unencrypted_char_pos = ( subindex * char_t_bit );
				next_scrambled ^= ( next_unencrypted_char << next_unencrypted_char_pos );
			}
			encrypted[index] = ( next_scrambled * hash_traits_t::prime ) ^ hash_traits_t::coprime;
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

	template<typename _HASH_TRAITS_, typename char_t, size_t _LENGHT_, size_t ... _INDICES_>
static inline constexpr auto encrypted_string_unsecured( const char_t* unencrypted, [[maybe_unused]] std::index_sequence<_INDICES_...> const& )
{
	using string_t = encrypted_string_base<char_t, _HASH_TRAITS_, _LENGHT_>;
	using array_t = typename string_t::decrypted_array_t;
	return string_t( array_t{ {unencrypted[_INDICES_]...} } );
}

	template<typename char_t, size_t _LENGHT_>
static inline constexpr auto encrypted_string_unsecured( const char_t( &unencrypted )[_LENGHT_] )
{
	return encrypted_string_unsecured<sbLibX::xhash_traits_t, char_t, _LENGHT_>( unencrypted, std::make_index_sequence<_LENGHT_>{} );
}

//	template<typename char_t, typename _HASH_TRAITS_, size_t _LENGHT_>
//static inline constexpr auto encrypted_string_unsecured( const char_t* unencrypted, size_t max_lenght = size_t{-1} )
//{
//	return encrypted_string_unsecured<char_t, _HASH_TRAITS_, _LENGHT_>( unencrypted, max_lenght, std::make_index_sequence<_LENGHT_>{} );
//}

// While possibly wasting space with duplicates, at least this garantees that 'unencrypted' will never
// be in the final binary data since it forces encrypted value evaluated as a constexpr.
// TODO: Find a way to prevent data duplication while still enforcing constexpr evaluation
// (and completely stripping out unencrypted data).
#define constexpr_encrypt( unencrypted ) []() { constexpr auto encrypted = encrypted_string_unsecured( unencrypted ); return encrypted; }()


	template <typename _CHAR_TYPE_, typename _HASH_TRAITS_, size_t _LENGHT_>
struct decrypted_string_base
{
	using char_t = _CHAR_TYPE_;
	using hash_traits_t = _HASH_TRAITS_;
	using const_pointer = const char_t*;
	using encrypt_type = typename hash_traits_t::hash_t;

	static inline constexpr size_t encrypted_size = sbLibX::align_up( _LENGHT_ * sizeof( char_t ), sizeof( encrypt_type ) ) / sizeof( encrypt_type );
	using encrypted_array_t = std::array<encrypt_type, encrypted_size>;
	using decrypted_array_t = std::array<char_t, _LENGHT_>;

	inline constexpr const char_t* data() const { return decrypted_data.data(); }
	inline constexpr size_t size() const { return decrypted_data.size(); }

	inline constexpr decrypted_string_base( const encrypted_array_t& encrypted_array )
		: decrypted_data( decrypt( encrypted_array ) )
	{
	}

	static inline constexpr decrypted_array_t decrypt( const encrypted_array_t& encrypted_data )
	{
		return decrypt( encrypted_data.data(), encrypted_data.data() + encrypted_data.size() );
	}
	static inline constexpr decrypted_array_t decrypt( const encrypt_type* encrypted_data, const encrypt_type* encrypted_data_end )
	{
		encrypt_type prev_scrambled{ hash_traits_t::coprime };
		decrypted_array_t decrypted_array{};
		const size_t encrypted_data_size = static_cast<size_t>( encrypted_data_end - encrypted_data );
		for( size_t index = 0; index < encrypted_data_size; ++index )
		{
			constexpr size_t encrypt_size = sizeof(encrypt_type)/sizeof(char_t);
			const size_t max_decrypted_lenght = std::min( _LENGHT_, encrypted_data_size * encrypt_size );
			const int64_t remainder_lenght = static_cast<int64_t>( max_decrypted_lenght ) - static_cast<int64_t>( index * encrypt_size );
			const int64_t next_encrypt_size = std::min<int64_t>( encrypt_size, std::max<int64_t>( remainder_lenght, 0 ) );

			encrypt_type next_scrambled = ( encrypted_data[index] ^ hash_traits_t::coprime ) * hash_traits_t::inverse_prime;
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

	template<typename char_t, typename _HASH_TRAITS_, size_t _LENGHT_>
static inline constexpr auto decrypt( const encrypted_string_base<char_t, _HASH_TRAITS_, _LENGHT_>& encrypted )
{
	using string_t = decrypted_string_base<char_t, _HASH_TRAITS_, _LENGHT_>;
	return string_t{ encrypted };
}

	template<typename char_t, typename _HASH_TRAITS_, size_t _LENGHT_>
static inline std::ostream& operator <<( std::ostream& os, decrypted_string_base<char_t, _HASH_TRAITS_, _LENGHT_> decrypted )
{
	return os << decrypted.data();
}
	template<typename char_t, typename _HASH_TRAITS_, size_t _LENGHT_>
static inline std::ostream& operator <<( std::ostream& os, encrypted_string_base<char_t, _HASH_TRAITS_, _LENGHT_> encrypted )
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

	constexpr encrypted_string_view( const char_t* unencrypted, size_t lenght ) noexcept
		: encrypted_begin(), encrypted_end()
	{
		encrypted_string_unsecured<char_t, hash_traits_t, hash_traits_t::default_length>( unencrypted, lenght );
	}

		template< size_t _LENGHT_ >
	constexpr encrypted_string_view( const encrypted_string_base<char_t, hash_traits_t, _LENGHT_>& encrypted ) noexcept
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

//	template <class _CHAR_TYPE_, class _TRAITS_TYPE_ = std::char_traits<_CHAR_TYPE_>>
//inline std::ostream& operator << ( std::ostream& os, encrypted_string_view< _CHAR_TYPE_, _TRAITS_TYPE_> encripted_string_view )
//{
//	return os << encripted_string_view;
//}
#endif



}} // namespace SB::LibX
namespace sbLibX = SB::LibX;
