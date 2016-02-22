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
#ifndef __ZFXYPlotData_VTK_H__
#define __ZFXYPlotData_VTK_H__

//#include "wx/progdlg.h"

#include "libbrathl/brathl.h"

#include "libbrathl/BratObject.h"
#include "libbrathl/Tools.h"
#include "libbrathl/List.h"
#include "libbrathl/Date.h"
#include "new-gui/Common/tools/Exception.h"
#include "libbrathl/Unit.h"

#include "libbrathl/InternalFiles.h"
#include "libbrathl/InternalFilesZFXY.h"

using namespace brathl;


class VTK_CZFXYPlotData;


#if defined (__unix__)
#if defined (__DEPRECATED)          //avoid linux warning in vtk include
#undef __DEPRECATED
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif	// __unix__
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
#include "vtkSphereSource.h"
#include "vtkContourFilter.h"
#include "vtkDoubleArray.h"
#include "vtkPoints.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkSelectVisiblePoints.h"
#include "vtkProgrammableFilter.h"
#include "vtkLabeledDataMapper.h"
#include "vtkRenderer.h"
#include "vtkScalarBarActor.h"

#include "vtkSmoothPolyDataFilter.h"

#include "new-gui/brat/PlotValues.h"
#include "new-gui/brat/DataModels/PlotData/PlotField.h"
#include "new-gui/brat/DataModels/PlotData/ZFXYPlot.h"
#include "new-gui/brat/DataModels/PlotData/ZFXYPlotData.h"

#include "PlotData/vtkProj2DFilter.h"
#include "PlotData/vtkGeoGridSource.h"
#include "PlotData/vtkGSHHSReader.h"
#include "PlotData/vtkList.h"
#include "PlotData/vtkZFXYPlotActor.h"
#include "PlotData/BratLookupTable.h"



//-------------------------------------------------------------
//------------------- CLUTRenderer class --------------------
//-------------------------------------------------------------

class CLUTZFXYRenderer
{

public:
  
  CLUTZFXYRenderer();
  virtual ~CLUTZFXYRenderer();

  void SetLUT(CBratLookupTable* lut);

  bool ShouldDisplay() {return (m_LUT != NULL);};

  void SetNumberOfLabels(int32_t n);
  int32_t GetNumberOfLabels();

  void SetBackground(double r, double g, double b);
  
  vtkRenderer* GetVtkRenderer() {return m_vtkRend;};
  vtkScalarBarActor* GetScalarBarActor() {return m_scalarBarActor;};

  CBratLookupTable* GetLUT() {return m_LUT;};

  void ResetTextActor(VTK_CZFXYPlotData* geoMap);
  void ResetTextActor(const std::string& text);

  vtkLookupTable* GetLookupTable() 
    { 
      if (m_LUT == NULL)
      {
        return NULL;
      }
      return m_LUT->GetLookupTable();
    };

protected:

  void DeleteLUT();


protected:

  vtkRenderer* m_vtkRend;

  CBratLookupTable* m_LUT;

  vtkScalarBarActor* m_scalarBarActor;


};



//-------------------------------------------------------------
//------------------- VTK_CZFXYPlotData class --------------------
//-------------------------------------------------------------

class VTK_CZFXYPlotData : public CZFXYPlotData
{
	using base_t = CZFXYPlotData;

public:
  VTK_CZFXYPlotData(CZFXYPlot* plot, CPlotField* field);

  virtual ~VTK_CZFXYPlotData();


private:
	void Create(CObArray* data, const std::string& fieldName, CZFXYPlot* plot);

public:
  
	virtual void Close()
	{
		m_finished= true;
	}

  void SetInput(vtkPolyData* output);
  

  virtual void OnKeyframeChanged(uint32_t i);
  


  vtkZFXYPlotFilter* GetCurrentPlotData();
  vtkZFXYPlotFilter* GetPlotData(int32_t i);



  vtkActor2D* GetVtkActor2D() {return m_vtkActor2D;};

  vtkActor2D* GetVtkContourActor2D() {return m_vtkContourActor2D;}

  vtkActor2D* GetVtkContourLabelActor() {return m_vtkContourLabelActor;}

  CLUTZFXYRenderer* GetColorBarRenderer() {return m_colorBarRenderer;}

  CBratLookupTable* GetLUT() {return m_LUT;}

  void SetLUT(CBratLookupTable* lut);

  vtkLookupTable* GetLookupTable() 
    { 
      if (m_LUT == NULL)
      {
        return NULL;
      }
      return m_LUT->GetLookupTable();
    };

  vtkObArray* GetZFXYPlotFilterList() {return &m_zfxyPlotFilterList;}

  vtkPolyDataMapper2D* GetVtkMapper2D() {return m_vtkMapper2D;}

  vtkPolyDataMapper2D* GetVtkContourMapper2D() {return m_vtkContourMapper2D;}

  vtkProgrammableFilter* GetVtkVisibleSpherePointsFilter() {return m_vtkVisibleSpherePointsFilter;}
  vtkPoints* GetVtkVisibleSpherePoints() {return m_vtkVisibleSpherePoints;}
  vtkDoubleArray* GetVtkVisibleSpherePointsData() {return m_vtkVisibleSpherePointsData;}

  vtkRenderer* GetVtkRenderer() {return m_vtkRend;}

  bool HasActor2D();


  virtual void SetContour2D();

  void SetContourLabelProperties();
  void ContourGenerateValues();
  void SetContour2DProperties();



  void SetRenderer(vtkRenderer* vtkRend) {m_vtkRend = vtkRend;}

  static void FindVisiblePlanePoints(void* arg);

  virtual void CreateContourLabels2D();
  virtual void UpdateContourLabels2D();




  

 
	bool GetContourLabelNeedUpdateOnWindow() const { return m_contourLabelNeedUpdateOnWindow; }
	void SetContourLabelNeedUpdateOnWindow( bool value ) { m_contourLabelNeedUpdateOnWindow = value; }

	bool GetContourLabelNeedUpdatePositionOnContour() const { return m_contourLabelNeedUpdatePositionOnContour; }
	void SetContourLabelNeedUpdatePositionOnContour( bool value ) { m_contourLabelNeedUpdatePositionOnContour = value; }


protected:

  void DeleteMapper2D();
  void DeleteContourMapper2D();

  void DeleteActor2D();
  void DeleteContourActor2D();

  void DeleteAll2D();
  void Delete2D();
  void DeleteContour2D();


  void DeleteContourFilter();

  void DeleteContourLabelActor();
  void DeleteContourLabelMapper();
  void DeleteVtkContourLabelObject();

  void NewVtkContourLabelObject();


  void DeleteLUT();

public:



protected:

protected:

	bool m_finished = false;
  
  vtkActor2D* m_vtkActor2D;
  vtkActor2D* m_vtkContourActor2D;
  

  vtkPolyDataMapper2D* m_vtkMapper2D;
  vtkPolyDataMapper2D* m_vtkContourMapper2D;

  vtkContourFilter* m_vtkContourFilter;

  vtkDoubleArray* m_vtkLabelContourData;
  vtkPoints* m_vtkLabelContourPoints;
  vtkPolyData* m_vtkLabelContourPositions;

  vtkLabeledDataMapper* m_vtkContourLabelMapper;
  vtkActor2D* m_vtkContourLabelActor;

  vtkPolyDataConnectivityFilter* m_vtkConnectivity;

  vtkSelectVisiblePoints* m_vtkVisiblePointsFilter;
  vtkProgrammableFilter* m_vtkVisibleSpherePointsFilter;
  vtkPoints* m_vtkVisibleSpherePoints;
  vtkDoubleArray* m_vtkVisibleSpherePointsData;

  CBratLookupTable* m_LUT;
  

  vtkRenderer* m_vtkRend;

  CLUTZFXYRenderer* m_colorBarRenderer;
    
  vtkObArray m_zfxyPlotFilterList;


  vtkObArray m_labelPts;
  vtkObArray m_labelIds;
  vtkObArray m_labelData;

	bool m_contourLabelNeedUpdateOnWindow;
	bool m_contourLabelNeedUpdatePositionOnContour;

};





#endif
