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
#if !defined(_BratAlgoFilterLoess_h_)
#define _BratAlgoFilterLoess_h_

#include "common/tools/brathl_error.h"
#include "brathl.h" 

#include <string>

#include "Tools.h" 
#include "Product.h" 
#include "ProductNetCdf.h" 
#include "BratAlgoFilterKernel.h" 

using namespace brathl;

namespace brathl
{

//-------------------------------------------------------------
//------------------- CBratAlgoFilterLoess class --------------------
//-------------------------------------------------------------


class CBratAlgoFilterLoess : public CBratAlgoFilterKernel 
{
public:
  /** Default contructor */
  CBratAlgoFilterLoess();
  /** Copy contructor */
  CBratAlgoFilterLoess(const CBratAlgoFilterLoess	&copy);

  /** Destructor */
  virtual ~CBratAlgoFilterLoess();


  /** Overloads operator '=' */
  CBratAlgoFilterLoess& operator=(const CBratAlgoFilterLoess &copy);

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr);
 
protected:

  void Init();
  void Set(const CBratAlgoFilterLoess &o);
  //Save current values to previous values previous values.
  virtual void SetPreviousValues(bool fromProduct);
  virtual void SetNextValues();

  void SetBeginOfFile();
  void SetEndOfFile();

  void PrepareReturn() ;  

protected:

  double m_loess;

  CDoubleArray m_initialWeights;

  //CDoubleArray m_weights;


};


/** @} */
}

#endif // !defined(_BratAlgoFilterLoess_h_)
