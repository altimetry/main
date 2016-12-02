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

#include <algorithm>
#include <sstream>

#include "brathl.h" 


#include "common/tools/TraceLog.h"
#include "common/tools/Exception.h"

#include "Product.h" 
#include "ProductNetCdf.h" 
#include "Expression.h" 
#include "BratAlgorithmBase.h" 
#include "BratAlgorithmGeosVelAtp.h" 
#include "BratAlgorithmGeosVelGrid.h" 
#include "BratAlgoFilterMedian1D.h" 
#include "BratAlgoFilterMedian2D.h" 
#include "BratAlgoFilterGaussian1D.h" 
#include "BratAlgoFilterGaussian2D.h" 
#include "BratAlgoFilterLanczos1D.h" 
#include "BratAlgoFilterLanczos2D.h" 
#include "BratAlgoFilterLoess1D.h" 
#include "BratAlgoFilterLoess2D.h" 

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;



namespace brathl
{

//-------------------------------------------------------------
//------------------- CBratAlgorithmBase class --------------------
//-------------------------------------------------------------
bool CBratAlgorithmBase::m_algorithmsRegistered = false;

CBratAlgorithmBase::CBratAlgorithmBase()
{
  Init();

}
//----------------------------------------
CBratAlgorithmBase::CBratAlgorithmBase(const CBratAlgorithmBase &o)
{
  Init();

  Set(o);

}
//----------------------------------------
CBratAlgorithmBase::~CBratAlgorithmBase()
{
  DeleteFieldNetCdf();

  DeleteProduct();
  DeleteExpressionValuesArray();

  m_algoParamExpressions.RemoveAll();
  m_listFieldsToRead.RemoveAll();


}

//----------------------------------------
void CBratAlgorithmBase::RegisterCAlgorithms()
{
  if (CBratAlgorithmBase::m_algorithmsRegistered)
  {
    return;
  }

  AUTO_REGISTER_BASE(CBratAlgorithmGeosVelAtp);
  AUTO_REGISTER_BASE(CBratAlgorithmGeosVelGridU);
  AUTO_REGISTER_BASE(CBratAlgorithmGeosVelGridV);
  AUTO_REGISTER_BASE(CBratAlgoFilterMedian1D);
  AUTO_REGISTER_BASE(CBratAlgoFilterMedian2D);
  AUTO_REGISTER_BASE(CBratAlgoFilterGaussian1D);
  AUTO_REGISTER_BASE(CBratAlgoFilterGaussian2D);
  AUTO_REGISTER_BASE(CBratAlgoFilterLanczos1D);
  AUTO_REGISTER_BASE(CBratAlgoFilterLanczos2D);
  AUTO_REGISTER_BASE(CBratAlgoFilterLoess1D);
  AUTO_REGISTER_BASE(CBratAlgoFilterLoess2D);

#ifdef BRAT_ALGO_EXAMPLE
  AUTO_REGISTER_BASE(CBratAlgoExample1);
#endif


  CBratAlgorithmBase::m_algorithmsRegistered = true;
}
//----------------------------------------
void CBratAlgorithmBase::Init()
{
  m_product = NULL;
  m_callerProduct = NULL;
  m_expressionValuesArray = NULL;

  m_fieldDependOnXDim.SetDelete(false);
  m_fieldDependOnYDim.SetDelete(false);
  m_fieldDependOnXYDim.SetDelete(false);

  m_fieldVars.SetDelete(false);
  m_fieldVarsCaller.SetDelete(false);

  SetBeginOfFile();

}
//----------------------------------------
void CBratAlgorithmBase::InitComplexExpressionArray()
{
  uint32_t nbParams = GetNumInputParam();
  m_isComplexExpression.resize(nbParams);
  m_isComplexExpressionWithAlgo.resize(nbParams);

  for(uint32_t i = 0 ; i < nbParams ; i++)
  {
    m_isComplexExpression[i] = true;
    m_isComplexExpressionWithAlgo[i] = true;
  }
}
//----------------------------------------
CProductNetCdf* CBratAlgorithmBase::GetProductNetCdf(CProduct* product)
{
  CProductNetCdf* productNetCdf = CProductNetCdf::GetProductNetCdf(product, false);

  if (productNetCdf == NULL)
  {
    throw CAlgorithmException(CTools::Format("Product file '%s' (%s) is not a Netcdf file product - '%s' algorithm can't be applied only on 'gridded' Netcdf files.", 
		product->GetCurrentFileName().c_str(), product->GetProductClassAndType().c_str(), this->GetName().c_str()), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  return productNetCdf;

}
//----------------------------------------
void CBratAlgorithmBase::SetBeginOfFile()
{
  m_indexRecordToRead = -1;
  m_nProductRecords = -1;
  m_callerProductRecordPrev = -1;

  m_field2DAsRef = NULL;
  
  m_varValueArray = NULL;


}

//----------------------------------------
void CBratAlgorithmBase::SetEndOfFile()
{
}
//----------------------------------------
void CBratAlgorithmBase::SetField2DAsRef()
{
  m_field2DAsRef = this->GetField2DAsRef();
  if (m_field2DAsRef == NULL)
  {
    throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because of an internal error: ERROR in CBratAlgorithmBase::SetField2DAsRef: reference 2D field is NULL and has not been set. This function shouldn't happened. There is something wrong in the Brat software."
                                              "\nPlease contact Brat Helpdesk", 
                                              this->GetName().c_str()),
                              this->GetName(), BRATHL_LOGIC_ERROR);

  }  
}
//----------------------------------------
CFieldNetCdf* CBratAlgorithmBase::GetField2DAsRef()
{
  size_t sizeXYFields = m_fieldDependOnXYDim.size();
  if ((long)sizeXYFields <= 0)
  {
    throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because of an internal error: ERROR in CBratAlgorithmBase::SetField2DAsRef: reference 2D fields map is empty. This function shouldn't happened. There is something wrong in the Brat software."
                                              "\nPlease contact Brat Helpdesk", 
                                              this->GetName().c_str()),
                              this->GetName(), BRATHL_LOGIC_ERROR);

  }

  return CExternalFiles::GetFieldNetCdf(m_fieldDependOnXYDim.begin()->second, false);
}
//----------------------------------------
void CBratAlgorithmBase::SetProduct(CProduct* product, bool forceReplace /* = false */)
{
  if (forceReplace)
  {
    DeleteProduct();
  }
  if (m_product == NULL)
  {
    if (product != NULL)
    {
      m_product = product->Clone();
      //CProductNetCdf* productNetCdf = CProductNetCdf::GetProductNetCdf(product, false);
      //if (productNetCdf != NULL)
      //{
      //  productNetCdf->CloseOnly();
      //}
      //else
      //{
      //}

      m_product->Close();
      m_product->SetDisableTrace(true);
      m_callerProduct = product;
    }
  }
}
//----------------------------------------
void CBratAlgorithmBase::DeleteProduct()
{
  if (m_product != NULL)
  {
    delete m_product;
    m_product = NULL;
  }
}

//----------------------------------------
void CBratAlgorithmBase::SetAlgoParamExpressions(const CObArray& obArray)
{
  m_algoParamExpressions.RemoveAll();

  CObArray::const_iterator it;

  for (it = obArray.begin() ; it != obArray.end() ; it++)
  {
    m_algoParamExpressions.Insert((*it)->Clone());
  }
}

//----------------------------------------
void CBratAlgorithmBase::DeleteExpressionValuesArray()
{
  if (m_expressionValuesArray != NULL)
  {
    delete m_expressionValuesArray;
    m_expressionValuesArray = NULL;
  }

}
//----------------------------------------
void CBratAlgorithmBase::NewExpressionValuesArray()
{
  DeleteExpressionValuesArray();

  m_expressionValuesArray = new CObArray();

}
//----------------------------------------
void CBratAlgorithmBase::Set(const CBratAlgorithmBase &o)
{
  SetProduct(o.m_product);
  SetAlgoParamExpressions(o.m_algoParamExpressions);

  m_algoExpression = o.m_algoExpression;
  m_listFieldsToRead = o.m_listFieldsToRead;

  m_callerProductRecordPrev = o.m_callerProductRecordPrev;

  m_expectedTypes.RemoveAll();
  m_expectedTypes.Insert(o.m_expectedTypes);

  m_varValueArray = NULL;



}

//----------------------------------------
void CBratAlgorithmBase::SetAlgoExpression(const std::string& value)
{
  m_algoExpression = value;
}
//----------------------------------------
void CBratAlgorithmBase::SetAlgoParamExpressions(const CStringArray& values)
{
  m_listFieldsToRead.RemoveAll();
  m_algoParamExpressions.RemoveAll();

  CStringArray::const_iterator it;

  for (it = values.begin() ; it != values.end() ; it++)
  {
    CExpression* expression = new CExpression(*it);
    m_listFieldsToRead.InsertUnique(expression->GetFieldNames());
    
    m_algoParamExpressions.Insert(expression);
  }

}

////----------------------------------------
//std::string CBratAlgorithmBase::GetParamFormats()
//{
//  std::string paramFormats;
//  
//  uint32_t nParameters = GetNumInputParam();
//
//  for (uint32_t i =  0 ; i < nParameters ; i++) 
//  {
//    paramFormats.append(GetInputParamFormat(i));
//  }
//
//  return paramFormats;    
//}

//----------------------------------------
CBratAlgorithmBase& CBratAlgorithmBase::operator=(const CBratAlgorithmBase &o)
{
  if (this == &o)
  {
    return *this;
  }

  Set(o);

  return *this;
}

//----------------------------------------
void CBratAlgorithmBase::CheckInputParams(CVectorBratAlgorithmParam& args) 
{
  //CTools::VaListToTypeUnion( fmt, args, typeUnions);
  if (args.size() != GetNumInputParam())
  {
    throw CAlgorithmException(CTools::Format("Error while running algorithm: number of input parameters (%ld) are not the expected number (%ld)", (long)args.size(), (long)GetNumInputParam()), this->GetName(), BRATHL_LOGIC_ERROR);
  }
}
//----------------------------------------
void CBratAlgorithmBase::CheckInputTypeParams(uint32_t index, CBratAlgorithmParam::bratAlgoParamTypeVal expectedType, CVectorBratAlgorithmParam& args) 
{
  if (index >= args.size())
  {
    throw CAlgorithmException(CTools::Format("Error while checking input parameters: parameter index (%ld) is out-of-range. Valid range is [0, %ld]", (long)index, (long)args.size()), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  CBratAlgorithmParam& algoParam = args.at(index);
  CBratAlgorithmParam::bratAlgoParamTypeVal type = algoParam.GetTypeVal();
  if (type != expectedType)
  {
    std::string msg = CTools::Format("'%s' algorithm can't be applied because of the following reason:"
                                "\n parameter %ld (%s): expected type is %ld (%s) but input type is %ld (%s))",
                                this->GetName().c_str(),
                                (long)index, 
                                this->GetParamName(index).c_str(),
                                (long)expectedType, 
                                CBratAlgorithmParam::TypeValAsString(expectedType).c_str(), 
                                (long)type, 
                                CBratAlgorithmParam::TypeValAsString(type).c_str());

    if (algoParam.GetTypeVal() == CBratAlgorithmParam::T_VECTOR_DOUBLE)
    {
      msg.append("\nIt seems the input data are an array of values and not a single value. "
                  "Perhaps, you are trying to reduce the two-dimensionals data to only one of its dimensions. " 
                  "This use case is not implemented."); 
    }

    throw CAlgorithmException(msg,
                               this->GetName(), BRATHL_LOGIC_ERROR);
  }
}
//----------------------------------------
void CBratAlgorithmBase::CheckInputTypeParams(uint32_t index, const CIntArray& expectedTypes, CVectorBratAlgorithmParam& args) 
{
  bool bOk = false;

  if (index >= args.size())
  {
    throw CAlgorithmException(CTools::Format("Error while checking input parameters: parameter index (%ld) is out-of-range. Valid range is [0, %ld]", (long)index, (long)args.size()), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  CBratAlgorithmParam& algoParam = args.at(index);
  CBratAlgorithmParam::bratAlgoParamTypeVal type = algoParam.GetTypeVal();
  CIntArray::const_iterator it;

  for (it = expectedTypes.begin() ; it != expectedTypes.end() ; it++)
  {
    bOk = (static_cast<int32_t>(type) == *it);
    if (bOk)
    {
      break;
    }
  }
  if (!bOk) 
  {
    throw CAlgorithmException(CTools::Format("Error while running algorithm: parameter %ld : possible types are %s but input type is %ld)", (long)index, expectedTypes.ToString().c_str(), (long)type), this->GetName(), BRATHL_LOGIC_ERROR);
  }
}

//----------------------------------------
double CBratAlgorithmBase::GetDataValue(uint32_t indexExpr)
{
  return GetDataValue(indexExpr, 0);
}

//----------------------------------------
double CBratAlgorithmBase::GetDataValue(uint32_t indexExpr, uint32_t x)
{
  if (m_expressionValuesArray == NULL)
  {
    throw CAlgorithmException("Error while getting data: data array is NULL", this->GetName(), BRATHL_LOGIC_ERROR);
  }

  uint32_t dataArraySize = static_cast<uint32_t>(m_expressionValuesArray->size());

  if (m_expressionValuesArray->size() != this->GetNumInputParam())
  {
    throw CAlgorithmException(CTools::Format("Error while getting data: data array size (%d) is different from number of algorithm input parameters (%d)", dataArraySize, this->GetNumInputParam()), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  if (indexExpr >= this->GetNumInputParam())
  {
    throw CAlgorithmException(CTools::Format("Error while getting data: data array index (%ld) is out-of-range. Valid range is [0, %ld]", (long)indexExpr, (long)this->GetNumInputParam()), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  CExpressionValues* exprValues = CExpressionValues::GetExpressionValues(m_expressionValuesArray->at(indexExpr));

  return exprValues->GetExpressionValueAsDouble(x);

}

//----------------------------------------
double CBratAlgorithmBase::GetDataValue(uint32_t indexExpr, uint32_t x, uint32_t y)
{
  if (m_expressionValuesArray == NULL)
  {
    throw CAlgorithmException("Error while getting data: data array is NULL", this->GetName(), BRATHL_LOGIC_ERROR);
  }

  uint32_t dataArraySize = static_cast<uint32_t>(m_expressionValuesArray->size());

  if (m_expressionValuesArray->size() != this->GetNumInputParam())
  {
    throw CAlgorithmException(CTools::Format("Error while getting data: data array size (%d) is different from number of algorithm input parameters (%d)", dataArraySize, this->GetNumInputParam()), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  if (indexExpr >= this->GetNumInputParam())
  {
    throw CAlgorithmException(CTools::Format("Error while getting data: data array index (%ld) is out-of-range. Valid range is [0, %ld]", (long)indexExpr, (long)this->GetNumInputParam()), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  CExpressionValues* exprValues = CExpressionValues::GetExpressionValues(m_expressionValuesArray->at(indexExpr));

  return exprValues->GetExpressionValueAsDouble(x, y);

}

//----------------------------------------
CObArray* CBratAlgorithmBase::GetDataExpressionValues(uint32_t indexExpr)
{
  if (m_expressionValuesArray == NULL)
  {
    throw CAlgorithmException("Error while getting data: data array is NULL", this->GetName(), BRATHL_LOGIC_ERROR);
  }

  uint32_t dataArraySize = static_cast<uint32_t>(m_expressionValuesArray->size());

  if (m_expressionValuesArray->size() != this->GetNumInputParam())
  {
    throw CAlgorithmException(CTools::Format("Error while getting data: data array size (%d) is different from number of algorithm input parameters (%d)", dataArraySize, this->GetNumInputParam()), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  if (indexExpr >= this->GetNumInputParam())
  {
    throw CAlgorithmException(CTools::Format("Error while getting data: data array index (%ld) is out-of-range. Valid range is [0, %ld]", (long)indexExpr, (long)this->GetNumInputParam()), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  CExpressionValues* exprValues = CExpressionValues::GetExpressionValues(m_expressionValuesArray->at(indexExpr));

  return exprValues->GetExpressionDataValues();

}
//----------------------------------------
void CBratAlgorithmBase::GetExpressionDataValuesAsArrayOfSingleValue(uint32_t index, double*& values, uint32_t& nbValues)
{
  if (m_expressionValuesArray == NULL)
  {
    throw CAlgorithmException("Error while getting data: data array is NULL", this->GetName(), BRATHL_LOGIC_ERROR);
  }

  uint32_t dataArraySize = static_cast<uint32_t>(m_expressionValuesArray->size());

  if (m_expressionValuesArray->size() != this->GetNumInputParam())
  {
    throw CAlgorithmException(CTools::Format("Error while getting data: data array size (%d) is different from number of algorithm input parameters (%d)", dataArraySize, this->GetNumInputParam()), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  if (index >= this->GetNumInputParam())
  {
    throw CAlgorithmException(CTools::Format("Error while getting data: data array index (%ld) is out-of-range. Valid range is [0, %ld]", (long)index, (long)this->GetNumInputParam()), this->GetName(), BRATHL_LOGIC_ERROR);
  }

  CExpressionValues* exprValues = CExpressionValues::GetExpressionValues(m_expressionValuesArray->at(index));

  return exprValues->GetExpressionDataValuesAsArrayOfSingleValue(index, values, nbValues);

}

//----------------------------------------
void CBratAlgorithmBase::GetData1D(int32_t iRecord)
{
  if (m_callerProduct == NULL)
  {
    throw CAlgorithmException("Error while running algorithm: Reading data needs the caller product and no caller product has been set.", this->GetName(), BRATHL_LOGIC_ERROR);
  }

  if (iRecord < 0)
  {
    throw CAlgorithmException("Error while running algorithm: record index of the caller product is negative.", this->GetName(), BRATHL_LOGIC_ERROR);
  }

  if (m_indexRecordToRead >= m_nProductRecords)
  {
    SetEndOfFile();
    return;
  }

  OpenProductFile();

  m_indexRecordToRead = this->ReadProductData(iRecord);

  if (m_indexRecordToRead >= m_nProductRecords)
  {
    SetEndOfFile();
  }
  else
  {
    SetNextValues();
  }

}
//----------------------------------------
void CBratAlgorithmBase::GetNextData()
{

  if (m_callerProduct == NULL)
  {
    throw CAlgorithmException("Error while running algorithm: Reading data needs the caller product and no caller product has been set.", this->GetName(), BRATHL_LOGIC_ERROR);
  }

  int32_t iRecord = m_callerProduct->GetCurrentRecordNumber();

  if (iRecord < 0)
  {
    throw CAlgorithmException("Error while running algorithm: record index of the caller product is negative.", this->GetName(), BRATHL_LOGIC_ERROR);
  }

  OpenProductFile();

  if ((m_indexRecordToRead - 1) >= iRecord)
  {
    // Do nothing: data have been already read and save
    return;
  }

  bool readPreviousValues = (   ((m_indexRecordToRead > 0) && (m_indexRecordToRead != iRecord ))
                             || ((m_indexRecordToRead < 0) && (iRecord > 0)) );

  if (readPreviousValues)
  {
    // Needs to read previous data
    this->ReadProductData(iRecord - 1);

    //Save read values to previous values.
    SetPreviousValues(true);
  } 


  if ((m_indexRecordToRead > 0) && (!readPreviousValues))
  {
    //If it's not the first call: save current values to previous values,
    SetPreviousValues(false);
  }

  m_indexRecordToRead = this->ReadProductData(iRecord + 1);

  if (m_indexRecordToRead >= m_nProductRecords)
  {
    SetEndOfFile();
  }
  else
  {
    SetNextValues();
  }

}
//----------------------------------------
void CBratAlgorithmBase::ProcessOpeningProductNetCdf() 
{
  ProcessOpeningProductNetCdf(m_product);
}
//----------------------------------------
void CBratAlgorithmBase::ProcessOpeningProductNetCdf(CProduct* product) 
{
  CProductNetCdf* productNetCdf = CProductNetCdf::GetProductNetCdf(product, false);

  if (productNetCdf == NULL)
  {
    return;
  }

  if (!m_callerProduct->IsOpened())
  {
    throw CAlgorithmException("Error while opening Netcdf product: original product must be opened and it's not.", BRATHL_LOGIC_ERROR);
  }

  // Set the same way to process the product.
  productNetCdf->NetCdfProductInitialization(m_callerProduct);

  const CStringArray* axisDims = productNetCdf->GetAxisDims(); 
  
  CStringArray newAxisDims; 
  if (axisDims != NULL)
  {
    // Add only known fields (exclude pseudo fields whose netcdf id is < 0)
    CStringArray::const_iterator it;
    for (it = axisDims->begin() ; it != axisDims->end() ; it++)
    {
      CField* field = productNetCdf->FindFieldByName(*it, productNetCdf->GetDataSetNameToRead(), false);
      if (field != NULL)
      {
        newAxisDims.Insert(*it);
      }
    }
   
    m_listFieldsToRead.InsertUnique(newAxisDims);
  }
}
//----------------------------------------
void CBratAlgorithmBase::OpenProductFile() 
{
  OpenProductFile(m_product);
}
//----------------------------------------
void CBratAlgorithmBase::OpenProductFile(CProduct* product) 
{
  if (m_callerProduct == NULL)
  {
    throw CAlgorithmException("Error while running algorithm: Reading data needs the caller product and no caller product has been set.", this->GetName(), BRATHL_LOGIC_ERROR);
  }

  m_currentFileName = m_callerProduct->GetCurrentFileName();
  if (m_currentFileName.empty())
  {
    throw CAlgorithmException("Error while running algorithm (read product data): product has no current file name (file name is empty)", this->GetName(), BRATHL_LOGIC_ERROR);
  }

  if (!product->IsOpened(m_currentFileName))
  {
    SetBeginOfFile();
    product->Open(m_currentFileName);
    
    ProcessOpeningProductNetCdf();

    product->SetListFieldToRead(m_listFieldsToRead, false);  

    m_nProductRecords = product->GetNumberOfRecords();
  }
}

//----------------------------------------
uint32_t CBratAlgorithmBase::ReadProductData(int32_t iRecord) 
{
  return ReadProductData(iRecord, m_algoParamExpressions);
}
//----------------------------------------
uint32_t CBratAlgorithmBase::ReadProductData(int32_t iRecord, CExpression* expression) 
{
  CObArrayOb arrayExpressions(false);
  arrayExpressions.Insert(expression);
  return ReadProductData(iRecord, arrayExpressions);
}
//----------------------------------------
uint32_t CBratAlgorithmBase::ReadProductData(int32_t iRecord, const CObArrayOb& arrayExpressions)
{
  return  ReadProductData(m_product, iRecord, arrayExpressions);
}
//----------------------------------------
uint32_t CBratAlgorithmBase::ReadProductData(CProduct* product, int32_t iRecord, const CObArrayOb& arrayExpressions)
{
  NewExpressionValuesArray();

  if (product == NULL)
  {
    throw CAlgorithmException("Error while running algorithm: Reading data needs a product and no product has been set.", this->GetName(), BRATHL_LOGIC_ERROR);
  }

  if (m_callerProduct == NULL)
  {
    throw CAlgorithmException("Error while running algorithm: Reading data needs the caller product and no caller product has been set.", this->GetName(), BRATHL_LOGIC_ERROR);
  }

  OpenProductFile(product);
  
  if (iRecord >= m_nProductRecords)
  {
    CTrace::Tracer(4, "Algorithm '%s' - Read data record %d of %d - No reading - End of file '%s' is reached", this->GetName().c_str(), iRecord+1, m_nProductRecords, m_currentFileName.c_str());
    return iRecord;
  }

  CTrace::Tracer(4, "Algorithm '%s' - Read data record %d of %d of file '%s'", this->GetName().c_str(), iRecord+1, m_nProductRecords, m_currentFileName.c_str());

  product->ReadBratRecord(iRecord);

  CRecordSet* recordSet = NULL;
  CObArray::iterator itDataSet;

  CDataSet* dataSet = product->GetDataSet();

  CObArray::const_iterator itParamsExpr;
  for (itParamsExpr = arrayExpressions.begin() ; itParamsExpr != arrayExpressions.end() ; itParamsExpr++)
  {
    CExpression* expr = CExpression::GetExpression(*itParamsExpr, false);

    if (expr == NULL)
    {
      throw CAlgorithmException("Error while running algorithm (read product data): parameter expression is NULL or not a CExpression object", this->GetName(), BRATHL_LOGIC_ERROR);
    }
#if _DEBUG_BRAT_ALGO    
    CTrace::Tracer(CTools::Format("Algorithm '%s' - Execute Expression : %s", this->GetName().c_str(), expr->AsString().c_str()));
#endif
    CObArray* expressionDataValues = new CObArray();
    CExpressionValues* expressionValues = new CExpressionValues(expressionDataValues);

    for (itDataSet = dataSet->begin(); itDataSet != dataSet->end() ; itDataSet++)
    {
      CExpressionValue* exprValue = new CExpressionValue();
      recordSet = dataSet->GetRecordSet(itDataSet);
      
      recordSet->ExecuteExpression(*expr, product->GetDataSetNameToRead(), *exprValue, m_product);
      
      expressionDataValues->Insert(exprValue);
    }

    m_expressionValuesArray->Insert(expressionValues);
  }

  return iRecord;
}

//----------------------------------------
void CBratAlgorithmBase::GetAllData(CExpression* expression, CDoubleArray& data)
{
  if (m_callerProduct == NULL)
  {
    throw CAlgorithmException("Error while running algorithm (GetAllData): Reading data needs the caller product and no caller product has been set.", this->GetName(), BRATHL_LOGIC_ERROR);
  }

  OpenProductFile();

  for (int32_t iRecord = 0 ; iRecord < m_nProductRecords ; iRecord++)
  {
    this->ReadProductData(iRecord, expression);
    data.Insert(this->GetDataValue(0));
  }

}
//----------------------------------------
void CBratAlgorithmBase::SetPreviousValues(bool fromProduct) 
{
    UNUSED(fromProduct);
}

//----------------------------------------
void CBratAlgorithmBase::SetNextValues() 
{
}
//----------------------------------------
void CBratAlgorithmBase::CheckConstantParam(uint32_t indexParam)
{
  uint32_t size = static_cast<uint32_t>(m_algoParamExpressions.size());

  if (indexParam >= size)
  {
    throw CAlgorithmException(CTools::Format("Error while checking '%s' parameter: index (%ld) is out-of-range. Valid range is [0, %ld]", 
                                             this->GetParamName(indexParam).c_str(),
                                             (long)indexParam,
                                             (long)size), 
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

  CExpression* expr = CExpression::GetExpression(m_algoParamExpressions.at(indexParam), true);

  bool invalid = expr->HasFieldNames();
  if (invalid)
  {
    throw CAlgorithmException(CTools::Format("'%s' parameter (index %ld) must be a constant value and must not refer to data fields. Parameter expression is: '%s'",
                                             this->GetParamName(indexParam).c_str(),
                                             (long)indexParam,
                                             expr->AsString().c_str()),
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

}
//----------------------------------------
void CBratAlgorithmBase::PrepareDataValues2DOneField(CExpressionValue& exprValue, uint32_t algoExprIndex)
{
  CProductNetCdf* productNetCdf = CBratAlgorithmBase::GetProductNetCdf(m_product);

  CExpression* expr = CExpression::GetExpression(m_algoParamExpressions.at(algoExprIndex), true);
  const CStringArray* fieldNames = expr->GetFieldNames();

  size_t nbFields = fieldNames->size();
  if (nbFields != 1)
  {
    throw CAlgorithmException(CTools::Format("'%s' algorithm can't be applied because of an internal error: ERROR in CBratAlgorithmBase::PrepareDataValues2DOneField: number of fields is: %d and the expected number is 1."
                                             " This function shouldn't be called here. There is something wrong in the Brat software."
                                             "\nPlease contact Brat Helpdesk", 
                                             this->GetName().c_str(), nbFields),
                              this->GetName(), BRATHL_LOGIC_ERROR);

  }


  // Read data
  std::string fieldName = fieldNames->at(0);
  CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(m_fieldVars.Exists(fieldName));
 
  field->SetAtBeginning(false);
    
  productNetCdf->GetExternalFile()->GetValues(fieldName, exprValue, CUnit::m_UNIT_SI);
    
}



//----------------------------------------
void CBratAlgorithmBase::PrepareDataValues2DComplexExpressionWithAlgo(CExpressionValue& exprValue, uint32_t algoExprIndex)
{
  NewExpressionValuesArray();

  CObArray::const_iterator itParamsExpr;
  for (itParamsExpr = m_algoParamExpressions.begin() ; itParamsExpr != m_algoParamExpressions.end() ; itParamsExpr++)
  {
    CObArray* expressionDataValues = new CObArray();
    CExpressionValues* expressionValues = new CExpressionValues(expressionDataValues);
    m_expressionValuesArray->Insert(expressionValues);
  }

  SetField2DAsRef();

  int32_t offset = m_field2DAsRef->GetPosFromDimIndexArray();
  int32_t count = m_field2DAsRef->GetCounFromDimCountArray();

  // Because the dimension array (index and count) will be change during reading
  // Save them and restore them after data reading
  uint32_t dimIndexArraySaved[2];
  dimIndexArraySaved[0] = m_field2DAsRef->GetDimsIndexArray()[0];
  dimIndexArraySaved[1] = m_field2DAsRef->GetDimsIndexArray()[1];
  
  uint32_t dimCountArraySaved[2];
  dimCountArraySaved[0] = m_field2DAsRef->GetDimsCountArray()[0];
  dimCountArraySaved[1] = m_field2DAsRef->GetDimsCountArray()[1];
  
  int32_t iRecord = 0;
  for (int32_t i = 0 ; i < count ; i++)
  //for (int32_t iRecord = nRecords - 1 ; iRecord >= 0 ; iRecord--)
  {
    iRecord = i + offset;
    m_product->ReadBratRecord(iRecord);

    CObArray::iterator itDataSet;
    CRecordSet* recordSet = NULL;

    CDataSet* dataSet = m_product->GetDataSet();
    std::string recordName = m_product->GetDataSetNameToRead();

      //dataSet->Dump(*CTrace::GetDumpContext());

    for (itDataSet = dataSet->begin(); itDataSet != dataSet->end() ; itDataSet++)
    {
      recordSet = dataSet->GetRecordSet(itDataSet);
      for (uint32_t indexExpr = 0 ; indexExpr < m_algoParamExpressions.size() ; indexExpr++)
      {
         CExpression* expr = CExpression::GetExpression(m_algoParamExpressions.at(indexExpr));
        if (expr == NULL)
        {
          continue;
        }

        //CExpressionValues::GetExpressionValues(m_expressionValuesArray->at(indexExpr));
        CObArray* expressionDataValues = this->GetDataExpressionValues(indexExpr);
        
        CExpressionValue* exprValueTmp = new CExpressionValue();
        expressionDataValues->Insert(exprValueTmp);

        recordSet->ExecuteExpression(*expr, recordName, *exprValueTmp, m_product);

      }
    }
  }

  // Restore the dimension array (index and count)
  m_field2DAsRef->GetDimsIndexArray()[0] = dimIndexArraySaved[0];
  m_field2DAsRef->GetDimsIndexArray()[1] = dimIndexArraySaved[1];
  
  m_field2DAsRef->GetDimsCountArray()[0] = dimCountArraySaved[0];
  m_field2DAsRef->GetDimsCountArray()[1] = dimCountArraySaved[1];

  double* dataValue = NULL;
  uint32_t nbValues = 0;

  this->GetExpressionDataValuesAsArrayOfSingleValue(algoExprIndex, dataValue, nbValues);

  exprValue.SetNewValue(dataValue, nbValues);

  if (dataValue != NULL)
  {
    delete []dataValue;
    dataValue = NULL;
    nbValues = 0;
  }

}
//----------------------------------------
void CBratAlgorithmBase::PrepareDataValues2DComplexExpression(CExpressionValue& exprValue, uint32_t algoExprIndex)
{
  NewExpressionValuesArray();
  CObMap expressionValuesName;

  CProductNetCdf* productNetCdf = CBratAlgorithmBase::GetProductNetCdf(m_product);

  CObMap::const_iterator itMap;

  // Read data
  for (itMap = m_fieldVars.begin(); itMap != m_fieldVars.end() ; itMap++)
  { 
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(itMap->second);

    field->SetAtBeginning(false);
    
    std::string fieldName = field->GetName();
    //CTrace::Tracer(fieldName);

    CExpressionValue* values = new CExpressionValue();

    productNetCdf->GetExternalFile()->GetValues(fieldName, *values, CUnit::m_UNIT_SI);

    // Store the data values
    expressionValuesName.Insert(fieldName, values);    
  }

  SetField2DAsRef();

  uint32_t commonDimX = m_field2DAsRef->GetDimsCountArray()[0];
  uint32_t commonDimY = m_field2DAsRef->GetDimsCountArray()[1];

  // Compute expressions and store computed values
  CObArray::const_iterator itParamsExpr;
  for (itParamsExpr = m_algoParamExpressions.begin() ; itParamsExpr != m_algoParamExpressions.end() ; itParamsExpr++)
  {
    CObArray* expressionDataValues = new CObArray();
    CExpressionValues* expressionValues = new CExpressionValues(expressionDataValues);


    CExpression* expr = CExpression::GetExpression(*itParamsExpr, false);
    //CTrace::Tracer(expr->AsString());

    if (expr == NULL)
    {
      throw CAlgorithmException("Error while running algorithm (read product data): parameter expression is NULL or not a CExpression object", this->GetName(), BRATHL_LOGIC_ERROR);
    }

    const CStringArray* fieldNames = expr->GetFieldNames();
    
    //fieldNames->Dump(*CTrace::GetDumpContext());

    CStringArray::const_iterator itFieldName;

    for (uint32_t x = 0 ; x < commonDimX ; x++)
    {
      for (uint32_t y = 0 ; y < commonDimY ; y++)
      {
        // Set the values for each fields in the expression
        for (itFieldName = fieldNames->begin(); itFieldName != fieldNames->end() ; itFieldName++)
        {
          //CTrace::Tracer(*itFieldName);

          CExpressionValue* exprValueTmp = dynamic_cast<CExpressionValue*>(expressionValuesName.Exists(*itFieldName));
          if (exprValueTmp == NULL)
          {
            continue;
          }

          double value;
          setDefaultValue(value);

          if (CExternalFiles::GetFieldNetCdf(m_fieldDependOnXYDim.Exists(*itFieldName), false) != NULL)
          {
            value = exprValueTmp->GetValue(x, y);
          }
          else if (CExternalFiles::GetFieldNetCdf(m_fieldDependOnXDim.Exists(*itFieldName), false) != NULL)
          {
            value = exprValueTmp->GetValue(x);
          }
          else if (CExternalFiles::GetFieldNetCdf(m_fieldDependOnYDim.Exists(*itFieldName), false) != NULL)
          {
            value = exprValueTmp->GetValue(y);
          }

          expr->SetValue(*itFieldName, value);

        }
        // All values are filled for the expression, now execute the expression
        CExpressionValue* exprValue = new CExpressionValue(expr->Execute(m_product));
        //exprValue->Dump(*CTrace::GetDumpContext());

        expressionDataValues->Insert(exprValue);    
      }
    }

    // Set explicit 2D dimensions
    expressionValues->SetDimensions(commonDimX, commonDimY);

    m_expressionValuesArray->Insert(expressionValues);

  }

  double* dataValue = NULL;
  uint32_t nbValues = 0;

  this->GetExpressionDataValuesAsArrayOfSingleValue(algoExprIndex, dataValue, nbValues);

  exprValue.SetNewValue(dataValue, nbValues);

  if (dataValue != NULL)
  {
    delete []dataValue;
    dataValue = NULL;
    nbValues = 0;
  }



}
//----------------------------------------
void CBratAlgorithmBase::GetParamDefValue(uint32_t indexParam, double& value)
{
  value = this->GetParamDefaultValue(indexParam);
}
//----------------------------------------
void CBratAlgorithmBase::GetParamDefValue( uint32_t indexParam, float& value )
{
	double v = GetParamDefaultValue( indexParam );
	if ( isDefaultValue( v ) )
	{
		setDefaultValue( value );
		return;
	}

	value = static_cast<float>( v );
}
//----------------------------------------
void CBratAlgorithmBase::GetParamDefValue( uint32_t indexParam, uint32_t& value )
{
	double v = GetParamDefaultValue( indexParam );
	if ( isDefaultValue( v ) )
	{
		setDefaultValue( value );
		return;
	}

	value = static_cast<uint32_t>( v );
}
//----------------------------------------
void CBratAlgorithmBase::GetParamDefValue( uint32_t indexParam, uint64_t& value )
{
	double v = GetParamDefaultValue( indexParam );
	if ( isDefaultValue( v ) )
	{
		setDefaultValue( value );
		return;
	}

	value = static_cast<uint64_t>( v );
}
//----------------------------------------
void CBratAlgorithmBase::GetParamDefValue( uint32_t indexParam, int32_t& value )
{
	double v = GetParamDefaultValue( indexParam );
	if ( isDefaultValue( v ) )
	{
		setDefaultValue( value );
		return;
	}

	value = static_cast<int32_t>( v );
}
//----------------------------------------
void CBratAlgorithmBase::GetParamDefValue( uint32_t indexParam, int64_t& value )
{
	double v = GetParamDefaultValue( indexParam );
	if ( isDefaultValue( v ) )
	{
		setDefaultValue( value );
		return;
	}

	value = static_cast<int64_t>( v );
}


//----------------------------------------
std::string CBratAlgorithmBase::GetParamDefValueAsString( uint32_t indexParam )
{
	std::string value;
	double defaultValue = this->GetParamDefaultValue( indexParam );
	if ( !isDefaultValue( defaultValue ) )
	{
		value = CTools::TrailingZeroesTrim( CTools::DoubleToStr( defaultValue ), true );
	}

	return value;
}

//----------------------------------------
std::string CBratAlgorithmBase::GetParamDefValueAsLabel(uint32_t indexParam)
{
  std::string label;
  std::string value = this->GetParamDefValueAsString(indexParam);
  if (value.empty())
  {
    return label;
  }

  label.append("Default value is: '");
  label.append(value);
  label.append("'.");

  return label;
}

//----------------------------------------
std::string CBratAlgorithmBase::GetInputParamDescWithDefValueLabel(uint32_t indexParam)
{
  std::string desc = this->GetInputParamDesc(indexParam);
  std::string label = this->GetParamDefValueAsLabel(indexParam);
  if (label.empty())
  {
    return desc;
  }
  
  desc.append(label);

  return desc;
}
//----------------------------------------
//double CBratAlgorithmBase::Exec(const char* function, const char *fmt, const type_union *arg)
//{
//  va_list out;
//
//  const int32_t STATIC_ARG_TBL_SIZE = 100;
//  double aligned[STATIC_ARG_TBL_SIZE]; // aligned buffer of ridiculously large size
//  memset (aligned, 0, STATIC_ARG_TBL_SIZE);
//
//
//  va_start_assign(out, aligned[0]); 
//
//  int i;
//  for(i = 0; arg[i].t != T_END; i++)
//  {
//    switch(arg[i].t) 
//    {
//    case T_INT:
//      va_assign(out, int, arg[i].i);
//      break;
//    case T_FLOAT:
//      va_assign(out, VA_FLOAT, arg[i].f);
//      break;
//    case T_DOUBLE:
//      va_assign(out, double, arg[i].d);
//      break;
//    case T_CHAR:
//      va_assign(out, VA_CHAR, arg[i].c);
//      break;
//    case T_STRING:
//      va_assign(out, const char *, arg[i].s);
//      break;
//    case T_DOUBLE_PTR:
//      va_assign(out, double *, arg[i].d_ptr);
//      break;
//    default:
//      throw CException(CTools::Format("unrecognized format code '%d' in the parameter format of CBratAlgorithmBase::Exec method", arg[i].t), BRATHL_LOGIC_ERROR);
//    }
//  }
//
//  va_end(out);
//
//  va_start_assign(out, aligned[0]);
//  double returnedValue;
//  CTools::SetDefaultValue(returnedValue);
//
//  CBratAlgorithmBaseRegistry& algoRegistry(CBratAlgorithmBaseRegistry::GetInstance());
//  for(CBratAlgorithmBaseRegistry::iterator it = algoRegistry.Begin(); it != algoRegistry.End(); ++it)
//  {
//    base_creator func = *it;
//    CBratAlgorithmBase* algo = func();
//    //std::auto_ptr<CBratAlgorithmBase> ptr(algo);
//    //std::cout << "running object name: " << ptr->GetName() <<  std::endl;
//    //std::cout << "running object name: " << _ptr->GetName() <<  std::endl;
//    std::string nameToSearch = function;
//
//    if ( nameToSearch.compare(algo->GetName()) == 0)
//    {
//      //ptr->do_something();
//      returnedValue = algo->Run(fmt, out);
//      //returnedValue = ptr->Run();
//      std::cout << "Run returned: " << returnedValue <<  std::endl;
//    }
//
//    delete algo;
//    algo = NULL;
//  }
//  //returnedValue = CTools::Exec(function, fmt, out);
//  va_end(out);
//
//  return returnedValue;
//}

//----------------------------------------
double CBratAlgorithmBase::ExecInternal(CBratAlgorithmBase* algo, CVectorBratAlgorithmParam& arg)
{
  if (algo == NULL)
  {
    throw CAlgorithmException("ERROR - CBratAlgorithmBase::ExecInternal - alogrithm object is NULL", BRATHL_LOGIC_ERROR);  
  }

  //va_list out;

  //const int32_t STATIC_ARG_TBL_SIZE = 100;
  //double aligned[STATIC_ARG_TBL_SIZE]; // aligned buffer of ridiculously large size
  //memset (aligned, 0, STATIC_ARG_TBL_SIZE);

  //va_start_assign(out, aligned[0]); 

  //int i;
  //for(i = 0; arg[i].t != T_END; i++)
  //{
  //  switch(arg[i].t) 
  //  {
  //  case T_INT:
  //    va_assign(out, int32_t, arg[i].i);
  //    break;
  //  case T_FLOAT:
  //    va_assign(out, VA_FLOAT, arg[i].f);
  //    break;
  //  case T_DOUBLE:
  //    va_assign(out, double, arg[i].d);
  //    break;
  //  case T_CHAR:
  //    va_assign(out, VA_CHAR, arg[i].c);
  //    break;
  //  case T_STRING:
  //    va_assign(out, const char *, arg[i].s);
  //    break;
  //  case T_DOUBLE_PTR:
  //    va_assign(out, double*, arg[i].d_ptr);
  //    break;
  //  default:
  //    throw CAlgorithmException(CTools::Format("unrecognized format code '%d' in the parameter format of CTools::Exec method", arg[i].t), algo->GetName(), BRATHL_LOGIC_ERROR);
  //  }
  //}

  //va_end(out);

  //va_start_assign(out, aligned[0]);
  //
  double returnedValue = algo->Run(arg);

  //va_end(out);

  return returnedValue;
}
//----------------------------------------
CBratAlgorithmBase* CBratAlgorithmBase::GetNew(const char* algorithName)
{
  CBratAlgorithmBase* algorithm = NULL;

  CBratAlgorithmBaseRegistry& registry(CBratAlgorithmBaseRegistry::GetInstance());

  for(CBratAlgorithmBaseRegistry::iterator it = registry.Begin(); it != registry.End(); ++it)
  {
    base_creator &func = **it;					//v4 "func" as reference: must be polymorphic...
    algorithm = func();
    std::string nameToSearch = algorithName;

    if (nameToSearch.compare(algorithm->GetName()) == 0)
    {
      break;
    }

    delete algorithm;
    algorithm = NULL;
  }

  return algorithm;
}

//----------------------------------------
std::string CBratAlgorithmBase::GetSyntax() const
{
  std::string syntax;
  const std::string paramSeparator = ", ";

  syntax.append(FonctionsAlgoN[0].Name);
  syntax.append("(");
  syntax.append("\"");
  syntax.append(this->GetName());
  syntax.append("\"");

  uint32_t nbParams = this->GetNumInputParam();

  if (nbParams > 0) 
  {
    syntax.append(paramSeparator);
  }


  for (uint32_t i = 0 ; i < nbParams ; i++)
  {
    std::string value = this->GetParamName(i);
    double defaultValue = this->GetParamDefaultValue(i);
    if (!isDefaultValue(defaultValue))
    {
      value = CTools::TrailingZeroesTrim(CTools::DoubleToStr(defaultValue), true);
    }
    else if (value.empty())
    {
      value = "param";
      value.append(CTools::IntToStr(i + 1));
    }
    syntax.append(value);
    syntax.append(paramSeparator);
  }

  if (nbParams > 0) 
  {
    syntax.erase(syntax.size() - paramSeparator.length());
  }

  syntax.append(")");

  return syntax;
}
////----------------------------------------
void CBratAlgorithmBase::DeleteFieldNetCdf()
{
  m_fieldVars.RemoveAll();
  m_fieldVarsCaller.RemoveAll();
  //m_fieldVarsCache.RemoveAll();

}
//----------------------------------------
void CBratAlgorithmBase::CheckComplexExpression(uint32_t index)
{
  if ((m_isComplexExpression.size() <= 0) || (m_isComplexExpressionWithAlgo.size() <= 0))
  {
      InitComplexExpressionArray();
  }
  CExpression* expr = CExpression::GetExpression(m_algoParamExpressions.at(index), true);

  const CUIntArray* code = expr->GetCode();

  m_isComplexExpression[index] =  m_isComplexExpression.at(index) & (code->size() > 1);

  const CVectorBratAlgorithm* algoArray = expr->GetAlgorithms();
  m_isComplexExpressionWithAlgo[index] =  m_isComplexExpressionWithAlgo.at(index) & (algoArray->size() > 0);
}
//----------------------------------------
void CBratAlgorithmBase::CheckVarExpression2D(uint32_t index)
{
  m_fieldDependOnXDim.RemoveAll();
  m_fieldDependOnYDim.RemoveAll();
  m_fieldDependOnXYDim.RemoveAll();

  CObMap oneDimFields(false);

  // Check and store field from the caller (to be able to get current dimension indexes)
  CProductNetCdf* productNetCdfCaller = CBratAlgorithmBase::GetProductNetCdf(m_callerProduct);
  CProductNetCdf* productNetCdf = CBratAlgorithmBase::GetProductNetCdf(m_product);


  CExpression* expr = CExpression::GetExpression(m_algoParamExpressions.at(index), true);
  const CStringArray* fields = expr->GetFieldNames();

  int32_t size = static_cast<int32_t>(fields->size());
  
  if (size <= 0)
  {
    throw CAlgorithmException(CTools::Format("Parameter %d is not valid: there is no field in the expression '%s'.",
                                              index,
                                              expr->AsString().c_str()),
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

  CStringArray::const_iterator it;
  
  int32_t maxDimsSize = -1;

  for (it = fields->begin() ; it != fields->end() ; it++)
  {
    CFieldNetCdf* fieldCaller = productNetCdfCaller->GetExternalFile()->GetFieldNetCdf(*it, true);
    
    const CStringArray& dims = fieldCaller->GetDimNames();
    
    int32_t dimsSize = static_cast<int32_t>(dims.size());
    maxDimsSize = (maxDimsSize < dimsSize) ? dimsSize : maxDimsSize;

    //fieldCaller->Dump(*CTrace::GetDumpContext());


    m_fieldVarsCaller.Insert(*it, fieldCaller, false);

    CFieldNetCdf* field = productNetCdf->GetExternalFile()->GetFieldNetCdf(*it, true);
    m_fieldVars.Insert(*it, field, false);

    if (dimsSize == 2)
    {    
      m_fieldDependOnXYDim.Insert(*it, field, false);
    }

    if (dimsSize == 1)
    {    
      oneDimFields.Insert(*it, field, false);
    }

  }

  if (maxDimsSize != 2)
  {    
    CObMap::const_iterator itMap;
    std::string str;
    for (itMap = m_fieldVarsCaller.begin() ; itMap != m_fieldVarsCaller.end() ; itMap++)
    {
      CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(itMap->second); 
      str.append(CTools::Format("field '%s' has dimensions: '%s'\n", 
                                 field->GetName().c_str(), 
                                 field->GetDimAsString().c_str()));
    }
    std::string msg = CTools::Format("'%s' algorithm can't be applied because there is no input parameter which is a two-dimensional field "
                                "or there is at least one parameter whose dimension is greater than 2.\n%s",
                      this->GetName().c_str(), str.c_str());
    
    throw CAlgorithmException(msg,
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

  // Check the 2-dim fields have the same dimension names
  std::string xDimName;
  std::string yDimName;

  CFieldNetCdf* fieldCompare = NULL;

  CObMap::const_iterator itMap;

  for (itMap = m_fieldDependOnXYDim.begin() ; itMap != m_fieldDependOnXYDim.end() ; itMap++)
  {
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(itMap->second);
    if (fieldCompare == NULL)
    {
      fieldCompare = field;
      xDimName = fieldCompare->GetDimNames().at(0);
      yDimName = fieldCompare->GetDimNames().at(1);
      continue;
    }

    bool sameDims = (field->GetDimNames() == fieldCompare->GetDimNames());
    if (!sameDims)
    {
      std::string str;
      str.append(CTools::Format("field '%s' has dimensions names: '%s'\n", 
                                   field->GetName().c_str(), 
                                   field->GetDimNames().ToString().c_str()));
      str.append(CTools::Format("field '%s' has dimensions names: '%s'\n", 
                                   fieldCompare->GetName().c_str(), 
                                   fieldCompare->GetDimNames().ToString().c_str()));
      std::string msg = CTools::Format("'%s' algorithm can't be applied because at least two 2-dimensions fields have not the same dimensions names.\n%s",
                        this->GetName().c_str(), str.c_str());
    
      throw CAlgorithmException(msg,
                                this->GetName(), BRATHL_LOGIC_ERROR);
      
    }
  }

  // Find 1-dim field dimension name
  for (itMap = oneDimFields.begin() ; itMap != oneDimFields.end() ; itMap++)
  {
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(itMap->second);
    this->AddXOrYFieldDependency(field, xDimName, yDimName);
    //if (field->GetDimNames().at(0).compare(xDimName) == 0)
    //{
    //  m_fieldDependOnXDim.Insert(itMap->first, field, false);
    //}
    //else if (field->GetDimNames().at(0).compare(yDimName) == 0)
    //{
    //  m_fieldDependOnYDim.Insert(itMap->first, field, false);
    //}
    //else
    //{
    //  std::string str;
    //  str.append(CTools::Format("field '%s' has dimension name: '%s'\n", 
    //                               field->GetName().c_str(), 
    //                               field->GetDimNames().ToString().c_str()));
    //  str.append(CTools::Format("expected dimension is one of the following: '%s', '%s'\n", 
    //                               xDimName.c_str(),
    //                               yDimName.c_str()));
    //  std::string msg = CTools::Format("'%s' algorithm can't be applied because at least a 1-dimension field has no common dimension with th 2-dimension fields.\n%s",
    //                    this->GetName().c_str(), str.c_str());
    //
    //  throw CAlgorithmException(msg,
    //                            this->GetName(), BRATHL_LOGIC_ERROR);
    //  

    //}
  }
}

//----------------------------------------
void CBratAlgorithmBase::AddXOrYFieldDependency(CFieldNetCdf* field, CFieldNetCdf* field2DAsRef)
{
  if (field2DAsRef == NULL)
  {
    return;
  }

  std::string fieldName = field2DAsRef->GetName();

  const CStringArray& dims = field2DAsRef->GetDimNames();
    
  int32_t dimsSize = static_cast<int32_t>(dims.size());


  if (dimsSize != 2)
  {    
    std::string msg = CTools::Format("'%s' algorithm can't be applied because in the CBratAlgorithmBase::AddXOrYFieldDependency the input 2D field (as ref.) have to be a 2-dimensions field "
                                "and field '%s' has %d dimensions: '%s'\n",
                                this->GetName().c_str(),
                                fieldName.c_str(), 
                                dimsSize, 
                                field2DAsRef->GetDimAsString().c_str());
    
    throw CAlgorithmException(msg,
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }

  std::string xDimName = field2DAsRef->GetDimNames().at(0);
  std::string yDimName = field2DAsRef->GetDimNames().at(1);

  this->AddXOrYFieldDependency(field, xDimName, yDimName);

}
//----------------------------------------
void CBratAlgorithmBase::AddXOrYFieldDependency(CFieldNetCdf* field, const std::string& xDimName, const std::string& yDimName)
{
  if (field == NULL)
  {
    return;
  }
  
  std::string fieldName = field->GetName();

  const CStringArray& dims = field->GetDimNames();
    
  int32_t dimsSize = static_cast<int32_t>(dims.size());

  if (dimsSize != 1)
  {    
    std::string msg = CTools::Format("'%s' algorithm can't be applied because in the CBratAlgorithmBase::AddXOrYFieldDependency the input field have to be a 1-dimension field "
                                "and field '%s' has %d dimensions: '%s'\n",
                                this->GetName().c_str(),
                                fieldName.c_str(), 
                                dimsSize, 
                                field->GetDimAsString().c_str());
    
    throw CAlgorithmException(msg,
                              this->GetName(), BRATHL_LOGIC_ERROR);
  }


  if (field->GetDimNames().at(0).compare(xDimName) == 0)
  {
    m_fieldDependOnXDim.Insert(fieldName, field, false);
  }
  else if (field->GetDimNames().at(0).compare(yDimName) == 0)
  {
    m_fieldDependOnYDim.Insert(fieldName, field, false);
  }
  else
  {
    std::string str;
    str.append(CTools::Format("field '%s' has dimension name: '%s'\n", 
                                  fieldName.c_str(), 
                                  field->GetDimNames().ToString().c_str()));
    str.append(CTools::Format("expected dimension is one of the following: '%s', '%s'\n", 
                                  xDimName.c_str(),
                                  yDimName.c_str()));
    std::string msg = CTools::Format("'%s' algorithm can't be applied because at least a 1-dimension field has no common dimension with th 2-dimension fields.\n%s",
                      this->GetName().c_str(), str.c_str());
    
    throw CAlgorithmException(msg,
                              this->GetName(), BRATHL_LOGIC_ERROR);
      

  }

}
//----------------------------------------
void CBratAlgorithmBase::Dump(std::ostream& fOut /*= std::cerr*/)
{
  if (! CTrace::IsTrace())
  {
    return;
  }

  fOut << "==> Dump a CBratAlgorithmBase Object at "<< this << std::endl;
  fOut << "GetName() returns: " << this->GetName() << std::endl;
  fOut << "GetDescription() returns: " << this->GetDescription() << std::endl;
  uint32_t nInputParams = this->GetNumInputParam();
  fOut << "GetNumInputParam() returns: " << nInputParams << std::endl;
  uint32_t i;
  for (i = 0 ; i < nInputParams ; i++)
  {
    fOut << "GetInputParamDesc(" << i <<") returns: " << this->GetInputParamDesc(i) << std::endl;

  }
  for (i = 0 ; i < nInputParams ; i++)
  {
    fOut << "GetInputParamFormat(" << i <<") returns: " << this->GetInputParamFormat(i) << std::endl;

  }
  for (i = 0 ; i < nInputParams ; i++)
  {
    fOut << "GetInputParamUnit(" << i <<") returns: " << this->GetInputParamUnit(i) << std::endl;

  }
  fOut << "GetOutputUnit() returns: " << this->GetOutputUnit() << std::endl;
  fOut << "m_algoExpression: " << this->m_algoExpression << std::endl;
  fOut << "m_expressionValuesArray: " << std::endl;
  if (m_expressionValuesArray != NULL)
  {
    m_expressionValuesArray->Dump(fOut);
  }
  else
  {
    fOut << "NULL" << std::endl;
  }

  fOut << "m_currentFileName: " << m_currentFileName << std::endl;
  fOut << "m_indexRecordToRead: " << m_indexRecordToRead << std::endl;
  fOut << "m_nProductRecords: " << m_nProductRecords << std::endl;
  fOut << "m_callerProductRecordPrev: " << m_callerProductRecordPrev << std::endl;

  fOut << "m_fieldVarsCaller: " << std::endl;
  m_fieldVarsCaller.Dump(fOut);
  fOut << "m_fieldVars: " << std::endl;
  m_fieldVars.Dump(fOut);
  //fOut << "m_fieldVarsCache: " << std::endl;
  //m_fieldVarsCache.Dump(fOut);

  fOut << "m_fieldDependOnXYDim: " << std::endl;
  m_fieldDependOnXYDim.Dump(fOut);
  fOut << "m_fieldDependOnXDim: " << std::endl;
  m_fieldDependOnXDim.Dump(fOut);
  fOut << "m_fieldDependOnYDim: " << std::endl;
  m_fieldDependOnYDim.Dump(fOut);
  
  fOut << "m_expectedTypes: " << std::endl;
  m_expectedTypes.Dump(fOut);


  fOut << "==> END Dump a CBratAlgorithmBase Object at "<< this << std::endl;

}

//-------------------------------------------------------------
//------------------- CMapBratAlgorithm class --------------------
//-------------------------------------------------------------

CMapBratAlgorithm::CMapBratAlgorithm(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;
}


//----------------------------------------
CMapBratAlgorithm::~CMapBratAlgorithm()
{
  RemoveAll();
}

//----------------------------------------
CBratAlgorithmBase* CMapBratAlgorithm::Insert(const std::string& key, CBratAlgorithmBase* ob, bool withExcept /* = true */)
{
  

  std::pair <CMapBratAlgorithm::iterator,bool> pairInsert;


  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object
  pairInsert = (mapbratalgorithmbase::insert(CMapBratAlgorithm::value_type(key, ob)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e(CTools::Format("ERROR in CMapBratAlgorithm::Insert - try to insert an algorithm that already exists ('%s'). Check that no algorithm have the same name", key.c_str()), BRATHL_INCONSISTENCY_ERROR);
    Dump(*CTrace::GetDumpContext());

    throw(e);
  }
  
  CMapBratAlgorithm::iterator it = (pairInsert.first);
  return (*it).second;;

}
//----------------------------------------
CBratAlgorithmBase* CMapBratAlgorithm::Find(const std::string& algoName) const
{
  CMapBratAlgorithm::const_iterator it = mapbratalgorithmbase::find(algoName);
  if (it == end())
  {
    return NULL;
  }
  else
  {
    return (*it).second;
  }

}

//----------------------------------------
std::string CMapBratAlgorithm::GetSyntaxAlgo(const std::string& name)
{
  CBratAlgorithmBase* algo = Find(name);
  if (algo == NULL)
  {
    return "";
  }

  return algo->GetSyntax();
}
//----------------------------------------
void CMapBratAlgorithm::RemoveAll()
{
  
  CMapBratAlgorithm::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      CBratAlgorithmBase* ob = it->second;
      if (ob != NULL)
      {
        delete  ob;
      }
    }
  }

  mapbratalgorithmbase::clear();

}
//----------------------------------------
void CMapBratAlgorithm::Dump(std::ostream& fOut /* = std::cerr */)
{

   if (CTrace::IsTrace() == false)
   { 
      return;
   }

   fOut << "==> Dump a CMapBratAlgorithm Object at "<< this << " with " <<  size() << " elements" << std::endl;

   CMapBratAlgorithm::iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      CBratAlgorithmBase *ob = it->second;
      fOut << "CMapBratAlgorithm Key is = " << (*it).first << std::endl;
      fOut << "CMapBratAlgorithm Value is = " << std::endl;
      ob->Dump(fOut);
   }

   fOut << "==> END Dump a CMapBratAlgorithm Object at "<< this << " with " <<  size() << " elements" << std::endl;

}
//-------------------------------------------------------------
//------------------- CVectorBratAlgorithm class --------------------
//-------------------------------------------------------------

CVectorBratAlgorithm::CVectorBratAlgorithm(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;
}


//----------------------------------------
CVectorBratAlgorithm::~CVectorBratAlgorithm()
{
  RemoveAll();
}

//----------------------------------------
void CVectorBratAlgorithm::Init()
{
}
//----------------------------------------
void CVectorBratAlgorithm::Insert(CBratAlgorithmBase* ob, bool bEnd /*= true*/)
{
  if (bEnd)
  {  
    vectorbratalgorithmbase::push_back(ob);   
  }
  else
  {  
    CVectorBratAlgorithm::InsertAt(this->begin(), ob);   
  }

}
//----------------------------------------
CVectorBratAlgorithm::iterator CVectorBratAlgorithm::InsertAt(CVectorBratAlgorithm::iterator where, CBratAlgorithmBase* ob)
{ 
  return vectorbratalgorithmbase::insert(where, ob);     
}


//----------------------------------------
void CVectorBratAlgorithm::RemoveAll()
{
  
  CVectorBratAlgorithm::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      CBratAlgorithmBase* ob = *it;
      if (ob != NULL)
      {
        delete  ob;
      }
    }
  }

  vectorbratalgorithmbase::clear();

}
//----------------------------------------
void CVectorBratAlgorithm::Dump(std::ostream& fOut /* = std::cerr */)
{

 if (CTrace::IsTrace() == false)
  {
    return;
  }

  CVectorBratAlgorithm::const_iterator it;
  int i = 0;

  fOut << "==> Dump a CVectorBratAlgorithm Object at "<< this << " with " <<  size() << " elements" << std::endl;

  for ( it = this->begin( ); it != this->end( ); it++ )
  {
    fOut << "CVectorBratAlgorithm[" << i << "]= " << std::endl;  
    if ((*it) != NULL)
    {
     (*it)->Dump(fOut);
    }
    i++;
  }

  fOut << "==> END Dump a CVectorBratAlgorithm Object at "<< this << " with " <<  size() << " elements" << std::endl;


  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CBratAlgorithmBaseRegistry class --------------------
//-------------------------------------------------------------

CBratAlgorithmBaseRegistry::CBratAlgorithmBaseRegistry()
{
  m_sortedArrayBratAlgo.SetDelete(false);
}
//----------------------------------------
CBratAlgorithmBaseRegistry::~CBratAlgorithmBaseRegistry()
{
}

//----------------------------------------
CBratAlgorithmBaseRegistry& CBratAlgorithmBaseRegistry::GetInstance()
{
    static CBratAlgorithmBaseRegistry instance;
    return instance;
}
//----------------------------------------

void CBratAlgorithmBaseRegistry::Add( base_creator *creator )
{
    CBratAlgorithmBase* algorithm = (*creator)();

    std::string algorithmName = algorithm->GetName();

    m_mapBratAlgo.Insert(algorithmName, algorithm);

    m_bases.push_back( creator );		//add to m_bases only after trying to create the algorithm (which can throw)
}
//----------------------------------------

CBratAlgorithmBaseRegistry::iterator CBratAlgorithmBaseRegistry::Begin()
{
    return m_bases.begin();
}
//----------------------------------------

CBratAlgorithmBaseRegistry::iterator CBratAlgorithmBaseRegistry::End()
{ 
  return m_bases.end(); 
}
//----------------------------------------

CBratAlgorithmBase* CBratAlgorithmBaseRegistry::Find(const std::string& algoName)
{
  return CBratAlgorithmBaseRegistry::GetInstance().m_mapBratAlgo.Find(algoName);
}

//----------------------------------------
CBratAlgorithmBase* CBratAlgorithmBaseRegistry::GetAlgorithm(const std::string& algoName, bool withExcept /*= true*/)
{
  CBratAlgorithmBase* algo =  CBratAlgorithmBaseRegistry::Find(algoName);
  if (withExcept)
  {
    if (algo == NULL)
    {
      CException e(CTools::Format("CBratAlgorithmBaseRegistry::GetAlgorithm - Unknown algorithm name '%s'", algoName.c_str()),
                   BRATHL_INCONSISTENCY_ERROR);
      throw (e);
    }
  }
  
  return algo;

}

//----------------------------------------
bool CBratAlgorithmBaseRegistry::AlgorithmCompare(CBratAlgorithmBase* o1, CBratAlgorithmBase* o2)
{
  return (o1->GetName().compare(o2->GetName()) < 0); 
}

//----------------------------------------
CMapBratAlgorithm* CBratAlgorithmBaseRegistry::GetAlgorithms()
{
  return &(CBratAlgorithmBaseRegistry::GetInstance().m_mapBratAlgo);
}
//----------------------------------------
CVectorBratAlgorithm* CBratAlgorithmBaseRegistry::GetAlgorithmsAsSortedArray(bool forceToFill /*= false */)
{
  if (( CBratAlgorithmBaseRegistry::GetInstance().m_sortedArrayBratAlgo.size() <= 0) || (forceToFill))
  {
    CBratAlgorithmBaseRegistry::GetInstance().FillAlgorithmsSortedArray();
  }

  return &(CBratAlgorithmBaseRegistry::GetInstance().m_sortedArrayBratAlgo);
}
//----------------------------------------
void CBratAlgorithmBaseRegistry::FillAlgorithmsSortedArray()
{

  CMapBratAlgorithm::iterator it;

  m_sortedArrayBratAlgo.RemoveAll();

  for (it = m_mapBratAlgo.begin() ; it != m_mapBratAlgo.end() ; it++)
  {
    CBratAlgorithmBase* ob = it->second;
    if (ob != NULL)
    {
      m_sortedArrayBratAlgo.Insert(ob);
    }
  }
  
  sort(m_sortedArrayBratAlgo.begin(), m_sortedArrayBratAlgo.end(), CBratAlgorithmBaseRegistry::AlgorithmCompare);
}


//-------------------------------------------------------------
//------------------- CBratAlgorithmBaseRegistration class --------------------
//-------------------------------------------------------------

CBratAlgorithmBaseRegistration::CBratAlgorithmBaseRegistration(base_creator *creator)
{
    CBratAlgorithmBaseRegistry::GetInstance().Add(creator);
}

//----------------------------------------

//-------------------------------------------------------------
//------------------- CBratAlgoExample1 class --------------------
//-------------------------------------------------------------

#ifdef BRAT_ALGO_EXAMPLE

void CBratAlgoExample1::SetParamValues(CVectorBratAlgorithmParam& args)
{     
  CheckInputParams(args);

  m_1 = args.at(0).GetValueAsDouble();
  m_2 = args.at(1).GetValueAsInt();
  m_3 = args.at(2).GetValueAsString();
  m_4 = args.at(3).GetValueAsLong();


}

//----------------------------------------
double CBratAlgoExample1::Run(CVectorBratAlgorithmParam& args)
{

  SetParamValues(args);
  
  Dump();
  
  return (static_cast<double>(m_2) + static_cast<double>(m_4)) ;
}
//----------------------------------------

void CBratAlgoExample1::Dump(std::ostream& fOut /* = std::cerr */) 
{
  if (CTrace::IsTrace() == false)
  { 
    return;
  }

  fOut << "==> Dump a CBratAlgoExample1 Object at "<< this << std::endl;

  CBratAlgorithmBase::Dump(fOut);
  
  fOut << "m_1: " << m_1 << std::endl;
  fOut << "m_2: " << m_2 << std::endl;
  fOut << "m_3: " << m_3 << std::endl;
  fOut << "m_4: " << m_4 << std::endl;

  fOut << "==> END Dump a CBratAlgoExample1 Object at "<< this << std::endl;

}

#endif
} // end namespace

