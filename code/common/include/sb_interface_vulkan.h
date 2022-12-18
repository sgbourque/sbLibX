#pragma once

#include <common/include/sb_utilities.h>
#include <common/include/sb_interface.h>

#include <vector>
namespace SB { namespace LibX {
struct Configuration;
}}

#include <vulkan/vulkan_core.h>

namespace SB { namespace LibX {
namespace vulkan
{
using InstanceHandle = VkInstance;
using AdapterHandle = VkPhysicalDevice;
using DeviceHandle = VkDevice;

#ifdef SB_WIN_EXPORT
#define SB_LIB_EXPORT SB_WIN_EXPORT
#else
#define SB_LIB_EXPORT SB_IMPORT_LIB
#endif

//////////////////////////////////////////////////////////////////////////
	template<typename _TYPE_, typename ..._OTHERS_>
SB_LIB_EXPORT std::vector<_TYPE_> enumerate( _OTHERS_...);

struct instance_extension_traits
{
	using properties_t = VkExtensionProperties;
};
	template<>
SB_LIB_EXPORT std::vector<instance_extension_traits::properties_t> enumerate<instance_extension_traits::properties_t>( const uint32_t max_count, const char* layer_name );
	template<>
inline std::vector<instance_extension_traits::properties_t> enumerate<instance_extension_traits::properties_t>( const char* layer_name )
{
	return enumerate<instance_extension_traits::properties_t>(~0u, layer_name);
}
	template<>
inline std::vector<instance_extension_traits::properties_t> enumerate<instance_extension_traits::properties_t>()
{
	return enumerate<instance_extension_traits::properties_t>(static_cast<const char*>(nullptr));
}

struct instance_layer_traits
{
	using properties_t = VkLayerProperties;
};
	template<>
SB_LIB_EXPORT std::vector<instance_layer_traits::properties_t> enumerate<instance_layer_traits::properties_t>( const uint32_t max_count );
	template<>
inline std::vector<instance_layer_traits::properties_t> enumerate<instance_layer_traits::properties_t>()
{
	return enumerate<instance_layer_traits::properties_t>( ~0u );
}

struct device_extension_traits
{
	using properties_t = VkExtensionProperties;
};
	template<>
SB_LIB_EXPORT std::vector<device_extension_traits::properties_t> enumerate<device_extension_traits::properties_t>( const uint32_t max_count, AdapterHandle adapter, const char* const layer_name );

//////////////////////////////////////////////////////////////////////////

enum class instance_layer_t : uint64_t
{
	#include <common/include/internal/vulkan/vulkan_instance_layers.h>
};
sb_enum_class_flags( instance_layer_t );

struct InstanceConfiguration : Configuration
{
	InstanceConfiguration() : Configuration{ "sbVulkanConfig"_xhash64, "0.1"_xhash64 } {}

	xhash_string_view_t applicationName = "sbVulkan"_xhash64;
	xhash_string_view_t engineName = "sbLibX"_xhash64;
	uint32_t applicationVersion = 0;
	uint32_t engineVersion = 0;

	instance_layer_t layer_mask{
		0
		| instance_layer_t::NV_mask
		| instance_layer_t::AMD_mask
#if defined( SB_VULKAN_GRAPHICS_TOOLS )
		// Could activate these by default through compile flags
		 | instance_layer_t::validation_mask
		 | instance_layer_t::
		 | instance_layer_t::full_debug_mask
#endif
#if defined( SB_VULKAN_VALVE_TOOLS )
		 | instance_layer_t::VALVE_mask
#endif
	};

	std::vector<xhash_string_view_t> requested_layers = {};
	std::vector<xhash_string_view_t> requested_extensions = {};
};

#define SBLIB_DECLARE_DEVICE_INTERNAL
#include <common/include/internal/device_generic.h>
#undef SBLIB_DECLARE_DEVICE_INTERNAL

static inline constexpr uint32_t vendor_order( vendor_t vendor )
{
	switch (vendor)
	{
	case vendor_t::NVIDIA:    return ( static_cast<uint32_t>( vendor_t::NVIDIA    ) | 0x01000000 );
	case vendor_t::AMD:       return ( static_cast<uint32_t>( vendor_t::AMD       ) | 0x02000000 );
	case vendor_t::Intel:     return ( static_cast<uint32_t>( vendor_t::Intel     ) | 0x04000000 );
		// Microsoft is usually a software (possibly hardware accelerated) device, so considered as debug-only
	case vendor_t::Microsoft: return ( static_cast<uint32_t>( vendor_t::Microsoft ) | 0xFFFF0000 );
	}
	return static_cast<uint32_t>( vendor );
};

}
using vulkan_instance = vulkan::unique_instance;
using vulkan_device = vulkan::unique_device;

}}
namespace sbVulkan = SB::LibX::vulkan;

SB_PLATFORM_DEPENDS
