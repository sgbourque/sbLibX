#pragma once

#include <string>

#include "Windows.h"
#include <wrl/client.h>
#include <combaseapi.h>

namespace SB
{
namespace LibX
{
namespace Windows
{

static constexpr wchar_t COM_CLSID[] = L"clsid";
using CLSID = GUID;

static inline bool operator ==(const CLSID& clsidA, const CLSID& clsidB)
{
	return memcmp(&clsidA, &clsidB, sizeof(clsidA)) == 0;
}
struct CLSIDHaser
{
	inline size_t operator ()(const CLSID& clsid) const
	{
		static_assert(sizeof(CLSID) == 2 * sizeof(size_t), "");
		return *reinterpret_cast<const size_t*>(&clsid);
	}
};


class RegistryKey
{
public:
	enum AccessMask : DWORD
	{
		Query = KEY_QUERY_VALUE,
		Read = KEY_READ,
		Enumerate = KEY_ENUMERATE_SUB_KEYS,
	};
	enum KeyType : DWORD
	{
		KeyType_NONE                    = ( 0ul ), // No value type
		KeyType_SZ                      = ( 1ul ), // Unicode nul terminated string
		KeyType_EXPAND_SZ               = ( 2ul ), // Unicode nul terminated string
		// (with environment variable references)
		KeyType_BINARY                  = ( 3ul ), // Free form binary
		KeyType_DWORD                   = ( 4ul ), // 32-bit number
		KeyType_DWORD_LITTLE_ENDIAN     = ( 4ul ), // 32-bit number (same as REG_DWORD)
		KeyType_DWORD_BIG_ENDIAN        = ( 5ul ), // 32-bit number
		KeyType_LINK                    = ( 6ul ), // Symbolic Link (unicode)
		KeyType_MULTI_SZ                = ( 7ul ), // Multiple Unicode strings
		KeyType_RESOURCE_LIST           = ( 8ul ), // Resource list in the resource map
		KeyType_FULL_RESOURCE_DESCRIPTOR = ( 9ul ), // Resource list in the hardware description
		KeyType_RESOURCE_REQUIREMENTS_LIST = ( 10ul ),
		KeyType_QWORD                   = ( 11ul ), // 64-bit number
		KeyType_QWORD_LITTLE_ENDIAN     = ( 11ul ), // 64-bit number (same as REG_QWORD)
	};

	RegistryKey(const RegistryKey& key) = delete;
	RegistryKey(RegistryKey&& key) = default;

	RegistryKey(HKEY parentKey, AccessMask access, const wchar_t* keyname = nullptr);
	RegistryKey(const RegistryKey& parentKey, AccessMask access, const wchar_t* keyname = nullptr);

	~RegistryKey();

	operator bool() const { return hkey; }
	operator HKEY() const { return hkey; }

	template<typename type = std::wstring, KeyType regDataTypeId = KeyType_SZ>
	type getValue( const wchar_t* keyname = nullptr, KeyType dataType = KeyType_SZ )
	{
		DWORD maxDataSize = getValue_internal( keyname, static_cast<DWORD>(dataType) );

		type value;
		value.resize((maxDataSize + sizeof(wchar_t) - 1) / sizeof(wchar_t));
		if( maxDataSize > 0 )
		{
			DWORD dataSize = getValue_internal( keyname, static_cast<DWORD>(dataType), value.data(), maxDataSize );
			value.resize((dataSize + sizeof(wchar_t) - 1) / sizeof(wchar_t));
		}
		return value;
	}

private:
	DWORD getValue_internal( const wchar_t* keyname, DWORD dataType, void* data = nullptr, DWORD dataSize = 0u );

	HKEY hkey;
};
static inline constexpr RegistryKey::AccessMask operator|(RegistryKey::AccessMask a, RegistryKey::AccessMask b) { return static_cast<RegistryKey::AccessMask>(static_cast<DWORD>(a) | static_cast<DWORD>(b)); }

class RegistryKeyIterator
{
public:
	RegistryKeyIterator(HKEY key);

	std::wstring operator *() const;

	RegistryKeyIterator& operator ++()
	{
		if (index >= 0)
			++index;
		return *this;
	}
	const RegistryKeyIterator& operator ++() const
	{
		if (index >= 0)
			++index;
		return *this;
	}
	bool operator ==(const RegistryKeyIterator& other) const
	{
		return hkey == other.hkey && index == other.index;
	}
	bool operator !=(const RegistryKeyIterator& other) const
	{
		return hkey != other.hkey || index != other.index;
	}

	static inline RegistryKeyIterator end(HKEY key)
	{
		return RegistryKeyIterator(key, -1, 0);
	}
private:
	explicit RegistryKeyIterator(HKEY key, int index, DWORD maxNameCharCount)
		: hkey(key), index(index), maxNameCharCount(maxNameCharCount)
	{
	}

	HKEY hkey;
	DWORD maxNameCharCount = 0;
	mutable int index = 0;
};

static inline RegistryKeyIterator begin(const HKEY hkey)
{
	return RegistryKeyIterator(hkey);
}
static inline RegistryKeyIterator end(const HKEY hkey)
{
	return RegistryKeyIterator::end(hkey);
}


}
}

}
namespace SBWindows = SB::LibX::Windows;
namespace SBWin32 = SBWindows;

//#include <vector>
//#include <string>
//
//#include "Windows.h"
//#include <wrl/client.h>
//#include <combaseapi.h>
//
//namespace SB
//{
//namespace Audio
//{
//template<typename Type>
//using com_ptr = Microsoft::WRL::ComPtr<Type>;
//
///////////////////////////////////////////////////////////
//// Please refer to official Steinberg ASIO documentation
////-------------------------------------------------------
////
////	Steinberg Audio Streaming Input Output Specification
////	Development Kit 2.3
////
////	Steinberg Audio Stream I / O API
////	(c) 1997 - 2019, Steinberg Media Technologies GmbH
////
////	ASIO Interface Specification v 2.3
////
//struct AsioDeviceInfo;
//std::vector<AsioDeviceInfo> EnumerateASIODevices();
//
//enum class AsioDriverResult
//{
//	Error_Failed = -2,
//	Error_Unitialized = -1,
//
//	Success = 0,
//	AlreadyExists = 1,
//};
//struct AsioDeviceInfo
//{
//	struct CLSID
//	{
//		unsigned long  Data1;
//		unsigned short Data2;
//		unsigned short Data3;
//		unsigned char  Data4[8];
//	};
//
//	CLSID			classID;
//	std::wstring	path;
//	std::wstring	name;
//
//	operator bool() const
//	{
//		return !path.empty();
//	}
//};
//AsioDriverResult CreateAsioDriver( const AsioDeviceInfo::CLSID& deviceId );
//AsioDriverResult ReleaseAsioDriver( const AsioDeviceInfo::CLSID& deviceId );
//
//void ASIOInitialize();
//void ASIOShutdown();
//
//struct IASIO;
//com_ptr<IASIO> QueryInterface( const AsioDeviceInfo& device );
//
//
//#define SB_ASIO_ERROR( code, value, name ) code = value,
//enum class ASIOError : long
//{
//	SB_ASIO_ERROR(OK, 0, "This value will be returned whenever the call succeeded")
//	SB_ASIO_ERROR(SuccessFuture, 0x3f4847a0, "unique success return value for ASIOFuture calls")
//	SB_ASIO_ERROR(NotPresent, -1000 + 0, "hardware input or output is not present or available")
//	SB_ASIO_ERROR(HWMalfunction, -1000 + 1, "hardware is malfunctioning (can be returned by any ASIO function)")
//	SB_ASIO_ERROR(InvalidParameter, -1000 + 2, "input parameter invalid")
//	SB_ASIO_ERROR(InvalidMode, -1000 + 3, "hardware is in a bad mode or used in a bad mode")
//	SB_ASIO_ERROR(SPNotAdvancing, -1000 + 4, "hardware is not running when sample position is inquired")
//	SB_ASIO_ERROR(NoClock, -1000 + 5, "sample clock or rate cannot be determined or is not present")
//	SB_ASIO_ERROR(NoMemory, -1000 + 6, "not enough memory for completing the request")
//};
//#undef SB_ASIO_ERROR
//inline std::wstring GetASIOErrorString(ASIOError error)
//{
//	switch (error)
//	{
//	case ASIOError::OK:               return L"no error";                      // = 0,				// This value will be returned whenever the call succeeded
//	case ASIOError::SuccessFuture:    return L"success (future)";              // = 0x3f4847a0,	// unique success return value for ASIOFuture calls
//	case ASIOError::NotPresent:       return L"not present";                   // = -1000,			// hardware input or output is not present or available
//	case ASIOError::HWMalfunction:    return L"hardware malfunction";          //,					// hardware is malfunctioning (can be returned by any ASIO function)
//	case ASIOError::InvalidParameter: return L"invalid parameter";             //,					// input parameter invalid
//	case ASIOError::InvalidMode:      return L"invalid mode";                  //,					// hardware is in a bad mode or used in a bad mode
//	case ASIOError::SPNotAdvancing:   return L"sample position not advancing"; //,					// hardware is not running when sample position is inquired
//	case ASIOError::NoClock:          return L"no clock";                      //,					// sample clock or rate cannot be determined or is not present
//	case ASIOError::NoMemory:         return L"no memory";                     //,					// not enough memory for completing the request
//	default:                          return L"unknown";
//	}
//}
//
//constexpr static inline bool succeeded( ASIOError code )
//{
//	return static_cast<long>(code) >= 0;
//}
//
//
//enum class ASIOBool : long
//{
//	False = 0,
//	True = 1
//};
//
//enum class ASIOSampleType : long
//{
//	Int16_MSB = 0,
//	Int24_MSB = 1,  		// used for 20 bits as well
//	Int32_MSB = 2,
//	Float32_MSB = 3,		// IEEE 754 32 bit float
//	Float64_MSB = 4,		// IEEE 754 64 bit double float
//
//	// these are used for 32 bit data buffer, with different alignment of the data inside
//	// 32 bit PCI bus systems can be more easily used with these
//	Int32_MSB16 = 8, 		// 32 bit data with 16 bit alignment
//	Int32_MSB18 = 9, 		// 32 bit data with 18 bit alignment
//	Int32_MSB20 = 10,		// 32 bit data with 20 bit alignment
//	Int32_MSB24 = 11,		// 32 bit data with 24 bit alignment
//
//	Int16_LSB = 16,
//	Int24_LSB = 17, 		// used for 20 bits as well
//	Int32_LSB = 18,
//	Float32_LSB = 19,		// IEEE 754 32 bit float, as found on Intel x86 architecture
//	Float64_LSB = 20, 		// IEEE 754 64 bit double float, as found on Intel x86 architecture
//
//	// these are used for 32 bit data buffer, with different alignment of the data inside
//	// 32 bit PCI bus systems can more easily used with these
//	Int32_LSB16 = 24,		// 32 bit data with 18 bit alignment
//	Int32_LSB18 = 25,		// 32 bit data with 18 bit alignment
//	Int32_LSB20 = 26,		// 32 bit data with 20 bit alignment
//	Int32_LSB24 = 27,		// 32 bit data with 24 bit alignment
//
//	//	ASIO DSD format.
//	DSD_Int8_LSB1 = 32,		// DSD 1 bit data, 8 samples per byte. First sample in Least significant bit.
//	DSD_Int8_MSB1 = 33,		// DSD 1 bit data, 8 samples per byte. First sample in Most significant bit.
//	DSD_Int8_NER8 = 40,		// DSD 8 bit data, 1 sample per byte. No Endianness required.
//};
//
//enum class ASIOFuture : long
//{
//	EnableTimeCodeRead = 1,	// no arguments
//	DisableTimeCodeRead,		// no arguments
//	SetInputMonitor,			// ASIOInputMonitor* in params
//	Transport,					// ASIOTransportParameters* in params
//	SetInputGain,				// ASIOChannelControls* in params, apply gain
//	GetInputMeter,				// ASIOChannelControls* in params, fill meter
//	SetOutputGain,				// ASIOChannelControls* in params, apply gain
//	GetOutputMeter,			// ASIOChannelControls* in params, fill meter
//	CanInputMonitor,			// no arguments for CanXXX selectors
//	CanTimeInfo,
//	CanTimeCode,
//	CanTransport,
//	CanInputGain,
//	CanInputMeter,
//	CanOutputGain,
//	CanOutputMeter,
//	OptionalOne,
//
//	//	DSD support
//	//	The following extensions are required to allow switching
//	//	and control of the DSD subsystem.
//	SetIoFormat = 0x23111961,		/* ASIOIoFormat * in params.			*/
//	GetIoFormat = 0x23111983,		/* ASIOIoFormat * in params.			*/
//	CanDoIoFormat = 0x23112004,		/* ASIOIoFormat * in params.			*/
//
//	// Extension for drop out detection
//	CanReportOverload = 0x24042012,	/* return ASE_SUCCESS if driver can detect and report overloads */
//
//	GetInternalBufferSamples = 0x25042012	/* ASIOInternalBufferInfo * in params. Deliver size of driver internal buffering, return ASE_SUCCESS if supported */
//};
//
//
//using ASIOSampleRate = double;
//using ASIOSamples = int64_t;
//using ASIOTimeStamp = int64_t;
//
//struct AsioTimeInfo
//{
//	double          speed;                  // absolute speed (1. = nominal)
//	ASIOTimeStamp   systemTime;             // system time related to samplePosition, in nanoseconds
//											// on mac, must be derived from Microseconds() (not UpTime()!)
//											// on windows, must be derived from timeGetTime()
//	ASIOSamples     samplePosition;
//	ASIOSampleRate  sampleRate;             // current rate
//	unsigned long flags;                    // (see below)
//	char reserved[12];
//};
//
//struct ASIOTimeCode
//{
//	double          speed;                  // speed relation (fraction of nominal speed)
//											// optional; set to 0. or 1. if not supported
//	ASIOSamples     timeCodeSamples;        // time in samples
//	unsigned long   flags;                  // some information flags (see below)
//	char future[64];
//};
//
//struct ASIOTime                          // both input/output
//{
//	long reserved[4];                       // must be 0
//	AsioTimeInfo     timeInfo;       // required
//	ASIOTimeCode     timeCode;       // optional, evaluated if (timeCode.flags & kTcValid)
//};
//
//struct ASIOClockSource
//{
//	long index;					// as used for ASIOSetClockSource()
//	long associatedChannel;		// for instance, S/PDIF or AES/EBU
//	long associatedGroup;		// see channel groups (ASIOGetChannelInfo())
//	ASIOBool isCurrentSource;	// ASIOTrue if this is the current clock source
//	char name[32];				// for user selection
//};
//
//struct ASIOBufferInfo
//{
//	ASIOBool isInput;			// on input:  ASIOTrue: input, else output
//	long channelNum;			// on input:  channel index
//	void* buffers[2];			// on output: double buffer addresses
//};
//
//struct ASIOChannelInfo
//{
//	long channel;			// on input, channel index
//	ASIOBool isInput;		// on input
//	ASIOBool isActive;		// on exit
//	long channelGroup;		// dto
//	ASIOSampleType type;	// dto
//	char name[32];			// dto
//};
//
//typedef struct ASIOCallbacks
//{
//	void (*bufferSwitch) (long doubleBufferIndex, ASIOBool directProcess);
//	// bufferSwitch indicates that both input and output are to be processed.
//	// the current buffer half index (0 for A, 1 for B) determines
//	// - the output buffer that the host should start to fill. the other buffer
//	//   will be passed to output hardware regardless of whether it got filled
//	//   in time or not.
//	// - the input buffer that is now filled with incoming data. Note that
//	//   because of the synchronicity of i/o, the input always has at
//	//   least one buffer latency in relation to the output.
//	// directProcess suggests to the host whether it should immedeately
//	// start processing (directProcess == ASIOTrue), or whether its process
//	// should be deferred because the call comes from a very low level
//	// (for instance, a high level priority interrupt), and direct processing
//	// would cause timing instabilities for the rest of the system. If in doubt,
//	// directProcess should be set to ASIOFalse.
//	// Note: bufferSwitch may be called at interrupt time for highest efficiency.
//
//	void (*sampleRateDidChange) (ASIOSampleRate sRate);
//	// gets called when the AudioStreamIO detects a sample rate change
//	// If sample rate is unknown, 0 is passed (for instance, clock loss
//	// when externally synchronized).
//
//	long (*asioMessage) (long selector, long value, void* message, double* opt);
//	// generic callback for various purposes, see selectors below.
//	// note this is only present if the asio version is 2 or higher
//
//	ASIOTime* (*bufferSwitchTimeInfo) (ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess);
//	// new callback with time info. makes ASIOGetSamplePosition() and various
//	// calls to ASIOGetSampleRate obsolete,
//	// and allows for timecode sync etc. to be preferred; will be used if
//	// the driver calls asioMessage with selector ASIOFuture::SupportsTimeInfo.
//} ASIOCallbacks;
//
//struct IASIO : public IUnknown
//{
//	virtual ASIOBool init(void* sysHandle) = 0;
//	virtual void getDriverName(char* name) = 0;
//	virtual long getDriverVersion() = 0;
//	virtual void getErrorMessage(char* string) = 0;
//	virtual ASIOError start() = 0;
//	virtual ASIOError stop() = 0;
//	virtual ASIOError getChannels(long* numInputChannels, long* numOutputChannels) = 0;
//	virtual ASIOError getLatencies(long* inputLatency, long* outputLatency) = 0;
//	virtual ASIOError getBufferSize(long* minSize, long* maxSize, long* preferredSize, long* granularity) = 0;
//	virtual ASIOError canSampleRate(ASIOSampleRate sampleRate) = 0;
//	virtual ASIOError getSampleRate(ASIOSampleRate* sampleRate) = 0;
//	virtual ASIOError setSampleRate(ASIOSampleRate sampleRate) = 0;
//	virtual ASIOError getClockSources(ASIOClockSource* clocks, long* numSources) = 0;
//	virtual ASIOError setClockSource(long reference) = 0;
//	virtual ASIOError getSamplePosition(ASIOSamples* sPos, ASIOTimeStamp* tStamp) = 0;
//	virtual ASIOError getChannelInfo(ASIOChannelInfo* info) = 0;
//	virtual ASIOError createBuffers(ASIOBufferInfo* bufferInfos, long numChannels, long bufferSize, ASIOCallbacks* callbacks) = 0;
//	virtual ASIOError disposeBuffers() = 0;
//	virtual ASIOError controlPanel() = 0;
//	virtual ASIOError future(ASIOFuture selector, void* opt) = 0;
//	virtual ASIOError outputReady() = 0;
//};
//
//struct AsioInstance : IUnknown
//{
//	BEGIN_INTERFACE
//		virtual HRESULT STDMETHODCALLTYPE QueryInterface(
//			/* [in] */ REFIID riid,
//			/* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) = 0;
//
//	virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
//
//	virtual ULONG STDMETHODCALLTYPE Release(void) = 0;
//};
//
//}
//}
//namespace SBAudio = SB::Audio;
//
//using SBAsioDeviceInfo = SBAudio::AsioDeviceInfo;
//using SBAsioHandle = SBAudio::com_ptr<SBAudio::IASIO>;
