#include <common/include/sb_common.h>

#include <common/include/sbwin.h>
#include <common/include/win32/sbwin_result.h>

//#include <unordered_map>


#include "common/include/sb_utilities.h"
namespace SB { namespace LibX {
namespace Windows  {

// TODO
//S_OK
//DXGI_ERROR_DEVICE_HUNG


struct Win32_impl
{
	using handle_t = handle<Win32_impl>;
	using message_id = std::underlying_type_t<message_t>;
	using word_param_t = uintptr_t;
	using long_param_t = intptr_t;
	using long_result_t = int32_t;
	using callback_t = long_result_t (*)( handle_t hwnd, message_id msg, word_param_t wparam, long_param_t lparam );
	//std::unordered_map< message_id, callback_t > message_map;
};

} // Windows
}} // sbLibX
namespace sbWindows = SB::LibX::Windows;


#include <string>
#include <iostream>


int64_t ProcessWindow( sbWindows::window_handle handle, uint32_t message, uintptr_t message_data1, intptr_t message_data2 )
{
	return sbWindows::default_win_proc( handle, message, message_data1, message_data2 );
}

SB_EXPORT_TYPE int SB_STDCALL test_win32( [[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[] )
{
	sbWindows::window test( SB_SYSTEM_STRING( "sbMainWindowClass" ), ProcessWindow );
	[[maybe_unused]] sbWindows::result_t result = test.register_class();

	//////////////////////////////////////////////////////////////////////////
	// temp ugly stuff
	using window_style_flags = sbWindows::window_style_flags;
	window_style_flags creationFlags = window_style_flags::OVERLAPPED;
	if (argc > 4)
	{
		bool isVisible = argv[1][0] > '0'; // temp
		creationFlags = ( creationFlags & ~window_style_flags::VISIBLE ) | ( isVisible ? window_style_flags::VISIBLE : window_style_flags::OVERLAPPED);
	}
	if (argc > 5)
	{
		bool isPopup = argv[2][0] > '0'; // temp
		creationFlags = ( creationFlags & ~window_style_flags::POPUP ) | ( isPopup ? window_style_flags::POPUP : window_style_flags::OVERLAPPED );
	}

	using window_style_flags_ex = sbWindows::window_style_flags_ex;
	window_style_flags_ex creationFlagsEx{};
	if ( argc > 3 )
	{
		bool forComposition = argv[0][0] > '0'; // temp
		creationFlagsEx = ( creationFlagsEx & ~(window_style_flags_ex::NOREDIRECTIONBITMAP | window_style_flags_ex::OVERLAPPEDWINDOW) ) | ( forComposition ? window_style_flags_ex::NOREDIRECTIONBITMAP : window_style_flags_ex::OVERLAPPEDWINDOW );
	}

	test.create_window(SB_SYSTEM_STRING("test/win32 - main"), { creationFlags, creationFlagsEx });
	//////////////////////////////////////////////////////////////////////////
	return 0;
}

#if 0
// for SBWin32
#include <thread>
#include <mutex>
#include <future>

// generic
#include <cctype>

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
template<class _Ty> using future = std::future<_Ty>;


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
#include <Windows.h>
//#include <uxtheme.h>
//#include <dwmapi.h>
#pragma comment(lib, "user32.lib")
//#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "Dwmapi.lib")
#endif

#include <map>
//#include <algorithm>
#include <functional>


// Note : this is a bad design (I know)... still proof-of-concept prototyping.
volatile bool sbPrintAllMessages = false;
#if defined(__clang__)
	#pragma clang diagnostic push
	// yes, this is a dirty win32 test app that could crash while unloading...
	// however, it's currently single threaded and I free everything before leaving...
	// so it will do until sbWin is ready
	#pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif
std::map< UINT, std::function<LRESULT(WPARAM, LPARAM)> > callback;
#if defined(__clang__)
	#pragma clang diagnostic pop
	// yes, this is a dirty win32 test app so there are c-cast...
	#pragma clang diagnostic ignored "-Wold-style-cast"
#endif

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

	#if defined(__clang__)
		// i must admit i dont understand that one
		#pragma clang diagnostic ignored "-Wunused-lambda-capture"
	#endif
	volatile bool& printAllMessages = sbPrintAllMessages;
	callback[WM_CLOSE] = [hwnd, &printAllMessages]( WPARAM wParam, LPARAM lParam )
	{
		(void)wParam;
		(void)lParam;
		printAllMessages = false;
		wchar_t text[] = L"<\u2205|\xd835\xdfd9|\u2205> \u2243 1\n" // <∅|𝟙|∅> ≃ 1
			L"1/\u221e \u2243 0\n" // 1/∞ ≃ 0
			L"|(\u2124\u2295\u2124)\u2215\u2124| \u227a |\u211D\u2216(\u2124\u2295\u2124)|"; // |(ℤ⊕ℤ)∕ℤ| ≺ |ℝ∖(ℤ⊕ℤ)| 
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

int TestVulkan( void* hwnd );

int Test()
{
	//CHECK_STDIO();
	//LibX::Debug::Console debugConsole;
	//debugConsole.SetUnicodeCodePage();
	//debugConsole.RedirectStdIO();
	CHECK_STDIO();

	const auto className = L"SBMainWinClass";
	HINSTANCE hinstance = GetModuleHandleW( nullptr );

	WNDCLASSEXW wcx;

	// Fill in the window class structure with parameters 
	// that describe the main window. 

	wcx.cbSize = sizeof( wcx );          // size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW; // redraw if size changes 
	wcx.lpfnWndProc = MainWndProc;       // points to window procedure 
	wcx.cbClsExtra = 0;                  // no extra class memory 
	wcx.cbWndExtra = 0;                  // no extra window memory 
	wcx.hInstance = hinstance;           // handle to instance 
	wcx.hIcon = LoadIcon( nullptr,
		IDI_WINLOGO );               // predefined app. icon 
	wcx.hCursor = LoadCursor( nullptr,
		IDC_CROSS );                     // predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject(
		DKGRAY_BRUSH );                   // dark grey background brush 
	wcx.lpszMenuName = nullptr;       // name of menu resource 
	wcx.lpszClassName = className;    // name of window class 
	wcx.hIconSm = (HICON)LoadImage( hinstance,  // small class icon 
		MAKEINTRESOURCE( 5 ),
		IMAGE_ICON,
		GetSystemMetrics( SM_CXSMICON ),
		GetSystemMetrics( SM_CYSMICON ),
		LR_DEFAULTCOLOR );

	if (!RegisterClassExW( &wcx ))
	{
		if( GetLastError() != ERROR_CLASS_ALREADY_EXISTS )
			return -1;
	}

	static volatile bool forComposition = true;
	uint32_t creationFlagsEx = forComposition ? WS_EX_NOREDIRECTIONBITMAP : WS_EX_OVERLAPPEDWINDOW;

	static volatile bool visible = true;
	uint32_t creationFlags   = visible ? WS_VISIBLE : 0;

	[[maybe_unused]] HWND hwnd = CreateWindowExW(
		creationFlagsEx,
		className, L"SBLibX Test Win32",
		creationFlags /*WS_POPUP*/ | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, hinstance, nullptr);

#if 0
	//using TYPE_AllowDarkModeForWindow = bool ( WINAPI * )( HWND a_HWND, bool a_Allow );
	//static const TYPE_AllowDarkModeForWindow AllowDarkModeForWindow = (TYPE_AllowDarkModeForWindow)GetProcAddress( hUxtheme, MAKEINTRESOURCEA( 133 ) );
	//auto darkMode = AllowDarkModeForWindow( hwnd, true );
	//[[maybe_unused]] auto theme = SetWindowTheme( hwnd, L"DarkMode_Explorer", NULL );
	//SetWindowTheme( hwnd, L"DarkMode_Explorer", NULL );
#endif

#if 0
	constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
	DWORD enableDark = 2;
	DwmSetWindowAttribute( hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE - 1, &enableDark, sizeof( enableDark ) );
	DwmSetWindowAttribute( hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &enableDark, sizeof( enableDark ) );
#endif

#if 1//SB_SUPPORTS( SBCONSOLE_UNICODE )
	if (!hwnd || true)
	{
		auto hresult = GetLastError();
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
			nullptr,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
			hresult,
			MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
#if RESERVE_ERROR
			errorText,  // output 
			sizeof(errorText)/sizeof(*errorText),
#else
			(wchar_t*)&errorText,  // output 
			0, // minimum size for output buffer
#endif
			nullptr);   // arguments - see note 

#if !RESERVE_ERROR
		if (nullptr != errorText)
#endif
		{
			// TODO: support unicode...
			wchar_t test[] = L"A \u2295 B ~ \xd835\xdfd9";
			std::wcout << test << " (console only)" << std::endl;
			std::wclog << test << " (debug only)" << std::endl;
			std::wcerr << test << " (both)" << std::endl;
			CHECK_STDIO();
			//CHECK_STDIO();
			//std::cin.get();
			//CHECK_STDIO();

#if !RESERVE_ERROR
			// release memory allocated by FormatMessage()
			LocalFree( errorText );
			errorText = nullptr;
#endif
		}

		CHECK_STDIO();
	}
	CHECK_STDIO();
#endif

	if (hwnd)
	{
		sbPrintAllMessages = true;
		InitCallback( hwnd );
		//ShowWindow( hwnd, SW_SHOW );
		//UpdateWindow( hwnd );

		CHECK_STDIO();
		TestVulkan( hwnd );
		CHECK_STDIO();

		ShowWindow( hwnd, SW_SHOW );
		UpdateWindow( hwnd );

		MSG msg;
		BOOL fGotMessage;
		while ((fGotMessage = GetMessage( &msg, (HWND)nullptr, 0, 0 )) != 0 && fGotMessage != -1)
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
//	template<typename _lowlevel_t, typename _dimension_t>
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
#endif
