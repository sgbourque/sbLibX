#ifndef SBLIB_DECLARE_DEVICE_INTERNAL
#error "If you see this, you probably messed up something..."
#endif

InstanceHandle CreateInstance( const Configuration* config = nullptr );
bool DestroyInstance( InstanceHandle instance, const Configuration* config = nullptr);

using adapter_array_t = std::vector<AdapterHandle>;
adapter_array_t EnumerateAdapters( InstanceHandle instance, size_t maxCount = ~0u );

#ifdef SBLIB_FORWARD_DECLARE_DEVICE_INFO_INTERNAL
struct DeviceInfo;
#else
struct DeviceInfo
{
	static constexpr size_t kDescSize = 256;
	static constexpr size_t kIIDSize = 16;
	char     description[kDescSize];
	uint32_t vendorID;
	uint32_t deviceID;
	uint32_t apiID;
	uint32_t driverVersion;
	uint8_t  uid[16];
};
#endif
DeviceInfo GetDeviceInfo(AdapterHandle adapter);

DeviceHandle CreateDevice(AdapterHandle adapter, const Configuration* config = nullptr);
bool DestroyDevice(DeviceHandle device, const Configuration* config = nullptr);

struct instance
{
	instance(const Configuration* config = nullptr) { handle = CreateInstance(config); }
	instance(instance&& other) { handle = other.handle; other.handle = InstanceHandle{}; }
	~instance() { DestroyInstance( std::move(handle) ); }

	operator InstanceHandle() const { return handle; }
public:
	// TODO : should validate that it is ref counted
	InstanceHandle handle;
};

struct device
{
	explicit device(AdapterHandle adapter = {}, const Configuration* config = nullptr) { handle = CreateDevice(adapter, config); }
	explicit device(device&& other) { handle = other.handle; other.handle = DeviceHandle{}; }
	~device() { DestroyDevice(handle); }

	void Release() { handle = DeviceHandle{}; }

	DeviceHandle operator ->() { return handle; }
	DeviceHandle operator ->() const { return handle; }

	operator DeviceHandle() const { return handle; }
	operator bool() const { return handle; }
public:
	// TODO : should validate that it is ref counted
	DeviceHandle handle;
};
