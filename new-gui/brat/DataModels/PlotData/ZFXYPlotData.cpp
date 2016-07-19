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

#include <ctime>

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"
#include "libbrathl/LatLonPoint.h"
using namespace brathl;

#include "Plots.h"
#include "ZFXYPlotData.h"

#if defined(BRAT_V3)
#include "wx/progdlg.h"
#endif

//-------------------------------------------------------------
//------------------- CZFXYPlotData class --------------------
//-------------------------------------------------------------

CZFXYPlotData::CZFXYPlotData( CZFXYPlot* plot, CZFXYPlotProperties* field )
	: base_t( field )
	, values_base_t()
{
	//m_currentMap = 0;

	//setDefaultValue( m_minhv );
	//setDefaultValue( m_maxhv );

	//setDefaultValue( m_xMin );
	//setDefaultValue( m_xMax );

	//setDefaultValue( m_yMin );
	//setDefaultValue( m_yMax );

	Create( plot );

#if defined (BRAT_V3)
	m_aborted = false;
#endif
}


//----------------------------------------
void CZFXYPlotData::Create( CZFXYPlot *plot )
{
	const std::vector< CInternalFiles* > &internal_files = GetPlotProperties()->InternalFiles();
	const std::string& fieldName = GetPlotProperties()->FieldName();


	if ( GetPlotProperties()->UserName().empty() )
	{
		GetPlotProperties()->SetUserName( fieldName );
	}

#if defined (BRAT_V3)
	if ( GetPlotProperties()->Title().empty() )
	{
		GetPlotProperties()->SetTitle( plot->mTitle );
	}

	if ( GetPlotProperties()->Xlabel().empty() )
	{
        GetPlotProperties()->SetXlabel( plot->TitleX() );
	}
	else
	{
		std::string titleX = GetPlotProperties()->Xlabel();
		titleX += plot->m_unitXLabel;
		GetPlotProperties()->SetXlabel( titleX );
	}

	if ( GetPlotProperties()->Ylabel().empty() )
	{
        GetPlotProperties()->SetYlabel( plot->TitleY() );
	}
	else
	{
		std::string titleY = GetPlotProperties()->Ylabel();
		titleY += plot->m_unitYLabel;
		GetPlotProperties()->SetYlabel( titleY );
	}
#else

	assert__( !plot->TitleX().empty() );
	assert__( !plot->TitleY().empty() );

#endif

	CUnit unitX = plot->XUnit();
	CUnit unitY = plot->YUnit();

	mCurrentFrame = 0;
	size_t nrMaps = internal_files.size();

#if defined(BRAT_V3)		// TODO replace by callback device to display progress
	wxProgressDialog pd("Calculating Plot Data", "", size(), nullptr, wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_SMOOTH| wxPD_APP_MODAL);
	pd.SetSize(-1,-1, 350, -1);
#endif


	bool firstValue = true;

	//double minHeightValue = 0;
	//double maxHeightValue = 0;

	//v4 IMPORTANT NOTE: in some steps, it seems that v3 supported more than one frame (**).
	//	In others it seems it really did not (*). v4 does not support more than ore frame, so
	//	the minimum and maximum values computed here are always relative to one frame.

														assert__( nrMaps < 2 );
	for ( size_t iMap = 0; iMap < nrMaps; iMap++ )		//(**)
	{
		AddMap();

		setDefaultValue( back().mMinX );
		setDefaultValue( back().mMaxX );
		setDefaultValue( back().mMinY );
		setDefaultValue( back().mMaxY );

		//	 (*) v4 note:
		//	These assignments and the assignments below to GetPlotProperties()->m_xxxContourValue 
		//	show that it was implicitly assumed that m_nrMaps was always 1, otherwise these 
		//	values wouldn't be set to GetPlotProperties()->m_xxxHeightValue (which never changes 
		//	in the loop) in each iteration. Or, this is a bug.
		//
		back().mMinHeightValue = GetPlotProperties()->AbsoluteMinValue();
		back().mMaxHeightValue = GetPlotProperties()->AbsoluteMaxValue();

		CExpressionValue varX;
		CExpressionValue varY;
		CExpressionValue varValue;
		NetCDFVarKind varKind;


		CInternalFiles* zfxy = dynamic_cast<CInternalFiles*>( internal_files.at( iMap ) );
		if ( zfxy == nullptr )
		{
			CException e( "CZFXYPlotData ctor - dynamic_cast<CInternalFiles*>internal_files.at(iMap) returns nullptr",	BRATHL_LOGIC_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );

		}
		//if ( zfxy->IsGeographic() )
		//{
		//	CException e( "CZFXYPlotData ctor - Geographical data found - zfxy->IsGeographic() is true", BRATHL_LOGIC_ERROR );
		//	CTrace::Tracer( "%s", e.what() );
		//	throw ( e );

		//}


#if defined(BRAT_V3)		// TODO replace by callback device to display progress
		//update ProgressDialog
		std::string msg = CTools::Format("Calculating Frame: %d of %d", iMap + 1, size() );
		if (pd.Update(iMap, msg.c_str()) == false)
		{
			m_aborted = true;
			break;
		}
#endif

		uint32_t dimRangeX = 0;
		uint32_t dimRangeY = 1;

		std::string varXName;
		std::string varYName;

		plot->GetPlotWidthHeight( zfxy, fieldName, back().mPlotWidth, back().mPlotHeight, varX, varY, dimRangeX, dimRangeY, varXName, varYName );

		// Get and control unit of X axis
		// X units are compatible but not the same --> convert
		CUnit unitXRead = zfxy->GetUnit( varXName );
		if ( unitX.AsString() != unitXRead.AsString() )
		{
			plot->XUnit().SetConversionFrom( unitXRead );
			plot->XUnit().ConvertVector( varX.GetValues(), (int32_t)varX.GetNbValues() );
		}

		// Get and control unit of Y axis
		// Y units are compatible but not the same --> convert
		CUnit unitYRead = zfxy->GetUnit( varYName );
		if ( unitY.AsString() != unitYRead.AsString() )
		{
			plot->YUnit().SetConversionFrom( unitYRead );
			plot->YUnit().ConvertVector( varY.GetValues(), (int32_t)varY.GetNbValues() );
		}


		// Get data unit
		//NOTE this impacts DataUnit function
		CUnit* unit = new CUnit( zfxy->GetUnit( fieldName ) );
		m_dataUnits.push_back( unit );

#if defined(BRAT_V3)

		// Get data title
		std::string dataTitle = zfxy->GetTitle( fieldName );
		if ( dataTitle.empty() )
		{
			dataTitle = GetPlotProperties()->UserName();
		}

		m_dataTitles.Insert( dataTitle );
#endif


		// read data
		zfxy->ReadVar( fieldName, varValue, unit->GetText() );
		varKind	= zfxy->GetVarKind( fieldName );

		if ( varKind != Data )
		{
			std::string msg = CTools::Format( "CZFXYPlotData ctor - variable '%s' is not a kind of Data (%d) : %s",
				fieldName.c_str(), Data, CNetCDFFiles::VarKindToString( varKind ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}

		const bool transpose = dimRangeY == 0 && dimRangeX == 1;

		const int maxX = back().mPlotWidth;
		const int maxY = back().mPlotHeight;

		for ( int iY = 0; iY < maxY; iY++ )
		{
			double valueY = varY.GetValues()[ iY ];
			AddY(valueY);

			if ( iY == 0 )
			{
				if ( iMap == 0 )
				{
					//m_yMin = valueY;
					//m_yMax = valueY;
				}
				back().mMinY = valueY;
				back().mMaxY = valueY;
			}
			else
			{
				//if ( valueY < m_yMin )
				//{
				//	m_yMin = valueY;
				//}
				//if ( valueY > m_yMax )
				//{
				//	m_yMax = valueY;
				//}
				if ( valueY < back().mMinY )
				{
					back().mMinY = valueY;
				}
				if ( valueY > back().mMaxY )
				{
					back().mMaxY = valueY;
				}
			}


			for ( int iX = 0; iX < maxX; iX++ )
			{

				int32_t iTemp;
				if ( transpose )
				{
					iTemp = ( iY * maxX ) + iX;
				}
				else
				{
					iTemp = ( iX * maxY ) + iY;
				}

				double v = varValue.GetValues()[ iTemp ];
				if ( ( CTools::IsNan( v ) != 0 ) || isDefaultValue( v ) )
				{
					AddValue( 0.0 );
					AddBit( false );
				}
				else
				{
					AddValue( v );
					AddBit( true );

					if ( firstValue )
					{
						firstValue = false;
						back().mMinHeightValue = v;
						back().mMaxHeightValue = v;
					}
					else
					{
						if ( v < back().mMinHeightValue )
						{
							back().mMinHeightValue = v;
						}
						if ( v > back().mMaxHeightValue )
						{
							back().mMaxHeightValue = v;
						}
					}
				}
			}
		}

		//if ( isDefaultValue( mMaps().mMinHeightValue ) )
		//	mMaps().mMinHeightValue = m_minhv;
		//if ( isDefaultValue( mMaps().mMaxHeightValue ) )
		//	mMaps().mMaxHeightValue = m_maxhv;

		for ( int iX = 0; iX < maxX; iX++ )
		{
			double valueX = varX.GetValues()[ iX ];
			AddX( valueX );

			if ( iX == 0 )
			{
				if ( iMap == 0 )
				{
					//m_xMin = valueX;
					//m_xMax = valueX;
				}
				back().mMinX = valueX;
				back().mMaxX = valueX;
			}
			else
			{
				//if ( valueX < m_xMin )
				//{
				//	m_xMin = valueX;
				//}
				//if ( valueX > m_xMax )
				//{
				//	m_xMax = valueX;
				//}
				if ( valueX < back().mMinX )
				{
					back().mMinX = valueX;
				}
				if ( valueX > back().mMaxX )
				{
					back().mMaxX = valueX;
				}
			}
		}

		back().OrderAxis();
	}


	if ( isDefaultValue( GetPlotProperties()->MinContourValue() ) || isDefaultValue( GetPlotProperties()->MaxContourValue() ) )
	{
		GetPlotProperties()->SetContourValueRange( back().mMinHeightValue, back().mMaxHeightValue );
	}


#if defined(BRAT_V3)		// TODO replace by callback device to display progress
	pd.Destroy();

	if ( m_aborted )
	{
		return;
	}
#endif
}
