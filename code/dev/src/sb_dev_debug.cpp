#include <dev/include/sb_dev.h>

#if (SB_TARGET_TYPE & (SB_TARGET_TYPE_STATIC|SB_TARGET_TYPE_DYNAMIC|SB_TARGET_TYPE_STANDALONE)) != SB_TARGET_TYPE_STATIC
	#error "sbDebug is not meant to be used as a DLL/exe. It should be a static lib"
#endif
static_assert( SB_TARGET_TYPE != 0 && (SB_TARGET_TYPE&(SB_TARGET_TYPE_DYNAMIC|SB_TARGET_TYPE_STANDALONE)) == 0 );

////
#if defined(SBWIN64) || defined(SBWIN32)
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif

	//#pragma warning(disable:4668 5039 4339)
	#include <windows.h>
	//#pragma warning(default)

	#ifndef WINAPI
		#define WINAPI SB_STDCALL
	#endif
#endif

// TODO : Add other iostream support, wrap console outputs here, color synthaxing, etc.
// Since sbDev is a layer to be used within sbLibX, it cannot depends on anything from sbLibX.
// This library might thus only manipulates streams or similar communication protocols.

#include <dev/include/sb_dev_debug.h>

#include <fcntl.h>
#include <io.h>
//#include <codecvt>

namespace SB { namespace LibX { namespace Debug
{
	////
	Settings::Settings() noexcept
	{
	#if defined(SBDEBUG)
		// TODO : better handling, no hardcoded values.
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_WNDW);
	#endif

		IsDebuggerPresent();
	}
	Settings::~Settings()
	{
	}

	////
	Console::Console() noexcept
		 : consoleRawFlags(0)
	{
	}
	Console::~Console()
	{
		if (consoleRawFlags != 0)
			Free();
	}

	bool Console::Allocate()
	{
		if ( consoleRawFlags != 0 )
			Free();

	#if defined(SBCONSOLE)
		consoleFlags.allocated = AllocConsole();
		if (consoleRawFlags == 0)
			Attach();

		#if SB_SUPPORTS(SBCONSOLE_UNICODE)
		originalCodePage = GetConsoleOutputCP();
		#endif
		return consoleRawFlags != 0;
	#else
		return false;
	#endif
	}
	bool Console::Free()
	{
		if (consoleFlags.attached)
			Detach();
		if ( (consoleRawFlags != 0) && (0 == FreeConsole()) )
			consoleRawFlags = 0;
		return consoleRawFlags == 0; // should return true
	}
	bool Console::Attach()
	{
	#if defined(SBCONSOLE)
		if (consoleRawFlags == 0)
			consoleFlags.attached = (0 != AttachConsole(GetCurrentProcessId()));
		return consoleFlags.attached;
	#else
		return false;
	#endif
	}
	bool Console::Detach()
	{
		ResetCodePage();
		ResetStdIO();
		#if SB_SUPPORTS(SBCONSOLE_UNICODE)
		if (originalCodePage)
		{
			SetConsoleOutputCP(originalCodePage);
			originalCodePage = 0;
		}
		#endif
		if ( consoleFlags.attached && ( 0 == FreeConsole() ) )
			consoleRawFlags = 0;
		return true;
	}
	bool Console::ResetCodePage()
	{
	#if defined(SBCONSOLE) && SB_SUPPORTS(SBCONSOLE_UNICODE)
		if (originalCodePage == 0)
		{
			std::cout.flush();
			std::cerr.flush();
			std::wcout.flush();
			std::wcerr.flush();
			SetConsoleOutputCP(originalCodePage);
			std::locale::global(std::locale(""));
			originalCodePage = 0;
			return true;
		}
	#endif
		return false;
	}
	bool Console::ResetStdIO()
	{
	#if defined(SBCONSOLE)
		if (stdinbuf)
		{
			fclose(stdinbuf);
			stdinbuf = nullptr;
		}
		if (stdoutbuf)
		{
			fclose(stdoutbuf);
			stdoutbuf = nullptr;
		}
		if (stderrbuf)
		{
			fclose(stderrbuf);
			stderrbuf = nullptr;
		}
	#endif
		return true;
	}

	bool Console::Show( [[maybe_unused]] const char* consoleName )
	{
		bool success = false;
	#if defined(SBCONSOLE)
		if ( consoleRawFlags == 0 )
			Allocate();

		success = (consoleRawFlags != 0);
		if ( success && consoleName )
			success = ( 0 != SetConsoleTitleA(consoleName) );
	#endif
		return success;
	}
	bool Console::RedirectStdIO( [[maybe_unused]] stdio_mask_t::enum_t mask )
	{
		int result = 0;
		ResetStdIO();
#if defined(SBCONSOLE)
		if ( mask & stdio_mask_t::in )
			result |= ( 0 == freopen_s(&stdinbuf, "CONIN$", "r", stdin) ) ? stdio_mask_t::in : 0;
		if (mask & stdio_mask_t::out)
			result |= (0 == freopen_s(&stdoutbuf, "CONOUT$", "w", stdout)) ? stdio_mask_t::out : 0;
		if (mask & stdio_mask_t::err)
			result |= (0 == freopen_s(&stderrbuf, "CONOUT$", "w", stderr)) ? stdio_mask_t::err : 0;
	#endif
		return result != 0;
	}

	bool Console::SetUnicodeCodePage()
	{
		int result = 0;
	#if defined(SBCONSOLE) && SB_SUPPORTS(SBCONSOLE_UNICODE)
		if ( originalCodePage == 0 )
			originalCodePage = GetConsoleOutputCP();

		// set StdIO mode to utf-8
		SetConsoleOutputCP(CP_UTF8);
		// must not change input mode
		std::cout.flush(); std::wcout.flush(); _setmode(_fileno(stdout), _O_U8TEXT);
		std::cerr.flush(); std::wcerr.flush(); _setmode(_fileno(stderr), _O_U8TEXT);
		std::locale::global(std::locale(""));
		//std::cout.sync_with_stdio(true);
		//std::wcout.sync_with_stdio(true);
		//std::cerr.sync_with_stdio(true);
		//std::wcerr.sync_with_stdio(true);
	#endif
		return result == 0;
	}
	// From the one of the worst formatted single line ever : https://support.microsoft.com/fr-ca/help/99261/how-to-performing-clear-screen-cls-in-a-console-application
	//void cls(HANDLE hConsole)
	//{
	//	COORD coordScreen = { 0, 0 };
	//	BOOL bSuccess;
	//	DWORD cCharsWritten;
	//	CONSOLE_SCREEN_BUFFER_INFO csbi;
	//	DWORD dwConSize;
	//	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
	//	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	//	bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);
	//	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
	//	bSuccess = FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
	//	bSuccess = SetConsoleCursorPosition(hConsole, coordScreen);
	//	return;
	//}


	////
#if SB_SUPPORTS(SBDEBUG_OUTPUT)
		template<bool redirect_output>
	void WriteDebugOutput(const char* begin, const char* end)
	{
		using char_type = char;
		std::basic_string<char_type> s(begin, std::distance(begin, end));
		OutputDebugStringA(s.data());
	#if defined(SBCONSOLE)
		if constexpr (redirect_output)
			std::cout << s;
	#endif
	};
		template<bool redirect_output>
	void WriteDebugOutput(const wchar_t* begin, const wchar_t* end)
	{
		using char_type = wchar_t;
		std::basic_string<char_type> s(begin, std::distance(begin, end));
		OutputDebugStringW(s.data());
	#if defined(SBCONSOLE)
		if constexpr (redirect_output)
			std::wcout << s;
	#endif
	};


		template<typename char_type, bool redirect_output, typename char_traits>
	OutputDebugStringBuf<char_type, redirect_output, char_traits>::OutputDebugStringBuf() : rdbuf{}
	{
		std::basic_stringbuf<char_type, char_traits>::setg(nullptr, nullptr, nullptr);
		std::basic_stringbuf<char_type, char_traits>::setp(rdbuf.data(), rdbuf.data(), rdbuf.data() + rdbuf.size());
	}

		template<typename char_type, bool redirect_output, typename char_traits>
	int OutputDebugStringBuf<char_type, redirect_output, char_traits>::sync()
	{
		const auto begin = std::basic_stringbuf<char_type, char_traits>::pbase();
		const auto end = std::basic_stringbuf<char_type, char_traits>::pptr();
		const size_t diff = std::distance(begin, end);
		const bool full = diff >= rdbuf.size();
		WriteDebugOutput<redirect_output>(begin, end);
		std::basic_stringbuf<char_type, char_traits>::setp(rdbuf.data(), rdbuf.data(), rdbuf.data() + rdbuf.size());
		return 0;
	}

		template<typename char_type, bool redirect_output, typename char_traits>
	typename OutputDebugStringBuf<char_type, redirect_output, char_traits>::int_type
		 OutputDebugStringBuf<char_type, redirect_output, char_traits>
		::overflow(int_type c)
	{
		int sync_result = sync();
		if (c != char_traits::eof())
		{
			rdbuf[0] = static_cast<char_type>(c);
			std::basic_stringbuf<char_type, char_traits>::setp(rdbuf.data(), rdbuf.data() + 1, rdbuf.data() + rdbuf.size());
		}
		return sync_result == -1 ? char_traits::eof() : 0;
	}

	template class OutputDebugStringBuf<char,     true, std::char_traits<char>>;
	template class OutputDebugStringBuf<char,    false, std::char_traits<char>>;
	template class OutputDebugStringBuf<wchar_t,  true, std::char_traits<wchar_t>>;
	template class OutputDebugStringBuf<wchar_t, false, std::char_traits<wchar_t>>;
#endif


	////
	DebuggerOutput::DebuggerOutput(const Console* debugConsole) : console(debugConsole) {}
	DebuggerOutput::~DebuggerOutput() {}

	bool DebuggerOutput::RedirectStdIO()
	{
	#if SB_SUPPORTS(SBDEBUG_OUTPUT)
		// TODO : Share DebugOutput buffers for clog & cerr + add log file

		// redirect cerr / clog to debugout and some other stream(s)
		// -	cerr writes to debug, console and log streams;
		// -	clog writes to debug and log streams;
		// -	cout writes to console and log streams;
		//
		// Thus:
		// -	debug stream should only output system messages and runtime checks, skipping application console output;
		// -	log stream should write everything for any exhaustive analysis;
		// -	console stream should only output app-side logs;
		std::cerr.rdbuf(&charDebugOutputRedirect);
		std::clog.rdbuf(&charDebugOutput);
		std::wcerr.rdbuf(&charDebugOutputRedirectW);
		std::wclog.rdbuf(&charDebugOutputW);
	#endif
		return std::cerr && std::clog;
	}

}}} // namespace SB::LibX::Debug
