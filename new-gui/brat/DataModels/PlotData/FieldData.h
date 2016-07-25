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
#ifndef DATA_MODELS_PLOT_DATA_FIELDDATA_H
#define DATA_MODELS_PLOT_DATA_FIELDDATA_H

#include <algorithm>

#include "libbrathl/BratObject.h"

using namespace brathl;

#include "DataModels/PlotData/MapColor.h"
#include "DataModels/PlotData/BratLookupTable.h"

class CPlotData;
class CXYPlotData;

namespace brathl {

	class CInternalFiles;
	class CInternalFilesYFX;
}




class CFieldData : public CBratObject
{
	// types

	using base_t = CBratObject;

	friend class CWorkspaceSettings;

public:

	using unsigned_t =

#if defined(BRAT_V3)
		double;
#else
		unsigned;
#endif

	// statics

#if defined(BRAT_V3)
	static const unsigned smDefaultNumberOfTicks;
	static const unsigned smDefaultNumberOfDigits;
#endif
	static const unsigned_t smDefaultOpacity;
	static const bool smDefaultShowLines;
	static const bool smDefaultShowPoints;
	static const double smDefaultPointSize;
	static const EPointGlyph smDefaultGlyphType;
	static const bool smDefaultFilledPoint;
	static const unsigned_t smDefaultLineWidth;
	static const EStipplePattern smDefaultStipplePattern;
	static const CPlotColor smDefaultPointColor;
	static const int smDefaultNumColorLabels;
	static const bool smDefaultWithContour;
	static const unsigned smDefaultNumContour;
	static const unsigned smContourPrecisionGrid1;
	static const unsigned smContourPrecisionGrid2;
	static const double smDefaultContourLineWidth;
	static const CPlotColor smDefaultContourLineColor;
    static const unsigned smDefaultNumberOfBins;


private:

	// instance data

#if defined (BRAT_V3)
	std::string mTitle;
#endif

	mutable std::vector< CInternalFiles* > mInternalFiles;		//TODO serialize ???


	/////////////////////////
	// YFX / ZFXY / LON-LAT
	/////////////////////////

	std::string mFieldName;
	std::string mUserName;

	unsigned_t mOpacity = smDefaultOpacity;	//v4: so far used only in YFX


	/////////////////////////
	//		YFX / ZFXY
	/////////////////////////

#if defined (BRAT_V3)
	double m_xMax = defaultValue< double >();
	double m_yMax = defaultValue< double >();
	double m_xMin = defaultValue< double >();
	double m_yMin = defaultValue< double >();

	std::string mXlabel;
	std::string mYlabel;

	unsigned m_xTicks = smDefaultNumberOfTicks;
	unsigned m_yTicks = smDefaultNumberOfTicks;
    unsigned m_zTicks = smDefaultNumberOfTicks;

    unsigned m_xDigits = smDefaultNumberOfDigits;	//v4 (...)
    unsigned m_yDigits = smDefaultNumberOfDigits;	//v4 (...)
    unsigned m_zDigits = smDefaultNumberOfDigits;	//v4 (...)
#endif

	bool mXlogarithmic = false;
	bool mYlogarithmic = false;


	/////////////////////////
	//	ZFXY / LON-LAT
	/////////////////////////

	CBratLookupTable mLUT;
	
	double mMinHeightValue = defaultValue< double >();
	double mMaxHeightValue = defaultValue< double >();

	bool mWithContour = false;
	unsigned mNumContours = smDefaultNumContour;
	unsigned mContourPrecisionGrid1 = smContourPrecisionGrid1;
	unsigned mContourPrecisionGrid2 = smContourPrecisionGrid2;
	double mContourLineWidth = smDefaultContourLineWidth;
	CPlotColor mContourLineColor = smDefaultContourLineColor;
	double mMinContourValue = defaultValue<double>();
	double mMaxContourValue = defaultValue<double>();

	bool mWithSolidColor = true;


	/////////////////////////
	//		LON-LAT
	/////////////////////////

	//std::string mProjection;
	bool mEastComponent = false;
	bool mNorthComponent = false;
	double mMagnitudeFactor = defaultValue< double >();


	/////////////////////////
	//			YFX
	/////////////////////////

	bool mLines = smDefaultShowLines;
	bool mPoints = smDefaultShowPoints;
	double mPointSize = smDefaultPointSize;
	EPointGlyph mPointGlyph = smDefaultGlyphType;
	bool mFilledPoint = smDefaultFilledPoint;

	CPlotColor mLineColor = CMapColor::GetInstance().NextPrimaryColors();
	unsigned_t mLineWidth = smDefaultLineWidth;
	EStipplePattern mStipplePattern = smDefaultStipplePattern;
	CPlotColor mPointColor = smDefaultPointColor;

	unsigned mNumberOfBins = smDefaultNumberOfBins;


	///////////////////////////////
	// construction / destruction
	///////////////////////////////

public:

	CFieldData();

	CFieldData( const CFieldData &o )
	{
		*this = o;
	}

	virtual ~CFieldData()
	{}

	///////////////////////////////
	//	assignment / identity
	///////////////////////////////

	const CFieldData& operator= ( const CFieldData& o )
	{
		if ( this != &o )
		{
			mInternalFiles = o.mInternalFiles;

#if defined (BRAT_V3)
			mTitle = o.mTitle;
			m_xMax = o.m_xMax;
			m_yMax = o.m_yMax;
			m_xMin = o.m_xMin;
			m_yMin = o.m_yMin;

			mXlabel = o.mXlabel;
			mYlabel = o.mYlabel;

			m_xTicks = o.m_xTicks;
			m_yTicks = o.m_yTicks;
			m_zTicks = o.m_zTicks;

			m_xDigits = o.m_xDigits;
			m_yDigits = o.m_yDigits;
			m_zDigits = o.m_zDigits;
#endif

			// YFX / ZFXY / LON-LAT

			mFieldName = o.mFieldName;
			mUserName = o.mUserName;

			mOpacity = o.mOpacity;

			// YFX / ZFXY

			mXlogarithmic = o.mXlogarithmic;
			mYlogarithmic = o.mYlogarithmic;

			//	ZFXY / LON-LAT

			mLUT = o.mLUT;
			mMinHeightValue = o.mMinHeightValue;
			mMaxHeightValue = o.mMaxHeightValue;
			//mCurrentMinValue = o.mCurrentMinValue;
			//mCurrentMaxValue = o.mCurrentMaxValue;

			mWithContour = o.mWithContour;
			mNumContours = o.mNumContours;
			mContourPrecisionGrid1 = o.mContourPrecisionGrid1;
            mContourPrecisionGrid2 = o.mContourPrecisionGrid2;
			mContourLineWidth = o.mContourLineWidth;
			mContourLineColor = o.mContourLineColor;
			mMinContourValue = o.mMinContourValue;
			mMaxContourValue = o.mMaxContourValue;

			mWithSolidColor = o.mWithSolidColor;

			// LON-LAT

			mEastComponent = o.mEastComponent;
			mNorthComponent = o.mNorthComponent;
			mMagnitudeFactor = o.mMagnitudeFactor;

			// YFX

			mLines = o.mLines;
			mPoints = o.mPoints;

			mPointSize = o.mPointSize;
			mPointGlyph = o.mPointGlyph;
			mFilledPoint = o.mFilledPoint;

			mLineColor = o.mLineColor;
			mLineWidth = o.mLineWidth;
			mStipplePattern = o.mStipplePattern;
			mPointColor = o.mPointColor;

			mNumberOfBins = o.mNumberOfBins;
		}
		return *this;
	}

	
	bool operator == ( const CFieldData& o ) const
	{
		return 
			mInternalFiles == o.mInternalFiles &&

#if defined (BRAT_V3)
			mTitle == o.mTitle &&
			m_xMax == o.m_xMax &&
			m_yMax == o.m_yMax &&
			m_xMin == o.m_xMin &&
			m_yMin == o.m_yMin &&

			mXlabel == o.mXlabel &&
			mYlabel == o.mYlabel &&

			m_xTicks == o.m_xTicks &&
			m_yTicks == o.m_yTicks &&
			m_zTicks == o.m_zTicks &&

			m_xDigits == o.m_xDigits &&
			m_yDigits == o.m_yDigits &&
			m_zDigits == o.m_zDigits &&
#endif
			// YFX / ZFXY / LON-LAT

			mFieldName == o.mFieldName &&
			mUserName == o.mUserName &&

			mOpacity == o.mOpacity &&

			// YFX / ZFXY

			mXlogarithmic == o.mXlogarithmic &&
			mYlogarithmic == o.mYlogarithmic &&

			//	ZFXY / LON-LAT

			mLUT == o.mLUT &&
			mMinHeightValue == o.mMinHeightValue &&
			mMaxHeightValue == o.mMaxHeightValue &&
			//mCurrentMinValue == o.mCurrentMinValue &&
			//mCurrentMaxValue == o.mCurrentMaxValue &&

			mWithContour == o.mWithContour &&
			mNumContours == o.mNumContours &&
			mContourPrecisionGrid1 == o.mContourPrecisionGrid1 &&
            mContourPrecisionGrid2 == o.mContourPrecisionGrid2 &&
			mContourLineWidth == o.mContourLineWidth &&
			mContourLineColor == o.mContourLineColor &&
			mMinContourValue == o.mMinContourValue &&
			mMaxContourValue == o.mMaxContourValue &&

			mWithSolidColor == o.mWithSolidColor &&

			// LON-LAT

			mEastComponent == o.mEastComponent &&
			mNorthComponent == o.mNorthComponent &&
			mMagnitudeFactor == o.mMagnitudeFactor &&

			// YFX

			mLines == o.mLines &&
			mPoints == o.mPoints &&

			mPointSize == o.mPointSize &&
			mPointGlyph == o.mPointGlyph &&
			mFilledPoint == o.mFilledPoint &&

			mLineColor == o.mLineColor &&
			mLineWidth == o.mLineWidth &&
			mStipplePattern == o.mStipplePattern &&
			mPointColor == o.mPointColor &&

			mNumberOfBins == o.mNumberOfBins &&

			true;
	}

	///////////////////////////////
	//		properties
	///////////////////////////////


	// YFX / ZFXY / LON-LAT

	const std::vector< CInternalFiles* >& InternalFiles() const
	{
		return mInternalFiles;
	}

	CInternalFiles* InternalFile( size_t index ) const
	{
		assert__( index < mInternalFiles.size() );

		return mInternalFiles[ index ];
	}
	void PushInternalFile( CInternalFiles *f )
	{
		mInternalFiles.push_back( f );
	}
	void ClearInternalFiles()
	{
		mInternalFiles.clear();
	}


	CInternalFilesYFX* InternalFileYFX( size_t index ) const;

public:

#if defined (BRAT_V3)
	const std::string& Title() const { return mTitle; }
	void SetTitle( const std::string& value ) { mTitle = value; }

	const std::string& Xlabel() const { return mXlabel; }
	virtual void SetXlabel( const std::string& value ) { mXlabel = value; }

	const std::string& Ylabel() const { return mYlabel; }
	virtual void SetYlabel( const std::string& value ) { mYlabel = value; }

	void XRange( double& min, double& max ) const { min = m_xMin; max = m_xMax; }
	void SetXRange( double min, double max ) { m_xMin = min; m_xMax = max; }

	void YRange( double& min, double& max ) const { min = m_yMin; max = m_yMax; }
	void SetYRange( double min, double max ) { m_yMin = min; m_yMax = max; }

	double Xmin() const { return m_xMin; }
	double Xmax() const { return m_xMax; }
	void SetXmin( double value ) { m_xMin = value; }
	void SetXmax( double value ) { m_xMax = value; }

	double Ymin() const { return m_yMin; }
	double Ymax() const { return m_yMax; }
	void SetYmin( double value ) { m_yMin = value; }
	void SetYmax( double value ) { m_yMax = value; }

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
#endif

	const std::string& FieldName() const { return mFieldName; }
	virtual void SetFieldName( const std::string& value ) 
	{ 
		mFieldName = value; 
		if ( mUserName.empty() )
			mUserName = mFieldName;
	}

	const std::string& UserName() const { return mUserName.empty() ? mFieldName : mUserName; }
	virtual void SetUserName( const std::string &value ) { mUserName = value; }

	unsigned_t Opacity() const { return mOpacity; }
	virtual void SetOpacity( unsigned_t op ) { mOpacity = op; }


	// YFX / ZFXY

	bool XLog() const { return mXlogarithmic; }
	void SetXLog( bool value );

	bool YLog() const { return mYlogarithmic; }
	void SetYLog( bool value );


	//	ZFXY / LON-LAT

	//...color table && color table range

	const std::string& ColorPalette() const { return mLUT.GetCurrentFunction(); }
	void SetColorPalette( const std::string& value )								//NOTE: not prepared for v3 file palettes
	{ 
		mLUT.ExecMethod( value );	  				assert__( ColorPalette() == value );
	}
	const std::string& ColorCurve() const { return mLUT.GetCurve(); }
	void SetColorCurve( const std::string& value )
	{ 
		mLUT.ExecCurveMethod( value );				assert__( ColorCurve() == value );
	}

	const CBratLookupTable& ColorTable() const { return mLUT; }
	CBratLookupTable* ColorTablePtr() { return &mLUT; }
	void SetColorTable( const CBratLookupTable &table ) { mLUT = table; }

	bool LoadColorTable( std::string &error_msg, const std::string &path ) 
	{ 
		return mLUT.LoadFromFile( error_msg, path );
	}

	void SetTableRange( double m, double M )
	{
		SetCurrentMinValue( m );
		SetCurrentMaxValue( M );
	}

protected:
	void SetColorTableRange( double m, double M )
	{
		mLUT.SetTableRange( m, M );
	}

public:

	double AbsoluteMinValue() const { return mMinHeightValue; }
	double AbsoluteMaxValue() const { return mMaxHeightValue; }
	void SetAbsoluteRangeValues( double m, double M ) 
	{ 
		if ( m > M )
			return;

		mMinHeightValue = m; 
		mMaxHeightValue = M; 

		double current_min = CurrentMinValue();
		double current_max = CurrentMaxValue();

		if ( current_min == CBratLookupTable::smDefaultRangeValues )
			current_min = m;

		if ( current_max == CBratLookupTable::smDefaultRangeValues )
			current_max = M;

		SetTableRange( current_min, current_max );
	}


	double CurrentMinValue() const { return mLUT.CurrentMinValue(); }
	void SetCurrentMinValue( double value ) 
	{ 
		double current_min = std::min( std::max( value, mMinHeightValue ), mMaxHeightValue ); 
		SetColorTableRange( current_min, CurrentMaxValue() );
	}

	double CurrentMaxValue() const { return mLUT.CurrentMaxValue(); }
	void SetCurrentMaxValue( double value ) 
	{ 
		double current_max = std::max( std::min( value, mMaxHeightValue ), mMinHeightValue ); 
		SetColorTableRange( CurrentMinValue(), current_max );
	}


	//

	std::string ContourAsText() const { return mWithContour ? "Y" : "N"; }
	bool WithContour() const { return mWithContour; }
	void SetWithContour( bool value ) { mWithContour = value; }

	unsigned NumContours() const { return mNumContours; }
	void SetNumContours( unsigned contours ) { mNumContours = contours; }

	std::pair< unsigned, unsigned > ContourPrecision() const
	{
		return std::pair< unsigned, unsigned > { mContourPrecisionGrid1, mContourPrecisionGrid2 };
	}
	void SetContourPrecision( unsigned int grid1, unsigned int grid2 )
	{
		mContourPrecisionGrid1 = grid1;
		mContourPrecisionGrid2 = grid2;
	}

	double ContourLineWidth() const { return mContourLineWidth; }
	void SetContourLineWidth( double width ) { mContourLineWidth = width; }

	CPlotColor ContourLineColor() const { return mContourLineColor;	}
	void SetContourLineColor( CPlotColor color ) { mContourLineColor = color; }

	double MinContourValue() const { return mMinContourValue; }
	double MaxContourValue() const { return mMaxContourValue; }
	void SetContourValueRange( double m, double M ) 
	{ 
		assert__( m <= M );

		mMinContourValue = m; 
		mMaxContourValue = M; 
	}

	bool WithSolidColor() const { return mWithSolidColor; }
	void SetWithSolidColor( bool value ) { mWithSolidColor = value; }


	// LON-LAT

	bool IsEastComponent() const { return mEastComponent; }
	void SetEastComponent( bool value ) { mEastComponent = value; }

	bool IsNorthComponent() const { return mNorthComponent; }
	void SetNorthComponent( bool value ) { mNorthComponent = value; }

	double MagnitudeFactor() const { return mMagnitudeFactor; }
	void SetMagnitudeFactor( double value ) { mMagnitudeFactor = value; }


	// YFX

	bool Lines() const { return mLines; }
	virtual void SetLines( bool value ) { mLines = value; }

	bool Points() const { return mPoints; }
	virtual void SetPoints( bool value ) { mPoints = value; }

	double PointSize() const { return mPointSize; }
	virtual void SetPointSize( double value ) { mPointSize = value; }

	EPointGlyph PointGlyph() const { return mPointGlyph; }
	virtual void SetPointGlyph( EPointGlyph value ) { mPointGlyph = value; }

	bool FilledPoint() const { return mFilledPoint; }
	virtual void SetFilledPoint( bool value ) { mFilledPoint = value; }

	virtual CPlotColor LineColor() const { return mLineColor; }
	virtual void SetLineColor( const CPlotColor& color ){ mLineColor = color; }

	unsigned_t LineWidth() const { return mLineWidth; }
	virtual void SetLineWidth( unsigned_t value ) { mLineWidth = value; }

	EStipplePattern StipplePattern() const { return mStipplePattern; }
	void SetStipplePattern( EStipplePattern e ) { mStipplePattern = e; }

	const CPlotColor& PointColor() const { return mPointColor; }
	void SetPointColor( const CPlotColor &color ) { mPointColor = color; }

	unsigned NumberOfBins() const { return mNumberOfBins; }
	void SetNumberOfBins( unsigned bins ) { mNumberOfBins = bins; }
};




#if defined(BRAT_V3)

	#include "../support/code/legacy/display/PlotData/XYPlotProperties.h"
	#include "../support/code/legacy/display/PlotData/ZFXYPlotProperties.h"
	#include "../support/code/legacy/display/PlotData/WorldPlotProperties.h"
#else

	#define CXYPlotProperties CFieldData
	#define CZFXYPlotProperties CFieldData
	#define CWorldPlotProperties CFieldData

#endif


#endif			//DATA_MODELS_PLOT_DATA_FIELDDATA_H
