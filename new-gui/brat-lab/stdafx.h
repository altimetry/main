#if !defined(BRAT_STDAFX_H)
#define BRAT_STDAFX_H

#ifndef Q_MOC_RUN       //problems with boost 1_59_0 caused by qt moc
                        // see https://bugreports.qt.io/browse/QTBUG-22829


//#if !defined(_USE_MATH_DEFINES)
//#define _USE_MATH_DEFINES
//#endif
//#include <cmath> 

#include <cassert>
#include <sstream>
#include <complex>
#include <vector>
#include <fstream>
#include <functional> 


#if defined(WIN32)
//avoid later inclusion of Microsoft XML stuff, which causes name collisions with xerces
#define DOMDocument MsDOMDocument
#include <msxml.h>
#include <urlmon.h>
#undef DOMDocument
#endif		//WIN32

//About the App

//App name: xerces-c
//App description: Validating XML parser
//App website: https://xerces.apache.org/xerces-c/
//Install the App

//Press Command+F and type Terminal and press enter/return key.
//Run in Terminal app:
//ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" < /dev/null 2> /dev/null
//and press enter/return key. Wait for the command to finish.
//Run:
//brew install xerces-c
//Done! You can now use xerces-c.

//#include <xercesc/util/PlatformUtils.hpp>
//#include <xercesc/framework/LocalFileFormatTarget.hpp>


#if !defined(NDEBUG)
	#if !defined(DEBUG)
	#define DEBUG
	#endif
	#if !defined(_DEBUG)
	#define _DEBUG
	#endif
#endif

//look at the mess caused by including Python.h: scripters are obviously... special programmers
#if !defined(WIN32)                              //avoid redefinition warnings (lots of them) - begin
#define SAVE_XOPEN_SOURCE   _XOPEN_SOURCE
#define SAVE_POSIX_C_SOURCE _POSIX_C_SOURCE
#undef _XOPEN_SOURCE
#undef _POSIX_C_SOURCE
#endif
#include <patchlevel.h>
#if PY_VERSION_HEX < 0x03040300
#if defined(_MSC_VER) && (_MSC_VER >= 1800)
#define HAVE_ROUND
#endif
#endif
//#define Py_NO_ENABLE_SHARED	0
#if defined (_DEBUG)            //avoid automatic inclusion of pythonX_Y.lib (see J:\Python\default\include\pyconfig.h)
#undef _DEBUG
#include <Python.h>				//must come before Qt headers: has a struct with a member named "slots"...
#define _DEBUG
#else
#include <Python.h>				//must come before Qt headers: has a struct with a member named "slots"...
#endif
#if !defined(WIN32)                              //avoid redefinition warnings (lots of them) - end
#undef _XOPEN_SOURCE
#undef _POSIX_C_SOURCE
#define _XOPEN_SOURCE   SAVE_XOPEN_SOURCE
#define _POSIX_C_SOURCE SAVE_POSIX_C_SOURCE
#endif

#if defined HAVE_STAT			 	//Python also defines this
#define SAVE_HAVE_STAT	HAVE_STAT
#undef HAVE_STAT
#include <brathl_config.h>
#if defined HAVE_STAT && ( HAVE_STAT != SAVE_HAVE_STAT )	//Use Python value, which should be the same
#error HAVE_STAT Python value which should be the same
#endif
#endif




#include <QTimer>

//#include <boost/archive/iterators/mb_from_wchar.hpp>    //for UNICODE/MBCS literals
//#include <boost/archive/iterators/wchar_from_mb.hpp>    //for UNICODE/MBCS literals

//#include <boost/locale.hpp>                             //for utf8 <-> utf16 conversions
//
//#include "../include/gtree.h"

//#include <Libraries/+/+UtilsIO.h>	//includes +Utils.h
//#include <Libraries/+/QtUtils.h>
//#define _TCHAR_DEFINED				//this is a define test in winnt.h, which can issue collisions with TCHAR definitions below in +ExtendedKeyb.h if a windows.h header is included


#if defined (WIN32)
#include <windows.h>
#undef min
#undef max
#endif


//#include <Libraries/+/+ExtendedKeyb.h>
//
//#if defined (WIN32)
//#include <Libraries/lexcept.h>
//#else
//
//class BASE_Exception {
//protected:
//     unsigned int code;
//     const TCHAR *msg;
//public:
//    BASE_Exception(unsigned int _code, const TCHAR* _msg = NULL) : code(_code), msg(_msg) {}
//    BASE_Exception(const TCHAR* _msg, unsigned int _code = 0) : code(_code), msg(_msg) {}
//    BASE_Exception(): code(0), msg(NULL) {}
//    BASE_Exception(BASE_Exception& o): code(o.code), msg(o.msg) {}
//    unsigned int getCode() const { return code; }
//    const TCHAR* getMsg() const { return msg; }
//};
//
//class SE_Exception : public BASE_Exception {
//public:
//    SE_Exception(unsigned int _code, const TCHAR* _msg = NULL): BASE_Exception(_code, _msg) {}
//    SE_Exception(const TCHAR* _msg, unsigned int _code = 0): BASE_Exception (_msg, _code) {}
//    SE_Exception(): BASE_Exception() {}
//    SE_Exception(SE_Exception& o): BASE_Exception(o) {}
//};
//
//#endif

//#include <Libraries/+/QtUtils.h>

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#if !defined (WIN32) && !defined (_WIN32) 
#include <QtOpenGL>     //This was added here to include, in debian 32 bit, glext.h, that defines 64 bit types like Gluint64 and so on
#endif

//#include "Graphics/RTS-VTK.h"
//#include "Graphics/RTS-OSG.h"


#define BOOST_LOCALE_LINKED   //required for utf <-> ascii conversions



#if defined (_MSC_VER)
#pragma warning( disable : 4100 )
#endif





#endif      //Q_MOC_RUN
#endif      //BRAT_STDAFX_H
