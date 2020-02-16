#pragma once
#include <dev/include/sb_dev.h>

#include <cstdint>
#if defined(SBCONSOLE) || (defined(SBDEBUG_OUTPUT) && SBDEBUG_OUTPUT)
#include <array>
#include <iosfwd>
#include <sstream>
#include <string>
#endif


#define SB_SUPPORTS(X)		(0 | X | 0)
#define SB_SUPPORTED		(true)
#define SB_UNSUPPORTED		(false)


#define SBCONSOLE_UNICODE	(false)


namespace SB { namespace LibX { namespace Debug
{

struct Settings
{
	Settings() noexcept;
	virtual ~Settings();
};

struct Console
{
	Console() noexcept;
	virtual ~Console();

	struct stdio_mask_t
	{
		enum enum_t : uint8_t
		{
			in  = (1 << 0),
			out = (1 << 1),
			err = (1 << 2),

			all = 0xFF,
		};
	};

	bool Show( [[maybe_unused]] const char* consoleName );

	bool RedirectStdIO( stdio_mask_t::enum_t mask = stdio_mask_t::all );
	bool ResetStdIO();

	bool SetUnicodeCodePage(); // this seems to be broken when messing with Win32
	bool ResetCodePage();

	bool Attach();
	bool Detach();

private:
	bool Allocate();
	bool Free();

private:
#if defined(SBCONSOLE)
	FILE*    stdinbuf = nullptr;
	FILE*    stdoutbuf = nullptr;
	FILE*    stderrbuf = nullptr;
	#if SB_SUPPORTS(SBCONSOLE_UNICODE)
		uint32_t originalCodePage = 0; // TODO : it now assumes CP = 0 is not valid... is it? can't find doc about which value is invalid
	#endif
#endif

	union
	{
		struct
		{
			bool allocated : 1;
			bool attached  : 1;
		} consoleFlags;

		uint32_t consoleRawFlags;
	};
};

#if SB_SUPPORTS(SBDEBUG_OUTPUT)
// TODO : de-hardcode buffer size
template<typename char_type, bool redirect_output, typename char_traits = std::char_traits<char_type>>
class OutputDebugStringBuf : public std::basic_stringbuf<char_type, char_traits>
{
public:
	explicit OutputDebugStringBuf();
	~OutputDebugStringBuf() {}

	static_assert(std::is_same<char_type,char>::value || std::is_same<char_type,wchar_t>::value,
		 "OutputDebugStringBuf only supports char and wchar_t types"
	);

	int sync();

	using int_type = typename std::basic_stringbuf<char_type, char_traits>::int_type;
	int_type overflow(int_type c = char_traits::eof());

private:
	std::array<char_type, 512> rdbuf;
};
#endif

struct DebuggerOutput
{
	DebuggerOutput(const Console* console);
	virtual ~DebuggerOutput();

	bool RedirectStdIO();

private:
	const Console* console;
#if SB_SUPPORTS(SBDEBUG_OUTPUT)
	OutputDebugStringBuf<char, true>  charDebugOutputRedirect; // expected for cerr : outputs to both debug output & back into console out
	OutputDebugStringBuf<char, false> charDebugOutput; // expected for clog : outputs to debug output only
	OutputDebugStringBuf<wchar_t, true>  charDebugOutputRedirectW; // expected for cerr : outputs to both debug output & back into console out
	OutputDebugStringBuf<wchar_t, false> charDebugOutputW; // expected for clog : outputs to debug output only
	// expected to use stdI/O for normal console output
#endif
};

}}} // namespace SB::LibX::Debug
