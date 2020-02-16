#include <test/include/test_vulkan.h>
#include <vulkan/vulkan.h>
//#include <vulkan/vk_sdk_platform.h>

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

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

InstanceHandle CreateInstance( [[maybe_unused]] const SB::Configuration* config )
{
	const VkApplicationInfo app = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = "SBTestVulkan",
		.applicationVersion = 0,
		.pEngineName = "SBLibXDev",
		.engineVersion = 0,
		.apiVersion = VK_API_VERSION_1_0,
	};
	VkInstanceCreateInfo inst_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.pApplicationInfo = &app,
		.enabledLayerCount = false,//demo->enabled_layer_count,
		.ppEnabledLayerNames = nullptr,//(const char* const*)instance_validation_layers,
		.enabledExtensionCount = 0,//demo->enabled_extension_count,
		.ppEnabledExtensionNames = nullptr,//(const char* const*)demo->extension_names,
	};

#if 1 // SB_DEVICE_DEBUG_LAYER
	VkDebugUtilsMessengerCreateInfoEXT dbg_messenger_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext = nullptr,
		.flags = 0,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = nullptr,
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
		case VK_ERROR_INCOMPATIBLE_DRIVER: __debugbreak(); break;
		case VK_ERROR_EXTENSION_NOT_PRESENT: __debugbreak(); break;
		default: __debugbreak(); break;
		};
	}
	return err == VK_SUCCESS ? instance : nullptr;
}

bool DestroyInstance(InstanceHandle instance, [[maybe_unused]] const SB::Configuration* config)
{
	if (instance)
	{
		vkDestroyInstance(instance, nullptr);
	}
	return true;
}

adapter_array_t GetDeviceAdapters( InstanceHandle instance )
{
	return enumerate<physical_device::device_t>(instance);
}

DeviceInfo GetDeviceInfo( AdapterHandle adapter )
{
	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(adapter, &device_properties);
	DeviceInfo device_info = {
		.description = "",
		.vendorID = device_properties.vendorID,
		.deviceID = device_properties.deviceID,
		.apiID = device_properties.apiVersion,
		.driverVersion = device_properties.driverVersion,
		.deviceType = static_cast<uint32_t>(device_properties.deviceType),
		.uid = "",
	};
	static_assert( sizeof(device_info.description) == sizeof(device_properties.deviceName) );
	memcpy( const_cast<char*>(device_info.description), device_properties.deviceName, sizeof(device_properties.deviceName) );

	static_assert( sizeof(device_info.uid) == sizeof(device_properties.pipelineCacheUUID) );
	memcpy( const_cast<char*>(device_info.uid), device_properties.pipelineCacheUUID, sizeof(device_properties.pipelineCacheUUID) );
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
		if (queue_family.queueFlags & ~(VK_QUEUE_GRAPHICS_BIT|VK_QUEUE_COMPUTE_BIT|VK_QUEUE_TRANSFER_BIT|VK_QUEUE_SPARSE_BINDING_BIT|VK_QUEUE_GRAPHICS_BIT))
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
		.pEnabledFeatures = nullptr,  // If specific features are required, pass them in here
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


int TestVulkan()
{
	using namespace SB::LibX;
	std::clog << std::endl;
	std::clog << "/////////////////////////////////////////////" << std::endl;
	std::clog << "Enumerating vulkan instance layers" << std::endl;
	std::clog << "/////////////////////////////////////////////" << std::endl;
	auto vulkan_instance_layer = vulkan::enumerate<vulkan::instance_layer::properties_t>();
	for (const auto& layer : vulkan_instance_layer)
	{
		std::clog << "\t" << layer.layerName << " (" << layer.implementationVersion << "." << layer.specVersion << ") : ";
		std::clog << layer.description << std::endl;

		auto vulkan_layers_extensions = vulkan::enumerate<vulkan::instance_extension::properties_t>(layer.layerName);
		if (!vulkan_layers_extensions.empty())
		{
			std::clog << "\t\tuses : ";
			for (const auto& layer_extension : vulkan_layers_extensions)
				std::clog << layer_extension.extensionName << " (" << layer_extension.specVersion << "); ";
			std::clog << std::endl;
		}
	}

	std::clog << "/////////////////////////////////////////////" << std::endl;
	std::clog << "Enumerating vulkan instance extensions" << std::endl;
	std::clog << "/////////////////////////////////////////////" << std::endl;
	auto vulkan_instance_extensions = vulkan::enumerate<vulkan::instance_extension::properties_t>();
	for (const auto& extension : vulkan_instance_extensions)
	{
		std::clog << "\t" << extension.extensionName << " (" << extension.specVersion << ")" << std::endl;
	}

	vulkan_instance instance;
#if 0
	// Sample of expected instance_extension_array
	//+ [0]{ extensionName = 0x000002a3b04f2000 "VK_KHR_surface" specVersion = 25 }	VkExtensionProperties
	//+ [1]{ extensionName = 0x000002a3b04f2104 "VK_KHR_win32_surface" specVersion = 6 }	VkExtensionProperties
	//+ [2]{ extensionName = 0x000002a3b04f2208 "VK_KHR_external_memory_capabilities" specVersion = 1 }	VkExtensionProperties
	//+ [3]{ extensionName = 0x000002a3b04f230c "VK_KHR_external_semaphore_capabilities" specVersion = 1 }	VkExtensionProperties
	//+ [4]{ extensionName = 0x000002a3b04f2410 "VK_KHR_external_fence_capabilities" specVersion = 1 }	VkExtensionProperties
	//+ [5]{ extensionName = 0x000002a3b04f2514 "VK_KHR_get_physical_device_properties2" specVersion = 1 }	VkExtensionProperties
	//+ [6]{ extensionName = 0x000002a3b04f2618 "VK_KHR_get_surface_capabilities2" specVersion = 1 }	VkExtensionProperties
	//+ [7]{ extensionName = 0x000002a3b04f271c "VK_KHR_device_group_creation" specVersion = 1 }	VkExtensionProperties
	//+ [8]{ extensionName = 0x000002a3b04f2820 "VK_EXT_swapchain_colorspace" specVersion = 3 }	VkExtensionProperties
	//+ [9]{ extensionName = 0x000002a3b04f2924 "VK_EXT_debug_report" specVersion = 9 }	VkExtensionProperties
	//+ [10]{ extensionName = 0x000002a3b04f2a28 "VK_EXT_debug_utils" specVersion = 1 }	VkExtensionProperties
	auto vulkan_instance_extensions = vulkan::instance_extension::enumerate();
	for (int id = 0; id < vulkan::instance_extension::required_count; ++id)
	{
		std::string cur_name = vulkan::instance_extension::name[id];
		auto is_equal = [id, cur_name](const vulkan::instance_extension::properties_t& properties) -> bool { return cur_name == properties.extensionName; };
		if (std::find_if(vulkan_instance_extensions.begin(), vulkan_instance_extensions.end(), is_equal) == vulkan_instance_extensions.end())
			__debugbreak(); // not found
	}

	auto vulkan_instance = vulkan::CreateInstance();
#endif


	std::clog << "/////////////////////////////////////////////" << std::endl;
	std::clog << "Enumerating devices w/extensions" << std::endl;
	std::clog << "/////////////////////////////////////////////" << std::endl;
	auto vulkan_adapters = vulkan::GetDeviceAdapters(instance);

	std::vector<vulkan_device> vulkan_devices;
	vulkan_devices.reserve(vulkan_adapters.size());
	for (const auto& adapter : vulkan_adapters)
	{
		vulkan::DeviceInfo device_info = vulkan::GetDeviceInfo(adapter);
		std::clog << device_info.description << " : " << std::endl;
		auto vulkan_device_extensions = vulkan::enumerate<vulkan::device_extension::properties_t>(adapter);
		for (const auto& extension_properties : vulkan_device_extensions)
		{
			std::clog << "\t" << extension_properties.extensionName << std::endl;
		}
		vulkan_devices.emplace_back(vulkan_device(adapter));
	}
	return 0;
}
