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
#ifndef _InternalFilesZFXY_h_
#define _InternalFilesZFXY_h_

#include <string>

#include "Expression.h"

#include "InternalFiles.h"

namespace brathl
{


/** \addtogroup tools Tools
  @{ */

/**
  Internal files access for internal files used to store
  Y=F(X) kind of data.



 \version 1.0
*/


class CInternalFilesZFXY : public CInternalFiles
{
public:
  CInternalFilesZFXY
		(std::string 		Name = "",
		 brathl_FileMode	Mode = ReadOnly)
	: CInternalFiles(Name, Mode)
  {};


  static std::string TypeOf() { return ZFXY_NETCDF_TYPE; };

  virtual std::string GetType
		();

  virtual bool IsGeographic
		();

  virtual void CreateDim
		(NetCDFVarKind		Kind,
		 const std::string		&XName,
		 const CExpressionValue	&Values,
		 const std::string		&Units,
		 const std::string		&LongName,
                 const std::string           &Comment = "",
      	         double	                ValidMin = CTools::m_defaultValueDOUBLE,
		 double	                ValidMax = CTools::m_defaultValueDOUBLE);

  virtual void CreateData
		(const std::string		&Name,
		 const std::string		&Units,
		 const std::string		&LongName,
		 const std::string		&Dim1Name,
		 const std::string		&Dim2Name,
                 const std::string           &Comment = "",
      	         double	                ValidMin = CTools::m_defaultValueDOUBLE,
		 double	                ValidMax = CTools::m_defaultValueDOUBLE,
		 nc_type		Type	= NC_DOUBLE);


protected:

private:

public:


};



/** @} */

}
#endif // Already included .h
