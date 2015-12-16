#ifndef QT_INTERFACE_H
#define QT_INTERFACE_H

//#include <cstdint>"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common declarations (interfaces) implemented differently in QtInterface.cpp and wxInterface.cpp / wxGuiInterface.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "display/PlotData/BratLookupTable.h"


//
//	CPlotField					=> CBratObject
//	CPlot		=> CPlotBase	=> CBratObject
//	CWPlot		=> CPlotBase
//	CZFXYPlot	=> CPlotBase
//
bool LoadFromFile( CBratLookupTable &lut, const std::string& fileName );

void SaveToFile( CBratLookupTable &lut, const std::string& fileName );



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Configurations
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(BRAT_V3)
#include "wx/fileconf.h"		// (wxFileConfig class)
#define CONFIG wxFileConfig
#else
#define CONFIG QSettings
#endif


class CDataset;


struct CConfiguration : public CONFIG
{
	typedef CONFIG base_t;

	CConfiguration( const std::string& localFilename );

	bool SaveConfig( const CDataset *d );
	bool LoadConfig( CDataset *d );


	void SetPath( const std::string& strPath );

	bool Write( const std::string& key, const std::string& value );
	bool Write( const std::string& key, const int& value );

	size_t GetNumberOfEntries();
	bool GetNextEntry( std::string& str, long& lIndex );

	std::string Read( const std::string& str ) const;
	bool Read( const std::string& str, std::string *value ) const;
	std::string Read( const std::string& str, const std::string &value ) const;
	bool Read( const std::string& str, int *value ) const;

	bool Read( const std::string& str, double *value, double defValue ) const;
	bool Read( const std::string& str, int32_t *value, double defValue ) const;

	bool Flush( bool bCurrentOnly = false );
};



#endif		//QT_INTERFACE_H
