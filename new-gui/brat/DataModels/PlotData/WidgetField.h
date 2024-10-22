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
#ifndef DATA_MODELS_PLOT_DATA_WIDGET_FIELD_H
#define DATA_MODELS_PLOT_DATA_WIDGET_FIELD_H

#include "libbrathl/brathl.h"
#include "libbrathl/Unit.h"
using namespace brathl;

#include "DataModels/PlotData/FieldData.h"

// Base class for all v3 CXXXPlotField hierarchy
//
// This class's interface simulates a single CFieldData
//
//	Works as a single unit for displaying and assigning properties
//	- Several plot data objects (instances of the class) can be displayed at the same time
//	- Properties are displayed and assigned only for the selected plot data unit (in GUI fields/layers list)
//
class CWidgetField : private std::pair< CFieldData*, CFieldData* >
{	
	friend class CAbstractDisplayEditor;	//for diagnostic only

	////////////////////////////
	//			types
	////////////////////////////

	using base_t = std::pair< CFieldData*, CFieldData* >;

	template< typename T, class FIELD_DATA >
	using const_getter_t = T( FIELD_DATA::* )( ) const;

	template< typename T, class FIELD_DATA >
	using getter_t = T( FIELD_DATA::* )( );


	////////////////////////////
	//		instance data
	////////////////////////////

protected:

	std::vector<CUnit*> m_dataUnits;	//not yfx

	////////////////////////////
	// construction /destruction
	////////////////////////////

public:
	//	NOTE: Inverse of CWorldPlotData 
	//	CWorldPlotData used north first; here (*) we follow the order in which field names form the v3 CWorldPlotProperty name
	//
	CWidgetField( CFieldData *north_data, CFieldData *east_data )
		: base_t( east_data, north_data )						//(*)
	{}

	CWidgetField( CFieldData *data )
		: base_t( data, nullptr )
	{}

	virtual ~CWidgetField()
	{}


protected:
	void Clear()
	{
		first = second = nullptr;
	}


public:
	CWidgetField& operator = ( const CWidgetField &o )
	{
		if ( this != &o )
		{
			Clear();					//really not necessary
			base_t::operator = ( o );
		}

		return *this;
	}


	//////////////////////////////
	//		integrity tests
	//////////////////////////////

	bool operator == ( const CFieldData &o ) const
	{
		return *first == o;
	}


	operator bool() const { return first != nullptr; }


	//////////////////////////////
	// protected field management
	//////////////////////////////

protected:

	template< typename T, typename SETTER >
	void Set( const T &value, SETTER set )
	{
		assert__( first );

		( first->*set )( value );
		if ( second )
			( second->*set )( value );
	}

	template< typename T1, typename T2, typename SETTER >
	void Set( const T1 &value1, const T2 &value2, SETTER set )
	{
		assert__( first );

		( first->*set )( value1, value2 );
		if ( second )
			( second->*set )( value1, value2 );
	}

	template< typename T, class FIELD_DATA >
	T Get( const_getter_t< T, FIELD_DATA > get ) const
	{
		assert__( first );

		return ( first->*get )( );
	}

	template< typename T, class FIELD_DATA >
	T Get( getter_t< T, FIELD_DATA > get )
	{
		assert__( first );

		return ( first->*get )( );
	}


	//////////////////////////////
	//		Public Interfaces
	//////////////////////////////

public:
	// This class exists almost only because of this: allowing a single plot field 
	//	composed of 2 data fields to be treated like one unit. So, the identification
	//	is always composed by the 2 field identifiers.  Field names, even in GUI 
	//	structures, are not changeable (there are "user names" for that).

	const std::string& FieldNameEast() const				
	{ 
		assert__( first );

		return first->FieldName(); 
	}

	const std::string& FieldNameNorth() const				
	{ 
		assert__( second );

		return second->FieldName(); 
	}

	//	Only for vector maps is the returned value different from calling FieldName 
	//	of the data fields that this instance represents. (Only in vector maps this
	//	instance represents 2 data fields.) For plots, in particular, it is always 
	//	as calling FieldName of the (single) data field.
	//
	//	From v3, composed like: fieldNameEast + "/" + fieldNameNorth;
	//
	std::string WidgetFieldName() const
	{
		if ( !second )
			return FieldNameEast();

		return FieldNameEast() + "/" + FieldNameNorth();
	}


	const std::string& AxisUserName() const				//used only by plots, which don't have a 2nd field
	{ 
		return first->UserName(); 
	}
	void SetAxisUserName( const std::string &value )	//idem
	{ 
		first->SetUserName( value ); 
	}


	////////////////////////////
	// Class Specific Interface
	////////////////////////////

	bool HasDataUnitInfo() const { return m_dataUnits.size() > 0; }

	virtual const CUnit* DataUnit() const { return DataUnit( 0 ); }		//TODO cannot use CurrentFrame() because YFX did not have m_dataUnits and unlike ZFXY does not have 1 unit per frame

	virtual const CUnit* DataUnit( size_t index ) const
	{ 
		assert__( index < m_dataUnits.size() );

		return m_dataUnits[ index ]; 
	}

	virtual size_t CurrentFrame() const = 0;

	
	////////////////////////////
	// FieldData Interface
	////////////////////////////


	const std::string& XAxis() const 								{ return Get( &CFieldData::GetXAxis ); }
	void SetXAxis( const std::string &dimension )					{ Set( dimension, &CFieldData::SetXAxis ); }
	CStringArray GetAvailableAxesDimensions() const					{ return Get( &CFieldData::GetAvailableAxes ); }


	CFieldData::unsigned_t Opacity() const 							{ return Get( &CFieldData::Opacity ); }
	void SetOpacity( CFieldData::unsigned_t op )					{ Set( op, &CFieldData::SetOpacity ); }


	// YFX / ZFXY

	unsigned int Zticks() const										{ return Get( &CFieldData::Zticks ); }
	void SetZticks( unsigned int value )							{ Set( value, &CFieldData::SetZticks ); }

    unsigned Zdigits() const										{ return Get( &CFieldData::Zdigits ); }
    void SetZdigits( unsigned value )								{ Set( value, &CFieldData::SetZdigits ); }

	bool XLog() const												{ return Get( &CFieldData::XLog ); }
	void SetXLog( bool value )										{ Set( value, &CFieldData::SetXLog ); }

	bool YLog() const												{ return Get( &CFieldData::YLog ); }
	void SetYLog( bool value )										{ Set( value, &CFieldData::SetYLog ); }

	//	ZFXY / LON-LAT

	const std::string& ColorPalette() const							{ return Get( &CFieldData::ColorPalette ); }
	void SetColorPalette( const std::string &value )				{ Set( value, &CFieldData::SetColorPalette ); }
	const CBratLookupTable& ColorTable() const						{ return Get( &CFieldData::ColorTable ); }
	CBratLookupTable* ColorTablePtr()								{ return Get( &CFieldData::ColorTablePtr ); }


	double CurrentMinValue() const									{ return Get( &CFieldData::CurrentMinValue ); }
	void SetCurrentMinValue( const double &value )					{ Set( value, &CFieldData::SetCurrentMinValue ); }
	double CurrentMaxValue() const									{ return Get( &CFieldData::CurrentMaxValue ); }
	void SetCurrentMaxValue( const double &value )					{ Set( value, &CFieldData::SetCurrentMaxValue ); }

	double DataMinValue() const										{ return Get( &CFieldData::DataMinValue ); }
	double DataMaxValue() const										{ return Get( &CFieldData::DataMaxValue ); }
	void SetDataRangeValues( const double &m, const double &M )		{ Set( m, M, &CFieldData::SetDataRangeValues ); }

	bool WithContour() const										{ return Get( &CFieldData::WithContour ); }
	void SetWithContour( bool value )								{ Set( value, &CFieldData::SetWithContour ); }

	CPlotColor ContourLineColor() const								{ return Get( &CFieldData::ContourLineColor ); }
	void SetContourLineColor( CPlotColor value )					{ Set( value, &CFieldData::SetContourLineColor ); }

	double MinContourValue() const									{ return Get( &CFieldData::MinContourValue ); }
	double MaxContourValue() const									{ return Get( &CFieldData::MaxContourValue ); }
	void SetContourValueRange( double m, double M )					{ Set( m, M, &CFieldData::SetContourValueRange ); }

	unsigned NumContours() const									{ return Get( &CFieldData::NumContours ); }
	void SetNumContours( unsigned value )							{ Set( value, &CFieldData::SetNumContours ); }

	std::pair< unsigned, unsigned > ContourPrecision() const		{ return Get( &CFieldData::ContourPrecision ); }
	void SetContourPrecision( unsigned grid1, unsigned grid2 )		{ Set( grid1, grid2, &CFieldData::SetContourPrecision ); }

	double ContourLineWidth() const									{ return Get( &CFieldData::ContourLineWidth ); }
	void SetContourLineWidth( double value )						{ Set( value, &CFieldData::SetContourLineWidth ); }

	bool WithSolidColor() const										{ return Get( &CFieldData::WithSolidColor ); }
	void SetWithSolidColor( bool value )							{ Set( value, &CFieldData::SetWithSolidColor ); }

	//	LON-LAT

	//for plot editor to identify vector components; in a map editor the vector components are first and second of this instance
	bool IsVectorComponent() const									{ return Get( &CFieldData::IsEastComponent ) || Get( &CFieldData::IsNorthComponent ); }

	double MagnitudeFactor() const									{ return Get( &CFieldData::MagnitudeFactor ); }
	void SetMagnitudeFactor( double value )							{ Set( value, &CFieldData::SetMagnitudeFactor ); }

	// YFX

	bool Lines() const												{ return Get( &CFieldData::Lines ); }
	void SetLines( bool value )										{ Set( value, &CFieldData::SetLines ); }

	bool Points() const												{ return Get( &CFieldData::Points ); }
	void SetPoints( bool value )									{ Set( value, &CFieldData::SetPoints ); }

	double PointSize() const										{ return Get( &CFieldData::PointSize ); }
	void SetPointSize( double value )								{ Set( value, &CFieldData::SetPointSize ); }

	EPointGlyph PointGlyph() const									{ return Get( &CFieldData::PointGlyph ); }
	void SetPointGlyph( EPointGlyph value )							{ Set( value, &CFieldData::SetPointGlyph ); }

	bool FilledPoint() const										{ return Get( &CFieldData::FilledPoint ); }
	void SetFilledPoint( bool value )								{ Set( value, &CFieldData::SetFilledPoint ); }

	CPlotColor LineColor() const									{ return Get( &CFieldData::LineColor ); }
	void SetLineColor( const CPlotColor& color )					{ Set( color, &CFieldData::SetLineColor ); }

	CFieldData::unsigned_t LineWidth() const						{ return Get( &CFieldData::LineWidth ); }
	void SetLineWidth( CFieldData::unsigned_t value )				{ Set( value, &CFieldData::SetLineWidth ); }

	EStipplePattern StipplePattern() const							{ return Get( &CFieldData::StipplePattern ); }
	void SetStipplePattern( EStipplePattern e )						{ Set( e, &CFieldData::SetStipplePattern ); }

	const CPlotColor& PointColor() const							{ return Get( &CFieldData::PointColor ); }
	void SetPointColor( const CPlotColor &color )					{ Set( color, &CFieldData::SetPointColor ); }

	unsigned NumberOfBins()	const									{ return Get( &CFieldData::NumberOfBins ); }
	void SetNumberOfBins( unsigned value )							{ Set( value, &CFieldData::SetNumberOfBins ); }


	////////////////////////////
	//	legacy interface
	////////////////////////////

	virtual CFieldData* GetPlotProperties() { return second ? second : first; };	//second because world plots used north as "property"
};




#endif			// DATA_MODELS_PLOT_DATA_WIDGET_FIELD_H
