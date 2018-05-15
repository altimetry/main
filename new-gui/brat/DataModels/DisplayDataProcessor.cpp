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

#include "PlotData/ZFXYPlotField.h"
#include "PlotData/Plots.h"
#include "PlotData/GeoPlotField.h"

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


const std::vector< CPlotInterfaces* >& CDisplayDataProcessorBase::BuildPlots()
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
		std::string inputFileTypeRead = CNetCDFFiles::IdentifyExistingFile( *it ).first;

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

	auto const &data_map = static_cast< const CDisplay* >( mDisplay )->GetData();
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
			assert__( dynamic_cast<CInternalFiles*>( internal_file ) );

			auto *props = GetXYPlotProperties( index );

			// Add field to xy plot group
			CYFXPlot* plot = dynamic_cast<CYFXPlot*>( m_plots[ groupNumber ] );
			if ( plot == nullptr )
			{
				plot = new CYFXPlot( mDisplay, groupNumber );
				m_plots[ groupNumber ] = plot;
			}
			plot->PushFieldData( props );

			//kwDISPLAY_XAXIS
			//kwDISPLAY_XLABEL

			std::string xAxisName;
			std::string xAxisLabel;
			
			if ( display_data->GetXAxis().empty() )							//if ( display_data->GetX()->GetName().empty() )
			{
				xAxisName = display_data->GetX()->GetName();
#if defined(BRAT_V3)
				xAxisLabel = display_data->GetX()->GetDescription();
#endif
			}
			else
			{
				xAxisName = display_data->GetXAxis();
#if defined(BRAT_V3)
				auto const *f = display_data->FindDimension( xAxisName );
				xAxisLabel = f ? f->GetDescription() : xAxisName;
#endif
			}

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
				wplot = new CGeoPlot( mDisplay, groupNumber );
				m_plots[ groupNumber ] = wplot;
			}

			CFieldData* field = nullptr;

#if defined (BRAT_V3)
			if ( !props->WithContour() && !props->WithSolidColor() )
			{
				props->SetWithSolidColor( true );
			}

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
			assert__( dynamic_cast<CInternalFiles*>( internal_file ) || dynamic_cast<CInternalFilesYFX*>( internal_file ) );

			auto *props = GetZFXYPlotProperties( index );

			// Add field to ZFXY plot group
			CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( m_plots[ groupNumber ] );
			if ( zfxyplot == nullptr )
			{
				zfxyplot = new CZFXYPlot( mDisplay, groupNumber );
				m_plots[ groupNumber ] = zfxyplot;
			}

			CFieldData* field = nullptr;

#if defined (BRAT_V3)
			if ( !props->WithContour() && !props->WithSolidColor() )
			{
				props->SetWithSolidColor( true );
			}

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




