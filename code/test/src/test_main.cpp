#include <dev/include/sb_dev.h>
#include <dev/include/sb_dev_debug.h>

#include <iostream>

void CHECK_STDIO()
{
#if defined(SBDEBUG)
	//std::cout.flush();
	//if (std::cout.bad())
	//	std::cout.clear();
	//std::cout << u8"c\u2295";
	//std::cout.flush();
	//if ( std::cout.bad() )
	//	std::cout.clear();

	std::wcout.flush();
	if (std::wcout.bad())
		std::cout.clear();
	std::wcout << L"w\u2295";
	std::wcout.flush();
	if ( std::wcout.bad() )
		std::cout.clear();
#endif
}


namespace SB { namespace LibX
{
}} // namespace SB::LibX
using namespace SB;

int Test();


#if 0
// see https://www.codeproject.com/Articles/1045674/Load-EXE-as-DLL-Mission-Possible
#pragma comment(linker, "/FIXED:NO")
// but we should try using _DllMainCRTStartup instead of WinMainCRTStartup (or mainCRTStartp in the present case)
extern "C" /*BOOL*/int /*WINAPI*/__cdecl _DllMainCRTStartup(
	/*HINSTANCE*/void* const instance, // GetModuleHandle(0);
	/*DWORD*/int     const reason, // 0: dll_detach;   1: dll_attach
	/*LPVOID*/void*    const reserved // 0
);
SB_EXPORT_TYPE int SB_STDCALL init([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	// see ParseIAT to be called before _DllMainCRTStartup
	static volatile void* module = 0;
	return _DllMainCRTStartup(const_cast<void*>(module), 1, 0);
}
SB_EXPORT_TYPE int SB_STDCALL deinit([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	static volatile void* module = 0;
	return _DllMainCRTStartup(const_cast<void*>(module), 0, 0);
}
#endif

#if ( (SB_TARGET_TYPE & SB_TARGET_TYPE_STANDALONE) != 0 )
#define test_main main
#endif
//LibX::Debug::Console debugConsole;
SB_EXPORT_TYPE int SB_STDCALL test_main([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	//LibX::Debug::Console debugConsole;
	//debugConsole.RedirectStdIO();
	CHECK_STDIO();
	//debugConsole.SetUnicodeCodePage();
	//debugConsole.RedirectStdIO();
	CHECK_STDIO();

	// This is the entry point of this process called through the engine (dev or retail), or even as a standalone app.
	//LibX::Debug::Settings debugSettings;
	// TODO: Parse base system configuration parameters / file(s)

	// TODO if ( sb_use_console )
	//{
	//	debugConsole.Show("SBLibX - dev");
	//	debugConsole.RedirectStdIO();
	//	debugConsole.SetUnicodeCodePage();
	//	debugConsole.RedirectStdIO();
	//}

	//LibX::Debug::DebuggerOutput debugOutput(&debugConsole);
	// TODO if ( sb_use_debug_output || IsDebuggerAttached() )
	//{
	//	debugOutput.RedirectStdIO();
	//}

	// Pseudo-code:
	std::clog << "\n-- Startup --";
	//	Parse input parameters / file(s)
	//	if (!dev)
	//		Attach libraries and open communication pipes;
	//
		
	std::cout << "\n-- Update --";
	std::clog << "\n-- Information --";
	std::cerr << "\n-- Validation --";
#if defined( LEAK_TEST )
	std::clog << "\n  - Creating a single leak for report system testing...";
	new int;
#endif // #if defined( LEAK_TEST )

	CHECK_STDIO();
	Test();
	CHECK_STDIO();

	//	while (!finished)
	//	{
	//		if (dev)
	//		{
	//			Detach outdated libraries and closes communication pipes;
	//			Attach libraries and open communication pipes;
	//		}
	//		Update libraries;
	//	}
	std::clog << "\n-- Shutdown --";
	//	Detach libraries and closes communication pipes;
	//std::cin.get();
	// No need for a return here if this function is int main(...) but the function can return a int.
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
