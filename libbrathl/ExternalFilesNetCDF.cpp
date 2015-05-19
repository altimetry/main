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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	 02110-1301, USA.
*/

#include <algorithm>
#include "Stl.h"

#include "brathl.h"

#include "Tools.h"
#include "Trace.h"
#include "Date.h"
#include "Exception.h"
#include "Expression.h"

#include "ExternalFilesNetCDF.h"
// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;


namespace brathl
{

//-------------------------------------------------------------
//------------------- CExternalFilesNetCDF class --------------------
//-------------------------------------------------------------


CExternalFilesNetCDF::CExternalFilesNetCDF
		(const string	&Name	/*= ""*/)
	: CExternalFiles()
{
  SetProductClass(NETCDF_PRODUCT_CLASS);

  SetName(Name);
}

//----------------------------------------
CExternalFilesNetCDF::~CExternalFilesNetCDF()
{
}

//----------------------------------------
string CExternalFilesNetCDF::GetName
		() const
{
  return m_file.GetName();
}

//----------------------------------------
void CExternalFilesNetCDF::SetName(const string	&name)
{
  m_file.SetName(name);
}
//----------------------------------------
void CExternalFilesNetCDF::SetMode(brathl_FileMode	mode)
{
  m_file.SetMode(mode);
}

//----------------------------------------
void CExternalFilesNetCDF::CheckVariables()
{
  CStringArray		varNames;
  CStringArray::iterator	index;

  m_file.GetVariables(varNames);
  for (index = varNames.begin(); index != varNames.end(); index++)
  {
    AddVar(*index);
  }
}
//----------------------------------------

void CExternalFilesNetCDF::GetVariables (CStringArray& varNames)
{
  m_file.GetVariables(varNames);
}

//----------------------------------------
void CExternalFilesNetCDF::GetDimensions (const string& varName, CUIntArray& dimensions)
{
  m_file.GetVarDims(varName, dimensions);
}
//----------------------------------------
void CExternalFilesNetCDF::GetDimensions(const string& varName, CStringArray& dimensions)
{
  m_file.GetVarDims(varName, dimensions);
}

//----------------------------------------
void CExternalFilesNetCDF::CheckDimensions
		()
{
  CStringArray		dimNamesArray;

  m_dimValues.RemoveAll();
  m_dimIds.RemoveAll();


  //CStringArray::iterator	index;

  m_file.GetDimensions(dimNamesArray);
//  int32_t nbDims = dimNamesArray.size();

  CStringArray::iterator it;
  for (it = dimNamesArray.begin() ; it != dimNamesArray.end() ; it++)
  {
    int32_t dimId = m_file.GetDimId(*it);
    uint32_t dimValue = m_file.GetDimension(dimId);
    
    m_dimValues.Insert(*it, dimValue);
    m_dimIds.Insert(*it, dimId);
  }

}

//----------------------------------------
void CExternalFilesNetCDF::Open
		()
{
  Close();
  m_file.Open();
  try
  {
    LoadStructure();
    CheckVariables();

    AddAttributesAsField();

    AddBratIndexData();

  }
  catch (...)
  {
    Close();
    throw;
  }
}

//----------------------------------------

bool CExternalFilesNetCDF::IsAxisVar (const string& name)
{
  return m_file.IsAxisVar(name);
}



//----------------------------------------

void CExternalFilesNetCDF::FreeResources()
{
  m_nbMeasures	= 0;
  m_dimIds.RemoveAll();
  m_dimValues.RemoveAll();
  m_varList.RemoveAll();
  
  Rewind();
}

//----------------------------------------
void CExternalFilesNetCDF::Close()
{
  FreeResources();
  
  m_file.Close();
}

//----------------------------------------
bool CExternalFilesNetCDF::IsOpened()	const
{
  return m_file.IsOpened();
}

//----------------------------------------
int32_t CExternalFilesNetCDF::NumberOfRecords()
{
  MustBeOpened();
  return m_nbMeasures;
}

//----------------------------------------
void CExternalFilesNetCDF::GetValue(const string& name, CExpressionValue& value, const string& wantedUnit)
{
  double result;
  GetValue(name, result, wantedUnit);
  value = result;

}

//----------------------------------------
void CExternalFilesNetCDF::GetValue(const string& name, double& value, const string& wantedUnit)
{
  //double	result;

  MustBeOpened();


  // Find field def.
  CFieldNetCdf* field = GetFieldNetCdf(name);
  
  if (m_file.VarExists(field->GetName()))
  {
    if (field->IsAtBeginning())
    {
      throw CFileException(CTools::Format("PROGRAM ERROR: Try to access file ('%s' field) while file is already rewinded (NextRecord must be called)",
				          name.c_str()),
			   GetName(),
			   BRATHL_LOGIC_ERROR);
    }
  
    m_file.ReadVar(field, value, wantedUnit);

    return;
  }

  if (field->GetValues().size() > 0 )
  {
    value = field->GetValues().at(0);
  }

}
//----------------------------------------
void CExternalFilesNetCDF::GetAllValues(CFieldNetCdf* field, const string& wantedUnit)
{
  if (field == NULL)
  {
    return;
  }

  if (m_file.VarExists(field->GetName()))
  {
    CExpressionValue values;

    GetAllValues(field, values, wantedUnit);

    field->EmptyValues();
    field->SetValues(values.GetValues(), values.GetNbValues());
  }

  //If variable is not in Netcdf file, variable values have to be in field (m_values, accessible with field->GetValues()) 
}

//----------------------------------------
void CExternalFilesNetCDF::GetAllValues(CFieldNetCdf* field, CExpressionValue& value, const string& wantedUnit)
{
  GetAllValues(field->GetName(), value, wantedUnit);

}
//----------------------------------------
void CExternalFilesNetCDF::GetAllValues(const string& name, CExpressionValue& value, const string& wantedUnit)
{

  if (m_file.VarExists(name))
  {
    m_file.ReadVar(name, value, wantedUnit);
  }
  else
  {
    CFieldNetCdf* field = GetFieldNetCdf(name);
    value = field->GetValues();
  }

}
//----------------------------------------
void CExternalFilesNetCDF::GetAllValues(const string& name, CDoubleArray& vect, const string& wantedUnit)
{

  if (m_file.VarExists(name))
  {
    m_file.ReadVar(name, vect, wantedUnit);
    return;
  }

  CFieldNetCdf* field = GetFieldNetCdf(name);
  if (&vect != &field->GetValues())
  {
    vect = field->GetValues();
  }


}

/*
//----------------------------------------
void CExternalFilesNetCDF::GetValues(CFieldNetCdfCFAttr* field,  CExpressionValue &value, const string	&wantedUnit)
{

  exprValue.SetNewValue(FloatType, count, nbDimensions, NULL);
  value.SetNewValue(FloatType, edges, nbDimensions, NULL);

  GetValues(field->GetName(), value, wantedUnit);
}
*/
//----------------------------------------
void CExternalFilesNetCDF::GetValues(CFieldNetCdf* field,  CExpressionValue &value, const string	&wantedUnit)
{
  GetValues(field->GetName(), value, wantedUnit);
}


//----------------------------------------
void CExternalFilesNetCDF::GetValues(const string& name, CExpressionValue &value, const string	&wantedUnit)
{
  
  MustBeOpened();


  // Find field def.
  CFieldNetCdf* field = GetFieldNetCdf(name);
  
  //---------------------------
  // Variable exists in netcdf file
  //---------------------------
  if (m_file.VarExists(name))
  {
    if (field->IsAtBeginning())
    {
      throw CFileException(CTools::Format("PROGRAM ERROR: Try to access file ('%s' field) while file is already rewinded (NextRecord must be called)",
				          name.c_str()),
			   GetName(),
			   BRATHL_LOGIC_ERROR);
    }
  
    m_file.ReadVar(field, value, wantedUnit);
    return;
  }


  //---------------------------
  // Variable DOESN'T exist in netcdf file
  //---------------------------
  //If variable is not in Netcdf file, variable values have to be in field (m_values, accessible with field->GetValues()) 

  //---------------------------
  // field is an netcdf attribute
  //---------------------------
  if (CField::GetFieldNetCdfCFAttr(field, false) != NULL)
  {
    value.SetNewValue(field->GetValuesWithUnitConversion(CUnit::m_UNIT_SI));
    return;
  }

  //---------------------------
  // field is not an array (no dimension)
  //---------------------------
  uint32_t nbDimensions = field->GetNbDims();

  if (nbDimensions == 0)
  {
    value.SetNewValue(field->GetValues());
    return;
  }


  //---------------------------
  // field data value is unique 
  //---------------------------
  if (field->GetValues().size() == 1)
  {
    value.SetNewValue(field->GetValues());
    return;
  }
  
  //---------------------------
  // field is an array (number of dimension is > 0)
  //---------------------------
  if (field->GetValuesAsArray() == NULL)
  {
    field->SetValuesAsArray();
  }


  uint32_t* shape = (uint32_t*)(field->GetDim());

  uint32_t* start = field->GetDimsIndexArray();

  uint32_t* edges = field->GetDimsCountArray();
  CUIntArray edgesArray;
  edgesArray.Insert(edges, nbDimensions);

  value.SetNewValue(FloatType, edges, nbDimensions, NULL);

  CTools::ExtractVector(field->GetValuesAsArray(), shape, nbDimensions, start, edges, value.GetValues());

}

//----------------------------------------
void CExternalFilesNetCDF::ExecuteExpression(CExpression &expr, CExpressionValue& exprValue, const string& wantedUnit, CProduct* product /* = NULL */)
{
  CStringArray::const_iterator itFieldName;
  const CStringArray* fields = expr.GetFieldNames();

  CExpressionValue* exprValueWork = NULL;

  if (fields->empty())
  {
    return;
  }

  // Set the values for each fields in the expression
  for (itFieldName = fields->begin(); itFieldName != fields->end() ; itFieldName++)
  {
    CFieldNetCdf* fieldNetCdf = GetFieldNetCdf(*itFieldName);
    exprValueWork = new CExpressionValue();

    GetValues(fieldNetCdf, *exprValueWork, wantedUnit);
    //m_file.ReadVar(fieldNetCdf, *exprValueWork, wantedUnit);


    // Insert a "CExpressionValue*" into the expression fields values
    // (delete CExpressionValue object is done within CExpression).
    expr.SetValue(*itFieldName, exprValueWork);
  }

  // Execute the expression and get the result
  exprValue = expr.Execute(product);

}


//----------------------------------------
void CExternalFilesNetCDF::GetFieldNames
		(CStringArray &names)
{
  m_varList.GetKeys(names);
}
//----------------------------------------
CFieldNetCdf* CExternalFilesNetCDF::GetFieldNetCdf(const string &name, bool withExcept /*= true*/)
{
  CFieldNetCdf *var  = CExternalFiles::GetFieldNetCdf(m_varList.Exists(name), withExcept); 

  return var;
}
//----------------------------------------
int32_t CExternalFilesNetCDF::GetNetCdfId(const string &name, bool withExcept /*= true*/)
{
  CFieldNetCdf* var = GetFieldNetCdf(name, withExcept);

  int32_t netCdfId = NC_GLOBAL;
  if (var != NULL) 
  {
    netCdfId = var->GetNetCdfId();
  }

  return netCdfId;

}
/*
//----------------------------------------
CExternalFileFieldDescription* CExternalFilesNetCDF::GetFieldDescription
		(const string& name)
{
  // Return its description
  CRegisteredVar *var  = GetRegisteredVar(name);
  if (var == NULL)
  {
    return NULL;
  }

  return &(var->GetExternalFileFieldDescription());
}
*/
//----------------------------------------
nc_type CExternalFilesNetCDF::GetVarType(const string &name)
{
  return m_file.GetVarType(name);
}

//----------------------------------------
string CExternalFilesNetCDF::GetVarTypeName(const string &name)
{
  return m_file.GetVarTypeName(name);
}


//----------------------------------------
void CExternalFilesNetCDF::AddVar
		            (int32_t	netcdfId,
		             const string	&name,
		             const string	&description,
		             const string	&unit,
                 int32_t type,
                 uint32_t dimValue,
                 const string dimName, 
                 int32_t dimId,
                 const CStringMap* mapAttributes /*= NULL*/)
{

  CUIntArray dimValues;
  dimValues.Insert(dimValue);

  CStringArray dimNames;
  dimNames.Insert(dimName);

  CIntArray dimIds;
  dimIds.Insert(dimId);

  AddVar(netcdfId, name, description, unit, type, &dimValues, &dimNames, &dimIds, mapAttributes);

}
//----------------------------------------
void CExternalFilesNetCDF::AddVar
		            (int32_t	netcdfId,
		             const string	&name,
		             const string	&description,
		             const string	&unit,
                 int32_t type /*= NC_NAT*/,
                 const CUIntArray* dimValues  /*= NULL*/,
                 const CStringArray* dimNames  /*= NULL*/, 
                 const CIntArray* dimIds /*= NULL*/,
                 const CStringMap* mapAttributes /*= NULL*/)
{
  
//  CExternalFileFieldDescription externalFileFieldDescription(name, description, unit, type, dimValues, dimNames);
  
  CFieldNetCdf* field = NULL;

  if (IsNetCdfCFProduct())
  {
    field = new CFieldNetCdfCF(name, description, unit, netcdfId, type, dimValues, dimNames, dimIds);
  }
  else
  {
    field = new CFieldNetCdf(name, description, unit, netcdfId, type, dimValues, dimNames, dimIds);
  }
  
  if (netcdfId >= 0)
  {
    field->SetScaleFactor(m_file.GetNetcdfVarScaleFactor(netcdfId));
  
    field->SetAddOffset(m_file.GetNetcdfVarAddOffset(netcdfId));
  
    field->SetFillValue(m_file.GetNetcdfVarDefValue(netcdfId, nc_type(type)));
    
    // if dimension is 0, read data once and for all.
    CNetCDFVarDef* netCDFVarDef = m_file.GetNetCDFVarDef(name, true);
  
    if (netCDFVarDef->GetNbDims() <= 0 )
    {
      m_file.ReadEntireVar(netCDFVarDef, field->GetValues(), CUnit::m_UNIT_SI);
    }


  }


  if (mapAttributes != NULL)
  {
    field->SetAttributes(mapAttributes);
  }


  CBratObject* ob = m_varList.Insert(name, field, false);
  if (ob == NULL)
  {
    throw CFileException(CTools::Format("Unable to register variable '%s'. Maybe name conflict",
				        name.c_str()),
			 GetName(),
			 BRATHL_INCONSISTENCY_ERROR);
  }

  AddAttributesAsField(field);

}


//----------------------------------------
void CExternalFilesNetCDF::AddAttributesAsField(CFieldNetCdf* field /*= NULL*/)
{

  CNetCDFVarDef* netCDFVarDef = NULL;
  CObMap* netCDFattributes = NULL;

  if (field != NULL)
  {

    netCDFVarDef = m_file.GetNetCDFVarDef(field->GetName());
    // Note that netCDFVarDef can be nULL, ie "virtual variables" of ATP and some Aviso Grid products.
    if (netCDFVarDef == NULL)
    {
      return;
    }

    netCDFattributes = netCDFVarDef->GetAttributes();
  }
  else // Global Attributes
  {
    netCDFattributes = m_file.GetNetCDFGlobalAttributes();
  }

  if(netCDFattributes == NULL)
  {
    return;
  }

  //const CStringMap& attributes = fieldCloned->GetAttributes();
  CObMap::const_iterator itAttr;

  for (itAttr = netCDFattributes->begin() ; itAttr != netCDFattributes->end() ; itAttr++)
  {
    CNetCDFAttr* netCDFattribute = CNetCDFAttr::GetNetCDFAttr(itAttr->second);
    if (netCDFattribute == NULL)
    {
      continue;
    }

    // Excludes non-numeric attributes, except string date representation attributes
    if (!netCDFattribute->IsTypeNumericOrDateString())
    {
      continue;
    }
    // Excludes specific attributes
    if (CExternalFiles::IsExcludedAttrAsField(netCDFattribute->GetName()))
    {
      continue;
    }

    //netCDFattribute->Dump(*CTrace::GetDumpContext());
    
    CFieldNetCdfCFAttr* fieldNetCdfCFAttr = NULL;

    if (field != NULL)
    {
      fieldNetCdfCFAttr = new CFieldNetCdfCFAttr(netCDFVarDef, netCDFattribute);
    }
    else
    {
      fieldNetCdfCFAttr = new CFieldNetCdfCFAttr(netCDFattribute);
    }

    CBratObject* ob = m_varList.Insert(fieldNetCdfCFAttr->GetName(), fieldNetCdfCFAttr, false);
    if (ob == NULL)
    {
      throw CFileException(CTools::Format("Unable to register variable '%s' (inherit from attribute '%s' of variable '%s'. Maybe name conflict",
				          fieldNetCdfCFAttr->GetName().c_str(),
				          netCDFattribute->GetName().c_str(),
				          field->GetName().c_str()),
			   GetName(),
			   BRATHL_INCONSISTENCY_ERROR);
    }
   
  }
}

//----------------------------------------
void CExternalFilesNetCDF::AddBratIndexData()
{
  CObArray::const_iterator it;
  CObArray timeVars(false);

  // Search variables wich represent dates
  // The result is a map whose key is the concatenation of the names of dimensions:
  // this allows to keep only the distinct dimensions
  m_file.GetNetCDFVarDefsTimeByDim(&timeVars, true);

  int32_t i = 0;
  for (it = timeVars.begin() ; it != timeVars.end(); it++)
  {
    i++;
    CNetCDFVarDef* var = CNetCDFVarDef::GetNetCDFVarDef(*it);
    if (var == NULL)
    {
      continue;
    }

    string name;
    name.append(CField::m_BRAT_INDEX_DATA_NAME);
    if (i > 1)
    {
      name.append("_");
      name.append(CTools::IntToStr(i));
    }
    
    // If name already exists then return (to avoid memory leaks)
    // Name may already exists if the input netcdf file is an output Brat file which contains 'brat index data'
    if (m_varList.Exists(name))
    {
      return;
    }

    CFieldNetCdfIndexData* fieldIndexData = new CFieldNetCdfIndexData(*var);
    fieldIndexData->SetName(name);
    string desc;
    desc.append(CField::m_BRAT_INDEX_DATA_DESC);
    desc.append(" based on field ");
    desc.append(var->GetName());

    fieldIndexData->SetDescription(desc);

    CBratObject* ob = m_varList.Insert(name, fieldIndexData, false);

    if (ob == NULL)
    {
      throw CException(CTools::Format("Unable to register variable '%s'. Maybe name conflict",
				          name.c_str()),			   
			   BRATHL_INCONSISTENCY_ERROR);
    }
  }

}

//----------------------------------------
bool CExternalFilesNetCDF::VarExists(const string& name)
{
  return m_file.VarExists(name);
}

//----------------------------------------
void CExternalFilesNetCDF::SubstituteDimNames(CStringArray& dimNames)
{
}
//----------------------------------------
void CExternalFilesNetCDF::AddVar(const string& name)
{
  int32_t varId	= m_file.GetVarId(name);
  string description;
  string comment;
  string unit;
  string standardName;

//  m_file.GetAtt(varId, LONG_NAME_ATTR, description, false, name + " field");
//  m_file.GetAtt(varId, UNITS_ATTR, unit, false, "count");

  CStringMap mapAttributes;
  m_file.GetAttributes(varId, mapAttributes);

  unit = mapAttributes.Exists(UNITS_ATTR);
  if (unit.empty()) 
  {
    unit = "count";
  }

  description = mapAttributes.Exists(LONG_NAME_ATTR);
  if (description.empty()) 
  {
    description = name + " field";
  }

  standardName = mapAttributes.Exists(STANDARD_NAME_ATTR);
  if (!standardName.empty()) 
  {
    description = standardName + " field";
  }

  comment = mapAttributes.Exists(COMMENT_ATTR);
  if (!comment.empty()) 
  {
    description += "\n" + comment;
  }

  description += "\n";

  nc_type type = GetVarType(name);

  ExpressionValueDimensions dimValues;
  m_file.GetVarDims(varId, dimValues);

  CStringArray dimNames;
  m_file.GetVarDims(varId, dimNames);

  //---------------
  SubstituteDimNames(dimNames);
  //---------------

  CIntArray dimIds;
  m_file.GetVarDimIds(varId, dimIds);

  uint32_t nbDims = dimNames.size();

  if ((nbDims == dimValues.size()) && (nbDims > 0))
  {
    description += "\nDimensions: ";
    for (uint32_t i = 0 ; i < nbDims ; i++)
    {
      description += CTools::Format("  %s=%d   ", dimNames[i].c_str(), dimValues[i]);
    }
    
  }

  description += "\n";

  CStringMap::iterator it;
  for (it = mapAttributes.begin() ; it != mapAttributes.end() ; it++)
  {
    string attName = (*it).first;
    string attValue = (*it).second;


    if ((attName.compare(LONG_NAME_ATTR) == 0) ||
//         (attName.compare(FILL_VALUE_ATTR) == 0) ||
//         (attName.compare(SCALE_FACTOR_ATTR) == 0) ||
//         (attName.compare(ADD_OFFSET_ATTR) == 0) ||
         (attName.compare(COMMENT_ATTR) == 0) 
         )
      {
        continue;
      }

    description += CTools::Format("\n- %s: %s", attName.c_str(), attValue.c_str());;
  }


  AddVar(varId, name, description, unit, type, &dimValues, &dimNames, &dimIds, &mapAttributes);
}


//----------------------------------------
CFieldNetCdf* CExternalFilesNetCDF::GetVarByAttribute(const string& attrName, const string& attrValueToSearch)
{

  CFieldNetCdf* fieldToReturn = NULL;
  CObMap::iterator it;
  for (it = m_varList.begin() ; it != m_varList.end() ; it++)
  {
    CFieldNetCdf* field = CExternalFiles::GetFieldNetCdf(it->second);
    string attrValue = field->GetAttribute(attrName);
    if ((attrValue.empty() == false) && (attrValue.compare(attrValueToSearch) == 0))
    {
      fieldToReturn = field;
      break;
    }
  }

  return fieldToReturn;
}

//----------------------------------------

int CExternalFilesNetCDF::GetGlobalAttribute
		(const string	&attName,
		       double	&attValue,
		       bool	mustExist	/*= true*/,
		       double   defaultValue	/*= CTools::m_defaultValueDOUBLE*/)
{
  return m_file.GetGlobalAttributeAsDouble(attName, attValue, mustExist, defaultValue);
}
//----------------------------------------
int CExternalFilesNetCDF::GetGlobalAttribute
		(const string	&attName,
		       string	&attValue,
		       bool	mustExist	/*= true*/,
		       string   defaultValue	/*= ""*/)
{
  return m_file.GetGlobalAttributeAsString(attName, attValue, mustExist, defaultValue);
}

//----------------------------------------
int CExternalFilesNetCDF::GetAttribute
		(const string	&varName,
		 const string	&attName,
		       double	&attValue,
		       bool	mustExist	/*= true*/,
		       double   defaultValue	/*= CTools::m_defaultValueDOUBLE*/)
{
  return m_file.GetAttributeAsDouble(varName, attName, attValue, mustExist, defaultValue);
}

//----------------------------------------
int CExternalFilesNetCDF::GetAttribute
		(const string	&varName,
		 const string	&attName,
		       string	&attValue,
		       bool	mustExist	/*= true*/,
		       string   defaultValue	/*= ""*/)
{
  return m_file.GetAttributeAsString(varName, attName, attValue, mustExist, defaultValue);
}

//----------------------------------------
nc_type CExternalFilesNetCDF::GetAttributeType(const string& attName)
{
  return m_file.GetAttributeType(NC_GLOBAL, attName);
}
//----------------------------------------
nc_type CExternalFilesNetCDF::GetAttributeType(const string& varName, const string& attName)
{
  return m_file.GetAttributeType(varName, attName);
}

//----------------------------------------
void CExternalFilesNetCDF::GetGlobalAttributes(CStringMap& mapAttributes)
{

  m_file.GetAttributes(NC_GLOBAL, mapAttributes);
}
//----------------------------------------
void CExternalFilesNetCDF::GetGlobalAttributes(CDoubleMap& mapAttributes)
{

  m_file.GetAttributes(NC_GLOBAL, mapAttributes);
}

//----------------------------------------
void CExternalFilesNetCDF::GetGlobalAttributes(string& attributes)
{
  CStringMap mapAttributes;
  m_file.GetAttributes(NC_GLOBAL, mapAttributes);
  
  attributes = "";

  CStringMap::iterator it;
  for (it = mapAttributes.begin() ; it != mapAttributes.end() ; it++)
  {
    string attName = (*it).first;
    string attValue = (*it).second;

    attributes += CTools::Format("\n %s: %s", attName.c_str(), attValue.c_str());;
  }


  return;
}


//----------------------------------------
void CExternalFilesNetCDF::GetOrderedDimNames(const string& value, CStringArray& commonDimensionNames)
{
  CExpression expr;
  
  string msg;

  bool bOk = CExpression::SetExpression(value, expr, msg);

  if (!bOk)
  {
    return;
  }
  
  GetOrderedDimNames(expr, commonDimensionNames);
}

//----------------------------------------
void CExternalFilesNetCDF::GetOrderedDimNames(const CExpression& value, CStringArray& commonDimensionNames)
{
  GetOrderedDimNames(value.GetFieldNames(), commonDimensionNames);
}

//----------------------------------------
void CExternalFilesNetCDF::GetOrderedDimNames(const CStringArray* fieldNames,CStringArray& commonDimensionNames)
{
  
  MustBeOpened();

  m_file.GetNetCDFCoordAxes(*fieldNames, &commonDimensionNames, false, true);

}
//----------------------------------------
void CExternalFilesNetCDF::GetOrderedDimNamesFromFieldNetcdf(const CStringArray* fieldNames,CStringArray& commonDimensionNames)
{
  
  MustBeOpened();

  CStringArray::const_iterator it;

  for (it = fieldNames->begin() ; it != fieldNames->end() ; it++)
  {
    CFieldNetCdf* var = GetFieldNetCdf(*it, false);
    if (var == NULL)
    {
      continue;
    }


    CStringArray::const_iterator itDim;
    
    const CStringArray& dimNames = var->GetDimNames();
    //const CUintMap& dimRanges = var->GetDimRanges();

    for (itDim = dimNames.begin() ; itDim != dimNames.end() ; itDim++)
    {
      uint32_t dimRange = var->GetDimRange(*itDim);

      //bool exists = commonDimensionNames.Exists(*itDim);
      int32_t index = commonDimensionNames.FindIndex(*itDim);
      

      if (index < 0)
      {
        commonDimensionNames.InsertAt(dimRange, *itDim);
      }
      else if (dimRange > (uint32_t)index)
      {
        commonDimensionNames.ReplaceAt(index, "");
        commonDimensionNames.InsertAt(dimRange, *itDim);
      }
    }
    //commonDimensionNames.InsertUnique(var->GetDimNames());
  }

}
//----------------------------------------
bool CExternalFilesNetCDF::IsLatField(CFieldNetCdf* field)
{
  if (field == NULL)
  {
    return false;
  }

  bool bOk = false;

  for (int i = 0 ; i < CNetCDFFiles::m_LAT_NAMES_SIZE - 1 ; i++)
  {
    if (field->GetName().compare(CNetCDFFiles::m_LAT_NAMES[i]) == 0)
    {
      bOk = true;
      break;
    }

    string value = field->GetAttribute(STANDARD_NAME_ATTR);
    if (!value.empty())
    {
      if (value.compare(CNetCDFFiles::m_LAT_NAMES[i]) == 0)
      {
        bOk = true;
        break;
      }
    }

    value = field->GetAttribute(LONG_NAME_ATTR);
    if (!value.empty())
    {
      if (value.compare(CNetCDFFiles::m_LAT_NAMES[i]) == 0)
      {
        bOk = true;
        break;
      }
    }
  }

  if (bOk) 
  {
    return bOk;
  }

  for (int i = 0 ; i < CNetCDFFiles::m_LAT_UNITS_SIZE - 1 ; i++)
  {
    string value = field->GetAttribute(UNITS_ATTR);
    if (!value.empty())
    {
      if (value.compare(CNetCDFFiles::m_LAT_UNITS[i]) == 0)
      {
        bOk = true;
        break;
      }
    }
  }

  return bOk;
    
}

//----------------------------------------
bool CExternalFilesNetCDF::IsLonField(CFieldNetCdf* field)
{
  if (field == NULL)
  {
    return false;
  }

  bool bOk = false;

  for (int i = 0 ; i < CNetCDFFiles::m_LON_NAMES_SIZE - 1 ; i++)
  {
    if (field->GetName().compare(CNetCDFFiles::m_LON_NAMES[i]) == 0)
    {
      bOk = true;
      break;
    }

    string value = field->GetAttribute(STANDARD_NAME_ATTR);
    if (!value.empty())
    {
      if (value.compare(CNetCDFFiles::m_LON_NAMES[i]) == 0)
      {
        bOk = true;
        break;
      }
    }

    value = field->GetAttribute(LONG_NAME_ATTR);
    if (!value.empty())
    {
      if (value.compare(CNetCDFFiles::m_LON_NAMES[i]) == 0)
      {
        bOk = true;
        break;
      }
    }
  }

  if (bOk) 
  {
    return bOk;
  }

  for (int i = 0 ; i < CNetCDFFiles::m_LON_UNITS_SIZE - 1 ; i++)
  {
    string value = field->GetAttribute(UNITS_ATTR);
    if (!value.empty())
    {
      if (value.compare(CNetCDFFiles::m_LON_UNITS[i]) == 0)
      {
        bOk = true;
        break;
      }
    }
  }

  return bOk;
    
}
//----------------------------------------
CFieldNetCdf* CExternalFilesNetCDF::FindTimeField()
{
  CFieldNetCdf* field = NULL;

  for (int i = 0 ; i < CNetCDFFiles::m_TIME_NAMES_SIZE ; i++)
  {
    field = GetFieldNetCdf(CNetCDFFiles::m_TIME_NAMES[i], false);
    if (field != NULL) 
    {
      break;
    }

    field = GetVarByAttribute(STANDARD_NAME_ATTR, CNetCDFFiles::m_TIME_NAMES[i]);

    if (field != NULL) 
    {
      break;
    }
  }

  return field;
    
}
//----------------------------------------
CFieldNetCdf* CExternalFilesNetCDF::FindLatField()
{
  CFieldNetCdf* field = NULL;

  for (int i = 0 ; i < CNetCDFFiles::m_LAT_NAMES_SIZE - 1 ; i++)
  {
    field = GetFieldNetCdf(CNetCDFFiles::m_LAT_NAMES[i], false);
    if (field != NULL) 
    {
      break;
    }

    field = GetVarByAttribute(STANDARD_NAME_ATTR, CNetCDFFiles::m_LAT_NAMES[i]);

    if (field != NULL) 
    {
      break;
    }

    field = GetVarByAttribute(LONG_NAME_ATTR, CNetCDFFiles::m_LAT_NAMES[i]);

    if (field != NULL) 
    {
      break;
    }
  }

  if (field != NULL) 
  {
    return field;
  }

  for (int i = 0 ; i < CNetCDFFiles::m_LAT_UNITS_SIZE - 1 ; i++)
  {
    field = GetVarByAttribute(UNITS_ATTR, CNetCDFFiles::m_LAT_UNITS[i]);
    if (field != NULL) 
    {
      break;
    }
  }

  return field;
    
}
//----------------------------------------
CFieldNetCdf* CExternalFilesNetCDF::FindLonField()
{
  CFieldNetCdf* field = NULL;

  for (int i = 0 ; i < CNetCDFFiles::m_LON_NAMES_SIZE - 1 ; i++)
  {
    field = GetFieldNetCdf(CNetCDFFiles::m_LON_NAMES[i], false);
    if (field != NULL) 
    {
      break;
    }

    field = GetVarByAttribute(STANDARD_NAME_ATTR, CNetCDFFiles::m_LON_NAMES[i]);

    if (field != NULL) 
    {
      break;
    }

    field = GetVarByAttribute(LONG_NAME_ATTR, CNetCDFFiles::m_LON_NAMES[i]);

    if (field != NULL) 
    {
      break;
    }
  }

  if (field != NULL) 
  {
    return field;
  }

  for (int i = 0 ; i < CNetCDFFiles::m_LON_UNITS_SIZE - 1 ; i++)
  {
    field = GetVarByAttribute(UNITS_ATTR, CNetCDFFiles::m_LON_UNITS[i]);
    if (field != NULL) 
    {
      break;
    }
  }

  return field;
    
}

//----------------------------------------
CFieldNetCdf* CExternalFilesNetCDF::FindPassField()
{
  CFieldNetCdf* field = NULL;

  for (int i = 0 ; i < CNetCDFFiles::m_PASS_NAMES_SIZE - 1 ; i++)
  {
    field = GetFieldNetCdf(CNetCDFFiles::m_PASS_NAMES[i], false);
    if (field != NULL) 
    {
      break;
    }

    field = GetVarByAttribute(STANDARD_NAME_ATTR, CNetCDFFiles::m_PASS_NAMES[i]);

    if (field != NULL) 
    {
      break;
    }
  }

  return field;
    
}

//----------------------------------------
CFieldNetCdf* CExternalFilesNetCDF::FindCycleField()
{
  CFieldNetCdf* field = NULL;

  for (int i = 0 ; i < CNetCDFFiles::m_CYCLE_NAMES_SIZE - 1 ; i++)
  {
    field = GetFieldNetCdf(CNetCDFFiles::m_CYCLE_NAMES[i], false);
    if (field != NULL) 
    {
      break;
    }

    field = GetVarByAttribute(STANDARD_NAME_ATTR, CNetCDFFiles::m_CYCLE_NAMES[i]);

    if (field != NULL) 
    {
      break;
    }
  }

  return field;
    
}

//----------------------------------------
void CExternalFilesNetCDF::SetOffset(double value, bool force /*= false*/)
{
  CExternalFiles::SetOffset(value);
  SetOffset(force);
}

//----------------------------------------
void CExternalFilesNetCDF::AddOffset(double value, bool force /*= false*/)
{
  CExternalFiles::AddOffset(value);
  SetOffset(force);
}

//----------------------------------------
void CExternalFilesNetCDF::SetOffset(bool force /*= false*/)
{
  CObMap::const_iterator it;
  for (it = m_varList.begin() ; it != m_varList.end() ; it++)
  {
    CFieldNetCdfIndexData* field = CExternalFiles::GetFieldNetCdfIndexData(it->second, false);
    if (field == NULL)
    {
      continue;
    }
    field->SetOffset(m_offset);
  }

}

////----------------------------------------
//void CExternalFilesNetCDF::ReadEntireVar(CNetCDFVarDef* netCDFVarDef, CDoubleArray& array, const string& wantedUnit)
//{
//
//  m_file.ReadEntireVar(netCDFVarDef, array, wantedUnit);
//
//}
////----------------------------------------
//void CExternalFilesNetCDF::ReadEntireVar(CNetCDFDimension* netCDFDimension, CDoubleArray& array, const string& wantedUnit)
//{
//
//  m_file.ReadEntireVar(netCDFDimension, array, wantedUnit);
//
//}
//-------------------------------------------------------------
//------------------- CExternalFilesNetCDFCF class --------------------
//-------------------------------------------------------------


CExternalFilesNetCDFCF::CExternalFilesNetCDFCF
		(const string	&name	/*= ""*/)
	: CExternalFilesNetCDF(name)
{
  SetProductClass(NETCDF_CF_PRODUCT_CLASS);
  
}

//----------------------------------------
CExternalFilesNetCDFCF::~CExternalFilesNetCDFCF()
{
}
//----------------------------------------
void CExternalFilesNetCDFCF::FreeResources()
{
  CExternalFilesNetCDF::FreeResources();

}
//----------------------------------------
void CExternalFilesNetCDFCF::LoadStructure ()
{
  CheckDimensions();

  m_nbMeasures = 1; 

  CUIntMap::iterator it;

  for (it = m_dimValues.begin(); it != m_dimValues.end(); it++)
  {
    m_nbMeasures *= it->second;
  }

}
//----------------------------------------
void CExternalFilesNetCDFCF::Rewind ()
{
//  CException e("ERROR - CExternalFilesNetCDFCF::Rewind() - unexpected call - CExternalFilesNetCDFCF::Rewind() have not to be called ", BRATHL_LOGIC_ERROR);
//  throw (e);
}

//----------------------------------------
// True while there is something to read
bool CExternalFilesNetCDFCF::NextRecord ()
{
  CException e("ERROR - CExternalFilesNetCDFCF::NextRecord() - unexpected call - CExternalFilesNetCDFCF::NextRecord() have not to be called ", BRATHL_LOGIC_ERROR);
  throw (e);
}
//----------------------------------------
// True while there is something to read
bool CExternalFilesNetCDFCF::PrevRecord ()
{
  CException e("ERROR - CExternalFilesNetCDFCF::PrevRecord() - unexpected call - CExternalFilesNetCDFCF::PrevRecord() have not to be called ", BRATHL_LOGIC_ERROR);
  throw (e);
}


//-------------------------------------------------------------
//------------------- CExternalFilesYFX class --------------------
//-------------------------------------------------------------

CExternalFilesYFX::CExternalFilesYFX
		(const string		&name /*= ""*/)
	: CExternalFilesNetCDFCF(name)
{
}

//----------------------------------------
CExternalFilesYFX::~CExternalFilesYFX()
{
}
//-------------------------------------------------------------
//------------------- CExternalFilesZFXY class --------------------
//-------------------------------------------------------------

CExternalFilesZFXY::CExternalFilesZFXY
		(const string		&name /*= ""*/)
	: CExternalFilesNetCDFCF(name)
{
}

//----------------------------------------
CExternalFilesZFXY::~CExternalFilesZFXY()
{
}
//-------------------------------------------------------------
//------------------- CExternalFilesNetCDFCFGeneric class --------------------
//-------------------------------------------------------------

CExternalFilesNetCDFCFGeneric::CExternalFilesNetCDFCFGeneric
		(const string		&name /*= ""*/)
	: CExternalFilesNetCDFCF(name)
{
}

//----------------------------------------
CExternalFilesNetCDFCFGeneric::~CExternalFilesNetCDFCFGeneric()
{
}

}


