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

#ifndef __WPlot_H__
#define __WPlot_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "WPlot.h"
#endif

#include "../libbrathl/brathl.h"
using namespace brathl;


namespace brathl{

	class CInternalFiles;
	class CInternalFilesZFXY;
}

#include "PlotBase.h"

//#include "BitSet32.h"



//-------------------------------------------------------------
//------------------- CWPlot class --------------------
//-------------------------------------------------------------
/** 
  A CWPlot object management class

 \version 1.0
*/
class CWPlot : public CPlotBase
{
public:
  CWPlot(uint32_t groupNumber = 0);
  virtual ~CWPlot();

  virtual CInternalFiles* GetInternalFiles(CBratObject* ob, bool withExcept = true);    

  static CInternalFilesZFXY* GetInternalFilesZFXY(CBratObject* ob, bool withExcept = true);

public:

  virtual void GetInfo();

protected:
  void Init();

  
};


#endif
