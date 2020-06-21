#pragma once

#include <cstdint>
namespace SB { namespace LibX {
namespace ASIO
{
#if defined( SBWIN32 ) || defined( SBWIN64 )
// This is binary-compatible with the COM interface, without requiring anything under "windows.h" and compiler's limitations involved
struct IUnknown
{
	using HRESULT = long;
	using ULONG = unsigned long;
	union GUID { struct CLSID { uint32_t data1; uint16_t data2; uint16_t data3; uint8_t data4[8]; }; struct UID { uint64_t dataHi; uint64_t dataLow; }; uint8_t data[16]; };
	virtual HRESULT __stdcall QueryInterface(GUID riid, void** ppvObject) = 0;
	virtual ULONG __stdcall AddRef() = 0;
	virtual ULONG __stdcall Release() = 0;
};
#endif

/////////////////////////////////////////////////////////
// Please refer to official Steinberg ASIO documentation
//-------------------------------------------------------
//
//	Steinberg Audio Streaming Input Output Specification
//	Development Kit 2.3
//
//	Steinberg Audio Stream I / O API
//	(c) 1997 - 2019, Steinberg Media Technologies GmbH
//
//	ASIO Interface Specification v 2.3
//

#define SB_ASIO_ERROR( code, value, name ) code = value,
enum class ErrorType : long
{
	#include <common/include/internal/interface_asio_error.hpp>
};
#define SB_ASIO_ERROR( code, value, name ) case ErrorType::code: return name;
constexpr static inline const char* to_cstring( ErrorType error )
{
	switch (error)
	{
	#include <common/include/internal/interface_asio_error.hpp>
	default: return "unknown";
	}
}
#define SB_ASIO_ERROR( code, value, name ) case ErrorType::code: return L ## name;
constexpr static inline const wchar_t* to_wstring( ErrorType error )
{
	switch (error)
	{
	#include <common/include/internal/interface_asio_error.hpp>
	default: return L"unknown";
	}
}
constexpr static inline bool succeeded( ErrorType code )
{
	return static_cast<long>(code) >= 0;
}

enum Bool : long
{
	False = 0,
	True = 1
};
enum class SampleType : long
{
	Int16_MSB = 0,
	Int24_MSB = 1,  		// used for 20 bits as well
	Int32_MSB = 2,
	Float32_MSB = 3,		// IEEE 754 32 bit float
	Float64_MSB = 4,		// IEEE 754 64 bit double float

	// these are used for 32 bit data buffer, with different alignment of the data inside
	// 32 bit PCI bus systems can be more easily used with these
	Int32_MSB16 = 8, 		// 32 bit data with 16 bit alignment
	Int32_MSB18 = 9, 		// 32 bit data with 18 bit alignment
	Int32_MSB20 = 10,		// 32 bit data with 20 bit alignment
	Int32_MSB24 = 11,		// 32 bit data with 24 bit alignment

	Int16_LSB = 16,
	Int24_LSB = 17, 		// used for 20 bits as well
	Int32_LSB = 18,
	Float32_LSB = 19,		// IEEE 754 32 bit float, as found on Intel x86 architecture
	Float64_LSB = 20, 		// IEEE 754 64 bit double float, as found on Intel x86 architecture

	// these are used for 32 bit data buffer, with different alignment of the data inside
	// 32 bit PCI bus systems can more easily used with these
	Int32_LSB16 = 24,		// 32 bit data with 18 bit alignment
	Int32_LSB18 = 25,		// 32 bit data with 18 bit alignment
	Int32_LSB20 = 26,		// 32 bit data with 20 bit alignment
	Int32_LSB24 = 27,		// 32 bit data with 24 bit alignment

	//	ASIO DSD format.
	DSD_Int8_LSB1 = 32,		// DSD 1 bit data, 8 samples per byte. First sample in Least significant bit.
	DSD_Int8_MSB1 = 33,		// DSD 1 bit data, 8 samples per byte. First sample in Most significant bit.
	DSD_Int8_NER8 = 40,		// DSD 8 bit data, 1 sample per byte. No Endianness required.
};

enum class Future : long
{
	EnableTimeCodeRead = 1,	// no arguments
	DisableTimeCodeRead,		// no arguments
	SetInputMonitor,			// InputMonitor* in params
	Transport,					// TransportParameters* in params
	SetInputGain,				// ChannelControls* in params, apply gain
	GetInputMeter,				// ChannelControls* in params, fill meter
	SetOutputGain,				// ChannelControls* in params, apply gain
	GetOutputMeter,			// ChannelControls* in params, fill meter
	CanInputMonitor,			// no arguments for CanXXX selectors
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
	SetIoFormat = 0x23111961,		/* ASIOIoFormat * in params.			*/
	GetIoFormat = 0x23111983,		/* ASIOIoFormat * in params.			*/
	CanDoIoFormat = 0x23112004,		/* ASIOIoFormat * in params.			*/

	// Extension for drop out detection
	CanReportOverload = 0x24042012,	/* return ASE_SUCCESS if driver can detect and report overloads */

	GetInternalBufferSamples = 0x25042012	/* ASIOInternalBufferInfo * in params. Deliver size of driver internal buffering, return ASE_SUCCESS if supported */
};

using SampleRate = double;
using Samples = int64_t;
using TimeStamp = int64_t;

struct TimeInfo
{
	double          speed;                  // absolute speed (1. = nominal)
	TimeStamp   systemTime;             // system time related to samplePosition, in nanoseconds
											// on mac, must be derived from Microseconds() (not UpTime()!)
											// on windows, must be derived from timeGetTime()
	Samples     samplePosition;
	SampleRate  sampleRate;             // current rate
	unsigned long flags;                    // (see below)
	char reserved[12];
};
struct TimeCode
{
	double          speed;                  // speed relation (fraction of nominal speed)
											// optional; set to 0. or 1. if not supported
	Samples     timeCodeSamples;        // time in samples
	unsigned long   flags;                  // some information flags (see below)
	char future[64];
};
struct Time                          // both input/output
{
	long reserved[4];                       // must be 0
	TimeInfo     timeInfo;       // required
	TimeCode     timeCode;       // optional, evaluated if (timeCode.flags & kTcValid)
};
struct ClockSource
{
	long index;					// as used for SetClockSource()
	long associatedChannel;		// for instance, S/PDIF or AES/EBU
	long associatedGroup;		// see channel groups (GetChannelInfo())
	Bool isCurrentSource;	// True if this is the current clock source
	char name[32];				// for user selection
};
struct BufferInfo
{
	Bool isInput;			// on input:  True: input, else output
	long channelNum;			// on input:  channel index
	void* buffers[2];			// on output: double buffer addresses
};
struct ChannelInfo
{
	long channel;			// on input, channel index
	Bool isInput;		// on input
	Bool isActive;		// on exit
	long channelGroup;		// dto
	SampleType type;	// dto
	char name[32];			// dto
};
struct Callbacks
{
	void (*bufferSwitch) (long doubleBufferIndex, Bool directProcess);
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

	long (*asioMessage) (long selector, long value, void* message, double* opt);
	// generic callback for various purposes, see selectors below.
	// note this is only present if the asio version is 2 or higher

	Time* (*bufferSwitchTimeInfo) (Time* params, long doubleBufferIndex, Bool directProcess);
	// new callback with time info. makes GetSamplePosition() and various
	// calls to GetSampleRate obsolete,
	// and allows for timecode sync etc. to be preferred; will be used if
	// the driver calls asioMessage with selector Future::SupportsTimeInfo.
};

struct Device : public IUnknown
{
	virtual Bool init(void* sysHandle) = 0;
	virtual void getDriverName(char* name) = 0;
	virtual long getDriverVersion() = 0;
	virtual void getErrorMessage(char* string) = 0;
	virtual ErrorType start() = 0;
	virtual ErrorType stop() = 0;
	virtual ErrorType getChannels(long* numInputChannels, long* numOutputChannels) = 0;
	virtual ErrorType getLatencies(long* inputLatency, long* outputLatency) = 0;
	virtual ErrorType getBufferSize(long* minSize, long* maxSize, long* preferredSize, long* granularity) = 0;
	virtual ErrorType canSampleRate(SampleRate sampleRate) = 0;
	virtual ErrorType getSampleRate(SampleRate* sampleRate) = 0;
	virtual ErrorType setSampleRate(SampleRate sampleRate) = 0;
	virtual ErrorType getClockSources(ClockSource* clocks, long* numSources) = 0;
	virtual ErrorType setClockSource(long reference) = 0;
	virtual ErrorType getSamplePosition(Samples* sPos, TimeStamp* tStamp) = 0;
	virtual ErrorType getChannelInfo(ChannelInfo* info) = 0;
	virtual ErrorType createBuffers(BufferInfo* bufferInfos, long numChannels, long bufferSize, Callbacks* callbacks) = 0;
	virtual ErrorType disposeBuffers() = 0;
	virtual ErrorType controlPanel() = 0;
	virtual ErrorType future(Future selector, void* opt) = 0;
	virtual ErrorType outputReady() = 0;
};

}
}}