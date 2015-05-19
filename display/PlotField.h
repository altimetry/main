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

#ifndef __PlotField_H__
#define __PlotField_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "PlotField.h"
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

class CXYPlotProperty;
class CWorldPlotProperty;
class CZFXYPlotProperty;
class CPlot;
class CWPlot;
class CZFXYPlot;
#include "PlotBase.h"
#include "Plot.h"
#include "WPlot.h"
#include "ZFXYPlot.h"
#include "XYPlotData.h"
#include "WorldPlotData.h"
#include "ZFXYPlotData.h"

//-------------------------------------------------------------
//------------------- CPlotField class --------------------
//-------------------------------------------------------------
/** 
  Class to manage field and their associated internal files 

 \version 1.0
*/
class CPlotField : public CBratObject
{
public:
  CPlotField(const wxString& name);
  virtual ~CPlotField();
  
  static CPlotField* GetPlotField(CBratObject* ob);

  CInternalFiles* GetInternalFiles(int32_t index);
  CInternalFilesYFX* GetInternalFilesYFX(int32_t index);

  wxString m_name;
  CObArray m_internalFiles;    
  
  CXYPlotProperty* m_xyProps;
  CWorldPlotProperty* m_worldProps;
  CZFXYPlotProperty* m_zfxyProps;


private :
  void Init();

};


#endif
