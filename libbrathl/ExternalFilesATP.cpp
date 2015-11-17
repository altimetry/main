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
#include <string>

#include "brathl.h"

#include "Tools.h"
#include "Trace.h"
#include "Date.h"
#include "Exception.h"
#include "Expression.h"
#include "LatLonPoint.h"

#include "ExternalFilesATP.h"
// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;

namespace brathl
{


//----------------------------------------

#define INTERNAL_LAT_ID		-1
#define INTERNAL_LON_ID		-2
#define INTERNAL_DATE_ID	-3
#define INTERNAL_CYCLE_ID	-4
#define INTERNAL_PASS_ID	-5
#define INTERNAL_BRAT_INDEX_DATA	-6

//#define DATE_UNIT		"days since 1950-01-01 00:00:00.000000 UTC"
//#define LATITUDE_UNIT		"degrees_north"
//#define LONGITUDE_UNIT		"degrees_east"
#define DELTAT_UNIT		"seconds"

// Sentinel indicating first access to file (or after a rewind)
// With this we are sure the value is greater than any valid number of pass
//#define AT_BEGINNING		0xFFFFFFFFUL

const std::string CExternalFilesATP::m_ALONG_TRACK_PRODUCT = "ALONG_TRACK_PRODUCT";
const std::string CExternalFilesATP::m_DATE_UNIT = "days since 1950-01-01 00:00:00.000000 UTC";
const std::string CExternalFilesATP::m_DATA_DIM_NAME = "Data";

CExternalFilesATP::CExternalFilesATP(const std::string		&Name /*= ""*/)
	: CExternalFilesNetCDFCF(Name)
{

}
//----------------------------------------

void CExternalFilesATP::FreeResources()
{

  m_passList.RemoveAll();

  m_dataDimValues.RemoveAll();
  m_dataDimNames.RemoveAll();
  m_dataDimIds.RemoveAll();
  
  CExternalFilesNetCDFCF::FreeResources();
}

//----------------------------------------
void CExternalFilesATP::Rewind()
{
// Rewinding is going back before to the first defined pass
  m_currentPass	= AT_BEGINNING;
  m_currentInPass	= AT_BEGINNING;
}

//----------------------------------------
CRegisteredPass* CExternalFilesATP::GetRegisteredPass(CBratObject* ob, bool withExcept /*= true*/)
{
  CRegisteredPass* p = dynamic_cast<CRegisteredPass*>(ob);
  if (withExcept)
  {
    if (p == NULL)
    {
      CException e("CExternalFilesATP::GetRegisteredPass - dynamic_cast<CRegisteredPass*>(ob) returns NULL"
                   "object seems not to be an instance of CRegisteredPass",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return p;

}

//----------------------------------------

  // True while there is something to read
bool CExternalFilesATP::NextRecord()
{
  MustBeOpened();
// Before beginning of file ?
  if (m_currentPass == AT_BEGINNING)
  {
    m_currentPass		= 0; // Yes, set to beginning
    m_currentInPass	= AT_BEGINNING;
  }
  else
  {
    // Already at end ?
    if (m_currentPass >= m_passList.size())
    {
      return false;
    }
    // Next measure in current pass
    m_currentInPass++;
    // Already in the same pass ?
    if (m_currentInPass < CExternalFilesATP::GetRegisteredPass(m_passList[m_currentPass])->m_nbData)
    {
      return true;
    }

    // No, goto next pass
    m_currentPass++;
  }

  return LoadPass();
}

//----------------------------------------

  // True while there is something to read
bool CExternalFilesATP::PrevRecord()
{
  bool bOk = false;

  MustBeOpened();
// Before beginning of file ?
  if (m_currentPass == AT_BEGINNING)
  {
    return bOk;
  }
  else
  {
    // Previous measure in current pass
    if (m_currentInPass == 0)
    {
      // Already at top ?
      if (m_currentPass <= 0)
      {
        return bOk;
      }
      else
      {
        m_currentPass--;
        bOk = LoadPass();
        m_currentInPass = CExternalFilesATP::GetRegisteredPass(m_passList[m_currentPass])->m_nbData - 1;
      }
    }
    else
    {
      m_currentInPass--;
      bOk = true;
    }
  }

  return bOk;
}

//----------------------------------------
void CExternalFilesATP::GetAllValues(CFieldNetCdf* field, const std::string& wantedUnit)
{
  if (field == NULL)
  {
    return;
  }

  uint32_t nbDimensions = field->GetNbDims();

  //---------------------------
  // field is not an array (no dimension)
  //variable values have to be in field (m_values, accessible with field->GetValues()) 
  //---------------------------
  if (nbDimensions == 0)
  {
    return;
  }

  
  //---------------------------
  // field is an array (number of dimension is > 0)
  //---------------------------

  uint32_t numPass = m_passList.size();

  Rewind();
  
  field->EmptyValues();

  for (m_currentPass = 0 ; m_currentPass < numPass ; m_currentPass++)
  {
    LoadPass(field);
  }

}
//----------------------------------------
void CExternalFilesATP::GetAllValues(const std::string& name, CDoubleArray& array, const std::string& wantedUnit)
{
  CFieldNetCdf* field = GetFieldNetCdf(name);
  if (field == NULL)
  {
    return;
  }

  GetAllValues(field, wantedUnit);

  if (&array != &field->GetValues())
  {
    array = field->GetValues();
  }

}

//----------------------------------------
void CExternalFilesATP::GetAllValues(const std::string& name, CExpressionValue& value, const std::string& wantedUnit)
{
  CDoubleArray array;
  
  GetAllValues(name, array, wantedUnit);

  ExpressionValueDimensions	dimensions;

  CFieldNetCdf* field = GetFieldNetCdf(name);

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
  field->GetDimAsVector(dimensions);

  CExpressionValue varValue(FloatType, dimensions, NULL);
  varValue = array;
}


//----------------------------------------
void CExternalFilesATP::GetValues(const std::string& name, CExpressionValue &value, const std::string	&wantedUnit)
{

  CFieldNetCdf* field = GetFieldNetCdf(name);


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
    GetAllValues(field, wantedUnit);
    field->SetValuesAsArray();
    field->EmptyValues();
  }

  uint32_t* shape = (uint32_t*)(field->GetDim());

  uint32_t* start = field->GetDimsIndexArray();

  uint32_t* edges = field->GetDimsCountArray();

  value.SetNewValue(FloatType, edges, nbDimensions, NULL);

  CTools::ExtractVector(field->GetValuesAsArray(), shape, nbDimensions, start, edges, value.GetValues());

 }

//----------------------------------------
void CExternalFilesATP::GetValue(const std::string& name, CExpressionValue& value, const std::string& wantedUnit)
{
  double result;
  GetValue(name, result, wantedUnit);
  value = result;

}
//----------------------------------------

// If WantedUnit is "", the returned value is in base (SI) unit.
void CExternalFilesATP::GetValue(const std::string& name, double& value, const std::string& wantedUnit)
{
  MustBeOpened();

  if (m_currentPass >= m_passList.size())
  {
    throw CFileException(CTools::Format("PROGRAM ERROR: Try to access file ('%s' field) while all measurements have been read",
				        name.c_str()),
			 GetName(),
			 BRATHL_LOGIC_ERROR);
  }

// Find field def.
  CFieldNetCdf* var = GetFieldNetCdf(name);

  CUnit	unit = var->GetUnit();

  // get the value (computed by LoadPass), and already converted to its base unit
/*  if (wantedUnit != "")
  {
    CUnit	unit	= wantedUnit;
    unit.SetConversionFrom(CUnit(var->GetUnit()).BaseUnit());
    value	= unit.Convert(var->GetValues()[m_currentInPass]);
  }
  else
  {// Already converted to the base unit by loadpass
    value	= var->GetValues()[m_currentInPass];
  }
  */
  if ((wantedUnit != "") && (wantedUnit != CUnit::m_UNIT_SI))
  {
    unit.SetConversionTo(wantedUnit);
  }
  else
  {  
    unit.SetConversionToBaseUnit();
  }

  value	= var->GetValues()[m_currentInPass];

  double defValue = var->GetFillValue();

  if (value == defValue)
  {
    value	= CTools::m_defaultValueDOUBLE;
  }
  else
  {
    value	= unit.Convert(var->GetValues()[m_currentInPass]);
  }

}


//----------------------------------------
bool CExternalFilesATP::PassCompare
		(CBratObject* o1,
		 CBratObject* o2)
{

  CRegisteredPass* T1 = CExternalFilesATP::GetRegisteredPass(o1);
  CRegisteredPass* T2 = CExternalFilesATP::GetRegisteredPass(o2);

  if (T1->m_cycle != T2->m_cycle)
    return T1->m_cycle < T2->m_cycle;
  return T1->m_pass < T2->m_pass;
}


//----------------------------------------
void CExternalFilesATP::LoadStructure
		()
{
  uint32_t		MaxPasses;
  uint32_t		MaxCycles;
  uint32_t		Start[2];
  uint32_t		Count[2];
  uint32_t		PassIndex;
  uint32_t		CycleIndex;
  CRegisteredPass	passDef;

  double		*Passes		= NULL;
  double		*PassSizes	= NULL;
  double		*Cycles		= NULL;
  double		*BeginDates	= NULL;

/*
  AddVar(INTERNAL_LAT_ID, "Latitude", "Latitude of measurement", CLatLonPoint::m_DEFAULT_UNIT_LATITUDE, NC_DOUBLE);
  AddVar(INTERNAL_LON_ID, "Longitude", "Longitude of measurement", CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE , NC_DOUBLE);
  AddVar(INTERNAL_DATE_ID, "Date", "Date of measurement", CExternalFilesATP::m_DATE_UNIT, NC_DOUBLE);
  AddVar(INTERNAL_CYCLE_ID, "Cycle", "Cycle number", "count", NC_INT);
  AddVar(INTERNAL_PASS_ID, "Pass", "Pass number", "count", NC_INT);
*/

  uint32_t dataDimLength = m_file.GetDimension(CExternalFilesATP::m_DATA_DIM_NAME);
  int32_t dataDimId = m_file.GetDimId(CExternalFilesATP::m_DATA_DIM_NAME);
  
  m_dataDim.SetName(CExternalFilesATP::m_DATA_DIM_NAME);
  m_dataDim.SetLength(dataDimLength);
  m_dataDim.SetDimId(dataDimId);
  m_dataDim.SetIsUnlimited(true);

  m_dataDimValues.Insert(dataDimLength);
  m_dataDimNames.Insert(CExternalFilesATP::m_DATA_DIM_NAME);
  m_dataDimIds.Insert(dataDimId);

  m_file.GetNetCDFDims()->RemoveAll();
  m_file.GetNetCDFVarDefs()->RemoveAll();

  m_file.AddNetCDFDim(m_dataDim);


// Get the delta T between 2 theorical measurements in order to compute
// dates
  m_file.ReadVar("DeltaT", 0, NULL, NULL, &m_deltaT, DELTAT_UNIT);
  
  MaxPasses	= m_file.GetDimension("Tracks");
  MaxCycles	= m_file.GetDimension("Cycles");

  try
  {
    Passes	= new double[MaxPasses];
    PassSizes	= new double[MaxPasses];
    Cycles	= new double[MaxCycles];
    BeginDates	= new double[MaxCycles];
  
    Start[0]	= 0;
    Count[0]	= MaxPasses;
    
    // Get the list of passes and their size (number of measurement)
    m_file.ReadVar("Tracks", 1, Start, Count, Passes, CUnit::m_UNIT_SI);
    m_file.ReadVar("NbPoints", 1, Start, Count, PassSizes, CUnit::m_UNIT_SI);

    m_nbMeasures	= 0;
    
    for (PassIndex=0; PassIndex<MaxPasses; ++PassIndex)
    {
      // Skip undefined entries or emptied ones
      if (CTools::IsDefaultValue(Passes[PassIndex]) || (PassSizes[PassIndex] == 0.0))
      {
	      continue;
      }

      Start[0]	= PassIndex;
      Start[1]	= 0;
      Count[0]	= 1;
      Count[1]	= MaxCycles;
      
      // Get cycle list for the pass associated with the dates of the
      // first theorical measurement
      m_file.ReadVar("Cycles", 2, Start, Count, Cycles, CUnit::m_UNIT_SI);
      //m_file.ReadVar("BeginDates", 2, Start, Count, BeginDates, CExternalFilesATP::m_DATE_UNIT);
      m_file.ReadVar("BeginDates", 2, Start, Count, BeginDates, CUnit::m_UNIT_SI);

      //passDef = new CRegisteredPass();

      passDef.m_pass		= static_cast<uint32_t>(Passes[PassIndex]);
      passDef.m_nbData		= static_cast<uint32_t>(PassSizes[PassIndex]);
      passDef.m_startPoint	= m_nbMeasures;
      
      m_nbMeasures		+= passDef.m_nbData;

      for (CycleIndex=0; CycleIndex < MaxCycles; ++CycleIndex)
      {
	      // Skip undefined cycle entry
	      if (CTools::IsDefaultValue(Cycles[CycleIndex]))
        {
	        continue;
        }

	      passDef.m_cycle		= static_cast<uint32_t>(Cycles[CycleIndex]);
	      passDef.m_cycleIndex	= CycleIndex;
	      passDef.m_beginDate	= BeginDates[CycleIndex];
	      
        m_passList.Insert(new CRegisteredPass(passDef));
      }
    }

    // Sort the passes in chronolological order (instead of sorted by pass
    // number)
    sort(m_passList.begin(), m_passList.end(), PassCompare);

    delete []Passes;
    delete []PassSizes;
    delete []Cycles;
    delete []BeginDates;
    Rewind();

  }
  catch (...)
  {
    delete []Passes;
    delete []PassSizes;
    delete []Cycles;
    delete []BeginDates;
    throw;
  }


}

//----------------------------------------
void CExternalFilesATP::AddVirtualVariables()
{

  CStringMap mapAttributes;

  mapAttributes.RemoveAll();
  mapAttributes.Insert(STANDARD_NAME_ATTR, "latitude");
  AddVar(INTERNAL_LAT_ID, "Latitude", "Latitude of measurement", CLatLonPoint::m_DEFAULT_UNIT_LATITUDE, NC_DOUBLE,
         &m_dataDimValues, &m_dataDimNames, &m_dataDimIds, &mapAttributes);

  mapAttributes.RemoveAll();
  mapAttributes.Insert(STANDARD_NAME_ATTR, "longitude");
  AddVar(INTERNAL_LON_ID, "Longitude", "Longitude of measurement", CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE , NC_DOUBLE,
         &m_dataDimValues, &m_dataDimNames, &m_dataDimIds, &mapAttributes);

  mapAttributes.RemoveAll();
  mapAttributes.Insert(STANDARD_NAME_ATTR, "time");
  AddVar(INTERNAL_DATE_ID, "Date", "Date of measurement", CExternalFilesATP::m_DATE_UNIT, NC_DOUBLE,
         &m_dataDimValues, &m_dataDimNames, &m_dataDimIds, &mapAttributes);

  mapAttributes.RemoveAll();
  mapAttributes.Insert(STANDARD_NAME_ATTR, CField::m_BRAT_INDEX_DATA_NAME);
  std::string desc = CField::m_BRAT_INDEX_DATA_DESC;
  desc.append(" based on date of measurement ('Date' field)");
  AddVar(INTERNAL_BRAT_INDEX_DATA, CField::m_BRAT_INDEX_DATA_NAME, desc , "count", NC_DOUBLE,
         &m_dataDimValues, &m_dataDimNames, &m_dataDimIds, &mapAttributes);

  AddVar(INTERNAL_CYCLE_ID, "Cycle", "Cycle number", "count", NC_INT,
         &m_dataDimValues, &m_dataDimNames, &m_dataDimIds);

  AddVar(INTERNAL_PASS_ID, "Pass", "Pass number", "count", NC_INT,
         &m_dataDimValues, &m_dataDimNames, &m_dataDimIds);

}

//----------------------------------------
void CExternalFilesATP::CheckVariables()
{

  AddVirtualVariables();
  
  CStringArray	varNames;
  CStringArray::iterator	it;

  m_file.GetVariables(varNames);

  for (it = varNames.begin(); it != varNames.end(); it++)
  {
    if (*it == "DeltaT")		continue;
    if (*it == "Tracks")		continue;
    if (*it == "NbPoints")		continue;
    if (*it == "Cycles")		continue;
    if (*it == "Longitudes")		continue;
    if (*it == "Latitudes")		continue;
    if (*it == "BeginDates")		continue;
    if (*it == "DataIndexes")	continue;
    if (*it == "GlobalCyclesList")	continue;

    AddVar(*it);
  }

  //m_file.Dump(*(CTrace::GetInstance()->GetDumpContext()));
}

//----------------------------------------
void CExternalFilesATP::AddVar
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
  
  //m_file.SetNetCDFDims(dimNames, dimValues, dimIds);
  //m_file.AddCoordinateVariable(m_dataDimNames, name);
  m_dataDim.AddCoordinateVariable(name);

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

  //netCDFVarDef.SetNetCDFDims(dimNames, dimValues, dimIds);
  netCDFVarDef.AddNetCDFDim(m_dataDim);
  m_file.AddNetCDFVarDef(netCDFVarDef);

  CExternalFilesNetCDF::AddVar(netcdfId,
		                         name,
		                         description,
		                         unit,
                             type,
                             &m_dataDimValues, // force to Data dimensions
                             &m_dataDimNames, // force to Data dimensions
                             &m_dataDimIds, // force to Data dimensions
                             mapAttributes);




}
//----------------------------------------
void CExternalFilesATP::AddVar(const std::string	&name)
{
  CExternalFilesNetCDF::AddVar(name);
}

//----------------------------------------
bool CExternalFilesATP::LoadPass(bool resetValues /* = false */)
{
  uint32_t			Start[2];
  uint32_t			Count[2];
  CRegisteredPass*		pass;
  uint32_t			Index;
  double			*passData	= NULL;

  if (m_currentPass >= m_passList.size())
  {
    return false;
  }

  try
  {
    pass	= CExternalFilesATP::GetRegisteredPass(m_passList[m_currentPass]);
    Start[0]	= pass->m_startPoint;
    Count[0]	= pass->m_nbData;
    Start[1]	= pass->m_cycleIndex;
    Count[1]	= 1;
    passData	= new double[Count[0]];
    CObMap::iterator it;

    for (it = m_varList.begin(); it != m_varList.end(); it++)
    {
      std::string varName = it->first;
      
      CFieldNetCdf* var = CExternalFiles::GetFieldNetCdf(it->second);
      
      int32_t netCdfId = var->GetNetCdfId();

      switch (netCdfId)
      {
        //-------------------
        case INTERNAL_LAT_ID:
        //-------------------
          m_file.ReadVar("Latitudes", 1, Start, Count, passData, CLatLonPoint::m_DEFAULT_UNIT_LATITUDE);
	        break;

        //-------------------
	      case INTERNAL_LON_ID:
        //-------------------
    	    m_file.ReadVar("Longitudes", 1, Start, Count, passData, CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE );
	        break;

        //-------------------
	      case INTERNAL_DATE_ID:
        //-------------------
	      
          m_file.ReadVar("DataIndexes", 1, Start, Count, passData, "count");
	        
          for (Index=0; Index<Count[0]; Index++)
          {
	          //passData[Index]	= pass->m_beginDate + (passData[Index]*m_deltaT)/86400.0;
            // pass->m_beginDate is expressed in SI unit ==> seconds since 1950-01-01 00:00:00.0
	          passData[Index]	= pass->m_beginDate + (passData[Index]*m_deltaT);
          }
	        
          break;

        //-------------------
	      case INTERNAL_BRAT_INDEX_DATA:
        //-------------------
	      
          for (Index=0; Index<Count[0]; Index++)
          {
	          passData[Index]	= Start[0] + Index + m_offset;
          }
	        
          break;
        //-------------------
	      case INTERNAL_CYCLE_ID:
        //-------------------
	        // Cycle is the same for all measures
	        for (Index=0; Index<Count[0]; Index++)
          {
	          passData[Index]	= pass->m_cycle;
          }
	        break;

        //-------------------
	      case INTERNAL_PASS_ID:
        //-------------------
	        // Pass is the same for all measures
	        for (Index=0; Index<Count[0]; Index++)
          {
	          passData[Index]	= pass->m_pass;
          }
	        break;

        //-------------------
	      default:
        //-------------------
	        if (netCdfId < 0)
          {
	          throw CFileException(CTools::Format("INTERNAL FATAL ERROR: Pseudo field id %d unknown. Program not up to date",
				        	      netCdfId),
				       GetName(),
				       BRATHL_LOGIC_ERROR);
          }

	        m_file.ReadVar(varName, 2, Start, Count, passData, CUnit::m_UNIT_SI);

      } // end switch

      if (resetValues)
      {
        var->EmptyValues();
      }
      var->SetValues(passData, Count[0]);
    } // end for

    delete []passData;
  }
  catch (...)
  {
    delete []passData;
    Close();
    throw;
  }

// Return at the beginning of the pass
  m_currentInPass	= 0;

  return true;
}

//----------------------------------------
//bool CExternalFilesATP::LoadPass(const std::string& name, bool resetValues /* = false */)
/*{
  CFieldNetCdf* field = GetFieldNetCdf(name);
  return LoadPass(field, resetValues);
}
*/
//----------------------------------------
bool CExternalFilesATP::LoadPass(CFieldNetCdf* var, bool resetValues /* = false */)
{
  uint32_t			Start[2];
  uint32_t			Count[2];
  CRegisteredPass*		pass;
  uint32_t			Index;
  double			*passData	= NULL;

  if (m_currentPass >= m_passList.size())
  {
    return false;
  }

  try
  {
    pass	= CExternalFilesATP::GetRegisteredPass(m_passList[m_currentPass]);
    Start[0]	= pass->m_startPoint;
    Count[0]	= pass->m_nbData;
    Start[1]	= pass->m_cycleIndex;
    Count[1]	= 1;
    passData	= new double[Count[0]];
    
    int32_t netCdfId = var->GetNetCdfId();

    switch (netCdfId)
    {
      //-------------------
      case INTERNAL_LAT_ID:
      //-------------------
        m_file.ReadVar("Latitudes", 1, Start, Count, passData, CLatLonPoint::m_DEFAULT_UNIT_LATITUDE);
	      break;

      //-------------------
      case INTERNAL_LON_ID:
      //-------------------
    	  m_file.ReadVar("Longitudes", 1, Start, Count, passData, CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE );
	      break;

      //-------------------
      case INTERNAL_DATE_ID:
      //-------------------
	      m_file.ReadVar("DataIndexes", 1, Start, Count, passData, "count");
	      
        for (Index=0; Index<Count[0]; Index++)
        {
	        //passData[Index]	= pass->m_beginDate + (passData[Index]*m_deltaT)/86400.0;
          // pass->m_beginDate is expressed in SI unit ==> seconds since 1950-01-01 00:00:00.0
	        passData[Index]	= pass->m_beginDate + (passData[Index]*m_deltaT);
        }
	      break;

      //-------------------
	    case INTERNAL_BRAT_INDEX_DATA:
      //-------------------
	      
        for (Index=0; Index<Count[0]; Index++)
        {
	        passData[Index]	= Start[0] + Index + m_offset;
        }
	        
        break;
      //-------------------
      case INTERNAL_CYCLE_ID:
      //-------------------
	      // Cycle is the same for all measures
	      for (Index=0; Index<Count[0]; Index++)
        {
	        passData[Index]	= pass->m_cycle;
        }
	      break;

      //-------------------
      case INTERNAL_PASS_ID:
      //-------------------
	      // Pass is the same for all measures
	      for (Index=0; Index<Count[0]; Index++)
        {
	        passData[Index]	= pass->m_pass;
        }
	      break;

      //-------------------
      default:
      //-------------------
	      if (netCdfId < 0)
        {
	        throw CFileException(CTools::Format("INTERNAL FATAL ERROR: Pseudo field id %d unknown. Program not up to date",
				                    netCdfId),
			             GetName(),
			             BRATHL_LOGIC_ERROR);
        }
	      
        m_file.ReadVar(var->GetName(), 2, Start, Count, passData, CUnit::m_UNIT_SI);
    }

    if (resetValues)
    {
      var->EmptyValues();
    }
    var->SetValues(passData, Count[0]);

    if (passData != NULL)
    {
      delete []passData;
      passData = NULL;
    }
  }
  catch (...)
  {
    if (passData != NULL)
    {
      delete []passData;
      passData = NULL;
    }
    Close();
    throw;
  }

// Return at the beginning of the pass
  m_currentInPass	= 0;

  return true;
}




}
