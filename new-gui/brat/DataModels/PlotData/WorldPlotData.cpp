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

#include "BratLookupTable.h"
#include "WorldPlotData.h"

//-------------------------------------------------------------
//------------------- CWorldPlotData class --------------------
//-------------------------------------------------------------

void CWorldPlotData::Create()
{
	// lambda

	auto Crash = []( const std::string &msg, int error_type )
	{
		CException e( msg, error_type );
		CTrace::Tracer( "%s", e.what() );
		throw ( e );
	};


	//function body 

	// v3 function parameters
	//
	const std::vector< CInternalFiles* > &internal_files = GetPlotProperties()->InternalFiles();
	const std::string& fieldName = GetPlotProperties()->FieldName();


	if ( GetPlotProperties()->UserName().empty() )
		GetPlotProperties()->SetUserName( fieldName );

	//m_finished = false;
	//m_currentMap = 0;
	size_t nrMaps = internal_files.size();				assert__( nrMaps < 2 );

	//v4 TODO - begin
	//wxProgressDialog pd( "Calculating Geomap Data", "", nrMaps, parent, wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_SMOOTH | wxPD_APP_MODAL );
	//pd.SetSize( -1, -1, 350, -1 );
	//v4 TODO - end

	bool firstValue = true;

	for ( size_t iMap = 0; iMap < nrMaps; iMap++ )
	{
		AddMap();

		back().mMinHeightValue = GetPlotProperties()->AbsoluteMinValue();
		back().mMaxHeightValue = GetPlotProperties()->AbsoluteMaxValue();

		CExpressionValue varLat;
		CExpressionValue varLon;
		CExpressionValue varValue;
		NetCDFVarKind varKind;
		ExpressionValueDimensions varDimVal;
		CStringArray varDimNames;


		CInternalFilesZFXY* zfxy = dynamic_cast< CInternalFilesZFXY* >( internal_files.at( iMap ) );		
		if ( zfxy == NULL ) 
			Crash( "CWorldPlotData - Non-geographical data found - dynamic_cast<CInternalFilesZFXY*>data->at(iMap) returns NULL", BRATHL_LOGIC_ERROR );

		if ( zfxy->IsGeographic() == false )			
			Crash( "CWorldPlotData - Non-geographical data found - zfxy->IsGeographic() is false", BRATHL_LOGIC_ERROR );

	//v4 TODO - begin
		//update ProgressDialog
		//std::string msg = CTools::Format( "Calculating Frame: %d of %d", iMap + 1, nrMaps );
		//if ( !pd.Update( iMap, msg.c_str() ) ) {
		//	m_aborted = true;
		//	break;
		//}
	//v4 TODO - end

		GetMapLatLon( zfxy, back().mPlotWidth, back().mPlotHeight, varLat, varLon );

		// Get data unit
		CUnit* unit = new CUnit( zfxy->GetUnit( fieldName ) );
		m_dataUnits.push_back( unit );

		// Get data title
		std::string dataTitle = zfxy->GetTitle( fieldName );
		if ( dataTitle.empty() )
			dataTitle = GetPlotProperties()->UserName();
#if defined (BRAT_V3)
		mDataTitles.Insert( dataTitle );
#endif

		// read data
		zfxy->ReadVar( fieldName, varValue, unit->GetText() );
		varKind	= zfxy->GetVarKind( fieldName );
		if ( varKind != Data )
			Crash( CTools::Format( "CWorldPlotData - variable '%s' is not a kind of Data (%d) : %s", fieldName.c_str(), Data, CNetCDFFiles::VarKindToString( varKind ).c_str() ), BRATHL_INCONSISTENCY_ERROR );

		zfxy->GetVarDims( fieldName, varDimVal );
		if ( varDimVal.size() != 2 )
			Crash( CTools::Format( "CWorldPlotData - variable '%s -> number of dimensions value not equal 2 : %ld", fieldName.c_str(), (long)varDimVal.size() ), BRATHL_INCONSISTENCY_ERROR );

		zfxy->GetVarDims( fieldName, varDimNames );
		if ( varDimNames.size() != 2 )
			Crash( CTools::Format( "CWorldPlotData - variable '%s -> number of dimensions name not equal 2 : %ld", fieldName.c_str(), (long)varDimNames.size() ), BRATHL_INCONSISTENCY_ERROR );

		bool firstDimIsLat = false;
		if ( ( zfxy->GetVarKind( varDimNames.at( 0 ) ) == Latitude ) &&
			( zfxy->GetVarKind( varDimNames.at( 1 ) ) == Longitude ) )
		{
			firstDimIsLat = true;
		}
		else if ( ( zfxy->GetVarKind( varDimNames.at( 0 ) ) == Longitude ) &&
			( zfxy->GetVarKind( varDimNames.at( 1 ) ) == Latitude ) )
		{
			firstDimIsLat = false;
		}
		else
			Crash( CTools::Format( "CWorldPlotData - Inconsistency kind of axis : varDimNames|0] = '%s' (%s) and "
				"varDimNames|1] = '%s' (%s)",
				varDimNames.at( 0 ).c_str(), CNetCDFFiles::VarKindToString( zfxy->GetVarKind( varDimNames.at( 0 ) ) ).c_str(),
				varDimNames.at( 1 ).c_str(), CNetCDFFiles::VarKindToString( zfxy->GetVarKind( varDimNames.at( 0 ) ) ).c_str() ), BRATHL_INCONSISTENCY_ERROR );

		//////////////////////////////////////vtkDoubleArray *values = vtkDoubleArray::New();
		//////////////////////////////////////vtkShortArray *bitarray = vtkShortArray::New();
		//////////////////////////////////////vtkShortArray *validMercatorLatitudes = vtkShortArray::New();
		//////////////////////////////////////vtkDoubleArray *longitudes = vtkDoubleArray::New();
		//////////////////////////////////////vtkDoubleArray *latitudes = vtkDoubleArray::New();

		for ( int32_t iY = 0; iY < back().mPlotHeight; iY++ )
		{
			//////////////latitudes->InsertNextValue( varLat.GetValues()[ iY ] );
			AddY( varLat.GetValues()[ iY ] );

			for ( int32_t iX = 0; iX < back().mPlotWidth; iX++ )
			{
				int32_t iTemp;
				if ( firstDimIsLat )
					iTemp = ( iY * back().mPlotWidth ) + iX;
				else
					iTemp = ( iX * back().mPlotHeight ) + iY;

				//int16_t validLatitude = ( CTools::AreValidMercatorLatitude( varLat.GetValues()[ iY ] ) ? 1 : 0 ); validMercatorLatitudes->InsertNextValue( validLatitude );
				AddmValidMercatorLatitude( CTools::AreValidMercatorLatitude( varLat.GetValues()[ iY ] ) );

				double v = varValue.GetValues()[ iTemp ];
				if ( ( CTools::IsNan( v ) != 0 ) || isDefaultValue( v ) )
				{
					//////////////////////////////values->InsertNextValue( 0 );
					//////////////////////////////bitarray->InsertNextValue( 0 );
					AddBit( false );
					AddValue( 0.0 );
				}
				else
				{
					AddBit( true );
					AddValue( v );

					////////////////////////////////////values->InsertNextValue( v );
					////////////////////////////////////bitarray->InsertNextValue( 1 );
					if ( firstValue )
					{
						firstValue = false;
						back().mMinHeightValue = v;
						back().mMaxHeightValue = v;
					}
					else {
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

		//if ( isDefaultValue( back().mMinHeightValue ) )
		//	back().mMinHeightValue = back().mMinHeightValue;
		//if ( isDefaultValue( back().mMaxHeightValue ) )
		//	back().mMaxHeightValue = back().mMaxHeightValue;

		for ( int32_t iX = 0; iX < back().mPlotWidth; iX++ )
		{
			////////////////longitudes->InsertNextValue( CTools::NormalizeLongitude( -180.0, varLon.GetValues()[ iX ] ) ); //longitudes->InsertNextValue(varLon.GetValues()[iX]);
			AddX(CTools::NormalizeLongitude( -180.0, varLon.GetValues()[ iX ] ));
		}

		//////vtkGeoMapFilter *geoMapFilter = vtkGeoMapFilter::New();
		//////geoMapFilter->SetValues( values );

		//////geoMapFilter->SetHeights( values );
		//////geoMapFilter->SetBitarray( bitarray );
		//////geoMapFilter->SetValidMercatorLatitudes( validMercatorLatitudes );
		//////geoMapFilter->SetLongitudes( longitudes );
		//////geoMapFilter->SetLatitudes( latitudes );

		/////////////////////CHECK THIS AGAINST LAST longitudes/latitudes->GetValue( 1/0 ) - BEGIN
		// Compute offset to center points in their cells
		double longitudeOffset = CTools::Abs( ( back().mXaxis[ 1 ] - back().mXaxis[ 0 ] ) / 2 );
		double latitudeOffset = CTools::Abs( ( back().mYaxis[ 1 ] - back().mYaxis[ 0 ] ) / 2 );
		back().mLongitudeOffset = longitudeOffset;
		back().mLatitudeOffset = latitudeOffset;
		//double longitudeOffset = CTools::Abs( ( longitudes->GetValue( 1 ) - longitudes->GetValue( 0 ) ) / 2 );
		//double latitudeOffset = CTools::Abs( ( latitudes->GetValue( 1 ) - latitudes->GetValue( 0 ) ) / 2 );
		/////////////////////CHECK THIS AGAINST LAST longitudes/latitudes->GetValue( 1/0 ) - EMD


		//////geoMapFilter->SetOffsetLatitude( latitudeOffset );
		//////geoMapFilter->SetOffsetLongitude( longitudeOffset );

		//////////geoMapFilter->SetFactor( GetPlotProperties()->m_heightFactor );
		//////////geoMapFilter->SetMapWidth( mapWidth );
		//////////geoMapFilter->SetMapHeight( mapHeight );

		////////////m_geoMapFilterList.Insert( geoMapFilter );

		////////////values->Delete();
		////////////bitarray->Delete();
		////////////validMercatorLatitudes->Delete();
		////////////longitudes->Delete();
		////////////latitudes->Delete();

		back().OrderAxis();
	}
	

	if ( isDefaultValue( GetPlotProperties()->MinContourValue() ) || isDefaultValue( GetPlotProperties()->MaxContourValue() ) )
	{
		GetPlotProperties()->SetContourValueRange( back().mMinHeightValue, back().mMaxHeightValue );
	}

	//v4 TODO - begin
	//pd.Destroy();
	//v4 TODO - end

#if defined (BRAT_V3)
	if ( m_aborted )
		return;
#endif

	//////////////for ( uint32_t iMap = 0; iMap < nrMaps; iMap++ )
	//////////////{
	//////////////	vtkGeoMapFilter *geoMapFilter = dynamic_cast<vtkGeoMapFilter*>( m_geoMapFilterList[ iMap ] );		//geoMapFilter = dynamic_cast<vtkVelocityGlyphFilter*>(m_geoMapFilterList[iMap]);
	//////////////	geoMapFilter->SetMinMappedValue( minHeightValue );
	//////////////	geoMapFilter->SetMaxMappedValue( maxHeightValue );
	//////////////}


	GetPlotProperties()->ColorTable().GetLookupTable()->SetTableRange( back().mMinHeightValue, back().mMaxHeightValue );

	//////////////m_colorBarRenderer->SetLUT( m_LUT );


	//////////////vtkGeoMapFilter *geoMapFilter = dynamic_cast<vtkGeoMapFilter*>( m_geoMapFilterList[ m_currentMap ] );	//geoMapFilter = dynamic_cast<vtkVelocityGlyphFilter*>(m_geoMapFilterList[m_currentMap]);
	//////////////if ( geoMapFilter == NULL )
	//////////////	Crash( CTools::Format( "ERROR in CWorldPlotData ctor  : dynamic_cast<vtkGeoMapFilter*>(m_geoMapFilterList[%d]) returns NULL pointer - " "m_geoMapFilterList std::list seems to contain objects other than those of the class vtkGeoMapFilter", m_currentMap ), BRATHL_LOGIC_ERROR );


	//////////////SetInput( geoMapFilter->GetOutput() );
}


//----------------------------------------
void CWorldPlotData::GetMapLatLon(CInternalFilesZFXY* zfxy,
                           int32_t& width, int32_t& height,
                           CExpressionValue& varLat, CExpressionValue& varLon)
{
  CStringArray axisNames;
  CStringArray::iterator is;
  ExpressionValueDimensions dimVal;

  width = -1;
  height = -1;

  zfxy->GetAxisVars(axisNames);

  for (is = axisNames.begin(); is != axisNames.end(); is++)
  {
    zfxy->GetVarDims(*is, dimVal);
    if (dimVal.size() != 1)
    {
      std::string msg = CTools::Format("CWorldPlotData::GetMapLatLon - '%s' axis -> number of dimensions not equal 1 : %ld",
                                   (*is).c_str(), (long)dimVal.size());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }
    NetCDFVarKind varKind = zfxy->GetVarKind(*is);
    if (varKind == Longitude)
    {
      width = dimVal.at(0);
      zfxy->ReadVar(*is, varLon, CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE);
    }
    else if (varKind == Latitude)
    {
      height = dimVal.at(0);
      zfxy->ReadVar(*is, varLat, CLatLonPoint::m_DEFAULT_UNIT_LATITUDE);
    }
  }

  if (width == -1)
    {
      std::string msg = CTools::Format("CWorldPlotData::GetMapLatLon - Longitude axis not found in input file '%s'",
                                   zfxy->GetName().c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

  if (height == -1)
    {
      std::string msg = CTools::Format("CWorldPlotData::GetMapLatLon - Latitude axis not found in input file '%s'",
                                   zfxy->GetName().c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

}


//-------------------------------------------------------------
//------------------- CWorldPlotVelocityData class --------------------
//-------------------------------------------------------------

//----------------------------------------
void CWorldPlotVelocityData::Create( const std::vector< CInternalFiles* > &north_files, const std::vector< CInternalFiles* > &east_files )
{
	auto Crash = []( const std::string &msg, int error_type )
	{
		CException e( msg, error_type );
		CTrace::Tracer( "%s", e.what() );
		throw ( e );
	};


	//GetPlotProperties()->SetName( FieldNameEast() + "/" + FieldNameNorth() );		in v4 THIS changes FieldNameNorth()

	/////////////////m_colorBarRenderer = new CLUTRenderer();
	/////////////////m_colorBarRenderer->GetVtkRenderer()->InteractiveOff();

	////////////////m_colorBarRenderer->SetNumberOfLabels( GetPlotProperties()->m_numColorLabels );

	//m_finished = false;
	//m_currentMap = 0;

	// check both are of the same size
	size_t nrMaps = north_files.size();
	if ( north_files.size() != east_files.size() )
		Crash("VTK_CWorldPlotVelocityData - North and East data of different size!", BRATHL_LOGIC_ERROR);

	//v4 TODO - begin
	//wxProgressDialog pd("Calculating Geomap Data", "", nrMaps, parent, wxPD_CAN_ABORT | wxPD_REMAINING_TIME | wxPD_SMOOTH| wxPD_APP_MODAL);
	//pd.SetSize(-1,-1, 350, -1);
	//v4 TODO - end

	bool firstValue = true;

	//double minHeightValue = 0;
	//double maxHeightValue = 0;

	for ( size_t iMap = 0; iMap < nrMaps; iMap++ )
	{
		AddMap();

		back().mMinHeightValue = GetPlotProperties()->AbsoluteMinValue();
		back().mMaxHeightValue = GetPlotProperties()->AbsoluteMaxValue();

		CExpressionValue varLat;
		CExpressionValue varLon;
		CExpressionValue varValueNorth;
		CExpressionValue varValueEast;
		NetCDFVarKind varKindNorth;
		NetCDFVarKind varKindEast;
		ExpressionValueDimensions varDimValNorth;
		ExpressionValueDimensions varDimValEast;
		CStringArray varDimNamesNorth;
		CStringArray varDimNamesEast;


		// check the file for North data
		CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>( north_files.at( iMap ) );
		CInternalFilesZFXY* zfxyEast = dynamic_cast<CInternalFilesZFXY*>( east_files.at( iMap ) );


		if ( zfxy == NULL || zfxy == NULL )
			Crash( "VTK_CWorldPlotVelocityData ctor - Non-geographical data found - dynamic_cast<CInternalFilesZFXY*>data->at(iMap) returns NULL", BRATHL_LOGIC_ERROR );


		if ( zfxy->IsGeographic() == false || zfxyEast->IsGeographic() == false )
			Crash( "VTK_CWorldPlotVelocityData ctor - Non-geographical data found - IsGeographic() is false", BRATHL_LOGIC_ERROR );

	//v4 TODO - begin
		//update ProgressDialog
		//std::string msg = CTools::Format( "Calculating Frame: %d of %d", iMap + 1, nrMaps );
		//if ( !pd.Update( iMap, msg.c_str() ) ) 
		//{
		//  m_aborted = true;
		//  break;
		//}
	//v4 TODO - end


//		int32_t mapWidth = 0;
//		int32_t mapHeight = 0;

		GetMapLatLon( zfxy, back().mPlotWidth, back().mPlotHeight, varLat, varLon );

		// Get data units
		CUnit* unitNorth = new CUnit( zfxy->GetUnit( FieldNameNorth() ) );
		CUnit* unitEast = new CUnit( zfxyEast->GetUnit( FieldNameEast() ) );
		if ( *unitNorth != *unitEast )
			Crash( CTools::Format( "VTK_CWorldPlotVelocityData ctor - Unequal Units (%s != %s)", "d", "d" ), BRATHL_INCONSISTENCY_ERROR );

		m_dataUnits.push_back( unitNorth );		//v3 original comment m_dataUnits.Insert(unitEast);

#if defined (BRAT_V3)
		mDataTitles.Insert( FieldNameEast() + "/" + FieldNameNorth() /*GetPlotProperties()->Name() */);
#endif

		// read data
		zfxy->ReadVar( FieldNameNorth(), varValueNorth, unitNorth->GetText() );
		varKindNorth	= zfxy->GetVarKind( FieldNameNorth() );

		zfxy->ReadVar( FieldNameEast(), varValueEast, unitEast->GetText() );
		// v3 original comment this may need to be changed
		varKindEast	= zfxyEast->GetVarKind( FieldNameEast() );


		if ( varKindNorth != Data || varKindEast != varKindNorth )
			Crash(CTools::Format( "VTK_CWorldPlotVelocityData ctor - variables '%s', '%s' are not a kind of Data (%d) : %s", FieldNameNorth().c_str(), FieldNameEast().c_str(), Data, CNetCDFFiles::VarKindToString( varKindNorth ).c_str() ), BRATHL_INCONSISTENCY_ERROR);


		zfxy->GetVarDims( FieldNameNorth(), varDimValNorth );
		zfxyEast->GetVarDims( FieldNameEast(), varDimValEast );
		if ( varDimValNorth.size() != 2 || varDimValNorth.size() != varDimValEast.size() )
			Crash( CTools::Format( "VTK_CWorldPlotVelocityData ctor - variables '%s or %s -> number of dimensions value not equal 2 : %ld, %ld", FieldNameNorth().c_str(), FieldNameEast().c_str(), (long)varDimValNorth.size(), (long)varDimValEast.size() ), BRATHL_INCONSISTENCY_ERROR );

		zfxy->GetVarDims( FieldNameNorth(), varDimNamesNorth );
		zfxyEast->GetVarDims( FieldNameEast(), varDimNamesEast );
		if ( varDimNamesNorth.size() != 2 || varDimNamesEast.size() != varDimNamesNorth.size() )
			Crash( CTools::Format( "VTK_CWorldPlotVelocityData ctor - variable '%s  or %s-> number of dimensions name not equal 2 : %ld, %ld", FieldNameNorth().c_str(), FieldNameEast().c_str(), (long)varDimNamesNorth.size(), (long)varDimNamesEast.size() ), BRATHL_INCONSISTENCY_ERROR);

		// v3 original comment TODO (cornejo#1#): should we check this with the East velocity as well??
		bool firstDimIsLat = false;
		if ( ( zfxy->GetVarKind( varDimNamesNorth.at( 0 ) ) == Latitude ) &&
			( zfxy->GetVarKind( varDimNamesNorth.at( 1 ) ) == Longitude ) )
		{
			firstDimIsLat = true;
		}
		else if ( ( zfxy->GetVarKind( varDimNamesNorth.at( 0 ) ) == Longitude ) &&
			( zfxy->GetVarKind( varDimNamesNorth.at( 1 ) ) == Latitude ) )
		{
			firstDimIsLat = false;
		}
		else
			Crash( CTools::Format( "VTK_CWorldPlotVelocityData ctor - Inconsistency kind of axis : varDimNames|0] = '%s' (%s) and "
				"varDimNames|1] = '%s' (%s)",
				varDimNamesNorth.at( 0 ).c_str(), CNetCDFFiles::VarKindToString( zfxy->GetVarKind( varDimNamesNorth.at( 0 ) ) ).c_str(),
				varDimNamesNorth.at( 1 ).c_str(), CNetCDFFiles::VarKindToString( zfxy->GetVarKind( varDimNamesNorth.at( 0 ) ) ).c_str() ), BRATHL_INCONSISTENCY_ERROR );
				
		const int32_t maxX = back().mPlotWidth;
		const int32_t maxY = back().mPlotHeight;


		//////////////////////////////////////vtkDoubleArray* valuesNorth = vtkDoubleArray::New();
		//////////////////////////////////////vtkDoubleArray* valuesEast = vtkDoubleArray::New();
		//////////////////////////////////////vtkShortArray* bitarray = vtkShortArray::New();
		//////////////////////////////////////vtkShortArray* validMercatorLatitudes = vtkShortArray::New();
		//////////////////////////////////////vtkDoubleArray* longitudes = vtkDoubleArray::New();
		//////////////////////////////////////vtkDoubleArray* latitudes = vtkDoubleArray::New();

		for ( int32_t iY = 0; iY < maxY; iY++ )
		{
			//////////////latitudes->InsertNextValue( varLat.GetValues()[ iY ] );
			AddY( varLat.GetValues()[ iY ] );

			for ( int32_t iX = 0; iX < maxX; iX++ )
			{
				int32_t iTemp;
				if ( firstDimIsLat )
					iTemp = ( iY * maxX ) + iX;
				else
					iTemp = ( iX * maxY ) + iY;

				int16_t validLatitude = ( CTools::AreValidMercatorLatitude( varLat.GetValues()[ iY ] ) ? 1 : 0 );
				////////////////////////////validMercatorLatitudes->InsertNextValue( validLatitude );
				AddmValidMercatorLatitude( validLatitude != 0 );
				
				double vN = varValueNorth.GetValues()[ iTemp ];
				double vE = varValueEast.GetValues()[ iTemp ];

				// invalid values?
				if ( ( CTools::IsNan( vN ) != 0 ) || ( CTools::IsNan( vE ) != 0 ) || isDefaultValue( vN ) || isDefaultValue( vE ) )
				{
					//////////////////////////////valuesNorth->InsertNextValue( 0 );
					//////////////////////////////valuesEast->InsertNextValue( 0 );
					//////////////////////////////bitarray->InsertNextValue( 0 );
					AddBit( false );
					AddValue( 0.0 );
					AddEastValue( 0.0 );
				}
				else
				{
					AddBit( true );
					AddValue( vN );
					AddEastValue( vE );
					
					double magnitude = sqrt( vE*vE + vN*vN );
					
					/////////////////////////////////////valuesNorth->InsertNextValue( vN );
					/////////////////////////////////////valuesEast->InsertNextValue( vE );
					/////////////////////////////////////bitarray->InsertNextValue( 1 );
					
					// this is just to figure out minimums and maximums
					if ( firstValue )
					{
						firstValue = false;
						m_minhvN = vN;
						m_maxhvN = vN;

						m_minhvE = vE;
						m_maxhvE = vE;

						back().mMaxHeightValue = magnitude;
						back().mMinHeightValue = magnitude;
					}
					else
					{
						if ( vN < m_minhvN )
						{
							m_minhvN = vN;
						}
						if ( vN > m_maxhvN )
						{
							m_maxhvN = vN;
						}


						if ( vE < m_minhvE )
						{
							m_minhvE = vE;
						}
						if ( vE > m_maxhvE )
						{
							m_maxhvE = vE;
						}

						if ( magnitude < back().mMinHeightValue )
						{
							back().mMinHeightValue = magnitude;
						}
						if ( magnitude > back().mMaxHeightValue )
						{
							back().mMaxHeightValue = magnitude;
						}

					}
				}
			}
		}

		if ( isDefaultValue( back().mMinHeightValue ) )
			back().mMinHeightValue = m_minhvN < m_minhvE ? m_minhvN : m_minhvE;
		if ( isDefaultValue( back().mMaxHeightValue ) )
			back().mMaxHeightValue = m_minhvN > m_minhvE ? m_minhvN : m_minhvE;

		for ( int32_t iX = 0; iX < maxX; iX++ )
		{
			////////////////////longitudes->InsertNextValue( CTools::NormalizeLongitude( -180.0, varLon.GetValues()[ iX ] ) );			//v3 original comment longitudes->InsertNextValue(varLon.GetValues()[iX]);
			AddX(CTools::NormalizeLongitude( -180.0, varLon.GetValues()[ iX ] ));
		}

		/////////vtkVelocityGlyphFilter* geoMapFilter = vtkVelocityGlyphFilter::New();
		/////////geoMapFilter->SetValuesNorth( valuesNorth );
		/////////geoMapFilter->SetValuesEast( valuesEast );

		/////////geoMapFilter->SetMaxVN( m_maxhvN );
		/////////geoMapFilter->SetMinVN( m_minhvN );
		/////////geoMapFilter->SetMaxVE( m_maxhvE );
		/////////geoMapFilter->SetMinVE( m_minhvE );
		//v3 original comment geoMapFilter->SetMinV(back().mMinHeightValue);
		/////////geoMapFilter->SetMaxV( back().mMaxHeightValue );

		//v3 original comment  TODO (cornejo#1#): what should we use here.  Magnitude??
		/////////geoMapFilter->SetHeights( valuesNorth );

		/////////geoMapFilter->SetBitarray( bitarray );
		/////////geoMapFilter->SetValidMercatorLatitudes( validMercatorLatitudes );
		/////////geoMapFilter->SetLongitudes( longitudes );
		/////////geoMapFilter->SetLatitudes( latitudes );

		//geoMapFilter->SetOffsetLatitude(1);
		//geoMapFilter->SetOffsetLongitude(1);

		//////////geoMapFilter->SetFactor( GetPlotProperties()->m_heightFactor );
		//////////geoMapFilter->SetMapWidth( mapWidth );
		//////////geoMapFilter->SetMapHeight( mapHeight );

		////////////m_geoMapFilterList.Insert( geoMapFilter );

		//////////valuesNorth->Delete();
		//////////valuesEast->Delete();

		////////////bitarray->Delete();
		////////////validMercatorLatitudes->Delete();
		////////////longitudes->Delete();
		////////////latitudes->Delete();

		back().OrderAxis();
	}


	if ( isDefaultValue( GetPlotProperties()->MinContourValue() ) || isDefaultValue( GetPlotProperties()->MaxContourValue() ) )
	{
		GetPlotProperties()->SetContourValueRange( back().mMinHeightValue, back().mMaxHeightValue );
	}


	//v4 TODO - begin
	//pd.Destroy();
	//v4 TODO - end

	//if ( m_aborted )
	//	return;

	//////////////for ( iMap = 0; iMap < nrMaps; iMap++ )
	//////////////{
	//////////////	vtkGeoMapFilter* gMapFilter = dynamic_cast<vtkGeoMapFilter*>( m_geoMapFilterList[ iMap ] );
	//////////////	if ( gMapFilter == NULL )
	//////////////	{
	//////////////		continue;
	//////////////	}

	//////////////	gMapFilter->SetMinMappedValue( minHeightValue );
	//////////////	gMapFilter->SetMaxMappedValue( maxHeightValue );
	//////////////}


	GetPlotProperties()->ColorTable().GetLookupTable()->SetTableRange( 0, back().mMaxHeightValue );
	GetPlotProperties()->SetColorPalette( PALETTE_BLACKTOWHITE );							//mLUT->Black();

	// v3 original comment no LUT displayed for this
	//v3 original comment m_colorBarRenderer->SetLUT(m_LUT);

	//////////////geoMapFilter = dynamic_cast<vtkVelocityGlyphFilter*>( m_geoMapFilterList[ m_currentMap ] );
	//////////////if ( geoMapFilter == NULL )
	//////////////{
	//////////////	throw CException( CTools::Format( "ERROR in VTK_CWorldPlotVelocityData ctor  : dynamic_cast<vtkVelocityGlyphFilter*>(m_geoMapFilterList[%d]) returns NULL pointer - "
	//////////////		"m_geoMapFilterList std::list seems to contain objects other than those of the class vtkVelocityGlyphFilter", m_currentMap ),
	//////////////		BRATHL_LOGIC_ERROR );
	//////////////	return;
	//////////////}

	//////////////m_geoMapFilter = geoMapFilter;
	//////////////m_geoMapFilter->SetIsGlyph( IsGlyph );
	//////////////SetInput( geoMapFilter->GetOutput() );
}
