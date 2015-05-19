/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
*
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


#include "vtkBratArrowSource.h"

#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkConeSource.h"
#include "vtkAppendPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkMath.h"
#include "vtkCellData.h"


#include <math.h>

vtkCxxRevisionMacro(vtkBratArrowSource, "$Revision: 1.55 $");
vtkStandardNewMacro(vtkBratArrowSource);

vtkBratArrowSource::vtkBratArrowSource ()
{
  this->TipResolution = 6;
  this->TipRadius = 0.1;
  this->TipLength = 0.35;
  this->SetNumberOfInputPorts(0);
}

int vtkBratArrowSource::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the ouptut
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  double pinit[3] = {0,0,0};
  double pend[3] = {1,0,0};

  vtkPoints *newPoints;
  vtkCellArray *newPolys;
  vtkPolyData *linePoly = vtkPolyData::New();

  newPoints = vtkPoints::New();
  newPoints->Allocate(2);

  newPolys = vtkCellArray::New();
  newPolys->Allocate(1);

  newPoints->InsertPoint(1,pinit);
  newPoints->InsertPoint(2,pend);

  newPolys->InsertNextCell(2);
  newPolys->InsertCellPoint(1);
  newPolys->InsertCellPoint(2);

  linePoly->SetPoints(newPoints);
  linePoly->SetLines(newPolys);


  vtkConeSource *cone = vtkConeSource::New();
  vtkAppendPolyData *append = vtkAppendPolyData::New();
  vtkTransform *trans1 = vtkTransform::New();
  vtkTransformFilter *tf1 = vtkTransformFilter::New();

  cone->SetResolution(this->TipResolution);
  cone->SetHeight(this->TipLength);
  cone->SetRadius(this->TipRadius);

  trans1->Translate(1.0-this->TipLength*0.5, 0.0, 0.0);
  tf1->SetTransform(trans1);
  tf1->SetInput(cone->GetOutput());

  append->AddInput(tf1->GetPolyDataOutput());
  append->AddInput(linePoly);

  append->Update();
  output->ShallowCopy(append->GetOutput());

  newPoints->Delete();
  newPolys->Delete();
  cone->Delete();
  trans1->Delete();
  tf1->Delete();

  return 1;
}

void vtkBratArrowSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

