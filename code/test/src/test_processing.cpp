#include <test/include/test_vulkan.h>
#include <common/include/internal/ref_ptr.h>

#include <vector>
namespace SB { namespace LibX {
struct Configuration;
}}

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

namespace SB { namespace LibX {
namespace dx12
{
using InstanceHandle = ref_ptr<IDXGIFactory>; // IDXGIFactory4
using AdapterHandle = ref_ptr<IDXGIAdapter>; // IDXGIAdapter4
using DeviceHandle = ref_ptr<ID3D12Device>; // ID3D12Device6

#ifndef SB_LIB_EXPORT
#define SB_LIB_EXPORT 
#endif

#define SBLIB_DECLARE_DEVICE_INTERNAL
#include <common/include/internal/device_generic.h>
#undef SBLIB_DECLARE_DEVICE_INTERNAL
}
using dx12_instance = dx12::unique_instance;
using dx12_device = dx12::unique_device;


	template<typename T>
struct iid_traits;

#define SB_REGISTER_COM( _type, a,b,c, ... ) \
	template<> struct iid_traits< _type > { static inline constexpr GUID value = { a,b,c, {__VA_ARGS__} }; };
#define SB_REGISTER_REF_COM( _type, a,b,c, ... ) \
	template<> struct iid_traits< _type > { static inline const GUID& value = IID_##_type; };

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

template<typename X> const auto& uuid_of = iid_traits< std::decay_t<X> >::value;
#define SB_IID_ARGS( X ) uuid_of< std::decay_t<decltype( **X.ReleaseAndGetAddressOf() )> >, reinterpret_cast<void**>(X.ReleaseAndGetAddressOf<void>())

}}


namespace SB {
namespace LibX {
namespace dx12
{
InstanceHandle CreateInstance([[maybe_unused]] const Configuration* config)
{
	InstanceHandle factory;
	[[maybe_unused]] HRESULT hr = CreateDXGIFactory2( 0 /*DXGI_CREATE_FACTORY_DEBUG*/, SB_IID_ARGS(factory) );
	return factory;
}
bool DestroyInstance([[maybe_unused]] InstanceHandle instance, [[maybe_unused]] const Configuration* config)
{
	instance = nullptr;
	return true;
}

using adapter_array_t = std::vector<AdapterHandle>;
adapter_array_t EnumerateAdapters([[maybe_unused]] InstanceHandle instance, [[maybe_unused]] size_t maxCount)
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
DeviceInfo GetDeviceInfo([[maybe_unused]] AdapterHandle adapter)
{
	//DXGI_ADAPTER_DESC3
	DXGI_ADAPTER_DESC adapter_properties{};
	adapter->GetDesc( &adapter_properties);
	DeviceInfo device_info = {
		.description = "",
		.vendorID = adapter_properties.VendorId,
		.deviceID = adapter_properties.DeviceId,
		.apiID = adapter_properties.SubSysId,
		.driverVersion = adapter_properties.Revision,
		.uid = "",
	};
	static_assert(sizeof(device_info.uid) >= sizeof(adapter_properties.AdapterLuid));
	memcpy(device_info.uid, &adapter_properties.AdapterLuid, sizeof(adapter_properties.AdapterLuid));

	int utf16len = static_cast<int>(wcslen(adapter_properties.Description) & 0x7FFFFFFF);
	int utf8len = 0;
	do
	{
		utf8len = WideCharToMultiByte(CP_UTF8, 0, adapter_properties.Description, utf16len, nullptr, 0, nullptr, nullptr);
		if( utf8len >= int{sizeof(device_info.description)} )
			--utf16len;
	} while( utf16len > 0 && utf8len >= int{sizeof(device_info.description)} );
	if(utf8len > 0 && utf8len < int{sizeof(device_info.description)} )
		WideCharToMultiByte(CP_UTF8, 0, adapter_properties.Description, utf16len, device_info.description, sizeof(device_info.description), nullptr, nullptr);

	return device_info;
}

DeviceHandle CreateDevice([[maybe_unused]] AdapterHandle adapter, [[maybe_unused]] const Configuration* config)
{
	DeviceHandle device;
	D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1/*get(config, "dx12.feature_level")*/, SB_IID_ARGS(device));
	return device;
}
bool DestroyDevice([[maybe_unused]] DeviceHandle device, [[maybe_unused]] const Configuration* config)
{
	device = nullptr;
	return false;
}
}}}


#include <algorithm>
#include <iostream>

SB_EXPORT_TYPE int SB_STDCALL test_processing([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	// Trying to create both vulkan & DirectX devices on every adapters available
	using namespace SB::LibX;

	auto vendor_order = [](uint32_t vendor) -> uint32_t
	{
		enum vendor_t
		{
			NVIDIA    = 0x10DE,
			AMD       = 0x1002,
			Intel     = 0x8086,
			Microsoft = 0x1414,
		};
		switch( static_cast<vendor_t>(vendor) )
		{
		case vendor_t::NVIDIA:    return 0x00000100;
		case vendor_t::AMD:       return 0x00000200;
		case vendor_t::Intel:     return 0x00000400;
		// Microsoft is usually a software (possibly hardware accelerated) device, so considered as debug-only
		case vendor_t::Microsoft: return 0x80000000;
		}
		return vendor;
	};
	constexpr uint32_t kDebugDeviceStart = 0x00010000;

	vulkan_instance test_vulkan{};
	std::vector<std::tuple<vulkan_device, vulkan::DeviceInfo>> vulkan_devices{};
	int vulkanActiveDeviceCount = 0;
	{
		auto vulkan_adapter_sort_predicate = [&vendor_order](const vulkan::AdapterHandle& first, const vulkan::AdapterHandle& second) -> bool
		{
			vulkan::DeviceInfo device1_info = vulkan::GetDeviceInfo(first);
			vulkan::DeviceInfo device2_info = vulkan::GetDeviceInfo(second);
			// TODO add sort information here from config
			return vendor_order(device1_info.vendorID) < vendor_order(device2_info.vendorID);
		};

		{
			std::cerr << "Creating vulkan Device(s)" << std::endl;
			auto vulkan_adapters = EnumerateAdapters( test_vulkan );
			std::sort( vulkan_adapters.begin(), vulkan_adapters.end(), vulkan_adapter_sort_predicate );

			vulkan_devices.reserve( vulkan_adapters.size() );
			for( const auto& adapter : vulkan_adapters )
			{
				vulkan::DeviceInfo device_info = vulkan::GetDeviceInfo( adapter );
				// TODO : filter by requirements from config
				vulkan_devices.emplace_back( std::make_tuple( vulkan_device( adapter ), device_info ) );
			}
		}

		for (const auto& device : vulkan_devices)
			if (std::get<0>(device) && vendor_order(std::get<1>(device).vendorID) < kDebugDeviceStart)
				++vulkanActiveDeviceCount;

		for( const auto& device : vulkan_devices )
		{
			auto device_info = std::get<1>( device );
			std::cerr << "\t" << device_info.description << std::endl;
		}
	}

	dx12_instance test_dx12{};
	std::vector<std::tuple<dx12::DeviceHandle, dx12::DeviceInfo>> dx12_devices;
	size_t dx12ActiveDeviceCount = 0;
	{
		auto dx12_adapter_sort_predicate = [&vendor_order](const dx12::AdapterHandle& first, const dx12::AdapterHandle& second) -> bool
		{
			dx12::DeviceInfo device1_info = dx12::GetDeviceInfo(first);
			dx12::DeviceInfo device2_info = dx12::GetDeviceInfo(second);
			// TODO add sort information here from config
			return vendor_order(device1_info.vendorID) < vendor_order(device2_info.vendorID);
		};

		{
			std::cerr << "Creating DirectX12 Device(s)" << std::endl;
			auto dx12_adapters = EnumerateAdapters( test_dx12 );
			std::sort(dx12_adapters.begin(), dx12_adapters.end(), dx12_adapter_sort_predicate);

			dx12_devices.reserve(dx12_adapters.size());
			{
				for (const auto& adapter : dx12_adapters)
				{
					dx12::DeviceInfo device_info = dx12::GetDeviceInfo(adapter);
					// TODO : filter by requirements from config
					if ( vendor_order(device_info.vendorID) <= kDebugDeviceStart)
					{
						dx12_devices.emplace_back(std::make_tuple(dx12_device(adapter), device_info));
					}
					else
					{
						dx12_devices.emplace_back(std::make_tuple(nullptr, device_info));
					}
				}
			}
		}

		for (const auto& device : dx12_devices)
			if(std::get<0>(device) && vendor_order(std::get<1>(device).vendorID) < kDebugDeviceStart)
				++dx12ActiveDeviceCount;

		for( const auto& device : dx12_devices )
		{
			auto device_info = std::get<1>( device );
			std::cerr << "\t" << device_info.description;
			if( !std::get<0>(device) )
				std::cerr << " (" << (vendor_order( device_info.vendorID ) <= kDebugDeviceStart ? "" : "debug: ") << "skipped)";
			std::cerr << std::endl;
		}
	}

	{
		using namespace vulkan;
		//vulkan::DeviceHandle primary_device;
		// TODO
	}
	{
		using namespace dx12;
		DeviceHandle primary_device;
		DeviceHandle secondary_device;
		DeviceHandle debug_device;
		ref_ptr<ID3D12CommandQueue> primaryComputeCommandQueue;
		ref_ptr<ID3D12CommandQueue> secondaryComputeCommandQueue;
		ref_ptr<ID3D12CommandQueue> debugComputeCommandQueue;
		if (dx12ActiveDeviceCount > 0)
		{
			HRESULT hr = S_OK;
			primary_device = std::get<0>(dx12_devices[0]);
			D3D12_COMMAND_QUEUE_DESC primaryComputeQueueDesc{
				.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE,
				.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
				.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
				.NodeMask = 0, // TODO: check device->GetNodeCount()
			};
			hr = primary_device->CreateCommandQueue(&primaryComputeQueueDesc, SB_IID_ARGS(primaryComputeCommandQueue));
			if( !SUCCEEDED(hr) )
				__debugbreak();

			for (size_t deviceIndex = std::min<size_t>(1, dx12ActiveDeviceCount - 1); !secondary_device && deviceIndex < dx12_devices.size(); ++deviceIndex)
			{
				DeviceHandle device = std::get<0>(dx12_devices[deviceIndex]);
				secondary_device = device;
			}
			if( !secondary_device )
				secondary_device = primary_device;

			D3D12_COMMAND_QUEUE_DESC secondaryComputeQueueDesc{
				.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE,
				.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
				.Flags = D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT,
				.NodeMask = 0,
			};
			hr = secondary_device->CreateCommandQueue(&secondaryComputeQueueDesc, SB_IID_ARGS(secondaryComputeCommandQueue));
			if (!SUCCEEDED(hr))
				__debugbreak();

			for (size_t deviceIndex = std::min<size_t>(2, dx12ActiveDeviceCount - 1); !debug_device && deviceIndex < dx12_devices.size(); ++deviceIndex)
			{
				DeviceHandle device = std::get<0>(dx12_devices[deviceIndex]);
				debug_device = device;
			}
			if (!debug_device)
				debug_device = secondary_device;

			D3D12_COMMAND_QUEUE_DESC debugComputeQueueDesc{
				.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE,
				.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
				.Flags = D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT,
				.NodeMask = 0,
			};
			hr = debug_device->CreateCommandQueue(&debugComputeQueueDesc, SB_IID_ARGS(debugComputeCommandQueue));
			if (!SUCCEEDED(hr))
				__debugbreak();
		}
	}

	return 0;
}
