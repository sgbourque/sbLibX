#include <common/include/sb_interface_asio.h>
#include <common/include/sb_common.h>
#include <common/include/sb_utilities.h>

#include <functional>
#include <iostream>


#if 1
namespace SB { namespace LibX { namespace ASIO
{
auto CreateDevices( adapter_array_t& adapters )
{
	std::vector< std::tuple<unique_device, DeviceInfo> > devices;
	devices.reserve( adapters.size() );
	for( const auto& adapter : adapters )
	{
		auto device_info = GetDeviceInfo( adapter );
//		std::clog << device_info.description << " : " << std::endl;
//		//auto device_extensions = enumerate<device_extension::properties_t>( adapter );
//		//for( const auto& extension_properties : device_extensions )
//		//{
//		//	std::clog << "\t" << extension_properties.extensionName << std::endl;
//		//}
		devices.emplace_back( std::make_tuple( unique_device( adapter ), device_info ) );
	}
	return devices;
}
}}} // sbLibASIO


//std::vector<sbLibASIO::ChannelControls>
//struct ChannelInfo
//{
//	int32_t          channel;     	// on input, channel index
//	StreamDirection  direction;   	// 1 on input
//	Bool             isActive;    	// on exit
//	int32_t          channelGroup;	// dto
//	SampleType       type;        	// dto
//	char             name[32];    	// dto
//};
//struct BufferInfo
//{
//	StreamDirection  direction;  	// 1 on input
//	int32_t          channelNum;	// on input:  channel index
//	void*            buffers[2];	// on output: double buffer addresses
//};


bool printInfo( sbLibASIO::DeviceHandle device, const sbLibASIO::DeviceInfo& deviceInfo )
{
	using namespace sbLibASIO;
	std::cout << "'" << deviceInfo.description << "':\n";

	{
		char driverName[32]{};
		device->getDriverName( driverName );
		std::cout << "driver name: '" << driverName << "' version "<< device->getDriverVersion() << "\n";
	}

	SampleRate rate_hz = -1.0;
	{
		SampleRate sample_rates[] ={
			44100.0,
			48000.0,
			88200.0,
			96000.0,
			176400.0,
			192000.0,
		};
		std::cout << "supported rates: ";
		bool first = true;
		for( auto rate : sample_rates )
		{
			if( succeeded( device->canSampleRate( rate ) ) )
			{
				std::cout << ( first ? "" : ", " ) << rate;
				first = false;
			}
		}

		device->getSampleRate( &rate_hz );
		std::cout << " ( current = " << rate_hz << ")\n";
	}

	{
		std::cout << "clock sources: ";
		std::vector<ClockSource> sources;
		int32_t max_count = 1;
		int32_t count = max_count;
		while( count == max_count )
		{
			count = ( max_count *= 2 );
			sources.resize( size_t(max_count) );
			device->getClockSources( sources.data(), &count );
		}
		sources.resize( size_t(count) );

		for( const auto& source : sources )
		{
			std::cout << ( source.isCurrentSource ? "\t " : "\t*" )
				<< source.name << " [" << source.index << "]: "
				<< "channel = " << source.associatedChannel << "; "
				<< "group = " << source.associatedGroup << "\n";
		}
	}

	{
		int32_t inputCount = 0, outputCount = 0;
		device->getChannels( &inputCount, &outputCount );
		std::cout << "Channels (in = " << inputCount << "; out = " << outputCount << ")\n";
		auto printChannelInfo = [device]( int32_t channel, StreamDirection direction )
		{
			ChannelInfo info{
				.channel = channel,
				.direction = direction,
			};
			device->getChannelInfo( &info );
			std::cout << ( info.isActive ? "\t*" : "\t " )
				<< ( info.direction == StreamDirection::Input ? "in " : "out " )
				<< info.channel << " [" << info.channelGroup << "]: "
				<< "'" << info.name << "' - ";
			switch( info.type )
			{
			case SampleType::Int16_MSB: std::cout << "16-bits int MSB\n"; break;
			case SampleType::Int24_MSB: std::cout << "20/24-bits int MSB\n"; break;
			case SampleType::Int32_MSB: std::cout << "32-bits int MSB\n"; break;
			case SampleType::Float32_MSB: std::cout << "IEEE 754 32-bits float MSB\n"; break;
			case SampleType::Float64_MSB: std::cout << "IEEE 754 64-bits float MSB\n"; break;

			case SampleType::Int32_MSB16: std::cout << "32(16)-bits int MSB\n"; break;
			case SampleType::Int32_MSB18: std::cout << "32(18)-bits int MSB\n"; break;
			case SampleType::Int32_MSB20: std::cout << "32(20)-bits int MSB\n"; break;
			case SampleType::Int32_MSB24: std::cout << "32(24)-bits int MSB\n"; break;
			case SampleType::Int16_LSB: std::cout << "16-bits LSB\n"; break;
			case SampleType::Int24_LSB: std::cout << "20/24-bits LSB\n"; break;
			case SampleType::Int32_LSB: std::cout << "32-bits LSB\n"; break;
			case SampleType::Float32_LSB: std::cout << "IEEE 754 32-bits float LSB\n"; break;
			case SampleType::Float64_LSB: std::cout << "IEEE 754 64-bits float LSB\n"; break;
			case SampleType::Int32_LSB16: std::cout << "32(16)-bits int LSB\n"; break;
			case SampleType::Int32_LSB18: std::cout << "32(18)-bits int LSB\n"; break;
			case SampleType::Int32_LSB20: std::cout << "32(20)-bits int LSB\n"; break;
			case SampleType::Int32_LSB24: std::cout << "32(24)-bits int LSB\n"; break;

			case SampleType::DSD_Int8_LSB1: std::cout << "DSD 8x1-bit data, first = MSB\n"; break;
			case SampleType::DSD_Int8_MSB1: std::cout << "DSD 8x1-bit data, first = LSB\n"; break;
			case SampleType::DSD_Int8_NER8: std::cout << "DSD 8-bits\n"; break;
			}
		};

		for( int32_t channel = 0; channel < inputCount; ++channel )
			printChannelInfo( channel, StreamDirection::Input );
		for( int32_t channel = 0; channel < outputCount; ++channel )
			printChannelInfo( channel, StreamDirection::Output );
	}

	int32_t minSize = 0, maxSize = 0, preferredSize = 0, granularity = 0;
	int32_t inputLatency = 0, outputLatency = 0;
	{
		device->getBufferSize( &minSize, &maxSize, &preferredSize, &granularity );
		std::cout << "buffer size: min = " << minSize << "; max = " << maxSize << "; preferred = " << preferredSize << "; grain = " << granularity << "\n";
		
		device->getLatencies( &inputLatency, &outputLatency );
		std::cout << "latency: in = " << inputLatency << "; out = " << outputLatency << "\n";
	}

	{
		std::cout << "features:";
		std::tuple<Future, const char*> features[] = {
			{ Future::CanInputMonitor, "input_monitor", },
			{ Future::CanTimeInfo, "time_info", },
			{ Future::CanTimeCode, "time_code", },
			{ Future::CanTransport, "tranport", },
			{ Future::CanInputGain, "input_gain", },
			{ Future::CanInputMeter, "input_meter", },
			{ Future::CanOutputGain, "output_gain", },
			{ Future::CanOutputMeter, "output_meter", },
			{ Future::OptionalOne, "optional1", },
			{ Future::CanReportOverload, "report_overload", },
		};
		for( const auto& feature : features )
			if( succeeded( device->future( std::get<Future>(feature) ) ) )
				std::cout << " " << std::get<const char*>(feature);
		std::cout << "\n";
	}

	{
		//	DSD support
		//	The following extensions are required to allow switching
		//	and control of the DSD subsystem.
		//GetIoFormat   = 0x23111983,		// IoFormat* in params
		//CanDoIoFormat = 0x23112004,		// IoFormat* in params
	}
	{
		//GetInternalBufferSamples = 0x25042012,	/* InternalBufferInfo * in params. Deliver size of driver internal buffering, return ASE_SUCCESS if supported */
	}

	//SampleRate rate = -1.0;
	//int32_t minSize = 0, maxSize = 0, preferredSize = 0, granularity = 0;
	//int32_t inputLatency = 0, outputLatency = 0;
	bool is_low_latency = false;
	{
		std::cout << "----\n";
		double target_lattency_ms = 5.0;
		std::cout << "target latency: " << target_lattency_ms << "ms\n";

		const double min_processing_lattency_ms = 1.0;
		std::cout << "min processing latency: " << min_processing_lattency_ms << "ms\n";

		const double sample_latency_ms = ( 1000.0 / rate_hz );
		const double input_latency_ms = inputLatency * sample_latency_ms;
		const double output_latency_ms = outputLatency * sample_latency_ms;
		const double total_internal_latency_ms = input_latency_ms + output_latency_ms;
		const double target_buffer_latency_ms = std::max( min_processing_lattency_ms, target_lattency_ms - total_internal_latency_ms );
		const double min_latency_ms = ( minSize * sample_latency_ms );
		const double max_latency_ms = ( maxSize * sample_latency_ms );
		const double grain_latency_ms = ( granularity * sample_latency_ms );

		const double extra_latency_ms = std::max( 0.0, target_buffer_latency_ms - min_latency_ms );
		const double buffer_count = std::ceil( extra_latency_ms / grain_latency_ms );
		const double buffer_latency_ms = std::min( max_latency_ms, min_latency_ms + grain_latency_ms * buffer_count );
		const double total_latency_ms = total_internal_latency_ms + buffer_latency_ms;
		std::cout << "using " << size_t(minSize) + size_t(buffer_count) * size_t(granularity) << " samples:\n";
		std::cout << "\tinput latency = " << input_latency_ms << "ms\n";
		std::cout << "\tbuffer latency = " << buffer_latency_ms << "ms\n";
		std::cout << "\toutput latency = " << output_latency_ms << "ms\n";
		std::cout << "\ttotal latency = " << total_latency_ms << "ms (~" << total_latency_ms * (340.0/1000.0) << "m at ~340m/s)\n";
		std::cout << "----\n";

		const double low_latency_limit = 5.0;
		is_low_latency = ( buffer_latency_ms < low_latency_limit );
	}

	std::cout << "\n";
	return is_low_latency;
}


//#if ( (SB_TARGET_TYPE & SB_TARGET_TYPE_STANDALONE) != 0 )
//#define asio main
//#endif
SB_EXPORT_TYPE int SB_STDCALL asio( [[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[] )
{
#if ( (SB_TARGET_TYPE & SB_TARGET_TYPE_STATIC) != 0 )
	[[maybe_unused]] auto appartmentThreadedResult = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
	sb_scope_exit( []() { CoUninitialize(); } );
#endif

	using namespace sbLibASIO;

	auto adapterArray = EnumerateAdapters();
	auto deviceArray = CreateDevices( adapterArray );
	for( auto& device : deviceArray )
	{
		// stupid lazy hack coz I dont want to pump win32 messages yet so just pop control panels of "real" low latency devices...
		bool is_low_latency = false;
		auto deviceInfo = std::get<DeviceInfo>( device );
		DeviceHandle handle = std::get<unique_device>( device );
		if( handle )
		{
			is_low_latency = printInfo( handle, deviceInfo );
	//		//std::unique_ptr<AsioCallbacks*> callbacks = GenerateASIOBuffers( device.handle.Get(), inputChannelCount, outputChannelCount, preferredSize );
	//		//if( preferredSize > 0 )
	//		//{
	//		//	auto& buffers = ( *callbacks )->data();
	//		//	info.reserve( inputChannelCount + outputChannelCount );
	//		//
	//		//	for( int32_t channel = 0; channel < inputChannelCount; ++channel )
	//		//	{
	//		//		info.emplace_back( sbLibASIO::BufferInfo{
	//		//			/*StreamDirection*/  sbLibASIO::StreamDirection::Input,
	//		//			/*int32_t        */  channel,
	//		//			/*void*          */ { buffers[0][channel].data(), buffers[1][channel].data() },
	//		//			} );
	//		//	}
	//		//	for( int32_t channel = 0; channel < outputChannelCount; ++channel )
	//		//	{
	//		//		info.emplace_back( sbLibASIO::BufferInfo{
	//		//			/*StreamDirection*/  sbLibASIO::StreamDirection::Output,
	//		//			/*int32_t        */  channel,
	//		//			/*void*          */ { buffers[0][channel].data(), buffers[1][inputChannelCount + channel].data() },
	//		//			} );
	//		//	}
	//		//}
	//		//
	//		//__debugbreak(); // d'oh ! should use buffers from the ASIO driver before the explicit buffers for private use!
	//		//result = device->createBuffers( info.data(), inputChannelCount + outputChannelCount, preferredSize, *callbacks );
	//		//if( !succeeded( result ) )
	//		//{
	//		//	device.Release();
	//		//	std::cerr << "error: '" << deviceInfo.description << "' failed to create buffers: " << sbLibASIO::to_cstring( result ) << std::endl;
	//		//	continue;
	//		//}
	//		//
	//		//std::cout << "ASIO driver '" << deviceInfo.description << "' is available." << std::endl;
	//		//result = device->start();
	//		//if( !succeeded( result ) )
	//		//{
	//		//	device.Release();
	//		//	std::cerr << "error: '" << deviceInfo.description << "' failed to start: "<< sbLibASIO::to_cstring( result ) << std::endl;
	//		//	continue;
	//		//}
	//		//
		}
		else
		{
			std::cout << "Could not initialize '" << deviceInfo.description
				<< "': " << sbLibASIO::to_cstring( sbLibASIO::ErrorType::NotPresent ) << std::endl;
		}
		if( handle && is_low_latency )
			handle->controlPanel();
	}

	std::cout << "press 'Enter' to continue\n";
	std::cin.get();

	for( auto& device : deviceArray )
	{
		auto& handle = std::get<unique_device>( device );
		if( handle )
		{
			std::cout << "Uninitializing '" << std::get<DeviceInfo>( device ).description << "'" << std::endl;
			handle->stop();
			handle->disposeBuffers();
		}
	}

	deviceArray.clear();
	return 0;
}

#else

struct cpu_buffers_accessor
{
//	using data_t = std::vector<std::vector<uint8_t>>[2];
//
//	virtual data_t& data() = 0;
//	virtual const data_t& data() const = 0;
//
//	virtual size_t in_size() const = 0;
//	virtual size_t out_size() const = 0;
};
//struct AsioCallbacks : sbLibASIO::Callbacks, cpu_buffers_accessor
#include <span>

#error "fuck... need ChannelInfo to actually get SampleType and deduce sample size...""

struct AsioCpuBuffers
{
	static inline constexpr size_t buffer_count = 2;
	using callback_t = sbLibASIO::Callbacks;
	using desc_t = std::vector<sbLibASIO::BufferInfo>;
	// TODO:
	//		- split for gpu upload via copy-queue and gpu readback buffers
	//		- add fence information for gpu buffer synchronisation
	using data_t = std::vector<uint8_t>[buffer_count];

	size_t buffer_size{};
	size_t output_count{};
	size_t input_count{};

	desc_t buffer_desc{};
	data_t scratch_buffer{};

	AsioCpuBuffers( size_t _buffer_size, size_t _output_count, size_t _input_count )
		: buffer_size{_buffer_size}, output_count{_output_count}, input_count{_input_count}
	{
		const size_t channel_count = ( input_count + output_count );
		buffer_desc.reserve( channel_count );
		for( int32_t channel = 0; channel < output_count; ++channel )
		{
			buffer_desc.emplace_back( sbLibASIO::BufferInfo{
				/*StreamDirection*/  sbLibASIO::StreamDirection::Output,
				/*int32_t        */  channel,
				/*void*          */  {},
				} );
		}
		for( int32_t channel = 0; channel < input_count; ++channel )
		{
			buffer_desc.emplace_back( sbLibASIO::BufferInfo{
				/*StreamDirection*/  sbLibASIO::StreamDirection::Input,
				/*int32_t        */  channel,
				/*void*          */  {},
				} );
		}

		scratch_buffer[0].resize( channel_count * buffer_size * buffer_count );
		scratch_buffer[1].resize( channel_count * buffer_size * buffer_count );
	}

	std::span<data_t> get_data( int32_t channel, sbLibASIO::StreamDirection direction, int32_t buffer_index )
	{
		const size_t channel_count = ( input_count + output_count );
		const size_t channel_offset = buffer_index * channel_count + ( direction == sbLibASIO::StreamDirection::Input ? output_count : 0 ) + channel;

		std::span<data_t> channel_data{};
		const size_t channel_strip_size = ( direction == sbLibASIO::StreamDirection::Input ? input_count : output_count );
		if( channel < channel_strip_size )
		{
			channel_data = std::span<data_t>{ scratch_buffer[buffer_index].data() + channel_offset * buffer_size, buffer_size };
		}
		return channel_data;
	}
	std::span<data_t> get_asio_buffer( int32_t channel, sbLibASIO::StreamDirection direction, int32_t buffer_index )
	{
		const size_t channel_count = ( input_count + output_count );
		const size_t channel_offset = buffer_index * channel_count + ( direction == sbLibASIO::StreamDirection::Input ? output_count : 0 ) + channel;

		std::span<data_t> asio_data{};
		const size_t channel_strip_size = ( direction == sbLibASIO::StreamDirection::Input ? input_count : output_count );
		if( channel < channel_strip_size )
		{
			asio_data = std::span<data_t>{ scratch_buffer[buffer_index].data() + channel_offset * buffer_size, buffer_size };
		}
		return asio_data;
	}

	size_t in_count()
	{
		return input_count;
	}
	size_t out_count()
	{
		return output_count;
	}
};


// ineffective proof-of-concept (buffers should be unified to a single allocation or at least optimal allocation count)
struct cpu_buffers_accessor
{
	using data_t = std::vector<std::vector<uint8_t>>[2];

	virtual data_t& data() = 0;
	virtual const data_t& data() const = 0;

	virtual size_t in_size() const = 0;
	virtual size_t out_size() const = 0;
};

struct AsioCallbacks : sbLibASIO::Callbacks, cpu_buffers_accessor
{
		template< typename... Types >
	AsioCallbacks( Types... types )
		: sbLibASIO::Callbacks{ std::forward<Types>( types )... }
	{
	}
};

	template<size_t instance_index>
struct AsioCallbacks_impl : AsioCallbacks
{
	AsioCallbacks_impl( sbLibASIO::Device* _device, size_t _in_buffer_count, size_t _out_buffer_count, size_t buffer_size )
		: AsioCallbacks{
			  bufferSwitch_impl
			, sampleRateDidChange_impl
			, asioMessage_impl
			, bufferSwitchTimeInfo_impl
		}
	{
		device = _device;
		in_buffer_count = _in_buffer_count, out_buffer_count = _out_buffer_count;

		size_t buffer_count = in_buffer_count + out_buffer_count;
		data_impl[0].resize( buffer_count );
		data_impl[1].resize( buffer_count );
		for( size_t i = 0; i < buffer_count; ++i)
		{
			data_impl[0][i].resize( buffer_size );
			data_impl[1][i].resize( buffer_size );
		}
	}

	using data_t = cpu_buffers_accessor::data_t;
	virtual data_t& data() override { return data_impl; }
	virtual const data_t& data() const override { return data_impl; }

	virtual size_t in_size() const { return in_buffer_count; }
	virtual size_t out_size() const { return out_buffer_count; }

	using Bool = sbLibASIO::Bool;
	using SampleRate = sbLibASIO::SampleRate;
	using MessageSelector = sbLibASIO::MessageSelector;
	using Time = sbLibASIO::Time;

	static void bufferSwitch_impl( [[maybe_unused]] int32_t doubleBufferIndex, [[maybe_unused]] Bool directProcess )
	{
		//TODO
		__debugbreak();
	}
	static void sampleRateDidChange_impl( [[maybe_unused]] SampleRate sRate )
	{
		//TODO
		__debugbreak();
	}
	static int32_t asioMessage_impl( [[maybe_unused]] MessageSelector selector, [[maybe_unused]] int32_t value, [[maybe_unused]] void* message, [[maybe_unused]] double* opt )
	{
		//TODO
		__debugbreak();
		return -1;
	}
	static Time* bufferSwitchTimeInfo_impl( [[maybe_unused]] Time* params, [[maybe_unused]] int32_t doubleBufferIndex, [[maybe_unused]] Bool directProcess )
	{
		auto& buffers = data_impl;
		const auto copyChannelCount = std::min( in_buffer_count, out_buffer_count );
		const auto channelSize = buffers[0][0].size();
		for( int32_t channel = 0; channel < in_buffer_count; ++channel )
		{
			for( int32_t sample = 0; sample < channelSize; ++sample )
				if( buffers[(doubleBufferIndex + 0) % 2][channel][sample] != 0 )
					__debugbreak();
		}

		for( int32_t channel = 0; channel < copyChannelCount; ++channel )
			memcpy( buffers[(doubleBufferIndex + 0) % 2][channel + in_buffer_count].data(), buffers[(doubleBufferIndex + 0) % 2][channel].data(), channelSize );

		device->outputReady();
		return params;
	};

	static sbLibASIO::Device* device;
	static size_t in_buffer_count;
	static size_t out_buffer_count;
	static data_t data_impl;
};
	template<size_t instance_index>
sbLibASIO::Device* AsioCallbacks_impl<instance_index>::device{};
	template<size_t instance_index>
size_t AsioCallbacks_impl<instance_index>::in_buffer_count{};
	template<size_t instance_index>
size_t AsioCallbacks_impl<instance_index>::out_buffer_count{};
	template<size_t instance_index>
typename AsioCallbacks_impl<instance_index>::data_t AsioCallbacks_impl<instance_index>::data_impl{};

static size_t current_asio_count = 0;
std::unique_ptr<AsioCallbacks*> GenerateASIOBuffers( sbLibASIO::Device* device, size_t in_buffer_count, size_t out_buffer_count, size_t buffer_size )
{
	#define ASIO_BUFFER( n ) case n: return std::make_unique<AsioCallbacks*>( new AsioCallbacks_impl<n>( device, in_buffer_count, out_buffer_count, buffer_size ) )
	switch( current_asio_count++ )
	{
	ASIO_BUFFER( 0 );
	ASIO_BUFFER( 1 );
	ASIO_BUFFER( 2 );
	ASIO_BUFFER( 3 );
	ASIO_BUFFER( 4 );
	}
	__debugbreak();
	return std::make_unique<AsioCallbacks*>( new AsioCallbacks_impl<-1>( device, 0, 0, 0 ) );
}

#if ( SB_TARGET_TYPE == SB_TARGET_TYPE_STANDALONE )
#define asio main
#endif
SB_EXPORT_TYPE int SB_STDCALL asio( [[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[] )
{
#if SB_TARGET_TYPE == SB_TARGET_TYPE_STATIC
	[[maybe_unused]] auto appartmentThreadedResult = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
	sb_scope_exit( []() { CoUninitialize(); } );
#endif
	
	sbLibASIO::adapter_array_t adapterArray{};
	{
		// make sure adapterArray is alright passed instanceHandle being released
		auto instanceHandle = sbLibASIO::CreateInstance();
		adapterArray = EnumerateAdapters( instanceHandle );
	}
	std::vector<sbLibASIO_device> deviceArray;
	deviceArray.reserve( adapterArray.size() );
	for( auto adapter : adapterArray )
	{
		sbLibASIO::DeviceInfo deviceInfo;
		adapter->GetDeviceInfo( &deviceInfo );
		std::cout << "Testing '" << deviceInfo.description << "'\n";

		sbLibASIO_device device( adapter );
		if( device )
		{
			int32_t inputChannelCount{-1}, outputChannelCount{-1};
			auto result = device->getChannels( &inputChannelCount, &outputChannelCount );
			std::cout << "inputs:  " << inputChannelCount << "\n";
			std::cout << "outputs: " << outputChannelCount << "\n";
			if( !succeeded( result ) )
			{
				device.Release();
				std::cerr << "error: '" << deviceInfo.description << "': " << sbLibASIO::to_cstring( result ) << std::endl;
				continue;
			}

			int32_t minSize{-1}, maxSize{-1}, preferredSize{-1}, granularity{-1};
			result = device->getBufferSize( &minSize, &maxSize, &preferredSize, &granularity );
			std::cout << "bufferSize (min/max): " << minSize << " / " << maxSize << "(preferred: " << preferredSize << ")\n";
			std::cout << "granularity:          " << granularity << "\n";
			if( !succeeded( result ) )
			{
				device.Release();
				std::cerr << "error: '" << deviceInfo.description << "': " << sbLibASIO::to_cstring( result ) << std::endl;
				continue;
			}

			sbLibASIO::SampleRate sampleRate{};
			result = device->getSampleRate( &sampleRate );
			std::cout << "sample rate: "<< sampleRate << "\n";
			if( !succeeded( result ) )
			{
				device.Release();
				std::cerr << "error: '" << deviceInfo.description << "': " << sbLibASIO::to_cstring( result ) << std::endl;
				continue;
			}

			std::unique_ptr<AsioCallbacks*> callbacks = GenerateASIOBuffers( device.handle.Get(), inputChannelCount, outputChannelCount, preferredSize );

			if( preferredSize > 0 )
			{
				auto& buffers = (*callbacks)->data();
				info.reserve( inputChannelCount + outputChannelCount );

				for( int32_t channel = 0; channel < inputChannelCount; ++channel )
				{
					info.emplace_back( sbLibASIO::BufferInfo{
						/*StreamDirection*/  sbLibASIO::StreamDirection::Input,
						/*int32_t        */  channel,
						/*void*          */ { buffers[0][channel].data(), buffers[1][channel].data() },
					} );
				}
				for( int32_t channel = 0; channel < outputChannelCount; ++channel )
				{
					info.emplace_back( sbLibASIO::BufferInfo{
						/*StreamDirection*/  sbLibASIO::StreamDirection::Output,
						/*int32_t        */  channel,
						/*void*          */ { buffers[0][channel].data(), buffers[1][inputChannelCount + channel].data() },
					} );
				}
			}

			__debugbreak(); // d'oh ! should use buffers from the ASIO driver before the explicit buffers for private use!
			result = device->createBuffers( info.data(), inputChannelCount + outputChannelCount, preferredSize, *callbacks );
			if( !succeeded( result ) )
			{
				device.Release();
				std::cerr << "error: '" << deviceInfo.description << "' failed to create buffers: " << sbLibASIO::to_cstring( result ) << std::endl;
				continue;
			}

			std::cout << "ASIO driver '" << deviceInfo.description << "' is available." << std::endl;
			result = device->start();
			if( !succeeded( result ) )
			{
				device.Release();
				std::cerr << "error: '" << deviceInfo.description << "' failed to start: "<< sbLibASIO::to_cstring( result ) << std::endl;
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
			std::cout << "ASIO driver '" << deviceInfo.description << "': " << sbLibASIO::to_cstring( sbLibASIO::ErrorType::NotPresent ) << std::endl;
		}
		deviceArray.emplace_back( std::move( device ) );
	}
	
	std::cin.get();

	for( auto& device : deviceArray )
	{
		if( device )
		{
			device->stop();
			device->disposeBuffers();
		}
	}

	deviceArray.clear();
	return 0;
}

#endif
