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
#if !defined(_BratAlgoFilterLanczos1D_h_)
#define _BratAlgoFilterLanczos1D_h_

#include "brathl_error.h" 
#include "brathl.h" 

#include "Stl.h"

#include "Tools.h" 
#include "BratAlgoFilterLanczos.h" 


using namespace brathl;

namespace brathl
{
//-------------------------------------------------------------
//------------------- CBratAlgoFilterLanczos1D class --------------------
//-------------------------------------------------------------


/** \addtogroup Algorithm Algorithms classes
  @{ */

/** 
  Algorithm base class. 
*/

class CBratAlgoFilterLanczos1D : public CBratAlgoFilterLanczos 
{
public:
  /** Default contructor */
  CBratAlgoFilterLanczos1D();
  /** Copy contructor */
  CBratAlgoFilterLanczos1D(const CBratAlgoFilterLanczos1D	&copy);

  /** Destructor */
  virtual ~CBratAlgoFilterLanczos1D() {};

  virtual string GetName() { return "BratAlgoFilterLanczosAtp"; };
  
  virtual string GetDescription() { return "Lanczos filter for one-dimensional data (e.g. along-track data) as input data source. "
                                           "Lanczos filter is low-pass windowed filter. The Lanczos window is the central lobe of a horizontally-stretched sinc, sinc(X/a) for -a <= X <= a, with (2 x a) = window length."
                                           "Thus the Lanczos filter, on its interval, is a product of two sinc functions. The resulting function is then used as a convolution kernel to resample the input field. "
                                           "The output value unit depends on the variable (data) filtered" ; };
  
  virtual double Run(CVectorBratAlgorithmParam& args);

    /** Overloads operator '=' */
  CBratAlgoFilterLanczos1D& operator=(const CBratAlgoFilterLanczos1D &copy);

  virtual uint32_t GetDataWindowSize() { return m_dataWindowLength; };

  /** Dump function */
  virtual void Dump(ostream& fOut = cerr);
 
protected:

  void Init();
  void Set(const CBratAlgoFilterLanczos1D &copy);
    
  virtual void SetPreviousValues(bool fromProduct);
  virtual void SetNextValues();

  virtual void CheckVarExpression(uint32_t index);

  void SetBeginOfFile();
  void SetEndOfFile();

  double ComputeLanczos();


protected:

};


/** @} */
}


#endif // !defined(_BratAlgoFilterLanczos1D_h_)
