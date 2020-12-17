#pragma once

namespace SB { namespace LibX
{
#if defined(__clang__)
#define SBCLANG_PUSH_IGNORE_UNUSED() \
	__pragma( clang diagnostic push ) \
	__pragma( clang diagnostic ignored "-W" "unneeded-internal-declaration" ) \
	__pragma( clang diagnostic ignored "-W" "unused-function" ) \
	__pragma( clang diagnostic ignored "-W" "unused-template" ) \
	__pragma( clang diagnostic ignored "-W" "unused-local-typedef" )

#define SBCLANG_POP_IGNORE() \
	__pragma( clang diagnostic pop )

#else
#define SBCLANG_PUSH_IGNORE_UNUSED( ... )
#define SBCLANG_POP_IGNORE( ... )
#endif

////
#define sb_enum_class_flags( ENUM_CLASS ) \
SBCLANG_PUSH_IGNORE_UNUSED() \
static inline constexpr ENUM_CLASS operator                         & ( ENUM_CLASS a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) & type_t(b) }; } \
static inline constexpr ENUM_CLASS operator                         &= ( ENUM_CLASS& a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a = ENUM_CLASS{ type_t(a) & type_t(b) }; } \
static inline constexpr ENUM_CLASS operator                         & ( ENUM_CLASS a, std::underlying_type_t<ENUM_CLASS> b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) & b }; } \
static inline constexpr ENUM_CLASS operator                         &= ( ENUM_CLASS& a, std::underlying_type_t<ENUM_CLASS> b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a = ENUM_CLASS{ type_t(a) & b }; } \
static inline constexpr ENUM_CLASS operator                         & ( std::underlying_type_t<ENUM_CLASS> a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) & type_t(b) }; } \
static inline constexpr std::underlying_type_t<ENUM_CLASS> operator &= ( std::underlying_type_t<ENUM_CLASS>& a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a &= type_t(b); } \
static inline constexpr ENUM_CLASS operator                         | ( ENUM_CLASS a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) | type_t(b) }; } \
static inline constexpr ENUM_CLASS operator                         |= ( ENUM_CLASS& a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a = ENUM_CLASS{ type_t(a) | type_t(b) }; } \
static inline constexpr ENUM_CLASS operator                         | ( ENUM_CLASS a, std::underlying_type_t<ENUM_CLASS> b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) | b }; } \
static inline constexpr ENUM_CLASS operator                         |= ( ENUM_CLASS& a, std::underlying_type_t<ENUM_CLASS> b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a = ENUM_CLASS{ type_t(a) | b }; } \
static inline constexpr std::underlying_type_t<ENUM_CLASS> operator | ( std::underlying_type_t<ENUM_CLASS> a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a | type_t(b); } \
static inline constexpr std::underlying_type_t<ENUM_CLASS> operator |= ( std::underlying_type_t<ENUM_CLASS>& a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a |= type_t(b); } \
static inline constexpr ENUM_CLASS operator                         ^ ( ENUM_CLASS a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) ^ type_t(b) }; } \
static inline constexpr ENUM_CLASS operator                         ^= ( ENUM_CLASS& a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a = ENUM_CLASS{ type_t(a) ^ type_t(b) }; } \
static inline constexpr ENUM_CLASS operator                         ^ ( ENUM_CLASS a, std::underlying_type_t<ENUM_CLASS> b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) ^ b }; } \
static inline constexpr ENUM_CLASS operator                         ^= ( ENUM_CLASS& a, std::underlying_type_t<ENUM_CLASS> b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a = ENUM_CLASS{ type_t(a) ^ b }; } \
static inline constexpr std::underlying_type_t<ENUM_CLASS> operator ^ ( std::underlying_type_t<ENUM_CLASS> a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return type_t(a) ^ type_t(b); } \
static inline constexpr std::underlying_type_t<ENUM_CLASS> operator ^= ( std::underlying_type_t<ENUM_CLASS>& a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a ^= type_t(b); } \
static inline constexpr std::underlying_type_t<ENUM_CLASS> operator ~ ( ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ~type_t(b); } \
static inline constexpr bool anybit( ENUM_CLASS a ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return type_t(a) != 0; } \
template< ENUM_CLASS mask > static inline constexpr bool allbits( ENUM_CLASS a ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return (a & mask) == mask; } \
static inline constexpr ENUM_CLASS firstbit( ENUM_CLASS a ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{(( ( type_t(a) - 1 ) ^ a ) + 1) >> 1}; } \
static_assert( firstbit( ENUM_CLASS{} ) == ENUM_CLASS{}, "enum class default should be empty" ); \
SBCLANG_POP_IGNORE()


	template< typename T >
static inline constexpr size_t array_count( const T& _array );
//{
//	return sizeof(_array)/sizeof(_array[0]);
//}

	template< typename T, size_t N >
static inline constexpr size_t array_count( const T( & )[N] ) { return N; }
//	template< typename T, size_t N >
//static inline constexpr size_t array_count( const std::array<T, N>& ) { return N; }

////
	template< typename dataSize_t, typename align_t >
static inline constexpr std::common_type_t<dataSize_t, align_t> align_down( dataSize_t size, align_t align ) { static_assert( std::is_integral_v<dataSize_t> && std::is_integral_v<align_t> ); return ( size / align ) * align; }
	template< typename dataSize_t, typename align_t >
static inline constexpr std::common_type_t<dataSize_t, align_t> align_up( dataSize_t size, align_t align ) { return align_down( size + align-1, align ); }

////
template<typename _KEY_TYPE_, typename _VALUE_TYPE_>
struct key_value_pair
{
	using key_t = _KEY_TYPE_;
	using value_t  = _VALUE_TYPE_;

	key_t    key;
	value_t  value;

	constexpr key_value_pair() noexcept = default;
	constexpr key_value_pair( const key_value_pair& ) noexcept = default;
	constexpr key_value_pair( key_value_pair&& ) noexcept = default;
	constexpr key_value_pair( key_t _key, const value_t& _value ) noexcept : key( _key ), value( _value ) {}

	constexpr key_t get_key() const { return key; }
	constexpr value_t get_value() const { return value; }
};

////
// Some N-bits modular arithmetic used for encryption
	template<typename type_t>
static inline constexpr type_t gcd( type_t a, type_t b )
{
	type_t sign_value = std::conditional_t< std::is_signed_v<type_t>
		, std::integral_constant<type_t, type_t{1}>
		, std::integral_constant<type_t, static_cast<type_t>(-1)>
	>::value;
	if ( a < 0 )
		return sign_value * gcd( b, sign_value * a );
	else if ( b < a )
		return gcd(b, a);
	else if ( a == 0 )
		return b;
	else
		return gcd((b % a), a);
}
	template< typename type_t >
static inline constexpr auto extended_gcd_modular_step( type_t a_rem, type_t a_bez, type_t b_rem, type_t b_bez, type_t a_r_offset = 0 )
{
	const type_t quotient = b_rem ? a_rem / b_rem : 0;
	type_t next_rem = a_rem + a_r_offset - quotient * b_rem;
	type_t next_bez = a_bez - quotient * b_bez;
	return std::make_tuple( b_rem, b_bez, next_rem, next_bez );
};
	template< typename type_t >
static inline constexpr type_t modular_inverse( type_t value, type_t characteristic = type_t{} )
{
	// If value is a zero divisor (so not a unit), returns a non-zero stabilizer of value in given characteristic :
	//		value = value * stabilizer = value * stabilizer^2 = ... = value * inverse( stabilizer ).
	// In such case, if stabilizer is 1 then value must be 0 since gcd = 1 would mean it is prime, but it was assumed a zero divisor.
	// For a similar reason, modular_inverse returns 0 iff value = 0 or 1 mod characteristic.
	using modular_type_t = std::make_unsigned_t<std::remove_const_t<type_t>>;
	const modular_type_t max_value = characteristic != 0 ? static_cast<modular_type_t>(characteristic) : std::numeric_limits<modular_type_t>::max();
	const modular_type_t offset = characteristic - max_value;

	struct type_pair { modular_type_t remainder; modular_type_t bezout; };
	type_pair first  ={ max_value, 0 };
	type_pair second ={ static_cast<modular_type_t>( value ),   1 };
	{
		const auto [ r1, b1, r2, b2 ] = extended_gcd_modular_step( first.remainder, first.bezout, second.remainder, second.bezout, offset );
		first  = { r1, b1 };
		second = { r2, b2 };
	}
	while( second.remainder != 0 )
	{
		const auto [ r1, b1, r2, b2 ] = extended_gcd_modular_step( first.remainder, first.bezout, second.remainder, second.bezout );
		first  ={ r1, b1 };
		second ={ r2, b2 };
	}
	return static_cast<type_t>( first.bezout );
}
// Least Common Multiple
	template<typename _TYPE_>
constexpr _TYPE_ lcm(_TYPE_ a, _TYPE_ b)
{
	// expects <,>, +,-, *,/,% are all defined
	return ( a / gcd(a, b) ) * b;
}

static_assert( std::is_integral_v<size_t> );


static inline auto at_scope_exit( void( *fct )( ) )
{
	class AutoExit {
	public:
		AutoExit() = delete;
		inline constexpr AutoExit( void( *fct )( ) ) noexcept : call( fct ) {}
		inline constexpr AutoExit( AutoExit&& other ) noexcept : call( other.call ) { other.call = nullptr; }
		inline ~AutoExit() { if( call ) call(); }
	private:
		void( *call )( );
	};
	return AutoExit{ fct };
}
#define sb_scope_exit( fct ) auto at_scope_exit ## __LINE__ = sbLibX::at_scope_exit( fct );

}} // namespace SB:LibX
