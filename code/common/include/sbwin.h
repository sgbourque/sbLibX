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


using result_handle = handle<intptr_t>;
static_assert(sizeof(*result_handle{}) == 1);

using win_proc = result_handle (SB_STDCALL*)(window_handle handle, message_t message, uintptr_t message_data1, intptr_t message_data2);
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

SB_DECLARE_WIN_EXPORT( get_module_handle_proxy, instance_handle, (*)(const_system_string_t module_name), get_module_handle,
	GetModuleHandle, (const_system_string_t module_name = nullptr), (module_name) )
SB_DECLARE_WIN_EXPORT( win_proc, result_handle, (SB_STDCALL*)(window_handle handle, message_t message, uintptr_t message_data1, intptr_t message_data2), default_win_proc,
	DefWindowProc, (window_handle handle, message_t message, uintptr_t message_data1, intptr_t message_data2), (handle, message, message_data1, message_data2) )
SB_DECLARE_WIN_EXPORT( load_icon_proxy, icon_handle, (*)(instance_handle instance, const_system_string_t icon_name), load_icon,
	LoadIcon, (instance_handle instance, const_system_string_t icon_name), (instance, icon_name) )
SB_DECLARE_WIN_EXPORT( create_icon_proxy, icon_handle, (*)(instance_handle instance, int width, int height, uint8_t plane, uint8_t bits_per_pixel, const uint8_t* and_mask, const uint8_t* xor_mask), create_icon,
	CreateIcon, (instance_handle instance, int width, int height, uint8_t plane, uint8_t bits_per_pixel, const uint8_t* and_mask, const uint8_t* xor_mask), (instance, width, height, plane, bits_per_pixel, and_mask, xor_mask) )
SB_DECLARE_WIN_EXPORT( load_cursor_proxy, cursor_handle, (*)(instance_handle instance, const_system_string_t cursor_name), load_cursor,
	LoadCursor, (instance_handle instance, const_system_string_t cursor_name), (instance, cursor_name) )
SB_DECLARE_WIN_EXPORT( get_stock_object_proxy, stock_object_handle, (*)( stock_object_resource resource ), get_stock_object,
	GetStockObject, ( stock_object_resource resource ), ( resource ) )
SB_DECLARE_WIN_EXPORT( load_image_proxy, image_handle, (*)(instance_handle instance, resource_handle_type resource, image_type type, int width, int height, image_load_flags option), load_image,
	LoadImage, (instance_handle instance, resource_handle_type type_name, image_type type, int width, int height, image_load_flags options), (instance, type_name, type, width, height, options) )
SB_DECLARE_WIN_EXPORT( get_system_metric_proxy, int, (*)(system_metric metric), get_system_metric,
	GetSystemMetrics, (system_metric metric), ( metric ) )
SB_DECLARE_WIN_EXPORT( register_class_proxy, uint16_t, (*)(const WindowClass* window_class), register_class,
	RegisterClass, (const WindowClass* window_class), (window_class) )
SB_DECLARE_WIN_EXPORT( unregister_class_proxy, uint32_t, (*)(const_system_string_t class_name, instance_handle instance), unregister_class,
	UnregisterClass, (const_system_string_t window_name, instance_handle instance), (window_name, instance) )
SB_DECLARE_WIN_EXPORT( create_window_proxy, window_handle, (*)(window_style_flags_ex flags_ex, const_system_string_t class_name, const_system_string_t window_name, window_style_flags flags, int pos_x, int pos_y, int width, int height, window_handle parent, menu_handle menu, instance_handle instance, void* user_data), create_window,
	CreateWindow, (window_style_flags_ex flags_ex, const_system_string_t class_name, const_system_string_t window_name, window_style_flags flags, int pos_x, int pos_y, int width, int height, window_handle parent, menu_handle menu, instance_handle instance, void* user_data), (flags_ex, class_name, window_name, flags, pos_x, pos_y, width, height, parent, menu, instance, user_data) )
SB_DECLARE_WIN_EXPORT( get_last_error_proxy, result_t, (*)(), get_last_error,
	GetLastError, (), () )
SB_DECLARE_WIN_EXPORT( show_window_proxy, uint32_t, (*)(window_handle window, show_window_properties properties), show_window,
	ShowWindow, (window_handle window, show_window_properties properties), (window, properties) )
SB_DECLARE_WIN_EXPORT( update_window_proxy, uint32_t, (*)(window_handle window), update_window,
	UpdateWindow, (window_handle window), (window) )
SB_DECLARE_WIN_EXPORT( destroy_window_proxy, uint32_t, (*)(window_handle window), destroy_window,
	DestroyWindow, (window_handle window), (window) )
SB_DECLARE_WIN_EXPORT( post_message_proxy, uint32_t, (SB_STDCALL*)(window_handle handle, message_t message, uintptr_t message_data1, intptr_t message_data2), post_message,
	PostMessage, (window_handle handle, message_t message, uintptr_t message_data1 = {}, intptr_t message_data2 = {} ), ( handle, message, message_data1, message_data2 ))
SB_DECLARE_WIN_EXPORT( post_quit_message_proxy, void, (SB_STDCALL*)( int32_t exit_result ), post_quit_message,
	PostQuitMessage, ( int32_t exit_result = 0 ), ( exit_result ))

// Desktop Window Manager
SB_DECLARE_WIN_EXPORT( dwm_set_window_attribute_proxy, result_t, (SB_STDCALL*)( window_handle handle, dwm_window_attribute attributes, void* attribute_data, uint32_t data_size_bytes ), dwm_set_window_attribute,
	DwmSetWindowAttribute, ( window_handle handle, dwm_window_attribute attributes, void* attribute_data, uint32_t data_size_bytes ), ( handle, attributes, attribute_data, data_size_bytes ))


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


struct vec2
{
	int32_t	x;
	int32_t	y;
};

class window_factory
{
public:
	using style = window_class_style;
	using system_char_t = SB::system_char_t;
	using system_string_t = system_char_t*;
	using const_system_string_t = const system_char_t*;
	window_factory(
		const_system_string_t class_name,
		win_proc process_callback,
		instance_handle instance = nullptr

		, style                 style_flags = {}
		, icon_handle           icon        = {}
		, cursor_handle         cursor      = {}
		, brush_handle          brush       = {}
		, const_system_string_t menu_name   = {}
		, icon_handle           small_icon  = {}
	)
		: window_class{
			SB_STRUCT_SET( .size             , =,  sizeof(window_class) ),
			SB_STRUCT_SET( .style_flags      , =,  style_flags ),
			SB_STRUCT_SET( .process_callback , =,  process_callback ),
			SB_STRUCT_SET( .class_extra_bytes, =,  0 ),
			SB_STRUCT_SET( .window_extra     , =,  0 ),
			SB_STRUCT_SET( .instance         , =,  instance ),
			SB_STRUCT_SET( .icon             , =,  icon ),
			SB_STRUCT_SET( .cursor           , =,  cursor ),
			SB_STRUCT_SET( .brush            , =,  brush ),
			SB_STRUCT_SET( .menu_name        , =,  menu_name ),
			SB_STRUCT_SET( .class_name       , =,  class_name ),
			SB_STRUCT_SET( .small_icon       , =,  small_icon ),
		}
	{
		SB_ASSIGN_IF_NULL( window_class.instance, instance, sbWindows::GetModuleHandle() );
	}

	~window_factory()
	{
		unregister_class();
	}

	SB_CLASS_MEMBER_SET(window_class.style_flags, style, style_flags, { style::vertical_redraw | style::horizontal_redraw }, style{})
	SB_CLASS_MEMBER_SET(window_class.icon, icon_handle, icon, {}, LoadIcon(icon_resource::WinLogo))
	SB_CLASS_MEMBER_SET(window_class.cursor, cursor_handle, cursor, {}, LoadCursor(cursor_resource::Cross));
	SB_CLASS_MEMBER_SET(window_class.brush, brush_handle, brush, {}, GetStockObject(brush_resource::DarkGray));
	SB_CLASS_MEMBER_SET(window_class.small_icon, icon_handle, small_icon, {}, LoadIcon(icon_resource::Shield));

	// TODO : use containers for class_ids/WindowClasses, and window_handles.
	inline constexpr result_t register_class()
	{
		SB_ASSIGN_IF_NULL( class_id, class_id, RegisterClass( &window_class ) );
		return ( class_id != 0 ) ? result_t{} : GetLastError();
	}
	inline constexpr result_t unregister_class()
	{
		result_t result{};
		if ( class_id && !UnregisterClass(window_class.class_name, window_class.instance) )
		{
			result = GetLastError();
		}
		return result;
	}

	static inline constexpr int default_pos = 0x80000000; // TODO: CW_USEDEFAULT
	inline constexpr window_handle create_window(const_system_string_t window_name, window_style style, vec2 pos = { default_pos , default_pos }, vec2 size = { default_pos, default_pos } )
	{
		return CreateWindow(
			style.flags_ex,
			window_class.class_name, window_name,
			style.flags,
			pos.x, pos.y,
			size.x, size.y,
			nullptr, nullptr, window_class.instance, nullptr);
	}
	inline constexpr result_t destroy_window(window_handle hwnd)
	{
		return DestroyWindow( hwnd ) != 0 ? result_t{} : GetLastError();
	}
	//inline constexpr window_handle get_handle() const { return handle; }
	//inline constexpr operator window_handle() const { return get_handle(); }

private:
	using window_class_id = uint16_t;
	WindowClass window_class{};
	window_class_id class_id{};
	//uint16_t pad0;
	//uint32_t pad1;
	//window_handle handle{};
	//std::unordered_map<>
};

//////////////////////////////////////////////////////////////////////////
struct message
{
	window_handle	hwnd;
	message_t     	message_type;
	uintptr_t    	unsigned_param; // wParam: w means 'word' standing for unsigned, I think...
	intptr_t     	signed_param;   // lParam: l means 'long' standing for signed, I think...
	uint16_t     	time_ms;        // Timestamp is in ms relative to some internal timer (probably when the .
	                                // According to https://devblogs.microsoft.com/oldnewthing/20140122-00/?p=2013 : 
	                                // The MSG structure has a field called time which is a DWORD.
	                                // There is also a function Get­Message­Time which returns a LONG.
	                                // While GetTickCount function (sysinfoapi.h) :
	                                // The return value is the number of milliseconds that have elapsed since the system was started.
	vec2         	point;
};

SB_DECLARE_WIN_EXPORT(get_message_proxy, int32_t, (*)(message* msg, window_handle hwnd, uint32_t message_type_begin, uint32_t message_type_last), get_message,
	GetMessage, (message* msg, window_handle hwnd = nullptr, uint32_t message_type_first = 0, uint32_t message_type_last = 0), (msg, hwnd, message_type_first, message_type_last))
SB_DECLARE_WIN_EXPORT(peek_message_proxy, uint32_t, (*)(message* msg, window_handle hwnd, uint32_t message_type_begin, uint32_t message_type_last, uint32_t remove_msg_from_queue), peek_message,
	PeekMessage, (message* msg, window_handle hwnd = nullptr , uint32_t message_type_first = 0, uint32_t message_type_last = 0, uint32_t remove_msg_from_queue = 0), (msg, hwnd, message_type_first, message_type_last, remove_msg_from_queue))
SB_DECLARE_WIN_EXPORT(translate_message_proxy, int32_t, (*)(const message* msg), translate_message,
	TranslateMessage, (const message* msg), (msg))
SB_DECLARE_WIN_EXPORT(dispatch_message_proxy, intptr_t, (*)(const message* msg), dispatch_message,
	DispatchMessage, (const message* msg), (msg))


//WINBASEAPI
//_Ret_maybenull_
//HANDLE
//WINAPI
//CreateThread(
//    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
//    _In_ SIZE_T dwStackSize,
//    _In_ LPTHREAD_START_ROUTINE lpStartAddress,
//    _In_opt_ __drv_aliasesMem LPVOID lpParameter,
//    _In_ DWORD dwCreationFlags,
//    _Out_opt_ LPDWORD lpThreadId
//    );

struct security_attributes
{
	uint32_t nLength;
	void**   lpSecurityDescriptor;
	uint32_t bInheritHandle;
};
using thread_routine = uint32_t (SB_STDCALL *)( void* lpThreadParameter );

using thread_handle = handle<struct thread>;
SB_DECLARE_WIN_EXPORT(create_thread_proxy, thread_handle, (*)( security_attributes* securityAttributes, size_t dwStackSize, thread_routine thread_start, void* parameters, uint32_t createFlags, uint32_t *threadId ), create_thread,
	CreateThread, (thread_routine thread_start, uint32_t createFlags = 0, void* parameters = nullptr, size_t dwStackSize = 0, security_attributes* securityAttributes = nullptr, uint32_t *threadId = nullptr ), (securityAttributes, dwStackSize, thread_start, parameters, createFlags, threadId) )


SB_DECLARE_WIN_EXPORT( wait_for_single_object_proxy, uint32_t, ( * )( void* objectHandle, uint32_t timeout_ms ), wait_for_single_object,
	WaitForSingleObject, (void* objectHandle, uint32_t timeout_ms = ~0u), (objectHandle, timeout_ms) )

SB_DECLARE_WIN_EXPORT( sleep_proxy, void, ( * )( uint32_t time_ms ), sleep_ms,
	Sleep, (uint32_t time_ms = 0), (time_ms) )

} // Windows
}} // sbLibX
namespace sbWindows = SB::LibX::Windows;
