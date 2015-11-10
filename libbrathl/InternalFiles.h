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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	 02110-1301, USA.
*/
#ifndef _InternalFiles_h_
#define _InternalFiles_h_

#include <string>

#include "netcdf.h"
#include "BratObject.h"
#include "Expression.h"

#include "NetCDFFiles.h"

namespace brathl
{

/** \addtogroup tools Tools
  @{ */

/**
  Internal files access.



 \version 1.0
*/


class CInternalFiles : public CBratObject
{
public:
  CInternalFiles
		(std::string			Name = "",
		 brathl_FileMode	Mode = ReadOnly);

  virtual ~CInternalFiles();

  static std::string TypeOf() { return "unknown"; };

  virtual std::string GetType
		();		// Abstract method must be defined in all derived class

  int32_t GetVarDimIndex
		(const std::string& varName,
                 const std::string& dimName);


  int GetDimId(const std::string& name);

  virtual std::string GetName() const;

  virtual void SetName(const std::string& name);
  virtual void SetMode(brathl_FileMode mode);

  virtual void Open(brathl_FileMode mode);

  virtual void Open();

  virtual void Close();

  virtual bool IsOpened();

  virtual void WriteFileTitle	(const std::string& Title);

  virtual bool IsGeographic() { return false; };

  bool IsAxisVar(const std::string	&Name);

  virtual void GetVariables(std::vector<std::string>	&VarNames);

  virtual void GetAxisVars(std::vector<std::string>	&VarNames);
  virtual void GetDataVars(std::vector<std::string> &VarNames);

  virtual NetCDFVarKind GetVarKind(const std::string& Name);

  virtual void GetVarDims(const std::string& Name, ExpressionValueDimensions	&Dimensions);

  virtual void GetVarDims(const std::string& Name, std::vector<std::string>			&Dimensions);

  virtual bool GetCommonVarDims(const std::string& varName1, const std::string& varName2, CStringArray& intersect);

  virtual bool GetComplementVarDims(const std::string& varName1, const std::string& varName2, CStringArray& complement);

  virtual bool GetComplementVars(const CStringArray& varNames, CStringArray& complement, bool excludeDim = true );

  virtual bool VarExists(const std::string& Name);

  virtual bool HasVar(NetCDFVarKind		VarKind);

  virtual void WriteVar
		(const std::string		&Name,
		 const CExpressionValue	&Value);

  virtual void ReadVar
		(const std::string			&Name,
		       CExpressionValue		&Value,
		 const std::string			&WantedUnit);

  virtual CUnit GetUnit
		(const std::string			&Name);

  // If name is empty, it is the title of the file which is returned
  // (global attribute 'title' instead of 'long_name' of the variable)
  virtual std::string GetTitle
		(const std::string			&Name);

  static bool IsVarNameValid
		(const std::string	&Name);

  int GetAttribute
		(const std::string	&varName,
		 const std::string	&attName,
		       double	&attValue,
		       bool	mustExist	= true,
		       double   defaultValue	= CTools::m_defaultValueDOUBLE);
  int GetAttribute
		(const std::string	&varName,
		 const std::string	&attName,
		       std::string	&attValue,
		       bool	mustExist	= true,
		       std::string   defaultValue	= "");


  std::string GetComment(const std::string& varName);

  uint32_t GetMaxFieldNumberOfDims(const CStringArray* fieldNames = NULL) {return m_file.GetMaxFieldNumberOfDims(fieldNames); };


  CObMap* GetNetCDFDims() {return m_file.GetNetCDFDims(); };
  CNetCDFDimension* AddNetCDFDim(CNetCDFDimension& dim, bool forceReplace = false) { return m_file.AddNetCDFDim(dim, forceReplace); };

  CNetCDFDimension* GetNetCDFDim(const std::string& name) {return  m_file.GetNetCDFDim(name); };
  
  void GetNetCDFDims(const std::string& varName, CObArray* dims) { m_file.GetNetCDFDims(varName, dims); };

  CObMap* GetNetCDFVarDefs() {return m_file.GetNetCDFVarDefs(); };
  CNetCDFVarDef* AddNetCDFVarDef(CNetCDFVarDef& var, bool forceReplace = false) { return m_file.AddNetCDFVarDef(var, forceReplace); };

  CNetCDFVarDef* GetNetCDFVarDef(const std::string& name) {return  m_file.GetNetCDFVarDef(name); };



  virtual void WriteDimensions();
  virtual void WriteVariables();
  virtual void WriteData(CNetCDFVarDef* varDef, CExpressionValue* data);
  virtual void WriteData(CNetCDFVarDef* varDef, CMatrix* matrix);

  void ReplaceNetCDFDim(CNetCDFDimension& dim);

  CNetCDFFiles*	GetFile() { return &m_file; };


  static bool IsZFLatLonFile(const std::string& fileName, CInternalFiles** pf = NULL);
  static bool IsZFLatLonFile(CInternalFiles* f);

  //static bool IsYFXORZFXYFile(const std::string& fileName, CStringArray* fieldNames = NULL, CInternalFiles** pf = NULL);
  //static bool IsYFXORZFXYFile(CInternalFiles* f, CStringArray* fieldNamesIn = NULL);

  static bool IsZFXYFile(const std::string& fileName, CStringArray* fieldNames = NULL, CInternalFiles** pf = NULL);
  static bool IsZFXYFile(CInternalFiles* f, CStringArray* fieldNames = NULL);

  static bool IsYFXFile(const std::string& fileName, CInternalFiles** pf = NULL);
  static bool IsYFXFile(CInternalFiles* f, CStringArray* fieldNamesIn = NULL);

  static CInternalFiles* Create(const std::string& fileName, bool open = true, bool withExcept = true);


protected:
  CNetCDFFiles	m_file;

  void SetFixedGlobalAttributes
		(void);

};

/** @} */

}

#endif // Already included .h
