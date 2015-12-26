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

class CWorldPlotProperty;
class CZFXYPlotProperty;
class CXYPlotProperty;
class CWPlot;



class CmdLineProcessor
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

	CObArray m_plots;
	std::vector<CExpression> m_fields;

	CObArray m_zfxyPlotProperties;
	CObArray m_xyPlotProperties;
	CObArray m_wPlotProperties;


public:

	//Public ctor

	CmdLineProcessor()
	{}

	//Destruction

	virtual ~CmdLineProcessor()
	{}


	//Access

	bool isZFLatLon() const { return m_isZFLatLon; }
	bool isYFX() const { return m_isYFX; }
	bool isZFXY() const { return m_isZFXY; }

	const CObArray& plots() const { return m_plots; }

	CWorldPlotProperty* GetWorldPlotProperty( int32_t index ) const;

	//Operation

	bool Process( int argc, const char **argv );


protected:
	bool GetCommandLineOptions( int argc, const char* argv[] );
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

	std::string GetFirstFileName();

	bool IsXYPlot();
	bool IsWPlot();
	bool IsZXYPlot();

	bool IsYFXType() const;
	bool IsZFXYType() const;

	CZFXYPlotProperty* GetZFXYPlotProperty( int32_t index );
	CXYPlotProperty* GetXYPlotProperty( int32_t index );

	void CheckFieldsData( CInternalFilesYFX* yfx, const std::string& fieldName );
	void CheckFieldsData( CInternalFilesZFXY* zfxy, const std::string& fieldName );
	void CheckFieldsData( CInternalFiles* f, const std::string& fieldName );
	CInternalFiles* Prepare( int32_t indexFile, const std::string& fieldName );
	CInternalFiles* Prepare( const std::string& fileName, const std::string& fieldName );
};



#endif // CMD_LINE_PROCESSOR_H
