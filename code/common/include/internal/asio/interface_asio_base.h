#pragma once
#include <common/include/sb_common.h>
#include <common/include/internal/ref_ptr.h>

#include <cstdint>
namespace SB { namespace LibX {
namespace ASIO
{
/////////////////////////////////////////////////////////
// Please refer to official Steinberg ASIO documentation
/////////////////////////////////////////////////////////

enum class ErrorType : int32_t
{
	#define SB_ASIO_ERROR( code, value, name ) \
		code = value,
	#include <common/include/internal/asio/interface_asio_error.hpp>
};
constexpr static inline const char* to_cstring( ErrorType error )
{
	switch (error)
	{
	#define SB_ASIO_ERROR( code, value, name ) \
		case ErrorType::code: return name;
	#include <common/include/internal/asio/interface_asio_error.hpp>
	default: return "unknown";
	}
}
constexpr static inline const wchar_t* to_wstring( ErrorType error )
{
	switch (error)
	{
	#define SB_ASIO_ERROR( code, value, name ) \
		case ErrorType::code: return L ## name;
	#include <common/include/internal/asio/interface_asio_error.hpp>
	default: return L"unknown";
	}
}
constexpr static inline bool succeeded( ErrorType code )
{
	return static_cast<int32_t>(code) >= 0;
}

enum Bool : int32_t
{
	False = 0,
	True = 1
};
enum class SampleType : int32_t
{
	#define SB_ASIO_SAMPLETYPE( name, index, type_name, bit_per_pack, bit_per_sample, sample_per_pack, endian, align_bits, container_type ) \
		name = index,
	#include <common/include/internal/asio/interface_asio_sampletype.hpp>
};


using Samples = int64_t;
using TimeStamp = int64_t;
using SampleRate = double;

struct TimeInfo
{
	enum Flags : uint32_t
	{
		SystemTimeValid     = ( 1 << 0 ),	// must always be valid
		SamplePositionValid = ( 1 << 1 ),	// must always be valid
		SampleRateValid     = ( 1 << 2 ),
		SpeedValid          = ( 1 << 3 ),

		SampleRateChanged   = ( 1 << 4 ),
		ClockSourceChanged  = ( 1 << 5 ),
	};

	double     speed;       	// absolute speed (1. = nominal)
	TimeStamp  systemTime;  	// system time related to samplePosition, in nanoseconds
	                        	// on mac, must be derived from Microseconds() (not UpTime()!)
	                        	// on windows, must be derived from timeGetTime()
	Samples    samplePosition;
	SampleRate sampleRate;    	// current rate
	Flags      flags;         	// (see below)
	uint8_t    reserved[12];
};
struct TimeCode
{
	enum Flags : uint32_t
	{
		Valid      = ( 1 << 0 ),
		Running    = ( 1 << 1 ),
		Reverse    = ( 1 << 2 ),
		Onspeed    = ( 1 << 3 ),
		Still      = ( 1 << 4 ),

		SpeedValid = ( 1 << 8 ),
	};

	double  speed;          	// speed relation (fraction of nominal speed)
	                        	// optional; set to 0. or 1. if not supported
	Samples timeCodeSamples;	// time in samples
	Flags   flags;          	// some information flags (see below)
	uint8_t future[64];
};
struct Time // both input/output
{
	int32_t  reserved[4];	// must be 0
	TimeInfo timeInfo;   	// required
	TimeCode timeCode;   	// optional, evaluated if (timeCode.flags & kTcValid)
};
struct ClockSource
{
	int32_t index;             	// as used for SetClockSource()
	int32_t associatedChannel; 	// for instance, S/PDIF or AES/EBU
	int32_t associatedGroup;   	// see channel groups (GetChannelInfo())
	Bool    isCurrentSource;   	// True if this is the current clock source
	char    name[32];          	// for user selection
};
enum class StreamDirection : int32_t
{
	Output,
	Input,
};
struct ChannelInfo
{
	int32_t          channel;     	// on input, channel index
	StreamDirection  direction;   	// 1 on input
	Bool             isActive;    	// on exit
	int32_t          channelGroup;	// dto
	SampleType       type;        	// dto
	char             name[32];    	// dto
};
struct BufferInfo
{
	StreamDirection  direction;  	// 1 on input
	int32_t          channelNum;	// on input:  channel index
	void*            buffers[2];	// on output: double buffer addresses
};


enum class MessageSelector : int32_t
{
	SelectorSupported = 1, 	// selector in <value>, returns 1L if supported,
	                       	// 0 otherwise
    EngineVersion,         	// returns engine (host) asio implementation version,
	                       	// 2 or higher
	ResetRequest,          	// request driver reset. if accepted, this
	                       	// will close the driver (ASIO_Exit() ) and
	                       	// re-open it again (ASIO_Init() etc). some
	                       	// drivers need to reconfigure for instance
	                       	// when the sample rate changes, or some basic
	                       	// changes have been made in ASIO_ControlPanel().
	                       	// returns 1L; note the request is merely passed
	                       	// to the application, there is no way to determine
	                       	// if it gets accepted at this time (but it usually
	                       	// will be).
	BufferSizeChange,      	// not yet supported, will currently always return 0L.
	                       	// for now, use kAsioResetRequest instead.
	                       	// once implemented, the new buffer size is expected
	                       	// in <value>, and on success returns 1L
	ResyncRequest,         	// the driver went out of sync, such that
	                       	// the timestamp is no longer valid. this
	                       	// is a request to re-start the engine and
	                       	// slave devices (sequencer). returns 1 for ok,
	                       	// 0 if not supported.
	LatenciesChanged,      	// the drivers latencies have changed. The engine
	                       	// will refetch the latencies.
	SupportsTimeInfo,      	// if host returns true here, it will expect the
	                       	// callback bufferSwitchTimeInfo to be called instead
	                       	// of bufferSwitch
	SupportsTimeCode,      	// 
	MMCCommand,            	// unused - value: number of commands, message points to mmc commands
	SupportsInputMonitor,  	// kAsioSupportsXXX return 1 if host supports this
	SupportsInputGain,     	// unused and undefined
	SupportsInputMeter,    	// unused and undefined
	SupportsOutputGain,    	// unused and undefined
	SupportsOutputMeter,   	// unused and undefined
	Overload,              	// driver detected an overload

	NumMessageSelectors
};

enum class CommandType : int32_t
{
	Start = 1,
	Stop,
	Locate,    	// to samplePosition
	PunchIn,
	PunchOut,
	ArmOn,     	// track
	ArmOff,    	// track
	MonitorOn, 	// track
	MonitorOff,	// track
	Arm,       	// trackSwitches
	Monitor,   	// trackSwitches
};


	template<MessageSelector __selector_value__>
struct message_traits
{
	using type_t = void;
};
template<> struct message_traits<MessageSelector::SelectorSupported   > { using type_t = Bool;        };
template<> struct message_traits<MessageSelector::EngineVersion       > { using type_t = int32_t;     };	// engine (host) asio implementation version (2 or higher)
template<> struct message_traits<MessageSelector::ResetRequest        > { using type_t = Bool;        };
template<> struct message_traits<MessageSelector::BufferSizeChange    > { using type_t = Bool;        };	// not yet supported, use ResetRequest
template<> struct message_traits<MessageSelector::ResyncRequest       > { using type_t = Bool;        };
template<> struct message_traits<MessageSelector::LatenciesChanged    > { using type_t = void;        };
template<> struct message_traits<MessageSelector::SupportsTimeInfo    > { using type_t = Bool;        };
template<> struct message_traits<MessageSelector::SupportsTimeCode    > { using type_t = Bool;        };
template<> struct message_traits<MessageSelector::MMCCommand          > { using type_t = CommandType; };	// unused - value: number of commands, message points to mmc commands
template<> struct message_traits<MessageSelector::SupportsInputMonitor> { using type_t = Bool;        };
template<> struct message_traits<MessageSelector::SupportsInputGain   > { using type_t = Bool;        };
template<> struct message_traits<MessageSelector::SupportsInputMeter  > { using type_t = Bool;        };
template<> struct message_traits<MessageSelector::SupportsOutputGain  > { using type_t = Bool;        };
template<> struct message_traits<MessageSelector::SupportsOutputMeter > { using type_t = Bool;        };
template<> struct message_traits<MessageSelector::Overload            > { using type_t = Bool;        };


struct Callbacks
{
	void (*bufferSwitch) (int32_t doubleBufferIndex, Bool directProcess);
	// bufferSwitch indicates that both input and output are to be processed.
	// the current buffer half index (0 for A, 1 for B) determines
	// - the output buffer that the host should start to fill. the other buffer
	//   will be passed to output hardware regardless of whether it got filled
	//   in time or not.
	// - the input buffer that is now filled with incoming data. Note that
	//   because of the synchronicity of i/o, the input always has at
	//   least one buffer latency in relation to the output.
	// directProcess suggests to the host whether it should immedeately
	// start processing (directProcess == True), or whether its process
	// should be deferred because the call comes from a very low level
	// (for instance, a high level priority interrupt), and direct processing
	// would cause timing instabilities for the rest of the system. If in doubt,
	// directProcess should be set to False.
	// Note: bufferSwitch may be called at interrupt time for highest efficiency.

	void (*sampleRateDidChange) (SampleRate sRate);
	// gets called when the AudioStreamIO detects a sample rate change
	// If sample rate is unknown, 0 is passed (for instance, clock loss
	// when externally synchronized).

	int32_t (*asioMessage) (MessageSelector selector, int32_t value, void* message, double* opt);
	// generic callback for various purposes, see selectors below.
	// note this is only present if the asio version is 2 or higher

	Time* (*bufferSwitchTimeInfo) (Time* params, int32_t doubleBufferIndex, Bool directProcess);
	// new callback with time info. makes GetSamplePosition() and various
	// calls to GetSampleRate obsolete,
	// and allows for timecode sync etc. to be preferred; will be used if
	// the driver calls asioMessage with selector Future::SupportsTimeInfo.

		template<MessageSelector selector>
	ErrorType message(int32_t value, typename message_traits<selector>::type_t* message = nullptr, double* opt = nullptr)
	{
		return asioMessage(selector, value, message, opt);
	}
};


enum class Future : int32_t
{
	EnableTimeCodeRead = 1,	// no arguments
	DisableTimeCodeRead,   	// no arguments
	SetInputMonitor,       	// InputMonitor* in params
	Transport,             	// TransportParameters* in params
	SetInputGain,          	// ChannelControls* in params, apply gain
	GetInputMeter,         	// ChannelControls* in params, fill meter
	SetOutputGain,         	// ChannelControls* in params, apply gain
	GetOutputMeter,        	// ChannelControls* in params, fill meter
	CanInputMonitor,       	// no arguments for CanXXX selectors
	CanTimeInfo,
	CanTimeCode,
	CanTransport,
	CanInputGain,
	CanInputMeter,
	CanOutputGain,
	CanOutputMeter,
	OptionalOne,

	//	DSD support
	//	The following extensions are required to allow switching
	//	and control of the DSD subsystem.
	SetIoFormat   = 0x23111961,		// IoFormat* in params
	GetIoFormat   = 0x23111983,		// IoFormat* in params
	CanDoIoFormat = 0x23112004,		// IoFormat* in params

	// Extension for drop out detection
	CanReportOverload        = 0x24042012,	/* return ASE_SUCCESS if driver can detect and report overloads */

	GetInternalBufferSamples = 0x25042012,	/* InternalBufferInfo * in params. Deliver size of driver internal buffering, return ASE_SUCCESS if supported */
};

struct InputMonitor
{
	int32_t  input; 	// this input was set to monitor (or off), -1: all
	int32_t  output;	// suggested output for monitoring the input (if so)
	int32_t  gain;  	// suggested gain, ranging 0 - 0x7fffffffL (-inf to +12 dB)
	Bool     state; 	// ASIOTrue => on, ASIOFalse => off
	int32_t  pan;   	// suggested pan, 0 => all left, 0x7fffffff => right
};

struct ChannelControls
{
	int32_t channel;    	// on input, channel index
	Bool    isInput;    	// on input
	int32_t gain;       	// on input,  ranges 0 thru 0x7fffffff
	int32_t meter;      	// on return, ranges 0 thru 0x7fffffff
	uint8_t future[32];
};

struct TransportParameters
{
	CommandType command;          	// see enum below
	Samples     samplePosition;
	int32_t     track;
	int32_t     trackSwitches[16];	// 512 tracks on/off
	uint8_t     future[64];
};

struct IoFormat
{
	enum IoFormatType :int32_t
	{
		kInvalidFormat = -1,
		kPCMFormat     = 0,
		kDSDFormat     = 1,
	};

	IoFormatType formatType;
	uint8_t      future[512 - sizeof(IoFormatType)];
};
static_assert( sizeof(IoFormat) == 512 );

// Extension for drop detection
// Note: Refers to buffering that goes beyond the double buffer e.g. used by USB driver designs
struct InternalBufferInfo
{
	int32_t inputSamples; 	// size of driver's internal input buffering which is included in getLatencies
	int32_t outputSamples;	// size of driver's internal output buffering which is included in getLatencies
};

	template<Future __selector_value__>
struct future_traits
{
	using type_t = void;
};
template<> struct future_traits<Future::SetInputMonitor         > { using type_t = InputMonitor;        };
template<> struct future_traits<Future::Transport               > { using type_t = TransportParameters; };
template<> struct future_traits<Future::SetInputGain            > { using type_t = ChannelControls;     };
template<> struct future_traits<Future::GetInputMeter           > { using type_t = ChannelControls;     };
template<> struct future_traits<Future::SetOutputGain           > { using type_t = ChannelControls;     };
template<> struct future_traits<Future::GetOutputMeter          > { using type_t = ChannelControls;     };
template<> struct future_traits<Future::SetIoFormat	            > { using type_t = IoFormat;            };
template<> struct future_traits<Future::GetIoFormat	            > { using type_t = IoFormat;            };
template<> struct future_traits<Future::CanDoIoFormat           > { using type_t = IoFormat;            };
template<> struct future_traits<Future::GetInternalBufferSamples> { using type_t = InternalBufferInfo;  };

struct Device : public IUnknown
{
	virtual Bool init(void* sysHandle) = 0;
	virtual void getDriverName(char ( &name )[32]) = 0;
	virtual int32_t getDriverVersion() = 0;
	virtual void getErrorMessage(char ( &string )[124]) = 0;
	virtual ErrorType start() = 0;
	virtual ErrorType stop() = 0;
	virtual ErrorType getChannels(int32_t* numInputChannels, int32_t* numOutputChannels) = 0;
	virtual ErrorType getLatencies(int32_t* inputLatency, int32_t* outputLatency) = 0;
	virtual ErrorType getBufferSize(int32_t* minSize, int32_t* maxSize, int32_t* preferredSize, int32_t* granularity) = 0;
	virtual ErrorType canSampleRate(SampleRate sampleRate) = 0;
	virtual ErrorType getSampleRate(SampleRate* sampleRate) = 0;
	virtual ErrorType setSampleRate(SampleRate sampleRate) = 0;
	virtual ErrorType getClockSources(ClockSource* clocks, int32_t* numSources) = 0;
	virtual ErrorType setClockSource(int32_t reference) = 0;
	virtual ErrorType getSamplePosition(Samples* sPos, TimeStamp* tStamp) = 0;
	virtual ErrorType getChannelInfo(ChannelInfo* info) = 0;
	virtual ErrorType createBuffers(BufferInfo* bufferInfos, int32_t numChannels, int32_t bufferSize, Callbacks* callbacks) = 0;
	virtual ErrorType disposeBuffers() = 0;
	virtual ErrorType controlPanel() = 0;
	virtual ErrorType future( Future selector, void* opt = nullptr ) = 0;
	virtual ErrorType outputReady() = 0;

		template<Future selector>
	ErrorType future( typename future_traits<selector>::type_t* opt = nullptr )
	{
		return future( selector, opt );
	}
};

}
}}