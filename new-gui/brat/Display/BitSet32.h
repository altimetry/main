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

#ifndef COMMON_BITSET32_H
#define COMMON_BITSET32_H

#include "BratObject.h"
#include "Tools.h"

using namespace brathl;

//-------------------------------------------------------------
//------------------- CBitSet32 class --------------------
//-------------------------------------------------------------


struct CBitSet32 : public CBratObject
{
	bitSet32 m_bitSet;


	CBitSet32()
	{}
	CBitSet32( const CBitSet32 &b )
	{
		*this = b;
	}
	const CBitSet32& operator =( const CBitSet32 &o )
	{
		if ( this != &o )
			m_bitSet = o.m_bitSet;

		return *this;
	}

	CBitSet32( unsigned long value )
		: m_bitSet( (int)value )
	{}

	virtual ~CBitSet32()
	{}
};


#endif			//	COMMON_BITSET32_H
