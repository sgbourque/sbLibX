#if  (  !defined( SBDEF_GC_PROPERTY ) \
     && !defined( SBDEF_CCC_PROPERTY ) \
     && !defined( SBDEF_BIDI_PROPERTY ) )
#error "should define SBDEF_GENERAL_CATEGORY_PROPERTY(property_name, category_alias, category_name, misc_alias, value)\n" \
       "or            SBDEF_CANONICAL_COMBINING_CLASS_PROPERTY(property_name, value, class_alias, class_name)\n" \
       "or            SBDEF_BIDI_PROPERTY(property_name, bidi_alias, bidi_name, value)\n"
#endif

// adapted from Unicode 13.0.0: UCD / PropertyValueAliases.txt

// General_Category (gc)
#ifndef SBDEF_GC_PROPERTY
#define SBDEF_GC_PROPERTY( ... )
#endif
SBDEF_GC_PROPERTY( gc , Cc, Control              ,          cntrl,                          ( 1 <<  0 ) )
SBDEF_GC_PROPERTY( gc , Cf, Format               ,               ,                          ( 1 <<  1 ) )
SBDEF_GC_PROPERTY( gc , Cn, Unassigned           ,               ,                          ( 1 <<  2 ) )
SBDEF_GC_PROPERTY( gc , Co, Private_Use          ,               ,                          ( 1 <<  3 ) )
SBDEF_GC_PROPERTY( gc , Cs, Surrogate            ,               ,                          ( 1 <<  4 ) )
SBDEF_GC_PROPERTY( gc , C , Other                ,               ,           ( Cc | Cf | Cn | Co | Cs ) )
SBDEF_GC_PROPERTY( gc , Ll, Lowercase_Letter     ,               ,                          ( 1 <<  5 ) )
SBDEF_GC_PROPERTY( gc , Lm, Modifier_Letter      ,               ,                          ( 1 <<  6 ) )
SBDEF_GC_PROPERTY( gc , Lo, Other_Letter         ,               ,                          ( 1 <<  7 ) )
SBDEF_GC_PROPERTY( gc , Lt, Titlecase_Letter     ,               ,                          ( 1 <<  8 ) )
SBDEF_GC_PROPERTY( gc , Lu, Uppercase_Letter     ,               ,                          ( 1 <<  9 ) )
SBDEF_GC_PROPERTY( gc , LC, Cased_Letter         ,               ,                     ( Ll | Lt | Lu ) )
SBDEF_GC_PROPERTY( gc , L , Letter               ,               ,           ( Ll | Lm | Lo | Lt | Lu ) )
SBDEF_GC_PROPERTY( gc , Mc, Spacing_Mark         ,               ,                          ( 1 << 10 ) )
SBDEF_GC_PROPERTY( gc , Me, Enclosing_Mark       ,               ,                          ( 1 << 11 ) )
SBDEF_GC_PROPERTY( gc , Mn, Nonspacing_Mark      ,               ,                          ( 1 << 12 ) )
SBDEF_GC_PROPERTY( gc , M , Mark                 , Combining_Mark,                     ( Mc | Me | Mn ) )
SBDEF_GC_PROPERTY( gc , Nd, Decimal_Number       ,          digit,                          ( 1 << 13 ) )
SBDEF_GC_PROPERTY( gc , Nl, Letter_Number        ,               ,                          ( 1 << 14 ) )
SBDEF_GC_PROPERTY( gc , No, Other_Number         ,               ,                          ( 1 << 15 ) )
SBDEF_GC_PROPERTY( gc , N , Number               ,               ,                     ( Nd | Nl | No ) )
SBDEF_GC_PROPERTY( gc , Pc, Connector_Punctuation,               ,                          ( 1 << 16 ) )
SBDEF_GC_PROPERTY( gc , Pd, Dash_Punctuation     ,               ,                          ( 1 << 17 ) )
SBDEF_GC_PROPERTY( gc , Pe, Close_Punctuation    ,               ,                          ( 1 << 18 ) )
SBDEF_GC_PROPERTY( gc , Pf, Final_Punctuation    ,               ,                          ( 1 << 19 ) )
SBDEF_GC_PROPERTY( gc , Pi, Initial_Punctuation  ,               ,                          ( 1 << 20 ) )
SBDEF_GC_PROPERTY( gc , Po, Other_Punctuation    ,               ,                          ( 1 << 21 ) )
SBDEF_GC_PROPERTY( gc , Ps, Open_Punctuation     ,               ,                          ( 1 << 22 ) )
SBDEF_GC_PROPERTY( gc , P , Punctuation          ,          punct, ( Pc | Pd | Pe | Pf | Pi | Po | Ps ) )
SBDEF_GC_PROPERTY( gc , Sc, Currency_Symbol      ,               ,                          ( 1 << 23 ) )
SBDEF_GC_PROPERTY( gc , Sk, Modifier_Symbol      ,               ,                          ( 1 << 24 ) )
SBDEF_GC_PROPERTY( gc , Sm, Math_Symbol          ,               ,                          ( 1 << 25 ) )
SBDEF_GC_PROPERTY( gc , So, Other_Symbol         ,               ,                          ( 1 << 26 ) )
SBDEF_GC_PROPERTY( gc , S , Symbol               ,               ,                ( Sc | Sk | Sm | So ) )
SBDEF_GC_PROPERTY( gc , Zl, Line_Separator       ,               ,                          ( 1 << 27 ) )
SBDEF_GC_PROPERTY( gc , Zp, Paragraph_Separator  ,               ,                          ( 1 << 28 ) )
SBDEF_GC_PROPERTY( gc , Zs, Space_Separator      ,               ,                          ( 1 << 29 ) )
SBDEF_GC_PROPERTY( gc , Z , Separator            ,               ,                     ( Zl | Zp | Zs ) )
// @missing: 0000..10FFFF; General_Category; Unassigned
#undef SBDEF_GC_PROPERTY

// Canonical_Combining_Class( ccc )
#ifndef SBDEF_CCC_PROPERTY
#define SBDEF_CCC_PROPERTY( ... )
#endif
SBDEF_CCC_PROPERTY( ccc, 0, NR, Not_Reordered )
SBDEF_CCC_PROPERTY( ccc, 1, OV, Overlay )
SBDEF_CCC_PROPERTY( ccc, 6, HANR, Han_Reading )
SBDEF_CCC_PROPERTY( ccc, 7, NK, Nukta )
SBDEF_CCC_PROPERTY( ccc, 8, KV, Kana_Voicing )
SBDEF_CCC_PROPERTY( ccc, 9, VR, Virama )
SBDEF_CCC_PROPERTY( ccc, 10, CCC10_alias, CCC10 )
SBDEF_CCC_PROPERTY( ccc, 11, CCC11_alias, CCC11 )
SBDEF_CCC_PROPERTY( ccc, 12, CCC12_alias, CCC12 )
SBDEF_CCC_PROPERTY( ccc, 13, CCC13_alias, CCC13 )
SBDEF_CCC_PROPERTY( ccc, 14, CCC14_alias, CCC14 )
SBDEF_CCC_PROPERTY( ccc, 15, CCC15_alias, CCC15 )
SBDEF_CCC_PROPERTY( ccc, 16, CCC16_alias, CCC16 )
SBDEF_CCC_PROPERTY( ccc, 17, CCC17_alias, CCC17 )
SBDEF_CCC_PROPERTY( ccc, 18, CCC18_alias, CCC18 )
SBDEF_CCC_PROPERTY( ccc, 19, CCC19_alias, CCC19 )
SBDEF_CCC_PROPERTY( ccc, 20, CCC20_alias, CCC20 )
SBDEF_CCC_PROPERTY( ccc, 21, CCC21_alias, CCC21 )
SBDEF_CCC_PROPERTY( ccc, 22, CCC22_alias, CCC22 )
SBDEF_CCC_PROPERTY( ccc, 23, CCC23_alias, CCC23 )
SBDEF_CCC_PROPERTY( ccc, 24, CCC24_alias, CCC24 )
SBDEF_CCC_PROPERTY( ccc, 25, CCC25_alias, CCC25 )
SBDEF_CCC_PROPERTY( ccc, 26, CCC26_alias, CCC26 )
SBDEF_CCC_PROPERTY( ccc, 27, CCC27_alias, CCC27 )
SBDEF_CCC_PROPERTY( ccc, 28, CCC28_alias, CCC28 )
SBDEF_CCC_PROPERTY( ccc, 29, CCC29_alias, CCC29 )
SBDEF_CCC_PROPERTY( ccc, 30, CCC30_alias, CCC30 )
SBDEF_CCC_PROPERTY( ccc, 31, CCC31_alias, CCC31 )
SBDEF_CCC_PROPERTY( ccc, 32, CCC32_alias, CCC32 )
SBDEF_CCC_PROPERTY( ccc, 33, CCC33_alias, CCC33 )
SBDEF_CCC_PROPERTY( ccc, 34, CCC34_alias, CCC34 )
SBDEF_CCC_PROPERTY( ccc, 35, CCC35_alias, CCC35 )
SBDEF_CCC_PROPERTY( ccc, 36, CCC36_alias, CCC36 )
SBDEF_CCC_PROPERTY( ccc, 84, CCC84_alias, CCC84 )
SBDEF_CCC_PROPERTY( ccc, 91, CCC91_alias, CCC91 )
SBDEF_CCC_PROPERTY( ccc, 103, CCC103_alias, CCC103 )
SBDEF_CCC_PROPERTY( ccc, 107, CCC107_alias, CCC107 )
SBDEF_CCC_PROPERTY( ccc, 118, CCC118_alias, CCC118 )
SBDEF_CCC_PROPERTY( ccc, 122, CCC122_alias, CCC122 )
SBDEF_CCC_PROPERTY( ccc, 129, CCC129_alias, CCC129 )
SBDEF_CCC_PROPERTY( ccc, 130, CCC130_alias, CCC130 )
SBDEF_CCC_PROPERTY( ccc, 132, CCC132_alias, CCC132 )
SBDEF_CCC_PROPERTY( ccc, 133, CCC133_alias, CCC133 )
SBDEF_CCC_PROPERTY( ccc, 200, ATBL, Attached_Below_Left )
SBDEF_CCC_PROPERTY( ccc, 202, ATB, Attached_Below )
SBDEF_CCC_PROPERTY( ccc, 214, ATA, Attached_Above )
SBDEF_CCC_PROPERTY( ccc, 216, ATAR, Attached_Above_Right )
SBDEF_CCC_PROPERTY( ccc, 218, BL, Below_Left )
SBDEF_CCC_PROPERTY( ccc, 220, B, Below )
SBDEF_CCC_PROPERTY( ccc, 222, BR, Below_Right )
SBDEF_CCC_PROPERTY( ccc, 224, L, Left )
SBDEF_CCC_PROPERTY( ccc, 226, R, Right )
SBDEF_CCC_PROPERTY( ccc, 228, AL, Above_Left )
SBDEF_CCC_PROPERTY( ccc, 230, A, Above )
SBDEF_CCC_PROPERTY( ccc, 232, AR, Above_Right )
SBDEF_CCC_PROPERTY( ccc, 233, DB, Double_Below )
SBDEF_CCC_PROPERTY( ccc, 234, DA, Double_Above )
SBDEF_CCC_PROPERTY( ccc, 240, IS, Iota_Subscript )
#undef SBDEF_CCC_PROPERTY

// Bidi_Class (bc)
#ifndef SBDEF_BIDI_PROPERTY
#define SBDEF_BIDI_PROPERTY( ... )
#endif
SBDEF_BIDI_PROPERTY( bc, AL , Arabic_Letter           , ( 1 <<  0 ) )
SBDEF_BIDI_PROPERTY( bc, AN , Arabic_Number           , ( 1 <<  1 ) )
SBDEF_BIDI_PROPERTY( bc, B  , Paragraph_Separator     , ( 1 <<  2 ) )
SBDEF_BIDI_PROPERTY( bc, BN , Boundary_Neutral        , ( 1 <<  3 ) )
SBDEF_BIDI_PROPERTY( bc, CS , Common_Separator        , ( 1 <<  4 ) )
SBDEF_BIDI_PROPERTY( bc, EN , European_Number         , ( 1 <<  5 ) )
SBDEF_BIDI_PROPERTY( bc, ES , European_Separator      , ( 1 <<  6 ) )
SBDEF_BIDI_PROPERTY( bc, ET , European_Terminator     , ( 1 <<  7 ) )
SBDEF_BIDI_PROPERTY( bc, FSI, First_Strong_Isolate    , ( 1 <<  8 ) )
SBDEF_BIDI_PROPERTY( bc, L  , Left_To_Right           , ( 1 <<  9 ) )
SBDEF_BIDI_PROPERTY( bc, LRE, Left_To_Right_Embedding , ( 1 << 10 ) )
SBDEF_BIDI_PROPERTY( bc, LRI, Left_To_Right_Isolate   , ( 1 << 11 ) )
SBDEF_BIDI_PROPERTY( bc, LRO, Left_To_Right_Override  , ( 1 << 12 ) )
SBDEF_BIDI_PROPERTY( bc, NSM, Nonspacing_Mark         , ( 1 << 13 ) )
SBDEF_BIDI_PROPERTY( bc, ON , Other_Neutral           , ( 1 << 14 ) )
SBDEF_BIDI_PROPERTY( bc, PDF, Pop_Directional_Format  , ( 1 << 15 ) )
SBDEF_BIDI_PROPERTY( bc, PDI, Pop_Directional_Isolate , ( 1 << 16 ) )
SBDEF_BIDI_PROPERTY( bc, R  , Right_To_Left           , ( 1 << 17 ) )
SBDEF_BIDI_PROPERTY( bc, RLE, Right_To_Left_Embedding , ( 1 << 18 ) )
SBDEF_BIDI_PROPERTY( bc, RLI, Right_To_Left_Isolate   , ( 1 << 19 ) )
SBDEF_BIDI_PROPERTY( bc, RLO, Right_To_Left_Override  , ( 1 << 20 ) )
SBDEF_BIDI_PROPERTY( bc, S  , Segment_Separator       , ( 1 << 21 ) )
SBDEF_BIDI_PROPERTY( bc, WS , White_Space             , ( 1 << 22 ) )
#undef SBDEF_BIDI_PROPERTY
