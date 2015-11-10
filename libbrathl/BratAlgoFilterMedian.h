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
#if !defined(_BratAlgoFilterMedian_h_)
#define _BratAlgoFilterMedian_h_

#include "brathl_error.h" 
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
//------------------- CBratAlgoFilterMedian class --------------------
//-------------------------------------------------------------


class CBratAlgoFilterMedian : public CBratAlgoFilterKernel 
{
public:
  /** Default contructor */
  CBratAlgoFilterMedian();
  /** Copy contructor */
  CBratAlgoFilterMedian(const CBratAlgoFilterMedian	&copy);

  /** Destructor */
  virtual ~CBratAlgoFilterMedian();


  /** Overloads operator '=' */
  CBratAlgoFilterMedian& operator=(const CBratAlgoFilterMedian &copy);

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr);
 
protected:

  void Init();
  void Set(const CBratAlgoFilterMedian &o);
  //Save current values to previous values previous values.
  virtual void SetPreviousValues(bool fromProduct);
  virtual void SetNextValues();

  void SetBeginOfFile();
  void SetEndOfFile();

  void PrepareReturn() ;

  virtual double ComputeMedian();
  

protected:

  double m_median;

};


/** @} */
}

#endif // !defined(_BratAlgoFilterMedian_h_)
