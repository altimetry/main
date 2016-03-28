/*
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
#include "new-gui/brat/stdafx.h"

#include "PlotColor.h"

//----------------------------------------
CPlotColor::CPlotColor()
{
  Reset();
}

//----------------------------------------
CPlotColor::CPlotColor(double r, double g, double b, double a )
{
  Set(r, g, b, a);
}

//----------------------------------------
CPlotColor::CPlotColor(const CPlotColor& color)
{
  Set(color);
}

//----------------------------------------
/*femmTODO	- uncomment
CPlotColor::CPlotColor(const wxColour& color)
{
  Set(color);
}
femmTODO	- uncomment */

//----------------------------------------
CPlotColor::~CPlotColor()
{

}

//----------------------------------------
const CPlotColor& CPlotColor::operator =(const CPlotColor& color)
{
  Set(color);
  return *this;
}

//----------------------------------------
/*femmTODO	- as inline function color_cast
const CPlotColor& CPlotColor::operator =(const wxColour& color)
{
  Set(color);
  return *this;
}
//----------------------------------------
const CPlotColor& CPlotColor::operator =(wxColourData& color)
{
  Set(color);
  return *this;
}
femmTODO */
//----------------------------------------
bool CPlotColor::operator ==(const CPlotColor& color)
{
  bool bEqual = true;
  bEqual &= (this->Red() == color.Red());
  bEqual &= (this->Green() == color.Green());
  bEqual &= (this->Blue() == color.Blue());
  bEqual &= (this->Alpha() == color.Alpha());
  bEqual &= (this->Ok() == color.Ok());

  return bEqual;
}

//----------------------------------------
void CPlotColor::Get(double& r, double& g, double& b, double& a) const
{
  r = m_r;
  g = m_g;
  b = m_b;
  a = m_a;
}

//----------------------------------------
void CPlotColor::Get(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const
{
  r = static_cast<uint8_t>(m_r * 255);
  g = static_cast<uint8_t>(m_g * 255);
  b = static_cast<uint8_t>(m_b * 255);
  a = static_cast<uint8_t>(m_a * 255);

}

//----------------------------------------
void CPlotColor::Get(uint32_t& r, uint32_t& g, uint32_t& b, uint32_t& a) const
{
  r = static_cast<uint32_t>(m_r * 255);
  g = static_cast<uint32_t>(m_g * 255);
  b = static_cast<uint32_t>(m_b * 255);
  a = static_cast<uint32_t>(m_a * 255);
}

//----------------------------------------
void CPlotColor::Get(int& r, int& g, int& b, int& a) const
{
  r = static_cast<int>(m_r * 255);
  g = static_cast<int>(m_g * 255);
  b = static_cast<int>(m_b * 255);
  a = static_cast<int>(m_a * 255);
}

//----------------------------------------
/*femmTODO	- as inline function color_cast
void CPlotColor::Get(wxColour& color) const
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
femmTODO */

//----------------------------------------
void CPlotColor::Set(double r, double g, double b, double a)
{
  m_r = r;
  m_g = g;
  m_b = b;
  m_a = a;
  m_isInit = true;
}

//----------------------------------------
void CPlotColor::Set(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  Set(static_cast<uint32_t>(r),
      static_cast<uint32_t>(g),
      static_cast<uint32_t>(b),
      static_cast<uint32_t>(a));

}

//----------------------------------------
void CPlotColor::Set(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
{
  m_r = static_cast<double>(r) / 255.0;
  m_g = static_cast<double>(g) / 255.0;
  m_b = static_cast<double>(b) / 255.0;
  m_a = static_cast<double>(a) / 255.0;
  m_isInit = true;

}

//----------------------------------------
void CPlotColor::Set(int r, int g, int b, int a)
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
void CPlotColor::Set(const CPlotColor& color)
{
  m_r = color.m_r;
  m_g = color.m_g;
  m_b = color.m_b;
  m_a = color.m_a;
  m_isInit = true;

}

//----------------------------------------
/*femmTODO	- as inline function color_cast
void CPlotColor::Set(const wxColour& color)
{
  Set(color.Red(), color.Green(), color.Blue());
}
//----------------------------------------
void CPlotColor::Set(wxColourData& colorData, int indexCustomColor)
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
femmTODO */
//----------------------------------------
void CPlotColor::Reset()
{
  m_r = 0.0;
  m_g = 0.0;
  m_b = 0.0;
  m_a = 1.0;
  m_isInit = false;
}
//----------------------------------------
/*femmTODO	- as inline function color_cast
wxColour CPlotColor::GetWXColor() const
{
  wxColour color;
  Get(color);
  return color;
}
femmTODO */
