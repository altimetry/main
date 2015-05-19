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
#ifndef _ExternalFiles_h_
#define _ExternalFiles_h_

#include <netcdf.h>
#include "BratObject.h"
#include "Stl.h"
#include "Expression.h"
#include "List.h" 

#include "Field.h" 
using namespace brathl;

namespace brathl
{

/** \addtogroup tools Tools
  @{ */

/**
  External files access.



 \version 1.0
*/
const string UNKNOWN_PRODUCT_CLASS = "UNKNOWN";
const string NETCDF_PRODUCT_CLASS = "NETCDF";
const string NETCDF_CF_PRODUCT_CLASS = "NETCDF_CF";

// Sentinel indicating first access to file (or after a rewind)
// With this we are sure the value is greater than any valid number of pass
#define AT_BEGINNING		0xFFFFFFFFUL



//-------------------------------------------------------------
//------------------- CExternalFiles class --------------------
//-------------------------------------------------------------

class CExternalFiles : public CBratObject
{
public:
  CExternalFiles
		();

  virtual ~CExternalFiles();

  static string TypeOf() { return "unknown"; };

  virtual string GetName() const = 0;
  virtual void SetName
		(const string		&Name) = 0;

  virtual void Open() = 0;
  
  virtual void Close() { };

  virtual bool IsOpened()	const	= 0;

  virtual string GetType() = 0;

  virtual void GetFieldNames(CStringArray& names) = 0;

//  virtual CExternalFileFieldDescription* GetFieldDescription
//		(const string& name) = 0;
  
  virtual CObMap* GetFields() = 0;

  virtual void GetAllValues(const string& name, CExpressionValue& value, const string& wantedUnit) = 0;

  virtual void GetAllValues(CFieldNetCdf* field, CExpressionValue& value, const string& wantedUnit) = 0;

  virtual void GetAllValues(const string& name, CDoubleArray& vect, const string& wantedUnit) = 0;

  virtual void GetAllValues(CFieldNetCdf* field, const string& wantedUnit) = 0;
  
  // If WantedUnit is "", the returned value is in base (SI) unit.
  virtual void GetValue(const string& name, CExpressionValue &value, const string	&wantedUnit) = 0;
  
  // If WantedUnit is "", the returned value is in base (SI) unit.
  virtual void GetValue(const string &name, double	&value, const string &wantedUnit) = 0;

  virtual void GetValues(const string& name, CExpressionValue &value, const string	&wantedUnit) = 0;

  virtual void GetValues(CFieldNetCdf* field, CExpressionValue &value, const string	&wantedUnit) = 0;



  virtual void Rewind () = 0;

  virtual int32_t NumberOfRecords () = 0;

  // True while there is something to read
  virtual bool NextRecord () = 0;
  virtual bool PrevRecord () = 0;


  virtual string GetVarTypeName(const string &name) = 0;

  virtual string GetProductClass() {return m_productClass;};
  
  bool IsUnknownProduct() {return (m_productClass.compare(UNKNOWN_PRODUCT_CLASS) == 0); };
  bool IsNetCdfProduct() {return (m_productClass.compare(NETCDF_PRODUCT_CLASS) == 0); };
  bool IsNetCdfCFProduct() {return (m_productClass.compare(NETCDF_CF_PRODUCT_CLASS) == 0); };

  virtual bool IsAxisVar (const string& name) = 0;

  virtual bool VarExists(const string& name) = 0;
  
  virtual void GetVariables (CStringArray& varNames) = 0;

  virtual void AddAttributesAsField(CFieldNetCdf* field) = 0;

  virtual void GetDimensions (const string& varName, CUIntArray& dimensions) = 0;
  virtual void GetDimensions(const string& varName, CStringArray& dimensions) = 0;

  // throws an exception if file is not opened
  void MustBeOpened();

  static CField* GetField(CBratObject* ob, bool withExcept = true);
  static CFieldNetCdf* GetFieldNetCdf(CBratObject* ob, bool withExcept = true);
  static CFieldNetCdfIndexData* GetFieldNetCdfIndexData(CBratObject* ob, bool withExcept = true);

  static bool IsExcludedAttrAsField(const string& name);


  virtual CFieldNetCdf* FindTimeField() = 0;
  virtual CFieldNetCdf* FindLatField()  = 0;
  virtual CFieldNetCdf* FindLonField()  = 0;
  virtual CFieldNetCdf* FindPassField()  = 0;
  virtual CFieldNetCdf* FindCycleField()  = 0;

  virtual void SetOffset(double value) { m_offset = value; };
  virtual void AddOffset(double value);
  double GetOffset() { return m_offset; };

public:
  static const int32_t m_EXCLUDE_ATTR_AS_FIELD_SIZE;
  static const string m_EXCLUDE_ATTR_AS_FIELD[];


protected:
  string m_productClass;		
/*
  static const int32_t m_TIME_NAMES_SIZE;
  static const string m_TIME_NAMES[];

  static const int32_t m_LAT_NAMES_SIZE;
  static const string m_LAT_NAMES[];

  static const int32_t m_LON_NAMES_SIZE;
  static const string m_LON_NAMES[];

  static const int32_t m_CYCLE_NAMES_SIZE;
  static const string m_CYCLE_NAMES[];

  static const int32_t m_PASS_NAMES_SIZE;
  static const string m_PASS_NAMES[];
*/

  double m_offset;

protected:
   virtual void SetProductClass(const string& value) {m_productClass = value;};
   
   void Init();

/*
  // Assign data from 'Data' to 'Vector'.
  // If ExactSize is true, Vector size is 'Count' upon exit. Otherwise,
  // Vector size is greater or equal.
  void SetVector
		(CDoubleArray& Vector,
		 double			*Data,
		 uint32_t		Count,
		 bool			ExactSize	= false);
 */ 
/*  
  void InsertVector
		(CDoubleArray &vector,
		 double	*data,
		 uint32_t count);
*/
  //static CExternalFileFieldDescription* GetRegisteredVarDescription(CBratObject* ob, bool withExcept = true);



};



/** @} */

}

#endif // Already included .h
