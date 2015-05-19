/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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

#include "Exception.h"
#include "FileParams.h"
#include "InternalFilesYFX.h"

#include "Field.h"
#include "BratProcess.h"
#include "BratProcessYFX.h"

using namespace brathl;
using namespace processes;

namespace processes
{

const KeywordHelp CBratProcessYFX::YFXKeywordList[]	= {
	KeywordHelp(kwFILE,		1, 0),
	KeywordHelp(kwRECORD),
	KeywordHelp(kwOUTPUT),
	KeywordHelp(kwOUTPUT_TITLE,	0, 1, "\"\""),
	KeywordHelp(kwSELECT,		0, 0, "1"),
	KeywordHelp(kwFIELD,		1, CBratProcess::NB_MAX_Y, NULL, 24),
	KeywordHelp(kwFIELD_COMMENT,	0, CBratProcess::NB_MAX_Y, NULL, 24),
	KeywordHelp(kwFIELD_DATA_MODE,	-24),
	KeywordHelp(kwFIELD_NAME,	-24),
	KeywordHelp(kwFIELD_TYPE,	-24),
	KeywordHelp(kwFIELD_UNIT,	-24),
	KeywordHelp(kwFIELD_TITLE,	-24),
	KeywordHelp(kwDATA_MODE,	0, 1, "MEAN"),
	KeywordHelp(kwX),
	KeywordHelp(kwX_NAME),
	KeywordHelp(kwX_TYPE),
	KeywordHelp(kwX_UNIT),
	KeywordHelp(kwX_TITLE),
	KeywordHelp(kwALIAS_NAME,	0, 0, "None", 14),
	KeywordHelp(kwALIAS_VALUE,	-14),
	KeywordHelp(kwUNIT_ATTR_NAME,	0, 0, "None", 14),
	KeywordHelp(kwUNIT_ATTR_VALUE,	-14),
	KeywordHelp(kwVERBOSE,		0, 1, "0"),
 	KeywordHelp(kwLOGFILE,		0, 1, ""),
	KeywordHelp(""),
};

//-------------------------------------------------------------
//------------------- CBratProcessYFX class --------------------
//-------------------------------------------------------------

CBratProcessYFX::CBratProcessYFX()
{
  Init();
}
//----------------------------------------
    
CBratProcessYFX::~CBratProcessYFX()
{

}
//----------------------------------------
void CBratProcessYFX::Init()
{
  //m_nbMaxDataSlices = 1;

}
//----------------------------------------
void CBratProcessYFX::ResizeArrayDependOnFields(uint32_t size)
{
  CBratProcess::ResizeArrayDependOnFields(size);

}

//----------------------------------------
void CBratProcessYFX::AddFieldIndexes(CFieldIndex* fieldIndex, CNetCDFVarDef* varDef)
{
  if (varDef == NULL)
  {
    return;
  }

  if (fieldIndex == NULL)
  {
    return;
  }

  CBratProcess::AddFieldIndexes(fieldIndex, varDef);

  /*
  IncrementOffsetValues();

  m_countOffsets.push_back(-1);
  m_meanOffsets.push_back(-1);

  //-----------------
  m_nbDataAllocated++;
  //-----------------
*/
}
//----------------------------------------
void CBratProcessYFX::GetParameters(const string& commandFileName)
{
  m_commandFileName = commandFileName;
  GetParameters();
}
//----------------------------------------
void CBratProcessYFX::GetParameters()
{
  LoadParams(m_commandFileName);
  CheckFileParams();

  CFileParams& params = *m_fileParams;
  CUnit	unit;

// Verify keyword occurences
  uint32_t nbFields		= params.CheckCount(kwFIELD,  1, CBratProcess::NB_MAX_Y);

  params.CheckCount(kwOUTPUT);
  params.CheckCount(kwRECORD);


// Get keyword values

  CTrace *p = CTrace::GetInstance();

  //---------
  
  params.m_mapParam[kwOUTPUT]->GetValue(m_outputFileName);
  p->Tracer(1, PCT_StrFmt, "Output file", m_outputFileName.c_str());

  m_internalFiles = new CInternalFilesYFX(m_outputFileName.c_str(), Replace);
  m_internalFiles->Open();

  //---------

  if (params.CheckCount(kwOUTPUT_TITLE, 0) == 1)
  {
    params.m_mapParam[kwOUTPUT_TITLE]->GetValue(m_outputTitle);
  }

  p->Tracer(1, PCT_QStrFmt, "Output title", m_outputTitle.c_str());

  //---------
  
  params.m_mapParam[kwRECORD]->GetValue(m_recordName);
  p->Tracer(1, PCT_StrFmt, "Data set name", m_recordName.c_str());

  //---------

  if (params.CheckCount(kwEXPAND_ARRAY, 0, 1) == 1)
  {
    params.m_mapParam[kwEXPAND_ARRAY]->GetValue(m_expandArray);
  }
  p->Tracer(1, PCT_IntFmt, "Expand array", m_expandArray);


  //---------

  GetSelectParameter(params);
   
  //m_select.Dump(*(CTrace::GetInstance()->GetDumpContext()));

  //---------
  m_dataModeGlobal = GetDataMode(params);
  p->Tracer(1, PCT_StrFmt, "Default data mode",  CBratProcess::DataModeStr(m_dataModeGlobal).c_str());

  //---------
  this->GetDefinition(params,
	    kwX,
	    m_xField,
	    &m_xName,
	    &m_xType,
	    &m_xUnit,
	    &m_xTitle,
	    &m_xComment,
	    NULL,
	    NULL,
	    "X data");

  if (m_xType == Data)
  {
    throw CParameterException("DATA type for X is not allowed, choose between X, Y, Z, T, Latitude and Longitude",
			     BRATHL_RANGE_ERROR);
  }

  ResizeArrayDependOnFields(nbFields);

  uint32_t accruedDataSlices = 0;
  uint32_t index;

  for (index = 0; index < nbFields; index++)
  {
    this->GetDefinition(params,
	      kwFIELD,
	      m_fields[index],
	      &m_names[index],
	      &m_types[index],
	      &m_units[index],
	      &m_titles[index],
	      &m_comments[index],
	      NULL,
	      NULL,
	      "Value",
	      index,
	      nbFields);

    m_dataMode[index] = CBratProcess::GetDataMode(params, kwFIELD, 0, nbFields, index, m_dataModeGlobal);

    // The stored values are:
    // X (1 double)
    // Y (nbFields doubles)
    // Counts for mean (1 double for mean if mode is MEAN, 0 otherwize)
    // Counts and Mean for stddev (2 doubles (1 for mean, 1 for stddev) if mode is STDDEV, 0 otherwize)
  
    // Compute the data slices : 
    // Data itself and count for mean (==> 2)
    // Data itself, mean and count for stddev (==> 3)
    // Data only for the others (==> 1)
    int32_t nbDataSlices = CBratProcess::GetMergedDataSlices(m_dataMode[index]);
    /*
    if (nbDataSlices > m_nbMaxDataSlices)
    {
      m_nbMaxDataSlices = nbDataSlices;
    }
    */

    // Computes count and means offsets for mean and sdtDev calculation (-1 if no offset ==> no mean and no stddev)
    m_countOffsets[index] = -1;
    m_meanOffsets[index] = -1;
    switch (nbDataSlices)
    {
      //-------
      case 2:
      //-------
        m_countOffsets[index] = nbFields + accruedDataSlices;
        break;
      //-------
      case 3:
      //-------
        m_countOffsets[index] = nbFields + accruedDataSlices;
        m_meanOffsets[index] = nbFields + accruedDataSlices + 1;
        break;
      //-------
      default:
      //-------
        break;
    }

    accruedDataSlices += (nbDataSlices - 1);
    // add :  nbDataSlices to number of data to allocate.
    m_nbDataAllocated	+= nbDataSlices ;
  }

  if (m_nbDataAllocated != (nbFields + accruedDataSlices))
  {
    throw CException(CTools::Format("ERROR: CBratProcessYFX::GetParameters() - number of data to allocate (%d) != number of fields (%d) + data slices (%d)\n",
                                    m_nbDataAllocated,
                                    nbFields,
                                    accruedDataSlices),
			               BRATHL_LOGIC_ERROR);
  }

  DeleteFileParams();
}
//----------------------------------------
bool CBratProcessYFX::CheckCommandLineOptions(int	argc, char	**argv)
{
  return CBratProcess::CheckCommandLineOptions(argc, argv,
		              "This program creates data files corresponding to Y=F(X) function",
	      		      YFXKeywordList);

}

//----------------------------------------
void CBratProcessYFX::BeforeBuildListFieldsToRead()
{

}
//----------------------------------------
void CBratProcessYFX::AfterBuildListFieldsToRead()
{

  AddComplementDimensionsFromNetCdf();

  m_listFieldsToRead.InsertUnique(m_xField.GetFieldNames());  

}

//----------------------------------------
void CBratProcessYFX::BuildListFieldsToRead()
{
  CBratProcess::BuildListFieldsToRead();
}
//----------------------------------------
void CBratProcessYFX::AddComplementDimensionsFromNetCdf()
{
  // Applies only to Netcdf products
  if (! CBratProcess::IsProductNetCdf() )
  {
   return;
  }
  CProductNetCdf* productNetCdf = GetProductNetCdf(m_product);

  // Get dimension's names of the X field
  m_netCdfAxisDims.RemoveAll();
  productNetCdf->GetNetCdfDimensions(m_xField, m_netCdfAxisDims, m_recordName);

  // Get dimension's names of all the data fields
  CStringArray fieldDims;
  CStringArray fieldDimsWithoutAlgo;
  productNetCdf->GetNetCdfDimensions(m_fields, fieldDims, m_recordName);
  productNetCdf->GetNetCdfDimensionsWithoutAlgo(m_fields, fieldDimsWithoutAlgo, m_recordName);
  
//  m_netCdfAxisDims.Dump(*(CTrace::GetInstance()->GetDumpContext()));
  //fieldDims.Dump(*(CTrace::GetInstance()->GetDumpContext()));
  
  //Remove the dimensions that have no common dimension with the 
  //fieldDims.Remove(m_fieldWithNoXYCommonDim);

  //Gets the dimension's name which are not dimensions of the X field
  m_netCdfComplementDims.RemoveAll();
  m_netCdfAxisDims.Complement(fieldDims, m_netCdfComplementDims);

  //m_netCdfComplementDims.Dump(*(CTrace::GetInstance()->GetDumpContext()));

  // Add dimensions which are not dimensions of the X field
  if (fieldDimsWithoutAlgo.size() > 0)
  {
    AddDimensionsFromNetCdf(m_netCdfComplementDims);
  }

  AddVarsFromNetCdf();

}



//----------------------------------------
int32_t CBratProcessYFX::Execute(string& msg)
{ 
  int32_t result = BRATHL_SUCCESS;

  CDate startExec;
  startExec.SetDateNow();

  if (m_product == NULL)
  {
	  throw CException("PROGRAM ERROR: product is NULL (CBratProcessYFX::Execute) - Perhaps, initialization has not been called",
			   BRATHL_LOGIC_ERROR);
  }

  m_measures.RemoveAll();

  uint32_t nbFiles = m_inputFiles.size();
  uint32_t cptFile = 0;

  CStringArray::iterator itFile;

  for (itFile = m_inputFiles.begin() ; itFile != m_inputFiles.end() ; itFile++)
  {
    cptFile++;


    CTrace::Tracer(1,"File %d/%d - Reading record data from %s ... and registering data ...",
                   cptFile, nbFiles, (*itFile).c_str());


    // open file an set record name and list of field to read
    m_product->SetFieldSpecificUnits(m_fieldSpecificUnit);
    m_product->Open(*itFile, m_recordName);

    NetCdfProductInitialization();
    
    SetExpandArray(m_xField, true);

    m_product->SetListFieldToRead(m_listFieldsToRead, false);

    ReplaceAxisDefinition();

    ReplaceFieldDefinition();

    // Get the number of record for the default record name (set in Open method of CProduct above)
    int32_t nRecords = m_product->GetNumberOfRecords();

    for (int32_t iRecord = 0 ; iRecord < nRecords ; iRecord++)
    {
      //Read fields for the record name  (listof field and record name are set in Open method of CProduct above)
      m_product->ReadBratRecord(iRecord);
      RegisterData();
    }

    m_product->Close();
  }

  if (m_product->GetSkippedRecordCount() > 0)
  {
    CTrace::Tracer(1, CTools::Format("WARNING - %d input data records have been skipped due to inconsistency between two measures",
                                     m_product->GetSkippedRecordCount()));
  }


  result = WriteData();

  CDate endExec;
  endExec.SetDateNow();
  
  CTrace::Tracer(1, CTools::Format("Processing time: %.3f seconds (%.2f minutes)\n", 
                                  (endExec - startExec),
                                  (endExec - startExec) / 60.0));

 
  return result;
}

//----------------------------------------

void CBratProcessYFX::RegisterData()
{

  if (m_product == NULL)
  {
	  throw CException("PROGRAM ERROR: product is NULL (CBratProcessYFX::RegisterData) - Perhaps, initialization has not been called",
			   BRATHL_LOGIC_ERROR);
  }

  //CTrace *p = CTrace::GetInstance();

  //p->Tracer(1,"Registering data ...");

  CDataSet* dataSet = NULL;
  CObArray::iterator itDataSet;

  CRecordSet* recordSet = NULL;

  CExpressionValue exprValue;
  uint32_t	nbValues;

  double xValue;
  
  CTools::SetDefaultValue(xValue);

//  OneMeasure	Measure;

  dataSet = m_product->GetDataSet();

  //---------------------------------
  // Picks up data from the recordset
  //---------------------------------
  for (itDataSet = dataSet->begin(); itDataSet != dataSet->end() ; itDataSet++)
  {
    recordSet = dataSet->GetRecordSet(itDataSet);

    ///bool firstRecord = (record == dataSet->front());

    //---------------------------------
    // Tests condition expression (select expression)
    //---------------------------------
    if (m_alwaysTrue == false)
    {
      recordSet->ExecuteExpression(m_select, m_recordName, exprValue, m_product);
      if (exprValue.IsTrue() != 1)
      {
        //data doesn't match SELECT options,
        //-------------
        continue;
        //-------------
      }

    }


    //---------------------------------
    // Reads X value
    //---------------------------------
    recordSet->ExecuteExpression(m_xField, m_recordName, exprValue, m_product);
    
    nbValues	= exprValue.GetNbValues();

    if (nbValues == 0)
    {
      continue;
    }

    if (nbValues == 1)
    {
      xValue = exprValue.GetValues()[0];
      
      if (CTools::IsDefaultValue(xValue))
      {
        //---------
        continue;
        //---------
      }

      xValue = CBratProcess::CheckLongitudeValue(xValue, -180.0, m_xType);
    }
    else
    {
      throw CException("X value must be scalar not a vector or a matrix",
		       BRATHL_LIMIT_ERROR);
    }
    uint32_t indexExpr = 0;

    //---------------------------------
    // Finds each data (expression) dimensions
    //---------------------------------
    if (m_matrixDims.size() <= 0)
    {
      m_matrixDims.resize(m_nbDataAllocated);

      for (indexExpr = 0 ; indexExpr < m_fields.size() ; indexExpr++)
      {
  
        recordSet->ExecuteExpression(m_fields.at(indexExpr), m_recordName, exprValue, m_product);
      
        nbValues	= exprValue.GetNbValues();
        
        if (nbValues <= 0)
        {
          nbValues = 1;
        }
        m_matrixDims[indexExpr] = nbValues;
        int32_t countOffset = m_countOffsets.at(indexExpr);
        int32_t meanOffset = m_meanOffsets.at(indexExpr);
        
        if (countOffset > 0)
        {
          m_matrixDims[countOffset] = nbValues;
        }

        if (meanOffset > 0)
        {
          m_matrixDims[meanOffset] = nbValues;
        }
      }

    //---------------------------------
    m_measures.SetMatrixDims(m_matrixDims);
    //---------------------------------

    }
  
    //---------------------------------
    // Searches X value into the map
    // if not found, create a new one (add a new element into the map)
    //---------------------------------

    bool foundX = true;

    //CObArrayOb* fieldsArray = GetOneMeasure(xValue, false);
    DoublePtr* fieldsArray = GetOneMeasure(xValue);

    if (fieldsArray == NULL)
    {
      foundX = false;
      try
      {
        fieldsArray = m_measures.Insert(xValue, CBratProcess::MergeIdentifyUnsetData);
      }
      catch (bad_alloc& e) // If memory allocation (new) failed...
      {
        m_measures.RemoveAll(); // free memory  to be able to allocate new for error msg
        throw CMemoryException(CTools::Format("ERROR: CBratProcessYFX::RegisterData() - Unable to allocate new memory to store the result.\nNative error: '%s'\n"
                                           " or try to allocate more than your system architecture can ever handle",
                                      e.what()));
      }
      catch (...) // If memory allocation (new) failed... ??? 
      {
        m_measures.RemoveAll(); // free memory  to be able to allocate new for error msg
        throw CMemoryException("ERROR: CBratProcessYFX::RegisterData() - Unable to allocate new memory to store the résult.\nPerhaps not enough memory available."
                                " or try to allocate more than your system architecture can ever handle"
                                );
      }

      if (fieldsArray == NULL)
      {
        m_measures.RemoveAll(); // free memory  to be able to allocate new for error msg
        throw CMemoryException("ERROR: CBratProcessYFX::RegisterData() - Unable to allocate new memory to store the résult.\nPerhaps not enough memory available."
                                " or try to allocate more than your system architecture can ever handle"
                                );
      }
      


    }
    //---------------------------------
    // Reads data values (expression result)
    //---------------------------------
    for (indexExpr = 0 ; indexExpr < m_fields.size() ; indexExpr++)
    {
  
      recordSet->ExecuteExpression(m_fields.at(indexExpr), m_recordName, exprValue, m_product);
      
      nbValues	= exprValue.GetNbValues();
      double* data = exprValue.GetValues();

      if (data == NULL)
      {
        nbValues = 0;
      }


      int32_t countOffset = m_countOffsets.at(indexExpr);
      int32_t meanOffset = m_meanOffsets.at(indexExpr);

      uint32_t cols = m_measures.GetMatrixColDim(indexExpr);
      
      if (cols <= 0)
      {
        throw CException(CTools::Format("ERROR: CBratProcessYFX::RegisterData() - data values array is empty (size is %d).\n"
                                        "Expression is: '%s'",
                                        cols, 
                                        m_fields.at(indexExpr).AsString().c_str()),
			                   BRATHL_LOGIC_ERROR);

      }

      //CDoubleArrayOb* dataValues = NULL;
      double* dataValues = NULL;
      //CDoubleArrayOb* countValues = NULL;
      double* countValues = NULL;
      //CDoubleArrayOb* meanValues = NULL;
      double* meanValues = NULL;

      if (indexExpr >= m_measures.GetMatrixNumberOfRows())
      {
        throw CException(CTools::Format("ERROR: CBratProcessYFX::RegisterData() - expression index (%d) >= array size (%d)\n"
                                        "Expression is: '%s'",
                                        indexExpr, 
                                        m_measures.GetMatrixNumberOfRows(),
                                        m_fields.at(indexExpr).AsString().c_str()),
			                   BRATHL_LOGIC_ERROR);
      }

      //dataValues = CBratProcess::GetDoubleArrayOb(fieldsArray->at(indexExpr), false);
      dataValues = fieldsArray[indexExpr];
      
      if (dataValues == NULL)
      {
        throw CException(CTools::Format("ERROR: CBratProcessYFX::RegisterData() - data values not found\n. Expression index is %d. Array size is %d\n"
                                        "Expression is: '%s'",
                                        indexExpr, 
                                        cols,
                                        m_fields.at(indexExpr).AsString().c_str()),
			                   BRATHL_LOGIC_ERROR);
      }
      

      if (countOffset > 0)
      {
        //countValues = CBratProcess::GetDoubleArrayOb(fieldsArray->at(countOffset), false);
        countValues = fieldsArray[countOffset];
        if (countValues == NULL)
        {
          throw CException(CTools::Format("ERROR: CBratProcessYFX::RegisterData() - count values array not found\n. Expression index is %d. Offset is %d\n"
                                          "Expression is: '%s'",
                                          indexExpr, 
                                          countOffset,
                                          m_fields.at(indexExpr).AsString().c_str()),
			                     BRATHL_LOGIC_ERROR);
        }
        else //  countValues != NULL)
        {
          uint32_t colsCount = m_measures.GetMatrixColDim(countOffset);
          if (cols != colsCount)
          {
            throw CException(CTools::Format("ERROR: CBratProcessYFX::RegisterData() - data values array size (%d) is not equal to count values array size (%d)\n"
                                            "Expression is: '%s'",
                                            cols, 
                                            colsCount,
                                            m_fields.at(indexExpr).AsString().c_str()),
			                       BRATHL_LOGIC_ERROR);

          }
        }
      }

      if (meanOffset > 0)
      {
        //meanValues = CBratProcess::GetDoubleArrayOb(fieldsArray->at(meanOffset), false);
        meanValues = fieldsArray[meanOffset];
        if (meanValues == NULL)
        {
          throw CException(CTools::Format("ERROR: CBratProcessYFX::RegisterData() - mean values array not found\n. Expression index is %d. Offset is %d\n"
                                          "Expression is: '%s'",
                                          indexExpr, 
                                          meanOffset,
                                          m_fields.at(indexExpr).AsString().c_str()),
			                     BRATHL_LOGIC_ERROR);
        }
        else   // (meanValues != NULL)
        {
          uint32_t colsMean = m_measures.GetMatrixColDim(meanOffset);
          if (cols != colsMean)
          {
            throw CException(CTools::Format("ERROR: CBratProcessYFX::RegisterData() - data values array size (%d) is not equal to  mean values array size (%d)\n"
                                            "Expression is: '%s'",
                                            cols, 
                                            colsMean,
                                            m_fields.at(indexExpr).AsString().c_str()),
			                       BRATHL_LOGIC_ERROR);

          }
        }
      }


      
      if (CBratProcess::IsProductNetCdf())
      {
        CNetCDFCoordinateAxis* coordVar = dynamic_cast<CNetCDFCoordinateAxis*>(m_internalFiles->GetNetCDFVarDef(m_names[indexExpr]));
        if (coordVar != NULL)
        {
          string msg;
          bool areValuesSimilar = CheckValuesSimilar(indexExpr, exprValue.GetValues(), nbValues, msg);
          if (! areValuesSimilar)
          {
            CTrace::Tracer(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
            CTrace::Tracer(msg);
            CTrace::Tracer(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

          }
        }
      }


      MergeDataValue(dataValues, exprValue.GetValues(), nbValues, indexExpr, countValues, meanValues);

    }

  }

  //p->Tracer(1,"End registering data");
}
//----------------------------------------
int32_t CBratProcessYFX::WriteData()
{
  int32_t result = BRATHL_SUCCESS;

  if (m_measures.size() <= 0)
  {
    CTrace::Tracer(1,">>>>>>>>>>>>>>>>>>>> WARNING >>>>>>>>>>>>>>>>>>>>>>>>>");
    CTrace::Tracer(1,"There is no data to write - perhaps because of your selection criteria ('%s')", m_select.AsString().c_str());
    CTrace::Tracer(1,"Output file '%s' have not been created", m_outputFileName.c_str());
    CTrace::Tracer(1,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    m_internalFiles->Close();
    CFile::Delete(m_outputFileName);

    result = BRATHL_WARNING;

    return result;
  }

  CTrace::Tracer(1,"Creating output file '%s'", m_outputFileName.c_str());
  
  if (m_internalFiles == NULL)
  {
    throw CException("ERROR: CBratProcessYFX::WriteData() - Unable to create NetCDF file because m_internalFiles is NULL.\n",
			               BRATHL_LOGIC_ERROR);

  }
  
  //CInternalFilesYFX	internalFilesYFX(m_outputFileName.c_str(), Replace);
  //m_internalFiles->Open();

  m_internalFiles->WriteFileTitle(m_outputTitle);
  
  CTrace::Tracer(1,"Writing data");

  CExpressionValue xValues;
  
  xValues.SetName("xValues");
  xValues.SetNewValue(m_measures);
  
  uint32_t nbXValues = xValues.GetNbValues();
  double* xDataValues = xValues.GetValues();

  // Warning : if data account for a date, they are expressed in numbers of seconds since 1950-01-01 00:00:00.0
  // XData and/or Ydata have to be converted to unit expressed in the file
  //if unit is set as 'xxxx since YYYY-MM-DD HH:MM:SS.MS'.

  CUnit unit = m_xUnit;
  unit.SetConversionFromBaseUnit();

  for (uint32_t iXValue = 0 ; iXValue < nbXValues ; iXValue++)
  {
    xDataValues[iXValue]	= unit.Convert(xDataValues[iXValue]);
  }


  //---------------------------------------------
  // Get X min and X max  (X is sorted)
  //---------------------------------------------
  CTools::SetDefaultValue(m_validMin);
  CTools::SetDefaultValue(m_validMax);

  if (nbXValues > 0)
  {
    m_validMin = xDataValues[0];
    m_validMax = xDataValues[nbXValues - 1];
  }

 

  //---------------------------------------------
  // Create X netcdf dimension and variable
  //---------------------------------------------
  CNetCDFCoordinateAxis* addedXCoordVar = dynamic_cast<CNetCDFCoordinateAxis*>(m_internalFiles->GetNetCDFVarDef(m_xName));
  
  bool addDimToXCoord = false;

  if (addedXCoordVar == NULL)
  {
    CNetCDFCoordinateAxis xCoordVar(m_xName);

    addedXCoordVar = dynamic_cast<CNetCDFCoordinateAxis*>(m_internalFiles->AddNetCDFVarDef(xCoordVar));
    addDimToXCoord = true;
  }

  addedXCoordVar->SetUnit(m_xUnit);
  addedXCoordVar->SetDimKind(m_xType);

  addedXCoordVar->AddAttribute(new CNetCDFAttrString(LONG_NAME_ATTR, m_xTitle));
  addedXCoordVar->AddAttribute(new CNetCDFAttrString(COMMENT_ATTR, m_xComment));
  
  addedXCoordVar->SetValidMinMax(m_validMin, m_validMax);

  CNetCDFDimension* addedXDim = m_internalFiles->GetNetCDFDim(m_xName);

  if (addedXDim == NULL)
  {
    CNetCDFDimension xDim(m_xName, nbXValues);

    addedXDim = m_internalFiles->AddNetCDFDim(xDim);
    
    addedXCoordVar->AddNetCDFDim(*addedXDim);
    addedXDim->AddCoordinateVariable(addedXCoordVar);
  }
  else
  {
    addedXDim->SetLength(nbXValues);
    addedXCoordVar->ReplaceNetCDFDim(*addedXDim);

  }


  //////////////string dimType = xCoordVar.GetDimType();




//  OutputFile.CreateDim(XType, XName, DataWritten, XUnit.GetText(), XTitle, 
//                       XComment, validMin, validMax);



  //-----------------------------
  // Merge data
  //-----------------------------
  CIntMap mapVarIndexExpr;

  // 2 loops for netcdf performance
  for (uint32_t indexExpr = 0; indexExpr < m_fields.size(); indexExpr++)
  {
    CTrace::Tracer(1,CTools::Format("\t==> Merge data of field '%s'", m_names[indexExpr].c_str()));

    CTools::SetDefaultValue(m_validMin);
    CTools::SetDefaultValue(m_validMax);
   
    int32_t countOffset = m_countOffsets.at(indexExpr);
    int32_t meanOffset = m_meanOffsets.at(indexExpr);
    
    uint32_t cols = m_measures.GetMatrixColDim(indexExpr);

    unit = m_units[indexExpr];

    unit.SetConversionFromBaseUnit();

    //VectorValues	= DataWritten.GetValues();
    
    //CObDoubleMap::const_iterator itX;
    CDoublePtrDoubleMap::const_iterator itX;

    for (itX = m_measures.begin(); itX != m_measures.end() ; itX++)
    {
      //CObArrayOb* fieldsArray = CBratProcess::GetObArrayOb(itX->second);
      DoublePtr* fieldsArray = itX->second;
      if (fieldsArray == NULL)
      {
        throw CException(CTools::Format("ERROR: CBratProcessYFX::WriteData() - Array of fields not found (X value is %.15g). Expression index is %d.\n"
                                        "Expression is: '%s'",
                                        itX->first,
                                        indexExpr, 
                                        m_fields.at(indexExpr).AsString().c_str()),
			                   BRATHL_LOGIC_ERROR);

      }

      //CDoubleArrayOb* dataValues = CBratProcess::GetDoubleArrayOb(fieldsArray->at(indexExpr), false);
      double* dataValues = fieldsArray[indexExpr];
        
      if (dataValues == NULL)
      {
        throw CException(CTools::Format("ERROR: CBratProcessYFX::WriteData() - data values not found\n. X value is %.15g. Expression index is %d. Array size is %d\n"
                                        "Expression is: '%s'",
                                        itX->first,
                                        indexExpr, 
                                        m_measures.GetMatrixNumberOfRows(),
                                        m_fields.at(indexExpr).AsString().c_str()),
			                   BRATHL_LOGIC_ERROR);
      }
 

      //CDoubleArrayOb* countValues = NULL;
      double* countValues = NULL;
      //CDoubleArrayOb* meanValues = NULL;
      double* meanValues = NULL;

      if (countOffset > 0)
      {
        //countValues = CBratProcess::GetDoubleArrayOb(fieldsArray->at(countOffset), false);
        countValues = fieldsArray[countOffset];
        if (countValues == NULL)
        {
          throw CException(CTools::Format("ERROR: CBratProcessYFX::WriteData() - count values array not found\n. X value is %.15g. Expression index is %d. Offset is %d\n"
                                          "Expression is: '%s'",
                                          itX->first,
                                          indexExpr, 
                                          countOffset,
                                          m_fields.at(indexExpr).AsString().c_str()),
			                     BRATHL_LOGIC_ERROR);
        }
        else //  countValues != NULL)
        {
          uint32_t colsCount = m_measures.GetMatrixColDim(countOffset);
          if (cols != colsCount)
          {
            throw CException(CTools::Format("ERROR: CBratProcessYFX::WriteData() - data values array size (%d) is not equal to count values array size (%d)\n"
                                            "Expression is: '%s'",
                                            cols, 
                                            colsCount,
                                            m_fields.at(indexExpr).AsString().c_str()),
			                       BRATHL_LOGIC_ERROR);

          }
        }

      }
      if (meanOffset > 0)
      {
        //meanValues = CBratProcess::GetDoubleArrayOb(fieldsArray->at(meanOffset), false);
        meanValues = fieldsArray[meanOffset];
        if (meanValues == NULL)
        {
          throw CException(CTools::Format("ERROR: CBratProcessYFX::WriteData() - mean values array not found\n. X value is %.15g. Expression index is %d. Offset is %d\n"
                                          "Expression is: '%s'",
                                          itX->first,
                                          indexExpr, 
                                          meanOffset,
                                          m_fields.at(indexExpr).AsString().c_str()),
			                     BRATHL_LOGIC_ERROR);
        }
        else   // (meanValues != NULL)
        {
          uint32_t colsMean = m_measures.GetMatrixColDim(meanOffset);
          if (cols != colsMean)
          {
            throw CException(CTools::Format("ERROR: CBratProcessYFX::WriteData() - data values array size (%d) is not equal to  mean values array size (%d)\n"
                                            "Expression is: '%s'",
                                            cols, 
                                            colsMean,
                                            m_fields.at(indexExpr).AsString().c_str()),
			                       BRATHL_LOGIC_ERROR);

          }
        }
      }

	    FinalizeMergingOfDataValues(dataValues,
				                          indexExpr,
                                  cols,
				                          countValues,
                                  meanValues);
      
      //CDoubleArrayOb::iterator itDataValues;
      uint32_t indexValues = 0;

      for (indexValues = 0 ; indexValues < cols; indexValues++)
      {
        //---------------------------------------------
        // converts to asked unit
        //---------------------------------------------
        if (!CTools::IsDefaultValue(dataValues[indexValues]))
        {
          dataValues[indexValues]	= unit.Convert(dataValues[indexValues]);
        }
        //---------------------------------------------
        // compute variable min and max
        //---------------------------------------------
        AdjustValidMinMax(dataValues[indexValues]);
      }
    
    }


    CNetCDFVarDef* addedVarDef = m_internalFiles->GetNetCDFVarDef(m_names[indexExpr]);
    if (addedVarDef == NULL)
    {
      CNetCDFVarDef varDef(m_names[indexExpr], m_units[indexExpr]);

      varDef.AddAttribute(new CNetCDFAttrString(LONG_NAME_ATTR, m_titles[indexExpr]));
      varDef.AddAttribute(new CNetCDFAttrString(COMMENT_ATTR, m_comments[indexExpr]));
      
      addedVarDef = m_internalFiles->AddNetCDFVarDef(varDef);

    }

    addedVarDef->SetValidMinMax(m_validMin, m_validMax);
    
    if (CBratProcess::IsProductNetCdf())
    {
      if ( ( ! addedVarDef->IsCoordinateAxis() ) && ( addedVarDef->HasCommonDims(m_xName) ) )
      {
        CNetCDFDimension* varDim = addedVarDef->GetNetCDFDim(addedXDim->GetName());
        if (varDim == NULL)
        {
          addedVarDef->InsertNetCDFDim(*addedXDim, 0);
        }
        else
        {
          varDim->SetLength(nbXValues);
          addedVarDef->ReplaceNetCDFDim(*varDim);
        }
      }

      //if ( addedVarDef->IsCoordinateAxis() && (addedVarDef->GetName().compare(addedXCoordVar->GetName()) != 0)) 
      //{
      //  CNetCDFDimension* varDim = addedVarDef->GetNetCDFDim(addedVarDef->GetName());
      //  if (varDim != NULL)
      //  {
      //    if (cols != varDim->GetLength())
      //    {
      //      varDim->SetLength(cols);
      //      m_internalFiles->ReplaceNetCDFDim(*varDim); 
      //    }

      //  }

      //}

    }
    else if ( ! addedVarDef->IsCoordinateAxis() )  // not a CPoductNetcdf object
    {
      CNetCDFDimension* varDim = addedVarDef->GetNetCDFDim(addedXDim->GetName());
      if (varDim == NULL)
      {
        addedVarDef->InsertNetCDFDim(*addedXDim, 0);
      }
      else
      {
        varDim->SetLength(nbXValues);
        addedVarDef->ReplaceNetCDFDim(*varDim);
      }
    }
 
    mapVarIndexExpr.Insert(addedVarDef->GetName(), indexExpr, false);

    //OutputFile.CreateData(Names[Index - 1], Units[Index - 1].GetText(), Titles[Index - 1], Comments[Index - 1], validMin, validMax);
  }

  CTrace::Tracer(1,"\t==> Write Netcdf file structure");

  //---------------------------------------------
  // Create netcdf dimensions
   //---------------------------------------------
  m_internalFiles->WriteDimensions();
  //---------------------------------------------
  // Create netcdf variables
   //---------------------------------------------
  m_internalFiles->WriteVariables();
  
  CTrace::Tracer(1,"\t==> Write Netcdf data values");

  //---------------------------------------------
  // Write netcdf dimension's data or variables that don't depend on X
  //---------------------------------------------

  CObMap* mapNetCdfVarDefs = m_internalFiles->GetNetCDFVarDefs();
  CObMap::const_iterator itVarDef;

  for (itVarDef = mapNetCdfVarDefs->begin() ;  itVarDef != mapNetCdfVarDefs->end() ; itVarDef++)
  {
    CNetCDFVarDef* varDef = dynamic_cast<CNetCDFVarDef*>(itVarDef->second);
    
    if (varDef == NULL)
    {
      //-----------
      continue;
      //-----------
    }

    CNetCDFCoordinateAxis* coordAxis = dynamic_cast<CNetCDFCoordinateAxis*>(varDef);
    
    string coordAxisName = ((coordAxis != NULL) ? coordAxis->GetName() : "");

    if (! CBratProcess::IsProductNetCdf() && (coordAxis == NULL) ) 
    {
      //-----------
      continue;
      //-----------
    }

    if ( CBratProcess::IsProductNetCdf() && (coordAxis == NULL) && varDef->HasCommonDims(m_xName) ) 
    {
      //-----------
      continue;
      //-----------
    }

    //if (! (coordAxis->GetAxis().empty()) )
    if ( coordAxisName.compare(m_xName) == 0 )
    {
      m_internalFiles->WriteData(coordAxis, &xValues);
      //-----------
      continue;
      //-----------
    }
    else 
    {
      // Is it a CFieldIndex object ? (only for "product" objects that are not CProductNetCdf object)
      // ==> Write variable data.
      CExpressionValue dataWritten;
      dataWritten.SetName("dataWritten2");

      CFieldIndex* fieldIndex = dynamic_cast<CFieldIndex*>(m_mapFieldIndexesToRead.Exists(varDef->GetName()));
      
      if (fieldIndex != NULL)
      {
        
        CDoubleArray vect;
        fieldIndex->Read(vect);
        dataWritten.SetNewValue(vect);

      }
      else if (CBratProcess::IsProductNetCdf())
      {
        // It's a CProductNetCdf, and variable doesn't depend on X
        // ==> Write variable data.
        
        DoublePtr* fieldsArray = m_measures.begin()->second;
      
        if (fieldsArray == NULL)
        {
          throw CException(CTools::Format("ERROR: CBratProcessYFX::WriteData() - Array of fields not found (X value is %.15g). Coordiante axis name is '%s'.\n",
                                          m_measures.begin()->first,
                                          varDef->GetName().c_str()),
			                     BRATHL_LOGIC_ERROR);

        }

        int32_t indexVar = mapVarIndexExpr.Exists(varDef->GetName());

        if (CTools::IsDefaultValue(indexVar))
        {
          throw CException(CTools::Format("ERROR: CBratProcessYFX::WriteData() - variable index not found\n. Coordinate axis name is '%s'.\n",
                                          varDef->GetName().c_str()),
			                     BRATHL_LOGIC_ERROR);
        }

        double* dataValues = fieldsArray[indexVar];
        if (dataValues == NULL)
        {
          throw CException(CTools::Format("ERROR: CBratProcessYFX::WriteData() - data values not found\n. X value is %.15g. Array size is %d. Variable index is %d. Coordinate axis name is '%s'.\n",
                                          m_measures.begin()->first,
                                          m_measures.GetMatrixNumberOfRows(),
                                          indexVar, 
                                          varDef->GetName().c_str()),
			                     BRATHL_LOGIC_ERROR);
        }
        

        dataWritten.SetNewValue(FloatType, *(varDef->GetDims()), dataValues);

      }

      //----------------------------------
      m_internalFiles->WriteData(varDef, &dataWritten);
      //----------------------------------

    }

  }

  //---------------------------------------------
  // Write netcdf variable's data (which ones have common dim with X)
  //---------------------------------------------


  for (itVarDef = mapNetCdfVarDefs->begin() ;  itVarDef != mapNetCdfVarDefs->end() ; itVarDef++)
  {
    CNetCDFVarDef* varDef = dynamic_cast<CNetCDFVarDef*>(itVarDef->second);
    
    if (varDef == NULL)
    {
      //-----------
      continue;
      //-----------
    }

    CNetCDFCoordinateAxis* coordAxis = dynamic_cast<CNetCDFCoordinateAxis*>(varDef);

    if (coordAxis != NULL)
    {
      //-----------
      continue;
      //-----------
    }

    if ( CBratProcess::IsProductNetCdf() && ( ! varDef->HasCommonDims(m_xName) ) )
    {
      //-----------
      continue;
      //-----------
    }



    int32_t indexVar = mapVarIndexExpr.Exists(varDef->GetName());
    
    CExpressionValue dataWritten;
    string name = "dataWritten1 ";
    name.append(varDef->GetName());
    dataWritten.SetName(name);

    dataWritten.SetNewValue(FloatType, *(varDef->GetDims()), NULL);

    double* data = dataWritten.GetValues();
    long iData = 0;

    //VectorValues	= DataWritten.GetValues();
  
    //CObDoubleMap::const_iterator itX;
    CDoublePtrDoubleMap::const_iterator itX;

    for (itX = m_measures.begin(); itX != m_measures.end() ; itX++)
    {
      //CObArrayOb* fieldsArray = CBratProcess::GetObArrayOb(itX->second);
      DoublePtr* fieldsArray = itX->second;
      
      if (fieldsArray == NULL)
      {
        throw CException(CTools::Format("ERROR: CBratProcessYFX::WriteData() - Array of fields not found (X value is %.15g). Variable name is '%s'.\n",
                                        itX->first,
                                        varDef->GetName().c_str()),
			                   BRATHL_LOGIC_ERROR);

      }


      if (CTools::IsDefaultValue(indexVar))
      {
        throw CException(CTools::Format("ERROR: CBratProcessYFX::WriteData() - variable index not found\n. Variable name is '%s'.\n",
                                        varDef->GetName().c_str()),
			                   BRATHL_LOGIC_ERROR);
      }

      //CDoubleArrayOb* dataValues = CBratProcess::GetDoubleArrayOb(fieldsArray->at(indexVar), false);
      double* dataValues = fieldsArray[indexVar];
        
      if (dataValues == NULL)
      {
        throw CException(CTools::Format("ERROR: CBratProcessYFX::WriteData() - data values not found\n. X value is %.15g. Array size is %d. Variable index is %d. Variable name is '%s'.\n",
                                        itX->first,
                                        m_measures.GetMatrixNumberOfRows(),
                                        indexVar, 
                                        varDef->GetName().c_str()),
			                   BRATHL_LOGIC_ERROR);
      }
        
      //CDoubleArrayOb::const_iterator itDataValues;
      uint32_t cols = m_measures.GetMatrixColDim(indexVar);
      
      uint32_t indexValues = 0;
      
      for (indexValues = 0 ; indexValues < cols; indexValues++)
      {
        data[iData] = dataValues[indexValues];
        //--------
        iData++;
        //--------

      }
    }

    m_internalFiles->WriteData(varDef, &dataWritten);

    //delete []data;
    //data = NULL;

  }

  CTrace::Tracer(1,"Data written");
  CTrace::Tracer(1,"Output file '%s' created", m_outputFileName.c_str());

  m_internalFiles->Close();

  return result;

}

//----------------------------------------
DoublePtr* CBratProcessYFX::GetOneMeasure(double key)
{   
  return m_measures.Exists(key);
}

//----------------------------------------
//CObArrayOb* CBratProcessYFX::GetOneMeasure(double key, bool withExcept /*= true*/)
//{   
//  return CBratProcess::GetObArrayOb(m_measures.Exists(key), withExcept);
//}
//----------------------------------------
void CBratProcessYFX::ResizeDataValues(CDoubleArrayOb* dataValues, uint32_t nbValues)
{   
  if (dataValues == NULL)
  {
    return;
  }

  int32_t size = (nbValues == 0 ? 1 : nbValues);
  dataValues->resize(size, CBratProcess::MergeIdentifyUnsetData);
}
//----------------------------------------
void CBratProcessYFX::SubstituteAxisDim(const CStringArray& fieldDims, CStringArray& fieldDimsOut)
{
    // Replace dimensions which have the same name of the X fields by the X expression name 
  fieldDims.Replace(m_netCdfAxisDims, m_xName, fieldDimsOut, true, true);


}

//----------------------------------------
void CBratProcessYFX::OnAddDimensionsFromNetCdf()
{
  CBratProcess::OnAddDimensionsFromNetCdf();

}


//----------------------------------------
bool CBratProcessYFX::CheckValuesSimilar(uint32_t indexExpr, double* dataValues, uint32_t nbValues, string& msg)
{

  bool bSimilarValues = true;
  

  if (m_matrixDims.size() <= 0)
  {
    return bSimilarValues;
  }

  uint32_t cols = m_measures.GetMatrixColDim(indexExpr);
  if (cols <= 0)
  {
    return bSimilarValues;
  }

  if (m_measures.size() <= 0)
  {
    return bSimilarValues;
  }

  if (cols != nbValues)
  {
    return false;
  }

  DoublePtr* fieldsArray = m_measures.begin()->second;
  double* dataValuesRef = fieldsArray[indexExpr];

  uint32_t indexValues = 0;

  for (indexValues = 0 ; indexValues < cols; indexValues++)
  {
    if (dataValuesRef[indexValues] == CBratProcess::MergeIdentifyUnsetData)
    {
      break;
    }
    if ( ! CTools::AreEqual(dataValuesRef[indexValues], dataValues[indexValues]) )
    {
      bSimilarValues = false;
      break;
    }
  }


  string format = "WARNING - Regarding to X value %.15g, values contained in %s dimension are not similar with read previous values.\n"
        "Only the first values will be stored in the NetCdf output file.\n" 
        "Expressions below that depend on %s dimension might be inconsistent:\n%s";

  CNetCDFDimension* netCDFdim = m_internalFiles->GetNetCDFDim(m_names[indexExpr]);
  if (netCDFdim == NULL)
  {
    throw CException(CTools::Format("ERROR - CBratProcessYFX::CheckValuesSimilar - Unable to find dimension '%s' in output Netcdf file.",
                                    m_names[indexExpr].c_str()), 
                     BRATHL_LOGIC_ERROR);
  }

  CStringArray strArray;
  strArray.Insert(m_names[indexExpr]);

  CStringArray complement;
  CStringArray* coordVars = netCDFdim->GetCoordinateVariables();

  strArray.Complement(*coordVars, complement);

  string coordVarAsString = complement.ToString(", ", false);

  msg = CTools::Format(format.c_str(), 
                       m_measures.begin()->first,
                       m_names[indexExpr].c_str(),
                       m_names[indexExpr].c_str(),
                       coordVarAsString.c_str());
                       ;
  return bSimilarValues;

}
//----------------------------------------
bool CBratProcessYFX::CheckValuesSimilar(uint32_t indexExpr)
{

  bool bSimilarValues = true;

  double* dataValuesRef = NULL;

  uint32_t cols = m_measures.GetMatrixColDim(indexExpr);
  if (cols <= 0)
  {
    return true;
  }

  CDoublePtrDoubleMap::const_iterator itX;
  
  for (itX = m_measures.begin(); itX != m_measures.end() ; itX++)
  {
    DoublePtr* fieldsArray = itX->second;
    if (fieldsArray == NULL)
    {
      continue;
    }

    double* dataValues = fieldsArray[indexExpr];
      
    if (dataValues == NULL)
    {
      continue;
    }

    if (dataValuesRef == NULL)
    {
      dataValuesRef = dataValues;
      continue;
    }


    uint32_t indexValues = 0;

    for (indexValues = 0 ; indexValues < cols; indexValues++)
    {
      if ( ! CTools::AreEqual(dataValues[indexValues], dataValues[indexValues]) )
      {
        bSimilarValues = false;
        break;
      }
    }

    if (! bSimilarValues)
    {
      break;
    }

  }

  return bSimilarValues;

}
//----------------------------------------

}
