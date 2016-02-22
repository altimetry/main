#ifndef BRAT_APPLICATION_H
#define BRAT_APPLICATION_H


#if QT_VERSION >= 0x050000
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif

#include <qgsapplication.h>

#include "libbrathl/List.h"
#include "libbrathl/FileParams.h"

#include "new-gui/Common/QtStringUtils.h"

#include "BratSettings.h"


using namespace brathl;



class CBratApplication : public QgsApplication
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


	//////////////////////////////////////
	//	types & friends
	//////////////////////////////////////

	typedef QgsApplication base_t;


	//////////////////////////////////////
	//	static members
	//////////////////////////////////////


	static bool smPrologueCalled;					//initialized in Prologue

	static CApplicationPaths *smApplicationPaths;	//initialized in Prologue


	static void CheckOpenGL( bool extended = false );

public:

	// Calling this in main is mandatory
	//	
	static void Prologue( int argc, char *argv[] );


    static int OffGuiErrorDialog( int error_type, char const *error_msg );


protected:
	//	This is accurate only if(when) no style sheet was also assigned.
	//	External clients should not rely on this, and use the name in options.
    //	Internal code can only rely on this before assigning a style sheet.
	// 
	static QString getCurrentStyleName()
	{
		return style()->objectName().toLower();
	}


	//////////////////////////////////////
    //	instance data
	//////////////////////////////////////

    CBratSettings mSettings;

	bool mLeakExceptions = false;


    bool m_isZFLatLon = false;
	bool m_isYFX = false;
	bool m_isZFXY = false;

	CObArray m_internalData;

	std::string m_paramFile;
	std::string m_paramXAxis;
	std::string m_paramYAxis;

	CStringList m_paramVars;
	CFileParams m_params;
	CStringArray m_inputFiles;
	std::string m_inputFileType;

	CObArray m_plots;
	std::vector<CExpression> m_fields;

	CObArray m_zfxyPlotProperties;
	CObArray m_xyPlotProperties;
	CObArray m_wPlotProperties;


	//////////////////////////////////////
	//	construction / destruction 
	//////////////////////////////////////

	QString mDefaultAppStyle;

public:
    CBratApplication( int &argc, char **argv, bool GUIenabled, QString customConfigPath = QString() );

	virtual ~CBratApplication();


	//////////////////////////////////////
	//	access
	//////////////////////////////////////

    CBratSettings& Settings() { return  mSettings; }

    const CBratSettings& Settings() const { return  mSettings; }

	void LeakExceptions( bool leak )
	{
		mLeakExceptions = leak;
	}


	//////////////////////////////////////
	//	operations
	//////////////////////////////////////

	virtual bool notify( QObject * receiver, QEvent * event ) override
	{
		if ( mLeakExceptions )
		{
			return QApplication::notify( receiver, event );
		}
		return base_t::notify( receiver, event );
	}


	//not tested enough
	//
	void Restart()
	{
		quit();
		QProcess::startDetached( arguments()[ 0 ], arguments() );
	}

public slots:
	void UpdateSettings();
};



#endif // BRAT_APPLICATION_H
