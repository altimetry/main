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

#ifndef __WindowHandler_H__
#define __WindowHandler_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "WindowHandler.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h"

#include "BratObject.h"
using namespace brathl;

//#include "BratDisplay_wdr.h"

// WDR: class declarations


class CWindowHandler : public CBratObject
{
public:
    
  CWindowHandler();

  ~CWindowHandler();
  
  static wxSize GetDefaultSize();

  static void GetSizeAndPosition(wxSize& size, wxPoint& pos);


protected:

  static int32_t m_locx;
  static int32_t m_locy;
  static int32_t m_offset;


};

#endif
