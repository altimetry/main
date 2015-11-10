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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#if !defined(_Parameter_h_)
#define _Parameter_h_

#include "Date.h"
#include "Expression.h"
#include <string>
#include "brathl.h"
#include "brathl_error.h"
#include "List.h"

using namespace brathl;

namespace brathl
{


/** \addtogroup parameters Parameters
  @{ */

/**
  Parameter management class.

  One parameter can have 1 to n value.

  This class stands for parameters

 \version 1.0
*/



class CParameter
{

public:

  struct KWValueListEntry {
	const char	*Name;
	uint32_t	Value;
	};

  /// Empty CParameter ctor
  CParameter();

  /** Creates a new CParameter object.
    \param keyword [in] : parameter name */
  CParameter(const char *keyword);
  //@{
  /** Creates a new CParameter object.
    \param keyword [in] : parameter name
    \param value [in] : parameter value */
  CParameter(const char *keyword, const char *value);
  CParameter(const std::string& keyword, const std::string& value);
  //@}

  /// Destructor
  virtual ~CParameter();

  //@{
  /** Adds a value to the CParameter object.
    \param value [in] : parameter value */
  void AddValue(const char *value);
  void AddValue(const std::string& value);
  //@}


  /** Removes a value at a given position.
      The first value is at the index 0.
    \param i [in] : index value to remove*/
  bool RemoveValue(uint32_t i);

  /** Removes all values. */
  bool RemoveAllValue();

  /** \return the number of values. */
  uint32_t Count();

  /** Register the formulas aliases defined.
    \param Aliases [in] : Names/values of aliases
  */
  void SetAliases(const CStringMap& aliases);

  //@{
  /** gets a CParameter object value at a given position
    If the list of values is empty or index pos is out of range a CParameterException is raised.
    \param value [out] : parameter value
    \param pos [in] : position of the parameter 0.. n (default is 0, first value)*/
  void GetValue(int32_t& value, int32_t pos = 0, int32_t DefValue = CTools::m_defaultValueINT32);
  void GetValue(uint32_t& value, int32_t pos = 0, uint32_t DefValue = CTools::m_defaultValueUINT32);
  void GetValue(double& value, int32_t pos = 0, double DefValue = CTools::m_defaultValueDOUBLE);
  void GetValue(bool& value, int32_t pos = 0, bool DefValue = false);
  
  void GetValue(CDate& value, int32_t pos = 0);
  void GetValue(CDate& value, CUnit& unit, int32_t pos = 0);
  void GetValue(CDate& value, const std::string& strUnit, int32_t pos = 0);
  void GetValue(CDate& value, CUnit* unit, int32_t pos = 0);

  void GetValue(std::string& value, int32_t pos  = 0, const std::string &DefValue = "");
  void GetValue(CExpression &value, int32_t pos = 0);
  void GetValue(CUnit &value, int32_t pos = 0, const std::string &DefValue = "count");
  // One keyword from the list. Last element of list must have NULL as name
  void GetValue
		(uint32_t		&value,
		 std::string			&ValueName,
		 const KWValueListEntry	*KeywordList,
		 int32_t		pos		= 0,
		 uint32_t		DefValue	= CTools::m_defaultValueUINT32);
  // Set of keywords from the list. Last element of list must have NULL as name
  void GetValue
		(bitSet32		&value,
		 const KWValueListEntry	*KeywordList,
		 int32_t		pos		= 0,
		 const bitSet32 	&DefValue	= 0);

  void GetValue
		(uint32_t		&value,
		 std::string			&ValueName,
		 CUIntMap&	KeywordList,
		 int32_t		pos		/*= 0*/,
		 uint32_t		DefValue	/*= CTools::m_defaultValueUINT32*/);

  void GetAllValues(CExpression &value, const std::string &Combine = "&&");
  void GetAllValues(CStringList& listValues);
  void GetAllValues(CStringArray& listValues);

  //@}
  /** gets a CParameter object value at a given position
    If the list of values is empty or index pos is out of range a CParameterException is raised.
    WARNING : if size of std::string value is smaller than the size of the parameter value,
    data will be truncated
    \param value [out] : parameter value
    \param bufferSize [in] : size of value
    \param pos [in] : position of the parameter 0.. n (default is 0, first value)
    \return false if one can't get the value, otherwise true */
  void GetValue(char* value, int32_t bufferSize, int32_t pos  = 0, const char *DefValue = "");

   ///Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr);


private:


  void CommonCtrl(int32_t pos = 0);


public:



private:

  std::string m_keyword;
  //vector<std::string>m_values;
  CStringArray m_values;


};

/** @} */

}

#endif // !defined(_Parameter_h_)
