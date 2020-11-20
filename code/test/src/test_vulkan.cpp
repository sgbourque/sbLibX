#include <test/include/test_vulkan.h>
#include <vulkan/vulkan.h>
//#include <vulkan/vk_sdk_platform.h>

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include <common/include/sb_hash.h>

namespace SB {
namespace LibX {
struct Configuration
{
	xhash_string_view_t name;
	xhash_string_view_t version;
};
}
}



#define SB_HASH_TRAITS
#if defined( SB_HASH_TRAITS )
	template<typename hash_type>
struct hash_traits;

template<> struct hash_traits<uint64_t>
{
	using value_type = uint64_t;
	enum : value_type
	{
		hash_base  = 0x9EF3455AD47C9E31,
		hash_prime = 0x03519CFFA7F0F405,
	};
};

template<> struct hash_traits<uint32_t>
{
	using value_type = uint32_t;
	enum : value_type
	{
		hash_base  = 0xBEB9B485,
		hash_prime = 0x34263E29,
	};
};


	template<typename hash_type = uint32_t, template<typename> class traits = hash_traits >
constexpr hash_type const_hash(const char* s)
{
	hash_type hash = 0;
	while (*s)
	{
		hash += *s++ * traits<hash_type>::hash_base;
		hash *= traits<hash_type>::hash_prime;
	}
	return hash;
}
#endif // #if defined( SB_HASH_TRAITS )

//#define SB_VULKAN_TRAITS
#if defined( SB_VULKAN_TRAITS )
	template<typename extension_type = void>
struct vulkan_traits;

#define DEFINE_VULKAN_TRAITS( index, ExtensionMacroName, ExtensionTypeName ) \
		template<> \
	struct vulkan_traits<ExtensionTypeName> \
	{ \
		using type = ExtensionTypeName; \
		inline static const char name[] = ExtensionMacroName##_EXTENSION_NAME; \
		enum : size_t { \
			id = index, \
			mask = (1 << index), \
			version = ExtensionMacroName##_SPEC_VERSION, \
			hash64  = const_hash<uint64_t>(ExtensionMacroName##_EXTENSION_NAME), \
			hash32  = const_hash<uint32_t>(ExtensionMacroName##_EXTENSION_NAME), \
		}; \
	}

// TODO :
// - generate vulkan_traits from vulkan headers;
// - platform support (assign to void? as there's no associated type...);
// - 
//DEFINE_VULKAN_TRAITS(0, VK_KHR_SURFACE, VkSurfaceKHR);
//DEFINE_VULKAN_TRAITS(1, VK_KHR_SWAPCHAIN, VkSwapchainKHR);
//DEFINE_VULKAN_TRAITS(2, VK_KHR_DISPLAY, VkDisplayKHR);
//DEFINE_VULKAN_TRAITS(2, VK_KHR_DISPLAY, VkDisplayModeKHR);
// etc.

//#define DECLARE_VULKAN_EXTENSION( name, stringref ) \
//	constexpr char   vkExt##name##_name[] = CSTR(stringref); \
//	constexpr size_t vkExt##name##_id = const_hash<size_t>(CSTR(stringref));
//
//DECLARE_VULKAN_EXTENSION( Swapchain, VK_KHR_Swapchain );
//constexpr char   vkExtSwapchain_name[] = CSTR(VK_KHR_Swapchain);
//constexpr size_t vkExtSwapchain_id = const_hash<uint32_t>(CSTR(VK_KHR_Swapchain));
#endif // #if defined( SB_VULKAN_TRAITS )



#pragma comment(lib, "vulkan-1.lib")

#pragma comment(lib, "VkLayer_utils.lib")
//#pragma comment(lib, "vulkan-1.lib")
//#pragma comment(lib, "vulkan-1.lib")

namespace SB { namespace LibX { namespace vulkan
{

////
// would be nice to try C++ static reflection here... maybe traits would do to : TODO!
	template<typename _TYPE_, typename ..._OTHERS_>
std::vector<_TYPE_> enumerate( _OTHERS_...);

////
struct instance_layer
{
	using properties_t = VkLayerProperties;
};

	template<>
std::vector<instance_layer::properties_t> enumerate<instance_layer::properties_t>(const uint32_t max_count)
{
	uint32_t count = 0;
	{
		[[maybe_unused]] VkResult err = vkEnumerateInstanceLayerProperties(&count, nullptr);
		assert(!err);
	}
	count = std::min(count, max_count);

	std::vector<instance_layer::properties_t> result(count);
	if (count > 0)
	{
		[[maybe_unused]] VkResult err = vkEnumerateInstanceLayerProperties(&count, result.data());
		assert(!err);
	}
	return result;
}

	template<>
std::vector<instance_layer::properties_t> enumerate<instance_layer::properties_t>()
{
	return enumerate<instance_layer::properties_t>(~0u);
}


////
struct instance_extension
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

	//// platform-specific
	#if defined(VK_USE_PLATFORM_WIN32_KHR)
	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_XLIB_SURFACE_EXTENSION_NAME
	#elif defined(VK_USE_PLATFORM_XCB_KHR)
	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_XCB_SURFACE_EXTENSION_NAME
	#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_XCB_SURFACE_EXTENSION_NAME
	#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_DISPLAY_EXTENSION_NAME
	#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
	#elif defined(VK_USE_PLATFORM_IOS_MVK)
	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_MVK_IOS_SURFACE_EXTENSION_NAME
	#elif defined(VK_USE_PLATFORM_MACOS_MVK)
	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_MVK_MACOS_SURFACE_EXTENSION_NAME
	#else
	#error "vulkan support missing"
	#endif

	inline static const char name[extension_id_t::kMaxExtension][VK_MAX_EXTENSION_NAME_SIZE]
	= {
		VK_KHR_SURFACE_EXTENSION_NAME, // kSurface
		SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME, // kPlatformSurface
		//VK_KHR_SWAPCHAIN_EXTENSION_NAME, // kSwapchain
		//VK_EXT_DEBUG_UTILS_EXTENSION_NAME, // kDebugUtils
		};
};

	template<>
std::vector<instance_extension::properties_t> enumerate<instance_extension::properties_t>(const uint32_t max_count, const char* const layer_name)
{
	uint32_t count = 0;
	{
		[[maybe_unused]] VkResult err = vkEnumerateInstanceExtensionProperties(layer_name, &count, nullptr);
		assert(layer_name || !err);
	}
	count = std::min(count, max_count);

	std::vector<instance_extension::properties_t> result(count);
	if (count > 0)
	{
		[[maybe_unused]] VkResult err = vkEnumerateInstanceExtensionProperties(layer_name, &count, result.data());
		assert(layer_name || !err);
	}
	return result;
}

	template<>
std::vector<instance_extension::properties_t> enumerate<instance_extension::properties_t>(const char* const layer_name)
{
	return enumerate<instance_extension::properties_t>(~0u, layer_name);
}

	template<>
std::vector<instance_extension::properties_t> enumerate<instance_extension::properties_t>()
{
	return enumerate<instance_extension::properties_t>(static_cast<const char* const>(nullptr));
}

struct physical_device
{
	//using instance_t = VkInstance;
	using device_t = VkPhysicalDevice;
};

	template<>
std::vector<physical_device::device_t> enumerate<physical_device::device_t>(const uint32_t max_count, InstanceHandle instance)
{
	uint32_t count = 0;
	{
		[[maybe_unused]] VkResult err = vkEnumeratePhysicalDevices(instance, &count, nullptr);
		assert(!err);
	}
	count = std::min(count, max_count);

	std::vector<physical_device::device_t> result(count);
	if (count > 0)
	{
		[[maybe_unused]] VkResult err = vkEnumeratePhysicalDevices(instance, &count, result.data());
		assert(!err);
	}
	return result;
}

	template<>
std::vector<physical_device::device_t> enumerate<physical_device::device_t>(InstanceHandle instance)
{
	return enumerate<physical_device::device_t>(~0u, instance);
}

struct device_extension
{
	//using device_t = VkPhysicalDevice;
	using properties_t = VkExtensionProperties;
};

	template<>
std::vector<device_extension::properties_t> enumerate<device_extension::properties_t>(const uint32_t max_count, AdapterHandle adapter, const char* const layer_name)
{
	uint32_t count = 0;
	{
		[[maybe_unused]] VkResult err = vkEnumerateDeviceExtensionProperties(adapter, layer_name, &count, nullptr);
		assert(!err);
	}
	count = std::min(count, max_count);

	std::vector<device_extension::properties_t> result(count);
	if (count > 0)
	{
		[[maybe_unused]] VkResult err = vkEnumerateDeviceExtensionProperties(adapter, nullptr, &count, result.data());
		assert(!err);
	}
	return result;
}

	template<>
std::vector<device_extension::properties_t> enumerate<device_extension::properties_t>(AdapterHandle adapter, const char* const layer_name)
{
	return enumerate<device_extension::properties_t>(~0u, adapter, layer_name);
}

	template<>
std::vector<device_extension::properties_t> enumerate<device_extension::properties_t>(AdapterHandle adapter)
{
	return enumerate<device_extension::properties_t>(adapter, static_cast<const char* const>(nullptr));
}

struct queue_families
{
	using properties_t = VkQueueFamilyProperties;
};

	template<>
std::vector<queue_families::properties_t> enumerate<queue_families::properties_t>(const uint32_t max_count, AdapterHandle adapter)
{
	uint32_t count = 0;
	{
		vkGetPhysicalDeviceQueueFamilyProperties(adapter, &count, nullptr);
	}
	count = std::min(count, max_count);

	std::vector<queue_families::properties_t> result(count);
	if (count > 0)
	{
		vkGetPhysicalDeviceQueueFamilyProperties(adapter, &count, result.data());
	}
	return result;
}

	template<>
std::vector<queue_families::properties_t> enumerate<queue_families::properties_t>(AdapterHandle adapter)
{
	return enumerate<queue_families::properties_t>(~0u, adapter);
}


VkBool32 VKAPI_PTR vulkanDebugCallback(
	[[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
	[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
	[[maybe_unused]] const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
	[[maybe_unused]] void*                                            pUserData )
{
	std::cerr << "Vuklan message level " << messageSeverity << " type " << messageTypes << std::endl;
	if( pCallbackData )
		std::cerr << pCallbackData->pMessageIdName << ": " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

struct vulkanInstanceConfig : Configuration
{
	vulkanInstanceConfig() : sbLibX::Configuration{ "sbVulkanConfig"_xhash64, "0.1"_xhash64 } {}

	xhash_string_view_t applicationName = "sbVulkan"_xhash64;
	xhash_string_view_t engineName = "sbLibX"_xhash64;
	uint32_t applicationVersion = 0;
	uint32_t engineVersion = 0;

	enum layer_t : uint32_t
	{
		NV_optimus                  = ( 1u <<  0 ), // NVIDIA Optimus layer
		// 
		LUNARG_api_dump             = ( 1u <<  1 ), // LunarG debug layer
		LUNARG_device_simulation    = ( 1u <<  2 ), // LunarG device simulation layer
		LUNARG_monitor              = ( 1u <<  3 ), // Execution Monitoring Layer
		LUNARG_screenshot           = ( 1u <<  4 ), // LunarG image capture layer
		debug_utils_mask            = ( LUNARG_api_dump | LUNARG_device_simulation | LUNARG_monitor | LUNARG_screenshot ),
		// 
		LUNARG_gfxreconstruct       = ( 1u <<  5 ), // GFXReconstruct Capture Layer Version 0.9.3
		RENDERDOC_Capture           = ( 1u <<  6 ), // Debugging capture layer for RenderDoc
		graphics_tools_mask         = ( LUNARG_gfxreconstruct | RENDERDOC_Capture ),
		//- deprecated - LUNARG_vktrace              = ( 1 <<  7 ), // Vktrace tracing library
		//- deprecated - LUNARG_assistant_layer      = ( 1 <<  8 ), // LunarG Validation Factory Layer
		KHRONOS_validation          = ( 1u <<  9 ), // Khronos Validation Layer (March 2019: Supported)
		full_debug_mask             = ( debug_utils_mask | KHRONOS_validation ),
		// ...
		//- deprecated - LUNARG_standard_validation  = ( 1 << 20 ), // LunarG Standard Validation (February 2016: Deprecated)
		//- deprecated - LUNARG_core_validation      = ( 1 << 21 ), // LunarG Validation Layer (March 2016: Deprecated)
		//- deprecated - LUNARG_parameter_validation = ( 1 << 22 ), // LunarG Validation Layer (December 2014: Deprecated)
		//- deprecated - LUNARG_object_tracker       = ( 1 << 23 ), // LunarG Validation Layer (October 2014: Deprecated)
		//- deprecated - GOOGLE_unique_objects       = ( 1 << 24 ), // Google Validation Layer (December 2015: Deprecated)
		//- deprecated - GOOGLE_threading            = ( 1 << 25 ), // Google Validation Layer (April 2015: Deprecated)
		// ...
		VALVE_steam_overlay         = ( 1u <<  30 ), // Steam Overlay Layer
		VALVE_steam_fossilize       = ( 1u <<  31 ), // Steam Pipeline Caching Layer
		VALVE_mask                  = ( VALVE_steam_fossilize | VALVE_steam_overlay ),
	};

	layer_t layer_mask{
		0
		| NV_optimus                  // NVIDIA Optimus layer
		// Could activate these by default with some compile arguments
		// | debug_utils_mask
		// | graphics_tools_mask
		// | full_debug_mask
		// | VALVE_mask
	};

	std::vector<xhash_string_view_t> required_layers = {
	};
	std::vector<xhash_string_view_t> required_extensions = {
	};
};
InstanceHandle CreateInstance( [[maybe_unused]] const Configuration* config )
{
	if( config && config->name != "sbVulkanConfig"_xhash64 )
		return InstanceHandle{};

	vulkanInstanceConfig defaultConfig{};
	auto instanceConfig = config ? static_cast<const vulkanInstanceConfig*>(config) : &defaultConfig;
	const VkApplicationInfo app = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = instanceConfig->applicationName.get_value().data(),
		.applicationVersion = instanceConfig->applicationVersion,
		.pEngineName = instanceConfig->engineName.get_value().data(),
		.engineVersion = instanceConfig->engineVersion,
		.apiVersion = VK_API_VERSION_1_0,
	};

	std::vector<const char*> required_layers; required_layers.reserve( instanceConfig->required_layers.size() );
	for( const auto& hash : instanceConfig->required_layers )
		required_layers.emplace_back( hash.get_value().data() );
	std::vector<const char*> required_extensions; required_extensions.reserve( instanceConfig->required_extensions.size() );
	for( const auto& hash : instanceConfig->required_extensions )
		required_extensions.emplace_back( hash.get_value().data() );
	VkInstanceCreateInfo inst_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.pApplicationInfo = &app,
		.enabledLayerCount = static_cast<uint32_t>( required_layers.size() & 0xFFFFFFFF ),
		.ppEnabledLayerNames = required_layers.data(),
		.enabledExtensionCount = static_cast<uint32_t>( required_extensions.size() & 0xFFFFFFFF ),
		.ppEnabledExtensionNames = required_extensions.data(),
	};

#if 1 // SB_DEVICE_DEBUG_LAYER
	VkDebugUtilsMessengerCreateInfoEXT dbg_messenger_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext = nullptr,
		.flags = 0,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = &vulkanDebugCallback,
		.pUserData = nullptr,
	};

	//if ( sbUseDeviceDebugLayer )
		inst_info.pNext = &dbg_messenger_create_info;
#endif

	InstanceHandle instance;
	VkResult err = vkCreateInstance(&inst_info, nullptr, &instance);
	if (err != VK_SUCCESS)
	{
		switch (err)
		{
		case VK_ERROR_INCOMPATIBLE_DRIVER: std::cerr << "incompatible driver\n"; break;
		case VK_ERROR_EXTENSION_NOT_PRESENT: std::cerr << "extension not present\n"; break;
		case VK_ERROR_LAYER_NOT_PRESENT: std::cerr << "layer not present\n"; break;
		default: __debugbreak(); break;
		}
	}
	return err == VK_SUCCESS ? instance : nullptr;
}

bool DestroyInstance(InstanceHandle instance, [[maybe_unused]] const Configuration* config)
{
	if (instance)
	{
		vkDestroyInstance(instance, nullptr);
	}
	return true;
}

adapter_array_t EnumerateAdapters([[maybe_unused]] InstanceHandle instance, [[maybe_unused]] size_t maxCount)
{
	return enumerate<physical_device::device_t>(instance);
}

DeviceInfo GetDeviceInfo( AdapterHandle adapter )
{
	VkPhysicalDeviceProperties adapter_properties;
	vkGetPhysicalDeviceProperties(adapter, &adapter_properties);
	DeviceInfo device_info = {
		.description = {},
		.vendorID = adapter_properties.vendorID,
		.deviceID = adapter_properties.deviceID,
		.apiID = adapter_properties.apiVersion,
		.driverVersion = adapter_properties.driverVersion,
		.uid = {}
	};
	static_assert(sizeof(device_info.uid) >= sizeof(adapter_properties.pipelineCacheUUID));
	memcpy(device_info.uid, adapter_properties.pipelineCacheUUID, sizeof(adapter_properties.pipelineCacheUUID));
	static_assert( sizeof(device_info.description) >= sizeof(adapter_properties.deviceName) );
	memcpy(device_info.description, adapter_properties.deviceName, sizeof(adapter_properties.deviceName) );

	return device_info;
}

DeviceHandle CreateDevice([[maybe_unused]] AdapterHandle adapter, [[maybe_unused]] const Configuration* config)
{
	// TODO : store this in config? remove const?
	// --> SB::Configuration should be const but got a few mutable members for attaching & detaching owned data (through data handle)
	// (mostly JSON-like)
	std::clog << "vulkan queue families : " << std::endl;
	auto vulkan_queue_families = vulkan::enumerate<queue_families::properties_t>(adapter);
	for (size_t index = 0; index < vulkan_queue_families.size(); ++index)
	{
		const auto& queue_family = vulkan_queue_families[index];
		std::clog << "\tQueue Family "<< index << " (count: " << queue_family.queueCount << ")\n";
		std::clog << "\t\tMin size = (" << queue_family.minImageTransferGranularity.width
			<< ", " << queue_family.minImageTransferGranularity.height
			<< ", " << queue_family.minImageTransferGranularity.depth << ")\n";
		std::clog << "\tSupports:\n";
		if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			std::clog << "\t\tGraphics\n";
		if (queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT)
			std::clog << "\t\tCompute\n";
		if (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT)
			std::clog << "\t\tTransfer\n";
		if (queue_family.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
			std::clog << "\t\tSparse binding\n";
		if (queue_family.queueFlags & VK_QUEUE_PROTECTED_BIT)
			std::clog << "\t\tProtected\n";
		if (queue_family.queueFlags & ~unsigned(VK_QUEUE_GRAPHICS_BIT|VK_QUEUE_COMPUTE_BIT|VK_QUEUE_TRANSFER_BIT|VK_QUEUE_SPARSE_BINDING_BIT|VK_QUEUE_GRAPHICS_BIT))
			std::clog << "\t\t(unknown)\n";
	}
	std::clog.flush();

	// Usually, we'd use config information. TODO
	// In the default case, we'd like :
	// -	1 or 2 graphics queues; *could use a distinct queue for processing data like baking lighting & shadows or generating textures or vertex data in general
	// -	a few async compute queues; *generic compute workers
	// -	usually one transfer queue; *data streaming
	// lets see what happens when we ask for all queues...

	using queue_create_info = std::vector<VkDeviceQueueCreateInfo>;
	queue_create_info queues_info(vulkan_queue_families.size());
	using float_array_t = std::vector<float>;
	std::vector<float_array_t> queue_priorities(vulkan_queue_families.size());
	for ( size_t index = 0; index < vulkan_queue_families.size(); ++index )
	{
		const auto& queues_properties = vulkan_queue_families[index];

		// lets keep all of them at lowest (0.0f) priority for now... TODO
		float_array_t& family_queues_priorities = queue_priorities[index];
		family_queues_priorities.resize(queues_properties.queueCount);

		VkDeviceQueueCreateFlags queueFlags{};
		if (queues_properties.queueFlags & VK_QUEUE_PROTECTED_BIT /*&& useProtectedQueue*/)
			queueFlags = VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT;
		queues_info[index] = VkDeviceQueueCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = nullptr,
			.flags = queueFlags,
			.queueFamilyIndex = static_cast<uint32_t>(index),
			.queueCount = queues_properties.queueCount,
			.pQueuePriorities = family_queues_priorities.data(),
		};
	}

	VkDeviceCreateInfo device_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = nullptr,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = queues_info.data(),
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = 0,//TODO
		.ppEnabledExtensionNames = nullptr,//TODO
		.pEnabledFeatures = nullptr,  // If specific features are desired, pass them in here
	};

	DeviceHandle device;
	[[maybe_unused]] VkResult err = vkCreateDevice(adapter, &device_info, nullptr, &device);
	assert(!err);
	return err == VK_SUCCESS ? device : DeviceHandle{};
}
bool DestroyDevice(DeviceHandle device, [[maybe_unused]] const Configuration* config)
{
	vkDestroyDevice(device, nullptr);
	return true;
}

}}} // namespace SB::LibX::vulkan

void PrintLayerExtensions( const char* layerName )
{
	using namespace sbLibX;
	auto vulkan_layers_extensions = vulkan::enumerate<vulkan::instance_extension::properties_t>( layerName );
	if( !vulkan_layers_extensions.empty() )
	{
		std::clog << "\t\t[ ";
		for( const auto& layer_extension : vulkan_layers_extensions )
			std::clog << layer_extension.extensionName << " (" << layer_extension.specVersion << "); ";
		std::clog << "]" << std::endl;
	}
}

using vulkan_instance_layer_array_t = decltype( sbLibX::vulkan::enumerate<sbLibX::vulkan::instance_layer::properties_t>() );
void PrintInstanceLayers( const vulkan_instance_layer_array_t& vulkan_instance_layers )
{
	using namespace sbLibX;
	std::clog << "/////////////////////////////////////////////" << std::endl;
	std::clog << "Enumerating vulkan instance layers" << std::endl;
	std::clog << "/////////////////////////////////////////////" << std::endl;
	for( const auto& layer : vulkan_instance_layers )
	{
		std::clog << "\t" << layer.layerName << " ( impl. " << layer.implementationVersion << ": vers. " << layer.specVersion << ") : ";
		std::clog << layer.description << std::endl;
		PrintLayerExtensions( layer.layerName );
	}
}


using vulkan_instance_extension_array_t = decltype( sbLibX::vulkan::enumerate<sbLibX::vulkan::instance_extension::properties_t>() );
auto PrintInstanceExtensions( const vulkan_instance_extension_array_t& vulkan_instance_extensions )
{
	using namespace sbLibX;
	std::clog << "/////////////////////////////////////////////" << std::endl;
	std::clog << "Enumerating vulkan instance extensions" << std::endl;
	std::clog << "/////////////////////////////////////////////" << std::endl;
	for( const auto& extension : vulkan_instance_extensions )
	{
		std::clog << "\t" << extension.extensionName << " (" << extension.specVersion << ")" << std::endl;
	}
}

auto FilterInstanceLayers( sbLibX::vulkan::vulkanInstanceConfig& config, const vulkan_instance_layer_array_t& vulkan_instance_layers )
{
	using namespace sbLibX;
	using layer_t =  sbLibX::vulkan::vulkanInstanceConfig::layer_t;
	#define ALLOWED_LAYER( X ) case xhash_string_view_t("VK_LAYER_" #X).get_key(): if ( ( config.layer_mask & layer_t::X ) != 0 ) config.required_layers.emplace_back( xhash_string_view_t("VK_LAYER_" #X) ); break
	for( const auto& layer_property : vulkan_instance_layers )
	{
		switch( xhash_string_view_t{ layer_property.layerName }.get_key() )
		{
			ALLOWED_LAYER( NV_optimus ); // NVIDIA Optimus layer
			// 
			ALLOWED_LAYER( LUNARG_api_dump ); // LunarG debug layer
			ALLOWED_LAYER( LUNARG_device_simulation ); // LunarG device simulation layer
			ALLOWED_LAYER( LUNARG_monitor ); // Execution Monitoring Layer
			ALLOWED_LAYER( LUNARG_screenshot ); // LunarG image capture layer
			// 
			ALLOWED_LAYER( LUNARG_gfxreconstruct ); // GFXReconstruct Capture Layer Version 0.9.3
			ALLOWED_LAYER( RENDERDOC_Capture ); // Debugging capture layer for RenderDoc
			//ALLOWED_LAYER( LUNARG_vktrace ); // Vktrace tracing library
			//ALLOWED_LAYER( LUNARG_assistant_layer ); // LunarG Validation Factory Layer
			ALLOWED_LAYER( KHRONOS_validation ); // Khronos Validation Layer (March 2019: Supported)
			// ...
			//ALLOWED_LAYER( LUNARG_standard_validation ); // LunarG Standard Validation
			//ALLOWED_LAYER( LUNARG_core_validation ); // LunarG Validation Layer
			//ALLOWED_LAYER( LUNARG_parameter_validation ); // LunarG Validation Layer
			//ALLOWED_LAYER( LUNARG_object_tracker ); // LunarG Validation Layer
			//ALLOWED_LAYER( GOOGLE_unique_objects ); // Google Validation Layer
			//ALLOWED_LAYER( GOOGLE_threading ); // Google Validation Layer
			// ...
			ALLOWED_LAYER( VALVE_steam_overlay ); // Steam Overlay Layer
			ALLOWED_LAYER( VALVE_steam_fossilize ); // Steam Pipeline Caching Layer
		}
	}
	#undef ALLOWED_LAYER
}

auto FilterInstanceExtensions( sbLibX::vulkan::vulkanInstanceConfig& config, const vulkan_instance_extension_array_t& vulkan_instance_extensions )
{
	using namespace sbLibX;
	auto requiresExtension = [&config]( xhash_string_view_t feature ) -> bool {
		auto found = std::find_if( config.required_layers.cbegin(), config.required_layers.cend(), [feature]( xhash_string_view_t layer ) -> bool {
			auto vulkan_layers_extensions = vulkan::enumerate<vulkan::instance_extension::properties_t>( layer.get_value().data() );
			auto found_internal = std::find_if( vulkan_layers_extensions.cbegin(), vulkan_layers_extensions.cend(), [feature]( auto properties ) -> bool {
				return xhash_string_view_t{ properties.extensionName } == feature;
			} ) != vulkan_layers_extensions.cend();
			return found_internal;
		} ) != config.required_layers.cend();
		return found;
	};
	const bool requiresDebugReport = requiresExtension( "VK_EXT_debug_report"_xhash64 );
	const bool requiresDebugUtils = requiresExtension( "VK_EXT_debug_utils"_xhash64 );

	#define ALLOWED_EXTENSION( X, isAllowed ) case xhash_string_view_t(#X).get_key(): if( isAllowed ) config.required_extensions.emplace_back( xhash_string_view_t(#X) ); break
	for( const auto& extension_property : vulkan_instance_extensions )
	{
		switch( xhash_string_view_t{ extension_property.extensionName }.get_key() )
		{
			ALLOWED_EXTENSION( VK_KHR_surface, true );
			ALLOWED_EXTENSION( VK_KHR_win32_surface, true );
			ALLOWED_EXTENSION( VK_KHR_external_memory_capabilities, true );
			ALLOWED_EXTENSION( VK_KHR_external_semaphore_capabilities, true );
			ALLOWED_EXTENSION( VK_KHR_external_fence_capabilities, true );
			ALLOWED_EXTENSION( VK_KHR_get_physical_device_properties2, true );
			ALLOWED_EXTENSION( VK_KHR_get_surface_capabilities2, true );
			ALLOWED_EXTENSION( VK_KHR_device_group_creation, true );
			ALLOWED_EXTENSION( VK_EXT_swapchain_colorspace, true );

			ALLOWED_EXTENSION( VK_KHR_surface_protected_capabilities, true );
			ALLOWED_EXTENSION( VK_NV_external_memory_capabilities, true );

			ALLOWED_EXTENSION( VK_EXT_debug_report, requiresDebugReport );
			ALLOWED_EXTENSION( VK_EXT_debug_utils, requiresDebugUtils );
		}
	}
	#undef ALLOWED_EXTENSION
}

void SortAdapters( sbLibX::vulkan::adapter_array_t& vulkan_adapters )
{
	using namespace sbLibX;
	auto vendor_order = []( uint32_t vendor ) -> uint32_t
	{
		enum vendor_t
		{
			NVIDIA    = 0x10DE,
			AMD       = 0x1002,
			Intel     = 0x8086,
			Microsoft = 0x1414,
		};
		switch( static_cast<vendor_t>( vendor ) )
		{
		case vendor_t::NVIDIA:    return 0x00000100;
		case vendor_t::AMD:       return 0x00000200;
		case vendor_t::Intel:     return 0x00000400;
			// Microsoft is usually a software (possibly hardware accelerated) device, so considered as debug-only
		case vendor_t::Microsoft: return 0x80000000;
		}
		return vendor;
	};
	[[maybe_unused]]
	constexpr uint32_t kDebugDeviceStart = 0x00010000;
	auto vulkan_adapter_sort_predicate = [&vendor_order]( const vulkan::AdapterHandle& first, const vulkan::AdapterHandle& second ) -> bool
	{
		vulkan::DeviceInfo device1_info = vulkan::GetDeviceInfo( first );
		vulkan::DeviceInfo device2_info = vulkan::GetDeviceInfo( second );
		// TODO add sort information here from config
		return vendor_order( device1_info.vendorID ) < vendor_order( device2_info.vendorID );
	};

	std::sort( vulkan_adapters.begin(), vulkan_adapters.end(), vulkan_adapter_sort_predicate );
}


auto CreateDevices( sbLibX::vulkan::adapter_array_t& vulkan_adapters )
{
	using namespace sbLibX;
	std::vector< std::tuple<vulkan_device, vulkan::DeviceInfo> > vulkan_devices;
	vulkan_devices.reserve( vulkan_adapters.size() );
	for( const auto& adapter : vulkan_adapters )
	{
		vulkan::DeviceInfo device_info = vulkan::GetDeviceInfo( adapter );
		std::clog << device_info.description << " : " << std::endl;
		auto vulkan_device_extensions = vulkan::enumerate<vulkan::device_extension::properties_t>( adapter );
		for( const auto& extension_properties : vulkan_device_extensions )
		{
			std::clog << "\t" << extension_properties.extensionName << std::endl;
		}
		vulkan_devices.emplace_back( std::make_tuple( vulkan_device( adapter ), device_info ) );
	}
	return vulkan_devices;
}

#include <algorithm>
int TestVulkan( void* hwnd )
{
	using namespace sbLibX;
	std::clog << std::endl;

	const auto vulkan_instance_layers = vulkan::enumerate<vulkan::instance_layer::properties_t>();
	PrintInstanceLayers( vulkan_instance_layers );

	auto vulkan_instance_extensions = vulkan::enumerate<vulkan::instance_extension::properties_t>();
	PrintInstanceExtensions( vulkan_instance_extensions );

	vulkan::vulkanInstanceConfig config{};
	config.applicationName = "sbVulkan"_xhash64;
	config.engineName = "sbLibX"_xhash64;
	config.applicationVersion = 0;
	config.engineVersion = 0;

	//if( useDebugLayers )
	//	config.layer_mask |= debug_utils_mask;
	//if( useGraphicToolsLayers )
	//	config.layer_mask |= graphics_tools_mask;
	//if( useValidationLayers )
	//	config.layer_mask |= full_debug_mask;
	//if( useValveLayers )
	//	config.layer_mask |= VALVE_mask;

	{
		config.required_layers.reserve( vulkan_instance_layers.size() );
		FilterInstanceLayers( config, vulkan_instance_layers );
		config.required_extensions.reserve( vulkan_instance_extensions.size() );
		FilterInstanceExtensions( config, vulkan_instance_extensions );
	}

	vulkan_instance instance( &config );
	if( !instance )
		return -1;

	std::clog << "/////////////////////////////////////////////" << std::endl;
	std::clog << "Enumerating devices w/extensions" << std::endl;
	std::clog << "/////////////////////////////////////////////" << std::endl;
	auto vulkan_adapters = vulkan::EnumerateAdapters(instance);
	SortAdapters( vulkan_adapters );

	auto vulkan_devices = CreateDevices( vulkan_adapters );

	const bool hasWin32Surface = std::find_if( vulkan_instance_extensions.cbegin(), vulkan_instance_extensions.cend(),
		[]( const vulkan::instance_extension::properties_t& properties ) -> bool {
			return xhash_string_view_t{ properties.extensionName } == "VK_KHR_win32_surface"_xhash64;
	} ) != vulkan_instance_extensions.cend();

	VkAllocationCallbacks* allocator = nullptr; // TODO
	VkSurfaceKHR surface{};
	if( hasWin32Surface )
	{
		VkWin32SurfaceCreateInfoKHR createInfo{
			/*VkStructureType                 */ .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			/*const void*                     */ .pNext{},
			/*VkWin32SurfaceCreateFlagsKHR    */ .flags{},
			/*HINSTANCE                       */ .hinstance = GetModuleHandleW( nullptr ),
			/*HWND                            */ .hwnd = reinterpret_cast<HWND>( hwnd ),
		};

		//VKAPI_ATTR VkResult VKAPI_CALL vkCreateWin32SurfaceKHR(
		//	VkInstance                                  instance,
		//	const VkWin32SurfaceCreateInfoKHR*          pCreateInfo,
		//	const VkAllocationCallbacks*                pAllocator,
		//	VkSurfaceKHR*                               pSurface );
		VkResult surfaceResult = vkCreateWin32SurfaceKHR( instance, &createInfo, allocator, &surface );
		if( surfaceResult != VK_SUCCESS )
		{
			std::cerr << "Could not create surface. Error was " << surfaceResult << std::endl;
		}
	}


	if( surface )
	{
		//surface

		decltype( surface ) releasedSurface{};
		std::swap( surface, releasedSurface );
		vkDestroySurfaceKHR( instance, releasedSurface, allocator );
	}

	// check see what happens with more than one vulan instance
	[[maybe_unused]]
	auto another_vulkan_instance = vulkan::CreateInstance();
	return 0;
}
