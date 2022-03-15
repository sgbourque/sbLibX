#ifdef SB_ASIO_ERROR
SB_ASIO_ERROR(OK, 0, "This value will be returned whenever the call succeeded")
SB_ASIO_ERROR(SuccessFuture, 0x3f4847a0, "unique success return value for ASIOFuture calls")
SB_ASIO_ERROR(NotPresent, -1000 + 0, "hardware input or output is not present or available")
SB_ASIO_ERROR(HWMalfunction, -1000 + 1, "hardware is malfunctioning (can be returned by any ASIO function)")
SB_ASIO_ERROR(InvalidParameter, -1000 + 2, "input parameter invalid")
SB_ASIO_ERROR(InvalidMode, -1000 + 3, "hardware is in a bad mode or used in a bad mode")
SB_ASIO_ERROR(SPNotAdvancing, -1000 + 4, "hardware is not running when sample position is inquired")
SB_ASIO_ERROR(NoClock, -1000 + 5, "sample clock or rate cannot be determined or is not present")
SB_ASIO_ERROR(NoMemory, -1000 + 6, "not enough memory for completing the request")

#undef SB_ASIO_ERROR
#else
#error "SB_ASIO_ERROR(enumName, value, desc) is not defined"
#endif
