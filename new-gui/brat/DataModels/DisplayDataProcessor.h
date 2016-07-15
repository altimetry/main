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
#ifndef DATA_MODELS_DISPLAY_DATA_PROCESSOR_H
#define DATA_MODELS_DISPLAY_DATA_PROCESSOR_H

		
#if QT_VERSION < 0x050000
#include <QtGui/QApplication>
#else
#include <QtWidgets/QApplication>
#endif

#include "libbrathl/List.h"
#include "libbrathl/FileParams.h"

#include "PlotData/FieldData.h"
#include "Workspaces/Display.h"


QT_BEGIN_NAMESPACE
class QString;
class QSettings;
QT_END_NAMESPACE


class CFieldData;
class CDisplay;
class CPlotInterface;


using namespace brathl;

namespace brathl {

	class CInternalFiles;
	class CExternalFilesNetCDF;
	class CInternalFilesYFX;
	class CInternalFilesZFXY;
}

class CWorldPlotProperties;


class CDisplayDataProcessorBase
{
	std::vector<CInternalFiles*> m_OpenFiles;

protected:

	//data

	bool m_isZFLatLon = false;
	bool m_isYFX = false;
	bool m_isZFXY = false;

	CStringArray mOperationOutputFiles;
	std::string m_inputFileType;

	std::vector< CPlotInterface* > m_plots;
	std::vector< CExpression > m_fields;

	std::vector< CZFXYPlotProperties* > m_zfxyPlotProperties;	
	std::vector< CXYPlotProperties* > m_xyPlotProperties;
	std::vector< CWorldPlotProperties* > m_wPlotProperties;

	bool mMapsAsPlots = false;
	CStringArray mFieldNames;

	CMapTypeDisp::ETypeDisp mDisplayType = CMapTypeDisp::Invalid();

public:

	CDisplayDataProcessorBase( bool maps_as_plots )
		: mMapsAsPlots( maps_as_plots )
	{}


	//Destruction

	virtual ~CDisplayDataProcessorBase();


	//Process

	const std::vector< CPlotInterface* >& BuildPlots();


	//Access

	virtual const std::string& ParamFile() const { return empty_string< std::string >(); }


	bool isZFLatLon() const { return m_isZFLatLon; }
	bool isYFX() const { return m_isYFX; }
	bool isZFXY() const { return m_isZFXY; }

	CWorldPlotProperties* GetWorldPlotProperties( size_t index ) const;
	CZFXYPlotProperties* GetZFXYPlotProperties( size_t index ) const;
	CXYPlotProperties* GetXYPlotProperties( size_t index ) const;

    size_t GetXYPlotPropertiesSize() const { return m_xyPlotProperties.size(); }
    size_t GetZFXYPlotPropertiesSize() const { return m_zfxyPlotProperties.size(); }
    size_t GetWorldPlotPropertiesSize() const { return m_wPlotProperties.size(); }


	//Operations

protected:
	virtual void BuildPlotsPrivate() = 0;

	//assumes mOperationOutputFiles filled

	void CheckOperationOutputFiles();

	void ComputeType();


	bool IsYFXType() const;
	bool IsZFXYType() const;

	template< class INTERNALFILES >
	void CheckFieldsData( INTERNALFILES *file, const std::string& fieldName );
	CInternalFiles* Prepare( int32_t indexFile, const std::string& fieldName );

	CInternalFiles* PrepareNetcdf( const std::string& fileName );	
};




class CDisplayDataProcessor : public CDisplayDataProcessorBase
{
	//types

	using base_t = CDisplayDataProcessorBase;

	//data

	const CDisplay *mDisplay = nullptr;
	std::vector< int > mGroups;
	std::vector< CDisplayData* > mDisplayDataArray;

public:

	CDisplayDataProcessor( bool maps_as_plots, const CDisplay *display )
		: base_t( maps_as_plots )
		, mDisplay( display )
	{}


	//Destruction

	virtual ~CDisplayDataProcessor()
	{}


	//Access


	//Operations

protected:
	virtual void BuildPlotsPrivate() override;

};



#endif // DATA_MODELS_DISPLAY_DATA_PROCESSOR_H
