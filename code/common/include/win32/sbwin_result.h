#pragma once

namespace SB { namespace LibX {
namespace Windows {

enum class message_t : uint32_t
{
#include "common/include/internal/win32/win32_messages.h"
};

enum class result_facility_t : uint32_t
{
#define SB_WIN32_FACILITY( name, wm_facility, value ) name = value,
#include "common/include/internal/win32/win32_results.h"
};
enum class result_severity_t : uint32_t
{
#define SB_WIN32_SEVERITY( name, wm_severity, value ) name = value,
#include "common/include/internal/win32/win32_results.h"
};
enum class result_code_t : uint32_t
{
#define SB_WIN32_RESULT_CODE( name, wm_error_code, value ) name = value,
#define SB_WIN32_RESULT_CODE_ALIAS( name, wm_error_code, value ) name = value,
#include "common/include/internal/win32/win32_result_codes.h"
};
enum class error_t : uint32_t
{
#define SB_WIN32_ERROR( name, wm_error_code, value ) name = value,
#define SB_WIN32_ERROR_ALIAS( name, wm_error_code, value ) name = value,
#include "common/include/internal/win32/win32_errors.h"
};

struct result_t
{
	using type_t = std::underlying_type_t<error_t>;
	static_assert( std::is_same_v<type_t, uint32_t> );

	// low word
	result_code_t     code              : 16;
	// high word
	result_facility_t facility          : 12;
	type_t            reserved          : 1;
	type_t            application_error : 1;
	result_severity_t severity          : 2;

	constexpr result_t( type_t value = 0 )
		: code             ( result_code_t    ((value >>  0u) & ((1u << 16u) - 1u)) )
		, facility         ( result_facility_t((value >> 16u) & ((1u << 12u) - 1u)) )
		, reserved         (                  ((value >> 28u) & ((1u <<  1u) - 1u)) ) 
		, application_error(                  ((value >> 29u) & ((1u <<  1u) - 1u)) ) 
		, severity         ( result_severity_t((value >> 30u) & ((1u <<  2u) - 1u)) ) 
	{
	}
	constexpr result_t( error_t value ) : result_t( static_cast<type_t>(value) ) {}
	constexpr result_t( result_code_t value )     : result_t{} { code = value; }
	constexpr result_t( result_facility_t value ) : result_t{} { facility = value; }
	constexpr result_t( result_severity_t value ) : result_t{} { severity = value; }

	constexpr type_t value() const
	{
		return (uint32_t(severity) << 30u) | (uint32_t(application_error) << 29u) | (uint32_t(reserved) << 28u) | (uint32_t(facility) << 16u) | (uint32_t(code) << 0u);
	}
};
static_assert( sizeof( result_t ) == sizeof( uint32_t ) );
// TODO: Unit test
static_assert(
	   result_t( 0x77654321u ).value()  == 0x77654321u
	&& result_t( 0x87654321u ).code     == result_code_t(0x4321u)
	&& result_t( 0x87654321u ).facility == result_facility_t(0x765u)
	&& result_t( 0x87654321u ).severity == result_severity_t::Error
);

// TODO
//S_OK
//DXGI_ERROR_DEVICE_HUNG

} // Windows
}} // sbLibX
namespace sbWindows = SB::LibX::Windows;


// Can be in global scope since using explicit non-trivial types from sbWindows namespace
SB_EXPORT_TYPE inline constexpr const char* get_result_name( sbWindows::result_t result )
{
	switch( result.value() )
	{
	#define SB_WIN32_ERROR( name, wm_error_code, value ) case value: return "" # name "(" # wm_error_code ")";
	#include "common/include/internal/win32/win32_errors.h"
	default: return "(unkown)";
	}
}
SB_EXPORT_TYPE inline constexpr const char* get_severity_name( sbWindows::result_t result )
{
	switch( sbWindows::result_t::type_t(result.severity) )
	{
	#define SB_WIN32_SEVERITY( name, wm_severity, value ) case value: return "" # name;
	#include "common/include/internal/win32/win32_results.h"
	default: return "(unknown)";
	}
}
SB_EXPORT_TYPE inline constexpr const char* get_facility_name( sbWindows::result_t result )
{
	switch( sbWindows::result_t::type_t(result.facility) )
	{
	#define SB_WIN32_FACILITY( name, wm_facility, value ) case value: return "" # name;
	#include "common/include/internal/win32/win32_results.h"
	default: return "(unknown)";
	}
}
SB_EXPORT_TYPE inline constexpr const char* get_code_name( sbWindows::result_t result )
{
	switch( sbWindows::result_t::type_t(result.code) )
	{
	#define SB_WIN32_RESULT_CODE( name, wm_code, value ) case value: return "" # name;
	#include "common/include/internal/win32/win32_result_codes.h"
	default: return "(unknown)";
	}
}
