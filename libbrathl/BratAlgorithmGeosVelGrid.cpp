
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

#include "TraceLog.h" 
#include "Exception.h" 
#include "Product.h" 
#include "ProductNetCdf.h" 

#include "BratAlgorithmGeosVelGrid.h" 

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

const uint32_t CBratAlgorithmGeosVelGrid::m_INPUT_PARAMS = 4;

const uint32_t CBratAlgorithmGeosVelGrid::m_LAT_PARAM_INDEX = 0;
const uint32_t CBratAlgorithmGeosVelGrid::m_LON_PARAM_INDEX = 1;
const uint32_t CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX = 2;
const uint32_t CBratAlgorithmGeosVelGrid::m_EQUATOR_LAT_LIMIT_INDEX = 3;

//-------------------------------------------------------------
//------------------- CBratAlgorithmGeosVelGrid class --------------------
//-------------------------------------------------------------

CBratAlgorithmGeosVelGrid::CBratAlgorithmGeosVelGrid()
{
  Init();

}
//----------------------------------------
CBratAlgorithmGeosVelGrid::CBratAlgorithmGeosVelGrid(const CBratAlgorithmGeosVelGrid &copy)
{
  Init();

  Set(copy);

}
//----------------------------------------
CBratAlgorithmGeosVelGrid::~CBratAlgorithmGeosVelGrid()
{
  DeleteFieldNetCdf();

}

//----------------------------------------
void CBratAlgorithmGeosVelGrid::Init()
{
  m_expectedTypes.Insert(static_cast<int32_t>(CBratAlgorithmParam::T_DOUBLE));
  m_expectedTypes.Insert(static_cast<int32_t>(CBratAlgorithmParam::T_VECTOR_DOUBLE));

  CTools::SetDefaultValue(m_varValue);
  CTools::SetDefaultValue(m_equatorLimit);

  m_fieldLon = NULL;
  m_fieldLat = NULL;

  SetBeginOfFile();
}

////----------------------------------------
void CBratAlgorithmGeosVelGrid::DeleteFieldNetCdf()
{
  if (m_fieldLon != NULL)
  {
    delete m_fieldLon;
    m_fieldLon = NULL;
  }
  if (m_fieldLat != NULL)
  {
    delete m_fieldLat;
    m_fieldLat = NULL;
  }

  CBratAlgorithmGeosVel::DeleteFieldNetCdf();
}
//----------------------------------------
void CBratAlgorithmGeosVelGrid::Set(const CBratAlgorithmGeosVelGrid &o)
{
  CBratAlgorithmGeosVel::Set(o);
  m_varValue = o.m_varValue;
  m_varValueW = o.m_varValueW;
  m_varValueE = o.m_varValueE;
  m_varValueN = o.m_varValueN;
  m_varValueS = o.m_varValueS;
  m_equatorLimit = o.m_equatorLimit;
  m_allLongitudes = o.m_allLongitudes;
  m_lonMin = o.m_lonMin;
  m_lonMax = o.m_lonMax;
  m_longitudes = o.m_longitudes;
  m_latitudes = o.m_latitudes;
  m_indexLon = o.m_indexLon;
  m_indexLat = o.m_indexLat;
  m_varDimLatIndex = o.m_varDimLatIndex;
  m_varDimLonIndex = o.m_varDimLonIndex;

  DeleteFieldNetCdf();

  if (o.m_fieldLon != NULL)
  {
    m_fieldLon = o.m_fieldLon->CloneThis();
  }
  if (o.m_fieldLat != NULL)
  {
    m_fieldLat = o.m_fieldLat->CloneThis();
  }


}


//----------------------------------------
CBratAlgorithmGeosVelGrid& CBratAlgorithmGeosVelGrid::operator=(const CBratAlgorithmGeosVelGrid &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}


//----------------------------------------
void CBratAlgorithmGeosVelGrid::SetBeginOfFile()
{
  CBratAlgorithmGeosVel::SetBeginOfFile();
  
  m_rawDataCache.DeleteValue();

  CTools::SetDefaultValue(m_varValueW);
  CTools::SetDefaultValue(m_varValueE);
  CTools::SetDefaultValue(m_varValueN);
  CTools::SetDefaultValue(m_varValueS);
  CTools::SetDefaultValue(m_lonMin);
  CTools::SetDefaultValue(m_lonMax);
  m_allLongitudes = false;
  m_longitudes.RemoveAll();
  m_latitudes.RemoveAll();
  m_indexLon = -1;
  m_indexLat = -1;
  m_varDimLatIndex = -1;
  m_varDimLonIndex = -1;

  m_field2DAsRef = NULL;

  DeleteFieldNetCdf();

}

//----------------------------------------
void CBratAlgorithmGeosVelGrid::SetEndOfFile()
{
  CBratAlgorithmGeosVel::SetEndOfFile();
  m_longitudes.RemoveAll();
  m_latitudes.RemoveAll();

}

//----------------------------------------
void CBratAlgorithmGeosVelGrid::DeleteProduct()
{

  DeleteFieldNetCdf();
  
  CBratAlgorithmGeosVel::DeleteProduct();

}


//----------------------------------------
double CBratAlgorithmGeosVelGrid::Run(CVectorBratAlgorithmParam& args)
{
  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();

  if (iRecord == m_callerProductRecordPrev)
  {
    // Do nothing: data have been already computed
    return m_velocity;
  }

  m_latArray = NULL;
  m_lonArray = NULL;
  m_varValueArray = NULL;

  OpenProductFile();

  GetLongitudes();
  GetLatitudes();
  
  SetParamValues(args);

  CTools::SetDefaultValue(m_velocity);

  if ( (m_lonArray == NULL) && (m_latArray == NULL) && (m_varValueArray == NULL) )
  {
    ComputeSingle();
  }
  else
  {
    ComputeMean();
  }

  m_callerProductRecordPrev = iRecord;

  if (iRecord + 1 >= m_nProductRecords)
  {
    SetEndOfFile();
  }


  return m_velocity;

}

//----------------------------------------
double CBratAlgorithmGeosVelGrid::ComputeSingle()
{
  CTools::SetDefaultValue(m_velocity);

  if (CTools::Abs(m_lat) <= m_equatorLimit)
  {
#if _DEBUG_BRAT_ALGO    
    CTrace::Tracer(3, "Algorithm '%s' - Latitude %f is outside computation limit +/- %f - No Computation is done.", this->GetName().c_str(), m_lat, m_equatorLimit);
#endif
    return m_velocity;
  }


  ComputeCoriolis();

  // Compute geostrophic velocity.
  ComputeVelocity();

  return m_velocity;

}
//----------------------------------------
double CBratAlgorithmGeosVelGrid::ComputeMean()
{
  CDoubleArray lonArrayTmp;
  CDoubleArray latArrayTmp;
  CDoubleArray varValueArrayTmp;


  if (m_lonArray == NULL)
  {
    lonArrayTmp.Insert(m_lon);
    m_lonArray = &lonArrayTmp;
  }
  if (m_latArray == NULL)
  {
    latArrayTmp.Insert(m_lat);
    m_latArray = &latArrayTmp;
  }
  if (m_varValueArray == NULL)
  {
    varValueArrayTmp.Insert(m_varValue);
    m_varValueArray = &varValueArrayTmp;
  }

  uint32_t iLon;
  uint32_t iLat;
  uint32_t iVarValue;

  //CDoubleArray* firstArrayTmp = NULL;
  //CDoubleArray* secondArrayTmp = NULL;

  //if (m_varDimLonIndex == 0)
  //{
  //  firstArrayTmp = m_lonArray;
  //  secondArrayTmp = m_latArray;
  //}
  //else if (m_varDimLatIndex == 0)
  //{
  //  firstArrayTmp = m_latArray;
  //  secondArrayTmp = m_lonArray;
  //}

  //uint32_t firstArraySize =  firstArrayTmp->size();
  //uint32_t secondArraySize =  secondArrayTmp->size();
  uint32_t lonArraySize =  m_lonArray->size();
  uint32_t latArraySize =  m_latArray->size();
  
  CTools::SetDefaultValue(m_velocity);
  double countValue = 0.0;
  double dummy = 0.0;

  double velocityTmp;
  CTools::SetDefaultValue(velocityTmp);

  if (m_varDimLonIndex == 0)
  {
    for (iLon = 0 ; iLon < lonArraySize ; iLon++)
    {
      m_lon = m_lonArray->at(iLon);
      m_indexLon = this->GetLongitudeIndex(m_lon);

      for (iLat = 0 ; iLat < latArraySize ; iLat++)
      {
        m_lat = m_latArray->at(iLat);
        m_indexLat = this->GetLatitudeIndex(m_lat);

        iVarValue = (iLon * m_latArray->size()) + iLat;
        m_varValue = m_varValueArray->at(iVarValue);
        
        ComputeSingle();

	      CTools::DoIncrementalStats(m_velocity,
	                           countValue,
	                           velocityTmp,
	                           dummy,
	                           dummy,
	                           dummy);
      }
    }
  }
  else if (m_varDimLatIndex == 0)
  {
    for (iLat = 0 ; iLat < latArraySize ; iLat++)
    {
      m_lat = m_latArray->at(iLat);
      m_indexLat = this->GetLatitudeIndex(m_lat);

      for (iLon = 0 ; iLon < lonArraySize ; iLon++)
      {
        m_lon = m_lonArray->at(iLon);
        m_indexLon = this->GetLongitudeIndex(m_lon);

        iVarValue = (iLat * m_lonArray->size()) + iLon;
        m_varValue = m_varValueArray->at(iVarValue);
        
        ComputeSingle();

	      CTools::DoIncrementalStats(m_velocity,
	                           countValue,
	                           velocityTmp,
	                           dummy,
	                           dummy,
	                           dummy);
      }
    }
  }

  m_velocity = velocityTmp;
  return m_velocity;
}

//----------------------------------------
bool CBratAlgorithmGeosVelGrid::PrepareComputeVelocity() 
{
  int32_t lastIndexLon = m_longitudes.size() - 1;
  int32_t lastIndexLat = m_latitudes.size() - 1;


  if ((m_indexLat == 0) || (m_indexLat == lastIndexLat))
  {
    return false;
  }

  int32_t im1;
  int32_t ip1;
  bool cache = true;

  if (m_indexLon == 0)
  {
    if (m_allLongitudes)
    {
      im1 = lastIndexLon;
	    ip1 = m_indexLon + 1;
      cache = false;
    }
    else
    {
	    im1 = -1;
	    ip1 = -1;
    }
  }
  else if (m_indexLon == lastIndexLon)
  {
    if (m_allLongitudes)
    {
	    im1 = m_indexLon - 1;
	    ip1 = 0;
      cache = false;
    }
    else
    {
      im1 = -1;
	    ip1 = -1;
    }
  }
  else
  {
    im1 = m_indexLon - 1;
    ip1 = m_indexLon + 1;
  }

  if ((im1 < 0) || (ip1 < 0))
  {
    return false;
  }

  if (cache)
  {
    int32_t minIndexLon = (im1 <= m_indexLon ? im1 : m_indexLon);
    int32_t maxIndexLon = (ip1 >= m_indexLon ? ip1 : m_indexLon);
    int32_t minIndexLat = m_indexLat - 1;
    int32_t maxIndexLat = m_indexLat + 1;
    this->GetVarCacheExpressionValue(minIndexLat, maxIndexLat, minIndexLon, maxIndexLon);
  }
  else
  {
    m_rawDataCache.DeleteValue();
  }

  if (cache)
  {
    m_varValueW = this->GetVarExpressionValueCache(m_indexLat, ip1);
    m_varValueE = this->GetVarExpressionValueCache(m_indexLat, im1);
    m_varValueN = this->GetVarExpressionValueCache(m_indexLat + 1, m_indexLon);
    m_varValueS = this->GetVarExpressionValueCache(m_indexLat - 1, m_indexLon);
  }
  else
  {
    m_varValueW = this->GetVarExpressionValue(m_indexLat, ip1);
    m_varValueE = this->GetVarExpressionValue(m_indexLat, im1);
    m_varValueN = this->GetVarExpressionValue(m_indexLat + 1, m_indexLon);
    m_varValueS = this->GetVarExpressionValue(m_indexLat - 1, m_indexLon);
  }
  
  if ( CTools::IsDefaultValue(m_varValueW) || CTools::IsDefaultValue(m_varValueE) 
    || CTools::IsDefaultValue(m_varValueN) || CTools::IsDefaultValue(m_varValueS) 
    || CTools::IsDefaultValue(m_varValue))
  {
    return false;
  }

  m_latPrev = m_latitudes.at(m_indexLat - 1);
  m_latNext = m_latitudes.at(m_indexLat + 1);

  m_lonPrev = m_longitudes.at(im1);
  m_lonNext = m_longitudes.at(ip1);


  return true;

}


//----------------------------------------
void CBratAlgorithmGeosVelGrid::CheckInputParams(CVectorBratAlgorithmParam& args) 
{
  CBratAlgorithmBase::CheckInputParams(args);

  for (uint32_t indexParam = 0 ; indexParam < CBratAlgorithmGeosVelGrid::m_INPUT_PARAMS ; indexParam++)
  {
    if ((indexParam == CBratAlgorithmGeosVelGrid::m_LAT_PARAM_INDEX) ||
      (indexParam == CBratAlgorithmGeosVelGrid::m_LON_PARAM_INDEX) ||
      (indexParam == CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX))
    {
      CheckInputTypeParams(indexParam, m_expectedTypes, args);
      continue;
    }
    CheckInputTypeParams(indexParam, m_expectedTypes, args);
  }

  m_latArray = NULL;
  m_lonArray = NULL;
  m_varValueArray = NULL;

  CBratAlgorithmParam* algoParam = &(args.at(CBratAlgorithmGeosVelGrid::m_LAT_PARAM_INDEX));

  if (algoParam->GetTypeVal() == CBratAlgorithmParam::T_VECTOR_DOUBLE)
  {
    m_latArray = algoParam->GetValueAsVectDouble();
  }

  algoParam = &(args.at(CBratAlgorithmGeosVelGrid::m_LON_PARAM_INDEX));

  if (algoParam->GetTypeVal() == CBratAlgorithmParam::T_VECTOR_DOUBLE)
  {
    m_lonArray = algoParam->GetValueAsVectDouble();
  }

  algoParam = &(args.at(CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX));

  if (algoParam->GetTypeVal() == CBratAlgorithmParam::T_VECTOR_DOUBLE)
  {
    m_varValueArray = algoParam->GetValueAsVectDouble();
  }

  if (((m_lonArray != NULL) || (m_latArray != NULL)) && (m_varValueArray == NULL))
  {
    throw CAlgorithmException(CTools::Format("Error while running algorithm: Parameter are not consistent: Parameter %d has type %d, parameter %d has type %d, and parameter %d has type %d but it should have type %d",
                                        CBratAlgorithmGeosVelGrid::m_LAT_PARAM_INDEX,
                                        static_cast<int32_t>(args.at(CBratAlgorithmGeosVelGrid::m_LAT_PARAM_INDEX).GetTypeVal()),
                                        CBratAlgorithmGeosVelGrid::m_LON_PARAM_INDEX,
                                        static_cast<int32_t>(args.at(CBratAlgorithmGeosVelGrid::m_LON_PARAM_INDEX).GetTypeVal()),
                                        CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX,
                                        static_cast<int32_t>(args.at(CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX).GetTypeVal()),
                                        CBratAlgorithmParam::T_VECTOR_DOUBLE),
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

}

//----------------------------------------
void CBratAlgorithmGeosVelGrid::SetParamValues(CVectorBratAlgorithmParam& args)
{    
  CheckInputParams(args);


  if (m_latArray == NULL)
  {
    // Set latitude
    m_lat = args.at(CBratAlgorithmGeosVelGrid::m_LAT_PARAM_INDEX).GetValueAsDouble();
    m_indexLat = this->GetLatitudeIndex(m_lat);
  }
  if (m_lonArray == NULL)
  {
  // Set longitude
    m_lon = args.at(CBratAlgorithmGeosVelGrid::m_LON_PARAM_INDEX).GetValueAsDouble();
    m_indexLon = this->GetLongitudeIndex(m_lon);
  }
  if (m_varValueArray == NULL)
  {
    // Set variable
    m_varValue = args.at(CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX).GetValueAsDouble();
  }

  // Set equator limit (once)
  if (CTools::IsDefaultValue(m_equatorLimit))
  {
    CheckEquatorLimit();
    m_equatorLimit = args.at(CBratAlgorithmGeosVelGrid::m_EQUATOR_LAT_LIMIT_INDEX).GetValueAsDouble();
  }
}

//----------------------------------------
void CBratAlgorithmGeosVelGrid::CheckEquatorLimit()
{
  uint32_t size = static_cast<uint32_t>(m_algoParamExpressions.size());

  if (CBratAlgorithmGeosVelGrid::m_EQUATOR_LAT_LIMIT_INDEX >= size)
  {
    throw CAlgorithmException(CTools::Format("Error while checking equator limit parameter: index (%ld) is out-of-range. Valid range is [0, %ld]", (long)CBratAlgorithmGeosVelGrid::m_EQUATOR_LAT_LIMIT_INDEX, (long)size), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  CExpression* expr = CExpression::GetExpression(m_algoParamExpressions.at(CBratAlgorithmGeosVelGrid::m_EQUATOR_LAT_LIMIT_INDEX), true);

  bool invalid = expr->HasFieldNames();
  if (invalid)
  {
    throw CAlgorithmException(CTools::Format("Equator limit parameter (index %ld) must be a constant value and must not refer to data fields. Parameter expression is: '%s'", (long)CBratAlgorithmGeosVelGrid::m_EQUATOR_LAT_LIMIT_INDEX, expr->AsString().c_str()), this->GetName(), BRATHL_LOGIC_ERROR);
  }

}
//----------------------------------------
void CBratAlgorithmGeosVelGrid::GetVarCacheExpressionValue(int32_t minIndexLat, int32_t maxIndexLat, int32_t minIndexLon, int32_t maxIndexLon)
{
  bool readNeeded = true;
  
  if (m_rawDataCache.HasValue())
  {
    if (m_field2DAsRef == NULL)
    {
      throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because of an internal error: ERROR in CBratAlgorithmGeosVelGrid::GetVarCacheExpressionValue: reference 2D field is NULL and has  not been set. This function shouldn't happened. There is something wrong in the Brat software."
                                                "\nPlease contact Brat Helpdesk", 
                                                this->GetName().c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);

    }

    if (m_varDimLonIndex == 0)
    {
      readNeeded = (static_cast<int32_t>(m_field2DAsRef->GetDimsIndexArray()[m_varDimLonIndex]) != minIndexLon); 
    }
    else if (m_varDimLatIndex == 0)
    {
      readNeeded = (static_cast<int32_t>(m_field2DAsRef->GetDimsIndexArray()[m_varDimLatIndex]) != minIndexLat);
    }

  }

  if (!readNeeded)
  {
    return;
  }

  PrepareDataReading2D(minIndexLat, maxIndexLat, minIndexLon, maxIndexLon);

  if (m_isComplexExpression.at(CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX))
  {
    if (m_isComplexExpressionWithAlgo.at(CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX))
    {
      PrepareDataValues2DComplexExpressionWithAlgo(m_rawDataCache);
    }
    else
    {
      PrepareDataValues2DComplexExpression(m_rawDataCache);
    }
  }
  else
  {
    PrepareDataValues2DOneField(m_rawDataCache);
  }
  
  //m_rawDataCache.Dump(*CTrace::GetDumpContext());

}



//----------------------------------------
void CBratAlgorithmGeosVelGrid::PrepareDataReading2D(int32_t minIndexLat, int32_t maxIndexLat, int32_t minIndexLon, int32_t maxIndexLon)
{
  m_field2DAsRef = NULL;

  CObMap::iterator it;

  // Set number of data to read for 2D fields
  for (it = m_fieldVars.begin() ; it != m_fieldVars.end() ; it++)
  {
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(it->second);
    int32_t nbDims = field->GetNbDims();
    if (nbDims != 2)
    {
      continue;
    }

    if (field->GetDimsIndexArray() == NULL)
    {
      field->NewDimIndexArray();
    }

    if (m_field2DAsRef == NULL)
    {
      m_field2DAsRef = field;
    }


    if (m_varDimLonIndex == 0)
    {
      field->GetDimsIndexArray()[m_varDimLonIndex] = minIndexLon;
      field->GetDimsIndexArray()[m_varDimLatIndex] = 0;
      field->GetDimsCountArray()[m_varDimLonIndex] = maxIndexLon - minIndexLon + 1;
      field->GetDimsCountArray()[m_varDimLatIndex] = m_latitudes.size();
    }
    else if (m_varDimLatIndex == 0)
    {
      field->GetDimsIndexArray()[m_varDimLonIndex] = 0;
      field->GetDimsIndexArray()[m_varDimLatIndex] = minIndexLat;
      field->GetDimsCountArray()[m_varDimLonIndex] = m_longitudes.size();
      field->GetDimsCountArray()[m_varDimLatIndex] = maxIndexLat - minIndexLat + 1;
    }
  }
  // Set number of data to read for 1D fields
  for (it = m_fieldVars.begin(); it != m_fieldVars.end() ; it++)
  { 
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(it->second);
    
    int32_t nbDims = field->GetNbDims();
    if (nbDims != 1)
    {
      continue;
    }
    
    if (field->GetDimsIndexArray() == NULL)
    {
      field->NewDimIndexArray();
    }

    CFieldNetCdf* fieldTemp = CExternalFiles::GetFieldNetCdf(m_fieldDependOnXDim.Exists(it->first), false);
    if (fieldTemp != NULL)
    {
      field->GetDimsIndexArray()[0] = m_field2DAsRef->GetDimsIndexArray()[0];
      field->GetDimsCountArray()[0] = m_field2DAsRef->GetDimsCountArray()[0];
      continue;
    }

    fieldTemp = CExternalFiles::GetFieldNetCdf(m_fieldDependOnYDim.Exists(it->first), false);
    if (fieldTemp != NULL)
    {
      field->GetDimsIndexArray()[0] = m_field2DAsRef->GetDimsIndexArray()[1];
      field->GetDimsCountArray()[0] = m_field2DAsRef->GetDimsCountArray()[1];
    }

  }
}


//----------------------------------------
void CBratAlgorithmGeosVelGrid::PrepareDataReading2D(int32_t indexLat, int32_t indexLon)
{
  m_field2DAsRef = NULL;

  CObMap::iterator it;

  // Set number of data to read for 2D fields
  for (it = m_fieldVars.begin() ; it != m_fieldVars.end() ; it++)
  {
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(it->second);
    int32_t nbDims = field->GetNbDims();
    if (nbDims != 2)
    {
      continue;
    }

    if (field->GetDimsIndexArray() == NULL)
    {
      field->NewDimIndexArray();
    }

    if (m_field2DAsRef == NULL)
    {
      m_field2DAsRef = field;
    }


    field->GetDimsIndexArray()[m_varDimLonIndex] = indexLon;
    field->GetDimsIndexArray()[m_varDimLatIndex] = indexLat;
    field->GetDimsCountArray()[m_varDimLonIndex] = 1;
    field->GetDimsCountArray()[m_varDimLatIndex] = 1;
  }

  // Set number of data to read for 1D fields
  for (it = m_fieldVars.begin(); it != m_fieldVars.end() ; it++)
  { 
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(it->second);
    
    int32_t nbDims = field->GetNbDims();
    if (nbDims != 1)
    {
      continue;
    }

    CFieldNetCdf* fieldTemp = CExternalFiles::GetFieldNetCdf(m_fieldDependOnXDim.Exists(it->first), false);
    if (fieldTemp != NULL)
    {
      field->GetDimsIndexArray()[0] = m_field2DAsRef->GetDimsIndexArray()[0];
      field->GetDimsCountArray()[0] = m_field2DAsRef->GetDimsCountArray()[0];
      continue;
    }

    fieldTemp = CExternalFiles::GetFieldNetCdf(m_fieldDependOnYDim.Exists(it->first), false);
    if (fieldTemp != NULL)
    {
      field->GetDimsIndexArray()[0] = m_field2DAsRef->GetDimsIndexArray()[1];
      field->GetDimsCountArray()[0] = m_field2DAsRef->GetDimsCountArray()[1];
    }

  }
}


//----------------------------------------
void CBratAlgorithmGeosVelGrid::PrepareDataValues2DOneField(CExpressionValue& exprValue)
{
  CBratAlgorithmGeosVel::PrepareDataValues2DOneField(exprValue, CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX);
  //CProductNetCdf* productNetCdf = CBratAlgorithmBase::GetProductNetCdf(m_product);

  //CExpression* expr = CExpression::GetExpression(m_algoParamExpressions.at(CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX),true);
  //const CStringArray* fieldNames = expr->GetFieldNames();

  //uint32_t nbFields = fieldNames->size();
  //if (nbFields != 1)
  //{
  //  throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because of an internal error: ERROR in CBratAlgoFilterKernel::PrepareDataValues2DOneField: number of fields is: %d and the expected number is 1."
  //                                           " This function shouldn't be called here. There is something wrong in the Brat software."
  //                                           "\nPlease contact Brat Helpdesk", 
  //                                           this->GetName().c_str(), nbFields),
  //                            this->GetName(), BRATHL_LOGIC_ERROR);

  //}


  //// Read data
  //string fieldName = fieldNames->at(0);
  //CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(m_fieldVars.Exists(fieldName));
 
  //field->SetAtBeginning(false);
  //  
  //productNetCdf->GetExternalFile()->GetValues(fieldName, exprValue, CUnit::m_UNIT_SI);
    
}



//----------------------------------------
void CBratAlgorithmGeosVelGrid::PrepareDataValues2DComplexExpressionWithAlgo(CExpressionValue& exprValue)
{
  CBratAlgorithmGeosVel::PrepareDataValues2DComplexExpressionWithAlgo(exprValue, CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX);
  //NewExpressionValuesArray();

  //CObArray::const_iterator itParamsExpr;
  //for (itParamsExpr = m_algoParamExpressions.begin() ; itParamsExpr != m_algoParamExpressions.end() ; itParamsExpr++)
  //{
  //  CObArray* expressionDataValues = new CObArray();
  //  CExpressionValues* expressionValues = new CExpressionValues(expressionDataValues);
  //  m_expressionValuesArray->Insert(expressionValues);
  //}

  //SetField2DAsRef();

  //int32_t offset = m_field2DAsRef->GetPosFromDimIndexArray();
  //int32_t count = m_field2DAsRef->GetCounFromDimCountArray();

  //// Because the dimension array (index and count) will be change during reading
  //// Save them and restore them after data reading
  //uint32_t dimIndexArraySaved[2];
  //dimIndexArraySaved[0] = m_field2DAsRef->GetDimsIndexArray()[0];
  //dimIndexArraySaved[1] = m_field2DAsRef->GetDimsIndexArray()[1];
  //
  //uint32_t dimCountArraySaved[2];
  //dimCountArraySaved[0] = m_field2DAsRef->GetDimsCountArray()[0];
  //dimCountArraySaved[1] = m_field2DAsRef->GetDimsCountArray()[1];
  //
  //int32_t iRecord = 0;
  //for (int32_t i = 0 ; i < count ; i++)
  ////for (int32_t iRecord = nRecords - 1 ; iRecord >= 0 ; iRecord--)
  //{
  //  iRecord = i + offset;
  //  m_product->ReadBratRecord(iRecord);

  //  CObArray::iterator itDataSet;
  //  CRecordSet* recordSet = NULL;

  //  CDataSet* dataSet = m_product->GetDataSet();
  //  string recordName = m_product->GetDataSetNameToRead();

  //    //dataSet->Dump(*CTrace::GetDumpContext());

  //  for (itDataSet = dataSet->begin(); itDataSet != dataSet->end() ; itDataSet++)
  //  {
  //    recordSet = dataSet->GetRecordSet(itDataSet);
  //    for (uint32_t indexExpr = 0 ; indexExpr < m_algoParamExpressions.size() ; indexExpr++)
  //    {
  //       CExpression* expr = CExpression::GetExpression(m_algoParamExpressions.at(indexExpr));
  //      if (expr == NULL)
  //      {
  //        continue;
  //      }

  //      //CExpressionValues::GetExpressionValues(m_expressionValuesArray->at(indexExpr));
  //      CObArray* expressionDataValues = this->GetDataExpressionValues(indexExpr);
  //      
  //      CExpressionValue* exprValueTmp = new CExpressionValue();
  //      expressionDataValues->Insert(exprValueTmp);

  //      recordSet->ExecuteExpression(*expr, recordName, *exprValueTmp, m_product);

  //    }
  //  }
  //}

  //// Restore the dimension array (index and count)
  //m_field2DAsRef->GetDimsIndexArray()[0] = dimIndexArraySaved[0];
  //m_field2DAsRef->GetDimsIndexArray()[1] = dimIndexArraySaved[1];
  //
  //m_field2DAsRef->GetDimsCountArray()[0] = dimCountArraySaved[0];
  //m_field2DAsRef->GetDimsCountArray()[1] = dimCountArraySaved[1];

  //double* dataValue = NULL;
  //uint32_t nbValues = 0;

  //this->GetExpressionDataValuesAsArrayOfSingleValue(CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX, dataValue, nbValues);

  //exprValue.SetNewValue(dataValue, nbValues);

  //if (dataValue != NULL)
  //{
  //  delete []dataValue;
  //  dataValue = NULL;
  //  nbValues = 0;
  //}

}
//----------------------------------------
void CBratAlgorithmGeosVelGrid::PrepareDataValues2DComplexExpression(CExpressionValue& exprValue)
{
  CBratAlgorithmGeosVel::PrepareDataValues2DComplexExpression(exprValue, CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX);
  
  //NewExpressionValuesArray();
  //CObMap expressionValuesName;

  //CProductNetCdf* productNetCdf = CBratAlgorithmBase::GetProductNetCdf(m_product);

  //CObMap::const_iterator itMap;

  //// Read data
  //for (itMap = m_fieldVars.begin(); itMap != m_fieldVars.end() ; itMap++)
  //{ 
  //  CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(itMap->second);

  //  field->SetAtBeginning(false);
  //  
  //  string fieldName = field->GetName();
  //  //CTrace::Tracer(fieldName);

  //  CExpressionValue* values = new CExpressionValue();

  //  productNetCdf->GetExternalFile()->GetValues(fieldName, *values, CUnit::m_UNIT_SI);

  //  // Store the data values
  //  expressionValuesName.Insert(fieldName, values);    
  //}

  //if (m_field2DAsRef == NULL)
  //{
  //  throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because of an internal error: ERROR in CBratAlgorithmGeosVelGrid::PrepareDataValues2DComplexExpression: reference 2D field is NULL and has  not been set. This function shouldn't happened. There is something wrong in the Brat software."
  //                                            "\nPlease contact Brat Helpdesk", 
  //                                            this->GetName().c_str()),
  //                            this->GetName(), BRATHL_LOGIC_ERROR);

  //}

  //uint32_t commonDimX = m_field2DAsRef->GetDimsCountArray()[0];
  //uint32_t commonDimY = m_field2DAsRef->GetDimsCountArray()[1];

  //// Compute expressions and store computed values
  //CObArray::const_iterator itParamsExpr;
  //for (itParamsExpr = m_algoParamExpressions.begin() ; itParamsExpr != m_algoParamExpressions.end() ; itParamsExpr++)
  //{
  //  CObArray* expressionDataValues = new CObArray();
  //  CExpressionValues* expressionValues = new CExpressionValues(expressionDataValues);


  //  CExpression* expr = CExpression::GetExpression(*itParamsExpr, false);
  //  //CTrace::Tracer(expr->AsString());

  //  if (expr == NULL)
  //  {
  //    throw CAlgorithmException("Error while running algorithm (read product data): parameter expression is NULL or not a CExpression object", this->GetName(), BRATHL_LOGIC_ERROR);
  //  }

  //  const CStringArray* fieldNames = expr->GetFieldNames();
  //  
  //  //fieldNames->Dump(*CTrace::GetDumpContext());

  //  CStringArray::const_iterator itFieldName;

  //  for (uint32_t x = 0 ; x < commonDimX ; x++)
  //  {
  //    for (uint32_t y = 0 ; y < commonDimY ; y++)
  //    {
  //      // Set the values for each fields in the expression
  //      for (itFieldName = fieldNames->begin(); itFieldName != fieldNames->end() ; itFieldName++)
  //      {
  //        //CTrace::Tracer(*itFieldName);

  //        CExpressionValue* exprValueTmp = dynamic_cast<CExpressionValue*>(expressionValuesName.Exists(*itFieldName));
  //        if (exprValueTmp == NULL)
  //        {
  //          continue;
  //        }

  //        double value;
  //        CTools::SetDefaultValue(value);

  //        if (CExternalFiles::GetFieldNetCdf(m_fieldDependOnXYDim.Exists(*itFieldName), false) != NULL)
  //        {
  //          value = exprValueTmp->GetValue(x, y);
  //        }
  //        else if (CExternalFiles::GetFieldNetCdf(m_fieldDependOnXDim.Exists(*itFieldName), false) != NULL)
  //        {
  //          value = exprValueTmp->GetValue(x);
  //        }
  //        else if (CExternalFiles::GetFieldNetCdf(m_fieldDependOnYDim.Exists(*itFieldName), false) != NULL)
  //        {
  //          value = exprValueTmp->GetValue(y);
  //        }

  //        expr->SetValue(*itFieldName, value);

  //      }
  //      // All values are filled for the expression, now execute the expression
  //      CExpressionValue* exprValue = new CExpressionValue(expr->Execute(m_product));
  //      //exprValue->Dump(*CTrace::GetDumpContext());

  //      expressionDataValues->Insert(exprValue);    
  //    }
  //  }

  //  // Set explicit 2D dimensions
  //  expressionValues->SetDimensions(commonDimX, commonDimY);

  //  m_expressionValuesArray->Insert(expressionValues);

  //}

  //double* dataValue = NULL;
  //uint32_t nbValues = 0;

  //this->GetExpressionDataValuesAsArrayOfSingleValue(CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX, dataValue, nbValues);

  //exprValue.SetNewValue(dataValue, nbValues);

  //if (dataValue != NULL)
  //{
  //  delete []dataValue;
  //  dataValue = NULL;
  //  nbValues = 0;
  //}

}
//----------------------------------------
double CBratAlgorithmGeosVelGrid::GetVarExpressionValue(int32_t indexLat, int32_t indexLon)
{

  PrepareDataReading2D(indexLat, indexLat);

  CExpressionValue exprValue;

  if (m_isComplexExpression.at(CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX))
  {

    if (m_isComplexExpressionWithAlgo.at(CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX))
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
  
  return exprValue.GetValue(0);
  
}
//----------------------------------------
double CBratAlgorithmGeosVelGrid::GetVarExpressionValueCache(int32_t indexLat, int32_t indexLon)
{
  if (m_field2DAsRef == NULL)
  {
    throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because of an internal error: ERROR in CBratAlgorithmGeosVelGrid::GetVarExpressionValueCache: reference 2D field is NULL and has  not been set. This function shouldn't happened. There is something wrong in the Brat software."
                                              "\nPlease contact Brat Helpdesk", 
                                              this->GetName().c_str()),
                              this->GetName(), BRATHL_LOGIC_ERROR);

  }

  int32_t index = 0;

  if (m_varDimLonIndex == 0)
  {
    int32_t offset = m_field2DAsRef->GetDimsIndexArray()[m_varDimLonIndex];
    index = ((indexLon - offset) * (m_latitudes.size())) + indexLat;
  }
  else if (m_varDimLatIndex == 0)
  {
    int32_t offset = m_field2DAsRef->GetDimsIndexArray()[m_varDimLatIndex];
    index = ((indexLat - offset) * (m_longitudes.size())) + indexLon;
  }
  else
  {
    throw CAlgorithmException(CTools::Format("ERROR in CBratAlgorithmGeosVelGrid::GetVarExpressionValueCache - latitude index (%d) and/or longitude index (%d) have not been set.",
                                              m_varDimLatIndex, m_varDimLonIndex),
                              this->GetName(), BRATHL_LOGIC_ERROR);

  }

  return m_rawDataCache.GetValue(index);
}

//----------------------------------------
void CBratAlgorithmGeosVelGrid::GetLongitudes() 
{
  if (m_longitudes.size() > 0)
  {
    return; 
  }

  CExpression* expr = CExpression::GetExpression(m_algoParamExpressions.at(CBratAlgorithmGeosVelGrid::m_LON_PARAM_INDEX), true);

  CProductNetCdf* productNetCdf = CBratAlgorithmBase::GetProductNetCdf(m_product);

  OpenProductFile();

  if (m_fieldLon == NULL)
  {
    throw CAlgorithmException("Error while getting longitude. No longitude field has been set.",
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

  CExpressionValue values;

  string fieldName = expr->GetFieldNames()->at(0);

  productNetCdf->GetExternalFile()->GetAllValues(fieldName, values, CUnit::m_UNIT_SI);
  
  expr->SetValue(fieldName, values);
  
  CExpressionValue result = expr->Execute(m_callerProduct);
  m_longitudes.Insert(result.GetValues(), result.GetNbValues());
 
  if (m_longitudes.size() <= 0)
  {
    throw CAlgorithmException(CTools::Format("Error while getting longitudes: No longitude have been read. 'Longitude' expression is: '%s'",
                                             expr->AsString().c_str()),
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

  m_lonMin = m_longitudes.at(0);
  m_lonMax = m_longitudes.at(m_longitudes.size() - 1);
  m_allLongitudes =  (CTools::IsLongitudeCircular(m_lonMin, m_lonMax, 2.0));
}
//----------------------------------------
void CBratAlgorithmGeosVelGrid::GetLatitudes() 
{
  if (m_latitudes.size() > 0)
  {
    return; 
  }

  CExpression* expr = CExpression::GetExpression(m_algoParamExpressions.at(CBratAlgorithmGeosVelGrid::m_LAT_PARAM_INDEX), true);
   
  CProductNetCdf* productNetCdf = CBratAlgorithmBase::GetProductNetCdf(m_product);

  OpenProductFile();

  if (m_fieldLat == NULL)
  {
    throw CAlgorithmException("Error while getting latitude. No latitude field has been set.",
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

  CExpressionValue values;

  string fieldName = expr->GetFieldNames()->at(0);

  productNetCdf->GetExternalFile()->GetAllValues(fieldName, values, CUnit::m_UNIT_SI);
  
  expr->SetValue(fieldName, values);
  
  CExpressionValue result = expr->Execute(m_callerProduct);
  m_latitudes.Insert(result.GetValues(), result.GetNbValues());

  if (m_latitudes.size() <= 0)
  {
    throw CAlgorithmException(CTools::Format("Error while getting latitudes: No latitude have been read. 'Latitude' expression is: '%s'", expr->AsString().c_str()), this->GetName(), BRATHL_LOGIC_ERROR);
  }

}
//----------------------------------------
int32_t CBratAlgorithmGeosVelGrid::GetLongitudeIndex(double value) 
{
  if (m_longitudes.size() <= 0)
  {
    this->GetLongitudes();
  }

  int32_t index = m_longitudes.FindIndex(value);

  if (index < 0)
  {
    throw CAlgorithmException(CTools::Format("Error while getting longitude index: Longitude '%5.20f' not found.", value), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  return index;
}
//----------------------------------------
int32_t CBratAlgorithmGeosVelGrid::GetLatitudeIndex(double value) 
{
  if (m_latitudes.size() <= 0)
  {
    this->GetLatitudes();
  }

  int32_t index = m_latitudes.FindIndex(value);

  if (index < 0)
  {
    throw CAlgorithmException(CTools::Format("Error while getting latitude index: Latitude '%5.20f' not found.", value), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  return index;
}

//----------------------------------------
void CBratAlgorithmGeosVelGrid::OpenProductFile() 
{
  CProductNetCdf* productNetCdf = CBratAlgorithmBase::GetProductNetCdf(m_product);

  productNetCdf->SetApplyNetcdfProductInitialisation(false);
  productNetCdf->SetForceReadDataOneByOne(true);

  CBratAlgorithmBase::OpenProductFile();

  if ( (m_varDimLatIndex < 0) && (m_varDimLonIndex < 0) )
  {
    CheckProduct();
  }


}
//----------------------------------------
void CBratAlgorithmGeosVelGrid::CheckProduct() 
{
  CheckComplexExpression(CBratAlgorithmGeosVelGrid::m_LAT_PARAM_INDEX);
  CheckComplexExpression(CBratAlgorithmGeosVelGrid::m_LON_PARAM_INDEX);
  CheckComplexExpression(CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX);

  // First, call common checking of the 'variable' parameter expression 
  CBratAlgorithmGeosVel::CheckVarExpression2D(CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX);
  // Then, check the lat/lon parameters expressions
  CheckLatLonExpression(CBratAlgorithmGeosVelGrid::m_LAT_PARAM_INDEX);
  CheckLatLonExpression(CBratAlgorithmGeosVelGrid::m_LON_PARAM_INDEX);  
  // Then, call specific checking of the 'variable' parameter expression
  CheckVarExpression(CBratAlgorithmGeosVelGrid::m_VAR_PARAM_INDEX);

}

//----------------------------------------
void CBratAlgorithmGeosVelGrid::CheckLatLonExpression(uint32_t index) 
{
  CProductNetCdf* productNetCdfCaller = CBratAlgorithmBase::GetProductNetCdf(m_callerProduct);
  CProductNetCdf* productNetCdf = CBratAlgorithmBase::GetProductNetCdf(m_product);

  CExpression* expr = CExpression::GetExpression(m_algoParamExpressions.at(index), true);
  
  const CStringArray* fields = expr->GetFieldNames();
  
  int32_t size = static_cast<int32_t>(fields->size());
  
  if (size != 1)
  {
    throw CAlgorithmException(CTools::Format("Parameter %d is not valid: only one field is allowed, but expression '%s' contains %d field(s).",
                                              index,
                                              expr->AsString().c_str(),
                              size), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  CFieldNetCdf* field = productNetCdf->GetExternalFile()->GetFieldNetCdf(fields->at(0), false);

  const CStringArray& dims = field->GetDimNames();
  
  int32_t dimsSize = static_cast<int32_t>(dims.size());
  
  if (dimsSize != 1)
  {
    throw CAlgorithmException(CTools::Format("Parameter %d is not valid: field '%s' is not a one-dimensional field (dims: '%s').",
                                             index, 
                                             field->GetName().c_str(), 
                                             field->GetDimAsString().c_str()),
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }


  if (index == CBratAlgorithmGeosVelGrid::m_LAT_PARAM_INDEX)
  {
    m_fieldLat = field->CloneThis();
  }
  if (index == CBratAlgorithmGeosVelGrid::m_LON_PARAM_INDEX)
  {
    m_fieldLon = field->CloneThis();
  }


  string fieldName = field->GetName();

  m_fieldVars.Insert(fieldName, field, false);

  CFieldNetCdf* fieldCaller = productNetCdfCaller->GetExternalFile()->GetFieldNetCdf(fieldName, true);
  m_fieldVarsCaller.Insert(fieldName, fieldCaller, false);

  SetField2DAsRef();

  this->AddXOrYFieldDependency(field, m_field2DAsRef);

}
//----------------------------------------
uint32_t CBratAlgorithmGeosVelGrid::GetLonDimRange(CFieldNetCdf* field)
{
  if (field == NULL)
  {
    throw CAlgorithmException("Error while getting longitude range. Input field is NULL.",
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }
  if (m_fieldLon == NULL)
  {
    throw CAlgorithmException("Error while getting longitude range. No longitude field has been set.",
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

  return field->GetDimRange(m_fieldLon->GetName());
}
//----------------------------------------
uint32_t CBratAlgorithmGeosVelGrid::GetLatDimRange(CFieldNetCdf* field)
{
  if (field == NULL)
  {
    throw CAlgorithmException("Error while getting latitude range. Input field is NULL.",
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

  if (m_fieldLat == NULL)
  {
    throw CAlgorithmException("Error while getting latitude range. No latitude field has been set.",
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

  return field->GetDimRange(m_fieldLat->GetName());

}
//----------------------------------------
void CBratAlgorithmGeosVelGrid::CheckVarExpression(uint32_t index)
{
  if (m_fieldLat == NULL)
  {
    throw CAlgorithmException("Error while checking variable expression. No latitude field has been set.",
                              this->GetName(), BRATHL_LOGIC_ERROR);

  }
  if (m_fieldLon == NULL)
  {
    throw CAlgorithmException("Error while checking variable expression. No longitude field has been set.",
                              this->GetName(), BRATHL_LOGIC_ERROR);

  }

  CExpression* expr = CExpression::GetExpression(m_algoParamExpressions.at(index), true);

  CObMap::const_iterator it;

  for (it = m_fieldDependOnXYDim.begin() ; it != m_fieldDependOnXYDim.end() ; it++)
  {
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(it->second);
    int32_t lonIndex = static_cast<int32_t>(GetLonDimRange(field));
    int32_t latIndex = static_cast<int32_t>(GetLatDimRange(field));
    
    if (CTools::IsDefaultValue(lonIndex))
    {
      throw CAlgorithmException(CTools::Format("Parameter %d is not valid: field '%s' has no longitude dimension (dimensions: '%s').",
                                               index, 
                                               field->GetName().c_str(), 
                                               field->GetDimAsString().c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    if (CTools::IsDefaultValue(latIndex))
    {
      throw CAlgorithmException(CTools::Format("Parameter %d is not valid: field '%s' has no latitude dimension (dimensions: '%s').",
                                               index, 
                                               field->GetName().c_str(), 
                                               field->GetDimAsString().c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    if (m_varDimLonIndex < 0)
    {
      m_varDimLonIndex = lonIndex;
    }
    else if (lonIndex != m_varDimLonIndex)
    {
      throw CAlgorithmException(CTools::Format("Parameter %d is not valid: fields within the expression '%s' have not the same longitude range (found %d previously and %d for field '%s').",
                                               index, 
                                               expr->AsString().c_str(),
                                               m_varDimLonIndex,
                                               lonIndex,
                                               field->GetName().c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

    if (m_varDimLatIndex < 0)
    {
      m_varDimLatIndex = latIndex;
    }
    else if (latIndex != m_varDimLatIndex)
    {
      throw CAlgorithmException(CTools::Format("Parameter %d is not valid: fields within the expression '%s' have not the same latitude range (found %d previously and %d for field '%s').",
                                               index, 
                                               expr->AsString().c_str(),
                                               m_varDimLatIndex,
                                               latIndex,
                                               field->GetName().c_str()),
                                this->GetName(), BRATHL_LOGIC_ERROR);
    }

  }


 

}

//----------------------------------------
void CBratAlgorithmGeosVelGrid::Dump(ostream& fOut /*= cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgorithmGeosVelGrid Object at "<< this << endl;
  CBratAlgorithmGeosVel::Dump(fOut);
  fOut << "m_varValue: " << m_varValue << endl;
  fOut << "m_varValueW: " << m_varValueW << endl;
  fOut << "m_varValueE: " << m_varValueE << endl;
  fOut << "m_varValueN: " << m_varValueN << endl;
  fOut << "m_varValueS: " << m_varValueS << endl;
  fOut << "m_equatorLimit: " << m_equatorLimit << endl;
  fOut << "m_allLongitudes: " << m_allLongitudes << endl;
  fOut << "m_lonMin: " << m_lonMin << endl;
  fOut << "m_lonMax: " << m_lonMax << endl;
  fOut << "m_indexLon: " << m_indexLon << endl;
  fOut << "m_indexLat: " << m_indexLat << endl;
  fOut << "m_lonMax: " << m_lonMax << endl;
  fOut << "m_varDimLatIndex: " << m_varDimLatIndex << endl;
  fOut << "m_varDimLonIndex: " << m_varDimLonIndex << endl;
  fOut << "m_longitudes: " << endl;
  m_longitudes.Dump(fOut);
  fOut << "m_latitudes: " << endl;
  m_latitudes.Dump(fOut);

  if (m_fieldLon == NULL)
  {
    fOut << "m_fieldLon: NULL " << endl;
  }
  else
  {
    fOut << "m_fieldLon: " << endl;
    m_fieldLon->Dump(fOut);
  }
  if (m_fieldLat == NULL)
  {
    fOut << "m_fieldLat: NULL " << endl;
  }
  else
  {
    fOut << "m_fieldLat: " << endl;
    m_fieldLat->Dump(fOut);
  }

  fOut << "==> END Dump a CBratAlgorithmGeosVelGrid Object at "<< this << endl;

}


//-------------------------------------------------------------
//------------------- CBratAlgorithmGeosVelGridU class --------------------
//-------------------------------------------------------------


CBratAlgorithmGeosVelGridU::CBratAlgorithmGeosVelGridU()
{
  Init();

}
//----------------------------------------
CBratAlgorithmGeosVelGridU::CBratAlgorithmGeosVelGridU(const CBratAlgorithmGeosVelGridU &copy)
{
  Init();

  Set(copy);

}
//----------------------------------------
CBratAlgorithmGeosVelGridU::~CBratAlgorithmGeosVelGridU()
{

}

//----------------------------------------
void CBratAlgorithmGeosVelGridU::Init()
{

}

//----------------------------------------
double CBratAlgorithmGeosVelGridU::ComputeVelocity() 
{
  //Dump(*CTrace::GetDumpContext());

  CTools::SetDefaultValue(m_velocity);

  bool bOk = PrepareComputeVelocity();

  if (!bOk)
  {
    return m_velocity;
  }



//--------------------------
// Compute distance :
//
//             a20=0., b20
//	  		          X
//		  	          |
//			            |
//   		            |
// a01,b01=0. X --- V --- X a02,b02=0.
//			            |
//			            |
//			            |
//			            X
//		         a10=0., b10
//--------------------------


  double a20;
  double b20;

  this->EtoB(m_lon,
		     m_lat,
		     m_lon,
		     m_latNext,
		     a20,
		     b20);

  double a10;
  double b10;

	this->EtoB(m_lon,
		     m_lat,
		     m_lon,
		     m_latPrev,
		     a10,
		     b10);
     

  //------------------
  // Zonal velocity (U)
  //------------------

  double distance = 1000 * ((CTools::Sqrt((a20 * a20) + (b20 * b20)))
                            + (CTools::Sqrt((a10 * a10) + (b10 * b10))));
  
  m_velocity = ((m_varValueN - m_varValueS) * m_coriolis) / distance; 
  
#if _DEBUG_BRAT_ALGO    
  CTrace::Tracer(3, "Algorithm '%s' - Compute Zonal Velocity (U): distance=%f, velocity=%f", this->GetName().c_str(),
             distance, m_velocity);
#endif

  return m_velocity;
}

//----------------------------------------
void CBratAlgorithmGeosVelGridU::Dump(ostream& fOut /*= cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgorithmGeosVelGridU Object at "<< this << endl;
  CBratAlgorithmGeosVelGrid::Dump(fOut);
  fOut << "==> END Dump a CBratAlgorithmGeosVelGridU Object at "<< this << endl;

}
//-------------------------------------------------------------
//------------------- CBratAlgorithmGeosVelGridV class --------------------
//-------------------------------------------------------------


CBratAlgorithmGeosVelGridV::CBratAlgorithmGeosVelGridV()
{
  Init();

}
//----------------------------------------
CBratAlgorithmGeosVelGridV::CBratAlgorithmGeosVelGridV(const CBratAlgorithmGeosVelGridV &copy)
{
  Init();

  Set(copy);

}
//----------------------------------------
CBratAlgorithmGeosVelGridV::~CBratAlgorithmGeosVelGridV()
{

}

//----------------------------------------
void CBratAlgorithmGeosVelGridV::Init()
{

}

//----------------------------------------
double CBratAlgorithmGeosVelGridV::ComputeVelocity() 
{
  //Dump(*CTrace::GetDumpContext());

  CTools::SetDefaultValue(m_velocity);

  bool bOk = PrepareComputeVelocity();

  if (!bOk)
  {
    return m_velocity;
  }



//--------------------------
// Compute distance :
//
//             a20=0., b20
//	  		          X
//		  	          |
//			            |
//   		            |
// a01,b01=0. X --- V --- X a02,b02=0.
//			            |
//			            |
//			            |
//			            X
//		         a10=0., b10
//--------------------------

  double a02;
  double b02;

  this->EtoB(m_lon,
		     m_lat,
		     m_lonNext,
		     m_lat,
		     a02,
		     b02);

  double a01;
  double b01;

	this->EtoB(m_lon,
		     m_lat,
		     m_lonPrev,
		     m_lat,
		     a01,
		     b01);

  //------------------
  // Meridian velocity (V)
  //------------------

  double distance = -1000 * ((CTools::Sqrt((a02 * a02) + (b02 * b02)))
                            + (CTools::Sqrt((a01 * a01) + (b01 * b01))));

  m_velocity = ((m_varValueW - m_varValueE) * m_coriolis) / distance; 
    
#if _DEBUG_BRAT_ALGO    
  CTrace::Tracer(3, "Algorithm '%s' - Compute Meridian Velocity (V): distance=%f, velocity=%f", this->GetName().c_str(),
             distance, m_velocity);
#endif

  return m_velocity;
}
//----------------------------------------

void CBratAlgorithmGeosVelGridV::Dump(ostream& fOut /*= cerr*/)
{
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

  fOut << "==> Dump a CBratAlgorithmGeosVelGridV Object at "<< this << endl;
  CBratAlgorithmGeosVelGrid::Dump(fOut);
  fOut << "==> END Dump a CBratAlgorithmGeosVelGridV Object at "<< this << endl;

}

} // end namespace
