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
#ifndef _Unit_h_
#define _Unit_h_

#include <cstdarg>

#include "brathl_error.h"
#include "brathl.h"
#include "BratObject.h"
#include "List.h"
#include "Tools.h"
#include "Date.h"
using namespace brathl;

extern "C" {
#include "udunits.h"
}

namespace brathl
{

/** \addtogroup tools Tools
  @{ */

/**
  Units management classes.



 \version 1.0
*/
class CUnitSmartCleaner;

class CUnit : public CBratObject
{
public:
  CUnit	(const std::string	&Text = "");
  //CUnit	(const char*	Text);
  CUnit (const CUnit	&Copy);
  virtual ~CUnit();

  CUnit& operator= (const std::string	&Text);
  //CUnit& operator= (const char*	text);
  CUnit& operator= (const CUnit	&Source);
  
  bool operator==(CUnit& u);
  bool operator==(const std::string& text);
  bool operator!= ( CUnit& u ) { return ! (*this == u); }
  bool operator!= ( const std::string& text ) { return ! (*this == text); }

  bool HasUnit();
  bool HasNoUnit();

  static CUnit ToUnit(const std::string& unitStr);

  CUnit BaseUnit
		();

  //void Set(const char* text);
  void Set(const std::string& text);
  void Set(const CUnit& u);

  void SetConversionTo
		(const CUnit	&Destination);
  void SetConversionTo
		(const std::string	&Destination);
  void SetConversionFrom
		(const CUnit	&Source);
  void SetConversionFrom
		(const std::string	&Source);

  void SetConversionFromBaseUnit();
  void SetConversionToBaseUnit();


  double ConvertToUnit(const double	FromValue, double defaultValue = CTools::m_defaultValueDOUBLE);
  static double ConvertToUnit(CUnit* unitIn, const double	fromValue, double defaultValue = CTools::m_defaultValueDOUBLE);
  double ConvertToBaseUnit(const double	FromValue, double defaultValue = CTools::m_defaultValueDOUBLE);
  static double ConvertToBaseUnit(CUnit* unitIn, const double	fromValue, double defaultValue = CTools::m_defaultValueDOUBLE);

  double Convert(const double	fromValue);
  void ConvertVector(std::vector<double>	&vect);

  void ConvertVector(double* vect, int32_t size);

  bool IsDate() const;

  bool IsCompatible(const std::string& otherUnit, std::string* errorMsg = NULL) const;
  bool IsCompatible(const CUnit& otherUnit) const;
  bool IsCompatible(const CUnit* otherUnit) const;
 
  
  std::string AsString(bool asBaseUnit = true, bool withDateOrigin = false, CDate* dateRef = NULL) const;

  bool HasDateRef(CDate* dateRef = NULL, CStringArray* array = NULL) const;
  std::string GetDateUnitWithoutDateOrigin() const;

  std::string GetDateRefAsString(CDate* dateRef = NULL) const;

  std::string GetDateRefAs1950() const;

  std::string GetText() const { return m_Text; };
  double GetSlope() const { return m_Slope; };
  double GetOffset() const { return m_Offset; };
  void Dump
		(std::ostream	&fOut  = std::cerr);

// Clear the error from initialization
  void ClearInitError
		();

  static void CheckUdunits
		(uint32_t	Code,
		 const std::string	&Msg1	= "",
		 const std::string	&Msg2	= "");

private:
  static void InitializeUnitSystem();

  void Compile
		();
  void FindBaseUnit
		(utUnit		&CompiledUnit);


public:
  static CDate m_dateRefUdunits;
  static const std::string m_DATE_REF_UNIT;
  static const std::string m_UNIT_SI;

private:
  std::string	m_Text;
  utUnit	m_Compiled;
  double	m_Slope;
  double	m_Offset;

  static std::string	m_ErrInit;
  static bool m_initialized;
};



class CUnitSmartCleaner
{
public :
    CUnitSmartCleaner();
    ~CUnitSmartCleaner();

private :
  int m_dummy;
};


/** @} */

}
#endif // !defined(_Unit_h_)
