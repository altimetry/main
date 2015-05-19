
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

#include <cmath>

#include "TraceLog.h" 
#include "Exception.h" 
#include "Product.h" 
#include "ProductNetCdf.h" 

#include "BratAlgoFilterLoess2D.h" 

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

const uint32_t CBratAlgoFilterLoess2D::m_INPUT_PARAMS = 5;


const uint32_t CBratAlgoFilterLoess2D::m_WINDOW_WIDTH_PARAM_INDEX = 1;
const uint32_t CBratAlgoFilterLoess2D::m_WINDOW_HEIGHT_PARAM_INDEX = 2;
const uint32_t CBratAlgoFilterLoess2D::m_VALID_PARAM_INDEX = 3;
const uint32_t CBratAlgoFilterLoess2D::m_EXTRAPOLATE_PARAM_INDEX = 4;


//-------------------------------------------------------------
//------------------- CBratAlgoFilterLoess2D class --------------------
//-------------------------------------------------------------

CBratAlgoFilterLoess2D::CBratAlgoFilterLoess2D()
{
  Init();

}
//----------------------------------------
CBratAlgoFilterLoess2D::CBratAlgoFilterLoess2D(const CBratAlgoFilterLoess2D &copy)
{
  Init();

  Set(copy);

}
//----------------------------------------
CBratAlgoFilterLoess2D::~CBratAlgoFilterLoess2D()
{
  DeleteFieldNetCdf();

}

//----------------------------------------
void CBratAlgoFilterLoess2D::Init()
{
  m_expectedTypes.Insert(static_cast<int32_t>(CBratAlgorithmParam::T_DOUBLE));
  m_expectedTypes.Insert(static_cast<int32_t>(CBratAlgorithmParam::T_VECTOR_DOUBLE));

  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilterLoess2D::Set(const CBratAlgoFilterLoess2D &o)
{
  CBratAlgoFilterLoess::Set(o);

  DeleteFieldNetCdf();

}


//----------------------------------------
CBratAlgoFilterLoess2D& CBratAlgoFilterLoess2D::operator=(const CBratAlgoFilterLoess2D &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}


//----------------------------------------
void CBratAlgoFilterLoess2D::SetBeginOfFile()
{
  CBratAlgoFilterLoess::SetBeginOfFile();

  CTools::SetDefaultValue(m_loess);



}

//----------------------------------------
void CBratAlgoFilterLoess2D::SetEndOfFile()
{
  CBratAlgoFilterLoess::SetEndOfFile();

}


//----------------------------------------
double CBratAlgoFilterLoess2D::Run(CVectorBratAlgorithmParam& args)
{

  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();

  if (iRecord == m_callerProductRecordPrev)
  {
    // Do nothing: data have been already computed
    return m_loess;
  }

  m_varValueArray = NULL;

  CTools::SetDefaultValue(m_loess);

  OpenProductFile();
  
  SetParamValues(args);

  if (GetDataWindowSize() < 1)
  {

    PrepareReturn();
    return m_loess;
  }

  if (GetDataWindowSize() == 1)
  {
    m_loess = m_varValue;
    PrepareReturn();
    return m_loess;
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

  return m_loess;

}

//----------------------------------------
void CBratAlgoFilterLoess2D::ComputeInitialWeights()
{
  // Compute just once 
  if (m_initialWeights.size() > 0)
  {
    // Already computed
    return;
  }

  int32_t windowSize = GetDataWindowSize();
  int32_t halfWindowWidth = m_dataWindowWidth / 2;
  int32_t halfWindowHeight = m_dataWindowHeight / 2;

  m_initialWeights.resize(windowSize);

  double distance = 0.0;

  int32_t indexWeight = 0;

  for (int32_t x = 0 ; x < static_cast<int32_t>(m_dataWindowWidth) ; x++)
  {
	  double sqrNormDistX	= static_cast<double>(x - halfWindowWidth) / static_cast<double>(halfWindowWidth);	          
#if _DEBUG_BRAT_ALGO    
    double normDistX = sqrNormDistX;
#endif
    sqrNormDistX *= sqrNormDistX;
    for (int32_t y = 0 ; y < static_cast<int32_t>(m_dataWindowHeight) ; y++)
	  {
	    double normDistY	= static_cast<double>(y - halfWindowHeight) / static_cast<double>(halfWindowHeight);
		              
      distance = sqrt( sqrNormDistX + (normDistY * normDistY) );

      double valueLocal = m_rawDataWindow[0];

      if (valueLocal == 1.0)
      {
        string msg = "";
      }

		  double weight = (distance <= 1.0 ? pow((1.0 - pow(distance, 3.0)),3.0) : 0.0);
#if _DEBUG_BRAT_ALGO    
      CTrace::Tracer(CTools::Format("xx = %d yy = %d value = %f normDistX = %f normDistY = %f dist = %f wij = %f" , 
                                      x, y, valueLocal, normDistX, normDistY, distance, weight));
#endif
      m_initialWeights[indexWeight] = weight;
      indexWeight++;
    }
  }

#if _DEBUG_BRAT_ALGO    

  string str;
  str.append("initial weights are ");

  str.append(CTools::Format("(%ld elements): ", static_cast<long>(m_initialWeights.size())));

  for (uint32_t i  = 0; i < m_initialWeights.size(); i++)
  {
	  str.append(CTools::Format("%f ",  m_initialWeights[i]));
  }
  str.append("\n");

  CTrace::Tracer(str);
#endif
}


//----------------------------------------
double CBratAlgoFilterLoess2D::ApplyFilter()
{
  ComputeInitialWeights();

  uint32_t windowSize = GetDataWindowSize();

  // Sum weight (exclude 'default value')
  double sumWeight = 0.0;

  uint32_t nbMissingValue = 0;

	//   Apply filter - calculate weighted mean
	double sum = 0.0;
  for (uint32_t i = 0; i < windowSize; i++)
  {
    if (CTools::IsDefaultValue(m_rawDataWindow.at(i)))
    {
      nbMissingValue++;
      continue;
    }

    sumWeight += m_initialWeights[i];

    sum += m_initialWeights.at(i) * m_rawDataWindow.at(i);

#if _DEBUG_BRAT_ALGO    
    CTrace::Tracer(CTools::Format("ApplyFilter sum = %f m_initialWeights = %f value = %f" , 
                                      sum, m_initialWeights.at(i), m_rawDataWindow.at(i)));
#endif
  }

  uint32_t nbValidPts = windowSize - nbMissingValue;

  if (nbValidPts < m_validPts)
  {
    CTools::SetDefaultValue(sum);
    return sum;
  }

  if (sumWeight == 0.0)
  {
    return sum;
  }

	return sum / sumWeight;
}

//----------------------------------------
double CBratAlgoFilterLoess2D::ComputeLoess()
{
  CTools::SetDefaultValue(m_loess);

  CDoubleArray::const_iterator it;

  //uint32_t nbMissingValue = SumWeights();

  //uint32_t windowSize = GetDataWindowSize();
  //uint32_t nbValidPts = windowSize - nbMissingValue;

  //if (nbValidPts < m_validPts)
  //{
  //  return m_loess;
  //}

  m_loess = ApplyFilter();

#if _DEBUG_BRAT_ALGO    
  CTrace::Tracer(3, CTools::Format("m_rawDataWindow contains (%d elements):", m_rawDataWindow.size()));
  string str;
  for (it = m_rawDataWindow.begin(); it != m_rawDataWindow.end(); it++)
  {
    if (CTools::IsDefaultValue(*it))
    {
      str.append("DV ");
    }
    else
    {
      str.append(CTools::Format("%f ", *it));
    }
  }

  CTrace::Tracer(3, str);

  CTrace::Tracer(3, CTools::Format("loess is: %f", m_loess));
#endif
  return m_loess;

}


//----------------------------------------
double CBratAlgoFilterLoess2D::ComputeSingle() 
{
  // If 'default value' and no extrapolation then returns
  if (CTools::IsDefaultValue(m_varValue) && (m_extrapolate == 0))
  {
    return m_loess;
  }

  PrepareDataReading2D();
  if (m_isComplexExpression.at(CBratAlgoFilterLoess2D::m_VAR_PARAM_INDEX))
  {
    if (m_isComplexExpressionWithAlgo.at(CBratAlgoFilterLoess2D::m_VAR_PARAM_INDEX))
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

  ComputeLoess();
  
  return m_loess;
}

//----------------------------------------
double CBratAlgoFilterLoess2D::ComputeMean()
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

  return m_loess;
}

//----------------------------------------
void CBratAlgoFilterLoess2D::CheckInputParams(CVectorBratAlgorithmParam& args) 
{
  CBratAlgorithmBase::CheckInputParams(args);

  for (uint32_t indexParam = 0 ; indexParam < CBratAlgoFilterLoess2D::m_INPUT_PARAMS ; indexParam++)
  {
    if (indexParam == CBratAlgoFilterLoess2D::m_VAR_PARAM_INDEX)
    {
      CheckInputTypeParams(indexParam, m_expectedTypes, args);
      continue;
    }
    CheckInputTypeParams(indexParam, this->GetInputParamFormat(indexParam), args);
  }

  CBratAlgorithmParam* algoParam = &(args.at(CBratAlgoFilterLoess2D::m_VAR_PARAM_INDEX));
  if (algoParam->GetTypeVal() == CBratAlgorithmParam::T_VECTOR_DOUBLE)
  {
    m_varValueArray = algoParam->GetValueAsVectDouble();
  }

}

//----------------------------------------
void CBratAlgoFilterLoess2D::SetParamValues(CVectorBratAlgorithmParam& args)
{    
  CheckInputParams(args);

  // Set variable
  m_varValue = args.at(CBratAlgoFilterLoess2D::m_VAR_PARAM_INDEX).GetValueAsDouble();

  int32_t valueInt32;

  // Set data window width (once)
  if (CTools::IsDefaultValue(m_dataWindowWidth))
  {
    this->CheckConstantParam(CBratAlgoFilterLoess2D::m_WINDOW_WIDTH_PARAM_INDEX);
    
    valueInt32 = args.at(CBratAlgoFilterLoess2D::m_WINDOW_WIDTH_PARAM_INDEX).GetValueAsInt();
    if (valueInt32 <= 0)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window width parameter (%d) is not greater than 0. " 
                                               "You have to adjust the '%s' parameter.", 
                                                this->GetName().c_str(), valueInt32, 
                                                this->GetParamName(CBratAlgoFilterLoess2D::m_WINDOW_WIDTH_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    m_dataWindowWidth = valueInt32;

    if (CTools::IsDefaultValue(valueInt32))
    {
      m_dataWindowWidth = 3;
    }
    if (CTools::IsEven(m_dataWindowWidth))
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window width parameter (%d) is not an odd number." 
                                               "You have to adjust the '%s' parameter.", 
                                                this->GetName().c_str(), m_dataWindowWidth,
                                                this->GetParamName(CBratAlgoFilterLoess2D::m_WINDOW_WIDTH_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);

    }
  }

  // Set data window height (once)
  if (CTools::IsDefaultValue(m_dataWindowHeight))
  {
    this->CheckConstantParam(CBratAlgoFilterLoess2D::m_WINDOW_HEIGHT_PARAM_INDEX);

    valueInt32 = args.at(CBratAlgoFilterLoess2D::m_WINDOW_HEIGHT_PARAM_INDEX).GetValueAsInt();
    if (valueInt32 <= 0)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window height parameter (%d) is not greater than 0. " 
                                               "You have to adjust the '%s'  parameter.", 
                                                this->GetName().c_str(), valueInt32, 
                                                this->GetParamName(CBratAlgoFilterLoess2D::m_WINDOW_HEIGHT_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    m_dataWindowHeight = valueInt32;

    if (CTools::IsDefaultValue(valueInt32))
    {
      m_dataWindowHeight = 3;
    }

    if (CTools::IsEven(m_dataWindowHeight))
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the window height parameter (%d) is not an odd number."
                                               "You have to adjust the '%s'  parameter.", 
                                                this->GetName().c_str(), m_dataWindowHeight,
                                                this->GetParamName(CBratAlgoFilterLoess2D::m_WINDOW_HEIGHT_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);

    }

  }

  // Set valid points limit (once)
  if (CTools::IsDefaultValue(m_validPts))
  {
    this->CheckConstantParam(CBratAlgoFilterLoess2D::m_VALID_PARAM_INDEX);

    valueInt32 = args.at(CBratAlgoFilterLoess2D::m_VALID_PARAM_INDEX).GetValueAsInt();
    if (valueInt32 <= 0)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because the '%s' parameter (%d) is less than or equal to 0. " 
                                               "You have to adjust the '%s'  parameter.", 
                                                this->GetName().c_str(), 
                                                this->GetParamName(CBratAlgoFilterLoess2D::m_VALID_PARAM_INDEX).c_str(),
                                                valueInt32, 
                                                this->GetParamName(CBratAlgoFilterLoess2D::m_VALID_PARAM_INDEX).c_str()),
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
                                                this->GetParamName(CBratAlgoFilterLoess2D::m_WINDOW_WIDTH_PARAM_INDEX).c_str(),
                                                this->GetParamName(CBratAlgoFilterLoess2D::m_WINDOW_HEIGHT_PARAM_INDEX).c_str(),
                                                this->GetParamName(CBratAlgoFilterLoess2D::m_VALID_PARAM_INDEX).c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }
  }

  // Set extrapolate flag (once)
  if (CTools::IsDefaultValue(m_extrapolate))
  {
    SetParamValueExtrapolate(args, CBratAlgoFilterLoess2D::m_EXTRAPOLATE_PARAM_INDEX);
  }

}

//----------------------------------------
void CBratAlgoFilterLoess2D::OpenProductFile() 
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
void CBratAlgoFilterLoess2D::CheckProduct() 
{
  CheckComplexExpression(CBratAlgoFilterLoess2D::m_VAR_PARAM_INDEX);
  CheckVarExpression(CBratAlgoFilterLoess2D::m_VAR_PARAM_INDEX);
}

//----------------------------------------
void CBratAlgoFilterLoess2D::CheckVarExpression(uint32_t index)
{
  CBratAlgoFilterLoess::CheckVarExpression2D(index);
}

//----------------------------------------
void CBratAlgoFilterLoess2D::Dump(ostream& fOut /*= cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgoFilterLoess2D Object at "<< this << endl;
  CBratAlgoFilterLoess::Dump(fOut);
  fOut << "==> END Dump a CBratAlgoFilterLoess2D Object at "<< this << endl;

}



} // end namespace
