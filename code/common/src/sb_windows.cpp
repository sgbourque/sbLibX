#include <common/include/sb_hash.h>
#include <common/include/sb_interface.h>
#include <common/include/sb_structured_buffer.h>
#include <common/include/sb_common.h>

#include <algorithm>
#include <array>
#include <cstdint>

#define NOMINMAX
#include <windows.h>

#ifndef assert
#include <cassert>
#endif

namespace SB { namespace LibX
{
namespace Windows
{
SB_STRUCT_BEGIN(ApplicationConfiguration, 1)
	SB_STRUCT_MEMBER(system_char_t[128], name) = STR("sbApplication (anonymous)");
	SB_STRUCT_MEMBER(system_char_t[128], className) = STR("sbGenericWindow");
	SB_STRUCT_MEMBER(uint64_t,           windowFlags) = 0;
SB_STRUCT_END(ApplicationConfiguration);

//
//// library exports
//int initialize(const database_t* registry)
//{
//	auto config = GetData<ApplicationConfiguration>(registry, "sbWinConfig"_xhash64);
//	const auto& name = config.name;
//	const auto& className = config.className;
//	HINSTANCE hinstance = GetModuleHandleW(NULL);
//
//	WNDCLASSEXW windowClassProperties = {
//		.cbSize = sizeof(WNDCLASSEXW),
//		.style = CS_HREDRAW | CS_VREDRAW,
//		.lpfnWndProc = MainWndProc,
//		.cbClsExtra = 0,
//		.cbWndExtra = 0,
//		.hInstance = GetModuleHandleW(NULL),
//		.hIcon = LoadIcon(NULL, IDI_WINLOGO),
//		.hCursor = LoadCursor(NULL, IDC_CROSS),
//		.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH),
//		.lpszMenuName = nullptr,
//		.lpszClassName = className,
//		hIconSm = (HICON)LoadImage(hinstance, MAKEINTRESOURCE(5), IMAGE_ICON,
//			GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR)
//};
//	if (!RegisterClassExW(&wcx))
//		return -1;
//
//	[[maybe_unused]] HWND hwnd = CreateWindowExW(
//		WS_EX_NOREDIRECTIONBITMAP,
//		className, name,
//		WS_OVERLAPPEDWINDOW /*WS_POPUP*/ | WS_VISIBLE,
//		CW_USEDEFAULT, CW_USEDEFAULT,
//		CW_USEDEFAULT, CW_USEDEFAULT,
//		nullptr, nullptr, hinstance, nullptr);
//
//	//const auto name  = get<system_string_t>(registry, "applicationName"_xhash64);
//	//const auto flags = get<WindowFlags>(registry, "mainWindowFlags"_xhash64);
//	//system_string name = get<system_string>(registry, "name"_xhash64);
//
//}
//#endif
//
//
//
//#if 0
//void register_class()
//{
//	const auto className = L"SBMainWinClass";
//	HINSTANCE hinstance = GetModuleHandleW(NULL);
//
//	WNDCLASSEXW wcx;
//
//	// Fill in the window class structure with parameters 
//	// that describe the main window. 
//
//	wcx.cbSize = sizeof(wcx);          // size of structure 
//	wcx.style = CS_HREDRAW | CS_VREDRAW; // redraw if size changes 
//	wcx.lpfnWndProc = MainWndProc;       // points to window procedure 
//	wcx.cbClsExtra = 0;                  // no extra class memory 
//	wcx.cbWndExtra = 0;                  // no extra window memory 
//	wcx.hInstance = hinstance;           // handle to instance 
//	wcx.hIcon = LoadIcon(NULL,
//		IDI_WINLOGO);               // predefined app. icon 
//	wcx.hCursor = LoadCursor(NULL,
//		IDC_CROSS);                     // predefined arrow 
//	wcx.hbrBackground = (HBRUSH)GetStockObject(
//		DKGRAY_BRUSH);                   // white background brush 
//	wcx.lpszMenuName = nullptr;       // name of menu resource 
//	wcx.lpszClassName = className;    // name of window class 
//	wcx.hIconSm = (HICON)LoadImage(hinstance,  // small class icon 
//		MAKEINTRESOURCE(5),
//		IMAGE_ICON,
//		GetSystemMetrics(SM_CXSMICON),
//		GetSystemMetrics(SM_CYSMICON),
//		LR_DEFAULTCOLOR);
//
//	if (!RegisterClassExW(&wcx))
//		return -1;
//
//
//	[[maybe_unused]] HWND hwnd = CreateWindowExW(
//		WS_EX_NOREDIRECTIONBITMAP,
//		className, L"SBLibX Test Win32",
//		WS_OVERLAPPEDWINDOW /*WS_POPUP*/ | WS_VISIBLE,
//		CW_USEDEFAULT, CW_USEDEFAULT,
//		CW_USEDEFAULT, CW_USEDEFAULT,
//		nullptr, nullptr, hinstance, nullptr);
//}
//#endif
} // namespace SB::LibX::Windows


	template<typename RETURN_TYPE, typename _IMPLEMENTATION_>
constexpr RETURN_TYPE get([[maybe_unused]] _IMPLEMENTATION_* buffer, std::enable_if_t<!std::is_pointer_v<RETURN_TYPE>, data_info_t> datainfo )
{
	return *reinterpret_cast<RETURN_TYPE*>(reinterpret_cast<uint8_t*>(buffer) + datainfo.offset);
}
	template<typename RETURN_TYPE, typename _IMPLEMENTATION_>
constexpr RETURN_TYPE get([[maybe_unused]] _IMPLEMENTATION_* buffer, std::enable_if_t<std::is_pointer_v<RETURN_TYPE>, data_info_t> datainfo )
{
	return reinterpret_cast<RETURN_TYPE>(reinterpret_cast<uint8_t*>(buffer) + datainfo.offset);
}

	template<typename _IMPLEMENTATION_>
constexpr data_info_t get([[maybe_unused]] _IMPLEMENTATION_* buffer, [[maybe_unused]] xhash_t hash)
{
	// Normally here we should use _IMPLEMENTATION_'s traits
	// to get the key_index from the hash and get the data_info
	// from the corresponding data_index if the hash is valid.
	// TODO : de-hardcode modulo as hash index map.
	if (hash != _IMPLEMENTATION_::hash_traits_t::invalid_hash)
	{
		constexpr size_t key_info_count = _IMPLEMENTATION_::kKeyCount;
		const size_t hint = (hash % key_info_count);
		for (size_t offset = 0; offset < key_info_count; ++offset)
		{
			auto key_info = buffer->key_info[(hint + offset) % key_info_count];
			if (key_info.name_hash.hash == hash)
				return _IMPLEMENTATION_::data_info[key_info.data_index];
#if defined(SBDEBUG)
			std::cerr << "^";
#endif
		}
	}
	return data_info_t{};
}
template<typename _IMPLEMENTATION_> constexpr data_info_t get([[maybe_unused]] _IMPLEMENTATION_& buffer, [[maybe_unused]] xhash_t hash) { return get<_IMPLEMENTATION_>(&buffer, hash); }

	template<typename return_type, typename _implementation_>
constexpr return_type get(_implementation_* buffer, xhash_t hash, return_type value)
{
	const data_info_t datainfo = get(buffer, hash);
	const auto keyinfo = buffer ? buffer->key_info[datainfo.key_index] : decltype(buffer->key_info[datainfo.key_index]){};
	if (keyinfo.name_hash.hash == hash)
	{
		if (keyinfo.type_hash.hash != get_type_hash<return_type>().hash)
			std::cerr << std::endl << "type conversion warning : " << keyinfo.type_hash.name << " -> " << get_type_hash<return_type>().name << std::endl;
		return get<return_type>(buffer, datainfo);
	}
	else
	{
		return value;
	}
}
template<typename return_type, typename _implementation_> constexpr return_type get(_implementation_& buffer, xhash_t hash, return_type value) { return get(&buffer, hash, value); }

#if _MSVC_LANG != 201703
#error "test"
#endif

}} // namespace SB::LibX
namespace sbWinX = SB::LibX::Windows;
//using namespace SB::LibX;

#include <dev/include/sb_dev_debug.h>
#include <iostream>
#include <iomanip>
#include <cstddef>

std::ostream& operator << (std::ostream& os, const wchar_t* wstr)
{
	// poor-man's implementation...
	while (wstr && *wstr)
		os << (char)*(wstr++);
	return os;
}

using sbLibX::xhash_t;
using config_t = sbLibX::Windows::ApplicationConfiguration;
template<xhash_t HASH> using config_traits = config_t::data_traits<HASH>;
template<xhash_t HASH> using key_traits = typename config_traits<HASH>::key_traits_t;

SB_EXPORT_TYPE int SB_STDCALL win32_main([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	using sbLibX::operator ""_xhash64;
	using sbLibX::StructuredBuffer::get;
	using sbLibX::get;
	//sbLibX::Debug::Console debugConsole; debugConsole.RedirectStdIO();

	std::cout << "\nTesting static & dynamic bindings...\n";
	config_t local_config{ L"my application name", L"my class name", 0xFull };
	std::cout << std::endl
		<< "\n" << ".name"_xhash64 << ": " << get<".name"_xhash64>(local_config)
		<< "\n" << ".className"_xhash64 << ": " << get(local_config, ".className"_xhash64, L"")
		<< "\n" << ".windowFlags"_xhash64 << ": " << get(local_config, ".windowFlags"_xhash64, 0ull)
		// following does not compile : customDataFlags is not defined
		//"\n" << ".customDataFlags"_xhash64 << " (unknown): " << sbLibX::get<".customDataFlags"_xhash64>(local_config)
		<< "\n" << ".customDataFlags"_xhash64 << " (unknown): " << get(local_config, ".customDataFlags"_xhash64, 0ull)
		<< std::endl;

	std::cout << "\nTesting dynamic bindings...\n";
	for (auto datainfo : local_config)
	{
		auto hash = local_config.key_info[datainfo.key_index].name_hash;

		// Assume we received some hash to be resolved dynamically
		auto local_datainfo = get(local_config, hash);
		uint8_t* raw_data = get<uint8_t*>(&local_config, local_datainfo);

		auto local_keyinfo  = local_config.key_info[local_datainfo.key_index];
		std::cout << "\n" << local_keyinfo.type_hash << " " << local_keyinfo.name_hash << ":\n\t";
		for ( size_t index = 0; index < local_datainfo.size; ++index )
			std::cout << std::hex << std::setw(2) << std::setfill('0') << uint32_t(raw_data[index]) << " ";
		//std::cout << std::hex << uint32_t(raw_data[index]) << " ";
		std::cout << "\n\t";
		for (size_t index = 0; index < local_datainfo.size; ++index)
			std::cout << (char)raw_data[index];
	}

	std::string input;
	std::cout << std::endl << "Enter a member name (e.g., 'name'): ";
	std::cin >> input;
	{
		auto hash = sbLibX::xhash_traits_t::hash(("." + input).c_str());
		auto local_datainfo = get(local_config, hash);
		uint8_t* raw_data = get<uint8_t*>(&local_config, local_datainfo);
		for (size_t index = 0; index < local_datainfo.size; ++index)
			std::cout << std::hex << std::setw(2) << std::setfill('0') << uint32_t(raw_data[index]) << " ";
		std::cout << "\n\t";
		for (size_t index = 0; index < local_datainfo.size; ++index)
			std::cout << (char)raw_data[index];
	}


	std::cout << std::endl;
	return 0;
}
