#pragma once

#include <vector>
namespace SB { namespace LibX {
struct Configuration;
}}


#if defined(SBWIN64) || defined(SBWIN32)
	#ifndef NOMINMAX
	#define NOMINMAX
	#endif
	#define VK_USE_PLATFORM_WIN32_KHR	1
#endif
#include <vulkan/vulkan.h>

namespace SB { namespace LibX {
namespace vulkan
{
using InstanceHandle = VkInstance;
using AdapterHandle = VkPhysicalDevice;
using DeviceHandle = VkDevice;

#define SBLIB_DECLARE_DEVICE_INTERNAL
#include <common/include/device_generic_internal.h>
#undef SBLIB_DECLARE_DEVICE_INTERNAL
}
using vulkan_instance = vulkan::instance;
using vulkan_device = vulkan::device;

}}
