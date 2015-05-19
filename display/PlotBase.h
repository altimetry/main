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

#ifndef __PlotBase_H__
#define __PlotBase_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "PlotBase.h"
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

//#include "BitSet32.h"

//-------------------------------------------------------------
//------------------- CPlotBase class --------------------
//-------------------------------------------------------------
/** 
  A plot object management base class

 \version 1.0
*/
class CPlotBase : public CBratObject
{
public:

  CPlotBase(uint32_t groupNumber = 0);
  virtual ~CPlotBase();

  CPlotField* GetPlotField(int32_t index);

  CPlotField* FindPlotField(const wxString& fieldName, bool* withContour = NULL, bool* withSolidColor = NULL);
  //CPlotField* FindPlotField(const wxString& fieldName, bool withContour);

  void GetAllInternalFiles(CObArray& allInternalFiles);

  virtual CInternalFiles* GetInternalFiles(CBratObject* ob, bool withExcept = true) = 0;
  virtual void GetInfo() = 0;

  void SetForcedVarXname(const wxString& value) { m_forcedVarXName = value; };
  wxString GetForcedVarXname() { return m_forcedVarXName; };

  void SetForcedVarYname(const wxString& value) { m_forcedVarYName = value; };
  wxString GetForcedVarYname() { return m_forcedVarYName; };

  virtual void GetVar(const string& varName, CInternalFiles* file, ExpressionValueDimensions* dimVal, CExpressionValue* var);

  virtual void GetForcedAxisX(CInternalFiles* file, ExpressionValueDimensions* dimVal, CExpressionValue* varX);
  virtual void GetForcedAxisY(CInternalFiles* file, ExpressionValueDimensions* dimVal, CExpressionValue* varY);


public:

  uint32_t m_groupNumber;

  wxString m_title;

  // a map that contains CPlotField object
  CObArray m_fields;
  CStringArray m_nonPlotFieldNames;

  wxString m_titleX;
  wxString m_titleY;

  CUnit m_unitX;
  CUnit m_unitY;

  bool m_unitXConv;
  bool m_unitYConv;

  
  wxString m_forcedVarXName;
  wxString m_forcedVarYName;

  wxString m_unitXLabel;
  wxString m_unitYLabel;

protected:


private:

  void Init();

};

#endif
