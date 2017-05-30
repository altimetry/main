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

#include "common/tools/Trace.h"
#include "common/tools/Exception.h"
#include "new-gui/Common/ApplicationLoggerBase.h"
#include "libbrathl/InternalFiles.h"
#include "libbrathl/InternalFilesYFX.h"
#include "libbrathl/InternalFilesZFXY.h"
#include "libbrathl/Tools.h"

using namespace brathl;

#include "FieldData.h"
#include "GeoPlotField.h"
#include "ZFXYPlotField.h"

#include "Plots.h"



////////////////////////////////////////////////////////////
//						CPlotBase class 
////////////////////////////////////////////////////////////


template< typename DATA_FIELD, typename PLOT_FIELD >
void CPlotBase< DATA_FIELD, PLOT_FIELD >::GetAllInternalFiles( std::vector< CInternalFiles* >& allInternalFiles ) const
{
	for ( auto *field : mDataFields )
	{
		if ( field != nullptr )
		{
			for ( auto *InternalFile : field->InternalFiles() )
			{
				allInternalFiles.push_back( InternalFile );
			}
		}
	}
}



template< typename DATA_FIELD, typename PLOT_FIELD >
//static 
void CPlotBase< DATA_FIELD, PLOT_FIELD >::GetVar( const std::string& varName, CInternalFiles* file, ExpressionValueDimensions* dimVal, CExpressionValue* var )
{
	if ( !file->VarExists( varName ) )
	{
		std::string msg = CTools::Format( "CPlotBase::GetAxis -  unknown variable name: '%s'", varName.c_str() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		CTrace::Tracer( "%s", e.what() );
		throw ( e );
	}

	// Get dim
	if ( dimVal != nullptr )
	{
		file->GetVarDims( varName, *dimVal );
		if ( ( dimVal->size() <= 0 ) || ( dimVal->size() > 2 ) )
		{
			std::string msg = CTools::Format( "CPlotBase::GetAxis - '%s' axis -> number of dimensions must be 1 or 2 - Found : %ld",
				varName.c_str(), (long)dimVal->size() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}
	}

	// Get values
	if ( var != nullptr )
	{
		file->ReadVar( varName, *var, file->GetUnit( varName ).GetText() );
	}
}




////////////////////////////////////////////////////////////
//						CGeoPlot class 
////////////////////////////////////////////////////////////


void CGeoPlot::GetInfo()
{
#if defined(BRAT_V3)

	const CFieldData *field = *mDataFields.begin();			assert__( field != nullptr );

	if ( mTitle.empty() )
	{
		if ( field->InternalFiles().size() > 0 )
		{
			CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>( field->InternalFile( 0 ) );		assert__( zfxy );
			if ( !zfxy->IsGeographic() )
			{
				CException e( "CGeoPlot::GetInternalFilesZFXY  - a ZFXY data seems to be NOT geographical  - zfxy->IsGeographic() is true",
					BRATHL_LOGIC_ERROR );
				LOG_TRACE( e.what() );
				throw e;
			}
			mTitle = zfxy->GetTitle( "" );
		}
	}

#else

	//////////////////////////////////////////////////
	// v4 ex-GUI Code
	//////////////////////////////////////////////////

	// for Geo-strophic velocity
	//
	CFieldData *northField =nullptr;
	CFieldData *eastField =nullptr;

	for ( auto *field : mDataFields )
	{
		if ( field->InternalFiles().empty() )
			continue;

		if ( field->IsNorthComponent() && northField == nullptr )
		{
			northField = field;
			continue;
		}
		else
			if ( field->IsEastComponent() && eastField == nullptr )
			{
				eastField = field;
				continue;
			}

		// otherwise just add it as regular data
		mPlotFields.push_back( new CGeoPlotField( field ) );		//v4 note: CGeoPlotField ctor is only invoked here
	}

	// we have a Vector Plot!
	if ( northField != nullptr && eastField != nullptr )
	{
		mPlotFields.push_back( new CGeoPlotVelocityField( northField, eastField ) );	//v4 note: CGeoPlotVelocityField ctor is only invoked here
	}
	else if ( northField != eastField )
	{
		CException e( "CMapEditor::ViewChanged() - incomplete std::vector plot components", BRATHL_INCONSISTENCY_ERROR );
		LOG_TRACE( e.what() );
		throw e;
	}

#endif

}




////////////////////////////////////////////////////////////
//						CMathPlot class 
////////////////////////////////////////////////////////////

//static
std::string CDisplayInterface::MakeUnitLabel( const CUnit &unit )
{
#if defined (BRAT_V3)
	return "\nUnit:\t" + unit.GetText();
#else
	return "[" + unit.GetText() + "]";
#endif
}


template< typename DATA_FIELD, typename PLOT_FIELD >
void CMathPlot< DATA_FIELD, PLOT_FIELD >::GetTitleAndUnitInfo( CInternalFiles *file, const std::string &varXName, std::string varYName, const std::string &fieldName, 
	bool useYnameInLabel )
{
	if ( varYName.empty() )
		varYName = fieldName;

#if defined (BRAT_V3)
	//-----------------------------------
	// Get plot Title --> title of the first file
	//-----------------------------------
	if ( mTitle.empty() )
	{
		mTitle = file->GetTitle( "" );
	}
#endif

	//-----------------------------------
	// Get and control unit of X axis
	//-----------------------------------

	CUnit unitXRead = file->GetUnit( varXName );

	if ( m_unitXLabel.empty() )	//unitX read for the 1st time
	{
		m_unitX = unitXRead;
		m_unitXLabel = MakeUnitLabel( m_unitX );
	}
	else
	{
		if ( !m_unitX.IsCompatible( unitXRead ) )
		{
			std::string msg = CTools::Format( "CMathPlot::GetTitleAndUnitInfo - In group field number %d, X field unit are not in the same way (not compatible)"
				"- Expected unit '%s' and found '%s' for axis X - File name is '%s'",
				m_groupNumber,
				m_unitX.AsString().c_str(),
				unitXRead.AsString().c_str(),
				file->GetName().c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}
		//if ( m_unitX.AsString().compare( unitXReadStr ) != 0 )
		//{
		//	m_unitXConv = true;
		//}
	}


	//-----------------------------------
	// Get title of X axis
	//-----------------------------------
	if ( Xtitle().empty() )
	{
		std::string titleX = file->GetTitle( varXName );
		if ( titleX.empty() )
		{
			titleX = varXName;
		}
		SetXtitle( titleX + " " + m_unitXLabel );
	}


	//--------------------------------------------------------
	// Get and control unit of Y axis (for YFX: --> unit of each field)
	//---------------------------------------------------------
	CUnit unitYRead = file->GetUnit( varYName );

	if ( m_unitYLabel.empty() )
	{
		m_unitY = unitYRead;
		m_unitYLabel = MakeUnitLabel( m_unitY );
	}
	else
	{
		if ( !m_unitY.IsCompatible( unitYRead ) )
		{
			std::string msg = CTools::Format( "CMathPlot::GetTitleAndUnitInfo - In group field number %d, Y field unit are not in the same way (not compatible)"
				"- Expected unit '%s' and found '%s' for axis Y - Field name is '%s' - File name is '%s'",
				m_groupNumber,
				m_unitY.AsString().c_str(),
				unitYRead.AsString().c_str(),
				fieldName.c_str(),
				file->GetName().c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );

		}
		//if ( m_unitY.AsString().compare( unitYReadStr ) != 0 )
		//{
		//	m_unitYConv = true;
		//}
	}


	//-----------------------------------
	// Get title of Y axis (as possible)
	//-----------------------------------

	if ( Ytitle().empty() )
	{
		std::string titleY;
		if ( useYnameInLabel )
		{
			titleY = file->GetTitle( varYName );
			if ( titleY.empty() )
			{
				titleY = varYName;
			}
		}
		SetYtitle( titleY + " " + m_unitYLabel );
	}
}




////////////////////////////////////////////////////////////
//						CYFXPlot class 
////////////////////////////////////////////////////////////


void CYFXPlot::GetAxisX( CInternalFiles *yfx, ExpressionValueDimensions *dimVal, CExpressionValue *varX, std::string *varXName ) const
{
	if ( !m_forcedVarXName.empty() )
	{
		if ( varXName != nullptr )
		{
			*varXName = m_forcedVarXName;
		}

		GetVar( m_forcedVarXName, yfx, dimVal, varX );

		return;
	}


	CStringArray axisNames;
	yfx->GetAxisVars( axisNames );

	if ( axisNames.size() != 1 )
	{
		std::string msg = CTools::Format( "CYFXPlot::GetAxisX -  wrong number of axis in file '%s' : %ld"
			"Correct number is 1",
			yfx->GetName().c_str(),
			(long)axisNames.size() );
		CException e( msg, BRATHL_INCONSISTENCY_ERROR );
		CTrace::Tracer( "%s", e.what() );
		throw ( e );
	}

	for ( CStringArray::iterator is = axisNames.begin(); is != axisNames.end(); is++ )
	{
		// Get dim
		if ( dimVal != nullptr )
		{
			yfx->GetVarDims( *is, *dimVal );
			if ( ( dimVal->size() <= 0 ) || ( dimVal->size() > 2 ) )
			{
				std::string msg = CTools::Format( "CYFXPlot::GetAxisX - '%s' axis -> number of dimensions must be 1 or 2 - Found : %ld",
					( *is ).c_str(), (long)dimVal->size() );
				CException e( msg, BRATHL_INCONSISTENCY_ERROR );
				CTrace::Tracer( "%s", e.what() );
				throw ( e );
			}
		}

		// Get values
		if ( varX != nullptr )
		{
			yfx->ReadVar( *is, *varX, yfx->GetUnit( *is ).GetText() );
		}

		// Get name
		if ( varXName != nullptr )
		{
			*varXName = ( *is );
		}
	}
}




void CYFXPlot::GetInfo()
{
	std::vector< CInternalFiles* > allInternalFiles;
	GetAllInternalFiles( allInternalFiles );
	const size_t nrFiles = allInternalFiles.size();

	const size_t nrFields = mDataFields.size();

	CStringArray plotFieldNames;

	// Axis Titles and Units

	for ( auto *field : mDataFields )
	{
#if defined (BRAT_V3)
		if ( field != nullptr && mTitle.empty() )
		{
			mTitle = field->Title();
		}
#endif
		plotFieldNames.InsertUnique( field->FieldName() );

		for ( auto *file : field->InternalFiles() )
		{
			std::string varXName;
			GetAxisX( file, nullptr, nullptr, &varXName );

			GetTitleAndUnitInfo( file, varXName, "", plotFieldNames.back(), nrFields == 1 && nrFiles <= 1 );
		}
	}


	// Collect non-PlotField Names

	if ( plotFieldNames.size() > 0 )
	{
		for ( auto *yfx : allInternalFiles )
		{
			if ( yfx == nullptr )
				continue;

			CStringArray complement;
			yfx->GetComplementVars( plotFieldNames, complement );

			for ( CStringArray::iterator itStringArray = complement.begin(); itStringArray != complement.end(); itStringArray++ )
			{
				CNetCDFVarDef* v1Def = yfx->GetNetCDFVarDef( plotFieldNames.at( 0 ) );
				CNetCDFVarDef* v2Def = yfx->GetNetCDFVarDef( *itStringArray );

				if ( v1Def == nullptr || v2Def == nullptr )
				{
					continue;
				}

				if ( yfx->GetFile()->HaveEqualDimNames( plotFieldNames.at( 0 ), *itStringArray ) )
				{
					m_nonPlotFieldNames.InsertUnique( *itStringArray );
				}
			}
		}
	}


#if !defined(BRAT_V3)

	//////////////////////////////////////////////////
	// v4 ex-GUI Code
	//////////////////////////////////////////////////

	for ( size_t i = 0; i < nrFields; i++ )
	{
		mPlotFields.push_back( new CXYPlotField( this, i ) );	//v4 note: CXYPlotField ctor only invoked here
	}

#endif
}







////////////////////////////////////////////////////////////
//						CZFXYPlot class 
////////////////////////////////////////////////////////////


void CZFXYPlot::GetInfo()
{
	// Check dimensions of the plotted variables

	CStringArray varDimsRef;

	for ( auto *field : mDataFields )
	{
		std::string fieldName = field->FieldName();

		CInternalFiles* zfxy = *field->InternalFiles().begin();

		CStringArray varDimNames;
		zfxy->GetVarDims( fieldName, varDimNames );

		if ( varDimNames.size() != 2 )
		{
			std::string msg = CTools::Format( "CZFXYPlot::GetInfo - '%s' variable -> number of dimensions must be equal to 2 - Found : %ld",
				fieldName.c_str(), (long)varDimNames.size() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}

		if ( varDimsRef.size() <= 0 )
		{
			varDimsRef.Insert( varDimNames );
		}

		if ( varDimsRef != varDimNames )
		{
			std::string msg = "CZFXYPlot::GetInfo - All variables to be plotted don't have the same dimensions";
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}
	}

	std::string varXName = varDimsRef.at( 0 );
	std::string varYName = varDimsRef.at( 1 );
	if ( !m_forcedVarXName.empty() )
	{
		varXName = m_forcedVarXName;
	}
	if ( !m_forcedVarYName.empty() )
	{
		varYName = m_forcedVarYName;
	}


	// Axis Titles and Units

	for ( auto *field : mDataFields )
	{

#if defined (BRAT_V3)
		if ( field != nullptr && mTitle.empty() )
		{
			mTitle = field->Title();						///m_xyProps ??? or m_zfxyProps ?
		}
#endif

		for ( auto *file : field->InternalFiles() )
		{
			GetTitleAndUnitInfo( file, varXName, varYName, field->FieldName(), true );
		}
	}

#if !defined(BRAT_V3)

	//////////////////////////////////////////////////
	// v4 ex-GUI Code
	//////////////////////////////////////////////////

	for ( auto *field : mDataFields )
	{
		if ( field->InternalFiles().empty() )
			continue;

		mPlotFields.push_back( new CZFXYPlotField( this, field ) );		// v4 note: CZFXYPlotField ctor only invoked here
	}

	//v4: complement GetTitleAndUnitInfo with Z title for ZFXY (for YFX is Y title)

	if ( ValueTitle().empty() && mPlotFields.size() > 0 )
	{
		if ( mPlotFields.size() == 1 )
			SetValueTitle( mPlotFields[ 0 ]->AxisUserName()  + " " + MakeUnitLabel( *mPlotFields[ 0 ]->DataUnit() ) );
		//else 
		//		???
	}

#endif
}





void CZFXYPlot::GetPlotWidthHeight(CInternalFiles* zfxy,
                           const std::string& fieldName,
                           int32_t& width, int32_t& height,
                           CExpressionValue& varX, CExpressionValue& varY,
                           uint32_t& dimRangeX, uint32_t& dimRangeY,
                           std::string& varXName, std::string& varYName)
{
  bool bOk = true;

  width = -1;
  height = -1;

  CNetCDFVarDef* netCDFVarDef = zfxy->GetNetCDFVarDef(fieldName);
  CStringArray fieldDimNames;
  netCDFVarDef->GetNetCdfDimNames(fieldDimNames);

  CObArray* netCDFDims = netCDFVarDef->GetNetCDFDims();

  if (netCDFDims->size() != 2)
  {
    std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - '%s' field -> number of dimensions not equal to 2 : %ld",
                                fieldName.c_str(), (long)netCDFDims->size());
    throw CException(msg, BRATHL_INCONSISTENCY_ERROR);
  }

  if ( !m_forcedVarXName.empty() )
  {
    bOk = netCDFVarDef->HasCommonDims( m_forcedVarXName );
    if (!bOk)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - '%s' dimension doesn't exist for the field '%s'",
                                  m_forcedVarXName.c_str(), fieldName.c_str());
      throw CException(msg, BRATHL_INCONSISTENCY_ERROR);
    }
  }

  if ( !m_forcedVarYName.empty() )
  {
    bOk = netCDFVarDef->HasCommonDims( m_forcedVarYName );
    if (!bOk)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - '%s' dimension doesn't exist for the field '%s'",
                                  m_forcedVarYName.c_str(), fieldName.c_str());
      throw CException(msg, BRATHL_INCONSISTENCY_ERROR);
    }
  }

  if ( !m_forcedVarXName.empty() && !m_forcedVarYName.empty() )
  {
	if ( str_icmp( m_forcedVarXName, m_forcedVarYName ) )
    {
      m_forcedVarYName = "";
    }
  }

  CStringArray complement;

  if ( m_forcedVarXName.empty() && !m_forcedVarYName.empty() )
  {
    complement.RemoveAll();
    netCDFVarDef->GetComplementDims(m_forcedVarYName, complement);

    if (complement.size() != 1)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - ERROR - Field '%s' : X dimension not found (Y "
                                  "dimension is '%s'), field dimensions are '%s'", fieldName.c_str(),
                                  m_forcedVarYName.c_str(), fieldDimNames.ToString().c_str());
      throw CException(msg, BRATHL_INCONSISTENCY_ERROR);
    }

    m_forcedVarXName = complement.at(0);
  }

  if ( !m_forcedVarXName.empty() && m_forcedVarYName.empty() )
  {
    complement.RemoveAll();
    netCDFVarDef->GetComplementDims( m_forcedVarXName, complement );

    if (complement.size() != 1)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - ERROR - Field '%s' : Y dimension not found (X "
                                  "dimension is '%s'), field dimensions are '%s'", fieldName.c_str(),
                                  m_forcedVarXName.c_str(), fieldDimNames.ToString().c_str());
      throw CException(msg, BRATHL_INCONSISTENCY_ERROR);
    }

    m_forcedVarYName = complement.at(0);
  }


  CStringArray dimNames;
  for (CObArray::iterator itOb = netCDFDims->begin(); itOb != netCDFDims->end(); itOb++)
  {
    CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(*itOb);
    dimNames.Insert(netCDFDim->GetName());
  }

  if ( !m_forcedVarXName.empty() && !m_forcedVarYName.empty() )
  {
    dimNames.RemoveAll();
    dimNames.Insert(m_forcedVarXName);
    dimNames.Insert(m_forcedVarYName);
  }


  for (uint32_t indexDim = 0; indexDim < dimNames.size(); indexDim++)
  {
    //CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(*it);
    CNetCDFCoordinateAxis* coordinateAxis = dynamic_cast<CNetCDFCoordinateAxis*>(zfxy->GetNetCDFVarDef(dimNames.at(indexDim)));
    CNetCDFDimension* netCDFDim = netCDFVarDef->GetNetCDFDim(dimNames.at(indexDim));

    if (coordinateAxis == nullptr)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - '%s' axis is not found or not a coordinate axis.",
                                   dimNames.at(indexDim).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    uint32_t nDims = (uint32_t)coordinateAxis->GetDims()->size();

    if (nDims != 1)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - '%s' axis -> number of dimensions not equal to 1 : %ld",
                                   coordinateAxis->GetName().c_str(), (long)nDims);
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    uint32_t dimRange =  netCDFVarDef->GetNetCDFDimRange(coordinateAxis->GetName());
    if (indexDim == 0)
    {
      dimRangeX = dimRange;
      width = netCDFDim->GetLength();
      varXName = netCDFDim->GetName(),
      zfxy->ReadVar(netCDFDim->GetName(), varX, coordinateAxis->GetUnit()->GetText());
    }
    else
    {
      dimRangeY = dimRange;
      height = netCDFDim->GetLength();
      varYName = netCDFDim->GetName(),
      zfxy->ReadVar(netCDFDim->GetName(), varY, coordinateAxis->GetUnit()->GetText());
    }
  }

  if (width == -1)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - X axis not found in input file '%s'",
                                   zfxy->GetName().c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

  if (height == -1)
    {
      std::string msg = CTools::Format("CZFXYPlot::GetPlotWidthHeight - Y axis not found in input file '%s'",
                                   zfxy->GetName().c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }
}



#if defined(BRAT_V3)

#include "../support/code/legacy/display/PlotData/XYPlotData_vtk.h"


using xy_plot_data_t = VTK_CXYPlotData;

xy_plot_data_t* CYFXPlot::Get(std::vector< CXYPlotField* >::iterator it )
{
  xy_plot_data_t* data = dynamic_cast<xy_plot_data_t*>(*it);
  if (data == nullptr)
  {
    throw CException("ERROR in  CYFXPlot::Get : dynamic_cast<CXYPlotField*>(*it); returns nullptr pointer - "
                 "XYPlotData Collection seems to contain objects other than those of the class CXYPlotField or derived class", BRATHL_LOGIC_ERROR);
  }
  return data;
}

//----------------------------------------
xy_plot_data_t* CYFXPlot::Get(int32_t index)
{
  if ( (index < 0) || (static_cast<uint32_t>(index) >= mPlotFields.size()) )
  {
    std::string msg = CTools::Format("ERROR in  CYFXPlot::Get : index %d out-of-range "
                                "Valid range is [0, %ld]",
                                index,
                                (long)mPlotFields.size());
    throw CException(msg, BRATHL_LOGIC_ERROR);

  }

  xy_plot_data_t* data = dynamic_cast<xy_plot_data_t*>(mPlotFields.at(index));
  if (data == nullptr)
  {
    throw CException("ERROR in  CYFXPlot::Get : dynamic_cast<CXYPlotField*>(this->at(index)); returns nullptr pointer - "
                 "XYPlotData Collection seems to contain objects other than those of the class CXYPlotField or derived class", BRATHL_LOGIC_ERROR);
  }
  return data;
}
//----------------------------------------
CXYPlotProperties* CYFXPlot::GetPlotProperties(int32_t index)
{
  CXYPlotField* data = Get(index);

  return data->GetPlotProperties();
}


void CYFXPlot::OnFrameChange(int32_t f)
{
  for (auto it = mPlotFields.begin(); it != mPlotFields.end() ; it++)
  {
    Get(it)->OnFrameChange(f);
  }

  m_currentFrame = f;
}


bool CYFXPlot::ShowPropertyMenu()
{
  for (auto it = mPlotFields.begin(); it != mPlotFields.end() ; it++)
  {
    CXYPlotField* data = Get(it);

    CXYPlotProperties* props = data->GetPlotProperties();

    if (props->GetShowPropertyPanel())
    {
      return true;
    }
  }

  return false;

}

//----------------------------------------
void CYFXPlot::GetNames( std::vector<std::string>& names )
{
	for ( auto *data : mPlotFields )
	{
        names.push_back( data->GetPlotProperties()->UserName() );
	}
}


/*
 * This function is calculating the general minimum along each frame.
 * however if CXYPlotProperties props already contains values for
 * m_xMin and m_xMax, it uses those instead.
 */
void CYFXPlot::GetXRange( double& min, double& max )
{
	setDefaultValue( min );
	setDefaultValue( max );
	for ( auto *data : mPlotFields )
	{
		double l = 0;
		double h = 0;

		const CXYPlotProperties* props = data->GetPlotProperties();

		//computes the X range (lower and upper)
		//notice that the function receives a pointer to
		// l and h
		data->GetXRange( l, h );

		//XYPlotProperties contains two CPlotArrays
		//m_xMin does not have an initial value yet
		if ( !isDefaultValue( props->Xmin() ) )
		{
			//only set l if xMin already contains an initial value
			//that is non default
			l = props->Xmin();
		}
		//same for m_xMax
		if ( !isDefaultValue( props->Xmax() ) )
		{
			h = props->Xmax();
		}

		//min has not been initialized
		if ( isDefaultValue( min ) )
		{
			min = l;
			max = h;
		}
		else
		{
			min = ( min > l ) ? l : min;
			max = ( max > h ) ? max : h;
		}
	}
}

void CYFXPlot::GetYRange( double& min, double& max )
{
	setDefaultValue( min );
	setDefaultValue( max );

	for ( auto *data : mPlotFields )
	{
		double l = 0;
		double h = 0;

		const CXYPlotProperties* props = data->GetPlotProperties();

		data->GetYRange( l, h );

		if ( !isDefaultValue( props->Ymin() ) )
		{
			l = props->Ymin();
		}
		if ( !isDefaultValue( props->Ymax() ) )
		{
			h = props->Ymax();
		}

		if ( isDefaultValue( min ) )
		{
			min = l;
			max = h;
		}
		else
		{
			min = ( min > l ) ? l : min;
			max = ( max > h ) ? max : h;
		}
	}
}


void CYFXPlot::GetXRange( double& min, double& max, uint32_t frame )
{
	setDefaultValue( min );
	setDefaultValue( max );
	for ( auto data : mPlotFields )
	{
		assert__( data != nullptr );

		double l = 0;
		double h = 0;

		const CXYPlotProperties* props = data->GetPlotProperties();

		data->GetXRange( l, h, frame );

		if ( !isDefaultValue( props->Xmin() ) )
		{
			l = props->Xmin();
		}
		if ( !isDefaultValue( props->Xmax() ) )
		{
			h = props->Xmax();
		}

		if ( isDefaultValue( min ) )
		{
			min = l;
			max = h;
		}
		else
		{
			min = ( min > l ) ? l : min;
			max = ( max > h ) ? max : h;
		}
	}
}



void CYFXPlot::GetYRange( double& min, double& max, uint32_t frame )
{
	setDefaultValue( min );
	setDefaultValue( max );

	for ( auto data : mPlotFields )
	{
		assert__( data != nullptr );

		double l = 0;
		double h = 0;

		const CXYPlotProperties* props = data->GetPlotProperties();

		data->GetYRange( l, h, frame );

		if ( !isDefaultValue( props->Ymin() ) )
		{
			l = props->Ymin();
		}
		if ( !isDefaultValue( props->Ymax() ) )
		{
			h = props->Ymax();
		}

		if ( isDefaultValue( min ) )
		{
			min = l;
			max = h;
		}
		else
		{
			min = ( min > l ) ? l : min;
			max = ( max > h ) ? max : h;
		}
	}
}


void CYFXPlot::AddData( CXYPlotField *pdata )
{
	mPlotFields.push_back( pdata );

	auto *properties = pdata->GetPlotProperties();				assert__( properties );

	if ( isDefaultValue( properties->Xmin() ) || isDefaultValue( properties->Xmax() ) )
	{
		double xrMin, xrMax;

		GetXRange( xrMin, xrMax );
		properties->SetXmin( xrMin );
		properties->SetXmax( xrMax );
	}

	if ( isDefaultValue( properties->Ymin() ) || isDefaultValue( properties->Ymax() ) )
	{
		double yrMin, yrMax;

		GetYRange( yrMin, yrMax );
		properties->SetYmin( yrMin );
		properties->SetYmax( yrMax );
	}
}

#endif
