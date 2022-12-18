#include <common/include/sb_common.h>
#include <common/include/sb_utilities.h>
#include <common/include/sb_math_base.h>

#if defined(_M_X64)
#include <intrin.h>
#else
#error "unsupported platform"
#endif

// Why TF is it so sub-optimized???
// In the end, +=/-= end up in a single tight loop with a fixed constexpr size!

#if 0
// Interesting intrisincs to be used here
__mulh : all
_mul128 : all
_div128 : all
_addcarry_u64 : all
_subborrow_u64 : all
// with AVX2 support
_mm256_add_epi64 : avx2
_mm256_sub_epi64 : avx2
__m256i _mm256_mul_epi32 : avx2
__m256i _mm256_mul_epu32 : avx2
#endif


#include <compare>

#if defined(_M_IX86) || defined(_M_X64)
// TODO : de-hardcode this and cleanup the arch_traits... it was a bad design after all.
//#define SB_AVX2 1
// Assumes AVX2 support for this
//using unsigned_int128_t = __m128i;
//using unsigned_int256_t = __m256i;
#endif

#ifdef __clang__
#define clang_pragma( ... ) __pragma( clang __VA_ARGS__ )
#define SB_CLANG_MESSAGE( ... ) __pragma( message( __VA_ARGS__ ) )
SB_CLANG_MESSAGE( "TODO" )
#else

namespace SB { namespace LibX {

#if defined(_M_X64) && defined( SB_AVX2 )
#define SB_INT128	SB_SUPPORTED
#define SB_INT256	SB_SUPPORTED
#else
#define SB_INT128	SB_UNSUPPORTED
#define SB_INT256	SB_UNSUPPORTED
#endif

//////////////////////////////////////////////////////////////////////////
	template< size_t _BIT_COUNT>
struct unsigned_aligned_integer_traits
{
	using unsigned_t = typename unsigned_aligned_integer_traits<_BIT_COUNT / 2>::unsigned_t;
	static_assert( !std::is_signed_v<unsigned_t> );
};

#define SB_UNSIGNED_TYPE( _BIT_COUNT, _NATIVE_T ) template<> struct unsigned_aligned_integer_traits<_BIT_COUNT> { using unsigned_t = std::make_unsigned_t<_NATIVE_T>; }
template<> struct unsigned_aligned_integer_traits<~0ull> { using unsigned_t = std::integral_constant<size_t, 0>; }; // -1 means dynamic range here so that 0 traps unexpected convergeance to 0
template<> struct unsigned_aligned_integer_traits<0> {}; // undefined 0-length integer type (trivial field or the infinite range?)
SB_UNSIGNED_TYPE( 1,  uint8_t );
SB_UNSIGNED_TYPE( 8,  uint8_t );
SB_UNSIGNED_TYPE( 16, uint16_t );
SB_UNSIGNED_TYPE( 32, uint32_t );
SB_UNSIGNED_TYPE( 64, uint64_t );
#if SB_SUPPORTS( SB_INT128 )
SB_UNSIGNED_TYPE( 128, __m128i; );
#endif
#if SB_SUPPORTS( SB_INT256 )
SB_UNSIGNED_TYPE( 256, __m256i; );
#endif

	template< size_t _BIT_COUNT  >
using unsigned_int_t = typename unsigned_aligned_integer_traits<ceil_pow2(_BIT_COUNT)>::unsigned_t;
static_assert( ceil_pow2(-1) == 0 );

}}
static_assert( std::is_same_v<sbLibX::unsigned_int_t<1>,    uint8_t> );
static_assert( std::is_same_v<sbLibX::unsigned_int_t<16>,   uint16_t> );
static_assert( std::is_same_v<sbLibX::unsigned_int_t<31>,   uint32_t> );
static_assert( std::is_same_v<sbLibX::unsigned_int_t<63>,   uint64_t> );
#if SB_SUPPORTS( SB_INT128 )
static_assert( std::is_same_v<sbLibX::unsigned_int_t<65>,   unsigned __m128i> );
#else
static_assert( std::is_same_v<sbLibX::unsigned_int_t<65>, uint64_t> );
#endif
#if SB_SUPPORTS( SB_INT256 )
static_assert( std::is_same_v<sbLibX::unsigned_int_t<1024>, unsigned __m256i> );
#else
static_assert( std::is_same_v<sbLibX::unsigned_int_t<1024>, sbLibX::unsigned_int_t<255>> );
#endif


#include <array>
#include <vector>

namespace SB { namespace LibX {

//////////////////////////////////////////////////////////////////////////
	template< typename _NATIVE_T, size_t _BIT_COUNT, typename _ALLOCATOR_T = std::allocator<std::make_unsigned_t<_NATIVE_T>> >
struct unsigned_raw_array_traits
{
	static_assert( std::is_integral_v<_NATIVE_T>, "Non-integral native types are not available" );
	using unsigned_t = std::make_unsigned_t<_NATIVE_T>;
	static inline constexpr size_t bit_count = _BIT_COUNT;
	static inline constexpr size_t element_count = bit_count / ( sizeof(unsigned_t) * CHAR_BIT );
	using allocator_t = _ALLOCATOR_T;
	using array_t = std::conditional_t< element_count == 0, std::vector<unsigned_t, allocator_t>, std::array<unsigned_t, element_count> >;
	using data_t = std::conditional_t< element_count == 1, unsigned_t, array_t >;
};

	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr size_t size( const typename _TRAITS::data_t& value ) noexcept
{
	return value.size();
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count == 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr size_t size( [[maybe_unused]] const typename _TRAITS::data_t& value ) noexcept
{
	return 1;
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto data( const typename _TRAITS::data_t& value ) noexcept
{
	return value.data();
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count == 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto data( const typename _TRAITS::data_t& value ) noexcept
{
	return &value;
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto data( typename _TRAITS::data_t& value ) noexcept
{
	return value.data();
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count == 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto data( typename _TRAITS::data_t& value ) noexcept
{
	return &value;
}

	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 0, size_t > _COUNT = _TRAITS::element_count >
static inline constexpr size_t resize( [[maybe_unused]] typename _TRAITS::data_t& value, [[maybe_unused]] size_t element_count )
{
	// fixed size: truncate value if element_count > _TRAITS::element_count
	return size<_TRAITS>( value );
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count == 0, size_t > _COUNT = _TRAITS::element_count >
static inline constexpr size_t resize( typename _TRAITS::data_t& value, size_t element_count ) noexcept
{
	// dynamic size:
	value.resize( element_count );
	return size<_TRAITS>( value );
}

// direct element access
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto get( const typename _TRAITS::data_t& value, size_t index ) noexcept
{
	return index < value.size() ? &value[index] : nullptr;
}

	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto get( typename _TRAITS::data_t& value, size_t index ) noexcept
{
	return index < value.size() ? &value[index] : nullptr;
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count == 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto get( typename _TRAITS::data_t& value, [[maybe_unused]] size_t index ) noexcept
{
	return &value;
}

// begin/end iterators
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto begin( typename _TRAITS::data_t& value ) noexcept
{
	return value.begin();
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count == 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto begin( typename _TRAITS::data_t& value ) noexcept
{
	return &value;
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto end( typename _TRAITS::data_t& value ) noexcept
{
	return value.end();
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count == 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto end( typename _TRAITS::data_t& value ) noexcept
{
	return &value + 1;
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto begin( const typename _TRAITS::data_t& value ) noexcept
{
	return value.begin();
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count == 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto begin( const typename _TRAITS::data_t& value ) noexcept
{
	return &value;
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto end( const typename _TRAITS::data_t& value ) noexcept
{
	return value.end();
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count == 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto end( const typename _TRAITS::data_t& value ) noexcept
{
	return &value + 1;
}
// rbegin/rend iterators
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto rbegin( typename _TRAITS::data_t& value ) noexcept
{
	return value.rbegin();
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count == 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto rbegin( typename _TRAITS::data_t& value ) noexcept
{
	return &value;
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto rend( typename _TRAITS::data_t& value ) noexcept
{
	return value.rend();
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count == 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto rend( typename _TRAITS::data_t& value ) noexcept
{
	return &value + 1;
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto rbegin( const typename _TRAITS::data_t& value ) noexcept
{
	return value.rbegin();
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count == 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto rbegin( const typename _TRAITS::data_t& value ) noexcept
{
	return &value;
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count != 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto rend( const typename _TRAITS::data_t& value ) noexcept
{
	return value.rend();
}
	template< typename _TRAITS, std::enable_if_t< _TRAITS::element_count == 1, size_t > element_count = _TRAITS::element_count >
static inline constexpr auto rend( const typename _TRAITS::data_t& value ) noexcept
{
	return &value + 1;
}


// Allows writing over out-of-range iterators without side effect unless references or pointers are kept.
// Any assignment should be immediate or any subsequent access to an out_of_range might invalidate any change.
// It is however thread-safe so its value is guaranteed to be valid
// until the running thread makes another out_of_range access
// and the value will be reset to 0 and possibly modified thereafter.
	template< typename _TYPE_T >
class out_of_range
{
public:
	using type_t = std::remove_cvref_t<_TYPE_T>;
	static inline type_t/*&*/ get() noexcept
	{
		// This should be emit a warning (but not break in ship).
		// While this behaves well locally,
		// it would behave very badly if any reference to this element is kept
		// and is modified while other references are also refenreced.
#if defined( SBDEBUG ) && SBDEBUG
		__debugbreak();
#endif
		return ( /*zero =*/ type_t{} );
	}
private:
	//thread_local static inline type_t zero{};
};


// Raw integers are plain data in basis 2^bit_count,
// with bit_count align to the given native underlying type.
// The default native type is the most suitable unsigned type for bit_count bits.
// It is not meant to define arithmetic operations but only store data and convert
// between different integer native (raw binary data) representations.
//
// TODO : support digital basis of characteristic N through trait.
// This will introduce a carry step in load, which should still be of linear complexity.
	template< size_t _BIT_COUNT = 0
	        , typename _NATIVE_T = std::conditional_t<_BIT_COUNT == 0, unsigned_int_t<~0ull>, unsigned_int_t<_BIT_COUNT>>
	        , typename _INTEGER_TRAITS = unsigned_raw_array_traits< _NATIVE_T, _BIT_COUNT > >
struct raw_integer
{
	static_assert( std::is_integral_v<_NATIVE_T>, "Integers needs to be of integer values" );
	using traits_t = _INTEGER_TRAITS;
	using native_t = _NATIVE_T;
	using unsigned_t = typename traits_t::unsigned_t;
	using data_t = typename traits_t::data_t;
	data_t value{};
	static inline constexpr native_t sign_value = native_t(-1);
	static inline constexpr bool is_signed_value = ( sign_value < 0 );
	static inline constexpr size_t digit_count = sbLibX::div_align_up( _BIT_COUNT, sizeof(native_t) * CHAR_BIT );
	static_assert( _BIT_COUNT == 0 || sizeof(data_t) == digit_count * sizeof(native_t) );

	constexpr raw_integer() = default;

		template< typename _INIT_T, typename ..._OTHER_T >
	constexpr raw_integer( _INIT_T most_significant, _OTHER_T... least_significant )
		: value( load( most_significant, std::forward<_OTHER_T>(least_significant)... ) )
	{
		// This assert could be removed but that implies implicit conversions, which usually mean a code error
		static_assert( false, "All initializers must be of the same integer type." );
	}

		template< typename _INIT_T >
	constexpr raw_integer( std::initializer_list<_INIT_T> _init_list )
		: value( load( _init_list.begin(), _init_list.size() ) ) {}

		template< typename _INIT_T >
	constexpr raw_integer( const _INIT_T* _data, size_t _size )
		: value{ load( _data, _size ) } {}

		template< size_t _OTHER_BIT_COUNT = 0, typename _OTHER_NATIVE_T >
	constexpr raw_integer( const raw_integer<_OTHER_BIT_COUNT, _OTHER_NATIVE_T>& _other )
		: value{ load( _other.data(), _other.size() ) } {}

		template< size_t _OTHER_BIT_COUNT = 0, typename _OTHER_NATIVE_T >
	constexpr raw_integer( const typename raw_integer<_OTHER_BIT_COUNT, _OTHER_NATIVE_T>::data_t& _other )
		: value{ load( _other.data(), _other.size() ) } {}

	constexpr size_t size() const noexcept { return sbLibX::size<traits_t>( value ); }
	constexpr unsigned_t get( size_t little_endian_index ) const noexcept { return sbLibX::get<traits_t>( value, little_endian_index ) ? *sbLibX::get<traits_t>( value, little_endian_index ) : unsigned_t{}; }
	constexpr unsigned_t& get( size_t little_endian_index ) noexcept { return /*sbLibX::get<traits_t>( value, index ) ?*/ *sbLibX::get<traits_t>( value, little_endian_index ) /*: out_of_range<unsigned_t>::get()*/; }

	constexpr unsigned_t get_digit( size_t big_endian_index ) const noexcept { auto index = size() - big_endian_index - 1; return sbLibX::get<traits_t>( value, index ) ? *sbLibX::get<traits_t>( value, index ) : unsigned_t{}; }
	constexpr unsigned_t& get_digit( size_t big_endian_index ) noexcept { auto index = size() - big_endian_index - 1; return /*sbLibX::get<traits_t>( value, index ) ?*/ *sbLibX::get<traits_t>( value, index ) /*: out_of_range<unsigned_t>::get()*/; }
	unsigned_t operator []( size_t big_endian_index ) const noexcept { return get_digit( big_endian_index ); }
	unsigned_t& operator []( size_t big_endian_index ) noexcept { return get_digit( big_endian_index ); }

	constexpr void resize( size_t required_capacity ) noexcept { sbLibX::resize<traits_t>( value, required_capacity ); }
	constexpr const unsigned_t* data() const noexcept { return sbLibX::data<traits_t>( value ); }
	constexpr unsigned_t* data() noexcept { return sbLibX::data( value ); }

	constexpr auto begin() const { return sbLibX::begin<traits_t>( value ); }
	constexpr auto end() const { return sbLibX::end<traits_t>( value ); }
	constexpr auto begin() { return sbLibX::begin<traits_t>( value ); }
	constexpr auto end() { return sbLibX::end<traits_t>( value ); }

	static inline constexpr const data_t& load( const data_t& data ) { return data; }
		template< typename _INT_T, size_t _LENGTH >
	static inline constexpr data_t load( const _INT_T ( &_data )[_LENGTH] ) { return convert( _data, _LENGTH ); }
		template< typename _INIT_T, typename ..._OTHER_T >
	static inline constexpr auto load_truncate( _INIT_T most_significant, _OTHER_T... least_significant )
	-> std::enable_if_t<std::is_integral_v<_INIT_T>, data_t>
	{
		// TODO : maybe should static_assert all initializer types are the same integer type?
		// for now, it will just convert the value
		using init_t = std::make_unsigned_t<_INIT_T>;
		init_t init_data[] = { init_t(most_significant), init_t(std::forward<_OTHER_T>(least_significant))... };
		return load( init_data );
	}
		template< typename _INIT_T, typename ..._OTHER_T >
	static inline constexpr auto load( _INIT_T most_significant, _OTHER_T... least_significant )
		-> std::enable_if_t<std::is_integral_v<_INIT_T>, data_t>
	{
		static_assert( traits_t::element_count == 0 || ( 1 + sizeof...( _OTHER_T ) ) * CHAR_BIT <= _BIT_COUNT, "Too many initializers for fixed size integer. If you want to allow truncating values, use load_truncate instead." );
		return load_truncate( most_significant, std::forward<_OTHER_T>( least_significant )... );
	}
		template< typename _INT_T >
	static inline constexpr data_t load( const _INT_T* _data, size_t _length )
	{
		data_t result = convert( _data, _length );
		return result;
	}

private:
		template< typename _INT_T, std::enable_if_t< std::is_same_v<_INT_T, unsigned_t>, size_t > = 0 >
	static inline constexpr data_t convert( const _INT_T* _data, size_t _length )
	{
		// Note that conversion will truncate the value.
		// The operation is considered modulo 2^(bit_count) unless bit_count is 0.
		// If no-truncation validation is required, 
		data_t result; // only the first 'most_significant_index' entries must be cleared to zero, everything else will be overwritten
		const size_t result_size = ::resize<traits_t>( result, _length );
		const size_t most_significant_index = _length > result_size ? 0 : result_size - _length;
		std::fill( ::begin<traits_t>( result ), ::begin<traits_t>( result ) + most_significant_index, 0 );

		const size_t truncate_index = _length > result_size ? _length - result_size : 0;
		for( size_t index = 0; index < _length - truncate_index; ++index )
			*::get<traits_t>(result, index + most_significant_index) = _data[index + truncate_index];
		return result;
	}
		template< typename _INT_T, std::enable_if_t< !std::is_same_v<std::make_unsigned_t<_INT_T>, unsigned_t>, size_t > = 0 >
	static inline constexpr data_t convert( const _INT_T* _data, size_t _length )
	{
		static_assert( std::is_integral_v<_INT_T>, "Integers needs to be initialized from integer values" );
		data_t result{}; // needs to be cleared
		using init_t = _INT_T;
		const size_t required_length = sbLibX::div_align_up( _length * sizeof(init_t), sizeof(unsigned_t) );
		const size_t result_size = sbLibX::resize<traits_t>( result, required_length );

		enum {
			data_stride = sizeof( init_t ) > sizeof( unsigned_t ) ? sizeof( init_t ) / sizeof( unsigned_t ) : 1,
			init_stride = sizeof( unsigned_t ) > sizeof( init_t ) ? sizeof( unsigned_t ) / sizeof( init_t ) : 1
		};
		const size_t most_significant_index = result_size - required_length;
		const size_t most_significant_index_offset = ((result_size * init_stride - _length) % init_stride);
		for( size_t index = 0; index < _length; ++index )
		{
			const size_t shifted_index = (index + most_significant_index_offset);
			const size_t data_shift = ( init_stride - ( shifted_index % init_stride ) - 1 ) * sizeof( init_t ) * CHAR_BIT;

			const size_t data_index_begin = sbLibX::div_align_down( shifted_index * sizeof( init_t ), sizeof( unsigned_t ) );
			const size_t data_index_end = sbLibX::div_align_up( ( shifted_index + 1 ) * sizeof( init_t ), sizeof( unsigned_t ) );
			for( size_t data_index = data_index_begin; data_index < data_index_end; ++data_index )
			{
				const size_t init_shift = ( data_stride - ( data_index % data_stride ) - 1 ) * sizeof( unsigned_t ) * CHAR_BIT;
				const unsigned_t data_mask = unsigned_t( ( _data[index] >> init_shift ) & type_mask<init_t, unsigned_t>() );
				if( auto digit = sbLibX::get<traits_t>( result, most_significant_index + data_index ) )
					*digit |= ( data_mask << data_shift );
			}
		}
		return result;
	}
};


	template< typename _FIRST_T, typename _SECOND_T, typename ..._OTHERS_T >
struct second_default_t
{
	using second_t = _SECOND_T;
	using tuple_ref_t = std::tuple<_FIRST_T, _SECOND_T, _OTHERS_T...>;
	using tuple_t = std::tuple<std::remove_cvref_t<_FIRST_T>, std::remove_cvref_t<_SECOND_T>, std::remove_cvref_t<_OTHERS_T>...>;
	tuple_ref_t value{};

	operator const second_t&( ) const noexcept
	{
		return std::get<1>( value );
	}
};

	template< typename _FIRST_T, typename _SECOND_T, typename ..._OTHERS_T >
auto as_tuple( const second_default_t<_FIRST_T, _SECOND_T, _OTHERS_T...>& data )
{
	return data.value;
}
	template< typename _FIRST_T, typename _SECOND_T, typename ..._OTHERS_T >
auto as_tuple( second_default_t<_FIRST_T, _SECOND_T, _OTHERS_T...>&& data )
{
	return data.value;
}

static inline auto add_carry( uint64_t& result, uint64_t a, uint64_t b, uint8_t carry )
{
	// assumes carry is a flag as it is used as a 8-bit bool as an intrisic
	return _addcarry_u64( carry, a, b, &result );
}

	template< typename _CARRY_T, size_t _BIT_COUNT, typename _NATIVE_T, typename _TRAITS >
auto add_carry( raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS >& result, const raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS >& a, const raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS >& b, _CARRY_T carry )
{
	const auto size = std::max( a.size(), b.size() );
	const auto a_size = a.size(); // a and result might have same address
	const auto b_size = a.size(); // b and result might have same address
	result.resize( size );
	const auto a_offset = a.size() - a_size; // a might have been resized
	const auto b_offset = b.size() - b_size; // b might have been resized
	for( auto index = 0; index < size; ++index )
		carry = add_carry( result.get_digit(index), a.get_digit(index + a_offset), b.get_digit(index + b_offset), carry );
	return carry;
}

static inline auto sub_borrow( uint64_t& result, uint64_t a, uint64_t b, uint8_t borrow )
{
	return _subborrow_u64( borrow, a, b, &result );
}
	template< typename _BORROW_T, size_t _BIT_COUNT, typename _NATIVE_T, typename _TRAITS, std::enable_if_t<_BIT_COUNT == 0, size_t> = 0 >
auto sub_borrow( raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS >& result, const raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS >& a, const raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS >& b, _BORROW_T borrow )
{
	const auto size = std::max( a.size(), b.size() );
	if( a.size() != b.size() )
	{
		// preparation / move (not required for fixed-size integers)
		const auto a_size = a.size(); // a and result might have same address
		const auto b_size = b.size(); // b and result might have same address
		result.resize( size );
		if( a.size() != a_size || b.size() != b_size )
		{
			// either a or b shares memory with result : data must be moved
			const size_t offset = std::max( a.size() - a_size, b.size() - b_size );
			// it is well defined to read out of bound, which will correctly zero-extend
			using data_t = std::remove_cvref_t<decltype( result )>;
			using const_data_t = std::add_const_t<data_t>;
			const auto in_place_move = [offset, size]( data_t& data, const_data_t& const_data )
			{
				for( size_t index = 0; index < size; ++index )
				{
					auto const_digit = const_data.get_digit( index + offset );
					data.get_digit( index ) = const_digit;
				}
			};
			in_place_move( result, result );
		}
	}

	const auto a_size = a.size(); // a size might have changed
	const auto b_size = b.size(); // b size might have changed
	const auto a_offset = a_size > b_size ? 0 : size - a_size;
	const auto b_offset = b_size > a_size ? 0 : size - b_size;
	for( auto index = 0; index < size; ++index )
	{
		// it is well defined to read out of bound, which will correctly zero-extend
		const auto a_value = a.get_digit( index );
		const auto b_value = b.get_digit( index );
		borrow = sub_borrow( result.get_digit( index ), a_value, b_value, borrow );
	}
	return borrow;
}
template< typename _BORROW_T, size_t _BIT_COUNT, typename _NATIVE_T, typename _TRAITS, std::enable_if_t<_BIT_COUNT != 0, size_t> = 0 >
auto sub_borrow( raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS >& result, const raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS >& a, const raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS >& b, _BORROW_T borrow )
{
	using raw_t = raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS >;
	for( auto index = 0; index < a.value.size(); ++index )
	{
		// it is well defined to read out of bound, which will correctly zero-extend
		const auto a_value = a[index];
		const auto b_value = b[index];
		borrow = sub_borrow( result[index], a_value, b_value, borrow );
	}
	return borrow;
}

//static inline auto multiply( this_type& reult, const this_type& first, const this_type& second ) { return ::multiply( result, first, second ); }
static inline auto multiply( int64_t& result_hi, int64_t& result_low, int64_t a, int64_t b )
{
	result_low = _mul128( a, b, &result_hi );
	return std::make_tuple( result_hi, result_low );
}

	template< typename _TYPE_T >
using pair_type = std::tuple<_TYPE_T, _TYPE_T>;
	template< size_t _BIT_COUNT, typename _NATIVE_T, typename _TRAITS = unsigned_raw_array_traits< _NATIVE_T, _BIT_COUNT >, typename _INT_T = raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS > >
const auto& multiply( pair_type<_INT_T>& result, const raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS >& a, const raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS >& b )
{
	using int_t = _INT_T;
	using pair_t = pair_type<int_t>;//_PAIR_T;
	using raw_int_t = raw_integer< _BIT_COUNT, _NATIVE_T, _TRAITS >;
	using native_t = typename raw_int_t::native_t;
	// TODO: brute force multiply as generic implementation
	auto& [result_hi, result_low] = result;
	
	const size_t size_a = a.size();
	const size_t size_b = a.size();
	const size_t max_size = std::max( size_a, size_b );
	result_hi.value.resize( max_size );
	result_low.value.resize( max_size );
	static_assert( std::is_same_v< pair_t, pair_type<int_t> > );

	(void) a,b;
	//int_t& result_hi = std::get<0>( result );
	//int_t& result_low = std::get<1>( result );
	////auto& [result_hi, result_low] = result;
	result_hi = result_low = int_t{{-1}};
	return result;
}


	template< typename _NATIVE_T, size_t _NATIVE_COUNT = 0 /* 0 is dynamic length */, typename _DATA_TRAITS = unsigned_raw_array_traits< _NATIVE_T, _NATIVE_COUNT * sizeof(_NATIVE_T) * CHAR_BIT > >
struct integer
{
	static inline constexpr size_t native_count = _NATIVE_COUNT;
	using native_t = _NATIVE_T;
	using traits_t = _DATA_TRAITS;
	static_assert( traits_t::bit_count >= _NATIVE_COUNT * sizeof(_NATIVE_T) * CHAR_BIT, "Container is not large enough to represent all values" );
	using data_t = raw_integer< traits_t::bit_count, _NATIVE_T, traits_t >;
	data_t value;

	constexpr auto data() const noexcept { return value.data(); }
	constexpr auto data() noexcept { return value.data(); }
	constexpr auto size() const noexcept { return value.size(); }
	constexpr auto begin() const noexcept { return value.begin(); }
	constexpr auto begin() noexcept { return value.begin(); }
	constexpr auto end() const noexcept { return value.end(); }
	constexpr auto end() noexcept { return value.end(); }

	native_t get( size_t little_endian_index ) const noexcept { return value.get( little_endian_index ); }
	native_t& get( size_t little_endian_index ) noexcept { return value.get( little_endian_index ); }
	native_t operator []( size_t big_endian_index ) const noexcept { return value.get_digit( big_endian_index ); }
	native_t& operator []( size_t big_endian_index ) noexcept { return value.get_digit( big_endian_index ); }

	using this_type = integer<native_t, _NATIVE_COUNT, traits_t>;
	using carry_borrow_type = make_signof_t<uint8_t, native_t>;
	using carry_result_type = second_default_t< carry_borrow_type, const this_type& >;
	static inline auto add_carry( this_type& result, const this_type& first, const this_type& second, carry_borrow_type carry = carry_borrow_type{} )
	{
		carry = sbLibX::add_carry( result.value, first.value, second.value, carry );
		return carry_result_type{ std::tie( carry, result ) };
	}
	static inline auto add_carry( const this_type& first, const this_type& second ) { this_type result; return add_carry( result, first, second ); }
	constexpr carry_result_type operator +=( const this_type& other ) noexcept { return add_carry( *this, *this, other ); }
	static inline auto sub_borrow( this_type& result, const this_type& first, const this_type& second, carry_borrow_type borrow = carry_borrow_type{} )
	{
		borrow = sbLibX::sub_borrow( result.value, first.value, second.value, borrow );
		return carry_result_type{ std::tie( borrow, result ) };
	}
	static inline auto sub_borrow( const this_type& first, const this_type& second ) { this_type result; return sub_borrow( result, first, second ); }
	constexpr carry_result_type operator -=( const this_type& other ) noexcept { return sub_borrow( *this, *this, other ); }

	using pair_t = pair_type< this_type >;
	static inline pair_t multiply( const this_type& first, const this_type& second ) { pair_t result; return multiply( result, first, second ); }
	static inline const pair_t& multiply( pair_t& result, const this_type& first, const this_type& second ) { return sbLibX::multiply( result, first.value, second.value ); }
	constexpr pair_t operator *=( const this_type& other ) noexcept { return multiply( *this, other ); }

	constexpr this_type operator /=( const this_type& other ) noexcept;
	constexpr this_type operator %=( const this_type& other ) noexcept;
};

}}

// Interesting intrisincs to be used here
//__mulh: all
//_mul128 : all
//_div128 : all
//_addcarry_u64 : all
//_subborrow_u64 : all

//	template< typename _NATIVE_T, size_t _NATIVE_COUNT = 0 /* 0 is dynamic length */, typename _DATA_TRAITS = sbLibX::unsigned_raw_array_traits< _NATIVE_T, _NATIVE_COUNT * sizeof( _NATIVE_T ) * CHAR_BIT > >
//using unsigned_raw_array_traits = sbLibX::unsigned_raw_array_traits<_NATIVE_T, _NATIVE_COUNT, _DATA_TRAITS>;
	template< typename _NATIVE_T, size_t _NATIVE_COUNT = 0 /* 0 is dynamic length */, typename _DATA_TRAITS = sbLibX::unsigned_raw_array_traits< _NATIVE_T, _NATIVE_COUNT * sizeof( _NATIVE_T ) * CHAR_BIT > >
using integer = sbLibX::integer<_NATIVE_T, _NATIVE_COUNT, _DATA_TRAITS>;

#pragma optimize("", off)
#include <iostream>
#include <iomanip>
	template< typename _NATIVE_T, size_t _NATIVE_COUNT = 0 /* 0 is dynamic length */, typename _DATA_TRAITS = unsigned_raw_array_traits< _NATIVE_T, _NATIVE_COUNT * sizeof( _NATIVE_T ) * CHAR_BIT > >
std::enable_if_t<std::is_unsigned_v<_NATIVE_T>, std::ostream&> operator << ( std::ostream& os, const integer<_NATIVE_T, _NATIVE_COUNT, _DATA_TRAITS>& value )
{
	for( auto c : value )
		os << std::hex << std::setw( sizeof( c ) * ( CHAR_BIT / 4 ) ) << std::setfill( '0' ) << c;
	return os;
}
	template< typename _NATIVE_T, size_t _NATIVE_COUNT = 0 /* 0 is dynamic length */, typename _DATA_TRAITS = unsigned_raw_array_traits< _NATIVE_T, _NATIVE_COUNT * sizeof( _NATIVE_T ) * CHAR_BIT > >
std::enable_if_t<std::is_signed_v<_NATIVE_T>, std::ostream&> operator << ( std::ostream& os, const integer<_NATIVE_T, _NATIVE_COUNT, _DATA_TRAITS>& value )
{
	//static_assert( false, "not implemented yet: required division" );
	using native_t = _NATIVE_T;
	using int_t = integer<native_t, _NATIVE_COUNT, _DATA_TRAITS>;
	//using carry_t = typename int_t::carry_borrow_type;
	//{0xFEFE, 0xDCDC} : 2^16 -> 10
	// 2^16 * ( 15 * 2^(16 - 4) + 14 * 2^(16 - 8) + 15 * 2^(16 - 12) + 14 * 2^(16 - 16) )
	//   +    ( 13 * 2^(16 - 4) + 12 * 2^(16 - 8) + 13 * 2^(16 - 12) + 12 * 2^(16 - 16) )
	//carry_t sign = value.get(0) < 0 ? -1 : 1;
	//carry_t borrow = sign < 0 ? 1 : 0;
	for( auto c : value.value )
	{
		os << std::hex << std::setw( sizeof( c ) * ( CHAR_BIT / 4 ) ) << std::setfill( '0' ) << c;
	}
	return os;
}
#pragma optimize("", on)

#include <common/include/sb_common.h>
SB_EXPORT_TYPE int SB_STDCALL test_int_type( [[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[] )
{
#if 0
	using int_t = integer<int64_t, 4>;
	int_t a0{{ 0x00000001,0x00000000,0x00000000,0x00000008 }};
	int_t b0{{ 0x00000001 }};
	a0 *= b0;

	//std::cout << "   " << b0 << std::endl;
	//std::cout << " - " << a0 << std::endl;
	_ReadWriteBarrier();
	auto [borrow, result0] = as_tuple( b0 -= a0 );
	_ReadWriteBarrier();
	//std::cout << " = " << b0 << std::endl;

	//std::cout << "   " << a0 << std::endl;
	//std::cout << " - " << b0 << std::endl;
	int_t result1 = (a0 += b0);
	_ReadWriteBarrier();
	//std::cout << " = " << result1 << std::endl;
#endif

#if 0 // basic data intitialisation tests
	using small_int_t = raw_integer<128, int16_t>;
	using fixed_int_t = raw_integer<256>; // 256-bits fits 8 ints
	using fixed_int8_t = raw_integer<256, int8_t>; // 256-bits fits 8 ints
	using dyn_int_t = raw_integer<0>;
	small_int_t small_zero{};
	for( auto c : small_zero.value )
		std::cout << std::hex << std::setw(4) << std::setfill('0') << c;
	std::cout << std::endl;
	small_int_t small_two_long = { 0x00010203, 0x04050607, 0x08090A0B, ~0x0C0D0E0F };
	for( auto c : small_two_long.value )
		std::cout << std::hex << std::setw(4) << std::setfill('0') << c;
	std::cout << std::endl;
	small_int_t small_two_char = { '\x00','\x01','\x02','\x03', '\x04','\x05','\x06','\x07', '\x08','\x09','\x0A','\x0B', '\xF3','\xF2','\xF1','\xF0' };
	for( auto c : small_two_char.value )
		std::cout << std::hex << std::setw(4) << std::setfill('0') << c;
	std::cout << std::endl;

	small_int_t small_two_sum = small_two_long;
	//small_two_sum += small_two_char;
	for( auto c : small_two_sum.value )
		std::cout << std::hex << std::setw( 4 ) << std::setfill( '0' ) << c;
	std::cout << std::endl;

	fixed_int_t fixed_zero{};
	fixed_int_t fixed_one{1};
	fixed_int_t fixed_7{ 1,2,3,4,5,6,7 };
	fixed_int8_t fixed_8{ 1,2,3,4,5,6,7,8 };
	//if constexpr (
	//	std::enable_if_t<
	//		  fixed_int_t{}.out_of_range
	//		, std::true_type
	//	>::value
	//)
	//{
	//	//ok
	//}
	//fixed_int_t fixed_11{ 1,2,3,4,5,6,7,8, 9,10,11 }; // should give 'too many initializers'
	//fixed_int_t fixed_16{ 1,2,3,4,5,6,7,8, 9,10,11,12,13,14,15,16 }; // should give 'too many initializers'
	dyn_int_t   dyn_11  { 1,2,3,4,5,6,7,8, 9,10,11 };
	dyn_int_t   dyn_7	{ 1,2,3,4,5,6,7, };

	dyn_int_t dyn(fixed_7);
	//dyn += dyn_11;
	for( auto c : dyn )
		std::cout << std::hex << std::setw( sizeof(c) * CHAR_BIT / 4 ) << std::setfill( '0' ) << c; // (hex is a 4-bits digit)
	std::cout << std::endl;

	raw_integer<0, int16_t> dyn_cast( dyn_7 );
	//dyn += dyn_11;
	for( auto c : dyn_cast )
		std::cout << std::hex << std::setw( sizeof( c ) * CHAR_BIT / 4 ) << std::setfill( '0' ) << c; // (hex is a 4-bits digit)
	std::cout << std::endl;

	using odd_int_t = raw_integer<7*CHAR_BIT, int8_t>;
	static_assert( sizeof(odd_int_t) == 7 && alignof(odd_int_t) == 1 );
	odd_int_t small_from_dyn(dyn_cast);
	for( auto c : small_from_dyn )
		std::cout << std::hex << std::setw( sizeof( c ) * CHAR_BIT / 4 ) << std::setfill( '0' ) << uint64_t(c); // (hex is a 4-bits digit)
	std::cout << std::endl;
#endif

	return 0;
}
#endif // #ifdef __clang__
