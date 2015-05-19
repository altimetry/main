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
#include "Date.h"
#include "Exception.h"
#include "Expression.h"

#include "InternalFilesFactory.h"

#include "InternalFiles.h"
#include "InternalFilesYFX.h"
#include "InternalFilesZFXY.h"

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

namespace brathl
{


//----------------------------------------

CInternalFiles::CInternalFiles
		(string			name /*= ""*/,
		 brathl_FileMode mode /*= ReadOnly*/)
{
  SetName(name);
  SetMode(mode);
}
//----------------------------------------

CInternalFiles::~CInternalFiles()
{
  Close();
}
//----------------------------------------

string CInternalFiles::GetName
		() const
{
  return m_file.GetName();
}

//----------------------------------------

void CInternalFiles::SetName(const string& name)
{
  m_file.SetName(name);
}
//----------------------------------------
void CInternalFiles::SetMode(brathl_FileMode	mode)
{
  m_file.SetMode(mode);
}
//----------------------------------------
void CInternalFiles::Open(brathl_FileMode	mode)
{
  m_file.SetMode(mode);
  Open();
}
//----------------------------------------
void CInternalFiles::Open()
{
  m_file.Open();
  SetFixedGlobalAttributes();
}
//----------------------------------------


void CInternalFiles::Close
		()
{
  m_file.Close();
}

//----------------------------------------

bool CInternalFiles::IsOpened
		()
{
  return m_file.IsOpened();
}

//----------------------------------------

void CInternalFiles::WriteFileTitle
		(const string	&Title)
{
  m_file.WriteFileTitle(Title);
}

//----------------------------------------

int CInternalFiles::GetDimId
		(const string& name)
{
  
  return m_file.GetDimId(name);
}

//----------------------------------------

int32_t CInternalFiles::GetVarDimIndex
		(const string& varName,
                 const string& dimName)
{
  CStringArray dimensions;
  GetVarDims(varName, dimensions);
  
  for (uint32_t i = 0 ; i < dimensions.size() ; i++)
  {
    if (dimensions.at(i).compare(dimName) == 0)
    {
      return i;
    }
  }

  return -1;
}

//----------------------------------------

string CInternalFiles::GetType
		()
{
  throw CException("Virtual function GetType must be defined in all derived types of CInternalFiles",
		   BRATHL_LOGIC_ERROR);
  return TypeOf();
}

//----------------------------------------

void CInternalFiles::GetVariables
		(vector<string> &VarNames)
{
  m_file.GetVariables(VarNames);
}
//----------------------------------------

void CInternalFiles::GetAxisVars
		(vector<string> &VarNames)
{
  m_file.GetAxisVars(VarNames);
}
//----------------------------------------

void CInternalFiles::GetDataVars
		(vector<string> &VarNames)
{
  m_file.GetDataVars(VarNames);
}
//----------------------------------------

NetCDFVarKind CInternalFiles::GetVarKind
		(const string		&Name)
{
  return m_file.GetVarKind(Name);
}
//----------------------------------------

void CInternalFiles::GetVarDims
		(const string			&Name,
		 ExpressionValueDimensions	&Dimensions)
{
  m_file.GetVarDims(Name, Dimensions);
}
//----------------------------------------

void CInternalFiles::GetVarDims
		(const string			&Name,
		 vector<string>			&Dimensions)
{
  m_file.GetVarDims(Name, Dimensions);
}
//----------------------------------------

bool CInternalFiles::GetCommonVarDims(const string& varName1, const string& varName2, CStringArray& intersect)
{
  CStringArray dims1;
  CStringArray dims2;
  
  m_file.GetVarDims(varName1, dims1);
  m_file.GetVarDims(varName2, dims2);

  return dims1.Intersect(dims2, intersect);
}
//----------------------------------------

bool CInternalFiles::GetComplementVarDims(const string& varName1, const string& varName2, CStringArray& complement)
{
  CStringArray dims1;
  CStringArray dims2;
  
  m_file.GetVarDims(varName1, dims1);
  m_file.GetVarDims(varName2, dims2);

  return dims1.Complement(dims2, complement);
}
//----------------------------------------

bool CInternalFiles::GetComplementVars(const CStringArray& varNames, CStringArray& complement, bool excludeDim /* = true */)
{
  CStringArray allVarNames;
  
  m_file.GetNetCDFVarDefNames(allVarNames);

  if (excludeDim)
  {
    CStringArray allDimNames;
    m_file.GetNetCDFDimsNames(allDimNames);
    allVarNames.Remove(allDimNames);
  }

  return varNames.Complement(allVarNames, complement);
}
//----------------------------------------

bool CInternalFiles::VarExists
		(const string		&Name)
{
  return m_file.VarExists(Name);
}

//----------------------------------------

bool CInternalFiles::HasVar(NetCDFVarKind	VarKind)
{
  return m_file.HasVar(VarKind);
}
//----------------------------------------

void CInternalFiles::WriteVar
		(const string		&Name,
		 const CExpressionValue	&Value)
{
  m_file.WriteVar(Name, Value);
}
//----------------------------------------

void CInternalFiles::ReadVar
		(const string			&Name,
		       CExpressionValue		&Value,
		 const string			&WantedUnit)
{
  m_file.ReadVar(Name, Value, WantedUnit);
}


//----------------------------------------

bool CInternalFiles::IsVarNameValid
		(const string	&Name)
{
  return CNetCDFFiles::IsVarNameValid(Name);
}

//----------------------------------------

bool CInternalFiles::IsAxisVar
		(const string	&Name)
{
  return m_file.IsAxisVar(Name);
}


//----------------------------------------

CUnit CInternalFiles::GetUnit
		(const string			&Name)
{
  return m_file.GetUnit(Name);
}

//----------------------------------------

string CInternalFiles::GetTitle
		(const string			&Name)
{
  return m_file.GetTitle(Name);
}
//----------------------------------------

void CInternalFiles::SetFixedGlobalAttributes
		(void)
{
  if (m_file.IsCreating())
  {
    m_file.PutAtt(NC_GLOBAL, FILE_TYPE_ATTR, GetType());
    m_file.PutAtt(NC_GLOBAL, CONVENTIONS_ATTR, "CF-1.0");
  }
}



//----------------------------------------
int CInternalFiles::GetAttribute
		(const string	&varName,
		 const string	&attName,
		       double	&attValue,
		       bool	mustExist	/*= true*/,
		       double   defaultValue	/*= CTools::m_defaultValueDOUBLE*/)
{
  return m_file.GetAttributeAsDouble(varName, attName, attValue, mustExist, defaultValue);
}

//----------------------------------------
int CInternalFiles::GetAttribute
		(const string	&varName,
		 const string	&attName,
		       string	&attValue,
		       bool	mustExist	/*= true*/,
		       string   defaultValue	/*= ""*/)
{
  return m_file.GetAttributeAsString(varName, attName, attValue, mustExist, defaultValue);
}

//----------------------------------------
string CInternalFiles::GetComment(const string& varName)
{
  string attValue;
  GetAttribute(varName, COMMENT_ATTR, attValue, false);
  return attValue;
}
//----------------------------------------
void CInternalFiles::WriteDimensions()
{
    m_file.WriteDimensions();
}

//----------------------------------------
void CInternalFiles::WriteVariables()
{
    m_file.WriteVariables();
}
//----------------------------------------
void CInternalFiles::WriteData(CNetCDFVarDef* varDef, CExpressionValue* data)
{
    m_file.WriteData(varDef, data);
}
//----------------------------------------
void CInternalFiles::WriteData(CNetCDFVarDef* varDef, CMatrix* matrix)
{
    m_file.WriteData(varDef, matrix);
}
//----------------------------------------
void CInternalFiles::ReplaceNetCDFDim(CNetCDFDimension& dim)
{
  m_file.ReplaceNetCDFDim(dim);
}

//----------------------------------------
CInternalFiles* CInternalFiles::Create(const string& fileName, bool open /* = true */, bool withExcept /* = true */)
{
  CInternalFiles* f = NULL;
  
  try
  {
    f = BuildExistingInternalFileKind(fileName);
  
    if (f == NULL)
    {
      return false;
    }

    f->Open(ReadOnly);

  }
  catch (CException& e)
  {
    CTools::DeleteObject(f);
    f = NULL;


    if (withExcept)
    {
      throw CFileException(CTools::Format("Unable to create '%s' file. Reason: %s",
			                                    fileName.c_str(),
			                                    e.what()),
	                          fileName,
	                          BRATHL_IO_ERROR);

    }
  }

  return f;

}

//----------------------------------------

//bool CInternalFiles::IsYFXORZFXYFile(CInternalFiles* f, CStringArray* fieldNamesIn /* = NULL */)
/*
{
  if (f == NULL)
  {
    return false;
  }

  CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(f);

  if (zfxy != NULL)
  {
    return ! zfxy->IsGeographic();
  }

  CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>(f);
  
  if (yfx == NULL)
  {
    return false;
  }

  uint32_t nbDims = yfx->GetMaxFieldNumberOfDims(fieldNamesIn);

  return (nbDims > 1);

}
*/
//----------------------------------------
bool CInternalFiles::IsZFLatLonFile(const string& fileName, CInternalFiles** pf /* = NULL */)
{
  CInternalFiles* f = Create(fileName, true, false);
  
  bool result = CInternalFiles::IsZFLatLonFile(f);

  if (pf == NULL)
  {
    CTools::DeleteObject(f);
    f = NULL;
  }
  else
  {
    *pf = f;
  }

  return result;

}
//----------------------------------------
bool CInternalFiles::IsZFLatLonFile(CInternalFiles* f)
{
  if (f == NULL)
  {
    return false;
  }

  CInternalFilesZFXY* data = dynamic_cast<CInternalFilesZFXY*>(f);

  if (data == NULL)
  {
    return false;
  }

  return data->IsGeographic();

}
//----------------------------------------
bool CInternalFiles::IsZFXYFile(const string& fileName, CStringArray* fieldNamesIn /* = NULL */, CInternalFiles** pf /* = NULL */)
{
  CInternalFiles* f = Create(fileName, true, false);
  
  bool result = CInternalFiles::IsZFXYFile(f, fieldNamesIn);

  if (pf == NULL)
  {
    CTools::DeleteObject(f);
    f = NULL;
  }
  else
  {
    *pf = f;
  }

  return result;

}

//----------------------------------------
bool CInternalFiles::IsZFXYFile(CInternalFiles* f, CStringArray* fieldNamesIn /* = NULL */)
{  

  if (f == NULL)
  {
    return false;
  }

  CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(f);

  if (zfxy != NULL)
  {
    return ! zfxy->IsGeographic();
  }

  CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>(f);
  
  if (yfx == NULL)
  {
    return false;
  }

  uint32_t nbDims = yfx->GetMaxFieldNumberOfDims(fieldNamesIn);

  return (nbDims > 1);
  

}
//----------------------------------------
bool CInternalFiles::IsYFXFile(const string& fileName, CInternalFiles** pf /* = NULL */)
{
  CInternalFiles* f = Create(fileName, true, false);
  
  bool result = CInternalFiles::IsYFXFile(f);

  if (pf == NULL)
  {
    CTools::DeleteObject(f);
    f = NULL;

  }
  else
  {
    *pf = f;
  }

  return result;

}
//----------------------------------------
bool CInternalFiles::IsYFXFile(CInternalFiles* f, CStringArray* fieldNamesIn /* = NULL */)
{

  if (f == NULL)
  {
    return false;
  }

  CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>(f);
  
  if (yfx != NULL)
  {
    return true;
  }


  CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(f);

  if (zfxy != NULL)
  {
    if (zfxy->IsGeographic())
    {
      return false;
    }
  }

  uint32_t nbDims = zfxy->GetMaxFieldNumberOfDims(fieldNamesIn);

  return (nbDims > 1);


  //bool bOk = CInternalFiles::IsYFXORZFXYFile(f, fieldNamesIn);

  //bOk |= CInternalFiles::IsZFLatLonFile(f);
  
  //return bOk;

  /*
  if (f == NULL)
  {
    return false;
  }
  CInternalFilesYFX* data = dynamic_cast<CInternalFilesYFX*>(f);
  
  return (data != NULL);
  */
}


}
