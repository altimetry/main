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
#if !defined (COMMON_VERSION_H)
#define COMMON_VERSION_H


#include <string>

#include "ccore-types.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												Versioning 
//
//										VERSION_STRING and VERSION_NUMBER
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Versioning Policy:
//
//	- In final (production) versions, versioning follows the pattern Major.Minor.Patch (M.m.p). 
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

// Versioning implementation:
//
//	The version is given by the macros VERSION_STRING (for display) and VERSION_NUMBER (for numeric comparisons)
//	For a correct definition of these macros:
//
// - #define MAJOR_VERSION with an unsigned integer value
// - #define MINOR_VERSION with an unsigned integer value
// - #define PATCH_VERSION with an unsigned integer value
// - #define BETA_VERSION with empty value if a version is beta; otherwise don't #define it at all
// - #define RC_VERSION with an unsigned integer value >0 if a version is a release candidate; otherwise don't 
//		#define it at all or #define it with 0 value
//
//	Whenever a version M.m.p starts being developed:
//	- the 1st 3 macros must be defined outside the source code by the development settings and CMakeLists.txt files;
//		these files can read the values from a centralized source, version.txt, distributed with the source code;
//	- the alternative to this is having the 3 macros defined (hard-coded) in Version.h and in CMakeLists.txt.
//	- These macro values are available in version.txt (or hard-coded in Version.h and CMakeLists.txt) so that builds
//		(with CMake or programing tools) are possible without requiring the end user/developer to define them;
//	- these macros can be overriden from the command line (and so by any development tool that supports preprocessor 
//		definitions); but, in a standard sequence of releases, there is no overriding of these macros. Also, the
//		existence of version files such as version.txt supersedes preprocessor definitions from the command line.
//		(that is, development settings and CMakeLists.txt files will always use first the information read from files,
//		if they exist).
//

#if !defined(MAJOR_VERSION)
#error MAJOR_VERSION must be defined
#endif
#if !defined(MINOR_VERSION)
#error MINOR_VERSION must be defined
#endif
#if !defined(PATCH_VERSION)
#error PATCH_VERSION must be defined
#endif

// BETA_VERSION and RC_VERSION
//	- are always defined outside Version.h and CMakeLists.txt
//	- must never be hard-coded in Version.h and CMakeLists.txt: development settings and CMakeLists.txt files must 
//		always define them (possibly reading files such as in the version.txt case) for beta and/or RC releases.
//  - BETA_VERSION should be removed when beta testing is over, that is, when the last beta-rcX is accepted
//  - RC_VERSION should be incremented whenever the current RC is delivered
//	- Version.h uses these macros if correctly defined (externally) as preprocessor macros
//	- CMakeLists.txt uses these macros if correctly defined as CMake macros, using them to build the version for CMake use 
//		and to define the preprocessor macros for Version.h use.


#define STRINGYFY( M )								#M
#define MAKE_BASE_VERSION_STRING( M, m, p )	STRINGYFY(M.m.p)

#define BASE_VERSION_STRING	MAKE_BASE_VERSION_STRING(MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION)


#if defined BETA_VERSION
#define VERSION_BETA_STRING "-beta"
#define VERSION_BETA_INT 0				//see (*)
#else
#define VERSION_BETA_STRING ""
#define VERSION_BETA_INT 1				//see (*)
#endif

#define FULL_VERSION_WITH_RC(rc)	BASE_VERSION_STRING VERSION_BETA_STRING "-rc" STRINGYFY(rc)


#if defined(RC_VERSION)
#if RC_VERSION <= 0
#error RC_VERSION must have value > 0
#endif
#endif

#if RC_VERSION
#define VERSION_STRING	FULL_VERSION_WITH_RC(RC_VERSION)
#else
#define VERSION_STRING	BASE_VERSION_STRING VERSION_BETA_STRING
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
// (*) If later this number includes beta or RC, VERSION_BETA_INT can be used as defined above, and
//	RC_VERSION can be used as is, since it is already a numeric value
//
#if defined(QT_VERSION_CHECK)
#define VERSION_NUMBER	QT_VERSION_CHECK(MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION)
#else
#define VERSION_NUMBER	((MAJOR_VERSION<<16)|(MINOR_VERSION<<8)|(PATCH_VERSION))
#endif




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//							Other Release Identification Properties (not part of version)
//								- architecture, configuration, character encoding
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


auto static const PROCESSOR_ARCH =

#if defined(ARCHITECTURE_64)

	"64 bit";

#elif defined(ARCHITECTURE_32)

	"32 bit";

#else
#error One of ARCHITECTURE_32 or ARCHITECTURE_64 must be defined
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




#endif  //COMMON_VERSION_H
