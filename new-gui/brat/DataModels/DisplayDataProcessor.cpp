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
#include "new-gui/brat/stdafx.h"

#include "libbrathl/InternalFilesFactory.h"
#include "libbrathl/InternalFilesZFXY.h"
#include "libbrathl/InternalFilesYFX.h"

#include "process/ParametersDictionary.h"

#include "PlotData/ZFXYPlotData.h"
#include "PlotData/Plots.h"
#include "PlotData/WorldPlotData.h"

#include "DisplayDataProcessor.h"


//using namespace processes;


//virtual 
CDisplayDataProcessorBase::~CDisplayDataProcessorBase()
{
	for ( auto *f : m_OpenFiles )
	{
		f->Close();
	}
}


const std::vector< CPlotInterface* >& CDisplayDataProcessorBase::BuildPlots()
{
	BuildPlotsPrivate();

	for ( auto *plot : m_plots )
	{
		if ( plot )
		{
			plot->GetInfo();
		}
	}

	return m_plots; 
}


CXYPlotProperties* CDisplayDataProcessorBase::GetXYPlotProperties( size_t index ) const
{
	assert__( index < m_xyPlotProperties.size() );

	return m_xyPlotProperties[ index ];
}
CZFXYPlotProperties* CDisplayDataProcessorBase::GetZFXYPlotProperties( size_t index ) const
{
	assert__( index < m_zfxyPlotProperties.size() );

	return m_zfxyPlotProperties[ index ];
}
CWorldPlotProperties* CDisplayDataProcessorBase::GetWorldPlotProperties( size_t index ) const 
{
	assert__( index < m_wPlotProperties.size() );

	return m_wPlotProperties[ index ];
}


bool CDisplayDataProcessorBase::IsYFXType() const
{
	if ( m_inputFileType.empty() )
	{
		CException e( "CDisplayDataProcessor::isYFX - input file type is empty", BRATHL_LOGIC_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );
	}

	return m_inputFileType.compare( CInternalFilesYFX::TypeOf() ) == 0;
}
bool CDisplayDataProcessorBase::IsZFXYType() const
{
	if ( m_inputFileType.empty() )
	{
		CException e( "CDisplayDataProcessor::IsZFXYType - input file type is empty", BRATHL_LOGIC_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );
	}

	return m_inputFileType.compare( CInternalFilesZFXY::TypeOf() ) == 0;
}


template< class INTERNALFILES >
void CDisplayDataProcessorBase::CheckFieldsData( INTERNALFILES *file, const std::string& fieldName )
{
	if ( file == nullptr )
		return;

	CStringArray names;

	file->GetDataVars( names );

	if ( !names.Exists( fieldName ) )
	{
		std::string msg = CTools::Format( "CDisplayDataProcessor::CheckFieldsData - Field '%s' in command file doesn't exist in input file '%s'",
			fieldName.c_str(), file->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );
	}

}
CInternalFiles* CDisplayDataProcessorBase::Prepare( int32_t indexFile, const std::string& fieldName )
{
	CInternalFiles* f = BuildExistingInternalFileKind( mOperationOutputFiles.at( indexFile ) );

	f->Open( ReadOnly );
	m_OpenFiles.push_back( f );

	CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>( f );
	CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>( f );
	if ( zfxy != nullptr )
	{
		CheckFieldsData( zfxy, fieldName );
	}
	if ( yfx != nullptr )
	{
		CheckFieldsData( yfx, fieldName );
	}

	return f;
}

CInternalFiles* CDisplayDataProcessorBase::PrepareNetcdf( const std::string& fileName )
{
	CInternalFiles* f = nullptr;

	if ( IsYFXType() )
	{
		f = new CInternalFilesYFX( fileName );
	}
	else if ( IsZFXYType() )
	{
		f = new CInternalFilesZFXY( fileName );
	}
	else
	{
        CException e( std::string( "CDisplayDataProcessor::PrepareNetcdf -  unknown input file type: " + fileName ),
			BRATHL_LOGIC_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );
	}

	f->Open( ReadOnly );
	m_OpenFiles.push_back( f );

	return f;
}




#include "DataModels/Workspaces/Operation.h"

class CDisplayCmdFile : CCmdFile
{
	const CDisplay &mDisplay;

public:
	CDisplayCmdFile( const std::string &path, const CDisplay &Disp ) :
		CCmdFile( path ), mDisplay( Disp )
	{}

	virtual ~CDisplayCmdFile()
	{}


	std::string FmtCmdParam( const std::string& name )
	{
		return name + "=";
	}


	///////////////////////////////////////////////////

	bool BuildCmdFileHeader( bool map_as_3dplot )
	{
		Comment( "Type:" + CMapTypeDisp::GetInstance().IdToName( mDisplay.GetPlotType( map_as_3dplot ) ) );
		return true;
	}
	bool BuildCmdFileVerbose()
	{
		WriteLn();
		Comment( "----- LOG -----" );
		WriteLn();
		WriteLn( kwVERBOSE + "=" + n2s<std::string>( mDisplay.m_verbose ) );

		return true;
	}

	bool BuildCmdFileGeneralProperties( bool map_as_3dplot )
	{
		WriteLn();
		Comment( "----- GENERAL PROPERTIES -----" );
		WriteLn();

		if ( !mDisplay.GetTitle().empty() )
			WriteLn( FmtCmdParam( kwDISPLAY_TITLE ) + mDisplay.GetTitle() );

		WriteLn();
		Comment( "Display Type:" + CMapTypeDisp::GetInstance().Enum() );
		WriteLn();

		WriteLn( FmtCmdParam( kwDISPLAY_PLOT_TYPE ) + mDisplay.GetTypeAsString( map_as_3dplot ) );

		WriteLn();

		switch ( mDisplay.GetPlotType( map_as_3dplot ) )
		{
			case CMapTypeDisp::eTypeDispYFX:
				BuildCmdFileGeneralPropertiesXY();
				break;

			case CMapTypeDisp::eTypeDispZFXY:
				BuildCmdFileGeneralPropertiesZXY();
				break;

			case CMapTypeDisp::eTypeDispZFLatLon:
				BuildCmdFileGeneralPropertiesZLatLon();
				break;
			default:
				break;
		}

		return true;
	}


	void WriteMinMax()
	{
		std::string	minx, maxx, miny, maxy;
		minx = maxx = miny = maxy = "DV";

#if defined(BRAT_V3)
		if ( !isDefaultValue( mDisplay.GetMinXValue() ) )
			minx = mDisplay.GetMinXValueAsText();
		if (! isDefaultValue( mDisplay.GetMaxXValue() ) ) 
			maxx = mDisplay.GetMaxXValueAsText();
		if (!isDefaultValue( mDisplay.GetMinYValue() ) ) 
			miny = mDisplay.GetMinYValueAsText();
		if (!isDefaultValue( mDisplay.GetMaxYValue() ) ) 
			maxy =mDisplay.GetMaxYValueAsText();
#endif

		WriteLn( FmtCmdParam( kwDISPLAY_XMINVALUE ) + minx );
		WriteLn( FmtCmdParam( kwDISPLAY_XMAXVALUE ) + maxx );
		WriteLn( FmtCmdParam( kwDISPLAY_YMINVALUE ) + miny );
		WriteLn( FmtCmdParam( kwDISPLAY_YMAXVALUE ) + maxy );
	}

	bool BuildCmdFileGeneralPropertiesXY()
	{
		WriteMinMax();
		WriteLn();

		std::string axisName;
		std::string axisLabel;

		Comment( "----- X AXIS -----" );

		auto &data = mDisplay.GetData();
		for ( CObMap::const_iterator it = data.begin(); it != data.end(); it++ )
		{
			CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
			if ( value == nullptr )
				continue;

#if defined(BRAT_V3)
			if ( value->GetXAxis().empty() )
			{
#endif
				axisName = value->GetX()->GetName().c_str();
				axisLabel = value->GetX()->GetDescription().c_str();
#if defined(BRAT_V3)
			}
			else
			{
				axisName = value->GetXAxis();
				auto const *f = value->FindDimension( axisName );
				axisLabel = f ? f->GetDescription() : axisName;
			}
#endif

			WriteLn();
			WriteLn( FmtCmdParam( kwDISPLAY_XAXIS ) + axisName );
			WriteLn( FmtCmdParam( kwDISPLAY_XLABEL ) + axisLabel );

			if ( mDisplay.AreFieldsGrouped() )
				break;
		}

		return true;
	}

	bool BuildCmdFileGeneralPropertiesZXY()
	{
		WriteLn( FmtCmdParam( kwDISPLAY_ANIMATION ) + mDisplay.GetWithAnimationAsText() );

		WriteMinMax();
		WriteLn();

		Comment( "----- X/Y AXES -----" );

		std::string xAxisName;
		std::string yAxisName;

		auto &data = mDisplay.GetData();
		for ( CObMap::const_iterator it = data.begin(); it != data.end(); it++ )
		{
			CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
			if ( value == nullptr )
				continue;

#if defined(BRAT_V3)
			if ( value->IsInvertXYAxes() )
			{
				xAxisName = value->GetY()->GetName().c_str();
				yAxisName = value->GetX()->GetName().c_str();
			}
			else
			{
#endif
				xAxisName = value->GetX()->GetName().c_str();
				yAxisName = value->GetY()->GetName().c_str();

#if defined(BRAT_V3)
			}
#endif

			WriteLn();
			WriteLn( FmtCmdParam( kwDISPLAY_XAXIS ) + xAxisName );
			WriteLn( FmtCmdParam( kwDISPLAY_YAXIS ) + yAxisName );

			if ( mDisplay.AreFieldsGrouped() )
				break;
		}

		return true;
	}

	bool BuildCmdFileGeneralPropertiesZLatLon()
	{
		WriteLn( FmtCmdParam( kwDISPLAY_ANIMATION ) + mDisplay.GetWithAnimationAsText() );

		WriteLn( FmtCmdParam( kwDISPLAY_PROJECTION ) + mDisplay.m_projection );

		CStringArray array;
		mDisplay.m_zoom.GetAsString( array );
		if ( array.size() == 4 )
		{
			WriteLn( FmtCmdParam( kwDISPLAY_ZOOM_LAT1 ) + array.at( 0 ) );
			WriteLn( FmtCmdParam( kwDISPLAY_ZOOM_LON1 ) + array.at( 1 ) );
			WriteLn( FmtCmdParam( kwDISPLAY_ZOOM_LAT2 ) + array.at( 2 ) );
			WriteLn( FmtCmdParam( kwDISPLAY_ZOOM_LON2 ) + array.at( 3 ) );
		}

		return true;
	}


	bool BuildCmdFileFields( std::string errorMsg )
	{
		for ( CMapDisplayData::const_iterator it = mDisplay.m_data.begin(); it != mDisplay.m_data.end(); it++ )
		{
			CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );

			if ( value == nullptr )
				continue;

			WriteLn();
			Comment( "----- " + value->GetField()->GetName() + " FIELD -----" );
			WriteLn();
			WriteLn( kwFIELD + "=" + value->GetField()->GetName() );
			WriteLn( kwFILE + "=" + value->Operation()->GetOutputPath() );
			BuildCmdFileFieldProperties( ( it->first ).c_str(), errorMsg );
		}

		return true;
	}


	bool BuildCmdFileFieldProperties( const std::string& dataKey, std::string errorMsg )
	{
		const CDisplayData *data = dynamic_cast< const CDisplayData* >( mDisplay.m_data.Exists( dataKey ) );
		if ( data == nullptr )
		{
			errorMsg += "ERROR in  CDisplay::BuildCmdFileFieldProperties\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplayData";

			//"Error",
			//wxOK | wxCENTRE | wxICON_ERROR );
			return false;
		}
		WriteLn();
		Comment( "----- " + data->GetField()->GetName() + " FIELDS PROPERTIES -----" );
		WriteLn();

		std::string displayName = data->GetField()->GetDescription();
		if ( !displayName.empty() )
		{
			WriteLn( FmtCmdParam( kwDISPLAY_NAME ) + displayName );
		}
		else
		{
			WriteLn( FmtCmdParam( kwDISPLAY_NAME ) + data->GetField()->GetName() );
		}


		WriteLn( FmtCmdParam( kwFIELD_GROUP ) + data->GetGroupAsText() );

		switch ( mDisplay.m_type )
		{
			case CMapTypeDisp::eTypeDispYFX:
				BuildCmdFileFieldPropertiesXY( data );
				break;

			case CMapTypeDisp::eTypeDispZFXY:
			case CMapTypeDisp::eTypeDispZFLatLon:
				BuildCmdFileFieldPropertiesZXY( data );
				break;

			default:
				break;
		}

		return true;
	}

	bool BuildCmdFileFieldPropertiesXY( const CDisplayData* value )
	{
		return value != nullptr;
	}

	bool BuildCmdFileFieldPropertiesZXY( const CDisplayData* value )
	{
		if ( value == nullptr )
			return false;

		std::string
		valueString = ( isDefaultValue( value->CurrentMinValue() ) ) ? "DV" : value->AbsoluteMinValueAsText();
		WriteLn( FmtCmdParam( kwDISPLAY_MINVALUE ) + valueString );

		valueString = ( isDefaultValue( value->CurrentMaxValue() ) ) ? "DV" : value->AbsoluteMaxValueAsText();
		WriteLn( FmtCmdParam( kwDISPLAY_MAXVALUE ) + valueString );

		WriteLn( FmtCmdParam( kwDISPLAY_CONTOUR ) + value->ContourAsText() );
		WriteLn( FmtCmdParam( kwDISPLAY_SOLID_COLOR ) + value->GetSolidColorAsText() );

		valueString = ( value->ColorPalette().empty() ) ? "DV" : value->ColorPalette();
		WriteLn( FmtCmdParam( kwDISPLAY_COLORTABLE ) + valueString );

		WriteLn( FmtCmdParam( kwDISPLAY_EAST_COMPONENT ) + ( value->IsEastComponent() ? "Y" : "N" ) );

		WriteLn( FmtCmdParam( kwDISPLAY_NORTH_COMPONENT ) + ( value->IsNorthComponent() ? "Y" : "N" ) );

		return true;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////


	static bool BuildCmdFile( const std::string &path, bool map_as_3dplot, const CDisplay &Disp, std::string &errorMsg )
	{
        CDisplayCmdFile f( path, Disp );

		return 
			f.IsOk()											&&
			f.BuildCmdFileHeader( map_as_3dplot )				&&
			f.BuildCmdFileVerbose()								&&
			f.BuildCmdFileGeneralProperties( map_as_3dplot )	&&
			f.BuildCmdFileFields( errorMsg )					&&
			f.IsOk();
	}
};






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






void CDisplayDataProcessorBase::CheckOperationOutputFiles()
{
	assert__( m_inputFileType.empty() );		//v4, so far, should be called only once

	if ( mOperationOutputFiles.empty() )
	{
		CException e( "CDisplayDataProcessor::CheckOperationOutputFiles - input data file list is empty()", BRATHL_COUNT_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );
	}

	for ( CStringArray::iterator it = mOperationOutputFiles.begin(); it != mOperationOutputFiles.end(); it++ )
	{
		std::string inputFileTypeRead = CNetCDFFiles::IdentifyExistingFile( *it );

		if ( it != mOperationOutputFiles.begin() )
		{
			if ( m_inputFileType.compare( inputFileTypeRead ) != 0 )
			{
				std::string msg = CTools::Format( "CDisplayDataProcessor::CheckOperationOutputFiles - Files are not in the same way - Expected type '%s' and found '%s' for file '%s'",
					m_inputFileType.c_str(), inputFileTypeRead.c_str(), it->c_str() );
				CException e( msg, BRATHL_INCONSISTENCY_ERROR );
				LOG_TRACE( e.what() );
				throw ( e );

			}
		}
		m_inputFileType = inputFileTypeRead;
	}
}


void CDisplayDataProcessorBase::ComputeType()
{
	assert__( mDisplayType != CMapTypeDisp::Invalid() );


	m_isYFX =		CInternalFiles::IsYFXFile( mOperationOutputFiles[ 0 ] );

	m_isZFLatLon =	!mMapsAsPlots && CInternalFiles::IsZFLatLonFile( mOperationOutputFiles[ 0 ] );

	m_isZFXY =		( mMapsAsPlots && CInternalFiles::IsZFLatLonFile( mOperationOutputFiles[ 0 ] ) ) 
					|| 
					CInternalFiles::IsZFXYFile( mOperationOutputFiles[ 0 ], &mFieldNames );


	if ( m_isYFX && m_isZFXY )
	{
		if ( mDisplayType == CMapTypeDisp::eTypeDispYFX )
		{
			m_isZFXY = false;
		}
		else
		{
			m_isYFX = false;
		}
	}

	if ( m_isYFX && m_isZFLatLon )
	{
		if ( mDisplayType == CMapTypeDisp::eTypeDispYFX )
		{
			m_isZFLatLon = false;
		}
		else
		{
			m_isYFX = false;
		}
	}
}


void CDisplayDataProcessor::BuildPlotsPrivate()
{

	////////////////////////////////////////////////////////////////////////////
	//	process kwFILE kwFIELD kwFIELD_GROUP and kwDISPLAY_PLOT_TYPE
	////////////////////////////////////////////////////////////////////////////

	auto const &data_map = mDisplay->GetData();
	for ( auto const &entry : data_map )
	{
		CDisplayData *ddata = dynamic_cast< CDisplayData* >( entry.second );				assert__( ddata );

		//kwFILE

		const std::string path = ddata->Operation()->GetOutputPath();
		mOperationOutputFiles.push_back( path );							//one file per field even if repeated

		//kwFIELD

		const std::string field_name = ddata->GetField()->GetName();						assert__( field_name == ddata->FieldName() );
		auto const it = std::find( mFieldNames.begin(), mFieldNames.end(), field_name );	assert__( it == mFieldNames.end() );
		if ( it == mFieldNames.end() )
			mFieldNames.push_back( field_name );

		//kwFIELD_GROUP

		mGroups.push_back( ddata->GetGroup() );

		//

		mDisplayDataArray.push_back( ddata );
	}


	const size_t nbExpr = mFieldNames.size();


	// Check all input files

	CheckOperationOutputFiles();

	// v4 not used: compute groups

	int groupNumberMax = std::numeric_limits< int >::lowest();
	int groupNumberMin = std::numeric_limits< int >::max();

	const size_t nbGroup = mGroups.size();
	if ( nbGroup != 0 )
	{
		for ( size_t index = 0; index < nbExpr; index++ )
		{
			int group = mGroups[ index ];
			groupNumberMax = std::max( group, groupNumberMax );
			groupNumberMin = std::min( group, groupNumberMin );
		}
	}

	// get mDisplayType

	mDisplayType = mDisplay->GetPlotType( mMapsAsPlots );


	///////////////////
	// get type of plot
	///////////////////

	ComputeType();


	///////////////////
	// build plots...
	///////////////////


	m_plots.resize( groupNumberMax + 1 );

	if ( m_isYFX )
	{
		for ( unsigned i = 0; i < nbExpr; i++ )
		{
			//m_xyPlotProperties.push_back( new CXYPlotProperties( *mDisplayDataArray[ i ] ) );
			m_xyPlotProperties.push_back( mDisplayDataArray[ i ] );
			m_xyPlotProperties.back()->ClearInternalFiles();
		}
	}

	if ( m_isZFLatLon )
	{
		for ( unsigned i = 0; i < nbExpr; i++ )
		{
			//m_wPlotProperties.push_back( new CWorldPlotProperties( *mDisplayDataArray[ i ] ) );
			m_wPlotProperties.push_back( mDisplayDataArray[ i ] );
			m_wPlotProperties.back()->ClearInternalFiles();
		}
	}

	if ( m_isZFXY )
	{
		for ( unsigned i = 0; i < nbExpr; i++ )
		{
			//m_zfxyPlotProperties.push_back( new CZFXYPlotProperties( *mDisplayDataArray[ i ] ) );
			m_zfxyPlotProperties.push_back( mDisplayDataArray[ i ] );
			m_zfxyPlotProperties.back()->ClearInternalFiles();
		}
	}



	m_fields.resize( nbExpr );

	for ( size_t index = 0; index < nbExpr; index++ )
	{
		CExpression expr;
		try
		{
			expr.SetExpression( mFieldNames[ index ] );
		}
		catch ( CException e )
		{
			throw CParameterException( "Invalid expression value for key " + kwFIELD + "[" + n2s<std::string>( index ) + "]: " + e.what(), BRATHL_ERROR );
		}

		if ( expr.GetFieldNames()->size() != 1 )
		{
			std::string msg = CTools::Format( "CDisplayDataProcessor::GetParameters - Expression '%s' has incorrect number of fields '%ld' (correct is 1)",
				expr.AsString().c_str(), (long)expr.GetFieldNames()->size() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			LOG_TRACE( e.what() );
			throw ( e );
		}

		const CDisplayData *display_data = mDisplayDataArray[ index ];
		const int groupNumber = display_data->GetGroup();
		const std::string fieldName = expr.GetFieldNames()->at( 0 );		assert__( fieldName == mFieldNames[ index ] );

		CInternalFiles *internal_file = Prepare( index, fieldName );

		// ------------------------------
		// -------------- YFX plot
		// ------------------------------
		if ( m_isYFX )
		{
			assert__( dynamic_cast<CInternalFilesYFX*>( internal_file ) );

			auto *props = GetXYPlotProperties( index );

			// Add field to xy plot group
			CYFXPlot* plot = dynamic_cast<CYFXPlot*>( m_plots[ groupNumber ] );
			if ( plot == nullptr )
			{
				plot = new CYFXPlot( mDisplay->GetTitle(), groupNumber );
				m_plots[ groupNumber ] = plot;
			}
			plot->PushFieldData( props );

			//kwDISPLAY_XAXIS
			//kwDISPLAY_XLABEL

			std::string xAxisName;
			std::string xAxisLabel;

#if defined(BRAT_V3)
			if ( display_data->GetXAxis().empty() )
			{
#endif
				xAxisName = display_data->GetX()->GetName();
				xAxisLabel = display_data->GetX()->GetDescription();
#if defined(BRAT_V3)
			}
			else
			{
				xAxisName = display_data->GetXAxis();
				auto const *f = display_data->FindDimension( xAxisName );
				xAxisLabel = f ? f->GetDescription() : xAxisName;
			}
#endif

			if ( !xAxisName.empty() )
			{
				plot->SetForcedVarXname( xAxisName );
#if defined(BRAT_V3)
				plot->m_titleX = xAxisLabel;
#endif
			}

			props->PushInternalFile( internal_file );			//field->m_xyProps = GetXYPlotProperties( index );
		}

		// ------------------------------
		// -------------- World plot
		// ------------------------------
		if ( m_isZFLatLon )
		{
			assert__( dynamic_cast<CInternalFilesZFXY*>( internal_file ) );

			auto *props = GetWorldPlotProperties( index );

			// Add field to world plot group
			CGeoPlot* wplot = dynamic_cast<CGeoPlot*>( m_plots[ groupNumber ] );
			if ( wplot == nullptr )
			{
				wplot = new CGeoPlot( mDisplay->GetTitle(), groupNumber );
				m_plots[ groupNumber ] = wplot;
			}

			if ( !props->WithContour() && !props->WithSolidColor() )
			{
				props->SetWithSolidColor( true );
			}

			CFieldData* field = nullptr;

#if defined (BRAT_V3)
			if ( props->m_withAnimation )
			{
				//group files by field
				// find an already existing field with the same name
				// and the same contour flag
				// (don't animate same named fields which with different contour flag)
				bool with_contour = props->WithContour(), with_solid_color = props->WithSolidColor();
				field = wplot->FindPlotField( fieldName, &with_contour, &with_solid_color );
			}
#endif
			if ( field == nullptr )
			{
				wplot->PushFieldData( props );
			}

			props->PushInternalFile( internal_file );			//field->m_worldProps = worldProps;
		}

		// ------------------------------
		// -------------- ZFXY plot
		// ------------------------------

		if ( m_isZFXY )
		{
			assert__( dynamic_cast<CInternalFilesZFXY*>( internal_file ) || dynamic_cast<CInternalFilesYFX*>( internal_file ) );

			auto *props = GetZFXYPlotProperties( index );

			// Add field to ZFXY plot group
			CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( m_plots[ groupNumber ] );
			if ( zfxyplot == nullptr )
			{
				zfxyplot = new CZFXYPlot( mDisplay->GetTitle(), groupNumber );
				m_plots[ groupNumber ] = zfxyplot;
			}

			if ( !props->WithContour() && !props->WithSolidColor() )
			{
				props->SetWithSolidColor( true );
			}

			CFieldData* field = nullptr;

#if defined (BRAT_V3)
			if ( props->m_withAnimation )
			{
				//group files by field
				// find an already existing field with the same name
				// and the same contour flag
				// (don't animate same named fields which with different contour flag)
				bool with_contour = props->WithContour(), with_solid_color = props->WithSolidColor();
				field = zfxyplot->FindPlotField( fieldName, &with_contour, &with_solid_color );
			}
#endif
			if ( field == nullptr )
			{
				zfxyplot->PushFieldData( props );
			}

			//kwDISPLAY_XAXIS
			//kwDISPLAY_YAXIS

			std::string xAxisName;
			std::string yAxisName;

#if defined(BRAT_V3)
			if ( display_data->IsInvertXYAxes() )
			{
				xAxisName = display_data->GetY()->GetName();
				yAxisName = display_data->GetX()->GetName();
			}
			else
			{
#endif
				xAxisName = display_data->GetX()->GetName();
				yAxisName = display_data->GetY()->GetName();

#if defined(BRAT_V3)
			}
#endif

			if ( !xAxisName.empty() )
			{
				zfxyplot->SetForcedVarXname( xAxisName );
			}

			if ( !yAxisName.empty() )
			{
				zfxyplot->SetForcedVarYname( yAxisName );
			}

			props->PushInternalFile( internal_file );			//field->m_zfxyProps = zfxyProps;
		}

		m_fields[ index ] = expr;
	}
}




