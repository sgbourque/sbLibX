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
	enum extension_id_t
	{
		kSurface,
		kPlatformSurface,
		//kSwapchain,
		//kDebugUtils,
		// add others

		kMaxExtension = 64, // raise or lower as desired
	};
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
	none                        = ( 0 ),

	// debug layer
	KHRONOS_validation          = ( 1ull <<  0 ), // Khronos Validation Layer (March 2019: Supported)
	validation_mask             = ( KHRONOS_validation ),

	// api tools
	LUNARG_api_dump             = ( 1ull <<  1 ), // LunarG debug layer
	LUNARG_device_simulation    = ( 1ull <<  2 ), // LunarG device simulation layer
	LUNARG_monitor              = ( 1ull <<  3 ), // Execution Monitoring Layer
	LUNARG_screenshot           = ( 1ull <<  4 ), // LunarG image capture layer
	LUNARG_gfxreconstruct       = ( 1ull <<  5 ), // GFXReconstruct Capture Layer Version 0.9.3
	api_utils                   = ( LUNARG_api_dump | LUNARG_device_simulation | LUNARG_monitor | LUNARG_screenshot | LUNARG_gfxreconstruct ),

	RENDERDOC_Capture           = ( 1ull <<  6ull ), // Debugging capture layer for RenderDoc
	graphics_tools_mask         = ( api_utils | RENDERDOC_Capture ),

	// deprecated
	LUNARG_vktrace              = ( 1ull << 20 ), // Vktrace tracing library
	LUNARG_assistant_layer      = ( 1ull << 21 ), // LunarG Validation Factory Layer
	LUNARG_standard_validation  = ( 1ull << 22 ), // LunarG Standard Validation (February 2016: Deprecated)
	LUNARG_core_validation      = ( 1ull << 23 ), // LunarG Validation Layer (March 2016: Deprecated)
	LUNARG_parameter_validation = ( 1ull << 24 ), // LunarG Validation Layer (December 2014: Deprecated)
	LUNARG_object_tracker       = ( 1ull << 25 ), // LunarG Validation Layer (October 2014: Deprecated)
	GOOGLE_unique_objects       = ( 1ull << 26 ), // Google Validation Layer (December 2015: Deprecated)
	GOOGLE_threading            = ( 1ull << 27 ), // Google Validation Layer (April 2015: Deprecated)
	deprecated_mask             = ( LUNARG_vktrace | LUNARG_assistant_layer | LUNARG_standard_validation | LUNARG_core_validation | LUNARG_parameter_validation | LUNARG_object_tracker | GOOGLE_unique_objects | GOOGLE_threading ),

	// Khronos/VALVE extensions
	VALVE_steam_overlay         = ( 1ull << 30 ), // Steam Overlay Layer
	VALVE_steam_fossilize       = ( 1ull << 31 ), // Steam Pipeline Caching Layer
	VALVE_mask                  = ( VALVE_steam_fossilize | VALVE_steam_overlay ),

	// vendor extensions
	NV_optimus                  = ( 1ull << 32 ), // NVIDIA Optimus layer
	NV_mask                     = ( NV_optimus ),

	AMD_mask                    = ( 0ull ), // TODO
};
sb_enum_class_flags( instance_layer_t );

struct vulkanInstanceConfig : Configuration
{
	vulkanInstanceConfig() : Configuration{ "sbVulkanConfig"_xhash64, "0.1"_xhash64 } {}

	xhash_string_view_t applicationName = "sbVulkan"_xhash64;
	xhash_string_view_t engineName = "sbLibX"_xhash64;
	uint32_t applicationVersion = 0;
	uint32_t engineVersion = 0;

	instance_layer_t layer_mask{
		0
		| instance_layer_t::NV_mask
		| instance_layer_t::AMD_mask
		// Could activate these by default through compile flags
		// | instance_layer_t::debug_utils_mask
		// | instance_layer_t::graphics_tools_mask
		// | instance_layer_t::full_debug_mask
		// | instance_layer_t::VALVE_mask
	};

	std::vector<xhash_string_view_t> requested_layers = {
	};
	std::vector<xhash_string_view_t> requested_extensions = {
	};
};

#define SBLIB_DECLARE_DEVICE_INTERNAL
#include <common/include/internal/device_generic.h>
#undef SBLIB_DECLARE_DEVICE_INTERNAL

}
using vulkan_instance = vulkan::unique_instance;
using vulkan_device = vulkan::unique_device;

}}
namespace sbLibVulkan = SB::LibX::vulkan;

SB_PLATFORM_DEPENDS
