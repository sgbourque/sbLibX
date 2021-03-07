#ifndef SB_INSTANCE_LAYER_DEF
#define SB_INSTANCE_LAYER_DEF( name, type, value )	name = value,
#endif
#ifndef SB_INSTANCE_DEPRECATED_LAYER_DEF
#define SB_INSTANCE_DEPRECATED_LAYER_DEF( name, type, value )
#endif

SB_INSTANCE_LAYER_DEF( none                       , none, ( 0 ) )

// KHRONOS_validation - Khronos Validation Layer (March 2019: Supported):
// The main, comprehensive Khronos validation layer
//  -- this layer encompasses the entire functionality of the deprecated layers listed below, and supercedes them.
// As the other layers are deprecated this layer should be used for all validation going forward.
SB_INSTANCE_LAYER_DEF( KHRONOS_validation         , validation, ( 1ull <<  0 ) )
SB_INSTANCE_LAYER_DEF( validation_mask            , validation, ( KHRONOS_validation ) )


// LUNARG_api_dump - LunarG debug layer:
// Print API calls and their parameters and values
SB_INSTANCE_LAYER_DEF( LUNARG_api_dump            , utility, ( 1ull <<  1 ) )
// LUNARG_device_simulation - LunarG device simulation layer:
// Highlight potential application issues that are not specifically prohibited by the Vulkan spec, but which can still create problems
SB_INSTANCE_LAYER_DEF( LUNARG_device_simulation   , utility, ( 1ull <<  2 ) )
// LUNARG_monitor - Execution Monitoring Layer:
// Allows modification of an actual device's reported features, limits, and capabilities
SB_INSTANCE_LAYER_DEF( LUNARG_monitor             , utility, ( 1ull <<  3 ) )
// LUNARG_screenshot - LunarG image capture layer:
// Outputs the frames-per-second of the target application in the applications title bar
SB_INSTANCE_LAYER_DEF( LUNARG_screenshot          , utility, ( 1ull <<  4 ) )
// LUNARG_gfxreconstruct - GFXReconstruct Capture Layer:
// Outputs specified frames to an image file as they are presented
SB_INSTANCE_LAYER_DEF( LUNARG_gfxreconstruct      , utility, ( 1ull <<  5 ) )
// RENDERDOC_Capture - Debugging capture layer for RenderDoc:
SB_INSTANCE_LAYER_DEF( RENDERDOC_Capture          , utility, ( 1ull <<  6ull ) )
SB_INSTANCE_LAYER_DEF( utility_mask               , utility, ( LUNARG_api_dump | LUNARG_device_simulation | LUNARG_monitor | LUNARG_screenshot | LUNARG_gfxreconstruct ) )

// deprecated
SB_INSTANCE_DEPRECATED_LAYER_DEF( LUNARG_vktrace             , utility,    ( 1ull << 20 ) ) // Vktrace tracing library - use gfxreconstruct
SB_INSTANCE_DEPRECATED_LAYER_DEF( LUNARG_assistant_layer     , utility,    ( 1ull << 21 ) ) // LunarG Validation Factory Layer - use KHRONOS_validation
SB_INSTANCE_DEPRECATED_LAYER_DEF( LUNARG_standard_validation , validation, ( 1ull << 22 ) ) // LunarG Standard Validation (February 2016: Deprecated) - use KHRONOS_validation
SB_INSTANCE_DEPRECATED_LAYER_DEF( LUNARG_core_validation     , validation, ( 1ull << 23 ) ) // LunarG Validation Layer (March 2016: Deprecated) - use KHRONOS_validation
SB_INSTANCE_DEPRECATED_LAYER_DEF( LUNARG_object_tracker      , validation, ( 1ull << 24 ) ) // LunarG Validation Layer (October 2014: Deprecated) - use KHRONOS_validation
SB_INSTANCE_DEPRECATED_LAYER_DEF( LUNARG_parameter_validation, validation, ( 1ull << 25 ) ) // LunarG Validation Layer (December 2014: Deprecated) - use KHRONOS_validation
SB_INSTANCE_DEPRECATED_LAYER_DEF( GOOGLE_threading           , validation, ( 1ull << 26 ) ) // Google Validation Layer (April 2015: Deprecated) - use KHRONOS_validation
SB_INSTANCE_DEPRECATED_LAYER_DEF( GOOGLE_unique_objects      , utility,    ( 1ull << 27 ) ) // Google Validation Layer (December 2015: Deprecated) - use KHRONOS_validation
SB_INSTANCE_DEPRECATED_LAYER_DEF( deprecated_mask            , deprecated, ( LUNARG_vktrace | LUNARG_assistant_layer | LUNARG_standard_validation | LUNARG_core_validation | LUNARG_parameter_validation | LUNARG_object_tracker | GOOGLE_unique_objects | GOOGLE_threading ) )

// Khronos/VALVE extensions
SB_INSTANCE_LAYER_DEF( VALVE_steam_overlay        , VALVE, ( 1ull << 30 ) ) // Steam Overlay Layer
SB_INSTANCE_LAYER_DEF( VALVE_steam_fossilize      , VALVE, ( 1ull << 31 ) ) // Steam Pipeline Caching Layer
SB_INSTANCE_LAYER_DEF( VALVE_mask                 , VALVE, ( VALVE_steam_fossilize | VALVE_steam_overlay ) )

// vendor extensions
SB_INSTANCE_LAYER_DEF( NV_optimus                 , NVIDIA, ( 1ull << 32 ) ) // NVIDIA Optimus layer
SB_INSTANCE_LAYER_DEF( NV_mask                    , NVIDIA, ( NV_optimus ) )

SB_INSTANCE_LAYER_DEF( AMD_mask                   , AMD, ( 0ull ) ) // TODO


#undef SB_INSTANCE_LAYER_DEF
#undef SB_INSTANCE_DEPRECATED_LAYER_DEF
