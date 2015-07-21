/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
*
* This file is part of BRAT
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	 02110-1301, USA.
*/
#ifndef _ParametersDictionary_h_
#define _ParametersDictionary_h_

struct KeywordHelp {
	const string	Name;
	int		MinCount;	// Positive = value, negative = group alone
	int		MaxCount;	// 0 = infinite, positive = value, negative = group
	int		Group;		// 1 to 26 letter identifying a group
	const char	*Default;	// If not NULL a default value is printed

	KeywordHelp
		(const string	&AName,
		 int		AMinCount	= 1,
		 int		AMaxCount	= 1,
		 const char	*ADefault	= NULL,
		 int		AGroup		= 0)
	  : Name(AName),
	    MinCount(AMinCount),
	    MaxCount(AMaxCount),
	    Group(AGroup),
	    Default(ADefault)
	{ };
};

struct KeywordDefinition {
	const string	Name;
	const char	*Type;
	const char	*Description;

	KeywordDefinition (const string &AName, const char *AType, const char *ADescription)
		: Name(AName), Type(AType), Description(ADescription)
	{
	}
};
#define KD(N,T,D)	KeywordDefinition(N,T,D)


struct TypeDefinition {
	const char	*Name;
	const char	*Description;

	TypeDefinition (const char *AName, const char *ADescription)
		: Name(AName), Description(ADescription)
	{
	}
};
#define TD(N,D)	TypeDefinition(N,D)





/*
** Names of known keywords (alphabetical order)
** Any modification (add/remove) must have its counterpart in
** ParametersDictionary.h in order to have up to date help
*/
const string kwALIAS_NAME			= "ALIAS_NAME";
const string kwALIAS_VALUE			= "ALIAS_VALUE";
const string kwDATA_MODE			= "DATA_MODE";
const string kwDATE_AS_PERIOD			= "DATE_AS_PERIOD";
const string kwDISPLAY_ANIMATIONBAR		= "DISPLAY_ANIMATIONBAR";
const string kwDISPLAY_CENTERLAT		= "DISPLAY_CENTERLAT";
const string kwDISPLAY_CENTERLON		= "DISPLAY_CENTERLON";
const string kwDISPLAY_COASTRESOLUTION		= "DISPLAY_COASTRESOLUTION";
const string kwDISPLAY_COLOR			= "DISPLAY_COLOR";
const string kwDISPLAY_COLORBAR			= "DISPLAY_COLORBAR";
const string kwDISPLAY_COLORCURVE		= "DISPLAY_COLORCURVE";
const string kwDISPLAY_COLORTABLE		= "DISPLAY_COLORTABLE";
const string kwDISPLAY_CONTOUR			= "DISPLAY_CONTOUR";
const string kwDISPLAY_CONTOUR_LABEL		= "DISPLAY_CONTOUR_LABEL";
const string kwDISPLAY_CONTOUR_LABEL_NUMBER	= "DISPLAY_CONTOUR_LABEL_NUMBER";
const string kwDISPLAY_CONTOUR_MAXVALUE		= "DISPLAY_CONTOUR_MAXVALUE";
const string kwDISPLAY_CONTOUR_MINVALUE		= "DISPLAY_CONTOUR_MINVALUE";
const string kwDISPLAY_CONTOUR_NUMBER		= "DISPLAY_CONTOUR_NUMBER";
const string kwDISPLAY_ANIMATION		= "DISPLAY_ANIMATION";
const string kwDISPLAY_LINES			= "DISPLAY_LINES";
const string kwDISPLAY_LINEWIDTH		= "DISPLAY_LINEWIDTH";
const string kwDISPLAY_LOGO_URL                 = "DISPLAY_LOGO_URL";
const string kwFILETYPE					= "FILETYPE";
const string kwPRODUCT_LIST				= "PRODUCT_LIST";
const string kwDISPLAY_MAXVALUE			= "DISPLAY_MAXVALUE";
const string kwDISPLAY_MINVALUE			= "DISPLAY_MINVALUE";
const string kwDISPLAY_PLOT_TYPE = "DISPLAY_PLOT_TYPE";
const string kwDISPLAY_NAME			= "DISPLAY_NAME";
const string kwDISPLAY_NUMCOLORLABELS		= "DISPLAY_NUMCOLORLABELS";
const string kwDISPLAY_OPACITY			= "DISPLAY_OPACITY";
const string kwDISPLAY_POINTFILLED		= "DISPLAY_POINTFILLED";
const string kwDISPLAY_POINTGLYPH		= "DISPLAY_POINTGLYPH";
const string kwDISPLAY_POINTS			= "DISPLAY_POINTS";
const string kwDISPLAY_POINTSIZE		= "DISPLAY_POINTSIZE";
const string kwDISPLAY_PROJECTION		= "DISPLAY_PROJECTION";
const string kwDISPLAY_PROPERTIES		= "DISPLAY_PROPERTIES";
const string kwDISPLAY_SOLID_COLOR		= "DISPLAY_SOLID_COLOR";
const string kwDISPLAY_EAST_COMPONENT		= "DISPLAY_EAST_COMPONENT";
const string kwDISPLAY_NORTH_COMPONENT		= "DISPLAY_NORTH_COMPONENT";
const string kwDISPLAY_STIPPLEPATTERN		= "DISPLAY_STIPPLEPATTERN";
const string kwDISPLAY_TITLE			= "DISPLAY_TITLE";
const string kwDISPLAY_XAXIS		= "DISPLAY_XAXIS";
const string kwDISPLAY_XLABEL			= "DISPLAY_XLABEL";
const string kwDISPLAY_YAXIS		= "DISPLAY_YAXIS";
const string kwDISPLAY_YLABEL			= "DISPLAY_YLABEL";
const string kwDISPLAY_XMAXVALUE		= "DISPLAY_XMAXVALUE";
const string kwDISPLAY_XMINVALUE		= "DISPLAY_XMINVALUE";
const string kwDISPLAY_XTICKS			= "DISPLAY_XTICKS";
const string kwDISPLAY_YMAXVALUE		= "DISPLAY_YMAXVALUE";
const string kwDISPLAY_YMINVALUE		= "DISPLAY_YMINVALUE";
const string kwDISPLAY_YTICKS			= "DISPLAY_YTICKS";
const string kwDISPLAY_ZOOM_LAT1		= "DISPLAY_ZOOM_LAT1";
const string kwDISPLAY_ZOOM_LAT2		= "DISPLAY_ZOOM_LAT2";
const string kwDISPLAY_ZOOM_LON1		= "DISPLAY_ZOOM_LON1";
const string kwDISPLAY_ZOOM_LON2		= "DISPLAY_ZOOM_LON2";
const string kwEXPAND_ARRAY			= "EXPAND_ARRAY";
const string kwFIELD				= "FIELD";
const string kwFIELD_COMMENT			= "FIELD_COMMENT";
const string kwFIELD_DATA_MODE			= "FIELD_DATA_MODE";
const string kwFIELD_FILTER			= "FIELD_FILTER";
const string kwFIELD_FORMAT			= "FIELD_FORMAT";
const string kwFIELD_GROUP			= "FIELD_GROUP";
const string kwFIELD_NAME			= "FIELD_NAME";
const string kwFIELD_TITLE			= "FIELD_TITLE";
const string kwFIELD_TYPE			= "FIELD_TYPE";
const string kwFIELD_UNIT			= "FIELD_UNIT";
const string kwFILE				= "FILE";
const string kwLOGFILE				= "LOGFILE";
const string kwOUTPUT				= "OUTPUT";
const string kwOUTPUT_KML                       = "OUTPUT_KML";
const string kwOUTPUT_TITLE			= "OUTPUT_TITLE";
const string kwOUTSIDE_MODE			= "OUTSIDE_MODE";
const string kwPOSITION_MODE			= "POSITION_MODE";
const string kwRECORD				= "RECORD";
const string kwSELECT				= "SELECT";
const string kwUNIT = "UNIT";
const string kwUNIT_ATTR_NAME = "UNIT_ATTR_NAME";
const string kwUNIT_ATTR_VALUE = "UNIT_ATTR_VALUE";
const string kwVERBOSE				= "VERBOSE";
const string kwX				= "X";
const string kwX_COMMENT			= "X_COMMENT";
const string kwX_FILTER				= "X_FILTER";
const string kwX_INTERVALS			= "X_INTERVALS";
const string kwX_LOESS_CUTOFF			= "X_LOESS_CUTOFF";
const string kwX_MAX				= "X_MAX";
const string kwX_MIN				= "X_MIN";
const string kwX_NAME				= "X_NAME";
const string kwX_TITLE				= "X_TITLE";
const string kwX_TYPE				= "X_TYPE";
const string kwX_UNIT				= "X_UNIT";
const string kwY				= "Y";
const string kwY_COMMENT			= "Y_COMMENT";
const string kwY_FILTER				= "Y_FILTER";
const string kwY_INTERVALS			= "Y_INTERVALS";
const string kwY_LOESS_CUTOFF			= "Y_LOESS_CUTOFF";
const string kwY_MAX				= "Y_MAX";
const string kwY_MIN				= "Y_MIN";
const string kwY_NAME				= "Y_NAME";
const string kwY_TITLE				= "Y_TITLE";
const string kwY_TYPE				= "Y_TYPE";
const string kwY_UNIT				= "Y_UNIT";










static const KeywordDefinition KnownKeywords[] = {

/*
**   ============ AAAAAAA ============
*/
KD(kwALIAS_NAME,		"Str",
		"Name of an alias. An alias is a value which can be used\n"
		"anywhere in another value of field by mean of\n"
		"%{NAME} construct. Names are case sensitive.\n"
		"If a name reference (%{XXX}) does not correspond to\n"
		"an actually defined alias, the expansion is an empty\n"
		"string."),
KD(kwALIAS_VALUE,		"Str",
		"The value of the alias. ALIAS_VALUE keyword must have at\n"
		"least as many occurences as the ALIAS_NAME one."),

/*
**   ============ DDDDDDD ============
*/
KD(kwDATA_MODE,			"KW2",
		"Keyword to indicate how data are stored/computed\n"
		"if they have to be registered at the same place.\n"
		"Apply only on field that haven't a *FIELD_DATA_MODE*.\n"),
KD(kwDATE_AS_PERIOD,		"Bool",
		"Indicates if date must be delivered as period (Yes) or as date format (No).\n"),
KD(kwDISPLAY_ANIMATIONBAR,	"Bool",
		"Keyword to indicate if property panel is shown."),
KD(kwDISPLAY_CENTERLAT,		"Flt",
		"Latitude of the projection's center point."),
KD(kwDISPLAY_CENTERLON,		"Flt",
		"Longitude of the projection's center point."),
KD(kwDISPLAY_COASTRESOLUTION,	"KW6",
		"Resolution of the coast line drawm on the map.\n"
		"Recommended value: low."),
KD(kwDISPLAY_COLOR,		"KW7",
		"Color name of the XY plot field."),
KD(kwDISPLAY_COLORBAR,		"Bool",
		"Keyword to indicate if color bar (legend) is shown."),
KD(kwDISPLAY_COLORCURVE,	"KW5",
		"Set the color table on a specific curve."),
KD(kwDISPLAY_COLORTABLE,		"Str",
		"Name of a predefined color table:\n"
		"    Aerosol\n"
		"    Blackbody\n"
		"    BlackToWhite\n"
		"    Cloud\n"
		"    Ozone\n"
		"    GreenToRed\n"
		"    Rainbow\n"
		"    RedToGreen\n"
		"    WhiteToBlack\n"
		"or name of a file containing the color table definition\n"
		"(absolute or relative path)."),
KD(kwDISPLAY_CONTOUR,		"Bool",
		"Indicates if the contour layer of the field is shown or not."),
KD(kwDISPLAY_CONTOUR_LABEL,	"Bool",
		"Indicate if the contour labels (value) are shown or not."),
KD(kwDISPLAY_CONTOUR_LABEL_NUMBER, "Int",
		"Number of labels on each contour."),
KD(kwDISPLAY_CONTOUR_MINVALUE,	"Flt",
		"Minimum value to use to contour calculation.\n"
		"Default values are the same as the color scale one."),
KD(kwDISPLAY_CONTOUR_MAXVALUE,	"Flt",
		"Maximum value to use to contour calculation.\n"
		"Default values are the same as the color scale one."),
KD(kwDISPLAY_CONTOUR_NUMBER,	"Int",
		"Number of contour lines to generate\n"
		"(equally spaced contour values between specified range\n"
		"See DISPLAY_CONTOUR_MINVALUE and DISPLAY_CONTOUR_MAXVALUE)."),
KD(kwDISPLAY_ANIMATION,	"Bool",
		"For world plot. When sevral same fields (same names) are in input, this\n"
		"parameter indicates if fields are displayed in different frames\n"
		"to be animated in the plot (animation = true) or if field are stacked (overlaid)."),
KD(kwDISPLAY_LINES,		"Bool",
		"Indicates if line is displayed in a XY plot (for the field)."),
KD(kwDISPLAY_LINEWIDTH,		"Flt",
		"Width of the line (XY plot, for the field)."),
KD(kwDISPLAY_LOGO_URL,		"Str",
		"URL of a logo image for use with KML export."),
KD(kwDISPLAY_MINVALUE,		"Flt",
		"Minimum color table value to use in plot."),
KD(kwDISPLAY_MAXVALUE,		"Flt",
		"Maximum color table value to use in plot."),
KD(kwDISPLAY_PLOT_TYPE,		"Int",
		"Type of plot: 0 = YFX, 1 = ZFXY, 2 = ZFLatLon."),
KD(kwDISPLAY_NAME,		"Str",
		"Field name to be displayed."),
KD(kwDISPLAY_NUMCOLORLABELS,	"Int",
		"Number of labels shown on the plot's color bar."),
KD(kwDISPLAY_OPACITY,		"Flt",
		"Opacity of the color value map image:\n"
		"1.0 color is totally opaque\n"
		"0.0 is completely transparent."),
KD(kwDISPLAY_POINTS,		"Bool",
		"Indicates if points are displayed in a XY plot\n"
		"(for the field)."),
KD(kwDISPLAY_POINTSIZE,		"Flt",
		"Size of the points (XY plot, for the field)."),
KD(kwDISPLAY_POINTFILLED,	"Bool",
		"Indicates if points are filled or not."),
KD(kwDISPLAY_POINTGLYPH,	"KW8",
		"Glyph of the points (field) (XY plot)."),
KD(kwDISPLAY_PROJECTION,	"KW9",
		"Projection to use for mapping the world globe."),
KD(kwDISPLAY_PROPERTIES,	"Bool",
		"Indicates if property panel is shown."),
KD(kwDISPLAY_SOLID_COLOR,	"Bool",
		"Indicates if color layer of the field is shown or not."),
KD(kwDISPLAY_EAST_COMPONENT,	"Bool",
		"Indicates if this field is the East component of a vector plot."),
KD(kwDISPLAY_NORTH_COMPONENT,	"Bool",
		"Indicates if this field is the North component of a vector plot."),
KD(kwDISPLAY_STIPPLEPATTERN,	"KW10",
		"Stipple pattern for the line (field) (XY plot)."),
KD(kwDISPLAY_TITLE,		"Str",
		"Title of the plot to be displayed."),
KD(kwDISPLAY_XAXIS,		"Str",
		"X axis to be displayed."),
KD(kwDISPLAY_XLABEL,		"Str",
		"X axis label to be displayed."),
KD(kwDISPLAY_YAXIS,		"Str",
		"Y axis to be displayed."),
KD(kwDISPLAY_YLABEL,		"Str",
		"Y axis label to be displayed."),
KD(kwDISPLAY_XMINVALUE,		"Flt",
		"Minimum X coordinate value to use in XY plot."),
KD(kwDISPLAY_XMAXVALUE,		"Flt",
		"Maximum X coordinate value to use in XY plot."),
KD(kwDISPLAY_XTICKS,		"Int",
		"Number of ticks for the X axis."),
KD(kwDISPLAY_YMINVALUE,		"Flt",
		"Minimum Y coordinate value to use in XY plot."),
KD(kwDISPLAY_YMAXVALUE,		"Flt",
		"Maximum Y coordinate value to use in XY plot."),
KD(kwDISPLAY_YTICKS,		"Int",
		"Number of ticks for the Y axis."),
KD(kwDISPLAY_ZOOM_LON1,		"Flt",
		"Zoom area west side."),
KD(kwDISPLAY_ZOOM_LON2,		"Flt",
		"Zoom area east side."),
KD(kwDISPLAY_ZOOM_LAT1,		"Flt",
		"Zoom area south side."),
KD(kwDISPLAY_ZOOM_LAT2,		"Flt",
		"Zoom area north side."),
/*
**   ============ EEEEEEE ============
*/
KD(kwEXPAND_ARRAY,		"Bool",
		"Indicates if data arrays must be expanded,\n"
                "i.e each element value are transformed into a simple value.\n"
		"If output contains several data arrays, these arrays must\n "
                "have the same dimensions, otherwise they are not expanded\n"
		"Note 1: expand array increased by 'array dimensions' the output\n"
		"Note 2: this parameter does not apply to 'high resolution data'\n"
                "that are processed separately\n"),
/*
**   ============ FFFFFFF ============
*/
KD(kwFIELD,			"Expr",
		"Expression of fields of *RECORD* to take into account."),

KD(kwFIELD_COMMENT,		"Str",
		"Comment used to describe data fields\n"),

KD(kwFIELD_DATA_MODE,		"KW2",
		"Keyword to indicate how data are stored/computed\n"
		"if they have to be registered at the same place."),
KD(kwFIELD_FILTER,		"KS1",
		"How to filter the data."),
KD(kwFIELD_FORMAT,		"Fmt",
		"Output format for *FIELD*."),
KD(kwFIELD_GROUP,		"Int",
		"Group id from where belongs *FIELD*. generally used to\n"
		"group many fields in one plot."),
KD(kwFIELD_NAME,		"Name",
		"Name of the *FIELD* data"),
KD(kwFIELD_TITLE,		"Str",
		"Long name describing *FIELD*. The one which should\n"
		"appear in graphics on axis or legends, for example."),
KD(kwFIELD_TYPE,		"KW1",
		"Type of *FIELD* data."),
KD(kwFIELD_UNIT,		"Unit",
		"Unit of *FIELD* expression."),
KD(kwFILE,			"Str",
		"Input file name."),
/*
**   ============ LLLLLLLL ============
*/
KD(kwLOGFILE,			"Str",
		"Path a log file. Use to log into a file, instead of standard output."),
/*
**   ============ OOOOOOO ============
*/
KD(kwOUTPUT,			"Str",
		"Name of created/modified file."),
KD(kwOUTPUT_KML,		 "Str",
		"Name of created KML file."),
KD(kwOUTPUT_TITLE,		"Str",
		"Title of created/modified file (string describing the\n"
		"content and which should appear as a graphic title,\n"
		"for example)."),
KD(kwOUTSIDE_MODE,		"KW4",
		"How data outside limits are managed."),
/*
**   ============ PPPPPPP ============
*/
KD(kwPOSITION_MODE,		"KW3",
		"How position is computed."),
/*
**   ============ RRRRRRR ============
*/
KD(kwRECORD,			"Str",
		"Record set name to take into account for a file."),
/*
**   ============ SSSSSSS ============
*/
KD(kwSELECT,			"Expr",
		"True for record values selected."),
/*
**   ============ UUUUUUU ============
*/
KD(kwUNIT_ATTR_NAME,		"Str",
		"Name of a specific (custom) unit of a Brat dataset field.\n"
		"Use only for Netcdf attributes (global or varaible level)\n"
		"The name of global attribute is the attribute's name itself.\n"
    "The name of the variable's attribute is made of the name of the variable\n"
    "followed by the name of the attribute: for instance, if the variable 'Grid'\n"
    "has an attribute named 'date', the name of a Brat dataset field is 'Grid_date'"),

KD(kwUNIT_ATTR_VALUE,		"Str",
		"Value of a specific (custom) unit of a Brat dataset field.\n"
		"See UNIT_ATTR_NAME keyword."),

/*
**   ============ VVVVVVV ============
*/
KD(kwVERBOSE,			"Int",
		"Amount of output: 0=None...5=Debug."),
/*
**   ============ XXXXXXX ============
*/
KD(kwX,				"Expr",
		"Expression of fields of *RECORD* to take into account."),
KD(kwX_COMMENT,		"Str",
		"Comment used to describe X fields\n"),
KD(kwX_INTERVALS,		"Int",
		"Number of intervals between Min and Max for *X*."),
KD(kwX_NAME,			"Name",
		"Name of the *X* data"),
KD(kwX_MAX,			"Flt",
		"Max value for *X* expression storage."),
KD(kwX_MIN,			"Flt",
		"Min value for *X* expression storage."),
KD(kwX_TITLE,			"Str",
		"Long name describing *X*. The one which should appear\n"
		"in graphics on axis or legends, for example."),
KD(kwX_TYPE,			"KW1",
		"Type of *X* data (normally X, T or longitude)."),
KD(kwX_UNIT,			"Unit",
		"Unit of *X* expression"),
KD(kwX_FILTER,			"KS1",
		"How to filter the X values."),
KD(kwX_LOESS_CUTOFF,		"Int",
		"Distance (in dots) where LOESS filter reaches 0 along\n"
		"X axis. Must be an odd integer. If 1 or 0, Distance\n"
		"computation is disabled. Needed only if at least\n"
		"one filter is asked."),
/*
**   ============ YYYYYYY ============
*/
KD(kwY,				"Expr",
		"Expression of fields of *RECORD* to take into account."),
KD(kwY_COMMENT,		"Str",
		"Comment used to describe X fields\n"),
KD(kwY_INTERVALS,		"Int",
		"Number of intervals between Min and Max for *Y*."),
KD(kwY_NAME,			"Name",
		"Name of the *Y* data."),
KD(kwY_MAX,			"Flt",
		"Max value for *Y* expression storage."),
KD(kwY_MIN,			"Flt",
		"Min value for *Y* expression storage."),
KD(kwY_TITLE,			"Str",
		"Long name describing *Y*. The one which should appear\n"
		"in graphics on axis or legends, for example."),
KD(kwY_TYPE,			"KW1",
		"Type of *Y* data (normally X, T or longitude)."),
KD(kwY_UNIT,			"Unit",
		"Unit of *Y* expression."),
KD(kwY_FILTER,			"KS1",
		"How to filter the Y values."),
KD(kwY_LOESS_CUTOFF,		"Int",
		"Distance (in dots) where LOESS filter reaches 0 along\n"
		"Y axis. Must be an odd integer. If 1 or 0, Distance\n"
		"computation is disabled. Needed only if at least\n"
		"one filter is asked."),

/*
** Sentinell indicating the last one
*/
KeywordDefinition("", NULL, NULL)

};



const TypeDefinition KnownTypes[] = {

TD("Name",	"String beginning with a letter and containing only letters,\n"
		"digits and '_'"),
TD("Bool",	"Boolean\n"
		"true  if : YES/Y/TRUE/T/OUI/O/VRAI/V/1\n"
		"false if : NO/N/FALSE/F/NON/N/FAUX/0"),
TD("Flt",	"Floating point number"),
TD("Int",	"Integer"),
TD("Expr",	"Combination of fields of the current record.\n"
		"An expression which can contain function calls like\n"
		"trigonometric, conversion, test..."),
TD("Str",	"String. Leading and trailing blanks are ignored."),
TD("Unit",	"Unit string conforming to Udunits package and the special\n"
		"keyword 'DATE' which means that the data is a date."),
TD("Fmt",	"Format of field (for ascii conversion). String explaining\n"
		"how to output a value."),
TD("KW1",	"Keywords: X/Y/Z/T/Latitude/Longitude/Data"),
TD("KW2",	"Keywords: FIRST/LAST/MIN/MAX/MEAN/STDDEV/COUNT/SUM/PRODUCT"),
TD("KW3",	"Keywords: EXACT/NEAREST\n"
		"    EXACT: Measures which are exactly on boundaries\n"
		"           (grid lines) are keeped others are ignored\n"
		"    NEAREST: Get the nearest boundary."),
TD("KW4",	"Keywords: STRICT/RELAXED/BLACK_HOLE\n"
		"    STRICT: Measure outside limits are ignored\n"
		"    RELAXED: Measure outside limits are ignored if\n"
		"             they are farther than a half step from\n"
		"             the limit otherwise they are considered\n"
		"	      being on the limit.\n"
		"    BLACK_HOLE: Everything outside the limit is\n"
		"             considered to be on the limit."),
TD("KW5",	"Keywords: cosine, linear, sqrt (square root)"),
TD("KW6",	"Keywords: In incresing resolution: crude, low, intermediate,\n"
		"	   full"),
TD("KW7",	"Keywords: AQUAMARINE, BLACK, BLUE, BLUE VIOLET, BROWN,\n"
		"          CADET BLUE, CORAL, CORNFLOWER BLUE, CYAN, DARK GREY,\n"
		"          DARK GREEN, DARK OLIVE GREEN, DARK ORCHID,\n"
		"          DARK SLATE BLUE, DARK SLATE GREY, DARK TURQUOISE,\n"
		"          DIM GREY, FIREBRICK, FOREST GREEN, GOLD, GOLDENROD,\n"
		"          GREY, GREEN, GREEN YELLOW, INDIAN RED, KHAKI,\n"
		"          LIGHT BLUE, LIGHT GREY, LIGHT STEEL BLUE, LIME GREEN,\n"
		"          MAGENTA, MAROON, MEDIUM AQUAMARINE, MEDIUM BLUE,\n"
		"          MEDIUM FOREST GREEN, MEDIUM GOLDENROD, MEDIUM ORCHID,\n"
		"          MEDIUM SEA GREEN, MEDIUM SLATE BLUE,\n"
		"          MEDIUM SPRING GREEN, MEDIUM TURQUOISE,\n"
		"          MEDIUM VIOLET RED, MIDNIGHT BLUE, NAVY, ORANGE,\n"
		"          ORANGE RED, ORCHID, PALE GREEN, PINK, PLUM, PURPLE,\n"
		"          RED, SALMON, SEA GREEN, SIENNA, SKY BLUE, SLATE BLUE,\n"
		"          SPRING GREEN, STEEL BLUE, TAN, THISTLE, TURQUOISE,\n"
		"          VIOLET, VIOLET RED, WHEAT, WHITE, YELLOW,\n"
		"          YELLOW GREEN."),
TD("KW8",	"Keywords: ARROW, CIRCLE, CROSS, DASH, DIAMOND, HOOKEDARROW,\n"
		"          SQUARE, THICKARROW, THICKCROSS, TRIANGLE"),
TD("KW9",	"Keywords: 3D, Azimuthal Equidistant, Lambert Cylindrical,\n"
		"          Lambert Azimuthal, Mercator, Mollweide, Plate Caree,\n"
		"          Robinson"),
TD("KW10",	"Keywords: DASHTINY, DASH, DASHDOT, DOT, FULL"),
TD("KS1",	"Set of keywords from: NONE, LOESS_SMOOTH, LOESS_EXTRAPOLATE,\n"
		"          LOESS (LOESS means LOESS_SMOOTH and LOESS_EXTRAPOLATE)"),


/*
** Sentinell indicating the last one
*/
TypeDefinition(NULL, NULL)

};


#endif
