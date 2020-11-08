#include <common/include/sb_interface_asio.h>

#include <iostream>

#include <common/include/sb_common.h>
SB_EXPORT_TYPE int SB_STDCALL asio( [[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[] )
{
	sbLibX::ASIO::adapter_array_t adapterArray{};
	{
		// make sure adapterArray is alright passed instanceHandle being released
		auto instanceHandle = sbLibX::ASIO::CreateInstance();
		adapterArray = EnumerateAdapters( instanceHandle );
	}
	std::vector<sbLibX::asio_device> deviceArray;
	deviceArray.reserve( adapterArray.size() );
	for( auto adapter : adapterArray )
	{
		sbLibX::ASIO::DeviceInfo deviceInfo;
		adapter->GetDeviceInfo( &deviceInfo );

		sbLibX::asio_device device( adapter );
		if( device )
		{
			// starts processing, this is the last steps for validating 
			auto result = device->start();
			if( !succeeded( result ) )
			{
				device.Release();
				std::cout << "ASIO driver '" << deviceInfo.description << "': " << sbLibX::ASIO::to_cstring( result ) << std::endl;
			}
			else
			{
				std::cout << "ASIO driver '" << deviceInfo.description << "': Available." << std::endl;
			}

			// Please do not call controlPanel unless current process is managing its message queue.
		}
		else
		{
			std::cout << "ASIO driver '" << deviceInfo.description << "': " << sbLibX::ASIO::to_cstring( sbLibX::ASIO::ErrorType::NotPresent ) << std::endl;
		}
		deviceArray.emplace_back( std::move( device ) );
	}
	deviceArray.clear();
	return 0;
}
