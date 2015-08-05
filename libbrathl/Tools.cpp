/*
* 
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

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <sys/stat.h>

#include "brathl.h"
#include "coda.h"

#include "Exception.h"
#include "Trace.h"
#include "Tools.h"
#include "LatLonPoint.h"
#include "deelx.h"
using namespace brathl;

namespace brathl
{

#if defined(min)
#undef min
#endif

#define MIN(X,Y) ((X) <= (Y) ? (X) : (Y))
//TYPE_FUN(i2u, T_INT, int, i)
//TYPE_FUN(c2u, T_CHAR, char, c)
//TYPE_FUN(f2u, T_FLOAT, float, f)
//TYPE_FUN(d2u, T_DOUBLE, double, d)
//TYPE_FUN(s2u, T_STRING, const char *, s)

// default values for int 8 bits
const int8_t CTools::m_defaultValueINT8 = 0x7F;

// default values for unsigned int 8 bits
const uint8_t CTools::m_defaultValueUINT8 = 0xFFU;

// default values for int 16 bits
const int16_t CTools::m_defaultValueINT16 = 0x7FFF;

// default values for unsigned int 16 bits
const uint16_t CTools::m_defaultValueUINT16 = 0xFFFFU;

// default values for char
const char CTools::m_defaultValueCHAR = '\0';

// default values for int 32 bits
const int32_t CTools::m_defaultValueINT32 = 0x7FFFFFFF;

// default values for unsigned int 32 bits
const uint32_t CTools::m_defaultValueUINT32 = 0xFFFFFFFFU;

// default values for int 64 bits
#ifdef WIN32
const int64_t CTools::m_defaultValueINT64 = 0x7FFFFFFFFFFFFFFF;
#else
const int64_t CTools::m_defaultValueINT64 = 0x7FFFFFFFFFFFFFFFLL;
#endif

// default values for unsigned int 64 bits
#ifdef WIN32
const uint64_t CTools::m_defaultValueUINT64 = 0xFFFFFFFFFFFFFFFFU;
#else
const uint64_t CTools::m_defaultValueUINT64 = 0xFFFFFFFFFFFFFFFFULL;
#endif

const char *CTools::m_defaultValueString = "";

// default values for double
const double CTools::m_defaultValueDOUBLE = 18446744073709551616.0;

// default values for float
const float CTools::m_defaultValueFLOAT = 18446744073709551616.0F;

// maximum difference used to declare that two real values are equal
const double CTools::m_CompareEpsilon = 1.0E-70;

const double CTools::m_deltaLatitudeMercator = 1.0E-7;

string CTools::m_DataDir = "";

//const string CTools::m_warningHeader = ">>>>>>>>>>>>>>>>>>>> WARNING >>>>>>>>>>>>>>>>>>>>>>>>>";
/*
** precision limit for computation of normalized longitude
*/
//-------------------------------------------------------------
//------------------- CTools class --------------------
//-------------------------------------------------------------


CTools::CTools()
{
}

//----------------------------------------

CTools::~CTools()
{
}

//----------------------------------------

bool CTools::IsEmpty(const char *pstrString)
{
  return (pstrString == NULL) || (*pstrString == '\0');
}

//----------------------------------------

double CTools::Int(double value)
{
  double	integral;

/*
** Not necessary since m_defaultValueDOUBLE is an integer
** and in many case it increase the speed

  if (value == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;
*/

  modf(value, &integral);
  return  integral;
}

//----------------------------------------

double CTools::Frac(double value)
{
  double	integral;

  if (value == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;

    double frac =  modf(value, &integral);
  return  frac;
}


//----------------------------------------

double CTools::Round(double value)
{
  double	integral;
  double	frac;


/*
** Not necessary since m_defaultValueDOUBLE is an integer
** and in many case it increase the speed

  if (value == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;
*/

/*
** round(X) function is not available everywhere
  return round(X);
*/

  frac = modf(value, &integral);
  if (fabs(frac) >= 0.5)
  {
    return (integral + (frac < 0.0 ? -1.0 : 1.0));
  }

  return  integral;
}

//----------------------------------------------------
double CTools::Rnd ( double value , double precision )
{
  if (precision < 0.0)
  {
    precision = 0.0;
  }
  if (precision > 20.0)
  {
    precision = 20.0;
  }

  double tmp = CTools::Pow(10, precision);

  double valueTmp = CTools::Multiply(value, tmp);
  value = CTools::Divide(CTools::Round(valueTmp), tmp);
  return value;
}

//----------------------------------------

void CTools::SetDefaultValue(float& value)
{
  value = m_defaultValueFLOAT;
}

//----------------------------------------

void CTools::SetDefaultValue(double& value)
{
  value = m_defaultValueDOUBLE;
}

//----------------------------------------

void CTools::SetDefaultValue(int8_t& value)
{
  value = CTools::m_defaultValueINT8;
}

//----------------------------------------

void CTools::SetDefaultValue(uint8_t& value)
{
  value = m_defaultValueUINT8;
}

//----------------------------------------

void CTools::SetDefaultValue(int16_t& value)
{
  value = CTools::m_defaultValueINT16;
}

//----------------------------------------

void CTools::SetDefaultValue(uint16_t& value)
{
  value = m_defaultValueUINT16;
}

//----------------------------------------

void CTools::SetDefaultValue(int32_t& value)
{
  value = CTools::m_defaultValueINT32;
}

//----------------------------------------

void CTools::SetDefaultValue(uint32_t& value)
{
  value = m_defaultValueUINT32;
}


//----------------------------------------

void CTools::SetDefaultValue(int64_t& value)
{
  value = CTools::m_defaultValueINT64;
}

//----------------------------------------

void CTools::SetDefaultValue(uint64_t& value)
{
  value = m_defaultValueUINT64;
}

//----------------------------------------

bool CTools::IsDefaultValue(const double value)
{
  return value == m_defaultValueDOUBLE;
}

//----------------------------------------

bool CTools::IsDefaultValue(const float value)
{
  return value == m_defaultValueFLOAT;
}

//----------------------------------------

bool CTools::IsDefaultValue(const int8_t value)
{
  return value == m_defaultValueINT8;
}
//----------------------------------------

bool CTools::IsDefaultValue(const uint8_t value)
{
  return value == m_defaultValueUINT8;
}

//----------------------------------------

bool CTools::IsDefaultValue(const int16_t value)
{
  return value == m_defaultValueINT16;
}
//----------------------------------------

bool CTools::IsDefaultValue(const uint16_t value)
{
  return value == m_defaultValueUINT16;
}

//----------------------------------------

bool CTools::IsDefaultValue(const int32_t value)
{
  return value == CTools::m_defaultValueINT32;
}

//----------------------------------------

bool CTools::IsDefaultValue(const uint32_t value)
{
  return value == m_defaultValueUINT32;

}

//----------------------------------------

bool CTools::IsDefaultValue(const int64_t value)
{
  return value == CTools::m_defaultValueINT64;
}

//----------------------------------------

bool CTools::IsDefaultValue(const uint64_t value)
{
  return value == m_defaultValueUINT64;

}

//----------------------------------------

bool CTools::CastValue(int32_t &Dest, const double Source)
{
  static const double max	= 4294967296.0;		// 2^32 pour les entiers non signes
  static const double min	= -2147483648.0;	// -2^31 pour les entiers signes
  double Src	= Round(Source);
  if (IsDefaultValue(Src) ||
      (Src < min) ||
      (Src > max))
  {
    SetDefaultValue(Dest);
    return false;
  }
  Dest	= static_cast<int32_t>(Src);
  return true;
}


//----------------------------------------

bool CTools::IsZero
		(double X)
{
  return fabs(X) < m_CompareEpsilon;
}

//----------------------------------------
bool CTools::AreEqual
		(double X, double Y)
{
  return CTools::AreEqual(X, Y, m_CompareEpsilon);
}
//----------------------------------------
bool CTools::AreEqual
		(double X, double Y, double compareEpsilon)
{
  if (X == m_defaultValueDOUBLE)
  {
    return Y == m_defaultValueDOUBLE;
  }
  if (Y == m_defaultValueDOUBLE)
  {
    return false;
  }

  return fabs(X-Y) < compareEpsilon;
}
//----------------------------------------

bool CTools::AreValidMercatorLatitude
		(double lat)
{
  return ((lat >= 0) ?  lat <= 90.0 - m_deltaLatitudeMercator : lat >= -90.0 + m_deltaLatitudeMercator);
}

//----------------------------------------
int32_t CTools::FindNoCase
		(const string& src,
		 const string& findWhat, uint32_t pos /* = 0 */)
{
  string szSrc = CTools::StringToLower(src);
  string szFindWhat = CTools::StringToLower(findWhat);
 ;
  return szSrc.find(szFindWhat, pos );

}
//----------------------------------------
int32_t CTools::FindNoCase
		(const char* src,
		 const char* findWhat, uint32_t pos /* = 0 */)
{
  string szSrc = src;
  string szFindWhat = findWhat;

  return FindNoCase(szSrc, szFindWhat, pos);
}
//----------------------------------------
int32_t CTools::RFindNoCase
		(const string& src,
		 const string& findWhat, uint32_t pos /* = 0 */)
{
  string szSrc = CTools::StringToLower(src);
  string szFindWhat = CTools::StringToLower(findWhat);
 ;
  return szSrc.rfind(szFindWhat, pos );

}
//----------------------------------------
int32_t CTools::RFindNoCase
		(const char* src,
		 const char* findWhat, uint32_t pos /* = 0 */)
{
  string szSrc = src;
  string szFindWhat = findWhat;

  return RFindNoCase(szSrc, szFindWhat, pos);
}
//----------------------------------------
bool CTools::CompareNoCase
		(const string& str1,
		 const string& str2)
{
  return CTools::CompareNoCase(str1.c_str(), str2.c_str());

}
//----------------------------------------
bool CTools::CompareNoCase
		(const char* str1,
		 const char* str2)
{
  string sz1 = CTools::StringToLower(str1);
  string sz2 = CTools::StringToLower(str2);

  return (CTools::StrCaseCmp(sz1.c_str(), sz2.c_str()) == 0);
}

//----------------------------------------
bool CTools::Compare
		(const char* str1,
		 const char* str2)
{
  return (CTools::StrCaseCmp(str1, str2) == 0);
}

//----------------------------------------

int CTools::Compare(double X, double Y, double compareEpsilon /*= CTools::m_CompareEpsilon*/)
{
  if (CTools::IsDefaultValue(X))
  {
    if (CTools::IsDefaultValue(Y))
    {
      return 0;
    }

    return -1;
  }

  if (CTools::IsDefaultValue(Y))
  {
    return 1;
  }

  if (fabs(X-Y) < compareEpsilon)
  {
    return 0;
  }

  if (X < Y)
  {
    return -1;
  }

  return 1;
}

//----------------------------------------

char * CTools::Trim(char *str)
{
  if (str == NULL)
  {
    return str;
  }

  uint32_t len = static_cast<uint32_t>(strlen(str));

  if (len <= 0)
  {
    return str;
  }

  string strTrimmed = CTools::StringTrim(str);

  memset(str, '\0', len);

  if (strTrimmed.size() <= 0)
  {
    return str;
  }

  memcpy(str, strTrimmed.c_str(), strTrimmed.size());
  
  return str;

}

//----------------------------------------
string CTools::StringReplace(const string& str, char c, char replaceBy )
{
  string strRet = str;
  for (uint32_t i = 0 ; i < str.length() ; i++)
  {
    if (strRet[i] == c)
    {
      strRet[i] = replaceBy;
    }
  }
  return strRet;

}
//----------------------------------------
bool CTools::StringCompare(const string& s1, const string& s2)
{
  return (s1.compare(s2) < 0); 
}

//----------------------------------------
string CTools::StringReplace(const string& str, const string& c, const string& replaceBy, bool compareNoCase /* = false */ )
{

  string strRet = str;
  string subStr;
  int32_t pos = -1;

  if (compareNoCase)
  {
    pos = CTools::FindNoCase(str, c);   
  }
  else
  {
    pos = strRet.find(c);
  }


  while (pos >= 0)
  {
    strRet.replace(pos, c.length(), replaceBy);
    if (compareNoCase)
    {
      pos = CTools::FindNoCase(str, c, pos + replaceBy.length());   
    }
    else
    {
      pos = strRet.find(c, pos + replaceBy.length());
    }

  }

  return strRet;

}


//----------------------------------------
string CTools::StringRemoveAllSpaces(const string& str)
{
  char* strTmp = new char[str.length() + 1];
  memset(strTmp, '\0', str.length() + 1);

  strcpy(strTmp, str.c_str());

  CTools::RemoveAllSpaces(strTmp);

  string strRet = strTmp;
  delete []strTmp;

  return strRet;

}

//----------------------------------------
string CTools::StringTrim(const string& str)
{
  string strRet;
// Trim Both leading and trailing spaces  
  string::size_type startpos = str.find_first_not_of(" \t"); // Find the first character position after excluding leading blank spaces  
  string::size_type endpos = str.find_last_not_of(" \t"); // Find the first character position from reverse 
  
  // if all spaces or empty return an empty string  
  if(( string::npos == startpos ) || ( string::npos == endpos))  
  {  
    strRet = "";  
  }  
  else  
  {
    strRet = str.substr( startpos, endpos - startpos + 1 );  
  }

  return strRet;

}

//----------------------------------------

char * CTools::RemoveAllSpaces(char *str)
{
 char  *P1, *P2;
 if (str == NULL)
 {
  return str;
 }


  P1		= str;
  P2		= str;

  /* Assertion: This algorithm ensure that the '\0' is also copied */
  do
  {
    if (! isspace(*P1))
    { /* Copy current character and go to next in P2 */
      *(P2++) = *P1;
    }
  } while (*(P1++) != '\0');

  return str;
}

//----------------------------------------

int32_t CTools::StrCaseCmp
		(const char *str1,
		 const char *str2)
{
  int32_t result = 0;	/* 0= Strings are equals */

  if (CTools::IsEmpty(str1))
  {
    if (!CTools::IsEmpty(str2))
      result	= -1;	/* First string lower lan non empty string */
  }
  else
  {
    if (CTools::IsEmpty(str2))
    {
        result	= 1;	/* First String greater than empty string */
    }
    else
    {
#if HAVE_STRCASECMP
      result = strcasecmp(str1, str2);
#else
  #if HAVE_USCORE_STRICMP
      result = _stricmp(str1, str2);
  #else
    #error "ERROR Compiling - There no function to perform a lowercase comparison of strings."
  #endif
#endif
    }
  }

  return result;
}

//----------------------------------------

char * CTools::ToUpper(char *str)
{
  if (CTools::IsEmpty(str))
  {
    return str;
  }

  char	*current;

  for(current = str ; *current ; current++)
  {
    *current = toupper(*current);
  }
  return str;
}

//----------------------------------------

char * CTools::ToLower(char *str)
{
  if (CTools::IsEmpty(str))
  {
    return str;
  }

  char	*current;

  for(current = str ; *current ; current++)
  {
    *current = tolower(*current);
  }
  return str;
}

//----------------------------------------

string CTools::StringToLower(const string& str)
{
  char* strTmp = new char[str.length() + 1];
  memset(strTmp, '\0', str.length() + 1);

  strcpy(strTmp, str.c_str());

  CTools::ToLower(strTmp);

  string strRet = strTmp;
  delete []strTmp;

  return strRet;

}

//----------------------------------------

string CTools::StringToUpper(const string& str)
{
  char* strTmp = new char[str.length() + 1];
  memset(strTmp, '\0', str.length() + 1);

  strcpy(strTmp, str.c_str());

  CTools::ToUpper(strTmp);

  string strRet = strTmp;
  delete []strTmp;

  return strRet;

}


//----------------------------------------

char CTools::ToUpper(const char chr)
{
  return toupper(chr);
}

//----------------------------------------

char CTools::ToLower(const char chr)
{
  return tolower(chr);
}

//----------------------------------------
int32_t CTools::snprintf(char* str, size_t size, const char *format, ...)
{
  int32_t result = -1;
  va_list args;
  va_start( args, format );

#if HAVE_VSNPRINTF
  result = vsnprintf(str, size, format, args );
#else
  #if HAVE_USCORE_VSNPRINTF
    result = _vsnprintf(str, size, format, args );
  #else
    result = vsprintf(str, format, args );
  #endif
#endif

  va_end( args );

  return result;
}


//----------------------------------------

string CTools::Format(size_t size, const char *format, ...)
{
  string result;
  va_list args;

  va_start( args, format );
  result  = Format(size, format, args);
  va_end(args);
  return result;
}


//----------------------------------------

string CTools::Format(const char *format, ...)
{
  string result;
  va_list args;

  va_start( args, format );
  result  = Format(4096, format, args);
  va_end(args);
  return result;
}


//----------------------------------------

string CTools::Format(size_t size, const char *format, va_list args)
{
  char		*str = new char[size];
  string	result;

  try
  {
#if HAVE_VSNPRINTF
  vsnprintf(str, size, format, args );
#else
  #if HAVE_USCORE_VSNPRINTF
    _vsnprintf(str, size, format, args );
  #else
    vsprintf(str, format, args );
  #endif
#endif
    result	= str;
  }
  catch (...)
  {
    delete []str;
    str = NULL;
    throw;
  }

  delete []str;
  str = NULL;

  return result;
}



//----------------------------------------

void CTools::DoIncrementalStats
		(double newValue,
		 double	&count,
		 double &mean,
		 double &stdDev,
		 double &min,
		 double &max)
{
  if (IsDefaultValue(newValue))
    return;

  if (IsDefaultValue(count) || (count == 0.0))
  {
    count	= 1.0;
    mean	= newValue;
    stdDev	= 0.0;
    min		= newValue;
    max		= newValue;
    return;
  }

  double OldCount	= count;
  count	+= 1.0;
  double X1	= newValue - mean;
  double X2	= X1/count;
  mean		+= X2;
  stdDev	+= X2 * OldCount * X1;
  if (newValue < min)
  {
    min	= newValue;
  }
  if (newValue > max)
  {
    max	= newValue;
  }
}

//----------------------------------------

void CTools::FinalizeIncrementalStats
		(double	count,
		 double &mean,
		 double &stdDev,
		 double &min,
		 double &max,
		 double defaultValue	/*= m_defaultValueDOUBLE*/)
{
  if (IsDefaultValue(count) || (count == 0.0))
  {
    mean	= defaultValue;
    stdDev	= defaultValue;
    min		= defaultValue;
    max		= defaultValue;
    return;
  }
  stdDev	= Sqrt(stdDev / count);
}

//----------------------------------------

double CTools::UnaryMinus
		(double X)
{
  if (X == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;
  return -X;
}


//----------------------------------------

double CTools::BitwiseNot
		(double X)
{
  int32_t	IntVal;
  if (CastValue(IntVal, X))
    return ~IntVal;
  return m_defaultValueDOUBLE;
}

//----------------------------------------

double CTools::UnaryNot
		(double X)
{
  if (X == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;
  return ! X;
}

//----------------------------------------

double CTools::Multiply
		(double X,
		 double Y)
{
  if ((X == m_defaultValueDOUBLE) || (Y == m_defaultValueDOUBLE))
    return m_defaultValueDOUBLE;
  return X*Y;
}

//----------------------------------------

double CTools::Divide
		(double X,
		 double Y)
{
  if ((X == m_defaultValueDOUBLE) || (Y == m_defaultValueDOUBLE) ||
      (fabs(Y) < 1.0E-70))
    return m_defaultValueDOUBLE;
  return X/Y;
}

//----------------------------------------

double CTools::Plus
		(double X,
		 double	 Y)
{
  if ((X == m_defaultValueDOUBLE) || (Y == m_defaultValueDOUBLE))
    return m_defaultValueDOUBLE;
  return X+Y;
}

//----------------------------------------
double CTools::Minus(double X, double Y)
{
  if ((X == m_defaultValueDOUBLE) || (Y == m_defaultValueDOUBLE))
  {
    return m_defaultValueDOUBLE;
  }

  return (X - Y);
}
//----------------------------------------

double CTools::And
		(double	X,
		 double	Y)
{
  if ((X == m_defaultValueDOUBLE) || (Y == m_defaultValueDOUBLE))
    return m_defaultValueDOUBLE;
  return X && Y;
}

//----------------------------------------

double CTools::BitwiseAnd
		(double X,
		 double	Y)
{
  int32_t	IntValX;
  int32_t	IntValY;
  if (CastValue(IntValX, X) && CastValue(IntValY, Y))
  {
    return IntValX & IntValY;
  }
  return m_defaultValueDOUBLE;
}

//----------------------------------------

double CTools::Or
		(double	X,
		 double	Y)
{
  if ((X == m_defaultValueDOUBLE) || (Y == m_defaultValueDOUBLE))
    return m_defaultValueDOUBLE;
  return X || Y;
}

//----------------------------------------

double CTools::BitwiseOr
		(double X,
		 double	Y)
{
  int32_t	IntValX;
  int32_t	IntValY;
  if (CastValue(IntValX, X) && CastValue(IntValY, Y))
  {
    return IntValX | IntValY;
}
  return m_defaultValueDOUBLE;
}

//----------------------------------------

double CTools::Sign
		(double	 X)
{
  if (X == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;
  if (X < 0.0)
    return -1.0;
  else
    return 1.0;
}

//----------------------------------------

double CTools::SinD
		(double X)
{
  return Sin(Deg2Rad(X));
}

//----------------------------------------

double CTools::CosD
		(double X)
{
  return Cos(Deg2Rad(X));
}
//----------------------------------------

double CTools::ACosD(double X)
{
  return CTools::ACos(Deg2Rad(X));
}

//----------------------------------------

double CTools::TanD
		(double X)
{
  return CTools::Tan(CTools::Deg2Rad(X));
}

//----------------------------------------

double CTools::Min
		(double X1,
		 double X2)
{
  if (X1 == m_defaultValueDOUBLE)
    return X2;
  if (X2 == m_defaultValueDOUBLE)
    return X1;
  return (X1 <= X2 ? X1 : X2);
}

//----------------------------------------

double CTools::Max
		(double X1,
		 double X2)
{
  if (X1 == m_defaultValueDOUBLE)
    return X2;
  if (X2 == m_defaultValueDOUBLE)
    return X1;
  return (X1 >= X2 ? X1 : X2);
}

//----------------------------------------

double CTools::IsBounded
		(double min,
		 double X,
		 double max)
{
  if ((min == m_defaultValueDOUBLE) ||
      (X   == m_defaultValueDOUBLE) ||
      (max == m_defaultValueDOUBLE))
  {
    return m_defaultValueDOUBLE;
  }
  return ((min <= X) && (X <= max) ? 1.0 : 0.0);
}

//----------------------------------------

double CTools::IsBoundedStrict
		(double min,
		 double X,
		 double max)
{
  if ((min == m_defaultValueDOUBLE) ||
      (X   == m_defaultValueDOUBLE) ||
      (max == m_defaultValueDOUBLE))
  {
    return m_defaultValueDOUBLE;
  }
  return ((min < X) && (X < max) ? 1.0 : 0.0);
}

//----------------------------------------

double CTools::Abs
		(double X)
{

/*
** Not necessary since m_defaultValueDOUBLE is positive
** and in many case it increase the speed

  if (value == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;
*/

  return fabs(X);
}

//----------------------------------------

double CTools::Sqr
		(double X)
{
  double result;
  if(X == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;
  result	= X*X;
  if (CTools::IsInf(result))
    return m_defaultValueDOUBLE;
  return X * X;
}

//----------------------------------------

double CTools::Sqrt
		(double X)
{
  double result;
  if ((X < 0.0) || (X == m_defaultValueDOUBLE))
    return m_defaultValueDOUBLE;

  result	= sqrt(X);
  if (result < m_CompareEpsilon)
    return 0.0;

  return result;
}

//----------------------------------------

double CTools::Log
		(double X)
{
  if ((X < m_CompareEpsilon) || (X == m_defaultValueDOUBLE))
    return m_defaultValueDOUBLE;
  return log(X);
}

//----------------------------------------

double CTools::Log10
		(double X)
{
  if ((X < 1.0E-70) || (X == m_defaultValueDOUBLE))
    return m_defaultValueDOUBLE;
  return log10(X);
}

//----------------------------------------

double CTools::Pow
		(double X,
		 double Y)
{
  double result;
  if ((X == m_defaultValueDOUBLE) || (Y == m_defaultValueDOUBLE))
    return m_defaultValueDOUBLE;

  // Accept power as decimal. Control above is suppressed
//  if (Frac(Y) != 0.0)
//    return m_defaultValueDOUBLE;

  result	= pow(X, Y);
  if (CTools::IsInf(result))
    return m_defaultValueDOUBLE;
  return result;
}

//----------------------------------------

double CTools::Exp
		(double X)
{
  double	result;

  if (X <= -700.0)
    return 0.0;

/*
** True for m_defaultValueDOUBLE since m_defaultValueDOUBLE is > 700
*/
  if (X >= 700.0)
    return m_defaultValueDOUBLE;

  result = exp(X);
  if (CTools::IsInf(result))
    return m_defaultValueDOUBLE;
  if (fabs(result) < m_CompareEpsilon)
    return 0.0;
  return result;
}

//----------------------------------------

double CTools::Floor
		(double X)
{

/*
** Not necessary since m_defaultValueDOUBLE is an integer
** and in many case it increase the speed

  if (value == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;
*/
  return floor(X);
}

//----------------------------------------

double CTools::Ceil
		(double X)
{

/*
** Not necessary since m_defaultValueDOUBLE is an integer
** and in many case it increase the speed

  if (value == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;
*/
  return ceil(X);
}

//----------------------------------------

double CTools::Mod
		(double X,
		 double Y)
{
  if ((X == m_defaultValueDOUBLE) || (Y == m_defaultValueDOUBLE) || Y == 0.0)
    return m_defaultValueDOUBLE;

  return  fmod(X, Y);
}
//----------------------------------------

bool CTools::Xor(bool p, bool q)
{
  return ( (p || q) && !(p && q) );
}

//----------------------------------------

double CTools::Deg2Rad
		(double X)
{
  if (X == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;

  return X * M_PI/180.0;
}

//----------------------------------------

double CTools::Rad2Deg
		(double X)
{
  if (X == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;

  return X * 180.0/M_PI;
}

//----------------------------------------

double CTools::Sin
		(double X)
{
  if (X == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;

  return sin(X);
}

//----------------------------------------

double CTools::Cos
		(double X)
{
  if (X == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;

  return cos(X);
}
//----------------------------------------

double CTools::ACos
		(double X)
{
  if (X == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;

  return acos(X);
}

//----------------------------------------

double CTools::Tan
		(double X)
{
  double result;
  if (X == m_defaultValueDOUBLE)
    return m_defaultValueDOUBLE;

  result	= tan(X);
  if (CTools::IsInf(result) || CTools::IsNan(result))
    return m_defaultValueDOUBLE;
  return result;
}
//----------------------------------------
double CTools::Sinc(double x)
{
  if (x == m_defaultValueDOUBLE)
  {
    return m_defaultValueDOUBLE;
  }

  return sin(x) / x;
}

//----------------------------------------
bool CTools::IsLongitudeCircular(double	min, double	max, double compareEpsilon /* = CTools::m_CompareEpsilon */)
{
  double maxNormalized = CTools::NormalizeLongitude(min, max);
  double maxValue = maxNormalized;

  if ((maxNormalized != max) && (maxNormalized == 0))
  {
    maxValue = 360;
  }


  double valueToCompare = maxValue - min;

  return (CTools::Compare( valueToCompare , 360.0, compareEpsilon) >= 0);
  //return (CTools::Compare( CTools::NormalizeLongitude(min, max) - min , 360.0, compareEpsilon) >= 0);
}

//----------------------------------------

double CTools::NormalizeLongitude
		(double Floor,
		 double Longitude)
{
  static const double Epsilon	= 1.0E-9;
  double result;

  if ((Floor == m_defaultValueDOUBLE) || (Longitude == m_defaultValueDOUBLE))
    return m_defaultValueDOUBLE;

  result	= Longitude;

  if (result == (Floor+360.0))
  {
    return result;
  }

  while (result >= ((Floor+360.0)-Epsilon))
  {
    result	-= 360.0;
  }

  while (result < Floor-Epsilon)
  {
    result	+= 360.0;
  }

  if (fabs(result-Floor) <= Epsilon)
  {
    result	= Floor;
  }

  return result;
}

//----------------------------------------

double CTools::IsDefaultFloat
		(double X)
{
  return (X == m_defaultValueDOUBLE ? 1.0 : 0.0);
}

//----------------------------------------
int32_t CTools::IsInf
		(double X)
{

#if HAVE_ISINF
  return isinf(X);
#else
  return (_fpclass(X) == _FPCLASS_NINF || _fpclass(X) == _FPCLASS_PINF);
#endif
}

//----------------------------------------
int32_t CTools::IsNan
		(double X)
{

#if HAVE_ISNAN
  return isnan(X);
#else
  #if HAVE_USCORE_ISNAN
    return _isnan(X);
  #else
    #error "ERROR Compiling - There no function to checks given double-precision floating-point value for not a number (NAN)."
  #endif
#endif
}



//----------------------------------------
string CTools::TrailingZeroesTrim(const string &Text, bool dotTrim /*= true*/)
{
  string      	result;
  uint32_t	cp, ep;
  bool		dot;

  result	= Text+'Z'; // Sentinel
  cp	= 0;

  dot = false;

  // Find the first character after the xxx.yyy representation
  if (Text[cp] == '-')
  {
    ++cp;
  }

  while (isdigit(Text[cp]) || Text[cp] == '.')
  {
    if (Text[cp]=='.')
    {
      dot = true;
    }
    ++cp;
  }

  if (dot && (Text[--cp] != '.'))
  {// Number does not ends with dot

    // Store position of the first character after the number
    ep = cp + 1;

    // count the leading zeroes
    while (Text[cp] == '0')
    {
	    --cp;
    }

    ++cp;

    if (cp != ep)
    {
	    /* Copy (pack) the end of the string */
	    result.erase(cp, ep - cp );
    }
  }

  result.resize(result.size()-1);	// Remove sentinel
  
  if (dotTrim && result[result.size()-1] == '.')
  {
    result.resize(result.size()-1);	// Remove dot
  }

  return result;
}

//----------------------------------------

bool CTools::FileExists
		(const string	&Name)
{

  std::ifstream file;

  file.open(Name.c_str());

  if(file.is_open())
  {
    file.close();
    return true;
  }
  else 
  {
    return false;
  }  

}

//----------------------------------------
bool CTools::DirectoryExists
		(const string	&Name)
{
    struct stat statBuf;
    const char *dir = Name.c_str();

    return stat(dir, &statBuf) == 0 && S_ISDIR(statBuf.st_mode);
}

//----------------------------------------
void CTools::SetDataDirForExecutable
        (const char *argv0)
{
  if (argv0 == NULL)
  {
    return;
  }

  string filepath = MakeCorrectPath(argv0);
  if (filepath.empty())
  {
    return;
  }

  string dirname = DirName(filepath);
  if (dirname.empty() || (dirname == "." && argv0[0] != '.'))
  {
    string filename = filepath;
    string path;
    char *pathEnvVar;
    
    filepath = "";

#ifdef WIN32
    if (filename.size() <= 4 || StringToLower(filename.substr(filename.size() - 4)) != ".exe")
    {
       filename += ".exe";
    }
    path = string(".") + PATH_LIST_SEPARATOR_CHAR;
#endif
    if ((pathEnvVar = getenv("PATH")) != NULL)
    {
      path += pathEnvVar;
    }
    
    if (!path.empty())
    {
      filepath = FindFileInPath(filename, path);
    }
    if (filepath.empty())
    {
      return;
    }
  }
  
  string datadir = MakeCorrectPath(AbsolutePath(DirName(filepath) + (PATH_SEPARATOR ".." PATH_SEPARATOR "data")));
  if (!datadir.empty())
  {
    SetDataDir(datadir);
  }
}

//----------------------------------------
void CTools::SetDataDir
        (const string &DataDir)
{
  m_DataDir = DataDir;
}


//----------------------------------------
string CTools::GetDataDir
		()
{
  // Warning : under Windows, memory leaks when using getenv : see Microsoft Article ID : 815315 
  // So it's why we do the call only once
  if (m_DataDir.length() == 0)
  {
    char *Path	= getenv(BRATHL_ENVVAR);
    if (Path == NULL || Path[0] == 0)
    {
      m_DataDir	= BRATHL_DEFDATADIR;
    }
    else
    {
      m_DataDir	= Path;
    }
  }

  return m_DataDir;
}


//----------------------------------------
string CTools::FindDataFile
		(const string	&FileName)
{
  string result	= GetDataDir() + PATH_SEPARATOR + FileName;

  if (! FileExists(result))
  {
    result	= "";
  }
  return result;
}
//----------------------------------------
bool CTools::LoadAndCheckUdUnitsSystem(string& errorMsg)
{
  try
  {
    CUnit unit;
  }
  catch (CException &e)
  {  
    errorMsg = CTools::Format("BRAT ERROR while loading udunits system:\n%s", e.what());
    return false;
  }
  catch (exception &e)
  {
    errorMsg = CTools::Format("BRAT RUNTIME ERROR while loading udunits system:\n%s", e.what());
    return false;
  }
  catch (...)
  {
    errorMsg = "BRAT FATAL ERROR - Unexpected error encountered while loading udunits system";
    return false;
  }

  return true;
}

//----------------------------------------
string CTools::FindFileInPath
		(const string &filename, const string &path)
{
  size_t last, first = 0;
  string filepath;

  while (first < path.size())
  {
    last = path.find(PATH_LIST_SEPARATOR_CHAR, first);
    if (last == string::npos)
    {
      last = path.size();
    }

    filepath = path.substr(first, last - first);
    filepath += PATH_SEPARATOR_CHAR;
    filepath += filename;
      
    if (FileExists(filepath))
    {
      break;
    }
      
    first = last + 1;
    filepath = "";
  }

  return filepath;
}

//----------------------------------------
string CTools::SlashesEncode (const string& str, const string& exclude, const string& literals, bool hexadecimal)
{
  uint32_t i;
  string ret;
  string replace;

  if (str.empty())
  {
    return "";
  }

  for (i = 0; i < str.size(); i++)
  {
    replace = str[i];

    switch (str[i])
    {

      case '\a':
	      if (strchr (exclude.c_str(), '\a')) break;
	      replace = "\\a";
	      break;

      case '\b':
	      if (strchr (exclude.c_str(), '\b')) break;
	      replace = "\\b";
	      break;

      case '\f':
	      if (strchr (exclude.c_str(), '\f')) break;
	      replace = "\\f";
	      break;

      case '\n':
	      if (strchr (exclude.c_str(), '\n')) break;
	      replace = "\\n";
	      break;

      case '\r':
	      if (strchr (exclude.c_str(), '\r')) break;
	      replace = "\\r";
	      break;

      case '\t':
	       if (strchr (exclude.c_str(), '\t')) break;
	      replace = "\\t";
	      break;

      case '\v':
	      if (strchr (exclude.c_str(), '\v')) break;
	      replace = "\\v";
	      break;

      case '\\':
	      if (strchr (exclude.c_str(), '\\')) break;
	      replace = "\\\\";
	      break;

      default:
	      if (static_cast<uint32_t>(str[i]) > 31 && static_cast<uint32_t>(str[i]) < 127)
              {
	        if (literals.empty()) break;
	        if (!strchr (literals.c_str(), str[i])) break;
                replace = CTools::Format(8, "\\%c", str[i]);
	      }
              else
              {
	        if (strchr (exclude.c_str(), str[i])) break;
	        if (hexadecimal)
                {
                  replace = CTools::Format(10, "\\x%.2x", static_cast<unsigned char>(str[i]));
	        }
                else
                {
                  replace = CTools::Format(10, "\\%.3o", static_cast<unsigned char>(str[i]));
	        }
	      }
	      break;
    }

    if (replace.empty() == false)
    {
      ret += replace;
    }

  }

  return ret;
}

//----------------------------------------
string CTools::SlashesDecode (const string& str, const string& exclude, bool decodeliterals)
{
  uint32_t i, tmpic, matchlen;
  string ret;
  string replace;

  if (str.empty())
  {
    return "";
  }

  for (i = 0; i < str.size(); i++)
  {
    replace = str[i];

    if (str[i] != '\\')
    {
      ret += replace;
      continue;
    }

    matchlen = 2;

    switch (str[i+1])
    {
      case 'a':
	      if (strchr (exclude.c_str(), 'a')) break;
	      replace = "\a";
	      break;

      case 'b':
	      if (strchr (exclude.c_str(), 'b')) break;
	      replace = "\b";
	      break;

      case 'f':
	      if (strchr (exclude.c_str(), 'f')) break;
	      replace = "\f";
	      break;

      case 'n':
	      if (strchr (exclude.c_str(), 'n')) break;
	      replace = "\n";
	      break;

      case 'r':
	      if (strchr (exclude.c_str(), 'r')) break;
	      replace = "\r";
	      break;

      case 't':
	      if (strchr (exclude.c_str(), 't')) break;
	      replace = "\t";
	      break;

      case 'v':
	      if (strchr (exclude.c_str(), 'v')) break;
	      replace = "\a";
	      replace=strdup ("\v");
	      break;

      case '\\':
	      // the one exclude exception
	      if (strchr (exclude.c_str(), '\\'))
              {
	        i++;
	        break;
	      }
	      replace = "\\";
	      break;

      case 'x':
	      if (strchr (exclude.c_str(), 'x')) break;
	      if (!isxdigit (str[i+2]))
              {
                //slashes_decode(): Malformed hexadecimal escape sequence.;
	        i += matchlen;
	        break;
	      }
	      replace.resize(10);
	      replace[0] = str[i+2];
	      matchlen++;
	      if (isxdigit (str[i+3]))
              {
	        replace[1] = str[i+3];
	        matchlen++;
	      }
	      tmpic = static_cast<int32_t>(strtol(replace.c_str(), NULL, 16));
	      if (tmpic == 0)
              {
	        //slashes_decode(): null hexadecimal escape sequence encountered, not decoding.
	        replace = "";
	        break;
	      }
              replace = CTools::Format(8, "%c", tmpic);
              i += matchlen - 2;

	      break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8': // malformed but accepted
      case '9': // malformed but accepted
	      // this exclusion is the only way unfortanly
	      if (strchr (exclude.c_str(), str[i+1])) break;
	      replace.resize(10);
	      replace[0] = str[i+1];
	      if (isdigit (str[i+2]))
              {
	        replace[1] = str[i+2];
	        matchlen++;
	        if (isdigit (str[i+3]))
                {
		  replace[2] = str[i+3];
		  matchlen++;
	        }
	      }
	      if (replace[0] == '8' || replace[0] == '9' ||
		  replace[1] == '8' || replace[1] == '9' ||
		  replace[2] == '8' || replace[2] == '9')
              {
		      //slashes_decode(): Malformed octal escape sequence, result may not be what is expected.
	      }
	      tmpic = static_cast<int32_t>(strtol(replace.c_str(), NULL, 8));
	      if (tmpic == 0)
              {
	        //slashes_decode(): null octal escape sequence encountered, not decoding.
	        replace = "";
	        break;
	      }
              replace = CTools::Format(8, "\\%c", tmpic);
              i += matchlen - 2;
	      break;

      default:
	      if (!decodeliterals) break;
	      if (strchr (exclude.c_str(), str[i+1])) break;
              replace.resize(4);
	      replace[0] = str[i+1];
	      break;
    }

    if (replace.empty() == false)
    {
      i++;
      ret += replace;
    }

  }

  return ret;
}

//----------------------------------------
string CTools::RemoveCharSurroundingNumber (const string& str, const char c1, const char c2)
{
  uint32_t i;
  string ret;
  string subStr;

  if (str.empty())
  {
    return "";
  }

  ret = str;

  int32_t begin  = 0;
  int32_t end = -1;
  int32_t pos = -1;
  do
  {
    bool foundC1 = false;
    bool foundC2 = false;
    for (pos = begin; pos < static_cast<int32_t>(ret.size()) ; pos++)
    {
      if (ret[pos] == c1)
      {
        foundC1= true;
        begin = pos;
      }

      if (ret[pos] == c2)
      {
        if (foundC1)
        {
          foundC2 = true;
          end = pos;
        }
      }
      if ( (foundC1) && (foundC2) )
      {
        break;
      }

    }

    if ( (foundC1 == false) || (foundC2 == false) )
    {
      return ret;
    }

    bool isNumber = true;
    bool foundPoint = false;

    int32_t iBegin = 0;

    if ((begin + 1) < end)
    {
      subStr = ret.substr(begin + 1, end - begin - 1);
      if ( (subStr[0] == '+') || (subStr[0] == '-') )
      {
        iBegin = 1;
      }
    }
    else
    {
      subStr = "";
      isNumber = false;
    }

    for ( i = iBegin; i < subStr.size(); i++)
    {
      switch (subStr[i])
      {
        case '.':
        {
          if (foundPoint)
          {
            isNumber = false;
            break;
          }
          foundPoint = true;
          break;
        }

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
          break;
        }
        default:
        {
          isNumber = false;
          break;
        }
      }
      if (isNumber == false)
      {
        break;
      }
    }

    if (isNumber)
    {
      ret.erase(end, 1);
      ret.erase(begin, 1);
      begin = end - 1;
    }
    else
    {
      begin = end + 1;
    }
  }
  while (begin < static_cast<int32_t>(ret.size() - 1));

  return ret;

}
//----------------------------------------
string CTools::Replace(const string& inText, const string& regexpPattern, const string replaceString)
{
  CRegexpT <char> regex(regexpPattern.c_str());

  // Warning : the value returned by CRegexpT <char>::Replace must be freed
  // It was allocate with malloc (or realloc), so use 'free' function is free memory 
  char* result = regex.Replace( inText.c_str(), replaceString.c_str() );

  string value = result;
  free(result);

  return value;

}
/*
//----------------------------------------
void CTools::ReplaceWord(const string& in_text, const string& find_word, const string& replace_word, string& out_text)
{
  if (in_text.empty())
  {
    return;
  }

  uint32_t start_loc = 0;
  uint32_t char_num = 0;
  ostringstream outString;

  while (char_num < in_text.length())
  {
    start_loc = in_text.find(find_word,char_num);

    if (start_loc != string::npos )
    {
      outString << in_text.substr(char_num,start_loc);
      outString << replace_word;
      char_num = start_loc+find_word.length();
    }
    else
    {
      outString << in_text.substr(char_num,in_text.length()-char_num);
      char_num = in_text.length();
    }
  }

  out_text = outString.str();

  return;
}
*/
//----------------------------------------
string CTools::ReplaceString(const string& inText, const vector<string>& findWords, const vector<string>& replaceWords)
{

  if (inText.empty())
  {
    return inText;
  }

  string outText = inText;

  size_t nbWords = std::min(findWords.size(), replaceWords.size());

  for (size_t  i = 0 ; i < nbWords ; i++)
  {
    outText = CTools::Replace(outText, findWords.at(i), replaceWords.at(i));
  }

  return outText;
}
//----------------------------------------
string CTools::ReplaceWord(const string& inText, const vector<string>& findWords, const vector<string>& replaceWords)
{

  if (inText.empty())
  {
    return inText;
  }

  string outText = inText;


  size_t nbWords = std::min(findWords.size(), replaceWords.size());

  for (size_t i = 0 ; i < nbWords ; i++)
  {
    string pattern;
    pattern.append("\\b");
    pattern.append(findWords.at(i));
    pattern.append("\\b");

    outText = CTools::Replace(outText, pattern, replaceWords.at(i));
  }

  return outText;
}
//----------------------------------------
string CTools::ReplaceWord(const string& inText, const string& findWords, const string& replaceWords)
{

  if (inText.empty())
  {
    return inText;
  }

  string outText = inText;

  string pattern;
  pattern.append("\\b");
  pattern.append(findWords);
  pattern.append("\\b");

  return CTools::Replace(outText, pattern, replaceWords);

}

//----------------------------------------
void CTools::Find(const string& inText, const string& regexpPattern, vector<string>& stringFound)
{
  if (inText.empty())
  {
    return;
  }

  CRegexpT <char> regexp(regexpPattern.c_str());
  
  CContext context;

  MatchResult result = regexp.Match(inText.c_str(), -1, &context);

  while( result.IsMatched() != 0) 
  {
    int32_t group = result.MaxGroupNumber();
    string str;
    if (group < 0)
    {
      str = inText.substr(result.GetStart(), result.GetEnd() - result.GetStart());
    }
    else
    {
      str = inText.substr(result.GetGroupStart(group), result.GetGroupEnd(group) - result.GetGroupStart(group));
    }

    if (! str.empty())
    {
      stringFound.push_back(str);
    }
    result = regexp.Match(&context);

  }

}
//----------------------------------------
void CTools::FindWord(const string& inText, vector<string>& wordsFound)
{

  if (inText.empty())
  {
    return;
  }

  string pattern = "\\b\\w+\\b";

  CTools::Find(inText, pattern, wordsFound);
}
//----------------------------------------
void CTools::FindAliases(const string& inText, vector<string>& aliasesFound, 
                         bool onlyName /*= false*/,                                 
                         const string& begining	/*= "%"*/,
                         bool recurse /*= false*/, 
                         const map<string, string>* varValues /*= NULL*/,
                         const map<string, string>* fieldAliases /*= NULL*/,
                         bool withExcept /*= false*/, string* errorMsg /*= NULL*/)
{

  string pattern;
  pattern.append("\\");
  pattern.append(begining);
  pattern.append("\\{[\\+,\\-]?");
  if (onlyName)
  {
    pattern.append("(");
  }
  pattern.append("\\w+");
  if (onlyName)
  {
    pattern.append(")");
  }
  pattern.append("\\}");
  
  vector<string> aliasesTmp;

  CTools::Find(inText, pattern, aliasesTmp);

  if (aliasesTmp.size() > 0)
  {
    aliasesFound.insert(aliasesFound.end(), aliasesTmp.begin(), aliasesTmp.end());
  }

  if (! recurse)
  {
    return;
  }

  if (varValues == NULL)
  {
    return;
  }

  vector<string>::const_iterator it;

  for (it = aliasesTmp.begin() ; it != aliasesTmp.end() ; it++)
  {
    string strAlias;
    if (onlyName)
    {
      strAlias.append(begining);
      strAlias.append("{");
      strAlias.append(it->c_str());
      strAlias.append("}");

    }
    else
    {
      strAlias.append(it->c_str());
    }
    // If errorMsg is NULL, force withExcept to true (force to raise an exception) to avoid an infinite loop.
    if (errorMsg == NULL)
    {
      withExcept = true;
    }

    string str = CTools::ExpandVariables(strAlias, varValues, fieldAliases, false, begining.at(0), false, withExcept, errorMsg);
    
    // If errorMsg not NULL and not empyt: there is an error: exit loop;
    if (errorMsg != NULL) 
    {
      if (!errorMsg->empty()) 
      {
        break;
      }
    }
      
    CTools::FindAliases(str, aliasesFound, onlyName, begining, recurse, varValues, fieldAliases, withExcept, errorMsg);

  }
}

//----------------------------------------
string CTools::ExpandShellVar(const string& value)
{
  return CTools::ExpandVariables(value, NULL, false, '$');
}

//----------------------------------------
string CTools::ExpandVariables(const string& valueIn, const map<string, string>	*varValues,
                               bool recurse /*= false*/, char beginning	/*= '%'*/, uint32_t* numberVarsExpanded /*= NULL*/,
                               bool withExcept /*= false*/, string* errorMsg /*= NULL*/)
{
  return CTools::ExpandVariables(valueIn, varValues, NULL, recurse,  beginning	, numberVarsExpanded);
}
//----------------------------------------
string CTools::ExpandVariables(const string& valueIn, const map<string, string>	*varValues, const map<string, string>	*fieldAliases,
                               bool recurse /*= false*/, char beginning	/*= '%'*/, uint32_t* numberVarsExpanded /*= NULL*/,
                               bool withExcept /*= false*/, string* errorMsg /*= NULL*/)
{
  string toExpand;
  string result	= valueIn;
  string toFind	= " {";
  string::size_type	begin;
  string::size_type	end;
  string::size_type	sizeToFind = toFind.size();
  
  bool oneExpanded;
  uint32_t numberExpanded = 0;
  
  char nonPrintable = 0x7F;

  bool hasNonPrintable = false;
  
  uint32_t count	= 0;

  const string validChars	= "+-ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789";

  toFind[0]	= beginning;

  do
  {
    // Check for circular references when doing recursive expansion
    if (++count >= 100)
    {
      break;
    }

    toExpand	= result;

    result	= "";

    oneExpanded	= false;
    begin	= 0;
    end		= toExpand.find(toFind);

    while (end != string::npos)
    {
      // test if \%{ is encountered
      if ((end != 0) && (toExpand[end-1] == '\\'))
      {
	      result.append(toExpand, begin, end + sizeToFind - begin);

	      begin	= end + sizeToFind;
        end	= toExpand.find(toFind, begin);

	      continue;
      }

      // Store characters before %{
      result.append(toExpand, begin, end - begin);

      begin	= end;

      // Find the end of the name
      end	= toExpand.find_first_not_of(validChars, end + sizeToFind);

      if (end == string::npos)
      {
	      continue; // until the end of the string: missing '}'
      }

      // Test if first char not belonging to a valid name is not a '}'
      // Or name is empty
      if ((toExpand[end] != '}') || (end == begin + sizeToFind))
      {
        result.append(toExpand, begin, end - begin);

	      begin	= end;
        end	= toExpand.find(toFind, begin);

	      continue;
      }

      // Name is found, expand it
      string name	= toExpand.substr(begin + sizeToFind, end - sizeToFind - begin);
      string value	= "";
      char resultCase	= name[0];

      // Check if upper or lower case is asked
      if ((resultCase == '+') || (resultCase == '-'))
      {
	        name.erase(0,1); // Yes, remove the char.
      }

      if ((varValues == NULL) && (fieldAliases == NULL))
      {// Name got from environment variables
	      char *getEnvValue	= getenv(name.c_str());

	      if (getEnvValue != NULL)
        {
	        value.assign(getEnvValue);
        }
      } 
      else if (varValues != NULL)
      {// Name got from variables values passed
	      map<string,string>::const_iterator itValue	= varValues->find(name);
	      if (itValue != varValues->end())
        {
	        value	= itValue->second;
        }
        else if (fieldAliases != NULL)
        {
	        itValue	= fieldAliases->find(name);
	        if (itValue != fieldAliases->end())
          {
	          value	= itValue->second;
          }
          else
          {
            // Rebuild the alias, but to avoid infinite loop if recurse is true:
            // use a non printable char as "beginning" that will be replaced by the "beginning" char later
            value.clear();
            CTools::StringToAlias(name, value, nonPrintable);
            hasNonPrintable = true;

            string msg = CTools::Format("Alias or variable '%s' is not defined in expression '%s'", name.c_str(), valueIn.c_str());
            if (errorMsg != NULL)
            {
              errorMsg->append(msg);
            }
            if (withExcept)
            {
              CException e(msg, BRATHL_INCONSISTENCY_ERROR);
              throw e;
            }
          }
        }
        else
        {
            // Rebuild the alias, but to avoid infinite loop if recurse is true:
            // use a non printable char as "beginning" that will be replaced by the "beginning" char later
            value.clear();
            CTools::StringToAlias(name, value, nonPrintable);
            hasNonPrintable = true;

            string msg = CTools::Format("Alias or variable '%s' is not defined in expression '%s'", name.c_str(), valueIn.c_str());
            if (errorMsg != NULL)
            {
              errorMsg->append(msg);
            }
            if (withExcept)
            {
              CException e(msg, BRATHL_INCONSISTENCY_ERROR);
              throw e;
            }
        }
      }

      // Add the expanded value according to the asked case
      switch (resultCase)
      {
      case '+':
	      result.append(CTools::StringToUpper(value));
	      break;
      case '-':
	      result.append(CTools::StringToLower(value));
	      break;
      default:
	      result.append(value);
      }

      oneExpanded	= true;
      numberExpanded++;


      // Try to find next one.
      begin		= end + 1;
      end		= toExpand.find(toFind, begin);

    } //end while (end != string::npos)

    // Register remaining chars.
    result.append(toExpand, begin, toExpand.size() - begin);

  } while (recurse && oneExpanded);

  if (hasNonPrintable)
  {
    result = CTools::StringReplace(result, nonPrintable, beginning);
  }

  if (numberVarsExpanded != NULL)
  {
    *numberVarsExpanded = numberExpanded;
  }
  return result;
}

//----------------------------------------
void CTools::StringToAlias(const string& in, string& out, const char beginning	/*= '%'*/)
{
  out.append(1, beginning);
  out.append(1, '{');
  out.append(in);
  out.append(1, '}');
}
//----------------------------------------
string CTools::MakeCorrectPath(const string& path)
{
    string p(path);
    string r;
    int i, j, cnt;

    char pathSep = PATH_SEPARATOR_CHAR;
    char reverseSep = ((pathSep == '\\') ? '/' : '\\');

    cnt = p.length();
    
    for (i = 0; i < cnt; i++)
      if (p.at(i) == reverseSep)
      {
        p.at(i) = pathSep; 
      }
    
    string tmp = ".";
    tmp += PATH_SEPARATOR;

    if (p.compare(0, 2, tmp) == 0) 
    {
      p = p.substr(2, p.size() - 2);
      cnt -= 2;
    }

    if (cnt < 3)
    {
      return p;
    }

    r.insert(r.end(), p.at(0));
    r.insert(r.end(), p.at(1));

    // skip trailing ../.., if any
    for (i = 2; (i < cnt) && ((p.at(i) == pathSep) || (p.at(i) == '.')); i++)
    {
      r.insert(r.end(), p.at(i));
    }

    // remove back references: translate dir1/../dir2 to dir2
    for (; i < cnt; i++)
    {
        r.insert(r.end(), p.at(i));
        if ( (p.at(i) == pathSep) && (p.at(i-1) == '.') && (p.at(i-2) == '.'))
        {
            for (j = r.length() - 2; (j >= 0) && (r.at(j) != pathSep) && (r.at(j) != ':'); j--) {}
            if (j >= 0 && r.at(j) != ':')
            {
                for (j = j - 1; j >= 0 && r.at(j) != pathSep && r.at(j) != ':'; j--) {}
                r.erase(r.begin() + j + 1, r.end());
            }
        }
    }

    for (; i < cnt; i++)
    {
      r.insert(r.end(), p.at(i));
    }

    return r;
}



//----------------------------------------
string CTools::AbsolutePath( const string& partialPath )
{

  char buff[BRATHL_PATH_MAX+1];
  memset(buff, '\0', BRATHL_PATH_MAX+1);
  string absolutePath;

#if HAVE_FULLPATH
   if( _fullpath( buff, partialPath.c_str(), BRATHL_PATH_MAX ) != NULL )
   {
     absolutePath = buff;
   }
#else
  #if HAVE_REALPATH
   if (realpath(partialPath.c_str() ,buff) != NULL)
   {
     absolutePath = buff;
   }
  #else
    #error "ERROR Compiling - There no function to perform a absolute path translation."
  #endif
#endif

  return absolutePath;
}

//----------------------------------------
string CTools::FileExtension(const string& fileName)
{
  char dirDelim = PATH_SEPARATOR_CHAR;

  // find  the right-most '.' which is also after the right-most dirDelim 
  const char* lastDot = strrchr(fileName.c_str(), '.');
  if (lastDot == NULL) // go ahead and bail if there wasn't even a '.' in the path
  {
    return "";
  }

  
  const char* lastDirDelim = strrchr(fileName.c_str(), dirDelim);


  if( lastDot > lastDirDelim)
  {
    return lastDot + 1;
  }
  else
  {
    return "";
  }
  
}

//----------------------------------------
string CTools::DirName(const string& fileName)
{
#if HAVE_DIRNAME
  char buff[BRATHL_PATH_MAX+1];
  memset(buff, '\0', BRATHL_PATH_MAX+1);
  int32_t len = (fileName.size() < BRATHL_PATH_MAX) ? fileName.size() : BRATHL_PATH_MAX;

  // make a copy because the function modifies the contents
   strncpy(buff, fileName.c_str(), len);
   return dirname(buff);
#else
  #if HAVE_SPLITPATH
   string drive;
   string dirName;

   CTools::SplitPath(fileName, &drive, &dirName);
   return drive + dirName;
  #else
    #error "ERROR Compiling - There is no function to get a dir name from a string."
  #endif
#endif
}

//----------------------------------------
string CTools::BaseName(const string& fileName)
{
#if HAVE_BASENAME
  char buff[BRATHL_PATH_MAX+1];
  memset(buff, '\0', BRATHL_PATH_MAX+1);
  int32_t len = (fileName.size() < BRATHL_PATH_MAX) ? fileName.size() : BRATHL_PATH_MAX;

  // make a copy because the function modifies the contents
   strncpy(buff, fileName.c_str(), len);
   return basename(buff);
#else
  #if HAVE_SPLITPATH
   string fname;

   CTools::SplitPath(fileName, NULL, NULL, &fname);
   return fname;
  #else
    #error "ERROR Compiling - There is no function to get a base file name from a string."
  #endif
#endif
}

//----------------------------------------
#if HAVE_SPLITPATH
void CTools::SplitPath(const string& path, string* drive, string* dir, string* fname, string* ext)
{
  char driveTmp[BRATHL_PATH_MAX+1];;
  char dirTmp[BRATHL_PATH_MAX+1];;
  char fnameTmp[BRATHL_PATH_MAX+1];;
  char extTmp[BRATHL_PATH_MAX+1];
  memset(driveTmp, '\0', BRATHL_PATH_MAX+1);
  memset(dirTmp, '\0', BRATHL_PATH_MAX+1);
  memset(fnameTmp, '\0', BRATHL_PATH_MAX+1);
  memset(extTmp, '\0', BRATHL_PATH_MAX+1);

  _splitpath(path.c_str(), driveTmp, dirTmp, fnameTmp, extTmp);

  if (drive != NULL)
  {
    *drive = driveTmp;
  }
  if (dir != NULL)
  {
    *dir = dirTmp;
  }
  if (fname != NULL)
  {
    *fname = fnameTmp;
  }
  if (ext != NULL)
  {
    *ext = extTmp;
  }
}

#endif // HAVE_SPLITPATH
//----------------------------------------

void CTools::SwapValue
		(int32_t &value)
{
  int32_t	Temp;
  unsigned char *P1	= static_cast<unsigned char *>(static_cast<void *>(&value))+3;
  unsigned char *P2	= static_cast<unsigned char *>(static_cast<void *>(&Temp));

  *(P2++)	= *(P1--);
  *(P2++)	= *(P1--);
  *(P2++)	= *(P1--);
  *P2		= *P1;
  value	= Temp;
}

//----------------------------------------
void CTools::SwapValue
		(float &value)
{
  float	Temp;
  unsigned char *P1	= static_cast<unsigned char *>(static_cast<void *>(&value))+3;
  unsigned char *P2	= static_cast<unsigned char *>(static_cast<void *>(&Temp));

  assert(sizeof(value) == 4);

  *(P2++)	= *(P1--);
  *(P2++)	= *(P1--);
  *(P2++)	= *(P1--);
  *P2		= *P1;
  value	= Temp;
}

//----------------------------------------
void CTools::SwapValue
		(int16_t &value)
{
  int16_t	Temp;
  unsigned char *P1	= static_cast<unsigned char *>(static_cast<void *>(&value));
  unsigned char *P2	= static_cast<unsigned char *>(static_cast<void *>(&Temp));

  *P2		= *(P1+1);
  *(P2+1)	= *P1;
  value	= Temp;
}

//----------------------------------------
void CTools::SwapValue
		(double &value)
{
  float	Temp;
  unsigned char *P1	= static_cast<unsigned char *>(static_cast<void *>(&value))+7;
  unsigned char *P2	= static_cast<unsigned char *>(static_cast<void *>(&Temp));

  assert(sizeof(value) == 8);

  *(P2++)	= *(P1--);
  *(P2++)	= *(P1--);
  *(P2++)	= *(P1--);
  *(P2++)	= *(P1--);
  *(P2++)	= *(P1--);
  *(P2++)	= *(P1--);
  *(P2++)	= *(P1--);
  *P2		= *P1;
  value	= Temp;
}

//----------------------------------------
double CTools::UnconvertLat(const string& value) 
{
  double origVal = m_defaultValueDOUBLE;
  string valueTrim = CTools::StringTrim(value.c_str());

  // regex object
  CRegexpT <char> regexp("[\\d*\\.*\\s*]*[nNsS]");
  CRegexpT <char> regexp2("[\\d*\\.*\\d*]*");
  CRegexpT <char> regexp3("[\\d*\\.*\\s*]*[sS]");

  // match result
  MatchResult result = regexp.MatchExact(valueTrim.c_str());

  // not match exactly ?
  if( result.IsMatched() == 0) 
  {
    origVal =  CTools::StrToDouble(valueTrim.c_str());
    if (CTools::IsDefaultValue(origVal))
    {
      return origVal;
    } 
    else 
    {
      return CLatLonPoint::LatNormal(origVal);
    }

  }

  MatchResult result2 = regexp2.Match(valueTrim.c_str());
  if( result2.IsMatched() == 0) 
  {
      return m_defaultValueDOUBLE;

  }

  string str = valueTrim.substr(result2.GetStart(), result2.GetEnd() - result2.GetStart());

  origVal = CTools::StrToDouble(str);

  if (CTools::IsDefaultValue(origVal))
  {
    return origVal;
  }

  MatchResult result3 = regexp3.MatchExact(valueTrim.c_str());
  if( result3.IsMatched() != 0) 
  {
    origVal = -origVal;

  }

  return CLatLonPoint::LatNormal(origVal);
}

//----------------------------------------
double CTools::UnconvertLon(const string& value, bool normalize /*= true*/) 
{

  double origVal = m_defaultValueDOUBLE;
  string valueTrim = CTools::StringTrim(value.c_str());

  // regex object
  CRegexpT <char> regexp("[\\d*\\.*\\s*]*[eEwW]");
  CRegexpT <char> regexp2("[\\d*\\.*\\d*]*");
  CRegexpT <char> regexp3("[\\d*\\.*\\s*]*[wW]");

  // match result
  MatchResult result = regexp.MatchExact(valueTrim.c_str());

  // not match exactly ?
  if( result.IsMatched() == 0) 
  {
    origVal =  CTools::StrToDouble(valueTrim.c_str());
    if (CTools::IsDefaultValue(origVal))
    {
      return origVal;
    } 
    else 
    {
      return (normalize ? CLatLonPoint::LonNormal(origVal) : origVal);
    }
    
  } 

  MatchResult result2 = regexp2.Match(valueTrim.c_str());
  if( result2.IsMatched() == 0) 
  {
      return m_defaultValueDOUBLE;

  }

  string str = valueTrim.substr(result2.GetStart(), result2.GetEnd() - result2.GetStart());

  origVal = CTools::StrToDouble(str);

  if (CTools::IsDefaultValue(origVal))
  {
    return origVal;
  }

  MatchResult result3 = regexp3.MatchExact(valueTrim.c_str());
  if( result3.IsMatched() != 0) 
  {
    origVal = -origVal;

  }

  return (normalize ? CLatLonPoint::LonNormal(origVal) : origVal);

}

//----------------------------------------
double CTools::StrToDouble(const string& value) 
{

  errno	= 0;
  char *endPtr = NULL;

  string valueTrim = CTools::StringTrim(value);

  double doubleValue	= strtod(valueTrim.c_str(), &endPtr);

  if (*endPtr || (errno != 0))
  {
    return m_defaultValueDOUBLE;
  }
  else
  {
    return doubleValue;

  }

}
//----------------------------------------
string CTools::DoubleToStr(double d, int32_t precision /* = 10 */)
{
  
  const string s;
  ostringstream myStream;

  //floating point values are converted to fixed 
  //notation with the number of digits after the decimal point equal 
  // to the value of ios::x_precision set by precision(value)
  myStream.setf(ios::fixed);
  myStream.precision(precision);

  if (myStream<<d)
  {
    return myStream.str();
  }
  else
  {
    return "";
  }
}
//----------------------------------------
float CTools::StrToFloat(const string& s) 
{

  float f;

  istringstream myStream(s);

  if (myStream>>f)
  {
    return f;
  }
  else
  {
    CTools::SetDefaultValue(f);
    return f;
  }

}
//----------------------------------------
string CTools::FloatToStr(float f, int32_t precision /* = 10 */)
{
  
  const string s;
  ostringstream myStream;

  //floating point values are converted to fixed 
  //notation with the number of digits after the decimal point equal 
  // to the value of ios::x_precision set by precision(value)
  myStream.setf(ios::fixed);
  myStream.precision(precision);

  if (myStream<<f)
  {
    return myStream.str();
  }
  else
  {
    return "";
  }
}
//----------------------------------------
int32_t CTools::StrToInt(const string &s)
{
  int32_t i;

  istringstream myStream(s);
  
  if (myStream>>i)
  {
    return i;
  }
  else
  {
    CTools::SetDefaultValue(i);
    return i;
  }
}
//----------------------------------------
int64_t CTools::StrToInt64(const string &s)
{
  int64_t i;

  istringstream myStream(s);
  
  if (myStream>>i)
  {
    return i;
  }
  else
  {
    CTools::SetDefaultValue(i);
    return i;
  }
}
//----------------------------------------
uint64_t CTools::StrToUInt64(const string &s)
{
  uint64_t i;

  istringstream myStream(s);
  
  if (myStream>>i)
  {
    return i;
  }
  else
  {
    CTools::SetDefaultValue(i);
    return i;
  }
}
//----------------------------------------
string CTools::IntToStr(int32_t i)
{
  
  ostringstream myStream;

  if (myStream<<i)
  {
    return myStream.str();
  }
  else
  {
    return "";
  }
}
//----------------------------------------
int64_t CTools::StrToLong(const string &s)
{
  int64_t i;

  istringstream myStream(s);
  
  if (myStream>>i)
  {
    return i;
  }
  else
  {
    CTools::SetDefaultValue(i);
    return i;
  }
}
//----------------------------------------
string CTools::LongToStr(int64_t i)
{
  
  ostringstream myStream;

  if (myStream<<i)
  {
    return myStream.str();
  }
  else
  {
    return "";
  }
}
//----------------------------------------
uint32_t CTools::VectorOffset(uint32_t* shape, uint32_t ndims, const uint32_t *coord)
{
	if(ndims == 0) /* 'scalar' variable */
  {
		return 0;
  }

	if(ndims == 1)
	{
		return (off_t)(*coord);
	}

	/* 
	 * Compute the right to left product of shape (the dsizes)
	 */

  uint32_t *dsizes = new uint32_t[ndims];
  memset(dsizes, 0, sizeof(*dsizes)*ndims );

 	uint32_t *shp, *dsp;
	uint32_t product = 1;


	/* ndims is > 0 here */
	for(shp = shape + ndims - 1,
			dsp = dsizes + ndims - 1;
 			shp >= shape;
			shp--, dsp--)
	{
		if(!(shp == shape))
    {
			product *= *shp;
    }

		*dsp = product;
	}

	uint32_t lcoord = coord[ndims -1];

	uint32_t *up = dsizes + 1;
	const uint32_t *ip = coord;
	const uint32_t *const end = dsizes + ndims;
	
	for(; up < end; up++, ip++)
  {
		lcoord += *up * *ip;
  }

  delete []dsizes;

  return lcoord;
}
/*
//----------------------------------------
int32_t CTools::VectorContiguousBlock(const CUIntArray& shape, const CUIntArray& edges,	size_t *const countContinousBlock)
{

  if (shape.size() != edges.size())
  {
    throw CException(CTools::Format("ERROR - CTools::VectorContiguousBlock- shape sisze (%d) is not equal to  edges size (%d)",
				   shape.size(), edges.size()),
			  BRATHL_INCONSISTENCY_ERROR);
  }

  size_t* shp = shape.ToSizeTArray();
  size_t* edg = edges.ToSizeTArray();

  int32_t result = CTools::VectorContiguousBlock(shape.size(), shp, edg, countContinousBlock);

  delete []shp;
  delete []edg;
}
*/
//----------------------------------------
int32_t CTools::VectorContiguousBlock(uint32_t ndims, const uint32_t *const shape, const uint32_t *const edges, uint32_t *const countContinousBlock)
{
	const uint32_t *edp0 = edges;
	const uint32_t *edp = edges + ndims;
	const uint32_t *shp = shape + ndims;


	assert(edges != NULL);

	/* find max contiguous */
	while(edp > edp0)
	{
		shp--; edp--;
		if(*edp < *shp )
		{
			const uint32_t *zedp = edp;
			while(zedp >= edp0)
			{
				if(*zedp == 0)
				{
					*countContinousBlock = 0;
					//goto done;
          return((int)(edp - edges) - 1);
				}
				/* Tip of the hat to segmented architectures */
				if(zedp == edp0)
					break;
				zedp--;
			}
			break;
		}
		assert(*edp == *shp);
	}

	/*
	 * edp, shp reference rightmost index s.t. *(edp +1) == *(shp +1)
	 *
	 * Or there is only one dimension.
	 * If there is only one dimension and it is 'non record' dimension,
	 * 	edp is &edges[0] and we will return -1.
	 * If there is only one dimension and and it is a "record dimension",
	 *	edp is &edges[1] (out of bounds) and we will return 0;
	 */
	assert(shp >= shape + ndims -1 
		|| *(edp +1) == *(shp +1));

	/* now accumulate max count for a single io operation */
	for(*countContinousBlock = 1, edp0 = edp;
		 	edp0 < edges + ndims;
			edp0++)
	{
		*countContinousBlock *= *edp0;
	}

//done:
	return((int)(edp - edges) - 1);
}
//----------------------------------------
void CTools::ExtractVector(const double* vectorIn, uint32_t* shape, uint32_t nDims, uint32_t *start, uint32_t *edges, double* vectorOut)
{

  if (vectorOut == NULL)
  {
    return;
  }

  /*
  uint32_t nbValuesToExtract = 1;

  for(uint32_t i = 0 ; i < nDims ; i++)
  {
    nbValuesToExtract		*= edges[i];
  }

  vectorOut = new double[nbValuesToExtract];
  */

  uint32_t countContinousBlock = 0;
  double* extract = vectorOut;

  int32_t  ii = CTools::VectorContiguousBlock(nDims, shape, edges, &countContinousBlock);
	if(ii == -1)
	{
    uint32_t *coord= new uint32_t[nDims];
	  /* copy in starting indices */
	  (void) memcpy(coord, start, nDims * sizeof(*start));

    uint32_t offset = CTools::VectorOffset(shape, nDims, coord);
    memcpy(extract, vectorIn + offset, sizeof(*vectorOut) * countContinousBlock);
    //memcpy(vectorOut, vectorIn, nDims * sizeof(double));
    delete []coord;

	}
  else
	{ 
    uint32_t *coord= new uint32_t[nDims];
    uint32_t *upper= new uint32_t[nDims];

	  const uint32_t index = ii;

	  /* copy in starting indices */
	  (void) memcpy(coord, start, nDims * sizeof(*start));

	  /* set up in maximum indices */
    CTools::SetUpper(upper, start, edges, &upper[nDims]);

	  /* ripple counter */
	  while(*coord < *upper)
	  {
      uint32_t offset = CTools::VectorOffset(shape, nDims, coord);
      memcpy(extract, vectorIn + offset, sizeof(*vectorOut) * countContinousBlock);
      //std::copy(vect.begin() + offset, vect.begin() + offset + countContinousBlock, extract);

      extract += countContinousBlock;

		  CTools::Odo1(start, upper, coord, &upper[index], &coord[index]);
	  }

    delete []upper;
    delete []coord;
	}

}
//----------------------------------------

/*
 * Set the elements of the array 'upp' to
 * the sum of the corresponding elements of
 */
void CTools::SetUpper(uint32_t *upp, /* modified on return */
	                      const uint32_t *stp,
	                      const uint32_t *edp,
	                      const uint32_t *const end)
{
	while(upp < end) {
		*upp++ = *stp++ + *edp++;
	}
}
//----------------------------------------

/*
 * The infamous and oft-discussed odometer code.
 *
 * 'start[]' is the starting coordinate.
 * 'upper[]' is the upper bound s.t. start[ii] < upper[ii].
 * 'coord[]' is the register, the current coordinate value.
 * For some ii,
 * upp == &upper[ii]
 * cdp == &coord[ii]
 * 
 * Running this routine increments *cdp.
 *
 * If after the increment, *cdp is equal to *upp
 * (and cdp is not the leftmost dimension),
 * *cdp is "zeroed" to the starting value and
 * we need to "carry", eg, increment one place to
 * the left.
 * 
 * TODO: Some architectures hate recursion?
 * 	Reimplement non-recursively.
 */

void CTools::Odo1(const uint32_t *const start, const uint32_t *const upper,
	uint32_t *const coord, /* modified on return */
	const uint32_t *upp,
	uint32_t *cdp)
{
	assert(coord <= cdp && cdp <= coord + 1024);
	assert(upper <= upp && upp <= upper + 1024);
	assert(upp - upper == cdp - coord);
	
	assert(*cdp <= *upp);

	(*cdp)++;
	if(cdp != coord && *cdp >= *upp)
	{
		*cdp = start[cdp - coord];
		CTools::Odo1(start, upper, coord, upp -1, cdp -1);
	}
}
//----------------------------------------
uint32_t CTools::GetProductValues(uint32_t* shape, uint32_t nbDims)
{
  uint32_t nbElts = 1;
  
  if (nbDims <= 0)
  {
    nbElts = 0;
  }

  for (uint32_t index = 0; index < nbDims; index++)
  {
    nbElts	*= shape[index];
  }

  return nbElts;
}
//----------------------------------------
void CTools::DeleteObject(CBratObject* ob)
{
  if (ob != NULL)
  {
    delete ob;
  }
}
//----------------------------------------
string CTools::BeforeFirst(const string& str, const char ch)
{
  size_t strLen = str.length();
  size_t iPos = str.find(ch);

  if ( iPos == string::npos ) 
  {
    return str;
  }

  size_t nLen = strLen < iPos ? strLen : iPos;
  return str.substr(0, nLen);
}
//----------------------------------------
string CTools::ToString(const char *s, size_t len /*= string::npos*/)
{
 return string(s, len != string::npos ? len : strlen(s));
}

//----------------------------------------
void CTools::ReplaceAliases(const string& in,  string& out, vector<string>* aliases /*= NULL*/)
{
  CTools::ReplaceAliases(in, "1", out, aliases);

}
//----------------------------------------
void CTools::ReplaceAliases(const string& in, const string& replacedBy, string& out, vector<string>* aliases /*= NULL*/)
{

  vector<string> aliasesTmp;
  vector<string>& aliasesFound = aliasesTmp;
  if (aliases !=  NULL)
  {
    aliasesFound = *aliases;
  }

  CTools::FindAliases(in.c_str(), aliasesFound);
  //CTrace::GetInstance()->SetTraceLevel(5);
  //aliasesFound.Dump();

  out = in;

  vector<string>::const_iterator it;

  for (it = aliasesFound.begin() ; it != aliasesFound.end() ; it++)
  {
    string pattern = *it;
    pattern = CTools::Replace(pattern, "\\%\\{", "\\%\\{");
    pattern = CTools::Replace(pattern, "\\}", "\\}");
    out = CTools::Replace(out, pattern, replacedBy);
  }

}

//----------------------------------------
double CTools::DistanceOnUnitSphere(double lat1, double long1, double lat2, double long2)
{

  // Convert latitude and longitude to 
  // spherical coordinates in radians.
  
  // phi = 90 - latitude
  double phi1 = CTools::Deg2Rad(90.0 - lat1);
  double phi2 = CTools::Deg2Rad(90.0 - lat2);
        
  // theta = longitude
  double theta1 = CTools::Deg2Rad(long1);
  double theta2 = CTools::Deg2Rad(long2);
        
  // Compute spherical distance from spherical coordinates.
  //      
  // For two locations in spherical coordinates 
  // (1, theta, phi) and (1, theta, phi)
  // cosine( arc length ) = 
  //    sin phi sin phi' cos(theta-theta') + cos phi cos phi'
  // distance = rho * arc length
    
  double cos = (CTools::Sin(phi1) * CTools::Sin(phi2) * CTools::Cos(theta1 - theta2) + 
                CTools::Cos(phi1) * CTools::Cos(phi2));
  double arc = CTools::ACos( cos );

  // Remember to multiply arc by the radius of the earth 
  // in your favorite set of units to get length.
  return arc;
}

//----------------------------------------
double CTools::DistanceKmOnUnitSphere(double lat1, double long1, double lat2, double long2)
{
  return CTools::DistanceOnUnitSphere(lat1, long1, lat2, long2) * 6373;
}

//----------------------------------------
//double CTools::ExecZ(const char* function, const char* fmt, ...)
//{
//const char *p;
//int32_t i;
//uint8_t c;
//double f;
//const char *s;
//CBratObject* o = NULL;
//
////char fmtbuf[256];
//
//va_list args;
//
//va_start(args, fmt);
//
//for(p = fmt; *p != '\0'; p++)
//	{
//	if(*p != '%')
//		{
//    cout << *p << endl;
//		continue;
//		}
//
//	switch(*++p)
//		{
//		case 'c':
//			c = va_arg(args, uint8_t);
//      cout << c << endl;
//			break;
//
//		case 'd':
//			i = va_arg(args, int32_t);
//      cout << i << endl;
//			break;
//
//		case 'f':
//			f = va_arg(args, double);
//      cout << f << endl;
//			break;
//
//		case 's':
//			s = va_arg(args, const char *);
//      cout << s << endl;
//			break;
//
//		case 'o':
// 			o = va_arg(args, CBratObject *);
//      o->Dump(cout);
//      cout << o << endl;
//			break;
//
//		//case 'x':
//		//	i = va_arg(argp, int);
//		//	s = itoa(i, fmtbuf, 16);
//		//	fputs(s, stdout);
//		//	break;
//
//		case '%':
//			putchar('%');
//			break;
//
//    default:
//      cout << "unrecognized format:'" << *(p-1) << *p << "'" << endl;
//      return 0.0;
//
//		}
//	}
//
//va_end(args);
//
//
//  return 0.0;
//}



//----------------------------------------
//void CTools::VaListToTypeUnion(const char* fmt, va_list args, CVectorTypeUnion& typeUnions)
//{
//const char *p;
//int32_t i;
//uint8_t c;
//double f;
//double* pd;
//const char *s;
////CBratObject* o = NULL;
//
//
//type_union typeUnion;
//
//for(p = fmt; *p != '\0'; p++)
//	{
//	if(*p != '%')
//		{
//    //cout << *p << endl;
//		continue;
//		}
//
//
//	switch(*++p)
//		{
//		case 'c':
//			c = va_arg(args, VA_CHAR);
//      //cout << c << endl;
//      typeUnion = CTools::c2u(c);
//      typeUnions.push_back(typeUnion);
//			break;
//
//		case 'd':
//			i = va_arg(args, int32_t);
//      //cout << i << endl;
//      typeUnion = CTools::d2u(i);
//      typeUnions.push_back(typeUnion);
//			break;
//
//		case 'f':
//			f = va_arg(args, VA_FLOAT);
//      //cout << f << endl;
//      typeUnion = CTools::d2u(f);
//      typeUnions.push_back(typeUnion);
//			break;
//
//		case 'p':
//			pd = va_arg(args, double*);
//      cout << pd << endl;
////      cout << static_cast<double>(*pd) << endl;
//      typeUnion = CTools::d_ptr2u(pd);
//      typeUnions.push_back(typeUnion);
//			break;
//
//		case 's':
//			s = va_arg(args, const char *);
//      //cout << s << endl;
//      typeUnion= CTools::s2u(s);
//      typeUnions.push_back(typeUnion);
//			break;
//
//		//case 'o':
// 	//		o = va_arg(args, CBratObject *);
//  //    o->Dump(cout);
//  //    cout << o << endl;
//		//	break;
//
//		//case 'x':
//		//	i = va_arg(argp, int);
//		//	s = itoa(i, fmtbuf, 16);
//		//	fputs(s, stdout);
//		//	break;
//
//		//case '%':
//		//	putchar('%');
//		//	break;
//
//    default:
//      throw CException(CTools::Format("CTools::VaListToTypeUnion: unrecognized format code '%c' in the parameter format '%s'", static_cast<char>(*(p-1)), fmt), BRATHL_LOGIC_ERROR);
//
//		}
//	}
//
//  va_end(args);
//
//  
//}
////----------------------------------------
//double CTools::Exec(const char* function, const char *fmt, const type_union *arg)
//{
//  va_list out;
//  
//  const int32_t STATIC_ARG_TBL_SIZE = 100;
//  double aligned[STATIC_ARG_TBL_SIZE]; /* aligned buffer of ridiculously large size */
//  memset (aligned, 0, STATIC_ARG_TBL_SIZE);
//
//
//  va_start_assign(out, aligned[0]);
//
//  int i;
//  for(i = 0; arg[i].t != T_END; i++)
//  {
//    switch(arg[i].t) 
//    {
//    case T_INT:
//      va_assign(out, int32_t, arg[i].i);
//      break;
//    case T_FLOAT:
//      va_assign(out, VA_FLOAT, arg[i].f);
//      break;
//    case T_DOUBLE:
//      va_assign(out, double, arg[i].d);
//      break;
//    case T_CHAR:
//      va_assign(out, VA_CHAR, arg[i].c);
//      break;
//    case T_STRING:
//      va_assign(out, const char *, arg[i].s);
//      break;
//    case T_DOUBLE_PTR:
//      va_assign(out, double *, arg[i].d_ptr);
//      break;
//    default:
//      throw CException(CTools::Format("CTools::Exec: unrecognized format code '%d' in the parameter format of CTools::Exec method", arg[i].t), BRATHL_LOGIC_ERROR);
//    }
//  }
//
//  va_end(out);
//
//  va_start_assign(out, aligned[0]);
//
//  double returnedValue = CTools::Exec(function, fmt, out);
//  va_end(out);
//
//  return returnedValue;
//}

//----------------------------------------
//void CTools::TypeUnionToArray(const type_union *arg, double* aligned, uint32_t alignedLen)
//{
//  va_list out;
//
//  va_start_assign(out, aligned[0]);
//
//  uint32_t i;
//  for(i = 0; arg[i].t != T_END; i++)
//  {
//    if (i >= alignedLen)
//    {
//      break;
//    }
//
//    switch(arg[i].t) 
//    {
//    case T_INT:
//      va_assign(out, int, arg[i].i);
//      break;
//    case T_FLOAT:
//      va_assign(out, VA_FLOAT, arg[i].f);
//      break;
//    case T_DOUBLE:
//      va_assign(out, double, arg[i].d);
//      break;
//    case T_CHAR:
//      va_assign(out, VA_CHAR, arg[i].c);
//      break;
//    case T_STRING:
//      va_assign(out, const char *, arg[i].s);
//      break;
//    case T_DOUBLE_PTR:
//      va_assign(out, double *, arg[i].d_ptr);
//      break;
//    default:
//      throw CException(CTools::Format("unrecognized format code '%d' in the parameter format of CTools::Exec method", arg[i].t), BRATHL_LOGIC_ERROR);
//    }
//  }
//
//  va_end(out);
//}


//-------------------------------------------------------------
//------------------- CVectorTypeUnion class --------------------
//-------------------------------------------------------------

//----------------------------------------
//void CVectorTypeUnion::Dump(ostream& fOut /* = cerr */)
//{
//
// if (CTrace::IsTrace() == false)
//  {
//    return;
//  }
//
//  CVectorTypeUnion::const_iterator it;
//  int i = 0;
//
//  fOut << "==> Dump a CVectorTypeUnion Object at "<< this << " with " <<  size() << " elements" << endl;
//
//  for ( it = this->begin( ); it != this->end( ); it++ )
//  {
//    fOut << "CVectorTypeUnion[" << i << "]= ";  
//    type_union typeUnion = *it;
//
//    switch(typeUnion.t) 
//    {
//    case T_INT:
//      fOut << typeUnion.i << endl;
//      break;
//    case T_FLOAT:
//      fOut << typeUnion.f << endl;
//      break;
//    case T_DOUBLE:
//      fOut << typeUnion.d << endl;
//      break;
//    case T_CHAR:
//      fOut << typeUnion.c << endl;
//      break;
//    case T_STRING:
//      fOut << typeUnion.s << endl;
//      break;
//    case T_DOUBLE_PTR:
//      fOut << typeUnion.d_ptr << endl;
//      break;
//    default:
//      fOut << CTools::Format("unrecognized format code '%d' in the parameter format of CTools::Exec method", typeUnion.t) << endl;
//    }
//    i++;
//  }
//
//  fOut << "==> END Dump a CVectorTypeUnion Object at "<< this << " with " <<  size() << " elements" << endl;
//
//
//  fOut << endl;
//
//}

} // end namespace
