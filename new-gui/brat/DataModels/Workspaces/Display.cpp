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



//const char* CDisplayData::FMT_FLOAT_XY = "%-#.5g";
const char* CDisplayData::FMT_FLOAT_XY = "%-.5g";



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
			const CDisplayData* value = dynamic_cast<const CDisplayData*>( it->second );
			if ( value == nullptr )
				continue;

#if defined(BRAT_V3)
			if ( value->GetXAxis().empty() )
			{
#endif
				axisName = value->GetX()->GetName();				assert__( !axisName.empty() );
				axisLabel = value->GetX()->GetDescription();		assert__( !axisLabel.empty() );

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

		auto &data = mDisplay.GetData();
		for ( CObMap::const_iterator it = data.begin(); it != data.end(); it++ )
		{
			const CDisplayData* value = dynamic_cast<const CDisplayData*>( it->second );
			if ( value == nullptr )
				continue;

			std::string xAxisName;
			std::string yAxisName;

#if defined(BRAT_V3)
			if ( value->IsInvertXYAxes() )
			{
				xAxisName = value->GetY()->GetName();
				yAxisName = value->GetX()->GetName();
			}
			else
			{
#endif
				xAxisName = value->GetX()->GetName();
				yAxisName = value->GetY()->GetName();

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

			assert__( value->GetField()->GetName() == value->FieldName() );

			WriteLn();
			Comment( "----- " + value->GetField()->GetName() + " FIELD -----" );
			WriteLn();
			WriteLn( kwFIELD + "=" + value->GetField()->GetName() );
			WriteLn( kwFILE + "=" + value->Operation()->GetOutputPath() );
			BuildCmdFileFieldProperties( it->first, errorMsg );
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

		//std::string displayName = data->GetField()->GetDescription();
		//if ( !displayName.empty() )
		//{
		//	WriteLn( FmtCmdParam( kwDISPLAY_NAME ) + displayName );
		//}
		//else
		//{
		//	WriteLn( FmtCmdParam( kwDISPLAY_NAME ) + data->GetField()->GetName() );
		//}

		WriteLn( FmtCmdParam( kwDISPLAY_NAME ) + data->UserName() );

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



//-------------------------------------------------------------
//------------------- CDisplayData class --------------------
//-------------------------------------------------------------



//----------------------------------------
CDisplayData::CDisplayData( const CDisplayData &o, const CWorkspaceOperation *wkso, const CWorkspaceDisplay *wksd )
	: base_t( o )
	, mDimFields( EAxisIndex_size )
{
	//	identity / fields

	m_type = o.m_type;

	m_field.SetName( o.m_field.GetName() );
	m_field.SetUnit( o.m_field.GetUnit() );
	m_field.SetDescription( o.m_field.GetDescription() );

	GetX()->SetName( o.GetX()->GetName() );
	GetX()->SetUnit( o.GetX()->GetUnit() );
	GetX()->SetDescription( o.GetX()->GetDescription() );

	GetY()->SetName( o.GetY()->GetName() );
	GetY()->SetUnit( o.GetY()->GetUnit() );
	GetY()->SetDescription( o.GetY()->GetDescription() );

	GetZ()->SetName( o.GetZ()->GetName() );
	GetZ()->SetUnit( o.GetZ()->GetUnit() );
	GetZ()->SetDescription( o.GetZ()->GetDescription() );
	
	//0...group ( TBC )
	m_group = o.m_group;

	//1...operation & display

	mOperation = nullptr;
	if ( o.mOperation != nullptr )
		mOperation = wkso->GetOperation( o.mOperation->GetName() );

#if defined(BRAT_V3)
	//2...axis

	m_xAxis = o.m_xAxis;
	m_invertXYAxes = o.m_invertXYAxes;
#else

	mDisplay = nullptr;
	if ( o.mDisplay != nullptr )
		mDisplay = wksd->GetDisplay( o.mDisplay->GetName() );

#endif
}


//----------------------------------------
#if defined(BRAT_V3)
void CDisplayData::CopyFieldUserProperties( CDisplayData &o  )
{
	//	identity / fields

	m_field.SetDescription( o.m_field.GetDescription() );

	mDimFields[eX].SetDescription( o.GetX()->GetDescription() );
	mDimFields[eY].SetDescription( o.GetY()->GetDescription() );
	mDimFields[eZ].SetDescription( o.GetZ()->GetDescription() );

	//2...vector

	SetEastComponent( o.IsEastComponent());
	SetNorthComponent( o.IsNorthComponent());

	//3...contour

	SetWithContour( o.WithContour() );

	//4...solid color

	SetWithSolidColor( o.WithSolidColor() );

	//..color table && color table range

	SetColorTable( o.ColorTable() );
	SetColorPalette( o.ColorPalette() );

	SetCurrentMinValue( o.CurrentMinValue() );
	SetCurrentMaxValue( o.CurrentMaxValue() );
	SetAbsoluteRangeValues( o.AbsoluteMinValue(), o.AbsoluteMaxValue() );

	//5...axis

	m_xAxis = o.m_xAxis;
	m_invertXYAxes = o.m_invertXYAxes;

	//6...histogram

	SetNumberOfBins( o.NumberOfBins() );
}

const std::string& CDisplayData::GetXAxisText( unsigned int index ) const
{
	assert__( index < mDimFields.size() );

	const CField &dim = mDimFields[ index ];

	return dim.GetDescription();
}


void CDisplayData::SetXAxisText( unsigned int index, const std::string &value )
{
	assert__( index < mDimFields.size() );

	CField &dim = mDimFields[ index ];

	dim.SetDescription( value );
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
void CDisplayData::GetAvailableAxes( CStringArray& names ) const
{
	for ( auto const &dim : mDimFields )
	{
		if ( !dim.GetName().empty() )
		{
			names.Insert( dim.GetName() );
		}
	}
}


#endif
//----------------------------------------


//static 
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

//static 
#if defined(BRAT_V3)
std::string CDisplayData::MakeKey( const COperation *operation, const std::string &field_name, CMapTypeDisp::ETypeDisp type )
#else
std::string CDisplayData::MakeKeyV3( const COperation *operation, const std::string &field_name, CMapTypeDisp::ETypeDisp type )
#endif
{
	if ( operation == nullptr )
		return "";

	return operation->GetName() + "_" + field_name + "_" + n2s<std::string>( type );
}

#if !defined(BRAT_V3)

//static 
std::string CDisplayData::MakeKey( const std::string &field_name, CMapTypeDisp::ETypeDisp type )
{
	return field_name + "_" + n2s<std::string>( type );
}


#endif


std::string CDisplayData::GetDataKey( CMapTypeDisp::ETypeDisp type )
{
#if defined(BRAT_V3)
    return MakeKey( mOperation, m_field.GetName(), type );
#else

	return MakeKey( FieldName(), type );

#endif
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


void CDisplayData::SetDimension( size_t index, const std::string &name, const std::string &description, const std::string &unit )
{
	assert__( index < mDimFields.size() );

	CField &dim = mDimFields[ index ];

	dim.SetName( name );
	dim.SetDescription( description );
	dim.SetUnit( unit );

	const std::string &base_label = description.empty() ? name : description;
	std::string label = base_label + " [" + unit + "]";
	switch ( index )
	{
		case eX:
			if ( Xlabel().empty() )		//when reading from persistence we don't want to change it; this is a user managed variable, we only suggest the first value
				SetXlabel( label );
			break;

		case eY:
			if ( Ylabel().empty() )		//idem
				SetYlabel( label );
			break;

		case eZ:
			assert__( false );
			break;

		default:
			assert__( false );
	}
}


const CField* CDisplayData::FindDimension( const std::string &name ) const
{
	for ( auto const &dim : mDimFields )
	{
		if ( str_icmp( dim.GetName(), name ) )
		{
			return &dim;
		}
	}
	return nullptr;
}

//-------------------------------------------------------------
//				Ex-CMapDisplayData methods
//-------------------------------------------------------------


// Ex-CMapDisplayData called by v3 DisplayPanel when fields were grouped or a field was 
//	added to the CDisplayData GUI list
//
//----------------------------------------
bool CDisplay::CheckFields( std::string &error_msg )
{
	if ( m_data.size() <= 1 )
		return true;

	CObMap internalFilesMap;
	CInternalFiles *file = nullptr;

	for ( CMapDisplayData::iterator it = m_data.begin(); it != m_data.end(); it++ )
	{
		CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
		if ( value == nullptr )
			continue;

		const COperation* operation = value->Operation();
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

	for ( CMapDisplayData::iterator it = m_data.begin(); it != m_data.end(); it++ )
	{
		CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
		if ( value == nullptr )
			continue;

		const COperation* operation = value->Operation();
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
			error_msg += msg;
			bOk = false;
			continue;
		}

		if ( ! firstNetCDFVardef->HaveCompatibleDimUnits( netCDFVardef, firstFile->GetFile(), file->GetFile(), &msg ) )
		{
			error_msg += msg;
			bOk = false;
			continue;
		}

		if ( IsYFXType() )
		{
			if ( !firstNetCDFVardef->HasCompatibleUnit( netCDFVardef ) )
			{
				error_msg += 
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

				error_msg += msg;
				bOk = false;
				continue;
			}
		}
	}

	internalFilesMap.RemoveAll();

	return bOk;
}


bool CDisplay::AreFieldsGrouped() const
{
	bool isGroup = true;

	int groupNumber = 1;

	for ( CMapDisplayData::const_iterator it = m_data.begin(); it != m_data.end(); it++ )
	{
		CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
		if ( value != nullptr )
		{
			int otherNumber = value->GetGroup();

			if ( it == m_data.begin() )
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


void CDisplay::GroupFields()
{
	for ( CMapDisplayData::iterator it = m_data.begin(); it != m_data.end(); it++ )
	{
		CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
		if ( value != nullptr )
		{
			value->SetGroup( 1 );
		}
	}
}


void CDisplay::SplitFields()
{
	int32_t groupNumber = 0;
	for ( CMapDisplayData::iterator it = m_data.begin(); it != m_data.end(); it++ )
	{
		CDisplayData* value = dynamic_cast<CDisplayData*>( it->second );
		if ( value != nullptr )
		{
			groupNumber++;
			value->SetGroup( groupNumber );
		}
	}
}


#if defined(BRAT_V3)

void CDisplay::SetAllAxis( unsigned int index, const std::string& axisName, const std::string& axisLabel )
{
	for ( CMapDisplayData::iterator it = m_data.begin(); it != m_data.end(); it++ )
	{
		CDisplayData* displayData = dynamic_cast<CDisplayData*>( it->second );

		if ( displayData == nullptr )
		{
			continue;
		}

		displayData->SetXAxis( axisName );
		displayData->SetXAxisText( index, axisLabel );
	}
}

void CDisplay::SetAllInvertXYAxes( bool value )
{
	for ( CMapDisplayData::iterator it = m_data.begin(); it != m_data.end(); it++ )
	{
		CDisplayData* displayData = dynamic_cast<CDisplayData*>( it->second );

		if ( displayData == nullptr )
		{
			continue;
		}

		displayData->SetInvertXYAxes( value );
	}
}

#endif




//-------------------------------------------------------------
//------------------- CDisplay class --------------------
//-------------------------------------------------------------

//static 
const std::string CDisplay::m_zoomDelimiter = " ";


void CDisplay::CloneDisplayData( const CMapDisplayData &o, const CWorkspaceDisplay *wksd, const CWorkspaceOperation *wkso )
{
	m_data.RemoveAll();
	for ( CMapDisplayData::const_iterator it = o.begin(); it != o.end(); it++ )
	{
		CDisplayData* value = dynamic_cast<CDisplayData*>( it->second ); 			assert__( value );

		m_data.Insert( value->GetDataKey(), new CDisplayData( *value, wkso, wksd ) );
	}
}


CDisplay::~CDisplay()
{
	if ( IsFile( m_cmdFile ) )
		RemoveFile( m_cmdFile );
}


void CDisplay::UpdateDisplayData( const CMapDisplayData *data_list, const CWorkspaceDisplay *wksd, const CWorkspaceOperation *wkso )
{
	CStringArray keys;
	m_data.GetKeys( keys );
	for ( auto it = keys.begin(); it != keys.end(); ++it )
	{
		if ( !data_list->Exists( *it ) )
			m_data.Erase( *it );
	}
	for ( auto it = data_list->begin(); it != data_list->end(); ++it )
	{
		CDisplayData *data = dynamic_cast<CDisplayData*>( it->second );
		if ( m_type == data->GetType() && !m_data.Exists( it->first ) )
			m_data.Insert( it->first, new CDisplayData( *data, wkso, wksd ), false );
	}
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
//bool CDisplay::AssignOperation( const COperation *operation, bool update )	//bool update = false 
//{
//	CInternalFiles *file = nullptr;
//	CUIntArray displayTypes;
//	CDisplay::GetDisplayType( operation, displayTypes, &file );	//must close returned files, if any
//	if ( file == nullptr )
//	{
//		return false;
//	}
//
//	CMapDisplayData *dataList = new CMapDisplayData;		//v3: dataList, data member
//
//	CStringArray names;
//	names.RemoveAll();
//	file->GetDataVars( names );
//
//	for ( CUIntArray::iterator itDispType = displayTypes.begin(); itDispType != displayTypes.end(); itDispType++ )
//	{
//		for ( CStringArray::iterator itField = names.begin(); itField != names.end(); itField++ )
//		{
//			CStringArray varDimensions;
//			file->GetVarDims( *itField, varDimensions );
//
//			size_t nbDims = varDimensions.size();
//			if ( nbDims > 2 )
//			{
//				continue;
//			}
//			if ( ( nbDims != 2 ) && ( *itDispType == CMapTypeDisp::eTypeDispZFXY || *itDispType == CMapTypeDisp::eTypeDispZFLatLon ) )
//			{
//				continue;
//			}
//
//			CDisplayData* displayData = new CDisplayData( operation );
//
//			displayData->SetType( *itDispType );
//
//			displayData->GetField()->SetName( *itField );
//
//			std::string unit = file->GetUnit( *itField ).GetText();
//			displayData->GetField()->SetUnit( unit );
//
//			std::string comment = file->GetComment( *itField );
//			std::string description = file->GetTitle( *itField );
//
//			if ( !comment.empty() )
//			{
//				description += "." + comment;
//			}
//
//			displayData->GetField()->SetDescription( description );
//
//			if ( nbDims >= 1 )
//			{
//				std::string dimName = varDimensions.at( 0 );
//				displayData->GetX()->SetName( varDimensions.at( 0 ) );
//
//				std::string unit = file->GetUnit( dimName ).GetText();
//				displayData->GetX()->SetUnit( unit );
//
//				displayData->GetX()->SetDescription( file->GetTitle( dimName ) );
//			}
//
//			if ( nbDims >= 2 )
//			{
//				std::string dimName = varDimensions.at( 1 );
//				displayData->GetY()->SetName( varDimensions.at( 1 ) );
//
//				std::string unit = file->GetUnit( dimName ).GetText();
//				displayData->GetY()->SetUnit( unit );
//
//				displayData->GetY()->SetDescription( file->GetTitle( dimName ) );
//			}
//
//			if ( nbDims >= 3 )
//			{
//				std::string dimName = varDimensions.at( 2 );
//				displayData->GetZ()->SetName( varDimensions.at( 2 ) );
//
//				std::string unit = file->GetUnit( dimName ).GetText();
//				displayData->GetZ()->SetUnit( unit );
//
//				displayData->GetZ()->SetDescription( file->GetTitle( dimName ) );
//			}
//
//			dataList->Insert( displayData->GetDataKey(), displayData, false );
//		}
//	}
//	delete file;	//file->Close();		//critical
//
//	if ( update )
//	{
//		m_data.clear();
//	}
//
//	for ( auto &data : *dataList )
//		InsertData( data.first, dynamic_cast<CDisplayData*>( data.second ) );
//
//	return true;
//}




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

#if !defined(BRAT_V3)

CDisplayData* CDisplay::GetFieldDisplayData( const std::string &field_name )
{
	std::string key = CDisplayData::MakeKey( field_name, GetType() );

	return dynamic_cast< CDisplayData* >( m_data.Exists( key ) );
}


CDisplayData* CDisplay::GetFieldDisplayDataV3( const COperation *operation, const std::string &field_name )
{
	std::string key = CDisplayData::MakeKeyV3( operation, field_name, GetType() );

	return dynamic_cast< CDisplayData* >( m_data.Exists( key ) );
}


#endif


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
		const CDisplayData* data = dynamic_cast<const CDisplayData*>( it->second );			assert__( data != nullptr && data->Operation() != nullptr );
		auto *op = data->Operation();
		if ( std::find( v.begin(), v.end(), op ) == v.end() )		//check repeated
			v.push_back( data->Operation() );
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

		if ( str_icmp( data->Operation()->GetName(), name ) )
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
void CDisplay::InitOutput( const CWorkspaceDisplay *wks )
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
void CDisplay::Dump( std::ostream& fOut /* = std::cerr */ )
{
	if ( !CTrace::IsTrace() )
		return;


	fOut << "==> Dump a CDisplay Object at " << this << std::endl;

	fOut << "==> END Dump a CDisplay Object at " << this << std::endl;

	fOut << std::endl;
}
