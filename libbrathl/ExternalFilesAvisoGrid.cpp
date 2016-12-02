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

#include <algorithm>
#include <cmath>
#include <string>

#include "brathl.h"

#include "Tools.h"
#include "Date.h"
#include "common/tools/Exception.h"
#include "Expression.h"
#include "LatLonPoint.h"

#include "ExternalFilesAvisoGrid.h"
// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

namespace brathl
{


/*
** Undefined constant under Microsoft C compilers
*/
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//----------------------------------------

const std::string CExternalFilesAvisoGrid::m_LON_DIM_NAME = "Longitude";
const std::string CExternalFilesAvisoGrid::m_LAT_DIM_NAME = "Latitude";
//const std::string CExternalFilesAvisoGrid::m_DEPTH_DIM_NAME = "Depth";

const std::string CExternalFilesAvisoGrid::m_INTERNAL_LON_DIM_NAME = "NbLongitudes";
const std::string CExternalFilesAvisoGrid::m_INTERNAL_LAT_DIM_NAME = "NbLatitudes";
const std::string CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME = "GridDepth";
const std::string CExternalFilesAvisoGrid::m_INTERNAL_LATLON_DIM_NAME = "LatLon";

const std::string CExternalFilesAvisoGrid::m_LATLONMIN_NAME = "LatLonMin";
const std::string CExternalFilesAvisoGrid::m_LATLONSTEP_NAME = "LatLonStep";

//----------------------------------------

#define INTERNAL_LAT_ID		-1
#define INTERNAL_LON_ID		-2
#define INTERNAL_DEPTH_ID	-3

/*
#define LATITUDE_NAME		"NbLatitudes"
#define LONGITUDE_NAME		"NbLongitudes"
#define DEPTH_NAME		"GridDepth"
#define LATLONMIN_NAME		"LatLonMin"
#define LATLONSTEP_NAME		"LatLonStep"
*/
//#define LATITUDE_UNIT		"degrees_north"
//#define LONGITUDE_UNIT		"degrees_east"

// Sentinel indicating first access to file (or after a rewind)
// With this we are sure the value is greater than any valid number of pass
//#define AT_BEGINNING		0xFFFFFFFFUL

#ifndef MAX
#define MAX(X,Y)	((X) >= (Y) ? (X) : (Y))
#endif

//-------------------------------------------------------------
//------------------- CExternalFilesMercatorDotGrid class --------------------
//-------------------------------------------------------------

//----------------------------------------
CExternalFilesAvisoGrid::CExternalFilesAvisoGrid(const std::string& name /*= ""*/)
	: CExternalFilesNetCDFCF(name)
{
  Init();
  Rewind();
}
//----------------------------------------
void CExternalFilesAvisoGrid::Init()
{
  m_latDim = NULL;
  m_lonDim = NULL;
  m_depthDim = NULL;
}
//----------------------------------------

void CExternalFilesAvisoGrid::FreeResources()
{
  

  CExternalFilesNetCDFCF::FreeResources();
}
//----------------------------------------

void CExternalFilesAvisoGrid::Rewind()
{
  m_latIndex	= AT_BEGINNING;
  m_lonIndex	= AT_BEGINNING;
  m_depthIndex	= AT_BEGINNING;
}
//----------------------------------------

  // True while there is something to read
bool CExternalFilesAvisoGrid::NextRecord()
{
  MustBeOpened();
  if (m_lonIndex == AT_BEGINNING)
  {
    m_latIndex		= 0;
    m_lonIndex		= 0;
    m_depthIndex		= 0;
  }
  else
  {
    if (CurrentMeasure() >= m_nbMeasures)
    {
      return false;
    }

    m_depthIndex++;

    if (m_depthIndex >= m_nbDepths)
    {
      m_depthIndex	= 0;
      m_latIndex++;

      if (m_latIndex >= m_nbLatitudes)
      {
	      m_latIndex	= 0;
	      m_lonIndex++;
      }
    }
  }

  return CurrentMeasure() < m_nbMeasures;
}

//----------------------------------------

  // True while there is something to read
bool CExternalFilesAvisoGrid::PrevRecord()
{

  bool bOk = false;

  MustBeOpened();
  
  if (m_lonIndex == AT_BEGINNING)
  {
    return bOk;
  }
  else
  {
    if (m_depthIndex == 0)
    {
      m_depthIndex = m_nbDepths - 1;
      if (m_latIndex == 0)
      {
        m_latIndex = m_nbLatitudes;
        if (m_lonIndex == 0)
        {
          m_lonIndex = AT_BEGINNING;        
        }
        else
        {
          m_lonIndex--;
          bOk = true;
        }
      }
      else
      {
        m_latIndex--;
        bOk = true;
      }

    }
    else
    {
      m_depthIndex--;
      bOk = true;
    }
  }

  return bOk;
}



//----------------------------------------
void CExternalFilesAvisoGrid::GetValue
		(const std::string		&name,
		 CExpressionValue	&value,
		 const std::string		&wantedUnit)
{
  double result;
  GetValue(name, result, wantedUnit);
  value = result;

}
//----------------------------------------

// If WantedUnit is "", the returned value is in base (SI) unit.
void CExternalFilesAvisoGrid::GetValue
		(const std::string		&name,
		 double         	&value,
		 const std::string		&wantedUnit)
{
  uint32_t	start[3];
  uint32_t	count[3]	= {1, 1, 1};

  MustBeOpened();

  if (m_latIndex == AT_BEGINNING)
  {
    throw CFileException(CTools::Format("PROGRAM ERROR: Try to access file ('%s' field) while file is already rewinded (NextRecord must be called)",
				        name.c_str()),
		   GetName(),
		   BRATHL_LOGIC_ERROR);
  }

  if (CurrentMeasure() >= m_nbMeasures)
  {
    throw CFileException(CTools::Format("PROGRAM ERROR: Try to access file ('%s' field) while all measurements have been read",
				        name.c_str()),
			 GetName(),
			 BRATHL_LOGIC_ERROR);
  }

  start[0]	= m_lonIndex;
  start[1]	= m_latIndex;
  start[2]	= m_depthIndex;

// Find field def.
  CFieldNetCdf* var = GetFieldNetCdf(name);
      
  int32_t netCdfId = var->GetNetCdfId();


  if (netCdfId < 0)
  {
    int32_t	index;
    switch (netCdfId)
    {
      //---------------------------
      case INTERNAL_LAT_ID:
      //---------------------------
	      index	= m_latIndex;
    	  break;
      //---------------------------
      case INTERNAL_LON_ID:
      //---------------------------
        index	= m_lonIndex;
        break;
      //---------------------------
      case INTERNAL_DEPTH_ID:
      //---------------------------
	      index	= m_depthIndex;
	      break;
      //---------------------------
      default:
      //---------------------------
	      throw CException(CTools::Format("PROGRAM ERROR: Unknown pseudo netcdf id %d",
					      netCdfId),
  		       BRATHL_LOGIC_ERROR);
    }

    value	= var->GetValues()[index];
    
    CUnit unit( var->GetUnit() );
    
    if ((wantedUnit != "") && (wantedUnit != CUnit::m_UNIT_SI))
    {
      unit.SetConversionTo(wantedUnit);
    }
    value	= unit.Convert(value);
  }
  else
  {
    int	nbDims	= m_file.GetVarNbDims(netCdfId);
    
    if (nbDims != 2) 
    {
      nbDims = 3;
    }

    m_file.ReadVar(name, nbDims, start, count, &value, wantedUnit);
  }
}

//----------------------------------------
void CExternalFilesAvisoGrid::LoadStructure()
{
  double	*data	= NULL;
  double	latLonMin[2];
  double	latLonStep[2];

// Get the delta T between 2 theorical measurements in order to compute
// dates



  /*
  AddVar(INTERNAL_LAT_ID, "Latitude", "Latitude of measurement", CLatLonPoint::m_DEFAULT_UNIT_LATITUDE, NC_DOUBLE);
  AddVar(INTERNAL_LON_ID, "Longitude", "Longitude of measurement", CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE, NC_DOUBLE);
  */

  m_file.GetNetCDFDims()->Erase(CExternalFilesAvisoGrid::m_INTERNAL_LAT_DIM_NAME);
  m_file.GetNetCDFVarDefs()->Erase(CExternalFilesAvisoGrid::m_INTERNAL_LAT_DIM_NAME);
  
  m_file.GetNetCDFDims()->Erase(CExternalFilesAvisoGrid::m_INTERNAL_LON_DIM_NAME);
  m_file.GetNetCDFVarDefs()->Erase(CExternalFilesAvisoGrid::m_INTERNAL_LON_DIM_NAME);
  
  m_nbLatitudes	= m_file.GetDimension(CExternalFilesAvisoGrid::m_INTERNAL_LAT_DIM_NAME);
  m_nbLongitudes	= m_file.GetDimension(CExternalFilesAvisoGrid::m_INTERNAL_LON_DIM_NAME);
  
  m_depthDim = m_file.GetNetCDFDim(CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME);

  if (m_depthDim != NULL)
  {
    m_nbDepths	= m_file.GetDimension(CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME);
  }
  else
  {
    //--------------
    // add depth dim
    //--------------
    m_nbDepths	= 1;
    CNetCDFDimension depthDim(CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME, m_nbDepths);
    depthDim.SetDimId(INTERNAL_LAT_ID);

    m_depthDim = m_file.AddNetCDFDim(depthDim);
  }

  //--------------
  // add latitude dim
  //-------------- 
  CNetCDFDimension latDim(CExternalFilesAvisoGrid::m_LAT_DIM_NAME, m_nbLatitudes);
  latDim.SetDimId(INTERNAL_LAT_ID);

  m_latDim = m_file.AddNetCDFDim(latDim);

  //--------------
  // add longitude dim
  //--------------
  CNetCDFDimension lonDim(CExternalFilesAvisoGrid::m_LON_DIM_NAME, m_nbLongitudes);
  lonDim.SetDimId(INTERNAL_LON_ID);

  m_lonDim = m_file.AddNetCDFDim(lonDim);


  //----------------------
  AddVirtualVariables();
  //----------------------


  m_nbMeasures	= m_nbLatitudes * m_nbLongitudes * m_nbDepths;


  try
  {
    CFieldNetCdf* var = NULL;

    uint32_t start	= 0;
    uint32_t count	= 2;

    data	= new double[MAX(m_nbLatitudes, MAX(m_nbLongitudes, m_nbDepths))];

    m_file.ReadVar(CExternalFilesAvisoGrid::m_LATLONMIN_NAME, 1, &start, &count, latLonMin, "angular_degrees");
    m_file.ReadVar(CExternalFilesAvisoGrid::m_LATLONSTEP_NAME, 1, &start, &count, latLonStep, "angular_degrees");


    GetLatitudes(latLonMin[0], latLonStep[0], m_nbLatitudes, data);

    var = GetFieldNetCdf(CExternalFilesAvisoGrid::m_LAT_DIM_NAME);
    var->SetValues(data, m_nbLatitudes);

    
    GetLongitudes(latLonMin[1], latLonStep[1], m_nbLongitudes, data);

    var = GetFieldNetCdf(CExternalFilesAvisoGrid::m_LON_DIM_NAME);
    var->SetValues(data, m_nbLongitudes);

    if (m_file.VarExists(CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME))
    {      
      start	= 0;
      count	= m_nbDepths;

      m_file.ReadVar(CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME, 1, &start, &count, data, CUnit::m_UNIT_SI);
      
      var = GetFieldNetCdf(CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME);
      
      var->SetNetCdfId(INTERNAL_DEPTH_ID);
    }
    else
    {
      for (start = 0; start < m_nbDepths; start++)
      {
	      data[start]	= start;
      }
    }

    var = GetFieldNetCdf(CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME);
    var->SetValues(data, m_nbDepths);

    delete []data;
    data = NULL;

    Rewind();
  }
  catch (...)
  {
    delete []data;
    data = NULL;
    
    throw;
  }
}

//----------------------------------------
uint32_t CExternalFilesAvisoGrid::CurrentMeasure() const
{
  return ((m_lonIndex * m_nbLatitudes) + m_latIndex) * m_nbDepths + m_depthIndex;
}

//----------------------------------------
void CExternalFilesAvisoGrid::CheckNetCDFDimensions()
{

  if (m_latDim == NULL)
  {
	  throw CException(CTools::Format("Dimension '%s' not found.",
					  CExternalFilesAvisoGrid::m_LAT_DIM_NAME.c_str()),
			   BRATHL_LOGIC_ERROR);
  }

  if (m_lonDim == NULL)
  {
	  throw CException(CTools::Format("Dimension '%s' not found.",
					  CExternalFilesAvisoGrid::m_LON_DIM_NAME.c_str()),
			   BRATHL_LOGIC_ERROR);
  }

  if (m_depthDim == NULL)
  {
	  throw CException(CTools::Format("Dimension '%s' not found.",
					  CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME.c_str()),
			   BRATHL_LOGIC_ERROR);
  }
}

//----------------------------------------
void CExternalFilesAvisoGrid::AddVirtualVariables()
{
  int32_t dimId = -1;

  try
  {
    CheckNetCDFDimensions();
  }
  catch(CException& e)
  {
    throw CException(CTools::Format("ERROR: CExternalFilesAvisoGrid::AddVirtualVariables - %s", e.what()),
                    BRATHL_LOGIC_ERROR);
  }


  CStringMap mapAttributes;
  //--------------
  // add latitude var
  //--------------
  mapAttributes.RemoveAll();
  mapAttributes.Insert(STANDARD_NAME_ATTR, "latitude");
  
  dimId = m_latDim->GetDimId();

  AddVar(dimId, CExternalFilesAvisoGrid::m_LAT_DIM_NAME, "Latitude of measurement", CLatLonPoint::m_DEFAULT_UNIT_LATITUDE, NC_DOUBLE,
         m_nbLatitudes, CExternalFilesAvisoGrid::m_LAT_DIM_NAME, dimId, &mapAttributes);


  CNetCDFCoordinateAxis latVar(CExternalFilesAvisoGrid::m_LAT_DIM_NAME, CLatLonPoint::m_DEFAULT_UNIT_LATITUDE, Latitude);
  latVar.AddNetCDFDim(*m_latDim);

  m_latDim->AddCoordinateVariable(latVar.GetName());
  m_file.AddNetCDFVarDef(latVar, true);


  //--------------
  // add longitude var
  //--------------
  mapAttributes.RemoveAll();
  mapAttributes.Insert(STANDARD_NAME_ATTR, "longitude");

  dimId = m_lonDim->GetDimId();

  AddVar(dimId, CExternalFilesAvisoGrid::m_LON_DIM_NAME, "Longitude of measurement", CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE, NC_DOUBLE,
         m_nbLongitudes, CExternalFilesAvisoGrid::m_LON_DIM_NAME, dimId, &mapAttributes);


  CNetCDFCoordinateAxis lonVar(CExternalFilesAvisoGrid::m_LON_DIM_NAME, CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE, Longitude);
  lonVar.AddNetCDFDim(*m_lonDim);

  m_lonDim->AddCoordinateVariable(lonVar.GetName());
  m_file.AddNetCDFVarDef(lonVar, true);


  //--------------
  // add depth
  //--------------
  if (m_file.VarExists(CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME))
  {
    AddVar(CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME);
  }
  else
  {
    mapAttributes.RemoveAll();
    mapAttributes.Insert(STANDARD_NAME_ATTR, "depth");

    dimId = m_depthDim->GetDimId();

    AddVar(dimId, CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME, "Depth layer", "count", NC_DOUBLE,
          m_nbDepths, CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME, dimId, &mapAttributes);

    CNetCDFCoordinateAxis depthVar(CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME, "count", Z);
    depthVar.AddNetCDFDim(*m_depthDim);

    m_depthDim->AddCoordinateVariable(depthVar.GetName());
    m_file.AddNetCDFVarDef(depthVar, true);

  }





}

//----------------------------------------
void CExternalFilesAvisoGrid::CheckVariables()
{

  CStringArray varNames;
  CStringArray::iterator	index;

  m_file.GetVariables(varNames);

  for (index=varNames.begin(); index != varNames.end(); index++)
  {
    if (*index == CExternalFilesAvisoGrid::m_INTERNAL_LAT_DIM_NAME)	continue;
    if (*index == CExternalFilesAvisoGrid::m_INTERNAL_LON_DIM_NAME)	continue;
    if (*index == CExternalFilesAvisoGrid::m_INTERNAL_DEPTH_DIM_NAME)	continue;
    if (*index == CExternalFilesAvisoGrid::m_LATLONMIN_NAME) continue;
    if (*index == CExternalFilesAvisoGrid::m_LATLONSTEP_NAME)	continue;
    if (*index == CExternalFilesAvisoGrid::m_INTERNAL_LATLON_DIM_NAME) continue;

    AddVar(*index);
  }

}

//----------------------------------------
void CExternalFilesAvisoGrid::AddVar
		            (int32_t	netcdfId,
		             const std::string	&name,
		             const std::string	&description,
		             const std::string	&unit,
                 int32_t type,
                 uint32_t dimValue,
                 const std::string dimName, 
                 int32_t dimId,
                 const CStringMap* mapAttributes /*= NULL*/)
{


  CExternalFilesNetCDF::AddVar(netcdfId, name, description, unit, type, dimValue, dimName, dimId, mapAttributes);

}

//----------------------------------------
void CExternalFilesAvisoGrid::AddVar
		            (int32_t	netcdfId,
		             const std::string	&name,
		             const std::string	&description,
		             const std::string	&unit,
                 int32_t type /*= NC_NAT*/,
                 const CUIntArray* dimValues  /*= NULL*/,
                 const CStringArray* dimNames  /*= NULL*/, 
                 const CIntArray* dimIds /*= NULL*/,
                 const CStringMap* mapAttributes /*= NULL*/)
{
  
  try
  {
    CheckNetCDFDimensions();
  }
  catch(CException& e)
  {
    throw CException(CTools::Format("ERROR: CExternalFilesAvisoGrid::AddVar - %s", e.what()),
                    BRATHL_LOGIC_ERROR);
  }



  CNetCDFVarDef netCDFVarDef(name, unit, static_cast<nc_type>(type));
  
  netCDFVarDef.SetVarId(netcdfId);

  if (mapAttributes != NULL)
  {
    netCDFVarDef.AddAttributes(*mapAttributes);
  }

  if (netcdfId >= 0)
  {
    m_file.LoadAttributes(&netCDFVarDef);
  }

  if ( (dimNames != NULL) && (dimValues != NULL) && (dimIds != NULL) )
  {
    /*
    CStringArray replaced;

    dimNames->Replace(CExternalFilesAvisoGrid::m_INTERNAL_LAT_DIM_NAME, m_latDim->GetName(), replaced, true, false);

    CStringArray replacedDims = replaced;
    replaced.RemoveAll();

    replacedDims.Replace(CExternalFilesAvisoGrid::m_INTERNAL_LON_DIM_NAME, m_lonDim->GetName(), replaced, true, false);

    netCDFVarDef.SetNetCDFDims(&replaced, dimValues, dimIds);

    CFieldNetCdf* field = GetFieldNetCdf(name);
    field->SetDimNames(replaced);
    */

    netCDFVarDef.SetNetCDFDims(dimNames, dimValues, dimIds);

  }
  
  m_file.AddNetCDFVarDef(netCDFVarDef);

  CExternalFilesNetCDF::AddVar(netcdfId,
		                           name,
		                           description,
		                           unit,
                               type,
                               dimValues,
                               dimNames,
                               dimIds,
                               mapAttributes);


  
}

//----------------------------------------
void CExternalFilesAvisoGrid::SubstituteDimNames(CStringArray& dimNames)
{
  CStringArray replaced;

  dimNames.Replace(CExternalFilesAvisoGrid::m_INTERNAL_LAT_DIM_NAME, m_latDim->GetName(), replaced, true, false);

  CStringArray replacedDims = replaced;
  replaced.RemoveAll();

  replacedDims.Replace(CExternalFilesAvisoGrid::m_INTERNAL_LON_DIM_NAME, m_lonDim->GetName(), replaced, true, false);

  dimNames.RemoveAll();
  dimNames = replaced;
}

//----------------------------------------
void CExternalFilesAvisoGrid::AddVar(const std::string	&name)
{
  CExternalFilesNetCDF::AddVar(name);
}

//----------------------------------------

void CExternalFilesAvisoGrid::GetLatitudes
		(double		Min,
		 double		Step,
		 uint32_t	count,
		 double		*Vector)
{
  for (uint32_t index=0; index < count; ++index)
  {
    Vector[index]	= Min+(Step*index);
  }
}
//----------------------------------------

void CExternalFilesAvisoGrid::GetLongitudes
		(double		Min,
		 double		Step,
		 uint32_t	count,
		 double		*Vector)
{
  for (uint32_t index=0; index < count; ++index)
  {
    Vector[index]	= Min+(Step*index);
  }
}
////----------------------------------------
//void CExternalFilesAvisoGrid::ReadEntireVar(CNetCDFVarDef* netCDFVarDef, CDoubleArray& array, const std::string& wantedUnit)
//{
//  m_file.ReadEntireVar(netCDFVarDef, array, wantedUnit);
//
//}
////----------------------------------------
//void CExternalFilesAvisoGrid::ReadEntireVar(CNetCDFDimension* netCDFDimension, CDoubleArray& array, const std::string& wantedUnit)
//{
//
//  m_file.ReadEntireVar(netCDFDimension, array, wantedUnit);
//
//}
//-------------------------------------------------------------
//------------------- CExternalFilesMercatorDotGrid class --------------------
//-------------------------------------------------------------

//----------------------------------------

void CExternalFilesMercatorDotGrid::GetLatitudes
		(double		Min,
		 double		Step,
		 uint32_t	count,
		 double		*Vector)
{
  double	Tmp;
  Step	= Step * M_PI/180.0;
  Min	= Min * M_PI/180.0;

  Tmp		= sin(Min);
  Tmp		= (0.5*log((1.0+Tmp)/(1.0-Tmp)))/Step;

  for (uint32_t index=0; index < count; ++index)
  {
    Vector[index]	= (asin(tanh(Step*(index+Tmp))))*180.0/M_PI;
  }
}




}
