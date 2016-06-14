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

#include "ZFXYPlotData.h"

#if defined(BRAT_V3)
#include "wx/progdlg.h"
#endif


//-------------------------------------------------------------
//------------------- CZFXYPlotProperties class --------------------
//-------------------------------------------------------------

CZFXYPlotProperties::CZFXYPlotProperties()
{
	m_opacity = 0.7;

	m_showPropertyPanel = true;
	m_showColorBar = true;
	m_showAnimationToolbar = false;

	m_numColorLabels = 5;

	setDefaultValue( m_minHeightValue );
	setDefaultValue( m_maxHeightValue );

	mLUT = new CBratLookupTable();
	mLUT->ExecMethodDefaultColorTable();

	setDefaultValue( m_xMax );
	setDefaultValue( m_xMin );
	setDefaultValue( m_yMax );
	setDefaultValue( m_yMin );

	m_xBase = 10.0;
	m_yBase = 10.0;

	setDefaultValue( m_xNumTicks );
	setDefaultValue( m_yNumTicks );
	setDefaultValue( m_zNumTicks );

	m_xLog = false;
	m_yLog = false;

	m_withContour = false;
	m_solidColor = true;;

	m_withContourLabel = false;
	setDefaultValue( m_minContourValue );
	setDefaultValue( m_maxContourValue );
	m_numContour = 5;
	m_numContourLabel = 1;

	m_contourLabelSize = 10;
	m_contourLineWidth = 1.0;

	m_contourLineColor.Set( 0.0, 0.0, 0.0 );
	m_contourLineColor.Set( 0.0, 0.0, 0.0 );

	m_contourLabelFormat = "%-#.3g";

	m_withAnimation = false;
}

//----------------------------------------

CZFXYPlotProperties::CZFXYPlotProperties( const CZFXYPlotProperties& p )
{
	mLUT = nullptr;
	Copy( p );
}

//----------------------------------------
const CZFXYPlotProperties& CZFXYPlotProperties::operator=( const CZFXYPlotProperties& p )
{
	Copy( p );
	return *this;
}
//----------------------------------------
CZFXYPlotProperties::~CZFXYPlotProperties()
{
	DeleteLUT();
}

//----------------------------------------
void CZFXYPlotProperties::DeleteLUT()
{
	if ( mLUT != nullptr )
	{
		delete mLUT;
		mLUT = nullptr;
	}
}

//----------------------------------------
void CZFXYPlotProperties::Copy( const CZFXYPlotProperties& p )
{
	DeleteLUT();
	mLUT = new CBratLookupTable( *( p.mLUT ) );

	m_title = p.m_title;

	m_name = p.m_name;

	m_xLabel = p.m_xLabel;
	m_yLabel = p.m_yLabel;

	m_opacity = p.m_opacity;
	m_showPropertyPanel = p.m_showPropertyPanel;
	m_showColorBar = p.m_showColorBar;
	m_showAnimationToolbar = p.m_showAnimationToolbar;

	m_numColorLabels = p.m_numColorLabels;

	m_minHeightValue = p.m_minHeightValue;
	m_maxHeightValue = p.m_maxHeightValue;

	m_xMax = p.m_xMax;
	m_xMin = p.m_xMin;
	m_yMax = p.m_yMax;
	m_yMin = p.m_yMin;

	m_xBase = p.m_xBase;
	m_yBase = p.m_yBase;

	m_xLog = p.m_xLog;
	m_yLog = p.m_yLog;

	m_xNumTicks = p.m_xNumTicks;
	m_yNumTicks = p.m_yNumTicks;
	m_zNumTicks = p.m_zNumTicks;

	m_withContour = p.m_withContour;
	m_withContourLabel = p.m_withContourLabel;
	m_minContourValue = p.m_minContourValue;
	m_maxContourValue = p.m_maxContourValue;
	m_numContour = p.m_numContour;
	m_numContourLabel = p.m_numContourLabel;

	m_contourLabelSize = p.m_contourLabelSize;
	m_contourLineWidth = p.m_contourLineWidth;

	m_contourLineColor = p.m_contourLineColor;
	m_contourLabelColor = p.m_contourLabelColor;

	m_contourLabelFormat = p.m_contourLabelFormat;

	m_solidColor = p.m_solidColor;

	m_withAnimation = p.m_withAnimation;
}

//-------------------------------------------------------------
//------------------- CZFXYPlotData class --------------------
//-------------------------------------------------------------

CZFXYPlotData::CZFXYPlotData( CZFXYPlot* plot, CPlotField* field )
{
	if ( field == nullptr )
	{
		return;
	}

	if ( field->m_zfxyProps != nullptr )
	{
		m_plotProperties = *field->m_zfxyProps;
	}

	m_currentMap = 0;

	m_aborted = false;

	setDefaultValue( m_minhv );
	setDefaultValue( m_maxhv );

	setDefaultValue( m_xMin );
	setDefaultValue( m_xMax );

	setDefaultValue( m_yMin );
	setDefaultValue( m_yMax );

	mLUT = nullptr;

	
	Create( &( field->m_internalFiles ), field->m_name, plot );


	SetLUT( m_plotProperties.mLUT );

	mLUT->GetLookupTable()->SetTableRange( m_plotProperties.m_minContourValue, m_plotProperties.m_maxContourValue );
}
//----------------------------------------
CZFXYPlotData::~CZFXYPlotData()
{
	DeleteLUT();
}


//----------------------------------------
void CZFXYPlotData::DeleteLUT()
{
	if ( mLUT != nullptr )
	{
		delete mLUT;
		mLUT = nullptr;
	}
}

void CZFXYPlotData::SetLUT( CBratLookupTable* lut )
{
	if ( lut == nullptr )
	{
		return;
	}

	DeleteLUT();

	mLUT = new CBratLookupTable( *lut );

	//if ( m_vtkMapper2D != nullptr )
	//{
	//	m_vtkMapper2D->SetLookupTable( m_LUT->GetLookupTable() );
	//}
}


//----------------------------------------
std::string CZFXYPlotData::GetDataDateString( size_t index ) const
{
	if ( index >= m_dataDates.size() )
	{
		return "";

	}

	CDate* dataDate = dynamic_cast<CDate*>( m_dataDates[ index ] );
	if ( dataDate->IsDefaultValue() == false )
	{
		return CTools::Format( 128, "Date:\t%s - Time:\t%s",
			dataDate->AsString( "%d-%b-%Y", false ).c_str(),
			dataDate->AsString( "%H:%M:%S", true ).c_str() );
	}

	return "";
}

//----------------------------------------
std::string CZFXYPlotData::GetDataUnitString( size_t index ) const
{
	if ( index >= m_dataUnits.size() )
	{
		return "";
	}

	CUnit* dataUnit = dynamic_cast<CUnit*>( m_dataUnits[ index ] );

	return dataUnit->GetText();
}

//----------------------------------------
void CZFXYPlotData::Create( CObArray* data, const std::string& fieldName, CZFXYPlot* plot )
{
	if ( m_plotProperties.m_name.empty() )
	{
		m_plotProperties.m_name = fieldName.c_str();
	}

	if ( m_plotProperties.m_title.empty() )
	{
		m_plotProperties.m_title = plot->m_title;
	}


	if ( m_plotProperties.m_xLabel.empty() )
	{
		m_plotProperties.m_xLabel = plot->m_titleX;
	}
	else
	{
		std::string titleX = m_plotProperties.m_xLabel;
		titleX += plot->m_unitXLabel;
		m_plotProperties.m_xLabel = titleX;
	}

	if ( m_plotProperties.m_yLabel.empty() )
	{
		m_plotProperties.m_yLabel = plot->m_titleY;
	}
	else
	{
		std::string titleY = m_plotProperties.m_yLabel;
		titleY += plot->m_unitYLabel;
		m_plotProperties.m_yLabel = titleY;
	}

	m_unitX = plot->m_unitX;
	m_unitY = plot->m_unitY;

	m_currentMap = 0;
	m_nrMaps = (uint32_t)data->size();

#if defined(BRAT_V3)		// TODO replace by callback device to display progress
	wxProgressDialog pd("Calculating Plot Data", "", m_nrMaps, nullptr, wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_SMOOTH| wxPD_APP_MODAL);
	pd.SetSize(-1,-1, 350, -1);
#endif


	bool firstValue = true;

	//double minHeightValue = 0;
	//double maxHeightValue = 0;

	for ( uint32_t iMap = 0; iMap < m_nrMaps; iMap++ )
	{
		mMaps.AddMap();

		setDefaultValue( mMaps().mMinX );
		setDefaultValue( mMaps().mMaxX );
		setDefaultValue( mMaps().mMinY );
		setDefaultValue( mMaps().mMaxY );

		//	v4 note:
		//	These assignments and the assignments below to m_plotProperties.m_xxxContourValue 
		//	show that it was implicitly assumed that m_nrMaps was always 1, otherwise these 
		//	values wouldn't be set to m_plotProperties.m_xxxHeightValue (which never changes 
		//	in the loop) in each iteration. Or, this is a bug.
		//
		mMaps().mMinHeightValue = m_plotProperties.m_minHeightValue;
		mMaps().mMaxHeightValue = m_plotProperties.m_maxHeightValue;

		CExpressionValue varX;
		CExpressionValue varY;
		CExpressionValue varValue;
		NetCDFVarKind varKind;


		CInternalFiles* zfxy = dynamic_cast<CInternalFiles*>( data->at( iMap ) );
		if ( zfxy == nullptr )
		{
			CException e( "CZFXYPlotData ctor - dynamic_cast<CInternalFiles*>data->at(iMap) returns nullptr",	BRATHL_LOGIC_ERROR );
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
		std::string msg = CTools::Format("Calculating Frame: %d of %d", iMap + 1, m_nrMaps);
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

		plot->GetPlotWidthHeight( zfxy, fieldName, mMaps().mPlotWidth, mMaps().mPlotHeight, varX, varY, dimRangeX, dimRangeY, varXName, varYName );

		CUnit unitXRead;
		CUnit unitYRead;

		// Get and control unit of X axis
		// X units are compatible but not the same --> convert
		unitXRead = zfxy->GetUnit( varXName );
		if ( m_unitX.AsString() != unitXRead.AsString() )
		{
			plot->m_unitX.SetConversionFrom( unitXRead );
			plot->m_unitX.ConvertVector( varX.GetValues(), (int32_t)varX.GetNbValues() );
		}

		// Get and control unit of Y axis
		// Y units are compatible but not the same --> convert
		unitYRead = zfxy->GetUnit( varYName );
		if ( m_unitY.AsString() != unitYRead.AsString() )
		{
			plot->m_unitY.SetConversionFrom( unitYRead );
			plot->m_unitY.ConvertVector( varY.GetValues(), (int32_t)varY.GetNbValues() );
		}


		// Get data unit
		CUnit* unit = new CUnit( zfxy->GetUnit( fieldName ) );
		m_dataUnits.Insert( unit );

		// Get data title
		std::string dataTitle = zfxy->GetTitle( fieldName );
		if ( dataTitle.empty() )
		{
			dataTitle = m_plotProperties.m_name;
		}

		m_dataTitles.Insert( dataTitle );


		// read data
		zfxy->ReadVar( fieldName, varValue, unit->GetText() );
		varKind	= zfxy->GetVarKind( fieldName );

		m_unitZ = *unit;

		if ( varKind != Data )
		{
			std::string msg = CTools::Format( "CZFXYPlotData ctor - variable '%s' is not a kind of Data (%d) : %s",
				fieldName.c_str(), Data, CNetCDFFiles::VarKindToString( varKind ).c_str() );
			CException e( msg, BRATHL_INCONSISTENCY_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}

		bool transpose = false;

		if ( ( dimRangeY == 0 ) &&
			( dimRangeX == 1 ) )
		{
			transpose = true;
		}

		const int maxX = mMaps().mPlotWidth;
		const int maxY = mMaps().mPlotHeight;

		for ( int iY = 0; iY < maxY; iY++ )
		{
			double valueY = varY.GetValues()[ iY ];
			mMaps.AddY(valueY);

			if ( iY == 0 )
			{
				if ( iMap == 0 )
				{
					m_yMin = valueY;
					m_yMax = valueY;
				}

				mMaps().mMinY = valueY;
				mMaps().mMaxY = valueY;
			}
			else
			{
				if ( valueY < m_yMin )
				{
					m_yMin = valueY;
				}
				if ( valueY > m_yMax )
				{
					m_yMax = valueY;
				}
				if ( valueY < mMaps().mMinY )
				{
					mMaps().mMinY = valueY;
				}
				if ( valueY > mMaps().mMaxY )
				{
					mMaps().mMaxY = valueY;
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
					mMaps.AddValue( 0.0 );
					mMaps.AddBit( false );
				}
				else
				{
					mMaps.AddValue( v );
					mMaps.AddBit( true );

					if ( firstValue )
					{
						firstValue = false;
						m_minhv = v;
						m_maxhv = v;
					}
					else
					{
						if ( v < m_minhv )
						{
							m_minhv = v;
						}
						if ( v > m_maxhv )
						{
							m_maxhv = v;
						}
					}
				}
			}
		}

		if ( isDefaultValue( mMaps().mMinHeightValue ) )
		{
			mMaps().mMinHeightValue = m_minhv;
		}
		if ( isDefaultValue( mMaps().mMaxHeightValue ) )
		{
			mMaps().mMaxHeightValue = m_maxhv;
		}

		for ( int iX = 0; iX < maxX; iX++ )
		{
			double valueX = varX.GetValues()[ iX ];
			mMaps.AddX( valueX );

			if ( iX == 0 )
			{
				if ( iMap == 0 )
				{
					m_xMin = valueX;
					m_xMax = valueX;
				}

				mMaps().mMinX = valueX;
				mMaps().mMaxX = valueX;

			}
			else
			{
				if ( valueX < m_xMin )
				{
					m_xMin = valueX;
				}
				if ( valueX > m_xMax )
				{
					m_xMax = valueX;
				}
				if ( valueX < mMaps().mMinX )
				{
					mMaps().mMinX = valueX;
				}
				if ( valueX > mMaps().mMaxX )
				{
					mMaps().mMaxX = valueX;
				}
			}

		}

		mMaps().OrderAxis();
	}


	if ( isDefaultValue( m_plotProperties.m_minContourValue ) )
	{
		m_plotProperties.m_minContourValue = mMaps().mMinHeightValue;
	}

	if ( isDefaultValue( m_plotProperties.m_maxContourValue ) )
	{
		m_plotProperties.m_maxContourValue = mMaps().mMaxHeightValue;
	}

#if defined(BRAT_V3)		// TODO replace by callback device to display progress
	pd.Destroy();
#endif

	if ( m_aborted )
	{
		return;
	}
}


void CZFXYPlotData::GetXRange( double& min, double& max, size_t frame ) const
{
	if ( frame >= mMaps.size() )
	{
		frame = mMaps.size() - 1;
	}

	auto &parameters = mMaps( frame );

	min = parameters.mMinX;
	max = parameters.mMaxX;
}


void CZFXYPlotData::GetYRange(double& min, double& max, size_t frame) const
{
	if ( frame >= mMaps.size() )
	{
		frame = mMaps.size() - 1;
	}

	auto &parameters = mMaps( frame );

	min = parameters.mMinY;
	max = parameters.mMaxY;
}
