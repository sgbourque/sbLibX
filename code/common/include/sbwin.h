#pragma once
#include <common/include/sb_common.h>
#include <common/include/win32/sbwin_result.h>
#include <common/include/win32/sbwin_enums.h>
#include "common/include/sb_utilities.h"

#ifndef SB_WIN_EXPORT
#define SB_WIN_EXPORT SB_IMPORT_LIB
#endif

namespace SB { namespace LibX {
namespace Windows {


template<typename __OBJ_T_> struct handle_ref {};

	template<typename __OBJ_T_>
using handle = struct handle_ref<__OBJ_T_>*;


using instance_handle = handle<struct instance>;
using window_handle = handle<class window>;
using menu_handle = handle<class menu>;
using icon_handle = handle<class icon>;
using cursor_handle = handle<class cursor>;
using stock_object_handle = handle<class stock_object>;
using brush_handle = handle<class brush>;
using image_handle = handle<class image>;


using win_proc = intptr_t (SB_STDCALL*)(window_handle handle, uint32_t message, uintptr_t message_data1, intptr_t message_data2);
struct WindowClass {
	uint32_t             	size;
	window_class_style   	style_flags;
	win_proc             	process_callback;
	int32_t              	class_extra_bytes; // The number of extra bytes to allocate following the window-class structure. The system initializes the bytes to zero.
	int32_t              	window_extra; // The number of extra bytes to allocate following the window instance. The system initializes the bytes to zero.
	instance_handle      	instance;
	icon_handle          	icon;
	cursor_handle        	cursor;
	brush_handle         	brush;
	const_system_string_t	menu_name;
	const_system_string_t	class_name;
	icon_handle          	small_icon;
};

struct window_style
{
	window_style_flags    flags;
	window_style_flags_ex flags_ex;
};


#define SB_DECLARE_WIN_EXPORT( type_name, return_type, declare_arguments, export_name, function, arguments_default, arguments ) \
	using type_name = return_type declare_arguments; \
	extern SB_WIN_EXPORT type_name export_name; \
	static inline constexpr return_type function arguments_default \
	{ \
		return export_name arguments; \
	}

SB_DECLARE_WIN_EXPORT( get_module_handle_t, instance_handle, (*)(const_system_string_t module_name), get_module_instance_handle,
	GetModuleHandle, (const_system_string_t module_name = nullptr), (module_name) )
SB_DECLARE_WIN_EXPORT( win_proc, intptr_t, (SB_STDCALL*)(window_handle handle, uint32_t message, uintptr_t message_data1, intptr_t message_data2), default_win_proc,
	DefWindowProc, (window_handle handle, uint32_t message, uintptr_t message_data1, intptr_t message_data2), (handle, message, message_data1, message_data2) )
SB_DECLARE_WIN_EXPORT( load_icon_t, icon_handle, (*)(instance_handle instance, const_system_string_t icon_name), load_icon_handle,
	LoadIcon, (instance_handle instance, const_system_string_t icon_name), (instance, icon_name) )
SB_DECLARE_WIN_EXPORT( load_cursor_t, cursor_handle, (*)(instance_handle instance, const SB::system_char_t* cursor_name), load_cursor_handle,
	LoadCursor, (instance_handle instance, const_system_string_t cursor_name), (instance, cursor_name) )
SB_DECLARE_WIN_EXPORT( get_stock_object_t, stock_object_handle, (*)( stock_object_resource resource ), get_stock_object_handle,
	GetStockObject, ( stock_object_resource resource ), ( resource ) )
SB_DECLARE_WIN_EXPORT( load_image_t, image_handle, (*)(instance_handle instance, resource_handle_type resource, image_type type, int width, int height, image_load_flags option), load_image_handle,
	LoadImage, (instance_handle instance, resource_handle_type type_name, image_type type, int width, int height, image_load_flags options), (instance, type_name, type, width, height, options) )
SB_DECLARE_WIN_EXPORT( get_system_metric_t, int, (*)(system_metric metric), get_system_metric,
	GetSystemMetrics, (system_metric metric), ( metric ) )
SB_DECLARE_WIN_EXPORT( register_class_t, uint16_t, (*)(const WindowClass* window_class), register_class,
	RegisterClass, (const WindowClass* window_class), (window_class) )
SB_DECLARE_WIN_EXPORT( unregister_class_t, uint32_t, (*)(const_system_string_t class_name, instance_handle instance), unregister_class,
	UnregisterClass, (const_system_string_t window_name, instance_handle instance), (window_name, instance) )
SB_DECLARE_WIN_EXPORT( create_window_t, window_handle, (*)(window_style_flags_ex flags_ex, const_system_string_t class_name, const_system_string_t window_name, window_style_flags flags, int pos_x, int pos_y, int width, int height, window_handle parent, menu_handle menu, instance_handle instance, void* user_data), create_window,
	CreateWindow, (window_style_flags_ex flags_ex, const_system_string_t class_name, const_system_string_t window_name, window_style_flags flags, int pos_x, int pos_y, int width, int height, window_handle parent, menu_handle menu, instance_handle instance, void* user_data), (flags_ex, class_name, window_name, flags, pos_x, pos_y, width, height, parent, menu, instance, user_data))
SB_DECLARE_WIN_EXPORT(get_last_error_t, result_t, (*)(), get_last_error,
	GetLastError, (), ())
SB_DECLARE_WIN_EXPORT(show_window_t, uint32_t, (*)(window_handle window, int properties), show_window,
	ShowWindow, (window_handle window, int properties), (window, properties))
SB_DECLARE_WIN_EXPORT(update_window_t, uint32_t, (*)(window_handle window), update_window,
	UpdateWindow, (window_handle window), (window))
SB_DECLARE_WIN_EXPORT(destroy_window_t, uint32_t, (*)(window_handle window), destroy_window,
	DestroyWindow, (window_handle window), (window))


	template<typename resource_type>
static inline const_system_string_t get_resource_handle(resource_type resource)
{
	return reinterpret_cast<const_system_string_t>(std::underlying_type_t<resource_type>(resource));
}
static inline icon_handle LoadIcon(icon_resource resource)
{
	return LoadIcon(nullptr, get_resource_handle(resource));
}
static inline cursor_handle LoadCursor(cursor_resource resource)
{
	return LoadCursor(nullptr, get_resource_handle(resource));
}
static inline brush_handle GetStockObject(brush_resource resource)
{
	return reinterpret_cast<brush_handle>(GetStockObject( stock_object_resource(underlying(resource)) ) );
}
//static inline icon_handle load_module_small_icon(instance_handle instance)
//{
//	return reinterpret_cast<icon_handle>(LoadImage(instance, resource_handle_type::Dialog, image_type::Icon, GetSystemMetrics(system_metric::small_icon_width), GetSystemMetrics(system_metric::small_icon_height), image_load_flags::DefaultColor));
//}


class window
{
public:
	using style = window_class_style;
	using system_char_t = SB::system_char_t;
	using system_string_t = system_char_t*;
	using const_system_string_t = const system_char_t*;
	window(
		const const_system_string_t class_name,
		win_proc process_callback,
		style style_flags = { style::vertical_redraw | style::horizontal_redraw },
		icon_handle icon = nullptr,
		cursor_handle cursor = nullptr,
		brush_handle brush = nullptr,
		const const_system_string_t menu_name = nullptr,
		icon_handle small_icon = nullptr
	)
	{
		[[maybe_unused]] instance_handle instance = sbWindows::GetModuleHandle();
		if (!icon)
		{
			icon = LoadIcon(icon_resource::WinLogo);
		}
		if (!cursor)
		{
			cursor = LoadCursor(cursor_resource::Cross);
		}
		if (!brush)
		{
			brush = GetStockObject(brush_resource::DarkGray);
		}
		if (!small_icon)
		{
			small_icon = LoadIcon(icon_resource::Shield);
		}

		window_class = WindowClass{
			/*uint32_t           */	SB_STRUCT_SET( .size              = ) sizeof(WindowClass),
			/*style              */	SB_STRUCT_SET( .style_flags       = ) style_flags,
			/*win_proc           */	SB_STRUCT_SET( .process_callback  = ) process_callback,
			/*int32_t	         */	SB_STRUCT_SET( .class_extra_bytes = ) 0,
			/*int32_t	         */	SB_STRUCT_SET( .window_extra      = ) 0,
			/*instance_handle    */	SB_STRUCT_SET( .instance          = ) instance,
			/*icon_handle        */	SB_STRUCT_SET( .icon              = ) icon,
			/*cursor_handle      */	SB_STRUCT_SET( .cursor            = ) cursor,
			/*brush_handle       */	SB_STRUCT_SET( .brush             = ) brush,
			/*const_system_string*/	SB_STRUCT_SET( .menu_name         = ) menu_name,
			/*const_system_string*/	SB_STRUCT_SET( .class_name        = ) class_name,
			/*icon_handle        */	SB_STRUCT_SET( .small_icon        = ) small_icon,
		};
	}
	~window()
	{
		unregister_class();
	}

	inline constexpr result_t register_class()
	{
		classId = RegisterClass( &window_class );
		return ( classId != 0 ) ? result_t{} : GetLastError();
	}
	inline constexpr result_t unregister_class()
	{
		result_t result{};
		if (classId && !UnregisterClass(window_class.class_name, window_class.instance) )
		{
			result = GetLastError();
		}
		return result;
	}
	inline constexpr window_handle create_window(const_system_string_t window_name, window_style style)
	{
		constexpr int default_pos = 0x80000000; // TODO: CW_USEDEFAULT
		handle = CreateWindow(
			style.flags_ex,
			window_class.class_name, window_name,
			style.flags,
			default_pos, default_pos,
			default_pos, default_pos,
			nullptr, nullptr, window_class.instance, nullptr);
		return handle;
	}
	inline constexpr window_handle get_handle() const { return handle; }
	inline constexpr operator window_handle() const { return get_handle(); }

private:
	WindowClass window_class{};
	uint16_t classId{};
	//uint16_t pad0;
	//uint32_t pad1;
	window_handle handle{};
};

//////////////////////////////////////////////////////////////////////////
struct POINT
{
	int32_t	x;
	int32_t	y;
};
struct MSG
{
	window_handle	hwnd;
	uint32_t     	message;
	uintptr_t    	wParam;
	intptr_t     	lParam;
	uint16_t     	time;
	POINT        	point;
};

SB_DECLARE_WIN_EXPORT(get_message_t, int32_t, (*)(MSG* msg, window_handle hwnd, uint32_t messageFirst, uint32_t messageLast), get_message,
	GetMessage, (MSG* msg, window_handle hwnd = nullptr , uint32_t messageFirst = 0, uint32_t messageLast = 0), (msg, hwnd, messageFirst, messageLast))
SB_DECLARE_WIN_EXPORT(translate_message_t, int32_t, (*)(const MSG* msg), translate_message,
	TranslateMessage, (const MSG* msg), (msg))
SB_DECLARE_WIN_EXPORT(dispatch_message_t, intptr_t, (*)(const MSG* msg), dispatch_message,
	DispatchMessage, (const MSG* msg), (msg))


} // Windows
}} // sbLibX
namespace sbWindows = SB::LibX::Windows;
