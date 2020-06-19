#include <test/include/test_vulkan.h>

#include <vector>
namespace SB { namespace LibX {
struct Configuration;
}}

#if defined(SBWIN64) || defined(SBWIN32)
	#ifndef NOMINMAX
	#define NOMINMAX
	#endif
#endif
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <combaseapi.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3D12.lib")


namespace SB { namespace LibX {
namespace dx12
{
	template<typename Type>
using com_ptr = Microsoft::WRL::ComPtr<Type>;
using InstanceHandle = com_ptr<IDXGIFactory>; // IDXGIFactory4
using AdapterHandle = com_ptr<IDXGIAdapter>; // IDXGIAdapter4
using DeviceHandle = com_ptr<ID3D12Device>; // ID3D12Device6

#define SBLIB_DECLARE_DEVICE_INTERNAL
#include <common/include/device_generic_internal.h>
#undef SBLIB_DECLARE_DEVICE_INTERNAL
}
using dx12_instance = dx12::instance;
using dx12_device = dx12::device;

}}


namespace SB {
namespace LibX {
namespace dx12
{
InstanceHandle CreateInstance([[maybe_unused]] const Configuration* config)
{
	InstanceHandle factory;
	[[maybe_unused]] HRESULT hr = CreateDXGIFactory2( 0 /*DXGI_CREATE_FACTORY_DEBUG*/, IID_PPV_ARGS(factory.GetAddressOf()) );
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
	com_ptr<IDXGIFactory7> factory7;
	instance.As(&factory7);
	AdapterHandle adapter;
	adapter_array_t adapters;
	for ( uint32_t adapter_index = 0; SUCCEEDED( factory7->EnumAdapterByGpuPreference( adapter_index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.GetAddressOf()) ) ); ++adapter_index)
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
		if( utf8len >= sizeof(device_info.description) )
			--utf16len;
	} while( utf16len > 0 && utf8len >= sizeof(device_info.description) );
	if(utf8len > 0 && utf8len < sizeof(device_info.description))
		WideCharToMultiByte(CP_UTF8, 0, adapter_properties.Description, utf16len, device_info.description, sizeof(device_info.description), nullptr, nullptr);

	return device_info;
}

DeviceHandle CreateDevice([[maybe_unused]] AdapterHandle adapter, [[maybe_unused]] const Configuration* config)
{
	DeviceHandle device;
	D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1/*get(config, "dx12.feature_level")*/, IID_PPV_ARGS(device.GetAddressOf()));
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

SB_EXPORT_TYPE int __stdcall test_processing([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	// Trying to create both vulkan & DirectX devices on every adapters available
	using namespace SB::LibX;

	auto vendor_order = [](uint32_t vendor) -> uint32_t
	{
		switch (vendor)
		{
		case 0x10DE: // NVIDIA
			return 0x00000000;
		case 0x1002: // AMD
			return 0x00000001;
		case 0x8086: // Intel
			return 0x00000400;
		case 0x1414: // Microsoft
			return 0x80000000;
		default:
			__debugbreak(); // unknown vendor
			break;
		}
		return 0xFFFFFFFF;
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

		std::cerr << "Creating vulkan Device(s)" << std::endl;
		auto vulkan_adapters = EnumerateAdapters(test_vulkan);
		std::sort(vulkan_adapters.begin(), vulkan_adapters.end(), vulkan_adapter_sort_predicate);

		vulkan_devices.reserve(vulkan_adapters.size());
		for (const auto& adapter : vulkan_adapters)
		{
			vulkan::DeviceInfo device_info = vulkan::GetDeviceInfo(adapter);
			std::cerr << "\t" << device_info.description << std::endl;
			// TODO : filter by requirements from config
			vulkan_devices.emplace_back(std::make_tuple(vulkan_device(adapter), device_info));
		}

		for (const auto& device : vulkan_devices)
			if (std::get<0>(device) && vendor_order(std::get<1>(device).vendorID) < kDebugDeviceStart)
				++vulkanActiveDeviceCount;
	}

	dx12_instance test_dx12{};
	std::vector<std::tuple<dx12::DeviceHandle, dx12::DeviceInfo>> dx12_devices;
	int dx12ActiveDeviceCount = 0;
	{
		auto dx12_adapter_sort_predicate = [&vendor_order](const dx12::AdapterHandle& first, const dx12::AdapterHandle& second) -> bool
		{
			dx12::DeviceInfo device1_info = dx12::GetDeviceInfo(first);
			dx12::DeviceInfo device2_info = dx12::GetDeviceInfo(second);
			// TODO add sort information here from config
			return vendor_order(device1_info.vendorID) < vendor_order(device2_info.vendorID);
		};

		std::cerr << "Creating DirectX12 Device(s)" << std::endl;
		auto dx12_adapters = EnumerateAdapters(test_dx12);
		std::sort(dx12_adapters.begin(), dx12_adapters.end(), dx12_adapter_sort_predicate);

		dx12_devices.reserve(dx12_adapters.size());
		{
			for (const auto& adapter : dx12_adapters)
			{
				dx12::DeviceInfo device_info = dx12::GetDeviceInfo(adapter);
				// TODO : filter by requirements from config
				if ( vendor_order(device_info.vendorID) <= kDebugDeviceStart)
				{
					std::cerr << "\t" << device_info.description << std::endl;
					dx12_devices.emplace_back(std::make_tuple(dx12_device(adapter), device_info));
				}
				else
				{
					std::cerr << "\t" << device_info.description << " (skipped)" << std::endl;
					dx12_devices.emplace_back(std::make_tuple(nullptr, device_info));
				}
			}
		}

		for (const auto& device : dx12_devices)
			if(std::get<0>(device) && vendor_order(std::get<1>(device).vendorID) < kDebugDeviceStart)
				++dx12ActiveDeviceCount;
	}

	{
		using namespace vulkan;
		//vulkan::DeviceHandle primary_device;
		//
	}
	{
		using namespace dx12;
		DeviceHandle primary_device;
		DeviceHandle secondary_device;
		DeviceHandle debug_device;
		com_ptr<ID3D12CommandQueue> primaryComputeCommandQueue;
		com_ptr<ID3D12CommandQueue> secondaryComputeCommandQueue;
		com_ptr<ID3D12CommandQueue> debugComputeCommandQueue;
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
			hr = primary_device->CreateCommandQueue(&primaryComputeQueueDesc, IID_PPV_ARGS(primaryComputeCommandQueue.GetAddressOf()));
			if( !SUCCEEDED(hr) )
				__debugbreak();

			for (int deviceIndex = std::min(1, dx12ActiveDeviceCount - 1); !secondary_device && deviceIndex < dx12_devices.size(); ++deviceIndex)
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
			hr = secondary_device->CreateCommandQueue(&secondaryComputeQueueDesc, IID_PPV_ARGS(secondaryComputeCommandQueue.GetAddressOf()));
			if (!SUCCEEDED(hr))
				__debugbreak();

			for (int deviceIndex = std::min(2, dx12ActiveDeviceCount - 1); !debug_device && deviceIndex < dx12_devices.size(); ++deviceIndex)
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
			hr = debug_device->CreateCommandQueue(&debugComputeQueueDesc, IID_PPV_ARGS(debugComputeCommandQueue.GetAddressOf()));
			if (!SUCCEEDED(hr))
				__debugbreak();
		}
	}

	return 0;
}
