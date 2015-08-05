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

#ifndef __Plot_H__
#define __Plot_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "Plot.h"
#endif

// Include wxWindows' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "brathl.h"

#include "List.h"
#include "Tools.h"
#include "Unit.h"
#include "InternalFiles.h"
#include "InternalFilesYFX.h"
using namespace brathl;

class CPlotField;
#include "PlotField.h"
#include "PlotBase.h"

//#include "BitSet32.h"

//-------------------------------------------------------------
//------------------- CPlot class --------------------
//-------------------------------------------------------------
/** 
  A XY CPlot object management class

 \version 1.0
*/
class CPlot : public CPlotBase
{
public:
  CPlot(uint32_t groupNumber = 0);
  virtual ~CPlot();

  
  virtual void GetInfo();
  
  void GetAxisX(CInternalFiles* yfx, 
                 ExpressionValueDimensions* dimVal,
                 CExpressionValue* varX,
                 string* varXName);

  virtual CInternalFiles* GetInternalFiles(CBratObject* ob, bool withExcept = true);    
  static CInternalFilesYFX* GetInternalFilesYFX(CBratObject* ob);

public:
  

protected:
  void Init();

  
};

#endif
