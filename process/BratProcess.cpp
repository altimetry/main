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

#include "new-gui/Common/tools/Exception.h"
#include "BratAlgorithmBase.h"
#include "BratProcess.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;
using namespace processes;

namespace processes
{
const int32_t CBratProcess::NB_MAX_Y = 100;

/*
** Value used to distinguish invalid/missing data (known as default value
** and set by set by setDefaultValue) and the fact that no data
** has yet been taken into account.
*/
const double CBratProcess::MergeIdentifyUnsetData = 61615590737044764481.0; // Reversed order of digits of CTools::m_defaultValueDOUBLE


const char* CBratProcess::PCT_HeaderFmt =	"     %s";
const char* CBratProcess::PCT_HeaderCountFmt = "     %s #%d";
const char* CBratProcess::PCT_StrFmt = "     %-25s: %s";
const char* CBratProcess::PCT_FltFmt = "     %-25s: %g";
const char* CBratProcess::PCT_IntFmt = "     %-25s: %d";
const char* CBratProcess::PCT_QStrFmt =	"     %-25s: '%s'";
const char* CBratProcess::PCT_StrFmt2 =	"          %-20s: %s";
const char* CBratProcess::PCT_FltFmt2	= "          %-20s: %g";
const char* CBratProcess::PCT_IntFmt2	= "          %-20s: %d";
const char* CBratProcess::PCT_QStrFmt2 = "          %-20s: '%s'";
const char* CBratProcess::PCT_FltFmt2BaseUnit = "          %-20s: %g (%g %s)";

const CParameter::KWValueListEntry	CBratProcess::FilterKeywords[] =
	{
		{ "NONE",		0		},
		{ "LOESS_SMOOTH",	1		},
		{ "LOESS_EXTRAPOLATE",	2		},
		{ "LOESS",		3		},
		{ NULL, 0 }
	};


const CParameter::KWValueListEntry	CBratProcess::DataModeKeywords[] =
{
	{ "FIRST",	pctFIRST	},
	{ "LAST",	pctLAST		},
	{ "MIN",	pctMIN		},
	{ "MAX",	pctMAX		},
	{ "MEAN",	pctMEAN		},
	{ "STDDEV",	pctSTDDEV	},
	{ "COUNT",	pctCOUNT	},
	{ "SUM",	pctSUM		},
	{ "SUBTRACTION",pctSUBSTRACT	},
	{ "PRODUCT",	pctPRODUCT	},
	{ NULL, 0 }
};

const CParameter::KWValueListEntry	CBratProcess::OutSideModeKeywords[] =
{
	{ "STRICT",	pctSTRICT	},
	{ "RELAXED",	pctRELAXED	},
	{ "BLACK_HOLE",	pctBLACK_HOLE	},
	{ NULL, 0 }
};

const CParameter::KWValueListEntry	CBratProcess::PositionModeKeywords[] =
{
	{ "EXACT",	pctEXACT	},
	{ "NEAREST",	pctNEAREST	},
	{ NULL, 0 }
};

//-------------------------------------------------------------
//------------------- CBratProcess class --------------------
//-------------------------------------------------------------

CBratProcess::CBratProcess()
{
  Init();
}
//----------------------------------------
    
CBratProcess::~CBratProcess()
{
  DeleteProduct();
  DeleteInternalFile();
  DeleteFileParams();
} 
//----------------------------------------
void CBratProcess::Init()
{
  m_nbDataAllocated = 0;
  
  m_fileParams = NULL;
  m_product = NULL;
  m_internalFiles = NULL;

  m_expandArray = false;
  m_alwaysFalse = false;
  m_alwaysTrue = false;

  setDefaultValue(m_validMin);
  setDefaultValue(m_validMax);

}

//----------------------------------------
void CBratProcess::DeleteProduct()
{
  if (m_product != NULL)
  {
    delete m_product;
    m_product = NULL;
  }
}
//----------------------------------------
void CBratProcess::DeleteInternalFile()
{
  if (m_internalFiles != NULL)
  {
    delete m_internalFiles;
    m_internalFiles = NULL;
  }
}
//----------------------------------------
void CBratProcess::DeleteFileParams()
{
  if (m_fileParams != NULL)
  {
    delete m_fileParams;
    m_fileParams = NULL;
  }
}
//----------------------------------------
void CBratProcess::LoadParams(const std::string& name, uint32_t mode /*= CFile::modeRead|CFile::typeBinary*/)
{
  DeleteFileParams();

  // Creates, opens and read parameters file
  m_fileParams = new CFileParams(name, mode);
  
  //CTrace::GetInstance()->SetTraceLevel(5);
  m_fileParams->SetVerboseLevel();
  m_fileParams->Dump(*CTrace::GetDumpContext());

  CTrace *p = CTrace::GetInstance();
  p->Tracer(1,"Parameters:");

  SetFieldSpecificUnits(m_fileParams->GetFieldSpecificUnits());

  // at this point, the formulas aliases have been substituted 
  // The substitution of fields aliases is needed:
  // Get the input files to be able to create a product instance.
  CBratProcess::GetFileList(*m_fileParams, kwFILE, m_inputFiles, "Input file");

  // Create a product instance.
  LoadProductDictionary(*(m_inputFiles.begin()), true);

  // Substitute fields aliases
  m_fileParams->SubstituteAliases(*(m_product->GetAliasesAsString()));

}
//----------------------------------------
void CBratProcess::CheckFileParams()
{
  if (m_fileParams == NULL)
  {
    throw CFileException("CBratProcess::CheckFileParams - m_fileParams is NULL - Parameters file doesn't seem to be loaded.", 
                          m_commandFileName.c_str(), BRATHL_IO_ERROR);
  }
}

//----------------------------------------
void CBratProcess::SetExpandArray(const CExpression& expr, bool expandArray)
{
  SetExpandArray(expr.GetFieldNames(), expandArray);

}
//----------------------------------------
void CBratProcess::SetExpandArray(const CStringArray* fields, bool expandArray)
{
  CStringArray::const_iterator it;
  
  for ( it = fields->begin( ); it != fields->end( ); it++ )
  {
    CField* field = m_product->FindFieldByName(*it, m_recordName);
    if (field != NULL)
    {
      field->SetExpandArray(expandArray);
    }
  }

}
//----------------------------------------
void CBratProcess::LoadProductDictionary(const std::string& filename, bool createVirtualField /* = true */)
{

  ConstructProduct(filename, true);
  
  m_product->Open(filename);

}
//----------------------------------------
void CBratProcess::GetOrderedDimNames(const std::vector<CExpression>& fields, CStringArray& commonDimensions)
{
  size_t nbExpr = fields.size();
  
  // Add each expression (as NetCdf variable) to the output Netcdf fle
  for (uint32_t i = 0 ; i < nbExpr ; i++)
  {
    GetOrderedDimNames(fields.at(i), commonDimensions);
  }
}

//----------------------------------------
void CBratProcess::GetOrderedDimNames(const CExpression& expr, CStringArray& commonDimensions)
{
  GetOrderedDimNames(*(expr.GetFieldNames()), commonDimensions);
}
//----------------------------------------
void CBratProcess::GetOrderedDimNames(const std::string& fieldName, CStringArray& commonDimensionNames)
{
  CStringArray fieldNames;
  fieldNames.Insert(fieldName);

  GetOrderedDimNames(fieldNames, commonDimensionNames);

}
//----------------------------------------
void CBratProcess::GetOrderedDimNames(const CStringArray& fieldNames, CStringArray& commonDimensionNames)
{
  if (! CBratProcess::IsProductNetCdf() )
  {
    return;
  }

  if (fieldNames.size() <= 0)
  {
    return;
  }


  CProductNetCdf* productNetCdf = GetProductNetCdf(m_product);

  
  CExternalFilesNetCDF* externalFile = productNetCdf->GetExternalFile();
  
  if (externalFile == NULL)
  {
    return;
  }

  
  //externalFile->GetOrderedDimNames(&fieldNames, commonDimensionNames);
  externalFile->GetOrderedDimNamesFromFieldNetcdf(&fieldNames, commonDimensionNames);

}
//----------------------------------------
void CBratProcess::CheckDimensions(const CExpression& expr, CUIntArray& commonDimensions)
{

  std::string msg;
  if (m_product == NULL)
  {
    throw CException("ERROR: CBratProcess::CheckDimensions - Unable to continue process because m_product is NULL.\n",
			               BRATHL_LOGIC_ERROR);

  }

  bool bOk = m_product->HasCompatibleDims(expr, m_recordName, msg, true, &commonDimensions);

  if (!bOk)
  {
    throw CException(msg, BRATHL_INCONSISTENCY_ERROR);
  }

  if (m_internalFiles == NULL)
  {
    throw CException("ERROR: CBratProcess::CheckDimensions - Unable to continue process because m_internalFiles is NULL.\n",
			               BRATHL_LOGIC_ERROR);

  }

}

//----------------------------------------
void CBratProcess::AddFieldIndexes(uint32_t indexExpr, CNetCDFVarDef* varDef, const CUIntArray& commonDimensions)
{
  if (varDef == NULL)
  {
    return;
  }
  

  CStringArray indexNames;

  CUIntArray::const_iterator itUint;

  for(itUint = commonDimensions.begin() ; itUint != commonDimensions.end() ; itUint++)
  {
    uint32_t nbElts = *itUint;

    std::string name = CBratProcess::GetIndexName(nbElts); 
    
    indexNames.Insert(name);

    CNetCDFDimension dim(name, nbElts);

    CNetCDFDimension* addedDim = m_internalFiles->AddNetCDFDim(dim);

    addedDim->AddCoordinateVariable(name);
    addedDim->AddCoordinateVariable(varDef->GetName());

    CNetCDFVarDef* addedVarDimIndex = m_internalFiles->GetNetCDFVarDef(name);
    if (addedVarDimIndex == NULL)
    {
      CNetCDFCoordinateAxis varDimIndex(name, "count");
      
      varDimIndex.SetValidMinMax(0, nbElts - 1);

      varDimIndex.AddAttribute(new CNetCDFAttrString(LONG_NAME_ATTR, CTools::Format("Dimension index of %d elements", nbElts)));
      varDimIndex.AddAttribute(new CNetCDFAttrString(COMMENT_ATTR, "Set to be compliant with the CF-1.1 convention"));

      addedVarDimIndex = m_internalFiles->AddNetCDFVarDef(varDimIndex);
      addedVarDimIndex->AddNetCDFDim(*addedDim);
    }
  

    CNetCDFVarDef* addedVarDef = m_internalFiles->AddNetCDFVarDef(*varDef);

    addedVarDef->AddNetCDFDim(*addedDim);
  }
  
  m_mapFieldIndexesByExpr.Insert(m_names[indexExpr], indexNames);

  // Warning use a copy of fields because expression will be added
  // to expression array and iterator will be changed.
  CStringArray fields = *(m_fields.at(indexExpr).GetFieldNames());

  CStringArray::const_iterator it;
  
  for ( it = fields.begin( ); it != fields.end( ); it++ )
  {
    AddFieldIndexes(*it, varDef);
  }
}

//----------------------------------------
void CBratProcess::AddFieldIndexes(const std::string& fieldName, CNetCDFVarDef* varDef)
{
  if (varDef == NULL)
  {
    return;
  }

  if (m_product == NULL)
  {
    LoadProductDictionary(*(m_inputFiles.begin()));
  }

  CField* field = m_product->FindFieldByName(fieldName, m_recordName);
  
  AddFieldIndexes(field, varDef);

}

//----------------------------------------
void CBratProcess::AddFieldIndexes(CField* field, CNetCDFVarDef* varDef)
{
  if (varDef == NULL)
  {
    return;
  }

  if (field == NULL)
  {
    return;
  }

  CObArray* fieldIndexes = field->GetFieldIndexes();
  if (fieldIndexes == NULL)
  {
    return;
  }

  CObArray::const_iterator it;
  
  for (it = fieldIndexes->begin() ; it != fieldIndexes->end() ; it++)
  {
    CFieldIndex* fieldIndex = dynamic_cast<CFieldIndex*>(*it);
    AddFieldIndexes(fieldIndex, varDef);
    
  }

}
//----------------------------------------
void CBratProcess::AddFieldIndexes(CFieldIndex* fieldIndex, CNetCDFVarDef* varDef)
{
  if (varDef == NULL)
  {
    return;
  }

  if (fieldIndex == NULL)
  {
    return;
  }

/*

  CExpression expr(fieldIndex->GetName());
  
  CUnit unit;
  
  std::string name = CTools::Format("index_%ld", nbElts); 

  std::string title = CTools::Format("Index of array dimension with %ld elements", nbElts); 

  m_fields.push_back(expr);
  
  m_units.push_back(unit);
  
  m_names.push_back(name);
  
  m_titles.push_back(title);
  
  m_comments.push_back("");
  
  m_types.push_back(Data);
  
  m_dataMode.push_back(CBratProcess::pctFIRST);

  m_listFieldsToRead.InsertUnique(fieldIndex->GetName());
  */

  // Make a copy of CFieldIndex object becuse product file can be close and so CFieldIndex object not eanble anymore)
  long nbElts = static_cast<long>(fieldIndex->GetNbElts());

  std::string name = CBratProcess::GetIndexName(nbElts);
  
  CFieldIndex* fIndex = new CFieldIndex(*fieldIndex);
  m_mapFieldIndexesToRead.Insert(name, fIndex, false);

/*  
  if (m_internalFiles == NULL)
  {
    throw CException("ERROR: CBratProcess::AddFieldIndexes(CFieldIndex* fieldIndex) - Unable to continue process because m_internalFiles is NULL.\n",
			               BRATHL_LOGIC_ERROR);

  }



  CNetCDFDimension dim(name, nbElts);

  CNetCDFDimension* addedDim = m_internalFiles->AddNetCDFDim(dim);
  
  addedDim->AddCoordinateVariable(name);
  addedDim->AddCoordinateVariable(varDef->GetName());

  CNetCDFVarDef* addedVarDimIndex = m_internalFiles->GetNetCDFVarDef(name);
  if (addedVarDimIndex == NULL)
  {
    CNetCDFCoordinateAxis varDimIndex(name);
    addedVarDimIndex = m_internalFiles->AddNetCDFVarDef(varDimIndex);
    addedVarDimIndex->AddNetCDFDim(*addedDim);
  }
  
//  CNetCDFCoordinateAxis coordVar(*varDef);
  
  //CNetCDFVarDef* addedVarDef = m_internalFiles->AddNetCDFVarDef(coordVar);
  CNetCDFVarDef* addedVarDef = m_internalFiles->AddNetCDFVarDef(*varDef);
  
  addedVarDef->AddNetCDFDim(*addedDim);
*/

}

//----------------------------------------
std::string CBratProcess::GetIndexName(long nbElts)
{
  return CTools::Format("index_%ld", nbElts); 

}
//----------------------------------------
void CBratProcess::ResizeArrayDependOnFields(uint32_t size)
{
  m_fields.resize(size);
  m_units.resize(size);
  m_names.resize(size);
  m_titles.resize(size);
  m_comments.resize(size);
  m_types.resize(size);
  m_dataMode.resize(size);
  m_countOffsets.resize(size);
  m_meanOffsets.resize(size);


}

//----------------------------------------
void CBratProcess::AdjustValidMinMax(double value)
{
  if (isDefaultValue(value))
  {
    return;
  }

  if (isDefaultValue(m_validMin))
  {
    m_validMin = value;
  }
  else if (m_validMin > value)
  {
    m_validMin = value;
  }

  if (isDefaultValue(m_validMax))
  {
    m_validMax = value;
  }
  else if (m_validMax < value)
  {
    m_validMax = value;
  }
}

//----------------------------------------

CBratProcess::MergeDataMode CBratProcess::GetDataMode
		(CFileParams	&params,
		 int32_t	minOccurences	/* = 0 */,
		 int32_t	maxOccurences	/* = 1 */,
		 const std::string	&Keyword	/* = "DATA_MODE" */,
		 int32_t	index		/* = 0 */,
		 MergeDataMode defaultValue /* = pctMEAN */)
{

  CTrace *p = CTrace::GetInstance();

  if (params.CheckCount(Keyword, minOccurences, maxOccurences) == 0)
  {
    return defaultValue;
  }

  std::string	tmp;
  uint32_t	tmpVal;

  params.m_mapParam[Keyword]->GetValue(tmpVal,
				                               tmp,
				                               CBratProcess::DataModeKeywords,
				                               index,
				                               defaultValue);

  CBratProcess::MergeDataMode val = static_cast<CBratProcess::MergeDataMode>(tmpVal);

  p->Tracer(1, CBratProcess::PCT_StrFmt2, "Data mode",  CBratProcess::DataModeStr(val).c_str());

  return val;
}

//----------------------------------------

CBratProcess::MergeDataMode CBratProcess::GetDataMode
		(CFileParams	&params,
		 const std::string	&prefix,
		 int32_t	minOccurences	/*= 0*/,
		 int32_t	maxOccurences	/*= 1*/,
		 int32_t	index		/*= 0*/,
		 MergeDataMode	defaultValue		/*= pctMEAN*/)
{

  std::string tmpKey	= prefix + "_DATA_MODE";

  return CBratProcess::GetDataMode(params, minOccurences, maxOccurences, tmpKey, index, defaultValue);

}

//----------------------------------------

std::string CBratProcess::DataModeStr
		(MergeDataMode	mode)
{
  std::string result;

  switch (mode)
  {
    case CBratProcess::pctFIRST:
      result = "FIRST";
      break;
    case CBratProcess::pctLAST:
      result =  "LAST";
      break;
    case CBratProcess::pctMIN:
      result =  "MIN";
      break;
    case CBratProcess::pctMAX:
      result =  "MAX";
      break;
    case CBratProcess::pctMEAN:
      result =  "MEAN";
      break;
    case CBratProcess::pctSTDDEV:
      result =  "STDDEV";
      break;
    case CBratProcess::pctCOUNT:
      result =  "COUNT";
      break;
    case CBratProcess::pctSUM:
      result =  "SUM";
      break;
    case CBratProcess::pctSUBSTRACT:
      result =	"SUBTRACTION";
      break;
    case CBratProcess::pctPRODUCT:
      result =  "PRODUCT";
      break;
    default:
	    throw CException(CTools::Format("PROGRAM ERROR: merge data mode %d unknown",
					    mode),
			     BRATHL_LOGIC_ERROR);
  }

  return result;
}

//----------------------------------------
int32_t CBratProcess::GetFileList
		(CFileParams		&params,
		 const std::string		&keyword,
		 CStringArray		&names,
		 const std::string		&traceDescription,
		 int32_t		minOccurences	/*= 1*/,
		 int32_t		maxOccurences	/*= -1*/,
		 bool			printTrace	/*= true*/)
{
  CTrace *p = CTrace::GetInstance();
  
  int32_t	result	= params.CheckCount(keyword, minOccurences, maxOccurences);
  
  names.resize(result);

  for (int32_t index=0; index<result; index++)
  {
    params.m_mapParam[keyword]->GetValue(names[index], index);
    if (index == 0)
    {
      p->Tracer(1, CBratProcess::PCT_StrFmt, traceDescription.c_str(), names[index].c_str());
    }
    else
    {
      p->Tracer(1, CBratProcess::PCT_StrFmt, "", names[index].c_str());
    }
  }
  return result;
}

//----------------------------------------
void CBratProcess::AdjustDefinition(CFileParams& params, CExpression& field, bool printTrace /*= true*/)
{

  CTrace* p	= CTrace::GetInstance();

  if (params.CheckCount(kwFILE, 0, -1) < 1)
  {
    return;
  }

  if (params.CheckCount(kwRECORD, 0, 1) < 1)
  {
    return;
  }

  if (m_product == NULL)
  {
    if (m_inputFiles.size() <= 0)
    {
      CBratProcess::GetFileList(params, kwFILE, m_inputFiles, "", 1 , -1, false);
    }
    LoadProductDictionary(*(m_inputFiles.begin()));
  }
  if (m_recordName.empty())
  {
    params.m_mapParam[kwRECORD]->GetValue(m_recordName);
  }

  //Try to correct lower/upper case error in field names.
  std::string out;
  m_product->ReplaceNamesCaseSensitive(field, out);

  //Add record name to field names
  std::string out2 = out;
  std::string errorString;
  bool bOk = m_product->AddRecordNameToField(out2, m_recordName, out, errorString);

  if (!bOk) 
  {
      throw CException(errorString, BRATHL_SYNTAX_ERROR);
  }

  try
  {
    field.SetExpression(out);
  }
  catch (CException& e)
  {
    throw e;
  }

  if (printTrace)
  {
    p->Tracer(1, CBratProcess::PCT_StrFmt2, "Adjusted expression", field.AsString().c_str());
  }

}


//----------------------------------------
//void CBratProcess::GetParametersSpecificUnit(const std::string& commandFileName)
//{
//  m_commandFileName = commandFileName;
//  GetParameters();
//}

//----------------------------------------
int32_t CBratProcess::GetDefinition
		(CFileParams		&params,
		 const std::string		&prefix,
		 CExpression    	&field,
		 std::string			*name,
		 NetCDFVarKind		*kind,
		 CUnit			*unit,
		 std::string			*title,
		 std::string			*comment,
		 std::string			*dataFormat,
		 uint32_t               *group,
 		 const std::string		&traceDescription,
		 int32_t		index		/*= 0*/,
		 int32_t		minOccurences	/*= 1*/,
		 int32_t		maxOccurences	/*= 1*/,
		 bool			printTrace	/*= true*/)
{
  int32_t result = CBratProcess::GetVarDef(params,
		                                       prefix,
		                                       field,
		                                       name,
		                                       kind,
		                                       unit,
		                                       title,
		                                       comment,
		                                       dataFormat,
		                                       group,
 		                                       traceDescription,
		                                       index,
		                                       minOccurences,
		                                       maxOccurences,
		                                       printTrace);

  AdjustDefinition(params, field);


  return result;

}


//----------------------------------------
int32_t CBratProcess::GetDefinition
		(CFileParams		&params,
		 const std::string		&prefix,
		 CExpression    	&field,
		 std::string			*name,
		 NetCDFVarKind		*kind,
		 CUnit			*unit,
		 std::string			*title,
		 std::string			*comment,
		 std::string			*dataFormat,
		 uint32_t               *group,
		 double			&min,
		 double			&max,
		 uint32_t		&count,
		 double			&step,
		 const std::string		&traceDescription,
		 int32_t		index		/*= 0*/,
		 int32_t		minOccurences	/*= 1*/,
		 int32_t		maxOccurences	/*= 1*/,
		 bool			printTrace	/*= true*/)
{

  int32_t result	= GetVarDef(params,
				                      prefix,
				                      field,
				                      name,
				                      kind,
				                      unit,
				                      title,
				                      comment,
				                      dataFormat,
				                      group,
		                          min,
		                          max,
		                          count,
		                          step,
				                      traceDescription,
				                      index,
				                      minOccurences,
				                      maxOccurences);


  AdjustDefinition(params, field);


  return result;

}

//----------------------------------------
int32_t CBratProcess::GetVarDef
		(CFileParams		&params,
		 const std::string		&prefix,
		 CExpression    	&field,
		 std::string			*name,
		 NetCDFVarKind		*kind,
		 CUnit			*unit,
		 std::string			*title,
		 std::string			*comment,
		 std::string			*dataFormat,
		 uint32_t               *group,
 		 const std::string		&traceDescription,
		 int32_t		index		/*= 0*/,
		 int32_t		minOccurences	/*= 1*/,
		 int32_t		maxOccurences	/*= 1*/,
		 bool			printTrace	/*= true*/)
{
  CTrace		*p	= CTrace::GetInstance();
  std::string		tmpKey;
  std::string		tmp;

  if (printTrace)
  {
    if ((index==0) && (minOccurences <= 1) && (maxOccurences <= 1))
      p->Tracer(1, CBratProcess::PCT_HeaderFmt, traceDescription.c_str());
    else
      p->Tracer(1, CBratProcess::PCT_HeaderCountFmt, traceDescription.c_str(), index+1);
  }

  int32_t result	= params.CheckCount(prefix, minOccurences, maxOccurences);

  params.m_mapParam[prefix]->GetValue(field, index);

  if (printTrace)
  {
    p->Tracer(1, CBratProcess::PCT_StrFmt2, "Expression", field.AsString().c_str());
  }

  /*
  p->SetTraceLevel(5);
  const CStringArray* ff= field.GetFieldNames();
  const_cast<CStringArray*>(ff)->Dump(*(p->GetDumpContext()));
*/
//  field.Dump();

  if (name != NULL)
  {
    tmpKey	= prefix + "_NAME";
    params.CheckCount(tmpKey, result);
    params.m_mapParam[tmpKey]->GetValue(*name, index);
    if (printTrace)
    {
      p->Tracer(1, CBratProcess::PCT_StrFmt2, "name",  name->c_str());
    }

    if (! CInternalFiles::IsVarNameValid(*name))
    {
      throw CParameterException(CTools::Format("Invalid name for a netcdf variable: '%s'",
					       name->c_str()),
				BRATHL_SYNTAX_ERROR);
    }
  }

  if (kind != NULL)
  {
    tmpKey	= prefix + "_TYPE";
    params.CheckCount(tmpKey, result);
    params.m_mapParam[tmpKey]->GetValue(tmp, index);

    *kind	= CNetCDFFiles::StringToVarKind(tmp);

    if (*kind == brathl::Unknown)
    {
      throw CParameterException(CTools::Format("Invalid type '%s' for variable '%s'",
					       tmp.c_str(),
					       name->c_str()),
				BRATHL_SYNTAX_ERROR);
    }
    
    if (printTrace)
    {
      p->Tracer(1, CBratProcess::PCT_StrFmt2, "Type",  CNetCDFFiles::VarKindToString(*kind).c_str());
    }
  }

  if (unit != NULL)
  {
    std::string defaultValue	= "count";
    if (kind != NULL)
    {
      switch (*kind)
      {
      //--------------
	    case Latitude:
      //--------------
        defaultValue	= CLatLonPoint::m_DEFAULT_UNIT_LATITUDE; 
        break;

      //--------------
	    case Longitude:	
      //--------------
        defaultValue	= CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE; 
        break;

      //--------------
	    case T:
      //--------------
        defaultValue	= "days since 1950-01-01 00:00:00.000000 UTC"; 
        break;

      //--------------
	    default:
      //--------------
	      defaultValue	= "count"; // Avoids compiler complaint
      }
    }

    tmpKey	= prefix + "_UNIT";
    params.CheckCount(tmpKey, result);
    params.m_mapParam[tmpKey]->GetValue(*unit, index, defaultValue);

    if (printTrace)
    {
      p->Tracer(1, CBratProcess::PCT_StrFmt2, "unit",  unit->GetText().c_str());
    }
    if (kind != NULL)
    {
      if ((*kind == T) && (! unit->IsDate()))
      {
	      throw CParameterException(CTools::Format("Type T and unit is not a time: : '%s'",
					         unit->GetText().c_str()),
				  BRATHL_UNIT_ERROR);
      }
    }
  }

  if (title != NULL)
  {
    tmpKey	= prefix + "_TITLE";
    params.CheckCount(tmpKey, result);
    params.m_mapParam[tmpKey]->GetValue(*title, index);

    if (*title == "")
    {
      *title	= *name;
    }
    if (printTrace)
    {
      p->Tracer(1, CBratProcess::PCT_QStrFmt2, "title (long name)",  title->c_str());
    }
  }

  if (comment != NULL)
  {
    tmpKey	= prefix + "_COMMENT";
    params.CheckCount(tmpKey, 0, result);

    CParameter* paramComment = params.m_mapParam.Exists(tmpKey);

    if (paramComment != NULL)
    {
      paramComment->GetValue(*comment, index);
    }
    if (printTrace)
    {
      p->Tracer(1, CBratProcess::PCT_QStrFmt2, "comment",  comment->c_str());
    }
  }
  
  if (dataFormat != NULL)
  {
    tmpKey	= prefix + "_FORMAT";
    int32_t	NbFormats = params.CheckCount(tmpKey, 0, result);

    if (NbFormats == 0)
    {
      *dataFormat	= "";
    }
    else
    {
      params.CheckCount(tmpKey, result);
      params.m_mapParam[tmpKey]->GetValue(*dataFormat, index);
    }
    if (printTrace)
    {
      p->Tracer(1, CBratProcess::PCT_QStrFmt2, "Data format",  dataFormat->c_str());
    }
  }

  if (group != NULL)
  {
    tmpKey	= prefix + "_GROUP";
    if (params.CheckCount(tmpKey, 0, result) == 0)
    {
      *group = 0;
    }
    else
    {
      params.CheckCount(tmpKey, result, result);
      params.m_mapParam[tmpKey]->GetValue(*group, index);
    }
    if (printTrace)
    {
      p->Tracer(1, CBratProcess::PCT_IntFmt2, "group",  *group);
    }
  }

  return result;
}

//----------------------------------------
int32_t CBratProcess::GetVarDef
		(CFileParams		&params,
		 const std::string		&prefix,
		 CExpression    	&field,
		 std::string			*name,
		 NetCDFVarKind		*kind,
		 CUnit			*unit,
		 std::string			*title,
		 std::string			*comment,
		 std::string			*dataFormat,
		 uint32_t               *group,
		 double			&min,
		 double			&max,
		 uint32_t		&count,
		 double			&step,
		 const std::string		&traceDescription,
		 int32_t		index		/*= 0*/,
		 int32_t		minOccurences	/*= 1*/,
		 int32_t		maxOccurences	/*= 1*/,
		 bool			printTrace	/*= true*/)
{
  CTrace		*p	= CTrace::GetInstance();
  std::string		tmpKey;
  CDate			TmpDate;
  NetCDFVarKind		TheKind = brathl::Unknown;
  bool			isDate	= false;
  uint32_t		Intervals;

  int32_t result	= GetVarDef(params,
				                      prefix,
				                      field,
				                      name,
				                      kind,
				                      unit,
				                      title,
				                      comment,
				                      dataFormat,
				                      group,
				                      traceDescription,
				                      index,
				                      minOccurences,
				                      maxOccurences);

  if (kind != NULL)
  {
    TheKind	= *kind;
  }

  CUnit unitTmp;

  if (unit != NULL)
  {
    unitTmp = *unit;
    isDate	= unit->IsDate();
  }


  //-------------------------
  tmpKey	= prefix + "_MIN";
  //-------------------------

  params.CheckCount(tmpKey, result);
  
  if ((TheKind == T) || isDate)
  {
    params.m_mapParam[tmpKey]->GetValue(TmpDate, unit, index);
    min	= TmpDate.Value();
  }
  else
  {
    params.m_mapParam[tmpKey]->GetValue(min, index);
  }

  //-------------------------
  tmpKey	= prefix + "_MAX";
  //-------------------------
  params.CheckCount(tmpKey, result);

  if ((TheKind == T) || isDate)
  {
    params.m_mapParam[tmpKey]->GetValue(TmpDate, unit, index);
    max	= TmpDate.Value();
  }
  else
  {
    params.m_mapParam[tmpKey]->GetValue(max, index);
  }

  //-------------------------
  tmpKey	= prefix + "_INTERVALS";
  //-------------------------
  params.CheckCount(tmpKey, result);
  params.m_mapParam[tmpKey]->GetValue(Intervals, index);
  
  if (Intervals <= 0)
  {
    throw CParameterException(CTools::Format("Invalid bin count, must be > 0: %d",
					     Intervals),
			      BRATHL_COUNT_ERROR);
  }

  switch (*kind)
  {
    //-------------------------
    case Latitude:
    //-------------------------
      if (unit == NULL)
      {
        unitTmp = CLatLonPoint::m_DEFAULT_UNIT_LATITUDE;
      }

      if (isDefaultValue(Intervals))
      {
        Intervals = static_cast<uint32_t>( CTools::Round(180.0 * CUnit::ConvertToUnit(&unitTmp, 1, 1)) );
      }

      break;
    //-------------------------
    case Longitude:
    //-------------------------
      if (unit == NULL)
      {
        unitTmp = CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE;
      }

      if (isDefaultValue(Intervals))
      {
       Intervals = static_cast<uint32_t>( CTools::Round(360.0 * CUnit::ConvertToUnit(&unitTmp, 1, 1)) );
      }

      break;
    //-------------------------
    default:
    //-------------------------
      if (isDefaultValue(Intervals))
      {
        throw CParameterException(CTools::Format("Invalid default value for interval of variable '%s' which is of type '%s'",
						name->c_str(), CNetCDFFiles::VarKindToString(*kind).c_str()),
				  BRATHL_RANGE_ERROR);
      }
  }
  
  switch (*kind)
  {
    //-----------------
    case Latitude:
    //-----------------
    {
      std::string unitText = unitTmp.GetText();

      if (isDate)
      {
	        throw CParameterException("Time unit for a latitude is onvalid", BRATHL_UNIT_ERROR);
      }

      if (isDefaultValue(min))
      {
	      min	= CUnit::ConvertToUnit(&unitTmp, -90, -90.0);
      }
      else
      {
        double value = CUnit::ConvertToBaseUnit(&unitTmp, min);
	      if ((value < -90.0) || (value > 90.0))
        {
          throw CParameterException(CTools::Format("Invalid latitude value for min: %g %s",
					                 value, unitText.c_str()),
				          BRATHL_RANGE_ERROR);
        }
      }

      if (isDefaultValue(max))
      {
	      max	= CUnit::ConvertToUnit(&unitTmp, 90, 90.0);
      }
      else
      {
        double value = CUnit::ConvertToBaseUnit(&unitTmp, max);
	      if ((value < -90.0) || (value > 90.0))
        {
	        throw CParameterException(CTools::Format("Invalid latitude value for max: %g %s",
					                 value, unitText.c_str()),
				          BRATHL_RANGE_ERROR);
        }
      }

      if (CTools::Compare(min, max) >= 0)
      {
	      throw CParameterException(CTools::Format("Invalid min/max order: %g/%g",
						       min, max),
				        BRATHL_RANGE_ERROR);
      }
      break;
    }
    //-----------------
    case Longitude:
    //-----------------
    {
      if (isDate)
      {
	      throw CParameterException("Time unit for a longitude is invalid", BRATHL_UNIT_ERROR);
      }
      if (isDefaultValue(min))
      {
	      min	= CUnit::ConvertToUnit(&unitTmp, -180, -180.0);
      }

      double minBaseUnit = CUnit::ConvertToBaseUnit(&unitTmp, min);

      if (isDefaultValue(max))
      {
      	max	= CUnit::ConvertToUnit(&unitTmp, minBaseUnit + 360.0, 180.0);
      }

      //max	= CTools::NormalizeLongitude(min, max);
      max = CLatLonPoint::LonNormalFrom(max, min, &unitTmp); 

      if (areEqual(min, max))
      {
      	max	= CUnit::ConvertToUnit(&unitTmp, minBaseUnit + 360.0, 180.0);
      }

      break;
    }
    //-----------------
    default:
    //-----------------
      if (isDefaultValue(min) || isDefaultValue(max))
      {
	      throw CParameterException(CTools::Format("Invalid default value for min/max of var '%s'",
				                 name->c_str()),
				  BRATHL_RANGE_ERROR);
      }
      if (CTools::Compare(min, max) >= 0)
      {
	      throw CParameterException(CTools::Format("Invalid min/max order: %g/%g for var '%s'",
						 min, max, name->c_str()),
				  BRATHL_RANGE_ERROR);
      }
  } // end switch (*kind)


  count	= Intervals + 1; // N intervals = N+1 dots

  step	= (max-min)/Intervals;
    
  if (*kind == Longitude)
  {
    // Check if intervals covers the whole earth and so, there is as many
    // dots as intervals
    if (CBratProcess::IsLongitudeCircular(min, max, TheKind, &unitTmp, CLatLonPoint::m_LONGITUDE_COMPARE_EPSILON))
    {
	    count	= Intervals;
    }
    else if (CBratProcess::IsLongitudeCircular(min, max + step, TheKind, &unitTmp, CLatLonPoint::m_LONGITUDE_COMPARE_EPSILON))
    {
      max += step;
      Intervals++;
    }
  }

  if (printTrace)
  {
    if ((TheKind == T) || isDate)
    {
      TmpDate = min;
      p->Tracer(1, CBratProcess::PCT_StrFmt2, "min value", TmpDate.AsString("", true).c_str());

      TmpDate = max;
      p->Tracer(1, CBratProcess::PCT_StrFmt2, "max value", TmpDate.AsString("", true).c_str());
    }
    else
    {
      if (unit != NULL)
      {
        p->Tracer(1, CBratProcess::PCT_FltFmt2BaseUnit, "min value",  min, unit->ConvertToBaseUnit(min), unit->BaseUnit().GetText().c_str());
        p->Tracer(1, CBratProcess::PCT_FltFmt2BaseUnit, "max value",  max, unit->ConvertToBaseUnit(max), unit->BaseUnit().GetText().c_str());
      }
      else
      {
        p->Tracer(1, CBratProcess::PCT_FltFmt2, "min value",  min);
        p->Tracer(1, CBratProcess::PCT_FltFmt2, "max value",  max);
      }
    }

    p->Tracer(1, CBratProcess::PCT_IntFmt2, "Bin count",  Intervals);
    p->Tracer(1, CBratProcess::PCT_FltFmt2, "step",  step);
  }

  if ( (TheKind != T) && !isDate )
  {
    min = CUnit::ConvertToBaseUnit(unit, min, min);
    max = CUnit::ConvertToBaseUnit(unit, max, max);
    step	= (max-min)/Intervals;

  }

  return result;
}

//----------------------------------------
void CBratProcess::GetSelectParameter(CFileParams& params)
{
  CTrace* p = CTrace::GetInstance();

  uint32_t nbSelect = params.CheckCount(kwSELECT, 0, -1);

  if (nbSelect != 0)
  {
    params.m_mapParam[kwSELECT]->GetAllValues(m_select);
  }
  else
  {
    m_select.SetExpression("1"); // Everything is selected
  }

  p->Tracer(1, PCT_StrFmt, "Selection",  m_select.AsString().c_str());

  AdjustDefinition(params, m_select);
}
//----------------------------------------

/*
** Determines how many values are needed to compute the incremental operation
** defined by 'mode'
*/
int32_t CBratProcess::GetMergedDataSlices(CBratProcess::MergeDataMode	mode)
{
/*
** Data itself and count for mean (==> 2)
** Data itself, mean and count for stddev (==> 3)
** Data only for the others (==> 1)
*/
  int32_t dataSlices = 1;
  switch (mode)
  {
    case CBratProcess::pctMEAN:
      dataSlices = 2;
      break;
    case CBratProcess::pctSTDDEV:
      dataSlices = 3;
      break;
    default:
      break;
  }

  return dataSlices;
}
//----------------------------------------
/*
** Checks the command line parameters (with -h -k or parameter file)
** and return true if command line is invalid
** Last entry of keywordList MUST BE NULL or result is unpredictible
** (and maybe a core dump)
*/
bool CBratProcess::CheckCommandLineOptions
		(int			argc,
		 char			**argv,
		 const std::string		&helpString,
		 const KeywordHelp	*keywordList,
		 std::string			&commandFileName)
{
  bool		error		= false;
  bool		help		= false;
  bool		keywords	= false;
  std::ostream	*out		= &std::cerr;
  if (argc != 2)
    error	= true;
  else if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
  {
    help	= true;
    out		= &std::cout;
  }
  else if ((strcmp(argv[1], "-k") == 0) || (strcmp(argv[1], "--keywords") == 0))
  {
    if (keywordList != NULL)
    {
      keywords		= true;
      out		= &std::cout;
    }
    else
    {
      *out << "invalid option: " << argv[1] << std::endl;
      error		= true;
    }
  }
  else
  {
    commandFileName.assign(argv[1]);

    if (! CTools::FileExists(commandFileName))
    {
      *out << "ERROR: Command file '" << commandFileName << "' not found" << std::endl << std::endl;
      error	= true;
    }
  }

  if (error || help)
  {
    *out << "Usage : " << argv[0] << " [ -h | --help";
    if (keywordList != NULL)
      *out << " | -k | --keywords";
    *out << "] commandFileName" << std::endl;
    *out << "Where commandFileName is the name of a file containing runtime parameters" << std::endl;
  }

  if (keywords)
  {
    CBratProcess::PrintParameterHelp(*out, keywordList);
  }

  if (help && (helpString != ""))
    *out << std::endl << helpString << std::endl;

  return error || help || keywords;
}

//----------------------------------------
bool CBratProcess::CheckCommandLineOptions(int argc, char	**argv, const std::string& helpString, const KeywordHelp	*keywordList)
{
  return CBratProcess::CheckCommandLineOptions(argc, argv, helpString, keywordList, m_commandFileName);
}

//----------------------------------------
/*
** Print help about a list of keywords which can be foun in a parameter file
** definitions are in ParametersDictionary.h
*/
void CBratProcess::PrintParameterHelp
		(std::ostream		&where,
		 const KeywordHelp	*keywordList)
{
  int NbTypes	  = sizeof(KnownTypes)/sizeof(KnownTypes[0]);
  int *TypesIndex = new int[NbTypes];
  int index;

  memset(TypesIndex, 0, NbTypes*sizeof(*TypesIndex));

  for (; keywordList->Name.length() != 0; ++keywordList)
  {
    KeywordDefinition	*CurrentKW;
    TypeDefinition	*CurrentType;
    //for (CurrentKW = static_cast<KeywordDefinition*>(KnownKeywords) ;
    for (CurrentKW = (KeywordDefinition*)(KnownKeywords) ;
	 CurrentKW->Name.length() != 0;
	 ++CurrentKW)
    {
      if (CTools::StrCaseCmp(keywordList->Name.c_str(), CurrentKW->Name.c_str()) == 0)
      {
        std::string	Count	= "";
	int	min	= keywordList->MinCount;
	int	max	= keywordList->MaxCount;
	int 	group	= keywordList->Group;
	if (min < 0)
	{
	  Count	= CTools::Format("%c", 64-min);
	}
	else
	{
	  if (max == 0)
	  {
	    Count	= CTools::Format("[%d-n]", min);
	  }
	  else if (max < 0)
	  {
	    Count	= CTools::Format("%d or %c ", min, 64-max);
	  }
	  else
	  {
	    if (max <= min)
	      Count	= CTools::Format("%d", min);
	    else
	      Count	= CTools::Format("[%d-%d]", min, max);
	  }
	  if (group != 0)
	  {
	    Count	+= CTools::Format("=%c", 64+abs(group));
	  }
	}
	where << CTools::Format("%-40sType : %-10s Count : %s",
					CurrentKW->Name.c_str(),
					CurrentKW->Type,
					Count.c_str())
		<< std::endl;
	PrintDescription(where, CurrentKW->Description, true, keywordList->Default);
	index	= 0;
	//for (CurrentType = static_cast<TypeDefinition*>(KnownTypes);
	for (CurrentType = (TypeDefinition*)(KnownTypes);
	     CurrentType->Name != NULL;
	     ++CurrentType)
	{
	  if (CTools::StrCaseCmp(CurrentKW->Type, CurrentType->Name) == 0)
	  {
	    TypesIndex[index]	= 1;
	    break;
	  }
	  index++;
	}
	if (CurrentType->Name == NULL)
	{
	  where << std::endl << CurrentKW->Type  << " ===== ERROR unknown type cannot print help" << std::endl << std::endl;
	}
      }
    }
    if (CurrentKW->Name.length() != 0)
    {
      where << std::endl << keywordList->Name << "   ERROR unknown keyword cannot print help" << std::endl << std::endl;
    }
  }

  where << "=====================" << std::endl << "Description of types:" << std::endl << std::endl;
  for (index = 0; index < NbTypes; index++)
  {
    if (TypesIndex[index] != 0)
    {
      where << CTools::Format("%-16s", KnownTypes[index].Name);
      PrintDescription(where,
		       KnownTypes[index].Description,
		       false,
		       NULL);
    }
  }
  delete []TypesIndex;
}
//----------------------------------------
/*
** Internal function used to print keyword/type definition
*/
void CBratProcess::PrintDescription
		(std::ostream	&where,
		 const char	*description,
		 bool		firstLeading,
		 const char	*defaultValue)

{
  const char *leading	= "                ";
/*
** Not very optimized but it works and there is not so
** many things to print
*/
  if (firstLeading)
    where << leading;
  while (*description != '\0')
  {
    if (*description == '\n')
      where << std::endl << leading;
    else
      where << *description;
    description++;
  }
  where << std::endl;
  if (defaultValue != NULL)
  {
    where << leading << "(defaultValue=" << defaultValue << ")" << std::endl;
  }
}


//----------------------------------------
void CBratProcess::GetFilterDefinitions
		(CFileParams		&params,
		 const std::string		&prefix,
		 bool			*smooth,
		 bool			*extrapolate,
		 int32_t		index		/*= 0*/,
		 int32_t		occurences	/*= 1*/,
		 bool			printTrace	/*= true*/)
{

  CTrace	*p	= CTrace::GetInstance();

  std::string	tmpKey;
  int32_t	Count;
  bitSet32	Choice;

  if ((smooth != NULL) || (extrapolate != NULL))
  {
    tmpKey	= prefix + "_FILTER";
    Count	= params.CheckCount(tmpKey, 0, occurences);
    if (Count == 0)
    {
      Choice.reset();
    }
    else
    {
      params.CheckCount(tmpKey, occurences);
      params.m_mapParam[tmpKey]->GetValue(Choice,
                                          CBratProcess::FilterKeywords,
					                                index);
    }
    if (smooth != NULL)
    {
      *smooth		= Choice[0];
    }
    if (extrapolate != NULL)
    {
      *extrapolate	= Choice[1];
    }
    if (printTrace)
    {
      if (smooth != NULL)
      {
	      p->Tracer(1, CBratProcess::PCT_StrFmt2, "smoothing", (*smooth ? "YES" : "NO"));
      }
      if (extrapolate != NULL)
      {
	      p->Tracer(1, CBratProcess::PCT_StrFmt2, "Extrapolating", (*extrapolate ? "YES" : "NO"));
      }
    }
  }
}

//----------------------------------------
void CBratProcess::GetLoessCutoff
		(CFileParams		&params,
		 int32_t		*xCutoff,
		 int32_t		*yCutoff,
		 int32_t		index		/*= 0*/,
		 int32_t		occurences	/*= 1*/,
		 bool			printTrace	/*= true*/)
{
  CTrace	*p	= CTrace::GetInstance();
  std::string	tmpKey;

  if (printTrace && ((xCutoff != NULL) || (yCutoff != NULL)))
  {
    p->Tracer(1, CBratProcess::PCT_HeaderFmt, "Loess parameters");
  }
  if (xCutoff != NULL)
  {
    tmpKey	= "X_LOESS_CUTOFF";
    params.CheckCount(tmpKey, occurences);
    params.m_mapParam[tmpKey]->GetValue(*xCutoff, index, 0);
    if (printTrace)
    {
      p->Tracer(1, CBratProcess::PCT_IntFmt2, "X cutoff", *xCutoff);
    }
  }
  if (yCutoff != NULL)
  {
    tmpKey	= "Y_LOESS_CUTOFF";
    params.CheckCount(tmpKey, occurences);
    params.m_mapParam[tmpKey]->GetValue(*yCutoff, index, 0);
    if (printTrace)
    {
      p->Tracer(1, CBratProcess::PCT_IntFmt2, "Y cutoff", *yCutoff);
    }
  }
}
//----------------------------------------

CBratProcess::OutsideMode CBratProcess::GetOutsideMode
		  (CFileParams	&params,
		   int32_t	minOccurences	/*= 0*/,
		   int32_t	maxOccurences	/*= 1*/,
		   const std::string	&keyword	/*= "OUTSIDE_MODE"*/,
		   int32_t	index		/*= 0*/,
		   OutsideMode	defaultValue		/*= CBratProcess::pctSTRICT*/)
{

  if (params.CheckCount(keyword, minOccurences, maxOccurences) == 0)
  {
    return defaultValue;
  }

  std::string	tmp;
  uint32_t	ympVal;
  
  params.m_mapParam[keyword]->GetValue(ympVal,
				                               tmp,
				                               CBratProcess::OutSideModeKeywords,
				                               index,
				                               defaultValue);

  return static_cast<OutsideMode>(ympVal);
}

//----------------------------------------
std::string CBratProcess::OutsideModeStr(CBratProcess::OutsideMode	mode)
{
  switch (mode)
  {
    case CBratProcess::pctSTRICT:	return "STRICT";
    case CBratProcess::pctRELAXED:  	return "RELAXED";
    case CBratProcess::pctBLACK_HOLE:	return "BLACK_HOLE";
    default:
	    throw CException(CTools::Format("PROGRAM ERROR: outside mode %d unknown",
					    mode),
			     BRATHL_LOGIC_ERROR);
  }
}
//----------------------------------------
bool CBratProcess::CheckOutsideValue
		                (double		&value,
		                 double		min,
		                 double		max,
		                 double		step,
		                 CBratProcess::OutsideMode	mode)
{
  switch (mode)
  {
    //---------------
    case CBratProcess::pctSTRICT:
    //---------------
		  if (CTools::Compare(value, min) < 0)
      {
        return true;
      }
		  if (CTools::Compare(value, max) > 0) 
      {
        return true;
      }
		  break;

      //---------------
    case CBratProcess::pctRELAXED:
    //---------------
		  if (CTools::Compare(value, min - step / 2.0) < 0) 
      {
        return true;
      }
		  if (CTools::Compare(value, max + step / 2.0) > 0)
      {
        return true;
      }
		  break;

    //---------------
    case CBratProcess::pctBLACK_HOLE:
    //---------------
		  if (CTools::Compare(value, min) < 0)
      {
		    value = min;
      }
		  else if (CTools::Compare(value, max) < 0)
      {
		    value	= max;
      }
		  break;

    //---------------
    default:
    //---------------
	    throw CException(CTools::Format("PROGRAM ERROR: outside mode %d unknown",
					    mode),
			     BRATHL_LOGIC_ERROR);
  }

  return false;
}

//----------------------------------------
CBratProcess::PositionMode CBratProcess::GetPositionMode
		(CFileParams	&params,
		 int32_t	minOccurences	/*= 0*/,
		 int32_t	maxOccurences	/*= 1*/,
		 const std::string	&keyword	/*= "POSITION_MODE"*/,
		 int32_t	index		/*= 0*/,
		 CBratProcess::PositionMode	defaultValue		/*= CBratProcess::pctNEAREST*/)
{
  if (params.CheckCount(keyword, minOccurences, maxOccurences) == 0)
  {
    return defaultValue;
  }

  std::string	tmp;
  uint32_t	ympVal;
  
  params.m_mapParam[keyword]->GetValue(ympVal,
				       tmp,
				       CBratProcess::PositionModeKeywords,
				       index,
				       defaultValue);

  return static_cast<PositionMode>(ympVal);
}
//----------------------------------------
std::string CBratProcess::PositionModeStr(CBratProcess::PositionMode	mode)
{
  switch (mode)
  {
    case CBratProcess::pctEXACT:	return "EXACT";
    case CBratProcess::pctNEAREST:	return "NEAREST";
    default:
	    throw CException(CTools::Format("PROGRAM ERROR: position mode %d unknown",
					    mode),
			     BRATHL_LOGIC_ERROR);
  }
}
//----------------------------------------
bool CBratProcess::CheckPositionValue
		(double		value,
		 double		min,
		 double		step,
		 uint32_t	count,
		 CBratProcess::PositionMode	mode,
		 uint32_t	&position)
{
  double pos;
  double posInt;

  pos		= (value - min) / step;
  posInt	= CTools::Int(CTools::Round(pos));

  switch (mode)
  {
    //---------------
    case CBratProcess::pctEXACT:
    //---------------
		  if (CTools::Compare(pos, posInt) != 0)
      {
        return true;
      }
		  break;

    //---------------
    case CBratProcess::pctNEAREST:
    //---------------
  		break;
    //---------------
    default:
    //---------------
	    throw CException(CTools::Format("PROGRAM ERROR: position mode %d unknown",
					    mode),
			     BRATHL_LOGIC_ERROR);
  }

  if (posInt < 0.0)
  {
    return true;
  }
  if (posInt >= count)
  {
    return true;
  }

  position	= static_cast<uint32_t>(posInt);
  
  return false;
}

//----------------------------------------
double CBratProcess::CheckLongitudeValue
		(double			value,
		 double			min,
		 NetCDFVarKind		kind)
{
  if (kind != Longitude)
  {
    return value;
  }

  return CTools::NormalizeLongitude(min, value);
}
//----------------------------------------
bool CBratProcess::IsLongitudeCircular
		(double			min,
		 double			max,
		 NetCDFVarKind		kind,
     CUnit* unit,
     double compareEpsilon /* = CTools::m_CompareEpsilon */)
{
  if (kind != Longitude)
  {
    return false;
  }

  double minBaseUnit = min;
  double maxBaseUnit = max;

  if (unit != NULL)
  {
    minBaseUnit = CUnit::ConvertToBaseUnit(unit, min);
    maxBaseUnit = CUnit::ConvertToBaseUnit(unit, max);
  }

  return CTools::IsLongitudeCircular(minBaseUnit, maxBaseUnit, compareEpsilon);
}

//----------------------------------------
void CBratProcess::BeforeBuildListFieldsToRead()
{

}
//----------------------------------------
void CBratProcess::AfterBuildListFieldsToRead()
{

}
//----------------------------------------
void CBratProcess::NetCdfProductInitialization()
{
  if (! CBratProcess::IsProductNetCdf() )
  {
    return;
  }

  CProductNetCdf* productNetCdf = GetProductNetCdf(m_product);
  
  productNetCdf->SetAxisDims(m_netCdfAxisDims);

  productNetCdf->SetComplementDims(m_netCdfComplementDims);

  //if (m_fieldWithNoXYCommonDim.size() > 0)
  //{
  //  productNetCdf->AddDimsToReadOneByOne(m_fieldWithNoXYCommonDim);
  //  productNetCdf->SetForceReadDataOneByOne(true);
  //}

  
//  AddCritSelectionComplementDimensionsFromNetCdf();

}

//----------------------------------------
void CBratProcess::AddCritSelectionComplementDimensionsFromNetCdf()
{
  // Applies only to Netcdf products
  if (! CBratProcess::IsProductNetCdf() )
  {
   return;
  }
  CProductNetCdf* productNetCdf = GetProductNetCdf(m_product);

  CStringArray selectDims;
  
  productNetCdf->GetNetCdfDimensions(m_select, selectDims, m_recordName);

  //selectDims.Dump(*(CTrace::GetInstance()->GetDumpContext()));
 
  CStringArray dimsToReadOneByOne;
  
  CStringArray selectDimsWithoutAxes;
  m_netCdfAxisDims.Complement(selectDims, selectDimsWithoutAxes);
  
  //selectDimsWithoutAxes.Dump(*(CTrace::GetInstance()->GetDumpContext()));

  CStringArray* productNetCdfComplementDims = productNetCdf->GetComplementDims();
  //productNetCdfComplementDims->Dump(*(CTrace::GetInstance()->GetDumpContext()));

  CStringArray selectDimsComplement;
  productNetCdfComplementDims->Complement(selectDimsWithoutAxes, selectDimsComplement);

  // Some dimensions in selection criteria expression are not in all other expressions
  // Adds them to complement dimension
  // Reads them one by one 
  //selectDimsComplement.Dump(*(CTrace::GetInstance()->GetDumpContext()));

  productNetCdfComplementDims->Insert(selectDimsComplement);
  //productNetCdfComplementDims->Dump(*(CTrace::GetInstance()->GetDumpContext()));

  productNetCdf->AddDimsToReadOneByOne(selectDimsComplement);

  productNetCdf->AddDimsToReadOneByOne(selectDimsWithoutAxes);

  //if (selectDimsWithoutAxes.size() > 0)
  //{
  //  productNetCdf->SetForceReadDataOneByOne(true);
  //}
  //productNetCdf->GetDimsToReadOneByOne()->Dump(*(CTrace::GetInstance()->GetDumpContext()));
}
//----------------------------------------
void CBratProcess::ConstructProduct(const std::string& fileName, bool createVirtualField /* = true */)
{
  CStringArray stringArray;
  stringArray.Insert(fileName);
  
  ConstructProduct(stringArray, createVirtualField); 

}
//----------------------------------------
void CBratProcess::ConstructProduct(CStringArray& fileName, bool createVirtualField /* = true */)
{
  DeleteProduct();

  m_product = CProduct::Construct(fileName);

  if (m_product == NULL)
  {
    return;
  }

  m_product->SetFieldSpecificUnits(m_fieldSpecificUnit);

  m_product->SetCreateVirtualField(createVirtualField);
}
//----------------------------------------
void CBratProcess::ConstructProduct(CStringList& fileName, bool createVirtualField /* = true */)
{
  CStringArray stringArray;
  stringArray.Insert(fileName);
  
  ConstructProduct(stringArray, createVirtualField); 
}
//----------------------------------------
bool CBratProcess::Initialize(std::string& msg)
{
  CTrace *p = CTrace::GetInstance();

  // Register Brat algorithms
  CBratAlgorithmBase::RegisterAlgorithms();

  // Load aliases dictionnary
  std::string errorMsg;
  CAliasesDictionary::LoadAliasesDictionary(&errorMsg, false);
  if (!(errorMsg.empty())) 
  {
    std::string msg = CTools::Format("WARNING: %s",  errorMsg.c_str());
    p->Tracer(1, msg);
  }
  
  GetParameters();
  
  //GetFieldWithNoXYCommonDim();
  
  //m_fieldWithNoXYCommonDim.Dump(*(CTrace::GetInstance()->GetDumpContext()));

  ReplaceAxisDefinition();

  BeforeBuildListFieldsToRead();
  BuildListFieldsToRead();
  AfterBuildListFieldsToRead();

  ConstructProduct(m_inputFiles, true);

  // if no field in Select expression, test if it is always true or always false
  if (!m_select.HasFieldNames())
  {
    
    // And get the result
    CExpressionValue exprValueSelect = m_select.Execute(m_product);
    
    if (exprValueSelect.IsTrue() == 1)
    {
      m_alwaysTrue = true;
    }
    else
    {
      m_alwaysFalse = true;
      msg.append("======> 'Selection' expression (SELECT parameter) is always 'false'. No data to select." 
                     "To select data, you have to change 'Selection expression'. <======\n");
      return false;

    }
  }
  return true;

}

//----------------------------------------
void CBratProcess::ReplaceFieldDefinition()
{
  if (! CBratProcess::IsProductNetCdf() )
  {
    return;
  }
  
  CProductNetCdf* productNetCdf = GetProductNetCdf(m_product);

  CStringArray::const_iterator itField;

  for (itField = m_fieldDefinitionToReplace.begin() ; itField != m_fieldDefinitionToReplace.end() ; itField++)
  {
    CUIntArray dimValues;
    CIntArray dimIds;
    CStringArray dimNames;

    CStringArray::const_iterator itAxis;

    for (itAxis = m_netCdfAxisDims.begin() ; itAxis != m_netCdfAxisDims.end() ; itAxis++)
    {
      CFieldNetCdf* axis = dynamic_cast<CFieldNetCdf*>(productNetCdf->FindFieldByName(*itAxis, m_recordName, false));
      if (axis == NULL)
      {
        continue;
      }
      
      dimNames.Insert(axis->GetDimNames());
      axis->GetDimValuesAsArray(dimValues, false);
      axis->GetDimIdsAsArray(dimIds, false);
    }

    CFieldNetCdf* fieldFromDictionnay = dynamic_cast<CFieldNetCdf*>(productNetCdf->FindFieldByName(*itField, m_recordName, false));

    if (fieldFromDictionnay != NULL)
    {
      fieldFromDictionnay->SetDim(dimValues);
      fieldFromDictionnay->SetDimInfo(dimNames, dimIds, dimValues);
    }

    CExternalFilesNetCDF* externalFile = productNetCdf->GetExternalFile();
    
    if (externalFile == NULL)
    {
      continue;
    }

    CFieldNetCdf* fieldFromFile = externalFile->GetFieldNetCdf(*itField, false);
    
    if (fieldFromFile != NULL)
    {
      fieldFromFile->SetDim(dimValues);
      fieldFromFile->SetDimInfo(dimNames, dimIds, dimValues);
    }


  }

}
//----------------------------------------
void CBratProcess::ReplaceAxisDefinition()
{
  if (! CBratProcess::IsProductNetCdf() )
  {
    return;
  }
  

  SetDimCoordAxesToFieldWithoutDim(m_xField);
  SetDimCoordAxesToFieldWithoutDim(m_yField);
 
}

//----------------------------------------
void CBratProcess::SetDimCoordAxesToFieldWithoutDim(const CExpression& expr)
{
  SetDimCoordAxesToFieldWithoutDim(*(expr.GetFieldNames()));
}
//----------------------------------------
void CBratProcess::SetDimCoordAxesToFieldWithoutDim(const CStringArray& fieldNames)
{
  if (m_product == NULL)
  {
      LoadProductDictionary(*(m_inputFiles.begin()));
  }

  
  if (! CBratProcess::IsProductNetCdf() )
  {
    return;
  }

  if (fieldNames.size() <= 0)
  {
    return;
  }


  CProductNetCdf* productNetCdf = GetProductNetCdf(m_product);

  
  CExternalFilesNetCDF* externalFile = productNetCdf->GetExternalFile();
  
  if (externalFile == NULL)
  {
    return;
  }

  CNetCDFFiles* file = externalFile->GetFile();

  if (file == NULL)
  {
    return;
  }

  
  CStringArray fieldDimensions;
  productNetCdf->GetNetCdfDimensions(fieldNames, fieldDimensions, m_recordName);

  // Result of the expression has dimensions then return
  if (fieldDimensions.size() > 0)
  {
    return;
  }
  
  // Dimensions of the reuslt of the expression is empty, it's considered as a constant expression (an expression 
  // which is made up of one  or several constants):
  // So, the field dimensions of the expression will be the coordinate axes dimensions of the file 

  
  CStringArray coordAxesNames;

  //file->GetNetCDFCoordAxes(&coordAxesNames);

  GetOrderedDimNames(m_fields, coordAxesNames);
  
  if (coordAxesNames.size() <= 0)
  {
    GetOrderedDimNames(m_xField, coordAxesNames);
  }

  if (coordAxesNames.size() <= 0)
  {
    GetOrderedDimNames(m_yField, coordAxesNames);
  }
  


  CStringArray::const_iterator itField;
  
  for (itField = fieldNames.begin() ; itField != fieldNames.end() ; itField++)
  {
    CUIntArray dimValues;
    CIntArray dimIds;
    CStringArray dimNames;

    CStringArray::const_iterator itCoordAxis;

    for (itCoordAxis = coordAxesNames.begin() ; itCoordAxis != coordAxesNames.end() ; itCoordAxis++)
    {

      CFieldNetCdf* axis = dynamic_cast<CFieldNetCdf*>(productNetCdf->FindFieldByName(*itCoordAxis, m_recordName, false));
      if (axis == NULL)
      {
        continue;
      }

      dimNames.Insert(axis->GetDimNames());
      axis->GetDimValuesAsArray(dimValues, false);
      axis->GetDimIdsAsArray(dimIds, false);

    }

    CFieldNetCdf* fieldFromDictionnay = dynamic_cast<CFieldNetCdf*>(productNetCdf->FindFieldByName(*itField, m_recordName, false));

    if ((fieldFromDictionnay != NULL) && (!fieldFromDictionnay->HasDim()))
    {
        fieldFromDictionnay->SetDim(dimValues);
        fieldFromDictionnay->SetDimInfo(dimNames, dimIds, dimValues);
    }


    CFieldNetCdf* fieldFromFile = externalFile->GetFieldNetCdf(*itField, false);

    if ((fieldFromFile != NULL) && (!fieldFromFile->HasDim()))
    {
      fieldFromFile->SetDim(dimValues);
      fieldFromFile->SetDimInfo(dimNames, dimIds, dimValues);
    }


  }  

}
//----------------------------------------
void CBratProcess::GetFieldWithNoXYCommonDim()
{
  if (! CBratProcess::IsProductNetCdf() )
  {
    return;
  }

  CStringArray coordAxesNames;
  GetOrderedDimNames(m_xField, coordAxesNames);
  GetOrderedDimNames(m_yField, coordAxesNames);

  CStringArray dimNames;

  size_t nbExpr = m_fields.size();
  
  for (uint32_t i = 0 ; i < nbExpr ; i++)
  {
    const CStringArray* fieldNames = m_fields.at(i).GetFieldNames();
    CStringArray::const_iterator itFiledName;

    for (itFiledName =  fieldNames->begin() ; itFiledName != fieldNames->end() ;  itFiledName++)
    {
      dimNames.RemoveAll();
      GetOrderedDimNames(*itFiledName, dimNames);

      CStringArray intersect;
      coordAxesNames.Intersect(dimNames, intersect);
      if (intersect.size() <= 0)
      {
        m_fieldWithNoXYCommonDim.InsertUnique(*itFiledName);
      }
    }

  }
  
}
//----------------------------------------
void CBratProcess::BuildListFieldsToRead()
{
  // get original number of expresion, because AddFielIndexes can add new expression (insert at the end of m_fields) 
  size_t nbExpr = m_fields.size();

  if (m_product == NULL)
  {
    LoadProductDictionary(*(m_inputFiles.begin()));
  }

  // build, from FIELD parameter, the list of the fields to read in the data files
  for (uint32_t i = 0 ; i < nbExpr ; i++)
  {
    m_listFieldsToRead.InsertUnique(m_fields.at(i).GetFieldNames());


    CUIntArray commonDimensions;
    CheckDimensions(m_fields.at(i), commonDimensions);
    
    //CProductNetCdf* productNetCdf = dynamic_cast<CProductNetCdf*>(m_product);

    if (! CBratProcess::IsProductNetCdf() )
    {
      CNetCDFVarDef varDef(m_names[i], m_units[i]);
      varDef.AddAttribute(new CNetCDFAttrString(LONG_NAME_ATTR, m_titles[i]));
      varDef.AddAttribute(new CNetCDFAttrString(COMMENT_ATTR, m_comments[i]));

      AddFieldIndexes(i, &varDef, commonDimensions);
    }
    /*
    else
    {
      CStringArray fields;
      CheckNetCdfFields(fields);
      
      AddDimensionsFromNetCdf(fields);

    }
    */
  }

  // build, from SELECT parameter, the list of the fields to read in the data files
  m_listFieldsToRead.InsertUnique(m_select.GetFieldNames());

}

//----------------------------------------
std::string CBratProcess::GetExpressionNewName(const std::string& name)
{
  int32_t i = 1;
  std::string str;

  if (! m_names.Exists(name) )
  {
    return name;
  }

  std::string format = name;
  format.append("_%d");

  do
  {
    str = CTools::Format(format.c_str(), i);
    i++;
  }
  while (m_names.Exists(str.c_str()));
  
  return str;
}

//----------------------------------------
CExternalFilesNetCDF* CBratProcess::OpenExternalFilesNetCDF(const std::string& fileName)
{
  CExternalFilesNetCDF* externalFile = NULL;
  
  try
  {
    // Test if file is a Netcdf file
    externalFile = dynamic_cast<CExternalFilesNetCDF*>(BuildExistingExternalFileKind(fileName));

    if (externalFile == NULL)
    {
      throw CFileException(CTools::Format("CBratProcess::OpenCExternalFilesNetCDF - Unknown product/unknown netcdf type - File %s", 
                        fileName.c_str()), BRATHL_IO_ERROR);
    }
  }
  catch(CException &e)
  {
      throw CFileException(CTools::Format("CBratProcess::OpenCExternalFilesNetCDF - Unknown product/unknown netcdf type - File %s. Reason: %s", 
                        fileName.c_str(), e.what()), BRATHL_IO_ERROR);
  }


  externalFile->Open();

  return externalFile;

}
//----------------------------------------
void CBratProcess::AddDimensionsFromNetCdf(CStringArray& dimNames)
{
  // Applies only to Netcdf products
  if (! CBratProcess::IsProductNetCdf() )
  {
   return;
  }

  CObMap mapDims(false);
  
  CExternalFilesNetCDF* externalFile1 = OpenExternalFilesNetCDF(*(m_inputFiles.begin()));
 

 // CNetCDFFiles file1(*(m_inputFiles.begin()));
  CNetCDFFiles* file1 = externalFile1->GetFile();
  CObMap* mapDims1 = file1->GetNetCDFDims();

  CStringArray::const_iterator it;
  for ( it = dimNames.begin( ); it != dimNames.end( ); it++ )
  {
    // Search if dim is in the input Netcdf file
    CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(mapDims1->Exists(*it));
    if (netCDFDim == NULL)
    {
      throw CException(CTools::Format("ERROR: CBratProcess::AddDimensionsFromNetCdf() - dimension '%s' not found in file '%s'\n",
                                    it->c_str(),
                                    file1->GetName().c_str()),
			               BRATHL_LOGIC_ERROR);
    }

    CFieldNetCdf* field = dynamic_cast<CFieldNetCdf*>(m_product->FindFieldByName(netCDFDim->GetName(), m_recordName, false));
    
    if (field == NULL)
    {
      throw CException(CTools::Format("ERROR: CBratProcess::AddDimensionsFromNetCdf() - field '%s' not found\n",
                                    netCDFDim->GetName().c_str()),
			               BRATHL_LOGIC_ERROR);
    }

    CUnit* unit = field->GetNetCdfUnit();    
    
    NetCDFVarKind dimKind = field->SearchDimKind();

    std::string title = field->GetAttribute(LONG_NAME_ATTR);
    
    std::string comment = field->GetAttribute(COMMENT_ATTR);


    // insert dimension in a map (for a later check)
    mapDims.Insert(*it, netCDFDim);
    
    // Insert the dimension to the output Netcdf file
    CNetCDFDimension* addedDim = m_internalFiles->AddNetCDFDim(*netCDFDim);
    addedDim->AddCoordinateVariable(netCDFDim->GetName());

    // Check if the inserted dimension has its variable in the output Netcdf file
    // If not, create the variable corresponding to the inserted dimension
    // and add it to in the output Netcdf file
    CNetCDFCoordinateAxis* addedVarDimIndex = dynamic_cast<CNetCDFCoordinateAxis*>(m_internalFiles->GetNetCDFVarDef(netCDFDim->GetName()));
    
    if (addedVarDimIndex == NULL)
    {
      CNetCDFCoordinateAxis varDimIndex(netCDFDim->GetName());

      addedVarDimIndex = dynamic_cast<CNetCDFCoordinateAxis*>(m_internalFiles->AddNetCDFVarDef(varDimIndex));
      
      addedVarDimIndex->AddAttribute(new CNetCDFAttrString(LONG_NAME_ATTR, title));
      addedVarDimIndex->AddAttribute(new CNetCDFAttrString(COMMENT_ATTR, comment));

      addedVarDimIndex->AddNetCDFDim(*addedDim);
      addedVarDimIndex->SetUnit(*unit);
      addedVarDimIndex->SetDimKind(dimKind);
      


    }

    // The dimension have to be added to the fields which have to be read  (as an expression object) 

    int32_t indexAlreadyExistName = m_names.FindIndex(netCDFDim->GetName());

    if (indexAlreadyExistName >= 0)
    {
      m_names[indexAlreadyExistName] = GetExpressionNewName(netCDFDim->GetName());
      CTrace::Tracer(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
      CTrace::Tracer(CTools::Format("WARNING - Expression '%s' have been renamed to '%s', because a Netcdf dimension has the same name", 
                     netCDFDim->GetName().c_str(),
                     m_names.at(indexAlreadyExistName).c_str()));
      CTrace::Tracer(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

    }


    CExpression expr(netCDFDim->GetName());


    m_fields.push_back(expr);

    m_units.push_back(*unit);

    m_names.push_back(netCDFDim->GetName());

    m_titles.push_back(title);

    m_comments.push_back(comment);

    m_types.push_back(Data);

    m_dataMode.push_back(CBratProcess::pctFIRST);

    //----------------------------------
    m_listFieldsToRead.InsertUnique(netCDFDim->GetName());
    //----------------------------------

    OnAddDimensionsFromNetCdf();

  }

  //------------------------
  // Checks Dimensions of the other files of the dataset
  //------------------------

  for ( it = m_inputFiles.begin() + 1 ; it != m_inputFiles.end( ); it++ )
  {
    CExternalFilesNetCDF* externalFileComp = OpenExternalFilesNetCDF(*it);

    CNetCDFFiles* fileComp = externalFileComp->GetFile();
    
    CObMap::const_iterator itDims;

    for ( itDims = mapDims.begin() ; itDims != mapDims.end( ); itDims++ )
    {
      std::string dimName = itDims->first;
      CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(itDims->second);
      if (netCDFDim == NULL)
      {
        throw CException(CTools::Format("ERROR: CBratProcess::AddDimensionsFromNetCdf() - dimension '%s' in map is NULL.\n",
                                      dimName.c_str()),
			                 BRATHL_LOGIC_ERROR);
      }

      CObMap* mapDimsComp = fileComp->GetNetCDFDims();

      CNetCDFDimension* netCDFDimComp = dynamic_cast<CNetCDFDimension*>(mapDimsComp->Exists(dimName));
      if (netCDFDimComp == NULL)
      {
        throw CException(CTools::Format("ERROR: CBratProcess::AddDimensionsFromNetCdf() - dimension '%s' not found in file '%s'\n",
                                      dimName.c_str(),
                                      fileComp->GetName().c_str()),
			                 BRATHL_LOGIC_ERROR);
      }

      if (netCDFDimComp->IsNotEqual(*netCDFDim))
      {
        CStringArray keys;
        mapDims.GetKeys(keys);

        std::string dimsAsString = keys.ToString(", ", false);

        throw CException(CTools::Format("ERROR: Unable to process. Reason: "
                                        "'%s' dimension value (%ld) in file '%s' is not equal to "
                                        "'%s' dimension value (%ld) in file '%s'\n"
                                        "Go through all the files of the dataset, check that each dimension below have the same value:\n%s",
                                      dimName.c_str(),
                                      (long)netCDFDimComp->GetLength(), 
                                      fileComp->GetName().c_str(),
                                      dimName.c_str(),
                                      (long)netCDFDim->GetLength(), 
                                      file1->GetName().c_str(),
                                      dimsAsString.c_str()),
			                 BRATHL_LOGIC_ERROR);
      }

      

    }

    if (externalFileComp != NULL)
    {
      delete externalFileComp;
      externalFileComp = NULL;
    }
  }

 
  if (externalFile1 != NULL)
  {
    delete externalFile1;
    externalFile1 = NULL;
  }


}
//----------------------------------------
void CBratProcess::OnAddDimensionsFromNetCdf()
{
  IncrementOffsetValues();

  m_countOffsets.push_back(-1);
  m_meanOffsets.push_back(-1);

  //-----------------
  m_nbDataAllocated++;
    //-----------------

}
//----------------------------------------
void CBratProcess::SubstituteAxisDim(const CStringArray& fieldDims, CStringArray& fieldDimsOut)
{
}
//----------------------------------------
void CBratProcess::AddVarsFromNetCdf()
{
  //Applies only to Netcdf products 
  if (! CBratProcess::IsProductNetCdf() )
  {
    return;
  }
  
  CProductNetCdf* productNetCdf = GetProductNetCdf(m_product);

  m_fieldDefinitionToReplace.RemoveAll();
  
  size_t nbExpr = m_fields.size();
  
  // Add each expression (as NetCdf variable) to the output Netcdf fle
  for (uint32_t i = 0 ; i < nbExpr ; i++)
  {
    CStringArray fieldDims;
    CStringArray fieldDimsWithoutAlgo;
    CStringArray fieldDimsOut;

    // Get Netcdf dimension of the expression (of all fields contained in the expression)
    // NB :dim. names appear only once in the result (fieldDims)
    // NB: if no algo in the expression : fieldDims and fieldDimsWithoutAlgo are equal
    productNetCdf->GetNetCdfDimensions(m_fields.at(i), fieldDims, m_recordName);
    productNetCdf->GetNetCdfDimensionsWithoutAlgo(m_fields.at(i), fieldDimsWithoutAlgo, m_recordName);
    

    // Replace dimension which have the same names of the X / Y fields by the X / Y expression name 
    //SubstituteAxisDim(fieldDims, fieldDimsOut);

    // If the dimensions of the expression is empty, it's considered as a constant expression (an expression 
    // which is made up of one  or several constants):
    // So, the ouput dimensions of the expression will be the axis dimensions (X or X/Y)
    if (fieldDims.size() <= 0)
    {
      //fieldDimsOut.Insert(m_netCdfAxisDims);
      if (!m_xName.empty())
      {
        fieldDimsOut.Insert(m_xName);
      }

      if (!m_yName.empty())
      {
        fieldDimsOut.Insert(m_yName);
      }
      
      CStringArray::const_iterator itField;
      const CStringArray& fields = *(m_fields.at(i).GetFieldNames());

      for (itField = fields.begin() ; itField != fields.end() ; itField++)
      {
        CFieldNetCdf* field = dynamic_cast<CFieldNetCdf*>(m_product->FindFieldByName(*itField, m_recordName, false));
        if (field == NULL)
        {
          continue;
        }

        m_fieldDefinitionToReplace.Insert(field->GetName());
      }

    }
    // If the dimensions of the expression withur algorithm call is empty
    // The ouput dimensions of the expression will be the axis dimensions (X or X/Y)
    else 
    {
      if (fieldDimsWithoutAlgo.size() <= 0) // else fieldDims.size() >= 0
      {
        fieldDimsOut.RemoveAll();

        if (!m_xName.empty())
        {
          fieldDimsOut.Insert(m_xName);
        }

        if (!m_yName.empty())
        {
          fieldDimsOut.Insert(m_yName);
        }

      }
      else
      {
        // Replace dimension which have the same names of the X / Y fields by the X / Y expression name 
        SubstituteAxisDim(fieldDimsWithoutAlgo, fieldDimsOut);
      }
    }

    // if var already exists, continue
    if (m_internalFiles->GetNetCDFVarDef(m_names[i]) != NULL)
    {
      //-----------
      continue;
      //-----------

    }
    //Create the variable and add it to the output NetCdf file
    CNetCDFVarDef varDef(m_names[i], m_units[i]);
    varDef.AddAttribute(new CNetCDFAttrString(LONG_NAME_ATTR, m_titles[i]));
    varDef.AddAttribute(new CNetCDFAttrString(COMMENT_ATTR, m_comments[i]));

    // The created var is returned (or the already existing var, i.e the dim variable).
    CNetCDFVarDef* addedVarDef = m_internalFiles->AddNetCDFVarDef(varDef);

    /*
    CNetCDFCoordinateAxis* coordAxis = dynamic_cast<CNetCDFCoordinateAxis*>(coordAxis);

    // If var is a dimension : (don't add to its own dimension, it have already done)
    if (coordAxis != NULL)
    {
      //-----------
      continue
      //-----------
    }
  */
    // Add dimension to the variable 
    
    CStringArray::const_iterator itFieldDimsOut;
    
    for (itFieldDimsOut = fieldDimsOut.begin() ; itFieldDimsOut != fieldDimsOut.end() ; itFieldDimsOut++)
    {
      CNetCDFDimension* addedDim = m_internalFiles->GetNetCDFDim(*itFieldDimsOut);
      // If dim is not found, it's an axis field ==> create the dim, set its length to 1 and create its variable.
      // because its length is actually not known (it will be known when all the input file will be processed)
      if (addedDim == NULL)
      {
        CNetCDFDimension dim(*itFieldDimsOut, 1);

        addedDim = m_internalFiles->AddNetCDFDim(dim);

        CNetCDFVarDef* addedVarDimIndex = m_internalFiles->GetNetCDFVarDef(addedDim->GetName());
        if (addedVarDimIndex == NULL)
        {
          CNetCDFCoordinateAxis varDimIndex(addedDim->GetName());

          addedVarDimIndex = m_internalFiles->AddNetCDFVarDef(varDimIndex);
      
          addedDim->AddCoordinateVariable(addedVarDimIndex->GetName());
          
          addedVarDimIndex->AddNetCDFDim(*addedDim);

          // The dimension have to be added to the fields which have to be read  (as an expression object) 

          bool addAsExpression = (m_names.FindIndex(addedVarDimIndex->GetName()) < 0)
                              && ( ! IsOutputAxis(addedVarDimIndex->GetName()) );

          if (addAsExpression)
          {

            CExpression expr(addedVarDimIndex->GetName());

            m_fields.push_back(expr);

            m_units.push_back(*(addedVarDimIndex->GetUnit()));

            m_names.push_back(addedVarDimIndex->GetName());

            m_titles.push_back("");

            m_comments.push_back("");

            m_types.push_back(Data);

            m_dataMode.push_back(CBratProcess::pctFIRST);

            //----------------------------------
            m_listFieldsToRead.InsertUnique(addedVarDimIndex->GetName());
            //----------------------------------

            OnAddDimensionsFromNetCdf();
          }

        }

      }
        
      addedDim->AddCoordinateVariable(addedVarDef->GetName());

      addedVarDef->AddNetCDFDim(*addedDim);
    }


  }


}

//----------------------------------------
void CBratProcess::IncrementOffsetValues(uint32_t incr /* = 1 */)
{
  IncrementValue(m_countOffsets, incr);
  IncrementValue(m_meanOffsets, incr);
}
//----------------------------------------
void CBratProcess::IncrementValue(CIntArray& vect, uint32_t incr /* = 1 */)
{
  CIntArray::iterator it;
 
  for (it = vect.begin() ; it != vect.end() ; it++)
  {
    if ((*it) > 0)
    {
      (*it) = (*it) + incr;     
    }
  }

  
}

//----------------------------------------
bool CBratProcess::IsProductNetCdf(CBratObject* ob)
{
  return (CBratProcess::GetProductNetCdf(ob, false) != NULL);
}
//----------------------------------------
bool CBratProcess::IsProductNetCdfCF(CBratObject* ob)
{
  return (CBratProcess::GetProductNetCdfCF(ob, false) != NULL);
}
//----------------------------------------
CProductNetCdf* CBratProcess::GetProductNetCdf(CBratObject* ob, bool withExcept /*= true*/)
{
  CProductNetCdf* productNetCdf = dynamic_cast<CProductNetCdf*>(ob);
  if (withExcept)
  {
    if (productNetCdf == NULL)
    {
      CException e("CBratProcess::GetProductNetCdf - dynamic_cast<CProductNetCdf*>(ob) returns NULL"
                   "object seems not to be an instance of CProductNetCdf",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return productNetCdf;

}

//----------------------------------------
CProductNetCdfCF* CBratProcess::GetProductNetCdfCF(CBratObject* ob, bool withExcept /*= true*/)
{
  CProductNetCdfCF* productNetCdfCF = dynamic_cast<CProductNetCdfCF*>(ob);
  if (withExcept)
  {
    if (productNetCdfCF == NULL)
    {
      CException e("CBratProcess::GetProductNetCdfCF - dynamic_cast<productNetCdfCF*>(ob) returns NULL"
                   "object seems not to be an instance of CProductNetCdfCF",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return productNetCdfCF;

}
//----------------------------------------
std::string CBratProcess::GetFieldSpecificUnit(const std::string& key)
{
  return m_fieldSpecificUnit.Exists(key); 
}
//----------------------------------------
void CBratProcess::SetFieldSpecificUnit(const std::string& key, const std::string& value)
{
  m_fieldSpecificUnit.Erase(key);
  m_fieldSpecificUnit.Insert(key, value, false);
  
}
//----------------------------------------
void CBratProcess::SetFieldSpecificUnits(const CStringMap& fieldSpecificUnit)
{
  m_fieldSpecificUnit.RemoveAll();
  m_fieldSpecificUnit.Insert(fieldSpecificUnit);
  
}
//----------------------------------------
void CBratProcess::SetFieldSpecificUnits(const CStringMap* fieldSpecificUnit)
{
  if (fieldSpecificUnit == NULL)
  {
    return;
  }

  SetFieldSpecificUnits(*fieldSpecificUnit);
  
}

//----------------------------------------
CObArrayOb* CBratProcess::GetObArrayOb(CBratObject* ob, bool withExcept /*= true*/)
{
  CObArrayOb* array = dynamic_cast<CObArrayOb*>(ob);
  if (withExcept)
  {
    if (array == NULL)
    {
      CException e("CBratProcess::GetObArrayOb - dynamic_cast<CObArrayOb*>(ob) returns NULL"
                   "object seems not to be an instance of CObArrayOb",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return array;

}
//----------------------------------------
CDoubleArrayOb* CBratProcess::GetDoubleArrayOb(CBratObject* ob, bool withExcept /*= true*/)
{
  CDoubleArrayOb* array = dynamic_cast<CDoubleArrayOb*>(ob);
  if (withExcept)
  {
    if (array == NULL)
    {
      CException e("CBratProcess::GetDoubleArrayOb - dynamic_cast<CDoubleArrayOb*>(ob) returns NULL"
                   "object seems not to be an instance of CDoubleArrayOb",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return array;

}

//----------------------------------------
void CBratProcess::MergeDataValue
		(double* data,
		 double* values,
		 uint32_t nbValues,
     uint32_t indexExpr,
		 double* countValues,
		 double* meanValues)
{

  if (nbValues == 0)
  {
    double* countValue = ((countValues != NULL) ? (&countValues[0]) : NULL);
    double* meanValue = ((meanValues != NULL) ? (&meanValues[0]) : NULL);
    
    MergeDataValue(data[0], CTools::m_defaultValueDOUBLE, countValue, meanValue, m_dataMode[indexExpr]);
    return;
  }


  uint32_t indexValues = 0;

  for (indexValues = 0 ; indexValues < nbValues; indexValues++)
  {
    double value = CBratProcess::CheckLongitudeValue(values[indexValues], -180.0, m_types[indexExpr]);
    double* countValue = ((countValues != NULL) ? (&countValues[0]) : NULL);
    double* meanValue = ((meanValues != NULL) ? (&meanValues[0]) : NULL);
    
    CBratProcess::MergeDataValue(data[indexValues], value, countValue, meanValue, m_dataMode[indexExpr]);
  }

}

//----------------------------------------
void CBratProcess::MergeDataValue
		(double& data,
		 double value,
		 uint32_t nbValues,
     uint32_t indexExpr,
		 double* countValue,
		 double* meanValue)
{

  double valueTmp = CBratProcess::CheckLongitudeValue(value, -180.0, m_types[indexExpr]);
  CBratProcess::MergeDataValue(data, valueTmp, countValue, meanValue, m_dataMode[indexExpr]);


}




//----------------------------------------
/*
** Add a new value to data and computes the partial result
** according to the operation asked
*/
//----------------------------------------
void CBratProcess::MergeDataValue
		(double& data,
		 double value,
		 double*	countValue,
		 double*	meanValue,
		 CBratProcess::MergeDataMode	mode)
{
  if (isDefaultValue(value))
  {
    return;
  }

  double dummy = 0.0;

  switch (mode)
  {
    //--------------------------
    case CBratProcess::pctFIRST:
    //--------------------------
	    if (data == CBratProcess::MergeIdentifyUnsetData)
      {
	      data	= value;
      }
	    break;
    //--------------------------
    case CBratProcess::pctLAST:
    //--------------------------
      data	= value;
      /*
	    if (! isDefaultValue(value))
      {
	      data	= value;
      }
      */
	    break;
    //--------------------------
    case CBratProcess::pctMIN:
    //--------------------------
	    if (data == CBratProcess::MergeIdentifyUnsetData)
      {
	      data	= value;
      }
	    else
      {
	      data	= CTools::Min(data, value);
      }
	    break;
    //--------------------------
    case CBratProcess::pctMAX:
    //--------------------------
	    if (data == CBratProcess::MergeIdentifyUnsetData)
      {
	      data	= value;
      }
	    else
      {
	      data	= CTools::Max(data, value);
      }
	    break;
    //--------------------------
    case CBratProcess::pctSUM:
    //--------------------------
	    if (data == CBratProcess::MergeIdentifyUnsetData)
      {
	      data	= value;
      }
	    else
      {
	      data	= CTools::Plus(data, value);
      }
	    break;
    //--------------------------
    case CBratProcess::pctSUBSTRACT:
    //--------------------------
	    if (data == CBratProcess::MergeIdentifyUnsetData)
      {
	      data	= value;
      }
	    else
      {
	      data	= CTools::Minus(data, value);
      }
	    break;
    //--------------------------
    case CBratProcess::pctPRODUCT:
    //--------------------------
	    if (data == CBratProcess::MergeIdentifyUnsetData)
      {
	      data	= value;
      }
	    else
      {
	      data	= CTools::Multiply(data, value);
      }
	    break;
    //--------------------------
    case CBratProcess::pctMEAN:
    //--------------------------
      if (countValue == NULL)
      {
        throw CException(CTools::Format("ERROR: CBratProcessZFXY::MergeDataValue() - count value  is NULL, but mode is '%s'\n",
                                        CBratProcess::DataModeStr(mode).c_str()),
			                   BRATHL_LOGIC_ERROR);

      }

	    if (data == CBratProcess::MergeIdentifyUnsetData)
	    {
	      *countValue = 0;
	    }

	    CTools::DoIncrementalStats(value,
				                         *countValue,
				                         data,
				                         dummy,
				                         dummy,
				                         dummy);
	    break;
    //--------------------------
    case CBratProcess::pctSTDDEV:
    //--------------------------
      if (countValue == NULL)
      {
        throw CException(CTools::Format("ERROR: CBratProcessZFXY::MergeDataValue() - count value  is NULL, but mode is '%s'\n",
                                        CBratProcess::DataModeStr(mode).c_str()),
			                   BRATHL_LOGIC_ERROR);

      }
      if (meanValue == NULL)
      {
        throw CException(CTools::Format("ERROR: CBratProcessZFXY::MergeDataValue() - mean value  is NULL, but mode is '%s'\n",
                                        CBratProcess::DataModeStr(mode).c_str()),
			                   BRATHL_LOGIC_ERROR);

      }

	    if (data == CBratProcess::MergeIdentifyUnsetData)
	    {
	      *countValue = 0;
	    }

	    CTools::DoIncrementalStats(value,
				                         *countValue,
				                         *meanValue,
				                         data,
				                         dummy,
				                         dummy);
	    break;
    //--------------------------
    case CBratProcess::pctCOUNT:
    //--------------------------
	    data	= (data == CBratProcess::MergeIdentifyUnsetData ? 1.0 : data + 1);
	    break;
    //--------------------------
    default:
    //--------------------------
	    throw CException(CTools::Format("PROGRAM ERROR: DataMode %d unknown (CBratProcessZFXY::MergeDataValue)",
					    mode),
			     BRATHL_LOGIC_ERROR);
  }
}


//----------------------------------------
void CBratProcess::FinalizeMergingOfDataValues
		(double* data,
     uint32_t indexExpr,
		 uint32_t nbValues,
		 double* countValues,
		 double* meanValues)

{
  uint32_t indexValues = 0;

  for (indexValues = 0 ; indexValues < nbValues; indexValues++)
  {
    double* countValue = ((countValues != NULL) ? (&countValues[0]) : NULL);
    double* meanValue = ((meanValues != NULL) ? (&meanValues[0]) : NULL);
    
    CBratProcess::FinalizeMergingOfDataValues(data[indexValues], countValue, meanValue, m_dataMode[indexExpr]);
  }

}

//----------------------------------------

/*
** All values for data have been integrated. Computes
** the final value.
*/
void CBratProcess::FinalizeMergingOfDataValues
		(double& data,
		 double*	countValue,
		 double*	meanValue,
		 CBratProcess::MergeDataMode	mode)
{
  double dummy	= 0.0;

  switch (mode)
  {
    //--------------------------
    case CBratProcess::pctFIRST:
    case CBratProcess::pctLAST:
    case CBratProcess::pctMIN:
    case CBratProcess::pctMAX:
    case CBratProcess::pctCOUNT:
    case CBratProcess::pctSUM:
    case CBratProcess::pctSUBSTRACT:
    case CBratProcess::pctPRODUCT:
    //--------------------------
	    if (data == CBratProcess::MergeIdentifyUnsetData)
	    {
	      setDefaultValue(data);
	    }
	    break;
    //--------------------------
    case CBratProcess::pctMEAN:
    //--------------------------
	    if (data == CBratProcess::MergeIdentifyUnsetData)
	    {
	      setDefaultValue(data);
	    }
	    else
	    {

        if (countValue == NULL)
        {
          throw CException(CTools::Format("ERROR: CBratProcessZFXY::FinalizeMergingOfDataValues() - count value  is NULL, but mode is '%s'\n",
                                          CBratProcess::DataModeStr(mode).c_str()),
			                     BRATHL_LOGIC_ERROR);

        }


	      CTools::FinalizeIncrementalStats(*countValue,
					                               data,
					                               dummy,
					                               dummy,
					                               dummy);
	    }
	    break;
    //--------------------------
    case CBratProcess::pctSTDDEV:
    //--------------------------
	    if (data == CBratProcess::MergeIdentifyUnsetData)
	    {
	      setDefaultValue(data);
	    }
	    else
	    {
        if (countValue == NULL)
        {
          throw CException(CTools::Format("ERROR: CBratProcessZFXY::FinalizeMergingOfDataValues() - count value  is NULL, but mode is '%s'\n",
                                          CBratProcess::DataModeStr(mode).c_str()),
			                     BRATHL_LOGIC_ERROR);

        }
        if (meanValue == NULL)
        {
          throw CException(CTools::Format("ERROR: CBratProcessZFXY::MergeDataValue() - mean value  is NULL, but mode is '%s'\n",
                                          CBratProcess::DataModeStr(mode).c_str()),
			                     BRATHL_LOGIC_ERROR);

        }

	      CTools::FinalizeIncrementalStats(*countValue,
					                               *meanValue,
					                               data,
					                               dummy,
					                               dummy);
	    }
	    break;
    default:
	    throw CException(CTools::Format("PROGRAM ERROR: DataMode %d unknown (CBratProcessZFXY::MergeDataValue)",
					    mode),
			     BRATHL_LOGIC_ERROR);
  }
}





}
