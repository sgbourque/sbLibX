#include <common/include/sb_range.h>
#include <common/include/sb_encrypted_string.h>
#include <common/include/sb_structured_buffer.h>

#include <compare>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <set>

//#include <cassert>
#define assert(X)\
	do{ if (!(X)) __debugbreak(); } while(false)


#define VALIDATE (false)


namespace SB { namespace LibX
{

namespace Character
{

// First let's support only basic properties of ASCII, just enough
// to parse unicode data files

// General_Category (gc)
enum class General_Category : uint32_t
{
#define SBDEF_GC_PROPERTY(property_name, category_alias, category_name, misc_alias, value) \
	category_name = value, category_alias = category_name,
#include "common/include/internal/unicode/property_value_aliases.h"
// @missing: 0000..10FFFF; General_Category; Unassigned
};
sb_enum_class_flags(General_Category);

// Canonical_Combining_Class( ccc )
enum class Canonical_Combining_Class : uint8_t
{
#define SBDEF_CCC_PROPERTY(property_name, value, class_alias, class_name) \
	class_name = value, class_alias = class_name,
#include "common/include/internal/unicode/property_value_aliases.h"
};

enum class Bidi_Class : uint32_t
{
#define SBDEF_BIDI_PROPERTY(property_name, bidi_alias, bidi_name, value) \
	bidi_name = value, bidi_alias = bidi_name,
#include "common/include/internal/unicode/property_value_aliases.h"
};
sb_enum_class_flags(Bidi_Class);

struct DecimalDigit
{
	enum value_t : int
	{
		undefined = -1,
		zero,
		one,
		two,
		three,
		four,
		five,
		six,
		seven,
		eight,
		nine
	};
	int value = undefined;
};


enum class Bidi_Mirrored : bool
{
	False = false, F = False, No = F,  N = No,
	True  = true,  T = True,  Yes = T, Y = Yes,
};


	template< typename unicode_subset >
struct Simple_Character_Mapping
{
	using underlying_type_t = std::underlying_type_t<unicode_subset>;
	static inline constexpr unicode_subset code_point = unicode_subset{ - 1};
	Simple_Character_Mapping( underlying_type_t _code_point = code_point ) : value( unicode_subset{ _code_point } ) {}
	auto operator <=>( const Simple_Character_Mapping& ) const = default;

	unicode_subset value;
};


// based on Unicode 13.0.0: UCD / UnicodeData.txt
template< typename unicode_subset = char32_t >
struct unicode_data
{
	using unicode_subset_t = unicode_subset;
	using simple_character_mapping_t = Simple_Character_Mapping<unicode_subset_t>;
	
	unicode_subset_t code_point;
	const char* name;
	General_Category general_category;
	Canonical_Combining_Class canonical_combining_class;
	Bidi_Class bidi_class;
	const char* decomposition_type;
	DecimalDigit decimal_value;
	DecimalDigit digit_value;
	const char* numeric_value;
	Bidi_Mirrored bidi_mirrored;
	const char* unicode_1_name_obsolete;
	const char* unicode_comment_obsolete;
	simple_character_mapping_t simple_uppercase_mapping;
	simple_character_mapping_t simple_lowercase_mapping;
	simple_character_mapping_t simple_titlecase_mapping;
	
	unicode_data() = delete;
	
	unicode_data(
		unicode_subset_t _code_point,
		const char* _name,
		General_Category _general_category,
		Canonical_Combining_Class _canonical_combining_class,
		Bidi_Class _bidi_class,
		const char* _decomposition_type = nullptr,
		DecimalDigit _decimal_value = DecimalDigit{DecimalDigit::undefined},
		DecimalDigit _digit_value = DecimalDigit{DecimalDigit::undefined},
		const char* _numeric_value = nullptr,
		Bidi_Mirrored _bidi_mirrored = Bidi_Mirrored::N,
		const char* _unicode_1_name_obsolete = nullptr,
		const char* _unicode_comment_obsolete = nullptr,
		simple_character_mapping_t _simple_uppercase_mapping = {},
		simple_character_mapping_t _simple_lowercase_mapping = {},
		simple_character_mapping_t _simple_titlecase_mapping = {}
	) :
		code_point               ( _code_point                ),
		name                     ( _name                      ),
		general_category         ( _general_category != General_Category{} ? _general_category : General_Category::Unassigned ),
		canonical_combining_class( _canonical_combining_class ),
		bidi_class               ( _bidi_class                ),
		decomposition_type       ( _decomposition_type        ),
		decimal_value            ( _decimal_value             ),
		digit_value              ( _digit_value               ),
		numeric_value            ( _numeric_value             ),
		bidi_mirrored            ( _bidi_mirrored             ),
		unicode_1_name_obsolete  ( _unicode_1_name_obsolete   ),
		unicode_comment_obsolete ( _unicode_comment_obsolete  ),
		simple_uppercase_mapping ( _simple_uppercase_mapping != simple_character_mapping_t{} ? _simple_uppercase_mapping : _code_point ),
		simple_lowercase_mapping ( _simple_lowercase_mapping != simple_character_mapping_t{} ? _simple_lowercase_mapping : _code_point ),
		simple_titlecase_mapping ( _simple_titlecase_mapping != simple_character_mapping_t{} ? _simple_titlecase_mapping : _simple_uppercase_mapping != simple_character_mapping_t{} ? _simple_uppercase_mapping : _code_point )
	{}
};

// ASCII unicode subset (UnicodeData.txt)
enum basic_latin_t : int8_t
{
	                       Null = U'\u0000',	//	Cc;	0;	BN;	;	;	;	;	N;	NULL;	;	; ;
	           Start_Of_Heading = U'\u0001',	//	Cc;	0;	BN;	;	;	;	;	N;	START OF HEADING;	;	; ;
	              Start_Of_Text = U'\u0002',	//	Cc;	0;	BN;	;	;	;	;	N;	START OF TEXT;	;	; ;
	                End_Of_Text = U'\u0003',	//	Cc;	0;	BN;	;	;	;	;	N;	END OF TEXT;	;	; ;
	        End_Of_Transmission = U'\u0004',	//	Cc;	0;	BN;	;	;	;	;	N;	END OF TRANSMISSION;	;	; ;
	                    Enquiry = U'\u0005',	//	Cc;	0;	BN;	;	;	;	;	N;	ENQUIRY;	;	; ;
	                Acknowledge = U'\u0006',	//	Cc;	0;	BN;	;	;	;	;	N;	ACKNOWLEDGE;	;	; ;
	                       Bell = U'\u0007',	//	Cc;	0;	BN;	;	;	;	;	N;	BELL;	;	; ;
	                  Backspace = U'\u0008',	//	Cc;	0;	BN;	;	;	;	;	N;	BACKSPACE;	;	; ;
	       Character_Tabulation = U'\u0009',	//	Cc;	0;	 S;	;	;	;	;	N;	CHARACTER TABULATION;	;	; ;
	                  Line_Feed = U'\u000A',	//	Cc;	0;	 B;	;	;	;	;	N;	LINE FEED( LF );	;	; ;
	                         LF = Line_Feed,
	            Line_Tabulation = U'\u000B',	//	Cc;	0;	 S;	;	;	;	;	N;	LINE TABULATION;	;	; ;
	                  Form_Feed = U'\u000C',	//	Cc;	0;	WS;	;	;	;	;	N;	FORM FEED( FF );	;	; ;
	                         FF = Form_Feed,
	            Carriage_Return = U'\u000D',	//	Cc;	0;	 B;	;	;	;	;	N;	CARRIAGE RETURN( CR );	;	; ;
	                         CR = Carriage_Return,
	                  Shift_Out = U'\u000E',	//	Cc;	0;	BN;	;	;	;	;	N;	SHIFT OUT;	;	; ;
	                   Shift_In = U'\u000F',	//	Cc;	0;	BN;	;	;	;	;	N;	SHIFT IN;	;	; ;
	           Data_Link_Escape = U'\u0010',	//	Cc;	0;	BN;	;	;	;	;	N;	DATA LINK ESCAPE;	;	; ;
	        Device_Controle_One = U'\u0011',	//	Cc;	0;	BN;	;	;	;	;	N;	DEVICE CONTROL ONE;	;	; ;
	        Device_Controle_Two = U'\u0012',	//	Cc;	0;	BN;	;	;	;	;	N;	DEVICE CONTROL TWO;	;	; ;
	      Device_Controle_Three = U'\u0013',	//	Cc;	0;	BN;	;	;	;	;	N;	DEVICE CONTROL THREE;	;	; ;
	       Device_Controle_Four = U'\u0014',	//	Cc;	0;	BN;	;	;	;	;	N;	DEVICE CONTROL FOUR;	;	; ;
	       Negative_Acknowledge = U'\u0015',	//	Cc;	0;	BN;	;	;	;	;	N;	NEGATIVE ACKNOWLEDGE;	;	; ;
	           Synchronous_Idle = U'\u0016',	//	Cc;	0;	BN;	;	;	;	;	N;	SYNCHRONOUS IDLE;	;	; ;
	  End_Of_Transmission_Block = U'\u0017',	//	Cc;	0;	BN;	;	;	;	;	N;	END OF TRANSMISSION BLOCK;	;	; ;
	                      Cance = U'\u0018',	//	Cc;	0;	BN;	;	;	;	;	N;	CANCEL;	;	; ;
	              End_Of_Medium = U'\u0019',	//	Cc;	0;	BN;	;	;	;	;	N;	END OF MEDIUM;	;	; ;
	                 Substitute = U'\u001A',	//	Cc;	0;	BN;	;	;	;	;	N;	SUBSTITUTE;	;	; ;
	                     Escape = U'\u001B',	//	Cc;	0;	BN;	;	;	;	;	N;	ESCAPE;	;	; ;
	 Information_Separator_Four = U'\u001C',	//	Cc;	0;	 B;	;	;	;	;	N;	INFORMATION SEPARATOR FOUR;	;	; ;
	Information_Separator_Three = U'\u001D',	//	Cc;	0;	 B;	;	;	;	;	N;	INFORMATION SEPARATOR THREE;	;	; ;
	  Information_Separator_Two = U'\u001E',	//	Cc;	0;	 B;	;	;	;	;	N;	INFORMATION SEPARATOR TWO;	;	; ;
	  Information_Separator_One = U'\u001F',	//	Cc;	0;	 S;	;	;	;	;	N;	INFORMATION SEPARATOR ONE;	;	; ;
	                      Space = U'\u0020',	//	Zs;	0;	WS;	;	;	;	;	N;	;	;	;	;
	           Exclamation_Mark = U'\u0021',	//	Po;	0;	ON;	;	;	;	;	N;	;	;	;	;
	             Quotation_Mark = U'\u0022',	//	Po;	0;	ON;	;	;	;	;	N;	;	;	;	;
	                Number_Sign = U'\u0023',	//	Po;	0;	ET;	;	;	;	;	N;	;	;	;	;
	                Dollar_Sign = U'\u0024',	//	Sc;	0;	ET;	;	;	;	;	N;	;	;	;	;
	               Percent_Sign = U'\u0025',	//	Po;	0;	ET;	;	;	;	;	N;	;	;	;	;
	                  Ampersand = U'\u0026',	//	Po;	0;	ON;	;	;	;	;	N;	;	;	;	;
	                 Apostrophe = U'\u0027',	//	Po;	0;	ON;	;	;	;	;	N;	APOSTROPHE-QUOTE;	;	; ;
	           Left_Parenthesis = U'\u0028',	//	Ps;	0;	ON;	;	;	;	;	Y;	OPENING PARENTHESIS;	;	; ;
	          Right_Parenthesis = U'\u0029',	//	Pe;	0;	ON;	;	;	;	;	Y;	CLOSING PARENTHESIS;	;	; ;
	                   Asterisk = U'\u002A',	//	Po;	0;	ON;	;	;	;	;	N;	;	;	;	;
	                  Plus_Sign = U'\u002B',	//	Sm;	0;	ES;	;	;	;	;	N;	;	;	;	;
	                      Comma = U'\u002C',	//	Po;	0;	CS;	;	;	;	;	N;	;	;	;	;
	                HyphenMinus = U'\u002D',	//	Pd;	0;	ES;	;	;	;	;	N;	;	;	;	;
	                  Full_Stop = U'\u002E',	//	Po;	0;	CS;	;	;	;	;	N;	PERIOD;	;	; ;
	                     Period = Full_Stop,
	                    Solidus = U'\u002F',	//	Po;	0;	CS;	;	;	;	;	N;	SLASH;	;	; ;
	                      Slash = Solidus,
	                 Digit_Zero = U'\u0030',	//	Nd;	0;	EN; ;	0;	0;	0;	N;	;	;	;	;
	                  Digit_One = U'\u0031',	//	Nd;	0;	EN; ;	1;	1;	1;	N;	;	;	;	;
	                  Digit_Two = U'\u0032',	//	Nd;	0;	EN; ;	2;	2;	2;	N;	;	;	;	;
	                Digit_Three = U'\u0033',	//	Nd;	0;	EN; ;	3;	3;	3;	N;	;	;	;	;
	                 Digit_Four = U'\u0034',	//	Nd;	0;	EN; ;	4;	4;	4;	N;	;	;	;	;
	                 Digit_Five = U'\u0035',	//	Nd;	0;	EN; ;	5;	5;	5;	N;	;	;	;	;
	                 Digit_Size = U'\u0036',	//	Nd;	0;	EN; ;	6;	6;	6;	N;	;	;	;	;
	                Digit_Seven = U'\u0037',	//	Nd;	0;	EN; ;	7;	7;	7;	N;	;	;	;	;
	                Digit_Eight = U'\u0038',	//	Nd;	0;	EN; ;	8;	8;	8;	N;	;	;	;	;
	                 Digit_Nine = U'\u0039',	//	Nd;	0;	EN; ;	9;	9;	9;	N;	;	;	;	;
	                      Colon = U'\u003A',	//	Po;	0;	CS;	;	;	;	;	N;	;	;	;	;
	                  Semicolon = U'\u003B',	//	Po;	0;	ON;	;	;	;	;	N;	;	;	;	;
	              LessThan_Sign = U'\u003C',	//	Sm;	0;	ON;	;	;	;	;	Y;	;	;	;	;
	                 Equal_Sign = U'\u003D',	//	Sm;	0;	ON;	;	;	;	;	N;	;	;	;	;
	           GreaterThan_Sign = U'\u003E',	//	Sm;	0;	ON;	;	;	;	;	Y;	;	;	;	;
	              Question_Mark = U'\u003F',	//	Po;	0;	ON;	;	;	;	;	N;	;	;	;	;
	              Commercial_At = U'\u0040',	//	Po;	0;	ON;	;	;	;	;	N;	;	;	;	;
	     Latin_Capital_Letter_A = U'\u0041',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0061;
	     Latin_Capital_Letter_B = U'\u0042',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0062;
	     Latin_Capital_Letter_C = U'\u0043',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0063;
	     Latin_Capital_Letter_D = U'\u0044',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0064;
	     Latin_Capital_Letter_E = U'\u0045',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0065;
	     Latin_Capital_Letter_F = U'\u0046',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0066;
	     Latin_Capital_Letter_G = U'\u0047',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0067;
	     Latin_Capital_Letter_H = U'\u0048',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0068;
	     Latin_Capital_Letter_I = U'\u0049',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0069;
	     Latin_Capital_Letter_J = U'\u004A',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	006A;
	     Latin_Capital_Letter_K = U'\u004B',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	006B;
	     Latin_Capital_Letter_L = U'\u004C',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	006C;
	     Latin_Capital_Letter_M = U'\u004D',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	006D;
	     Latin_Capital_Letter_N = U'\u004E',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	006E;
	     Latin_Capital_Letter_O = U'\u004F',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	006F;
	     Latin_Capital_Letter_P = U'\u0050',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0070;
	     Latin_Capital_Letter_Q = U'\u0051',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0071;
	     Latin_Capital_Letter_R = U'\u0052',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0072;
	     Latin_Capital_Letter_S = U'\u0053',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0073;
	     Latin_Capital_Letter_T = U'\u0054',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0074;
	     Latin_Capital_Letter_U = U'\u0055',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0075;
	     Latin_Capital_Letter_V = U'\u0056',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0076;
	     Latin_Capital_Letter_W = U'\u0057',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0077;
	     Latin_Capital_Letter_X = U'\u0058',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0078;
	     Latin_Capital_Letter_Y = U'\u0059',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	0079;
	     Latin_Capital_Letter_Z = U'\u005A',	//	Lu;	0;	 L;	;	;	;	;	N;	;	; ;	007A;
	        Left_Square_Bracket = U'\u005B',	//	Ps;	0;	ON;	;	;	;	;	Y;	OPENING SQUARE BRACKET;	;	; ;
	            Reverse_Solidus = U'\u005C',	//	Po;	0;	ON;	;	;	;	;	N;	BACKSLASH;	;	; ;
	                  Backslash = Reverse_Solidus,
	       Right_Square_Bracket = U'\u005D',	//	Pe;	0;	ON;	;	;	;	;	Y;	CLOSING SQUARE BRACKET;	;	; ;
	          Circumflex_Accent = U'\u005E',	//	Sk;	0;	ON;	;	;	;	;	N;	SPACING CIRCUMFLEX;	;	; ;
	                   Low_Line = U'\u005F',	//	Pc;	0;	ON;	;	;	;	;	N;	SPACING UNDERSCORE;	;	; ;
	                 Underscore = Low_Line,
	               Grave_Accent = U'\u0060',	//	Sk;	0;	ON;	;	;	;	;	N;	SPACING GRAVE;	;	; ;
	       Latin_Small_Letter_a = U'\u0061',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0041; ;	0041
	       Latin_Small_Letter_b = U'\u0062',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0042; ;	0042
	       Latin_Small_Letter_c = U'\u0063',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0043; ;	0043
	       Latin_Small_Letter_d = U'\u0064',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0044; ;	0044
	       Latin_Small_Letter_e = U'\u0065',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0045; ;	0045
	       Latin_Small_Letter_f = U'\u0066',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0046; ;	0046
	       Latin_Small_Letter_g = U'\u0067',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0047; ;	0047
	       Latin_Small_Letter_h = U'\u0068',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0048; ;	0048
	       Latin_Small_Letter_i = U'\u0069',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0049; ;	0049
	       Latin_Small_Letter_j = U'\u006A',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	004A; ;	004A
	       Latin_Small_Letter_k = U'\u006B',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	004B; ;	004B
	       Latin_Small_Letter_l = U'\u006C',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	004C; ;	004C
	       Latin_Small_Letter_m = U'\u006D',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	004D; ;	004D
	       Latin_Small_Letter_n = U'\u006E',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	004E; ;	004E
	       Latin_Small_Letter_o = U'\u006F',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	004F; ;	004F
	       Latin_Small_Letter_p = U'\u0070',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0050; ;	0050
	       Latin_Small_Letter_q = U'\u0071',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0051; ;	0051
	       Latin_Small_Letter_r = U'\u0072',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0052; ;	0052
	       Latin_Small_Letter_s = U'\u0073',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0053; ;	0053
	       Latin_Small_Letter_t = U'\u0074',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0054; ;	0054
	       Latin_Small_Letter_u = U'\u0075',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0055; ;	0055
	       Latin_Small_Letter_v = U'\u0076',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0056; ;	0056
	       Latin_Small_Letter_w = U'\u0077',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0057; ;	0057
	       Latin_Small_Letter_x = U'\u0078',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0058; ;	0058
	       Latin_Small_Letter_y = U'\u0079',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	0059; ;	0059
	       Latin_Small_Letter_z = U'\u007A',	//	Ll;	0;	 L;	;	;	;	;	N;	;	;	005A; ;	005A
	         Left_Curly_Bracket = U'\u007B',	//	Ps;	0;	ON;	;	;	;	;	Y;	OPENING CURLY BRACKET;	;	; ;
	              Vertical_Line = U'\u007C',	//	Sm;	0;	ON;	;	;	;	;	N;	VERTICAL BAR;	;	; ;
	               Vertical_Bar = Vertical_Line,
	                       Pipe = Vertical_Bar,
	        Right_Curly_Bracket = U'\u007D',	//	Pe;	0;	ON;	;	;	;	;	Y;	CLOSING CURLY BRACKET;	;	; ;
	                      Tilde = U'\u007E',	//	Sm;	0;	ON;	;	;	;	;	N;	;	;	;	;
	                     Delete = U'\u007F',	//	Cc;	0;	BN;	;	;	;	;	N;	DELETE;	;	; ;
};
using ascii_t = basic_latin_t;

/*static inline constexpr*/ unicode_data<ascii_t> ascii_data[] =
{
#define SBDEF_UNICODE_DATA(	code_point, name, gc, ccc, bidi,\
							decomposition, decimal, digit, value, mirror, legacy, comment, upper, lower, title ) \
		{\
			ascii_t{code_point}, {"" # name}, General_Category :: gc, Canonical_Combining_Class{ ccc }, Bidi_Class :: bidi,\
			{"" # decomposition}, {decimal}, {digit}, ("" # value), Bidi_Mirrored :: mirror, ("" # legacy), ("" # comment),\
			ascii_t{upper}, ascii_t{lower}, ascii_t{title}, \
		},

#include "common/include/internal/unicode/unicode_data_basic_latin.h"
//#include "common/include/internal/unicode/unicode_data_latin-1_suppl.h"
#undef SBDEF_UNICODE_DATA
};

//static inline constexpr char32_t ascii_blank_separator[] = {
//	U'\0', U' ', U'\t', U'\n', U'\v', U'\f',
//};
//static inline constexpr char32_t ascii_blank_separator[] ={
//	U'\0', U' ', U'\t', U'\n', U'\v', U'\f',
//};


}

#if 1
// WIP: lexer (w/ basic_unicode transform + buffer_view) - phase 1 libX integration...
////
constexpr auto UTF8_to_UTF32(const char utf8_input[], size_t max_length)
{
	const char* utf8_iterator = utf8_input;
#if VALIDATE
	const char* const utf8_end = utf8_input + max_length;
	uint32_t length = 0;
#endif

	uint32_t partmask = 0x7f;
	uint32_t fullmask = 0;
	uint32_t utf32 = 0;

	uint32_t nextcode = static_cast<unsigned char>(*utf8_iterator);
	uint32_t nextpart = (nextcode & partmask);
	partmask = 0x3f;
	do
	{
		assert((utf32 == 0) || (nextcode & 0xC0) == 0x80);
		uint32_t shiftpart = ((utf32 & fullmask) << 6);
		assert(shiftpart + nextpart == (shiftpart | nextpart));
		utf32 = shiftpart | nextpart;

		++utf8_iterator;
		nextcode = (--max_length > 0) ? static_cast<unsigned char>(*utf8_iterator) : 0;
		nextpart = (nextcode & partmask);
		fullmask = (fullmask << 5) | partmask;
#if VALIDATE
		assert(utf8_iterator <= utf8_end);
		++length;
#endif
	}
	while ((nextcode & 0xC0u) == 0x80);

#if VALIDATE
	// Validate UTF-8 input :
	// Any valid sequence U crom from one of these masks :
	//	0xxxxxxx, 0x00 <= U <= 0x7F;
	//	0x110xxxxx 10xxxxxx, 0x0080 <= U <= 0x07FF;
	//	0x1110xxxx 10xxxxxx 10xxxxxx, 0x0800 <= U <= 0xFFFF,
	// or
	//	0x11110xxx 10xxxxxx 10xxxxxx 10xxxxxx, 0x10000 <= U <= 0x10FFFF.
	switch (length)
	{
	case 1:
		assert((utf8_input[0] & 0x80) == 0x00);
		break;
	case 2:
		assert((utf8_input[0] & 0xE0) == 0xC0 && (utf8_input[1] & 0xC0) == 0x80);
		break;
	case 3:
		assert((utf8_input[0] & 0xF0) == 0xE0 && (utf8_input[1] & 0xC0) == 0x80 && (utf8_input[2] & 0xC0) == 0x80);
		break;
	case 4:
		assert((utf8_input[0] & 0xF8) == 0xF0 && (utf8_input[1] & 0xC0) == 0x80 && (utf8_input[2] & 0xC0) == 0x80 && (utf8_input[3] & 0xC0) == 0x80);
		break;
	default:
		__debugbreak();
		break;
	}
	// And then validate it is not an UTF16 surrogate or an out-of-bound code.
	assert(utf32 < 0x110000 && (utf32 < 0xD800 || utf32 > 0xDFFF));
	assert(utf8_iterator == utf8_input + length);
#endif
	return std::make_tuple(static_cast<char32_t>(utf32), utf8_iterator);
}

std::string UTF32_to_UTF8(const char32_t utf32)
{
	std::string utf8{};
	utf8.reserve(4);
	//	0xxxxxxx, 0x00 <= U <= 0x7F;
	//	0x110xxxxx 10xxxxxx, 0x0080 <= U <= 0x07FF;
	//	0x1110xxxx 10xxxxxx 10xxxxxx, 0x0800 <= U <= 0xFFFF,
	// or
	//	0x11110xxx 10xxxxxx 10xxxxxx 10xxxxxx, 0x10000 <= U <= 0x10FFFF.
	if (utf32 < 0x80)
	{
		utf8.resize(1);
		utf8[0] = (utf32 & 0x7F);
	}
	else if (utf32 < 0x0800)
	{
		utf8.resize(2);
		utf8[0] = static_cast<char>(0xC0 | ((utf32 >> 6) & 0x3F));
		utf8[1] = static_cast<char>(0x80u | (utf32 & 0x3Fu));
	}
	else if (utf32 < 0x00010000)
	{
		utf8.resize(3);
		utf8[0] = static_cast<char>(0xE0 | ((utf32 >> 12) & 0x3F));
		utf8[1] = static_cast<char>(0x80 | ((utf32 >> 6) & 0x3F));
		utf8[2] = static_cast<char>(0x80 | (utf32 & 0x3F));
	}
	else
	{
		utf8.resize(4);
		utf8[0] = static_cast<char>(0xF0 | ((utf32 >> 18) & 0x3F));
		utf8[1] = static_cast<char>(0x80 | ((utf32 >> 12) & 0x3F));
		utf8[2] = static_cast<char>(0x80 | ((utf32 >> 6) & 0x3F));
		utf8[3] = static_cast<char>(0x80 | (utf32 & 0x3F));
	}
	assert(std::get<0>(UTF8_to_UTF32(&utf8[0], 4)) == utf32);
	return utf8;
}

////
	template< typename VIEW_TYPE, typename _CONTAINER_, typename TOKEN_TYPE >
auto token_to_view(const _CONTAINER_* container, const TOKEN_TYPE& token)
{
	return VIEW_TYPE(container->data() + token.begin(), token.end() - token.begin());
}

	template< typename _CONTAINER_, typename _HASH_TRAITS_ = sbLibX::xhash_traits<typename _CONTAINER_::value_type, sbLibX::xhash_t, 64, 3, 7> >
struct token_hash_t : std::hash< std::basic_string_view<typename _CONTAINER_::value_type> >
{
	using container_t = _CONTAINER_;
	using value_t = typename container_t::value_type;
	using string_view = std::basic_string_view<value_t>;
	using hash_traits = _HASH_TRAITS_;

	token_hash_t(const container_t* _container) : container(_container) {}

		template< typename token_type >
	auto operator()(const token_type& token) const
	{
		return hash_traits::hash(token_to_view<string_view>(container, token).data());
	}
private:
	const container_t* container;
};
	template< typename container_type >
struct token_equal_t
{
	using container_t = container_type;
	using value_t = typename container_t::value_type;
	using string_view = std::basic_string_view<value_t>;

	token_equal_t(const container_t* _container) : container(_container) {}

	template< typename token_type >
	bool operator ()(const token_type& a, const token_type& b) const
	{
		return token_to_view<string_view>(container, a) == token_to_view<string_view>(container, b);
	}
private:
	const container_t* container;
};

////
	template< typename char_type >
class symbol_lexer_t
{
public:
	using char_t = char_type;
	using view_t = std::basic_string_view<char_t>;
	using unicode_t = char32_t;
	using token_t = index_range_t<size_t>;

	symbol_lexer_t(
		size_t token_buffer_size = default_buffer_size,
		size_t sequence_length = default_sequence_length,
		size_t token_bucket_count = default_bucket_count
	) : token_buffer(token_buffer_size)
		, token_sequence(sequence_length)
		, token_collection(token_bucket_count, token_hash_t<token_buffer_t>(&token_buffer), token_equal_t<token_buffer_t>(&token_buffer))
	{
		token_buffer.resize(0);
		token_sequence.resize(0);
	}

	view_t input(const view_t module_name, const view_t file_name, view_t input);

	constexpr bool add_lexical_separator(unicode_t whitespace_char)
	{
		insert_token<token_type::SILENT>(whitespace_char);
		return whitespace_set.insert(whitespace_char).second;
	}
		template< typename container_type >
	constexpr bool add_lexical_separator(const container_type& whitespace_table)
	{
		bool success = true;
		for (auto whitespace : whitespace_table)
			success &= add_lexical_separator(whitespace);
		return success;
	}

	// raw token data
	auto data() const { return token_buffer.data(); }
	auto datasize() const { return token_buffer.size(); }

	// token sequence data
	auto begin() const { return token_sequence.begin(); }
	auto end() const { return token_sequence.end(); }
	auto size() const { return token_sequence.size(); }
	operator bool() { return !token_sequence.empty(); }

	// lexicon data
	auto count() const { return token_collection.size(); }

protected:
	using bounds = index_range_t<const char_t*>;
	bounds get_next_token(bounds bounds);
	auto get_next_char(bounds bounds);

private:
	using token_buffer_t = std::vector<unicode_t>;
	using token_sequence_t = std::vector<token_t>;
	using token_collection_t = std::unordered_set<token_t, token_hash_t<token_buffer_t>, token_equal_t<token_buffer_t>>;
	enum {
		default_buffer_size = 64 * 1024 / sizeof(unicode_t), // lexicon size (in dword) reserved at creation
		default_sequence_length = 32 * 1024, // word & control sequence length (in token) reserved at creation
		default_bucket_count = 16 * 1024, // hash size at creation
	};

	token_buffer_t     token_buffer;
	token_sequence_t   token_sequence;
	token_collection_t token_collection;

	std::set<unicode_t>	whitespace_set;

private:
	enum class token_type { ACTIVE, SILENT };
		template< token_type token_type = token_type::ACTIVE >
	void activate_token(token_t next_token)
	{
		assert(next_token.begin() < next_token.end());
		token_sequence.emplace_back(next_token);
	}
	template<> void activate_token<token_type::SILENT>(token_t) {}

		template< token_type token_type = token_type::ACTIVE >
	token_t insert_token(unicode_t single_char_token)
	{
		token_t token{ { token_buffer.size(), token_buffer.size() + 1 } };
		token_buffer.emplace_back(single_char_token);
		auto inserted = token_collection.insert(token);
		activate_token<token_type>(*inserted.first);
		if (!inserted.second)
		{
			token_buffer.resize(token.begin());
			token.end() = token.begin();
		}
		return token;
	}
		template< token_type token_type = token_type::ACTIVE >
	token_t insert_token(token_t token)
	{
		assert(token_sequence.size() == 0 || token.begin() >= token_sequence.back().end());
		if (token.size())
		{
			auto inserted = token_collection.insert(token);
			activate_token<token_type>(*inserted.first);
			if (!inserted.second)
			{
				std::fill(token_buffer.data() + token.begin(), token_buffer.data() + token.end(), 0);
				token_buffer.resize(token.begin());
				token.end() = token.begin();
			}
		}
		return token;
	}
};

	template< typename char_type >
auto symbol_lexer_t<char_type>::get_next_char(bounds bounds)
{
#if VALIDATE
	const auto begin = bounds.begin();
#endif
	auto next_uchar = reinterpret_cast<const std::make_unsigned_t<char_t>*>(bounds.data());
	char32_t next_char = bounds.size() ? static_cast<char32_t>(*next_uchar) : 0;
	size_t length = 1;
	if (next_char > 0x7f)
	{
		// > 1 Byte;
		static const size_t max_unicode_length = 4;
		uint32_t distance_to_max_length = (((next_char & 0xF0) ^ 0xF0) >> 4);
		length = std::max<size_t>((4u - distance_to_max_length), 2);
		if (length > max_unicode_length || bounds.size() < length)
		{
			length = 0;
			bounds.end() = bounds.begin();
			next_char = char32_t(-1);
		}
	}
	if (length > 0)
		std::tie(next_char, bounds.begin()) = UTF8_to_UTF32(bounds.data(), length);
	assert(bounds.begin() <= bounds.end());
#if VALIDATE
	assert(bounds.data() == begin + length);
#endif
	return std::make_tuple(next_char, bounds);
}

	template< typename char_type >
typename symbol_lexer_t<char_type>::bounds symbol_lexer_t<char_type>::get_next_token(typename symbol_lexer_t<char_type>::bounds bounds)
{
	token_t token{ { token_buffer.size(), token_buffer.size() } };
	token_buffer.reserve(token_buffer.size() + bounds.size());
	typename symbol_lexer_t<char_type>::bounds incompleteBounds = bounds;
	while (bounds.size())
	{
		char32_t next_char;
		std::tie(next_char, bounds) = get_next_char(bounds);
		auto found_whitespace = whitespace_set.find(next_char);
		if (found_whitespace != whitespace_set.end())
		{
			token = insert_token(token);
			token = insert_token(next_char);
			incompleteBounds = bounds;
		}
		else
		{
			token_buffer.emplace_back(next_char);
			++token.end();
		}
	}

	if (token.size())
	{
		//unfinished token
		bounds.begin() = bounds.end() = incompleteBounds.begin();
		token_buffer.resize(token_buffer.size() - token.size());
	}
	return bounds;
}

	template< typename char_type >
std::basic_string_view<char_type> symbol_lexer_t<char_type>::input(const view_t module_name, const view_t file_name, view_t input)
{
	(void)module_name;
	(void)file_name;

	assert(RangeAccessors::get<0>(input) <= RangeAccessors::get<1>(input));
	auto current = input.data();
	auto end = input.data() + input.size();
	auto next = end;
	while (current != next)
	{
		auto nextToken = get_next_token({ { current, end } });
		std::tie( current, next ) = nextToken.bounds();
	}

	return std::basic_string_view<char_type>(next, static_cast<size_t>(std::distance(next, end)));
}

////
	template<>
auto token_to_view< std::basic_string_view<char> >(const symbol_lexer_t<char>* container, const typename symbol_lexer_t<char>::token_t& token)
{
	static_assert(std::is_same_v<char32_t, typename symbol_lexer_t<char>::unicode_t>);
	const char32_t* token_it = container->data() + token.begin();
	const char32_t* const token_end = container->data() + token.end();

	std::basic_string<char> view;
	while (token_it < token_end)
		view += UTF32_to_UTF8(*(token_it++));
	return view;
}

	template< typename out_char_type >
std::basic_ostream<out_char_type>& operator <<(std::basic_ostream<out_char_type>& out, const symbol_lexer_t<out_char_type>& lexer)
{
	using view_type = typename symbol_lexer_t<out_char_type>::view_t;
	out << "unique token count: " << lexer.count() << "; "
		<< "sequence length: " << std::size(lexer) << "; "
		<< "token data size: " << lexer.datasize()
		<< std::endl;
	size_t last_token = 0;
	for (auto token : lexer)
	{
		const bool newToken = (token.end() > last_token);
		last_token = newToken ? token.end() : last_token;
		const auto token_view = token_to_view<view_type>(&lexer, token);
		if (!(out << token_view))
		{
			out.clear();
			out << '?';
		}
	}
	return out << "(eof)" << std::endl;
}
#endif
}} // namespace sbLibX

//static /*inline constexpr*/ char32_t unicode_bidi_white_space[] = {
//	U'\u000C',//; <control>; Cc; 0; WS;;;;; N; FORM FEED( FF );;;;
//	U'\u0020',//; SPACE; Zs; 0; WS;;;;; N;;;;;
//	U'\u1680',//; OGHAM SPACE MARK; Zs; 0; WS;;;;; N;;;;;
//	U'\u2000',//; EN QUAD; Zs; 0; WS; 2002;;;; N;;;;;
//	U'\u2001',//; EM QUAD; Zs; 0; WS; 2003;;;; N;;;;;
//	U'\u2002',//; EN SPACE; Zs; 0; WS; <compat> 0020;;;; N;;;;;
//	U'\u2003',//; EM SPACE; Zs; 0; WS; <compat> 0020;;;; N;;;;;
//	U'\u2004',//; THREE-PER-EM SPACE; Zs; 0; WS; <compat> 0020;;;; N;;;;;
//	U'\u2005',//; FOUR-PER-EM SPACE; Zs; 0; WS; <compat> 0020;;;; N;;;;;
//	U'\u2006',//; SIX-PER-EM SPACE; Zs; 0; WS; <compat> 0020;;;; N;;;;;
//	U'\u2007',//; FIGURE SPACE; Zs; 0; WS; <noBreak> 0020;;;; N;;;;;
//	U'\u2008',//; PUNCTUATION SPACE; Zs; 0; WS; <compat> 0020;;;; N;;;;;
//	U'\u2009',//; THIN SPACE; Zs; 0; WS; <compat> 0020;;;; N;;;;;
//	U'\u200A',//; HAIR SPACE; Zs; 0; WS; <compat> 0020;;;; N;;;;;
//	U'\u2028',//; LINE SEPARATOR; Zl; 0; WS;;;;; N;;;;;
//	U'\u205F',//; MEDIUM MATHEMATICAL SPACE; Zs; 0; WS; <compat> 0020;;;; N;;;;;
//	U'\u3000',//; IDEOGRAPHIC SPACE; Zs; 0; WS; <wide> 0020;;;; N;;;;;
//
//200B; ZW # Cf       ZERO WIDTH SPACE
//
//};


////
#include <iomanip>
#include <iostream>
#include <fstream>
//#include <common/include/sb_common.h>
SB_EXPORT_TYPE int SB_STDCALL test_parser([[maybe_unused]] int argc, [[maybe_unused]] const char* const argv[])
{
	using sbLibX::operator "" _xhash64;
	using sbLibX::StructuredBuffer::get;
	using std::get;
	sbLibX::symbol_lexer_t<char> lexer;
	{
		// TODO : de-hardcode this and use Unicode Data (UCD) instead
		using General_Category = sbLibX::Character::General_Category;
		using Bidi_Class = sbLibX::Character::Bidi_Class;

		const auto& ascii_data = sbLibX::Character::ascii_data;
		constexpr General_Category separator_category = General_Category::Z;
		constexpr Bidi_Class       bidi_space = Bidi_Class::White_Space | Bidi_Class::Paragraph_Separator | Bidi_Class::Segment_Separator;
		static const char32_t whitespaces[] = { U'\0', U'\n', U'\t', U'\v', U'\f', U'\r', U' ', U'\uFEFF' };
		for( const auto& c : ascii_data )
		{
			const auto props = ascii_data[c.code_point];
			const bool is_separator = anybit( props.general_category & separator_category );
			const bool is_space = anybit( props.bidi_class & bidi_space );
			if( is_space || is_separator )
			{
				using code_point_t = std::underlying_type_t<decltype(c.code_point)>;
				std::cout << "U+" << std::setw(2) << std::hex << code_point_t{c.code_point} << " ("
					<< ( props.unicode_1_name_obsolete ? props.unicode_1_name_obsolete : props.name ) << ")\n";
			
				auto categories = props.general_category;
				for( auto currentbit = firstbit( categories ); anybit( currentbit ); currentbit = firstbit( categories ) )
				{
					std::cout << "\t"  << std::underlying_type_t<General_Category>(currentbit) << std::endl;
					categories &= ~currentbit;
				}
			}
		}

		auto test = []( int value )
		{
			auto inverse_value = sbLibX::modular_inverse( value );
			std::cout << value << " * " << inverse_value << " = " << value * inverse_value << std::endl;
		};

		test( 0x00000000 );
		test( 0x00000001 );
		test( 0x00000002 );
		test( 0x00000003 );
		test( 0x00001000 );
		test( 0x00001001 );
		test( 0x40000000 );
		test( 0x40000001 );

		lexer.add_lexical_separator(
			#if 0
				unicode_bidi_white_space
			#else
				whitespaces
			#endif
		);
	}
	{
		const std::string module_name = "";
		const std::string file_name = "ReadMe.md";
		std::ifstream input_file{ file_name };

		std::vector<char> content;
		while (input_file)
		{
			enum { read_size = 32 * 1024, };
			const auto current_size = content.size();
			content.resize(current_size + read_size);
			const auto read_count = input_file.read(content.data() + current_size, read_size).gcount();
			content.resize(current_size + static_cast<size_t>(read_count));
			std::string_view content_data(content.data(), content.size());
			std::string_view unprocessed_content = lexer.input(module_name, file_name, content_data);

			std::copy(unprocessed_content.begin(), unprocessed_content.end(), content.begin());
			content.resize(unprocessed_content.size());
		}
		if (!content.empty())
		{
			content.push_back('\0');
			std::cerr << "Error : Incomplete data :\n" << content.data() << std::endl;
		}
		if (!input_file.eof())
			std::cerr << "Error reading file";
	}
	std::cerr << lexer;

	std::cout.flush();
	return 0;
}
