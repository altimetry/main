#if !defined(STDAFX_H)
#define STDAFX_H

#include <cassert>
#include <sstream>
#include <complex>
#include <vector>
#include <fstream>
#include <functional> 

#if !defined(NDEBUG)
	#if !defined(DEBUG)
	#define DEBUG
	#endif
	#if !defined(_DEBUG)
	#define _DEBUG
	#endif
#endif


#if defined(WIN32)
//avoid later inclusion of Microsoft XML stuff, which causes name collisions with xerces
#define DOMDocument MsDOMDocument
#include <msxml.h>
#include <urlmon.h>
#undef DOMDocument
#endif		//WIN32


#if defined HAVE_STAT			 	//Python also defines this
#define SAVE_HAVE_STAT	HAVE_STAT
#undef HAVE_STAT
#include <brathl_config.h>
#if defined HAVE_STAT && ( HAVE_STAT != SAVE_HAVE_STAT )	//Use Python value, which should be the same
#error HAVE_STAT Python value which should be the same
#endif
#endif


#include <QTimer>

#if defined (WIN32)
#include <windows.h>
#undef min
#undef max
#endif


#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#if !defined (WIN32) && !defined (_WIN32) 
#include <QtOpenGL>     //This was added here to include, in debian 32 bit, glext.h, that defines 64 bit types like Gluint64 and so on
#endif


#define BOOST_LOCALE_LINKED   //required for utf <-> ascii conversions


#if defined (DEBUG) || defined (_DEBUG)
#if defined (WIN32) || defined (_WIN32)
	#define MEM_LEAKS
#endif
#endif


#endif      //STDAFX_H
