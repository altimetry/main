/*
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
#include "stdafx.h"

#if defined (__APPLE__)
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

#include "../QtUtilsIO.h"
#include "../QtUtils.h"
#include "ApplicationUserPaths.h"



////////////////////////////////////////////////////////////////////////////////////////////
//
//									CApplicationUserPaths
//
////////////////////////////////////////////////////////////////////////////////////////////


CApplicationUserPaths::CApplicationUserPaths( const std::string &exec_path, const std::string &app_name ) 
	: base_t( exec_path, app_name )
{
    ValidatePaths();
}


//virtual
std::string CApplicationUserPaths::ToString() const
{
	std::string s = base_t::ToString();

    // s += ...

    return s;
}


std::string CApplicationUserPaths::DefaultUserDocumentsPath() const
{
    return ::SystemUserDocumentsPath() + "/" + mApplicationName;
}


std::string CApplicationUserPaths::DefaultUserDataPath4Application( const std::string&application_name, bool create ) const        //create = true
{
    std::string data = ::SystemUserDataPath();  //"/Users/brat/Library/Application Support/ESA"
    if ( !EndsWith( data, application_name ) )
    {
        if ( EndsWith( data, mApplicationName ) )
            data = GetDirectoryFromPath( data );

        data += ( "/" + application_name );
    }

    if ( create && !IsDir( data ) && !MakeDirectory( data ) )
    {
        mValid = false;
        mErrorMsg += ( "\nCould not create " +  data );
    }

    return data;
}
