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

#include <string>

#include "brathl.h"

#include "Tools.h"
#include "Date.h"
#include "Exception.h"
#include "Expression.h"

#include "InternalFilesZFXY.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

namespace brathl
{


//----------------------------------------


std::string CInternalFilesZFXY::GetType()
{
  return TypeOf();
}
//----------------------------------------

bool CInternalFilesZFXY::IsGeographic()
{
  return m_file.IsGeographic();
}
//----------------------------------------

void CInternalFilesZFXY::CreateDim
		(NetCDFVarKind		Kind,
		 const std::string		&XName,
		 const CExpressionValue	&Values,
		 const std::string		&Units,
		 const std::string		&LongName,
                 const std::string           &Comment /* = ""*/,
      	         double	                ValidMin /* = CTools::m_defaultValueDOUBLE */,
		 double	                ValidMax /* = CTools::m_defaultValueDOUBLE */)
{
  if (m_file.GetNbDimensions() >= 2)
    throw CFileException(CTools::Format("Only two dimensions is allowed in %s file type, not %d",
				        GetType().c_str(),
					m_file.GetNbDimensions()),
			 GetName(),
			 BRATHL_LOGIC_ERROR);
  m_file.SetDimension(Kind, XName, Values.GetNbValues(), Values, Units, LongName, Comment, ValidMin, ValidMax);
}

//----------------------------------------

void CInternalFilesZFXY::CreateData
		(const std::string		&Name,
		 const std::string		&Units,
		 const std::string		&LongName,
		 const std::string		&Dim1Name,
		 const std::string		&Dim2Name,
                 const std::string           &Comment /* = ""*/,
      	         double	                ValidMin /* = CTools::m_defaultValueDOUBLE */,
		 double	                ValidMax /* = CTools::m_defaultValueDOUBLE */,
		 nc_type		Type	/*= NC_DOUBLE*/)
{
  if (VarExists(Name))
  {
    throw CFileException(CTools::Format("Try to create data variable '%s' while it already exists",
	                              				Name.c_str()),
		                                    GetName(),
  		                                  BRATHL_LOGIC_ERROR);
  }
  
  if (m_file.GetNbDimensions() != 2)
  {
    throw CFileException(CTools::Format("Try to create data variable '%s' while not only two dimensions exist",
					                              Name.c_str()),
			                                  GetName(),
			                                  BRATHL_LOGIC_ERROR);
  }
 
  CUnit unit = CUnit::ToUnit(Units);
  std::string strUnit = unit.AsString(false, true);


  m_file.AddVar(Name,
	            Type,
	            strUnit,
	            LongName,
              Comment,
	            m_file.GetDimId(Dim1Name),
	            m_file.GetDimId(Dim2Name),
              -1,
              -1,
              ValidMin,
              ValidMax);
}




}
