#ifndef QBRAT_APP_H
#define QBRAT_APP_H


#include <QtGui/QApplication>

#include <qgsapplication.h>

#include "../libbrathl/List.h"
#include "../libbrathl/FileParams.h"


enum EApplicationStyleSheets
{
	e_DarkStyle,
	e_DarkOrangeStyle,
	e_Dark_2015Style,

	EApplicationStylesSheets_size
};


struct TApplicationOptions
{
	static const EApplicationStyleSheets sm_DefaultCustomAppStyleSheet = e_Dark_2015Style;

	QString m_DefaultAppStyle;
	QString m_AppStyle;
	bool m_UseDefaultStyle;

	EApplicationStyleSheets m_CustomAppStyleSheet;
	bool m_NoStyleSheet;

	TApplicationOptions() :
		m_UseDefaultStyle( false ),
		m_CustomAppStyleSheet( sm_DefaultCustomAppStyleSheet ),
		m_NoStyleSheet( false )
	{}
	TApplicationOptions( const TApplicationOptions &o )
	{
		*this = o;
	}

	TApplicationOptions& operator = ( const TApplicationOptions &o )
	{
		if ( this != &o )
		{
			m_DefaultAppStyle = o.m_DefaultAppStyle;
			m_AppStyle = o.m_AppStyle;
			m_UseDefaultStyle = o.m_UseDefaultStyle;

			m_CustomAppStyleSheet = o.m_CustomAppStyleSheet;
			m_NoStyleSheet = o.m_NoStyleSheet;
		}
		return *this;
	}

	bool operator == ( const TApplicationOptions &o ) const
	{
		return 
			m_DefaultAppStyle == o.m_DefaultAppStyle &&
			m_AppStyle == o.m_AppStyle &&
			m_UseDefaultStyle == o.m_UseDefaultStyle &&

			m_CustomAppStyleSheet == o.m_CustomAppStyleSheet &&
			m_NoStyleSheet == o.m_NoStyleSheet;
	}
};

//  - empirical assumption (without this crashes): a forward like this is
//  necessary not to crash on delete because we have a data member of this
//  type in our Q_OBJECT class (and it seems it will be automatically
//  deleted
//
QT_BEGIN_NAMESPACE
class QString;
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



class QbrtApplication : public QgsApplication
{
	Q_OBJECT

	//types

	typedef QgsApplication base_t;


	//data

	//QString m_execName;
	QSettings *m_config = nullptr;

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


public:

	//Public ctor

    QbrtApplication( int &argc, char ** argv, bool GUIenabled, QString customConfigPath = QString() );


	//Destruction

	virtual ~QbrtApplication();


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

    static std::string getNameOfStyle( const QStyle *s );
    static	std::string getNameOfStyle( QStyle *s, bool del );
    static const std::vector< std::string >& getStyles();
    static size_t getStyleIndex( const QString &qname );

    const std::vector< std::string >& getStyleSheets( bool resources );

	bool setApplicationStyle( TApplicationOptions &options );

private:
	friend const std::vector< std::string >& buildStyleNamesList();

	TApplicationOptions& getAppOptions()
	{
		static TApplicationOptions AppOptions;
		return AppOptions;
	}
};



#endif // QBRAT_APP_H
