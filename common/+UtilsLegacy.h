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
#ifndef BRAT_CROSS_PLATFORM_UTILS_LEGACY_H
#define BRAT_CROSS_PLATFORM_UTILS_LEGACY_H

#if defined (CROSS_PLATFORM_UTILS_LEGACY_H)
#error Wrong +UtilsLegacy.h included 
#endif

#ifndef __cplusplus
#error Must use C++ for +utils.h
#endif


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//					REFORMULATED BRAT UTILITIES
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

#if defined (WIN32) || defined (_WIN32)
#pragma warning ( disable : 4996 )		//_CRT_SECURE_NO_WARNINGS: "This function or variable may be unsafe"
#endif


inline std::string stdFormat( size_t size, const char *format, va_list args )
{
	std::string	result;
	char *str = nullptr;
	try
	{
		str = new char[ size ];
		vsnprintf( str, size, format, args );	//_vsnprintf(str, size, format, args );		//vsprintf( str, format, args );
		result	= str;
	}
	catch ( ... )
	{
		delete[] str;
		throw;
	}
	delete[]str;
	return result;
}


inline std::string stdFormat( const char *format, va_list args )
{
	return stdFormat( 4096, format, args );
}


#if defined (WIN32) || defined (_WIN32)
#pragma warning ( default : 4996 )
#endif





#endif  //BRAT_CROSS_PLATFORM_UTILS_LEGACY_H
