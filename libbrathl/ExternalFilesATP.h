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
#ifndef _ExternalFilesATP_h_
#define _ExternalFilesATP_h_

#include <netcdf.h>
#include "BratObject.h"
#include "Stl.h"
#include "ExternalFilesNetCDF.h"

namespace brathl
{

/** \addtogroup tools Tools
  @{ */

/**
  External files access.



 \version 1.0
*/


//-------------------------------------------------------------
//------------------- CRegisteredPass class --------------------
//-------------------------------------------------------------

class CRegisteredPass : public CBratObject 
{
public:

  CRegisteredPass() {};
  
  CRegisteredPass(CRegisteredPass& p) 
  {
    Set(p);
  }

  virtual ~CRegisteredPass() {}; 

  void Set(CRegisteredPass& p) 
  {
    m_cycle = p.m_cycle;
    m_cycleIndex = p.m_cycleIndex;
    m_pass = p.m_pass;
    m_nbData = p.m_nbData;
    m_startPoint = p.m_startPoint;
    m_beginDate = p.m_beginDate;
  }

  const CRegisteredPass& operator= (CRegisteredPass& p)
  {
    Set(p);    
    return *this;    
  }


public:
  uint32_t m_cycle;
  uint32_t m_cycleIndex;
  uint32_t m_pass;
  uint32_t m_nbData;
  uint32_t m_startPoint;
  double m_beginDate; // expressed in SI unit ==> seconds since 1950-01-01 00:00:00.0
};

//-------------------------------------------------------------
//------------------- CExternalFilesATP class --------------------
//-------------------------------------------------------------


class CExternalFilesATP : public CExternalFilesNetCDFCF
{
public:
  CExternalFilesATP
		(const string		&Name = "");

  virtual ~CExternalFilesATP() { };

  static string TypeOf() { return CExternalFilesATP::m_ALONG_TRACK_PRODUCT; };
  virtual string GetType() { return TypeOf(); };

  virtual void Rewind();

  // True while there is something to read
  virtual bool NextRecord();
  virtual bool PrevRecord();

  // If WantedUnit is "", the returned value is in base (SI) unit.
  virtual void GetValue
		(const string		&Name,
		 CExpressionValue	&Value,
		 const string		&WantedUnit);

  // If WantedUnit is "", the returned value is in base (SI) unit.
  virtual void GetValue
		(const string &name,
		 double	&value,
		 const string &wantedUnit);

  virtual void GetAllValues
		(const string		&name,
		 CExpressionValue	&value,
		 const string		&wantedUnit);

  virtual void GetAllValues
		(const string& name,
		 CDoubleArray& array,
		 const string& wantedUnit);


  virtual void GetAllValues(CFieldNetCdf* field, const string& wantedUnit);

  virtual void GetValues(const string& name, CExpressionValue &value, const string	&wantedUnit);

  static CRegisteredPass* GetRegisteredPass(CBratObject* ob, bool withExcept = true);


protected:
  // On open retrieve the structure information and register it
  virtual void LoadStructure();

  // On open retrieve the name of user variables and system variables
  // (system variables are those defined by the structure itself: latitude
  // longitude, date...)
  virtual void CheckVariables();

  void AddVirtualVariables();

  // On close free all allocated resources in order to have a fresh object (able to reopen)
  virtual void FreeResources
		();

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
  
  // Add a variable in the variable list got from description in file
  virtual void AddVar(const string	&Name);

  virtual void AddBratIndexData() {};

private:

// File structure and global informations
  double			m_deltaT;
  //vector<CRegisteredPass>	m_passList;
  CObArray m_passList;

// Current loaded Pass
  uint32_t		m_currentPass;
  uint32_t		m_currentInPass;

  CNetCDFDimension m_dataDim;
  CUIntArray m_dataDimValues;
  CStringArray m_dataDimNames;
  CIntArray m_dataDimIds;

// Callback for 'sort' algorithm' in order to sort passes in chronological
// order: Sort on cycle number then on pass number
  static bool PassCompare
		(CBratObject* T1,
		 CBratObject* T2);

  // Return false if current Pass does not exists (end of data reached)
  bool LoadPass(bool resetValues = false);

  //bool LoadPass(const string& name, bool resetValues = false);
  bool LoadPass(CFieldNetCdf* var, bool resetValues = false);

public:

  static const string m_ALONG_TRACK_PRODUCT;
  static const string m_DATE_UNIT;
  static const string m_DATA_DIM_NAME;

};

/** @} */

}

#endif // Already included .h
