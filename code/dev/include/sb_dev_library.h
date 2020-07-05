#pragma once
#include <common/include/sb_library.h>
//#include <dev/include/sb_dev.h>

#include <string_view>
#include <vector>

#if defined(SBDEBUG)
#pragma comment(lib, "sbWindows_x64_debug.lib")
#elif defined(SBRELEASE)
#pragma comment(lib, "sbWindows_x64_release.lib")
#endif

namespace SB { namespace LibX { namespace Dev
{

class library : public unique_dll
{
public:
	explicit library( const char* libName = nullptr ) : unique_dll() { if( libName ) load( libName ); }
	//explicit library( const wchar_t* libName ) : unique_dll() { if( libName ) load( libName ); }
	explicit library( library&& other ) : unique_dll( std::move(other) ) {}
	explicit library( const library& other ) = delete;
	~library() { unload(); }

	bool load( const char* libName );
	//bool load( const wchar_t* libName );
	bool unload() { aliases.clear(); auto old = handle; handle = nullptr; return unique_dll::unload( std::move(old) ); }

	std::string name() const { return aliases.size() ? aliases[0] : std::string{}; }
	std::string alias( int index = 1 ) const { return aliases.size() > index ? aliases[index] : std::string{}; }

protected:
	std::vector<std::string> aliases;
};

}}} // namespace SB::LibX::Dev
