#include "SBAsioDevice.h"

#define SB_WIDEN_INTERNAL(X) 	L##X
#define SB_WIDEN(X)          	SB_WIDEN_INTERNAL(X)


#define SB_APPLICATION_NAME  	"SBAudio"
#define SB_W_APPLICATION_NAME	SB_WIDEN(SB_APPLICATION_NAME)

#define SB_USER_PATH		"User"
#define SB_DATA_PATH		"Data"
#define SB_SHARED_PATH		"Shared"
#define SB_DOCUMENTS_PATH	"Documents"

#define SB_WARNING(...)		

using SB_PLATFORM_HANDLE = void*;


#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>

struct SBApplicationContext
{
	size_t
		version = 1;
	SB_PLATFORM_HANDLE
		handle = nullptr;
	std::unordered_map<std::string, std::wstring>
		folders = {};

	mutable std::vector< std::tuple< SBAsioHandle, SBAsioDeviceInfo > >
		asioDevices;

	operator bool() const { return version > 0 && handle != nullptr && !folders.empty(); }
};

struct SBSetup
{
	size_t
		version = 0;
	std::unordered_map<std::string, std::string>
		settings = {};

	operator bool() const { return version > 0 && !settings.empty(); }
};

#include <tuple>
#include "Shlobj.h"

namespace SB { namespace Audio {


}}


namespace
{

bool InitializeSystemPath(SBApplicationContext& context)
{
	using callback_t = bool (*)(const std::wstring&);
	callback_t createDirCallback = [](const std::wstring& path) -> bool
	{
		BOOL success = CreateDirectoryW(path.c_str(), NULL);
		return success || GetLastError() == ERROR_ALREADY_EXISTS;
	};
	callback_t verifyDirCallback = [](const std::wstring& path) -> bool
	{
		HANDLE dir = CreateFileW(path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		if (dir != INVALID_HANDLE_VALUE)
		{
			CloseHandle(dir);
			return true;
		}
		else
		{
			return false;
		}
	};

	std::tuple<std::string, GUID, callback_t> knownFolders[] = {
		{ SB_USER_PATH, FOLDERID_LocalAppData, createDirCallback },  	// %LOCALAPPDATA% (%USERPROFILE%\AppData\Local)
		{ SB_DATA_PATH, FOLDERID_ProgramData, verifyDirCallback },   	// %ALLUSERSPROFILE% (%ProgramData%, %SystemDrive%\ProgramData)
		{ SB_SHARED_PATH, FOLDERID_Public, verifyDirCallback },        	// %PUBLIC% (%SystemDrive%\Users\Public)
		{ SB_DOCUMENTS_PATH, FOLDERID_Documents, verifyDirCallback },	// %USERPROFILE%\Documents
	};

	for (const auto& knownFolder : knownFolders)
	{
		const auto folderName = std::get<0>(knownFolder);
		const auto folderGUID = std::get<1>(knownFolder);
		const auto callback = std::get<2>(knownFolder);

		PWSTR	path = nullptr;
		if (SHGetKnownFolderPath(folderGUID, 0, NULL, &path) == S_OK)
		{
			std::wstring applicationPath = std::wstring(path) + std::wstring(L"\\" SB_W_APPLICATION_NAME);
			CoTaskMemFree(path);
			path = nullptr;

			if (callback && callback(applicationPath))
			{
				context.folders.insert({ folderName, applicationPath });
			}
		}
	}
	return context;
}


bool InitializeSystemAudio(SBApplicationContext& context)
{
	using namespace SBAudio;
	ASIOInitialize();
	std::wcout << "Initializing audio";
	const auto list = EnumerateASIODevices();
	for (const auto& it : list)
	{
		CreateAsioDriver(it.classID);
		auto handle = QueryInterface(it);
		handle->init(GetCurrentProcess());
		ASIOError started = handle->start();
		std::wcout << "\n\t" << it.name << ": " << GetASIOErrorString(started);
		context.asioDevices.emplace_back(std::make_tuple(succeeded(started) ? handle : SBAsioHandle{}, it));
		if( !succeeded(started) )
		{
			AsioDriverResult released = ReleaseAsioDriver(it.classID);
		}
	}
	//std::sort( context.asioDevices.begin(), context.asioDevices.end(),
	//	[]( const std::tuple< SBAsioHandle, SBAsioDeviceInfo >& deviceA, const std::tuple< SBAsioHandle, SBAsioDeviceInfo >& deviceB )
	//	{
	//		const auto deviceInfoA = std::get<SBAsioDeviceInfo>( deviceA );
	//		const auto deviceHandleA = std::get<SBAsioHandle>( deviceA );
	//		const auto deviceInfoB = std::get<SBAsioDeviceInfo>( deviceB );
	//		const auto deviceHandleB = std::get<SBAsioHandle>( deviceB );
	//		const bool onlyDeviceAValid = ( !!deviceHandleA && !deviceHandleB );
	//		const bool onlyDeviceBValid = ( !deviceHandleA && !!deviceHandleB);
	//		return onlyDeviceAValid || ( !onlyDeviceBValid && ( deviceInfoA.name < deviceInfoB.name ) );
	//	}
	//);

	std::wcout << std::endl;
	return context;
}

void ShutdownSystemAudio(const SBApplicationContext& context)
{
	using namespace SBAudio;
	std::wcout << "Shutdown audio";
	for (auto& it : context.asioDevices)
	{
		auto& deviceInfo = std::get<SBAsioDeviceInfo>( it );
		auto& handle = std::get<SBAsioHandle>( it );
		if( handle )
		{
			ASIOError stopped = handle->stop();
			std::wcout << "\n\t" << deviceInfo.name << ": " << GetASIOErrorString(stopped);
		}
		handle.Reset();
	}
	std::wcout << std::endl;
	ASIOShutdown();
}


SBApplicationContext InitializeApplicationContext()
{
	SBApplicationContext context = {};
	context.handle = GetCurrentProcess();
	InitializeSystemPath(context);
	// Network
	// Display
	// Input
	// Audio
	InitializeSystemAudio(context);
	return context;
}

void ShutdownApplicationContext(const SBApplicationContext& context)
{
	ShutdownSystemAudio(context);
}

/*
//#include "Windows.h"
#include <fstream>
SBSetup ReadConfiguration(const SBApplicationContext& context)
{
	SBSetup setup;
	std::ifstream configFile(config.configFilePath.c_str());
	while (configFile)
	{
		std::string key;
		std::getline(configFile, key);
		std::string value;
		std::getline(configFile, value);
		if (!key.empty())
		{
			config.settings.insert({ key, value });
		}
		else
		{
			SB_WARNING("Invalid key found in config file.");
		}
	}
	return setup;
}
*/
}

int main(int argc, char* argv[])
{
	// 1. Initialize system / get configuration
	//	-	Get system configuration file;
	//	-	Get user configuration file;
	//	-	Initialize worker threads;
	const SBApplicationContext context = InitializeApplicationContext();

	ShutdownApplicationContext(context);
}
