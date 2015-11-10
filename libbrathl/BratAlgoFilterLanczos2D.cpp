
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
#include "Product.h" 
#include "ProductNetCdf.h" 

#include "BratAlgoFilterLanczos2D.h" 

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

//-------------------------------------------------------------
//------------------- CBratAlgoFilterLanczos2D class --------------------
//-------------------------------------------------------------

CBratAlgoFilterLanczos2D::CBratAlgoFilterLanczos2D()
{
  Init();

}
//----------------------------------------
CBratAlgoFilterLanczos2D::CBratAlgoFilterLanczos2D(const CBratAlgoFilterLanczos2D &copy)
{
  Init();

  Set(copy);

}
//----------------------------------------
CBratAlgoFilterLanczos2D::~CBratAlgoFilterLanczos2D()
{
  DeleteFieldNetCdf();

}

//----------------------------------------
void CBratAlgoFilterLanczos2D::Init()
{
  m_expectedTypes.Insert(static_cast<int32_t>(CBratAlgorithmParam::T_DOUBLE));
  m_expectedTypes.Insert(static_cast<int32_t>(CBratAlgorithmParam::T_VECTOR_DOUBLE));

  SetBeginOfFile();
}

//----------------------------------------
void CBratAlgoFilterLanczos2D::Set(const CBratAlgoFilterLanczos2D &o)
{
  CBratAlgoFilterLanczos::Set(o);

  DeleteFieldNetCdf();

}


//----------------------------------------
CBratAlgoFilterLanczos2D& CBratAlgoFilterLanczos2D::operator=(const CBratAlgoFilterLanczos2D &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}


//----------------------------------------
void CBratAlgoFilterLanczos2D::SetBeginOfFile()
{
  CBratAlgoFilterLanczos::SetBeginOfFile();

}

//----------------------------------------
void CBratAlgoFilterLanczos2D::SetEndOfFile()
{
  CBratAlgoFilterLanczos::SetEndOfFile();

}


//----------------------------------------
double CBratAlgoFilterLanczos2D::Run(CVectorBratAlgorithmParam& args)
{

  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();

  if (iRecord == m_callerProductRecordPrev)
  {
    // Do nothing: data have been already computed
    return m_lanczos;
  }

  m_varValueArray = NULL;

  CTools::SetDefaultValue(m_lanczos);

  OpenProductFile();
  
  SetParamValues(args);

  if (GetDataWindowSize() < 1)
  {

    PrepareReturn();
    return m_lanczos;
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

  return m_lanczos;

}
//----------------------------------------
double CBratAlgoFilterLanczos2D::ComputeLanczos(CExpressionValue& exprValue)
{
  CTools::SetDefaultValue(m_lanczos);

  CDoubleArray dataLanczosX;
  
  // Save initial context
  uint32_t countY = m_countY;
  uint32_t countX = m_countX;
  uint32_t addHeightBottom = m_addHeightBottom;
  uint32_t addHeightTop = m_addHeightTop;
  uint32_t addWidthRight = m_addWidthRight;
  uint32_t addWidthLeft = m_addWidthLeft;
  uint32_t gapWidth = m_gapWidth;
  uint32_t gapHeight = m_gapHeight;


  //uint32_t indexX = m_field2DAsRef->GetDimsIndexArray()[0];

  // Read all Y values for n X 
  CExpressionValue exprValueXReading;
  
  PrepareDataReading2DX();
  
  if (m_isComplexExpression.at(CBratAlgoFilterLanczos::m_VAR_PARAM_INDEX))
  {
    if (m_isComplexExpressionWithAlgo.at(CBratAlgoFilterLanczos::m_VAR_PARAM_INDEX))
    {
      PrepareDataValues2DComplexExpressionWithAlgo(exprValueXReading);
    }
    else
    {
      PrepareDataValues2DComplexExpression(exprValueXReading);
    }
  }
  else
  {
    PrepareDataValues2DOneField(exprValueXReading);
  }

  double* dataValue = exprValueXReading.GetValues();
  uint32_t nbValues = exprValueXReading.GetNbValues();

  uint32_t nbValidPoints = nbValues;
  for (uint32_t i = 0 ; i < nbValues ; i++)
  {
    if (CTools::IsDefaultValue(dataValue[i]))
    {
      nbValidPoints--;    
    }
  }

  if (nbValidPoints < m_validPts)
  {
    return m_lanczos;
  }

  // For each X, filter on all Y values (Horizontal mode)
  for (int32_t i = 0 ; i < m_countX ; i++)
  {
    CDoubleArray dataTmp;
    dataTmp.resize(m_countY);

    std::fill(dataTmp.begin(), dataTmp.end(), CTools::m_defaultValueDOUBLE); 

    std::copy(dataValue + (m_countY * i),
        dataValue + (m_countY * (i + 1)), 
        dataTmp.begin());

		// Copy data and compute extension
    PrepareDataWindow(ModeHorizontal, dataTmp);
    // Compute weights
    //SumWeights(ModeHorizontal, 0);
    // Apply filter 
    double lanczosTmp = ApplyFilter(ModeHorizontal, 0);
    // Save result 
    dataLanczosX.Insert(lanczosTmp);
  }

  // Filter to every X (Vertical mode)
  // Copy data and compute extension
  PrepareDataWindow(ModeVertical, dataLanczosX);
    // Compute weights
  //SumWeights(ModeVertical, 0);
    // Apply filter 
  m_lanczos = ApplyFilter(ModeVertical, 0);

  // Restore intial context
  m_countY = countY;
  m_countX = countX;
  m_addHeightBottom = addHeightBottom;
  m_addHeightTop = addHeightTop;
  m_addWidthRight = addWidthRight;
  m_addWidthLeft = addWidthLeft;
  m_gapWidth = gapWidth;
  m_gapHeight = gapHeight;

#if _DEBUG_BRAT_ALGO    
  CTrace::Tracer(3, CTools::Format("lanczos is: %f", m_lanczos));
#endif

  return m_lanczos;

}

//----------------------------------------
double CBratAlgoFilterLanczos2D::ComputeSingle() 
{
  // If 'default value' and no extrapolation then returns
  if (CTools::IsDefaultValue(m_varValue) && (m_extrapolate == 0))
  {
    return m_lanczos;
  }

  PrepareDataReading2DY();
  CExpressionValue exprValue;

  if (m_isComplexExpression.at(CBratAlgoFilterLanczos2D::m_VAR_PARAM_INDEX))
  {
    if (m_isComplexExpressionWithAlgo.at(CBratAlgoFilterLanczos2D::m_VAR_PARAM_INDEX))
    {
      PrepareDataValues2DComplexExpressionWithAlgo(exprValue);
    }
    else
    {
      PrepareDataValues2DComplexExpression(exprValue);
    }
  }
  else
  {
    PrepareDataValues2DOneField(exprValue);
  }

  ComputeLanczos(exprValue);
  
  return m_lanczos;
}

//----------------------------------------
double CBratAlgoFilterLanczos2D::ComputeMean()
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

  return m_lanczos;
}




//----------------------------------------
void CBratAlgoFilterLanczos2D::OpenProductFile() 
{
  CProductNetCdf* productNetCdf = CBratAlgorithmBase::GetProductNetCdf(m_product);

  productNetCdf->SetApplyNetcdfProductInitialisation(false);
  productNetCdf->SetForceReadDataOneByOne(true);

  CBratAlgorithmBase::OpenProductFile();

  if (m_fieldVarsCaller.size() <= 0)
  {
    CheckProduct();
  }


}
//----------------------------------------
void CBratAlgoFilterLanczos2D::CheckProduct() 
{
  CheckComplexExpression(CBratAlgoFilterLanczos2D::m_VAR_PARAM_INDEX);
  CheckVarExpression(CBratAlgoFilterLanczos2D::m_VAR_PARAM_INDEX);
}

//----------------------------------------
void CBratAlgoFilterLanczos2D::CheckVarExpression(uint32_t index)
{
  CBratAlgoFilterLanczos::CheckVarExpression2D(index);
}

//----------------------------------------
void CBratAlgoFilterLanczos2D::Dump(std::ostream& fOut /*= std::cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgoFilterLanczos2D Object at "<< this << std::endl;
  CBratAlgoFilterLanczos::Dump(fOut);
  fOut << "==> END Dump a CBratAlgoFilterLanczos2D Object at "<< this << std::endl;

}



} // end namespace
