#include <common/include/sb_interface_dx12.h>
#include <common/include/sb_interface_vulkan.h>
#include <common/include/internal/ref_ptr.h>

#include <vector>
namespace SB { namespace LibX {
struct Configuration;
}}

// Note: eventually, we should not require this.
// Hoever, since not all functionalities are exposed in sbLib, we are going to use low-level access here
#define COM_NO_WINDOWS_H
#include <dxgi1_6.h>
#include <d3d12.h>


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
	std::vector<std::tuple<vulkan_device, vulkan::DeviceDesc>> vulkan_devices{};
	int vulkanActiveDeviceCount = 0;
	{
		auto vulkan_adapter_sort_predicate = [&vendor_order](const vulkan::AdapterHandle& first, const vulkan::AdapterHandle& second) -> bool
		{
			vulkan::DeviceDesc device1_desc = vulkan::GetDeviceDesc(first);
			vulkan::DeviceDesc device2_desc = vulkan::GetDeviceDesc(second);
			// TODO add sort information here from config
			return vendor_order(device1_desc.vendorID) < vendor_order(device2_desc.vendorID);
		};

		{
			std::cerr << "Creating vulkan Device(s)" << std::endl;
			auto vulkan_adapters = EnumerateAdapters( test_vulkan );
			std::sort( vulkan_adapters.begin(), vulkan_adapters.end(), vulkan_adapter_sort_predicate );

			vulkan_devices.reserve( vulkan_adapters.size() );
			for( const auto& adapter : vulkan_adapters )
			{
				vulkan::DeviceDesc device_desc = vulkan::GetDeviceDesc( adapter );
				// TODO : filter by requirements from config (we probably won't want all adapters... but we may want to be able to :) )
				vulkan_devices.emplace_back( std::make_tuple( vulkan_device( adapter ), device_desc ) );
			}
		}

		for (const auto& device : vulkan_devices)
			if (std::get<0>(device) && vendor_order(std::get<1>(device).vendorID) < kDebugDeviceStart)
				++vulkanActiveDeviceCount;

		for( const auto& device : vulkan_devices )
		{
			auto device_desc = std::get<1>( device );
			std::cerr << "\t" << device_desc.name << std::endl;
		}
	}

	dx12_instance test_dx12{};
	std::vector<std::tuple<dx12::DeviceHandle, dx12::DeviceDesc>> dx12_devices;
	size_t dx12ActiveDeviceCount = 0;
	{
		auto dx12_adapter_sort_predicate = [&vendor_order](const dx12::AdapterHandle& first, const dx12::AdapterHandle& second) -> bool
		{
			dx12::DeviceDesc device1_desc = dx12::GetDeviceDesc(first);
			dx12::DeviceDesc device2_desc = dx12::GetDeviceDesc(second);
			// TODO add sort information here from config
			return vendor_order(device1_desc.vendorID) < vendor_order(device2_desc.vendorID);
		};

		{
			std::cerr << "Creating DirectX12 Device(s)" << std::endl;
			auto dx12_adapters = EnumerateAdapters( test_dx12 );
			std::sort(dx12_adapters.begin(), dx12_adapters.end(), dx12_adapter_sort_predicate);

			dx12_devices.reserve(dx12_adapters.size());
			{
				for (const auto& adapter : dx12_adapters)
				{
					dx12::DeviceDesc device_desc = dx12::GetDeviceDesc(adapter);
					// TODO : filter by requirements from config
					if ( vendor_order(device_desc.vendorID) <= kDebugDeviceStart)
					{
						dx12_devices.emplace_back(std::make_tuple(dx12_device(adapter), device_desc));
					}
					else
					{
						dx12_devices.emplace_back(std::make_tuple(nullptr, device_desc));
					}
				}
			}
		}

		for (const auto& device : dx12_devices)
			if(std::get<0>(device) && vendor_order(std::get<1>(device).vendorID) < kDebugDeviceStart)
				++dx12ActiveDeviceCount;

		for( const auto& device : dx12_devices )
		{
			auto device_desc = std::get<1>( device );
			std::cerr << "\t" << device_desc.name;
			if( !std::get<0>(device) )
				std::cerr << " (" << (vendor_order( device_desc.vendorID ) <= kDebugDeviceStart ? "" : "debug: ") << "skipped)";
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
			[[maybe_unused]] HRESULT hr = S_OK;
			primary_device = std::get<0>(dx12_devices[0]);
			[[maybe_unused]] D3D12_COMMAND_QUEUE_DESC primaryComputeQueueDesc{
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

			[[maybe_unused]] D3D12_COMMAND_QUEUE_DESC secondaryComputeQueueDesc{
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

			[[maybe_unused]] D3D12_COMMAND_QUEUE_DESC debugComputeQueueDesc{
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
