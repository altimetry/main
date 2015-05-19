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
#if !defined(_BratAlgoFilterGaussian1D_h_)
#define _BratAlgoFilterGaussian1D_h_

#include "brathl_error.h" 
#include "brathl.h" 

#include "Stl.h"

#include "Tools.h" 
#include "BratAlgoFilterGaussian.h" 


using namespace brathl;

namespace brathl
{
//-------------------------------------------------------------
//------------------- CBratAlgoFilterGaussian1D class --------------------
//-------------------------------------------------------------


/** \addtogroup Algorithm Algorithms classes
  @{ */

/** 
  Algorithm base class. 
*/

class CBratAlgoFilterGaussian1D : public CBratAlgoFilterGaussian 
{
public:
  /** Default contructor */
  CBratAlgoFilterGaussian1D();
  /** Copy contructor */
  CBratAlgoFilterGaussian1D(const CBratAlgoFilterGaussian1D	&copy);

  /** Destructor */
  virtual ~CBratAlgoFilterGaussian1D() {};

  virtual string GetName() { return "BratAlgoFilterGaussianAtp"; };
  
  virtual string GetDescription() { return "Gaussian filter for one-dimensional data (e.g. along-track data) as input data source. "
                                           "Gaussian filter is windowed filter of linear class, by its nature is weighted mean. "
                                           "Gaussian filter is a type of data-smoothing filter that uses a Gaussian function (normal distribution) for calculating the transformation to apply to each data. "
                                           "The output value unit depends on the variable (data) filtered" ; };
  
  virtual double Run(CVectorBratAlgorithmParam& args);

    /** Overloads operator '=' */
  CBratAlgoFilterGaussian1D& operator=(const CBratAlgoFilterGaussian1D &copy);

  virtual uint32_t GetDataWindowSize() { return m_dataWindowLength; };

  /** Dump function */
  virtual void Dump(ostream& fOut = cerr);
 
protected:

  void Init();
  void Set(const CBratAlgoFilterGaussian1D &copy);
    
  virtual void SetPreviousValues(bool fromProduct);
  virtual void SetNextValues();

  virtual void CheckVarExpression(uint32_t index);

  void SetBeginOfFile();
  void SetEndOfFile();

  //virtual void GetNextData(uint32_t gapFromCurrent);

  double ComputeGaussian();


protected:

};


/** @} */
}


#endif // !defined(_BratAlgoFilterGaussian1D_h_)
