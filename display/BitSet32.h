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

#ifndef __BitSet32_H__
#define __BitSet32_H__

// Include wxWindows' headers


#include "brathl.h"

#include <string>

#include "BratObject.h"
#include "Tools.h"
using namespace brathl;

//-------------------------------------------------------------
//------------------- CBitSet32 class --------------------
//-------------------------------------------------------------

class CBitSet32 : public CBratObject
{
public:
  CBitSet32();
  CBitSet32(const CBitSet32& b);

  CBitSet32(unsigned long value);
  virtual ~CBitSet32();
  virtual const CBitSet32& operator =(const CBitSet32& b);

  bitSet32 m_bitSet;

};

#endif
