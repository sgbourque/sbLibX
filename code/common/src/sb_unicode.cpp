#include "common/include/sb_common.h"
#include "common/include/sb_unicode.h"
#include <unordered_map>

namespace SB {
namespace LibX {
namespace Unicode
{
static const std::unordered_map<General_Category, const char*> general_category_name =
{
#define SBDEF_GC_PROPERTY(property_name, category_alias, category_name, misc_alias, value) \
	{General_Category :: category_name, ("" # category_name)},
#include "common/include/internal/unicode/unicode_property_value_aliases.h"
};
SB_EXPORT_LIB const char* get_name( General_Category gc )
{
	return general_category_name.at( gc );
}

static const std::unordered_map<Canonical_Combining_Class, const char*> canonical_combining_class_name =
{
#define SBDEF_CCC_PROPERTY(property_name, value, class_alias, class_name) \
	{Canonical_Combining_Class :: class_name, ("" # class_name)},
#include "common/include/internal/unicode/unicode_property_value_aliases.h"
};
SB_EXPORT_LIB const char* get_name( Canonical_Combining_Class ccc )
{
	return canonical_combining_class_name.at( ccc );
}

static const std::unordered_map<Bidi_Class, const char*> bidi_class_name =
{
#define SBDEF_BIDI_PROPERTY(property_name, bidi_alias, bidi_name, value) \
	{Bidi_Class :: bidi_name, ("" # bidi_name)},
#include "common/include/internal/unicode/unicode_property_value_aliases.h"
};
SB_EXPORT_LIB const char* get_name( Bidi_Class bidi )
{
	return bidi_class_name.at( bidi );
}


// Basic Latin (ASCII)
static const std::unordered_map<ascii_t, const char*> ascii_name =
{
#define SBDEF_NAME_ALIAS( code_point, name, alias, tag ) \
	{ascii_t :: alias, ("" # name)},
#include "common/include/internal/unicode/unicode_name_aliases_basic_latin.h"

#define SBDEF_UNICODE_DATA(	code_point, name, gc, ccc, bidi,\
							decomposition, decimal, digit, value, mirror, legacy, comment, upper, lower, title ) \
	{ascii_t{code_point}, ("" # name)},
#include "common/include/internal/unicode/unicode_data_basic_latin.h"
#undef SBDEF_UNICODE_DATA
};
SB_EXPORT_LIB const char* get_name( ascii_t a )
{
	return ascii_name.at( a );
}

}
}
} // sbUnicode
