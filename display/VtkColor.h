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

#ifndef __VtkColor_H__
#define __VtkColor_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "VtkColor.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include "brathl.h"
#include "BratObject.h"
#include "Tools.h"
using namespace brathl;

//-------------------------------------------------------------
//------------------- CVtkColor class --------------------
//-------------------------------------------------------------

class CVtkColor : public CBratObject
{
public:
  CVtkColor();

  CVtkColor(double r, double g, double b, double a = 1);
  CVtkColor(int32_t r, int32_t g, int32_t b, int32_t a = 255);
  CVtkColor(const CVtkColor& vtkColor);
  CVtkColor(const wxColour& color);

  ~CVtkColor();

  const CVtkColor& operator =(const CVtkColor& vtkColor);
  const CVtkColor& operator =(const wxColour& color);
  const CVtkColor& operator =(wxColourData& color);

  bool operator ==(const CVtkColor& color);

  void Get(double& r, double& g, double& b, double& a) const;
  void Get(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const;
  void Get(uint32_t& r, uint32_t& g, uint32_t& b, uint32_t& a) const;
  void Get(int32_t& r, int32_t& g, int32_t& b, int32_t& a) const;
  void Get(wxColour& color) const;

  void Set(double r, double g, double b, double a = 1);
  void Set(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
  void Set(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 255);
  void Set(int32_t r, int32_t g, int32_t b, int32_t a = 255);
  void Set(const CVtkColor& vtkColor);
  void Set(const wxColour& color);
  void Set(wxColourData& colorData, int32_t indexCustomColor = -1);

  bool Ok() const {return m_isInit;};
  void Reset();

  wxColour GetWXColor() const;

  double Red() const {return m_r;};
  double Green() const {return m_g;};
  double Blue() const {return m_b;};
  double Alpha() const {return m_a;};

private:
  double m_r;
  double m_g;
  double m_b;
  double m_a;

  bool m_isInit;
};



#endif
