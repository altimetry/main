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


#define BRAT_VERSION             "4.0.0-rc3"



#if defined(BRAT_ARCHITECTURE_64)

    static const std::string build_platform( " 64-bit" );

#elif defined(BRAT_ARCHITECTURE_32)

	static const std::string build_platform( " 32-bit" );
#else
#error One of BRAT_ARCHITECTURE_32 or BRAT_ARCHITECTURE_64 must be defined
#endif


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


auto static const PROCESSOR_ARCH =

#if defined(BRAT_ARCHITECTURE_64)
	"64 bit";
#else
	"32 bit";
#endif




#endif  //COMMON_BRAT_VERSION_H
