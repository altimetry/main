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

#if !defined (COMMON_BRAT_VERSION_H)
#define COMMON_BRAT_VERSION_H


#if !defined(BRAT_MAJOR_VERSION_INT)
#define BRAT_MAJOR_VERSION_INT			4
#endif
#if !defined(BRAT_MINOR_VERSION_INT)
#define BRAT_MINOR_VERSION_INT			0
#endif
#if !defined(BRAT_PATCH_VERSION_INT)
#define BRAT_PATCH_VERSION_INT			0
#endif


#define STRINGYFY( M )		#M

#define MAKE_BRAT_VERSION_STRING( M, m, p )		STRINGYFY(M.m.p)


#define BRAT_VERSION	MAKE_BRAT_VERSION_STRING(BRAT_MAJOR_VERSION_INT, BRAT_MINOR_VERSION_INT, BRAT_PATCH_VERSION_INT)



auto static const PROCESSOR_ARCH =

#if defined(BRAT_ARCHITECTURE_64)

	"64 bit";

#elif defined(BRAT_ARCHITECTURE_32)

	"32 bit";

#else
#error One of BRAT_ARCHITECTURE_32 or BRAT_ARCHITECTURE_64 must be defined
#endif


static const std::string build_platform( " " + std::string( PROCESSOR_ARCH ) );



#if defined(_UNICODE) || defined(UNICODE)
    static const std::string build_charset( " Un" );
#else
	static const std::string build_charset( "" );
#endif


#if defined (DEBUG) || defined(_DEBUG)
    static const std::string build_configuration = build_platform + " [DEBUG] " + build_charset;
#else
	static const std::string build_configuration = build_platform + build_charset;
#endif




#endif  //COMMON_BRAT_VERSION_H
