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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//											BRAT Versioning 
//
//								BRAT_VERSION_STRING and BRAT_VERSION_INT
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Versioning Policy:
//
//	- In final (production) versions, BRAT versioning follows the pattern Major.Minor.Patch (M.m.p). 
//	- This is called the base version. Final versions are always base versions. 
//	- Suffixes are added to base versions for all releases since the 1st from development to the final one, where 
//		the suffix is dropped; so, all test/unstable releases have a suffix that identifies that release in the 
//		sequence of releases from development to final:
//		- In beta versions, the suffix -beta is added to the base version.
//		- In release candidate versions, either beta or final, the "-rcX" suffix is added, where X is a number > 0.
//
// Beta and Release Candidates:
//
//	After the 1st release of an M.m.p version from the development team: 
//
//	- the 1st version released for internal tests is M.m.p-beta-rc1;
//	- other M.m.p-beta-rcX versions follow, each including the solution of problems found in the previous M.m.p-beta-rcX.
//
//	- The 1st M.m.p version released for external (client) tests is M.m.p-beta, which is equal to the last M.m.p-beta-rcX;
//	- starting with M.m.p-rc1, versions M.m.p-rcX follow M.m.p-beta, each one including the corrections of problems found 
//		by the client in the previous M.m.p-rcX.
//	- The final M.m.p version is reached, and equal to the last M.m.p-rcX, when no more corrections are required.
//

// BRAT versioning implementation:
//
//	The BRAT version is given by the macros BRAT_VERSION_STRING and BRAT_VERSION_INT
//	For a correct definition of these macros:
//
// - #define BRAT_MAJOR_VERSION with an unsigned integer value
// - #define BRAT_MINOR_VERSION with an unsigned integer value
// - #define BRAT_PATCH_VERSION with an unsigned integer value
// - #define BRAT_BETA_VERSION with empty value if a version is beta; otherwise don't #define it at all
// - #define BRAT_RC_VERSION with an unsigned integer value >0 if a version is a release candidate; otherwise don't 
//		#define it at all or #define it with 0 value
//
// The 1st 3 macros should be defined (hard-coded) in BratVersion.h and in CMakeLists.txt whenever a version M.m.p 
//		starts being developed
//	- these macros are always hard-coded so that builds (with CMake or programing tools) are possible without 
//		requiring the final user to define them
//	- these macros can be overriden from the command line (and so by any development tool that supports preprocessor 
//		definitions); but, in a standard sequence of releases, there is no overriding of these macros.
//

#if !defined(BRAT_MAJOR_VERSION)
#define BRAT_MAJOR_VERSION			4
#endif
#if !defined(BRAT_MINOR_VERSION)
#define BRAT_MINOR_VERSION			1
#endif
#if !defined(BRAT_PATCH_VERSION)
#define BRAT_PATCH_VERSION			0
#endif

// BRAT_BETA_VERSION and BRAT_RC_VERSION
//	- are always defined outside BratVersion.h and CMakeLists.txt
//	- are never hard-coded: the command line or development tools must always define them for beta and/or RC releases
//  - BRAT_BETA_VERSION should be removed when beta testing is over, that is, when the last beta-rcX is accepted
//  - BRAT_RC_VERSION should be incremented whenever the current RC is delivered
//	- BratVersion.h uses these macros if correctly defined (externally) as preprocessor macros
//	- CMakeLists.txt uses these macros if correctly defined as CMake macros, using them to build the version for CMake use 
//		and to define the preprocessor macros for BratVersion.h use.


#define STRINGYFY( M )								#M
#define MAKE_BRAT_BASE_VERSION_STRING( M, m, p )	STRINGYFY(M.m.p)

#define BRAT_BASE_VERSION_STRING	MAKE_BRAT_BASE_VERSION_STRING(BRAT_MAJOR_VERSION, BRAT_MINOR_VERSION, BRAT_PATCH_VERSION)


#if defined BRAT_BETA_VERSION
#define BRAT_VERSION_BETA_STRING "-beta"
#define BRAT_VERSION_BETA_INT 0				//see (*)
#else
#define BRAT_VERSION_BETA_STRING ""
#define BRAT_VERSION_BETA_INT 1				//see (*)
#endif

#define BRAT_FULL_VERSION_WITH_RC(rc)	BRAT_BASE_VERSION_STRING BRAT_VERSION_BETA_STRING "-rc" STRINGYFY(rc)


#if defined(BRAT_RC_VERSION)
#if BRAT_RC_VERSION <= 0
#error BRAT_RC_VERSION must have value > 0
#endif
#endif

#if BRAT_RC_VERSION
#define BRAT_VERSION_STRING	BRAT_FULL_VERSION_WITH_RC(BRAT_RC_VERSION)
#else
#define BRAT_VERSION_STRING	BRAT_BASE_VERSION_STRING BRAT_VERSION_BETA_STRING
#endif



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Version as Numeric Value
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This is the base version as a numeric value, suitable for numeric comparisons
//	(can be used to test for the existence of features version dependent)
//
// This value cannot be used to compare beta or release candidate versions, which are not
//	considered to build the numeric value. Features are usually not dependent of a version 
//	being beta or RC.
//
// The hexadecimal display of this number suggests the M.m.p pattern in
//	decimal format. For example, 4.1.0 is 0x040100 (262400)
//
// (*) If later this number includes beta or RC, BRAT_VERSION_BETA_INT can be used as defined above, and
//	BRAT_RC_VERSION can be used as is, since it is already a numeric value
//
#if defined(QT_VERSION_CHECK)
#define BRAT_VERSION_INT	QT_VERSION_CHECK(BRAT_MAJOR_VERSION, BRAT_MINOR_VERSION, BRAT_PATCH_VERSION)
#else
#define BRAT_VERSION_INT	((BRAT_MAJOR_VERSION<<16)|(BRAT_MINOR_VERSION<<8)|(BRAT_PATCH_VERSION))
#endif




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//							Other Release Identification Properties (not part of version)
//							- architecture, configuration, character encoding
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
