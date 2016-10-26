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


#if !defined (DEBUG)
#define QT_NO_DEBUG_OUTPUT
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



//////////////////////////////////////////////////////////
//				Python Engine
//////////////////////////////////////////////////////////

#if !defined (BRAT_V3)
#include "process/PythonEngine.hpp"
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


//////////////////////////////////////////////////////////
//				v3.1.0 Compatibility
//////////////////////////////////////////////////////////

#if defined (BRAT_V3)
#include "../support/code/legacy/v4Interface.h"
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
//				Qt / QGIS Headers
//////////////////////////////////////////////////////////


#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#if !defined(QGIS_DISABLE_DEPRECATED)
#define QGIS_DISABLE_DEPRECATED
#endif
#endif
#if !defined (WIN32) && !defined (_WIN32) 
#include <QtOpenGL>     //This was added here to include, in debian 32 bit, glext.h, that defines 64 bit types like Gluint64 and so on
#endif



#if QT_VERSION >= 0x050000
#include <QtPrintSupport/qprinter.h>
#include <QtPrintSupport/qprintdialog.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLayout>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QActionGroup>
#include <QtWidgets/QMenu>
#include <QtWidgets/QInputDialog>
#include <QDesktopServices>
#include <QtWidgets/QDesktopWidget>
#else
#include <qprinter.h>
#include <qprintdialog.h>
#include <QtGui/QApplication>
#include <QtGui/QFileDialog>
#include <QtGui/QTextEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QLayout>
#include <QtGui/QSplitter>
#include <QtGui/QToolBar>
#include <QtGui/QMainWindow>
#include <QtGui/QListWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QToolButton>
#include <QtGui/QPushButton>
#include <QtGui/QActionGroup>
#include <QtGui/QMenu>
#include <QtGui/QInputDialog>
#include <QtGui/QDesktopServices>
#include <QtGui/QDesktopWidget>
#endif
#include <QSettings>
#include <QResource>
#include <QElapsedTimer>
#include <QThread>
#include <QUrl>


#include <new-gui/Common/SingleApplication/QtLockedFile>


#include <qgsapplication.h>
#include <qgsmapcanvas.h>
#include <qgsvectorlayer.h>
#include <qgsrasterlayer.h>
#include <qgslayertreeview.h>
#include <qgslayertreemodel.h>
#include <qgslayertreegroup.h>

#include <qgscollapsiblegroupbox.h>
#include <qgsmaptool.h>



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



//////////////////////////////////////////////////////////
//				Miscellaneous
//////////////////////////////////////////////////////////

#if defined (_MSC_VER)
#include "new-gui/Common/tools/ExceptionWin.h"
#endif


#if !defined (BRAT_V3)
#include "BratLogger.h"
#endif


#define BOOST_LOCALE_LINKED   //required for utf <-> ascii conversions


#if defined (DEBUG) || defined (_DEBUG)
#if defined (WIN32) || defined (_WIN32)
	#define MEM_LEAKS
#endif
#endif




#endif      //STDAFX_H
