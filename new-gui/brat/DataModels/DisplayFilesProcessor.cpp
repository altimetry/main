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

#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/QtUtilsIO.h"

#include "libbrathl/Tools.h"

#include "BratLogger.h"
#include "libbrathl/InternalFilesFactory.h"
#include "libbrathl/InternalFilesZFXY.h"
#include "libbrathl/InternalFilesYFX.h"
#include "libbrathl/brathl_c/argtable2.h"

#include "process/ParametersDictionary.h"
#include "process/BratProcess.h"

#include "PlotData/XYPlotData.h"
#include "MapTypeDisp.h"
#include "PlotData/Plots.h"
#include "PlotData/ZFXYPlotData.h"
#include "PlotData/WorldPlotData.h"

#include "DataModels/PlotData/ColorPalleteNames.h"
#include "DataModels/PlotData/MapProjection.h"
#include "PlotData/BratLookupTable.h"



#include "DisplayFilesProcessor.h"


using namespace processes;



static const KeywordHelp keywordList[]	= {
	KeywordHelp(kwFILE,			1, 0),
	KeywordHelp(kwFIELD,			1, 23, nullptr, 24),
	KeywordHelp(kwFIELD_GROUP,		-24),

#if defined (BRAT_V3)
	KeywordHelp(kwDISPLAY_PROPERTIES,	0, 1, "No"),
	KeywordHelp(kwDISPLAY_ANIMATIONBAR,	0, 1, "No"),
	KeywordHelp(kwDISPLAY_COLORBAR,		0, 1, "Yes"),
	KeywordHelp(kwDISPLAY_ANIMATION,	0, 1, "No"),
#endif
	KeywordHelp(kwDISPLAY_TITLE,		0, 1, "\"\""),
	KeywordHelp(kwDISPLAY_CENTERLAT,	0, 1, "0"),
	KeywordHelp(kwDISPLAY_CENTERLON,	0, 1, "0"),
	KeywordHelp(kwDISPLAY_PROJECTION,	0, 1, "3D"),
	KeywordHelp(kwDISPLAY_COASTRESOLUTION,	0, 1, "low"),
	KeywordHelp(kwDISPLAY_ZOOM_LON1,	0, 1, "-180"),
	KeywordHelp(kwDISPLAY_ZOOM_LON2,	0, 1, "180"),
	KeywordHelp(kwDISPLAY_ZOOM_LAT1,	0, 1, "-90"),
	KeywordHelp(kwDISPLAY_ZOOM_LAT2,	0, 1, "90"),
	KeywordHelp(kwDISPLAY_XMINVALUE,	0, 1, "min of data values for X axis"),
	KeywordHelp(kwDISPLAY_XMAXVALUE,	0, 1, "max of data values for X axis"),
	KeywordHelp(kwDISPLAY_YMINVALUE,	0, 1, "min of data values for Y axis"),
	KeywordHelp(kwDISPLAY_YMAXVALUE,	0, 1, "max of data values for Y axis"),
	KeywordHelp(kwDISPLAY_PLOT_TYPE,	0, 1, "Type of plot: 0 = YFX, 1 = ZFXY, 2 = ZFLatLon"),
	KeywordHelp(kwDISPLAY_XAXIS,		0, 24, "X axis name"),
	KeywordHelp(kwDISPLAY_XLABEL,		0, 24, "field title or field name"),
	KeywordHelp(kwDISPLAY_YAXIS,		0, 24, "Y axis name"),
	KeywordHelp(kwDISPLAY_YLABEL,		0, 1, "field title or field name"),
	KeywordHelp(kwDISPLAY_XTICKS,		0, 1, "6"),
	KeywordHelp(kwDISPLAY_YTICKS,		0, 1, "6"),
	KeywordHelp(kwDISPLAY_NAME,		0, 0, nullptr, -23),
	KeywordHelp(kwDISPLAY_OPACITY,		0, -23, "0.7"),
	KeywordHelp(kwDISPLAY_MINVALUE,		0, -23, "min of data values"),
	KeywordHelp(kwDISPLAY_MAXVALUE,		0, -23, "max of data values"),
	KeywordHelp(kwDISPLAY_NUMCOLORLABELS,	0, -23, "2"),
	KeywordHelp(kwDISPLAY_COLORTABLE,	0, -23, "Aerosol"),
	KeywordHelp(kwDISPLAY_COLORCURVE,	0, -23, "Linear"),
	KeywordHelp(kwDISPLAY_CONTOUR,		0, -23, "No"),
	KeywordHelp(kwDISPLAY_CONTOUR_NUMBER,	0, -23, "5"),
	KeywordHelp(kwDISPLAY_CONTOUR_LABEL,	0, -23, "No"),
	KeywordHelp(kwDISPLAY_CONTOUR_LABEL_NUMBER, 0, -23, "1"),
	KeywordHelp(kwDISPLAY_CONTOUR_MINVALUE,	0, -23, "min of data values"),
	KeywordHelp(kwDISPLAY_CONTOUR_MAXVALUE,	0, -23, "max of data values"),
	KeywordHelp(kwDISPLAY_SOLID_COLOR,	0, -23, "Yes"),
	KeywordHelp(kwDISPLAY_EAST_COMPONENT,	0, -23, "No"),
	KeywordHelp(kwDISPLAY_NORTH_COMPONENT,	0, -23, "No"),
	KeywordHelp(kwDISPLAY_COLOR,		0, -23, "randow color"),
	KeywordHelp(kwDISPLAY_POINTS,		0, -23, "No"),
	KeywordHelp(kwDISPLAY_LINES,		0, -23, "Yes"),
	KeywordHelp(kwDISPLAY_POINTSIZE,	0, -23, "1.0"),
	KeywordHelp(kwDISPLAY_LINEWIDTH,	0, -23, "0.8"),
	KeywordHelp(kwDISPLAY_STIPPLEPATTERN,	0, -23, "Full"),
	KeywordHelp(kwDISPLAY_POINTGLYPH,	0, -23, "Circle"),
	KeywordHelp(kwDISPLAY_POINTFILLED,	0, -23, "Yes"),

	KeywordHelp(kwALIAS_NAME,		0, 0, "None", 14),
	KeywordHelp(kwALIAS_VALUE,		-14),
	KeywordHelp(kwVERBOSE,			0, 1, "0"),
	KeywordHelp(""),
};


bool CDisplayFilesProcessor::GetCommandLineOptions( const std::vector< std::string > &args )
{
	struct arg_file *infiles = arg_file1( nullptr, nullptr, nullptr, "parameter file or netcdf file" );
	struct arg_str  *xAxis    = arg_str0( "xX", nullptr, nullptr, "specifies the x axis (only with netcdf file)" );
	struct arg_str  *yAxis   = arg_str0( "yY", nullptr, nullptr, "specifies the y axis (only with netcdf file)" );
	struct arg_str  *vars    = arg_str0( "vV", nullptr, "var,var2,varN...", "specifies the list of variables to display. (only with netcdf file)" );
	struct arg_lit  *help    = arg_lit0( "h", "help", "print this help and exit" );
	struct arg_lit  *keywords    = arg_lit0( "k", "keywords", "print list of allowed parameters in the parameter file" );
	struct arg_end  *end     = arg_end( 20 );

	void* argtable[] = { infiles, xAxis, yAxis, vars, help, keywords, end };

	std::string progname = q2a( QApplication::applicationName() );

	int nerrors;
	int i;

	/* verify the argtable[] entries were allocated sucessfully */
	if ( arg_nullcheck( argtable ) != 0 )
	{
		/* nullptr entries were detected, some allocations must have failed */
		CException e( "CDisplayFilesProcessor::GetCommandLineOptions - insufficient memory", BRATHL_UNIMPLEMENT_ERROR );
		LOG_TRACE( e.what() );
		throw e;
	}

	/* Parse the command line as defined by argtable[] */
	const int argc = args.size();

	std::vector<const char*> charv;
	for ( int i = 0; i < argc; ++i )
		charv.push_back( args[ i ].c_str() );
	nerrors = arg_parse( argc, const_cast<char**>( &charv[ 0 ] ), argtable );

	/* special case: '--help' takes precedence over error reporting */
	if ( ( help->count > 0 ) || ( argc == 1 ) )
	{
		printf( "Usage: %s", progname.c_str() );
		arg_print_syntax( stdout, argtable, "\n" );
		printf( "BratDisplay - An application to display BRAT netCDF data\n" );
		arg_print_glossary( stdout, argtable, "  %-25s %s\n" );

		/* deallocate each non-null entry in argtable[] */
		arg_freetable( argtable, sizeof( argtable ) / sizeof( argtable[ 0 ] ) );
		return false;
	}

    if ( keywords->count > 0 )
	{
		CBratProcess::PrintParameterHelp( std::cout, keywordList );

		/* deallocate each non-null entry in argtable[] */
		arg_freetable( argtable, sizeof( argtable ) / sizeof( argtable[ 0 ] ) );
		return false;
	}

	/* If the parser returned any errors then display them and exit */
	if ( nerrors > 0 )
	{
		/* Display the error details contained in the arg_end struct.*/
		arg_print_errors( stdout, end, progname.c_str() );
		printf( "Try '%s --help' for more information.\n", progname.c_str() );

		/* deallocate each non-null entry in argtable[] */
		arg_freetable( argtable, sizeof( argtable ) / sizeof( argtable[ 0 ] ) );
		return false;
	}



	for ( i = 0; i < infiles->count; i++ )
	{
		m_paramFile = infiles->filename[ i ];
	}

	for ( i = 0; i < xAxis->count; i++ )
	{
		m_paramXAxis = xAxis->sval[ i ];
	}

	for ( i = 0; i < yAxis->count; i++ )
	{
		m_paramYAxis = yAxis->sval[ i ];
	}

	for ( i = 0; i < vars->count; i++ )
	{
		m_paramVars.ExtractStrings( vars->sval[ i ], ',' );
	}


	/* deallocate each non-null entry in argtable[] */
	arg_freetable( argtable, sizeof( argtable ) / sizeof( argtable[ 0 ] ) );

	return true;
}






bool CDisplayFilesProcessor::Process( const std::vector< std::string > &args )
{
	if ( !GetCommandLineOptions( args ) )
		return false;

	// Init parameters with a netcdf input file

	if ( BuildPlotsFromNetcdf() )		//L:\project\workspaces\RCCC\Operations\CreateEnv_GDR_04_copy.nc
	{
		return true;
	}
										//L:\project\workspaces\RCCC\Displays\DisplayWaveforms_Lat_2D_Animated_Summer.par

	// Init parameters with a parameter input file

	m_params.Load( m_paramFile );
	m_params.SetVerboseLevel();
	m_params.Dump();

	return true;
}


bool CDisplayFilesProcessor::Process( const QStringList &args )
{
	std::vector< std::string > v;
	for ( auto &arg : args )
		v.push_back( q2a( arg ) );

	return Process( v );
}


void CDisplayFilesProcessor::BuildPlotsPrivate()
{
	assert__( m_params.IsLoaded() );


	////////////////////////////////////////////////////////////////////////////
	//	process kwFILE kwFIELD kwFIELD_GROUP and kwDISPLAY_PLOT_TYPE
	////////////////////////////////////////////////////////////////////////////

	// Check all input files

	mOperationOutputFiles.RemoveAll();
	CBratProcess::GetFileList( m_params, kwFILE, mOperationOutputFiles, "Input file", 1, -1 );
	CheckOperationOutputFiles();

	// Verify keyword occurrences

	const uint32_t nbExpr = m_params.CheckCount( kwFIELD, 1, 32 );	// max 32 because of class CBitSet32
	m_params.GetFieldNames( kwFIELD, mFieldNames );					assert__( mFieldNames.size() == nbExpr );

	// v4 not used: compute groups

	uint32_t groupNumberMax = 0;
	uint32_t groupNumberMin = 0;
    setDefaultValue( groupNumberMin );

	int32_t nbGroup = m_params.CheckCount( kwFIELD_GROUP, 0, nbExpr );
	if ( nbGroup != 0 )
	{
		m_params.CheckCount( kwFIELD_GROUP, nbExpr, nbExpr );		// why not nbGroup ?

		for ( size_t index = 0; index < nbExpr; index++ )
		{
			uint32_t groupNumber;
			m_params.m_mapParam[ kwFIELD_GROUP ]->GetValue( groupNumber, index );
			groupNumberMax = ( groupNumber > groupNumberMax ) ? groupNumber : groupNumberMax;
			groupNumberMin = ( groupNumber < groupNumberMin ) ? groupNumber : groupNumberMin;
		}
	}


	// get mDisplayType

	const uint32_t nPlotType = m_params.CheckCount( kwDISPLAY_PLOT_TYPE, 0, 1 );		assert__( nPlotType > 0 );			UNUSED( nPlotType );
	uint32_t plotType;
	m_params.m_mapParam[ kwDISPLAY_PLOT_TYPE ]->GetValue( plotType );
	mDisplayType = static_cast< decltype(mDisplayType) >( plotType );


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
		GetXYPlotPropertyParams( nbExpr );
	}

	if ( m_isZFLatLon )
	{
		GetWPlotPropertyParams( nbExpr );
	}

	if ( m_isZFXY )
	{
		GetZFXYPlotPropertyParams( nbExpr );
	}

	m_fields.resize( nbExpr );

	for ( uint32_t index = 0; index < nbExpr; index++ )
	{
		CExpression expr;

		uint32_t groupNumber;
		CBratProcess::GetVarDef( m_params,
			kwFIELD,
			expr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			&groupNumber,
			"Data Field",
			index,
			nbExpr );


		if ( expr.GetFieldNames()->size() != 1 )
		{
			std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParameters - Expression '%s' has incorrect number of fields '%ld' (correct is 1)",
				expr.AsString().c_str(), (long)expr.GetFieldNames()->size() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			LOG_TRACE( e.what() );
			throw ( e );
		}

		const std::string fieldName = expr.GetFieldNames()->at( 0 );

		// ------------------------------
		// -------------- YFX plot
		// ------------------------------
		if ( m_isYFX )
		{
			auto *props = GetXYPlotProperties( index );

			// Add field to xy plot group
			CYFXPlot* plot = dynamic_cast<CYFXPlot*>( m_plots[ groupNumber ] );
			if ( plot == nullptr )
			{
				plot = new CYFXPlot( nullptr, groupNumber );
				m_plots[ groupNumber ] = plot;
			}
			plot->PushFieldData( props );

			std::string xAxisName;
			uint32_t nXAxisName = m_params.CheckCount( kwDISPLAY_XAXIS, 0, nbGroup );

			if ( nXAxisName > 0 )
			{
				m_params.m_mapParam[ kwDISPLAY_XAXIS ]->GetValue( xAxisName, groupNumber - groupNumberMin, "" );
			}

			if ( !xAxisName.empty() )
			{
				plot->SetForcedVarXname( xAxisName );

				std::string xAxisLabel;
				uint32_t nXAxisLabel = m_params.CheckCount( kwDISPLAY_XLABEL, 0, nbGroup );
				if ( nXAxisLabel > 0 )
				{
					m_params.m_mapParam[ kwDISPLAY_XLABEL ]->GetValue( xAxisLabel, groupNumber - groupNumberMin, xAxisName );
				}

#if defined(BRAT_V3)
				plot->m_titleX = xAxisLabel;
#endif
			}

			CInternalFiles* yfx = dynamic_cast<CInternalFiles*>( Prepare( index, fieldName ) );

			props->PushInternalFile( yfx );			//field->m_xyProps = GetXYPlotProperties( index );
		}

		// ------------------------------
		// -------------- World plot
		// ------------------------------
		if ( m_isZFLatLon )
		{
			CWorldPlotProperties *props = GetWorldPlotProperties( index );

			// Add field to world plot group
			CGeoPlot* wplot = dynamic_cast<CGeoPlot*>( m_plots[ groupNumber ] );
			if ( wplot == nullptr )
			{
				wplot = new CGeoPlot( nullptr, groupNumber );
				m_plots[ groupNumber ] = wplot;
			}

			if ( !props->WithContour() && !props->WithSolidColor() )
			{
				props->SetWithSolidColor( true );
			}

			CFieldData *field = nullptr;

#if defined (BRAT_V3)
			if ( props->m_withAnimation )
			{
				//group files by field
				// find an already existing field with the same name
				// and the same contour flag
				// (don't animate same named fields which with different contour flag)
				bool with_contour = props->WithContour(), with_solid_color = props->WithSolidColor();
				field = wplot->FindFieldData( fieldName, &with_contour, &with_solid_color );
			}
#endif
			if ( field == nullptr )
			{
				wplot->PushFieldData( props );
			}

			CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>( Prepare( index, fieldName ) );

			props->PushInternalFile( zfxy );			//field->m_worldProps = worldProps;
		}

		// ------------------------------
		// -------------- ZFXY plot
		// ------------------------------

		if ( m_isZFXY )
		{
			auto *props = GetZFXYPlotProperties( index );

			// Add field to ZFXY plot group
			CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( m_plots[ groupNumber ] );
			if ( zfxyplot == nullptr )
			{
				zfxyplot = new CZFXYPlot( nullptr, groupNumber );
				m_plots[ groupNumber ] = zfxyplot;
			}

			if ( !props->WithContour() && !props->WithSolidColor() )
			{
				props->SetWithSolidColor( true );
			}

			CFieldData *field = nullptr;

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


			std::string xAxisName;
			uint32_t nXAxisName = m_params.CheckCount( kwDISPLAY_XAXIS, 0, nbGroup );
			if ( nXAxisName > 0 )
			{
				m_params.m_mapParam[ kwDISPLAY_XAXIS ]->GetValue( xAxisName, groupNumber - groupNumberMin, "" );
				if ( !xAxisName.empty() )
				{
					zfxyplot->SetForcedVarXname( xAxisName );
				}
			}

			std::string yAxisName;
			uint32_t nYAxisName = m_params.CheckCount( kwDISPLAY_YAXIS, 0, nbGroup );
			if ( nYAxisName > 0 )
			{
				m_params.m_mapParam[ kwDISPLAY_YAXIS ]->GetValue( yAxisName, groupNumber - groupNumberMin, "" );
				if ( !yAxisName.empty() )
				{
					zfxyplot->SetForcedVarYname( yAxisName );
				}
			}

			CInternalFiles* f = dynamic_cast<CInternalFiles*>( Prepare( index, fieldName ) );

			props->PushInternalFile( f );			//field->m_zfxyProps = zfxyProps;
		}

		m_fields[ index ] = expr;
	}
}


template< class PROPS_CONTAINER >
void CDisplayFilesProcessor::GetPlotPropertyParams4all( size_t nFields, PROPS_CONTAINER &data )
{
	std::string string_value;
	CFieldData::unsigned_t opacity_value = 0;
	bool bool_value;
	double double_value;

	//v4
	unsigned field_name = m_params.CheckCount( kwFIELD, 0, nFields );
	for ( unsigned i = 0; i < field_name; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];					assert__( props );
		m_params.m_mapParam[ kwFIELD ]->GetValue( string_value, i );		assert__( !string_value.empty() );
		props->SetFieldName( string_value );
	}


	unsigned name = m_params.CheckCount( kwDISPLAY_NAME, 0, nFields );
	for ( unsigned i = 0; i < name; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];					assert__( props );
		m_params.m_mapParam[ kwDISPLAY_NAME ]->GetValue( string_value, i );		assert__( !string_value.empty() );
		props->SetUserName( string_value );
	}


	unsigned opacity = m_params.CheckCount( kwDISPLAY_OPACITY, 0, nFields );
	for ( unsigned i = 0; i < opacity; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];					assert__( props );
		m_params.m_mapParam[ kwDISPLAY_OPACITY ]->GetValue( opacity_value, i, CFieldData::smDefaultOpacity );
		props->SetOpacity( opacity_value );
	}


	// YFX

	unsigned lines = m_params.CheckCount( kwDISPLAY_LINES, 0, nFields );
	for ( unsigned i = 0; i < lines; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];					assert__( props );
		m_params.m_mapParam[ kwDISPLAY_LINES ]->GetValue( bool_value, i );
		props->SetLines( bool_value );
	}

	unsigned indexPointSize = 0;
	unsigned indexPointGlyph = 0;
	unsigned indexPointFilled = 0;
	unsigned pointsCount = m_params.CheckCount( kwDISPLAY_POINTS, 0, nFields );
	unsigned pointSizeCount = m_params.CheckCount( kwDISPLAY_POINTSIZE, 0, nFields );
	unsigned pointGlyphCount = m_params.CheckCount( kwDISPLAY_POINTGLYPH, 0, nFields );
	unsigned pointFilledCount = m_params.CheckCount( kwDISPLAY_POINTFILLED, 0, nFields );
	for ( unsigned i = 0; i < pointsCount; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];					assert__( props );
		m_params.m_mapParam[ kwDISPLAY_POINTS ]->GetValue( bool_value, i );
		props->SetPoints( bool_value );

		//------------------------------------------
		// Get Pointsize Property for each plot
		//------------------------------------------
		if ( bool_value )  // points representation
		{
			if ( indexPointSize < pointSizeCount )
			{
				m_params.m_mapParam[ kwDISPLAY_POINTSIZE ]->GetValue( double_value, indexPointSize, CFieldData::smDefaultPointSize );
				props->SetPointSize( double_value );
				indexPointSize++;
			}
			if ( indexPointGlyph < pointGlyphCount )
			{
				EPointGlyph pointGlyph = CMapPointGlyph::GetInstance().GetPointGlyph( m_params, indexPointGlyph );
				props->SetPointGlyph( pointGlyph );
				indexPointGlyph++;
			}
			if ( indexPointFilled < pointFilledCount )
			{
				m_params.m_mapParam[ kwDISPLAY_POINTFILLED ]->GetValue( bool_value, indexPointFilled, CFieldData::smDefaultFilledPoint );
				props->SetFilledPoint( bool_value );
				indexPointFilled++;
			}
		}
	}

	unsigned color = m_params.CheckCount( kwDISPLAY_COLOR, 0, nFields );
	for ( unsigned i = 0; i < color; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];					assert__( props );
		m_params.m_mapParam[ kwDISPLAY_COLOR ]->GetValue( string_value, i );
		CPlotColor vtkColor = CMapColor::GetInstance().NameToPlotColor( string_value );
		if ( !vtkColor.Ok() )
		{
			vtkColor = CMapColor::GetInstance().NextPrimaryColors();
		}
		props->SetLineColor( vtkColor );
	}

	//
	// v4 NOTE: point color missing here
	//

	unsigned lineWidth = m_params.CheckCount( kwDISPLAY_LINEWIDTH, 0, nFields );
	for ( unsigned i = 0; i < lineWidth; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];					assert__( props );
		m_params.m_mapParam[ kwDISPLAY_LINEWIDTH ]->GetValue( double_value, i, 0.8 );
		props->SetLineWidth( double_value );
	}

	unsigned stipplePattern = m_params.CheckCount( kwDISPLAY_STIPPLEPATTERN, 0, nFields );
	for ( unsigned i = 0; i < stipplePattern; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];					assert__( props );
		EStipplePattern stipplePatternValue = CMapStipplePattern::GetInstance().GetStipplePattern( m_params, i );	//v4 always returns default value
#if defined(BRAT_V3)
		props->SetLineStipple( stipplePatternValue );
#else
		props->SetStipplePattern( stipplePatternValue );
#endif
	}



	// ZFXY / LON-LAT

	unsigned colorCurve = m_params.CheckCount( kwDISPLAY_COLORCURVE, 0, nFields );
	for ( unsigned i = 0; i < colorCurve; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];					assert__( props );
		m_params.m_mapParam[ kwDISPLAY_COLORCURVE ]->GetValue( string_value, i );

		std::string stringValueOk = props->ColorTable().CurveToLabeledCurve( string_value );
		if ( stringValueOk.empty() )
		{
			throw  CParameterException( CTools::Format( "Unknown color curve name value '%s' for  parameter '%s'",
				string_value.c_str(),
				kwDISPLAY_COLORCURVE.c_str() ),
				BRATHL_SYNTAX_ERROR );

		}
		props->SetColorCurve( stringValueOk );
	}

	unsigned colorTable = m_params.CheckCount( kwDISPLAY_COLORTABLE, 0, nFields );
	for ( unsigned i = 0; i < colorTable; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];					assert__( props );
		m_params.m_mapParam[ kwDISPLAY_COLORTABLE ]->GetValue( string_value, i, PALETTE_AEROSOL );

		std::string stringValueOk = props->ColorTable().MethodToLabeledMethod( string_value );

		if ( stringValueOk.empty() )
		{
			if ( !IsFile( string_value ) )
			{
				throw  CParameterException( CTools::Format( "Unknown color table name value or color file name '%s' for  parameter '%s'",
					string_value.c_str(),
					kwDISPLAY_COLORTABLE.c_str() ),
					BRATHL_SYNTAX_ERROR );
			}
			else
			{
				// load the color table file definition
				std::string error_msg;
				try
				{
					props->LoadColorTable( error_msg, string_value );		//fileName.GetFullPath() 
				}
				catch ( CException& e )
				{
					SimpleWarnBox( e.what() );
				}
				catch ( ... )
				{
					if (!error_msg.empty())
						SimpleErrorBox( error_msg );
					else
						SimpleErrorBox( "Can't load " + string_value );
				}
			}
		}
		else  //stringValueOk is not empty, it's a predefined color
		{
			props->SetColorPalette( stringValueOk );
		}
	}

	double m, M;

	unsigned maxHeigth = m_params.CheckCount( kwDISPLAY_MAXVALUE, 0, nFields );
    unsigned minHeigth = m_params.CheckCount( kwDISPLAY_MINVALUE, 0, nFields );		assert__( minHeigth == maxHeigth );     UNUSED(maxHeigth);
	for ( unsigned i = 0; i < minHeigth; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];						assert__( props );
		m_params.m_mapParam[ kwDISPLAY_MINVALUE ]->GetValue( m, i );
		m_params.m_mapParam[ kwDISPLAY_MAXVALUE ]->GetValue( M, i );
		props->SetAbsoluteRangeValues( m, M );
	}

	int int_value;

	unsigned contourMinValue = m_params.CheckCount( kwDISPLAY_CONTOUR_MINVALUE, 0, nFields );
    unsigned contourMaxValue = m_params.CheckCount( kwDISPLAY_CONTOUR_MAXVALUE, 0, nFields );		assert__( contourMinValue == contourMaxValue );     UNUSED(contourMaxValue);
	for ( unsigned i = 0; i < contourMinValue; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];						assert__( props );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_MINVALUE ]->GetValue( m, i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_MAXVALUE ]->GetValue( M, i );
		props->SetContourValueRange( m, M );
	}


	unsigned contour = m_params.CheckCount( kwDISPLAY_CONTOUR, 0, nFields );
	for ( unsigned i = 0; i < contour; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];						assert__( props );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR ]->GetValue( bool_value, i );
		props->SetWithContour( bool_value );
	}
	unsigned contourNum = m_params.CheckCount( kwDISPLAY_CONTOUR_NUMBER, 0, nFields );
	for ( unsigned i = 0; i < contourNum; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];						assert__( props );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_NUMBER ]->GetValue( int_value, i );
		props->SetNumContours( int_value );
	}

	unsigned solidColor = m_params.CheckCount( kwDISPLAY_SOLID_COLOR, 0, nFields );
	for ( unsigned i = 0; i < solidColor; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];						assert__( props );
		m_params.m_mapParam[ kwDISPLAY_SOLID_COLOR ]->GetValue( bool_value, i );
		props->SetWithSolidColor( bool_value );
	}

	unsigned eastComponent = m_params.CheckCount( kwDISPLAY_EAST_COMPONENT, 0, nFields );
	for ( unsigned i = 0; i < eastComponent; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];						assert__( props );
		m_params.m_mapParam[ kwDISPLAY_EAST_COMPONENT ]->GetValue( bool_value, i );
		props->SetEastComponent( bool_value );
	}

	unsigned northComponent = m_params.CheckCount( kwDISPLAY_NORTH_COMPONENT, 0, nFields );
	for ( unsigned i = 0; i < northComponent; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];						assert__( props );
		m_params.m_mapParam[ kwDISPLAY_NORTH_COMPONENT ]->GetValue( bool_value, i );
		props->SetNorthComponent( bool_value );
	}
}



template< class UGGLY, typename T >
inline bool ReadParamValue( UGGLY &uggly, const std::string &key, T &value )
{
	unsigned count = uggly.CheckCount( key, 0, 1 );
	if ( count != 0 )
	{
		uggly.m_mapParam[ key ]->GetValue( value );
	}
	return count != 0;
}


void CDisplayFilesProcessor::GetPlotPropertyCommonParams( size_t nFields, CFieldData &data )
{
	if ( (int)nFields <= 0 )
	{
		throw  CParameterException( "CDisplayFilesProcessor::GetPlotPropertyParams - There is no field to plot", BRATHL_INCONSISTENCY_ERROR );
	}

	std::string string_value;

#if defined (BRAT_V3)
	if ( ReadParamValue( m_params, kwDISPLAY_PROPERTIES, boolValue ) )
		data.SetShowPropertyPanel( boolValue );

	if ( ReadParamValue( m_params, kwDISPLAY_TITLE, string_value ) )
		data.SetTitle( string_value );
#else
    UNUSED( data );
#endif

#if defined(BRAT_V3)
	unsigned uint_value;
	double double_value;

	if ( ReadParamValue( m_params, kwDISPLAY_XMINVALUE, double_value ) )
		data.SetXmin( double_value );

	if ( ReadParamValue( m_params, kwDISPLAY_XMAXVALUE, double_value ) )
		data.SetXmax( double_value );

	if ( ReadParamValue( m_params, kwDISPLAY_YMINVALUE, double_value ) )
		data.SetYmin( double_value );

	if ( ReadParamValue( m_params, kwDISPLAY_YMAXVALUE, double_value ) )
		data.SetYmax( double_value );

	//if ( ReadParamValue( m_params, kwDISPLAY_XLABEL, string_value ) )	processed elsewhere
	//	data.SetXLabel( string_value );

	if ( ReadParamValue( m_params, kwDISPLAY_YLABEL, string_value ) )
		data.SetYlabel( string_value );

	if ( ReadParamValue( m_params, kwDISPLAY_XTICKS, uint_value ) )
		data.SetXticks( uint_value );

	if ( ReadParamValue( m_params, kwDISPLAY_YTICKS, uint_value ) )
		data.SetYticks( uint_value );
#endif
}



void CDisplayFilesProcessor::GetXYPlotPropertyParams( size_t nFields )
{
	//------------------------------------------
	// Get common property (same for each plot)
	//------------------------------------------

	CXYPlotProperties xyPlotProperty;
	GetPlotPropertyCommonParams( nFields, xyPlotProperty );

	//v4 note: apparently, all properties above are common; from now on property values are per field

	//------------------------------------------
	// Fill plot properties array (a color is set by default for each property)
	//------------------------------------------
	for ( unsigned i = 0; i < nFields; i++ )
	{
		m_xyPlotProperties.push_back( new CXYPlotProperties( xyPlotProperty ) );
	}
	//------------------------------------------
	// Get Name Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get Opacity Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get Lines Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get Points Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get Color Property for each plot
	//------------------------------------------
	//
	// v4 NOTE: point color missing here
	//
	//------------------------------------------
	// Get Line Width Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get Line Stipple Pattern Properties for each plot
	//------------------------------------------
	GetPlotPropertyParams4all( nFields, m_xyPlotProperties );
}


void CDisplayFilesProcessor::GetZFXYPlotPropertyParams( size_t nFields )
{
	CZFXYPlotProperties zfxyPlotProperty;
	GetPlotPropertyCommonParams( nFields, zfxyPlotProperty );

	int32_t i = 0;
	std::string stringValue;

#if defined (BRAT_V3)
	bool boolValue;
	int32_t intValue;
	int32_t showProp = m_params.CheckCount( kwDISPLAY_PROPERTIES, 0, 1 );
	if ( showProp != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_PROPERTIES ]->GetValue( boolValue );
		zfxyPlotProperty.m_showPropertyPanel = boolValue;
	}
	int32_t showColorBar = m_params.CheckCount( kwDISPLAY_COLORBAR, 0, 1 );
	if ( showColorBar != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_COLORBAR ]->GetValue( boolValue );
		zfxyPlotProperty.m_showColorBar = boolValue;
	}
	int32_t showAnimationBar = m_params.CheckCount( kwDISPLAY_ANIMATIONBAR, 0, 1 );
	if ( showAnimationBar != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_ANIMATIONBAR ]->GetValue( boolValue );
		zfxyPlotProperty.m_showAnimationToolbar = boolValue;
	}

	int32_t withAnimation = m_params.CheckCount( kwDISPLAY_ANIMATION, 0, 1 );
	if ( withAnimation != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_ANIMATION ]->GetValue( boolValue );
		zfxyPlotProperty.m_withAnimation = boolValue;

		// if 'withAnimation' is true, force animation bar to be shown.
		if ( zfxyPlotProperty.m_withAnimation )
		{
			zfxyPlotProperty.m_showAnimationToolbar = zfxyPlotProperty.m_withAnimation;
		}
	}

#endif

	for ( i = 0; i < nFields; i++ )
	{
		m_zfxyPlotProperties.push_back( new CZFXYPlotProperties( zfxyPlotProperty ) );
	}


#if defined (BRAT_V3)
	//------------------------------------------
	// Get contour Label Property for each plot
	//------------------------------------------
	int32_t contourLabel = m_params.CheckCount( kwDISPLAY_CONTOUR_LABEL, 0, nFields );
	for ( i = 0; i < contourLabel; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_LABEL ]->GetValue( boolValue, i );
		props->m_withContourLabel = boolValue;
	}
	//------------------------------------------
	// Get number of contour label Property for each plot
	//------------------------------------------
	int32_t contourLabelNumber = m_params.CheckCount( kwDISPLAY_CONTOUR_LABEL_NUMBER, 0, nFields );
	for ( i = 0; i < contourLabelNumber; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_LABEL_NUMBER ]->GetValue( intValue, i );
		props->m_numContourLabel = intValue;
	}
#endif

	//------------------------------------------
	// Get Name Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get opacity Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get color Curve Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get color table Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get max value Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get min value Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get number of color labels Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get contour min value Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get contour max value Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get contour Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get number of contours Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get solid color Property for each plot
	//------------------------------------------

	GetPlotPropertyParams4all( nFields, m_zfxyPlotProperties );
}


void CDisplayFilesProcessor::GetWPlotPropertyParams( size_t nFields )
{
	CWorldPlotProperties wPlotProperty;
	GetPlotPropertyCommonParams( nFields, wPlotProperty );

#if defined (BRAT_V3)
	int32_t i = 0;
	double doubleValue;
	std::string stringValue;

	int32_t centerLat = m_params.CheckCount( kwDISPLAY_CENTERLAT, 0, 1 );
	int32_t centerLon = m_params.CheckCount( kwDISPLAY_CENTERLON, 0, 1 );
	int32_t projection = m_params.CheckCount( kwDISPLAY_PROJECTION, 0, 1 );
	int32_t coastResolution = m_params.CheckCount( kwDISPLAY_COASTRESOLUTION, 0, 1 );
	int32_t zoomLon1 = m_params.CheckCount( kwDISPLAY_ZOOM_LON1, 0, 1 );
	int32_t zoomLon2 = m_params.CheckCount( kwDISPLAY_ZOOM_LON2, 0, 1 );
	int32_t zoomLat1 = m_params.CheckCount( kwDISPLAY_ZOOM_LAT1, 0, 1 );
	int32_t zoomLat2 = m_params.CheckCount( kwDISPLAY_ZOOM_LAT2, 0, 1 );

	if ( centerLat != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_CENTERLAT ]->GetValue( doubleValue );
		wPlotProperty.m_centerLatitude = doubleValue;
	}
	if ( centerLon != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_CENTERLON ]->GetValue( doubleValue );
		wPlotProperty.m_centerLongitude = doubleValue;
	}
	if ( projection != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_PROJECTION ]->GetValue( stringValue );
		wPlotProperty.m_projection = CMapProjection::GetInstance()->NameToLabeledName( stringValue );
		if ( wPlotProperty.m_projection.empty() )
		{
			throw  CParameterException( CTools::Format( "Unknown projection name value '%s' for  parameter '%s'",
				stringValue.c_str(),
				kwDISPLAY_PROJECTION.c_str() ),
				BRATHL_SYNTAX_ERROR );

		}
	}
	if ( coastResolution != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_COASTRESOLUTION ]->GetValue( stringValue );
		wPlotProperty.m_coastResolution = CTools::StringToLower( stringValue );
	}
	if ( zoomLon1 != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_ZOOM_LON1 ]->GetValue( doubleValue );
		wPlotProperty.m_zoomLon1 = doubleValue;
	}
	if ( zoomLon2 != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_ZOOM_LON2 ]->GetValue( doubleValue );
		wPlotProperty.m_zoomLon2 = doubleValue;
	}
	if ( zoomLat1 != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_ZOOM_LAT1 ]->GetValue( doubleValue );
		wPlotProperty.m_zoomLat1 = doubleValue;
	}
	if ( zoomLat2 != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_ZOOM_LAT2 ]->GetValue( doubleValue );
		wPlotProperty.m_zoomLat2 = doubleValue;
	}
	if ( ( zoomLon1 != 0 ) || ( zoomLon2 != 0 ) || ( zoomLat1 != 0 ) || ( zoomLat2 != 0 ) )
	{
		wPlotProperty.m_zoom = true;
	}


	bool boolValue;
	int32_t intValue;

	int32_t showProp = m_params.CheckCount( kwDISPLAY_PROPERTIES, 0, 1 );
	if ( showProp != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_PROPERTIES ]->GetValue( boolValue );
		wPlotProperty.m_showPropertyPanel = boolValue;
	}

	int32_t showColorBar = m_params.CheckCount( kwDISPLAY_COLORBAR, 0, 1 );
	if ( showColorBar != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_COLORBAR ]->GetValue( boolValue );
		wPlotProperty.m_showColorBar = boolValue;
	}
	int32_t showAnimationBar = m_params.CheckCount( kwDISPLAY_ANIMATIONBAR, 0, 1 );
	if ( showAnimationBar != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_ANIMATIONBAR ]->GetValue( boolValue );
		wPlotProperty.m_showAnimationToolbar = boolValue;
	}
	int32_t withAnimation = m_params.CheckCount( kwDISPLAY_ANIMATION, 0, 1 );
	if ( withAnimation != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_ANIMATION ]->GetValue( boolValue );
		wPlotProperty.m_withAnimation = boolValue;

		// if 'withAnimation' is true, force animation bar to be shown.
		if ( wPlotProperty.m_withAnimation )
		{
			wPlotProperty.m_showAnimationToolbar = wPlotProperty.m_withAnimation;
		}
	}
#endif


	for ( size_t i = 0; i < nFields; i++ )
	{
		m_wPlotProperties.push_back( new CWorldPlotProperties( wPlotProperty ) );
	}

#if defined (BRAT_V3)
	//------------------------------------------
	// Get contour Label Property for each plot
	//------------------------------------------
	int32_t contourLabel = m_params.CheckCount( kwDISPLAY_CONTOUR_LABEL, 0, nFields );
	for ( i = 0; i < contourLabel; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_LABEL ]->GetValue( boolValue, i );
		props->m_withContourLabel = boolValue;
	}
	//------------------------------------------
	// Get number of contour label Property for each plot
	//------------------------------------------
	int32_t contourLabelNumber = m_params.CheckCount( kwDISPLAY_CONTOUR_LABEL_NUMBER, 0, nFields );
	for ( i = 0; i < contourLabelNumber; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_LABEL_NUMBER ]->GetValue( intValue, i );
		props->m_numContourLabel = intValue;
	}

	//------------------------------------------
	// Get number of color labels Property for each plot
	//------------------------------------------
	unsigned numColorLabels = m_params.CheckCount( kwDISPLAY_NUMCOLORLABELS, 0, nFields );
	for ( unsigned i = 0; i < numColorLabels; i++ )
	{
		typename PROPS_CONTAINER::value_type props = data[ i ];						assert__( props );
		m_params.m_mapParam[ kwDISPLAY_NUMCOLORLABELS ]->GetValue( int_value, i );
		props->mNumColorLabels = int_value;
	}

#endif

	//------------------------------------------
	// Get Name Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get opacity Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get color Curve Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get color table Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get max value Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get min value Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get contour min value Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get contour max value Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get contour Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get number of contours Property for each plot
	//------------------------------------------
	//------------------------------------------
	// Get solid color Property for each plot
	//------------------------------------------
	//-------------------------------------------------------------
	// Are the plots vector components? Just one east and one north
	//-------------------------------------------------------------

	GetPlotPropertyParams4all( nFields, m_wPlotProperties );
}


