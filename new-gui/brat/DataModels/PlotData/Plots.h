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
#include "XYPlotData.h"
#include "WorldPlotData.h"
#include "ZFXYPlotData.h"


namespace brathl {

	class CInternalFiles;
	class CInternalFilesZFXY;
	class CInternalFilesYFX;
}



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//							 Abstract Plot Classes
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////
//				Display Interface - for workspace displays & plot editors
//						- interface to global plot properties
///////////////////////////////////////////////////////////////////////////////////////////


class CDisplayInterface : public CBratObject
{
	///////////////////////////
	//			types
	///////////////////////////

	using base_t = CBratObject;


	///////////////////////////
	//		static data
	///////////////////////////

public:
	static const unsigned smDefaultNumberOfTicks;
	static const unsigned smDefaultNumberOfDigits;


	///////////////////////////
	//		instance data
	///////////////////////////

#if defined (BRAT_V3)
public:
#else
protected:
#endif


	std::string mTitle;
	std::string mTitleX;
	std::string mTitleY;

	unsigned m_xTicks = smDefaultNumberOfTicks;
	unsigned m_yTicks = smDefaultNumberOfTicks;
    unsigned m_zTicks = smDefaultNumberOfTicks;

    unsigned m_xDigits = smDefaultNumberOfDigits;
    unsigned m_yDigits = smDefaultNumberOfDigits;
    unsigned m_zDigits = smDefaultNumberOfDigits;


	/////////////////////////////////
	//	construction / destruction
	/////////////////////////////////

protected:
	CDisplayInterface( const std::string &title )
		: base_t()
		, mTitle( title )
	{}

public:
	CDisplayInterface( const CDisplayInterface *po )
		: base_t()
	{
		if ( po )
			*this = *po;
	}

	CDisplayInterface( const CDisplayInterface &o )
		: base_t( o )
	{
		*this = o;
	}

	virtual ~CDisplayInterface()
	{}


	CDisplayInterface& operator = ( const CDisplayInterface &o )
	{
		if ( this != &o )
		{
			mTitle = o.mTitle;
			mTitleX = o.mTitleX;
			mTitleY = o.mTitleY;

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

	bool operator == ( const CDisplayInterface &o ) const
	{
		return
			mTitle == o.mTitle &&
			mTitleX == o.mTitleX &&
			mTitleY == o.mTitleY &&

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

	const std::string& Title() const { return mTitle; }
	void SetTitle( const std::string &title ) 
	{ 
		if ( !title.empty() )
			mTitle = title; 
	}

	const std::string& TitleX() const { return mTitleX; }
	void SetTitleX( const std::string &title ) 
	{ 
		if ( !title.empty() )
			mTitleX = title; 
	}

	const std::string& TitleY() const { return mTitleY; }
	void SetTitleY( const std::string &title ) 
	{ 
		if ( !title.empty() )
			mTitleY = title; 
	}

	unsigned int Xticks() const { return m_xTicks; }
	unsigned int Yticks() const { return m_yTicks; }
	unsigned int Zticks() const { return m_zTicks; }
	void SetXticks( unsigned int value ) { m_xTicks = value; }
	void SetYticks( unsigned int value ) { m_yTicks = value; }
	void SetZticks( unsigned int value ) { m_zTicks = value; }

    unsigned Xdigits() const { return m_xDigits; }
    unsigned Ydigits() const { return m_yDigits; }
    unsigned Zdigits() const { return m_zDigits; }
    void SetXdigits( unsigned value ) { m_xDigits = value; }
    void SetYdigits( unsigned value ) { m_yDigits = value; }
    void SetZdigits( unsigned value ) { m_zDigits = value; }

};





///////////////////////////////////////////////////////////////////////////////////////////
//				(Non-generic) Plot Interface - for plot builders & plot editors
//							- interface for access to plot fields
///////////////////////////////////////////////////////////////////////////////////////////

class CPlotInterface : public CDisplayInterface
{
	///////////////////////////
	//			types
	///////////////////////////

	using base_t = CDisplayInterface;


	///////////////////////////
	//		instance data
	///////////////////////////

#if defined (BRAT_V3)
public:
#else
protected:
#endif

	uint32_t m_groupNumber;


	/////////////////////////////////
	//	construction / destruction
	/////////////////////////////////

public:
	CPlotInterface( const CDisplayInterface *po, uint32_t groupNumber = 0 )
		: base_t( po )
		, m_groupNumber( groupNumber )
	{}

	
	CPlotInterface( const CPlotInterface &o )
		: base_t( o )
		, m_groupNumber( o.m_groupNumber )
	{}

	
	virtual ~CPlotInterface()
	{}



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

	virtual CPlotData* PlotData( size_t index ) = 0;


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
//							Generic for All Plot Types
///////////////////////////////////////////////////////////////////////////////////////////


template< typename DATA_FIELD, typename PLOT_FIELD >
class CPlotBase : public CPlotInterface
{
	////////////////////////////////
	//			types
	////////////////////////////////

	using base_t = CPlotInterface;


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
	std::vector< PLOT_FIELD* > mPlotFields;	//ex-CXXPlotData, represents a plot field: can assemble 2 data fields (vector pots) and results from GetInfo (plot building)


//protected:
//	bool m_unitXConv = false;		//v3 assigned never used
//	bool m_unitYConv = false;		//idem


	////////////////////////////////
	//	construction / destruction
	////////////////////////////////

public:
	CPlotBase( const CDisplayInterface *po, uint32_t groupNumber = 0 )
		: base_t( po, groupNumber )
	{}

	virtual ~CPlotBase()
	{}


	////////////////////////////////
	//	Common Plot Interface
	////////////////////////////////

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
    using base_t::mTitleX;
    using base_t::mTitleY;


	//////////////////////
	// instance data
	//////////////////////


//for linux - begin
//
#if defined (BRAT_V3)
public:
#endif

    using base_t::mTitle;
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
	CMathPlot( const CDisplayInterface *po, uint32_t groupNumber = 0 )
		: base_t( po, groupNumber )
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
//								Concrete Plot Classes
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////
//								Geo Plot Type (Lon/Lat)
///////////////////////////////////////////////////////////////////////////////////////////


class CGeoPlot : public CPlotBase< CWorldPlotProperties, CWorldPlotData >
{
	//////////////////////
	//		types
	//////////////////////

	using base_t = CPlotBase< CWorldPlotProperties, CWorldPlotData >;


	////////////////////////////////////////////
	//		construction / destruction
	////////////////////////////////////////////

public:
	CGeoPlot( const CDisplayInterface *po, uint32_t groupNumber = 0 )
		: base_t( po, groupNumber )
	{}

	virtual ~CGeoPlot()
	{}


	//////////////////////
	//	Plot Interface 
	//////////////////////


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


class CYFXPlot : public CMathPlot< CXYPlotProperties, CXYPlotData >
{
	//////////////////////
	//		types
	//////////////////////

	using base_t = CMathPlot< CXYPlotProperties, CXYPlotData >;


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
	CYFXPlot( const CDisplayInterface *po, uint32_t groupNumber = 0 )
		: base_t( po, groupNumber )
	{}
	virtual ~CYFXPlot()
	{}


	//////////////////////
	//	Plot Interface 
	//////////////////////

	virtual void GetInfo() override;

public:

	//////////////////////
	//		Specific
	//////////////////////


	void GetAxisX( CInternalFiles* yfx, ExpressionValueDimensions* dimVal, CExpressionValue* varX, std::string* varXName ) const;


#if defined(BRAT_V3)
	void AddData( CXYPlotData *pdata );

    int32_t GetCurrentFrame() { return m_currentFrame; }
    void SetCurrentFrame( int32_t value ) { m_currentFrame = value; }

    bool IsZoomToDataRange() { return m_zoomToDataRange; }
    void SetZoomToDataRange( bool value ) { m_zoomToDataRange = value; }

	void OnFrameChange( int32_t f );

	VTK_CXYPlotData* Get( std::vector< CXYPlotData* >::iterator it );
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


class CZFXYPlot : public CMathPlot< CZFXYPlotProperties, CZFXYPlotData >
{
	//////////////////////
	//		types
	//////////////////////

	using base_t = CMathPlot< CZFXYPlotProperties, CZFXYPlotData >;


	//////////////////////
	//	instance data
	//////////////////////


	////////////////////////////////////////////
	//		construction / destruction
	////////////////////////////////////////////

public:
	CZFXYPlot( const CDisplayInterface *po, uint32_t groupNumber = 0 )
		: base_t( po, groupNumber )
	{}
	virtual ~CZFXYPlot()
	{}


	//////////////////////
	//	Plot Interface 
	//////////////////////

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
