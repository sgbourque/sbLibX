#pragma once
#include "common/include/sb_utilities.h"

namespace SB { namespace LibX {
namespace Windows {

// WIP : TODO (c.f., sbwin_result.h)

enum class icon_resource : uintptr_t
{
//#include "common/include/internal/win32/win32_icon_resources.h"
	Sample   = 32512,
	Hand     = 32513,
	Question = 32514,
	Bang     = 32515,
	Note     = 32516,
	WinLogo  = 32517,
	Shield   = 32518,

	// alias
	Warning     = Bang,
	Error       = Hand,
	Information = Note,
};

enum class cursor_resource : uintptr_t {
//#include "common/include/internal/win32/win32_cursor_resources.h"
	Arrow = 32512,
	IBeam = 32513,
	Wait = 32514,
	Cross = 32515,
	UpArrow = 32516,
	Size = 32640,  /* OBSOLETE: use IDC_SIZEALL */
	Icon = 32641,  /* OBSOLETE: use IDC_ARROW */
	SizeNWSE = 32642,
	SizeNESW = 32643,
	SizeWE = 32644,
	SizeNS = 32645,
	SizeAll = 32646,
	No = 32648, /*not in win3.1 */
	Hand = 32649,
	AppStarting = 32650, /*not in win3.1 */
	Help = 32651,
	Pin = 32671,
	Person = 32672,
};

enum class stock_object_resource : int
{
	// brush
	White_Brush = 0,
	LightGray_Brush = 1,
	Gray_Brush = 2,
	DarkGray_Brush = 3,
	Black_Brush = 4,
	Null_Brush = 5,
	Hollow_Brush = Null_Brush,

	//WHITE_PEN           6
	//BLACK_PEN           7
	//NULL_PEN            8
	//OEM_FIXED_FONT      10
	//ANSI_FIXED_FONT     11
	//ANSI_VAR_FONT       12
	//SYSTEM_FONT         13
	//DEVICE_DEFAULT_FONT 14
	//DEFAULT_PALETTE     15
	//SYSTEM_FIXED_FONT   16
	//DEFAULT_GUI_FONT    17
};
enum class brush_resource : std::underlying_type_t<stock_object_resource>
{
	White     = underlying( stock_object_resource::White_Brush     ),
	LightGray = underlying( stock_object_resource::LightGray_Brush ),
	Gray      = underlying( stock_object_resource::Gray_Brush      ),
	DarkGray  = underlying( stock_object_resource::DarkGray_Brush  ),
	Black     = underlying( stock_object_resource::Black_Brush     ),
	Null      = underlying( stock_object_resource::Null_Brush      ),
	Hollow    = underlying( stock_object_resource::Hollow_Brush    ),
};

enum class resource_handle_type : uintptr_t
{
	Null = 0, // 
	Cursor = 1,	// Hardware - dependent cursor resource.
	Bitmap = 2, // Bitmap resource.
	Icon = 3, // Hardware - dependent icon resource.
	Menu = 4, // Menu resource.
	Dialog = 5, // Dialog box.
	String = 6, // String - table entry.
	FontDirectory = 7, // Font directory resource.
	Font = 8, // Font resource.
	Accelerator = 9, // Accelerator table.
	Data = 10, // Application - defined resource(raw data).
	MessageTable = 11, // Message - table entry.

	GroupCursor = 12, // Hardware - independent cursor resource.
	GroupIcon = 14, // Hardware - independent icon resource.

	Version = 16, // Version resource.
	DialogInclude = 17, // Allows a resource editing tool to associate a string with an.rc file.
	                    // Typically, the string is the name of the header file that provides symbolic names.
	                    // The resource compiler parses the string but otherwise ignores the value. For example,
	                    // 
	                    // DLGINCLUDE "MyFile.h"

	PlugPlay = 19, // Plug and Play resource.
	VXD = 20, // VXD.
	AnimatedCursor = 21, // Animated cursor.
	AnimatedIcon = 22, // Animated icon.
	HTML = 23, // HTML resource.
	Manifest = 24, // Side - by - Side Assembly Manifest.
};
enum class image_type
{
	Bitmap      = 0,
	Icon        = 1,
	Cursor      = 2,
	ENHMetaFile = 3,
};
enum class image_load_flags : uint32_t
{
	DefaultColor       = 0x00000000,
	Monochrome         = 0x00000001,
	Color              = 0x00000002,
	CopyReturnOriginal = 0x00000004,
	CopyDeleteOriginal = 0x00000008,
	LoadFromFile       = 0x00000010,
	LoadTransparent    = 0x00000020,
	DefaultSize        = 0x00000040,
	VGAColor           = 0x00000080,
	LoadMap3DColors    = 0x00001000,
	CreateDIBSection   = 0x00002000,
	CopyFromResource   = 0x00004000,
	Shared             = 0x00008000,
};
sb_enum_class_flags(image_load_flags);

enum class system_metric
{
	/*
	 * GetSystemMetrics() codes
	 */

	ScreenWidth          = 0,
	ScreenHeight         = 1,
	CXVSCROLL            = 2,
	CYHSCROLL            = 3,
	CYCAPTION            = 4,
	CXBORDER             = 5,
	CYBORDER             = 6,
	CXDLGFRAME           = 7,
	CYDLGFRAME           = 8,
	CYVTHUMB             = 9,
	CXHTHUMB             = 10,
	CXICON               = 11,
	CYICON               = 12,
	CXCURSOR             = 13,
	CYCURSOR             = 14,
	CYMENU               = 15,
	CXFULLSCREEN         = 16,
	CYFULLSCREEN         = 17,
	CYKANJIWINDOW        = 18,
	MOUSEPRESENT         = 19,
	CYVSCROLL            = 20,
	CXHSCROLL            = 21,
	Debug                = 22,
	SWAPBUTTON           = 23,
	RESERVED1            = 24,
	RESERVED2            = 25,
	RESERVED3            = 26,
	RESERVED4            = 27,
	CXMIN                = 28,
	CYMIN                = 29,
	CXSIZE               = 30,
	CYSIZE               = 31,
	CXFRAME              = 32,
	CYFRAME              = 33,
	CXMINTRACK           = 34,
	CYMINTRACK           = 35,
	CXDOUBLECLK          = 36,
	CYDOUBLECLK          = 37,
	CXICONSPACING        = 38,
	CYICONSPACING        = 39,
	MENUDROPALIGNMENT    = 40,
	PENWINDOWS           = 41,
	DBCSENABLED          = 42,
	CMOUSEBUTTONS        = 43,
	
	CXFIXEDFRAME         = CXDLGFRAME,
	CYFIXEDFRAME         = CYDLGFRAME,
	CXSIZEFRAME          = CXFRAME,
	CYSIZEFRAME          = CYFRAME,
	
	SECURE               = 44,
	CXEDGE               = 45,
	CYEDGE               = 46,
	CXMINSPACING         = 47,
	CYMINSPACING         = 48,
	small_icon_width     = 49,
	small_icon_height    = 50,
	CYSMCAPTION          = 51,
	CXSMSIZE             = 52,
	CYSMSIZE             = 53,
	CXMENUSIZE           = 54,
	CYMENUSIZE           = 55,
	ARRANGE              = 56,
	CXMINIMIZED          = 57,
	CYMINIMIZED          = 58,
	CXMAXTRACK           = 59,
	CYMAXTRACK           = 60,
	CXMAXIMIZED          = 61,
	CYMAXIMIZED          = 62,
	NETWORK              = 63,
	CLEANBOOT            = 67,
	CXDRAG               = 68,
	CYDRAG               = 69,
	SHOWSOUNDS           = 70,
	CXMENUCHECK          = 71,
	CYMENUCHECK          = 72,
	SLOWMACHINE          = 73,
	MIDEASTENABLED       = 74,
	MOUSEWHEELPRESENT    = 75,
	XVIRTUALSCREEN       = 76,
	YVIRTUALSCREEN       = 77,
	CXVIRTUALSCREEN      = 78,
	CYVIRTUALSCREEN      = 79,
	CMONITORS            = 80,
	SAMEDISPLAYFORMAT    = 81,
	IMMENABLED           = 82,
	/////////////////////////////////////////////////////////////////////
	CXFOCUSBORDER        = 83,
	CYFOCUSBORDER        = 84,
	TABLETPC             = 86,
	MEDIACENTER          = 87,
	STARTER              = 88,
	SERVERR2             = 89,
	///////////////////////////////////////////////////////////////////
	MOUSEHORIZONTALWHEELPRESENT = 91,
	CXPADDEDBORDER              = 92,
	///////////////////////////////////////////////////////////////////
	DIGITIZER            =94,
	MAXIMUMTOUCHES       =95,
	
	CMETRICS_legacy      = 76,
	CMETRICS_500         = 83,
	CMETRICS_501         = 91,
	CMETRICS_600         = 93,
	CMETRICS             = 97,
	///////////////////////////////////////////////////////////////////
	REMOTESESSION        = 0x1000,
	
	SHUTTINGDOWN         = 0x2000,
	REMOTECONTROL        = 0x2001,
	CARETBLINKINGENABLED = 0x2002,
	CONVERTIBLESLATEMODE = 0x2003,
	SYSTEMDOCKED         = 0x2004,
	///////////////////////////////////////////////////////////////////
};

enum class window_style_flags : uint32_t
{
	// Window Styles
	OVERLAPPED       = 0x00000000L,
	POPUP            = 0x80000000L,
	CHILD            = 0x40000000L,
	MINIMIZE         = 0x20000000L,
	VISIBLE          = 0x10000000L,
	DISABLED         = 0x08000000L,
	CLIPSIBLINGS     = 0x04000000L,
	CLIPCHILDREN     = 0x02000000L,
	MAXIMIZE         = 0x01000000L,
	CAPTION          = 0x00C00000L,     // WS_BORDER | WS_DLGFRAME
	BORDER           = 0x00800000L,
	DLGFRAME         = 0x00400000L,
	VSCROLL          = 0x00200000L,
	HSCROLL          = 0x00100000L,
	SYSMENU          = 0x00080000L,
	THICKFRAME       = 0x00040000L,
	GROUP            = 0x00020000L,
	TABSTOP          = 0x00010000L,

	MINIMIZEBOX      = 0x00020000L,
	MAXIMIZEBOX      = 0x00010000L,


	TILED            = OVERLAPPED,
	ICONIC           = MINIMIZE,
	SIZEBOX          = THICKFRAME,

	 // Common Window Styles
	OVERLAPPEDWINDOW = (OVERLAPPED     | \
	                    CAPTION        | \
	                    SYSMENU        | \
	                    THICKFRAME     | \
	                    MINIMIZEBOX    | \
	                    MAXIMIZEBOX),
	TILEDWINDOW      = OVERLAPPEDWINDOW,

	POPUPWINDOW      = (POPUP          | \
	                    BORDER         | \
	                    SYSMENU),
	
	CHILDWINDOW      = (CHILD),
};
sb_enum_class_flags(window_style_flags)
enum class window_style_flags_ex : uint32_t
{
	/*
	 * Extended Window Styles
	 */
	DLGMODALFRAME     = 0x00000001L,
	NOPARENTNOTIFY    = 0x00000004L,
	TOPMOST           = 0x00000008L,
	ACCEPTFILES       = 0x00000010L,
	TRANSPARENT       = 0x00000020L,
	MDICHILD          = 0x00000040L,
	TOOLWINDOW        = 0x00000080L,
	WINDOWEDGE        = 0x00000100L,
	CLIENTEDGE        = 0x00000200L,
	CONTEXTHELP       = 0x00000400L,

	RIGHT             = 0x00001000L,
	LEFT              = 0x00000000L,
	RTLREADING        = 0x00002000L,
	LTRREADING        = 0x00000000L,
	LEFTSCROLLBAR     = 0x00004000L,
	RIGHTSCROLLBAR    = 0x00000000L,

	CONTROLPARENT     = 0x00010000L,
	STATICEDGE        = 0x00020000L,
	APPWINDOW         = 0x00040000L,
	
	OVERLAPPEDWINDOW  = (WINDOWEDGE | CLIENTEDGE),
	PALETTEWINDOW     = (WINDOWEDGE | TOOLWINDOW | TOPMOST),
	
	LAYERED           = 0x00080000,
	
	NOINHERITLAYOUT   = 0x00100000L, // Disable inheritence of mirroring by children
	
	NOREDIRECTIONBITMAP = 0x00200000L,
	
	LAYOUTRTL         = 0x00400000L, // Right to left mirroring
	
	COMPOSITED        = 0x02000000L,
	NOACTIVATE        = 0x08000000L,
};
sb_enum_class_flags(window_style_flags_ex)

enum class window_class_style : uint32_t {
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
sb_enum_class_flags(window_class_style);


enum class show_window_properties
{
	HIDE             = 0,
	SHOWNORMAL       = 1,
	NORMAL           = 1,
	SHOWMINIMIZED    = 2,
	SHOWMAXIMIZED    = 3,
	MAXIMIZE         = 3,
	SHOWNOACTIVATE   = 4,
	SHOW             = 5,
	MINIMIZE         = 6,
	SHOWMINNOACTIVE  = 7,
	SHOWNA           = 8,
	RESTORE          = 9,
	SHOWDEFAULT      = 10,
	FORCEMINIMIZE    = 11,
	MAX              = 11,
};
sb_enum_class_flags(show_window_properties);


// DWM
enum class dwm_window_attribute {
	NCRENDERING_ENABLED,
	NCRENDERING_POLICY,
	TRANSITIONS_FORCEDISABLED,
	ALLOW_NCPAINT,
	CAPTION_BUTTON_BOUNDS,
	NONCLIENT_RTL_LAYOUT,
	FORCE_ICONIC_REPRESENTATION,
	FLIP3D_POLICY,
	EXTENDED_FRAME_BOUNDS,
	HAS_ICONIC_BITMAP,
	DISALLOW_PEEK,
	EXCLUDED_FROM_PEEK,
	CLOAK,
	CLOAKED,
	FREEZE_REPRESENTATION,
	PASSIVE_UPDATE_MODE,
	USE_HOSTBACKDROPBRUSH,
	USE_IMMERSIVE_DARK_MODE_LEGACY = 19,
	USE_IMMERSIVE_DARK_MODE = 20, // since 20H1
	WINDOW_CORNER_PREFERENCE = 33,
	BORDER_COLOR,
	CAPTION_COLOR,
	TEXT_COLOR,
	VISIBLE_FRAME_BORDER_THICKNESS,
	LAST
};

} // Windows
}} // sbLibX
namespace sbWindows = SB::LibX::Windows;
