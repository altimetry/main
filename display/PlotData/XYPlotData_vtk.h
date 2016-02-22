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
#include "new-gui/brat/DataModels/PlotData/MapColor.h"
#include "new-gui/brat/DataModels/PlotData/XYPlot.h"
#include "new-gui/brat/PlotValues.h"
#include "new-gui/brat/DataModels/PlotData/XYPlotData.h"


class VTK_CXYPlotData;
class CPlotColor;


// WDR: class declarations
//-------------------------------------------------------------
//------------------- vtkDoubleArrayBrathl class
//-------------------------------------------------------------
class vtkDoubleArrayBrathl : public vtkDoubleArray
{
	// redefinition with suffix 2 because these attributes (without suffix 2) are private in vtkDataArray
    // range2[0] is the vtkDoubleArray lower_bound
    // range2[1] is the vtkDoubleArray upper_bound
    double Range2[ 2 ];

	// 5 components since you can compute the range of components
	// less than 0 to get a magnitude range. ComponentRange[4] is 
	// this magnitude range

    //for each component its ComputeTime(?)
	vtkTimeStamp ComponentRangeComputeTime2[ 5 ];
    //for each componenet its range (min, max)
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




//-------------------------------------------------------------
//------------------- CPlotArray class --------------------
//-------------------------------------------------------------

class CPlotArray : public CBratObject
{
protected:
    //we did just calculate the range recently
	bool m_didCalculateRange;
    //this plot array has aminimum and a maximum
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
//------------------- VTK_CXYPlotData class --------------------
//-------------------------------------------------------------

class VTK_CXYPlotData : public CXYPlotData
{
	using base_t = CXYPlotData;

protected:


	vtkDataArrayPlotData* m_vtkDataArrayPlotData;

    //m_rawData will contain two elements of type CPlotArray
    // the first possibly for x values
    // the second for y values
	CObArray m_rawData;

public:
	VTK_CXYPlotData( CPlot* plot, int32_t iField );

	virtual ~VTK_CXYPlotData();


	vtkDataArrayPlotData* GetVtkDataArrayPlotData() { return m_vtkDataArrayPlotData; }

	CPlotArray* GetRawData( int32_t i );

protected:
	void Create();

public:


	virtual void GetXRange( double& min, double& max, uint32_t frame ) override;
	virtual void GetYRange( double& min, double& max, uint32_t frame ) override;

	virtual void GetXRange( double& min, double& max ) override;
	virtual void GetYRange( double& min, double& max ) override;

	virtual void NormalizeX( int32_t value ) override;
	virtual void NormalizeY( int32_t value ) override;
	virtual void Normalize( int32_t value ) override;

	virtual int32_t GetNumberOfFrames() override;

protected:

	void SetRawData( CPlotArray* x, CPlotArray* y );
	virtual void Update() override;
};


//-------------------------------------------------------------
//------------------- CXYPlotDataCollection class --------------------
//-------------------------------------------------------------

//class CXYPlotDataCollection : public CObArray
//{
//
//	bool m_zoomToDataRange;
//	int32_t m_currentFrame;
//
//public:
//
//	CXYPlotDataCollection()
//		: m_zoomToDataRange( false )
//		, m_currentFrame( 0 )
//	{}
//	virtual ~CXYPlotDataCollection()
//	{}
//
//	void GetXRange( double& min, double& max, uint32_t frame );
//	void GetYRange( double& min, double& max, uint32_t frame );
//
//	void GetXRange( double& min, double& max );
//	void GetYRange( double& min, double& max );
//
//	VTK_CXYPlotData* Get( CObArray::iterator it );
//	VTK_CXYPlotData* Get( int32_t index );
//	CXYPlotProperties* GetPlotProperties( int32_t index );
//
//	void OnFrameChange( int32_t f );
//
//	bool ShowPropertyMenu();
//
//	void GetNames( std::vector<std::string>& names );
//
//	bool IsZoomToDataRange() { return m_zoomToDataRange; };
//	void SetZoomToDataRange( bool value ) { m_zoomToDataRange = value; };
//
//	int32_t GetCurrentFrame() { return m_currentFrame; };
//	void SetCurrentFrame( int32_t value ) { m_currentFrame = value; };
//};


#endif
