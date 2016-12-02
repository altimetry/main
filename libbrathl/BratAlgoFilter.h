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
#if !defined(_BratAlgoFilter_h_)
#define _BratAlgoFilter_h_

#include "common/tools/brathl_error.h"
#include "brathl.h" 

#include <string>

#include "Tools.h" 
#include "BratAlgorithmBase.h" 

using namespace brathl;

namespace brathl
{

//-------------------------------------------------------------
//------------------- CBratAlgoFilter class --------------------
//-------------------------------------------------------------


class CBratAlgoFilter : public CBratAlgorithmBase 
{
public:
  /** Default contructor */
  CBratAlgoFilter();
  /** Copy contructor */
  CBratAlgoFilter(const CBratAlgoFilter	&copy);

  /** Destructor */
  virtual ~CBratAlgoFilter();


  /** Overloads operator '=' */
  CBratAlgoFilter& operator=(const CBratAlgoFilter &copy);

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr);
 
protected:

  void Init();
  void Set(const CBratAlgoFilter &o);
  //Save current values to previous values previous values.
  virtual void SetPreviousValues(bool fromProduct);
  virtual void SetNextValues();

  void SetBeginOfFile();
  void SetEndOfFile();

protected:





};


/** @} */
}

#endif // !defined(_BratAlgoFilter_h_)
