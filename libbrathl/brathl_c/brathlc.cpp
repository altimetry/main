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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/mman.h>
#endif

#include "brathlc.h"
#include "new-gui/Common/tools/brathl_error.h"

#include "Date.h"
#include "Mission.h"
#include "List.h"
#include "new-gui/Common/tools/Exception.h"
#include "Product.h"
#include "Tools.h"
#include "BratAlgorithmBase.h"


/*
#define NUMBER_OF_STATISTICS	5
#define COUNT_INDEX		0
#define MEAN_INDEX		1
#define STDDEV_INDEX		2
#define MIN_INDEX		3
#define MAX_INDEX		4
*/
/*
static void ReadDataForOneMeasure
		(CDataSet			*dataSet,
		 const std::string			&recordName,
		 CExpression			&Select,
		 std::vector<CExpression>		&Expressions,
		 const std::vector<CUnit>		&WantedUnits,
		 double				**results,
		 int32_t			*sizes,
		 int32_t			*actualSize,
		 int				ignoreOutOfRange,
		 int				statistics)
{
  CExpressionValue	exprValue;
  
  
  for (CObArray::iterator itDataSet = dataSet->begin(); itDataSet != dataSet->end() ; itDataSet++)
  {
    CRecord	*record		= dynamic_cast<CRecord*>(*itDataSet);
    CRecordSet	*recordSet	= record->GetRecordSet();

    recordSet->ExecuteExpression(Select, recordName, exprValue);
    if (exprValue.IsTrue() != 1)
    {
      continue;	// Not selected
    }

    if (results != NULL)
    {
      for (uint32_t indexExpr = 0 ; indexExpr < Expressions.size() ; indexExpr++)
      {
	      recordSet->ExecuteExpression(Expressions[indexExpr], recordName, exprValue);

	      int32_t NbValues	= exprValue.GetNbValues();

	      double Value;

	      if (NbValues == 0)
        {
	        Value	= CTools::m_defaultValueDOUBLE;
        }
	      else if (NbValues == 1)
        {
	        Value	= exprValue.GetValues()[0];
        }
	      else
        {
	        throw CException("Field value must be scalar not a std::vector or a matrix",
		                 BRATHL_LIMIT_ERROR);
        }

	      if ((sizes[indexExpr] >= 0) && (*actualSize >= sizes[indexExpr]))
	      {
	        if (! ((sizes[indexExpr] == 0) || ignoreOutOfRange))
          {
            if (!statistics)
            {
	            throw CException(CTools::Format("Too much data to store for given space at least %d values needed",
					            *actualSize),
			             BRATHL_RANGE_ERROR);
            }
            else
            {
	            double *std::vector	= results[indexExpr];
 	            CTools::DoIncrementalStats(Value,
				               std::vector[COUNT_INDEX],
				               std::vector[MEAN_INDEX],
				               std::vector[STDDEV_INDEX],
				               std::vector[MIN_INDEX],
				               std::vector[MAX_INDEX]);
            }
          }
	      }
	      else
	      {
	        if ((sizes[indexExpr] < 0) && (-sizes[indexExpr] <= *actualSize))
	        {// Size of expandable std::vector reached, resize it by doubling its size
	          sizes[indexExpr]	*= 2;
	          void 	*NewPtr	= realloc(results[indexExpr],
					        -sizes[indexExpr]*sizeof(*(results[indexExpr])));
	          if (NewPtr == NULL)
	            throw CMemoryException(CTools::Format("ReadDataForOneMeasure: Not enough memory to allocate std::vector of size %d",
						          -sizes[indexExpr]));

	          results[indexExpr]	= static_cast<double *>(NewPtr);
	        }
	        if (statistics)
	        {
	          double *std::vector	= results[indexExpr];
 	          CTools::DoIncrementalStats(Value,
				             std::vector[COUNT_INDEX],
				             std::vector[MEAN_INDEX],
				             std::vector[STDDEV_INDEX],
				             std::vector[MIN_INDEX],
				             std::vector[MAX_INDEX]);

	        }
	        else
	        {
	          CUnit Unit = WantedUnits[indexExpr];
	          results[indexExpr][*actualSize]	= Unit.Convert(Value);
	        }
	      }
      }
    }
    (*actualSize)++;
  }
}

*/

extern "C" {

LIBRATHL_API int brathl_errno;

//----------------------------------------



LIBRATHL_API const char* brathl_Errno2String(const int32_t err)
{
  switch (err)
  {
    case BRATHL_SUCCESS:                                   return "Success (no error)";
    case BRATHL_ERROR_INVALID_DATE:                        return "Invalid date";
    case BRATHL_ERROR_INVALID_DATE_REF:                    return "Invalid reference date";
    case BRATHL_ERROR_INVALID_DATE_REF_CONV:  	      	   return "Invalid reference date conversion";
    case BRATHL_ERROR_INVALID_DSM:  	      		   return "Invalid days or seconds or microseconds values (must be >= 0)";
    case BRATHL_ERROR_INVALID_YEAR:  	      		   return "Invalid year value (must be >= 1950)";
    case BRATHL_ERROR_INVALID_MONTH:  	      		   return "Invalid month value (must be >= 1 and <= 12)";
    case BRATHL_ERROR_INVALID_DAY:  	      		   return "Invalid day value";
    case BRATHL_ERROR_INVALID_HOUR:  	      		   return "Invalid hour value (must be >= 0 and <= 23)";
    case BRATHL_ERROR_INVALID_MINUTE:  	      		   return "Invalid minute value (must be >= 0 and <= 59)";
    case BRATHL_ERROR_INVALID_SECOND:  	      		   return "Invalid second value (must be >= 0 and <= 59)";
    case BRATHL_ERROR_INVALID_MUSECOND:  	           return "Invalid musecond value (must be >= 0 and <= 999999)";
    case BRATHL_ERROR_INVALID_DATE_NEGATIVE:  	      	   return "Invalid date (date must be > 0)";

    case BRATHL_ERROR_INVALID_NB_PASS:  	      	   return "Invalid nb pass (must be > 0)";
    case BRATHL_ERROR_INVALID_REPETITION:  	      	   return "Invalid repetition (must be > 0)";
    case BRATHL_ERROR_INVALID_MISSION:  	      	   return "Unknown mission value";
    case BRATHL_WARNING_OPEN_FILE_REF_FILE:                return "WARNING - Unable to open reference mission file";
    case BRATHL_WARNING_INVALID_REF_FILE_FIELD:            return "WARNING - Invalid reference mission file format";
    case BRATHL_WARNING_INVALID_REF_FILE_FIELDDATE:        return "WARNING - Invalid reference mission date";

    default:						   return "Unknown error";

  }
}


//----------------------------------------
LIBRATHL_API int32_t brathl_Seconds2DSM(brathl_DateSecond *dateSeconds, brathl_refDate refDate,
      	      	      	      	        brathl_DateDSM *dateDSM)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDate(*dateSeconds);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  dateDSM->refDate = refDate;

  //strcpy(dateDSM->refDateUser, dateSeconds->refDateUser);

  brathl_errno = date.Convert2DSM(dateDSM->days, dateDSM->seconds, dateDSM->muSeconds, refDate);
  return brathl_errno;
}

//----------------------------------------
LIBRATHL_API int32_t brathl_DSM2Seconds(brathl_DateDSM *dateDSM, brathl_refDate refDate,
      	      	      	      	        brathl_DateSecond *dateSeconds)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDate(*dateDSM);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  dateSeconds->refDate = refDate;
  //strcpy(dateSeconds->refDateUser, dateDSM->refDateUser);

  brathl_errno = date.Convert2Second(dateSeconds->nbSeconds, refDate);
  return brathl_errno;
}

//----------------------------------------
LIBRATHL_API int32_t brathl_Julian2DSM(brathl_DateJulian *dateJulian, brathl_refDate refDate,
      	      	      	      	       brathl_DateDSM *dateDSM)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDate(*dateJulian);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  dateDSM->refDate = refDate;
  //strcpy(dateDSM->refDateUser, dateJulian->refDateUser);

  brathl_errno = date.Convert2DSM(dateDSM->days, dateDSM->seconds, dateDSM->muSeconds, refDate);
  return brathl_errno;
}


//----------------------------------------
LIBRATHL_API int32_t brathl_DSM2Julian( brathl_DateDSM *dateDSM, brathl_refDate refDate,
      	      	      	      	        brathl_DateJulian *dateJulian)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDate(*dateDSM);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  dateJulian->refDate = refDate;
  //strcpy(dateJulian->refDateUser, dateDSM->refDateUser);

  brathl_errno = date.Convert2DecimalJulian(dateJulian->julian, refDate);
  return brathl_errno;
}

//----------------------------------------
LIBRATHL_API int32_t brathl_YMDHMSM2DSM(brathl_DateYMDHMSM *dateYMDHMSM, brathl_refDate refDate,
      	      	      	      	       brathl_DateDSM *dateDSM)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDate(*dateYMDHMSM);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  dateDSM->refDate = refDate;
  //strcpy(dateDSM->refDateUser, dateJulian->refDateUser);

  brathl_errno = date.Convert2DSM(dateDSM->days, dateDSM->seconds, dateDSM->muSeconds, refDate);
  return brathl_errno;
}


//----------------------------------------
LIBRATHL_API int32_t brathl_DSM2YMDHMSM( brathl_DateDSM *dateDSM,
      	      	      	      	        brathl_DateYMDHMSM *dateYMDHMSM)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDate(*dateDSM);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  brathl_errno = date.Convert2YMDHMSM(dateYMDHMSM->year, dateYMDHMSM->month, dateYMDHMSM->day,
      	      	      	  dateYMDHMSM->hour, dateYMDHMSM->minute, dateYMDHMSM->second,
      	      	      	  dateYMDHMSM->muSecond);
  return brathl_errno;
}


//----------------------------------------

LIBRATHL_API int32_t brathl_Seconds2Julian(brathl_DateSecond *dateSeconds, brathl_refDate refDate,
      	      	      	      	           brathl_DateJulian *dateJulian)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDate(*dateSeconds);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  dateJulian->refDate = refDate;


  brathl_errno = date.Convert2DecimalJulian(dateJulian->julian, refDate);
  return brathl_errno;
}

//----------------------------------------

LIBRATHL_API int32_t brathl_Julian2Seconds(brathl_DateJulian *dateJulian, brathl_refDate refDate,
      	      	      	      	           brathl_DateSecond *dateSeconds)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDate(*dateJulian);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  dateSeconds->refDate = refDate;


  brathl_errno = date.Convert2Second(dateSeconds->nbSeconds, refDate);
  return brathl_errno;
}

//----------------------------------------

LIBRATHL_API int32_t brathl_Seconds2YMDHMSM(brathl_DateSecond *dateSeconds,
      	      	      	      	        brathl_DateYMDHMSM *dateYMDHMSM)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDate(*dateSeconds);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  brathl_errno = date.Convert2YMDHMSM(dateYMDHMSM->year, dateYMDHMSM->month, dateYMDHMSM->day,
      	      	      	  dateYMDHMSM->hour, dateYMDHMSM->minute, dateYMDHMSM->second,
      	      	      	  dateYMDHMSM->muSecond);
  return brathl_errno;
}

//----------------------------------------
LIBRATHL_API int32_t brathl_YMDHMSM2Seconds(brathl_DateYMDHMSM *dateYMDHMSM, brathl_refDate refDate,
      	      	      	      	       brathl_DateSecond *dateSeconds)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDate(*dateYMDHMSM);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  dateSeconds->refDate = refDate;

  brathl_errno = date.Convert2Second(dateSeconds->nbSeconds, refDate);
  return brathl_errno;
}

//----------------------------------------

LIBRATHL_API int32_t brathl_Julian2YMDHMSM(brathl_DateJulian *dateJulian,
      	      	      	      	        brathl_DateYMDHMSM *dateYMDHMSM)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDate(*dateJulian);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  brathl_errno = date.Convert2YMDHMSM(dateYMDHMSM->year, dateYMDHMSM->month, dateYMDHMSM->day,
      	      	      	  dateYMDHMSM->hour, dateYMDHMSM->minute, dateYMDHMSM->second,
      	      	      	  dateYMDHMSM->muSecond);
  return brathl_errno;
}


//----------------------------------------
LIBRATHL_API int32_t brathl_YMDHMSM2Julian(brathl_DateYMDHMSM *dateYMDHMSM, brathl_refDate refDate,
      	      	      	      	       brathl_DateJulian *dateJulian)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDate(*dateYMDHMSM);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  dateJulian->refDate = refDate;

  brathl_errno = date.Convert2DecimalJulian(dateJulian->julian, refDate);
  return brathl_errno;
}

//----------------------------------------
LIBRATHL_API int32_t brathl_NowYMDHMSM(brathl_DateYMDHMSM *dateYMDHMSM)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDateNow();

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  brathl_errno = date.Convert2YMDHMSM(dateYMDHMSM->year, dateYMDHMSM->month, dateYMDHMSM->day,
      	      	      	  dateYMDHMSM->hour, dateYMDHMSM->minute, dateYMDHMSM->second,
      	      	      	  dateYMDHMSM->muSecond);
  return brathl_errno;
}

//----------------------------------------
LIBRATHL_API int32_t brathl_DayOfYear(brathl_DateYMDHMSM *dateYMDHMSM, uint32_t *dayOfYear)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date;
  brathl_errno = date.SetDate(*dateYMDHMSM);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  *dayOfYear = date.DayOfYear();
  return brathl_errno;
}

//----------------------------------------
LIBRATHL_API int32_t brathl_DiffYMDHMSM(brathl_DateYMDHMSM *dateYMDHMSM1, brathl_DateYMDHMSM *dateYMDHMSM2, double *diff)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date1;
  brathl_errno = date1.SetDate(*dateYMDHMSM1);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  CDate date2;
  brathl_errno = date2.SetDate(*dateYMDHMSM2);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  *diff = date1 - date2;
  return brathl_errno;

}

//----------------------------------------
LIBRATHL_API int32_t brathl_DiffDSM(brathl_DateDSM *dateDSM1, brathl_DateDSM *dateDSM2, double *diff)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date1;
  brathl_errno = date1.SetDate(*dateDSM1);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  CDate date2;
  brathl_errno = date2.SetDate(*dateDSM2);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  *diff = date1 - date2;
  return brathl_errno;

}

//----------------------------------------
LIBRATHL_API int32_t brathl_DiffJulian(brathl_DateJulian *dateJulian1, brathl_DateJulian *dateJulian2, double *diff)
{
  brathl_errno = BRATHL_SUCCESS;
  CDate date1;
  brathl_errno = date1.SetDate(*dateJulian1);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  CDate date2;
  brathl_errno = date2.SetDate(*dateJulian2);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  *diff = date1 - date2;
  return brathl_errno;

}


//----------------------------------------

LIBRATHL_API int32_t brathl_Cycle2YMDHMSM(brathl_mission mission, uint32_t cycle, uint32_t pass,
      	      	      	      	      	  brathl_DateYMDHMSM *dateYMDHMSM)
{
  brathl_errno = BRATHL_SUCCESS;

  CDate date;
  CMission m(mission);

  brathl_errno = m.CtrlMission();

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  brathl_errno = m.Convert(cycle, pass, date);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  brathl_errno = date.Convert2YMDHMSM(dateYMDHMSM->year, dateYMDHMSM->month, dateYMDHMSM->day,
      	      	      	  dateYMDHMSM->hour, dateYMDHMSM->minute, dateYMDHMSM->second,
      	      	      	  dateYMDHMSM->muSecond);

  return brathl_errno;

}

//----------------------------------------

LIBRATHL_API int32_t brathl_YMDHMSM2Cycle(brathl_mission mission, brathl_DateYMDHMSM *dateYMDHMSM,
      	      	      	      	          uint32_t *cycle, uint32_t *pass)
{
  brathl_errno = BRATHL_SUCCESS;

  CDate date;
  CMission m(mission);

  brathl_errno = m.CtrlMission();

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  brathl_errno = date.SetDate(*dateYMDHMSM);

  if (brathl_errno != BRATHL_SUCCESS)
  {
    return brathl_errno;
  }

  brathl_errno = m.Convert(date, *cycle, *pass);

  return brathl_errno;

}
//----------------------------------------

/*
LIBRATHL_API int32_t brathl_ReadData
		(int32_t	nbFiles,
		 char		**fileNames,
		 const char	*recordName,
		 const char	*selection,
		 int32_t	nbData,
		 char		**dataExpressions,
		 char		**units,
		 double		**results,
		 int32_t	sizes[],
		 int32_t	*actualSize,
		 int		ignoreOutOfRange,
		 int		statistics,
		 double		defaultValue
		 )
{
  CProduct	*Product	= NULL;
  int32_t	Index;
  brathl_errno = BRATHL_SUCCESS;

  std::string	strRecordName(recordName);

  try
  {
    CExpression			Select(CTools::IsEmpty(selection) ? "1" : selection);
    std::vector<CExpression>		Expressions;
    CUIntArray		Positions;
    CStringList			ListFieldsToRead;
    std::vector<CUnit>		WantedUnits;

    CStringArray		FileList;
    
    if (ignoreOutOfRange && statistics)
    {
      throw CException("brathl_ReadData: Cannot ignore out of range when doing statistics",
		       BRATHL_INCONSISTENCY_ERROR);
    }

    // Build file list
    for (Index=0; Index < nbFiles; Index++)
    {      
      if (! CTools::IsEmpty(fileNames[Index]))
      {
	      FileList.Insert(fileNames[Index]);
      }
    }
    // Check expressions
    for (Index=0; Index<nbData; Index++)
    {
      CExpression	Expr;
      CUnit		Unit;
      
      if (CTools::IsEmpty(dataExpressions[Index]))
      {
        Expr.SetExpression("DV");
      }
      else
      {
        Expr.SetExpression(dataExpressions[Index]);
      	ListFieldsToRead.InsertUnique(Expr.GetFieldNames());
      }
      
      Expressions.push_back(Expr);

      if ((units == NULL) || CTools::IsEmpty(units[Index]))
      {
        Unit	= "count";
      }
      else
      {
      	Unit	= units[Index];
        Unit.SetConversionFrom(Unit.BaseUnit());
      }

      WantedUnits.push_back(Unit);
      
      if (results == NULL)
      {
	      if (sizes != NULL)
	      {
	        throw CException("brathl_ReadData: if 'results' is NULL, 'sizes' must also be NULL",
			         BRATHL_INCONSISTENCY_ERROR);
	      }
      }
      else
      {
	      if (sizes == NULL)
	      {
	        throw CException("brathl_ReadData: if 'results' is not NULL, 'sizes' must also not be NULL",
			         BRATHL_INCONSISTENCY_ERROR);
	      }
	      if (sizes[Index] != 0)
	      {
	        if (sizes[Index] < 0)
	        {	    
	          if (results[Index] != NULL)
	            throw CException("brathl_ReadData: if 'sizes' is negative, corresponding 'results' must be NULL",
			             BRATHL_INCONSISTENCY_ERROR);
	          results[Index]	= static_cast<double *>(malloc(4096 * sizeof(*(results[Index]))));
	          if (results[Index] == NULL)
	            throw CMemoryException("brathl_ReadData: not enough memory for initial vectors");
	          sizes[Index]	= -4096;
	        }
	        else if ((results[Index] == NULL) && (sizes[Index] > 0))
	        {
	          throw CException("brathl_ReadData: if 'sizes' is positive, corresponding 'results' must not be NULL",
			           BRATHL_INCONSISTENCY_ERROR);
	        }
	        else if ((sizes[Index] < NUMBER_OF_STATISTICS) && statistics)
	        {
	          throw CException(CTools::Format("brathl_ReadData: when statistics are asked, size of result must be at least %d, not %d",
					          NUMBER_OF_STATISTICS,
					          sizes[Index]),
			           BRATHL_INCONSISTENCY_ERROR);
	        }
	        // Initialises statistics
	        if (statistics)
 	        {
	          for (int StatIndex=0; StatIndex < NUMBER_OF_STATISTICS; StatIndex++)
            {
	            results[Index][StatIndex]	= CTools::m_defaultValueDOUBLE;
            }
	          results[Index][COUNT_INDEX]	= 0.0;
	        }
	      }
      }
    }

    ListFieldsToRead.InsertUnique(Select.GetFieldNames());
        
    Product	= CProduct::Construct(FileList);

    *actualSize	= 0;

    uint32_t nbFiles = FileList.size();
    uint32_t cptFile = 0;
    
    Product->SetExpandArray(true);
    
    for (CStringArray::iterator itFile = FileList.begin() ; itFile != FileList.end() ; itFile++)
    {
      cptFile++;

      CTrace::Tracer(1,"File %d/%d - Reading record data from %s ... and registering data ...",
                   cptFile, nbFiles, (*itFile).c_str());


      Product->SetForceReadDataOneByOne(true);
      Product->Open(*itFile, strRecordName, ListFieldsToRead);


      uint32_t nRecords = Product->GetNumberOfRecords();


      for (uint32_t iRecord = 0 ; iRecord < nRecords ; iRecord++)
      {
	      CDataSet* dataSet = NULL;
        Product->ReadBratRecord(iRecord);
	      dataSet = Product->GetDataSet();

	      ReadDataForOneMeasure(dataSet, strRecordName, Select, Expressions, WantedUnits, results, sizes, actualSize, ignoreOutOfRange, statistics);
      }

      Product->Close();
    }

    if (statistics)
    {
      *actualSize	= NUMBER_OF_STATISTICS;
    }
    if (sizes != NULL)
    {
      // Set expandable vectors to fixed (final) size
      for (Index=0; Index<nbData; Index++)
      {
	      double *std::vector	= results[Index];
	      sizes[Index]	= abs(sizes[Index]);
	      // Finalize statistics
	      if (statistics && (sizes[Index] != 0) && (std::vector[COUNT_INDEX] != 0.0))
	      {// Compute final STDDEV
 	        CTools::FinalizeIncrementalStats(std::vector[COUNT_INDEX],
					         std::vector[MEAN_INDEX],
					         std::vector[STDDEV_INDEX],
					         std::vector[MIN_INDEX],
					         std::vector[MAX_INDEX]);
	      }
	      else
	      {
	        for (int32_t IndexVal=0; IndexVal<*actualSize; IndexVal++)
	        {
	          if (CTools::IsDefaultValue(std::vector[IndexVal]))
            {
	            std::vector[IndexVal]	= defaultValue;
            }
	        }
	      }
      }
    }
  }
  catch (CException &e)
  {
    brathl_errno = e.error();
    std::cerr << "ERROR brathl_ReadData:" << e.what() << std::endl;
  }
  catch (...)
  {
    brathl_errno = BRATHL_ERROR;
  }

  // Free resources
  try
  {
    delete Product;
  }
  catch (CException &e)
  {
    brathl_errno = e.error();
    std::cerr << "ERROR brathl_ReadData:" << e.what() << std::endl;
  }
  catch (...)
  {
    brathl_errno = BRATHL_ERROR;
  }

  return brathl_errno;
}

*/
//----------------------------------------

LIBRATHL_API int32_t brathl_ReadData
		(int32_t	nbFiles,
		 char		**fileNames,
		 const char	*recordName,
		 const char	*selection,
		 int32_t	nbData,
		 char		**dataExpressions,
		 char		**units,
		 double		**results,
		 int32_t	sizes[],
		 size_t		*actualSize,
		 int		ignoreOutOfRange,
		 int		statistics,
		 double		defaultValue
		 )
{
  brathl_errno = BRATHL_SUCCESS;

  try
  {
    brathl_errno = CProduct::ReadData( nbFiles,
		                                   fileNames,
		                                   recordName,
		                                   selection,
		                                   nbData,
		                                   dataExpressions,
		                                   units,
		                                   results,
		                                   sizes,
		                                   actualSize,
		                                   ignoreOutOfRange,
		                                   statistics,
		                                   defaultValue
                                      );

  }
  catch (CException &e)
  {
    brathl_errno = e.error();
    std::cerr << "ERROR brathl_ReadData:" << e.what() << std::endl;
  }
  catch (...)
  {
    brathl_errno = BRATHL_ERROR;
  }

  return brathl_errno;
}


//----------------------------------------

LIBRATHL_API void brathl_RegisterAlgorithms()
{	
  // Register Brat algorithms
  CBratAlgorithmBase::RegisterCAlgorithms();

}

//----------------------------------------
LIBRATHL_API void brathl_LoadAliasesDictionary()
{	
  // Load aliases dictionnary
  std::string errorMsg;
  CAliasesDictionary::LoadAliasesDictionary(&errorMsg, false);
  if (!(errorMsg.empty())) 
  {
    std::cerr << "WARNING: " << errorMsg << std::endl;
  }

}

} // end extern "C"