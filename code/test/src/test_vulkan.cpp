#include <common/include/sb_interface_vulkan.h>

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include <common/include/sb_encrypted_string.h>

void PrintLayerExtensions( const char* layerName )
{
	using namespace sbLibVulkan;
	auto vulkan_layers_extensions = enumerate<instance_extension_traits::properties_t>( layerName );
	if( !vulkan_layers_extensions.empty() )
	{
		std::clog << "\t\t[ ";
		for( const auto& layer_extension : vulkan_layers_extensions )
			std::clog << layer_extension.extensionName << " (" << layer_extension.specVersion << "); ";
		std::clog << "]" << std::endl;
	}
}

using vulkan_instance_layer_array_t = decltype( sbLibVulkan::enumerate<sbLibVulkan::instance_layer_traits::properties_t>() );
void PrintInstanceLayers( const vulkan_instance_layer_array_t& vulkan_instance_layers )
{
	using namespace sbLibVulkan;
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


using vulkan_instance_extension_array_t = decltype( sbLibVulkan::enumerate<sbLibVulkan::instance_extension_traits::properties_t>() );
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

auto FilterInstanceLayers( sbLibVulkan::InstanceConfiguration& config, const vulkan_instance_layer_array_t& vulkan_instance_layers )
{
	using namespace sbLibX;
	using instance_layer_t =  sbLibVulkan::instance_layer_t;
	for( const auto& layer_property : vulkan_instance_layers )
	{
		switch( xhash_string_view_t{ layer_property.layerName }.get_key() )
		{
			#define SB_INSTANCE_LAYER_DEF( name, type, value ) \
			case xhash_string_view_t("VK_LAYER_" # name).get_key(): \
				if ( ( config.layer_mask & instance_layer_t::name ) != instance_layer_t::none ) \
					config.requested_layers.emplace_back( xhash_string_view_t("VK_LAYER_" # name) ); \
				break;
			#include <common/include/internal/vulkan/vulkan_instance_layers.h>
		}
	}
}

auto FilterInstanceExtensions( sbLibVulkan::InstanceConfiguration& config, const vulkan_instance_extension_array_t& vulkan_instance_extensions )
{
	using namespace sbLibX;
	using namespace sbLibVulkan;
	auto requestedExtension = [&config]( xhash_string_view_t feature ) -> bool {
		auto found = std::find_if( config.requested_layers.cbegin(), config.requested_layers.cend(), [feature]( xhash_string_view_t layer ) -> bool {
			auto vulkan_layers_extensions = enumerate<instance_extension_traits::properties_t>( layer.get_value().data() );
			auto found_internal = std::find_if( vulkan_layers_extensions.cbegin(), vulkan_layers_extensions.cend(), [feature]( auto properties ) -> bool {
				return xhash_string_view_t{ properties.extensionName } == feature;
			} ) != vulkan_layers_extensions.cend();
			return found_internal;
		} ) != config.requested_layers.cend();
		return found;
	};
	const bool requiresDebugReport = requestedExtension( "VK_EXT_debug_report"_xhash64 );
	const bool requiresDebugUtils = requestedExtension( "VK_EXT_debug_utils"_xhash64 );
	const bool requiresValidationFeature = requestedExtension( "VK_EXT_validation_features"_xhash64 );

	//RDOC 022076: [22:28:01] vk_device_funcs.cpp( 403 ) - Error   - RenderDoc does not support instance extension 'VK_KHR_surface_protected_capabilities'.
	const bool supportsProtectedSurface = !requiresDebugUtils &&
		std::find_if( config.requested_extensions.cbegin(), config.requested_extensions.cend(), []( xhash_string_view_t feature ) -> bool { return feature == "VK_KHR_get_physical_device_properties2"_xhash64; } ) == config.requested_extensions.cend();

	#define ALLOWED_EXTENSION( X, isAllowed ) case xhash_string_view_t(#X).get_key(): if( isAllowed ) config.requested_extensions.emplace_back( xhash_string_view_t(#X) ); break
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

			ALLOWED_EXTENSION( VK_KHR_surface_protected_capabilities, supportsProtectedSurface );
			ALLOWED_EXTENSION( VK_NV_external_memory_capabilities, true );

			ALLOWED_EXTENSION( VK_EXT_debug_report, requiresDebugReport );
			ALLOWED_EXTENSION( VK_EXT_debug_utils, requiresDebugUtils );
			ALLOWED_EXTENSION( VK_EXT_validation_features, requiresValidationFeature );

			default:
			{
				std::cout << "Unrecognized extension: " << extension_property.extensionName << "\n";
				//__debugbreak();
			}
		}
	}
	#undef ALLOWED_EXTENSION
}

void SortAdapters( sbLibVulkan::adapter_array_t& vulkan_adapters )
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
		vulkan::DeviceDesc device1_desc = vulkan::GetDeviceDesc( first );
		vulkan::DeviceDesc device2_desc = vulkan::GetDeviceDesc( second );
		// TODO add sort information here from config
		return vendor_order( device1_desc.vendorID ) < vendor_order( device2_desc.vendorID );
	};

	std::sort( vulkan_adapters.begin(), vulkan_adapters.end(), vulkan_adapter_sort_predicate );
}


auto CreateDevices( const sbLibVulkan::adapter_array_t& adapters )
{
	using namespace sbLibX;
	using namespace vulkan;
	std::vector< std::tuple<unique_device, DeviceDesc> > devices;
	devices.reserve( adapters.size() );
	for( const auto& adapter : adapters )
	{
		DeviceDesc device_info = GetDeviceDesc( adapter );
		std::clog << device_info.name << " : " << std::endl;
		auto device_extensions = enumerate<device_extension_traits::properties_t>( adapter );
		for( const auto& extension_properties : device_extensions )
		{
			std::clog << "\t" << extension_properties.extensionName << std::endl;
		}
		devices.emplace_back( std::make_tuple( unique_device( adapter ), device_info ) );
	}
	return devices;
}


// I know, it's still can't be run by itself (because I depends on a UI api to create a swapchain).
// Once I have a proper Win32 wrapper interface, I can make test_vulkan as an independant unit.
// For now, it is part of test_main (which eventually should be transformed as the central runtime unit test
// entry point).

#include <algorithm>
int TestVulkan( [[maybe_unused]] void* hwnd )
{
	using namespace sbLibX;
	std::clog << std::endl;

	const auto vulkan_instance_layers = vulkan::enumerate<vulkan::instance_layer_traits::properties_t>();
	PrintInstanceLayers( vulkan_instance_layers );

	auto vulkan_instance_extensions = vulkan::enumerate<vulkan::instance_extension_traits::properties_t>();
	PrintInstanceExtensions( vulkan_instance_extensions );

	vulkan::InstanceConfiguration config{};
	config.applicationName = "sbVulkan"_xhash64;
	config.engineName = "sbLibX"_xhash64;
	config.applicationVersion = 0;
	config.engineVersion = 0;

	////if( useValidationLayers )
	//	config.layer_mask |= vulkan::instance_layer_t::validation_mask;
	////if( useApiUtils )
	//	config.layer_mask |= vulkan::instance_layer_t::utility_mask;
	////if( useGraphicToolsLayers )
	//	config.layer_mask |= vulkan::instance_layer_t::LUNARG_gfxreconstruct | vulkan::instance_layer_t::RENDERDOC_Capture;
	////if( useValveLayers )
	//	config.layer_mask |= vulkan::instance_layer_t::VALVE_mask;

	{
		config.requested_layers.reserve( vulkan_instance_layers.size() );
		FilterInstanceLayers( config, vulkan_instance_layers );
		config.requested_extensions.reserve( vulkan_instance_extensions.size() );
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
	// RenderDoc only supports a single adapter : only keep the first one
	if( ( config.layer_mask & vulkan::instance_layer_t::RENDERDOC_Capture ) != vulkan::instance_layer_t::none )
		vulkan_adapters.resize( 1 );

	auto vulkan_devices = CreateDevices( vulkan_adapters );

	const bool hasWin32Surface = std::find_if( vulkan_instance_extensions.cbegin(), vulkan_instance_extensions.cend(),
		[]( const vulkan::instance_extension_traits::properties_t& properties ) -> bool {
			return xhash_string_view_t{ properties.extensionName } == "VK_KHR_win32_surface"_xhash64;
	} ) != vulkan_instance_extensions.cend();

	[[maybe_unused]] VkAllocationCallbacks* allocator = nullptr; // TODO
	VkSurfaceKHR surface{};
	if( hasWin32Surface )
	{
		//VkWin32SurfaceCreateInfoKHR createInfo{
		//	/*VkStructureType                 */ .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		//	/*const void*                     */ .pNext{},
		//	/*VkWin32SurfaceCreateFlagsKHR    */ .flags{},
		//	/*HINSTANCE                       */ .hinstance = GetModuleHandleW( nullptr ),
		//	/*HWND                            */ .hwnd = reinterpret_cast<HWND>( hwnd ),
		//};

		////VKAPI_ATTR VkResult VKAPI_CALL vkCreateWin32SurfaceKHR(
		////	VkInstance                                  instance,
		////	const VkWin32SurfaceCreateInfoKHR*          pCreateInfo,
		////	const VkAllocationCallbacks*                pAllocator,
		////	VkSurfaceKHR*                               pSurface );
		//VkResult surfaceResult = vkCreateWin32SurfaceKHR( instance, &createInfo, allocator, &surface );
		//if( surfaceResult != VK_SUCCESS )
		//{
		//	std::cerr << "Could not create surface. Error was " << surfaceResult << std::endl;
		//}
	}

	//if( surface )
	//{
	//	VkSwapchainCreateInfoKHR 
	//	surface
	//}

	if( surface )
	{
		//surface

		decltype( surface ) releasedSurface{};
		std::swap( surface, releasedSurface );
		//vkDestroySurfaceKHR( instance, releasedSurface, allocator );
	}

	// check see what happens with more than one vulan instance
	[[maybe_unused]]
	auto another_vulkan_instance = vulkan::CreateInstance();
	return 0;
}
