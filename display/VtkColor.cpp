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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "VtkColor.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "VtkColor.h"

//----------------------------------------
CVtkColor::CVtkColor()
{
  Reset();
}

//----------------------------------------
CVtkColor::CVtkColor(double r, double g, double b, double a )
{
  Set(r, g, b, a);
}

//----------------------------------------
CVtkColor::CVtkColor(int32_t r, int32_t g, int32_t b, int32_t a)
{
  Set(r, g, b, a);
}

//----------------------------------------
CVtkColor::CVtkColor(const CVtkColor& vtkColor)
{
  Set(vtkColor);
}

//----------------------------------------
CVtkColor::CVtkColor(const wxColour& color)
{
  Set(color);
}

//----------------------------------------
CVtkColor::~CVtkColor()
{

}

//----------------------------------------
const CVtkColor& CVtkColor::operator =(const CVtkColor& vtkColor)
{
  Set(vtkColor);
  return *this;
}

//----------------------------------------
const CVtkColor& CVtkColor::operator =(const wxColour& color)
{
  Set(color);
  return *this;
}
//----------------------------------------
const CVtkColor& CVtkColor::operator =(wxColourData& color)
{
  Set(color);
  return *this;
}
//----------------------------------------
bool CVtkColor::operator ==(const CVtkColor& vtkColor)
{
  bool bEqual = true;
  bEqual &= (this->Red() == vtkColor.Red());
  bEqual &= (this->Green() == vtkColor.Green());
  bEqual &= (this->Blue() == vtkColor.Blue());
  bEqual &= (this->Alpha() == vtkColor.Alpha());
  bEqual &= (this->Ok() == vtkColor.Ok());

  return bEqual;
}

//----------------------------------------
void CVtkColor::Get(double& r, double& g, double& b, double& a) const
{
  r = m_r;
  g = m_g;
  b = m_b;
  a = m_a;
}

//----------------------------------------
void CVtkColor::Get(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const
{
  r = static_cast<uint8_t>(m_r * 255);
  g = static_cast<uint8_t>(m_g * 255);
  b = static_cast<uint8_t>(m_b * 255);
  a = static_cast<uint8_t>(m_a * 255);

}

//----------------------------------------
void CVtkColor::Get(uint32_t& r, uint32_t& g, uint32_t& b, uint32_t& a) const
{
  r = static_cast<uint32_t>(m_r * 255);
  g = static_cast<uint32_t>(m_g * 255);
  b = static_cast<uint32_t>(m_b * 255);
  a = static_cast<uint32_t>(m_a * 255);
}

//----------------------------------------
void CVtkColor::Get(int32_t& r, int32_t& g, int32_t& b, int32_t& a) const
{
  r = static_cast<int32_t>(m_r * 255);
  g = static_cast<int32_t>(m_g * 255);
  b = static_cast<int32_t>(m_b * 255);
  a = static_cast<int32_t>(m_a * 255);
}

//----------------------------------------
void CVtkColor::Get(wxColour& color) const
{
  if (Ok() == false)
  {
    return;
  }
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
  Get(r, g, b, a);
  color.Set(r,g,b);
}

//----------------------------------------
void CVtkColor::Set(double r, double g, double b, double a)
{
  m_r = r;
  m_g = g;
  m_b = b;
  m_a = a;
  m_isInit = true;
}

//----------------------------------------
void CVtkColor::Set(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  Set(static_cast<uint32_t>(r),
      static_cast<uint32_t>(g),
      static_cast<uint32_t>(b),
      static_cast<uint32_t>(a));

}

//----------------------------------------
void CVtkColor::Set(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
{
  m_r = static_cast<double>(r) / 255.0;
  m_g = static_cast<double>(g) / 255.0;
  m_b = static_cast<double>(b) / 255.0;
  m_a = static_cast<double>(a) / 255.0;
  m_isInit = true;

}

//----------------------------------------
void CVtkColor::Set(int32_t r, int32_t g, int32_t b, int32_t a)
{
  r = r < 0 ? 0 : r;
  g = g < 0 ? 0 : g;
  b = b < 0 ? 0 : b;
  a = a < 0 ? 255 : a;
  m_r = static_cast<double>(r) / 255.0;
  m_g = static_cast<double>(g) / 255.0;
  m_b = static_cast<double>(b) / 255.0;
  m_a = static_cast<double>(a) / 255.0;
  m_isInit = true;

}

//----------------------------------------
void CVtkColor::Set(const CVtkColor& vtkColor)
{
  m_r = vtkColor.m_r;
  m_g = vtkColor.m_g;
  m_b = vtkColor.m_b;
  m_a = vtkColor.m_a;
  m_isInit = true;

}

//----------------------------------------
void CVtkColor::Set(const wxColour& color)
{
  Set(color.Red(), color.Green(), color.Blue());
}

//----------------------------------------
void CVtkColor::Set(wxColourData& colorData, int32_t indexCustomColor)
{
  if (indexCustomColor < 0)
  {
    Set(colorData.GetColour());
  }
  else
  {
    Set(colorData.GetCustomColour(indexCustomColor));
  }
}
//----------------------------------------
void CVtkColor::Reset()
{
  m_r = 0.0;
  m_g = 0.0;
  m_b = 0.0;
  m_a = 1.0;
  m_isInit = false;
}
//----------------------------------------
wxColour CVtkColor::GetWXColor() const
{
  wxColour color;
  Get(color);
  return color;
}
