
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
#include "brathl.h" 


#include "TraceLog.h" 
#include "Exception.h" 
#include "Expression.h" 
#include "Product.h" 
#include "ProductNetCdf.h" 

#include "BratAlgoFilterKernel.h" 

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{
  const uint32_t CBratAlgoFilterKernel::m_VAR_PARAM_INDEX = 0;

  const std::string CBratAlgoFilterKernel::m_VAR_PARAM_DESCR = "The input data (variable or Brat expression) on which the filter is applied.";
  const std::string CBratAlgoFilterKernel::m_VALID_PARAM_DESCR = "The minimum number of valid points below which the algorithm is not applied. The parameter must be a constant value.";
  const std::string CBratAlgoFilterKernel::m_EXTRAPOLATE_PARAM_DESCR = "A flag to specify if the algorithm is applied when the current data is 'defaut value' (no value). "
                                                                  "0: not applied, 1: applied.";

  const std::string CBratAlgoFilterKernel::m_VAR_PARAM_NAME = "Expr";
  const std::string CBratAlgoFilterKernel::m_VALID_PARAM_NAME = "ValidPts";
  const std::string CBratAlgoFilterKernel::m_EXTRAPOLATE_PARAM_NAME = "Extrapolate";

//-------------------------------------------------------------
//------------------- CBratAlgoFilterKernel class --------------------
//-------------------------------------------------------------

CBratAlgoFilterKernel::CBratAlgoFilterKernel()
{
  Init();

}
//----------------------------------------
CBratAlgoFilterKernel::CBratAlgoFilterKernel(const CBratAlgoFilterKernel &copy)
{
  Init();

  Set(copy);

}

//----------------------------------------
CBratAlgoFilterKernel::~CBratAlgoFilterKernel()
{
}
//----------------------------------------
void CBratAlgoFilterKernel::Init()
{
  CTools::SetDefaultValue(m_dataWindowHeight);
  CTools::SetDefaultValue(m_dataWindowWidth);

  CTools::SetDefaultValue(m_dataWindowLength);
  CTools::SetDefaultValue(m_extrapolate);

  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilterKernel::Set(const CBratAlgoFilterKernel &o)
{
  CBratAlgoFilter::Set(o);

  m_varValue = o.m_varValue;
  m_varValuePrev = o.m_varValuePrev;
  m_varValueNext = o.m_varValueNext;
  m_extrapolate = o.m_extrapolate;

  m_rawDataWindow = o.m_rawDataWindow;
  m_dataWindow = o.m_dataWindow;

  m_validPts = o.m_validPts;

  m_expectedTypes = o.m_expectedTypes; 

  m_countX = o.m_countX;
  m_countY = o.m_countY;
  m_addHeightBottom = o.m_addHeightBottom;
  m_addHeightTop = o.m_addHeightTop;
  m_addWidthRight = o.m_addWidthRight;
  m_addWidthLeft = o.m_addWidthLeft;
  m_gapWidth = o.m_gapWidth;
  m_gapHeight = o.m_gapHeight;

  m_dataWindowHeight = o.m_dataWindowHeight;
  m_dataWindowWidth = o.m_dataWindowWidth;

  m_dataWindowLength = o.m_dataWindowLength;

  m_lastIndexDataToInsert = o.m_lastIndexDataToInsert;

  DeleteFieldNetCdf();


}
//----------------------------------------
void CBratAlgoFilterKernel::SetParamValueExtrapolate(CVectorBratAlgorithmParam& args, uint32_t paramIndex)
{
  if (!CTools::IsDefaultValue(m_extrapolate))
  {
    return;
  }

  int32_t valueInt32;

  // Set extrapolate flag (once)
  this->CheckConstantParam(paramIndex);

  valueInt32 = args.at(paramIndex).GetValueAsInt();
  if (valueInt32 < 0)
  {
    throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the '%s' parameter (%d) is not equal to 0 or 1. " 
                                              "You have to adjust the '%s'  parameter.", 
                                              this->GetName().c_str(), 
                                              this->GetParamName(paramIndex).c_str(),
                                              valueInt32, 
                                              this->GetParamName(paramIndex).c_str()),
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

  m_extrapolate = valueInt32;
  if (CTools::IsDefaultValue(valueInt32))
  {
    m_extrapolate = 0;
  }

}
//----------------------------------------
void CBratAlgoFilterKernel::DeleteProduct()
{

  DeleteFieldNetCdf();
  
  CBratAlgoFilter::DeleteProduct();

}

//----------------------------------------
CBratAlgoFilterKernel& CBratAlgoFilterKernel::operator=(const CBratAlgoFilterKernel &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}
//----------------------------------------
void CBratAlgoFilterKernel::SetBeginOfFile()
{
  CBratAlgoFilter::SetBeginOfFile();

  CTools::SetDefaultValue(m_varValue);
  CTools::SetDefaultValue(m_varValuePrev);
  CTools::SetDefaultValue(m_varValueNext);

  //m_varDimXIndex = -1;
  //m_varDimYIndex = -1;

  //m_indexX = -1;
  //m_indexY = -1;

  CTools::SetDefaultValue(m_validPts);
  
  m_rawDataWindow.RemoveAll();
  
  m_field2DAsRef = NULL;
  m_countY = 0;
  m_countX = 0;
  m_addHeightBottom = 0;
  m_addHeightTop = 0;
  m_addWidthRight = 0;
  m_addWidthLeft = 0;
  m_gapWidth = 0;
  m_gapHeight = 0;

  m_lastIndexDataToInsert = 0;

  DeleteFieldNetCdf();

}

//----------------------------------------
void CBratAlgoFilterKernel::SetEndOfFile()
{
  CBratAlgoFilter::SetEndOfFile();

  CTools::SetDefaultValue(m_varValueNext);

}
//----------------------------------------
void CBratAlgoFilterKernel::SetPreviousValues(bool fromProduct) 
{
  CBratAlgoFilter::SetPreviousValues(fromProduct);

}
//----------------------------------------
void CBratAlgoFilterKernel::SetNextValues() 
{
  CBratAlgoFilter::SetNextValues();
}

//----------------------------------------
void CBratAlgoFilterKernel::OpenProductFile() 
{
  CBratAlgoFilter::OpenProductFile();
}

//----------------------------------------
void CBratAlgoFilterKernel::CheckProduct() 
{
  CheckVarExpression(CBratAlgoFilterKernel::m_VAR_PARAM_INDEX);
}

//----------------------------------------
void CBratAlgoFilterKernel::TreatLeftEdge1D(uint32_t shiftSymmetry, uint32_t index)
{
  m_rawDataWindow.InsertAt(m_rawDataWindow.begin(), *(m_rawDataWindow.begin() + shiftSymmetry + (index * 2)));
}
//----------------------------------------
void CBratAlgoFilterKernel::TreatRightEdge1D(uint32_t shiftSymmetry, uint32_t index)
{
  m_rawDataWindow.InsertAt(m_rawDataWindow.end(), *(m_rawDataWindow.end() - (index * 2) - 1 - shiftSymmetry));
}

//----------------------------------------
void CBratAlgoFilterKernel::RemoveFirstItemDataWindow1D()
{
  m_rawDataWindow.Erase(m_rawDataWindow.begin());
}
//----------------------------------------
void CBratAlgoFilterKernel::InsertCurrentValueDataWindow1D()
{
  m_rawDataWindow.Insert(m_varValue);
}
//----------------------------------------
void CBratAlgoFilterKernel::PrepareDataValues1D(uint32_t shiftSymmetry)
{
  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();

  uint32_t gapHalfWindow = 1;
  uint32_t halfWindow =  m_dataWindowLength / 2;
  if (CTools::IsEven(m_dataWindowLength))
  {
    gapHalfWindow = 0;
  }

  // Is it the begin of the data file ?
  // read data (half window) and treat left edge
  int32_t iRecordTmp = iRecord - halfWindow;

  if ((m_rawDataWindow.size() < m_dataWindowLength) && (iRecordTmp <= 0))
  {
    // Need to read previous values ? 
    if (iRecord > 0)
    {
      // Read previous data and add a new element within the window
      for (int32_t i = 0 ; i < iRecord ; i++)
      {
        this->GetData1D(i);
      }

    }

    // Add current value (the centered value)
    this->InsertCurrentValueDataWindow1D();

    int32_t endRead = halfWindow + gapHalfWindow;
    // read next values;
    for (int32_t i = 1 ; i < endRead ; i++)
    {
      this->GetData1D(iRecord + i);
    }
    
    //while (m_rawDataWindow.size() < (halfWindow + gapHalfWindow))
    //{
    //  this->GetData1D(iRecord + m_gapFromCurrentRecord + 1);
    //  m_gapFromCurrentRecord++;
    //}
    //if (m_gapFromCurrentRecord > 0)
    //{
    //  m_gapFromCurrentRecord--;
    //}

    // Treating left edge
    // For all window filters there is some problem. 
    // That is edge treating. If you place window over first element, the left  part of the window will be empty.
    // Data should be extended symmetrically.
    uint32_t i = 0;
    while (m_rawDataWindow.size() < m_dataWindowLength)
    //for (uint32_t i = 0 ; i < halfWindow ; i++)
    {
      this->TreatLeftEdge1D(shiftSymmetry, i);
      i++;
      //m_rawDataWindow.InsertAt(m_rawDataWindow.begin(), *(m_rawDataWindow.begin() + shiftSymmetry + (i * 2)));
    }

  }
  else
  {   
    // At least a new record has been read from the caller

    // Get the current record of the product use in the algo.
    // and compare it with the current record of the caller's product
    int32_t currentAlgoRecord = m_product->GetCurrentRecordNumber();

    // Treating right edge
    // For all window filters there is some problem. 
    // That is edge treating. If you place window over last element, the right part of the window will be empty.
    // Data should be extended symmetrically.
    if (currentAlgoRecord >= m_nProductRecords - 1)
    {
      //std::cout << "THIS IS THE END OF FILE:" << std::endl;
      this->TreatRightEdge1D(shiftSymmetry, m_lastIndexDataToInsert);
      m_lastIndexDataToInsert++;
    }

    // Need to read previous values ? 
    if (currentAlgoRecord < iRecordTmp)
    {
      currentAlgoRecord = iRecordTmp;

      if (currentAlgoRecord < 0)
      {
        throw CAlgorithmException(CTools::Format("Algorithm %s - The current record of the product is negative (%d). This should never happened. "
                                    "\nThe current record of the caller is %d."
                                    "\nThere is something wrong in the Brat software."
                                    "\nPlease contact Brat Helpdesk", 
                                    this->GetName().c_str(), currentAlgoRecord, iRecord),
                                    this->GetName(), BRATHL_LOGIC_ERROR);
      }


      // Read previous data and add a new element within the window
      while (currentAlgoRecord < iRecord)
      {
        this->GetData1D(currentAlgoRecord);
        currentAlgoRecord++;
      }

      // Add current value (the centered value)
      this->InsertCurrentValueDataWindow1D();

      int32_t endRead =  iRecord + halfWindow + gapHalfWindow;
      
      // read next values;
      for (int32_t i = currentAlgoRecord + 1 ; i < endRead ; i++)
      {
        if (i >= m_nProductRecords)
        {
          // Treating right edge
          this->TreatRightEdge1D(shiftSymmetry, m_lastIndexDataToInsert);
          m_lastIndexDataToInsert++;
        }
        else
        {
         this->GetData1D(i);
        }
      }
    }
    else
    {
      // Read the next values
      // Add a new element within the window

      int32_t endRead =  iRecord + halfWindow + gapHalfWindow;

      for (int32_t i = currentAlgoRecord + 1 ; i < endRead ; i++)
      {
          this->GetData1D(i);
      }
    }

    // Remove elements that are not inside the window.
    while (m_rawDataWindow.size() > m_dataWindowLength)
    {
      this->RemoveFirstItemDataWindow1D();
    }
  
  }

}

//----------------------------------------
void CBratAlgoFilterKernel::PrepareDataReading2D()
{
  uint32_t halfWindowWidth = m_dataWindowWidth/2;
  uint32_t halfWindowHeight = m_dataWindowHeight/2;

  m_countY = 0;
  m_countX = 0;
  m_addHeightBottom = 0;
  m_addHeightTop = 0;
  m_addWidthRight = 0;
  m_addWidthLeft = 0;
  m_gapWidth = 0;
  m_gapHeight = 0;

  if (CTools::IsEven(m_dataWindowWidth))
  {
    m_gapWidth = 1;
  }

  if (CTools::IsEven(m_dataWindowHeight))
  {
    m_gapHeight = 1;
  }


  // Set number of data to read for 2D fields
  m_field2DAsRef = NULL;

  CObMap::const_iterator itMap;
  for (itMap = m_fieldVarsCaller.begin(); itMap != m_fieldVarsCaller.end() ; itMap++)
  { 
    CFieldNetCdf* fieldCaller = CExternalFiles::GetFieldNetCdf(itMap->second);
    
    int32_t nbDims = fieldCaller->GetNbDims();
    if (nbDims != 2)
    {
      continue;
    }

    //fieldCaller->Dump(*CTrace::GetDumpContext());
  
    uint32_t indexX = fieldCaller->GetDimsIndexArray()[0];
    uint32_t indexY = fieldCaller->GetDimsIndexArray()[1];

    uint32_t dimX = fieldCaller->GetDimAt(0);
    uint32_t dimY = fieldCaller->GetDimAt(1);

    if (m_dataWindowWidth > dimX)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window width parameter (%d) is greater than the X-dimension value (%d) of the variable '%s' within the data file (%s). "
                                               "You have to decrease the window width parameter", 
                                                this->GetName().c_str(), m_dataWindowWidth, dimX, itMap->first.c_str(), m_currentFileName.c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    if (m_dataWindowHeight > dimY)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window height parameter (%d) is greater than the Y-dimension value (%d) of the variable '%s' within the data file (%s). "
                                               "You have to decrease the window width parameter", 
                                                this->GetName().c_str(), m_dataWindowHeight, dimY, itMap->first.c_str(), m_currentFileName.c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }
    
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(m_fieldVars.Exists(fieldCaller->GetName()));
    if (field->GetDimsIndexArray() == NULL)
    {
      field->NewDimIndexArray();
    }

    if (m_field2DAsRef == NULL)
    {
      m_field2DAsRef = field;
    }


    m_addWidthRight = 0;
    m_addWidthLeft = 0;
    int32_t minIndexX = 0;
    m_countX = m_dataWindowWidth;

    if (indexX < halfWindowWidth) 
    {
      m_addWidthLeft = halfWindowWidth - indexX;
      m_countX -= m_addWidthLeft;
    }
    else 
    {
      minIndexX = indexX - halfWindowWidth;
      if ((indexX + halfWindowWidth) >= dimX)
      {
        m_addWidthRight = (halfWindowWidth + indexX + 1) - dimX;
        m_countX -= m_addWidthRight;
      }
    }


    int32_t minIndexY = 0;
    m_countY = m_dataWindowHeight;
    m_addHeightBottom = 0;
    m_addHeightTop = 0;

    if (indexY < halfWindowHeight) 
    {
      m_addHeightTop = (halfWindowHeight - indexY);
      m_countY -= m_addHeightTop;
    }
    else 
    {
      minIndexY = indexY - halfWindowHeight;
      if ((indexY + halfWindowHeight) >= dimY)
      {
        m_addHeightBottom = (halfWindowHeight + indexY + 1) - dimY;
        m_countY -= m_addHeightBottom;
      }
    }
    
    // Read data from Netcdf file
    field->GetDimsIndexArray()[0] = minIndexX;
    field->GetDimsCountArray()[0] = m_countX;
    
    field->GetDimsIndexArray()[1] = minIndexY;
    field->GetDimsCountArray()[1] = m_countY;
    
  }

  // Set number of data to read for 1D fields
  for (itMap = m_fieldVars.begin(); itMap != m_fieldVars.end() ; itMap++)
  { 
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(itMap->second);
    
    int32_t nbDims = field->GetNbDims();
    if (nbDims != 1)
    {
      continue;
    }

    if (field->GetDimsIndexArray() == NULL)
    {
      field->NewDimIndexArray();
    }

    CFieldNetCdf* fieldTemp = CExternalFiles::GetFieldNetCdf(m_fieldDependOnXDim.Exists(itMap->first), false);
    if (fieldTemp != NULL)
    {
      field->GetDimsIndexArray()[0] = m_field2DAsRef->GetDimsIndexArray()[0];
      field->GetDimsCountArray()[0] = m_field2DAsRef->GetDimsCountArray()[0];
      continue;
    }

    fieldTemp = CExternalFiles::GetFieldNetCdf(m_fieldDependOnYDim.Exists(itMap->first), false);
    if (fieldTemp != NULL)
    {
      field->GetDimsIndexArray()[0] = m_field2DAsRef->GetDimsIndexArray()[1];
      field->GetDimsCountArray()[0] = m_field2DAsRef->GetDimsCountArray()[1];
    }

  }

}
//----------------------------------------
void CBratAlgoFilterKernel::PrepareDataValues2DComplexExpressionWithAlgo()
{
  CExpressionValue exprValue;
  CBratAlgoFilter::PrepareDataValues2DComplexExpressionWithAlgo(exprValue, CBratAlgoFilterKernel::m_VAR_PARAM_INDEX);

  this->PrepareDataWindow2DSymmetrically(exprValue);

}
//----------------------------------------
void CBratAlgoFilterKernel::PrepareDataValues2DComplexExpression()
{
  CExpressionValue exprValue;
  CBratAlgoFilter::PrepareDataValues2DComplexExpression(exprValue, CBratAlgoFilterKernel::m_VAR_PARAM_INDEX);

  this->PrepareDataWindow2DSymmetrically(exprValue);
}
//----------------------------------------
void CBratAlgoFilterKernel::PrepareDataValues2DOneField()
{
  CExpressionValue exprValue;
  CBratAlgoFilter::PrepareDataValues2DOneField(exprValue, CBratAlgoFilterKernel::m_VAR_PARAM_INDEX);
  
  this->PrepareDataWindow2DSymmetrically(exprValue);

}

//----------------------------------------
void CBratAlgoFilterKernel::PrepareDataWindow2DSymmetrically(CExpressionValue& exprValue, uint32_t shiftSymmetry /* = 0 */)
{
  PrepareDataWindow2DSymmetrically(exprValue.GetValues(), exprValue.GetNbValues(), shiftSymmetry);
}

//----------------------------------------
void CBratAlgoFilterKernel::PrepareDataWindow2DSymmetrically(double* dataValue, uint32_t nbValues, uint32_t shiftSymmetry /* = 0 */)
{

  // Fill data window with data values
  m_rawDataWindow.RemoveAll();

  m_rawDataWindow.resize(GetDataWindowSize());

  std::fill(m_rawDataWindow.begin(), m_rawDataWindow.end(), CTools::m_defaultValueDOUBLE); 
        
  int32_t start = (m_addWidthLeft * m_dataWindowHeight) + m_addHeightTop;

  // Copy each width (x)
  for (int32_t i = 0 ; i < m_countX ; i++)
  {
    int32_t offsetCopy = start + (i * m_dataWindowHeight);

  // Copy all height (y) for this width
    std::copy(dataValue + (m_countY * i),
            dataValue + (m_countY * (i + 1)), 
            m_rawDataWindow.begin() + offsetCopy);
      
    //m_rawDataWindow.Dump(*CTrace::GetDumpContext());
      
    // Treating top height edge
    // Data are extended symmetrically.
    for (int32_t h = 0 ; h < m_addHeightTop ; h++)
    {
      m_rawDataWindow[offsetCopy - h - 1]  = m_rawDataWindow[offsetCopy + h + shiftSymmetry - m_gapHeight];
    }

    // Treating bottom height edge
    // Data are extended symmetrically.
    for (int32_t h = 0 ; h < m_addHeightBottom ; h++)
    {
      m_rawDataWindow[offsetCopy + m_countY + h]  = m_rawDataWindow[offsetCopy + m_countY - 1 - shiftSymmetry - h + m_gapHeight];
    }
     
    //m_rawDataWindow.Dump(*CTrace::GetDumpContext());
  }


  // Treating left width edge
  // Data are extended symmetrically.
  uint32_t offset = m_addWidthLeft * m_dataWindowHeight;

  for (int32_t w = 0 ; w < m_addWidthLeft ; w++)
  {
    std::copy(m_rawDataWindow.begin() + offset + (m_dataWindowHeight * (w + shiftSymmetry - m_gapWidth)),
              m_rawDataWindow.begin() + offset + (m_dataWindowHeight * (w + 1 + shiftSymmetry - m_gapWidth)),
              m_rawDataWindow.begin() + offset - (m_dataWindowHeight * (w + 1)));
  }

  // Treating right width edge
  // Data are extended symmetrically.
  offset = (m_addWidthRight * m_dataWindowHeight);

  for (int32_t w = 0 ; w < m_addWidthRight ; w++)
  {
    std::copy(m_rawDataWindow.end() - offset - (m_dataWindowHeight * (w + 1 + shiftSymmetry - m_gapWidth)),
              m_rawDataWindow.end() - offset - (m_dataWindowHeight * (w + shiftSymmetry - m_gapWidth)),
              m_rawDataWindow.end() - offset + (m_dataWindowHeight * w));
  }

  //m_rawDataWindow.Dump(*CTrace::GetDumpContext());

  CTrace::Tracer(3, CTools::Format("===> m_varValue is %f", m_varValue));
  CTrace::Tracer(3, "===> WINDOW IS:");

  for (uint32_t i = 0 ; i < m_dataWindowWidth ; i++)
  {
    std::string str;
    for (uint32_t j = 0 ; j < m_dataWindowHeight ; j++)
    {
      double value = m_rawDataWindow[(i * m_dataWindowHeight) + j];
      if (CTools::IsDefaultValue(value))
      {
        str.append("DV ");
      }
      else
      {
        str.append(CTools::Format("%f ", value));
      }
    }
    CTrace::Tracer(3, str);

  }

  CTrace::Tracer(3, "===> END WINDOW <=======");

}  
//----------------------------------------
void CBratAlgoFilterKernel::Dump(std::ostream& fOut /*= std::cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgoFilterKernel Object at "<< this << std::endl;
  CBratAlgoFilter::Dump(fOut);

  fOut << "m_varValue: " << m_varValue << std::endl;
  fOut << "m_varValuePrev: " << m_varValuePrev << std::endl;
  fOut << "m_varValueNext: " << m_varValueNext << std::endl;
  fOut << "m_extrapolate: " << m_extrapolate << std::endl;  
  fOut << "m_dataWindowLength: " << m_dataWindowLength << std::endl;
  fOut << "m_dataWindowHeight: " << m_dataWindowHeight << std::endl;
  fOut << "m_dataWindowWidth: " << m_dataWindowWidth << std::endl;
  fOut << "m_countX: " << m_countX << std::endl;
  fOut << "m_countY: " << m_countY << std::endl;
  fOut << "m_addHeightBottom: " << m_addHeightBottom << std::endl;
  fOut << "m_addHeightTop: " << m_dataWindowWidth << std::endl;
  fOut << "m_addWidthRight: " << m_addWidthRight << std::endl;
  fOut << "m_addWidthLeft: " << m_addWidthLeft << std::endl;
  fOut << "m_gapWidth: " << m_gapWidth << std::endl;
  fOut << "m_gapHeight: " << m_gapHeight << std::endl;
  fOut << "m_lastIndexDataToInsert: " << m_lastIndexDataToInsert << std::endl;

  fOut << "m_expectedTypes: " << std::endl;
  m_expectedTypes.Dump(fOut);

  fOut << "m_rawDataWindow: " << std::endl;
  m_rawDataWindow.Dump(fOut);
  fOut << "m_dataWindow: " << std::endl;
  m_dataWindow.Dump(fOut);

  fOut << "==> END Dump a CBratAlgoFilterKernel Object at "<< this << std::endl;

}


} // end namespace
