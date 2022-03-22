#include <common/include/sbwin.h>

#include <common/include/sb_hash.h>
//#include <common/include/sb_interface.h>
#include <common/include/sb_structured_buffer.h>
#include <common/include/sb_common.h>

#include <algorithm>
#include <array>
#include <cstdint>

#define NOMINMAX
#include <windows.h>
// for GetStockObject
#include <wingdi.h>
#pragma comment( lib, "Gdi32.lib" )
// for LoadImage
#include <winuser.h>
#pragma comment( lib, "User32.lib" )

#ifndef assert
#include <cassert>
#endif



//SB_IMPORT_LIB HICON LoadIconW(_In_opt_ HINSTANCE hInstance, _In_ LPCWSTR lpIconName);

namespace SB { namespace LibX {
namespace Windows  {

#define SB_EXPORT_WIN_FUNCTION( type, name, function, first_ops, second_ops ) \
	SB_EXPORT_LIB type name = reinterpret_cast<type>( &function ); \
	static_assert( sizeof( name ) == sizeof( &function ) ); \
	static_assert( sizeof( name first_ops ) == sizeof( function second_ops ) )


SB_EXPORT_WIN_FUNCTION(get_module_handle_t, get_module_instance_handle, ::GetModuleHandleW, (nullptr), (nullptr) );
SB_EXPORT_WIN_FUNCTION(win_proc, default_win_proc, ::DefWindowProcW, (nullptr, 0,0,0), (nullptr, 0,0,0));
SB_EXPORT_WIN_FUNCTION(load_icon_t, load_icon_handle, ::LoadIconW, (nullptr,nullptr), (nullptr,nullptr));
SB_EXPORT_WIN_FUNCTION(load_cursor_t, load_cursor_handle, ::LoadCursorW, (nullptr, nullptr), (nullptr, nullptr));
SB_EXPORT_WIN_FUNCTION(get_stock_object_t, get_stock_object_handle, ::GetStockObject, (stock_object_resource{}), (underlying(stock_object_resource{})));
SB_EXPORT_WIN_FUNCTION(load_image_t, load_image_handle, ::LoadImageW, (nullptr, resource_handle_type{}, image_type{}, 0, 0, image_load_flags{}), (nullptr, (LPCWSTR)resource_handle_type{}, underlying(image_type{}), 0, 0, underlying(image_load_flags{})));
SB_EXPORT_WIN_FUNCTION(get_system_metric_t, get_system_metric, ::GetSystemMetrics, (system_metric{}), (underlying(system_metric{})));
SB_EXPORT_WIN_FUNCTION(register_class_t, register_class, ::RegisterClassExW, (nullptr), (nullptr));
SB_EXPORT_WIN_FUNCTION(unregister_class_t, unregister_class, ::UnregisterClassW, (nullptr, nullptr), (nullptr, nullptr));
SB_EXPORT_WIN_FUNCTION(get_last_error_t, get_last_error, ::GetLastError, (), ());
SB_EXPORT_WIN_FUNCTION(create_window_t, create_window, ::CreateWindowExW, (window_style_flags_ex{}, const_system_string_t{}, const_system_string_t{}, window_style_flags{}, 0, 0, 0, 0, window_handle{}, menu_handle{}, instance_handle{}, nullptr), (underlying(window_style_flags_ex{}), (LPCWSTR)const_system_string_t {}, (LPCWSTR)const_system_string_t {}, underlying(window_style_flags{}), 0, 0, 0, 0, (HWND)window_handle {}, (HMENU)menu_handle {}, (HINSTANCE)instance_handle {}, nullptr));
SB_EXPORT_WIN_FUNCTION(show_window_t, show_window, ::ShowWindow, (window_handle{}, int{}), (nullptr, 0));
SB_EXPORT_WIN_FUNCTION(update_window_t, update_window, ::UpdateWindow, (window_handle{}), (nullptr));
SB_EXPORT_WIN_FUNCTION(destroy_window_t, destroy_window, ::DestroyWindow, (window_handle{}), (nullptr));

SB_EXPORT_WIN_FUNCTION(get_message_t, get_message, ::GetMessage, (nullptr, nullptr, 0,0), (nullptr, nullptr, 0, 0));
SB_EXPORT_WIN_FUNCTION(translate_message_t, translate_message, ::TranslateMessage, (nullptr), (nullptr));
SB_EXPORT_WIN_FUNCTION(dispatch_message_t, dispatch_message, ::DispatchMessage, (nullptr), (nullptr));

//SB_WIN_EXPORT result_t window::register_class() const
//{
//	HRESULT result = S_OK;
//	if ( !::RegisterClassExW( reinterpret_cast<const WNDCLASSEXW*>( &window_class ) ) )
//	{
//		result = GetLastError();
//	}
//	return result;
//}

//SB_WIN_EXPORT window_handle window::create_window(const_system_string_t window_name, window_style style)
//{
//	HWND hwnd = ::CreateWindowExW(
//		underlying( style.flags_ex ),
//		reinterpret_cast<LPCWSTR>( window_class.class_name ), reinterpret_cast<LPCWSTR>( window_name ),
//		underlying( style.flags ),
//		CW_USEDEFAULT, CW_USEDEFAULT,
//		CW_USEDEFAULT, CW_USEDEFAULT,
//		nullptr, nullptr, reinterpret_cast<HINSTANCE>( window_class.instance ), nullptr);
//	return reinterpret_cast<window_handle>( hwnd );
//}


#if 1
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// random tests (WIP at a random point in time)...

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
			if (key_info.name_hash.get_key() == hash)
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
	if (keyinfo.name_hash.get_key() == hash)
	{
		if (keyinfo.type_hash.get_key() != get_type_hash<return_type>().get_key())
			std::cerr << std::endl << "type conversion warning : " << keyinfo.type_hash.get_value().decrypt() << " -> " << get_type_hash<return_type>().get_value().decrypt() << std::endl;
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
namespace sbWindows = SB::LibX::Windows;
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

SB_EXPORT_TYPE int SB_STDCALL main([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	using sbLibX::operator ""_xhash64;
	using sbLibX::StructuredBuffer::get;
	using sbLibX::get;
	//sbLibX::Debug::Console debugConsole; debugConsole.RedirectStdIO();

	std::cout << "\nTesting static & dynamic bindings...\n";
	config_t local_config{ u"my application name", u"my class name", 0xFull };
	std::cout << std::endl
		<< "\n" << ".name"_xhash64 << ": " << get<".name"_xhash64>(local_config)
		<< "\n" << ".className"_xhash64 << ": " << get(local_config, ".className"_xhash64, L"")
		<< "\n" << ".windowFlags"_xhash64 << ": " << get(local_config, ".windowFlags"_xhash64, 0ull)
		// following does not compile : customDataFlags is not defined
		//"\n" << ".customDataFlags"_xhash64 << " (unknown): " << sbLibX::get<".customDataFlags"_xhash64>(local_config)
		<< "\n" << ".customDataFlags"_xhash64 << " (unknown): " << get(local_config, ".customDataFlags"_xhash64, 0ull)
		<< std::endl;

	std::cout << "\nTesting dynamic bindings...\n";
	std::for_each(local_config.info_begin(), local_config.info_end(), [&local_config]( const auto& datainfo )
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
	});

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
#endif
