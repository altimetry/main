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
#ifndef _ExternalFilesNetCDF_h_
#define _ExternalFilesNetCDF_h_

#include <netcdf.h>
#include "BratObject.h"
#include "Stl.h"
#include "Expression.h"
#include "ExternalFiles.h"
#include "List.h"

#include "Field.h" 
using namespace brathl;

namespace brathl
{

/** \addtogroup tools Tools
  @{ */

/**
  External NetCdf files access.



 \version 1.0
*/



//-------------------------------------------------------------
//------------------- CExternalFilesNetCDF class -----
//-------------------------------------------------------------
class CExternalFilesNetCDF : public CExternalFiles
{
public:
  CExternalFilesNetCDF
		(const string	&Name = "");

  virtual ~CExternalFilesNetCDF();

  static string TypeOf() { return "unknown"; };

  virtual string GetName() const;
  virtual void SetName(const string	&Name);

  virtual void SetMode(brathl_FileMode mode);

  virtual void Open();
  virtual void Close ();

  virtual bool IsOpened()	const;

  virtual bool IsAxisVar (const string& name);

  virtual int32_t NumberOfRecords ();

  virtual void GetFieldNames (CStringArray& names);

  // If WantedUnit is "", the returned value is in base (SI) unit.
  virtual void GetValue(const string& name, CExpressionValue &value, const string	&wantedUnit);
  
  // If WantedUnit is "", the returned value is in base (SI) unit.
  virtual void GetValue(const string &name, double	&value, const string &wantedUnit);

  virtual void GetAllValues(const string& name, CExpressionValue& value, const string& wantedUnit);

  virtual void GetAllValues(const string& name, CDoubleArray& vect, const string& wantedUnit);

  virtual void GetAllValues(CFieldNetCdf* field, CExpressionValue& value, const string& wantedUnit);

  virtual void GetAllValues(CFieldNetCdf* field, const string& wantedUnit);

  virtual void GetValues(const string& name, CExpressionValue &value, const string	&wantedUnit);

  virtual void GetValues(CFieldNetCdf* field,  CExpressionValue &value, const string	&wantedUnit);


  void ExecuteExpression(CExpression &expr, CExpressionValue& exprValue, const string& wantedUnit, CProduct* product = NULL);


  virtual CObMap* GetFields() { return &m_varList; };
//  virtual CObMap* GetFieldsToRead() { return &m_varListToRead; };

  virtual bool VarExists(const string& name);

  virtual void AddAttributesAsField(CFieldNetCdf* field = NULL);

  virtual void GetVariables (CStringArray& varNames);

  virtual void GetDimensions (const string& varName, CUIntArray& dimensions);
  virtual void GetDimensions(const string& varName, CStringArray& dimensions);



//  virtual CExternalFileFieldDescription* GetFieldDescription
//		(const string& name);

  CFieldNetCdf* GetFieldNetCdf(const string &name, bool withExcept = true);
  
  int32_t GetNetCdfId(const string &name, bool withExcept = true);

  nc_type GetVarType(const string &name);

  virtual string GetVarTypeName(const string &name);

  CIntMap& GetDimIds() { return m_dimIds; };
  CUIntMap& GetDimValues() { return m_dimValues; };

  CFieldNetCdf* GetVarByAttribute(const string& attrName, const string& attrValueToSearch);

  // Get global attributes and set them into a string map.
  void GetGlobalAttributes(CStringMap& mapAttributes);
  // Get global attributes and set them into a double map.
  void GetGlobalAttributes(CDoubleMap& mapAttributes);
  // Get global attributes et set them into a string.
  void GetGlobalAttributes(string& attributes);

  int GetGlobalAttribute
		(const string	&attName,
		       double	&attValue,
		       bool	mustExist	= true,
		       double   defaultValue	= CTools::m_defaultValueDOUBLE);

  int GetGlobalAttribute
		(const string	&attName,
		       string	&attValue,
		       bool	mustExist	= true,
		       string   defaultValue	= "");
  
  int GetAttribute
		(const string	&varName,
		 const string	&attName,
		       double	&attValue,
		       bool	mustExist	= true,
		       double   defaultValue	= CTools::m_defaultValueDOUBLE);
  int GetAttribute
		(const string	&varName,
		 const string	&attName,
		       string	&attValue,
		       bool	mustExist	= true,
		       string   defaultValue	= "");

  nc_type GetAttributeType(const string& attName);
  nc_type GetAttributeType(const string& varName, const string& attName);


  void GetOrderedDimNames(const string& value, CStringArray& commonDimensionNames);
  void GetOrderedDimNames(const CExpression& value, CStringArray& commonDimensionNames);
  void GetOrderedDimNames(const CStringArray* fieldNames, CStringArray& commonDimensionNames);

  void GetOrderedDimNamesFromFieldNetcdf(const CStringArray* fieldNames,CStringArray& commonDimensionNames);

  bool IsLatField(CFieldNetCdf* field);
  bool IsLonField(CFieldNetCdf* field);


  virtual CFieldNetCdf* FindTimeField();
  virtual CFieldNetCdf* FindLatField();
  virtual CFieldNetCdf* FindLonField();
  virtual CFieldNetCdf* FindPassField();
  virtual CFieldNetCdf* FindCycleField();
  
  CNetCDFFiles*	GetFile() { return &m_file; };

  virtual void SetOffset(double value, bool force = false);
  virtual void AddOffset(double value, bool force = false);

  //virtual void ReadEntireVar(CNetCDFVarDef* netCDFVarDef, CDoubleArray& array, const string& wantedUnit);
  //virtual void ReadEntireVar(CNetCDFDimension* netCDFDimension, CDoubleArray& array, const string& wantedUnit);

protected:
  CNetCDFFiles	m_file;
  uint32_t	m_nbMeasures;

  //map<string, CRegisteredVar>	m_varList;
  CObMap m_varList;
  //CObMap m_varListToRead;

  //CUIntArray m_nbDimsArray;
  //map of NetCdf file dimension name and id (key : dim name --> dim id)
  CIntMap m_dimIds;

  //map of NetCdf file dimension (key : dim name --> dim value)
  CUIntMap m_dimValues;

  /** Array of the global dimension's index 
   */
//  CUIntArray m_dimsIndexArray;


protected:


  // On open retrieve the structure information and register it
  virtual void LoadStructure () = 0;
  // On open retrieve the name of user variables and system variables
  // (system variables are those defined by the structure itself: latitude
  // longitude, date...)
  virtual void CheckVariables();

  // Retrieves the dimensions of the NetCdfFile.
  virtual void CheckDimensions ();

  // On close free all allocated resources in order to have a fresh object (able to reopen)
  virtual void FreeResources();

  virtual void SubstituteDimNames(CStringArray& dimNames);

  // Add a variable in the variable list
  virtual void AddVar
		      (int32_t	NetcdfId,
		       const string	&Name,
		       const string	&Description,
		       const string	&Unit,
           int32_t type = NC_NAT,
           const CUIntArray* dimValues = NULL,
           const CStringArray* dimNames = NULL,
           const CIntArray* dimIds = NULL,
           const CStringMap* mapAttributes = NULL);

  virtual void AddVar
		      (int32_t	netcdfId,
		       const string	&name,
		       const string	&description,
		       const string	&unit,
           int32_t type,
           uint32_t dimValue,
           const string dimName, 
           int32_t dimId,
           const CStringMap* mapAttributes = NULL);


  // Add a variable in the variable list got from description in file
  virtual void AddVar(const string	&Name);

  virtual void AddBratIndexData();

  void SetOffset(bool force = false);


};

//-------------------------------------------------------------
//------------------- CExternalFilesNetCDFCF class -----
//-------------------------------------------------------------
class CExternalFilesNetCDFCF : public CExternalFilesNetCDF
{
public:
  CExternalFilesNetCDFCF
		(const string	&name = "");

  virtual ~CExternalFilesNetCDFCF();

  static string TypeOf() { return "unknown"; };

  virtual void Rewind ();

  virtual bool NextRecord ();
  virtual bool PrevRecord ();

  virtual void LoadStructure ();


protected:
    // On close free all allocated resources in order to have a fresh object (able to reopen)
  virtual void FreeResources();



};
//-------------------------------------------------------------
//------------------- CExternalFilesNetCDFCFGeneric class -----
//-------------------------------------------------------------
class CExternalFilesNetCDFCFGeneric : public CExternalFilesNetCDFCF
{
public:
  CExternalFilesNetCDFCFGeneric
		(const string	&name = "");

  virtual ~CExternalFilesNetCDFCFGeneric();

  static string TypeOf() { return GENERIC_NETCDF_TYPE; };
  virtual string GetType() { return TypeOf(); };


protected:

};

//-------------------------------------------------------------
//------------------- CExternalFilesYFX class -----
//-------------------------------------------------------------
class CExternalFilesYFX : public CExternalFilesNetCDFCF
{
public:
  CExternalFilesYFX
		(const string	&name = "");

  virtual ~CExternalFilesYFX();

  static string TypeOf() { return YFX_NETCDF_TYPE; };
  virtual string GetType() { return TypeOf(); };


protected:

};
//-------------------------------------------------------------
//------------------- CExternalFilesZFXY class -----
//-------------------------------------------------------------
class CExternalFilesZFXY : public CExternalFilesNetCDFCF
{
public:
  CExternalFilesZFXY
		(const string	&name = "");

  virtual ~CExternalFilesZFXY();

  static string TypeOf() { return ZFXY_NETCDF_TYPE; };
  virtual string GetType() { return TypeOf(); };


protected:

};

/** @} */

}

#endif // Already included .h
