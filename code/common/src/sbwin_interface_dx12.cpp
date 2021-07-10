#include "common/include/sb_interface_dx12.h"

//#if defined(SBWIN64) || defined(SBWIN32)
//	#ifndef NOMINMAX
//	#define NOMINMAX
//	#endif
//#endif
#define COM_NO_WINDOWS_H
#include <dxgi1_6.h>
#include <d3d12.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3D12.lib")


#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include <common/include/sb_encrypted_string.h>
#include <common/include/sb_interface_dx12.h>


namespace SB { namespace LibX {

////
	template<typename T>
struct iid_traits;

#define SB_REGISTER_COM( _type, a,b,c, ... ) \
	template<> struct iid_traits< _type > { static inline constexpr GUID value = { a,b,c, {__VA_ARGS__} }; }; \
	template<> SB_WIN_EXPORT const _GUID& get_uuid<_type>( const _type* ) { return iid_traits< _type >::value; }

#define SB_REGISTER_REF_COM( _type, a,b,c, ... ) \
	template<> struct iid_traits< _type > { static inline const GUID& value = IID_##_type; }; \
	template<> SB_WIN_EXPORT const _GUID& get_uuid<_type>( const _type* ) { return iid_traits< _type >::value; }

SB_REGISTER_REF_COM( IDXGIObject,          0xaec22fb8, 0x76f3, 0x4639, 0x9b, 0xe0, 0x28, 0xeb, 0x43, 0xa6, 0x7a, 0x2e );
SB_REGISTER_REF_COM( IDXGIDeviceSubObject, 0x3d3e0379, 0xf9de, 0x4d58, 0xbb, 0x6c, 0x18, 0xd6, 0x29, 0x92, 0xf1, 0xa6 );
SB_REGISTER_REF_COM( IDXGIResource,        0x035f3ab4, 0x482e, 0x4e50, 0xb4, 0x1f, 0x8a, 0x7f, 0x8b, 0xd8, 0x96, 0x0b );
SB_REGISTER_REF_COM( IDXGIKeyedMutex,      0x9d8e1289, 0xd7b3, 0x465f, 0x81, 0x26, 0x25, 0x0e, 0x34, 0x9a, 0xf8, 0x5d );
SB_REGISTER_REF_COM( IDXGISurface,         0xcafcb56c, 0x6ac3, 0x4889, 0xbf, 0x47, 0x9e, 0x23, 0xbb, 0xd2, 0x60, 0xec );
SB_REGISTER_REF_COM( IDXGISurface1,        0x4AE63092, 0x6327, 0x4c1b, 0x80, 0xAE, 0xBF, 0xE1, 0x2E, 0xA3, 0x2B, 0x86 );
SB_REGISTER_REF_COM( IDXGIAdapter,         0x2411e7e1, 0x12ac, 0x4ccf, 0xbd, 0x14, 0x97, 0x98, 0xe8, 0x53, 0x4d, 0xc0 );
SB_REGISTER_REF_COM( IDXGIOutput,          0xae02eedb, 0xc735, 0x4690, 0x8d, 0x52, 0x5a, 0x8d, 0xc2, 0x02, 0x13, 0xaa );
SB_REGISTER_REF_COM( IDXGISwapChain,       0x310d36a0, 0xd2e7, 0x4c0a, 0xaa, 0x04, 0x6a, 0x9d, 0x23, 0xb8, 0x88, 0x6a );
SB_REGISTER_REF_COM( IDXGIFactory,         0x7b7166ec, 0x21c7, 0x44ae, 0xb2, 0x1a, 0xc9, 0xae, 0x32, 0x1a, 0xe3, 0x69 );
SB_REGISTER_REF_COM( IDXGIDevice,          0x54ec77fa, 0x1377, 0x44e6, 0x8c, 0x32, 0x88, 0xfd, 0x5f, 0x44, 0xc8, 0x4c );
SB_REGISTER_REF_COM( IDXGIFactory1,        0x770aae78, 0xf26f, 0x4dba, 0xa8, 0x29, 0x25, 0x3c, 0x83, 0xd1, 0xb3, 0x87 );
SB_REGISTER_REF_COM( IDXGIAdapter1,        0x29038f61, 0x3839, 0x4626, 0x91, 0xfd, 0x08, 0x68, 0x79, 0x01, 0x1a, 0x05 );
SB_REGISTER_REF_COM( IDXGIDevice1,         0x77db970f, 0x6276, 0x48ba, 0xba, 0x28, 0x07, 0x01, 0x43, 0xb4, 0x39, 0x2c );

// DXGI 1.6 is not yet part of dxguid.lib as of version 19041
SB_REGISTER_COM( IDXGIAdapter4, 0x3c8d99d1, 0x4fbf, 0x4181, 0xa8, 0x2c, 0xaf, 0x66, 0xbf, 0x7b, 0xd2, 0x4e );
SB_REGISTER_COM( IDXGIOutput6,  0x068346e8, 0xaaec, 0x4b84, 0xad, 0xd7, 0x13, 0x7f, 0x51, 0x3f, 0x77, 0xa1 );
SB_REGISTER_COM( IDXGIFactory6, 0xc1b6694f, 0xff09, 0x44a9, 0xb0, 0x3c, 0x77, 0x90, 0x0a, 0x0a, 0x1d, 0x17 );
SB_REGISTER_COM( IDXGIFactory7, 0xa4966eed, 0x76db, 0x44da, 0x84, 0xc1, 0xee, 0x9a, 0x7a, 0xfb, 0x20, 0xa8 );

SB_REGISTER_REF_COM( ID3D12Object,                             0xc4fec28f, 0x7966, 0x4e95, 0x9f, 0x94, 0xf4, 0x31, 0xcb, 0x56, 0xc3, 0xb8 );
SB_REGISTER_REF_COM( ID3D12DeviceChild,                        0x905db94b, 0xa00c, 0x4140, 0x9d, 0xf5, 0x2b, 0x64, 0xca, 0x9e, 0xa3, 0x57 );
SB_REGISTER_REF_COM( ID3D12RootSignature,                      0xc54a6b66, 0x72df, 0x4ee8, 0x8b, 0xe5, 0xa9, 0x46, 0xa1, 0x42, 0x92, 0x14 );
SB_REGISTER_REF_COM( ID3D12RootSignatureDeserializer,          0x34AB647B, 0x3CC8, 0x46AC, 0x84, 0x1B, 0xC0, 0x96, 0x56, 0x45, 0xC0, 0x46 );
SB_REGISTER_REF_COM( ID3D12VersionedRootSignatureDeserializer, 0x7F91CE67, 0x090C, 0x4BB7, 0xB7, 0x8E, 0xED, 0x8F, 0xF2, 0xE3, 0x1D, 0xA0 );
SB_REGISTER_REF_COM( ID3D12Pageable,                           0x63ee58fb, 0x1268, 0x4835, 0x86, 0xda, 0xf0, 0x08, 0xce, 0x62, 0xf0, 0xd6 );
SB_REGISTER_REF_COM( ID3D12Heap,                               0x6b3b2502, 0x6e51, 0x45b3, 0x90, 0xee, 0x98, 0x84, 0x26, 0x5e, 0x8d, 0xf3 );
SB_REGISTER_REF_COM( ID3D12Resource,                           0x696442be, 0xa72e, 0x4059, 0xbc, 0x79, 0x5b, 0x5c, 0x98, 0x04, 0x0f, 0xad );
SB_REGISTER_REF_COM( ID3D12CommandAllocator,                   0x6102dee4, 0xaf59, 0x4b09, 0xb9, 0x99, 0xb4, 0x4d, 0x73, 0xf0, 0x9b, 0x24 );
SB_REGISTER_REF_COM( ID3D12Fence,                              0x0a753dcf, 0xc4d8, 0x4b91, 0xad, 0xf6, 0xbe, 0x5a, 0x60, 0xd9, 0x5a, 0x76 );
SB_REGISTER_REF_COM( ID3D12Fence1,                             0x433685fe, 0xe22b, 0x4ca0, 0xa8, 0xdb, 0xb5, 0xb4, 0xf4, 0xdd, 0x0e, 0x4a );
SB_REGISTER_REF_COM( ID3D12PipelineState,                      0x765a30f3, 0xf624, 0x4c6f, 0xa8, 0x28, 0xac, 0xe9, 0x48, 0x62, 0x24, 0x45 );
SB_REGISTER_REF_COM( ID3D12DescriptorHeap,                     0x8efb471d, 0x616c, 0x4f49, 0x90, 0xf7, 0x12, 0x7b, 0xb7, 0x63, 0xfa, 0x51 );
SB_REGISTER_REF_COM( ID3D12QueryHeap,                          0x0d9658ae, 0xed45, 0x469e, 0xa6, 0x1d, 0x97, 0x0e, 0xc5, 0x83, 0xca, 0xb4 );
SB_REGISTER_REF_COM( ID3D12CommandSignature,                   0xc36a797c, 0xec80, 0x4f0a, 0x89, 0x85, 0xa7, 0xb2, 0x47, 0x50, 0x82, 0xd1 );
SB_REGISTER_REF_COM( ID3D12CommandList,                        0x7116d91c, 0xe7e4, 0x47ce, 0xb8, 0xc6, 0xec, 0x81, 0x68, 0xf4, 0x37, 0xe5 );
SB_REGISTER_REF_COM( ID3D12GraphicsCommandList,                0x5b160d0f, 0xac1b, 0x4185, 0x8b, 0xa8, 0xb3, 0xae, 0x42, 0xa5, 0xa4, 0x55 );
SB_REGISTER_REF_COM( ID3D12GraphicsCommandList1,               0x553103fb, 0x1fe7, 0x4557, 0xbb, 0x38, 0x94, 0x6d, 0x7d, 0x0e, 0x7c, 0xa7 );
SB_REGISTER_REF_COM( ID3D12GraphicsCommandList2,               0x38C3E585, 0xFF17, 0x412C, 0x91, 0x50, 0x4F, 0xC6, 0xF9, 0xD7, 0x2A, 0x28 );
SB_REGISTER_REF_COM( ID3D12CommandQueue,                       0x0ec870a6, 0x5d7e, 0x4c22, 0x8c, 0xfc, 0x5b, 0xaa, 0xe0, 0x76, 0x16, 0xed );
SB_REGISTER_REF_COM( ID3D12Device,                             0x189819f1, 0x1db6, 0x4b57, 0xbe, 0x54, 0x18, 0x21, 0x33, 0x9b, 0x85, 0xf7 );
SB_REGISTER_REF_COM( ID3D12PipelineLibrary,                    0xc64226a8, 0x9201, 0x46af, 0xb4, 0xcc, 0x53, 0xfb, 0x9f, 0xf7, 0x41, 0x4f );
SB_REGISTER_REF_COM( ID3D12PipelineLibrary1,                   0x80eabf42, 0x2568, 0x4e5e, 0xbd, 0x82, 0xc3, 0x7f, 0x86, 0x96, 0x1d, 0xc3 );
SB_REGISTER_REF_COM( ID3D12Device1,                            0x77acce80, 0x638e, 0x4e65, 0x88, 0x95, 0xc1, 0xf2, 0x33, 0x86, 0x86, 0x3e );
SB_REGISTER_REF_COM( ID3D12Device2,                            0x30baa41e, 0xb15b, 0x475c, 0xa0, 0xbb, 0x1a, 0xf5, 0xc5, 0xb6, 0x43, 0x28 );
SB_REGISTER_REF_COM( ID3D12Device3,                            0x81dadc15, 0x2bad, 0x4392, 0x93, 0xc5, 0x10, 0x13, 0x45, 0xc4, 0xaa, 0x98 );
SB_REGISTER_REF_COM( ID3D12ProtectedSession,                   0xA1533D18, 0x0AC1, 0x4084, 0x85, 0xB9, 0x89, 0xA9, 0x61, 0x16, 0x80, 0x6B );
SB_REGISTER_REF_COM( ID3D12ProtectedResourceSession,           0x6CD696F4, 0xF289, 0x40CC, 0x80, 0x91, 0x5A, 0x6C, 0x0A, 0x09, 0x9C, 0x3D );
SB_REGISTER_REF_COM( ID3D12Device4,                            0xe865df17, 0xa9ee, 0x46f9, 0xa4, 0x63, 0x30, 0x98, 0x31, 0x5a, 0xa2, 0xe5 );
SB_REGISTER_REF_COM( ID3D12LifetimeOwner,                      0xe667af9f, 0xcd56, 0x4f46, 0x83, 0xce, 0x03, 0x2e, 0x59, 0x5d, 0x70, 0xa8 );
SB_REGISTER_REF_COM( ID3D12SwapChainAssistant,                 0xf1df64b6, 0x57fd, 0x49cd, 0x88, 0x07, 0xc0, 0xeb, 0x88, 0xb4, 0x5c, 0x8f );
SB_REGISTER_REF_COM( ID3D12LifetimeTracker,                    0x3fd03d36, 0x4eb1, 0x424a, 0xa5, 0x82, 0x49, 0x4e, 0xcb, 0x8b, 0xa8, 0x13 );
SB_REGISTER_REF_COM( ID3D12StateObject,                        0x47016943, 0xfca8, 0x4594, 0x93, 0xea, 0xaf, 0x25, 0x8b, 0x55, 0x34, 0x6d );
SB_REGISTER_REF_COM( ID3D12StateObjectProperties,              0xde5fa827, 0x9bf9, 0x4f26, 0x89, 0xff, 0xd7, 0xf5, 0x6f, 0xde, 0x38, 0x60 );
SB_REGISTER_REF_COM( ID3D12Device5,                            0x8b4f173b, 0x2fea, 0x4b80, 0x8f, 0x58, 0x43, 0x07, 0x19, 0x1a, 0xb9, 0x5d );
SB_REGISTER_REF_COM( ID3D12DeviceRemovedExtendedDataSettings,  0x82BC481C, 0x6B9B, 0x4030, 0xAE, 0xDB, 0x7E, 0xE3, 0xD1, 0xDF, 0x1E, 0x63 );
SB_REGISTER_REF_COM( ID3D12DeviceRemovedExtendedData,          0x98931D33, 0x5AE8, 0x4791, 0xAA, 0x3C, 0x1A, 0x73, 0xA2, 0x93, 0x4E, 0x71 );
SB_REGISTER_REF_COM( ID3D12Device6,                            0xc70b221b, 0x40e4, 0x4a17, 0x89, 0xaf, 0x02, 0x5a, 0x07, 0x27, 0xa6, 0xdc );
SB_REGISTER_REF_COM( ID3D12Resource1,                          0x9D5E227A, 0x4430, 0x4161, 0x88, 0xB3, 0x3E, 0xCA, 0x6B, 0xB1, 0x6E, 0x19 );
SB_REGISTER_REF_COM( ID3D12Heap1,                              0x572F7389, 0x2168, 0x49E3, 0x96, 0x93, 0xD6, 0xDF, 0x58, 0x71, 0xBF, 0x6D );
SB_REGISTER_REF_COM( ID3D12GraphicsCommandList3,               0x6FDA83A7, 0xB84C, 0x4E38, 0x9A, 0xC8, 0xC7, 0xBD, 0x22, 0x01, 0x6B, 0x3D );
SB_REGISTER_REF_COM( ID3D12MetaCommand,                        0xDBB84C27, 0x36CE, 0x4FC9, 0xB8, 0x01, 0xF0, 0x48, 0xC4, 0x6A, 0xC5, 0x70 );
SB_REGISTER_REF_COM( ID3D12GraphicsCommandList4,               0x8754318e, 0xd3a9, 0x4541, 0x98, 0xcf, 0x64, 0x5b, 0x50, 0xdc, 0x48, 0x74 );
SB_REGISTER_REF_COM( ID3D12Tools,                              0x7071e1f0, 0xe84b, 0x4b33, 0x97, 0x4f, 0x12, 0xfa, 0x49, 0xde, 0x65, 0xc5 );
SB_REGISTER_REF_COM( ID3D12GraphicsCommandList5,               0x55050859, 0x4024, 0x474c, 0x87, 0xf5, 0x64, 0x72, 0xea, 0xee, 0x44, 0xea );

#ifdef SB_IID_ARGS
#undef SB_IID_ARGS
#endif

template<typename X> const auto& uuid_of = iid_traits< std::decay_t<X> >::value;
#define SB_IID_ARGS( X ) uuid_of< std::decay_t<decltype( **X.ReleaseAndGetAddressOf() )> >, reinterpret_cast<void**>(X.ReleaseAndGetAddressOf<void>())


namespace dx12
{

////
SB_WIN_EXPORT InstanceHandle CreateInstance([[maybe_unused]] const Configuration* config)
{
	InstanceHandle factory;
	[[maybe_unused]] HRESULT hr = CreateDXGIFactory2( 0 /*DXGI_CREATE_FACTORY_DEBUG*/, SB_IID_ARGS(factory) );
	return factory;
}
SB_WIN_EXPORT bool DestroyInstance([[maybe_unused]] InstanceHandle instance, [[maybe_unused]] const Configuration* config)
{
	instance = nullptr;
	return true;
}

//using adapter_array_t = std::vector<AdapterHandle>;
SB_WIN_EXPORT adapter_array_t EnumerateAdapters([[maybe_unused]] InstanceHandle instance, [[maybe_unused]] size_t maxCount)
{
	ref_ptr<IDXGIFactory7> factory7;
	instance->QueryInterface( SB_IID_ARGS(factory7) );
	AdapterHandle adapter;
	adapter_array_t adapters;
	for ( uint32_t adapter_index = 0; SUCCEEDED( factory7->EnumAdapterByGpuPreference( adapter_index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, SB_IID_ARGS(adapter) ) ); ++adapter_index)
		adapters.emplace_back( adapter );

	//AdapterHandle adapter;
	//adapter_array_t adapters;
	//for ( uint32_t adapter_index = 0; SUCCEEDED( instance->EnumAdapters( adapter_index, adapter.GetAddressOf() ) ); ++adapter_index)
	//	adapters.emplace_back( adapter );
	return adapters;
}
SB_WIN_EXPORT DeviceDesc GetDeviceDesc([[maybe_unused]] AdapterHandle adapter)
{
	//DXGI_ADAPTER_DESC3
	DXGI_ADAPTER_DESC adapter_properties{};
	adapter->GetDesc( &adapter_properties);
	DeviceDesc device_desc = {
		SB_STRUCT_SET( .name   = ) "",
		SB_STRUCT_SET( .vendorID      = ) adapter_properties.VendorId,
		SB_STRUCT_SET( .deviceID      = ) adapter_properties.DeviceId,
		SB_STRUCT_SET( .apiID         = ) adapter_properties.SubSysId,
		SB_STRUCT_SET( .driverVersion = ) adapter_properties.Revision,
		SB_STRUCT_SET( .uid           = ) "",
	};
	static_assert(sizeof(device_desc.uid) >= sizeof(adapter_properties.AdapterLuid));
	memcpy(device_desc.uid, &adapter_properties.AdapterLuid, sizeof(adapter_properties.AdapterLuid));

	int utf16len = static_cast<int>(wcslen(adapter_properties.Description) & 0x7FFFFFFF);
	int utf8len = 0;
	do
	{
		utf8len = WideCharToMultiByte(CP_UTF8, 0, adapter_properties.Description, utf16len, nullptr, 0, nullptr, nullptr);
		if( utf8len >= int{sizeof(device_desc.name)} )
			--utf16len;
	} while( utf16len > 0 && utf8len >= int{sizeof(device_desc.name)} );
	if(utf8len > 0 && utf8len < int{sizeof(device_desc.name)} )
		WideCharToMultiByte(CP_UTF8, 0, adapter_properties.Description, utf16len, device_desc.name, sizeof(device_desc.name), nullptr, nullptr);

	return device_desc;
}

SB_WIN_EXPORT DeviceHandle CreateDevice([[maybe_unused]] AdapterHandle adapter, [[maybe_unused]] const Configuration* config)
{
	DeviceHandle device;
	D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1/*get(config, "dx12.feature_level")*/, SB_IID_ARGS(device));
	return device;
}
SB_WIN_EXPORT bool DestroyDevice([[maybe_unused]] DeviceHandle device, [[maybe_unused]] const Configuration* config)
{
	device = nullptr;
	return false;
}


//struct instance_layer
//{
//	using properties_t = VkLayerProperties;
//};
//
//	template<>
//SB_WIN_EXPORT std::vector<instance_layer::properties_t> enumerate<instance_layer::properties_t>(const uint32_t max_count)
//{
//	uint32_t count = 0;
//	{
//		[[maybe_unused]] VkResult err = vkEnumerateInstanceLayerProperties(&count, nullptr);
//		assert(!err);
//	}
//	count = std::min(count, max_count);
//
//	std::vector<instance_layer::properties_t> result(count);
//	if (count > 0)
//	{
//		[[maybe_unused]] VkResult err = vkEnumerateInstanceLayerProperties(&count, result.data());
//		assert(!err);
//	}
//	return result;
//}
//
//////
//struct instance_extension
//{
//	using properties_t = typename instance_extension_traits::properties_t;
//	using extension_id_t = typename instance_extension_traits::extension_id_t;
//
//	//// platform-specific
//	#if defined(VK_USE_PLATFORM_WIN32_KHR)
//	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
//	#elif defined(VK_USE_PLATFORM_XLIB_KHR)
//	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_XLIB_SURFACE_EXTENSION_NAME
//	#elif defined(VK_USE_PLATFORM_XCB_KHR)
//	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_XCB_SURFACE_EXTENSION_NAME
//	#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
//	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_XCB_SURFACE_EXTENSION_NAME
//	#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
//	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_DISPLAY_EXTENSION_NAME
//	#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
//	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
//	#elif defined(VK_USE_PLATFORM_IOS_MVK)
//	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_MVK_IOS_SURFACE_EXTENSION_NAME
//	#elif defined(VK_USE_PLATFORM_MACOS_MVK)
//	#define SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME VK_MVK_MACOS_SURFACE_EXTENSION_NAME
//	#else
//	#error "vulkan support missing"
//	#endif
//
//	//inline static const char name[extension_id_t::kMaxExtension][VK_MAX_EXTENSION_NAME_SIZE]
//	//= {
//	//	"Generic",
//	//	VK_KHR_SURFACE_EXTENSION_NAME, // kSurface
//	//	SB_VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME, // kPlatformSurface
//	//	//VK_KHR_SWAPCHAIN_EXTENSION_NAME, // kSwapchain
//	//	//VK_EXT_DEBUG_UTILS_EXTENSION_NAME, // kDebugUtils
//	//	};
//};
//
//	template<>
//SB_WIN_EXPORT std::vector<instance_extension::properties_t> enumerate<instance_extension::properties_t>(const uint32_t max_count, const char* const layer_name)
//{
//	uint32_t count = 0;
//	{
//		[[maybe_unused]] VkResult err = vkEnumerateInstanceExtensionProperties(layer_name, &count, nullptr);
//		assert(layer_name || !err);
//	}
//	count = std::min(count, max_count);
//
//	std::vector<instance_extension::properties_t> result(count);
//	if (count > 0)
//	{
//		[[maybe_unused]] VkResult err = vkEnumerateInstanceExtensionProperties(layer_name, &count, result.data());
//		assert(layer_name || !err);
//	}
//	return result;
//}
//
//struct physical_device
//{
//	//using instance_t = VkInstance;
//	using device_t = VkPhysicalDevice;
//};
//
//	template<>
//SB_WIN_EXPORT std::vector<physical_device::device_t> enumerate<physical_device::device_t>(const uint32_t max_count, InstanceHandle instance)
//{
//	uint32_t count = 0;
//	{
//		[[maybe_unused]] VkResult err = vkEnumeratePhysicalDevices(instance, &count, nullptr);
//		assert(!err);
//	}
//	count = std::min(count, max_count);
//
//	std::vector<physical_device::device_t> result(count);
//	if (count > 0)
//	{
//		[[maybe_unused]] VkResult err = vkEnumeratePhysicalDevices(instance, &count, result.data());
//		assert(!err);
//	}
//	return result;
//}
//
//	template<>
//SB_WIN_EXPORT std::vector<physical_device::device_t> enumerate<physical_device::device_t>(InstanceHandle instance)
//{
//	return enumerate<physical_device::device_t>(~0u, instance);
//}
//
//
//	template<>
//SB_WIN_EXPORT std::vector<device_extension_traits::properties_t> enumerate<device_extension_traits::properties_t>(const uint32_t max_count, AdapterHandle adapter, const char* const layer_name)
//{
//	uint32_t count = 0;
//	{
//		[[maybe_unused]] VkResult err = vkEnumerateDeviceExtensionProperties(adapter, layer_name, &count, nullptr);
//		assert(!err);
//	}
//	count = std::min(count, max_count);
//
//	std::vector<device_extension_traits::properties_t> result(count);
//	if (count > 0)
//	{
//		[[maybe_unused]] VkResult err = vkEnumerateDeviceExtensionProperties(adapter, nullptr, &count, result.data());
//		assert(!err);
//	}
//	return result;
//}
//
//	template<>
//SB_WIN_EXPORT std::vector<device_extension_traits::properties_t> enumerate<device_extension_traits::properties_t>(AdapterHandle adapter, const char* const layer_name)
//{
//	return enumerate<device_extension_traits::properties_t>(~0u, adapter, layer_name);
//}
//
//	template<>
//SB_WIN_EXPORT std::vector<device_extension_traits::properties_t> enumerate<device_extension_traits::properties_t>(AdapterHandle adapter)
//{
//	return enumerate<device_extension_traits::properties_t>(adapter, static_cast<const char* const>(nullptr));
//}
//
//struct queue_families
//{
//	using properties_t = VkQueueFamilyProperties;
//};
//
//	template<>
//SB_WIN_EXPORT std::vector<queue_families::properties_t> enumerate<queue_families::properties_t>(const uint32_t max_count, AdapterHandle adapter)
//{
//	uint32_t count = 0;
//	{
//		vkGetPhysicalDeviceQueueFamilyProperties(adapter, &count, nullptr);
//	}
//	count = std::min(count, max_count);
//
//	std::vector<queue_families::properties_t> result(count);
//	if (count > 0)
//	{
//		vkGetPhysicalDeviceQueueFamilyProperties(adapter, &count, result.data());
//	}
//	return result;
//}
//
//	template<>
//SB_WIN_EXPORT std::vector<queue_families::properties_t> enumerate<queue_families::properties_t>(AdapterHandle adapter)
//{
//	return enumerate<queue_families::properties_t>(~0u, adapter);
//}
//
//
//VkBool32 VKAPI_PTR vulkanDebugCallback(
//	[[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
//	[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
//	[[maybe_unused]] const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
//	[[maybe_unused]] void*                                            pUserData )
//{
//	std::cerr << "Vuklan message level " << messageSeverity << " type " << messageTypes << std::endl;
//	if( pCallbackData )
//		std::cerr << pCallbackData->pMessageIdName << ": " << pCallbackData->pMessage << std::endl;
//
//	return VK_FALSE;
//}
//
//SB_WIN_EXPORT InstanceHandle CreateInstance( [[maybe_unused]] const Configuration* config )
//{
//	if( config && config->name != "sbVulkanConfig"_xhash64 )
//		return InstanceHandle{};
//
//	InstanceConfiguration defaultConfig{};
//	auto instanceConfig = config ? static_cast<const InstanceConfiguration*>( config ) : &defaultConfig;
//	auto requestedExtension = [instanceConfig]( xhash_string_view_t feature ) -> bool {
//		auto found = std::find_if( instanceConfig->requested_extensions.cbegin(), instanceConfig->requested_extensions.cend(), [feature]( xhash_string_view_t extension ) -> bool {
//			return feature == extension;
//		} ) != instanceConfig->requested_extensions.cend();
//		return found;
//	};
//	const bool requires1_1 = requestedExtension( "VK_KHR_surface_protected_capabilities" );
//
//	
//	const VkApplicationInfo app = {
//		SB_STRUCT_SET( .sType              = ) VK_STRUCTURE_TYPE_APPLICATION_INFO,
//		SB_STRUCT_SET( .pNext              = ) nullptr,
//		SB_STRUCT_SET( .pApplicationName   = ) instanceConfig->applicationName.get_value().data(),
//		SB_STRUCT_SET( .applicationVersion = ) instanceConfig->applicationVersion,
//		SB_STRUCT_SET( .pEngineName        = ) instanceConfig->engineName.get_value().data(),
//		SB_STRUCT_SET( .engineVersion      = ) instanceConfig->engineVersion,
//		SB_STRUCT_SET( .apiVersion         = ) requires1_1 ? VK_API_VERSION_1_1 : VK_API_VERSION_1_0,
//	};
//
//	std::vector<const char*> requested_layers; requested_layers.reserve( instanceConfig->requested_layers.size() );
//	for( const auto& hash : instanceConfig->requested_layers )
//		requested_layers.emplace_back( hash.get_value().data() );
//	std::vector<const char*> requested_extensions; requested_extensions.reserve( instanceConfig->requested_extensions.size() );
//	for( const auto& hash : instanceConfig->requested_extensions )
//		requested_extensions.emplace_back( hash.get_value().data() );
//	VkInstanceCreateInfo inst_info = {
//		SB_STRUCT_SET( .sType                   = ) VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
//		SB_STRUCT_SET( .pNext                   = ) nullptr,
//		SB_STRUCT_SET( .flags                   = ) 0, // VkInstanceCreateFlags is a bitmask type for setting a mask, but is currently reserved for future use.
//		SB_STRUCT_SET( .pApplicationInfo        = ) &app,
//		SB_STRUCT_SET( .enabledLayerCount       = ) static_cast<uint32_t>( requested_layers.size() & 0xFFFFFFFF ),
//		SB_STRUCT_SET( .ppEnabledLayerNames     = ) requested_layers.data(),
//		SB_STRUCT_SET( .enabledExtensionCount   = ) static_cast<uint32_t>( requested_extensions.size() & 0xFFFFFFFF ),
//		SB_STRUCT_SET( .ppEnabledExtensionNames = ) requested_extensions.data(),
//	};
//
//#if 1 // SB_DEVICE_DEBUG_LAYER
//	VkDebugUtilsMessengerCreateInfoEXT dbg_messenger_create_info = {
//		SB_STRUCT_SET( .sType           = ) VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
//		SB_STRUCT_SET( .pNext           = ) nullptr,
//		SB_STRUCT_SET( .flags           = ) 0,
//		SB_STRUCT_SET( .messageSeverity = ) VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
//		SB_STRUCT_SET( .messageType     = ) VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
//		SB_STRUCT_SET( .pfnUserCallback = ) &vulkanDebugCallback,
//		SB_STRUCT_SET( .pUserData       = ) nullptr,
//	};
//
//	//if ( sbUseDeviceDebugLayer )
//		inst_info.pNext = &dbg_messenger_create_info;
//#endif
//
//	InstanceHandle instance;
//	VkResult err = vkCreateInstance(&inst_info, nullptr, &instance);
//	if (err != VK_SUCCESS)
//	{
//		switch (err)
//		{
//		case VK_ERROR_INCOMPATIBLE_DRIVER: std::cerr << "incompatible driver\n"; break;
//		case VK_ERROR_EXTENSION_NOT_PRESENT: std::cerr << "extension not present\n"; break;
//		case VK_ERROR_LAYER_NOT_PRESENT: std::cerr << "layer not present\n"; break;
//		default: __debugbreak(); break;
//		}
//	}
//	return err == VK_SUCCESS ? instance : nullptr;
//}
//
//SB_WIN_EXPORT bool DestroyInstance(InstanceHandle instance, [[maybe_unused]] const Configuration* config)
//{
//	if (instance)
//	{
//		vkDestroyInstance(instance, nullptr);
//	}
//	return true;
//}
//
//SB_WIN_EXPORT adapter_array_t EnumerateAdapters([[maybe_unused]] InstanceHandle instance, [[maybe_unused]] size_t maxCount)
//{
//	return enumerate<physical_device::device_t>(instance);
//}
//
//SB_WIN_EXPORT DeviceDesc GetDeviceDesc( AdapterHandle adapter )
//{
//	VkPhysicalDeviceProperties adapter_properties;
//	vkGetPhysicalDeviceProperties(adapter, &adapter_properties);
//	DeviceDesc device_desc = {
//		SB_STRUCT_SET( .name   = ) {},
//		SB_STRUCT_SET( .vendorID      = ) adapter_properties.vendorID,
//		SB_STRUCT_SET( .deviceID      = ) adapter_properties.deviceID,
//		SB_STRUCT_SET( .apiID         = ) adapter_properties.apiVersion,
//		SB_STRUCT_SET( .driverVersion = ) adapter_properties.driverVersion,
//		SB_STRUCT_SET( .uid           = ) {}
//	};
//	static_assert(sizeof(device_desc.uid) >= sizeof(adapter_properties.pipelineCacheUUID));
//	memcpy(device_desc.uid, adapter_properties.pipelineCacheUUID, sizeof(adapter_properties.pipelineCacheUUID));
//	static_assert( sizeof(device_desc.name) >= sizeof(adapter_properties.deviceName) );
//	memcpy(device_desc.name, adapter_properties.deviceName, sizeof(adapter_properties.deviceName) );
//
//	return device_desc;
//}
//
//SB_WIN_EXPORT DeviceHandle CreateDevice([[maybe_unused]] AdapterHandle adapter, [[maybe_unused]] const Configuration* config)
//{
//	// TODO : store this in config? remove const?
//	// --> SB::Configuration should be const but got a few mutable members for attaching & detaching owned data (through data handle)
//	// (mostly JSON-like)
//	std::clog << "vulkan queue families : " << std::endl;
//	auto vulkan_queue_families = vulkan::enumerate<queue_families::properties_t>(adapter);
//	for (size_t index = 0; index < vulkan_queue_families.size(); ++index)
//	{
//		const auto& queue_family = vulkan_queue_families[index];
//		std::clog << "\tQueue Family "<< index << " (count: " << queue_family.queueCount << ")\n";
//		std::clog << "\t\tMin size = (" << queue_family.minImageTransferGranularity.width
//			<< ", " << queue_family.minImageTransferGranularity.height
//			<< ", " << queue_family.minImageTransferGranularity.depth << ")\n";
//		std::clog << "\tSupports:\n";
//		if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
//			std::clog << "\t\tGraphics\n";
//		if (queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT)
//			std::clog << "\t\tCompute\n";
//		if (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT)
//			std::clog << "\t\tTransfer\n";
//		if (queue_family.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
//			std::clog << "\t\tSparse binding\n";
//		if (queue_family.queueFlags & VK_QUEUE_PROTECTED_BIT)
//			std::clog << "\t\tProtected\n";
//		if (queue_family.queueFlags & ~unsigned(VK_QUEUE_GRAPHICS_BIT|VK_QUEUE_COMPUTE_BIT|VK_QUEUE_TRANSFER_BIT|VK_QUEUE_SPARSE_BINDING_BIT|VK_QUEUE_GRAPHICS_BIT))
//			std::clog << "\t\t(unknown)\n";
//	}
//	std::clog.flush();
//
//	// Usually, we'd use config information. TODO
//	// In the default case, we'd like :
//	// -	1 or 2 graphics queues; *could use a distinct queue for processing data like baking lighting & shadows or generating textures or vertex data in general
//	// -	a few async compute queues; *generic compute workers
//	// -	usually one transfer queue; *data streaming
//	// lets see what happens when we ask for all queues...
//
//	using queue_create_info = std::vector<VkDeviceQueueCreateInfo>;
//	queue_create_info queues_info(vulkan_queue_families.size());
//	using float_array_t = std::vector<float>;
//	std::vector<float_array_t> queue_priorities(vulkan_queue_families.size());
//	for ( size_t index = 0; index < vulkan_queue_families.size(); ++index )
//	{
//		const auto& queues_properties = vulkan_queue_families[index];
//
//		// lets keep all of them at lowest (0.0f) priority for now... TODO
//		float_array_t& family_queues_priorities = queue_priorities[index];
//		family_queues_priorities.resize(queues_properties.queueCount);
//
//		VkDeviceQueueCreateFlags queueFlags{};
//		if (queues_properties.queueFlags & VK_QUEUE_PROTECTED_BIT /*&& useProtectedQueue*/)
//			queueFlags = VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT;
//		queues_info[index] = VkDeviceQueueCreateInfo{
//			SB_STRUCT_SET( .sType            = ) VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
//			SB_STRUCT_SET( .pNext            = ) nullptr,
//			SB_STRUCT_SET( .flags            = ) queueFlags,
//			SB_STRUCT_SET( .queueFamilyIndex = ) static_cast<uint32_t>(index),
//			SB_STRUCT_SET( .queueCount       = ) queues_properties.queueCount,
//			SB_STRUCT_SET( .pQueuePriorities = ) family_queues_priorities.data(),
//		};
//	}
//
//	VkDeviceCreateInfo device_info = {
//		SB_STRUCT_SET( .sType                   = ) VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
//		SB_STRUCT_SET( .pNext                   = ) nullptr,
//		SB_STRUCT_SET( .flags                   = ) 0, // reserved for future use
//		SB_STRUCT_SET( .queueCreateInfoCount    = ) 1,
//		SB_STRUCT_SET( .pQueueCreateInfos       = ) queues_info.data(),
//		SB_STRUCT_SET( .enabledLayerCount       = ) 0,
//		SB_STRUCT_SET( .ppEnabledLayerNames     = ) nullptr,
//		SB_STRUCT_SET( .enabledExtensionCount   = ) 0,//TODO
//		SB_STRUCT_SET( .ppEnabledExtensionNames = ) nullptr,//TODO
//		SB_STRUCT_SET( .pEnabledFeatures        = ) nullptr,  // If specific features are desired, pass them in here
//	};
//
//	DeviceHandle device{};
//	[[maybe_unused]] VkResult err = vkCreateDevice( adapter, &device_info, nullptr, &device );
//	assert(!err);
//	return err == VK_SUCCESS ? device : DeviceHandle{};
//}
//SB_WIN_EXPORT bool DestroyDevice(DeviceHandle device, [[maybe_unused]] const Configuration* config)
//{
//	vkDestroyDevice(device, nullptr);
//	return true;
//}

}}} // namespace SB::LibX::dx12
