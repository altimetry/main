#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/QtUtilsIO.h"

#include "libbrathl/Tools.h"

#include "ApplicationLogger.h"
#include "libbrathl/InternalFilesFactory.h"
#include "libbrathl/InternalFilesZFXY.h"
#include "libbrathl/InternalFilesYFX.h"
#include "libbrathl/brathl_c/argtable2.h"

#include "process/ParametersDictionary.h"
#include "process/BratProcess.h"

#include "MapTypeDisp.h"
#include "PlotData/ZFXYPlot.h"
#include "PlotData/ZFXYPlotData.h"
#include "PlotData/XYPlot.h"
#include "PlotData/XYPlotData.h"
#include "PlotData/WorldPlot.h"
#include "PlotData/WorldPlotData.h"

#include "DataModels/PlotData/ColorPalleteNames.h"
#include "DataModels/PlotData/MapProjection.h"
#if defined (BRAT_V3)
#include "display/PlotData/BratLookupTable.h"
#endif



#include "DisplayFilesProcessor.h"


using namespace processes;



static const KeywordHelp keywordList[]	= {
	KeywordHelp(kwFILE,			1, 0),
	KeywordHelp(kwFIELD,			1, 23, NULL, 24),
	KeywordHelp(kwFIELD_GROUP,		-24),

	KeywordHelp(kwDISPLAY_PROPERTIES,	0, 1, "No"),
	KeywordHelp(kwDISPLAY_TITLE,		0, 1, "\"\""),
	KeywordHelp(kwDISPLAY_ANIMATIONBAR,	0, 1, "No"),
	KeywordHelp(kwDISPLAY_COLORBAR,		0, 1, "Yes"),
	KeywordHelp(kwDISPLAY_CENTERLAT,	0, 1, "0"),
	KeywordHelp(kwDISPLAY_CENTERLON,	0, 1, "0"),
	KeywordHelp(kwDISPLAY_PROJECTION,	0, 1, "3D"),
	KeywordHelp(kwDISPLAY_COASTRESOLUTION,	0, 1, "low"),
	KeywordHelp(kwDISPLAY_ZOOM_LON1,	0, 1, "-180"),
	KeywordHelp(kwDISPLAY_ZOOM_LON2,	0, 1, "180"),
	KeywordHelp(kwDISPLAY_ZOOM_LAT1,	0, 1, "-90"),
	KeywordHelp(kwDISPLAY_ZOOM_LAT2,	0, 1, "90"),
	KeywordHelp(kwDISPLAY_ANIMATION,	0, 1, "No"),
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
	KeywordHelp(kwDISPLAY_NAME,		0, 0, NULL, -23),
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




//virtual 
CDisplayFilesProcessor::~CDisplayFilesProcessor()
{
	for ( auto *f : m_OpenFiles )
		f->Close();
}


bool CDisplayFilesProcessor::Process( const std::vector< std::string > &args )
{
	if ( !GetCommandLineOptions( args ) )
		return false;

	GetParameters();

	return true;
}


bool CDisplayFilesProcessor::Process( const QStringList &args )
{
	std::vector< std::string > v;
	for ( auto &arg : args )
		v.push_back( q2a( arg ) );

	return Process( v );
}


bool CDisplayFilesProcessor::GetCommandLineOptions( const std::vector< std::string > &args )
{
	struct arg_file *infiles = arg_file1( NULL, NULL, NULL, "parameter file or netcdf file" );
	struct arg_str  *xAxis    = arg_str0( "xX", NULL, NULL, "specifies the x axis (only with netcdf file)" );
	struct arg_str  *yAxis   = arg_str0( "yY", NULL, NULL, "specifies the y axis (only with netcdf file)" );
	struct arg_str  *vars    = arg_str0( "vV", NULL, "var,var2,varN...", "specifies the list of variables to display. (only with netcdf file)" );
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
		/* NULL entries were detected, some allocations must have failed */
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




void CDisplayFilesProcessor::LoadParameters()
{
	if ( !m_params.IsLoaded() )
	{
		m_params.Load( m_paramFile );
	}
}

void CDisplayFilesProcessor::CheckFiles()
{

	CStringArray::iterator it;
	std::string inputFileTypeRead;

	if ( m_inputFiles.empty() )
	{
		CException e( "CDisplayFilesProcessor::CheckFiles - input data file list is empty()", BRATHL_COUNT_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );
	}


	/*
	  if (m_inputFiles.size() == 1)
	  {
	  m_inputFileType = CNetCDFFiles::IdentifyExistingFile(*(m_inputFiles.begin()));
	  return;
	  }
	  */

	m_inputFileType = "";


	for ( it = m_inputFiles.begin(); it != m_inputFiles.end(); it++ )
	{
		inputFileTypeRead = CNetCDFFiles::IdentifyExistingFile( *it );

		if ( it != m_inputFiles.begin() )
		{
			if ( m_inputFileType.compare( inputFileTypeRead ) != 0 )
			{
				std::string msg = CTools::Format( "CDisplayFilesProcessor::CheckFiles - Files are not in the same way - Expected type '%s' and found '%s' for file '%s'",
					m_inputFileType.c_str(), inputFileTypeRead.c_str(), ( *it ).c_str() );
				CException e( msg, BRATHL_INCONSISTENCY_ERROR );
				LOG_TRACE( e.what() );
				throw ( e );

			}
		}
		m_inputFileType = inputFileTypeRead;
	}

}

std::string CDisplayFilesProcessor::GetFirstFileName()
{
	LoadParameters();

	return m_params.GetFirstFile( kwFILE );

}

bool CDisplayFilesProcessor::IsXYPlot()
{
	std::string name = GetFirstFileName();

	return CInternalFiles::IsYFXFile( name );
}

bool CDisplayFilesProcessor::IsWPlot()
{
	std::string name = GetFirstFileName();

	return !mMapsAsPlots && CInternalFiles::IsZFLatLonFile( name );
}
bool CDisplayFilesProcessor::IsZXYPlot()
{
	LoadParameters();

	CStringArray fieldNames;

	m_params.GetFieldNames( kwFIELD, fieldNames );

	std::string name = GetFirstFileName();

	return ( mMapsAsPlots && CInternalFiles::IsZFLatLonFile( name ) ) || CInternalFiles::IsZFXYFile( name, &fieldNames );
}


void CDisplayFilesProcessor::GetParameters()
{
	//--------------------------------------------
	// Init parameters with a netcdf input file
	//--------------------------------------------
	if ( GetParametersNetcdf() )
	{
		//---------
		return;
		//---------
	}

	// Init parameters with a parameter input file
	uint32_t index = 0;

	LoadParameters();

	m_params.SetVerboseLevel();
	m_params.Dump();


	// Verify keyword occurences
	uint32_t nbExpr = m_params.CheckCount( kwFIELD, 1, 32 ); // max 32 because of class CBitSet32


	// Get keyword values
	m_inputFiles.RemoveAll();
	CBratProcess::GetFileList( m_params, kwFILE, m_inputFiles, "Input file", 1, -1 );

	// Check all input files
	CheckFiles();

	uint32_t groupNumber;
	uint32_t groupNumberMax = 0;
	uint32_t groupNumberMin = 0;
    setDefaultValue( groupNumberMin );

	int32_t nbGroup = m_params.CheckCount( kwFIELD_GROUP, 0, nbExpr );
	if ( nbGroup != 0 )
	{
		m_params.CheckCount( kwFIELD_GROUP, nbExpr, nbExpr );

		for ( index = 0; index < nbExpr; index++ )
		{
			m_params.m_mapParam[ kwFIELD_GROUP ]->GetValue( groupNumber, index );
			groupNumberMax = ( groupNumber > groupNumberMax ) ? groupNumber : groupNumberMax;
			groupNumberMin = ( groupNumber < groupNumberMin ) ? groupNumber : groupNumberMin;
		}
	}

	// get type of plot
	m_isYFX = IsXYPlot();
	m_isZFLatLon = IsWPlot();
	m_isZFXY = IsZXYPlot();

	if ( m_isYFX && m_isZFXY )
	{
		uint32_t nPlotType = m_params.CheckCount( kwDISPLAY_PLOT_TYPE, 0, 1 );
		if ( nPlotType > 0 )
		{
			uint32_t plotType;
			m_params.m_mapParam[ kwDISPLAY_PLOT_TYPE ]->GetValue( plotType );
			if ( plotType == CMapTypeDisp::eTypeDispYFX )
			{
				m_isZFXY = false;
			}
			else
			{
				m_isYFX = false;
			}
		}
	}

	if ( m_isYFX && m_isZFLatLon )
	{
		uint32_t nPlotType = m_params.CheckCount( kwDISPLAY_PLOT_TYPE, 0, 1 );
		if ( nPlotType > 0 )
		{
			uint32_t plotType;
			m_params.m_mapParam[ kwDISPLAY_PLOT_TYPE ]->GetValue( plotType );
			if ( plotType == CMapTypeDisp::eTypeDispYFX )
			{
				m_isZFLatLon = false;
			}
			else
			{
				m_isYFX = false;
			}
		}
	}


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

	for ( index = 0; index < nbExpr; index++ )
	{
		CExpression expr;
		//CUnit* unit = new CUnit;

		CBratProcess::GetVarDef( m_params,
			kwFIELD,
			expr,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
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

		std::string fieldName = expr.GetFieldNames()->at( 0 ).c_str();

		// ------------------------------
		// -------------- YFX plot
		// ------------------------------
		if ( m_isYFX )
		{
			// Add field to xy plot group
			CPlot* plot = dynamic_cast<CPlot*>( m_plots[ groupNumber ] );
			if ( plot == NULL )
			{
				plot = new CPlot( groupNumber );
				m_plots[ groupNumber ] = plot;
			}
			CPlotField* field = new CPlotField( fieldName.c_str() );
			plot->m_fields.Insert( field );

			std::string xAxisName;
			uint32_t nXAxisName = m_params.CheckCount( kwDISPLAY_XAXIS, 0, nbGroup );

			if ( nXAxisName > 0 )
			{
				m_params.m_mapParam[ kwDISPLAY_XAXIS ]->GetValue( xAxisName, groupNumber - groupNumberMin, "" );
			}

			if ( ! xAxisName.empty() )
			{
				plot->SetForcedVarXname( xAxisName.c_str() );

				std::string xAxisLabel;
				uint32_t nXAxisLabel = m_params.CheckCount( kwDISPLAY_XLABEL, 0, nbGroup );

				if ( nXAxisLabel > 0 )
				{
					m_params.m_mapParam[ kwDISPLAY_XLABEL ]->GetValue( xAxisLabel, groupNumber - groupNumberMin, xAxisName );
				}

				plot->m_titleX = xAxisLabel.c_str();

			}

			CInternalFiles* yfx = dynamic_cast<CInternalFiles*>( Prepare( index, fieldName ) );

			field->m_internalFiles.Insert( yfx );
			field->m_xyProps = GetXYPlotProperties( index );
		}

		// ------------------------------
		// -------------- World plot
		// ------------------------------
		if ( m_isZFLatLon )
		{
			// Add field to world plot group
			CWPlot* wplot = dynamic_cast<CWPlot*>( m_plots[ groupNumber ] );
			if ( wplot == NULL )
			{
				wplot = new CWPlot( groupNumber );
				m_plots[ groupNumber ] = wplot;
			}

			CWorldPlotProperties* worldProps = GetWorldPlotProperties( index );

			if ( ( ! worldProps->m_withContour ) && ( ! worldProps->m_solidColor ) )
			{
				worldProps->m_solidColor = true;
			}

			CPlotField* field = NULL;

			if ( worldProps->m_withAnimation )
			{
				//group files by field
				// find an already existing field with the same name
				// and the same contour flag
				// (don't animate same named fields which with different contour flag)
				field = wplot->FindPlotField( fieldName, &worldProps->m_withContour, &worldProps->m_solidColor );
				if ( field == NULL )
				{
					field = new CPlotField( fieldName.c_str() );
					wplot->m_fields.Insert( field );
				}
			}
			else
			{
				field = new CPlotField( fieldName.c_str() );
				wplot->m_fields.Insert( field );
			}

			CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>( Prepare( index, fieldName ) );

			field->m_internalFiles.Insert( zfxy );
			field->m_worldProps = worldProps;
		}

		// ------------------------------
		// -------------- ZFXY plot
		// ------------------------------

		if ( m_isZFXY )
		{
			// Add field to ZFXY plot group
			CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( m_plots[ groupNumber ] );
			if ( zfxyplot == NULL )
			{
				zfxyplot = new CZFXYPlot( groupNumber );
				m_plots[ groupNumber ] = zfxyplot;
			}

			CZFXYPlotProperties* zfxyProps = GetZFXYPlotProperties( index );

			if ( ( ! zfxyProps->m_withContour ) && ( ! zfxyProps->m_solidColor ) )
			{
				zfxyProps->m_solidColor = true;
			}

			CPlotField* field = NULL;

			if ( zfxyProps->m_withAnimation )
			{
				//group files by field
				// find an already existing field with the same name
				// and the same contour flag
				// (don't animate same named fields which with different contour flag)
				field = zfxyplot->FindPlotField( fieldName, &zfxyProps->m_withContour, &zfxyProps->m_solidColor );
				if ( field == NULL )
				{
					field = new CPlotField( fieldName.c_str() );
					zfxyplot->m_fields.Insert( field );
				}
			}
			else
			{
				field = new CPlotField( fieldName.c_str() );
				zfxyplot->m_fields.Insert( field );
			}


			std::string xAxisName;
			uint32_t nXAxisName = m_params.CheckCount( kwDISPLAY_XAXIS, 0, nbGroup );

			if ( nXAxisName > 0 )
			{
				m_params.m_mapParam[ kwDISPLAY_XAXIS ]->GetValue( xAxisName, groupNumber - groupNumberMin, "" );
			}

			if ( ! xAxisName.empty() )
			{
				zfxyplot->SetForcedVarXname( xAxisName.c_str() );
			}

			std::string yAxisName;
			uint32_t nYAxisName = m_params.CheckCount( kwDISPLAY_YAXIS, 0, nbGroup );

			if ( nYAxisName > 0 )
			{
				m_params.m_mapParam[ kwDISPLAY_YAXIS ]->GetValue( yAxisName, groupNumber - groupNumberMin, "" );
			}

			if ( ! yAxisName.empty() )
			{
				zfxyplot->SetForcedVarYname( yAxisName.c_str() );
			}

			CInternalFiles* f = dynamic_cast<CInternalFiles*>( Prepare( index, fieldName ) );

			field->m_internalFiles.Insert( f );
			field->m_zfxyProps = zfxyProps;

		}

		m_fields[ index ] = expr;
	}
}


void CDisplayFilesProcessor::GetXYPlotPropertyParams( int32_t nFields )
{
	int32_t i = 0;
	bool boolValue = false;
	double doubleValue = 0.0;
	std::string stringValue;
	int32_t uintValue = 0;

	CXYPlotProperties xyPlotProperty;
	CMapColor::GetInstance().ResetPrimaryColors();

	if ( nFields <= 0 )
		throw  CParameterException( "CDisplayFilesProcessor::GetXYPlotPropertyParams - There is no field to plot", BRATHL_INCONSISTENCY_ERROR );

	int32_t showProp = m_params.CheckCount( kwDISPLAY_PROPERTIES, 0, 1 );
	int32_t xMin = m_params.CheckCount( kwDISPLAY_XMINVALUE, 0, 1 );
	int32_t xMax = m_params.CheckCount( kwDISPLAY_XMAXVALUE, 0, 1 );
	int32_t yMin = m_params.CheckCount( kwDISPLAY_YMINVALUE, 0, 1 );
	int32_t yMax = m_params.CheckCount( kwDISPLAY_YMAXVALUE, 0, 1 );
	int32_t xTicks = m_params.CheckCount( kwDISPLAY_XTICKS, 0, 1 );
	int32_t yTicks = m_params.CheckCount( kwDISPLAY_YTICKS, 0, 1 );
	//int32_t xLabel = m_params.CheckCount(kwDISPLAY_XLABEL, 0, 1);
	int32_t yLabel = m_params.CheckCount( kwDISPLAY_YLABEL, 0, 1 );
	int32_t title = m_params.CheckCount( kwDISPLAY_TITLE, 0, 1 );

	int32_t name = m_params.CheckCount( kwDISPLAY_NAME, 0, nFields );
	int32_t color = m_params.CheckCount( kwDISPLAY_COLOR, 0, nFields );
	int32_t opacity = m_params.CheckCount( kwDISPLAY_OPACITY, 0, nFields );
	int32_t points = m_params.CheckCount( kwDISPLAY_POINTS, 0, nFields );
	int32_t lines = m_params.CheckCount( kwDISPLAY_LINES, 0, nFields );

	int32_t pointSize = m_params.CheckCount( kwDISPLAY_POINTSIZE, 0, nFields );

	int32_t lineWidth = m_params.CheckCount( kwDISPLAY_LINEWIDTH, 0, nFields );
	int32_t stipplePattern = m_params.CheckCount( kwDISPLAY_STIPPLEPATTERN, 0, nFields );

	int32_t pointGlyph = m_params.CheckCount( kwDISPLAY_POINTGLYPH, 0, nFields );
	int32_t pointFilled = m_params.CheckCount( kwDISPLAY_POINTFILLED, 0, nFields );

	//------------------------------------------
	// Get common property (same for each plot)
	//------------------------------------------

	if ( showProp != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_PROPERTIES ]->GetValue( boolValue );
		xyPlotProperty.SetShowPropertyPanel( boolValue );
	}
	if ( xMin != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_XMINVALUE ]->GetValue( doubleValue );
		xyPlotProperty.SetXMin( doubleValue );
	}
	if ( xMax != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_XMAXVALUE ]->GetValue( doubleValue );
		xyPlotProperty.SetXMax( doubleValue );
	}
	if ( yMin != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_YMINVALUE ]->GetValue( doubleValue );
		xyPlotProperty.SetYMin( doubleValue );
	}
	if ( yMax != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_YMAXVALUE ]->GetValue( doubleValue );
		xyPlotProperty.SetYMax( doubleValue );
	}
	if ( xTicks != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_XTICKS ]->GetValue( uintValue );
		xyPlotProperty.SetXNumTicks( uintValue );
	}
	if ( yTicks != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_YTICKS ]->GetValue( uintValue );
		xyPlotProperty.SetYNumTicks( uintValue );
	}

	//if (xLabel != 0)
	//{
	//m_params.m_mapParam[kwDISPLAY_XLABEL]->GetValue(stringValue);
	//xyPlotProperty.SetXLabel(stringValue.c_str());
	//}

	if ( yLabel != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_YLABEL ]->GetValue( stringValue );
		xyPlotProperty.SetYLabel( stringValue.c_str() );
	}
	if ( title != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_TITLE ]->GetValue( stringValue );
		xyPlotProperty.SetTitle( stringValue.c_str() );
	}

	//------------------------------------------
	// Fill plot porperties array and set a default color for each property
	//------------------------------------------
	CMapColor::GetInstance().ResetPrimaryColors();
	for ( i = 0; i < nFields; i++ )
	{
		CXYPlotProperties* props = new CXYPlotProperties( xyPlotProperty );

		m_xyPlotProperties.Insert( props );
		CPlotColor vtkColor = CMapColor::GetInstance().NextPrimaryColors();
		if ( vtkColor.Ok() )
		{
			props->SetColor( vtkColor );
		}

	}


	//------------------------------------------
	// Get Name Property for each plot
	//------------------------------------------
	for ( i = 0; i < name; i++ )
	{
		CXYPlotProperties* props = GetXYPlotProperties( i );
		if ( props == NULL )
		{
			continue;
		}

		m_params.m_mapParam[ kwDISPLAY_NAME ]->GetValue( stringValue, i );
		if ( stringValue.empty() == false )
		{
			props->SetName( stringValue.c_str() );
		}
	}

	//------------------------------------------
	// Get Color Property for each plot
	//------------------------------------------
	for ( i = 0; i < color; i++ )
	{
		CXYPlotProperties* props = GetXYPlotProperties( i );
		if ( props == NULL )
		{
			continue;
		}

		m_params.m_mapParam[ kwDISPLAY_COLOR ]->GetValue( stringValue, i );
		CPlotColor vtkColor = CMapColor::GetInstance().NameToPlotColor( stringValue );

		if ( vtkColor.Ok() == false )
		{
			vtkColor = CMapColor::GetInstance().NextPrimaryColors();
		}
		props->SetColor( vtkColor );
	}

	//------------------------------------------
	// Get Opacity Property for each plot
	//------------------------------------------
	for ( i = 0; i < opacity; i++ )
	{
		CXYPlotProperties* props = GetXYPlotProperties( i );
		if ( props == NULL )
		{
			continue;
		}

		m_params.m_mapParam[ kwDISPLAY_OPACITY ]->GetValue( doubleValue, i, 0.6 );
		if ( isDefaultValue( doubleValue ) == false )
		{
			props->SetOpacity( doubleValue );
		}
	}

	//------------------------------------------
	// Get Points Property for each plot
	//------------------------------------------
	int32_t indexPointSize = 0;
	int32_t indexPointGlyph = 0;
	int32_t indexPointFilled = 0;
	for ( i = 0; i < points; i++ )
	{
		CXYPlotProperties* props = GetXYPlotProperties( i );
		if ( props == NULL )
		{
			continue;
		}

		m_params.m_mapParam[ kwDISPLAY_POINTS ]->GetValue( boolValue, i );
		props->SetPoints( boolValue );
		//------------------------------------------
		// Get Pointsize Property for each plot
		//------------------------------------------
		if ( boolValue )  // points representation
		{
			if ( indexPointSize < pointSize )
			{
				m_params.m_mapParam[ kwDISPLAY_POINTSIZE ]->GetValue( doubleValue, indexPointSize, 1.0 );
				props->SetPointSize( doubleValue );
				indexPointSize++;
			}
			if ( indexPointGlyph < pointGlyph )
			{
				EPointGlyph pointGlyph = CMapPointGlyph::GetInstance().GetPointGlyph( m_params, indexPointGlyph );
				props->SetPointGlyph( pointGlyph );
				indexPointGlyph++;
			}
			if ( indexPointFilled < pointFilled )
			{
				m_params.m_mapParam[ kwDISPLAY_POINTFILLED ]->GetValue( boolValue, indexPointFilled, true );
				props->SetFilledPoint( boolValue );
				indexPointFilled++;
			}
		}
	}
	//------------------------------------------
	// Get Lines Property for each plot
	//------------------------------------------
	for ( i = 0; i < lines; i++ )
	{
		CXYPlotProperties* props = GetXYPlotProperties( i );
		if ( props == NULL )
		{
			continue;
		}

		m_params.m_mapParam[ kwDISPLAY_LINES ]->GetValue( boolValue, i );
		props->SetLines( boolValue );
	}

	//------------------------------------------
	// Get Line Width Property for each plot
	//------------------------------------------
	for ( i = 0; i < lineWidth; i++ )
	{
		CXYPlotProperties* props = GetXYPlotProperties( i );
		if ( props == NULL )
		{
			continue;
		}

		m_params.m_mapParam[ kwDISPLAY_LINEWIDTH ]->GetValue( doubleValue, i, 0.8 );
		props->SetLineWidth( doubleValue );
	}
	//------------------------------------------
	// Get Line Stipple Pattern Properties for each plot
	//------------------------------------------
	for ( i = 0; i < stipplePattern; i++ )
	{
		CXYPlotProperties* props = GetXYPlotProperties( i );
		if ( props == NULL )
		{
			continue;
		}

		EStipplePattern stipplePatternValue = CMapStipplePattern::GetInstance().GetStipplePattern( m_params, i );	//v4 always returns default value

#if defined(BRAT_V3)
		props->SetLineStipple( stipplePatternValue );
#else
		props->SetStipplePattern( stipplePatternValue );
#endif
	}
}




void CDisplayFilesProcessor::GetWPlotPropertyParams( int32_t nFields )
{
	int32_t i = 0;
	bool boolValue;
	double doubleValue;
	std::string stringValue;
	int32_t intValue;

	CWorldPlotProperties wPlotProperty;
	//CMapColor::GetInstance().ResetPrimaryColors();

	if ( nFields <= 0 )
	{
		throw  CParameterException( "CDisplayFilesProcessor::GetWPlotPropertyParams - There is no field to plot",
			BRATHL_INCONSISTENCY_ERROR );
	}

	int32_t title = m_params.CheckCount( kwDISPLAY_TITLE, 0, 1 );
	int32_t showProp = m_params.CheckCount( kwDISPLAY_PROPERTIES, 0, 1 );
	int32_t showColorBar = m_params.CheckCount( kwDISPLAY_COLORBAR, 0, 1 );
	int32_t showAnimationBar = m_params.CheckCount( kwDISPLAY_ANIMATIONBAR, 0, 1 );
	int32_t centerLat = m_params.CheckCount( kwDISPLAY_CENTERLAT, 0, 1 );
	int32_t centerLon = m_params.CheckCount( kwDISPLAY_CENTERLON, 0, 1 );
	int32_t projection = m_params.CheckCount( kwDISPLAY_PROJECTION, 0, 1 );
	int32_t coastResolution = m_params.CheckCount( kwDISPLAY_COASTRESOLUTION, 0, 1 );
	int32_t zoomLon1 = m_params.CheckCount( kwDISPLAY_ZOOM_LON1, 0, 1 );
	int32_t zoomLon2 = m_params.CheckCount( kwDISPLAY_ZOOM_LON2, 0, 1 );
	int32_t zoomLat1 = m_params.CheckCount( kwDISPLAY_ZOOM_LAT1, 0, 1 );
	int32_t zoomLat2 = m_params.CheckCount( kwDISPLAY_ZOOM_LAT2, 0, 1 );
	int32_t withAnimation = m_params.CheckCount( kwDISPLAY_ANIMATION, 0, 1 );

	int32_t minHeigth = m_params.CheckCount( kwDISPLAY_MINVALUE, 0, nFields );
	int32_t maxHeigth = m_params.CheckCount( kwDISPLAY_MAXVALUE, 0, nFields );
	int32_t opacity = m_params.CheckCount( kwDISPLAY_OPACITY, 0, nFields );
	int32_t numColorLabels = m_params.CheckCount( kwDISPLAY_NUMCOLORLABELS, 0, nFields );
	int32_t colorTable = m_params.CheckCount( kwDISPLAY_COLORTABLE, 0, nFields );
	int32_t colorCurve = m_params.CheckCount( kwDISPLAY_COLORCURVE, 0, nFields );
	int32_t contour = m_params.CheckCount( kwDISPLAY_CONTOUR, 0, nFields );
	int32_t contourNum = m_params.CheckCount( kwDISPLAY_CONTOUR_NUMBER, 0, nFields );
	int32_t contourLabel = m_params.CheckCount( kwDISPLAY_CONTOUR_LABEL, 0, nFields );
	int32_t contourLabelNumber = m_params.CheckCount( kwDISPLAY_CONTOUR_LABEL_NUMBER, 0, nFields );
	int32_t contourMinValue = m_params.CheckCount( kwDISPLAY_CONTOUR_MINVALUE, 0, nFields );
	int32_t contourMaxValue = m_params.CheckCount( kwDISPLAY_CONTOUR_MAXVALUE, 0, nFields );
	int32_t solidColor = m_params.CheckCount( kwDISPLAY_SOLID_COLOR, 0, nFields );

	int32_t eastComponent = m_params.CheckCount( kwDISPLAY_EAST_COMPONENT, 0, nFields );
	int32_t northComponent = m_params.CheckCount( kwDISPLAY_NORTH_COMPONENT, 0, nFields );
	//int32_t gridLabels = m_params.CheckCount(kwDISPLAY_GRID_LABELS, 0, nFields);
	int32_t name = m_params.CheckCount( kwDISPLAY_NAME, 0, nFields );

	if ( title != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_TITLE ]->GetValue( stringValue );
		wPlotProperty.m_title = stringValue.c_str();
	}
	if ( showProp != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_PROPERTIES ]->GetValue( boolValue );
		wPlotProperty.m_showPropertyPanel = boolValue;
	}
	if ( showColorBar != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_COLORBAR ]->GetValue( boolValue );
		wPlotProperty.m_showColorBar = boolValue;
	}
	if ( showAnimationBar != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_ANIMATIONBAR ]->GetValue( boolValue );
		wPlotProperty.m_showAnimationToolbar = boolValue;
	}
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

	for ( i = 0; i < nFields; i++ )
	{
		m_wPlotProperties.Insert( new CWorldPlotProperties( wPlotProperty ) );
	}

	//------------------------------------------
	// Get max value Property for each plot
	//------------------------------------------
	for ( i = 0; i < maxHeigth; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_MAXVALUE ]->GetValue( doubleValue, i );
		props->m_maxHeightValue = doubleValue;
	}
	//------------------------------------------
	// Get min value Property for each plot
	//------------------------------------------
	for ( i = 0; i < minHeigth; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_MINVALUE ]->GetValue( doubleValue, i );
		props->m_minHeightValue = doubleValue;
	}
	//------------------------------------------
	// Get opacity Property for each plot
	//------------------------------------------
	for ( i = 0; i < opacity; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_OPACITY ]->GetValue( doubleValue, i );
		props->m_opacity = doubleValue;
	}
	//------------------------------------------
	// Get number of color labels Property for each plot
	//------------------------------------------
	for ( i = 0; i < numColorLabels; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_NUMCOLORLABELS ]->GetValue( intValue, i );
		props->m_numColorLabels = intValue;
	}
	//------------------------------------------
	// Get color Curve Property for each plot
	//------------------------------------------
	for ( i = 0; i < colorCurve; i++ )
	{
        CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_COLORCURVE ]->GetValue( stringValue, i );
#if defined (BRAT_V3)
		std::string stringValueOk = wPlotProperty.m_LUT->CurveToLabeledCurve( stringValue );
		if ( stringValueOk.empty() )
		{
			throw  CParameterException( CTools::Format( "Unknown color curve name value '%s' for  parameter '%s'",
				stringValue.c_str(),
				kwDISPLAY_COLORCURVE.c_str() ),
				BRATHL_SYNTAX_ERROR );

		}
		props->m_LUT->ExecCurveMethod( stringValueOk );
#else
        UNUSED( props );
#endif
	}
	//------------------------------------------
	// Get color table Property for each plot
	//------------------------------------------
	for ( i = 0; i < colorTable; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_COLORTABLE ]->GetValue( stringValue, i, PALETTE_AEROSOL );
#if defined (BRAT_V3)
		std::string stringValueOk = wPlotProperty.m_LUT->MethodToLabeledMethod( stringValue );

		if ( stringValueOk.empty() )
		{
			if ( !IsFile( stringValue ) )
			{
				throw  CParameterException( CTools::Format( "Unknown color table name value or color file name '%s' for  parameter '%s'",
					stringValue.c_str(),
					kwDISPLAY_COLORTABLE.c_str() ),
					BRATHL_SYNTAX_ERROR );
			}
			else
			{
				// load the color table file definition
				try
				{
					props->m_LUT->LoadFromFile( stringValue );
				}
				catch ( CException& e )
				{
					SimpleErrorBox( e.what() );
				}
				catch ( ... )
				{
					SimpleErrorBox( "Can't load " + stringValue );
				}
			}
		}
		else  //stringValueOk is not empty, it's a predefined color
		{
			props->m_LUT->ExecMethod( stringValueOk );
		}
#else
        UNUSED( props );
#endif
	}
	//------------------------------------------
	// Get contour Property for each plot
	//------------------------------------------
	for ( i = 0; i < contour; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR ]->GetValue( boolValue, i );
		props->m_withContour = boolValue;
	}
	//------------------------------------------
	// Get number of contours Property for each plot
	//------------------------------------------
	for ( i = 0; i < contourNum; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_NUMBER ]->GetValue( intValue, i );
		props->m_numContour = intValue;
	}
	//------------------------------------------
	// Get contour Label Property for each plot
	//------------------------------------------
	for ( i = 0; i < contourLabel; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_LABEL ]->GetValue( boolValue, i );
		props->m_withContourLabel = boolValue;
	}
	//------------------------------------------
	// Get number of contour label Property for each plot
	//------------------------------------------
	for ( i = 0; i < contourLabelNumber; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_LABEL_NUMBER ]->GetValue( intValue, i );
		props->m_numContourLabel = intValue;
	}
	//------------------------------------------
	// Get contour min value Property for each plot
	//------------------------------------------
	for ( i = 0; i < contourMinValue; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_MINVALUE ]->GetValue( doubleValue, i );
		props->m_minContourValue = doubleValue;
	}
	//------------------------------------------
	// Get contour max value Property for each plot
	//------------------------------------------
	for ( i = 0; i < contourMaxValue; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_MAXVALUE ]->GetValue( doubleValue, i );
		props->m_maxContourValue = doubleValue;
	}
	//------------------------------------------
	// Get solid color Property for each plot
	//------------------------------------------
	for ( i = 0; i < solidColor; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_SOLID_COLOR ]->GetValue( boolValue, i );
		props->m_solidColor = boolValue;
	}


	//-------------------------------------------------------------
	// Are the plots vector components? Just one east and one north
	//-------------------------------------------------------------
	for ( i = 0; i < eastComponent; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_EAST_COMPONENT ]->GetValue( boolValue, i );
		props->m_eastComponent = boolValue;
		//if ( boolValue )
		//	break;
	}

	for ( i = 0; i < northComponent; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_NORTH_COMPONENT ]->GetValue( boolValue, i );
		props->m_northComponent = boolValue;
		//if ( boolValue )
		//	break;
	}


	//------------------------------------------
	// Get Name Property for each plot
	//------------------------------------------
	for ( i = 0; i < name; i++ )
	{
		CWorldPlotProperties* props = GetWorldPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_NAME ]->GetValue( stringValue, i );
		props->m_name = stringValue.c_str();
	}
}


void CDisplayFilesProcessor::GetZFXYPlotPropertyParams( int32_t nFields )
{
	int32_t i = 0;
	bool boolValue;
	double doubleValue;
	std::string stringValue;
	int32_t intValue;
	uint32_t uintValue;

	CZFXYPlotProperties zfxyPlotProperty;
	//CMapColor::GetInstance().ResetPrimaryColors();

	if ( nFields <= 0 )
	{
		throw  CParameterException( "CDisplayFilesProcessor::GetZFXYPlotPropertyParams - There is no field to plot",
			BRATHL_INCONSISTENCY_ERROR );
	}

	int32_t title = m_params.CheckCount( kwDISPLAY_TITLE, 0, 1 );
	int32_t showProp = m_params.CheckCount( kwDISPLAY_PROPERTIES, 0, 1 );
	int32_t showColorBar = m_params.CheckCount( kwDISPLAY_COLORBAR, 0, 1 );
	int32_t showAnimationBar = m_params.CheckCount( kwDISPLAY_ANIMATIONBAR, 0, 1 );


	int32_t xMin = m_params.CheckCount( kwDISPLAY_XMINVALUE, 0, 1 );
	int32_t xMax = m_params.CheckCount( kwDISPLAY_XMAXVALUE, 0, 1 );
	int32_t yMin = m_params.CheckCount( kwDISPLAY_YMINVALUE, 0, 1 );
	int32_t yMax = m_params.CheckCount( kwDISPLAY_YMAXVALUE, 0, 1 );
	int32_t xTicks = m_params.CheckCount( kwDISPLAY_XTICKS, 0, 1 );
	int32_t yTicks = m_params.CheckCount( kwDISPLAY_YTICKS, 0, 1 );

	int32_t withAnimation = m_params.CheckCount( kwDISPLAY_ANIMATION, 0, 1 );

	int32_t yLabel = m_params.CheckCount( kwDISPLAY_YLABEL, 0, 1 );

	int32_t minHeigth = m_params.CheckCount( kwDISPLAY_MINVALUE, 0, nFields );
	int32_t maxHeigth = m_params.CheckCount( kwDISPLAY_MAXVALUE, 0, nFields );
	int32_t opacity = m_params.CheckCount( kwDISPLAY_OPACITY, 0, nFields );
	int32_t numColorLabels = m_params.CheckCount( kwDISPLAY_NUMCOLORLABELS, 0, nFields );
	int32_t colorTable = m_params.CheckCount( kwDISPLAY_COLORTABLE, 0, nFields );
	int32_t colorCurve = m_params.CheckCount( kwDISPLAY_COLORCURVE, 0, nFields );
	int32_t contour = m_params.CheckCount( kwDISPLAY_CONTOUR, 0, nFields );
	int32_t contourNum = m_params.CheckCount( kwDISPLAY_CONTOUR_NUMBER, 0, nFields );
	int32_t contourLabel = m_params.CheckCount( kwDISPLAY_CONTOUR_LABEL, 0, nFields );
	int32_t contourLabelNumber = m_params.CheckCount( kwDISPLAY_CONTOUR_LABEL_NUMBER, 0, nFields );
	int32_t contourMinValue = m_params.CheckCount( kwDISPLAY_CONTOUR_MINVALUE, 0, nFields );
	int32_t contourMaxValue = m_params.CheckCount( kwDISPLAY_CONTOUR_MAXVALUE, 0, nFields );
	int32_t solidColor = m_params.CheckCount( kwDISPLAY_SOLID_COLOR, 0, nFields );

	int32_t name = m_params.CheckCount( kwDISPLAY_NAME, 0, nFields );

	if ( yLabel != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_YLABEL ]->GetValue( stringValue );
		zfxyPlotProperty.m_yLabel = stringValue.c_str();
	}

	if ( title != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_TITLE ]->GetValue( stringValue );
		zfxyPlotProperty.m_title = stringValue.c_str();
	}
	if ( showProp != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_PROPERTIES ]->GetValue( boolValue );
		zfxyPlotProperty.m_showPropertyPanel = boolValue;
	}
	if ( showColorBar != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_COLORBAR ]->GetValue( boolValue );
		zfxyPlotProperty.m_showColorBar = boolValue;
	}
	if ( showAnimationBar != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_ANIMATIONBAR ]->GetValue( boolValue );
		zfxyPlotProperty.m_showAnimationToolbar = boolValue;
	}
	if ( xMin != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_XMINVALUE ]->GetValue( doubleValue );
		zfxyPlotProperty.m_xMin = doubleValue;
	}
	if ( xMax != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_XMAXVALUE ]->GetValue( doubleValue );
		zfxyPlotProperty.m_xMax = doubleValue;
	}
	if ( yMin != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_YMINVALUE ]->GetValue( doubleValue );
		zfxyPlotProperty.m_yMin = doubleValue;
	}
	if ( yMax != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_YMAXVALUE ]->GetValue( doubleValue );
		zfxyPlotProperty.m_yMax = doubleValue;
	}
	if ( xTicks != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_XTICKS ]->GetValue( uintValue );
		zfxyPlotProperty.m_xNumTicks = uintValue;
	}
	if ( yTicks != 0 )
	{
		m_params.m_mapParam[ kwDISPLAY_YTICKS ]->GetValue( uintValue );
		zfxyPlotProperty.m_yNumTicks = uintValue;
	}

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


	for ( i = 0; i < nFields; i++ )
	{
		m_zfxyPlotProperties.Insert( new CZFXYPlotProperties( zfxyPlotProperty ) );
	}

	//------------------------------------------
	// Get max value Property for each plot
	//------------------------------------------
	for ( i = 0; i < maxHeigth; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_MAXVALUE ]->GetValue( doubleValue, i );
		props->m_maxHeightValue = doubleValue;
	}
	//------------------------------------------
	// Get min value Property for each plot
	//------------------------------------------
	for ( i = 0; i < minHeigth; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_MINVALUE ]->GetValue( doubleValue, i );
		props->m_minHeightValue = doubleValue;
	}
	//------------------------------------------
	// Get opacity Property for each plot
	//------------------------------------------
	for ( i = 0; i < opacity; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_OPACITY ]->GetValue( doubleValue, i );
		props->m_opacity = doubleValue;
	}
	//------------------------------------------
	// Get number of color labels Property for each plot
	//------------------------------------------
	for ( i = 0; i < numColorLabels; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_NUMCOLORLABELS ]->GetValue( intValue, i );
		props->m_numColorLabels = intValue;
	}
	//------------------------------------------
	// Get color Curve Property for each plot
	//------------------------------------------
	for ( i = 0; i < colorCurve; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_COLORCURVE ]->GetValue( stringValue, i );
#if defined (BRAT_V3)
		std::string stringValueOk = zfxyPlotProperty.m_LUT->CurveToLabeledCurve( stringValue );
		if ( stringValueOk.empty() )
		{
			throw  CParameterException( CTools::Format( "Unknown color curve name value '%s' for  parameter '%s'",
				stringValue.c_str(),
				kwDISPLAY_COLORCURVE.c_str() ),
				BRATHL_SYNTAX_ERROR );

		}
		props->m_LUT->ExecCurveMethod( stringValueOk );
#else
        UNUSED( props );
#endif
	}
	//------------------------------------------
	// Get color table Property for each plot
	//------------------------------------------
	for ( i = 0; i < colorTable; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_COLORTABLE ]->GetValue( stringValue, i, PALETTE_AEROSOL );
#if defined (BRAT_V3)
		std::string stringValueOk = zfxyPlotProperty.m_LUT->MethodToLabeledMethod( stringValue );

		if ( stringValueOk.empty() )
		{
			std::string fileName = stringValue;		//			fileName.Normalize();

			if ( !IsFile( fileName ) )
			{
				throw  CParameterException( CTools::Format( "Unknown color table name value or color file name '%s' for  parameter '%s'",
					stringValue.c_str(),
					kwDISPLAY_COLORTABLE.c_str() ),
					BRATHL_SYNTAX_ERROR );
			}
			else
			{
				// load the color table file definition
				try
				{
					props->m_LUT->LoadFromFile( fileName );		//fileName.GetFullPath() 
				}
				catch ( CException& e )
				{
					SimpleWarnBox( e.what() );
				}
				catch ( ... )
				{
					SimpleWarnBox( "Can't load " + fileName );
				}
			}
		}
		else  //stringValueOk is not empty, it's a predefined color
		{
			props->m_LUT->ExecMethod( stringValueOk );
		}
#else
        UNUSED( props );
#endif
	}
	//------------------------------------------
	// Get contour Property for each plot
	//------------------------------------------
	for ( i = 0; i < contour; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR ]->GetValue( boolValue, i );
		props->m_withContour = boolValue;
	}
	//------------------------------------------
	// Get number of contours Property for each plot
	//------------------------------------------
	for ( i = 0; i < contourNum; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_NUMBER ]->GetValue( intValue, i );
		props->m_numContour = intValue;
	}
	//------------------------------------------
	// Get contour Label Property for each plot
	//------------------------------------------
	for ( i = 0; i < contourLabel; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_LABEL ]->GetValue( boolValue, i );
		props->m_withContourLabel = boolValue;
	}
	//------------------------------------------
	// Get number of contour label Property for each plot
	//------------------------------------------
	for ( i = 0; i < contourLabelNumber; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_LABEL_NUMBER ]->GetValue( intValue, i );
		props->m_numContourLabel = intValue;
	}
	//------------------------------------------
	// Get contour min value Property for each plot
	//------------------------------------------
	for ( i = 0; i < contourMinValue; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_MINVALUE ]->GetValue( doubleValue, i );
		props->m_minContourValue = doubleValue;
	}
	//------------------------------------------
	// Get contour max value Property for each plot
	//------------------------------------------
	for ( i = 0; i < contourMaxValue; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_CONTOUR_MAXVALUE ]->GetValue( doubleValue, i );
		props->m_maxContourValue = doubleValue;
	}
	//------------------------------------------
	// Get solid color Property for each plot
	//------------------------------------------
	for ( i = 0; i < solidColor; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_SOLID_COLOR ]->GetValue( boolValue, i );
		props->m_solidColor = boolValue;
	}



	//------------------------------------------
	// Get Name Property for each plot
	//------------------------------------------
	for ( i = 0; i < name; i++ )
	{
		CZFXYPlotProperties* props = GetZFXYPlotProperties( i );
		m_params.m_mapParam[ kwDISPLAY_NAME ]->GetValue( stringValue, i );
		props->m_name = stringValue.c_str();
	}
}





CXYPlotProperties* CDisplayFilesProcessor::GetXYPlotProperties( int32_t index ) const
{
	if ( ( index < 0 ) || ( static_cast<uint32_t>( index ) >= m_xyPlotProperties.size() ) )
	{
		std::string msg = "ERROR in  CDisplayFilesProcessor::GetXYPlotProperties : index "
			+ n2s<std::string>(index) 
			+ " out-of-range "
			+ "Valid range is [0, "
			+ n2s<std::string>( m_xyPlotProperties.size() )
			+ "]";

		throw CException( msg, BRATHL_LOGIC_ERROR );
	}

	CXYPlotProperties* props = dynamic_cast<CXYPlotProperties*>( m_xyPlotProperties.at( index ) );
	if ( props == NULL )
	{
		 throw CException("ERROR in  CDisplayFilesProcessor::GetXYPlotProperties : dynamic_cast<CXYPlotProperties*>(m_xyPlotProperties->at(index)); returns NULL pointer - "
		              "XY Plot Property array seems to contain objects other than those of the class CXYPlotProperties or derived class", BRATHL_LOGIC_ERROR);
	}
	return props;
}
CZFXYPlotProperties* CDisplayFilesProcessor::GetZFXYPlotProperties( int32_t index ) const
{
	if ( ( index < 0 ) || ( static_cast<uint32_t>( index ) >= m_zfxyPlotProperties.size() ) )
	{
		std::string msg = "ERROR in  CDisplayFilesProcessor::GetZFXYPlotProperties : index "
			+ n2s<std::string>(index) 
			+ " out-of-range "
			+ "Valid range is [0, "
			+ n2s<std::string>( m_zfxyPlotProperties.size() )
			+ "]";

		throw CException( msg, BRATHL_LOGIC_ERROR );
	}

	CZFXYPlotProperties* props = dynamic_cast<CZFXYPlotProperties*>( m_zfxyPlotProperties.at( index ) );
	if ( props == NULL )
	{
		throw CException( "ERROR in  CDisplayFilesProcessor::GetZFXYPlotProperties : dynamic_cast<CZFXYPlotProperties*>(m_zfxyPlotProperties->at(index)); returns NULL pointer - "
			"zFxy Plot Property array seems to contain objects other than those of the class CZFXYPlotProperties or derived class", BRATHL_LOGIC_ERROR );
	}
	return props;
}
CWorldPlotProperties* CDisplayFilesProcessor::GetWorldPlotProperties( int32_t index ) const 
{
	if ( ( index < 0 ) || ( static_cast<uint32_t>( index ) >= m_wPlotProperties.size() ) )
	{
		std::string msg = "ERROR in  CDisplayFilesProcessor::CWorldPlotProperties : index " 
			+ n2s<std::string>(index) 
			+ " out-of-range.\nValid range is [0, " 
			+ n2s<std::string>( m_wPlotProperties.size() )
			+ "]";

		throw CException( msg, BRATHL_LOGIC_ERROR );
	}

	CWorldPlotProperties* props = dynamic_cast<CWorldPlotProperties*>( m_wPlotProperties.at( index ) );
	if ( props == NULL )
	{
		throw CException( "ERROR in  CDisplayFilesProcessor::GetWorldPlotProperties : dynamic_cast<CWorldPlotProperties*>(m_wPlotProperties->at(index)); returns NULL pointer - "
			"world Plot Property array seems to contain objects other than those of the class CWorldPlotProperties or derived class", BRATHL_LOGIC_ERROR );
	}
	return props;
}



bool CDisplayFilesProcessor::IsYFXType() const
{
	if ( m_inputFileType.empty() )
	{
		CException e( "CDisplayFilesProcessor::isYFX - input file type is empty", BRATHL_LOGIC_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );
	}

	return m_inputFileType.compare( CInternalFilesYFX::TypeOf() ) == 0;
}
bool CDisplayFilesProcessor::IsZFXYType() const
{
	if ( m_inputFileType.empty() )
	{
		CException e( "CDisplayFilesProcessor::IsZFXYType - input file type is empty", BRATHL_LOGIC_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );
	}

	return m_inputFileType.compare( CInternalFilesZFXY::TypeOf() ) == 0;
}



void CDisplayFilesProcessor::CheckFieldsData( CInternalFilesYFX* yfx, const std::string& fieldName )
{
	if ( yfx == NULL )
		return;

	CStringArray names;

	yfx->GetDataVars( names );

	if ( names.Exists( fieldName ) == false )
	{
		std::string msg = CTools::Format( "CDisplayFilesProcessor::CheckFieldsData - Field '%s' in command file doesn't exist in intput file '%s'",
			fieldName.c_str(), yfx->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );
	}

}
void CDisplayFilesProcessor::CheckFieldsData( CInternalFilesZFXY* zfxy, const std::string& fieldName )
{
	if ( zfxy == NULL )
		return;

	CStringArray names;

	zfxy->GetDataVars( names );

	if ( names.Exists( fieldName ) == false )
	{
		std::string msg = CTools::Format( "CDisplayFilesProcessor::CheckFieldsData - Field '%s' in command file doesn't exist in intput file '%s'",
			fieldName.c_str(), zfxy->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );
	}

}
void CDisplayFilesProcessor::CheckFieldsData( CInternalFiles* f, const std::string& fieldName )
{
	CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>( f );
	CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>( f );
	if ( zfxy != NULL )
	{
		CheckFieldsData( zfxy, fieldName );
	}
	if ( yfx != NULL )
	{
		CheckFieldsData( yfx, fieldName );
	}
}
CInternalFiles* CDisplayFilesProcessor::Prepare( int32_t indexFile, const std::string& fieldName )
{
	CInternalFiles* f = BuildExistingInternalFileKind( m_inputFiles.at( indexFile ).c_str() );

	f->Open( ReadOnly );
	m_OpenFiles.push_back( f );

	CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>( f );
	CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>( f );
	if ( zfxy != NULL )
	{
		CheckFieldsData( zfxy, fieldName );
	}
	if ( yfx != NULL )
	{
		CheckFieldsData( yfx, fieldName );
	}

	//m_internalData.Insert( f );

	return f;
}

CInternalFiles* CDisplayFilesProcessor::Prepare( const std::string& fileName )
{
	CInternalFiles* f = NULL;

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
        CException e( std::string( "CDisplayFilesProcessor::Prepare -  unknown input file type: " + fileName ),
			BRATHL_LOGIC_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );
	}

	f->Open( ReadOnly );
	m_OpenFiles.push_back( f );

	/*
		CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(f);
		CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>(f);
		if (zfxy != NULL)
		{
		CheckFieldsData(zfxy, fieldName);
		}
		if (yfx != NULL)
		{
		CheckFieldsData(yfx, fieldName);
		}
		*/
	//m_internalData.Insert( f );

	return f;
}
