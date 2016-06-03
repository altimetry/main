#if !defined(STDAFX_H)
#define STDAFX_H

//////////////////////////////////////////////////////////
//				Standard Libraries
//////////////////////////////////////////////////////////

#if defined(WIN32)
#define __STDC_LIMIT_MACROS		//allow inclusion of mutex without problems
#include <stdint.h>
#endif

#include <cassert>
#include <sstream>
#include <complex>
#include <vector>
#include <fstream>
#include <functional> 
#include <mutex>


//////////////////////////////////////////////////////////
//				Establish Debug Macros
//////////////////////////////////////////////////////////

#if !defined(NDEBUG)
	#if !defined(DEBUG)
	#define DEBUG
	#endif
	#if !defined(_DEBUG)
	#define _DEBUG
	#endif
#endif

#if defined (DEBUG)
	#if !defined (QGISDEBUG)
	#define QGISDEBUG
	#endif
#endif


//////////////////////////////////////////////////////////
//				Avoid XML Header Collisions
//////////////////////////////////////////////////////////

#if defined(WIN32)
//avoid later inclusion of Microsoft XML stuff, which causes name collisions with xerces
#define DOMDocument MsDOMDocument
#include <msxml.h>
#include <urlmon.h>
#undef DOMDocument
#endif		//WIN32


//////////////////////////////////////////////////////////
//				Avoid Python Header Collisions
//////////////////////////////////////////////////////////

#if defined HAVE_STAT			 	//Python also defines this
#define SAVE_HAVE_STAT	HAVE_STAT
#undef HAVE_STAT
#include "libbrathl/brathl.h"
#if defined HAVE_STAT && ( HAVE_STAT != SAVE_HAVE_STAT )	//Use Python value, which should be the same
#error HAVE_STAT Python value which should be the same
#endif
#endif


//////////////////////////////////////////////////////Avoid 3rd Party Warnings - begin
//////////////////////////////////////////////////////
#if defined (__APPLE__)
#pragma clang diagnostic ignored "-Wunknown-pragmas"    //do not reenable below
#endif
//#if defined (__unix__)
//#pragma GCC diagnostic ignored "-Wall"
//#endif
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#pragma clang diagnostic ignored "-Wall"
#endif
#if defined (WIN32) || defined(_WIN32)
#pragma warning ( disable: 4100 )           //unreferenced formal parameter
#endif


#include <QTimer>

//////////////////////////////////////////////////////////
//				Delete min/max Macros
//////////////////////////////////////////////////////////

#if defined (WIN32)
#include <windows.h>
#undef min
#undef max
#endif


//////////////////////////////////////////////////////////
//					Qt Headers
//////////////////////////////////////////////////////////


#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#if !defined (WIN32) && !defined (_WIN32) 
#include <QtOpenGL>     //This was added here to include, in debian 32 bit, glext.h, that defines 64 bit types like Gluint64 and so on
#endif

#if QT_VERSION >= 0x050000
	#include <QtWidgets/QApplication>
	#include <QtWidgets/QFileDialog>
	#include <QtWidgets/QMessageBox>
	#include <QtWidgets/QLayout>
	#include <QtWidgets/QSplitter>
	#include <QtWidgets/QToolBar>
	#include <QtWidgets/QMainWindow>
	#include <QtWidgets/QListWidget>
    #include <QtWidgets/QGroupBox>
	#include <QtWidgets/QToolButton>
	#include <QtWidgets/QActionGroup>
	#include <QMenu/QActionGroup>
#else
	#include <QtGui/QApplication>
	#include <QtGui/QFileDialog>
	#include <QMessageBox>
	#include <QLayout>
	#include <QSplitter>
	#include <QToolBar>
	#include <QMainWindow>
	#include <QListWidget>
    #include <QGroupBox>
	#include <QToolButton>
	#include <QActionGroup>
	#include <QMenu>
#endif

#if defined (WIN32) || defined(_WIN32)
#pragma warning ( default: 4100 )           //unreferenced formal parameter
#endif
#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif
//#if defined (__unix__)
//#pragma GCC diagnostic warning "-Wall"
//#endif

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////Avoid 3rd Party Warnings - end

#if defined (_MSC_VER)
#include "new-gui/Common/tools/ExceptionWin.h"
#endif

//////////////////////////////////////////////////////////
//				Miscellaneous Experimental
//////////////////////////////////////////////////////////

#define BOOST_LOCALE_LINKED   //required for utf <-> ascii conversions


#if defined (DEBUG) || defined (_DEBUG)
#if defined (WIN32) || defined (_WIN32)
	#define MEM_LEAKS
#endif
#endif


//#define TESTING_GLOBE



#endif      //STDAFX_H
