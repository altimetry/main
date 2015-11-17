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

#include "Exception.h"
#include "NetCDFFiles.h"
#include "InternalFilesYFX.h"
#include "InternalFilesZFXY.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

namespace brathl
{

#define CHECK_TYPE(Class)			\
	if (Class::TypeOf() == TypeStr)		\
	  return new Class(name)

CInternalFiles* BuildExistingInternalFileKind (const std::string& name, const CStringArray* fieldNames /* = NULL */)
{

  CNetCDFFiles netCdfFile(name);

  std::string TypeStr	= netCdfFile.IdentifyExistingFile();
  CHECK_TYPE(CInternalFilesYFX);
  CHECK_TYPE(CInternalFilesZFXY);

  if (netCdfFile.IsGeographic())
  {
    return new CInternalFilesZFXY(name);
  }

  uint32_t nbDims = netCdfFile.GetMaxFieldNumberOfDims(fieldNames);

  if (nbDims >= 2)
  {
    return new CInternalFilesZFXY(name);
  }

  if (nbDims >= 1)
  {
    return new CInternalFilesYFX(name);
  }


  throw CFileException(CTools::Format("Unknown internal netcdf file type '%s'",
				      TypeStr.c_str()),
		       name,
		       BRATHL_ERROR);
}


}
