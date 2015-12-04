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

#include <fstream>

#include "new-gui/Common/tools/Exception.h"
#include "FileParams.h"

#include "Field.h"
#include "BratProcess.h"
#include "BratProcessExportAscii.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;
using namespace processes;

namespace processes
{

const KeywordHelp CBratProcessExportAscii::ExportAsciiKeywordList[]	= {
	KeywordHelp(kwFILE,		1, 0),
	KeywordHelp(kwRECORD),
	KeywordHelp(kwOUTPUT),
	KeywordHelp(kwFIELD,		1, 0, NULL, 24),
	KeywordHelp(kwFIELD_UNIT,	-24),
	KeywordHelp(kwFIELD_FORMAT,	0, -24),
	KeywordHelp(kwSELECT,		0, 0),
	KeywordHelp(kwALIAS_NAME,	0, 0, "None", 14),
	KeywordHelp(kwALIAS_VALUE,	-14),
	KeywordHelp(kwUNIT_ATTR_NAME,	0, 0, "None", 14),
	KeywordHelp(kwUNIT_ATTR_VALUE,	-14),
	KeywordHelp(kwVERBOSE,		0, 1, "0"),
 	KeywordHelp(kwLOGFILE,		0, 1, ""),
	KeywordHelp(kwEXPAND_ARRAY,	0, 1, "No"),
	KeywordHelp(kwDATE_AS_PERIOD,	0, 1, "No"),
  KeywordHelp(""),
};


//-------------------------------------------------------------
//------------------- CBratProcessExportAscii class --------------------
//-------------------------------------------------------------

CBratProcessExportAscii::CBratProcessExportAscii()
{
  Init();
}
//----------------------------------------
    
CBratProcessExportAscii::~CBratProcessExportAscii()
{
  DeleteOutputFile();
}
//----------------------------------------
void CBratProcessExportAscii::Init()
{
  m_outputFile = NULL;
  m_dateAsPeriod = false;
  m_writeHeader = true;


}
//----------------------------------------
void CBratProcessExportAscii::ResizeArrayDependOnFields(uint32_t size)
{
  CBratProcess::ResizeArrayDependOnFields(size);
  
  m_formats.resize(size);
  
}


//----------------------------------------
void CBratProcessExportAscii::GetParameters(const std::string& commandFileName)
{
  m_commandFileName = commandFileName;
  GetParameters();
}
//----------------------------------------
void CBratProcessExportAscii::GetParameters()
{
  LoadParams(m_commandFileName);
  CheckFileParams();

  CFileParams& params = *m_fileParams;
  CUnit	unit;

// Verify keyword occurences
  uint32_t	nbExpr		= params.CheckCount(kwFIELD,  1, -1);
  uint32_t	nbFormats	= params.CheckCount(kwFIELD_FORMAT, 0, -1);
  uint32_t	nbNames    	= params.CheckCount(kwFIELD_NAME, 0, -1);
  uint32_t	nbOutput	= params.CheckCount(kwOUTPUT, 0);

  params.CheckCount(kwRECORD);
  params.CheckCount(kwFIELD_UNIT, nbExpr);

  if (nbFormats != 0)
  {
    params.CheckCount(kwFIELD_FORMAT, nbExpr);
  }

  if (nbNames != 0)
  {
    params.CheckCount(kwFIELD_NAME, nbExpr);
  }

// Get keyword values

  //CTrace *p =
  CTrace::GetInstance();

  if (nbOutput != 0)
  {
    params.m_mapParam[kwOUTPUT]->GetValue(m_outputFileName);
  }

  if (m_outputFileName != "")
  {
    std::ofstream	*oFile;
    CTrace::Tracer(1, CBratProcess::PCT_StrFmt, "Output file", m_outputFileName.c_str());
    oFile = new std::ofstream;
    oFile->open(m_outputFileName.c_str());
    if (oFile->fail())
    {
      throw CFileException("Error creating file", m_outputFileName, BRATHL_IO_ERROR);
    }
    m_outputFile	= oFile;
  }
  else
  {
    CTrace::Tracer(1, CBratProcess::PCT_StrFmt, "Output file", "Standard output");
    m_outputFile	= &std::cout;
  }

  params.m_mapParam[kwRECORD]->GetValue(m_recordName);
  CTrace::Tracer(1, CBratProcess::PCT_StrFmt, "Data set name", m_recordName.c_str());

  //ExpandArray = false;
  if (params.CheckCount(kwEXPAND_ARRAY, 0, 1) == 1)
  {
    params.m_mapParam[kwEXPAND_ARRAY]->GetValue(m_expandArray);
  }
  CTrace::Tracer(1, CBratProcess::PCT_IntFmt, "Expand array", m_expandArray);

  if (params.CheckCount(kwDATE_AS_PERIOD, 0, 1) == 1)
  {
    params.m_mapParam[kwDATE_AS_PERIOD]->GetValue(m_dateAsPeriod);
  }
  CTrace::Tracer(1, CBratProcess::PCT_IntFmt, "Date as period", m_dateAsPeriod);

  GetSelectParameter(params);

  ResizeArrayDependOnFields(nbExpr);

  uint32_t index;

  for (index = 0; index < nbExpr; index++)
  {
    std::string* namesTmp = NULL;

    if (nbNames > 0)
    {
      namesTmp = &m_names[index];
    }

    this->GetDefinition(params,
	      kwFIELD,
	      m_fields[index],
	      namesTmp,
	      NULL,
	      &m_units[index],
	      NULL,
	      NULL,
	      &m_formats[index],
	      NULL,
	      "Value/Column",
	      index,
	      nbExpr);
  }

  DeleteFileParams();
}
//----------------------------------------
bool CBratProcessExportAscii::CheckCommandLineOptions(int	argc, char	**argv)
{
  return CBratProcess::CheckCommandLineOptions(argc, argv,
      			      "This program writes formatted data in ascii files",
	      		      ExportAsciiKeywordList);

}

//----------------------------------------
void CBratProcessExportAscii::BeforeBuildListFieldsToRead()
{

}
//----------------------------------------
void CBratProcessExportAscii::AfterBuildListFieldsToRead()
{

}

//----------------------------------------
void CBratProcessExportAscii::BuildListFieldsToRead()
{
  // get original number of expresion, because AddFielIndexes can add new expression (insert at the end of m_fields) 
  size_t nbExpr = m_fields.size();

  // build, from FIELD parameter, the list of the fields to read in the data files
  for (uint32_t i = 0 ; i < nbExpr ; i++)
  {
    m_listFieldsToRead.InsertUnique(m_fields.at(i).GetFieldNames());
  }

  // build, from SELECT parameter, the list of the fields to read in the data files
  m_listFieldsToRead.InsertUnique(m_select.GetFieldNames());
}


//----------------------------------------
int32_t CBratProcessExportAscii::Execute(std::string& msg)
{ 
    UNUSED(msg);

  int32_t result = BRATHL_SUCCESS;

  CDate startExec;
  startExec.SetDateNow();

  if (m_product == NULL)
  {
	  throw CException("PROGRAM ERROR: product is NULL (CBratProcessExportAscii::Execute) - Perhaps, initialization has not been called",
			   BRATHL_LOGIC_ERROR);
  }


  m_product->SetExpandArray(m_expandArray);


  // Always 'expand array' for Netcdf Product (To show data in column and data value by line)
  m_product->SetForceReadDataOneByOne(true);

  m_writeHeader = true;

  size_t nbFiles = m_inputFiles.size();
  uint32_t cptFile = 0;

  CStringArray::iterator itFile;

  //for each file, read and write data
  for (itFile = m_inputFiles.begin() ; itFile != m_inputFiles.end() ; itFile++)
  {
    cptFile++;

    CTrace::Tracer(1,"File %d/%d - Reading record data from %s ... and registering data ...",
                   cptFile, nbFiles, (*itFile).c_str());

    // open file an set record name and list of field to read
    m_product->SetFieldSpecificUnits(m_fieldSpecificUnit);
    m_product->Open(*itFile, m_recordName);

    m_product->SetListFieldToRead(m_listFieldsToRead, false);

    // Get the number of record for the default record name (set in Open method of CProduct above)
    int32_t nRecords = m_product->GetNumberOfRecords();

    for (int32_t iRecord = 0 ; iRecord < nRecords ; iRecord++)
    {
      //Read fields for the record name  (listof field and record name are set in Open method of CProduct above)
      m_product->ReadBratRecord(iRecord);

      WriteData();
      //m_writeHeader = false;
    }

    m_product->Close();

  }

  if (m_product->GetSkippedRecordCount() > 0)
  {
    std::string msg = CTools::Format("WARNING - %d input data records have been skipped due to inconsistency between two measures",
                                m_product->GetSkippedRecordCount());
    CTrace::Tracer(1, msg);
  }

  CTrace::Tracer(1,"End processing - OK");



  CDate endExec;
  endExec.SetDateNow();
  
  CTrace::Tracer(1, CTools::Format("Processing time: %.3f seconds (%.2f minutes)\n", 
                                  (endExec - startExec),
                                  (endExec - startExec) / 60.0));

 
  return result;
}


//----------------------------------------
int32_t CBratProcessExportAscii::WriteData()
{
  int32_t result = BRATHL_SUCCESS;
  //CTrace *p = CTrace::GetInstance();

  //p->Tracer(1, "Writing data ...");

  CObArray::iterator itDataSet;
  CRecordSet* recordSet = NULL;

  CDataSet* dataSet = m_product->GetDataSet();

  for (itDataSet = dataSet->begin(); itDataSet != dataSet->end() ; itDataSet++)
  {
    recordSet = dataSet->GetRecordSet(itDataSet);

    ///bool firstRecord = (record == dataSet->front());

    CExpressionValue exprValueSelect;
  
    if (m_alwaysTrue == false)
    {
      recordSet->ExecuteExpression(m_select, m_recordName, exprValueSelect, m_product);
    }

    if (m_writeHeader)
    {
      for (uint32_t indexExpr = 0 ; indexExpr < m_fields.size() ; indexExpr++)
      {
        CExpressionValue exprValueData;
        recordSet->ExecuteExpression(m_fields[indexExpr], m_recordName, exprValueData, m_product);
        
        std::string colName = (m_names[indexExpr].empty() ? m_fields[indexExpr].AsString() : m_names[indexExpr]);

        std::string unitStr;
        unitStr.append(" (");
        unitStr.append(m_units[indexExpr].AsString(false, true));
        unitStr.append(") ");

        if (m_units[indexExpr].IsDate() && !m_dateAsPeriod )
        {
          unitStr = "";
        }

        if (m_units[indexExpr].IsDate() && !m_units[indexExpr].HasDateRef() )
        {
          unitStr = "";
        }

        *m_outputFile << colName << unitStr  << " " << exprValueData.GetDimensionsAsString() << "\t";
      }
      *m_outputFile << std::endl;
      m_writeHeader = false;
    }

    if (m_alwaysTrue == false)
    {
      if (exprValueSelect.IsTrue() != 1)
      {
        //Data doesn't match SELECT options,
        //-------------
        continue;
        //-------------
      }
    }

    for (uint32_t indexExpr = 0 ; indexExpr < m_fields.size() ; indexExpr++)
    {
      CExpressionValue exprValueData;
      recordSet->ExecuteExpression(m_fields[indexExpr], m_recordName, exprValueData, m_product);

      *m_outputFile << exprValueData.AsString(m_units[indexExpr], m_formats[indexExpr], m_dateAsPeriod) << "\t";
    }
    *m_outputFile << std::endl;

  }

  //p->Tracer(1,"End writing data ...");

  return result;
}
//----------------------------------------
void CBratProcessExportAscii::DeleteOutputFile()
{
  if (m_outputFile != NULL)
  {
    delete m_outputFile;
    m_outputFile = NULL;
  }
}

//----------------------------------------


}
