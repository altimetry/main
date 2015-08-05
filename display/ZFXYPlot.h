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

#ifndef __ZFXYPlot_H__
#define __ZFXYPlot_H__


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ZFXYPlot.h"
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
#include "InternalFilesZFXY.h"
using namespace brathl;

class CPlotField;
#include "PlotField.h"

//#include "BitSet32.h"



//-------------------------------------------------------------
//------------------- CZFXYPlot class --------------------
//-------------------------------------------------------------
/** 
  A CZFXYPlot object management class

 \version 1.0
*/
class CZFXYPlot : public CPlotBase
{

public:

  CZFXYPlot(uint32_t groupNumber = 0);
  virtual ~CZFXYPlot();

  void GetPlotWidthHeight(CInternalFiles* zfxy, 
                           const string& fieldName,
                           int32_t& width, int32_t& height,
                           CExpressionValue& varY, CExpressionValue& varX,
                           uint32_t& dimRangeX, uint32_t& dimRangeY,
                           string& varXName, string& varYName);

  virtual CInternalFiles* GetInternalFiles(CBratObject* ob, bool withExcept = true);    

  static CInternalFilesZFXY* GetInternalFilesZFXY(CBratObject* ob, bool withExcept = true);
  
  static CInternalFilesYFX* GetInternalFilesYFX(CBratObject* ob, bool withExcept = true);

public:

  virtual void GetInfo();

public:
  

protected:
  void Init();



  
};


#endif
