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
#ifndef _ExternalFilesAvisoGrid_h_
#define _ExternalFilesAvisoGrid_h_

#include "netcdf.h"
#include "BratObject.h"
#include <string>
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
//------------------- CExternalFilesAvisoGrid class --------------------
//-------------------------------------------------------------

class CExternalFilesAvisoGrid : public CExternalFilesNetCDFCF
{
public:
  CExternalFilesAvisoGrid
		(const std::string		&Name = "");

  virtual ~CExternalFilesAvisoGrid() { };

  static std::string TypeOf() { return "unknown"; };

  virtual void Rewind
		();

  // True while there is something to read
  virtual bool NextRecord();
  virtual bool PrevRecord();

  // If WantedUnit is "", the returned value is in base (SI) unit.
  virtual void GetValue
		(const std::string		&Name,
		 CExpressionValue	&Value,
		 const std::string		&WantedUnit);

  // If WantedUnit is "", the returned value is in base (SI) unit.
  virtual void GetValue
		(const std::string &name,
		 double	&value,
		 const std::string &wantedUnit);

  //virtual void ReadEntireVar(CNetCDFVarDef* netCDFVarDef, CDoubleArray& array, const std::string& wantedUnit);
  //virtual void ReadEntireVar(CNetCDFDimension* netCDFDimension, CDoubleArray& array, const std::string& wantedUnit);

public:
  static const std::string m_LON_DIM_NAME;
  static const std::string m_LAT_DIM_NAME;
  //static const std::string m_DEPTH_DIM_NAME;

  static const std::string m_INTERNAL_LON_DIM_NAME;
  static const std::string m_INTERNAL_LAT_DIM_NAME;
  static const std::string m_INTERNAL_DEPTH_DIM_NAME;
  static const std::string m_INTERNAL_LATLON_DIM_NAME;

  static const std::string m_LATLONMIN_NAME;
  static const std::string m_LATLONSTEP_NAME;


protected:

  uint32_t	m_nbLatitudes;
  uint32_t	m_nbLongitudes;
  uint32_t	m_nbDepths;
  uint32_t	m_latIndex;	// if < 0, rewinded
  uint32_t	m_lonIndex;
  uint32_t	m_depthIndex;

  
  CNetCDFDimension* m_latDim;
  CNetCDFDimension* m_lonDim;
  CNetCDFDimension* m_depthDim;

protected:
  
  void Init();

  virtual void LoadStructure();

  virtual void CheckVariables();

  void AddVirtualVariables();

  void CheckNetCDFDimensions();

  uint32_t CurrentMeasure() const;

  virtual void SubstituteDimNames(CStringArray& dimNames);

  // On close free all allocated resources in order to have a fresh object (able to reopen)
  virtual void FreeResources();

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
  
  // Add a variable in the variable list got from description in file
  virtual void AddVar(const std::string	&Name);

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

  virtual void GetLatitudes(double Min, double Step, uint32_t Count, double* Vector);
  virtual void GetLongitudes(double Min, double Step, uint32_t Count, double* Vector);

  virtual void AddBratIndexData() {};

};

//-------------------------------------------------------------
//------------------- CExternalFilesDotGrid class --------------------
//-------------------------------------------------------------

class CExternalFilesDotGrid : public CExternalFilesAvisoGrid
{
public:
  CExternalFilesDotGrid
		(const std::string		&Name = "")
	: CExternalFilesAvisoGrid(Name)
  {
  }

  virtual ~CExternalFilesDotGrid() { };

  static std::string TypeOf() { return "GRID_DOTS"; };
  virtual std::string GetType() { return TypeOf(); };
};

//-------------------------------------------------------------
//------------------- CExternalFilesMercatorDotGrid class --------------------
//-------------------------------------------------------------

class CExternalFilesMercatorDotGrid : public CExternalFilesAvisoGrid
{
public:
  CExternalFilesMercatorDotGrid
		(const std::string		&Name = "")
	: CExternalFilesAvisoGrid(Name)
  {
  }

  virtual ~CExternalFilesMercatorDotGrid() { };

  static std::string TypeOf() { return "GRID_DOTS_MERCATOR"; };
  virtual std::string GetType() { return TypeOf(); };
  virtual void GetLatitudes
		(double		Min,
		 double		Step,
		 uint32_t	Count,
		 double		*Vector);
};

//-------------------------------------------------------------
//------------------- CExternalFilesBoxGrid class --------------------
//-------------------------------------------------------------
class CExternalFilesBoxGrid : public CExternalFilesDotGrid
{
public:
  CExternalFilesBoxGrid
		(const std::string		&Name = "")
	: CExternalFilesDotGrid(Name)
  {
  }
  virtual ~CExternalFilesBoxGrid() { };

  static std::string TypeOf() { return "GRID_BOXES"; };
  virtual std::string GetType() { return TypeOf(); };
};

//-------------------------------------------------------------
//------------------- CExternalFilesMercatorBoxGrid class --------------------
//-------------------------------------------------------------
class CExternalFilesMercatorBoxGrid : public CExternalFilesMercatorDotGrid
{
public:
  CExternalFilesMercatorBoxGrid
		(const std::string		&Name = "")
	: CExternalFilesMercatorDotGrid(Name)
  {
  }

  virtual ~CExternalFilesMercatorBoxGrid() { };

  static std::string TypeOf() { return "GRID_BOXES_MERCATOR"; };
  virtual std::string GetType() { return TypeOf(); };
};



/** @} */

}

#endif // Already included .h
