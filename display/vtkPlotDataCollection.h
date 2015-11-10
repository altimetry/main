/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPlotDataCollection.h
  Language:  C++
  Date:      -
  Version:   S&T

    *** THIS FILE WAS CREATED BY SCIENCE & TECHNOLOGY BV ***

  Copyright (C) 2002-2005 Science & Technology BV, The Netherlands.

  This file is part of VISAN.

  VISAN is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  VISAN is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
                     
  You should have received a copy of the GNU General Public License
  along with VISAN; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

=========================================================================*/
#ifndef __vtkPlotDataCollection_h
#define __vtkPlotDataCollection_h

#include "vtkCollection.h"
#include "vtkPlotData.h"

class VTK_EXPORT vtkPlotDataCollection : public vtkCollection
{
public:
  static vtkPlotDataCollection *New();
  vtkTypeMacro(vtkPlotDataCollection,vtkCollection);

  // Description:
  // Add a plotdata item to the std::list.
  void AddItem(vtkPlotData *pd)
    { this->vtkCollection::AddItem((vtkObject *)pd); }

  // Description:
  // Get the next plotdata item in the std::list.
  vtkPlotData *GetNextItem()
    { return static_cast<vtkPlotData *>(this->GetNextItemAsObject()); }

  // Description:
  // Get the ith plotdata item in the std::list.
  vtkPlotData *GetItem(int i)
    { return static_cast<vtkPlotData *>(this->GetItemAsObject(i)); }

protected:  
  vtkPlotDataCollection() {}
  ~vtkPlotDataCollection() {}

private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->vtkCollection::AddItem(o); }

private:
  vtkPlotDataCollection(const vtkPlotDataCollection&);  // Not implemented.
  void operator=(const vtkPlotDataCollection&);  // Not implemented.
};

#endif
