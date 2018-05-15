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
#ifndef DATA_MODELS_PLOT_DATA_PLOTBASE_H
#define DATA_MODELS_PLOT_DATA_PLOTBASE_H


#include "libbrathl/brathl.h"
#include "libbrathl/Expression.h"
using namespace brathl;

#include "FieldData.h"
#include "XYPlotField.h"
#include "GeoPlotField.h"
#include "ZFXYPlotField.h"


namespace brathl {

	class CInternalFiles;
	class CInternalFilesZFXY;
	class CInternalFilesYFX;
}



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//							 Abstract Display<=>Plot Classes
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


//	Display Interface	<= CDisplayBase <= CDisplay
//	Display Interface	<= CPlotInterface(CDisplay) <= CPlotBase< DATA_FIELD, PLOT_FIELD > <= CMathPlot< DATA_FIELD, PLOT_FIELD > <= CYFXPlot
//	Display Interface	<= CPlotInterface(CDisplay) <= CPlotBase< DATA_FIELD, PLOT_FIELD > <= CMathPlot< DATA_FIELD, PLOT_FIELD > <= CZFXYPlot
//	Display Interface	<= CPlotInterface(CDisplay) <= CPlotBase< DATA_FIELD, PLOT_FIELD > <= CGeoPlot

//	- CDisplay (a workspace module) is the instantiable workspace class of CDisplayInterface
//
//	- CYFXPlot && CZFXYPlot && CGeoPlot are the instantiable plot classes of CDisplayInterface (see
//		"Concrete Plot Classes" section at the last part of this file)


///////////////////////////////////////////////////////////////////////////////////////////
//								Display Interface 
//				- display interface to link workspace and plot classes
///////////////////////////////////////////////////////////////////////////////////////////


class CDisplayInterface : public CBratObject
{
	///////////////////////////
	//			types
	///////////////////////////

	using base_t = CBratObject;


	///////////////////////////
	//		statics
	///////////////////////////


public:
	static std::string MakeUnitLabel( const CUnit &unit );


	///////////////////////////
	//		instance data
	///////////////////////////

	/////////////////////////////////
	//	construction / destruction
	/////////////////////////////////

protected:
	CDisplayInterface()
		: base_t()
	{}

public:
	virtual ~CDisplayInterface()
	{}


	/////////////////////////////////
	//			test
	/////////////////////////////////

	/////////////////////////////////
	//			access
	/////////////////////////////////

public:

	//titles: do not accept empty ones

	virtual const std::string& Title() const = 0;
	virtual void SetTitle( const std::string &title ) = 0;

	virtual const std::string& Xtitle() const = 0;
	virtual const std::string& Ytitle() const = 0;
	virtual const std::string& ValueTitle() const = 0;

	virtual void SetXtitle( const std::string &title ) = 0;
	virtual void SetYtitle( const std::string &title ) = 0;
	virtual void SetValueTitle( const std::string &title ) = 0;


	virtual unsigned int Xticks() const = 0;
	virtual unsigned int Yticks() const = 0;
	virtual unsigned int ValueTicks() const = 0;

	virtual void SetXticks( unsigned int value ) = 0;
	virtual void SetYticks( unsigned int value ) = 0;
	virtual void SetValueTicks( unsigned int value ) = 0;


    virtual unsigned Xdigits() const = 0;
    virtual unsigned Ydigits() const = 0;
    virtual unsigned ValueDigits() const = 0;

    virtual void SetXdigits( unsigned value ) = 0;
    virtual void SetYdigits( unsigned value ) = 0;
    virtual void SetValueDigits( unsigned value ) = 0;

	// Index for z accessors necessary because a single
	//	ZFXY plot structure really supports as much real plots as its data
	//	fields, so when an indexed accessor is used, this means that the
	//	assignment is being made to a data field, not the whole "plot" class,
	//	which in the ZFXY case has no correspondence to a single view (plot).
	//
	virtual std::string ValueTitle( size_t index ) const = 0;
	virtual void SetValueTitle( size_t index, const std::string &title ) = 0;	

	virtual unsigned int ValueTicks( size_t index ) const = 0;
	virtual void SetValueTicks( size_t index, unsigned int value ) = 0;

    virtual unsigned ValueDigits( size_t index ) const = 0;
    virtual void SetValueDigits( size_t index, unsigned value ) = 0;
};





///////////////////////////////////////////////////////////////////////////////////////////
//
//									Display Base Class
//
//			- implementation of CDisplayInterface for workspace display class
//
///////////////////////////////////////////////////////////////////////////////////////////


class CDisplayBase : public CDisplayInterface
{
	///////////////////////////
	//			types
	///////////////////////////

	using base_t = CDisplayInterface;

	friend class CWorkspaceSettings;


	///////////////////////////
	//		statics
	///////////////////////////


	///////////////////////////
	//		instance data
	///////////////////////////

#if defined (BRAT_V3)
public:
#else
private:
#endif

	std::string mTitle;
	std::string mTitleX;
	std::string mTitleY;
	std::string mTitleValue;

	unsigned m_xTicks = CFieldData::smDefaultNumberOfTicks;
	unsigned m_yTicks = CFieldData::smDefaultNumberOfTicks;
    unsigned m_zTicks = CFieldData::smDefaultNumberOfTicks;

    unsigned m_xDigits = CFieldData::smDefaultNumberOfDigits;
    unsigned m_yDigits = CFieldData::smDefaultNumberOfDigits;
    unsigned m_zDigits = CFieldData::smDefaultNumberOfDigits;


	/////////////////////////////////
	//	construction / destruction
	/////////////////////////////////

protected:
	CDisplayBase( const std::string &title )
		: base_t()
		, mTitle( title )
	{}

public:
	CDisplayBase( const CDisplayBase *po )
		: base_t()
	{
		if ( po )
			*this = *po;
	}

	CDisplayBase( const CDisplayBase &o )
		: base_t( o )
	{
		*this = o;
	}

	virtual ~CDisplayBase()
	{}


	CDisplayBase& operator = ( const CDisplayBase &o )
	{
		if ( this != &o )
		{
			mTitle = o.mTitle;
			mTitleX = o.mTitleX;
			mTitleY = o.mTitleY;
			mTitleValue = o.mTitleValue;

			m_xTicks = o.m_xTicks;
			m_yTicks = o.m_yTicks;
			m_zTicks = o.m_zTicks;

			m_xDigits = o.m_xDigits;
			m_yDigits = o.m_yDigits;
			m_zDigits = o.m_zDigits;

		}
		return *this;
	}


	/////////////////////////////////
	//			test
	/////////////////////////////////

	bool operator == ( const CDisplayBase &o ) const
	{
		return
			mTitle == o.mTitle &&
			mTitleX == o.mTitleX &&
			mTitleY == o.mTitleY &&
			mTitleValue == o.mTitleValue &&

			m_xTicks == o.m_xTicks &&
			m_yTicks == o.m_yTicks &&
			m_zTicks == o.m_zTicks &&

			m_xDigits == o.m_xDigits &&
			m_yDigits == o.m_yDigits &&
			m_zDigits == o.m_zDigits &&

			true;
	}


	/////////////////////////////////
	//			access
	/////////////////////////////////

public:

	//titles: do not accept empty ones

	virtual const std::string& Title() const override { return mTitle; }
	virtual void SetTitle( const std::string &title ) override 
	{ 
		if ( !title.empty() )
			mTitle = title; 
	}

	virtual const std::string& Xtitle() const override { return mTitleX; }
	virtual void SetXtitle( const std::string &title ) override 
	{ 
		if ( !title.empty() )
			mTitleX = title; 
	}

	virtual const std::string& Ytitle() const override { return mTitleY; }
	virtual void SetYtitle( const std::string &title ) override 
	{ 
		if ( !title.empty() )
			mTitleY = title; 
	}

	virtual const std::string& ValueTitle() const override 	{ return mTitleValue; }
	virtual void SetValueTitle( const std::string &title ) override 
	{ 
		if ( !title.empty() )
			mTitleValue = title; 
	}

	virtual unsigned int Xticks() const override { return m_xTicks; }
	virtual unsigned int Yticks() const override { return m_yTicks; }
	virtual unsigned int ValueTicks() const override { return m_zTicks; }
	virtual void SetXticks( unsigned int value ) override { m_xTicks = value; }
	virtual void SetYticks( unsigned int value ) override { m_yTicks = value; }
	virtual void SetValueTicks( unsigned int value ) override { m_zTicks = value; }

    virtual unsigned Xdigits() const override { return m_xDigits; }
    virtual unsigned Ydigits() const override { return m_yDigits; }
    virtual unsigned ValueDigits() const override { return m_zDigits; }
    virtual void SetXdigits( unsigned value ) override { m_xDigits = value; }
    virtual void SetYdigits( unsigned value ) override { m_yDigits = value; }
    virtual void SetValueDigits( unsigned value ) override { m_zDigits = value; }


	virtual std::string ValueTitle( size_t index ) const override 
	{ 
		UNUSED( index );

		return ValueTitle();
	}
	virtual void SetValueTitle( size_t index, const std::string &title ) override 
	{ 
		UNUSED( index );

		SetValueTitle( title );
	}
	

	virtual unsigned int ValueTicks( size_t index ) const override 
	{ 
		UNUSED( index );

		return ValueTicks();
	}
	virtual void SetValueTicks( size_t index, unsigned int value ) override 
	{ 
		UNUSED( index );

		SetValueTicks( value );
	}


    virtual unsigned ValueDigits( size_t index ) const override 
	{ 
		UNUSED( index );

		return ValueDigits();
	}
    virtual void SetValueDigits( size_t index, unsigned value ) override 
	{ 
		UNUSED( index );

		SetValueDigits( value );
	}
};





///////////////////////////////////////////////////////////////////////////////////////////
//									Plot Multi-Interface
//
//							- interface for plot structure builders
//							- interface for plot structure editors
//			- implementation of CDisplayInterface for plot structure classes, delegating
//							to an embedded workspace display class 
//
///////////////////////////////////////////////////////////////////////////////////////////

#if defined(BRAT_V3)
using display_interface_t = CDisplayBase;
#else
using display_interface_t = CDisplayInterface;
#endif



class CPlotInterfaces : public display_interface_t
{
	///////////////////////////
	//			types
	///////////////////////////

	using base_t = display_interface_t;


	///////////////////////////
	//		instance data
	///////////////////////////

#if defined (BRAT_V3)
public:
#else
protected:

	CDisplayInterface &mDI;
#endif

	uint32_t m_groupNumber;


	/////////////////////////////////
	//	construction / destruction
	/////////////////////////////////

public:
	CPlotInterfaces( CDisplayInterface *i, uint32_t groupNumber = 0 )

#if defined(BRAT_V3)
		: base_t( nullptr )
#else
		: base_t()
		, mDI( *i )
#endif
		, m_groupNumber( groupNumber )
	{}

	
	CPlotInterfaces( const CPlotInterfaces &o ) = delete;
	CPlotInterfaces& operator = ( const CPlotInterfaces &o ) = delete;

	
	virtual ~CPlotInterfaces()
	{}



	///////////////////////////////////////////
	//	implementation of CDisplayInterface
	///////////////////////////////////////////

#if !defined(BRAT_V3)

	virtual const std::string& Title() const override { return mDI.Title(); }
	virtual void SetTitle( const std::string &title ) override { mDI.SetTitle( title ); }

	virtual const std::string& Xtitle() const override { return mDI.Xtitle(); }
	virtual void SetXtitle( const std::string &title ) override { mDI.SetXtitle( title ); }

	virtual const std::string& Ytitle() const override { return mDI.Ytitle(); }
	virtual void SetYtitle( const std::string &title ) override { mDI.SetYtitle( title ); }

	virtual const std::string& ValueTitle() const override { return mDI.ValueTitle(); }
	virtual void SetValueTitle( const std::string &title ) override { mDI.SetValueTitle( title ); }

	virtual unsigned int Xticks() const override { return mDI.Xticks(); }
	virtual unsigned int Yticks() const override { return mDI.Yticks(); }
	virtual unsigned int ValueTicks() const override { return mDI.ValueTicks(); }
	virtual void SetXticks( unsigned int value ) override { mDI.SetXticks( value ); }
	virtual void SetYticks( unsigned int value ) override { mDI.SetYticks( value ); }
	virtual void SetValueTicks( unsigned int value ) override { mDI.SetValueTicks( value ); }

    virtual unsigned Xdigits() const override { return mDI.Xdigits(); }
    virtual unsigned Ydigits() const override { return mDI.Ydigits(); }
    virtual unsigned ValueDigits() const override { return mDI.ValueDigits(); }
    virtual void SetXdigits( unsigned value ) override { mDI.SetXdigits( value ); }
    virtual void SetYdigits( unsigned value ) override { mDI.SetYdigits( value ); }
    virtual void SetValueDigits( unsigned value ) override { mDI.SetValueDigits( value ); }

	virtual std::string ValueTitle( size_t index ) const override  { return mDI.ValueTitle( index ); }
	virtual void SetValueTitle( size_t index, const std::string &title ) override { mDI.SetValueTitle( index, title ); }

    virtual unsigned int ValueTicks( size_t index ) const override { return mDI.ValueTicks( index ); }
    virtual void SetValueTicks( size_t index, unsigned int value ) override { mDI.SetValueTicks( index, value ); }

    virtual unsigned ValueDigits( size_t index ) const override { return mDI.ValueDigits( index ); }
    virtual void SetValueDigits( size_t index, unsigned value ) override { mDI.SetValueDigits( index, value );	}

#endif


	/////////////////////////////////
	//	interface for plot builders
	/////////////////////////////////

	virtual void GetInfo() = 0;


	/////////////////////////////////
	//	interface for plot editors
	/////////////////////////////////

public:

	virtual bool Empty() const = 0;

	virtual size_t Size() const = 0;

	virtual CWidgetField* PlotData( size_t index ) = 0;


#if defined(BRAT_V3)

	virtual std::string MakeTitle()
	{
		std::string titleTmp = mTitle;
		titleTmp = CTools::SlashesDecode( titleTmp );

		replace( titleTmp, "\n", "-" );
		replace( titleTmp, "\t", " " );

		return BaseTitle() + " - " + titleTmp + n2s< std::string >( m_groupNumber );
	}

protected:
	virtual const std::string& BaseTitle() const = 0;

#endif

};





///////////////////////////////////////////////////////////////////////////////////////////
//								Generic for All Plot Classes
//
//						- implements CPlotInterface interface for all plot classes
//
///////////////////////////////////////////////////////////////////////////////////////////


template< typename DATA_FIELD, typename PLOT_FIELD >
class CPlotBase : public CPlotInterfaces
{
	////////////////////////////////
	//			types
	////////////////////////////////

	using base_t = CPlotInterfaces;


	////////////////////////////////
	//			statics
	////////////////////////////////

protected:
	static void GetVar( const std::string& varName, CInternalFiles* file, ExpressionValueDimensions* dimVal, CExpressionValue* var );	


	////////////////////////////////
	//		instance data
	////////////////////////////////

#if defined(BRAT_V3)
public:

    using base_t::mTitle;
    using base_t::m_groupNumber;

#endif

	std::vector< DATA_FIELD* > mDataFields;	//ex-CDisplayData/CXXXProperties, represents a data field display data
	std::vector< PLOT_FIELD* > mPlotFields;	//ex-CXXPlotData, represents a plot field: can assemble 2 data fields (vector plots) and results from GetInfo (plot building)


//protected:
//	bool m_unitXConv = false;		//v3 assigned never used
//	bool m_unitYConv = false;		//idem


	////////////////////////////////
	//	construction / destruction
	////////////////////////////////

public:
	CPlotBase( CDisplayInterface *i, uint32_t groupNumber = 0 )
		: base_t( i, groupNumber )
	{}

	virtual ~CPlotBase()
	{}


	/////////////////////////////////
	//	interface for plot editors
	/////////////////////////////////

	virtual bool Empty() const override 
	{ 
		return mPlotFields.empty();
	}

	
	virtual size_t Size() const override 
	{ 
		return mPlotFields.size();
	}


	virtual PLOT_FIELD* PlotData( size_t index ) override
	{
		assert__( index < mPlotFields.size() );

		return mPlotFields[ index ];
	}



	////////////////////////////////
	//		remaining
	////////////////////////////////

protected:
	void GetAllInternalFiles( std::vector< CInternalFiles*>& allInternalFiles ) const;

public:

	DATA_FIELD* FieldData( size_t index )
	{
		assert__( index < mDataFields.size() );

		return mDataFields[ index ];
	}

	void PushFieldData( DATA_FIELD *field_data )
	{
		mDataFields.push_back( field_data );
	}


	DATA_FIELD* FindFieldData( const std::string &fieldName, bool *withContour = nullptr, bool *withSolidColor = nullptr )
	{
		for ( auto *plotField : mDataFields )
		{
            if ( plotField->FieldName() == fieldName )
			{
				if ( ( !withContour || *withContour == plotField->WithContour() ) &&
					( !withSolidColor || *withSolidColor == plotField->WithSolidColor() ) )
				{
					return plotField;
				}
			}
		}

		return nullptr;
	}
};



///////////////////////////////////////////////////////////////////////////////////////////
//							Generic for Math (non-map) Plot Types
///////////////////////////////////////////////////////////////////////////////////////////

template< typename DATA_FIELD, typename PLOT_FIELD >
class CMathPlot : public CPlotBase< DATA_FIELD, PLOT_FIELD >
{
	//////////////////////
	//		types
	//////////////////////


	using base_t = CPlotBase< DATA_FIELD, PLOT_FIELD >;

protected:
    using base_t::mPlotFields;

public:
    using base_t::Xtitle;
    using base_t::Ytitle;
    using base_t::SetXtitle;
    using base_t::SetYtitle;
    using base_t::MakeUnitLabel;

private:

	//////////////////////
	// instance data
	//////////////////////


//for linux - begin
//
#if defined (BRAT_V3)
public:
    using base_t::mTitle;
#endif

    using base_t::m_groupNumber;
//
//for linux - end


	std::string m_unitXLabel;
	std::string m_unitYLabel;	//private in v4


protected:
	std::string m_forcedVarXName;
	std::string m_forcedVarYName;

	CUnit m_unitX;
	CUnit m_unitY;

	///////////////////////////////
	// construction / destruction
	///////////////////////////////

public:
	CMathPlot( CDisplayInterface *i, uint32_t groupNumber = 0 )
		: base_t( i, groupNumber )
	{}
	virtual ~CMathPlot()
	{}


	///////////////////////////////
	//		remaining
	///////////////////////////////


	//testers

	virtual bool XLogarithmsEnabled() const
	{
		for ( auto const *pdata : mPlotFields )
		{
			double m, M;
			pdata->GetXRange( m, M );
			if ( m <= 0 )
				return false;
		}
		return true;
	}
	virtual bool YLogarithmsEnabled() const
	{
		for ( auto const *pdata : mPlotFields )
		{
			double m, M;
			pdata->GetYRange( m, M );
			if ( m <= 0 )
				return false;
		}
		return true;
	}
	virtual bool ZLogarithmsEnabled() const
	{
		for ( auto const *pdata : mPlotFields )
		{
			double m, M;
			pdata->GetDataRange( m, M );
			if ( m <= 0 )
				return false;
		}
		return true;
	}


	//getters - non const for plot data builders (Create functions) to modify...

	CUnit& XUnit() { return m_unitX; }

	CUnit& YUnit() { return m_unitY; }

	//setters, used by netcdf reader (and other) functions of display files processor...

	void SetForcedVarXname( const std::string& value ) { m_forcedVarXName = value; }
	void SetForcedVarYname( const std::string& value ) { m_forcedVarYName = value; }

	//other

protected:
	void GetTitleAndUnitInfo( CInternalFiles *file, const std::string &varXName, std::string varYName, const std::string &fieldName, bool useYnameInLabel );

};





///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//
//								Concrete Plot Classes
//
//			- all implement the interface for plot builders of CPlotInterfaces 
//
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////
//								Geo Plot Type (Lon/Lat)
///////////////////////////////////////////////////////////////////////////////////////////


class CGeoPlot : public CPlotBase< CWorldPlotProperties, CGeoPlotField >
{
	//////////////////////
	//		types
	//////////////////////

	using base_t = CPlotBase< CWorldPlotProperties, CGeoPlotField >;


	////////////////////////////////////////////
	//		construction / destruction
	////////////////////////////////////////////

public:
	CGeoPlot( CDisplayInterface *i, uint32_t groupNumber = 0 )
		: base_t( i, groupNumber )
	{}

	virtual ~CGeoPlot()
	{}


	/////////////////////////////////
	//	interface for plot builders
	/////////////////////////////////

	virtual void GetInfo() override;

#if defined(BRAT_V3)
protected:

	virtual const std::string& BaseTitle() const override
	{
		static const std::string s = "BRAT World Plot";
		return s;
	}
#endif
};




///////////////////////////////////////////////////////////////////////////////////////////
//									YFX Plot Type
///////////////////////////////////////////////////////////////////////////////////////////


class CYFXPlot : public CMathPlot< CXYPlotProperties, CXYPlotField >
{
	//////////////////////
	//		types
	//////////////////////

	using base_t = CMathPlot< CXYPlotProperties, CXYPlotField >;


    //////////////////////
	//	instance data
	//////////////////////


#if defined(BRAT_V3)
	int32_t m_currentFrame = 0;
	bool m_zoomToDataRange = false;
#endif


public:
	CStringArray m_nonPlotFieldNames;


	////////////////////////////////////////////
	//		construction / destruction
	////////////////////////////////////////////
public:
	CYFXPlot( CDisplayInterface *i, uint32_t groupNumber = 0 )
		: base_t( i, groupNumber )
	{}
	virtual ~CYFXPlot()
	{}


	//////////////////////
	//	Display Interface 
	//////////////////////

private:
	// prevent use of axis field counterparts: for YFX, all fields are in the same plot widget

	virtual std::string ValueTitle( size_t index ) const override 
	{ 
		UNUSED( index );
		throw; 
	}
	virtual void SetValueTitle(  size_t index, const std::string &title ) override
	{ 
		UNUSED( index );		UNUSED( title );
		throw; 
	}

	virtual unsigned int ValueTicks( size_t index ) const override 
	{ 
		UNUSED( index );
		throw; 
	}
	virtual void SetValueTicks( size_t index, unsigned int value ) override 
	{ 
		UNUSED( index );		UNUSED( value );
		throw; 
	}

    virtual unsigned ValueDigits( size_t index ) const override 
	{ 
		UNUSED( index );
		throw; 
	}
    virtual void SetValueDigits( size_t index, unsigned value ) override
	{ 
		UNUSED( index );		UNUSED( value );
		throw; 
	}

public:
	// force use of plot field counterparts: for YFX, all fields are in the same plot widget

	virtual const std::string& ValueTitle() const override { return Ytitle(); }
	virtual void SetValueTitle( const std::string &title ) override { SetYtitle( title ); }

	virtual unsigned int ValueTicks() const override { return Yticks(); }
	virtual void SetValueTicks( unsigned int value ) override { SetYticks( value ); }

    virtual unsigned ValueDigits() const override { return Ydigits(); }
    virtual void SetValueDigits( unsigned value ) override { SetYdigits( value ); }


	/////////////////////////////////
	//	interface for plot builders
	/////////////////////////////////

	virtual void GetInfo() override;

public:

	//////////////////////
	//		Specific
	//////////////////////


	void GetAxisX( CInternalFiles* yfx, ExpressionValueDimensions* dimVal, CExpressionValue* varX, std::string* varXName ) const;


#if defined(BRAT_V3)
	void AddData( CXYPlotField *pdata );

    int32_t GetCurrentFrame() { return m_currentFrame; }
    void SetCurrentFrame( int32_t value ) { m_currentFrame = value; }

    bool IsZoomToDataRange() { return m_zoomToDataRange; }
    void SetZoomToDataRange( bool value ) { m_zoomToDataRange = value; }

	void OnFrameChange( int32_t f );

	VTK_CXYPlotData* Get( std::vector< CXYPlotField* >::iterator it );
	VTK_CXYPlotData* Get( int32_t index );

	bool ShowPropertyMenu();

	void GetNames( std::vector<std::string>& names );

	CXYPlotProperties* GetPlotProperties(int32_t index);

	void GetXRange( double& min, double& max );
	void GetYRange( double& min, double& max );

	void GetXRange( double& min, double& max, uint32_t frame );
	void GetYRange( double& min, double& max, uint32_t frame );

protected:

	virtual const std::string& BaseTitle() const override
	{
		static const std::string s = "BRAT Y=F(X) Plot";
		return s;
	}
#endif
};





///////////////////////////////////////////////////////////////////////////////////////////
//									ZFXY Plot Type
///////////////////////////////////////////////////////////////////////////////////////////


class CZFXYPlot : public CMathPlot< CZFXYPlotProperties, CZFXYPlotField >
{
	//////////////////////
	//		types
	//////////////////////

	using base_t = CMathPlot< CZFXYPlotProperties, CZFXYPlotField >;

public:
    using base_t::ValueTitle;
    using base_t::SetValueTitle;

private:
	//////////////////////
	//	instance data
	//////////////////////


	////////////////////////////////////////////
	//		construction / destruction
	////////////////////////////////////////////

public:
	CZFXYPlot( CDisplayInterface *i, uint32_t groupNumber = 0 )
		: base_t( i, groupNumber )
	{}
	virtual ~CZFXYPlot()
	{}


	//////////////////////
	//	Display Interface 
	//////////////////////

private:
	// prevent use of plot counterparts of axis functions: for ZFXY, each field has its own plot widget

	virtual const std::string& ValueTitle() const override 
	{ 
		return base_t::ValueTitle(); 
	}
	virtual void SetValueTitle( const std::string &title ) override
	{ 
		base_t::SetValueTitle( title ); 
	}
	virtual unsigned int ValueTicks() const override 
	{ 
		return base_t::ValueTicks(); 
	}
	virtual void SetValueTicks( unsigned int value ) override 
	{ 
		base_t::SetValueTicks( value ); 
	}
    virtual unsigned ValueDigits() const override 
	{ 
		return base_t::ValueDigits(); 
	}
    virtual void SetValueDigits( unsigned value ) override
	{ 
		base_t::SetValueDigits( value ); 
	}

public:
	// force use of axis field function counterparts: for ZFXY, each field has its own plot widget

	virtual std::string ValueTitle( size_t index ) const override
	{ 
		assert__( index < mPlotFields.size() );

		return mPlotFields[ index ]->AxisUserName(); 
	}

	virtual void SetValueTitle( size_t index, const std::string &title ) override 
	{ 
		assert__( index < mPlotFields.size() );

		mPlotFields[ index ]->SetAxisUserName( title ); 
	}
	

	virtual unsigned int ValueTicks( size_t index ) const override 
	{ 
		assert__( index < mPlotFields.size() );

		return mPlotFields[ index ]->Zticks(); 
	}
	
	virtual void SetValueTicks( size_t index, unsigned int value ) override 
	{ 
		assert__( index < mPlotFields.size() );

		mPlotFields[ index ]->SetZticks( value ); 
	}


    virtual unsigned ValueDigits( size_t index ) const override 
	{ 
		assert__( index < mPlotFields.size() );

		return mPlotFields[ index ]->Zdigits(); 
	}
    
	virtual void SetValueDigits( size_t index, unsigned value ) override
	{ 
		assert__( index < mPlotFields.size() );

		mPlotFields[ index ]->SetZdigits( value ); 
	}


	/////////////////////////////////
	//	interface for plot builders
	/////////////////////////////////

	virtual void GetInfo() override;


	//////////////////////
	//		Specific
	//////////////////////

public:

	void GetPlotWidthHeight( CInternalFiles* zfxy, const std::string& fieldName, int32_t& width, int32_t& height,
		CExpressionValue& varY, CExpressionValue& varX,
		uint32_t& dimRangeX, uint32_t& dimRangeY,
		std::string& varXName, std::string& varYName );


#if defined(BRAT_V3)
protected:
	virtual const std::string& BaseTitle() const override
	{
		static const std::string s = "BRAT Z=F(X,Y) Plot";
		return s;
	}
#endif

};





#endif		//DATA_MODELS_PLOT_DATA_PLOTBASE_H
