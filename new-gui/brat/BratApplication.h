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


////  - empirical assumption (without this crashes): a forward like this is
////  necessary not to crash on delete because we have a data member of this
////  type in our Q_OBJECT class (and it seems it will be automatically
////  deleted)
////
QT_BEGIN_NAMESPACE
//class QString;
class QSettings;
QT_END_NAMESPACE


using namespace brathl;

namespace brathl {

	class CInternalFiles;
	class CExternalFilesNetCDF;
	class CInternalFilesYFX;
	class CInternalFilesZFXY;
}

class CWorldPlotProperty;
class CZFXYPlotProperty;
class CXYPlotProperty;
class CWPlot;
class CWorkspace;



class CBratApplication : public QgsApplication
{
	Q_OBJECT


	//////////////////////////////////////
	//	types & friends
	//////////////////////////////////////

	typedef QgsApplication base_t;

    //friend int main( int argc, char *argv[] );    TODO delete; for test purpose only


	//////////////////////////////////////
	//	static members
	//////////////////////////////////////


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
    CBratApplication( CApplicationPaths &brat_paths, int &argc, char ** argv, bool GUIenabled, QString customConfigPath = QString() );

	virtual ~CBratApplication();


	//////////////////////////////////////
	//	access
	//////////////////////////////////////

    CBratSettings& Settings() { return  mSettings; }

    const CBratSettings& Settings() const { return  mSettings; }



	//////////////////////////////////////
	//	operations
	//////////////////////////////////////

	void Restart()
	{
		quit();
		QProcess::startDetached( arguments()[ 0 ], arguments() );
	}

public slots:
	void updateSettings();
};



#endif // BRAT_APPLICATION_H
