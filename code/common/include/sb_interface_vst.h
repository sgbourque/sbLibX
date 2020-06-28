#pragma once

namespace SB { namespace LibX {
struct Configuration;
}}

#include "common/include/internal/interface_vst_base.h"
#include <common/include/internal/ref_ptr.h>

#include <vector>

namespace SB { namespace LibX {
namespace VST
{

using Instance = IUnknown;
using Adapter = IPluginFactory;
using DeviceInfo = ClassInfo;
using Device = IPluginFactory;

/////////////////////////////////////////////////////////
using InstanceHandle = ref_ptr<VST::Instance>;
using AdapterHandle = ref_ptr<VST::Adapter>;
using DeviceHandle = ref_ptr<VST::Device>;

#define SBLIB_FORWARD_DECLARE_DEVICE_INFO_INTERNAL
#define SBLIB_DECLARE_DEVICE_INTERNAL
#include <common/include/internal/device_generic.h>
#undef SBLIB_DECLARE_DEVICE_INTERNAL
}
using vst_instance = VST::unique_instance;
using vst_device = VST::unique_device;
}}
