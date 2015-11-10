#include "../stdafx.h"

#include "CmdLineProcessor.h"

#include "../libbrathl/Trace.h"
#include "../libbrathl/ExternalFilesNetCDF.h"
#include "../libbrathl/ExternalFilesFactory.h"
#include "../libbrathl/InternalFilesZFXY.h"
#include "../libbrathl/InternalFilesYFX.h"

//#define __ZFXYPlotData_H__
//#define __vtkPlotData_h
#include "../display/PlotData/PlotField.h"
#include "../display/PlotData/Plot.h"
#include "../display/PlotData/WPlot.h"
#include "../display/PlotData/ZFXYPlot.h"


bool CmdLineProcessor::GetParametersNetcdfYFX( CExternalFilesNetCDF* externalFile )
{
	uint32_t groupNumber = 0;

	m_inputFileType = CInternalFilesYFX::TypeOf();

	m_plots.resize( groupNumber + 1 );

	int32_t nFields = m_paramVars.size();

	if ( nFields == 0 )
	{
		CStringArray varNames;
		externalFile->GetVariables( varNames );
		CStringArray::const_iterator it;

		for ( it = varNames.begin(); it != varNames.end(); it++ )
		{
			if ( ( *it ).compare( m_paramXAxis.c_str() ) == 0 )
			{
				continue;
			}

			if ( ( *it ).compare( m_paramYAxis.c_str() ) == 0 )
			{
				continue;
			}

			if ( externalFile->IsAxisVar( ( *it ) ) )
			{
				continue;
			}

			CUIntArray dimValues;
			externalFile->GetDimensions( ( *it ), dimValues );

			if ( ( dimValues.size() < 1 ) || ( dimValues.size() > 2 ) )
			{
				continue;
			}

			m_paramVars.Insert( ( *it ) );
			nFields = m_paramVars.size();
			break;

		}

		if ( m_paramVars.size() <= 0 )
		{
			std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfYFX - No field with one dimension to display has been found in file '%s'",
				externalFile->GetName().c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}
	}

	if ( ( m_paramYAxis.empty() == false ) && ( nFields > 1 ) )
	{
		m_paramXAxis = m_paramYAxis;
		m_paramYAxis = "";
	}

	if ( ( m_paramYAxis.empty() == false ) && ( nFields == 1 ) )
	{
		m_paramXAxis = *( m_paramVars.begin() );
		m_paramVars.RemoveAll();
		m_paramVars.Insert( m_paramYAxis );
	}


	CUIntArray xDimValues;
	externalFile->GetDimensions( m_paramXAxis, xDimValues );
	if ( xDimValues.size() <= 0 )
	{
		std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfYFX : Array of field '%s' is 0 dimensional - It have to be at least 1 or 2 dimensional",
			m_paramXAxis.c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );

	}

	if ( xDimValues.size() > 2 )
	{
		std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfYFX : Array of field '%s' is more than 2 dimensional - It have to be 1 or 2 dimensional",
			m_paramXAxis.c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}


	CStringList::const_iterator itParamVar;

	for ( itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
	{
		CUIntArray dimValues;
		externalFile->GetDimensions( ( *itParamVar ), dimValues );

		if ( dimValues.size() <= 0 )
		{
			std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfYFX : Array of field '%s' is 0 dimensional - It have to be at least 1 or 2 dimensional",
				( *itParamVar ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );

		}

		if ( dimValues.size() > 2 )
		{
			std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfYFX : Array of field '%s' is more than 2 dimensional - It have to be 1 or 2 dimensional",
				( *itParamVar ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}

	}


	m_fields.resize( nFields );

	GetXYPlotPropertyParams( nFields );

	uint32_t index = 0;

	for ( itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
	{
		// Add field to xy plot group
		CPlot* plot = dynamic_cast<CPlot*>( m_plots[ groupNumber ] );
		if ( plot == NULL )
		{
			plot = new CPlot( groupNumber );
			m_plots[ groupNumber ] = plot;
		}
		CPlotField* field = new CPlotField( ( *itParamVar ).c_str() );
		plot->m_fields.Insert( field );

		if ( m_paramXAxis.empty() == false )
		{
			plot->SetForcedVarXname( m_paramXAxis.c_str() );
		}


		CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>( Prepare( externalFile->GetName().c_str(), ( *itParamVar ).c_str() ) );

		field->m_internalFiles.Insert( yfx );
		field->m_xyProps = GetXYPlotProperty( index );

		m_fields[ index ] = ( *itParamVar );

		index++;
	}

	m_isYFX = true;

	return true;
}



bool CmdLineProcessor::GetParametersNetcdfZFXY( CExternalFilesNetCDF* externalFile )
{

	CNetCDFFiles* netCDFFile = externalFile->GetFile();

	if ( netCDFFile == NULL )
	{
		std::string msg = CTools::Format( "ERROR - The input file '%s' is not a NetCdf file (internal file is NULL)",
			externalFile->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}


	uint32_t groupNumber = 0;

	m_inputFileType = CInternalFilesZFXY::TypeOf();

	m_plots.resize( groupNumber + 1 );

	int32_t nFields = m_paramVars.size();

	CFieldNetCdf* fieldX = NULL;
	CFieldNetCdf* fieldY = NULL;

	bool bFoundLatDim = false;
	bool bFoundLonDim = false;

	std::string coordAxisX;
	std::string coordAxisY;

	if ( nFields == 0 )
	{
		CStringArray varNames;
		externalFile->GetVariables( varNames );
		CStringArray::const_iterator it;

		for ( it = varNames.begin(); it != varNames.end(); it++ )
		{
			bFoundLatDim = false;
			bFoundLonDim = false;

			if ( externalFile->IsAxisVar( ( *it ) ) )
			{
				continue;
			}

			CStringArray dimNames;
			externalFile->GetDimensions( ( *it ), dimNames );

			if ( dimNames.size() != 2 )
			{
				continue;
			}

			fieldX = externalFile->GetFieldNetCdf( dimNames.at( 0 ) );
			fieldY = externalFile->GetFieldNetCdf( dimNames.at( 1 ) );

			if ( ( fieldX == NULL ) || ( fieldY == NULL ) )
			{
				continue;
			}

			m_paramVars.Insert( ( *it ) );
			nFields = m_paramVars.size();
			break;

		}

		if ( m_paramVars.size() <= 0 )
		{
			std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfZFXY - No field with at least one dimension to display has been found in file '%s'",
				externalFile->GetName().c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}
	}




	if ( fieldX->GetDimIds().size() != 1 )
	{
		std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfZFXY : Array of field '%s' is not 1 dimensional.",
			fieldX->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}

	if ( fieldY->GetDimIds().size() != 1 )
	{
		std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfZFXY : Array of field '%s' is not 1 dimensional.",
			fieldY->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}


	CStringList::const_iterator itParamVar;

	for ( itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
	{
		CUIntArray dimValues;
		externalFile->GetDimensions( ( *itParamVar ), dimValues );


		if ( dimValues.size() != 2 )
		{
			std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfZFXY : Array of field '%s' is not 2 dimensional.",
				( *itParamVar ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}
	}


	CNetCDFVarDef* firstNetCDFVardef = NULL;

	for ( itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
	{
		CNetCDFVarDef* netCDFVarDef = netCDFFile->GetNetCDFVarDef( *itParamVar );

		if ( firstNetCDFVardef == NULL )
		{
			continue;
		}

		if ( netCDFVarDef->GetNbDims() != 2 )
		{
			std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfZFXY : Array of field '%s' is not 2 dimensional.",
				( *itParamVar ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}

		if ( firstNetCDFVardef == NULL )
		{
			firstNetCDFVardef = netCDFVarDef;
			continue;
		}


		if ( ! firstNetCDFVardef->HaveEqualDims( netCDFVarDef ) )
		{
			std::string msg = CTools::Format( "ERROR : All the fields to plot have not the same dimensions." );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}

	}


	m_fields.resize( nFields );

	GetZFXYPlotPropertyParams( nFields );

	uint32_t index = 0;

	for ( itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
	{
		// Add field to ZFXY plot group
		CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( m_plots[ groupNumber ] );
		if ( zfxyplot == NULL )
		{
			zfxyplot = new CZFXYPlot( groupNumber );
			m_plots[ groupNumber ] = zfxyplot;
		}

		CPlotField* field = new CPlotField( ( *itParamVar ).c_str() );
		zfxyplot->m_fields.Insert( field );


		if ( m_paramXAxis.empty() == false )
		{
			zfxyplot->SetForcedVarXname( m_paramXAxis.c_str() );
		}

		if ( m_paramYAxis.empty() == false )
		{
			zfxyplot->SetForcedVarYname( m_paramYAxis.c_str() );
		}


		CInternalFiles* zfxy = Prepare( externalFile->GetName().c_str(), ( *itParamVar ).c_str() );

		field->m_internalFiles.Insert( zfxy );
		field->m_zfxyProps = GetZFXYPlotProperty( index );

		m_fields[ index ] = ( *itParamVar );

		index++;
	}

	m_isZFXY = true;

	return true;
}


bool CmdLineProcessor::GetParametersNetcdfZFLatLon( CExternalFilesNetCDF* externalFile )
{

	uint32_t groupNumber = 0;

	m_inputFileType = CInternalFilesZFXY::TypeOf();

	m_plots.resize( groupNumber + 1 );

	int32_t nFields = m_paramVars.size();

	CFieldNetCdf* fieldLat = externalFile->FindLatField();
	CFieldNetCdf* fieldLon = externalFile->FindLonField();

	if ( fieldLat == NULL )
	{
		std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfZFLatLon - No Latitude field has been found in file '%s'",
			externalFile->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		CTrace::Tracer( "%s", e.what() );
		throw ( e );
	}

	if ( fieldLon == NULL )
	{
		std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfZFLatLon - No Longitude field has been found in file '%s'",
			externalFile->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		CTrace::Tracer( "%s", e.what() );
		throw ( e );
	}
	bool bFoundLatDim = false;
	bool bFoundLonDim = false;

	if ( nFields == 0 )
	{
		CStringArray varNames;
		externalFile->GetVariables( varNames );
		CStringArray::const_iterator it;

		for ( it = varNames.begin(); it != varNames.end(); it++ )
		{
			bFoundLatDim = false;
			bFoundLonDim = false;

			if ( externalFile->IsAxisVar( ( *it ) ) )
			{
				continue;
			}

			CUIntArray dimValues;
			externalFile->GetDimensions( ( *it ), dimValues );

			if ( dimValues.size() != 2 )
			{
				continue;
			}

			CStringArray dimNames;
			externalFile->GetDimensions( ( *it ), dimNames );


			CStringArray::const_iterator itDimNames;

			for ( itDimNames = dimNames.begin(); itDimNames != dimNames.end(); itDimNames++ )
			{
				if ( ( *itDimNames ).compare( fieldLat->GetName() ) )
				{
					bFoundLatDim = true;
				}
				if ( ( *itDimNames ).compare( fieldLon->GetName() ) )
				{
					bFoundLonDim = true;
				}

			}

			if ( ( !bFoundLatDim ) || ( !bFoundLonDim ) )
			{
				continue;
			}

			m_paramVars.Insert( ( *it ) );
			nFields = m_paramVars.size();
			break;

		}

		if ( m_paramVars.size() <= 0 )
		{
			std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfZFLatLon - No field with at least one dimension to display has been found in file '%s'",
				externalFile->GetName().c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}
	}




	if ( fieldLat->GetDimIds().size() != 1 )
	{
		std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfZFLatLon : Array of field '%s' is not 1 dimensional",
			fieldLat->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}

	if ( fieldLon->GetDimIds().size() != 1 )
	{
		std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfZFLatLon : Array of field '%s' is not 1 dimensional",
			fieldLon->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}


	CStringList::const_iterator itParamVar;

	for ( itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
	{
		CUIntArray dimValues;
		externalFile->GetDimensions( ( *itParamVar ), dimValues );

		if ( dimValues.size() != 2 )
		{
			std::string msg = CTools::Format( "CBratDisplayApp::GetParametersNetcdfZFXY : Array of field '%s' is not 2 dimensional",
				( *itParamVar ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}
	}


	m_fields.resize( nFields );

	GetWPlotPropertyParams( nFields );

	uint32_t index = 0;

	for ( itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
	{
		// Add field to world plot group
		CWPlot* wplot = dynamic_cast<CWPlot*>( m_plots[ groupNumber ] );
		if ( wplot == NULL )
		{
			wplot = new CWPlot( groupNumber );
			m_plots[ groupNumber ] = wplot;
		}

		CPlotField* field = new CPlotField( ( *itParamVar ).c_str() );
		wplot->m_fields.Insert( field );


		CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>( Prepare( externalFile->GetName().c_str(), ( *itParamVar ).c_str() ) );

		field->m_internalFiles.Insert( zfxy );
		field->m_worldProps = GetWorldPlotProperty( index );

		m_fields[ index ] = ( *itParamVar );

		index++;
	}

	m_isZFLatLon = true;

	return true;
}



bool CmdLineProcessor::GetParametersNetcdf()
{
	bool bOk = false;

	CExternalFiles* externalFileTmp = NULL;

	try
	{
		// Test if file is a Netcdf file
		externalFileTmp = BuildExistingExternalFileKind( m_paramFile );
		if ( externalFileTmp != NULL )
		{
			bOk = true;
		}
	}
	catch ( CFileException &e )
	{
		// it's not a netcdf file (it's a parameters file) or not a valid netcdf file
		//Do Nothing
		e.what(); // to avoid warning compilation error
	}
	catch ( CException& e )
	{
		throw ( e );
	}
	catch ( ... )
	{
		throw;
	}

	//-----------------
	if ( !bOk )
	{
		return bOk;
	}
	//-----------------

	CExternalFilesNetCDF* externalFile = dynamic_cast<CExternalFilesNetCDF*>( externalFileTmp );

	if ( externalFile == NULL )
	{
		std::string msg = CTools::Format( "ERROR - The input file '%s' is not a NetCdf file",
			externalFile->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}

	// Open the Netcdf file
	externalFile->Open();

	m_inputFiles.Insert( m_paramFile );

	if ( ( ( !m_paramXAxis.empty() ) && ( !m_paramYAxis.empty() ) )
		|| ( ( m_paramXAxis.empty() ) && ( m_paramYAxis.empty() ) ) )
	{

		CFieldNetCdf* fieldLat = externalFile->FindLatField();
		CFieldNetCdf* fieldLon = externalFile->FindLonField();

		if ( ( fieldLat != NULL ) && ( fieldLon != NULL ) )
		{
			bOk = GetParametersNetcdfZFLatLon( externalFile );
		}
		else
		{
			bOk = GetParametersNetcdfZFXY( externalFile );
		}


	}
	else
	{
		bOk = GetParametersNetcdfYFX( externalFile );
	}

	externalFile->Close();
	delete externalFile;
	externalFile = NULL;

	return bOk;
}

