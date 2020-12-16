#pragma once

namespace SB { namespace LibX {
struct Configuration;
}}

#include <common/include/internal/interface_asio_base.h>
#include <common/include/internal/ref_ptr.h>

#include <vector>

namespace SB { namespace LibX {
namespace ASIO
{

// This is where the COM CoInitialize is managed
struct Instance : IUnknown
{
};
// This is where device driver binding is setup
struct DeviceInfo;
struct Adapter : IUnknown
{
	virtual void GetDeviceInfo( DeviceInfo* ) const = 0;
};

struct DeviceInfo
{
	static constexpr size_t kDescSize = 256;
	static constexpr size_t kIIDSize = 16;
	char     description[kDescSize];
	uint8_t  uid[16];
};

/////////////////////////////////////////////////////////
using InstanceHandle = ref_ptr<ASIO::Instance>;
using AdapterHandle = ref_ptr<ASIO::Adapter>;
using DeviceHandle = ref_ptr<ASIO::Device>;

#ifdef SB_WIN_EXPORT
#define SB_LIB_EXPORT SB_WIN_EXPORT
#else
#define SB_LIB_EXPORT SB_IMPORT_LIB
#endif

#define SBLIB_FORWARD_DECLARE_DEVICE_INFO_INTERNAL
#define SBLIB_DECLARE_DEVICE_INTERNAL
#include <common/include/internal/device_generic.h>
#undef SBLIB_DECLARE_DEVICE_INTERNAL
}
using asio_instance = ASIO::unique_instance;
using asio_device = ASIO::unique_device;

}}

SB_EXPORT_TYPE void SB_STDCALL SB_ASIO_COINIT_APARTMENTTHREADED() {}
SB_PLATFORM_DEPENDS
