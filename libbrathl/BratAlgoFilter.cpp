
/*
* Copyright (C) 2008 Collecte Localisation Satellites (CLS), France (31)
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
#include "brathl.h" 


#include "TraceLog.h" 

#include "BratAlgoFilter.h" 

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

//-------------------------------------------------------------
//------------------- CBratAlgoFilter class --------------------
//-------------------------------------------------------------

CBratAlgoFilter::CBratAlgoFilter()
{
  Init();

}
//----------------------------------------
CBratAlgoFilter::CBratAlgoFilter(const CBratAlgoFilter &copy)
{
  Init();

  Set(copy);

}

//----------------------------------------
CBratAlgoFilter::~CBratAlgoFilter()
{

}
//----------------------------------------
void CBratAlgoFilter::Init()
{
  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilter::Set(const CBratAlgoFilter &o)
{
  CBratAlgorithmBase::Set(o);

}


//----------------------------------------
CBratAlgoFilter& CBratAlgoFilter::operator=(const CBratAlgoFilter &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}

//----------------------------------------
void CBratAlgoFilter::SetBeginOfFile()
{
  CBratAlgorithmBase::SetBeginOfFile();

}

//----------------------------------------
void CBratAlgoFilter::SetEndOfFile()
{
  CBratAlgorithmBase::SetEndOfFile();

}
//----------------------------------------
void CBratAlgoFilter::SetPreviousValues(bool fromProduct) 
{
  CBratAlgorithmBase::SetPreviousValues(fromProduct);

}
//----------------------------------------
void CBratAlgoFilter::SetNextValues() 
{
  CBratAlgorithmBase::SetNextValues();
}

//----------------------------------------
void CBratAlgoFilter::Dump(ostream& fOut /*= cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgoFilter Object at "<< this << endl;
  CBratAlgorithmBase::Dump(fOut);

  fOut << "==> END Dump a CBratAlgoFilter Object at "<< this << endl;

}


} // end namespace
