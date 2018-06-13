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

#include "netcdf.h"
#include "BratObject.h"
#include <string>
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
		(const std::string	&Name = "");

  virtual ~CExternalFilesNetCDF();

  static std::string TypeOf() { return "unknown"; };

  virtual std::string GetName() const;
  virtual void SetName(const std::string	&Name);

  virtual void SetMode(brathl_FileMode mode);

  virtual void Open();
  virtual void Close ();

  virtual bool IsOpened()	const;

  virtual bool IsAxisVar (const std::string& name);

  virtual int32_t NumberOfRecords ();

  virtual void GetFieldNames (CStringArray& names);

  // If WantedUnit is "", the returned value is in base (SI) unit.
  virtual void GetValue(const std::string& name, CExpressionValue &value, const std::string	&wantedUnit);
  
  // If WantedUnit is "", the returned value is in base (SI) unit.
  virtual void GetValue(const std::string &name, double	&value, const std::string &wantedUnit);

  virtual void GetAllValues(const std::string& name, CExpressionValue& value, const std::string& wantedUnit);

  virtual void GetAllValues(const std::string& name, CDoubleArray& vect, const std::string& wantedUnit);

  virtual void GetAllValues(CFieldNetCdf* field, CExpressionValue& value, const std::string& wantedUnit);

  virtual void GetAllValues(CFieldNetCdf* field, const std::string& wantedUnit);

  virtual void GetValues(const std::string& name, CExpressionValue &value, const std::string	&wantedUnit);

  virtual void GetValues(CFieldNetCdf* field,  CExpressionValue &value, const std::string	&wantedUnit);


  void ExecuteExpression(CExpression &expr, CExpressionValue& exprValue, const std::string& wantedUnit, CProduct* product = NULL);


  virtual CObMap* GetFields() { return &m_varList; };
//  virtual CObMap* GetFieldsToRead() { return &m_varListToRead; };

  virtual bool VarExists(const std::string& name);

  virtual void AddAttributesAsField(CFieldNetCdf* field = NULL);

  virtual void GetVariables (CStringArray& varNames);

  virtual void GetDimensions (const std::string& varName, CUIntArray& dimensions);
  virtual void GetDimensions(const std::string& varName, CStringArray& dimensions);



//  virtual CExternalFileFieldDescription* GetFieldDescription
//		(const std::string& name);

  CFieldNetCdf* GetFieldNetCdf(const std::string &name, bool withExcept = true);
  
  int32_t GetNetCdfId(const std::string &name, bool withExcept = true);

  nc_type GetVarType(const std::string &name);

  virtual std::string GetVarTypeName(const std::string &name);

  CIntMap& GetDimIds() { return m_dimIds; };
  CUIntMap& GetDimValues() { return m_dimValues; };

  CFieldNetCdf* GetVarByAttribute(const std::string& attrName, const std::string& attrValueToSearch);

  // Get global attributes and set them into a std::string map.
  void GetGlobalAttributes(CStringMap& mapAttributes);
  // Get global attributes and set them into a double map.
  void GetGlobalAttributes(CDoubleMap& mapAttributes);
  // Get global attributes et set them into a std::string.
  void GetGlobalAttributes(std::string& attributes);

  int GetGlobalAttribute
		(const std::string	&attName,
		       double	&attValue,
		       bool	mustExist	= true,
		       double   defaultValue	= CTools::m_defaultValueDOUBLE);

  int GetGlobalAttribute
		(const std::string	&attName,
		       std::string	&attValue,
		       bool	mustExist	= true,
		       std::string   defaultValue	= "");
  
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

  nc_type GetAttributeType(const std::string& attName);
  nc_type GetAttributeType(const std::string& varName, const std::string& attName);


  void GetOrderedDimNames(const std::string& value, CStringArray& commonDimensionNames);
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

  //virtual void ReadEntireVar(CNetCDFVarDef* netCDFVarDef, CDoubleArray& array, const std::string& wantedUnit);
  //virtual void ReadEntireVar(CNetCDFDimension* netCDFDimension, CDoubleArray& array, const std::string& wantedUnit);

protected:
  CNetCDFFiles	m_file;
  uint32_t	m_nbMeasures;

  //map<std::string, CRegisteredVar>	m_varList;
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
		       const std::string	&Name,
		       const std::string	&Description,
		       const std::string	&Unit,
           int32_t type = NC_NAT,
           const CUIntArray* dimValues = NULL,
           const CStringArray* dimNames = NULL,
           const CIntArray* dimIds = NULL,
           const CStringMap* mapAttributes = NULL);

  virtual void AddVar
		      (int32_t	netcdfId,
		       const std::string	&name,
		       const std::string	&description,
		       const std::string	&unit,
           int32_t type,
           uint32_t dimValue,
           const std::string dimName, 
           int32_t dimId,
           const CStringMap* mapAttributes = NULL);


  // Add a variable in the variable list got from description in file
  virtual void AddVar(const std::string	&Name);

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
		(const std::string	&name = "");

  virtual ~CExternalFilesNetCDFCF();

  static std::string TypeOf() { return "unknown"; };

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
class CExternalFilesNetCDFCFGenericStandard : public CExternalFilesNetCDFCF
{
	DECLARE_BASE_TYPE( CExternalFilesNetCDFCF )
public:
	CExternalFilesNetCDFCFGenericStandard( const std::string &name = "" );

  virtual ~CExternalFilesNetCDFCFGenericStandard();

  static std::string TypeOf() { return GENERIC_NETCDF_TYPE_STANDARD; }
  virtual std::string GetType() { return TypeOf(); }
};



class CExternalFilesNetCDFCFGenericVariant1 : public CExternalFilesNetCDFCF
{
	DECLARE_BASE_TYPE( CExternalFilesNetCDFCF )
public:
	CExternalFilesNetCDFCFGenericVariant1( const std::string &name = "" );

	virtual ~CExternalFilesNetCDFCFGenericVariant1();

	static std::string TypeOf() { return GENERIC_NETCDF_TYPE_VARIANT_1; }
	virtual std::string GetType() { return TypeOf(); }
};



class CExternalFilesRads : public CExternalFilesNetCDFCF
{
	DECLARE_BASE_TYPE( CExternalFilesNetCDFCF )

	const std::string mMission;

public:
	CExternalFilesRads( const std::string &mission, const std::string &name = "" )
		: base_t( name )
		, mMission( mission )
	{}

	virtual ~CExternalFilesRads()
	{}

	static std::string TypeOf() { return "RADS"; }

	static bool IsTypeOf( const std::string& type_str ) { return 0 == type_str.find( CExternalFilesRads::TypeOf() );	}

	virtual std::string GetType() { return TypeOf() + "-" + mMission; }
};



class CExternalFilesSentinel3 : public CExternalFilesNetCDFCF
{
    DECLARE_BASE_TYPE( CExternalFilesNetCDFCF )

public:
	CExternalFilesSentinel3( const std::string	&name = "" )
		: base_t( name )
	{}

	virtual ~CExternalFilesSentinel3()
	{}

    static std::string TypeOf() { return "Sentinel 3"; }
    virtual std::string GetType() { return TypeOf(); }
};


class CExternalFilesReaper : public CExternalFilesNetCDFCF
{
    DECLARE_BASE_TYPE( CExternalFilesNetCDFCF )

public:
	CExternalFilesReaper( const std::string	&name = "" )
		: base_t( name )
	{}

	virtual ~CExternalFilesReaper()
	{}

    static std::string TypeOf() { return "REAPER/ERS_ALT_2"; }
    virtual std::string GetType() { return TypeOf(); }
};


class CExternalFilesGeosatGDR : public CExternalFilesNetCDFCF
{
    DECLARE_BASE_TYPE( CExternalFilesNetCDFCF )

public:
    CExternalFilesGeosatGDR( const std::string	&name = "" )
        : base_t( name )
    {}

    virtual ~CExternalFilesGeosatGDR()
    {}

    static std::string TypeOf() { return "Geosat_GDR"; }
    virtual std::string GetType() { return TypeOf(); }
};


class CExternalFilesJason1GPN2P : public CExternalFilesNetCDFCF
{
    DECLARE_BASE_TYPE( CExternalFilesNetCDFCF )

public:
    CExternalFilesJason1GPN2P( const std::string	&name = "" )
        : base_t( name )
    {}

    virtual ~CExternalFilesJason1GPN2P()
    {}

    static std::string TypeOf() { return "Jason1_GPN_2P"; }
    virtual std::string GetType() { return TypeOf(); }
};


class CExternalFilesJason1GPS2P : public CExternalFilesNetCDFCF
{
    DECLARE_BASE_TYPE( CExternalFilesNetCDFCF )

public:
    CExternalFilesJason1GPS2P( const std::string	&name = "" )
        : base_t( name )
    {}

    virtual ~CExternalFilesJason1GPS2P()
    {}

    static std::string TypeOf() { return "Jason1_GPS_2P"; }
    virtual std::string GetType() { return TypeOf(); }
};


class CExternalFilesEnvisat2P : public CExternalFilesNetCDFCF
{
    DECLARE_BASE_TYPE( CExternalFilesNetCDFCF )

public:
    CExternalFilesEnvisat2P( const std::string	&name = "" )
        : base_t( name )
    {}

    virtual ~CExternalFilesEnvisat2P()
    {}

    static std::string TypeOf() { return "Envisat_2P"; }
    virtual std::string GetType() { return TypeOf(); }
};


class CExternalFilesSentinel3_enhanced : public CExternalFilesSentinel3
{
    DECLARE_BASE_TYPE( CExternalFilesSentinel3 )

public:
    CExternalFilesSentinel3_enhanced( const std::string	&name = "" )
        : base_t( name )
    {}

    virtual ~CExternalFilesSentinel3_enhanced()
    {}

    static std::string TypeOf() { return "S3/SR_2_ENH"; }
    virtual std::string GetType() { return TypeOf(); }
};



class CExternalFilesSentinel3_standard : public CExternalFilesSentinel3
{
    DECLARE_BASE_TYPE( CExternalFilesSentinel3 )

public:
    CExternalFilesSentinel3_standard( const std::string	&name = "" )
        : base_t( name )
    {}

    virtual ~CExternalFilesSentinel3_standard()
    {}

    static std::string TypeOf() { return "S3/SR_2_STD"; }
    virtual std::string GetType() { return TypeOf(); }
};



class CExternalFilesSentinel3_reduced : public CExternalFilesSentinel3
{
    DECLARE_BASE_TYPE( CExternalFilesSentinel3 )

public:
    CExternalFilesSentinel3_reduced( const std::string	&name = "" )
        : base_t( name )
    {}

    virtual ~CExternalFilesSentinel3_reduced()
    {}

    static std::string TypeOf() { return "S3/SR_2_RED"; }
    virtual std::string GetType() { return TypeOf(); }
};


class CExternalFilesSentinel3_l1b : public CExternalFilesSentinel3
{
    DECLARE_BASE_TYPE( CExternalFilesSentinel3 )

public:
    CExternalFilesSentinel3_l1b( const std::string	&name = "" )
        : base_t( name )
    {}

    virtual ~CExternalFilesSentinel3_l1b()
    {}

    static std::string TypeOf() { return "S3/SR_1_B"; }
    virtual std::string GetType() { return TypeOf(); }
};


class CExternalFilesSentinel3_l1a : public CExternalFilesSentinel3
{
    DECLARE_BASE_TYPE( CExternalFilesSentinel3 )

public:
    CExternalFilesSentinel3_l1a( const std::string	&name = "" )
        : base_t( name )
    {}

    virtual ~CExternalFilesSentinel3_l1a()
    {}

    static std::string TypeOf() { return "S3/SR_1_A"; }
    virtual std::string GetType() { return TypeOf(); }
};


class CExternalFilesSentinel3_l1bs : public CExternalFilesSentinel3
{
    DECLARE_BASE_TYPE( CExternalFilesSentinel3 )

public:
    CExternalFilesSentinel3_l1bs( const std::string	&name = "" )
        : base_t( name )
    {}

    virtual ~CExternalFilesSentinel3_l1bs()
    {}

    static std::string TypeOf() { return "S3/SR_1_BS"; }
    virtual std::string GetType() { return TypeOf(); }
};




//-------------------------------------------------------------
//------------------- CExternalFilesYFX class -----
//-------------------------------------------------------------
class CExternalFilesYFX : public CExternalFilesNetCDFCF
{
public:
  CExternalFilesYFX
		(const std::string	&name = "");

  virtual ~CExternalFilesYFX();

  static std::string TypeOf() { return YFX_NETCDF_TYPE; }
  virtual std::string GetType() { return TypeOf(); }


protected:

};
//-------------------------------------------------------------
//------------------- CExternalFilesZFXY class -----
//-------------------------------------------------------------
class CExternalFilesZFXY : public CExternalFilesNetCDFCF
{
public:
  CExternalFilesZFXY
		(const std::string	&name = "");

  virtual ~CExternalFilesZFXY();

  static std::string TypeOf() { return ZFXY_NETCDF_TYPE; }
  virtual std::string GetType() { return TypeOf(); }


protected:

};

/** @} */

}

#endif // Already included .h
