#if defined( SBWIN32 ) || defined( SBWIN64 )
// Please see sbWindows\sbwin_interface_asio.cpp for platform-specific implementation
#else
#error "ASIO not implemented"
#endif

#include <common/include/sb_interface_asio.h>
SB_PLATFORM_DEPENDS
