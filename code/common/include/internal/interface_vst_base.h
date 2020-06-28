#pragma once
#include <common/include/sb_common.h>
#include <common/include/internal/ref_ptr.h>

#include <cstdint>
namespace SB { namespace LibX {
namespace VST
{
/////////////////////////////////////////////////////////
// Please refer to official Steinberg ASIO documentation
/////////////////////////////////////////////////////////

// VST3 :
// The VST provide the entry points "InitDll" and "GetPluginFactory"
class IPluginFactory;
using InitDll_t = bool (SB_STDCALL*)();
using GetPluginFactory_t = IPluginFactory * (SB_STDCALL*)();

using size_type = int64_t;
using result_t = int32_t;

struct FactoryInfo
{
	enum FactoryFlags : uint32_t
	{
		kNoFlags					= 0,		///< Nothing
		kClassesDiscardable			= 1 << 0,	///< The number of exported classes can change each time the Module is loaded. If this flag is set, the host does not cache class information. This leads to a longer startup time because the host always has to load the Module to get the current class information.
		kLicenseCheck				= 1 << 1,	///< Class IDs of components are interpreted as Syncrosoft-License (LICENCE_UID). Loaded in a Steinberg host, the module will not be loaded when the license is not valid
		kComponentNonDiscardable	= 1 << 3,	///< Component won't be unloaded until process exit
		kUnicode                    = 1 << 4    ///< Components have entirely unicode encoded strings. (True for VST 3 Plug-ins so far)
	};

	enum
	{
		kURLSize = 256,
		kEmailSize = 128,
		kNameSize = 64
	};

	char vendor[kNameSize]{};		///< e.g. "Steinberg Media Technologies"
	char url[kURLSize]{};			///< e.g. "http://www.steinberg.de"
	char email[kEmailSize]{};		///< e.g. "info@steinberg.de"
	FactoryFlags flags{};			///< (see above)
};
struct ClassInfo
{
	enum Cardinality : int32_t
	{
		kManyInstances = 0x7FFFFFFF
	};

	enum
	{
		kCategorySize = 32,
		kNameSize = 64
	};

	using CLSID = IUnknown::GUID::CLSID;
	CLSID       cid;                       ///< Class ID 16 Byte class GUID
	Cardinality cardinality;              ///< cardinality of the class, set to kManyInstances (see \ref ClassCardinality)
	char category[kCategorySize];  ///< class category, host uses this to categorize interfaces
	char name[kNameSize];          ///< class name, visible to the user
};
// To keep POD as POD, no inheritance should be used on info structs
union ClassInfo2
{
	ClassInfo info1;
	struct ClassInfo2_t
	{
		ClassInfo::CLSID       cid;
		ClassInfo::Cardinality cardinality;
		char category[ClassInfo::kCategorySize];
		char name[ClassInfo::kNameSize];
	};
};

class IPluginFactory : public IUnknown
{
public:
	virtual result_t SB_STDCALL getFactoryInfo (FactoryInfo* info) = 0;

	virtual int32_t SB_STDCALL countClasses () = 0;
	virtual result_t SB_STDCALL getClassInfo (int32_t index, ClassInfo* info) = 0;
	virtual result_t SB_STDCALL createInstance (const char cid[], const char _iid[], void** obj) = 0;

public:
	static inline const GUID iid = { 0x7A4D811C, 0x5211, 0x4A1F, { 0xAE,0xD9,0xD2,0xEE,0x0B,0x43,0xBF,0x9F, } };
};


class IPluginBase: public IUnknown
{
public:
	virtual result_t SB_STDCALL initialize (IUnknown* context) = 0;
	virtual result_t SB_STDCALL terminate () = 0;

public:
	static inline const GUID iid = { 0x22888DDB, 0x156E, 0x45AE, { 0x83,0x58,0xB3,0x48,0x08,0x19,0x06,0x25 } };
};


}
}}
