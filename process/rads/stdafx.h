#if !defined(STDAFX_H)
#define STDAFX_H

#include "targetver.h"

#include <stdio.h>
#if defined (Q_OS_WIN)
#include <tchar.h>
#endif

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




#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#pragma clang diagnostic ignored "-Wall"
#endif

#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextStream>
#include <QDateTime>
#include <QStringList>
#include <QDir>
#include <QSettings>
#include <QTimer>
#include <QThread>

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


#endif      //STDAFX_H
