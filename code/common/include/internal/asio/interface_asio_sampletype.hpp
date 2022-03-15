#ifdef SB_ASIO_SAMPLETYPE
//SB_ASIO_SAMPLETYPE( name, index, type_name, bit_per_pack, bit_per_sample, sample_per_pack, endian, align_bits, container_type )
SB_ASIO_SAMPLETYPE( Int16_MSB  , 0, Int,   16, 16, 1, MSB, 16, int16_t )
SB_ASIO_SAMPLETYPE( Int24_MSB  , 1, Int,   24, 24, 1, MSB, 8,  int32_t ) // used for 20 bits as well
SB_ASIO_SAMPLETYPE( Int32_MSB  , 2, Int,   32, 32, 1, MSB, 32, int32_t )
SB_ASIO_SAMPLETYPE( Float32_MSB, 3, Float, 32, 32, 1, MSB, 32, float   ) // IEEE 754 32 bit float
SB_ASIO_SAMPLETYPE( Float64_MSB, 4, Float, 64, 64, 1, MSB, 64, double  ) // IEEE 754 64 bit double float

// these are used for 32 bit data buffer, with different alignment of the data inside
// 32 bit PCI bus systems can be more easily used with these
SB_ASIO_SAMPLETYPE( Int32_MSB16, 8 , Int, 32, 32, 1, MSB, 16, int32_t )
SB_ASIO_SAMPLETYPE( Int32_MSB18, 9 , Int, 32, 32, 1, MSB, 18, int32_t )
SB_ASIO_SAMPLETYPE( Int32_MSB20, 10, Int, 32, 32, 1, MSB, 20, int32_t )
SB_ASIO_SAMPLETYPE( Int32_MSB24, 11, Int, 32, 32, 1, MSB, 24, int32_t )

SB_ASIO_SAMPLETYPE( Int16_LSB  , 16, Int,   16, 16, 1, LSB, 16, int16_t )
SB_ASIO_SAMPLETYPE( Int24_LSB  , 17, Int,   24, 24, 1, LSB, 8,  int32_t ) // used for 20 bits as well
SB_ASIO_SAMPLETYPE( Int32_LSB  , 18, Int,   32, 32, 1, LSB, 32, int32_t )
SB_ASIO_SAMPLETYPE( Float32_LSB, 19, Float, 32, 32, 1, LSB, 32, float   ) // IEEE 754 32 bit float, as found on Intel x86 architecture
SB_ASIO_SAMPLETYPE( Float64_LSB, 20, Float, 64, 64, 1, LSB, 64, double  ) // IEEE 754 64 bit double float, as found on Intel x86 architecture

// these are used for 32 bit data buffer, with different alignment of the data inside
// 32 bit PCI bus systems can more easily used with these
SB_ASIO_SAMPLETYPE( Int32_LSB16, 24, Int, 32, 32, 1, LSB, 16, int32_t  )
SB_ASIO_SAMPLETYPE( Int32_LSB18, 25, Int, 32, 32, 1, LSB, 18, int32_t  )
SB_ASIO_SAMPLETYPE( Int32_LSB20, 26, Int, 32, 32, 1, LSB, 20, int32_t  )
SB_ASIO_SAMPLETYPE( Int32_LSB24, 27, Int, 32, 32, 1, LSB, 24, int32_t  )

//	ASIO DSD format.
/*
// DSD support
//	Some notes on how to use ASIOIoFormatType.
//
//	The caller will fill the format with the request types.
//	If the board can do the request then it will leave the
//	values unchanged. If the board does not support the
//	request then it will change that entry to Invalid (-1)
//
//	So to request DSD then
//
//	ASIOIoFormat NeedThis={kASIODSDFormat};
//
//	if(ASE_SUCCESS != ASIOFuture(kAsioSetIoFormat,&NeedThis) ){
//		// If the board did not accept one of the parameters then the
//		// whole call will fail and the failing parameter will
//		// have had its value changes to -1.
//	}
//
// Note: Switching between the formats need to be done before the "prepared"
// state (see ASIO 2 documentation) is entered.
*/
SB_ASIO_SAMPLETYPE( DSD_Int8_LSB1, 32, DSD_Int, 8, 1, 8, LSB, 8, uint8_t )		// DSD 1 bit data, 8 samples per byte. First sample in Least significant bit.
SB_ASIO_SAMPLETYPE( DSD_Int8_MSB1, 33, DSD_Int, 8, 1, 8, MSB, 8, uint8_t )		// DSD 1 bit data, 8 samples per byte. First sample in Most significant bit.
SB_ASIO_SAMPLETYPE( DSD_Int8_NER8, 40, DSD_Int, 8, 8, 1, NER, 8, uint8_t )		// DSD 8 bit data, 1 sample per byte. No Endianness required.

#undef SB_ASIO_SAMPLETYPE
#else
#error "SB_ASIO_SAMPLETYPE( name, index, type_name, bit_per_pack, bit_per_sample, sample_per_pack, endian, align_bits, container_type ) is not defined"
#endif
