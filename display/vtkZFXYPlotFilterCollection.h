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

#ifndef __vtkZFXYPlotFilterCollection_h
#define __vtkZFXYPlotFilterCollection_h

#include "vtkCollection.h"
#include "vtkZFXYPlotFilter.h"

class VTK_EXPORT vtkZFXYPlotFilterCollection : public vtkCollection
{
public:
  static vtkZFXYPlotFilterCollection *New();
  vtkTypeMacro(vtkZFXYPlotFilterCollection,vtkCollection);

  // Description:
  // Add a plotdata item to the list.
  void AddItem(vtkZFXYPlotFilter *pd)
    { this->vtkCollection::AddItem((vtkObject *)pd); }

  // Description:
  // Get the next plotdata item in the list.
  vtkZFXYPlotFilter *GetNextItem()
    { return static_cast<vtkZFXYPlotFilter *>(this->GetNextItemAsObject()); }

  // Description:
  // Get the ith plotdata item in the list.
  vtkZFXYPlotFilter *GetItem(int i)
    { return static_cast<vtkZFXYPlotFilter *>(this->GetItemAsObject(i)); }
  
  // Description:
  // Replace the i'th item in the collection with a vtkZFXYPlotFilter object
  void ReplaceItem(int i, vtkZFXYPlotFilter *pd)
    { this->vtkCollection::ReplaceItem(i, (vtkObject *)pd); };

protected:  
  vtkZFXYPlotFilterCollection() {}
  ~vtkZFXYPlotFilterCollection() {}

private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->vtkCollection::AddItem(o); }

private:
  vtkZFXYPlotFilterCollection(const vtkZFXYPlotFilterCollection&);  // Not implemented.
  void operator=(const vtkZFXYPlotFilterCollection&);  // Not implemented.
};

#endif
