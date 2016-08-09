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
#if !defined COMMON_GUI_APPLICATION_USER_PATHS_H
#define COMMON_GUI_APPLICATION_USER_PATHS_H

#include "new-gui/Common/ApplicationStaticPaths.h"




///////////////////////////////////////////////////////////////////////////
//				Application Paths for Desktop Applications
///////////////////////////////////////////////////////////////////////////




class CApplicationUserPaths : public CApplicationStaticPaths
{
    ////////////////////////////////////////////
    //	types & friends
    ////////////////////////////////////////////


	using base_t = CApplicationStaticPaths;

	friend class CBratSettings;


    ////////////////////////////////////////////
    //	static members
    ////////////////////////////////////////////

    ////////////////////////////////////////////
    //	instance data
    ////////////////////////////////////////////

protected:

    ////////////////////////////////////////////
    //	construction / destruction
    ////////////////////////////////////////////

public:
    explicit CApplicationUserPaths( const std::string &exec_path, const std::string &app_name );


    virtual ~CApplicationUserPaths()
    {}


    ////////////////////////////////////////////
    //	access (getters/setters/testers)
    ////////////////////////////////////////////

public:

    std::string DefaultUserDocumentsPath() const;


    std::string DefaultUserDataPath4Application( const std::string&application_name, bool create = true ) const;


	std::string DefaultUserDataPath( bool create = true ) const
	{
		return DefaultUserDataPath4Application( mApplicationName, create );
	}


    ////////////////////////////////////////////
    //	remaining member functions
    ////////////////////////////////////////////

	virtual std::string ToString() const;
};






#endif	//COMMON_GUI_APPLICATION_USER_PATHS_H
