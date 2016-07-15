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
#ifndef DATA_MODELS_PLOT_DATA_BRAT_LOOKUP_TABLE_H
#define DATA_MODELS_PLOT_DATA_BRAT_LOOKUP_TABLE_H

#if defined(BRAT_V3)

#include <vtkLookupTable.h>

#else

#include <qwt3d_color.h>
#include <qwt_color_map.h>


#if defined(GL_LINES_ADJACENCY_EXT)
#undef GL_LINES_ADJACENCY_EXT
#endif
#if defined(GL_LINE_STRIP_ADJACENCY_EXT)
#undef GL_LINE_STRIP_ADJACENCY_EXT
#endif
#if defined(GL_TRIANGLES_ADJACENCY_EXT)
#undef GL_TRIANGLES_ADJACENCY_EXT
#endif
#if defined(GL_TRIANGLE_STRIP_ADJACENCY_EXT)
#undef GL_TRIANGLE_STRIP_ADJACENCY_EXT
#endif
#if defined(GL_BLEND_EQUATION_RGB)
#undef GL_BLEND_EQUATION_RGB
#endif



class QLookupTable : public QwtColorMap, public Qwt3D::Color
{
	// types

	typedef QwtColorMap qwt_base_t;
	typedef Qwt3D::Color qwt3d_base_t;

	typedef long long vtkIdType;


	// data

	std::vector<QColor> mTable;
	double mTableRange[2];
	//double HueRange[2];
	//double SaturationRange[2];
	//double ValueRange[2];
	//double AlphaRange[2];
	//int Scale;
	//int Ramp;

	//double RGBA[4]; //used during conversion process

	//int OpaqueFlag;
	////vtkTimeStamp OpaqueFlagBuildTime;

	// construction / destruction

	QLookupTable() 
		: qwt_base_t()
		, qwt3d_base_t()
	{
		mTableRange[0] = 0;
		mTableRange[1] = 0;
	}

public:
	QLookupTable( const QLookupTable&o );

	static QLookupTable* New()
	{
		return new QLookupTable();
	}
	void Delete()
	{
	}

	// remaining

	size_t GetNumberOfTableValues() const;
	void SetNumberOfTableValues(vtkIdType number);

	const double *GetTableRange() const
	{ 
		return mTableRange; 
	};

	void SetTableRange( const double r[2] )
	{
		SetTableRange( r[0], r[1] );
	}

	virtual void SetTableRange(double m, double M);

	void SetTableValue( size_t indx, double r, double g, double b, double a=1.0);


	const QColor& GetTableValue( size_t indx) const;

	size_t GetIndex( double v ) const;


	// QWT interface

protected:
	virtual QwtColorMap *copy() const override;

	//Map a value of a given interval into a rgb value.
	// param interval Range for the values
	// param value Value
	// return rgb value, corresponding to value
	//
    virtual QRgb rgb( const QwtDoubleInterval &interval, double value ) const override;

    //Map a value of a given interval into a color index
    // param interval Range for the values
    // param value Value
    // return color index, corresponding to value
	//
    virtual unsigned char colorIndex( const QwtDoubleInterval &interval, double value ) const  override;

    //QColor color(const QwtDoubleInterval &, double value) const;

    virtual QVector<QRgb> colorTable(const QwtDoubleInterval &) const override;


	// QWT3D interface

	virtual Qwt3D::RGBA operator()(double x, double y, double z) const override;

	virtual Qwt3D::ColorVector& createVector(Qwt3D::ColorVector& vec) override;
};

#endif


#include "libbrathl/brathl.h"

#include "libbrathl/Tools.h"
#include "libbrathl/List.h"
#include "libbrathl/CallBack.h"
using namespace brathl;

#include "new-gui/brat/DataModels/PlotData/ColorPalleteNames.h"
#include "new-gui/brat/DataModels/PlotData/PlotColor.h"



class CBratLookupTable;

//bool LoadFromFile( CBratLookupTable &lut, const std::string& file_name );
//void SaveToFile( CBratLookupTable &lut, const std::string& file_name );


//-------------------------------------------------------------
//------------------- CCustomColor class --------------------
//-------------------------------------------------------------

class CCustomColor : public CBratObject
{
public:
	CPlotColor mColor;
	int mXvalue;

public:
	/// Ctor
	CCustomColor()
	{
		setDefaultValue( mXvalue );
	}
	CCustomColor( const CCustomColor &o ) :
		  mColor( o.mColor )
		, mXvalue( o.mXvalue )
	{}
	CCustomColor( const CPlotColor &color, int xValue ) :
		  mColor( color )
		, mXvalue( xValue )
	{}

	/// Dtor
	virtual ~CCustomColor()
	{}

	CPlotColor* GetColor() { return &mColor; }
	int GetXValue() { return mXvalue; }
	std::string GetStringXValue() { return std::string( CTools::Format( 20, "%d", mXvalue ).c_str() ); }

	void SetColor( const CPlotColor& color ) { mColor = color; }
	void SetXValue( int xValue ) { mXvalue = xValue; }
};

//-------------------------------------------------------------
//------------------- CBratLookupTable class --------------------
//-------------------------------------------------------------

typedef CallBackv< CBratLookupTable, void > CallbackVoid;
typedef CallBack3< CBratLookupTable, double, double, double, double > CallbackFlank;


class CBratLookupTable : public CBratObject
{
	friend class CBratLookupTableSettings;

	static const std::string ENTRY_NUMCOLORS()	{ return "ColorTable/NumberOfColors"; }
	static const std::string ENTRY_CURRENTFCT() { return "ColorTable/CurrentFunction"; }
	static const std::string ENTRY_CURVE()		{ return "ColorTable/Curve"; }

	static const std::string GROUP_COLOR()		{ return "ColorDef"; }

	static const std::string CURVE_LINEAR()		{ return "Linear"; }
	static const std::string CURVE_SQRT()		{ return "SQRT"; }
	static const std::string CURVE_COSINUS()	{ return "Cosinus"; }


protected:

	const std::string mDefaultColorTable = PALETTE_AEROSOL;
	const std::string m_resetFunction = "Reset";

	QLookupTable *m_vtkLookupTable = nullptr;

	std::string m_currentFunction;

	std::string m_curve;
	CStringArray m_curveNames;

	CObMap m_nameToMethod;

	std::vector< std::string > m_colorTableList;

	std::string m_gradientFunction;
	std::string m_customFunction;

	CallbackFlank *mUpFlank = nullptr;
	CallbackFlank *mDownFlank = nullptr;

	CObArray m_cust;
	CObArray m_grad;

	std::string m_fileName;

public:

	static inline std::string brathlFmtEntryColorMacro( const std::string &entry, int index )
	{
		return CBratLookupTable::GROUP_COLOR() + n2s<std::string>( index ) + "/" + entry;
	}

	//	friend bool LoadFromFile( CBratLookupTable &lut, const std::string& file_name );
	//	friend void SaveToFile( CBratLookupTable &lut, const std::string& file_name );

protected:
	void Init();

public:

	CBratLookupTable();
	CBratLookupTable( const CBratLookupTable &lut );

	virtual ~CBratLookupTable();

	CBratLookupTable& operator = ( const CBratLookupTable &lut );

	bool operator == ( const CBratLookupTable &o ) const;


	QLookupTable* GetLookupTable() const { return m_vtkLookupTable; }

	const std::string& GetCurve() const { return m_curve; }
	const std::string& GetCurrentFunction() const { return m_currentFunction; }
	const std::string& GetResetFunction() { return m_resetFunction; }

	const CObArray* GetGrad() const { return &m_grad; }
	const CObArray* GetCust() const { return &m_cust; }
	CObArray* GetCust() { return &m_cust; }
	int GetMaxCustomXValue();

	const std::vector<std::string >* GetColorTableList() const { return &m_colorTableList; }

	std::string MethodToLabeledMethod( const std::string& method ) const;
	bool IsValidMethod( const std::string& methodName );

	std::string CurveToLabeledCurve( const std::string& curve ) const;
	bool IsValidCurve( const std::string& curve );

	void ExecMethod( const std::string& methodName );

	void ExecMethodDefaultColorTable();

	void ExecCurveMethod( const std::string& curve );

	bool IsCurrentCustom() { return m_currentFunction == m_customFunction; }
	bool IsCurrentGradient() { return m_currentFunction == m_gradientFunction; }
	bool IsCurrentReset() { return m_currentFunction == m_resetFunction; }

	std::string GetDefaultColorTable() { return mDefaultColorTable; }


	void Black();
protected:

	void BlackToWhite();
	void WhiteToBlack();
	void RedToGreen();
	void GreenToRed();
	void Cloud();
	void Rainbow();
	void Rainbow2();
	void Aerosol();
	void Aerosol2();
	void Ozone();
	void Blackbody();

public:

	void SetCurveLinear();
	void SetCurveSQRT();
	void SetCurveCosinus();

	void SetFacets( int f );
	void FacetsCorrection();

	void Reset();

	void Gradient( const CPlotColor& c1, const CPlotColor& c2 );
	void Gradient();

	void Custom( const CObArray& cust );
	void Custom( const CCustomColor& c1, const CCustomColor& c2 );
	void Custom();

	static void DupCustMap( const CObArray& custSrc, CObArray& custDest );

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr );

protected:

	void DupCustMap( const CObArray& cust );
	void DupGradMap( const CObArray& grad );


	void SetCurveUpDown( CallbackFlank* up, CallbackFlank* down );

	double LineUp( double a, double b, double x );
	double LineDown( double a, double b, double x );
	double SqrtUp( double a, double b, double x );
	double SqrtDown( double a, double b, double x );
	double CosUp( double a, double b, double x );
	double CosDown( double a, double b, double x );

	void Update();

	void DrawGradient( const CPlotColor& c1, const CPlotColor& c2, int i1, int i2 );

	////////////////////////////////////////////////////////v4
public:
	void SaveToFile( const std::string& file_name );
	bool LoadFromFile( std::string &error_msg, const std::string& file_name );

protected:
	//void SaveGradToFile( CBratLookupTableSettings& file );
	//void SaveCustToFile( CBratLookupTableSettings& file );

	//bool LoadGradFromFile( CBratLookupTableSettings& file );
	//bool LoadCustFromFile( CBratLookupTableSettings& file );

	void HandleLoadError( bool bOk, const std::string& entry );
	void HandleLoadColorError( std::string &error_msg, bool bOk, const std::string& entry, double& color );
	////////////////////////////////////////////////////////v4

	int InsertCustomColor( CCustomColor *color, std::string &warning );
};




#endif		//DATA_MODELS_PLOT_DATA_BRAT_LOOKUP_TABLE_H
