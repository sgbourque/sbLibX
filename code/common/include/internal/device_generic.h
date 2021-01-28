#ifndef SBLIB_DECLARE_DEVICE_INTERNAL
#error "If you see this, you probably messed up something..."
#endif

#ifndef SB_LIB_EXPORT
#define SB_LIB_EXPORT SB_IMPORT_LIB
#endif

SB_LIB_EXPORT InstanceHandle CreateInstance( const Configuration* config = nullptr );
SB_LIB_EXPORT bool DestroyInstance( InstanceHandle instance, const Configuration* config = nullptr);

#ifndef SBLIB_CUSTOM_ADAPTER_ARRAY
using adapter_array_t = std::vector<AdapterHandle>;
#endif
SB_LIB_EXPORT adapter_array_t EnumerateAdapters( InstanceHandle instance = CreateInstance(), size_t maxCount = ~0u );

#ifndef SBLIB_FORWARD_DECLARE_DEVICE_INFO_INTERNAL
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
SB_LIB_EXPORT DeviceInfo GetDeviceInfo(AdapterHandle adapter);

SB_LIB_EXPORT DeviceHandle CreateDevice(AdapterHandle adapter, const Configuration* config = nullptr);
SB_LIB_EXPORT bool DestroyDevice(DeviceHandle device, const Configuration* config = nullptr);

struct unique_instance
{
	explicit unique_instance(const Configuration* config = nullptr)
	{
		handle = CreateInstance(config);
	}
	explicit unique_instance(unique_instance&& other)
	{
		handle = other.Detach();
	}
	explicit unique_instance(const unique_instance& other) = delete;
	~unique_instance()
	{
		Release();
	}

	InstanceHandle Detach()
	{
		return std::move(handle);
	}
	void Release() { if( handle ) DestroyInstance( std::move( handle ) ); }

	InstanceHandle& operator ->()
	{
		return handle;
	}
	const InstanceHandle& operator ->() const
	{
		return handle;
	}

	operator InstanceHandle() const
	{
		return handle;
	}
	operator bool() const
	{
		return handle;
	}
public:
	InstanceHandle handle;
};

struct unique_device
{
	explicit unique_device(AdapterHandle adapter = {}, const Configuration* config = nullptr) { if( adapter ) handle = CreateDevice(adapter, config); }
	explicit unique_device(unique_device&& other) { handle = other.handle; other.Detach(); }
	explicit unique_device(const unique_device& other) = delete;
	~unique_device() { Release(); }

	void Detach() { handle = DeviceHandle{}; }
	void Release() { if( handle ) DestroyDevice( std::move(handle) ); }

	DeviceHandle& operator ->() { return handle; }
	const DeviceHandle& operator ->() const { return handle; }

	operator DeviceHandle() const { return handle; }
	operator bool() const { return handle; }
public:
	DeviceHandle handle;
};

#undef SB_LIB_EXPORT
