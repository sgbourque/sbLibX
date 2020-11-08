#include <dev/include/sb_dev.h>
#include <dev/include/sb_dev_library.h>
#include <dev/include/sb_dev_debug.h>
using namespace SB;


////
//#pragma warning(disable:4081 4365 4472 4514 4571 4625 4626 4710 4774 4820 5026 5027)
#include <iostream>
#include <string>
#include <type_traits>
//#pragma warning(default)


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


////
	template<size_t kMaxArgc>
int getCommandArgv( const char* argv[kMaxArgc], char* commandline )
{
	// TODO : use a real parser w/ config file/script
	auto get_next = [](char* line) -> char* const
	{
		while (*line && !std::isspace(*line))
		{
			if (*(line++) == '"')
			{
				// single argument between double-quote pairs (if found)
				while (*line && *(line++) != '"');
			}
		}
		if (std::isspace(*line))
		{
			*line = '\0';
			++line;
		}
		return line;
	};

	int argc = 0;
	while (*commandline && argc < kMaxArgc)
	{
		// skip to next argument
		argv[argc++] = commandline;
		commandline = get_next(commandline);
	}
	return argc;
}


	template< typename fct_type/*, typename = std::enable_if_t<std::is_function_v<fct_type>> */>
struct function_exception_helper
{
	function_exception_helper(fct_type _function = nullptr) : function(_function) {}

		template<typename ...ARG_TYPE>
	auto operator ()([[maybe_unused]] ARG_TYPE... args)
	{
		using return_t = decltype(function(args...));
		return_t value{};
		if (function)
		{
			__try
			{
				value = function(args...);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				value = return_t(-1);
				throw std::exception("uncaught exception");
			}
		}
		return value;
	}
	fct_type function = nullptr;
};

	template< typename fct_type/*, typename = std::enable_if_t<std::is_function_v<fct_type>> */>
function_exception_helper<fct_type> unsafe_entry([[maybe_unused]] fct_type _function)
{
	return function_exception_helper<fct_type>(_function);
}


#include <future>
#include <thread>

//#pragma warning(disable:4711) // main can be inlined
#pragma comment(linker, "/subsystem:windows")
int WinMain(
	[[maybe_unused]] _In_ HINSTANCE hInstance,	[[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
	[[maybe_unused]] _In_ LPSTR lpCmdLine,
	[[maybe_unused]] _In_ int nShowCmd
)
{
	LibX::Debug::Settings debugSettings;
	// TODO: Parse base system configuration parameters / file(s)

	LibX::Debug::Console debugConsole;
	// TODO if ( sb_use_console )
	{
		debugConsole.Show("SBLibX - dev");
		//debugConsole.SetUnicodeCodePage();
		debugConsole.RedirectStdIO();
	}

	LibX::Debug::DebuggerOutput debugOutput( &debugConsole );
	// TODO if ( sb_use_debug_output || IsDebuggerAttached() )
	{
		debugOutput.RedirectStdIO();
	}

#if SB_SUPPORTS(SBDEBUG_OUTPUT)
	{
		std::clog.flush();
		std::clog << "---------------------------";
		std::clog.flush();
		std::wclog << L"\n- Welcome to SBLibX© - dev -";
		std::wclog.flush();
		std::clog << "\n---------------------------";
		std::clog << std::endl;
		std::cerr.flush();
	}
#endif

	// TODO : call any setup/update library first

	// TODO : cleanup that mess (this is a proof-of-concept)
	enum
	{
		kError=-1,
		kQuit,
		kContinue,
		kExecute,
	};

	using main_t = int(SB_STDCALL*)([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[]);
	int return_code = kContinue;

	constexpr size_t kMaxCommandline = 8192; // 8191 + 1. c.f., https://support.microsoft.com/en-us/help/830473/command-prompt-cmd-exe-command-line-string-limitation
	using sys_char_t = std::remove_cv_t<std::remove_reference_t<decltype(*lpCmdLine)>>;
	std::basic_string<sys_char_t> localCmdLine(lpCmdLine, kMaxCommandline);

	std::string module_name;
	std::string function_name;
	LibX::Dev::library main_module;
	main_t local_main{};
	if (hInstance != 0) do
	{
		constexpr size_t kMaxArgC = kMaxCommandline / 2; // with every argument got 1 char we can reach the the max argument count possible
		const char* argv[kMaxArgC]{};
		int argc = getCommandArgv<kMaxArgC>(argv, localCmdLine.data());
		int entryArgOffset = 0;

		{
			// TODO : better parser!! This is a quick & dirty one
			for (int arg = 0; arg < argc; ++arg)
			{
				const char* curArg = argv[arg];
				while (*curArg == '-' || *curArg == '/')
					++curArg;

				auto currentView = std::string_view(curArg);
				if (currentView == "help")
				{
					entryArgOffset = argc; // skip the whole line (maybe you asked help for a specific command?)
					std::cout << "Type 'quit' or 'exit' (without the quotes) and have a look at the code while it's being implemented..." << std::endl;
					return_code = kContinue;
				}
				else if (currentView == "quit" || currentView == "exit")
				{
					module_name.clear();
					function_name.clear();
					return_code = kQuit;
					break;
				}
				else if (currentView == "run")
				{
					entryArgOffset = arg;
					return_code = kExecute;
					if (!main_module && (++arg < argc))
						module_name = argv[arg];
					if (++arg < argc)
						function_name = argv[arg];
					break;
				}
				else if (currentView == "load")
				{
					if (++arg < argc)
						module_name = argv[arg];
				}
				else if (currentView == "entry")
				{
					if (++arg < argc)
						function_name = argv[arg];
				}
				else if (currentView == "unload")
				{
					module_name = function_name = std::string();
					if (++arg < argc)
						sb_noop(argv + arg, argc - arg); // noop : TODO : expects module name here!
				}
				else
				{
					std::cerr << "unrecognised command '" << curArg << "'" << std::endl;
					return_code = kError;
				}
			}

			// load library & find entry function
			if (!module_name.empty())
			{
				auto module_separator = module_name.find_first_of('/');
				if (module_separator != std::string::npos)
				{
					function_name = std::string_view(module_name.data() + module_separator + 1, module_name.length() - module_separator - 1);
					module_name = std::string_view(module_name.data(), module_separator);
					local_main = nullptr;
				}

				if (!main_module)
				{
					if (main_module.load(module_name.c_str()))
						std::cout << "library '" << main_module.name() << "' loaded\n";
					local_main = nullptr;
				}
				if (!main_module)
				{
					std::cerr << "error: cannot load library '" << main_module.name() << "'.\n";
					module_name.clear();
					local_main = nullptr;
				}
			}
			else if (main_module)
			{
				auto name = main_module.name();
				if (main_module.unload())
					std::cout << "library '" << name << "' unloaded\n";
			}

			if (main_module && !function_name.empty())
			{
				local_main = main_module[ function_name ];
				if( !local_main )
					local_main = main_module[ main_module.alias() + "_" + function_name ];
				if (local_main)
					std::cout << "function '" << function_name << "' found\n";
				else
					local_main = nullptr;
			}
			else if (local_main)
			{
				local_main = nullptr;
			}
		}

		if (return_code != kQuit)
		{
			if (!module_name.empty() && !main_module)
			{
				// unable to load module
				return_code = kError;
			}
			else if (!function_name.empty() && !local_main)
			{
				// unable to find entry point
				std::cerr << "cannot find entry '" << function_name << "'" << std::endl;
				return_code = kError;
			}
			else if (!local_main && return_code == kExecute)
			{
				std::cerr << "(no entry)" << std::endl;
				return_code = kError;
			}
			else if (return_code == kExecute)
			{
				// TODO : use config file to get input parameters back into dll
				std::cerr << "-- '" << module_name << "/" << function_name << "' --" << std::endl;
				int local_argc = std::max(0, argc - entryArgOffset);
				const char** local_argv = local_argc > 0 ? &argv[entryArgOffset] : nullptr;
				{
					// run on a separate thread (still sync'ed for now)
					// Note : running on a separate thread is needed for any DLL that requires a particular thread appartment models (as ASIO).
					std::packaged_task<int()> run_task(
						[&local_main, &module_name, &function_name, local_argc, local_argv]()
						{
							decltype(local_main(local_argc, local_argv)) return_code = kError;
							try
							{
								return_code = unsafe_entry(local_main)(local_argc, local_argv);
							}
							catch (std::exception except)
							{
								std::cerr << except.what() << " in '" << module_name << "/" << function_name << "'" << std::endl;
							}
							return return_code;
						}
					);
					std::future<int> result = run_task.get_future();
					std::thread(std::move(run_task)).detach();
					return_code = result.get();
				}
				//debugConsole.RedirectStdIO();
				//module_name.clear();
				function_name.clear();
				std::cerr << "\n\\_ Return code: " << return_code << std::endl;
			}

			std::cout << "sbDev>";
			std::cin.getline(localCmdLine.data(), kMaxCommandline);
			return_code = kContinue;
		}
	}
	while (return_code != kQuit);

	PostQuitMessage(return_code);
#if SB_SUPPORTS(SBDEBUG_OUTPUT)
	std::cerr.flush();
	std::clog.flush();
	std::clog << "\n\\_ Return code: " << return_code;
	std::clog << std::endl;
	std::clog << "\n---------------";
	std::clog << "\n-   Goodbye   -";
	std::clog << "\n---------------";
	std::clog << std::endl;
#endif
	return return_code;
}
#endif
