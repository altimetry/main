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
#if !defined RADS_SERVICE_LOGGER_H
#define RADS_SERVICE_LOGGER_H

#include "new-gui/Common/ApplicationLoggerBase.h"



class CRadsServiceLogger : public CApplicationLoggerBase
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types & friends

	using base_t = CApplicationLoggerBase;

	
	// static members

public:
	static CRadsServiceLogger& Instance();

	// instance data 

protected:

	// construction / destruction

	CRadsServiceLogger();

public:

	virtual ~CRadsServiceLogger();
};




#define LOG_INFO( msg ) log_info< CRadsServiceLogger >( msg )
#define LOG_WARN( msg ) log_warn< CRadsServiceLogger >( msg )
#define LOG_FATAL( msg ) log_fatal< CRadsServiceLogger >( msg )



#endif	//RADS_SERVICE_LOGGER_H
