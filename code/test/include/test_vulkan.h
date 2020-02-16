#pragma once
//#if defined(SBWIN64) || defined(SBWIN32)
//	#pragma warning(disable: 4472)
//
//	#if !defined(SBDEBUG)
//	#pragma warning(disable:4668 5039 4339)
//	#define NOMINMAX
//	#include <windows.h>
//	#pragma warning(default:4668 5039)
//	#ifndef WINAPI
//		#define WINAPI __stdcall
//	#endif
//	#endif
//#endif

//#if !defined(SBDEBUG)
//	#pragma warning(disable:4081 4365 4472 4514 4571 4625 4626 4710 4774 4820 5026 5027)
//	#include <iostream>
//	#pragma warning(default)
//#endif

//extern int main(int _nArgs = 0,  const char* const pArgs[] = nullptr);

//#include <ctype.h>
#include <unordered_map>

#ifndef NOMINMAX
	#define NOMINMAX
#endif
#define VK_USE_PLATFORM_WIN32_KHR	1
#include <vulkan/vulkan.h>
//#include <vulkan/vk_sdk_platform.h>

namespace SB {
struct Configuration;

namespace LibX { namespace vulkan
{
using InstanceHandle = VkInstance;
using AdapterHandle = VkPhysicalDevice;
using DeviceHandle = VkDevice;

InstanceHandle CreateInstance( const Configuration* config = nullptr );
bool DestroyInstance( InstanceHandle instance, const Configuration* config = nullptr);

using adapter_array_t = std::vector<AdapterHandle>;
adapter_array_t GetDeviceAdapters( InstanceHandle instance );

struct DeviceInfo
{
	static constexpr size_t kMaxNameLength = 256;
	const char description[kMaxNameLength];
	uint32_t vendorID;
	uint32_t deviceID;
	uint32_t apiID;
	uint32_t driverVersion;
	uint32_t deviceType;
	const char uid[16];
};
DeviceInfo GetDeviceInfo(AdapterHandle adapter);

DeviceHandle CreateDevice(AdapterHandle adapter, const Configuration* config = nullptr);
bool DestroyDevice(DeviceHandle device, const Configuration* config = nullptr);


} // namespace vulkan


struct vulkan_instance
{
	vulkan_instance(const Configuration* config = nullptr) { handle = vulkan::CreateInstance(config); }
	vulkan_instance(vulkan_instance&& other) { handle = other.handle; other.handle = vulkan::InstanceHandle{}; }
	~vulkan_instance() { vulkan::DestroyInstance(handle); }

	operator vulkan::InstanceHandle() const { return handle; }
public:
	vulkan::InstanceHandle handle;
};

struct vulkan_device
{
	vulkan_device(vulkan::AdapterHandle adapter = {}, const Configuration* config = nullptr) { handle = vulkan::CreateDevice(adapter, config); }
	vulkan_device(vulkan_device&& other) { handle = other.handle; other.handle = vulkan::DeviceHandle{}; }
	~vulkan_device() { vulkan::DestroyDevice(handle); }

	operator vulkan::DeviceHandle() const { return handle; }
public:
	vulkan::DeviceHandle handle;
};


}} // namespace SB::LibX
