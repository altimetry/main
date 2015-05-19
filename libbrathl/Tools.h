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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef _Tools_h_
#define _Tools_h_

#include <cstdarg>
#include <cstring>
#include "Stl.h"

#include "brathl_error.h"
#include "brathl.h"

#include "BratObject.h"
using namespace brathl;

namespace brathl
{


typedef bitset<32> bitSet32;


//typedef struct _type_union type_union;
//
//typedef enum _type_val {
//  T_END = 0, /* terminator flag for variable-length array */
//  T_INT,
//  T_FLOAT,
//  T_DOUBLE,
//  T_CHAR,
//  T_STRING,
//  T_DOUBLE_PTR,
//} type_val;
//
//struct _type_union
//{
//  type_val t;
//  union {
//    float f;
//    double d;
//    int32_t i;
//    char c;
//    const char *s;
//    double *d_ptr;
//  };
//};

/*
* Treat ac_list, ac_arg as lvalues.
*/

//#define va_start_assign(list, var) ((list) = ((char*)&var))
//#define va_start_assign(list, var) ((list) = (reinterpret_cast<va_list>(&var)))
/*
#define va_assign(lvalp, type, rval) \
*((type*)lvalp) = rval; \
va_arg(lvalp, type);

#define VA_FLOAT double
#define VA_CHAR uint32_t

#define TYPE_FUN(v2u, tv, T, v) \
  type_union v2u(T v) { \
  type_union r; \
  r.v = v; \
  r.t = tv; \
  return r; \
}

#define TYPE_UNION_SET(tv, v) \
  type_union r; \
  r.v = v; \
  r.t = tv; \
  return r;
*/

//static const type_union END = { T_END };
//
//typedef vector<type_union> vectorTypeUnion;

//-------------------------------------------------------------
//------------------- CVectorTypeUnion class --------------------
//-------------------------------------------------------------
//
//
//class CVectorTypeUnion : public vectorTypeUnion
//{
//public:
//  CVectorTypeUnion() { };  
//  virtual ~CVectorTypeUnion() { RemoveAll(); };
//
//  virtual void Insert(const type_union& u) { this->push_back(u); };
//
//  virtual void RemoveAll() { vectorTypeUnion::clear(); };
//
//  virtual void Dump(ostream& fOut  = cerr);
//
//};

/** \addtogroup tools Tools
  @{ */

/**
  Tools management class.

 This class provides various static utility methods

 \version 1.0
*/


class CTools
{
public:
  CTools();
  ~CTools();

  static bool IsEmpty(const char *pstrString);

  static double Int(double dValue);

  static double Frac(double value);

  static double Round(double value);

  static double Rnd (double value, double precision);
  
  static void SetDefaultValue(float& value);
  static void SetDefaultValue(double& value);
  static void SetDefaultValue(int8_t& value);
  static void SetDefaultValue(uint8_t& value);
  static void SetDefaultValue(int16_t& value);
  static void SetDefaultValue(uint16_t& value);
  static void SetDefaultValue(int32_t& value);
  static void SetDefaultValue(uint32_t& value);
  static void SetDefaultValue(int64_t& value);
  static void SetDefaultValue(uint64_t& value);

  static bool IsDefaultValue(const float value);
  static bool IsDefaultValue(const double value);
  static bool IsDefaultValue(const int8_t value);
  static bool IsDefaultValue(const uint8_t value);
  static bool IsDefaultValue(const int16_t value);
  static bool IsDefaultValue(const uint16_t value);
  static bool IsDefaultValue(const int32_t value);
  static bool IsDefaultValue(const uint32_t value);
  static bool IsDefaultValue(const int64_t value);
  static bool IsDefaultValue(const uint64_t value);

// Return false if Source is default value or does not fall
// in Dest range
  static bool CastValue(int32_t &Dest, const double Source);

  static bool IsZero(double X);
  static bool AreEqual(double X, double Y);
  static bool AreEqual(double X, double Y, double compareEpsilon);
  static bool AreValidMercatorLatitude(double lat);

// Default value is greater than anything except default value (they are equal)
  static int Compare(double X, double Y, double compareEpsilon = CTools::m_CompareEpsilon);

  static bool Compare(const char *str1, const char *str2);
  static bool CompareNoCase(const char *str1, const char *str2);
  static bool CompareNoCase(const string& str1, const string& str2);

  static int32_t FindNoCase(const string& src, const string& findWhat, uint32_t pos = 0);
  static int32_t FindNoCase(const char* src, const char* findWhat, uint32_t pos = 0);
  
  static int32_t RFindNoCase(const string& src, const string& findWhat, uint32_t pos = 0);
  static int32_t RFindNoCase(const char* src, const char* findWhat, uint32_t pos = 0);


  /** Remove all the blank characters at the  beginning and the end of a string.
    Blank characters are identified by the function isspace (3C).
    \param str [in/out] : string to be modified
    \return a pointer to the string*/
  static char * Trim(char *str);

  /** Compare the two strings str1 and str2,
     while being unaware of the differences between upper-case and lower-case.
     This  method is thus identical to the function strcasecmp (3C) with the following difference :
     str1, str2 can be NULL, in this case, the string concerned is regarded as a null string.
    \param[in] str1 : string 1
    \param[in] str2 : string 2
     \return : negative, null (= 0) or positive value if the str1 is respectively lower, equal or higher than str2.
  */
  static int32_t StrCaseCmp(const char *str1, const char *str2);

  /** Remove all the blank characters in a string.
    Blank characters are identified by the function isspace (3C).
    \param str [in/out] : string to be modified
    \return a pointer to the string*/
  static char * RemoveAllSpaces(char *str);

  /** Remove all the blank characters in a string.
    Blank characters are identified by the function isspace (3C).
    \param[in] str : string to be modified
    \return the modified string */
  static string StringRemoveAllSpaces(const string& str);

  /** Replace all tokens of char c by char replaceBy  in a string.
    \param[in] str : string to be modified
    \param[in] c : char to replace
    \param[in] replaceBy : char replaced
    \return the modified string */
  static string StringReplace(const string& str, char c, char replaceBy);

  static bool StringCompare(const string& s1, const string& s2);

  /** Replace all tokens of string c by string replaceBy  in a string.
    \param[in] str : string to be modified
    \param[in] c : string to replace
    \param[in] replaceBy : string replaced
    \return the modified string */
  static string StringReplace(const string& str, const string& c, const string& replaceBy, bool compareNoCase = false);

  /** Remove all the blank characters at the  beginning and the end of a string.
    Blank characters are identified by the function isspace (3C).
    \param str [in/out] : string to be modified
    \return a trimmed string*/
  static string StringTrim(const string& str);


  /** Set a string in uppercase
    \param str [in/out] : string to be modified
    \return a pointer to the string*/
  static char * ToUpper(char *str);

  /** Set a string in lowercase
    \param str [in/out] : string to be modified
    \return a pointer to the string*/
  static char * ToLower(char *str);

  /** Set a character in uppercase
    \param[in] chr : character
    \return the uppercase character */
  static char ToUpper(const char chr);

  /** Set a string in lowercase
    \param[in] chr : character
    \return the lowercase character */
  static char ToLower(const char chr);

  /** Set a string object in lowercase
    \param str [in/out] : string to be modified
    \return a new string object in lowercase*/
  static string StringToLower(const string& str);

  /** Set a string object in uppercase
    \param[in] str : character
    \return a new string object in uppercase */
  static string StringToUpper(const string& str);

  /**Write formatted data to a string.
    WARNING : this method use vsnprintf if vsnprintf is defined,
    otherwise vsprintf is used and 'size' parameter is ignored
    \param[out] str : storage location for output.
    \param[in] size : maximum number of characters to store
    \param[in] format : format-control string
    \param[in] ... : optional arguments
    \return return value of the vsnprintf or vsprintf - see documentation of these functions*/
  static int32_t snprintf(char* str, size_t size, const char *format, ...)
      	__attribute__ ((format(printf, 3, 4)));

  /**Write formatted data to a string.
    WARNING : this method use vsnprintf if vsnprintf is defined,
    otherwise vsprintf is used and 'size' parameter is ignored
    \param[in] size : maximum number of characters to store
    \param[in] format : format-control string
    \param[in] ... : optional arguments
    \return formatted string*/
  static string Format(size_t size, const char *format, ...)
      	__attribute__ ((format(printf, 2, 3)));

  /**Write formatted data to a string.
    WARNING : this method use vsnprintf if vsnprintf is defined,
    otherwise vsprintf is used and 'size' parameter is ignored
    \param[in] format : format-control string
    \param[in] ... : optional arguments
    \return formatted string*/
  static string Format(const char *format, ...)
	__attribute__ ((format(printf, 1, 2)));

  /**Write formatted data to a string.
    WARNING : this method use vsnprintf if vsnprintf is defined,
    otherwise vsprintf is used and 'size' parameter is ignored
    \param[in] size : maximum number of characters to store
    \param[in] format : format-control string
    \param[in] args : optional arguments
    \return formatted string*/
  static string Format(size_t size, const char *format, va_list args);



//----------------------------------------

  /**Do incremental statistics.
    Incremental statistics are done to avoid memory consumption
    needed when we do 'classical' stats: an array of all the values
    involved with statistics must be kept before computing them.
    After first call to this the parameters must not be modified
    until end of statistics or result will be unpredictible.
    \param[in] NewValue : New value to take into account for statistics.
		Only valid values are kept; valid values are those
		different from default value (#IsDefaultValue#)
    \param[in/out] Count : number of valid data used for stats.
		Valid data is a number which is not a default value.
		On first call, this parameter must be 0 or a default
		value. And it is not modified since the first valid
		value.
    \param[in/out] Mean : Incremental mean
    \param[in/out] StdDev : Temporary value used to compute standard deviation
    \param[in/out] Min : Minimum value
    \param[in/out] Max : Maximum value
  */
  static void DoIncrementalStats
		(double NewValue,
		 double	&Count,
		 double &Mean,
		 double &StdDev,
		 double &Min,
		 double &Max);

//----------------------------------------

  /**Terminates incremental statistics.
    Computes the final value of standard deviation
    \param[in] Count : number of valid data used for stats.
		If count is 0 or default value, all other output
		parameters are set to default value.
    \param[in/out] Mean : Computed mean or default value (see Count)
    \param[in/out] StdDev : On output, actual value of standard deviation
    \param[in/out] Min : Computed min or default value (see Count)
    \param[in/out] Max : Computed max or default value (see Count)
    \param[in] DefaultValue : Default value wanted
		Value to put in output parameters if no stats can be done
		(no valid data: count is 0 or default value #m_defaultValueDOUBLE#).
  */
  static void FinalizeIncrementalStats
		(double	Count,
		 double &Mean,
		 double &StdDev,
		 double &Min,
		 double &Max,
		 double DefaultValue	= m_defaultValueDOUBLE);

//----------------------------------------

  /**Negates a number.
    Takes default values into account
    \param[in] X : Number involved
    \return Negated number*/
  static double UnaryMinus
		(double X);



//----------------------------------------

  /**Complement an integer.
    The number is taken as a signed integer (int32_t).
    Then a bitwize not is computed and the integer is
    converted back to a float.
    If the parameter is a default values or do not fall
    in integer range, a default value is returned.
    \param[in] X : Number involved
    \return Complemented number*/
  static double BitwiseNot
		(double X);

//----------------------------------------

  /**Negates a logical value (0 is false, other (except default value) is true.
    Takes default values into account
    \param[in] X : Number involved
    \return Negated value*/
  static double UnaryNot
		(double X);
//----------------------------------------

  /**Multiply two numbers.
    Takes default values into account
    \param[in] X : Number involved
    \param[in] Y : Number involved
    \return Result of operation*/
  static double Multiply
		(double X,
		 double Y);
//----------------------------------------

  /**Divide two numbers.
    Takes default values into account
    \param[in] X : Number involved
    \param[in] Y : Number involved
    \return Result of operation*/
  static double Divide
		(double X,
		 double Y);
//----------------------------------------

  /**Add two numbers.
    Takes default values into account
    \param[in] X : Number involved
    \param[in] Y : Number involved
    \return Result of operation*/
  static double Plus
		(double X,
		 double	Y);
//----------------------------------------

  /**Substracts one number from another.
    TAKES default values into account
    \param[in] X : Number involved
    \param[in] Y : Number involved
    \return Result of operation*/
  static double Minus(double X, double Y);
//----------------------------------------

  /**Do a logical and on two numbers.
    Takes default values into account
    \param[in] X : Number involved
    \param[in] Y : Number involved
    \return Result of operation*/
  static double And
		(double X,
		 double	Y);


//----------------------------------------

  /**Do a bitwise AND operation an integer.
    The numbers are taken as signed integers (int32_t).
    Then a bitwize AND is computed and the integer is
    converted back to a float.
    If the parameters are default values or do not fall
    in integer range, a default value is returned.
    \param[in] X : Number involved
    \param[in] Y : Number involved
    \return Result of operation*/
  static double BitwiseAnd
		(double X,
		 double	Y);

//----------------------------------------

  /**Do a logical or on two numbers.
    Takes default values into account
    \param[in] X : Number involved
    \param[in] Y : Number involved
    \return Result of operation*/
  static double Or
		(double X,
		 double	Y);


//----------------------------------------

  /**Do a bitwise OR operation an integer.
    The numbers are taken as signed integers (int32_t).
    Then a bitwize OR is computed and the integer is
    converted back to a float.
    If the parameters are default values or do not fall
    in integer range, a default value is returned.
    \param[in] X : Number involved
    \param[in] Y : Number involved
    \return Result of operation*/
  static double BitwiseOr
		(double X,
		 double	Y);

//----------------------------------------

  /**Find the sign of a number (1 if positive or null, -1 if negative).
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Sign
		(double	 X);
//----------------------------------------

  /**Do the sine of a number expressed in degrees.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double SinD
		(double X);
//----------------------------------------

  /**Do the cosine of a number expressed in degrees.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double CosD
		(double X);
//----------------------------------------

  /**Do the arc cosine of a number expressed in degrees.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double ACosD (double X);
//----------------------------------------

  /**Do the tangent of a number expressed in degrees.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double TanD
		(double X);
//----------------------------------------

  /**Find the minimum value of two numbers.
    Takes default values into account
    \param[in] X1 : Number involved
    \param[in] X2 : Number involved
    \return Result of operation*/
  static double Min
		(double X1,
		 double X2);
//----------------------------------------

  /**Find the maximum value of two numbers.
    Takes default values into account
    \param[in] X1 : Number involved
    \param[in] X2 : Number involved
    \return Result of operation*/
  static double Max
		(double X1,
		 double X2);
//----------------------------------------

  /**Indicates if a number is comprised between two others.
    Takes default values into account
    \param[in] Min : Lower bound
    \param[in] X : Number involved
    \param[in] Max : Upper bound
    \return Result of operation: 0 if not Min <= X <= Max.*/
  static double IsBounded
		(double Min,
		 double X,
		 double Max);
//----------------------------------------

  /**Indicates if a number is comprised between two others.
    Takes default values into account
    \param[in] Min : Lower bound
    \param[in] X : Number involved
    \param[in] Max : Upper bound
    \return Result of operation: 0 if not Min < X < Max.*/
  static double IsBoundedStrict
		(double Min,
		 double X,
		 double Max);
//----------------------------------------

  /**Find the absolute value of a number.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Abs
		(double X);
//----------------------------------------

  /**Find the square value of a number.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Sqr
		(double X);
//----------------------------------------

  /**Find the square root value of a number.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Sqrt
		(double X);
//----------------------------------------

  /**Find the natural logarithm of a number.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Log
		(double X);
//----------------------------------------

  /**Find the decimal logarithm of a number.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Log10
		(double X);
//----------------------------------------

  /**Find the power of a number by another.
    Takes default values into account
    \param[in] X : Number involved
    \param[in] Y : Power. Can be a integral or decimal
    \return Result of operation*/
  static double Pow
		(double X,
		 double Y);
//----------------------------------------

  /**Find exponential of a number.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Exp
		(double X);
//----------------------------------------

  /**Find the integral value part below of a number.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Floor
		(double X);
//----------------------------------------

  /**Find the integral value part over of a number.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Ceil
		(double X);
//----------------------------------------

  /**Find the modulus of a number divided by another.
    Takes default values into account
    \param[in] X : Number involved
    \param[in] Y : Divider
    \return Result of operation*/
  static double Mod
		(double X,
		 double Y);

  static bool Xor(bool p, bool q);

//----------------------------------------

  /**Convert degrees to radians.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Deg2Rad
		(double X);
//----------------------------------------

  /**Convert radians to degrees.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Rad2Deg
		(double X);
//----------------------------------------

  /**Do the sine of a number expressed in radians.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Sin
		(double X);
//----------------------------------------

  /**Do the cosine of a number expressed in radians.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Cos
		(double X);
//----------------------------------------
  /**Do the arc cosine of a number expressed in radians.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double ACos(double X);
//----------------------------------------

  /**Do the tangent of a number expressed in radians.
    Takes default values into account
    \param[in] X : Number involved
    \return Result of operation*/
  static double Tan
		(double X);
//----------------------------------------

  /**Find a number satisfying the condition Floor <= Longitude < Floor+360.
    Takes default values into account
    \param[in] Floor : Base longitude
    \param[in] Longitude : Longitude to normalize
    \return Result of operation*/
  static double NormalizeLongitude
		(double Floor,
		 double Longitude);

//----------------------------------------
  static double Sinc(double x);
//----------------------------------------
  static bool IsLongitudeCircular(double	min, double	max, double compareEpsilon = CTools::m_CompareEpsilon);

//----------------------------------------

  /**Checks a default value.
    \param[in] X : Number involved
    \return 0.0 if X is not a default value, 1.0 otherwize*/
  static double IsDefaultFloat
		(double X);
//----------------------------------------
  /**Indicates if a number is infinite.
    \param[in] X : Number involved
    \return 0 if X in finite 1 if infinite*/
  static int IsInf
		(double X);
//----------------------------------------
  /**Indicates if a value is a valid number.
    \param[in] X : Number involved
    \return 0 if X is valid, 1 if X is not a number*/
  static int IsNan
		(double X);
//----------------------------------------

  /**Removes trailing zeroes from a number: 2.30000 is transformed into 2.3.
    \param[in] Text : String
    \param[in] dotTrim : if true, remove dot at the end : 2.000 --> 2, if false, leave dot : 2.000 --> 2.
    \return Returns modifed string*/
  static string TrailingZeroesTrim(const string	&Text, bool dotTrim = true);

//----------------------------------------

  /**Indicates if a file exists
    \param[in] Name : File name
    \return Returns true if file exists and is readable*/
  static bool FileExists
		(const string	&Name);

//----------------------------------------
  /**Indicates if a directory exists
    \param[in] Name : Directory name
    \return Returns true if directory exists*/
  static bool DirectoryExists
		(const string	&Name);

//----------------------------------------
  /**Explicitly set the Data Directory based on a relative path to the current executable.
     The Data Directory will be set to '../data' relative to the location of the executable.
    \param[in] argv0 : pass argv[0] that you got from main(int argc, char *argv[]).*/
  static void SetDataDirForExecutable
		(const char *argv0);

//----------------------------------------

  /**Explicitly set the Data Directory.
    \param[in] DataDir : Full path to data directory.*/
  static void SetDataDir
		(const string &DataDir);

//----------------------------------------

  /**Returns the constant data directory defined at compilation time,
     by environment variable, or set by application.
    \return Returns the path of found file or an empty string if not found*/
  static string GetDataDir
		();

//----------------------------------------

  /**Finds a file path known only by its name. The path is retreived
     from compilation (intallation prefix) or by environment variable.
    \param[in] Name : File name
    \return Returns the path of found file or an empty string if not found*/
  static string FindDataFile
		(const string	&Name);

  /**Finds a file location known only by its name using the give path.
     The path should be similar to what can be used for the PATH environment
     variable on the current system.
    \param[in] filename : File name
    \param[in] path : Search path
    \return Returns the full path to the file or an empty string if not found*/
  static string FindFileInPath
    (const string &filename, const string &path);

  /** This encodes characters that are not printable or can be encode with
      one of the C/C++ standard escape sequences. The 'exclude' list is a
      list of chars to exclude from the encoding process. Since the '\\0' is
      used to determine the end of the string and will not be encoded.
   \param[in] str  : The string to encode.
   \param[in] exclude : A list of charters to exclude from encoding.
   \param[in] literals :A list of printable characters to be included in the
     encodeing.
   \param hexadecimal If true, non-standard, non-printable charecters will
     be encoded in hexadecimal. If false they will be encoded in octal
     format.
   \return the newly encoded string.*/
  static string SlashesEncode (const string& str, const string& exclude = "", const string& literals = "", bool hexadecimal = true);

  /** Takes a string with escaped charters including decimal and
      hexadecimal escapes and decodes them to the literal charter. This
      function supports only standard C/C++ escaped literals.
   \param[in] str : The string to decode.
   \param[in] exclude : A list of charters to exclude from decoding.
   \param[in] decodeliterals : Set if non standard escaped literals are to be
     deocded.
   \return the newly encoded string.*/
  static string SlashesDecode (const string& str, const string& exclude = "", bool decodeliterals = true);

  /** Removes characters c1 and c2, if these characters surround an number (integer or decimal).
    For example:
      RemoveCharSurroundingNumber("ABCD (125)", '(', ')') will return "ABCD 125"
      RemoveCharSurroundingNumber("ABCD (+125.63)", '(', ')') will return "ABCD +125.63"
      RemoveCharSurroundingNumber("ABCD (-45) (XYZ*2)", '(', ')') will return "ABCD -45 (XYZ*2)"
      RemoveCharSurroundingNumber("(ABCD ((-45)))", '(', ')') will return "(ABCD (-45))"
   \param[in] str : The string to modify
   \param[in] c1 : the first surrounding char
   \param[in] c2 : the last surrounding char
   \return the newly modified string.*/
  static string RemoveCharSurroundingNumber (const string& str, const char c1 = '(', const char c2 = ')');

  /** Expands shell variables (i.e. ${HOME}).
    If the '$' character is preceded by '\', it's taken into account as a common character.and 
    not as a shell variable identifier.
    Shell variables beginning by '+' are expanded in uppercase.
    Shell variables beginning by '-' are expanded in lowercase.
   \param[in] value : The string to expand
   \return the newly expanded string.*/
  static string ExpandShellVar(const string& value);

  /** Expand variables (i.e. %{VAR}).
    If the '%' character is preceded by '\', it's taken into account as a common
    character and not as a variable identifier.
    Variables begining by '+' are expanded in uppercase.
    Variables begining by '-' are expanded in lowercase.
   \param[in] value : The string to expand
   \param[in] VarValues : The values of the variables. If NULL, the environment
			variables are taken.
   \param[in] Begining : Char identifying the begining of a var reference
   \param[in] Recurse : If true, variable expanded can contain references to
		other variables which are then expanded.
   \return the newly expanded string.*/
  static string ExpandVariables	(const string	&valueIn, const map<string, string> *varValues,
                                 bool recurse = false, char beginning = '%', uint32_t* numberVarsExpanded = NULL, bool withExcept = false, string* errorMsg = NULL);

  static string ExpandVariables	(const string	&valueIn, const map<string, string> *varValues, const map<string, string>	*fieldAliases,
                                 bool recurse = false, char beginning = '%', uint32_t* numberVarsExpanded = NULL, bool withExcept = false, string* errorMsg = NULL);

  static void StringToAlias(const string& in, string& out, const char beginning	/*= '%'*/);

  /** Cleans a path variable
    - change path separator in a suitable path separator ('\' or '/' depending on the system)
    - skip trailing "../..", if any
    - remove back references: translate dir1/../dir2 to dir2
   \param[in] path : The string to clean
   \return the newly cleaned string.*/
  static string MakeCorrectPath(const string& path);

  /** Creates an absolute or full path name for the specified relative path name.
    - change path separator in a suitable path separator ('\' or '/' depending on the system)
    - skip trailing "../..", if any
    - remove back references: translate dir1/../dir2 to dir2
   \param[in] partialPath : the relative path
   \return the absolute path name, or  empty string if there is an error 
      (for example, if the value passed in relPath includes a drive letter that is not valid or cannot be found,
       or if the length of the created absolute path name  is greater than the BRATHL_PATH_MAX defined in brathl.h) */
  static string AbsolutePath(const string& partialPath);

  /** Gets a file name extension.
   \param[in] filename : file name 
   \return the extension, or empty string if none */
  static string FileExtension(const string& fileName);

  /** Gets a directory name from a string
   \param[in] path : full path
   \return the directory name, or '.' if path has only one component */
  static string DirName(const string& fileName);

  /** Gets a base file name from a string
   \param[in] path : full path
   \return the base file name (no extension), or empty string, or :
      '.' returns '.',
      './' returns '.',
      '/' returns '/',
      '..' returns '..',
      '../' returns '..'
      'abc/def/' returns 'def'
  */
  static string BaseName(const string& fileName);

#if HAVE_SPLITPATH
  /** Breaks a path name into components
   \param[in] path : full path
   \param[out] drive : Optional drive letter, followed by a colon (:)
   \param[out] dir : Optional directory path, including trailing path separator
   \param[out] fname : Optional Base filename (no extension) 
   \param[out] ext : Optional filename extension, including leading period (.) 
   */
  static void SplitPath(const string& path, 
                        string* drive = NULL, string* dir = NULL, string* fname = NULL, string* ext= NULL);
#endif


// Swap values for endiannes compliance
  static void SwapValue(int32_t &value);
  static void SwapValue(int16_t &value);
  static void SwapValue(float &value);
  static void SwapValue(double &value);

   /**
   * Converts and normalize a latitude string representation (eg 60 N, 75.56 W, 60, -75.56) Normalize +/-90.
   * 
   * @param value latitude string representation
   * 
   */
  static double UnconvertLat(const string& value);

   /**
     * Converts and eventually normalize a longitude string representation (eg 60 E, 120.23 W, 60, -120.23)
     * Normalize +/-180.
     * 
     * @param normalize set to true to normalize longitude value
     * @param value longitude string representation
     * 
     * @return converted longitude.
     */
  static double UnconvertLon(const string& value, bool normalize = true);

  /** Convert an string to double
   \param[in] value : string to be converted
   \return coanverted value or CTool::m_defaultValueDOUBLE if no possible conversion.
   */
  static double StrToDouble(const string& value);

  /** Convert an string to int
   \param[in] value : string to be converted
   \return coanverted value or CTool::m_defaultValueINT if no possible conversion.
   */
  static int32_t StrToInt(const string &s);

  /** Convert an int to string
   \param[in] value : int to be converted
   \return coanverted value or empty string if no possible conversion.
   */
  static string IntToStr(int32_t i);

  static int64_t StrToLong(const string &s);
  static string LongToStr(int64_t i);

  /** Convert an double to string
   \param[in] value : double to be converted
   \return coanverted value or empty string if no possible conversion.
   */
  static string DoubleToStr(double d, int32_t precision = 10);

  static float StrToFloat(const string& value);
  static string FloatToStr(float f, int32_t precision = 10);

  /* 
  * Translate the array indexes pair into a seek index
  */
  static uint32_t VectorOffset(uint32_t* shape, uint32_t ndims, const uint32_t *coord);

/*
 *  find the largest contiguous block from within 'edges'.
 *  returns the index to the left of this (which may be -1).
 *  Compute the number of contiguous elements and return
 *  that in *iocountp.
 */
  //static int32_t VectorContiguousBlock(size_t ndims, const size_t *const shape, 	const size_t *const edges,	size_t *const iocountp);
  static int32_t VectorContiguousBlock(uint32_t ndims, const uint32_t *const shape, const uint32_t *const edges, uint32_t *const countContinousBlock);

//  static int32_t VectorContiguousBlock(const CUIntArray& shape, const CUIntArray& edges,	uint32_t *const countContinousBlock);
  //static void ExtractVector(const double* vectorIn, uint32_t* dims, uint32_t nDims, uint32_t *start, uint32_t *edges, double*& vectorOut);
  static void ExtractVector(const double* vectorIn, uint32_t* shape, uint32_t nDims, uint32_t *start, uint32_t *edges, double* vectorOut);

  static uint32_t GetProductValues(uint32_t* shape, uint32_t nbDims);

  static string Replace(const string& inText, const string& regexpPattern, const string replaceString);

  static string ReplaceString(const string& inText, const vector<string>& findString, const vector<string>& replaceWords);
  static string ReplaceWord(const string& inText, const vector<string>& findWords, const vector<string>& replaceWords);
  static string ReplaceWord(const string& inText, const string& findWords, const string& replaceWords);

  static void Find(const string& inText, const string& regexpPattern, vector<string>& stringFound);

  static void FindWord(const string& inText, vector<string>& wordsFound);

  static void FindAliases(const string& inText, vector<string>& aliasesFound,                                 
                         bool onlyName = false,                                 
                         const string& begining	= "%",
                         bool recurse = false, 
                         const map<string, string>* varValues = NULL,
                         const map<string, string>* fieldAliases = NULL,
                         bool withExcept = false, string* errorMsg = NULL);

  static void DeleteObject(CBratObject* ob);

  static bool LoadAndCheckUdUnitsSystem(string& errorMsg);

  // get all characters before the first occurence of ch
  // (returns the whole string if ch not found)
  static string BeforeFirst(const string& str, const char ch);

  static string ToString(const char *s, size_t len = string::npos);

  static void ReplaceAliases(const string& in, string& out, vector<string>* aliases = NULL);
  static void ReplaceAliases(const string& in, const string& replacedBy, string& out, vector<string>* aliases = NULL);

  // The following code returns the distance between to locations based on each point's longitude and latitude. 
  // The distance returned is relative to Earth's radius. 
  // To get the distance in miles, multiply by 3960. 
  // To get the distance in kilometers, multiply by 6373.
  //
  // Latitude is measured in degrees north of the equator; 
  // southern locations have negative latitude. 
  // Similarly, longitude is measured in degrees east of the Prime Meridian.
  // A location 10° west of the Prime Meridian, for example, could be expressed as either 350° east or as -10° east.
  // WARNING: The function above assumes the earth is perfectly spherical.
  // (from http://www.johndcook.com/python_longitude_latitude.html)

  static double DistanceOnUnitSphere(double lat1, double long1, double lat2, double long2);

  static double DistanceKmOnUnitSphere(double lat1, double long1, double lat2, double long2);

  static inline bool IsOdd(uint32_t value) { return IsOdd(static_cast<int32_t>(value)); };
  static inline bool IsOdd(int32_t value) { return ((value % 2) != 0); };

  static inline bool IsEven(uint32_t value) { return !CTools::IsOdd(value); };
  static inline bool IsEven(int32_t value) { return !CTools::IsOdd(value); };

  //static double ExecZ(const char* function, const char* fmt, ...);
//  static void VaListToTypeUnion(const char* fmt, va_list args, CVectorTypeUnion& typeUnions);
  //static double Exec(const char* function, const char *fmt, const type_union *arg);
  //static void TypeUnionToArray(const type_union *arg, double* aligned, uint32_t alignedLen);

  //static type_union i2u(int32_t i) { TYPE_UNION_SET(T_INT, i) };
  //static type_union c2u(uint8_t c) { TYPE_UNION_SET(T_CHAR, c) };
  //static type_union f2u(float f) { TYPE_UNION_SET(T_FLOAT, f) };
  //static type_union d2u(double d) { TYPE_UNION_SET(T_DOUBLE, d) };
  //static type_union s2u(const char * s) { TYPE_UNION_SET(T_STRING, s) };
  //static type_union d_ptr2u(double* d_ptr) { TYPE_UNION_SET(T_DOUBLE_PTR, d_ptr) };




private:

  static void SetUpper(uint32_t *upp, /* modified on return */
	                const uint32_t *stp,
	                const uint32_t *edp,
	                const uint32_t *const end);

  static void Odo1(const uint32_t *const start, const uint32_t *const upper,
	  uint32_t *const coord, /* modified on return */
	  const uint32_t *upp,
	  uint32_t *cdp);

public:

  /// default values for int 8 bits
  static const int8_t m_defaultValueINT8;

  /// default values for unsigned int 8 bits
  static const uint8_t m_defaultValueUINT8;

  /// default values for int 16 bits
  static const int16_t m_defaultValueINT16;

  /// default values for unsigned int 16 bits
  static const uint16_t m_defaultValueUINT16;

  /// default values for chars
  static const char m_defaultValueCHAR;

  /// default values for int 32 bits
  static const int32_t m_defaultValueINT32;

  /// default values for unsigned int 32 bits
  static const uint32_t m_defaultValueUINT32;

  /// default values for unsigned int 64 bits
  static const int64_t m_defaultValueINT64;

  /// default values for unsigned int 64 bits
  static const uint64_t m_defaultValueUINT64;

  /// default values for double
  static const double m_defaultValueDOUBLE;

  /// default values for float
  static const float m_defaultValueFLOAT;

  // maximum difference used to declare that two real values are equal
  static const double m_CompareEpsilon;

  static const double m_deltaLatitudeMercator;

  //static const string CTools::m_warningHeader;

private:
  static string	m_DataDir;

};

/** @} */

}

#endif // !defined(_Tools.h_)

