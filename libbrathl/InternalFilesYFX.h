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
#ifndef _InternalFilesYFX_h_
#define _InternalFilesYFX_h_

#include "Expression.h"
#include "Stl.h"

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


class CInternalFilesYFX : public CInternalFiles
{
public:
  CInternalFilesYFX(string 		Name = "", brathl_FileMode	Mode = ReadOnly);
  virtual ~CInternalFilesYFX();

  static string TypeOf() { return YFX_NETCDF_TYPE; };

  virtual string GetType
		();

  virtual void CreateDim
		(NetCDFVarKind		Kind,
		 const string		&XName,
		 const CExpressionValue	&Values,
		 const string		&Units,
		 const string		&LongName,
                 const string           &Comment = "",
      	         double	                ValidMin = CTools::m_defaultValueDOUBLE,
		 double	                ValidMax = CTools::m_defaultValueDOUBLE);

  virtual void CreateData
		(const string		&Name,
		 const string		&Units,
		 const string		&LongName,
                 const string           &Comment = "",
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
