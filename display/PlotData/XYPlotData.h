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

#ifndef __XYPlotData_H__
#define __XYPlotData_H__

#if defined (__unix__)
#if defined (__DEPRECATED)          //avoid linux warning in vtk include
#undef __DEPRECATED
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif	// __unix__

#include <qwt_data.h>


#include <vtkProperty2D.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>

#include "vtkPlotData.h"		//VISAN

#include "new-gui/Common/tools/Exception.h"
#include "new-gui/Common/+Utils.h"

#include "libbrathl/brathl.h"
#include "libbrathl/BratObject.h"
#include "libbrathl/Tools.h"
#include "libbrathl/List.h"
#include "libbrathl/Date.h"
#include "libbrathl/Unit.h"

#include "libbrathl/InternalFiles.h"
#include "libbrathl/InternalFilesZFXY.h"
#include "libbrathl/InternalFilesYFX.h"
using namespace brathl;

#include "vtkColor.h"
#include "vtkList.h"
#include "MapColor.h"
#include "Plot.h"


class CXYPlotData;
class CVtkColor;


// WDR: class declarations
//-------------------------------------------------------------
//------------------- vtkDoubleArrayBrathl class
//-------------------------------------------------------------
class vtkDoubleArrayBrathl : public vtkDoubleArray
{
	// redefinition with suffix 2 because these attributes (without suffix 2) are private in vtkDataArray
	double Range2[ 2 ];

	// 5 components since you can compute the range of components
	// less than 0 to get a magnitude range. ComponentRange[4] is 
	// this magnitude range

	vtkTimeStamp ComponentRangeComputeTime2[ 5 ];
	double ComponentRange2[ 5 ][ 2 ];

protected:

	vtkDoubleArrayBrathl( vtkIdType numComp=1 ) : vtkDoubleArray( numComp )
	{}
	~vtkDoubleArrayBrathl()
	{}

public:
	static vtkDoubleArrayBrathl* New();
	void PrintSelf( std::ostream& os, vtkIndent indent );

	// Description:
	// Return the range of the array values for the 0th component. 
	// Range is copied into the array provided.
	double* GetRange2()
	{
		this->ComputeRange( 0 );
		return this->Range2;
	}
private:
	virtual void ComputeRange( int comp );
};



//-------------------------------------------------------------
//------------------- vtkDataArrayPlotData class
//-------------------------------------------------------------
class vtkDataArrayPlotData : public vtkPlotData
{
public:
	vtkTypeMacro( vtkDataArrayPlotData, vtkPlotData );
	static vtkDataArrayPlotData *New() { return new vtkDataArrayPlotData; }

protected:
	void SetXDataArray( vtkDoubleArrayBrathl *dataArray );
	void SetYDataArray( vtkDoubleArrayBrathl *dataArray );
	void SetZDataArray( vtkDoubleArrayBrathl *dataArray );

public:
	void SetDataArrays( vtkDoubleArrayBrathl *XdataArray, vtkDoubleArrayBrathl *YdataArray )
	{
		SetXDataArray( XdataArray );
		SetYDataArray( YdataArray );
	}

	vtkDataArray *GetXDataArray() { return this->XDataArray; }
	vtkDataArray *GetYDataArray() { return this->YDataArray; }
	vtkDataArray *GetZDataArray() { return this->ZDataArray; }

	virtual double GetXValue( int i );
	virtual double GetYValue( int i );
	virtual double GetZValue( int i );
	virtual int GetNumberOfItems();

	// Description:
	// Override GetMTime because we depend on vtkDataArray objects
	unsigned long GetMTime();

protected:
	vtkDataArrayPlotData();
	virtual ~vtkDataArrayPlotData();

	// Description:
	// Override Execute so we can update NumberOfItems;
	virtual void Execute();

	vtkDoubleArrayBrathl *XDataArray;
	vtkDoubleArrayBrathl *YDataArray;
	vtkDoubleArrayBrathl *ZDataArray;

	int NumberOfItems;

private:
	vtkDataArrayPlotData( const vtkDataArrayPlotData& );  // Not implemented.
	void operator=( const vtkDataArrayPlotData& );  // Not implemented.
};


class CQwtArrayPlotData : public QwtData
{
	// The x values depend on its index and the y values
	// can be calculated from the corresponding x value.
	// So we don't need to store the values.
	// Such an implementation is slower because every point
	// has to be recalculated for every replot, but it demonstrates how
	// QwtData can be used.

	struct CCurve
	{
		double *mX = nullptr;
		double *mY = nullptr;

		size_t mSizeX = 0;
		size_t mSizeY = 0;

		CCurve( double *x, double *y, size_t size_x, size_t size_y ) :
			mX( x ), mY( y ), mSizeX( size_x ), mSizeY( size_y )
		{
			assert__( mX && mY );
		}

		~CCurve()
		{
			//free( mX );
			//free( mY );
			//delete[] mX;
			//delete[] mY;
		}
	};

	std::vector<CCurve> mFrames;

public:
	CQwtArrayPlotData()
	{}
	CQwtArrayPlotData( const CQwtArrayPlotData &o )
	{
		*this = o;
	}

	CQwtArrayPlotData& operator = ( const CQwtArrayPlotData &o )
	{
		if ( this != &o )
		{
			mFrames = o.mFrames;
		}
		return *this;
	}

	virtual ~CQwtArrayPlotData()
	{}

	void SetFrameData( size_t r, double *x, size_t sizex, double *y, size_t sizey )
	{
		Q_UNUSED( r );

		mFrames.push_back( CCurve( x, y, sizex, sizey ) );
	}

	virtual QwtData *copy() const
	{
		return new CQwtArrayPlotData( *this );
	}

	virtual size_t size() const
	{
		assert__( mFrames[0].mSizeX == mFrames[0].mSizeY );

		return mFrames[0].mSizeX;
	}

	virtual double x( size_t i ) const
	{
		assert__( i < mFrames[0].mSizeX );

		return mFrames[0].mX[ i ];
	}

	virtual double y( size_t i ) const
	{
		assert__( i < mFrames[0].mSizeY );

		return mFrames[0].mY[ i ];
	}
};






//-------------------------------------------------------------
//------------------- CPlotArray class --------------------
//-------------------------------------------------------------

class CPlotArray : public CBratObject
{
protected:
	bool m_didCalculateRange;
	double m_min;
	double m_max;

	vtkObArray m_rows;

public:
	CPlotArray();
	virtual ~CPlotArray();
	vtkDoubleArrayBrathl* GetFrameData( size_t r );
	vtkDoubleArrayBrathl* GetFrameData( vtkObArray::iterator it );

	void SetFrameData( uint32_t r, double* vect, int32_t size );
	void SetFrameData( uint32_t r, vtkDoubleArrayBrathl* vect );

	void GetRange( double& min, double& max );

	void GetRange( double& min, double& max, uint32_t frame );

	size_t GetNumberOfFrames() { return m_rows.size(); }

	void Normalize( int32_t value = 1 );
};

//-------------------------------------------------------------
//------------------- CXYPlotProperty class --------------------
//-------------------------------------------------------------



class CXYPlotProperty : public CBratObject
{
public:

  CXYPlotProperty(CXYPlotData* parent = NULL);
  CXYPlotProperty(CXYPlotProperty& p);

  virtual ~CXYPlotProperty();

  const CXYPlotProperty& operator=(CXYPlotProperty& p);

  vtkProperty2D* GetVtkProperty2D() {return m_vtkProperty2D;}

  CXYPlotData* GetParent() {return m_parent;}
  void SetParent(CXYPlotData* parent) {m_parent = parent;}

  bool GetPoints() {return m_points;}
  void SetPoints();
  void SetPoints(bool value);

  double GetPointSize() {return m_pointSize;}
  void SetPointSize();
  void SetPointSize(double value);

  bool GetLines() {return m_lines;}
  void SetLines();
  void SetLines(bool value);

  double GetLineWidth() {return m_vtkProperty2D->GetLineWidth();}
  void SetLineWidth(double value);

  StipplePattern GetLineStipple() {return static_cast<StipplePattern>(m_vtkProperty2D->GetLineStipplePattern());}
  void SetLineStipple(StipplePattern value) { m_vtkProperty2D->SetLineStipplePattern(value);}

  std::string GetName() {return m_name;}
  void SetName(const std::string& value);

  double GetOpacity() {return m_vtkProperty2D->GetOpacity();}
  void SetOpacity(double value);

  bool GetXLog() {return m_xLog;}
  void SetXLog(bool value);

  bool GetYLog() {return m_yLog;}
  void SetYLog(bool value);

  void GetXRange(double& min, double& max) {min = m_xMin; max = m_xMax;}
  void SetXRange(double min, double max) {m_xMin = min; m_xMax = max;}

  void GetYRange(double& min, double& max) {min = m_yMin; max = m_yMax;}
  void SetYRange(double min, double max) {m_yMin = min; m_yMax = max;}

  double GetXMin() {return m_xMin;}
  double GetXMax() {return m_xMax;}
  void SetXMin(double value) {m_xMin = value;}
  void SetXMax(double value) {m_xMax = value;}

  double GetYMin() {return m_yMin;}
  double GetYMax() {return m_yMax;}
  void SetYMin(double value) {m_yMin = value;}
  void SetYMax(double value) {m_yMax = value;}

  double GetXBase() {return m_xBase;}
  double GetYBase() {return m_yBase;}
  void SetXBase(double value) {m_xBase = value;}
  void SetYBase(double value) {m_yBase = value;}

  std::string GetTitle() {return m_title;}
  void SetTitle(const std::string& value) {m_title = value;}

  std::string GetXAxis() {return m_xAxis;}
  void SetXAxis(const std::string& value) {m_xAxis = value;}
  
  std::string GetXLabel() {return m_xLabel;}
  void SetXLabel(const std::string& value) {m_xLabel = value;}
  
  std::string GetYLabel() {return m_yLabel;}
  void SetYLabel(const std::string& value) {m_yLabel = value;}

  bool GetShowPropertyPanel() {return m_showPropertyPanel;}
  void SetShowPropertyPanel(bool value) {m_showPropertyPanel = value;}

  bool GetShowAnimationToolbar() {return m_showAnimationToolbar;}
  void SetShowAnimationToolbar(bool value) {m_showAnimationToolbar = value;}

  bool GetFocus() {return m_focus;}
  void SetFocus(bool value);

  bool GetLoop() {return m_loop;}
  void SetLoop(bool value) {m_loop = value;}

  uint32_t GetFps() {return m_fps;}
  void SetFps(uint32_t value) { m_fps = value;}

  uint32_t GetXNumTicks() {return m_xNumTicks;}
  uint32_t GetYNumTicks() {return m_yNumTicks;}
  void SetXNumTicks(uint32_t value) {m_xNumTicks = value;}
  void SetYNumTicks(uint32_t value) {m_yNumTicks = value;}

  int32_t GetNormalizeX() {return m_normalizeX;}
  int32_t GetNormalizeY() {return m_normalizeY;}
  void SetNormalizeX();
  void SetNormalizeY();
  void SetNormalizeX(int32_t value);
  void SetNormalizeY(int32_t value);

  CVtkColor GetColor();
  void GetColor(CVtkColor& vtkColor);
  void GetColor(double* value) {m_vtkProperty2D->GetColor(value);}
  void SetColor(double* value) {m_vtkProperty2D->SetColor(value);}
  void SetColor(const CVtkColor& vtkColor);
  void SetColor(double r, double g, double b) {m_vtkProperty2D->SetColor(r, g, b);}
  // TODO v4 void SetColor(const wxColour& color);										   replaced by global in wxInterface.h
  // TODO v4 void SetColor(wxColourData& colorData, int32_t indexCustomColor = -1);  	   replaced by global in wxInterface.h

  PointGlyph GetPointGlyph() { return m_glyphType;}
  void SetPointGlyph(PointGlyph value);
  void SetPointGlyph();

  bool GetFilledPoint() { return m_filledPoint;}
  void SetFilledPoint(bool value);
  void SetFilledPoint();

  double GetLineWidthFactor() { return m_lineWidthFactor;}
  void SetLineWidthFactor(double value) { m_lineWidthFactor = value;}
  double GetOpacityFactor() { return m_opacityFactor;}
  void SetOpacityFactor(double value) { m_opacityFactor = value;}

  bool GetHide() {return m_hide;}
  void SetHide(bool value);
  void SetHide();

  void Update();


protected:
  bool HasParent();
  void Copy(CXYPlotProperty& p);
  void Init();

protected:
  vtkProperty2D* m_vtkProperty2D;
  
  CXYPlotData* m_parent;

  //int32_t m_lineStipple;
  //double m_lineWidth;
  //double m_opacity;
  
  bool m_lines;
  bool m_points;
  
  double m_pointSize;

  std::string m_name;
  std::string m_title;
  std::string m_xAxis;
  std::string m_xLabel;
  std::string m_yLabel;

  double m_xMax;
  double m_yMax;
  double m_xMin;
  double m_yMin;

  double m_xBase;
  double m_yBase;

  bool m_xLog;
  bool m_yLog;


  bool m_showAnimationToolbar;
  bool m_showPropertyPanel;
  bool m_focus;
  bool m_loop;
  int32_t m_normalizeX;
  int32_t m_normalizeY;

  uint32_t m_fps;

  uint32_t m_xNumTicks;
  uint32_t m_yNumTicks;


  PointGlyph m_glyphType;
  bool m_filledPoint;

  double m_lineWidthFactor;
  double m_opacityFactor;

  bool m_hide;
};


//-------------------------------------------------------------
//------------------- CXYPlotData class --------------------
//-------------------------------------------------------------

class CXYPlotData : public CBratObject
{

protected:

  //std::string m_name;
  int32_t m_currentFrame;
  int32_t m_length;

  CXYPlotProperty m_plotProperty;

  vtkDataArrayPlotData* m_vtkDataArrayPlotData;
  CQwtArrayPlotData mQwtPlotData;

  CObArray m_rawData;

  CUnit m_unitX;
  CUnit m_unitY;

//  std::string m_varComplementName;
  CExpressionValue m_varComplement;
//  ExpressionValueDimensions m_dimValComplement;

  CFieldNetCdf m_fieldComplement;


  //std::vector<CExpressionValue> m_otherVars;
  // A "CExpressionValue*" array
  CObArray m_otherVars;
  
  CObArray m_otherFields;
  

  bool m_aborted;

public:
  CXYPlotData(const std::string& name = "", CXYPlotProperty* plotProperty = NULL);
  virtual ~CXYPlotData();

  void GetXRange(double& min, double& max, uint32_t frame);
  void GetYRange(double& min, double& max, uint32_t frame);

  void GetXRange(double& min, double& max);
  void GetYRange(double& min, double& max);
  
  const CUnit& GetXUnit(){return m_unitX;}
  const CUnit& GetYUnit(){return m_unitY;}

  void SetXUnit(const CUnit& unit) {m_unitX = unit;}
  void SetYUnit(const CUnit& unit) {m_unitY = unit;}
  
  int32_t GetNumberOfFrames();

  void NormalizeX(int32_t value);
  void NormalizeY(int32_t value);
  void Normalize(int32_t value);

  virtual void SetData(double* xArray, const CUIntArray& xDims,  
                       double* yArray, const CUIntArray& yDims, 
                       const CUIntArray& xCommonDimIndex,
                       const CUIntArray& yCommonDimIndex);

  std::string GetName() {return m_plotProperty.GetName();}
  void SetName(const std::string& name ) {m_plotProperty.SetName(name);}

  void OnFrameChange(int32_t f);

  vtkDataArrayPlotData* GetVtkDataArrayPlotData() { return m_vtkDataArrayPlotData; }
  const CQwtArrayPlotData*  GetQwtArrayPlotData() const { return &mQwtPlotData; }
  
  CXYPlotProperty* GetPlotProperty() {return &m_plotProperty;}

  CPlotArray* GetRawData(int32_t i);

  void Create(CInternalFiles* yfx, CPlot* plot, int32_t iField);

  CExpressionValue& GetVarComplement() { return m_varComplement; };
  
  CFieldNetCdf& GetFieldComplement() { return m_fieldComplement; };
  
  const CObArray& GetOtherVars() { return m_otherVars; };
  const CObArray& GetOtherFields() { return m_otherFields; };

protected:

  void Init();
  
  void SetRawData(CPlotArray* x, CPlotArray* y);

  void Update();
};


//-------------------------------------------------------------
//------------------- CXYPlotDataCollection class --------------------
//-------------------------------------------------------------

class CXYPlotDataCollection : public CObArray
{

	bool m_zoomToDataRange;
	int32_t m_currentFrame;

public:

	CXYPlotDataCollection()
		: m_zoomToDataRange( false )
		, m_currentFrame( 0 )
	{}
	virtual ~CXYPlotDataCollection()
	{}

	void GetXRange( double& min, double& max, uint32_t frame );
	void GetYRange( double& min, double& max, uint32_t frame );

	void GetXRange( double& min, double& max );
	void GetYRange( double& min, double& max );

	CXYPlotData* Get( CObArray::iterator it );
	CXYPlotData* Get( int32_t index );
	CXYPlotProperty* GetPlotProperty( int32_t index );

	void OnFrameChange( int32_t f );

	bool ShowPropertyMenu();

	void GetNames( std::vector<std::string>& names );

	bool IsZoomToDataRange() { return m_zoomToDataRange; };
	void SetZoomToDataRange( bool value ) { m_zoomToDataRange = value; };

	int32_t GetCurrentFrame() { return m_currentFrame; };
	void SetCurrentFrame( int32_t value ) { m_currentFrame = value; };
};


#endif
