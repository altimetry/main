#include "new-gui/brat/stdafx.h"

#include "DisplayFilesProcessor.h"

#include "new-gui/Common/tools/Trace.h"
#include "libbrathl/ExternalFilesNetCDF.h"
#include "libbrathl/ExternalFilesFactory.h"
#include "libbrathl/InternalFilesZFXY.h"
#include "libbrathl/InternalFilesYFX.h"

#include "DataModels/PlotData/Plots.h"
#include "DataModels/PlotData/FieldData.h"



bool CDisplayFilesProcessor::GetParametersNetcdfYFX( CExternalFilesNetCDF* externalFile )
{
	uint32_t groupNumber = 0;

	m_inputFileType = CInternalFilesYFX::TypeOf();

	m_plots.resize( groupNumber + 1 );

	int32_t nFields = (int32_t )m_paramVars.size();

	if ( nFields == 0 )
	{
		CStringArray varNames;
		externalFile->GetVariables( varNames );

		for ( auto it = varNames.begin(); it != varNames.end(); it++ )
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
			nFields = (int32_t)m_paramVars.size();
			break;

		}

		if ( m_paramVars.size() <= 0 )
		{
			std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfYFX - No field with one dimension to display has been found in file '%s'",
				externalFile->GetName().c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}
	}

	if ( !m_paramYAxis.empty() && nFields > 1 )
	{
		m_paramXAxis = m_paramYAxis;
		m_paramYAxis = "";
	}

	if ( !m_paramYAxis.empty() && nFields == 1 )
	{
		m_paramXAxis = *( m_paramVars.begin() );
		m_paramVars.RemoveAll();
		m_paramVars.Insert( m_paramYAxis );
	}


	CUIntArray xDimValues;
	externalFile->GetDimensions( m_paramXAxis, xDimValues );
	if ( xDimValues.size() <= 0 )
	{
		std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfYFX : Array of field '%s' is 0 dimensional - It have to be at least 1 or 2 dimensional",
			m_paramXAxis.c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}

	if ( xDimValues.size() > 2 )
	{
		std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfYFX : Array of field '%s' is more than 2 dimensional - It have to be 1 or 2 dimensional",
			m_paramXAxis.c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}


	for ( auto itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
	{
		CUIntArray dimValues;
		externalFile->GetDimensions( ( *itParamVar ), dimValues );

		if ( dimValues.size() <= 0 )
		{
			std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfYFX : Array of field '%s' is 0 dimensional - It have to be at least 1 or 2 dimensional",
				( *itParamVar ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );

		}

		if ( dimValues.size() > 2 )
		{
			std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfYFX : Array of field '%s' is more than 2 dimensional - It have to be 1 or 2 dimensional",
				( *itParamVar ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}

	}


	m_fields.resize( nFields );

	GetXYPlotPropertyParams( nFields );

	unsigned index = 0;
	for ( auto itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
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

		if ( !m_paramXAxis.empty() )
		{
			plot->SetForcedVarXname( m_paramXAxis.c_str() );
		}

		CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>( PrepareNetcdf( externalFile->GetName().c_str() ) );

		props->PushInternalFile( yfx );		//field->m_xyProps = GetXYPlotProperties( index );

		m_fields[ index ] = *itParamVar;

		index++;
	}

	m_isYFX = true;

	return true;
}



bool CDisplayFilesProcessor::GetParametersNetcdfZFXY( CExternalFilesNetCDF* externalFile )
{

	CNetCDFFiles* netCDFFile = externalFile->GetFile();

	if ( netCDFFile == nullptr )
	{
		std::string msg = CTools::Format( "ERROR - The input file '%s' is not a NetCdf file (internal file is nullptr)",
			externalFile->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}


	uint32_t groupNumber = 0;

	m_inputFileType = CInternalFilesZFXY::TypeOf();

	m_plots.resize( groupNumber + 1 );

	int32_t nFields = (int32_t)m_paramVars.size();

	CFieldNetCdf* fieldX = nullptr;
	CFieldNetCdf* fieldY = nullptr;

	bool bFoundLatDim = false;
	bool bFoundLonDim = false;

	std::string coordAxisX;
	std::string coordAxisY;

	if ( nFields == 0 )
	{
		CStringArray varNames;
		externalFile->GetVariables( varNames );


		for ( auto it = varNames.begin(); it != varNames.end(); it++ )
		{
			bFoundLatDim = false;
			bFoundLonDim = false;

			if ( externalFile->IsAxisVar( *it ) )
			{
				continue;
			}

			CStringArray dimNames;
			externalFile->GetDimensions( *it, dimNames );

			if ( dimNames.size() != 2 )
			{
				continue;
			}

			fieldX = externalFile->GetFieldNetCdf( dimNames.at( 0 ) );
			fieldY = externalFile->GetFieldNetCdf( dimNames.at( 1 ) );

			if ( ( fieldX == nullptr ) || ( fieldY == nullptr ) )
			{
				continue;
			}

			m_paramVars.Insert( *it );
			nFields = (int32_t)m_paramVars.size();
			break;

		}

		if ( m_paramVars.size() <= 0 )
		{
			std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfZFXY - No field with at least one dimension to display has been found in file '%s'",
				externalFile->GetName().c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}
	}


	if ( fieldX->GetDimIds().size() != 1 )
	{
		std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfZFXY : Array of field '%s' is not 1 dimensional.",
			fieldX->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}

	if ( fieldY->GetDimIds().size() != 1 )
	{
		std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfZFXY : Array of field '%s' is not 1 dimensional.",
			fieldY->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}


	for ( auto itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
	{
		CUIntArray dimValues;
		externalFile->GetDimensions( ( *itParamVar ), dimValues );


		if ( dimValues.size() != 2 )
		{
			std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfZFXY : Array of field '%s' is not 2 dimensional.",
				( *itParamVar ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}
	}


	CNetCDFVarDef* firstNetCDFVardef = nullptr;

	for ( auto itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
	{
		CNetCDFVarDef* netCDFVarDef = netCDFFile->GetNetCDFVarDef( *itParamVar );

		if ( firstNetCDFVardef == nullptr )
		{
			continue;
		}

		if ( netCDFVarDef->GetNbDims() != 2 )
		{
			std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfZFXY : Array of field '%s' is not 2 dimensional.",
				( *itParamVar ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}

		if ( firstNetCDFVardef == nullptr )
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

	for ( auto itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
	{
		auto *props = GetZFXYPlotProperties( index );

		// Add field to ZFXY plot group
		CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( m_plots[ groupNumber ] );
		if ( zfxyplot == nullptr )
		{
			zfxyplot = new CZFXYPlot( nullptr, groupNumber );
			m_plots[ groupNumber ] = zfxyplot;
		}

		zfxyplot->PushFieldData( props );

		if ( !m_paramXAxis.empty() )
		{
			zfxyplot->SetForcedVarXname( m_paramXAxis );
		}

		if ( !m_paramYAxis.empty() )
		{
			zfxyplot->SetForcedVarYname( m_paramYAxis );
		}


		CInternalFiles* zfxy = PrepareNetcdf( externalFile->GetName() );

		props->PushInternalFile( zfxy );		//field->m_zfxyProps = GetZFXYPlotProperties( index );

		m_fields[ index ] = *itParamVar;

		index++;
	}

	m_isZFXY = true;

	return true;
}


bool CDisplayFilesProcessor::GetParametersNetcdfZFLatLon( CExternalFilesNetCDF* externalFile )
{
	uint32_t groupNumber = 0;

	m_inputFileType = CInternalFilesZFXY::TypeOf();

	m_plots.resize( groupNumber + 1 );

	int32_t nFields = (int32_t)m_paramVars.size();

	CFieldNetCdf* fieldLat = externalFile->FindLatField();
	CFieldNetCdf* fieldLon = externalFile->FindLonField();

	if ( fieldLat == nullptr )
	{
		std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfZFLatLon - No Latitude field has been found in file '%s'",
			externalFile->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		CTrace::Tracer( "%s", e.what() );
		throw ( e );
	}

	if ( fieldLon == nullptr )
	{
		std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfZFLatLon - No Longitude field has been found in file '%s'",
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

		for ( auto it = varNames.begin(); it != varNames.end(); it++ )
		{
			bFoundLatDim = false;
			bFoundLonDim = false;

			if ( externalFile->IsAxisVar( *it ) )
			{
				continue;
			}

			CUIntArray dimValues;
			externalFile->GetDimensions( *it, dimValues );

			if ( dimValues.size() != 2 )
			{
				continue;
			}

			CStringArray dimNames;
			externalFile->GetDimensions( *it, dimNames );

			for ( auto const &itDimName : dimNames )
			{
				bFoundLatDim = itDimName == fieldLat->GetName();
				bFoundLonDim = itDimName == fieldLon->GetName();
			}

			if ( !bFoundLatDim || !bFoundLonDim )
			{
				continue;
			}

			m_paramVars.Insert( ( *it ) );
			nFields = (int32_t)m_paramVars.size();
			break;
		}

		if ( m_paramVars.size() <= 0 )
		{
			std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfZFLatLon - No field with at least one dimension to display has been found in file '%s'",
				externalFile->GetName().c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}
	}

	if ( fieldLat->GetDimIds().size() != 1 )
	{
		std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfZFLatLon : Array of field '%s' is not 1 dimensional",
			fieldLat->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}

	if ( fieldLon->GetDimIds().size() != 1 )
	{
		std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfZFLatLon : Array of field '%s' is not 1 dimensional",
			fieldLon->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}


	for ( auto itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
	{
		CUIntArray dimValues;
		externalFile->GetDimensions( ( *itParamVar ), dimValues );

		if ( dimValues.size() != 2 )
		{
			std::string msg = CTools::Format( "CDisplayFilesProcessor::GetParametersNetcdfZFXY : Array of field '%s' is not 2 dimensional",
				( *itParamVar ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}
	}


	m_fields.resize( nFields );

	GetWPlotPropertyParams( nFields );

	uint32_t index = 0;

	for ( auto itParamVar = m_paramVars.begin(); itParamVar != m_paramVars.end(); itParamVar++ )
	{
		auto *props = GetWorldPlotProperties( index );

		// Add field to world plot group
		CGeoPlot* wplot = dynamic_cast<CGeoPlot*>( m_plots[ groupNumber ] );
		if ( wplot == nullptr )
		{
			wplot = new CGeoPlot( nullptr, groupNumber );
			m_plots[ groupNumber ] = wplot;
		}

		wplot->PushFieldData( props );

		CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>( PrepareNetcdf( externalFile->GetName().c_str() ) );

		props->PushInternalFile( zfxy );		//field->m_worldProps = GetWorldPlotProperties( index );

		m_fields[ index ] = *itParamVar;

		index++;
	}

	m_isZFLatLon = true;

	return true;
}



bool CDisplayFilesProcessor::BuildPlotsFromNetcdf()
{
	bool bOk = false;

	CExternalFiles* externalFileTmp = nullptr;

	try
	{
		// Test if file is a Netcdf file

		externalFileTmp = BuildExistingExternalFileKind( m_paramFile );
		if ( externalFileTmp != nullptr )
		{
			bOk = true;
		}
	}
	catch ( CFileException & )
	{
		// it's not a netcdf file (it's a parameters file) or not a valid netcdf file: Do Nothing
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

	if ( externalFile == nullptr )
	{
		std::string msg = CTools::Format( "ERROR - The input file '%s' is not a NetCdf file", externalFile->GetName().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );
	}

	externalFile->Open();

	mOperationOutputFiles.Insert( m_paramFile );

	if (( !m_paramXAxis.empty() && !m_paramYAxis.empty() ) || 
		( m_paramXAxis.empty() && m_paramYAxis.empty() ) )
	{
		CFieldNetCdf* fieldLat = externalFile->FindLatField();
		CFieldNetCdf* fieldLon = externalFile->FindLonField();

		if ( !mMapsAsPlots && ( fieldLat != nullptr && fieldLon != nullptr ) )
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

	return bOk;
}

