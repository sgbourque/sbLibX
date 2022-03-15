#pragma once
#include <common/include/sb_common.h>
#include <common/include/win32/sbwin_result.h>
#include "common/include/sb_utilities.h"

#ifndef SB_WIN_EXPORT
#define SB_WIN_EXPORT SB_IMPORT_LIB
#endif

namespace SB { namespace LibX {
namespace Windows {

template<typename __OBJ_T_> struct handle_ref {};
	template<typename __OBJ_T_>
using handle = struct handle_ref<__OBJ_T_>*;

#define SB_HANDLE(name) using name##_handle = handle<struct name##__>;
SB_HANDLE(instance);



using get_module_handle_t = instance_handle(*)( const_system_string_t module_name );
extern SB_WIN_EXPORT get_module_handle_t get_module_instance_handle;
static inline constexpr instance_handle get_module_handle(const SB::system_char_t* module_name = nullptr)
{
	return get_module_instance_handle(module_name);
}


using window_handle = handle<class window>;
using win_proc = result_t(SB_STDCALL*)(window_handle, uint32_t, uintptr_t, int64_t);


	template<typename resource_type>
static inline const_system_string_t get_resource_handle(resource_type resource)
{
	return reinterpret_cast<const_system_string_t>(std::underlying_type_t<resource_type>(resource));
}


enum class icon_resource : uintptr_t {
	Sample      = 32512,
	Hand        = 32513,
	Question    = 32514,
	Bang        = 32515,
	Note        = 32516,
	WinLogo     = 32517,
	Shield      = 32518,

	// alias
	Warning     = Bang,
	Error       = Hand,
	Information = Note,
};
using icon_handle = handle<class icon>;
using load_icon_t = icon_handle(*)(instance_handle instance, const_system_string_t icon_name);
extern SB_WIN_EXPORT load_icon_t load_icon_handle;
static inline constexpr icon_handle load_icon( instance_handle instance, const_system_string_t icon_name )
{
	return load_icon_handle(instance, icon_name);
}
static inline icon_handle load_icon( icon_resource resource )
{
	return load_icon( nullptr, get_resource_handle(resource) );
}

enum class cursor_resource : uintptr_t {
	Arrow           = 32512,
	IBeam           = 32513,
	Wait            = 32514,
	Cross           = 32515,
	UpArrow         = 32516,
	Size            = 32640,  /* OBSOLETE: use IDC_SIZEALL */
	Icon            = 32641,  /* OBSOLETE: use IDC_ARROW */
	SizeNWSE        = 32642,
	SizeNESW        = 32643,
	SizeWE          = 32644,
	SizeNS          = 32645,
	SizeAll         = 32646,
	No              = 32648, /*not in win3.1 */
	Hand            = 32649,
	AppStarting     = 32650, /*not in win3.1 */
	Help            = 32651,
	Pin             = 32671,
	Person          = 32672,
};
using cursor_handle = handle<class cursor>;
using load_cursor_t = cursor_handle(*)(instance_handle instance, const SB::system_char_t* cursor_name);
extern SB_WIN_EXPORT load_cursor_t load_cursor_handle;
static inline constexpr cursor_handle load_cursor( instance_handle instance, const_system_string_t cursor_name )
{
	return load_cursor_handle(instance, cursor_name);
}
static inline cursor_handle load_cursor( cursor_resource resource )
{
	return load_cursor( nullptr, get_resource_handle(resource) );
}


using brush_handle = handle<class brush>;

//using const_system_string = const system_char_t*;

struct WindowClass {
	enum class style : uint32_t {
		vertical_redraw       = ( 1 <<  0 ), // Redraws the entire window if a movement or size adjustment changes the height of the client area.
		horizontal_redraw     = ( 1 <<  1 ), // Redraws the entire window if a movement or size adjustment changes the width of the client area.

		// undefined	     
		double_clicks         = ( 1 <<  3 ), // Sends a double - click message to the window procedure when the user double
		                                    // - clicks the mouse while the cursor is within a window belonging to the class.

		// undefined         
		own_device_context    = ( 1 <<  5 ), // Allocates a unique device context for each window in the class.
		class_device_context  = ( 1 <<  6 ), // Allocates one device context to be shared by all windows in the class.
		                                     // Because window classes are process specific, it is possible for multiple threads
		                                     // of an application to create a window of the same class. It is also possible for the
		                                     // threads to attempt to use the device context simultaneously. When this happens,
		                                     // the system allows only one thread to successfully finish its drawing operation.
		                     
		parent_device_context = ( 1 <<  7 ), // Sets the clipping rectangle of the child window to that of the parent window
		                                     // so that the child can draw on the parent.A window with the CS_PARENTDC style bit
		                                     // receives a regular device context from the system's cache of device contexts.
		                                     // It does not give the child the parent's device context or device context settings.
		                                     // Specifying CS_PARENTDC enhances an application's performance.
		                     
		// undefined         
		no_close              = ( 1 <<  9 ), // Disables Close on the window menu.

		// undefined         
		save_bitmap           = ( 1 << 11 ), // Saves, as a bitmap, the portion of the screen image obscured by a window of this class.
		                                     // When the window is removed, the system uses the saved bitmap to restore the screen image,
		                                     // including other windows that were obscured. Therefore, the system does not send WM_PAINT
		                                     // messages to windows that were obscured if the memory used by the bitmap has not been
		                                     // discarded and if other screen actions have not invalidated the stored image.
		                                     // 
		                                     // This style is useful for small windows (for example, menus or dialog boxes) that are
		                                     // displayed briefly and then removed before other screen activity takes place.
		                                     // This style increases the time required to display the window, because
		                                     // the system must first allocate memory to store the bitmap.
		                     
		byte_align_client     = ( 1 << 12 ), // Aligns the window's client area on a byte boundary (in the x direction).
		                                     // This style affects the width of the window and its horizontal placement on the display. 
		                     
		byte_align_window     = ( 1 << 13 ), // Aligns the window on a byte boundary (in the x direction). This style
		                                     // affects the width of the window and its horizontal placement on the display.

		global_class          = ( 1 << 14 ), // Indicates that the window class is an application global class. For more information,
		                                     // see the "Application Global Classes" section of About Window Classes.

		// undefined         
		IME                   = ( 1 << 16 ), // Undocumented
		drop_shadow           = ( 1 << 17 ), // Enables the drop shadow effect on a window. The effect is turned on and off
		                                     // through SPI_SETDROPSHADOW. Typically, this is enabled for small, short-lived windows
		                                     // such as menus to emphasize their Z-order relationship to other windows.
		                                     // Windows created from a class with this style must be top-level windows;
		                                     // they may not be child windows.
	};

	uint32_t             	size;
	style                	style_flags;
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
sb_enum_class_flags(WindowClass::style);

} // Windows
}} // sbLibX
namespace sbWindows = SB::LibX::Windows;
