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
* -----------------------------------------------------------------------------
* There is a bug in vtkPointLocator 
* =================================
* in BuildLocator method which can potentially access a negative index into the Hashtable array 
* (buckets) if there are points in the dataset which fall outside the 
* bounds of the dataset. 
*/
#include "vtkPointLocatorBrat.h"

#include "vtkCellArray.h"
#include "vtkIdList.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"

vtkStandardNewMacro(vtkPointLocatorBrat);



// Construct with automatic computation of divisions, averaging
// 25 points per bucket.
vtkPointLocatorBrat::vtkPointLocatorBrat()
{

}

vtkPointLocatorBrat::~vtkPointLocatorBrat()
{
}

//
//  Method to form subdivision of space based on the points provided and
//  subject to the constraints of levels and NumberOfPointsPerBucket.
//  The result is directly addressable and of uniform subdivision.
//
void vtkPointLocatorBrat::BuildLocator()
{
  double *bounds;
  vtkIdType numBuckets;
  double level;
  int ndivs[3], product;
  int i, j, ijk[3];
  vtkIdType idx;
  vtkIdList *bucket;
  vtkIdType numPts;
  double *x;
  typedef vtkIdList *vtkIdListPtr;

  if ( (this->HashTable != NULL) && (this->BuildTime > this->MTime)
       && (this->BuildTime > this->DataSet->GetMTime()) )
    {
    return;
    }

  vtkDebugMacro( << "Hashing points..." );
  this->Level = 1; //only single lowest level

  if ( !this->DataSet || (numPts = this->DataSet->GetNumberOfPoints()) < 1 )
    {
    vtkErrorMacro( << "No points to subdivide");
    return;
    }
  //
  //  Make sure the appropriate data is available
  //
  if ( this->HashTable )
    {
    this->FreeSearchStructure();
    }
  //
  //  Size the root bucket.  Initialize bucket data structure, compute 
  //  level and divisions.
  //
  bounds = this->DataSet->GetBounds();
  for (i=0; i<3; i++)
    {
    this->Bounds[2*i] = bounds[2*i];
    this->Bounds[2*i+1] = bounds[2*i+1];
    if ( this->Bounds[2*i+1] <= this->Bounds[2*i] ) //prevent zero width
      {
      this->Bounds[2*i+1] = this->Bounds[2*i] + 1.0;
      }
    }

  if ( this->Automatic ) 
    {
    level = (double) numPts / this->NumberOfPointsPerBucket;
    level = ceil( pow((double)level,(double)0.33333333) );
    for (i=0; i<3; i++)
      {
      ndivs[i] = (int) level;
      }
    } 
  else 
    {
    for (i=0; i<3; i++)
      {
      ndivs[i] = (int) this->Divisions[i];
      }
    }

  for (i=0; i<3; i++) 
    {
    ndivs[i] = (ndivs[i] > 0 ? ndivs[i] : 1);
    this->Divisions[i] = ndivs[i];
    }

  this->NumberOfBuckets = numBuckets = ndivs[0]*ndivs[1]*ndivs[2];
  this->HashTable = new vtkIdListPtr[numBuckets];
  memset (this->HashTable, 0, numBuckets*sizeof(vtkIdListPtr));
  //
  //  Compute width of bucket in three directions
  //
  for (i=0; i<3; i++) 
    {
    this->H[i] = (this->Bounds[2*i+1] - this->Bounds[2*i]) / ndivs[i] ;
    }
  //
  //  Insert each point into the appropriate bucket.  Make sure point
  //  falls within bucket.
  //
  product = ndivs[0]*ndivs[1];
  for (i=0; i<numPts; i++) 
    {
    x = this->DataSet->GetPoint(i);
    for (j=0; j<3; j++) 
      {
      ijk[j] = (int) ((double) ((x[j] - this->Bounds[2*j]) / 
                        (this->Bounds[2*j+1] - this->Bounds[2*j])) * ndivs[j]);
      if (ijk[j] >= this->Divisions[j])
        {
        ijk[j] = this->Divisions[j] - 1;
        }
      }

    idx = ijk[0] + ijk[1]*ndivs[0] + ijk[2]*product;

// There is a bug in vtkPointLocator 
//=================================
// in BuildLocator method which can potentially access a negative index into the Hashtable array 
// (buckets) if there are points in the dataset which fall outside the 
// bounds of the dataset. 
    if (idx > -1)
      { // make sure valid bucket id, negative if point outside
          // dataset bounds.../*
      bucket = this->HashTable[idx];
      if ( ! bucket )
          {
          bucket = vtkIdList::New();
          bucket->Allocate(this->NumberOfPointsPerBucket,
                           this->NumberOfPointsPerBucket/3);
          this->HashTable[idx] = bucket;
          }
        bucket->InsertNextId(i);
        }
      }

  this->BuildTime.Modified();
}



