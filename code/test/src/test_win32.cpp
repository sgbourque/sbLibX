﻿
// for SBWin32
#include <thread>
#include <mutex>
#include <future>

// generic
#include <ctype.h>

namespace SB
{
// SBLibX module architecture.
// Main division. This is bundled 
// 1. SBWin32, ... (one and only one of these libray will be available through unified headers)
namespace SBWin32
{
// System-specific (mostly Win32x64 on Win10 target for now);
using thread = std::thread;
using mutex  = std::mutex;
using future = std::future;


} // SBWin32
#if defined(SBWIN32) || defined(SBWIN64)
using namespace SBWin32;
#endif

//
// 2. SBMemory
// Memory management (generic through registered callbacks);

// 3. Graphics & display management (API-specific through uniformalized functionality w/capability );
// c.f., test_vulkan

// 4. Audio management (mostly ASIO target for now);
// c.f., SBAudio

// 5. Generic flow
// multiple partial WIP all over different SBLib iterations & some other misc projects (as SBWin & SBAudio)

}

#include <dev/include/sb_dev_debug.h>


////
//#pragma warning(disable:4081 4365 4472 4514 4571 4625 4626 4710 4774 4820 5026 5027)
#include <iostream>
//#pragma warning(default)

void CHECK_STDIO(); // test_main.cpp


#if 1
#include "windows.h"
#pragma comment(lib, "user32.lib")
//#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "Gdi32.lib")
#endif

#include <map>
//#include <algorithm>
#include <functional>


volatile bool sbPrintAllMessages = false;
std::map< UINT, std::function<LRESULT( WPARAM, LPARAM )> > callback;
void InitCallback( HWND hwnd )
{
	callback[WM_DESTROY] = [hwnd]( WPARAM wParam, LPARAM lParam )
	{
		(void)wParam;
		(void)lParam;
		//PostQuitMessage( 0 );
		callback.clear();
		return DefWindowProcW( hwnd, WM_DESTROY, wParam, lParam );
	};
	volatile bool& printAllMessages = sbPrintAllMessages;
	callback[WM_CLOSE] = [hwnd, &printAllMessages]( WPARAM wParam, LPARAM lParam )
	{
		(void)wParam;
		(void)lParam;
		sbPrintAllMessages = false;
		wchar_t text[] = L"<\u2205|\xd835\xdfd9|\u2205> ~ 1\n" // <∅|𝟙|∅> ~ 1
			L"1/\u221e ~ 0\n" // 1/∞ ~ 0
			L"|(\u2124\u2295\u2124)\u2215\u2124| < |\u211D\u2216(\u2124\u2295\u2124)|"; // |(ℤ⊕ℤ)∕ℤ| < |ℝ∖(ℤ⊕ℤ)| 
		if ( MessageBoxW( hwnd,
			text,
			L"Stéphane Bourque", MB_OKCANCEL ) == IDOK )
		{
			DestroyWindow( hwnd );
		}
		return 0;
	};
}

LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if ( sbPrintAllMessages )
		std::wcout << hWnd << L" -> " << msg << L"(" << wParam << L", " << lParam << L")" << std::endl;

	LRESULT result;
	auto it = callback.find( msg );
	if (it != callback.end())
	{
		result = it->second(wParam, lParam);
	}
	else
	{
		result = DefWindowProcW( hWnd, msg, wParam, lParam );
	}
	return result;
}

#include <fcntl.h>
#include <io.h>
#include <codecvt>

int TestVulkan();

int Test()
{
	//CHECK_STDIO();
	//LibX::Debug::Console debugConsole;
	//debugConsole.SetUnicodeCodePage();
	//debugConsole.RedirectStdIO();
	CHECK_STDIO();

	const auto className = L"SBMainWinClass";
	HINSTANCE hinstance = GetModuleHandleW( NULL );

	WNDCLASSEXW wcx;

	// Fill in the window class structure with parameters 
	// that describe the main window. 

	wcx.cbSize = sizeof( wcx );          // size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW; // redraw if size changes 
	wcx.lpfnWndProc = MainWndProc;       // points to window procedure 
	wcx.cbClsExtra = 0;                  // no extra class memory 
	wcx.cbWndExtra = 0;                  // no extra window memory 
	wcx.hInstance = hinstance;           // handle to instance 
	wcx.hIcon = LoadIcon( NULL,
		IDI_WINLOGO );               // predefined app. icon 
	wcx.hCursor = LoadCursor( NULL,
		IDC_CROSS );                     // predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject(
		DKGRAY_BRUSH );                   // white background brush 
	wcx.lpszMenuName = nullptr;       // name of menu resource 
	wcx.lpszClassName = className;    // name of window class 
	wcx.hIconSm = (HICON)LoadImage( hinstance,  // small class icon 
		MAKEINTRESOURCE( 5 ),
		IMAGE_ICON,
		GetSystemMetrics( SM_CXSMICON ),
		GetSystemMetrics( SM_CYSMICON ),
		LR_DEFAULTCOLOR );

	if (!RegisterClassExW( &wcx ))
		return -1;


	[[maybe_unused]] HWND hwnd = CreateWindowExW(
		WS_EX_NOREDIRECTIONBITMAP,
		className, L"SBLibX Test Win32",
		WS_OVERLAPPEDWINDOW /*WS_POPUP*/ | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, hinstance, nullptr);

#if SB_SUPPORTS( SBCONSOLE_UNICODE )
	if (!hwnd || true)
	{
		HRESULT hresult = GetLastError();
#define RESERVE_ERROR 0
#if RESERVE_ERROR
		wchar_t errorText[256];
#else
		wchar_t* errorText = nullptr;
#endif
		FormatMessageW(
			// use system message tables to retrieve error text
			FORMAT_MESSAGE_FROM_SYSTEM
#if !RESERVE_ERROR
			// allocate buffer on local heap for error text
			| FORMAT_MESSAGE_ALLOCATE_BUFFER
#endif
			// Important! will fail otherwise, since we're not 
			// (and CANNOT) pass insertion parameters
			| FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
			hresult,
			MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
#if RESERVE_ERROR
			errorText,  // output 
			sizeof(errorText)/sizeof(*errorText),
#else
			(wchar_t*)&errorText,  // output 
			0, // minimum size for output buffer
#endif
			NULL );   // arguments - see note 

#if !RESERVE_ERROR
		if (NULL != errorText)
#endif
		{
//			// Fuck! Why does the one in dev_Debug does not work?
//			// Why do I have to setmode & reopen the streambuf again!
//
//			////UINT oldcp = GetConsoleOutputCP();
			//SetConsoleOutputCP(CP_UTF8);
			//// should not change input mode
			//std::cout.flush(); _setmode(_fileno(stdout), _O_U8TEXT);
			//std::cerr.flush(); _setmode(_fileno(stderr), _O_U8TEXT);
			//std::locale::global( std::locale( "" ) );
//
			//CHECK_STDIO();
			//LibX::Debug::Console debugConsole1;
			//debugConsole1.SetUnicodeCodePage();
			//debugConsole1.RedirectStdIO();
			//CHECK_STDIO();
			//LibX::Debug::Console debugConsole;
			//debugConsole.RedirectStdIO();
			wchar_t test[] = L"A \u2295 B ~ \xd835\xdfd9";
			std::wcout << test << " (console only)" << std::endl;
			std::wclog << test << " (debug only)" << std::endl;
			std::wcerr << test << " (both)" << std::endl;
			CHECK_STDIO();
			//
//			//////UINT oldcp = GetConsoleOutputCP();
//			//SetConsoleOutputCP(CP_UTF8);
//			//// should not change input mode
//			//std::cout.flush(); _setmode(_fileno(stdout), _O_U8TEXT);
//			//std::cerr.flush(); _setmode(_fileno(stderr), _O_U8TEXT);
//			//std::locale::global( std::locale( "" ) );
//
//			//std::wcerr << "Error " << hresult << ": " << errorText << std::endl;
//
//			//wchar_t test[] = L"A \u2295 B ~ \xd835\xdfd9";
//			//std::wcout << test << " (console only)" << std::endl;
//			//std::wclog << test << " (debug only)" << std::endl;
//			////SetConsoleOutputCP(oldcp);
//			////std::cerr.flush(); _setmode(_fileno(stderr), _O_U8TEXT);
//			////std::wcout.flush(); _setmode(_fileno(stdout), _O_U8TEXT);
//
//			FILE* stdinbuf = nullptr;
//			FILE* stdoutbuf = nullptr;
//			FILE* stderrbuf = nullptr;
//			int result = 0;
//			result |= freopen_s(&stdinbuf, "CONIN$", "r", stdin);
//			result |= freopen_s(&stdoutbuf, "CONOUT$", "w", stdout);
//			result |= freopen_s(&stderrbuf, "CONOUT$", "w", stderr);
//
//
			CHECK_STDIO();
			//LibX::Debug::Console debugConsole2;
			//debugConsole2.RedirectStdIO();
			std::cin.get();
			CHECK_STDIO();
			//
//#if !RESERVE_ERROR
//			// release memory allocated by FormatMessage()
//			LocalFree( errorText );
//			errorText = NULL;
//#endif
		}

		CHECK_STDIO();
	}
	CHECK_STDIO();
#endif

	if (hwnd)
	{
		sbPrintAllMessages = true;
		InitCallback( hwnd );
		ShowWindow( hwnd, SW_SHOW );
		UpdateWindow( hwnd );

		CHECK_STDIO();
		TestVulkan();
		CHECK_STDIO();

		MSG msg;
		BOOL fGotMessage;
		while ((fGotMessage = GetMessage( &msg, (HWND)NULL, 0, 0 )) != 0 && fGotMessage != -1)
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			if (callback.empty())
				break;
		}

		//LibX::Debug::Console debugConsole;
		//debugConsole.RedirectStdIO();
		std::cout << "done" << std::endl;
		CHECK_STDIO();
	}

	CHECK_STDIO();
	if (!UnregisterClassW( className, hinstance ))
		return -1;

	CHECK_STDIO();
	return 0;
}


#if 0
//namespace SBLib::Mathematics
//{
//
//template<typename _lowlevel_t, typename _dimension_t>
//struct vector
//{
//};
//
//}
//namespace SBMath = SBLib::Mathematics;


template<typename float_type, size_t size = sizeof(float_type)>
struct integer_traits;

template<>
struct integer_traits<float, sizeof(float)> { using type = unsigned int; static_assert(sizeof(float) == sizeof(type)); };
template<>
struct integer_traits<double, sizeof(double)> { using type = unsigned long long; static_assert(sizeof(double) == sizeof(type)); };

template<typename _float_type>
union float_int_union
{
	using float_type = _float_type;
	using int_type = typename integer_traits<float_type>::type;
	float_type float_value;
	int_type   int_value;

	constexpr float_int_union(int n) : int_value(static_cast<int_type>(n)) {}
	constexpr float_int_union(unsigned int n) : int_value(static_cast<int_type>(n)) {}
	constexpr float_int_union(int_type n) : int_value(n) {}
	constexpr float_int_union(float_type x) : float_value(x) {}
};

void test()
{
	using float_type = float;
	using float_union = float_int_union<float_type>;
	auto print_float = [](const char* name, float_union X)
	{
		std::cout
			<< name << " (long):  " << std::hex << X.int_value << "\n"
			<< name << " (float): " << X.float_value << std::endl;
	};
#define PRINT_FLOAT(X) print_float(#X, X)
#define CONSTANT_FLOAT(X, V) constexpr float_union X{ V }; PRINT_FLOAT(X)

	{
		CONSTANT_FLOAT(zero, 0u);
		CONSTANT_FLOAT(denorm_min, std::numeric_limits<float_type>::denorm_min());
		CONSTANT_FLOAT(norm_min, std::numeric_limits<float_type>::min());
		print_float("denorm_min/norm_min", float_union{ denorm_min.float_value / norm_min.float_value });
		CONSTANT_FLOAT(epsilon, std::numeric_limits<float_type>::epsilon());

		constexpr float_type ONE = static_cast<float_type>(1.0);
		CONSTANT_FLOAT(one, ONE);
		constexpr float_type HALF = static_cast<float_type>(0.5);
		print_float("1.0 - delta/4", float_union{ ONE - HALF * HALF * epsilon.float_value });
		print_float("1.0 + delta/4", float_union{ ONE + HALF * HALF * epsilon.float_value });
		print_float("1.0 - delta/2", float_union{ ONE - HALF * epsilon.float_value });
		print_float("1.0 + delta/2", float_union{ ONE + HALF * epsilon.float_value });
		print_float("1.0 - delta", float_union{ ONE - epsilon.float_value });
		print_float("1.0 + delta", float_union{ ONE + epsilon.float_value });
	}
	std::cin.get();
}
#endif