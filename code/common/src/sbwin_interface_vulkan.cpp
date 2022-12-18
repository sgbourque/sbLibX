#if defined(SBWIN64) || defined(SBWIN32)
	#ifndef NOMINMAX
	#define NOMINMAX
	#endif
	#define VK_USE_PLATFORM_WIN32_KHR	1
#endif
#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>
#pragma comment(lib, "vulkan-1.lib")
#pragma comment(lib, "VkLayer_utils.lib")


#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include <common/include/sb_encrypted_string.h>
#include <common/include/sb_interface_vulkan.h>


//#define SB_HASH_TRAITS
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

namespace SB { namespace LibX { namespace vulkan
{

////
	template<>
SB_WIN_EXPORT std::vector<instance_layer_traits::properties_t> enumerate<instance_layer_traits::properties_t>(const uint32_t max_count)
{
	uint32_t count = 0;
	{
		[[maybe_unused]] VkResult err = vkEnumerateInstanceLayerProperties(&count, nullptr);
		assert(!err);
	}
	count = std::min(count, max_count);

	std::vector<instance_layer_traits::properties_t> result(count);
	if (count > 0)
	{
		[[maybe_unused]] VkResult err = vkEnumerateInstanceLayerProperties(&count, result.data());
		assert(!err);
	}
	return result;
}

////
struct instance_extension
{
	using properties_t = typename instance_extension_traits::properties_t;
	//using extension_id_t = typename instance_extension_traits::extension_id_t;
	enum extension_id_t
	{
		kGeneric = 0,
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

	//inline static const char name[extension_id_t::kMaxExtension][VK_MAX_EXTENSION_NAME_SIZE]
	//= {
	//	"Generic",
	//	VK_KHR_SURFACE_EXTENSION_NAME, // kSurface
	//	SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME, // kPlatformSurface
	//	//VK_KHR_SWAPCHAIN_EXTENSION_NAME, // kSwapchain
	//	//VK_EXT_DEBUG_UTILS_EXTENSION_NAME, // kDebugUtils
	//	};
};

	template<>
SB_WIN_EXPORT std::vector<instance_extension::properties_t> enumerate<instance_extension::properties_t>(const uint32_t max_count, const char* const layer_name)
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

struct physical_device
{
	//using instance_t = VkInstance;
	using device_t = VkPhysicalDevice;
};

	template<>
SB_WIN_EXPORT std::vector<physical_device::device_t> enumerate<physical_device::device_t>(const uint32_t max_count, InstanceHandle instance)
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
SB_WIN_EXPORT std::vector<physical_device::device_t> enumerate<physical_device::device_t>(InstanceHandle instance)
{
	return enumerate<physical_device::device_t>(~0u, instance);
}


	template<>
SB_WIN_EXPORT std::vector<device_extension_traits::properties_t> enumerate<device_extension_traits::properties_t>(const uint32_t max_count, AdapterHandle adapter, const char* const layer_name)
{
	uint32_t count = 0;
	{
		[[maybe_unused]] VkResult err = vkEnumerateDeviceExtensionProperties(adapter, layer_name, &count, nullptr);
		assert(!err);
	}
	count = std::min(count, max_count);

	std::vector<device_extension_traits::properties_t> result(count);
	if (count > 0)
	{
		[[maybe_unused]] VkResult err = vkEnumerateDeviceExtensionProperties(adapter, nullptr, &count, result.data());
		assert(!err);
	}
	return result;
}

	template<>
SB_WIN_EXPORT std::vector<device_extension_traits::properties_t> enumerate<device_extension_traits::properties_t>(AdapterHandle adapter, const char* const layer_name)
{
	return enumerate<device_extension_traits::properties_t>(~0u, adapter, layer_name);
}

	template<>
SB_WIN_EXPORT std::vector<device_extension_traits::properties_t> enumerate<device_extension_traits::properties_t>(AdapterHandle adapter)
{
	return enumerate<device_extension_traits::properties_t>(adapter, static_cast<const char* const>(nullptr));
}

struct queue_families
{
	using properties_t = VkQueueFamilyProperties;
};

	template<>
SB_WIN_EXPORT std::vector<queue_families::properties_t> enumerate<queue_families::properties_t>(const uint32_t max_count, AdapterHandle adapter)
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
SB_WIN_EXPORT std::vector<queue_families::properties_t> enumerate<queue_families::properties_t>(AdapterHandle adapter)
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

SB_WIN_EXPORT InstanceHandle CreateInstance( [[maybe_unused]] const Configuration* config )
{
	if( config && config->name != "sbVulkanConfig"_xhash64 )
		return InstanceHandle{};

	InstanceConfiguration defaultConfig{};
	auto instanceConfig = config ? static_cast<const InstanceConfiguration*>( config ) : &defaultConfig;
	auto requestedExtension = [instanceConfig]( xhash_string_view_t feature ) -> bool {
		auto found = std::find_if( instanceConfig->requested_extensions.cbegin(), instanceConfig->requested_extensions.cend(), [feature]( xhash_string_view_t extension ) -> bool {
			return feature == extension;
		} ) != instanceConfig->requested_extensions.cend();
		return found;
	};
	const bool requires1_1 = requestedExtension( "VK_KHR_surface_protected_capabilities" );

	
	const VkApplicationInfo app = {
		SB_STRUCT_SET( .sType             , =,  VK_STRUCTURE_TYPE_APPLICATION_INFO ),
		SB_STRUCT_SET( .pNext             , =,  nullptr ),
		SB_STRUCT_SET( .pApplicationName  , =,  instanceConfig->applicationName.get_value().data() ),
		SB_STRUCT_SET( .applicationVersion, =,  instanceConfig->applicationVersion ),
		SB_STRUCT_SET( .pEngineName       , =,  instanceConfig->engineName.get_value().data() ),
		SB_STRUCT_SET( .engineVersion     , =,  instanceConfig->engineVersion ),
		SB_STRUCT_SET( .apiVersion        , =,  requires1_1 ? VK_API_VERSION_1_1 : VK_API_VERSION_1_0 ),
	};

	std::vector<const char*> requested_layers; requested_layers.reserve( instanceConfig->requested_layers.size() );
	for( const auto& hash : instanceConfig->requested_layers )
		requested_layers.emplace_back( hash.get_value().data() );
	std::vector<const char*> requested_extensions; requested_extensions.reserve( instanceConfig->requested_extensions.size() );
	for( const auto& hash : instanceConfig->requested_extensions )
		requested_extensions.emplace_back( hash.get_value().data() );
	VkInstanceCreateInfo inst_info = {
		SB_STRUCT_SET( .sType                  , =,  VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO ),
		SB_STRUCT_SET( .pNext                  , =,  nullptr ),
		SB_STRUCT_SET( .flags                  , =,  0 ), // VkInstanceCreateFlags is a bitmask type for setting a mask, but is currently reserved for future use.
		SB_STRUCT_SET( .pApplicationInfo       , =,  &app ),
		SB_STRUCT_SET( .enabledLayerCount      , =,  static_cast<uint32_t>( requested_layers.size() & 0xFFFFFFFF ) ),
		SB_STRUCT_SET( .ppEnabledLayerNames    , =,  requested_layers.data() ),
		SB_STRUCT_SET( .enabledExtensionCount  , =,  static_cast<uint32_t>( requested_extensions.size() & 0xFFFFFFFF ) ),
		SB_STRUCT_SET( .ppEnabledExtensionNames, =,  requested_extensions.data() ),
	};

#if 1 // SB_DEVICE_DEBUG_LAYER
	VkDebugUtilsMessengerCreateInfoEXT dbg_messenger_create_info = {
		SB_STRUCT_SET( .sType          , =,  VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT ),
		SB_STRUCT_SET( .pNext          , =,  nullptr ),
		SB_STRUCT_SET( .flags          , =,  0 ),
		SB_STRUCT_SET( .messageSeverity, =,  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT ),
		SB_STRUCT_SET( .messageType    , =,  VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT ),
		SB_STRUCT_SET( .pfnUserCallback, =,  &vulkanDebugCallback ),
		SB_STRUCT_SET( .pUserData      , =,  nullptr ),
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

SB_WIN_EXPORT bool DestroyInstance(InstanceHandle instance, [[maybe_unused]] const Configuration* config)
{
	if (instance)
	{
		vkDestroyInstance(instance, nullptr);
	}
	return true;
}

SB_WIN_EXPORT adapter_array_t EnumerateAdapters([[maybe_unused]] InstanceHandle instance, [[maybe_unused]] size_t maxCount)
{
	return enumerate<physical_device::device_t>(instance);
}

SB_WIN_EXPORT DeviceDesc GetDeviceDesc( AdapterHandle adapter )
{
	VkPhysicalDeviceProperties adapter_properties;
	vkGetPhysicalDeviceProperties(adapter, &adapter_properties);
	DeviceDesc device_desc = {
		SB_STRUCT_SET( .name         , =,  {} ),
		SB_STRUCT_SET( .vendorID     , =,  vendor_t{adapter_properties.vendorID} ),
		SB_STRUCT_SET( .deviceID     , =,  adapter_properties.deviceID ),
		SB_STRUCT_SET( .apiID        , =,  adapter_properties.apiVersion ),
		SB_STRUCT_SET( .driverVersion, =,  adapter_properties.driverVersion ),
		SB_STRUCT_SET( .uid          , =, {} ),
	};
	static_assert(sizeof(device_desc.uid) >= sizeof(adapter_properties.pipelineCacheUUID));
	memcpy(device_desc.uid, adapter_properties.pipelineCacheUUID, sizeof(adapter_properties.pipelineCacheUUID));
	static_assert( sizeof(device_desc.name) >= sizeof(adapter_properties.deviceName) );
	memcpy(device_desc.name, adapter_properties.deviceName, sizeof(adapter_properties.deviceName) );

	return device_desc;
}

SB_WIN_EXPORT DeviceHandle CreateDevice([[maybe_unused]] AdapterHandle adapter, [[maybe_unused]] const Configuration* config)
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
			SB_STRUCT_SET( .sType           , =,  VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO ),
			SB_STRUCT_SET( .pNext           , =,  nullptr ),
			SB_STRUCT_SET( .flags           , =,  queueFlags ),
			SB_STRUCT_SET( .queueFamilyIndex, =,  static_cast<uint32_t>(index) ),
			SB_STRUCT_SET( .queueCount      , =,  queues_properties.queueCount ),
			SB_STRUCT_SET( .pQueuePriorities, =,  family_queues_priorities.data() ),
		};
	}

	VkDeviceCreateInfo device_info = {
		SB_STRUCT_SET( .sType                  , =,  VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO ),
		SB_STRUCT_SET( .pNext                  , =,  nullptr ),
		SB_STRUCT_SET( .flags                  , =,  0 ), // reserved for future use
		SB_STRUCT_SET( .queueCreateInfoCount   , =,  1 ),
		SB_STRUCT_SET( .pQueueCreateInfos      , =,  queues_info.data() ),
		SB_STRUCT_SET( .enabledLayerCount      , =,  0 ),
		SB_STRUCT_SET( .ppEnabledLayerNames    , =,  nullptr ),
		SB_STRUCT_SET( .enabledExtensionCount  , =,  0 ),//TODO
		SB_STRUCT_SET( .ppEnabledExtensionNames, =,  nullptr ),//TODO
		SB_STRUCT_SET( .pEnabledFeatures       , =,  nullptr ),  // If specific features are desired, pass them in here
	};

	DeviceHandle device{};
	[[maybe_unused]] VkResult err = vkCreateDevice( adapter, &device_info, nullptr, &device );
	assert(!err);
	return err == VK_SUCCESS ? device : DeviceHandle{};
}
SB_WIN_EXPORT bool DestroyDevice(DeviceHandle device, [[maybe_unused]] const Configuration* config)
{
	vkDestroyDevice(device, nullptr);
	return true;
}

}}} // namespace SB::LibX::vulkan
