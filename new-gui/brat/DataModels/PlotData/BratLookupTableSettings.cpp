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

#include "ApplicationLogger.h"

#include "BratLookupTableSettings.h"



void CBratLookupTableSettings::SaveToFile( const CBratLookupTable &table )
{
    UNUSED(table);
}
void CBratLookupTableSettings::SaveGradToFile( const CBratLookupTable &table )
{
    UNUSED(table);
}
void CBratLookupTableSettings::SaveCustToFile( const CBratLookupTable &table )
{
    UNUSED(table);
}

bool CBratLookupTableSettings::LoadFromFile( std::string &error_msg, CBratLookupTable &table )
{
    UNUSED(error_msg);
    UNUSED(table);
    return true;
}

bool CBratLookupTableSettings::LoadGradFromFile( std::string &error_msg, CBratLookupTable &table )
{
    UNUSED(error_msg);
    UNUSED(table);
    return true;
}
bool CBratLookupTableSettings::LoadCustFromFile( std::string &error_msg, CBratLookupTable &table )
{
    UNUSED(error_msg);
    UNUSED(table);
    return true;
}
