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

#include "new-gui/Common/tools/Exception.h"
#include "NetCDFFiles.h"
#include "ExternalFiles.h"
#include "ExternalFilesATP.h"
#include "ExternalFilesAvisoGrid.h"
#include "ExternalFilesJason2.h"
// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;

namespace brathl
{

#define CHECK_TYPE(Class)			\
	if (Class::TypeOf() == TypeStr)		\
	  return new Class(Name)

CExternalFiles* BuildExistingExternalFileKind
		(const std::string		&Name)
{
  std::string TypeStr	= CNetCDFFiles::IdentifyExistingFile(Name, true, true);
  CHECK_TYPE(CExternalFilesATP);
  CHECK_TYPE(CExternalFilesDotGrid);
  CHECK_TYPE(CExternalFilesBoxGrid);
  CHECK_TYPE(CExternalFilesMercatorDotGrid);
  CHECK_TYPE(CExternalFilesMercatorBoxGrid);

  // Jason-2 SSHA dataset products
  CHECK_TYPE(CExternalFilesJason2SSHA);
//  CHECK_TYPE(CExternalFilesJason2SSHAOGDR);
//  CHECK_TYPE(CExternalFilesJason2SSHAIGDR);
//  CHECK_TYPE(CExternalFilesJason2SSHAGDR);
  
  // Jason-2 GDR dataset products
  CHECK_TYPE(CExternalFilesJason2GDR);
//  CHECK_TYPE(CExternalFilesJason2GDROGDR);
//  CHECK_TYPE(CExternalFilesJason2GDRIGDR);
//  CHECK_TYPE(CExternalFilesJason2GDRGDR);
  
  // Jason-2 SGDR dataset products
  CHECK_TYPE(CExternalFilesJason2SGDR);
//  CHECK_TYPE(CExternalFilesJason2SGDRIGDR);
//  CHECK_TYPE(CExternalFilesJason2SGDRGDR);
  
  // Brat Output file Y=F(X)
  CHECK_TYPE(CExternalFilesYFX);
  // Brat Output file Z=F(X,Y)
  CHECK_TYPE(CExternalFilesZFXY);

  // Generic Netcdf roducts
  CHECK_TYPE(CExternalFilesNetCDFCFGeneric);

  // Sentinel 3A
  CHECK_TYPE(CExternalFilesSentinel3A);

  CHECK_TYPE(CExternalFilesSentinel3A_enhanced);
  CHECK_TYPE(CExternalFilesSentinel3A_standard);
  CHECK_TYPE(CExternalFilesSentinel3A_reduced);

  CHECK_TYPE(CExternalFilesSentinel3A_l1b);
  CHECK_TYPE(CExternalFilesSentinel3A_l1a);
  CHECK_TYPE(CExternalFilesSentinel3A_l1bs);

  // Reaper
  CHECK_TYPE(CExternalFilesReaper);

  // Geosat GDR
  CHECK_TYPE(CExternalFilesGeosatGDR);

  return NULL;
}


}
