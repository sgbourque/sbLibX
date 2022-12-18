#pragma once

#include <tuple>
#include <array>
#include <limits>

namespace SB { namespace LibX
{
// designed initializer not supported before 201705
#if _MSVC_LANG < 201705
	#define SB_STRUCT_SET( X, op, Y ) Y
#else
	#define SB_STRUCT_SET( X, op, Y ) X op Y
#endif

#define SB_ASSIGN_IF_NULL( X, Ref, Value ) do { if( !(Ref) ) { (X) = (Value); } } while(false)

#define SB_CLASS_MEMBER_SET(member_name, type, name, default_value, load_value) \
	auto& set_##name( type name = default_value ) \
	{ \
		SB_ASSIGN_IF_NULL( member_name, ( member_name = name ) != type{}, load_value );\
		return *this; \
	}


#if 0//defined(__clang__)
#define clang_push_ignore_unused() \
	clang_pragma( diagnostic push ) \
	clang_pragma( diagnostic ignored "-W" "unneeded-internal-declaration" ) \
	clang_pragma( diagnostic ignored "-W" "unused-function" ) \
	clang_pragma( diagnostic ignored "-W" "unused-template" ) \
	clang_pragma( diagnostic ignored "-W" "unused-local-typedef" )

#define clang_pop_ignore() \
	clang_pragma( diagnostic pop )

#else
#define clang_push_ignore_unused( ... )
#define clang_pop_ignore( ... )
#endif

////
#define sb_enum_class_flags( ENUM_CLASS ) \
[[maybe_unused]] static inline constexpr ENUM_CLASS operator                         & ( ENUM_CLASS a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) & type_t(b) }; } \
[[maybe_unused]] static inline constexpr ENUM_CLASS operator                         &= ( ENUM_CLASS& a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a = ENUM_CLASS{ type_t(a) & type_t(b) }; } \
[[maybe_unused]] static inline constexpr ENUM_CLASS operator                         & ( ENUM_CLASS a, std::underlying_type_t<ENUM_CLASS> b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) & b }; } \
[[maybe_unused]] static inline constexpr ENUM_CLASS operator                         &= ( ENUM_CLASS& a, std::underlying_type_t<ENUM_CLASS> b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a = ENUM_CLASS{ type_t(a) & b }; } \
[[maybe_unused]] static inline constexpr ENUM_CLASS operator                         & ( std::underlying_type_t<ENUM_CLASS> a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) & type_t(b) }; } \
[[maybe_unused]] static inline constexpr std::underlying_type_t<ENUM_CLASS> operator &= ( std::underlying_type_t<ENUM_CLASS>& a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a &= type_t(b); } \
[[maybe_unused]] static inline constexpr ENUM_CLASS operator                         | ( ENUM_CLASS a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) | type_t(b) }; } \
[[maybe_unused]] static inline constexpr ENUM_CLASS operator                         |= ( ENUM_CLASS& a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a = ENUM_CLASS{ type_t(a) | type_t(b) }; } \
[[maybe_unused]] static inline constexpr ENUM_CLASS operator                         | ( ENUM_CLASS a, std::underlying_type_t<ENUM_CLASS> b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) | b }; } \
[[maybe_unused]] static inline constexpr ENUM_CLASS operator                         |= ( ENUM_CLASS& a, std::underlying_type_t<ENUM_CLASS> b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a = ENUM_CLASS{ type_t(a) | b }; } \
[[maybe_unused]] static inline constexpr std::underlying_type_t<ENUM_CLASS> operator | ( std::underlying_type_t<ENUM_CLASS> a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a | type_t(b); } \
[[maybe_unused]] static inline constexpr std::underlying_type_t<ENUM_CLASS> operator |= ( std::underlying_type_t<ENUM_CLASS>& a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a |= type_t(b); } \
[[maybe_unused]] static inline constexpr ENUM_CLASS operator                         ^ ( ENUM_CLASS a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) ^ type_t(b) }; } \
[[maybe_unused]] static inline constexpr ENUM_CLASS operator                         ^= ( ENUM_CLASS& a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a = ENUM_CLASS{ type_t(a) ^ type_t(b) }; } \
[[maybe_unused]] static inline constexpr ENUM_CLASS operator                         ^ ( ENUM_CLASS a, std::underlying_type_t<ENUM_CLASS> b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{ type_t(a) ^ b }; } \
[[maybe_unused]] static inline constexpr ENUM_CLASS operator                         ^= ( ENUM_CLASS& a, std::underlying_type_t<ENUM_CLASS> b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a = ENUM_CLASS{ type_t(a) ^ b }; } \
[[maybe_unused]] static inline constexpr std::underlying_type_t<ENUM_CLASS> operator ^ ( std::underlying_type_t<ENUM_CLASS> a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return type_t(a) ^ type_t(b); } \
[[maybe_unused]] static inline constexpr std::underlying_type_t<ENUM_CLASS> operator ^= ( std::underlying_type_t<ENUM_CLASS>& a, ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return a ^= type_t(b); } \
[[maybe_unused]] static inline constexpr std::underlying_type_t<ENUM_CLASS> operator ~ ( ENUM_CLASS b ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ~type_t(b); } \
[[maybe_unused]] static inline constexpr bool anybit( ENUM_CLASS a ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return type_t(a) != 0; } \
template< ENUM_CLASS mask > static inline constexpr bool allbits( ENUM_CLASS a ) { return (a & mask) == mask; } \
[[maybe_unused]] static inline constexpr ENUM_CLASS firstbit( ENUM_CLASS a ) { using type_t = std::underlying_type_t<ENUM_CLASS>; return ENUM_CLASS{(( ( type_t(a) - 1 ) ^ a ) + 1) >> 1}; } \
static_assert( firstbit( ENUM_CLASS{} ) == ENUM_CLASS{}, "enum class default should be empty" ); \


	template< typename T >
static inline constexpr std::underlying_type_t<T> underlying( T enum_value )
{
	using underlying_type = std::underlying_type_t<T>;
	return static_cast<underlying_type>( enum_value );
}


	template< typename T >
static inline constexpr size_t array_count( const T& _array )
{
	static_assert( std::is_array_v<T>, "should only be used on array type" );
	return sizeof(_array)/sizeof(_array[0]);
}

	template< typename T, size_t N >
static inline constexpr size_t array_count( const T( & )[N] ) { return N; }
	template< typename T, size_t N >
static inline constexpr size_t array_count( const std::array<T, N>& ) { return N; }

////
	template< typename dataSize_t, typename align_t
		, std::enable_if_t< !std::is_integral_v<dataSize_t> || !std::is_integral_v<align_t>, size_t > = 0
	>
static inline constexpr std::common_type_t<dataSize_t, align_t >
div_align_down( dataSize_t size, align_t align )
{
	// warning : can return nan/inf
	return floor( size / align );
}
	template< typename dataSize_t, typename align_t, std::enable_if_t< std::is_integral_v<dataSize_t> && std::is_integral_v<align_t>, size_t > = 0 >
static inline constexpr std::common_type_t<dataSize_t, align_t >
div_align_down( dataSize_t size, align_t align )
{
	// warning can cause a division by 0 exception
	static_assert( std::is_integral_v<dataSize_t> && std::is_integral_v<align_t> && !std::is_signed_v<dataSize_t> && !std::is_signed_v<align_t>,
		"Aligning data requires unsigned integer values." );
	return ( size / align );
}
	template< typename dataSize_t, typename align_t >
static inline constexpr std::common_type_t<dataSize_t, align_t> div_align_up( dataSize_t size, align_t align ) { return div_align_down( size + align-1, align ); }

	template< typename dataSize_t, typename align_t >
static inline constexpr std::common_type_t<dataSize_t, align_t> align_down( dataSize_t size, align_t align ) { return div_align_down( size, align ) * align; }
	template< typename dataSize_t, typename align_t >
static inline constexpr std::common_type_t<dataSize_t, align_t> align_up( dataSize_t size, align_t align ) { return div_align_up( size, align ) * align; }


//////////////////////////////////////////////////////////////////////////
	template< typename _TYPE_T, typename _REF_TYPE_T >
using make_signof_t = std::conditional_t< std::is_signed_v<_REF_TYPE_T>, std::make_signed_t<_TYPE_T>, std::make_unsigned_t<_TYPE_T> >;

// type_mask can be called in two ways :
//	1. type_mask<int_t, mask_t> -> mask_t(-1) -> int_t
//	2. type_mask<int_t>(mask_t{}) : mask_t(-1) -> int_t
// This is because type_mask<a,b>() might not be clear that it means a(b(-1)) and not b(a(-1)).
// Hence, type_mask<a,b>() = type_mask<a>(b()) = type_mask<a>(b(-1)) means a(b(-1)) without sign extent.
// 
// Notes:
// - type masks are always unsigned to prevent sign extent
// - type_mask takes all bits from a type (so 0 or ~0 represents the same mask).
//   This is because a type cannot have less that CHAR_BIT bits.
//   If unused bits are required, 
// - We could define a generic mask that might even sign extent
//   or restricts some bits (similar to a sub-bitfield) mapped into type_t if required.
	template< typename _TYPE_T, typename _TYPE_MASK_T = _TYPE_T >
static inline constexpr std::make_unsigned_t<_TYPE_T> type_mask( [[maybe_unused]] _TYPE_MASK_T = _TYPE_MASK_T{} ) noexcept
{
	static_assert( std::is_integral_v<_TYPE_T> && std::is_integral_v<_TYPE_MASK_T> );
	if constexpr ( sizeof( _TYPE_T ) >= sizeof( _TYPE_MASK_T ) )
		return std::make_unsigned_t<_TYPE_MASK_T>(~0);
	else
		return std::make_unsigned_t<_TYPE_T>(~0);
}
static_assert( type_mask<int8_t>(uint64_t{}) == 0xFFull && std::is_same_v<uint8_t, decltype( type_mask<int8_t>(uint64_t{}) )> );
static_assert( type_mask<int64_t>(uint8_t{}) == 0xFFull && std::is_same_v<uint64_t, decltype( type_mask<int64_t>(uint8_t{}) )> );


////
//template<typename _KEY_TYPE_, typename _VALUE_TYPE_>
//struct key_value_pair
//{
//	using key_t = _KEY_TYPE_;
//	using value_t  = _VALUE_TYPE_;
//
//	key_t    key;
//	value_t  value;
//
//	constexpr key_value_pair() noexcept = default;
//	constexpr key_value_pair( const key_value_pair& ) noexcept = default;
//	constexpr key_value_pair( key_value_pair&& ) noexcept = default;
//	constexpr key_value_pair( key_t _key, const value_t& _value ) noexcept : key( _key ), value( _value ) {}
//
//	constexpr key_t get_key() const { return key; }
//	constexpr value_t get_value() const { return value; }
//};


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
#define sb_scope_exit( fct ) auto at_scope_exit ## __LINE__ = sbLibX::at_scope_exit( fct )


	template< bool condition_check, auto... >
struct static_assert_proxy
{
	static inline constexpr bool success = condition_check;
	static_assert( success, "Static assertion fail: Check input to static_assert_proxy" );
};
#define sb_static_assert( X, msg, ... ) \
	static_assert( sbLibX::static_assert_proxy< X, __VA_ARGS__ >::success, msg );


}} // namespace SB:LibX
