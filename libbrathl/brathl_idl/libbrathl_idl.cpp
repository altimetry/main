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
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "export.h"

#include "brathl_error.h" 
#include "brathlc.h" 
#include "brathl.h" 

#include "Tools.h"

static void brathli_SetDataDir(void)
{
    if (getenv("IDL_DLM_PATH") == NULL)
    {
        return;
    }
    string filepath = CTools::FindFileInPath("brathl-idl.dlm", getenv("IDL_DLM_PATH"));
    if (filepath.empty())
    {
        return;
    }
    string datadir = CTools::MakeCorrectPath(CTools::AbsolutePath(CTools::DirName(filepath) +
                                                                  (PATH_SEPARATOR ".." PATH_SEPARATOR "data")));
    if (!datadir.empty())
    {
        CTools::SetDataDir(datadir);
    }
}

#if defined( __cplusplus )
extern "C" {
#endif  /* __cplusplus */

static int brathli_option_verbose = 1;


IDL_StructDefPtr brathli_DateYMDHMSM_sdef;
IDL_StructDefPtr brathli_DateDSM_sdef;
IDL_StructDefPtr brathli_DateSecond_sdef;
IDL_StructDefPtr brathli_DateJulian_sdef;

IDL_StructDefPtr brathli_Error_sdef;


struct IDL_BrathlError
{
    short errnr;
    IDL_STRING message;
};

/*
static IDL_VPTR brathli_IsDateYMDHMSM(int argc, IDL_VPTR *argv)
{
    IDL_VPTR retval;

    assert(argc == 1);
    idl_brat_load_dd(1);

    retval = IDL_Gettmp();
    retval->type = IDL_TYP_INT;
    retval->value.i = 0;

    if ((argv[0]->type == IDL_TYP_STRUCT) && (argv[0]->value.s.sdef == brathli_DateYMDHMSM_sdef))
    {
        retval->value.i = 1;
    }

    return retval;
}

*/
#define BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEDSM                           (-1001)
#define BRATHL_IDL_ERR_EXPECTED_STRUCT_DATESECOND                        (-1002)
#define BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEJULIAN                        (-1003)
#define BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM                       (-1004)
#define BRATHL_IDL_ERR_INVALID_NUMBER_OF_UNITS	                         (-1005)
#define BRATHL_IDL_ERR_INVALID_NUMBER_OF_DATA	                         (-1006)
    
static void free_cb(UCHAR* ptr)
{
  //printf("free_cb freeing 0x%x\n", ptr); 
  free(ptr);
}

static void brathli_FillError(struct IDL_BrathlError *fill, const short err)
{
    const char *message;

    switch (err)
    {      
        case BRATHL_IDL_ERR_INVALID_NUMBER_OF_DATA:
            message = "Number of data (results) must be equal to number of data expression";
            break;
        case BRATHL_IDL_ERR_INVALID_NUMBER_OF_UNITS:
            message = "Number of units must be either 0 or equal to number of data expression";
            break;
        case BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEDSM:
            message = "BRATHL_DATEDSM structure parameter expected";
            break;
        case BRATHL_IDL_ERR_EXPECTED_STRUCT_DATESECOND:
            message = "BRATHL_DATESECOND structure parameter expected";
            break;
        case BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEJULIAN:
            message = "BRATHL_DATEJULIAN structure parameter expected";
            break;
        case BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM:
            message = "BRATHL_DATEYMDHMSM structure parameter expected";
            break;
        default:
            message = brathl_Errno2String(err);
    }

    /* fill ERRNR and MESSAGE fields */
    fill->errnr = err;
    IDL_StrStore(&fill->message, (char *)message);
}



static IDL_VPTR brathli_IsError(int argc, IDL_VPTR *argv)
{
    IDL_VPTR retval;

    assert(argc == 1);

    retval = IDL_Gettmp();
    retval->type = IDL_TYP_INT;
    retval->value.i = 0;

    if ((argv[0]->type == IDL_TYP_STRUCT) && (argv[0]->value.s.sdef == brathli_Error_sdef))
    {
        struct IDL_BrathlError *error = (struct IDL_BrathlError *)argv[0]->value.s.arr->data;

        if (error->errnr != BRATHL_SUCCESS)
        {
            retval->value.i = 1;
        }
    }

    return retval;
}



static IDL_VPTR brathli_MkError(const int32_t err)
{
    IDL_VPTR retval;
    struct IDL_BrathlError *data;

    data = (struct IDL_BrathlError *)IDL_MakeTempStructVector(brathli_Error_sdef, 1, &retval, FALSE);
    brathli_FillError(data, err);

    if (data->errnr != BRATHL_SUCCESS && brathli_option_verbose)
    {
        char errmsg[1000];

        sprintf(errmsg, "BRATHL-IDL ERROR %d: \"%s\"\n", data->errnr, IDL_STRING_STR(&data->message));
        IDL_Message(IDL_M_GENERIC, IDL_MSG_INFO, errmsg);
    }

    return retval;
}

static IDL_VPTR brathli_MkErrorStr(const int32_t err, const char * message)
{
    IDL_VPTR retval;
    struct IDL_BrathlError *data;

    data = (struct IDL_BrathlError *)IDL_MakeTempStructVector(brathli_Error_sdef, 1, &retval, FALSE);
    /* fill ERRNR and MESSAGE fields */
    data->errnr = err;
    IDL_StrStore(&data->message, (char *)message);

    return retval;
}



/*
static IDL_VPTR brathli_test(int argc, IDL_VPTR *argv)
{
    IDL_VPTR retval;
    IDL_VPTR v;
    IDL_MEMINT n; 

    uint32_t l = IDL_ULongScalar(argv[0]);
   
    
    v = argv[1];
    //int32_t result =  brathl_test(l, &((argv[1])->value.ul)); 
    int32_t result =  brathl_test(l, (uint32_t*)&(v->value.ul)); 
    
    printf("refUser1 %s\n", brathl_refDateUser1);
    printf("refUser2 %s\n", brathl_refDateUser2);
    ///(argv[1])->value.ul = lr;
    retval = brathli_MkError(result);
    
    return retval;
}
*/

//------------------------------------------------------
static IDL_VPTR brathli_Seconds2DSM(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_refDate refDate = REF19500101;
  brathl_DateSecond *dateSecond = NULL;
  brathl_DateDSM *dateDSM = NULL;
  
  v0 = argv[0];
  refDate = (brathl_refDate)((argv[1])->value.l);
  v2 = argv[2];

  if (v0->value.s.sdef != brathli_DateSecond_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATESECOND);
    return retval;
  }

  if (v2->value.s.sdef != brathli_DateDSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEDSM);
    return retval;
  }
  
  dateSecond = (brathl_DateSecond *)v0->value.s.arr->data;
  dateDSM = (brathl_DateDSM *)v2->value.s.arr->data;


  result = brathl_Seconds2DSM(dateSecond, refDate, 
      	      	      	      dateDSM);
  
  retval = brathli_MkError(result);
  return retval;
}


//------------------------------------------------------
static IDL_VPTR brathli_DSM2Seconds(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_refDate refDate = REF19500101;
  brathl_DateSecond *dateSecond = NULL;
  brathl_DateDSM *dateDSM = NULL;
  
  v0 = argv[0];
  refDate = (brathl_refDate)((argv[1])->value.l);
  v2 = argv[2];

  if (v0->value.s.sdef != brathli_DateDSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEDSM);
    return retval;
  }

  if (v2->value.s.sdef != brathli_DateSecond_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATESECOND);
    return retval;
  }
  
  dateDSM = (brathl_DateDSM *)v0->value.s.arr->data;
  dateSecond = (brathl_DateSecond *)v2->value.s.arr->data;


  result = brathl_DSM2Seconds(dateDSM, refDate, 
      	      	      	      dateSecond);
  
  retval = brathli_MkError(result);
  return retval;
}


//------------------------------------------------------
static IDL_VPTR brathli_Julian2DSM(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_refDate refDate = REF19500101;
  brathl_DateJulian *dateJulian = NULL;
  brathl_DateDSM *dateDSM = NULL;
  
  v0 = argv[0];
  refDate = (brathl_refDate)((argv[1])->value.l);
  v2 = argv[2];

  if (v0->value.s.sdef != brathli_DateJulian_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEJULIAN);
    return retval;
  }

  if (v2->value.s.sdef != brathli_DateDSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEDSM);
    return retval;
  }


  dateJulian = (brathl_DateJulian *)v0->value.s.arr->data;
  dateDSM = (brathl_DateDSM *)v2->value.s.arr->data;


  result = brathl_Julian2DSM(dateJulian, refDate, dateDSM);
  
  retval = brathli_MkError(result);
  return retval;
}


//------------------------------------------------------
static IDL_VPTR brathli_DSM2Julian(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_refDate refDate = REF19500101;
  brathl_DateJulian *dateJulian = NULL;
  brathl_DateDSM *dateDSM = NULL;
  
  v0 = argv[0];
  refDate = (brathl_refDate)((argv[1])->value.l);
  v2 = argv[2];

  if (v0->value.s.sdef != brathli_DateDSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEDSM);
    return retval;
  }

  if (v2->value.s.sdef != brathli_DateJulian_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEJULIAN);
    return retval;
  }


  dateDSM = (brathl_DateDSM *)v0->value.s.arr->data;
  dateJulian = (brathl_DateJulian *)v2->value.s.arr->data;


  result = brathl_DSM2Julian(dateDSM, refDate, dateJulian);
  
  retval = brathli_MkError(result);
  return retval;
}


//------------------------------------------------------
static IDL_VPTR brathli_YMDHMSM2DSM(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_refDate refDate = REF19500101;
  brathl_DateYMDHMSM *dateYMDHMSM = NULL;
  brathl_DateDSM *dateDSM = NULL;
  
  v0 = argv[0];
  refDate = (brathl_refDate)((argv[1])->value.l);
  v2 = argv[2];

  if (v0->value.s.sdef != brathli_DateYMDHMSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM);
    return retval;
  }

  if (v2->value.s.sdef != brathli_DateDSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEDSM);
    return retval;
  }


  dateYMDHMSM = (brathl_DateYMDHMSM *)v0->value.s.arr->data;
  dateDSM = (brathl_DateDSM *)v2->value.s.arr->data;


  result = brathl_YMDHMSM2DSM(dateYMDHMSM, refDate, dateDSM);
  
  retval = brathli_MkError(result);
  return retval;
}


//------------------------------------------------------
static IDL_VPTR brathli_DSM2YMDHMSM(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_DateYMDHMSM *dateYMDHMSM = NULL;
  brathl_DateDSM *dateDSM = NULL;
  
  v0 = argv[0];
  v2 = argv[1];


  if (v0->value.s.sdef != brathli_DateDSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEDSM);
    return retval;
  }

  if (v2->value.s.sdef != brathli_DateYMDHMSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM);
    return retval;
  }

  dateDSM = (brathl_DateDSM *)v0->value.s.arr->data;
  dateYMDHMSM = (brathl_DateYMDHMSM *)v2->value.s.arr->data;

  result = brathl_DSM2YMDHMSM(dateDSM, dateYMDHMSM);
  
  retval = brathli_MkError(result);
  return retval;
}


//------------------------------------------------------
static IDL_VPTR brathli_Seconds2Julian(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_refDate refDate = REF19500101;
  brathl_DateSecond *dateSecond = NULL;
  brathl_DateJulian *dateJulian = NULL;
  
  v0 = argv[0];
  refDate = (brathl_refDate)((argv[1])->value.l);
  v2 = argv[2];

  if (v0->value.s.sdef != brathli_DateSecond_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATESECOND);
    return retval;
  }

  if (v2->value.s.sdef != brathli_DateJulian_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEJULIAN);
    return retval;
  }
  
  dateSecond = (brathl_DateSecond *)v0->value.s.arr->data;
  dateJulian = (brathl_DateJulian *)v2->value.s.arr->data;


  result = brathl_Seconds2Julian(dateSecond, refDate, 
      	      	      	         dateJulian);
  
  retval = brathli_MkError(result);
  return retval;
}


//------------------------------------------------------
static IDL_VPTR brathli_Julian2Seconds(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_refDate refDate = REF19500101;
  brathl_DateSecond *dateSecond = NULL;
  brathl_DateJulian *dateJulian = NULL;
  
  v0 = argv[0];
  refDate = (brathl_refDate)((argv[1])->value.l);
  v2 = argv[2];

  if (v0->value.s.sdef != brathli_DateJulian_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEJULIAN);
    return retval;
  }

  if (v2->value.s.sdef != brathli_DateSecond_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATESECOND);
    return retval;
  }

  
  dateJulian = (brathl_DateJulian *)v0->value.s.arr->data;
  dateSecond = (brathl_DateSecond *)v2->value.s.arr->data;


  result = brathl_Julian2Seconds(dateJulian, refDate, 
      	      	      	         dateSecond);
  
  retval = brathli_MkError(result);
  return retval;
}

//------------------------------------------------------
static IDL_VPTR brathli_YMDHMSM2Seconds(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_refDate refDate = REF19500101;
  brathl_DateYMDHMSM *dateYMDHMSM = NULL;
  brathl_DateSecond *dateSecond = NULL;
  
  v0 = argv[0];
  refDate = (brathl_refDate)((argv[1])->value.l);
  v2 = argv[2];

  if (v0->value.s.sdef != brathli_DateYMDHMSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM);
    return retval;
  }

  if (v2->value.s.sdef != brathli_DateSecond_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATESECOND);
    return retval;
  }


  dateYMDHMSM = (brathl_DateYMDHMSM *)v0->value.s.arr->data;
  dateSecond = (brathl_DateSecond *)v2->value.s.arr->data;


  result = brathl_YMDHMSM2Seconds(dateYMDHMSM, refDate, dateSecond);
  
  retval = brathli_MkError(result);
  return retval;
}

//------------------------------------------------------
static IDL_VPTR brathli_Seconds2YMDHMSM(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_DateYMDHMSM *dateYMDHMSM = NULL;
  brathl_DateSecond *dateSecond = NULL;
  
  v0 = argv[0];
  v2 = argv[1];


  if (v0->value.s.sdef != brathli_DateSecond_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATESECOND);
    return retval;
  }
  
  if (v2->value.s.sdef != brathli_DateYMDHMSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM);
    return retval;
  }


  dateSecond = (brathl_DateSecond *)v0->value.s.arr->data;
  dateYMDHMSM = (brathl_DateYMDHMSM *)v2->value.s.arr->data;


  result = brathl_Seconds2YMDHMSM(dateSecond, dateYMDHMSM);
  
  retval = brathli_MkError(result);
  return retval;
}


//------------------------------------------------------
static IDL_VPTR brathli_YMDHMSM2Julian(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_refDate refDate = REF19500101;
  brathl_DateYMDHMSM *dateYMDHMSM = NULL;
  brathl_DateJulian *dateJulian = NULL;
  
  v0 = argv[0];
  refDate = (brathl_refDate)((argv[1])->value.l);
  v2 = argv[2];



  if (v0->value.s.sdef != brathli_DateYMDHMSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM);
    return retval;
  }

  if (v2->value.s.sdef != brathli_DateJulian_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEJULIAN);
    return retval;
  }


  dateYMDHMSM = (brathl_DateYMDHMSM *)v0->value.s.arr->data;
  dateJulian = (brathl_DateJulian *)v2->value.s.arr->data;


  result = brathl_YMDHMSM2Julian(dateYMDHMSM, refDate, dateJulian);
  
  retval = brathli_MkError(result);
  return retval;
}

//------------------------------------------------------
static IDL_VPTR brathli_Julian2YMDHMSM(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_DateYMDHMSM *dateYMDHMSM = NULL;
  brathl_DateJulian *dateJulian = NULL;
  
  v0 = argv[0];
  v2 = argv[1];



  if (v0->value.s.sdef != brathli_DateJulian_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEJULIAN);
    return retval;
  }
  
  if (v2->value.s.sdef != brathli_DateYMDHMSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM);
    return retval;
  }


  dateJulian = (brathl_DateJulian *)v0->value.s.arr->data;
  dateYMDHMSM = (brathl_DateYMDHMSM *)v2->value.s.arr->data;


  result = brathl_Julian2YMDHMSM(dateJulian, dateYMDHMSM);
  
  retval = brathli_MkError(result);
  return retval;
}

//------------------------------------------------------
static IDL_VPTR brathli_NowYMDHMSM(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  brathl_DateYMDHMSM *dateYMDHMSM = NULL;
  
  v0 = argv[0];


  
  if (v0->value.s.sdef != brathli_DateYMDHMSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM);
    return retval;
  }


  dateYMDHMSM = (brathl_DateYMDHMSM *)v0->value.s.arr->data;


  result = brathl_NowYMDHMSM(dateYMDHMSM);
  
  retval = brathli_MkError(result);
  return retval;
}

//------------------------------------------------------
static IDL_VPTR brathli_DayOfYear(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  brathl_DateYMDHMSM *dateYMDHMSM = NULL;
  
  v0 = argv[0];
  v1 = argv[1];
  
  if (v0->value.s.sdef != brathli_DateYMDHMSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM);
    return retval;
  }


  dateYMDHMSM = (brathl_DateYMDHMSM *)v0->value.s.arr->data;


  result = brathl_DayOfYear(dateYMDHMSM, (uint32_t*)&(v1->value.ul));
  
  retval = brathli_MkError(result);
  return retval;
}


//------------------------------------------------------
static IDL_VPTR brathli_DiffYMDHMSM(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_DateYMDHMSM *d0 = NULL;
  brathl_DateYMDHMSM *d1 = NULL;
  
  v0 = argv[0];
  v1 = argv[1];
  v2 = argv[2];



  if (v0->value.s.sdef != brathli_DateYMDHMSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM);
    return retval;
  }
  
  if (v1->value.s.sdef != brathli_DateYMDHMSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM);
    return retval;
  }

  d0 = (brathl_DateYMDHMSM *)v0->value.s.arr->data;
  d1 = (brathl_DateYMDHMSM *)v1->value.s.arr->data;


  result = brathl_DiffYMDHMSM(d0, d1, &(v2->value.d));
  
  retval = brathli_MkError(result);
  return retval;
}

//------------------------------------------------------
static IDL_VPTR brathli_DiffDSM(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_DateDSM *d0 = NULL;
  brathl_DateDSM *d1 = NULL;
  
  v0 = argv[0];
  v1 = argv[1];
  v2 = argv[2];



  if (v0->value.s.sdef != brathli_DateDSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEDSM);
    return retval;
  }

  if (v1->value.s.sdef != brathli_DateDSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEDSM);
    return retval;
  }

  d0 = (brathl_DateDSM *)v0->value.s.arr->data;
  d1 = (brathl_DateDSM *)v1->value.s.arr->data;


  result = brathl_DiffDSM(d0, d1, &(v2->value.d));
  
  retval = brathli_MkError(result);
  return retval;
}

//------------------------------------------------------
static IDL_VPTR brathli_DiffJulian(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  IDL_VPTR v1;
  IDL_VPTR v2;
  brathl_DateJulian *d0 = NULL;
  brathl_DateJulian *d1 = NULL;
  
  v0 = argv[0];
  v1 = argv[1];
  v2 = argv[2];



  if (v0->value.s.sdef != brathli_DateJulian_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEJULIAN);
    return retval;
  }


  if (v1->value.s.sdef != brathli_DateJulian_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEJULIAN);
    return retval;
  }

  d0 = (brathl_DateJulian *)v0->value.s.arr->data;
  d1 = (brathl_DateJulian *)v1->value.s.arr->data;


  result = brathl_DiffJulian(d0, d1, &(v2->value.d));
  
  retval = brathli_MkError(result);
  return retval;
}



//------------------------------------------------------

static IDL_VPTR brathli_SetRefUser1(int argc, IDL_VPTR *argv)
{
  
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  const char *dateRefUser;
  
  v0 = argv[0];
  
  IDL_ENSURE_STRING(v0);

  dateRefUser = IDL_STRING_STR(&v0->value.str);
  
  int32_t lenSrc = strlen(dateRefUser);
  int32_t lenToCopy = ((lenSrc > BRATHL_REF_DATE_USER_LEN) ? BRATHL_REF_DATE_USER_LEN - 1 : lenSrc);
  strncpy (brathl_refDateUser1, dateRefUser, lenToCopy);
  
  retval = brathli_MkError(result);
  return retval;
   

}

//------------------------------------------------------

static IDL_VPTR brathli_SetRefUser2(int argc, IDL_VPTR *argv)
{
  
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v0;
  const char *dateRefUser;
  
  v0 = argv[0];
  
  IDL_ENSURE_STRING(v0);

  dateRefUser = IDL_STRING_STR(&v0->value.str);
  
  int32_t lenSrc = strlen(dateRefUser);
  int32_t lenToCopy = ((lenSrc > BRATHL_REF_DATE_USER_LEN) ? BRATHL_REF_DATE_USER_LEN - 1 : lenSrc);
  strncpy (brathl_refDateUser2, dateRefUser, lenToCopy);
  
  retval = brathli_MkError(result);
  return retval;
   

}


//------------------------------------------------------
static IDL_VPTR brathli_Cycle2YMDHMSM(int argc, IDL_VPTR *argv)
{
    
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v3;
  brathl_mission mission = TOPEX;
  uint32_t cycle = 0;
  uint32_t pass = 0;
  brathl_DateYMDHMSM *dateYMDHMSM = NULL;
  
  mission = (brathl_mission)(IDL_ULongScalar(argv[0]));
  cycle = IDL_ULongScalar(argv[1]);
  pass = IDL_ULongScalar(argv[2]);
 
  v3 = argv[3];
 
  if (v3->value.s.sdef != brathli_DateYMDHMSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM);
    return retval;
  }


  dateYMDHMSM = (brathl_DateYMDHMSM *)v3->value.s.arr->data;


  result = brathl_Cycle2YMDHMSM(mission, cycle, pass, dateYMDHMSM);
  
  retval = brathli_MkError(result);
  return retval;
}


//------------------------------------------------------
static IDL_VPTR brathli_YMDHMSM2Cycle(int argc, IDL_VPTR *argv)
{
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;
  IDL_VPTR v1;
  IDL_VPTR v2;
  IDL_VPTR v3;
  brathl_mission mission = TOPEX;
  uint32_t cycle = 0;
  uint32_t pass = 0;
  brathl_DateYMDHMSM *dateYMDHMSM = NULL;


  mission = (brathl_mission)(IDL_ULongScalar(argv[0]));
  v1 = argv[1];
  v2 = argv[2];
  v3 = argv[3];
  
  if (v1->value.s.sdef != brathli_DateYMDHMSM_sdef)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_EXPECTED_STRUCT_DATEYMDHMSM);
    return retval;
  }


  dateYMDHMSM = (brathl_DateYMDHMSM *)v1->value.s.arr->data;


  result = brathl_YMDHMSM2Cycle(mission,
      	      	      	      	dateYMDHMSM,
      	      	      	      	(uint32_t*)&(v2->value.ul),
      	      	      	      	(uint32_t*)&(v3->value.ul));
  
  retval = brathli_MkError(result);
  return retval;
}

//------------------------------------------------------
static IDL_VPTR brathli_ReadData(int argc, IDL_VPTR *argv)
{
    
  int32_t result = BRATHL_SUCCESS;
  IDL_VPTR retval;

  IDL_VPTR vFiles;
  IDL_VPTR vRecord;
  IDL_VPTR vSelection;
  IDL_VPTR vDataExpr;
  IDL_VPTR vUnits;
  IDL_VPTR vData;
  //IDL_VPTR vSizes;
  //IDL_VPTR vActualSize;
  IDL_VPTR vIgnoreOutOfRange;
  IDL_VPTR vStatistics;
  IDL_VPTR vDefaultValue;
    
  
  int32_t	num_files = 0;
  char	      	**fileNames = NULL;
  //const char	**fileNames = NULL;
  const char	*recordName = NULL;
  const char	*selection = NULL;
  int32_t	num_expr = 0;
  char  	**dataExpressions = NULL;
  //char  	**dataExpressions = NULL;
  int32_t	num_units = 0;
  char		**units = NULL;
  int32_t	num_data = 0;
  double	**data = NULL;
  int32_t	num_sizes = 0;
  int32_t	*sizes = NULL;
  int32_t	actualSize = -1;
  //int		ignoreOutOfRange;
  int		statistics = 0;
  double	defaultValue = 0.0;

  int32_t numberOfErrors = 0;
  int32_t i = 0;

  //--------------------------------
  //-------------------- Input files 
  //--------------------------------
  vFiles = 	argv[0];
  IDL_ENSURE_STRING(vFiles);

  if ((vFiles->flags & IDL_V_ARR) == 0)
  {
    // single file 
    num_files = 1;
    fileNames = (char **)malloc(sizeof(char *));
    //fileNames = (const char**)fileNamesTmp;
    //fileNames[0] = IDL_STRING_STR(&vFiles->value.str);
    fileNames[0] = vFiles->value.str.s;
  }
  else
  {
    // list of files 
    num_files = vFiles->value.arr->n_elts;
    fileNames = (char **)malloc(num_files * sizeof(char *));
    //fileNames = (const char**)fileNamesTmp;
    for (i = 0; i < num_files; i++)
    {
      //fileNames[i] = IDL_STRING_STR(&((IDL_STRING *)vFiles->value.arr->data)[i]);
      fileNames[i] = ((IDL_STRING *)vFiles->value.arr->data)[i].s;
    }
  }
      
  //--------------------------------
  //-------------------- Record name
  //-------------------------------
  vRecord = argv[1];
  IDL_ENSURE_STRING(vRecord);
  recordName = IDL_STRING_STR(&vRecord->value.str);
      
  //--------------------------------
  //-------------------- Selection name
  //-------------------------------
  vSelection = argv[2];
  IDL_ENSURE_STRING(vSelection);
  selection = IDL_STRING_STR(&vSelection->value.str);
  
  //--------------------------------
  //-------------------- Data expression
  //--------------------------------
  vDataExpr = argv[3];
  IDL_ENSURE_STRING(vDataExpr);

  if ((vDataExpr->flags & IDL_V_ARR) == 0)
  {
    // single data expression
    num_expr = 1;
    dataExpressions = (char **)malloc(sizeof(char *));
    //dataExpressions[0] = IDL_STRING_STR(&vDataExpr->value.str);
    dataExpressions[0] = vDataExpr->value.str.s;
  }
  else
  {
    // list of data expressions 
    num_expr = vDataExpr->value.arr->n_elts;
    dataExpressions = (char **)malloc(num_expr * sizeof(char *));
    for (i = 0; i < num_expr; i++)
    {
      //dataExpressions[i] = IDL_STRING_STR(&((IDL_STRING *)vDataExpr->value.arr->data)[i]);
      dataExpressions[i] = ((IDL_STRING *)vDataExpr->value.arr->data)[i].s;
    }
  }
      
  
  //--------------------------------
  //-------------------- Units
  //--------------------------------
  vUnits = argv[4];
  IDL_ENSURE_STRING(vUnits);

  if ((vUnits->flags & IDL_V_ARR) == 0)
  {
    // single unit

    if (vUnits->value.str.slen != 0)
    {
      num_units = 1;
      units = (char **)malloc(sizeof(char *));
      units[0] = vUnits->value.str.s;
    }
  }
  else
  {
    // list of units 
    num_units = vUnits->value.arr->n_elts;
    units = (char **)malloc(num_units * sizeof(char *));
    for (i = 0; i < num_units; i++)
    {
      units[i] = ((IDL_STRING *)vUnits->value.arr->data)[i].s;
    }
  }
  
  //--------------------------------
  //-------------------- Results
  //--------------------------------
  vData = argv[5];
  /*
  IDL_ENSURE_ARRAY(vData);

  if ((vData->flags & IDL_V_ARR) == 0)
  {
    // single data
    num_data = 1;
    data = (double **)malloc(sizeof(double *));
    data[0] = NULL;
  }
  else
  {
    // list of data 
    num_data = vData->value.arr->n_elts;
    data = (double **)malloc(num_data * sizeof(double *));
    for (i = 0; i < num_data; i++)
    {
      data[i] = NULL;
    }
  }
 */
     
  num_data = num_expr;
  data = (double **)malloc(num_data * sizeof(double *));
  for (i = 0; i < num_data; i++)
  {
    data[i] = NULL;
  }
  //--------------------------------
  //-------------------- Sizes
  //--------------------------------
  /*vSizes = argv[6];
  IDL_ENSURE_ARRAY(vSizes);

  if ((vSizes->flags & IDL_V_ARR) == 0)
  {
    // single size
    num_sizes = 1;
    sizes = (int32_t *)malloc(sizeof(int32_t));
    sizes[0] = (int32_t)(vSizes->value.l);
  }
  else
  {
    // list of sizes
    num_sizes = vSizes->value.arr->n_elts;
    sizes = (int32_t *)malloc(num_sizes * sizeof(int32_t));
    for (i = 0; i < num_sizes; i++)
    {
      sizes[i] = (int32_t)(((IDL_LONG *)vSizes->value.arr->data)[i]);
    }
  }
  */
   

  num_sizes = num_data;
  sizes = (int32_t *)malloc(num_sizes * sizeof(int32_t));
  for (i = 0; i < num_sizes; i++)
  {
    sizes[i] = -1;
  }
      
  //--------------------------------
  //-------------------- Actual size
  //--------------------------------
  /*vActualSize = argv[7];
  actualSize = (int32_t*)&(vActualSize->value.l);
  */
  //--------------------------------
  //-------------------- Ignore out of range
  //--------------------------------
  vIgnoreOutOfRange = argv[6];
  
  //--------------------------------
  //-------------------- Statistics
  //--------------------------------
  vStatistics = argv[7];
  
  //--------------------------------
  //-------------------- Default value
  //--------------------------------
  vDefaultValue = argv[8];
  
//-----------  
  
  
  if (num_units > 0)
  {
    if (num_units != num_expr)
    {
      retval = brathli_MkError(BRATHL_IDL_ERR_INVALID_NUMBER_OF_UNITS);
      numberOfErrors++;    
    }
  }  
  
  if (num_units != num_expr)
  {
    retval = brathli_MkError(BRATHL_IDL_ERR_INVALID_NUMBER_OF_DATA);
    numberOfErrors++;    
  }

  if (numberOfErrors > 0)
  {
    return retval;
  }    

  /*
  printf("ReaddataBeforeCall\n");
  printf("num_files %d\n", num_files);
  for(int32_t index = 0 ; index < num_files ; index++)
  {
    printf("fileNames[%d] '%s'\n", index, fileNames[index]);
  }
  printf("num_files %d\n", num_files);
  printf("recordName '%s'\n", recordName);
  printf("selection '%s'\n", selection);
  printf("num_expr %d\n", num_expr);
  for(int32_t index = 0 ; index < num_expr ; index++)
  {
    printf("dataExpressions[%d] '%s'\n", index, dataExpressions[index]);
  }
  printf("num_units %d\n", num_units);
  for(int32_t index = 0 ; index < num_units ; index++)
  {
    printf("units[%d] '%s'\n", index, units[index]);
  }
  printf("num_data %d\n", num_data);
  for(int32_t index = 0 ; index < num_data ; index++)
  {
    printf("data[%d] %d\n", index, data[index]);
  }
  printf("num_sizes %d\n", num_sizes);
  for(int32_t index = 0 ; index < num_sizes ; index++)
  {
    printf("sizes[%d] %d\n", index, sizes[index]);
  }
  
  printf("Actual size %d\n", actualSize);
  */  

  result = brathl_ReadData(num_files,
      	      	      	   fileNames,
      	      	      	   IDL_STRING_STR(&vRecord->value.str),
      	      	      	   IDL_STRING_STR(&vSelection->value.str),
      	      	      	   num_expr,
      	      	      	   dataExpressions,
      	      	      	   units,
      	      	      	   data,
      	      	      	   sizes,
			   &actualSize,
      	      	      	   (int)(vIgnoreOutOfRange->value.i),
      	      	      	   (int)(vStatistics->value.i),
      	      	      	   (double)(vDefaultValue->value.d));

  /*  			   
  printf("ReaddataAfterCall %d\n", result);
  printf("num_files %d\n", num_files);
  for(int32_t index = 0 ; index < num_files ; index++)
  {
    printf("fileNames[%d] '%s'\n", index, fileNames[index]);
  }
  printf("num_files %d\n", num_files);
  printf("recordName '%s'\n", recordName);
  printf("selection '%s'\n", selection);
  printf("num_expr %d\n", num_expr);
  for(int32_t index = 0 ; index < num_expr ; index++)
  {
    printf("dataExpressions[%d] '%s'\n", index, dataExpressions[index]);
  }
  printf("num_units %d\n", num_units);
  for(int32_t index = 0 ; index < num_units ; index++)
  {
    printf("units[%d] '%s'\n", index, units[index]);
  }
  printf("num_data %d\n", num_data);
  for(int32_t index = 0 ; index < num_data ; index++)
  {
    printf("data[%d] %d\n", index, data[index]);
  }
  printf("num_sizes %d\n", num_sizes);
  for(int32_t index = 0 ; index < num_sizes ; index++)
  {
    printf("sizes[%d] %d\n", index, sizes[index]);
  }
  
  printf("Actual size %d\n", actualSize);
  printf("Data pointer at 0x%x\n", data);
  */  
  
  if (result != BRATHL_SUCCESS)
  {
    retval = brathli_MkErrorStr(BRATHL_ERROR, "Brathl error - See error output");
    return retval;
  }

  IDL_MEMINT dimData[IDL_MAX_ARRAY_DIM]; 
  IDL_MEMINT dimSizes[IDL_MAX_ARRAY_DIM]; 
  
  IDL_VPTR IDL_TmpData;
  double* tmpData_d = NULL;
  
  if (actualSize > 0)
  { 
  
    //Warning : in the output array the first dimension is the number of record (ie actualsize)
    // and the second dimension is the number of varaible (ie num_data)
    dimData[0] = actualSize;
    dimData[1] = num_data;
  
    tmpData_d = (double*)IDL_MakeTempArray(IDL_TYP_DOUBLE, 2, dimData, IDL_ARR_INI_ZERO, &IDL_TmpData);
    
    for (int32_t i = 0 ; i < num_data ; i++)
    {
      double *vector = data[i];
      for (int32_t j = 0 ; j < actualSize ; j++)
      {
      	*tmpData_d = vector[j];
	tmpData_d++;
      }
      // free memory allocated by brathl_readData.
      delete []vector;
    }
    
    //IDL_ImportNamedArray("vData", 2, dim, IDL_TYP_DOUBLE, (UCHAR*)data, free_cb, (void*)0);
    //IDL_VPTR IDL_TmpData = IDL_ImportArray(2, dimData, IDL_TYP_DOUBLE, (UCHAR*)data, free_cb, (void*)0);
    /*
    IDL documentation : If the source is a temporary variable, IDL_VarCopy() does not make a
    duplicate of the dynamic part for the destination. Instead, the dynamic part of
    the source is given to the destination, and the source variable itself is returned
    to the pool of free temporary variables. This is the equivalent of freeing the
    temporary variable. Therefore, the variable must not be used any further and
    the caller should not explicitly free the variable. This optimization significantly
    improves resource utilization and performance because this special case occurs
    frequently.
    */
    IDL_VarCopy(IDL_TmpData, vData);
    
  }

  if (fileNames != NULL)
  {
    free(fileNames);
    fileNames = NULL;
  }
  if (dataExpressions != NULL)
  {
    free(dataExpressions);
    dataExpressions = NULL;
  }

  if (sizes != NULL)
  {
    free(sizes);
    sizes = NULL;
  }
  if (units != NULL)
  {
    free(units);
    units = NULL;
  }
  
  retval = brathli_MkError(result);
  return retval;
}

//------------------------------------------------------

static void brathli_RegisterIDLStructTypes()
{

    static IDL_STRUCT_TAG_DEF brathli_DateYMDHMSM_tags[] = {
        { "YEAR"      , 0 , (void *)IDL_TYP_ULONG    , 0 },
        { "MONTH"     , 0 , (void *)IDL_TYP_ULONG    , 0 },
        { "DAY"       , 0 , (void *)IDL_TYP_ULONG    , 0 },
        { "HOUR"      , 0 , (void *)IDL_TYP_ULONG    , 0 },
        { "MINUTE"    , 0 , (void *)IDL_TYP_ULONG    , 0 },
        { "SECOND"    , 0 , (void *)IDL_TYP_ULONG    , 0 },
        { "MUSECOND"  , 0 , (void *)IDL_TYP_ULONG    , 0 },
    };

	
    static IDL_STRUCT_TAG_DEF brathli_DateDSM_tags[] = {
        { "REFDATE"   	, 0 , (void *)IDL_TYP_INT     , 0 },
        { "DAYS"      	, 0 , (void *)IDL_TYP_LONG    , 0 },
        { "SECONDS"     , 0 , (void *)IDL_TYP_LONG    , 0 },
        { "MUSECONDS"   , 0 , (void *)IDL_TYP_LONG    , 0 },
    };
	
    static IDL_STRUCT_TAG_DEF brathli_DateSecond_tags[] = {
        { "REFDATE"   	, 0 , (void *)IDL_TYP_INT     , 0 },
        { "NBSECONDS"   , 0 , (void *)IDL_TYP_DOUBLE  , 0 },
    };

    static IDL_STRUCT_TAG_DEF brathli_DateJulian_tags[] = {
        { "REFDATE"   	, 0 , (void *)IDL_TYP_INT     , 0 },
        { "JULIAN"      , 0 , (void *)IDL_TYP_DOUBLE  , 0 },
    };
	
    static IDL_STRUCT_TAG_DEF brathli_Error_tags[] = {
        {"ERRNO"   , 0 , (void *)IDL_TYP_INT    , 0 },
        {"MESSAGE" , 0 , (void *)IDL_TYP_STRING , 0 },
        {0         , 0 , 0                      , 0 }
    };

    brathli_DateYMDHMSM_sdef    = IDL_MakeStruct("BRATHL_DATEYMDHMSM"       , brathli_DateYMDHMSM_tags);
    brathli_DateDSM_sdef      	= IDL_MakeStruct("BRATHL_DATEDSM"     	    , brathli_DateDSM_tags);
    brathli_DateSecond_sdef     = IDL_MakeStruct("BRATHL_DATESECOND"        , brathli_DateSecond_tags);
    brathli_DateJulian_sdef     = IDL_MakeStruct("BRATHL_DATEJULIAN"        , brathli_DateJulian_tags);

    brathli_Error_sdef          = IDL_MakeStruct("BRATHL_ERROR"             , brathli_Error_tags);
    
}

static int brathli_RegisterIDLFunctionsAndProcedures()
{
    static IDL_SYSFUN_DEF2 idl_func_addr[] = {
//        { {(IDL_SYSRTN_GENERIC) brathli_test            } , "BRATHL_TEST"           , 2 , 2             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Seconds2DSM     } , "BRATHL_SECONDS2DSM"    , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_DSM2Seconds     } , "BRATHL_DSM2SECONDS"    , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Julian2DSM      } , "BRATHL_JULIAN2DSM"     , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_DSM2Julian      } , "BRATHL_DSM2JULIAN"     , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_YMDHMSM2DSM     } , "BRATHL_YMDHMSM2DSM"    , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_DSM2YMDHMSM     } , "BRATHL_DSM2YMDHMSM"    , 2 , 2             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Seconds2Julian  } , "BRATHL_SECONDS2JULIAN" , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Julian2Seconds  } , "BRATHL_JULIAN2SECONDS" , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_YMDHMSM2Seconds } , "BRATHL_YMDHMSM2SECONDS", 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Seconds2YMDHMSM } , "BRATHL_SECONDS2YMDHMSM", 2 , 2             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_YMDHMSM2Julian  } , "BRATHL_YMDHMSM2JULIAN" , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Julian2YMDHMSM  } , "BRATHL_JULIAN2YMDHMSM" , 2 , 2             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_NowYMDHMSM      } , "BRATHL_NOWYMDHMSM"     , 1 , 1             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_DayOfYear       } , "BRATHL_DAYOFYEAR"      , 2 , 2             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_DiffYMDHMSM     } , "BRATHL_DIFFYMDHMSM"    , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_DiffDSM       	} , "BRATHL_DIFFDSM"        , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_DiffJulian      } , "BRATHL_DIFFJULIAN"     , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_SetRefUser1     } , "BRATHL_SETREFUSER1"    , 1 , 1             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_SetRefUser2     } , "BRATHL_SETREFUSER2"    , 1 , 1             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Cycle2YMDHMSM   } , "BRATHL_CYCLE2YMDHMSM"  , 4 , 4             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_YMDHMSM2Cycle   } , "BRATHL_YMDHMSM2CYCLE"  , 4 , 4             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_ReadData        } , "BRATHL_READDATA"       , 9 , 9             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_IsError         } , "BRATHL_IS_ERROR"       , 1 , 1             , 0 , 0 }
     };


    /// new-style procedure declarations 

	
    //static IDL_SYSFUN_DEF2 idl_proc_addr[] = {
    //    {{(IDL_SYSRTN_GENERIC)brathli_proc}, "BRATHL_PROC", 0, 0, 0, 0}
    //};
     
    //return
    //    IDL_SysRtnAdd(idl_func_addr, TRUE, sizeof(idl_func_addr) / sizeof(IDL_SYSFUN_DEF2)) &&
    //    IDL_SysRtnAdd(idl_proc_addr, FALSE, sizeof(idl_proc_addr) / sizeof(IDL_SYSFUN_DEF2));
    return IDL_SysRtnAdd(idl_func_addr, TRUE, sizeof(idl_func_addr) / sizeof(IDL_SYSFUN_DEF2));

/*
#ifdef HAVE_IDL_SYSFUN_DEF2

    // new-style function declarations 

#ifdef HAVE_IDL_SYSRTN_UNION

    // sysrtn is a union in sysfun_def2 


    static IDL_SYSFUN_DEF2 idl_func_addr[] = {
        { {(IDL_SYSRTN_GENERIC) brathli_test            } , "BRATHL_TEST"           , 2 , 2             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Seconds2DSM     } , "BRATHL_SECONDS2DSM"    , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_DSM2Seconds     } , "BRATHL_DSM2SECONDS"    , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Julian2DSM      } , "BRATHL_JULIAN2DSM"     , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_DSM2Julian      } , "BRATHL_DSM2JULIAN"     , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_YMDHMSM2DSM     } , "BRATHL_YMDHMSM2DSM"    , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_DSM2YMDHMSM     } , "BRATHL_DSM2YMDHMSM"    , 2 , 2             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Seconds2Julian  } , "BRATHL_SECONDS2JULIAN" , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Julian2Seconds  } , "BRATHL_JULIAN2SECONDS" , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_YMDHMSM2Seconds } , "BRATHL_YMDHMSM2SECONDS", 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Seconds2YMDHMSM } , "BRATHL_SECONDS2YMDHMSM", 2 , 2             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_YMDHMSM2Julian  } , "BRATHL_YMDHMSM2JULIAN" , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Julian2YMDHMSM  } , "BRATHL_JULIAN2YMDHMSM" , 2 , 2             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_NowYMDHMSM      } , "BRATHL_NOWYMDHMSM"     , 1 , 1             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_Quantieme       } , "BRATHL_QUANTIEME"      , 2 , 2             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_DiffYMDHMSM     } , "BRATHL_DIFFYMDHMSM"    , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_DiffDSM       	} , "BRATHL_DIFFDSM"        , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_DiffJulian      } , "BRATHL_DIFFJULIAN"     , 3 , 3             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_SetRefUser1     } , "BRATHL_SETREFUSER1"    , 1 , 1             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_SetRefUser2     } , "BRATHL_SETREFUSER2"    , 1 , 1             , 0 , 0 },
        { {(IDL_SYSRTN_GENERIC) brathli_IsError         } , "BRATHL_IS_ERROR"       , 1 , 1             , 0 , 0 }
     };


    /// new-style procedure declarations 

	
    //static IDL_SYSFUN_DEF2 idl_proc_addr[] = {
    //    {{(IDL_SYSRTN_GENERIC)brathli_proc}, "BRATHL_PROC", 0, 0, 0, 0}
    };

#else

    // sysrtn is a of type IDL_FUN_RET 


    static IDL_SYSFUN_DEF2 idl_func_addr[] = {
        { (IDL_SYSRTN_GENERIC) brathli_test            , "BRATHL_TEST"           , 2 , 2             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Seconds2DSM     , "BRATHL_SECONDS2DSM"    , 3 , 3             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_DSM2Seconds     , "BRATHL_DSM2SECONDS"    , 3 , 3             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Julian2DSM      , "BRATHL_JULIAN2DSM"     , 3 , 3             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_DSM2Julian      , "BRATHL_DSM2JULIAN"     , 3 , 3             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_YMDHMSM2DSM     , "BRATHL_YMDHMSM2DSM"    , 3 , 3             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_DSM2YMDHMSM     , "BRATHL_DSM2YMDHMSM"    , 2 , 2             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Seconds2Julian  , "BRATHL_SECONDS2JULIAN" , 3 , 3             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Julian2Seconds  , "BRATHL_JULIAN2SECONDS" , 3 , 3             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_YMDHMSM2Seconds , "BRATHL_YMDHMSM2SECONDS", 3 , 3             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Seconds2YMDHMSM , "BRATHL_SECONDS2YMDHMSM", 2 , 2             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_YMDHMSM2Julian  , "BRATHL_YMDHMSM2JULIAN" , 3 , 3             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Julian2YMDHMSM  , "BRATHL_JULIAN2YMDHMSM" , 2 , 2             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_NowYMDHMSM      , "BRATHL_NOWYMDHMSM"     , 1 , 1             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Quantieme       , "BRATHL_QUANTIEME"      , 2 , 2             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_DiffYMDHMSM     , "BRATHL_DIFFYMDHMSM"    , 3 , 3             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_DiffDSM         , "BRATHL_DIFFDSM"        , 3 , 3             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_DiffJulian      , "BRATHL_DIFFJULIAN"     , 3 , 3             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_SetRefUser1     , "BRATHL_SETREFUSER1"    , 1 , 1             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_SetRefUser2     , "BRATHL_SETREFUSER2"    , 1 , 1             , 0 , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_IsError         , "BRATHL_IS_ERROR"       , 1 , 1             , 0 , 0 }
    };

    /// new-style procedure declarations 

    //static IDL_SYSFUN_DEF2 idl_proc_addr[] = {
    //    {(IDL_FUN_RET)brathli_proc, "BRATHL_PROC", 0, 0, 0, 0}
    };

#endif

    //return
    //    IDL_SysRtnAdd(idl_func_addr, TRUE, sizeof(idl_func_addr) / sizeof(IDL_SYSFUN_DEF2)) &&
    //    IDL_SysRtnAdd(idl_proc_addr, FALSE, sizeof(idl_proc_addr) / sizeof(IDL_SYSFUN_DEF2));
    return IDL_SysRtnAdd(idl_func_addr, TRUE, sizeof(idl_func_addr) / sizeof(IDL_SYSFUN_DEF2));

#else

    // old-style function declarations 

    static IDL_SYSFUN_DEF idl_func_addr[] = {
        { (IDL_SYSRTN_GENERIC) brathli_test            , "BRATHL_TEST"            , 2 , 2             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Seconds2DSM     , "BRATHL_SECONDS2DSM"     , 3 , 3             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_DSM2Seconds     , "BRATHL_DSM2SECONDS"     , 3 , 3             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Julian2DSM      , "BRATHL_JULIAN2DSM"      , 3 , 3             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_DSM2Julian      , "BRATHL_DSM2JULIAN"      , 3 , 3             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_YMDHMSM2DSM     , "BRATHL_YMDHMSM2DSM"     , 3 , 3             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_DSM2YMDHMSM     , "BRATHL_DSM2YMDHMSM"     , 2 , 2             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Seconds2Julian  , "BRATHL_SECONDS2JULIAN"  , 3 , 3             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Julian2Seconds  , "BRATHL_JULIAN2SECONDS"  , 3 , 3             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_YMDHMSM2Seconds , "BRATHL_YMDHMSM2SECONDS" , 3 , 3             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Seconds2YMDHMSM , "BRATHL_SECONDS2YMDHMSM" , 2 , 2             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_YMDHMSM2Julian  , "BRATHL_YMDHMSM2JULIAN"  , 3 , 3             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Julian2YMDHMSM  , "BRATHL_JULIAN2YMDHMSM"  , 2 , 2             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_NowYMDHMSM      , "BRATHL_NOWYMDHMSM"      , 1 , 1             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_Quantieme       , "BRATHL_QUANTIEME"       , 2 , 2             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_DiffYMDHMSM     , "BRATHL_DIFFYMDHMSM"     , 3 , 3             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_DiffDSM         , "BRATHL_DIFFDSM"         , 3 , 3             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_DiffJulian      , "BRATHL_DIFFJULIAN"      , 3 , 3             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_SetRefUser1     , "BRATHL_SETREFUSER1"     , 1 , 1             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_SetRefUser2     , "BRATHL_SETREFUSER2"     , 1 , 1             , 0 },
        { (IDL_SYSRTN_GENERIC) brathli_IsError         , "BRATHL_IS_ERROR"        , 1 , 1             , 0 }
    };


    // old-style procedure declarations 

    ///static IDL_SYSFUN_DEF idl_proc_addr[] = {
    ///    {(IDL_FUN_RET)brathli_proc, "BRATHL_PROC", 0, 0, 0}
    };
    //return
    //    IDL_AddSystemRoutine(idl_func_addr, TRUE, sizeof(idl_func_addr) / sizeof(IDL_SYSFUN_DEF)) &&
    //    IDL_AddSystemRoutine(idl_proc_addr, FALSE, sizeof(idl_proc_addr) / sizeof(IDL_SYSFUN_DEF));

    return IDL_AddSystemRoutine(idl_func_addr, TRUE, sizeof(idl_func_addr) / sizeof(IDL_SYSFUN_DEF));

#endif
*/
    
}

int IDL_Load(void)
{
    brathli_SetDataDir();

    /* register types, functions, and procedures */

    brathli_RegisterIDLStructTypes();
    return brathli_RegisterIDLFunctionsAndProcedures();
}

#if defined( __cplusplus )
}
#endif  /* __cplusplus */
