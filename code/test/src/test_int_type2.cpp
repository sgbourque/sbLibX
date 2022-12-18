#include <common/include/sb_common.h>
#include <common/include/sb_utilities.h>
#include <common/include/sb_math_base.h>

[[maybe_unused]] static inline constexpr auto P_512 = std::array{
	0xA535B4FF2305C507ull,
	0x450F38B1F7B81519ull,
	0x6D18024F08ACFB68ull,
	0xE3BEF3EB624D6951ull,
	0xBA76AA0CD544EB9Cull,
	0xC7DF83CD3487F1CAull,
	0x3007D8B6938C79EDull,
	0x0343DDC7422B195Full,
};
[[maybe_unused]] static inline constexpr auto Q_512 = std::array{
	0xB7F0E67DDFBAB37Eull,
	0x95ECD227C27B5BDBull,
	0x229C04FE9F33CDD3ull,
	0xF97C6E916C7AA7B7ull,
	0x5589C0B0C58CF747ull,
	0x5FB985EF245AEE27ull,
	0x4B373B4533A75027ull,
	0xB7C7E18A00CF8A71ull,
};
[[maybe_unused]] static inline constexpr auto PQ_1024 = std::array{
	0x76B4DB7B8D72D4D2ull,
	0x31A467C29817B744ull,
	0xE8085116FFCEB731ull,
	0x1DC797D01C3CED1Dull,
	0x6222533FE3CE7BF5ull,
	0x421788EB2C0F8A9Bull,
	0x933E1D56C036CE4Eull,
	0x5236D5BAE8E7F29Dull,

	0xEAE92E7E710E626Aull,
	0x37830ABC17CAE80Eull,
	0x6446A2F3125AFF41ull,
	0x7B007009711B0803ull,
	0x95A2052025E680B5ull,
	0x33240A5DD8D9FAA5ull,
	0x00100373B68BB3EFull,
	0x8901FBAEF48468EFull,
};
[[maybe_unused]] static inline constexpr auto PQ_inv_512 = {
	0x5D2F8F16DE631187ull,
	0xB0F5072BFC3E9AFDull,
	0x33FF9EB7D30AFC2Dull,
	0x835F300D77EDE161ull,
	0xBF4F2087C70B2521ull,
	0x349157A863A28296ull,
	0x594ED52350D2F55Full,
	0xA3FAD62C2C3BC60Full,
};
[[maybe_unused]] static inline constexpr auto PQ_inv_min512 = { 3 * 7 * 67 * 751ull };
[[maybe_unused]] static inline constexpr auto PQ_inv_maj512 = {
	0x000005C791E625C7ull,
	0x489DC747584131A4ull,
	0xE847500E98207B9Aull,
	0xD39DB8DA708B2DE8ull,
	0x9B5D179B8EB245D9ull,
	0xC3620AABE7B3B63Cull,
	0x6F8D905A52208DB3ull,
	0x9F25CEB88675FB9Full,
};
// For P,Q, and PQ defined above, this is satisfied and should be used in unit tests.
// P * Q * PQ_inv_min512 * PQ_inv_maj512 = 1 mod 2^512.
// and P * Q = PQ_1024 in any base.

#if 0
/*
* Prime number above were generated from 
Random: 
dec: 10042277197282691397026644448597026181609350749071580294351029113771591310201112554960910201858497765285864552346091962825405741910795206899175654392571880
hex: 0xBFBDA0E2362A703DC9046E2C2F41D6C2C5EEC3933233E2EBE47133261A78EE4EF7CCB802B30E0493499916C0F94CDBF3C0F9B9ED8F208583E2973AF88217A7E8
Length: 512

Prime (p):
dec: 8652738957880625789620461616562250144676608466310981139847664892786853989474557848774126127560244535323561019428748612282977407638305070704798613874874719
hex: 0xA535B4FF2305C507450F38B1F7B815196D18024F08ACFB68E3BEF3EB624D6951BA76AA0CD544EB9CC7DF83CD3487F1CA3007D8B6938C79ED0343DDC7422B195F
Length: 512 bits, Digits: 154

Prime (q):
dec: 9633772760539785911577493307973561259086172487716189204387023627846939167951281214011120798200402527612436172897927976182363699725885127705058894259653233
hex: 0xB7F0E67DDFBAB37E95ECD227C27B5BDB229C04FE9F33CDD3F97C6E916C7AA7B75589C0B0C58CF7475FB985EF245AEE274B373B4533A75027B7C7E18A00CF8A71
Length: 512 bits, Digits: 154

Prime (N=pq):
dec: 83358520876491786649820776925181533238883801245311478249729276226262085232626894415457689035116010968549124659266021630432148720155754732076756334464371631789552603111924828715705166615900194419594932249191002083820541730936736930008826009843343852692296141774156654819462205374895950551085473230664258316527
hex: 0x76B4DB7B8D72D4D231A467C29817B744E8085116FFCEB7311DC797D01C3CED1D6222533FE3CE7BF5421788EB2C0F8A9B933E1D56C036CE4E5236D5BAE8E7F29DEAE92E7E710E626A37830ABC17CAE80E6446A2F3125AFF417B007009711B080395A2052025E680B533240A5DD8D9FAA500100373B68BB3EF8901FBAEF48468EF
Length: 1023 bits, Digits: 308
*/

namespace SB {
namespace LibX {

#if 1 // base_integer_traits
static_assert( sbLibX::ceil_pow2( 0ull ) == 0, "base_integer_traits assumption failed" );
static_assert( sbLibX::ceil_pow2( 0x8000000000000001ull ) == 0ull, "base_integer_traits assumption failed" );

	template< uint64_t DigitBase = 0 /* DigitBase 0 means 2^( sizeof(uint64_t) * CHAR_BIT ) */ >
struct base_integer_traits: base_integer_traits< sbLibX::ceil_pow2( DigitBase + 1 ) >
{
	static_assert( DigitBase != 1, "Digit base 1 means the trivial ZERO group, which is not even really a ring" );
};
template<> struct base_integer_traits< 0x0000000000000100ull > { using unsigned_digit_type = uint8_t;  using signed_digit_type = int8_t;  };
template<> struct base_integer_traits< 0x0000000000010000ull > { using unsigned_digit_type = uint16_t; using signed_digit_type = int16_t; };
template<> struct base_integer_traits< 0x0000000100000000ull > { using unsigned_digit_type = uint32_t; using signed_digit_type = int32_t; };
template<> struct base_integer_traits< 0x0000000000000000ull > { using unsigned_digit_type = uint64_t; using signed_digit_type = int64_t; };

static_assert( std::is_same_v< base_integer_traits<  0ull           >::unsigned_digit_type, uint64_t > );
static_assert( std::is_same_v< base_integer_traits< ~0ull           >::unsigned_digit_type, uint64_t > );
static_assert( std::is_same_v< base_integer_traits<  0xFFFFFFFFFull >::unsigned_digit_type, uint64_t > );
static_assert( std::is_same_v< base_integer_traits<  0xFFFFFFFF     >::unsigned_digit_type, uint32_t > );
static_assert( std::is_same_v< base_integer_traits<  0xFFF          >::unsigned_digit_type, uint16_t > );
static_assert( std::is_same_v< base_integer_traits<  0x7            >::signed_digit_type, int8_t > );
#endif // base_integer_traits

namespace Arithetics
{
// command allocator stores commands and data for command lists.
struct command_allocator
{
	// could be cpu or gpu memory
};

struct command_list
{
	// 4 kB should stay cache friendly with multiple other command lists to combine
	// for temporary node allocation instead of using the stack.
	// It can hold 512 pointers or 256 key value pairs.
	// If the work_data is filled or the command_list is evaluated,
	// work_data is being flushed into the command_buffer.
	// A new buffer is then allocated and reassociated to the command list.
	static inline constexpr size_t WORK_DATA_SIZE = 4 * 1024;
	static thread_local uint8_t work_data[WORK_DATA_SIZE];
	static thread_local size_t  work_data_size = 0;

	~command_list()
	{
		// No flush : the command_list is being destroyed so data might be lost.
		assert( work_data_size == 0 ); // call reset/flush before 
		reset();
	}

	size_t flush()
	{
		// if store is nullptr, old data is lost...
		if ( work_data_size > 0 && store != nullptr )
		{
			store = copy( store, work_data, work_data_size );
		}

		size_t old_size = work_data_size;
		work_data_size = 0;
		return old_size;
	}
	bool reset( command_allocator* allocator = nullptr )
	{
		bool data_lost = false;
		if ( work_data_size > 0 )
		{
			// warning, buffer was not flushed before reset, data will be lost.
			data_lost = true;
		}

		work_data_size = 0;

		if( store && allocator != store )
		{
			// anything left in the buffer can be discard
			store->release();
		}
		if( allocator && allocator != store )
		{
			allocator->create();
		}
		store = allocator;
	}
private:
	command_allocator* store = nullptr;
};
}

}} // namespace sbLibX
#endif // 0

#include <array>
#include <vector>
namespace SB { namespace LibX {

	template< typename _NATIVE_TYPE_, size_t _SIZE_ = 0 >
struct static_container_traits
{
	using scratch_allocator_t = std::allocator<_NATIVE_TYPE_>; // tmp
	using container_t = std::conditional_t < _SIZE_ != 0, std::array< _NATIVE_TYPE_, _SIZE_ >, std::vector<_NATIVE_TYPE_, scratch_allocator_t>>;
};

	template< typename _NATIVE_TYPE_, size_t _SIZE_ = 0, typename _CONTAINER_TRAITS_ = static_container_traits<_NATIVE_TYPE_, _SIZE_> >
struct fixed_integer
{
	using container_t = typename _CONTAINER_TRAITS_::container_t;
	container_t data; // store as big endian such that the n-th element is of order 2^( sizeof(_NATIVE_TYPE_) * CHAR_BIT * n )
};

	template< typename _NATIVE_TYPE_, size_t _SIZE_, typename _OTHER_TYPE_ >
static inline constexpr auto operator +( const fixed_integer<_NATIVE_TYPE_, _SIZE_> a, const _OTHER_TYPE_ b )
{
	static_assert( std::is_integral_v<_OTHER_TYPE_>, "Adddition is not defined for non-integral (native) type."  );
	using native_t = _NATIVE_TYPE_;
	using return_t = fixed_integer<native_t, _SIZE_>;
	return_t result = a;
	native_t carry = b;
	for( size_t index = 0; carry != 0 && index < a.data.size(); ++index )
	{
		native_t current_value = result.data[index];
		native_t current_carry = carry;

		result.data[index] = ( current_value + current_carry );
		carry = ( result.data[index] < current_value ) ? 1 : 0;
	}
	return result;
}

//	template< typename _NATIVE_TYPE_, size_t _SIZE_, typename _OTHER_TYPE_ >
//static inline constexpr auto operator *( const fixed_integer<_NATIVE_TYPE_, _SIZE_> a, const _OTHER_TYPE_ b )
//{
//	static_assert( std::is_integral_v<_OTHER_TYPE_>, "Adddition is not defined for non-integral (native) type."  );
//	using native_t = _NATIVE_TYPE_;
//	using return_t = fixed_integer<native_t, _SIZE_>;
//	return_t result = a;
//	native_t carry = b;
//	for( size_t index = 0; carry != 0 && index < a.data.size(); ++index )
//	{
//		native_t current_carry = carry;
//
//		result.data[index] += current_carry;
//	}
//	return result;
//}
}}


#include <common/include/sb_common.h>

#include <iostream>
#include <iomanip>

SB_EXPORT_TYPE int SB_STDCALL test_int_type2( [[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[] )
{
	sbLibX::fixed_integer<uint64_t, 0> test{ {{0x1000000000000004ull,0x0000000000000000ull,0x0000000000000000ull, 0x1000000000000000ll}} };
	test = test + (-0x1000000000000006ll);
	for( const auto& t : test.data )
		std::cout << std::hex << std::setfill('0') << std::setw(16) << t << ' ';
	std::cout << std::endl;

	test = test + ( 0x1000000000000002ll );
	for (const auto& t : test.data)
		std::cout << std::hex << std::setfill( '0' ) << std::setw( 16 ) << t << ' ';
	std::cout << std::endl;
	return 0;
}
