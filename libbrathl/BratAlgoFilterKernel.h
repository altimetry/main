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
#if !defined(_BratAlgoFilterKernel_h_)
#define _BratAlgoFilterKernel_h_

#include <string>

#include "common/tools/brathl_error.h"
#include "brathl.h" 

#include "Tools.h" 
#include "BratAlgoFilter.h" 

using namespace brathl;

namespace brathl
{

//-------------------------------------------------------------
//------------------- CBratAlgoFilterKernel class --------------------
//-------------------------------------------------------------


class CBratAlgoFilterKernel : public CBratAlgoFilter 
{
public:
  /** Default contructor */
  CBratAlgoFilterKernel();
  /** Copy contructor */
  CBratAlgoFilterKernel(const CBratAlgoFilterKernel	&copy);

  /** Destructor */
  virtual ~CBratAlgoFilterKernel();


  /** Overloads operator '=' */
  CBratAlgoFilterKernel& operator=(const CBratAlgoFilterKernel &copy);

  /** Dump function */
  virtual void Dump(std::ostream& fOut = std::cerr);
 
protected:

  void Init();
  void Set(const CBratAlgoFilterKernel &o);

  virtual void DeleteProduct();

  //Save current values to previous values previous values.
  virtual void SetPreviousValues(bool fromProduct);
  virtual void SetNextValues();
  
  virtual void OpenProductFile();
  void CheckProduct();
  
  virtual void CheckVarExpression(uint32_t index) = 0;

  virtual uint32_t GetDataWindowSize() = 0;


  void SetBeginOfFile();
  void SetEndOfFile();

  virtual void TreatLeftEdge1D(uint32_t shiftSymmetry, uint32_t index);
  virtual void TreatRightEdge1D(uint32_t shiftSymmetry, uint32_t index);
  virtual void RemoveFirstItemDataWindow1D();
  virtual void InsertCurrentValueDataWindow1D();

  virtual void PrepareDataValues1D(uint32_t shiftSymmetry);

  virtual void PrepareDataReading2D();

  virtual void PrepareDataValues2DOneField();
  virtual void PrepareDataValues2DComplexExpression();
  virtual void PrepareDataValues2DComplexExpressionWithAlgo();

  //virtual void PrepareDataWindow2D(CExpressionValue& exprValue);
  //virtual void PrepareDataWindow2D(double* dataValue, uint32_t nbValues);

  virtual void PrepareDataWindow2DSymmetrically(CExpressionValue& exprValue, uint32_t shiftSymmetry = 0);
  virtual void PrepareDataWindow2DSymmetrically(double* dataValue, size_t nbValues, uint32_t shiftSymmetry = 0);

  void SetParamValueExtrapolate(CVectorBratAlgorithmParam& args, uint32_t paramIndex);
  
protected:

  double m_varValue;
  double m_varValuePrev;
  double m_varValueNext;

  // The raw data window : it can contain 'default value' (no value)
  CDoubleArray m_rawDataWindow;
  // The data window from which the filter is applied: it doesn't contain 'default value'
  CDoubleArray m_dataWindow;

  //int32_t m_indexX;
  //int32_t m_indexY;

  //int32_t m_varDimXIndex;
  //int32_t m_varDimYIndex;

  uint32_t m_validPts;

  CIntArray m_expectedTypes;

  //-------------------------------------- 
  // Variables for input gridded data (2D)
  //-------------------------------------- 
  int32_t m_countX;
  int32_t m_countY;
  int32_t m_addHeightBottom;
  int32_t m_addHeightTop;
  int32_t m_addWidthRight;
  int32_t m_addWidthLeft;
  uint32_t m_gapWidth;
  uint32_t m_gapHeight;

  CFieldNetCdf* m_field2DAsRef;

  uint32_t m_dataWindowHeight;
  uint32_t m_dataWindowWidth;

  uint32_t m_dataWindowLength;

  uint32_t m_extrapolate;

  uint32_t m_lastIndexDataToInsert;

  static const std::string m_VAR_PARAM_DESCR;
  static const std::string m_VALID_PARAM_DESCR;
  static const std::string m_EXTRAPOLATE_PARAM_DESCR;

  static const std::string m_VAR_PARAM_NAME;
  static const std::string m_VALID_PARAM_NAME;
  static const std::string m_EXTRAPOLATE_PARAM_NAME;


  static const uint32_t m_VAR_PARAM_INDEX;


};


/** @} */
}

#endif // !defined(_BratAlgoFilterKernel_h_)
