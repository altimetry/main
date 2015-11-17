
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

#include <cstdlib>

#include "TraceLog.h" 
#include "Exception.h" 
#include "Product.h" 
#include "ProductNetCdf.h" 

#include "BratAlgoFilterMedian2D.h" 

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

const uint32_t CBratAlgoFilterMedian2D::m_INPUT_PARAMS = 5;


const uint32_t CBratAlgoFilterMedian2D::m_WINDOW_WIDTH_PARAM_INDEX = 1;
const uint32_t CBratAlgoFilterMedian2D::m_WINDOW_HEIGHT_PARAM_INDEX = 2;
const uint32_t CBratAlgoFilterMedian2D::m_VALID_PARAM_INDEX = 3;
const uint32_t CBratAlgoFilterMedian2D::m_EXTRAPOLATE_PARAM_INDEX = 4;

//-------------------------------------------------------------
//------------------- CBratAlgoFilterMedian2D class --------------------
//-------------------------------------------------------------

CBratAlgoFilterMedian2D::CBratAlgoFilterMedian2D()
{
  Init();

}
//----------------------------------------
CBratAlgoFilterMedian2D::CBratAlgoFilterMedian2D(const CBratAlgoFilterMedian2D &copy)
{
  Init();

  Set(copy);

}
//----------------------------------------
CBratAlgoFilterMedian2D::~CBratAlgoFilterMedian2D()
{
  DeleteFieldNetCdf();

}

//----------------------------------------
void CBratAlgoFilterMedian2D::Init()
{
  m_expectedTypes.Insert(static_cast<int32_t>(CBratAlgorithmParam::T_DOUBLE));
  m_expectedTypes.Insert(static_cast<int32_t>(CBratAlgorithmParam::T_VECTOR_DOUBLE));

  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilterMedian2D::Set(const CBratAlgoFilterMedian2D &o)
{
  CBratAlgoFilterMedian::Set(o);

  DeleteFieldNetCdf();

}


//----------------------------------------
CBratAlgoFilterMedian2D& CBratAlgoFilterMedian2D::operator=(const CBratAlgoFilterMedian2D &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}


//----------------------------------------
void CBratAlgoFilterMedian2D::SetBeginOfFile()
{
  CBratAlgoFilterMedian::SetBeginOfFile();

  CTools::SetDefaultValue(m_median);



}

//----------------------------------------
void CBratAlgoFilterMedian2D::SetEndOfFile()
{
  CBratAlgoFilterMedian::SetEndOfFile();

}


//----------------------------------------
double CBratAlgoFilterMedian2D::Run(CVectorBratAlgorithmParam& args)
{

  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();

  if (iRecord == m_callerProductRecordPrev)
  {
    // Do nothing: data have been already computed
    return m_median;
  }

  m_varValueArray = NULL;

  CTools::SetDefaultValue(m_median);

  OpenProductFile();
  
  SetParamValues(args);

  if (GetDataWindowSize() < 1)
  {

    PrepareReturn();
    return m_median;
  }

  if (GetDataWindowSize() == 1)
  {
    m_median = m_varValue;
    PrepareReturn();
    return m_median;
  }

  if ( m_varValueArray == NULL)
  {
    ComputeSingle();
  }
  else
  {
    ComputeMean();
  }


  PrepareReturn();

  return m_median;

}

//----------------------------------------
double CBratAlgoFilterMedian2D::ComputeSingle() 
{
  // If 'default value' and no extrapolation then returns
  if (CTools::IsDefaultValue(m_varValue) && (m_extrapolate == 0))
  {
    return m_median;
  }

  PrepareDataReading2D();
  if (m_isComplexExpression.at(CBratAlgoFilterMedian2D::m_VAR_PARAM_INDEX))
  {
    if (m_isComplexExpressionWithAlgo.at(CBratAlgoFilterMedian2D::m_VAR_PARAM_INDEX))
    {
      PrepareDataValues2DComplexExpressionWithAlgo();
    }
    else
    {
      PrepareDataValues2DComplexExpression();
    }
  }
  else
  {
    PrepareDataValues2DOneField();
  }

  ComputeMedian();
  
  return m_median;
}

//----------------------------------------
double CBratAlgoFilterMedian2D::ComputeMean()
{

  throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because input data are an array of values and not a single value. "
                                           "Perhaps, you are trying to reduce the two-dimensionals data to only one of its dimensions. " 
                                           "This use case is not implemented.", 
                                          this->GetName().c_str()),
                          this->GetName(), BRATHL_LOGIC_ERROR);

  // TODO
  //CDoubleArray varValueArrayTmp;

  //if (m_varValueArray == NULL)
  //{
  //  varValueArrayTmp.Insert(m_varValue);
  //  m_varValueArray = &varValueArrayTmp;
  //}

  //uint32_t iVarValue;
  //
  //CTools::SetDefaultValue(m_median);
  //double countValue = 0.0;
  //double dummy = 0.0;

  //double medianTmp;
  //CTools::SetDefaultValue(medianTmp);

  //SetField2DAsRef();

  //uint32_t xArraySize =  m_field2DAsRef->Get->size();
  //uint32_t yArraySize =  m_latArray->size();

  //if (m_varDimLonIndex == 0)
  //{
  //  for (iLon = 0 ; iLon < lonArraySize ; iLon++)
  //  {
  //    m_lon = m_lonArray->at(iLon);
  //    m_indexLon = this->GetLongitudeIndex(m_lon);

  //    for (iLat = 0 ; iLat < latArraySize ; iLat++)
  //    {
  //      m_lat = m_latArray->at(iLat);
  //      m_indexLat = this->GetLatitudeIndex(m_lat);

  //      iVarValue = (iLon * m_latArray->size()) + iLat;
  //      m_varValue = m_varValueArray->at(iVarValue);
  //      
  //      ComputeSingle();

	 //     CTools::DoIncrementalStats(m_velocity,
	 //                          countValue,
	 //                          velocityTmp,
	 //                          dummy,
	 //                          dummy,
	 //                          dummy);
  //    }
  //  }
  //}
  //else if (m_varDimLatIndex == 0)
  //{
  //  for (iLat = 0 ; iLat < latArraySize ; iLat++)
  //  {
  //    m_lat = m_latArray->at(iLat);
  //    m_indexLat = this->GetLatitudeIndex(m_lat);

  //    for (iLon = 0 ; iLon < lonArraySize ; iLon++)
  //    {
  //      m_lon = m_lonArray->at(iLon);
  //      m_indexLon = this->GetLongitudeIndex(m_lon);

  //      iVarValue = (iLat * m_lonArray->size()) + iLon;
  //      m_varValue = m_varValueArray->at(iVarValue);
  //      
  //      ComputeSingle();

	 //     CTools::DoIncrementalStats(m_velocity,
	 //                          countValue,
	 //                          velocityTmp,
	 //                          dummy,
	 //                          dummy,
	 //                          dummy);
  //    }
  //  }
  //}

  //m_median = medianTmp;

  return m_median;
}

//----------------------------------------
void CBratAlgoFilterMedian2D::CheckInputParams(CVectorBratAlgorithmParam& args) 
{
  CBratAlgorithmBase::CheckInputParams(args);

  for (uint32_t indexParam = 0 ; indexParam < CBratAlgoFilterMedian2D::m_INPUT_PARAMS ; indexParam++)
  {
    if (indexParam == CBratAlgoFilterMedian2D::m_VAR_PARAM_INDEX)
    {
      CheckInputTypeParams(indexParam, m_expectedTypes, args);
      continue;
    }
    CheckInputTypeParams(indexParam, this->GetInputParamFormat(indexParam), args);
  }

  CBratAlgorithmParam* algoParam = &(args.at(CBratAlgoFilterMedian2D::m_VAR_PARAM_INDEX));
  if (algoParam->GetTypeVal() == CBratAlgorithmParam::T_VECTOR_DOUBLE)
  {
    m_varValueArray = algoParam->GetValueAsVectDouble();
  }

}

//----------------------------------------
void CBratAlgoFilterMedian2D::SetParamValues(CVectorBratAlgorithmParam& args)
{    
  CheckInputParams(args);

  // Set variable
  m_varValue = args.at(CBratAlgoFilterMedian2D::m_VAR_PARAM_INDEX).GetValueAsDouble();

  int32_t valueInt32;

  // Set data window width (once)
  if (CTools::IsDefaultValue(m_dataWindowWidth))
  {
    this->CheckConstantParam(CBratAlgoFilterMedian2D::m_WINDOW_WIDTH_PARAM_INDEX);
    
    valueInt32 = args.at(CBratAlgoFilterMedian2D::m_WINDOW_WIDTH_PARAM_INDEX).GetValueAsInt();
    if (valueInt32 <= 0)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window width parameter (%d) is not greater than 0. " 
                                               "You have to adjust the '%s' parameter.", 
                                                this->GetName().c_str(), valueInt32, 
                                                this->GetParamName(CBratAlgoFilterMedian2D::m_WINDOW_WIDTH_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    m_dataWindowWidth = valueInt32;

    if (CTools::IsDefaultValue(valueInt32))
    {
      m_dataWindowWidth = 3;
    }
  }

  // Set data window height (once)
  if (CTools::IsDefaultValue(m_dataWindowHeight))
  {
    this->CheckConstantParam(CBratAlgoFilterMedian2D::m_WINDOW_HEIGHT_PARAM_INDEX);

    valueInt32 = args.at(CBratAlgoFilterMedian2D::m_WINDOW_HEIGHT_PARAM_INDEX).GetValueAsInt();
    if (valueInt32 <= 0)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window height parameter (%d) is not greater than 0. " 
                                               "You have to adjust the '%s'  parameter.", 
                                                this->GetName().c_str(), valueInt32, 
                                                this->GetParamName(CBratAlgoFilterMedian2D::m_WINDOW_HEIGHT_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    m_dataWindowHeight = valueInt32;

    if (CTools::IsDefaultValue(valueInt32))
    {
      m_dataWindowHeight = 3;
    }
  }

  // Set valid points limit (once)
  if (CTools::IsDefaultValue(m_validPts))
  {
    this->CheckConstantParam(CBratAlgoFilterMedian2D::m_VALID_PARAM_INDEX);

    valueInt32 = args.at(CBratAlgoFilterMedian2D::m_VALID_PARAM_INDEX).GetValueAsInt();
    if (valueInt32 <= 0)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the '%s' parameter (%d) is less than or equal to 0. " 
                                               "You have to adjust the '%s'  parameter.", 
                                                this->GetName().c_str(), 
                                                this->GetParamName(CBratAlgoFilterMedian2D::m_VALID_PARAM_INDEX).c_str(),
                                                valueInt32, 
                                                this->GetParamName(CBratAlgoFilterMedian2D::m_VALID_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }


    m_validPts = valueInt32;
    if (CTools::IsDefaultValue(valueInt32))
    {
      m_validPts = GetDataWindowSize()/2;
    }

    if (GetDataWindowSize() < m_validPts)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window size parameter (%dx%d=%d ) is less than the minimum number of valid points parameter (%d). " 
                                               "You have to adjust either the '%s' or '%s' parameters or the '%s' parameter.", 
                                                this->GetName().c_str(), m_dataWindowWidth, m_dataWindowHeight, GetDataWindowSize(), m_validPts,
                                                this->GetParamName(CBratAlgoFilterMedian2D::m_WINDOW_WIDTH_PARAM_INDEX).c_str(),
                                                this->GetParamName(CBratAlgoFilterMedian2D::m_WINDOW_HEIGHT_PARAM_INDEX).c_str(),
                                                this->GetParamName(CBratAlgoFilterMedian2D::m_VALID_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }
  }

  // Set extrapolate flag (once)
  if (CTools::IsDefaultValue(m_extrapolate))
  {
    SetParamValueExtrapolate(args, CBratAlgoFilterMedian2D::m_EXTRAPOLATE_PARAM_INDEX);
  }


}

//----------------------------------------
void CBratAlgoFilterMedian2D::OpenProductFile() 
{
  CProductNetCdf* productNetCdf = CBratAlgorithmBase::GetProductNetCdf(m_product);

  productNetCdf->SetApplyNetcdfProductInitialisation(false);
  productNetCdf->SetForceReadDataOneByOne(true);

  CBratAlgorithmBase::OpenProductFile();

  //if ( (m_varDimXIndex < 0) && (m_varDimYIndex < 0) )
  if (m_fieldVarsCaller.size() <= 0)
  {
    CheckProduct();
  }


}
//----------------------------------------
void CBratAlgoFilterMedian2D::CheckProduct() 
{
  CheckComplexExpression(CBratAlgoFilterMedian2D::m_VAR_PARAM_INDEX);
  CheckVarExpression(CBratAlgoFilterMedian2D::m_VAR_PARAM_INDEX);
}

//----------------------------------------
void CBratAlgoFilterMedian2D::CheckVarExpression(uint32_t index)
{
  CBratAlgoFilterMedian::CheckVarExpression2D(index);
}

//----------------------------------------
void CBratAlgoFilterMedian2D::Dump(std::ostream& fOut /*= std::cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgoFilterMedian2D Object at "<< this << std::endl;
  CBratAlgoFilterMedian::Dump(fOut);
  fOut << "==> END Dump a CBratAlgoFilterMedian2D Object at "<< this << std::endl;

}



} // end namespace
