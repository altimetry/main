#ifndef CMD_LINE_PROCESSOR_H
#define CMD_LINE_PROCESSOR_H

		
#if QT_VERSION < 0x050000
#include <QtGui/QApplication>
#else
#include <QtWidgets/QApplication>
#endif

#include "libbrathl/List.h"
#include "libbrathl/FileParams.h"


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

class CWorldPlotProperties;
struct CZFXYPlotProperties;
class CXYPlotProperties;
class CPlotBase;



class CDisplayFilesProcessor
{
	//data

	bool m_isZFLatLon = false;
	bool m_isYFX = false;
	bool m_isZFXY = false;

	std::string m_paramFile;
	std::string m_paramXAxis;
	std::string m_paramYAxis;

	CStringList m_paramVars;
	CFileParams m_params;
	CStringArray m_inputFiles;
	std::string m_inputFileType;

	std::vector< CPlotBase* > m_plots;
	std::vector<CExpression> m_fields;

	CObArray m_zfxyPlotProperties;
	CObArray m_xyPlotProperties;
	CObArray m_wPlotProperties;


public:

	//Public ctor

	CDisplayFilesProcessor()
	{}

	CDisplayFilesProcessor( const std::string &display_cmd_file )
	{
		Process( display_cmd_file );
	}

	//Destruction

	virtual ~CDisplayFilesProcessor()
	{}


	//Access

	const std::string& ParamFile() const { return m_paramFile;  }

	bool isZFLatLon() const { return m_isZFLatLon; }
	bool isYFX() const { return m_isYFX; }
	bool isZFXY() const { return m_isZFXY; }

	const std::vector< CPlotBase* > & plots() const { return m_plots; }

	CWorldPlotProperties* GetWorldPlotProperties( int32_t index ) const;
	CZFXYPlotProperties* GetZFXYPlotProperties( int32_t index ) const;
	CXYPlotProperties* GetXYPlotProperties( int32_t index ) const;

	std::string GetFirstFileName();


	//Operations

	bool Process( const std::vector< std::string > &args );

	bool Process( int argc, const char **argv )
	{
		std::vector< std::string > v;
		for ( int i = 0; i < argc; ++i )
			v.push_back( argv[ i ] );

		return Process( v );
	}

	bool Process( const std::string &file_path )
	{
		const char *argv[] = { "", file_path.c_str() };
		return Process( 2, argv );
	}

	bool Process( const QStringList &args );

	CInternalFiles* Prepare( const std::string& fileName );
	
protected:
	bool GetCommandLineOptions( const std::vector< std::string > &args );
	void GetParameters();

	bool GetParametersNetcdf();
	bool GetParametersNetcdfZFLatLon( CExternalFilesNetCDF* externalFile );
	bool GetParametersNetcdfZFXY( CExternalFilesNetCDF* externalFile );
	bool GetParametersNetcdfYFX( CExternalFilesNetCDF* externalFile );

	void LoadParameters();
	void CheckFiles();


	void GetXYPlotPropertyParams( int32_t nFields );
	void GetWPlotPropertyParams( int32_t nFields );
	void GetZFXYPlotPropertyParams( int32_t nFields );

	bool IsXYPlot();
	bool IsWPlot();
	bool IsZXYPlot();

	bool IsYFXType() const;
	bool IsZFXYType() const;

	void CheckFieldsData( CInternalFilesYFX* yfx, const std::string& fieldName );
	void CheckFieldsData( CInternalFilesZFXY* zfxy, const std::string& fieldName );
	void CheckFieldsData( CInternalFiles* f, const std::string& fieldName );
	CInternalFiles* Prepare( int32_t indexFile, const std::string& fieldName );
};



#endif // CMD_LINE_PROCESSOR_H
