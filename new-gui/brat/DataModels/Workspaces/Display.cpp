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

#include "libbrathl/brathl.h"

#include "new-gui/Common/tools/Exception.h"
#include "new-gui/Common/tools/Trace.h"

#include "libbrathl/Tools.h"
#include "libbrathl/InternalFilesFactory.h"
#include "libbrathl/InternalFiles.h"
#include "process/BratProcess.h"

#include "Workspace.h"
#include "Operation.h"
#include "WorkspaceSettings.h"

using namespace processes;
using namespace brathl;

#include "DataModels/PlotData/MapProjection.h"

#include "Display.h"




const char* CDisplayData::FMT_FLOAT_XY = "%-#.5g";
//const unsigned int CDisplayData::NB_DIMS = 3;




class CDisplayCmdFile : CCmdFile
{
	const CDisplay &mDisp;

public:
	CDisplayCmdFile( const std::string &path, const CDisplay &Disp ) :
		CCmdFile( path ), mDisp( Disp )
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
		Comment( "Type:" + CMapTypeDisp::GetInstance().IdToName( mDisp.GetPlotType( map_as_3dplot ) ) );
		return true;
	}
	bool BuildCmdFileVerbose()
	{
		WriteLn();
		Comment( "----- LOG -----" );
		WriteLn();
		WriteLn( kwVERBOSE + "=" + n2s<std::string>( mDisp.m_verbose ) );

		return true;
	}

	bool BuildCmdFileGeneralProperties( bool map_as_3dplot )
	{
		WriteLn();
		Comment( "----- GENERAL PROPERTIES -----" );
		WriteLn();

		if ( !mDisp.GetTitle().empty() )
			WriteLn( FmtCmdParam( kwDISPLAY_TITLE ) + mDisp.GetTitle() );

		WriteLn();
		Comment( "Display Type:" + CMapTypeDisp::GetInstance().Enum() );
		WriteLn();

		WriteLn( FmtCmdParam( kwDISPLAY_PLOT_TYPE ) + mDisp.GetTypeAsString( map_as_3dplot ) );

		WriteLn();

		switch ( mDisp.GetPlotType( map_as_3dplot ) )
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

	bool BuildCmdFileGeneralPropertiesXY()
	{
		std::string
		valueString = ( isDefaultValue( mDisp.GetMinXValue() ) ) ? "DV" : mDisp.GetMinXValueAsText();
		WriteLn( FmtCmdParam( kwDISPLAY_XMINVALUE ) + valueString );

		valueString = ( isDefaultValue( mDisp.GetMaxXValue() ) ) ? "DV" : mDisp.GetMaxXValueAsText();
		WriteLn( FmtCmdParam( kwDISPLAY_XMAXVALUE ) + valueString );

		valueString = ( isDefaultValue( mDisp.GetMinYValue() ) ) ? "DV" : mDisp.GetMinYValueAsText();
		WriteLn( FmtCmdParam( kwDISPLAY_YMINVALUE ) + valueString );

		valueString = ( isDefaultValue( mDisp.GetMaxYValue() ) ) ? "DV" : mDisp.GetMaxYValueAsText();
		WriteLn( FmtCmdParam( kwDISPLAY_YMAXVALUE ) + valueString );

		WriteLn();

		std::string axisName;
		std::string axisLabel;

		Comment( "----- X AXIS -----" );

		auto &data = mDisp.GetData();
		for ( CObMap::const_iterator it = data.begin(); it != data.end(); it++ )
		{
			CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
			if ( value == nullptr )
				continue;

			if ( value->GetXAxis().empty() )
			{
				axisName = value->GetX()->GetName().c_str();
				axisLabel = value->GetX()->GetDescription().c_str();
			}
			else
			{
				axisName = value->GetXAxis();
				axisLabel = value->GetXAxisText( (const char *)value->GetXAxis().c_str() );
			}

			WriteLn();
			WriteLn( FmtCmdParam( kwDISPLAY_XAXIS ) + axisName );
			WriteLn( FmtCmdParam( kwDISPLAY_XLABEL ) + axisLabel );

			if ( mDisp.AreFieldsGrouped() )
				break;
		}

		return true;
	}

	bool BuildCmdFileGeneralPropertiesZXY()
	{
		WriteLn( FmtCmdParam( kwDISPLAY_ANIMATION ) + mDisp.GetWithAnimationAsText() );

		std::string 
		valueString = ( isDefaultValue( mDisp.GetMinXValue() ) ) ? "DV" : mDisp.GetMinXValueAsText();
		WriteLn( FmtCmdParam( kwDISPLAY_XMINVALUE ) + valueString );

		valueString = ( isDefaultValue( mDisp.GetMaxXValue() ) ) ? "DV" : mDisp.GetMaxXValueAsText();
		WriteLn( FmtCmdParam( kwDISPLAY_XMAXVALUE ) + valueString );

		valueString = ( isDefaultValue( mDisp.GetMinYValue() ) ) ? "DV" : mDisp.GetMinYValueAsText();
		WriteLn( FmtCmdParam( kwDISPLAY_YMINVALUE ) + valueString );

		valueString = ( isDefaultValue( mDisp.GetMaxYValue() ) ) ? "DV" : mDisp.GetMaxYValueAsText();
		WriteLn( FmtCmdParam( kwDISPLAY_YMAXVALUE ) + valueString );

		WriteLn();

		Comment( "----- X/Y AXES -----" );

		std::string xAxisName;
		std::string yAxisName;

		auto &data = mDisp.GetData();
		for ( CObMap::const_iterator it = data.begin(); it != data.end(); it++ )
		{
			CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
			if ( value == nullptr )
				continue;

			if ( value->IsInvertXYAxes() )
			{
				xAxisName = value->GetY()->GetName().c_str();
				yAxisName = value->GetX()->GetName().c_str();
			}
			else
			{
				xAxisName = value->GetX()->GetName().c_str();
				yAxisName = value->GetY()->GetName().c_str();
			}

			WriteLn();
			WriteLn( FmtCmdParam( kwDISPLAY_XAXIS ) + xAxisName );
			WriteLn( FmtCmdParam( kwDISPLAY_YAXIS ) + yAxisName );

			if ( mDisp.AreFieldsGrouped() )
				break;
		}

		return true;
	}

	bool BuildCmdFileGeneralPropertiesZLatLon()
	{
		WriteLn( FmtCmdParam( kwDISPLAY_ANIMATION ) + mDisp.GetWithAnimationAsText() );

		WriteLn( FmtCmdParam( kwDISPLAY_PROJECTION ) + mDisp.m_projection );

		CStringArray array;
		mDisp.m_zoom.GetAsString( array );
		if ( array.size() == 4 )
		{
			WriteLn( FmtCmdParam( kwDISPLAY_ZOOM_LAT1 ) + array.at( 0 ).c_str() );
			WriteLn( FmtCmdParam( kwDISPLAY_ZOOM_LON1 ) + array.at( 1 ).c_str() );
			WriteLn( FmtCmdParam( kwDISPLAY_ZOOM_LAT2 ) + array.at( 2 ).c_str() );
			WriteLn( FmtCmdParam( kwDISPLAY_ZOOM_LON2 ) + array.at( 3 ).c_str() );
		}

		return true;
	}


	bool BuildCmdFileFields( std::string errorMsg )
	{
		return mDisp.m_type == CMapTypeDisp::eTypeDispYFX ? BuildCmdFileFieldsYFX( errorMsg ) : BuildCmdFileFieldsZFXY( errorMsg );
	}

	bool BuildCmdFileFieldsYFX( std::string errorMsg )
	{
		for ( CMapDisplayData::const_iterator it = mDisp.m_data.begin(); it != mDisp.m_data.end(); it++ )
		{
			CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );

			if ( value == nullptr )
				continue;

			WriteLn();
			Comment( "----- " + value->GetField()->GetName() + " FIELD -----" );
			WriteLn();
			WriteLn( kwFIELD + "=" + value->GetField()->GetName() );
			WriteLn( kwFILE + "=" + value->GetOperation()->GetOutputPath() );
			BuildCmdFileFieldProperties( ( it->first ).c_str(), errorMsg );
		}

		return true;
	}

	bool BuildCmdFileFieldsZFXY( std::string errorMsg )
	{
		for ( CMapDisplayData::const_iterator it = mDisp.m_data.begin(); it != mDisp.m_data.end(); it++ )
		{
			CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
			if ( value == nullptr )
				continue;

			WriteLn();
			Comment( "----- " + value->GetField()->GetName() + " FIELD -----" );
			WriteLn();
			WriteLn( kwFIELD + "=" + value->GetField()->GetName() );
			WriteLn( kwFILE + "=" + value->GetOperation()->GetOutputPath() );
			BuildCmdFileFieldProperties( ( it->first ).c_str(), errorMsg );
		}

		return true;
	}

	bool BuildCmdFileFieldProperties( const std::string& dataKey, std::string errorMsg )
	{
		const CDisplayData *data = mDisp.m_data.GetDisplayData( dataKey );
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
		if ( displayName.empty() == false )
		{
			WriteLn( FmtCmdParam( kwDISPLAY_NAME ) + displayName );
		}
		else
		{
			WriteLn( FmtCmdParam( kwDISPLAY_NAME ) + data->GetField()->GetName() );
		}


		WriteLn( FmtCmdParam( kwFIELD_GROUP ) + data->GetGroupAsText() );

		switch ( mDisp.m_type )
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
		valueString = ( isDefaultValue( value->GetMinValue() ) ) ? "DV" : value->GetMinValueAsText();
		WriteLn( FmtCmdParam( kwDISPLAY_MINVALUE ) + valueString );

		valueString = ( isDefaultValue( value->GetMaxValue() ) ) ? "DV" : value->GetMaxValueAsText();
		WriteLn( FmtCmdParam( kwDISPLAY_MAXVALUE ) + valueString );

		WriteLn( FmtCmdParam( kwDISPLAY_CONTOUR ) + value->GetContourAsText() );
		WriteLn( FmtCmdParam( kwDISPLAY_SOLID_COLOR ) + value->GetSolidColorAsText() );

		valueString = ( value->GetColorPalette().empty() ) ? "DV" : value->GetColorPalette();
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



//-------------------------------------------------------------
//------------------- CDisplayData class --------------------
//-------------------------------------------------------------

//----------------------------------------
CDisplayData::CDisplayData( const CDisplayData &o, CWorkspaceOperation *wkso )
{
	Init();
	m_field.SetName( o.m_field.GetName() );
	m_field.SetUnit( o.m_field.GetUnit() );
	m_field.SetDescription( o.m_field.GetDescription() );

	m_type = o.m_type;

	m_x.SetName( o.m_x.GetName() );
	m_x.SetUnit( o.m_x.GetUnit() );
	m_x.SetDescription( o.m_x.GetDescription() );

	m_y.SetName( o.m_y.GetName() );
	m_y.SetUnit( o.m_y.GetUnit() );
	m_y.SetDescription( o.m_y.GetDescription() );

	m_z.SetName( o.m_z.GetName() );
	m_z.SetUnit( o.m_z.GetUnit() );
	m_z.SetDescription( o.m_z.GetDescription() );

	m_operation = nullptr;
	if ( o.m_operation != nullptr )
		m_operation = wkso->GetOperation( o.m_operation->GetName() );

	m_group = o.m_group;
	m_withContour = o.m_withContour;
	m_withSolidColor = o.m_withSolidColor;
	m_minValue = o.m_minValue;
	m_maxValue = o.m_maxValue;
	m_colorPalette = o.m_colorPalette;
	m_xAxis = o.m_xAxis;

	m_invertXYAxes = o.m_invertXYAxes;

	m_eastcomponent = o.m_eastcomponent;
	m_northcomponent = o.m_northcomponent;
}

std::string CDisplayData::GetValueAsText( double value )
{
	auto SprintfFormat = []( const std::string &format, double value )
	{
		char buffer[ 128 ];

		sprintf( buffer, format.c_str(), value );

		return std::string( buffer );
	};

	///

	return SprintfFormat( FMT_FLOAT_XY, value );
}


//----------------------------------------
void CDisplayData::CopyFieldUserProperties(CDisplayData& d)
{
  m_field.SetDescription(d.m_field.GetDescription());

  m_x.SetDescription(d.m_x.GetDescription());
  m_y.SetDescription(d.m_y.GetDescription());
  m_z.SetDescription(d.m_z.GetDescription());

  m_withContour = d.m_withContour;
  m_withSolidColor = d.m_withSolidColor;
  m_minValue = d.m_minValue;
  m_maxValue = d.m_maxValue;
  m_colorPalette = d.m_colorPalette;
  m_xAxis = d.m_xAxis;

  m_invertXYAxes = d.m_invertXYAxes;

  m_eastcomponent = d.m_eastcomponent;
  m_northcomponent = d.m_northcomponent;

}
//----------------------------------------
std::string CDisplayData::GetDataKey( int32_t type )
{
	if ( m_operation == nullptr )
		return "";

	return m_operation->GetName() + "_" + m_field.GetName() + "_" + n2s<std::string>( type );
}
std::string CDisplayData::GetDataKey()
{
	return GetDataKey( GetType() );
}

//----------------------------------------
bool CDisplayData::IsYFXType()
{
  return m_type == CMapTypeDisp::eTypeDispYFX;
}
//----------------------------------------
bool CDisplayData::IsZYFXType()
{
  return m_type == CMapTypeDisp::eTypeDispZFXY;
}
//----------------------------------------
bool CDisplayData::IsZLatLonType()
{
	return m_type == CMapTypeDisp::eTypeDispZFLatLon;
}
//----------------------------------------
void CDisplayData::GetAvailableDisplayTypes( CUIntArray& displayTypes )
{
	displayTypes.RemoveAll();

	if ( m_operation == nullptr )
	{
		return;
	}

	CDisplay::GetDisplayType( m_operation, displayTypes );
}
//----------------------------------------
std::string CDisplayData::GetXAxisText(const std::string& name)
{

  std::string str = name;

  for (unsigned int index = 0 ; index < m_dimFields.size() ; index++)
  {
    CField* dim = dynamic_cast<CField*>(m_dimFields.at(index));

    if (dim == nullptr)
    {
      return "";
    }

    if (str_icmp(dim->GetName(), name))
    {
      str = dim->GetDescription().c_str();
    }

  }

  return str.c_str();

}
//----------------------------------------
std::string CDisplayData::GetXAxisText(unsigned int index)
{
  if (index >= m_dimFields.size())
  {
    return "";
  }

  CField* dim = dynamic_cast<CField*>(m_dimFields.at(index));

  if (dim == nullptr)
  {
    return "";
  }

  return dim->GetDescription().c_str();

}
//----------------------------------------
void CDisplayData::SetXAxisText(unsigned int index, const std::string& value)
{
  if (index >= m_dimFields.size())
  {
    return;
  }

  CField* dim = dynamic_cast<CField*>(m_dimFields.at(index));

  if (dim == nullptr)
  {
    return;
  }

  dim->SetDescription((const char *)value.c_str());
}
//----------------------------------------
bool CDisplayData::HasXComplement()
{
  CStringArray complement;
  GetXComplement(complement);

  return (complement.size() >= 1);

}
//----------------------------------------
void CDisplayData::GetXComplement(CStringArray& complement)
{
  CStringArray xName;
  CStringArray names;

  xName.Insert(GetX()->GetName());

  GetAvailableAxes(names);

  xName.Complement(names, complement);

}

//----------------------------------------
void CDisplayData::GetAvailableAxes( CStringArray& names )
{
	size_t nbDims = m_dimFields.size();

	for ( unsigned int i = 0; i < nbDims; i++ )
	{
		CField* dim = dynamic_cast<CField*>( m_dimFields.at( i ) );

		if ( dim == nullptr )
		{
			continue;
		}

		std::string name = dim->GetName().c_str();

		if ( name.empty() )
		{
			continue;
		}

		names.Insert( (const char *)name.c_str() );

	}
}
//----------------------------------------
bool CDisplayData::LoadConfig( CWorkspaceSettings *config, const std::string& path, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso )
{
	assert__( config );	//v4
	//return !config || config->LoadConfig( *this, path, wks, wkso );

	return config->LoadConfig( *this, path, wks, wkso );
}
//----------------------------------------
bool CDisplayData::SaveConfig( CWorkspaceSettings *config, const std::string& pathSuff, CWorkspaceDisplay *wksd )
{
	assert__( config );	//v4
	//return !config || config->SaveConfig( *this, pathSuff, wksd );

	return config->SaveConfig( *this, pathSuff, wksd );
}

//-------------------------------------------------------------
//------------------- CMapDisplayData class --------------------
//-------------------------------------------------------------

CMapDisplayData::CMapDisplayData( const CMapDisplayData &o, CWorkspaceOperation *wkso )
{
	if ( this != &o )					 
	{
		RemoveAll();
		for ( CMapDisplayData::const_iterator it = o.begin(); it != o.end(); it++ )
		{
			CDisplayData* value = dynamic_cast< CDisplayData* >( it->second );
			if ( value == nullptr )
				continue;

			Insert( value->GetDataKey(), new CDisplayData( *value, wkso ) );
		}
	}
}
//----------------------------------------
void CMapDisplayData::SetGroups(bool groupFields)
{
  if (groupFields)
  {
    GroupFields();
  }
  else
  {
    SplitFields();
  }

}
//----------------------------------------
bool CMapDisplayData::AreFieldsGrouped() const
{
	bool isGroup = true;

	int groupNumber = 1;

	for ( CMapDisplayData::const_iterator it = begin(); it != end(); it++ )
	{
		CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
		if ( value != nullptr )
		{
			int otherNumber = value->GetGroup();

			if ( it == begin() )
				groupNumber = otherNumber;

			if ( groupNumber != otherNumber )
			{
				isGroup = false;
				break;
			}
		}
	}

	return isGroup;
}
//----------------------------------------
void CMapDisplayData::GroupFields()
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value != nullptr)
    {
      value->SetGroup(1);
    }
  }
}
//----------------------------------------
void CMapDisplayData::SplitFields()
{
  CMapDisplayData::iterator it;

  int32_t groupNumber = 0;
  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value != nullptr)
    {
      groupNumber++;
      value->SetGroup(groupNumber);
    }
  }
}

//----------------------------------------
bool CMapDisplayData::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapDisplayData::ValidName(const char* name)
{
  CDisplayData* value = dynamic_cast<CDisplayData*>(Exists(name));
  return (value != nullptr);
}
//----------------------------------------
void CMapDisplayData::SetAllAxis(unsigned int index, const std::string& axisName, const std::string& axisLabel)
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* displayData = dynamic_cast<CDisplayData*>(it->second);

    if (displayData == nullptr)
    {
      continue;
    }

    displayData->SetXAxis(axisName);
    displayData->SetXAxisText(index, axisLabel);
  }

}

//----------------------------------------
void CMapDisplayData::SetAllInvertXYAxes(bool value)
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* displayData = dynamic_cast<CDisplayData*>(it->second);

    if (displayData == nullptr)
    {
      continue;
    }

    displayData->SetInvertXYAxes(value);
  }

}

//----------------------------------------
CDisplayData* CMapDisplayData::GetDisplayData(CMapDisplayData::const_iterator& it)
{
  return dynamic_cast<CDisplayData*>(it->second);
}
//----------------------------------------
const CDisplayData* CMapDisplayData::GetDisplayData( const std::string& name ) const
{
  return dynamic_cast<CDisplayData*>(Exists(name.c_str()));
}
//----------------------------------------
const CDisplayData* CMapDisplayData::GetDisplayData(const char* name) const
{
  return dynamic_cast<CDisplayData*>(Exists(name));
}
//----------------------------------------
bool CMapDisplayData::LoadConfig( CWorkspaceSettings *config, std::string &errorMsg, CWorkspaceDisplay *wks, CWorkspaceOperation *wkso, const std::string& pathSuff )
{
	return config && config->LoadConfig( *this, errorMsg, wks, wkso, pathSuff );
}

//----------------------------------------
bool CMapDisplayData::SaveConfig( CWorkspaceSettings *config, CWorkspaceDisplay *wks, const std::string& pathSuff ) const
{
	return config && config->SaveConfig( *this, wks, pathSuff );
}

//----------------------------------------
void CMapDisplayData::GetDistinctFiles(CStringMap& array)
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value != nullptr)
    {
      array.Insert(value->GetOperation()->GetName(), value->GetOperation()->GetOutputPath(), false);
    }
  }

}

//----------------------------------------
void CMapDisplayData::GetDistinctFields(CStringMap& array)
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value != nullptr)
    {
      array.Insert(value->GetField()->GetName(), it->first, false);
    }
  }

}

//----------------------------------------
bool CMapDisplayData::CheckFields( std::string& errorMsg, CDisplay* display )
{
	if ( this->size() <= 1 )
		return true;

	if ( display == nullptr )
		return false;

	CObMap internalFilesMap;
	CInternalFiles *file = nullptr;

	for ( CMapDisplayData::iterator it = begin(); it != end(); it++ )
	{
		CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
		if ( value == nullptr )
			continue;

		const COperation* operation = value->GetOperation();
		if ( operation == nullptr )
			continue;

		bool bOk = IsFile( operation->GetOutputPath() );
		if ( bOk == false )
			continue;

		file = dynamic_cast<CInternalFiles*>( internalFilesMap.Exists( operation->GetOutputPath() ) );
		if ( file != nullptr )
			continue;

		try
		{
			file = BuildExistingInternalFileKind( operation->GetOutputPath() );
			file->Open( ReadOnly );
		}
		catch ( CException& e )
		{
			e.what();
			if ( file != nullptr )
				delete file;

			continue;
		}

		internalFilesMap.Insert( operation->GetOutputPath(), file, false );
	}


	CNetCDFVarDef* firstNetCDFVardef = nullptr;
	CInternalFiles *firstFile = nullptr;

	bool bOk = true;

	for ( CMapDisplayData::iterator it = begin(); it != end(); it++ )
	{
		CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
		if ( value == nullptr )
			continue;

		const COperation* operation = value->GetOperation();
		if ( operation == nullptr )
			continue;

		CInternalFiles *file = dynamic_cast<CInternalFiles*>( internalFilesMap.Exists( operation->GetOutputPath() ) );
		if ( file == nullptr )
			continue;

		const CField *field = value->GetField();
		if ( field == nullptr )
			continue;

		CNetCDFVarDef* netCDFVardef = file->GetNetCDFVarDef( field->GetName() );
		if ( netCDFVardef == nullptr )
			continue;

		if ( firstNetCDFVardef == nullptr )
		{
			firstNetCDFVardef = netCDFVardef;
			firstFile = file;
			continue;
		}

		std::string msg;

		//if ( ! firstNetCDFVardef->HaveEqualDims(netCDFVardef, firstFile->GetFile(), file->GetFile()) )
		if ( ! firstNetCDFVardef->HaveEqualDimNames( netCDFVardef, &msg ) )
		{
			errorMsg += msg;
			bOk = false;
			continue;
		}

		if ( ! firstNetCDFVardef->HaveCompatibleDimUnits( netCDFVardef, firstFile->GetFile(), file->GetFile(), &msg ) )
		{
			errorMsg += msg;
			bOk = false;
			continue;
		}

		if ( display->IsYFXType() )
		{
			if ( !firstNetCDFVardef->HasCompatibleUnit( netCDFVardef ) )
			{
				errorMsg += 
					( "\tExpressions '"
					+ firstNetCDFVardef->GetName()
					+ "' and '"
					+ netCDFVardef->GetName()
					+ "' have incompatible units (unit "
					+ firstNetCDFVardef->GetUnit()->GetText()
					+ " vs "
					+ netCDFVardef->GetUnit()->GetText()
					+ ")\n"
					);

				errorMsg += msg;
				bOk = false;
				continue;
			}
		}
	}

	internalFilesMap.RemoveAll();

	return bOk;
}

//----------------------------------------


//-------------------------------------------------------------
//------------------- CDisplay class --------------------
//-------------------------------------------------------------

const std::string CDisplay::m_zoomDelimiter = " ";


//static
void CDisplay::GetDisplayType( const COperation* operation, CUIntArray& displayTypes, CInternalFiles** pf )		//pf = nullptr
{
	displayTypes.RemoveAll();

	if ( operation == NULL )
		return;

	CInternalFiles* f = CInternalFiles::Create( operation->GetOutputPath(), true, false );

	if ( CInternalFiles::IsYFXFile( f ) )
	{
		displayTypes.Insert( CMapTypeDisp::eTypeDispYFX );
	}

	if ( CInternalFiles::IsZFLatLonFile( f ) )
	{
		displayTypes.Insert( CMapTypeDisp::eTypeDispZFLatLon );
	}

	if ( CInternalFiles::IsZFXYFile( f ) )
	{
		displayTypes.Insert( CMapTypeDisp::eTypeDispZFXY );
	}

	if ( pf == NULL )
	{
		delete f;
		f = NULL;
	}
	else
	{
		*pf = f;
	}
}


//----------------------------------------

CDisplay::CDisplay(std::string name)
{
  Init();
  m_name = name;
}
//----------------------------------------

CDisplay::~CDisplay()
{
	if ( IsFile( m_cmdFile ) )
		RemoveFile( m_cmdFile );
}

//----------------------------------------
void CDisplay::Init()
{
  //m_type = CMapTypeDisp::typeOpYFX;
  m_type = CMapTypeDisp::Invalid();
  m_withAnimation = false;
//  m_projection = CMapProjection::GetInstance()->IdToName(VTK_PROJ2D_3D);
  m_projection = PROJECTION_3D_VALUE;

  setDefaultValue(m_minXValue);
  setDefaultValue(m_maxXValue);

  setDefaultValue(m_minYValue);
  setDefaultValue(m_maxYValue);

}

// After executing an operation, the following call were made (in views tab)
//
//	CDisplayPanel::GetOperations(); - inserted all operations data in member m_dataList
//	which then was loaded in available data tree widget:
//
//	CDisplayPanel::GetDispavailtreectrl()->InsertData( &m_dataList );
//
//  When a field was dragged from this tree to the selected data list
//	 a succession of calls CDisplayPanel::AddField resulted in
//
//		m_display->InsertData( GetDataKey( *data ).ToStdString(), newdata );
//
//	This function is based on all that.
//
//	The parameter update is an attempt to maintain display consistency when the 
//	associated operation is executed again: if update is true, existing fields 
//	not in operation are removed
//
bool CDisplay::AssignOperation( const COperation *operation, bool update )	//bool update = false 
{
	CInternalFiles *file = nullptr;
	CUIntArray displayTypes;
	CDisplay::GetDisplayType( operation, displayTypes, &file );
	if ( file == nullptr )
	{
		return false;
	}

	CMapDisplayData *dataList = new CMapDisplayData;		//v3: dataList, data member

	CStringArray names;
	names.RemoveAll();
	file->GetDataVars( names );

	for ( CUIntArray::iterator itDispType = displayTypes.begin(); itDispType != displayTypes.end(); itDispType++ )
	{
		for ( CStringArray::iterator itField = names.begin(); itField != names.end(); itField++ )
		{
			CStringArray varDimensions;
			file->GetVarDims( *itField, varDimensions );

			size_t nbDims = varDimensions.size();
			if ( nbDims > 2 )
			{
				continue;
			}
			if ( ( nbDims != 2 ) && ( *itDispType == CMapTypeDisp::eTypeDispZFXY || *itDispType == CMapTypeDisp::eTypeDispZFLatLon ) )
			{
				continue;
			}

			CDisplayData* displayData = new CDisplayData( operation );

			displayData->SetType( *itDispType );

			displayData->GetField()->SetName( *itField );

			std::string unit = file->GetUnit( *itField ).GetText();
			displayData->GetField()->SetUnit( unit );

			std::string comment = file->GetComment( *itField );
			std::string description = file->GetTitle( *itField );

			if ( !comment.empty() )
			{
				description += "." + comment;
			}

			displayData->GetField()->SetDescription( (const char *)description.c_str() );

			if ( nbDims >= 1 )
			{
				std::string dimName = varDimensions.at( 0 );
				displayData->GetX()->SetName( varDimensions.at( 0 ) );

				std::string unit = file->GetUnit( dimName ).GetText();
				displayData->GetX()->SetUnit( unit );

				displayData->GetX()->SetDescription( file->GetTitle( dimName ) );
			}

			if ( nbDims >= 2 )
			{
				std::string dimName = varDimensions.at( 1 );
				displayData->GetY()->SetName( varDimensions.at( 1 ) );

				std::string unit = file->GetUnit( dimName ).GetText();
				displayData->GetY()->SetUnit( unit );

				displayData->GetY()->SetDescription( file->GetTitle( dimName ) );
			}

			if ( nbDims >= 3 )
			{
				std::string dimName = varDimensions.at( 2 );
				displayData->GetZ()->SetName( varDimensions.at( 2 ) );

				std::string unit = file->GetUnit( dimName ).GetText();
				displayData->GetZ()->SetUnit( unit );

				displayData->GetZ()->SetDescription( file->GetTitle( dimName ) );
			}

			dataList->Insert( displayData->GetDataKey(), displayData, false );
		}
	}

	if ( update )
	{
		m_data.clear();
		//while ( m_data.size() > 0 )
		//{
  //          auto const &data = m_data.begin();
		//	CDisplayData* dataTmp = dynamic_cast<CDisplayData*>( dataList->Exists( data->first ) );
		//	if ( dataTmp != nullptr )
		//		RemoveData( data->first );
		//}
	}

	for ( auto &data : *dataList )
		InsertData( data.first, dynamic_cast<CDisplayData*>( data.second ) );

	return true;
}




//----------------------------------------
bool CDisplay::IsYFXType() const
{
	return m_type == CMapTypeDisp::eTypeDispYFX;
}
//----------------------------------------
bool CDisplay::IsZYFXType() const
{
	return m_type == CMapTypeDisp::eTypeDispZFXY;
}
//----------------------------------------
bool CDisplay::IsZLatLonType() const
{
	return m_type == CMapTypeDisp::eTypeDispZFLatLon;
}

//----------------------------------------
CDisplayData* CDisplay::GetDisplayData( const std::string& name )
{
  return dynamic_cast< CDisplayData* >( m_data.Exists( name ) );
}
//----------------------------------------
CDisplayData* CDisplay::GetDisplayData(CMapDisplayData::iterator it)
{
  return dynamic_cast< CDisplayData* >( it->second );
}

//----------------------------------------
std::string CDisplay::FmtCmdParam( const std::string& name )
{
	return name + "=";
}

//----------------------------------------
bool CDisplay::SaveConfig( CWorkspaceSettings *config, CWorkspaceDisplay *wksd ) const
{
	assert__( config );

	return config->SaveConfig( *this, wksd );
}
//----------------------------------------
bool CDisplay::LoadConfig( CWorkspaceSettings *config, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
{
	assert__( config );
	//return !config || config->LoadConfig( *this, errorMsg, wksd, wkso );

	return config->LoadConfig( *this, errorMsg, wksd, wkso );
}
//----------------------------------------
std::vector<const COperation*> CDisplay::GetOperations() const
{
	std::vector< const COperation* > v;
	for ( CMapDisplayData::const_iterator it = m_data.begin(); it != m_data.end(); it++ )
	{
		const CDisplayData* data = dynamic_cast<const CDisplayData*>( it->second );			assert__( data != nullptr && data->GetOperation() != nullptr );
		auto *op = data->GetOperation();
		if ( std::find( v.begin(), v.end(), op ) == v.end() )		//check repeated
			v.push_back( data->GetOperation() );
	}

	return v;
}

bool CDisplay::UsesOperation( const std::string& name ) const
{
	bool uses = false;
	for ( CMapDisplayData::const_iterator it = m_data.begin(); it != m_data.end(); it++ )
	{
		const CDisplayData* data = dynamic_cast<const CDisplayData*>( it->second );
		if ( data == nullptr )
		{
			const std::string errorMsg = "ERROR in  CDisplay::UseOperation\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplayData";

			throw CException( errorMsg );
		}

		if ( str_icmp( data->GetOperation()->GetName(), name ) )
		{
			uses = true;
			break;
		}
	}

	return uses;
}

//----------------------------------------
std::string CDisplay::GetTaskName()
{
	if ( m_cmdFile.empty() )
		return "NoName";

	return GetFileName( m_cmdFile );
}

//----------------------------------------
bool CDisplay::BuildCmdFile( std::string &error_msg, bool map_as_3dplot )	//map_as_3dplot = false 
{
#ifndef OLD_CREATE_FILES

	return CDisplayCmdFile::BuildCmdFile( m_cmdFile, map_as_3dplot, *this, error_msg );

#else
	m_cmdFile.Normalize();
	m_file.Create( m_cmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR );

	BuildCmdFileHeader();
	BuildCmdFileVerbose();
	BuildCmdFileGeneralProperties();
	BuildCmdFileFields();

	m_file.Close();
	return true;

#endif
}
//----------------------------------------
void CDisplay::InitOutput( CWorkspaceDisplay *wks )
{
	//CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
	if ( wks == nullptr )
		return;

	//wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR	!!! femm: Check this !!!
	std::string output = wks->GetPath()	+ "/Display" + GetName() + "." + COMMANDFILE_EXTENSION;
	SetOutput( output );
}

//----------------------------------------
bool CDisplay::ExistData( const std::string& key )
{
	CDisplayData* dataTmp = dynamic_cast< CDisplayData* >( m_data.Exists( key ) );
	return dataTmp != nullptr;
}
//----------------------------------------
bool CDisplay::InsertData(const std::string& key, CDisplayData* data)
{
  if (ExistData(key))
  {
    return false;
  }
  m_data.Insert(key, data, false);

  m_type = data->GetType();

  return true;
}
//----------------------------------------
bool CDisplay::RemoveData(const std::string& key)
{
  bool bOk = m_data.Erase((const char *)key.c_str());

  if (m_data.size() <= 0)
  {
    m_type = CMapTypeDisp::Invalid();
  }

  return bOk;
}
//----------------------------------------

void CDisplay::SetGroups(bool groupFields)
{
  m_data.SetGroups(groupFields);
}
//----------------------------------------
void CDisplay::Dump( std::ostream& fOut /* = std::cerr */ )
{
	if ( !CTrace::IsTrace() )
		return;


	fOut << "==> Dump a CDisplay Object at " << this << std::endl;

	fOut << "==> END Dump a CDisplay Object at " << this << std::endl;

	fOut << std::endl;
}
