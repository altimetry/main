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
#ifndef QSCHEDULER_APPLICATION
#define QSCHEDULER_APPLICATION

#include "stdafx.h"


class CApplicationUserPaths;





class CSchedulerApplication : public QtSingleApplication
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = QtSingleApplication;


    // static members

    static const CApplicationUserPaths *smApplicationPaths;	//initialized in Prologue

	static bool smPrologueCalled;							//initialized in Prologue


public:

	// Calling this in main is mandatory
	//	
	static void Prologue( int argc, char *argv[] );


protected:

    // static member functions

public:

    static int OffGuiErrorDialog( int error_code, char const *error_msg );

    static const CApplicationUserPaths* ApplicationPaths()
    {
        return smApplicationPaths;
    }

private:

    // instance data members

    // Ctors / Dtor

public:
    CSchedulerApplication( int &argc, char **argv );

    virtual ~CSchedulerApplication();

protected slots:

};




#endif // QSCHEDULER_APPLICATION
