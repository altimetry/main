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
#include <cmath>
#include <string>

#include "brathl.h"

#include "Tools.h"
#include "Date.h"
#include "common/tools/Exception.h"
#include "Expression.h"

#include "ExternalFilesJason2.h"
// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"


using namespace brathl;



namespace brathl
{


//-------------------------------------------------------------
//------------------- CExternalFilesJason2 class --------------------
//-------------------------------------------------------------
const std::string  CExternalFilesJason2::smMissionName = CTools::StringToUpper( "Jason-2" );


CExternalFilesJason2::CExternalFilesJason2
		(const std::string		&name /*= ""*/)
	: CExternalFilesNetCDFCF(name)
{
}

//-------------------------------------------------------------
//------------------- CExternalFilesJason2SSHA class --------------------
//-------------------------------------------------------------
CExternalFilesJason2SSHA::CExternalFilesJason2SSHA
		(const std::string		&name /*= ""*/)
	: CExternalFilesJason2(name)
{
}

//-------------------------------------------------------------
//------------------- CExternalFilesJason2GDR class --------------------
//-------------------------------------------------------------

CExternalFilesJason2GDR::CExternalFilesJason2GDR
		(const std::string		&name /*= ""*/)
	: CExternalFilesJason2(name)
{
}


//-------------------------------------------------------------
//------------------- CExternalFilesJason2SGDR class --------------------
//-------------------------------------------------------------
CExternalFilesJason2SGDR::CExternalFilesJason2SGDR
		(const std::string		&name /*= ""*/)
	: CExternalFilesJason2(name)
{

}



} // end namespace

