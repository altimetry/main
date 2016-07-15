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

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"
#include "libbrathl/ExternalFiles.h"
#include "libbrathl/ExternalFilesNetCDF.h"
#include "libbrathl/ExternalFilesFactory.h"
#include "libbrathl/Field.h"

using namespace brathl;

#if defined(BRAT_V3)
	#include "MapColor.h"
	#include "wxInterface.h"
	#include "wx/progdlg.h"
#endif


#include "BratLogger.h"
#include "Plots.h"
#include "XYPlotData.h"


//-------------------------------------------------------------
//------------------- CXYPlotData class --------------------
//-------------------------------------------------------------

CXYPlotData::CXYPlotData( CYFXPlot *plot, size_t iField )
	: base_t( plot->FieldData( iField ) )
{
	//m_aborted = false;

	Create( plot );
}

//virtual 
CXYPlotData::~CXYPlotData()
{
	m_otherVars.RemoveAll();
}



void CXYPlotData::SetXLog( bool value )
{
	if ( values_base_t::mMinXValue <= 0 && value )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotData::SetXLog : X range [%g, %g] contains value <= 0 - no logarithmic axis possible", 
			values_base_t::mMinXValue, values_base_t::mMaxXValue );
		CException e( msg, BRATHL_LOGIC_ERROR );
		throw ( e );
	}

	GetPlotProperties()->SetXLog( value );	//v3 assigned before throw
}


void CXYPlotData::SetYLog( bool value )
{
	if ( values_base_t::mMinYValue <= 0 && value )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotData::SetYLog : Y range [%g, %g] contains value <= 0 - no logarithmic axis possible", 
			values_base_t::mMinYValue, values_base_t::mMaxYValue );
		CException e( msg, BRATHL_LOGIC_ERROR );
		throw ( e );
	}

	GetPlotProperties()->SetYLog( value );	//v3 assigned before throw
}
//----------------------------------------
void CXYPlotData::Create( CYFXPlot *plot )
{
	CInternalFiles *yfx = GetPlotProperties()->InternalFile( 0 );		//GetPlotProperties() == plot->FieldData( iField )

	std::string varXName;

	CUnit unitXRead;
	CUnit unitYRead;

	std::string unitXStr;
	std::string unitYStr;

#if defined (BRAT_V3)
	if ( GetPlotProperties()->Title().empty() )
	{
		GetPlotProperties()->SetTitle( plot->mTitle );
	}

	if ( GetPlotProperties()->Xlabel().empty() )
	{
		GetPlotProperties()->SetXlabel( plot->m_titleX );
	}
	else
	{
		std::string titleX = GetPlotProperties()->Xlabel();
		titleX += plot->m_unitXLabel;
		GetPlotProperties()->SetXlabel( titleX );
	}

	if ( GetPlotProperties()->Ylabel().empty() )
	{
		GetPlotProperties()->SetYlabel( plot->m_titleY );
	}
	else
	{
		std::string titleY = GetPlotProperties()->Ylabel();
		titleY += plot->m_unitYLabel;
		GetPlotProperties()->SetYlabel( titleY );
	}

#else

	assert__( !GetPlotProperties()->Xlabel().empty() );
	assert__( !GetPlotProperties()->Ylabel().empty() );

#endif

	CUnit unitX = plot->XUnit();
	CUnit unitY = plot->YUnit();

	CExpressionValue varX;
	ExpressionValueDimensions dimValX;

	NetCDFVarKind varKind;
	CStringArray varDimNames;


	// Get X axis data
	plot->GetAxisX( yfx, &dimValX, &varX, &varXName );

	// Get and control unit of X axis
	// X units are compatible but not the same --> convert
	unitXRead = yfx->GetUnit( varXName );
	if ( unitX.AsString() != unitXRead.AsString() )
	{
		plot->XUnit().SetConversionFrom( unitXRead );
		plot->XUnit().ConvertVector( varX.GetValues(), (int32_t)varX.GetNbValues() );
	}

	CExpressionValue varY;
	std::string fieldName =  GetPlotProperties()->FieldName();

	// Set name of the data

	if ( GetPlotProperties()->UserName().empty() )
	{
		std::string str = yfx->GetTitle( fieldName ) + " (" + fieldName + ")";
		GetPlotProperties()->SetUserName( str );
	}

	// Read Y data dimension
	ExpressionValueDimensions dimValY;

	yfx->GetVarDims( fieldName, dimValY );
	if ( ( dimValY.size() <= 0 ) || ( dimValY.size() > 2 ) )
	{
		std::string msg = CTools::Format( "CXYPlotData::Create - '%s' axis -> number of dimensions  must be 1 or 2 - Found : %ld",
			fieldName.c_str(), (long)dimValY.size() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );
	}

	// Read Y data
	unitYRead = yfx->GetUnit( fieldName );
	yfx->ReadVar( fieldName, varY, unitYRead.GetText() );
	varKind	= yfx->GetVarKind( fieldName );
	/************************DEDEDE
	if (varKind != Data)
	{
	std::string msg = CTools::Format("CXYPlotData::Create - variable '%s' is not a kind of Data (%d) : %s",
	fieldName.c_str(), Data, CNetCDFFiles::VarKindToString(varKind).c_str());
	CException e(msg, BRATHL_INCONSISTENCY_ERROR);
	CTrace::Tracer("%s", e.what());
	throw (e);
	}
	*/
	// Y units are compatible but not the same --> convert
	if ( unitY.AsString() != unitYRead.AsString() )
	{
		plot->YUnit().SetConversionFrom( unitYRead );
		plot->YUnit().ConvertVector( varY.GetValues(), (int32_t)varY.GetNbValues() );
	}

	CStringArray commonDims;
	bool intersect = yfx->GetCommonVarDims( varXName, fieldName, commonDims );

	if ( !intersect )
	{
		std::string msg = CTools::Format( "CXYPlotData::Create - variables '%s' and '%s' have no common dimension",	fieldName.c_str(), varXName.c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );

	}

	CStringArray dimXNames;
	CStringArray dimYNames;
	yfx->GetVarDims( varXName, dimXNames );
	yfx->GetVarDims( fieldName, dimYNames );

	if ( ( commonDims.size() == dimXNames.size() ) && ( commonDims.size() == dimYNames.size() ) && ( dimXNames != dimYNames ) )
	{
		std::string msg = CTools::Format( "CXYPlotData::Create - variables '%s' (dim %s) and '%s' (dim %s) must have the same dimension fields",
			fieldName.c_str(), dimYNames.ToString().c_str(), varXName.c_str(), dimXNames.ToString().c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		LOG_TRACE( e.what() );
		throw ( e );

	}


	CUIntArray xCommonDimIndex;
	CUIntArray yCommonDimIndex;

	for ( uint32_t iIntersect = 0; iIntersect < commonDims.size(); iIntersect++ )
	{
		int32_t dimIndex = -1;

		dimIndex = yfx->GetVarDimIndex( varXName, commonDims.at( iIntersect ) );
		if ( dimIndex < 0 )
		{
			std::string msg = CTools::Format( "CXYPlotData::Create - variables '%s' - dim '%s' not found ",
				varXName.c_str(), commonDims.at( iIntersect ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			LOG_TRACE( e.what() );
			throw ( e );
		}

		xCommonDimIndex.Insert( dimIndex );

		dimIndex = yfx->GetVarDimIndex( fieldName, commonDims.at( iIntersect ) );
		if ( dimIndex < 0 )
		{
			std::string msg = CTools::Format( "CXYPlotData::Create - variables '%s' - dim '%s' not found ",
				fieldName.c_str(), commonDims.at( iIntersect ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			LOG_TRACE( e.what() );
			throw ( e );
		}

		yCommonDimIndex.Insert( dimIndex );
	}


	CStringArray complementDims;
	bool complement = false;

	if ( dimXNames.size() < dimYNames.size() )
	{
		complement = yfx->GetComplementVarDims( varXName, fieldName, complementDims );
	}
	else
	{
		complement = yfx->GetComplementVarDims( fieldName, varXName, complementDims );
	}


	CExternalFilesNetCDF* externalFile = dynamic_cast<CExternalFilesNetCDF*> ( BuildExistingExternalFileKind( yfx->GetName() ) );
	if ( externalFile != nullptr )
	{
		externalFile->Open();
	}

	if ( complement && externalFile != nullptr )
	{
		// Read 'Complement dim' var data
		CFieldNetCdf* field = externalFile->GetFieldNetCdf( complementDims.at( 0 ), false );
		if ( field != nullptr )
		{
			m_fieldComplement = *field;
			CUnit unit = yfx->GetUnit( field->GetName() );

			yfx->ReadVar( field->GetName(), m_varComplement, unit.GetText() );
		}
	}


	if ( externalFile != nullptr )
	{
		// Read other var data (non plot data)
		//CStringArray::const_iterator itStringArray;

		m_otherVars.RemoveAll();
		//m_otherVars.resize(plot->m_nonPlotFieldNames.size());


		for ( uint32_t iOtherVars = 0; iOtherVars < plot->m_nonPlotFieldNames.size(); iOtherVars++ )
		{
			CFieldNetCdf* field = externalFile->GetFieldNetCdf( plot->m_nonPlotFieldNames.at( iOtherVars ), false );
			if ( field != nullptr )
			{
				m_otherFields.Insert( new CFieldNetCdf( *field ) );

				CUnit unit = yfx->GetUnit( field->GetName() );

				CExpressionValue* exprValue = new CExpressionValue();

				//yfx->ReadVar(field->GetName(), m_otherVars[iOtherVars], unit.GetText());
				yfx->ReadVar( field->GetName(), *exprValue, unit.GetText() );

				m_otherVars.Insert( exprValue );
			}
		}
	}

	//m_otherFields.Dump(*(CTrace::GetInstance()->GetDumpContext()));

	if ( externalFile != nullptr )
	{
		externalFile->Close();
		delete externalFile;
		externalFile = nullptr;
	}


	SetData( varX.GetValues(), dimValX, varY.GetValues(), dimValY, xCommonDimIndex, yCommonDimIndex );
}


//-------------------------------------------------------------
//------------------- ex-CXYPlotDataMulti class --------------------
//-------------------------------------------------------------

void CXYPlotData::SetData( 
	double* xArray, const CUIntArray& xDims, 
	double* yArray, const CUIntArray& yDims,
	const CUIntArray& xCommonDimIndex,
	const CUIntArray& yCommonDimIndex
	)
{
	assert__( !GetPlotProperties()->UserName().empty() );
	//{
	//	GetPlotProperties()->SetUserName( "Dataset" );	//what ???
	//}

	CUIntArray xDimIndex;
	CUIntArray yDimIndex;

	uint32_t iDimIndex = 0;

	for ( iDimIndex = 0; iDimIndex < xDims.size(); iDimIndex++ )
	{
		xDimIndex.Insert( iDimIndex );
	}

	for ( iDimIndex = 0; iDimIndex < yDims.size(); iDimIndex++ )
	{
		yDimIndex.Insert( iDimIndex );
	}

	double* newX = nullptr;
	double* newY = nullptr;

	if ( ( xDims.size() <= 0 ) || ( yDims.size() <= 0 ) )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : At least one of Arrays is 0 dimensional  -"
			"xArray dim. is %ld and yArray dim. is %ld",
			(long)xDims.size(), (long)yDims.size() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );

	}

	if ( ( xCommonDimIndex.size() <= 0 ) || ( yCommonDimIndex.size() <= 0 ) )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : No common dimension have been set."
			"xArray dim. is %ld and yArray dim. is %ld - "
			"xCommonDimIndex dim. is %ld and yCommonDimIndex dim. is %ld",
			(long)xDims.size(), (long)yDims.size(),
			(long)xCommonDimIndex.size(), (long)yCommonDimIndex.size() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );

	}

	if ( ( xDims.size() > 2 ) || ( yDims.size() > 2 ) )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : Arrays must be 2 dimensional or less -"
			"xArray dim. is %ld and yArray dim. is %ld",
			(long)xDims.size(), (long)yDims.size() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );

	}

	if ( ( xCommonDimIndex.size() > 2 ) || ( yCommonDimIndex.size() > 2 ) )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : Common dimension arrays must be 2 dimensional or less -"
			"xCommonDimIndex dim. is %ld and yCommonDimIndex dim. is %ld",
			(long)xCommonDimIndex.size(), (long)yCommonDimIndex.size() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );


	}

	if ( xCommonDimIndex.size() != yCommonDimIndex.size() )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : Common dimension arrays have not the same dimension -"
			"xCommonDimIndex dim. is %ld and yCommonDimIndex dim. is %ld",
			(long)xCommonDimIndex.size(), (long)yCommonDimIndex.size() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		throw ( e );

	}

	int32_t maxFrames = 0;
	bool hasComplement = false;
	CUIntArray complementX;
	CUIntArray complementY;

	if ( xCommonDimIndex.size() == 1 )
	{
		if ( ( xDims.size() == 1 ) && ( yDims.size() == 1 ) )
		{
			maxFrames = 1;
		}
		else if ( xDims.size() == 1 )
		{
			hasComplement = yCommonDimIndex.Complement( yDimIndex, complementX );
			if ( !hasComplement )
			{
				std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : No X complement have been found:"
					"xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s -"
					"xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
					(long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
					(long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
					(long)xDimIndex.size(), xDimIndex.ToString().c_str(),
					(long)yDimIndex.size(), yDimIndex.ToString().c_str()
					);
				CException e( msg, BRATHL_INCONSISTENCY_ERROR );
				throw ( e );
			}
			maxFrames = yDims.at( complementX.at( 0 ) );
		}
		else if ( yDims.size() == 1 )
		{
			hasComplement = xCommonDimIndex.Complement( xDimIndex, complementY );
			if ( !hasComplement )
			{
				std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : No Y complement have been found:"
					"xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s"
					"xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
					(long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
					(long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
					(long)xDimIndex.size(), xDimIndex.ToString().c_str(),
					(long)yDimIndex.size(), yDimIndex.ToString().c_str()
					);
				CException e( msg, BRATHL_INCONSISTENCY_ERROR );
				throw ( e );
			}
			maxFrames = xDims.at( complementY.at( 0 ) );
		}
		else
		{
			std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : X and Y dimension must be equals:"
				"xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s"
				"xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
				(long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
				(long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
				(long)xDimIndex.size(), xDimIndex.ToString().c_str(),
				(long)yDimIndex.size(), yDimIndex.ToString().c_str()
				);
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}
	}
	else
	{
		hasComplement = yCommonDimIndex.Complement( yDimIndex, complementX );
		hasComplement |= xCommonDimIndex.Complement( xDimIndex, complementY );
		if ( hasComplement )
		{
			std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : X and Y must have the same dimension fields:"
				"xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s"
				"xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
				(long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
				(long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
				(long)xDimIndex.size(), xDimIndex.ToString().c_str(),
				(long)yDimIndex.size(), yDimIndex.ToString().c_str()
				);
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}
		else if ( xCommonDimIndex != yCommonDimIndex )
		{
			std::string msg = CTools::Format( "ERROR in CXYPlotDataMulti::SetData : X and Y dimension must be equals:"
				"xCommonDimIndex dim. is %ld, values are %s and yCommonDimIndex dim. is %ld, values are %s"
				"xDimIndex dim. is %ld, values are %s and yDimIndex dim. is %ld, values are %s",
				(long)xCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
				(long)yCommonDimIndex.size(), xCommonDimIndex.ToString().c_str(),
				(long)xDimIndex.size(), xDimIndex.ToString().c_str(),
				(long)yDimIndex.size(), yDimIndex.ToString().c_str()
				);
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			throw ( e );
		}

		maxFrames = 1;

	}

#if defined BRAT_V3		// TODO: some callback device to display progress

	wxProgressDialog* pd = new wxProgressDialog("Calculating Frames...",
		"",
		maxFrames, nullptr,
		wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_APP_MODAL);
	pd->SetSize(-1,-1, 350, -1);

#endif

	int32_t i = 0;

	for ( i = 0; i < maxFrames; i++ )
	{
		int32_t xDim = -1;
		int32_t yDim = -1;

#if defined BRAT_V3		// TODO: some callback device to display progress
		std::string msg = CTools::Format( "Calculating Frame: %d of %d", i + 1, maxFrames );
		if (pd->Update(i, msg.c_str()) == false)
		{
			m_aborted = true;
			break;
		}
#endif

		if ( xDims.size() == 1 )
		{
			xDim = xDims[ 0 ];
			newX = new double[xDim];
			memcpy( newX, xArray, xDim * sizeof( *xArray ) );
		}
		else
		{
			if ( xCommonDimIndex.size() == 1 )
			{
				xDim = xDims[ xCommonDimIndex.at( 0 ) ];
				newX = new double[xDim];
				if ( xCommonDimIndex.at( 0 ) == 1 )
				{
					memcpy( newX, xArray + ( i * xDim ), xDim * sizeof( *xArray ) );
				}
				else
				{
					for ( int32_t j = 0; j < xDim; j++ )
					{
						newX[ j ] = xArray[ i + ( j * maxFrames ) ];
					}
				}
			}
			else
			{
				xDim = xDims[ xCommonDimIndex.at( 0 ) ] * xDims[ xCommonDimIndex.at( 1 ) ];
				newX = new double[xDim];
				memcpy( newX, xArray + ( i * xDim ), xDim * sizeof( *xArray ) );
			}

		}

		if ( yDims.size() == 1 )
		{
			yDim = yDims[ 0 ];
			newY = new double[yDim];
			memcpy( newY, yArray, yDim * sizeof( *yArray ) );
		}
		else
		{
			if ( yCommonDimIndex.size() == 1 )
			{
				yDim = yDims[ yCommonDimIndex.at( 0 ) ];
				newY = new double[yDim];
				if ( yCommonDimIndex.at( 0 ) == 1 )
				{
					memcpy( newY, yArray + ( i * yDim ), yDim * sizeof( *yArray ) );
				}
				else
				{
					for ( int32_t j = 0; j < yDim; j++ )
					{
						newY[ j ] = yArray[ i + ( j * maxFrames ) ];
					}
				}
			}
			else
			{
				yDim = yDims[ yCommonDimIndex.at( 0 ) ] * yDims[ yCommonDimIndex.at( 1 ) ];
				newY = new double[yDim];
				memcpy( newY, yArray + ( i * yDim ), yDim * sizeof( *yArray ) );
			}
		}

		values_base_t::AddFrameData( newX, xDim, newY, yDim );
	}

#if defined BRAT_V3		// TODO: some callback device to display progress

	pd->Destroy();

	if ( m_aborted )
	{
		return;
	}
#endif

}
