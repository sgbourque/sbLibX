#include <common/include/sb_interface_asio.h>
#include <common/include/sb_common.h>
#include <common/include/sb_utilities.h>

#include <iostream>


#define asio main
SB_EXPORT_TYPE int SB_STDCALL asio( [[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[] )
{
#if SB_TARGET_TYPE == SB_TARGET_TYPE_STATIC
	[[maybe_unused]] auto appartmentThreadedResult = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
	sb_scope_exit( []() { CoUninitialize(); } );
#endif
	
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
		std::cout << "Testing '" << deviceInfo.description << "'\n";

		sbLibX::asio_device device( adapter );
		if( device )
		{
			int32_t inputChannelCount{-1}, outputChannelCount{-1};
			auto result = device->getChannels( &inputChannelCount, &outputChannelCount );
			std::cout << "inputs:  " << inputChannelCount << "\n";
			std::cout << "outputs: " << outputChannelCount << "\n";
			if( !succeeded( result ) )
			{
				device.Release();
				std::cerr << "error: '" << deviceInfo.description << "': " << sbLibX::ASIO::to_cstring( result ) << std::endl;
				continue;
			}

			int32_t minSize{-1}, maxSize{-1}, preferredSize{-1}, granularity{-1};
			result = device->getBufferSize( &minSize, &maxSize, &preferredSize, &granularity );
			std::cout << "bufferSize (min/max): " << minSize << " / " << maxSize << "(preferred: " << preferredSize << ")\n";
			std::cout << "granularity:          " << granularity << "\n";
			if( !succeeded( result ) )
			{
				device.Release();
				std::cerr << "error: '" << deviceInfo.description << "': " << sbLibX::ASIO::to_cstring( result ) << std::endl;
				continue;
			}

			sbLibX::ASIO::SampleRate sampleRate{};
			result = device->getSampleRate( &sampleRate );
			std::cout << "sample rate: "<< sampleRate << "\n";
			if( !succeeded( result ) )
			{
				device.Release();
				std::cerr << "error: '" << deviceInfo.description << "': " << sbLibX::ASIO::to_cstring( result ) << std::endl;
				continue;
			}
	
			//
			std::vector<uint8_t> buffers[2];
			if( preferredSize > 0 )
				for( auto buffer : buffers )
					buffer.resize( static_cast<size_t>( preferredSize ) );
			sbLibX::ASIO::BufferInfo info = {
				/*StreamDirection*/  sbLibX::ASIO::StreamDirection::Output,
				/*int32_t        */  0,	// on input:  channel index
				/*void*          */ { buffers[0].data(), buffers[1].data() },	// on output: double buffer addresses
			};
			sbLibX::ASIO::Callbacks callbacks = {
				//void ( *bufferSwitch ) ( int32_t doubleBufferIndex, Bool directProcess );
				.bufferSwitch = []( [[maybe_unused]] int32_t doubleBufferIndex, [[maybe_unused]] sbLibX::ASIO::Bool directProcess ) {},
				//void ( *sampleRateDidChange ) ( SampleRate sRate );
				.sampleRateDidChange = []( [[maybe_unused]] sbLibX::ASIO::SampleRate sRate ) {},
				//int32_t( *asioMessage ) ( MessageSelector selector, int32_t value, void* message, double* opt );
				.asioMessage = []( [[maybe_unused]] sbLibX::ASIO::MessageSelector selector, [[maybe_unused]] int32_t value, [[maybe_unused]] void* message, [[maybe_unused]] double* opt ) -> int32_t { return 0; },
				//Time* ( *bufferSwitchTimeInfo ) ( Time* params, int32_t doubleBufferIndex, Bool directProcess );
				.bufferSwitchTimeInfo = []( sbLibX::ASIO::Time* params, [[maybe_unused]] int32_t doubleBufferIndex, [[maybe_unused]] sbLibX::ASIO::Bool directProcess ) -> sbLibX::ASIO::Time* { return params; },
			};
			result = device->createBuffers( &info, 1, preferredSize, &callbacks );
			if( !succeeded( result ) )
			{
				device.Release();
				std::cerr << "error: '" << deviceInfo.description << "' failed to create buffers: " << sbLibX::ASIO::to_cstring( result ) << std::endl;
				continue;
			}

			std::cout << "ASIO driver '" << deviceInfo.description << "' is available." << std::endl;
			result = device->start();
			if( !succeeded( result ) )
			{
				device.Release();
				std::cerr << "error: '" << deviceInfo.description << "' failed to start: "<< sbLibX::ASIO::to_cstring( result ) << std::endl;
				continue;
			}

			std::cout << "ASIO driver running"<< std::endl;
			//virtual ErrorType getLatencies( int32_t* inputLatency, int32_t* outputLatency ) = 0;

			//virtual ErrorType setSampleRate( SampleRate sampleRate ) = 0;

			//virtual ErrorType getClockSources( ClockSource* clocks, int32_t* numSources ) = 0;
			//virtual ErrorType setClockSource( int32_t reference ) = 0;

			//virtual ErrorType getSamplePosition( Samples* sPos, TimeStamp* tStamp ) = 0;
			//virtual ErrorType getChannelInfo( ChannelInfo* info ) = 0;

			//virtual ErrorType createBuffers( BufferInfo* bufferInfos, int32_t numChannels, int32_t bufferSize, Callbacks* callbacks ) = 0;

			//virtual ErrorType disposeBuffers() = 0;
			//virtual ErrorType controlPanel() = 0;
			//virtual ErrorType future( Future selector, void* opt ) = 0;
			//virtual ErrorType outputReady() = 0;
			
			//virtual ErrorType start();
			//virtual ErrorType stop();
		}
		else
		{
			std::cout << "ASIO driver '" << deviceInfo.description << "': " << sbLibX::ASIO::to_cstring( sbLibX::ASIO::ErrorType::NotPresent ) << std::endl;
		}
		deviceArray.emplace_back( std::move( device ) );
	}

	for( auto& device : deviceArray )
	{
		if( device )
			device->stop();
	}

	deviceArray.clear();
	return 0;
}
