/*
* 
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

#ifndef VTKBRATARROWSOURCE_H_INCLUDED
#define VTKBRATARROWSOURCE_H_INCLUDED

#include "vtkPolyDataAlgorithm.h"

#include "vtkCell.h" // Needed for VTK_CELL_SIZE

#include "vtkGlyph3D.h"

class VTK_GRAPHICS_EXPORT vtkBratArrowSource : public vtkPolyDataAlgorithm
{
public:
  static vtkBratArrowSource *New();
  vtkTypeRevisionMacro(vtkBratArrowSource,vtkPolyDataAlgorithm);
  void PrintSelf(std::ostream& os, vtkIndent indent);

  vtkSetMacro(TipRadius, double);
  vtkSetMacro(TipLength, double);

protected:
  vtkBratArrowSource();
  ~vtkBratArrowSource() {};

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
  vtkBratArrowSource(const vtkBratArrowSource&);  // Not implemented.
  void operator=(const vtkBratArrowSource&);  // Not implemented.

  double TipResolution, TipLength, TipRadius;
};


#endif // VTKBRATARROWSOURCE_H_INCLUDED
