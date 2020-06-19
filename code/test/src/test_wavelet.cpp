
#include <utility>
//#include <ctype>
#include <array>
#include <deque>
#include <iostream>

namespace SB
{

enum class float_representation_type
{
	binary_16,
	binary_32,
	binary_64,
	binary_128,
	binary_256,
	//decimal_32,
	//decimal_64,
	//decimal_128,

	dxgi_vulkan_10, // both Microsoft and Khronos uses the same representation
	dxgi_vulkan_11, // both Microsoft and Khronos uses the same representation

	half_precision = binary_16,
	single_precision = binary_32,
	double_precision = binary_64,
};
template <size_t _sign_bits, size_t _exponent_bits, size_t _mantissa_bits>
struct float_raw_representation_traits
{
	enum
	{
		sign_bits     = _sign_bits,
		exponent_bits = _exponent_bits,
		mantissa_bits = _mantissa_bits,

		exponent_bias = (1 << (exponent_bits - 1)) - 1,
		total_bits    = sign_bits + exponent_bits + mantissa_bits,

		representation_size = ((total_bits + (CHAR_BIT - 1)) / CHAR_BIT) * sizeof(char),
	};
};


template <float_representation_type float_type>
struct float_binary_traits;
template<> struct float_binary_traits<float_representation_type::binary_16>      { using representation_traits = float_raw_representation_traits<1,  5,  16 -  5 - 1>; };
template<> struct float_binary_traits<float_representation_type::binary_32>      { using representation_traits = float_raw_representation_traits<1,  8,  32 -  8 - 1>; };
template<> struct float_binary_traits<float_representation_type::binary_64>      { using representation_traits = float_raw_representation_traits<1, 11,  64 - 11 - 1>; };
template<> struct float_binary_traits<float_representation_type::binary_128>     { using representation_traits = float_raw_representation_traits<1, 15, 128 - 15 - 1>; };
template<> struct float_binary_traits<float_representation_type::binary_256>     { using representation_traits = float_raw_representation_traits<1, 19, 256 - 19 - 1>; };
template<> struct float_binary_traits<float_representation_type::dxgi_vulkan_10> { using representation_traits = float_raw_representation_traits<0,  5,  10 -  5 - 0>; };
template<> struct float_binary_traits<float_representation_type::dxgi_vulkan_11> { using representation_traits = float_raw_representation_traits<0,  5,  11 -  5 - 0>; };

template <typename _type, typename _float_representation_traits = float_representation_traits<_type> >
struct float_traits
{
	using type_t = _type;
	using representation_traits = typename _float_representation_traits::representation_traits;
	using signed_integer_type   = typename _float_representation_traits::signed_integer_type;
	using unsigned_integer_type = typename _float_representation_traits::unsigned_integer_type;
	enum
	{
		sign_bits     = representation_traits::sign_bits,
		exponent_bits = representation_traits::exponent_bits,
		mantissa_bits = representation_traits::mantissa_bits,

		exponent_bias = representation_traits::exponent_bias,
		total_bits    = representation_traits::total_bits,

		representation_size = representation_traits::representation_size,
	};
	//static_assert( sizeof(type_t) == representation_size );
};


template <typename _type>
struct float_representation_traits;
template<> struct float_representation_traits<float>       { static inline constexpr float_representation_type representation_type = float_representation_type::binary_32; using signed_integer_type = int32_t; using unsigned_integer_type = uint32_t; using representation_traits = typename float_binary_traits<representation_type>::representation_traits; };
template<> struct float_representation_traits<double>      { static inline constexpr float_representation_type representation_type = float_representation_type::binary_64; using signed_integer_type = int64_t; using unsigned_integer_type = uint64_t; using representation_traits = typename float_binary_traits<representation_type>::representation_traits; };
//template<> struct float_representation_traits<float>       { using signed_integer_type = int32_t; using unsigned_integer_type = uint32_t; using representation_traits = typename float_binary_traits<float_representation_type::binary_32>::representation_traits; };
//template<> struct float_representation_traits<double>      { using signed_integer_type = int64_t; using unsigned_integer_type = uint64_t; using representation_traits = typename float_binary_traits<float_representation_type::binary_64>::representation_traits; };

template< typename T, typename unsigned_integer_type, typename signed_integer_type >
struct raw_float_type
{
	// not portable : assumes LSB -> MSB ordering (as on x86)
	unsigned_integer_type mantissa            : float_traits<T>::mantissa_bits;
	unsigned_integer_type normalised_exponent : float_traits<T>::exponent_bits; // normalized to 0dB at float_traits<T>::exponent_bias (3dB steps)
	signed_integer_type   sign                : float_traits<T>::sign_bits;
};

template< typename T >
union float_type
{
	using unsigned_integer_type = typename float_traits<T>::unsigned_integer_type;
	using signed_integer_type = typename float_traits<T>::signed_integer_type;
	using raw_type = raw_float_type<T, unsigned_integer_type, signed_integer_type>;
	raw_type              raw;
	uint8_t               raw_buffer[float_traits<T>::representation_size];
	unsigned_integer_type raw_value;
	T                     value;

	float_type() {};
	template<typename U> constexpr float_type( U _value) : value(_value) {}

	explicit constexpr float_type( int32_t _raw ) : raw_value(_raw) {}; // mostly for sample_type{0};
	explicit constexpr float_type( raw_type&& _value ) : raw(std::forward<raw_type>(_value)) {}
	explicit constexpr float_type( int32_t&& _sign, uint32_t&& _exponent, uint32_t&& _mantissa ) : raw{std::forward<uint32_t>(_mantissa), std::forward<uint32_t>(_exponent), std::forward<int32_t>(_sign)} {}

	constexpr operator T() const { return value; }
};


struct half;
template<> struct float_representation_traits<half> { static inline constexpr float_representation_type representation_type = float_representation_type::binary_16; using signed_integer_type = int16_t; using unsigned_integer_type = uint16_t; using representation_traits = typename float_binary_traits<representation_type>::representation_traits; };

struct half
{
	using unsigned_integer_type = uint16_t;
	using signed_integer_type = int16_t;
	using raw_type = raw_float_type<half, unsigned_integer_type, signed_integer_type>;
	raw_type raw_value;
	constexpr half(float _value)
	{
		float_type<float> value = _value;
		//raw_value = float_type<half>::raw_type{
		//	static_cast<uint16_t>(value.raw.mantissa & 0x1),
		//	static_cast<uint16_t>(value.raw.normalised_exponent & 0x1),
		//	static_cast<int16_t>(value.raw.sign & 0x1)
		//};
	}
	constexpr operator float() const
	{
		float_type<float> float_value{
		//	raw_float_type{ raw_value.mantissa, raw_value.normalised_exponent, raw_value.sign }
		};
		//raw_value = float_type<half>::raw_type{
		//	static_cast<uint16_t>(value.raw.mantissa & 0x1),
		//	static_cast<uint16_t>(value.raw.normalised_exponent & 0x1),
		//	static_cast<int16_t>(value.raw.sign & 0x1)
		//};
		return float_value;
	}
};
}


using sample_type = SB::float_type<float>;
static_assert( sizeof(sample_type) == sizeof(sample_type::value) );
static_assert( sizeof(sample_type::raw_type) == sizeof(sample_type::raw_value) );
constexpr sample_type zero      =  0.0f;//	static_assert( (float)zero.value      ==  0.0f );
constexpr sample_type plus_one  =  1.0f;//	static_assert( plus_one  ==  1.0f );
constexpr sample_type minus_one = -1.0f;//	static_assert( minus_one == -1.0f );
constexpr sample_type plus_two  =  2.0f;//	static_assert( plus_two  ==  2.0f );

// renormalize audio such that all values are normalized from max_value to 1
//auto soft_limit(sample_type value, sample_type max_value)
//{
//	max_value.value = std::max(value.value, max_value.value);
//	return std::make_tuple(value.value / max_value, max_value);
//}


SB_EXPORT_TYPE int __stdcall test_wavelet([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	if( plus_one.value + minus_one.value != zero.value )
		__debugbreak();
	if( plus_one.value - minus_one.value == zero.value )
		__debugbreak();
	if (plus_one.value - minus_one.value != plus_two.value)
		__debugbreak();

	constexpr size_t packet_size = 32; // assuming at least 44.1kHz, this gives less than 0.75 processing latency
	using packet = std::array<sample_type, packet_size>;
	
	constexpr size_t packet_count = 300; // assuming at most 192kHz, this allows down to 20Hz signal analysis
	using buffer = std::array<packet, packet_count>;

	constexpr size_t sample_count = packet_size * packet_count; // assuming at most 192kHz, this allows down to 20Hz signal analysis


	return 0;
}

