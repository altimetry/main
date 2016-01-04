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

#include "ApplicationSettings.h"


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



enum EApplicationStyleSheets
{
	e_DarkStyle,
	e_DarkOrangeStyle,
	e_Dark_2015Style,

	EApplicationStylesSheets_size
};



class CBratApplication : public QgsApplication
{
	Q_OBJECT


	//////////////////////////////////////
	//	types & friends
	//////////////////////////////////////

	typedef QgsApplication base_t;

	friend int main( int argc, char *argv[] );


	//////////////////////////////////////
	//	static members
	//////////////////////////////////////

	static CApplicationSettings& Settings()
	{
		static CApplicationSettings settings( "ESA", q2t< std::string >( QgsApplication::applicationName() ) );
		return settings;
	}


	//////////////////////////////////////
	//	data
	//////////////////////////////////////

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

public:
    CBratApplication( int &argc, char ** argv, bool GUIenabled, QString customConfigPath = QString() );

	virtual ~CBratApplication();


	//////////////////////////////////////
	//	access
	//////////////////////////////////////


	//////////////////////////////////////
	//	operations
	//////////////////////////////////////

protected:
	//bool GetCommandLineOptions( int argc, char* argv[] );
	//void GetParameters();

	//bool GetParametersNetcdf();
	//bool GetParametersNetcdfZFLatLon( CExternalFilesNetCDF* externalFile );
	//bool GetParametersNetcdfZFXY( CExternalFilesNetCDF* externalFile );
	//bool GetParametersNetcdfYFX( CExternalFilesNetCDF* externalFile );

	//void LoadParameters();
	//void CheckFiles();


	//void GetXYPlotPropertyParams( int32_t nFields );
	//void GetWPlotPropertyParams( int32_t nFields );
	//void GetZFXYPlotPropertyParams( int32_t nFields );

	//std::string GetFirstFileName();

	//void CreateWPlot( CWPlot* wplot, QSize& size, QPoint& pos );
	//void WorldPlot();
	//void XYPlot();
	//void ZFXYPlot();

	//bool IsXYPlot();
	//bool IsWPlot();
	//bool IsZXYPlot();

	//bool IsYFXType() const;
	//bool IsZFXYType() const;

	//CWorldPlotProperty* GetWorldPlotProperty( int32_t index );
	//CZFXYPlotProperty* GetZFXYPlotProperty( int32_t index );
	//CXYPlotProperty* GetXYPlotProperty( int32_t index );

	//void CheckFieldsData( CInternalFilesYFX* yfx, const std::string& fieldName );
	//void CheckFieldsData( CInternalFilesZFXY* zfxy, const std::string& fieldName );
	//void CheckFieldsData( CInternalFiles* f, const std::string& fieldName );
	//CInternalFiles* Prepare( int32_t indexFile, const std::string& fieldName );
	//CInternalFiles* Prepare( const std::string& fileName, const std::string& fieldName );
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:

};



#endif // BRAT_APPLICATION_H