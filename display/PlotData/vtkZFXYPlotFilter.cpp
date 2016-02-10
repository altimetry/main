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

#include "libbrathl/Tools.h"
#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"
using namespace brathl;


#if defined (__unix__)
#if defined (__DEPRECATED)          //avoid linux warning in vtk include
#undef __DEPRECATED
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif	// __unix__

#include "vtkTools.h"
#include "vtkZFXYPlotFilter.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkShortArray.h"
#include "vtkPolyDataWriter.h"

#include "vtkPlotData.h"

#include "math.h"

// Undefine max/min first (because of windows system header conflict)
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#define max(x,y)	((x)<(y) ? (y) : (x))
#define min(x,y)	((x)<(y) ? (x) : (y))

vtkStandardNewMacro(vtkZFXYPlotFilter);

vtkCxxSetObjectMacro(vtkZFXYPlotFilter,Values,vtkDoubleArray);
vtkCxxSetObjectMacro(vtkZFXYPlotFilter,ZDataArray,vtkDoubleArray);
vtkCxxSetObjectMacro(vtkZFXYPlotFilter,YDataArray,vtkDoubleArray);
vtkCxxSetObjectMacro(vtkZFXYPlotFilter,XDataArray,vtkDoubleArray);
vtkCxxSetObjectMacro(vtkZFXYPlotFilter,Bitarray,vtkShortArray);

//----------------------------------------------------------------------------
vtkZFXYPlotFilter::vtkZFXYPlotFilter()
{

  this->PlotLines = 0;
  this->PlotPoints = 0;
  this->PlotPolys = 1;

  this->LogX = 0;
  this->LogY = 0;
  this->LogZ = 0;

  this->PlotLabel = 0;

  this->InnerPlotPoint = NULL;
  this->Values = NULL;
  this->ZDataArray = NULL;
  this->Bitarray = NULL;
  this->YDataArray = NULL;
  this->XDataArray = NULL;
  this->MinMappedValue = 0.0;
  this->MaxMappedValue = 1.0;
  this->OffsetY = 0;
  this->OffsetX = 0;
  this->PlotWidth = 0;
  this->PlotHeight = 0;
  this->Id = 0;

  this->XRange[0] = 1.0;
  this->XRange[1] = 0.0;
  this->YRange[0] = 1.0;
  this->YRange[1] = 0.0;
  this->ZRange[0] = 1.0;
  this->ZRange[1] = 0.0;
  
  this->XDataRange[0] = 1.0;
  this->XDataRange[1] = 0.0;
  this->YDataRange[0] = 1.0;
  this->YDataRange[1] = 0.0;
  this->ZDataRange[0] = 1.0;
  this->ZDataRange[1] = 0.0;



  this->ViewportBounds[0] = 1.0;
  this->ViewportBounds[1] = 0.0;
  this->ViewportBounds[2] = 1.0;
  this->ViewportBounds[3] = 0.0;
  this->ViewportBounds[4] = 1.0;
  this->ViewportBounds[5] = 0.0;


  this->PlotPointsData = NULL;
  //this->PlotPointsData = vtkPolyData::New();
  
  this->PlotLinesData = NULL;
  //this->PlotLinesData = vtkPolyData::New();
  
  this->PlotPolysData = vtkPolyData::New();

  this->GlyphSize = 0.01;

  this->GlyphSource = vtkGlyphSource2D::New();
  this->GlyphSource->SetGlyphTypeToCircle();
  this->GlyphSource->FilledOff();

  this->PlotGlyph = NULL;
  

  this->PlotAppend = vtkAppendPolyData::New();
  this->PlotAppend->AddInput(this->PlotPolysData);
  //this->PlotAppend->AddInput(this->PlotGlyph->GetOutput());

  //this->PlotGlyph->Delete();
  //this->PlotPointsData->Delete();
  //this->PlotLinesData->Delete();
  this->PlotPolysData->Delete();


}

//----------------------------------------------------------------------------
vtkZFXYPlotFilter::~vtkZFXYPlotFilter()
{

  if (this->Values != NULL)
    {
    this->Values->Delete();
    this->Values = NULL;
    }

  if (this->ZDataArray != NULL)
    {
    this->ZDataArray->Delete();
    this->ZDataArray = NULL;
    }

  if (this->Bitarray != NULL)
    {
    this->Bitarray->Delete();
    this->Bitarray = NULL;
    }

  if (this->InnerPlotPoint != NULL)
    {
    this->InnerPlotPoint->Delete();
    this->InnerPlotPoint = NULL;
    }

  if (this->YDataArray != NULL)
    {
    this->YDataArray->Delete();
    this->YDataArray = NULL;
    }

  if (this->XDataArray != NULL)
    {
    this->XDataArray->Delete();
    this->XDataArray = NULL;
    }

  if (this->PlotLabel)
    {
    delete [] this->PlotLabel;
    PlotLabel = NULL;
    }


  if (this->PlotAppend != NULL)
    {
    this->PlotAppend->Delete();
    this->PlotAppend = NULL;
    }

  if (this->GlyphSource != NULL)
    {
    this->GlyphSource->Delete();
    this->GlyphSource = NULL;
    }

}
//----------------------------------------------------------------------------
bool vtkZFXYPlotFilter::IsValidPoint(int32_t index)
{
  if (this->Bitarray == NULL)
  {
    return false;
  }
  short* bits = this->InnerPlotPoint->GetPointer(0);
  
  if (bits == NULL)
  {
    return false;
  }
  bool bOk = (bits[index] == 1);

  return bOk;
}

//----------------------------------------------------------------------------
void vtkZFXYPlotFilter::GetOffset(int32_t x, int32_t y, double& offsetX, double& offsetY)
{
  double* yCoords = this->YDataArray->GetPointer(0);
  double* xCoords = this->XDataArray->GetPointer(0);

  offsetX = 0.0;
  if (x == 0)
  {
    if ( (x + 1) < this->PlotWidth)
    {
      offsetX = CTools::Abs((xCoords[x+1] - xCoords[x]) / 2);
    }
    else
    {
      offsetX = 1.0;
    }
  }
  else
  {
    if ( (x + 1) < this->PlotWidth)
    {
      offsetX = CTools::Abs((xCoords[x + 1] - xCoords[x]) / 2);
    }
    else
    {
      if ( x < this->PlotWidth)
      {
        offsetX = CTools::Abs((xCoords[x] - xCoords[x - 1]) / 2);
      }
      else
      {
        offsetX = CTools::Abs((xCoords[this->PlotWidth - 1] - xCoords[this->PlotWidth - 2]) / 2);
      }
    }

  }
  
  offsetY = 0.0;
  if (y == 0)
  {
    if ( (y + 1) < this->PlotHeight)
    {
      offsetY = CTools::Abs((yCoords[y + 1] - yCoords[y]) / 2);
    }
    else
    {
      offsetY = 1.0;
    }
  }
  else
  {
    if ( (y + 1) < this->PlotHeight)
    {
      offsetY = CTools::Abs((yCoords[y + 1] - yCoords[y]) / 2);
    }
    else
    {
      if ( y < this->PlotHeight)
      {
        offsetY = CTools::Abs((yCoords[y] - yCoords[y - 1]) / 2);
      }
      else
      {
        offsetY = CTools::Abs((yCoords[this->PlotHeight - 1] - yCoords[this->PlotHeight - 2]) / 2);
      }
    }

  }
}

//----------------------------------------------------------------------------
void vtkZFXYPlotFilter::ComputeRange(double range[2], int dim)
{
  int numComp = this->GetNumberOfItems(dim);

  range[0] = VTK_LARGE_FLOAT;
  range[1] = -VTK_LARGE_FLOAT;

  for (int i=0; i<numComp; i++)
    {
    double x;
    switch(dim)
      {
      case 0:
        x = this->GetValueX(i);
        break;
      case 1:
        x = this->GetValueY(i);
        break;
      case 2:
        x = this->GetValueZ(i);
        break;
      default:
        x = 0.0;
      }
    if ( x < range[0] )
      {
      range[0] = x;
      }
    if ( x > range[1] )
      {
      range[1] = x;
      }
    } //over all data objects

  if ( range[0] > range[1] )
    {
    range[0] = 0;
    range[1] = 0;
    }

  vtkDebugMacro(<<"ComputeRange (dim=" << dim << "): range = (" <<
                range[0] << "," << range[1] << ")");
}
//----------------------------------------------------------------------------
double vtkZFXYPlotFilter::GetValueX(int i)
{
  if (this->XDataArray != NULL)
    {
    return this->XDataArray->GetTuple1(i);
    }
  return 0.0;
}

//----------------------------------------------------------------------------
double vtkZFXYPlotFilter::GetValueY(int i)
{
  if (this->YDataArray != NULL)
    {
    return this->YDataArray->GetTuple1(i);
    }
  return 0.0;
}

//----------------------------------------------------------------------------
double vtkZFXYPlotFilter::GetValueZ(int i)
{
  if (this->ZDataArray != NULL)
    {
    return this->ZDataArray->GetTuple1(i);
    }
  return 0.0;
}
//----------------------------------------------------------------------------
int vtkZFXYPlotFilter::GetNumberOfItems(int dim)
{
  int numComp = 0;

  switch(dim)
    {
    case 0:
      if (this->XDataArray != NULL)
      {
        numComp = this->XDataArray->GetNumberOfTuples();
      }
      break;
    case 1:
      if (this->YDataArray != NULL)
      {
        numComp = this->YDataArray->GetNumberOfTuples();
      }
      break;
    case 2:
      if (this->ZDataArray != NULL)
      {
        numComp = this->ZDataArray->GetNumberOfTuples();
      }
      break;
    default:
      numComp = 0;
    }

  return numComp;
}

//----------------------------------------------------------------------------

void vtkZFXYPlotFilter::Execute()
{

  vtkTools::DeleteObject(InnerPlotPoint);
  this->InnerPlotPoint = vtkShortArray::New();
  this->InnerPlotPoint->DeepCopy(this->Bitarray);

  if (this->Values == NULL || this->Bitarray == NULL ||
      this->YDataArray == NULL || this->XDataArray == NULL)
    {
    vtkErrorMacro(<< "One or more data arrays are empty");
    return;
    }
  if (this->Values->GetNumberOfTuples() != this->PlotHeight * this->PlotWidth)
    {
    vtkErrorMacro(<< "Input 'Values' contains invalid number of elements");
    return;
    }
  if (this->Bitarray->GetNumberOfTuples() != this->PlotHeight * this->PlotWidth)
    {
    vtkErrorMacro(<< "Input 'Bitarray' contains invalid number of elements");
    return;
    }

  if (this->YDataArray->GetNumberOfTuples() != this->PlotHeight)
    {
    vtkErrorMacro(<< "Input 'yData' contains invalid number of elements");
    return;
    }
  if (this->XDataArray->GetNumberOfTuples() != this->PlotWidth)
    {
    vtkErrorMacro(<< "Input 'xData' contains invalid number of elements");
    return;
    }
  


  double* values = this->Values->GetPointer(0);

  
  double* yData = this->YDataArray->GetPointer(0);
  double* xData = this->XDataArray->GetPointer(0);

    
  vtkPoints* points = vtkPoints::New(VTK_DOUBLE);
  vtkPoints* mappedPoints = vtkPoints::New(VTK_DOUBLE);
  vtkCellArray* polys = vtkCellArray::New();
  vtkDoubleArray* colors = vtkDoubleArray::New();
  
  this->PlotPolysData->SetPoints(mappedPoints);
  this->PlotPolysData->SetPolys(polys);
  this->PlotPolysData->GetCellData()->SetScalars(colors);
  
  polys->Delete();
  mappedPoints->Delete();
  colors->Delete();

  vtkIdType* pointIds = new vtkIdType[(this->PlotHeight + 1) * (this->PlotWidth + 1)];
    
  int viewportMappingNeeded =
    (this->ViewportBounds[0]<=this->ViewportBounds[1] ||
     this->ViewportBounds[2]<=this->ViewportBounds[3] ||
     this->ViewportBounds[4]<=this->ViewportBounds[5]);
  
  double dataBounds[6];

  this->GetXRange(dataBounds);
  this->GetYRange(&(dataBounds[2]));
  this->GetZRange(&(dataBounds[4]));
  
  int dataClippingNeeded = (dataBounds[0]<=dataBounds[1] ||
                            dataBounds[2]<=dataBounds[3] ||
                            dataBounds[4]<=dataBounds[5]);

  if (viewportMappingNeeded || dataClippingNeeded)
    {
    // Make sure that 'dataBounds' corresponds with the real bounds
    // (for all dimensions that have no user-specified range)
    if (dataBounds[0]>dataBounds[1])
      {
      this->ComputeXRange(dataBounds);
      }
    if (dataBounds[2]>dataBounds[3])
      {
      this->ComputeYRange(&(dataBounds[2]));
      }
    if (dataBounds[4]>dataBounds[5])
      {
      this->ComputeZRange(&(dataBounds[4]));
      }
    }

  vtkDebugMacro(<<"  Using databounds: (" << dataBounds[0] << ", " <<
                  dataBounds[1] << ", " << dataBounds[2] << ", " <<
                  dataBounds[3] << ", " << dataBounds[4] << ", " <<
                  dataBounds[5] << ")");

  vtkIdType ptId;
  double interpolatedHeight = 0.0;
  int numValidPoints;
  int y;
  int x;
  int val_id = 0;
  double offsetX = 0.0;
  double offsetY = 0.0;


  // Compute  points
  for (y = 0; y < this->PlotHeight + 1 ; y++)
  {
    for (x = 0; x < this->PlotWidth + 1 ; x++)
    {


      offsetX = 0.0;
      offsetY = 0.0;
      
      GetOffset(x, y, offsetX, offsetY);

      if ((x == 0) || (x >= this->PlotWidth))
      {
        offsetX = 0.0;
      }
      if ((y == 0) || (y >= this->PlotHeight))
      {
        offsetY = 0.0;
      }

      ptId = -1;
      
      if (x < this->PlotWidth)
      {
        if (y < this->PlotHeight)
        {
          ptId = points->InsertNextPoint(xData[x] - offsetX, yData[y] - offsetY, interpolatedHeight);
        }
        else
        {
          ptId = points->InsertNextPoint(xData[x] - offsetX, yData[this->PlotHeight - 1] + offsetY, interpolatedHeight);
        }
      }
      else
      {
        if (y < this->PlotHeight)
        {
          ptId = points->InsertNextPoint(xData[this->PlotWidth - 1] + offsetX, yData[y] - offsetY, interpolatedHeight);
        }
        else
        {
          ptId = points->InsertNextPoint(xData[this->PlotWidth - 1] + offsetX, yData[this->PlotHeight-1] + offsetY, interpolatedHeight);
        }
      
        //CTrace::Tracer("Intermediate\tInsert pointIds[%d]=%d at xData[%d]=%f, yData[%d]=%f", 
        //     val_id, 
        //     ptId, x, xData[x], y, yData[y]);
      }

      pointIds[val_id] = ptId;
      
      val_id++;
    }
  }


  // Compute plot points
  this->ComputePlotPolysPoints(points, this->InnerPlotPoint, dataBounds, viewportMappingNeeded, dataClippingNeeded, mappedPoints);

  long invalid = 0;
  long valid = 0;
  // Create Polys
  val_id = 0;
      
  for (y = 0; y < this->PlotHeight; y++)
  {
    for (x = 0; x < this->PlotWidth; x++)
    {
     
      ptId = val_id + y;

      if (viewportMappingNeeded)
      {
        double xyz1[3];
        double xyz2[3];
        double xyz3[3];
        double xyz4[3];
      
        mappedPoints->GetPoint(pointIds[ptId], xyz1);
        mappedPoints->GetPoint(pointIds[ptId + 1], xyz2);
        mappedPoints->GetPoint(pointIds[ptId + this->PlotWidth + 2], xyz3);
        mappedPoints->GetPoint(pointIds[ptId + this->PlotWidth + 1], xyz4);

        if ((xyz1[0] == xyz2[0]) &&  (xyz3[1] == xyz4[1])) 
        {
          this->InnerPlotPoint->SetValue(val_id, 0);
        }
      }


      numValidPoints = (pointIds[ptId] != -1) + 
          (pointIds[ptId + 1] != -1) +
          (pointIds[ptId + this->PlotWidth + 2] != -1) +
          (pointIds[ptId + this->PlotWidth + 1] != -1);

      if (IsValidPoint(val_id))
      {
        valid++;
	      vtkIdType polyId = polys->InsertNextCell(numValidPoints);
	      
        colors->InsertTuple1(polyId, values[val_id]);


        polys->InsertCellPoint(pointIds[ptId]);
        polys->InsertCellPoint(pointIds[ptId + 1]);
        polys->InsertCellPoint(pointIds[ptId + this->PlotWidth + 2]);
        polys->InsertCellPoint(pointIds[ptId + this->PlotWidth + 1]);

        
        //CTrace::Tracer("---->InsertNexCell values[%d]=%f", 
        //       val_id, values[val_id]);
          
      }
      else
      {
        invalid++;
      }

      val_id++;
    }
  }

  points->Delete();
  delete [] pointIds;

  // Update the PlotData, PlotGlyph and PlotAppend polydata
  this->PlotAppend->Update();

  // Update the output
  this->GetOutput()->ShallowCopy(this->PlotAppend->GetOutput());
    
}  

//----------------------------------------------------------------------------
void vtkZFXYPlotFilter::ComputePlotPolysPoints(vtkPoints *points, vtkShortArray *innerPlotPoint, double dataBounds[6],
                                               int viewportMappingNeeded, 
                                               int dataClippingNeeded,
                                               vtkPoints *mappedPoints)
{
    UNUSED( dataClippingNeeded );
    UNUSED( innerPlotPoint );

  vtkIdType ptId = -1;

  int numberOfDataPoints = points->GetNumberOfPoints();

  int logXYZ[3];
  logXYZ[0] = this->LogX;
  logXYZ[1] = this->LogY;
  logXYZ[2] = this->LogZ;
  
  double xyz[3];

  for (ptId = 0; ptId < numberOfDataPoints; ptId++)
  {
    points->GetPoint(ptId, xyz);

    if (vtkTools::IsNaN(xyz[0]) || vtkTools::IsNaN(xyz[1]))
    {
      continue;
    }

    if (viewportMappingNeeded)
    {

      vtkTools::DataToViewport3D(xyz, this->ViewportBounds, dataBounds, logXYZ);
      
      if (xyz[0] < ViewportBounds[0] || xyz[0] > ViewportBounds[1] ||
          xyz[1] < ViewportBounds[2] || xyz[1] > ViewportBounds[3] )
      {
        if (xyz[0] < ViewportBounds[0])
        {
          xyz[0] = ViewportBounds[0];
        }
        if (xyz[0] > ViewportBounds[1])
        {
          xyz[0] = ViewportBounds[1];
        }

        if (xyz[1] < ViewportBounds[2])
        {
          xyz[1] = ViewportBounds[2];
        }
        if (xyz[1] > ViewportBounds[3])
        {
          xyz[1] = ViewportBounds[3];
        }

      }
      
    }

    mappedPoints->InsertNextPoint(xyz);
    

  }

}



//----------------------------------------------------------------------------
void vtkZFXYPlotFilter::SetPlotSymbol(vtkPolyData *input)
{
  if (input != this->PlotGlyph->GetSource())
    {
    this->PlotGlyph->SetSource(input);
    this->Modified();
    }
}

//----------------------------------------------------------------------------
vtkPolyData *vtkZFXYPlotFilter::GetPlotSymbol()
{
  return this->PlotGlyph->GetSource();
}
//----------------------------------------------------------------------------
vtkGlyphSource2D* vtkZFXYPlotFilter::GetGlyphSource()
{
  return this->GlyphSource;
}

//----------------------------------------------------------------------------
void vtkZFXYPlotFilter::ComputePlotPoints(vtkPoints* pts,
                                    double dataBounds[6],
                                    int viewportMappingNeeded,
                                    int dataClippingNeeded)
{
  this->PlotPointsData->SetPoints(0);
  if (this->PlotPoints)
    {
    // PlotPointsData contains only those points that are within the
    // X, Y, and Z ranges.
    vtkDebugMacro(<<"  Calculating PlotPoints with DataClipping");
    vtkPoints* newPts = vtkPoints::New(VTK_DOUBLE);
    int numberOfPoints = 0;
    int logXYZ[3];
    logXYZ[0] = this->LogX;
    logXYZ[1] = this->LogY;
    logXYZ[2] = this->LogZ;
    for (vtkIdType ptId=0; ptId<pts->GetNumberOfPoints(); ptId++)
      {
      double xyz[3];
      pts->GetPoint(ptId, xyz);
      if (!vtkTools::IsNaN(xyz[0]) && !vtkTools::IsNaN(xyz[1]) && (!dataClippingNeeded ||
          (xyz[0] >= dataBounds[0] && xyz[0] <= dataBounds[1] &&
           xyz[1] >= dataBounds[2] && xyz[1] <= dataBounds[3] &&
           xyz[2] >= dataBounds[4] && xyz[2] <= dataBounds[5])))
        {
        if (viewportMappingNeeded)
          {
          vtkTools::DataToViewport3D(xyz, this->ViewportBounds, dataBounds, logXYZ);
          }
        newPts->InsertNextPoint(xyz);
        numberOfPoints++;
        }
      }
    if (numberOfPoints>0)
      {
      this->PlotPointsData->SetPoints(newPts);
      }
    newPts->Delete();
    // Set scale of the Glyph with respect to the viewport bounds
    this->PlotGlyph->SetScaleFactor(ComputeGlyphScale());
  }
}

//----------------------------------------------------------------------------
void vtkZFXYPlotFilter::ComputePlotLines(vtkPoints* pts,
                                   double dataBounds[6],
                                   int viewportMappingNeeded)
{
  this->PlotLinesData->SetLines(0);
  if (this->PlotLines)
    {
    vtkDebugMacro(<<"  Calculating PlotLines without DataClipping");
    int numberOfDataPoints = pts->GetNumberOfPoints();
    if (numberOfDataPoints>0)
      {
      int logXYZ[3];
      int numberOfPoints = 0; // Number of non-NaN points
      vtkPoints* newPts = vtkPoints::New(VTK_DOUBLE);
      vtkCellArray* lines = vtkCellArray::New(); 
      logXYZ[0] = this->LogX;
      logXYZ[1] = this->LogY;
      logXYZ[2] = this->LogZ;
      lines->InsertNextCell(numberOfDataPoints);
      for (vtkIdType ptId=0; ptId<numberOfDataPoints; ptId++)
        {
        double xyz[3];
        pts->GetPoint(ptId, xyz);
        if (!vtkTools::IsNaN(xyz[0]) && !vtkTools::IsNaN(xyz[1]))
          {
          if (viewportMappingNeeded)
            {
            vtkTools::DataToViewport3D(xyz, this->ViewportBounds, dataBounds, logXYZ);
            }
          vtkIdType newPtId = newPts->InsertNextPoint(xyz);
          lines->InsertCellPoint(newPtId);
          numberOfPoints++;
          }
        }
      if (numberOfPoints > 0)
        {
        this->PlotLinesData->SetPoints(newPts);
        this->PlotLinesData->SetLines(lines);
        }
      newPts->Delete();
      lines->Delete();
      }
    }
}

//----------------------------------------------------------------------------
void vtkZFXYPlotFilter::ComputePlotLinesWithClipping(vtkPoints* pts,
                                            double dataBounds[6],
                                            int viewportMappingNeeded)
{
  this->PlotLinesData->SetLines(0);
  if (this->PlotLines)
    {
    vtkDebugMacro(<<"  Calculating PlotLines with DataClipping");
    // The lines array contains the line segments that results form clipping
    // the original line with the X, Y, and Z ranges. Each linesegment is
    // contained within one cell and contains both the points from the dataset
    // and possible new intersection endpoints.
    int numberOfDataPoints = pts->GetNumberOfPoints();
    if (numberOfDataPoints>0)
      {
      vtkPoints* newPts = vtkPoints::New(VTK_DOUBLE);
      vtkCellArray* lines = vtkCellArray::New(); 
      int numberOfLines = 0;
      int numberOfPoints = 0; // Number of points within a line segment
      int logXYZ[3];
      double xyz1[3];
      double xyz2[3];
      logXYZ[0] = this->LogX;
      logXYZ[1] = this->LogY;
      logXYZ[2] = this->LogZ;
      pts->GetPoint(0, xyz1);
      // NaN values are considered outside the bounds
      int isnan1 = vtkTools::IsNaN(xyz1[0]) || vtkTools::IsNaN(xyz1[1]);
      int withinBounds1 = (!isnan1 &&
         xyz1[0] >= dataBounds[0] && xyz1[0] <= dataBounds[1] &&
         xyz1[1] >= dataBounds[2] && xyz1[1] <= dataBounds[3] &&
         xyz1[2] >= dataBounds[4] && xyz1[2] <= dataBounds[5]);
      for (vtkIdType ptId=0; ptId<numberOfDataPoints-1; ptId++)
        {
        pts->GetPoint(ptId+1, xyz2);
        int isnan2 = vtkTools::IsNaN(xyz2[0]) || vtkTools::IsNaN(xyz2[1]);
        int withinBounds2 = (!isnan2 &&
          xyz2[0] >= dataBounds[0] && xyz2[0] <= dataBounds[1] &&
          xyz2[1] >= dataBounds[2] && xyz2[1] <= dataBounds[3] &&
          xyz2[2] >= dataBounds[4] && xyz2[2] <= dataBounds[5]);
        if (withinBounds1)
          {
          double xyz3[3];
          if (!withinBounds2 && !isnan2)
            {
            // calculate intersection
            vtkTools::ComputeIntersection(dataBounds, xyz2, xyz1, xyz3);
            }
          // add xyz1 to points
          if (viewportMappingNeeded)
            {
            vtkTools::DataToViewport3D(xyz1, this->ViewportBounds, dataBounds,
                                   logXYZ);
            }
          vtkIdType newPtId = newPts->InsertNextPoint(xyz1);
          // add xyz1 to lines
          if (numberOfPoints == 0)
            {
            lines->InsertNextCell(0);
            }
          lines->InsertCellPoint(newPtId);
          numberOfPoints++;
          if (!withinBounds2) // Ending line
            {
            if (!isnan2) // Leaving the boundaries
              {
              // add intersection point to points
              if (viewportMappingNeeded)
                {
                vtkTools::DataToViewport3D(xyz3, this->ViewportBounds, dataBounds,
                                      logXYZ);
                }
              newPtId = newPts->InsertNextPoint(xyz3);
              // add intersection point to lines
              lines->InsertCellPoint(newPtId);
              numberOfPoints++;
              }
            // finish line segment
            lines->UpdateCellCount(numberOfPoints);
            numberOfLines++;
            numberOfPoints = 0;
            }
          }
        else if (!isnan1)
          {
          if (withinBounds2) // Entering the boundaries
            {
            double xyz3[3];
            // calculate intersection
            vtkTools::ComputeIntersection(dataBounds, xyz1, xyz2, xyz3);
            // add intersection point to points
            if (viewportMappingNeeded)
              {
              vtkTools::DataToViewport3D(xyz3, this->ViewportBounds, dataBounds,
                                     logXYZ);
              }
            vtkIdType newPtId = newPts->InsertNextPoint(xyz3);
            // add intersection point to lines starting a new line segment
            lines->InsertNextCell(0);
            lines->InsertCellPoint(newPtId);
            numberOfPoints = 1;
            }
          else if (!isnan2) // Staying outside the boundaries
            {
            // check if linesegment doesn't intersect with boundaries
            double xyz3[3];
            if(vtkTools::ComputeIntersection(dataBounds, xyz1, xyz2, xyz3))
              {
              double xyz4[3];
              // Calculate second intersectionpoint
              vtkTools::ComputeIntersection(dataBounds, xyz2, xyz1, xyz4);
              // add first intersection point to points
              if (viewportMappingNeeded)
                {
                vtkTools::DataToViewport3D(xyz3, this->ViewportBounds, dataBounds,
                                       logXYZ);
                }
              vtkIdType newPtId = newPts->InsertNextPoint(xyz3);
              // add first intersection point to lines
              lines->InsertNextCell(2);
              lines->InsertCellPoint(newPtId);
              // Only add the second intersection point to points if the
              // first and second intersection point are not the same
              // (i.e. we don't intersect one of the axes or corner points of
              // the bounding box)
              if (viewportMappingNeeded)
                {
                vtkTools::DataToViewport3D(xyz4, this->ViewportBounds, dataBounds,
                                       logXYZ);
                }
              if (xyz3[0]!=xyz4[0] || xyz3[1]!=xyz4[1] || xyz3[2]!=xyz4[2])
                {
                newPtId = newPts->InsertNextPoint(xyz4);
                }
              // add second intersection point to lines
              lines->InsertCellPoint(newPtId);
              // go to next line
              numberOfLines++;
              numberOfPoints = 0;
              }
            }
          }
        xyz1[0] = xyz2[0];
        xyz1[1] = xyz2[1];
        xyz1[2] = xyz2[2];
        withinBounds1 = withinBounds2;
        isnan1 = isnan2;
        }
      if (withinBounds1)
        {
        // Add last point to points
        if (viewportMappingNeeded)
          {
          vtkTools::DataToViewport3D(xyz1, this->ViewportBounds, dataBounds,
                                 logXYZ);
          }
        vtkIdType newPtId = newPts->InsertNextPoint(xyz1);
        // Add last point to lines
        if (numberOfPoints == 0)
          {
          lines->InsertNextCell(0);
          }
        lines->InsertCellPoint(newPtId);
        numberOfPoints++;
        }
      // Finish last line segment
      if (numberOfPoints>0)
        {
        lines->UpdateCellCount(numberOfPoints);
        numberOfLines++;
        }
      if (numberOfLines>0)
        {
        this->PlotLinesData->SetPoints(newPts);
        this->PlotLinesData->SetLines(lines);
        }
      newPts->Delete();
      lines->Delete();
      }
    }
}
  
//----------------------------------------------------------------------------
double vtkZFXYPlotFilter::ComputeGlyphScale()
{
  vtkPolyData *pd = this->PlotGlyph->GetSource();
  pd->Update();
  return this->GlyphSize * sqrt((double)
        (this->ViewportBounds[1]-this->ViewportBounds[0])*
        (this->ViewportBounds[1]-this->ViewportBounds[0]) +
        (this->ViewportBounds[3]-this->ViewportBounds[2])*
        (this->ViewportBounds[3]-this->ViewportBounds[2]) +
        (this->ViewportBounds[5]-this->ViewportBounds[4])*
        (this->ViewportBounds[5]-this->ViewportBounds[4])) / 
        pd->GetLength();
}



//----------------------------------------------------------------------------
void vtkZFXYPlotFilter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "OffsetY : " << this->OffsetY << std::endl;
  os << indent << "OffsetX : " << this->OffsetX << std::endl;
  os << indent << "MinMappedValue : " << this->MinMappedValue << std::endl;
  os << indent << "MaxMappedValue : " << this->MaxMappedValue << std::endl;
  os << indent << "PlotWidth : " << this->PlotWidth << std::endl;
  os << indent << "PlotHeight : " << this->PlotHeight << std::endl;
}
