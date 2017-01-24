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

#include <cstring>

#include "brathl_matlab.h"

#define MAX_FUNCNAME_LENGTH     50

#include "libbrathl/Tools.h"

using namespace brathl;




static void brathlm_SetDataDir(void)
{
    mxArray *mxpath;
    
    mxArray *arg = mxCreateString("brathl_matlab");
    if (mexCallMATLAB(1, &mxpath, 1, &arg, "which") != 0)
    {
        mexErrMsgTxt("Could not retrieve module path");
    }
    
    int path_length = mxGetN(mxpath) * mxGetM(mxpath) + 1;
    char *path = (char *)mxCalloc(path_length + 1, 1);
    if (mxGetString(mxpath, path, path_length + 1) != 0)
    {
        mexErrMsgTxt("Error copying std::string");
    }

    std::string datadir = CTools::MakeCorrectPath( CTools::AbsolutePath( std::string( path ) + ( PATH_SEPARATOR ".." PATH_SEPARATOR "data") ) );
    if (!datadir.empty())
    {
        CTools::SetInternalDataDir( datadir );
    }
    mxFree(path);
}

#if defined( __cplusplus )
extern "C" {
#endif  /* __cplusplus */

static int brathlm_initialised = 0;

typedef enum {  
                YMDHMSM_STRUCT,    
                DSM_STRUCT,      
                SECONDS_STRUCT, 
                JULIAN_STRUCT     
      } brathlm_dateStructName;

const char refDateN[] = "refDate";

const char daysN[] = "days";
const char secondsN[] = "seconds";
const char muSecondsN[] = "muSeconds";

const char yearN[] = "year";
const char monthN[] = "month";
const char dayN[] = "day";
const char hourN[] = "hour";
const char minuteN[] = "minute";
const char secondN[] = "second";
const char muSecondN[] = "muSecond";

const char nbSecondsN[] = "nbSeconds";

const char julianN[] = "julian";

const char *field_namesDSM[] = {refDateN, daysN, secondsN, muSecondsN};
const char *field_namesYMDHMSM[] = {yearN, monthN, dayN, hourN, minuteN, secondN, muSecondN};
const char *field_namesJulian[] = {refDateN, julianN};
const char *field_namesSeconds[] = {refDateN, nbSecondsN};

#define NFIELDS_DSM (sizeof(field_namesDSM)/sizeof(*field_namesDSM))
#define NFIELDS_YMDHMSM (sizeof(field_namesYMDHMSM)/sizeof(*field_namesYMDHMSM))
#define NFIELDS_JULIAN (sizeof(field_namesJulian)/sizeof(*field_namesJulian))
#define NFIELDS_SECONDS (sizeof(field_namesSeconds)/sizeof(*field_namesSeconds))

static void brathlm_SetRefUser1(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
static void brathlm_SetRefUser2(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

static void brathlm_CreateStruct(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

static void brathlm_Seconds2DSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
static void brathlm_DSM2Seconds(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

static void brathlm_Julian2DSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
static void brathlm_DSM2Julian(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

static void brathlm_YMDHMSM2DSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
static void brathlm_DSM2YMDHMSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

static void brathlm_Seconds2Julian(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
static void brathlm_Julian2Seconds(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

static void brathlm_Seconds2YMDHMSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
static void brathlm_YMDHMSM2Seconds(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

static void brathlm_Julian2YMDHMSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
static void brathlm_YMDHMSM2Julian(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

static void brathlm_NowYMDHMSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

static void brathlm_DayOfYear(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

static void brathlm_DiffYMDHMSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
static void brathlm_DiffDSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
static void brathlm_DiffJulian(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

static void brathlm_ReadData(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

bool brathlm_IsStruct(const mxArray *array_ptr, const char *field_names[], const int32_t nFields);

void brathlm_FillStructSecondsIn(const mxArray *array_ptr, brathl_DateSecond *dateSeconds);
void brathlm_FillStructSecondsOut(const brathl_DateSecond *dateSeconds, mxArray **array_ptr);
void brathlm_FillStructDSMIn(const mxArray *array_ptr, brathl_DateDSM *dateDSM);
void brathlm_FillStructDSMOut(const brathl_DateDSM *dateDSM, mxArray **array_ptr);
void brathlm_FillStructJulianIn(const mxArray *array_ptr, brathl_DateJulian *dateJulian);
void brathlm_FillStructJulianOut(const brathl_DateJulian *dateJulian, mxArray **array_ptr);
void brathlm_FillStructYMDHMSMIn(const mxArray *array_ptr, brathl_DateYMDHMSM *dateYMDHMSM);
void brathlm_FillStructYMDHMSMOut(const brathl_DateYMDHMSM *dateYMDHMSM, mxArray **array_ptr);

void brathlm_CreateStructDSM(mxArray **array_ptr);
void brathlm_CreateStructSeconds(mxArray **array_ptr);
void brathlm_CreateStructJulian(mxArray **array_ptr);
void brathlm_CreateStructYMDHMSM(mxArray **array_ptr);

//-----------------------------
void brathlm_Error()
{
  if (brathl_errno != BRATHL_SUCCESS)
  {
    mexPrintf("ERROR : %s\n", brathl_Errno2String(brathl_errno));
    mexErrMsgTxt("BRATHL Error");
  }
}



//------------------------------------------------------

static void brathlm_SetRefUser1(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  char *dateRefUser;
  int32_t status;
  
  if (nrhs != 1) 
  {
    mexErrMsgTxt("brathlm_SetRefUser1  - No input argument required.");
  } 
  if(nlhs != 0)
  {
    mexErrMsgTxt("brathlm_SetRefUser1 - No output argument required.");
  }

  // Input must be a std::string. 
  if (mxIsChar(prhs[0]) != 1)
  {
    mexErrMsgTxt("brathlm_SetRefUser1 - Input must be a std::string.");
  }

  // Input must be a row std::vector.
  if (mxGetM(prhs[0]) != 1)
  {
    mexErrMsgTxt("brathlm_SetRefUser1 - Input must be a row std::vector.");
  }
    
  // Get the length of the input std::string.
  int32_t lenSrc = (mxGetM(prhs[0]) * mxGetN(prhs[0])) + 1;

  // Allocate memory for input and output strings. 
  dateRefUser = static_cast<char*>(mxCalloc(lenSrc, sizeof(char)));
 
  // Copy the std::string data from prhs[0] into a C std::string 
  // dateRefUser.
  status = mxGetString(prhs[0], dateRefUser, lenSrc);
  if (status != 0) 
  {
    mexWarnMsgTxt("Not enough space. String is truncated.");
  }
    
  int32_t lenToCopy = ((lenSrc > BRATHL_REF_DATE_USER_LEN) ? BRATHL_REF_DATE_USER_LEN - 1 : lenSrc);
  strncpy (brathl_refDateUser1, dateRefUser, lenToCopy);
}


//-----------------------------


static void brathlm_SetRefUser2(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  char *dateRefUser;
  int32_t status;
  
  if (nrhs != 1) 
  {
    mexErrMsgTxt("brathlm_SetRefUser1  - No input argument required.");
  } 
  if(nlhs != 0)
  {
    mexErrMsgTxt("brathlm_SetRefUser1 - No output argument required.");
  }

  
  // Input must be a std::string. 
  if (mxIsChar(prhs[0]) != 1)
  {
    mexErrMsgTxt("brathlm_SetRefUser1 - Input must be a std::string.");
  }

  // Input must be a row std::vector.
  if (mxGetM(prhs[0]) != 1)
  {
    mexErrMsgTxt("brathlm_SetRefUser1 - Input must be a row std::vector.");
  }
    
  // Get the length of the input std::string.
  int32_t lenSrc = (mxGetM(prhs[0]) * mxGetN(prhs[0])) + 1;

  // Allocate memory for input and output strings. 
  dateRefUser = static_cast<char*>(mxCalloc(lenSrc, sizeof(char)));
 
  // Copy the std::string data from prhs[0] into a C std::string 
  // dateRefUser.
  status = mxGetString(prhs[0], dateRefUser, lenSrc);
  if (status != 0) 
  {
    mexWarnMsgTxt("Not enough space. String is truncated.");
  }
    
  int32_t lenToCopy = ((lenSrc > BRATHL_REF_DATE_USER_LEN) ? BRATHL_REF_DATE_USER_LEN - 1 : lenSrc);
  strncpy (brathl_refDateUser2, dateRefUser, lenToCopy);
}


//-----------------------------


bool brathlm_IsStruct(const mxArray *array_ptr, const char *field_names[], int32_t nFields)
{
  int32_t i = 0;
  int32_t fieldNum = -1;
    
  if (!mxIsStruct(array_ptr))
  {
    //mexErrMsgTxt("brathlm_IsStructDSM Input 0 must be a structure.");
    return false;
  }
  
  for (i = 0 ; i < nFields ; i++)
  {
    
    fieldNum = mxGetFieldNumber(array_ptr, field_names[i]);  
    if (fieldNum < 0)
    {
      return false;
    }
  }

  return true;
}  

//-----------------------------

void brathlm_FillStructSecondsIn(const mxArray *array_ptr, brathl_DateSecond *dateSeconds)
{
  if (!mxIsStruct(array_ptr))
  {
    mexErrMsgTxt("brathlm_FillStructSecondsIn - Input must be a structure.");
    return;
  }
  mxArray *pa = NULL;
  
  pa = mxGetField(array_ptr, 0, refDateN);
  dateSeconds->refDate = static_cast<brathl_refDate>((int) mxGetScalar(pa));
  
  pa = mxGetField(array_ptr, 0, nbSecondsN);

  dateSeconds->nbSeconds = mxGetScalar(pa);
}  

//-----------------------------

void brathlm_FillStructSecondsOut(const brathl_DateSecond *dateSeconds, mxArray **array_ptr)
{
  int dims[2] = {1, 1};

  if (dateSeconds == NULL)
  {
    mexWarnMsgTxt("brathlm_FillStructSecondsOut - dateSeconds is NULL)");
    return;
  }
  
  brathlm_CreateStructSeconds(array_ptr);
  //*array_ptr = mxCreateStructArray(2, dims, (sizeof(field_namesSeconds)/sizeof(*field_namesSeconds)), field_namesSeconds);

  mxSetField(*array_ptr, 0, refDateN, mxCreateDoubleScalar(dateSeconds->refDate));
  mxSetField(*array_ptr, 0, nbSecondsN, mxCreateDoubleScalar(dateSeconds->nbSeconds));
}  

//-----------------------------

void brathlm_FillStructDSMIn(const mxArray *array_ptr, brathl_DateDSM *dateDSM)
{
  if (!mxIsStruct(array_ptr))
  {
    mexErrMsgTxt("brathlm_FillStructDSMIn - Input must be a structure.");
    return;
  }
  mxArray *pa = NULL;
  
  pa = mxGetField(array_ptr, 0, refDateN);
  dateDSM->refDate = static_cast<brathl_refDate>((int) mxGetScalar(pa));
  
  pa = mxGetField(array_ptr, 0, daysN);
  dateDSM->days = static_cast<int32_t>(mxGetScalar(pa));
    
  pa = mxGetField(array_ptr, 0, secondsN);
  dateDSM->seconds = static_cast<int32_t>(mxGetScalar(pa));

  pa = mxGetField(array_ptr, 0, muSecondsN);
  dateDSM->muSeconds = static_cast<int32_t>(mxGetScalar(pa));
}  

//-----------------------------

void brathlm_FillStructDSMOut(const brathl_DateDSM *dateDSM, mxArray **array_ptr)
{
  int dims[2] = {1, 1};
  
  if (dateDSM == NULL)
  {
    mexWarnMsgTxt("brathlm_FillStructDSMOut - dateDSM is NULL)");
    return;
  }
  
  brathlm_CreateStructDSM(array_ptr);
    
  mxSetField(*array_ptr, 0, refDateN, mxCreateDoubleScalar(dateDSM->refDate));
  mxSetField(*array_ptr, 0, daysN, mxCreateDoubleScalar(dateDSM->days));
  mxSetField(*array_ptr, 0, secondsN, mxCreateDoubleScalar(dateDSM->seconds));
  mxSetField(*array_ptr, 0, muSecondsN, mxCreateDoubleScalar(dateDSM->muSeconds));
}  

//-----------------------------

void brathlm_FillStructYMDHMSMIn(const mxArray *array_ptr, brathl_DateYMDHMSM *dateYMDHMSM)
{
  if (!mxIsStruct(array_ptr))
  {
    mexErrMsgTxt("brathlm_FillStructYMDHMSMIn - Input must be a structure.");
    return;
  }
  mxArray *pa = NULL;
    
  pa = mxGetField(array_ptr, 0, yearN);
  dateYMDHMSM->year = static_cast<int32_t>(mxGetScalar(pa));
    
  pa = mxGetField(array_ptr, 0, monthN);
  dateYMDHMSM->month = static_cast<int32_t>(mxGetScalar(pa));

  pa = mxGetField(array_ptr, 0, dayN);
  dateYMDHMSM->day = static_cast<int32_t>(mxGetScalar(pa));
        
  pa = mxGetField(array_ptr, 0, hourN);
  dateYMDHMSM->hour = static_cast<int32_t>(mxGetScalar(pa));
        
  pa = mxGetField(array_ptr, 0, minuteN);
  dateYMDHMSM->minute = static_cast<int32_t>(mxGetScalar(pa));
        
  pa = mxGetField(array_ptr, 0, secondN);
  dateYMDHMSM->second = static_cast<int32_t>(mxGetScalar(pa));
        
  pa = mxGetField(array_ptr, 0, muSecondN);
  dateYMDHMSM->muSecond = static_cast<int32_t>(mxGetScalar(pa));
}  

//-----------------------------

void brathlm_FillStructYMDHMSMOut(const brathl_DateYMDHMSM *dateYMDHMSM, mxArray **array_ptr)
{
  int dims[2] = {1, 1};
   
  if (dateYMDHMSM == NULL)
  {
    mexWarnMsgTxt("brathlm_FillStructDSMOut - dateYMDHMSM is NULL)");
    return;
  }
  
  brathlm_CreateStructYMDHMSM(array_ptr);
    
  mxSetField(*array_ptr, 0, yearN, mxCreateDoubleScalar(dateYMDHMSM->year));
  mxSetField(*array_ptr, 0, monthN, mxCreateDoubleScalar(dateYMDHMSM->month));
  mxSetField(*array_ptr, 0, dayN, mxCreateDoubleScalar(dateYMDHMSM->day));
  mxSetField(*array_ptr, 0, hourN, mxCreateDoubleScalar(dateYMDHMSM->hour));
  mxSetField(*array_ptr, 0, minuteN, mxCreateDoubleScalar(dateYMDHMSM->minute));
  mxSetField(*array_ptr, 0, secondN, mxCreateDoubleScalar(dateYMDHMSM->second));
  mxSetField(*array_ptr, 0, muSecondN, mxCreateDoubleScalar(dateYMDHMSM->muSecond));
}  


//-----------------------------

void brathlm_FillStructJulianIn(const mxArray *array_ptr, brathl_DateJulian *dateJulian)
{
  if (!mxIsStruct(array_ptr))
  {
    mexErrMsgTxt("brathlm_FillStructJulianIn - Input must be a structure.");
    return;
  }
  mxArray *pa = NULL;
  
  pa = mxGetField(array_ptr, 0, refDateN);
  dateJulian->refDate = static_cast<brathl_refDate>((int) mxGetScalar(pa));
  
  pa = mxGetField(array_ptr, 0, julianN);
  dateJulian->julian = mxGetScalar(pa);
}  

//-----------------------------

void brathlm_FillStructJulianOut(const brathl_DateJulian *dateJulian, mxArray **array_ptr)
{
  int dims[2] = {1, 1};
   
  if (dateJulian == NULL)
  {
    mexWarnMsgTxt("brathlm_FillStructJulianOut - dateSeconds is NULL)");
    return;
  }
  
  brathlm_CreateStructJulian(array_ptr);  
  
  mxSetField(*array_ptr, 0, refDateN, mxCreateDoubleScalar(dateJulian->refDate));
  mxSetField(*array_ptr, 0, julianN, mxCreateDoubleScalar(dateJulian->julian));
}  

//-----------------------------


void brathlm_CreateStructDSM(mxArray **array_ptr)
{
  int dims[2] = {1, 1};

  *array_ptr = mxCreateStructArray(2, dims, NFIELDS_DSM, field_namesDSM);
}  

//-----------------------------

void brathlm_CreateStructSeconds(mxArray **array_ptr)
{
  int dims[2] = {1, 1};

  *array_ptr = mxCreateStructArray(2, dims, NFIELDS_SECONDS, field_namesSeconds);
}  

//-----------------------------

void brathlm_CreateStructJulian(mxArray **array_ptr)
{
  int dims[2] = {1, 1};

  *array_ptr = mxCreateStructArray(2, dims, NFIELDS_JULIAN, field_namesJulian);
}  


//-----------------------------

void brathlm_CreateStructYMDHMSM(mxArray **array_ptr)
{
  int dims[2] = {1, 1};
 
  *array_ptr = mxCreateStructArray(2, dims, NFIELDS_YMDHMSM, field_namesYMDHMSM);
}  

//-----------------------------

static void brathlm_CreateStruct(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_CreateStruct Function needs 1 output arguments.");
  }
  if (nrhs != 1)
  {
      mexErrMsgTxt("brathlm_CreateStruct Function needs 1 input arguments.");
  }

  int32_t whichStruct = static_cast<uint32_t>(mxGetScalar(prhs[0]));

  switch (whichStruct)
  {
    case YMDHMSM_STRUCT :   
      brathlm_CreateStructYMDHMSM(&(plhs[0]));
      break; 
    case DSM_STRUCT :  
      brathlm_CreateStructDSM(&(plhs[0]));
      break; 
    case SECONDS_STRUCT :   
      brathlm_CreateStructSeconds(&(plhs[0]));
      break; 
    case JULIAN_STRUCT :   
      brathlm_CreateStructJulian(&(plhs[0]));
      break; 
    default :  
      mexPrintf("brathlm_CreateStructDSM - invalid parameter value."
	      	   "\n\t YMDHMSM structure : %d"
      	      	   "\n\t DSM structure : %d"
      	      	   "\n\t SECONDS structure : %d"
      	      	   "\n\t JULIAN structure : %d"
      	      	   "\n",
		   YMDHMSM_STRUCT, DSM_STRUCT, SECONDS_STRUCT, JULIAN_STRUCT);	   
      mexErrMsgTxt("Create structure ERROR");	   
      break; 
  } 
}


//-----------------------------

static void brathlm_Seconds2DSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 2)
  {
      mexErrMsgTxt("brathlm_Seconds2DSM Function needs 2 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_Seconds2DSM Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesSeconds, NFIELDS_SECONDS) == false)
  {
    mexErrMsgTxt("brathlm_Seconds2DSM Input 0 must be a SECONDS structure.");
  }
  
  brathl_DateSecond dateSeconds;
  brathl_DateDSM dateDSM;
  
  brathl_refDate refDate = static_cast<brathl_refDate>((int) mxGetScalar(prhs[1]));
  
  brathlm_FillStructSecondsIn(prhs[0], &dateSeconds);
  
  brathl_Seconds2DSM(&dateSeconds, refDate, &dateDSM);

  brathlm_Error();
  
  brathlm_FillStructDSMOut(&dateDSM, &(plhs[0]));
}

//-----------------------------

static void brathlm_DSM2Seconds(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
 
  if (nrhs != 2)
  {
      mexErrMsgTxt("brathlm_DSM2Seconds Function needs 2 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_DSM2Seconds Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesDSM, NFIELDS_DSM) == false)
  {
    mexErrMsgTxt("brathlm_DSM2Seconds Input 0 must be a DSM structure.");
  }
  
  brathl_DateSecond dateSeconds;
  brathl_DateDSM dateDSM;
  
  brathl_refDate refDate = static_cast<brathl_refDate>((int) mxGetScalar(prhs[1]));
  
  brathlm_FillStructDSMIn(prhs[0], &dateDSM);
  
  brathl_DSM2Seconds(&dateDSM, refDate, &dateSeconds);

  brathlm_Error();
  
  brathlm_FillStructSecondsOut(&dateSeconds, &(plhs[0]));
    
}

//-----------------------------

static void brathlm_Julian2DSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 2)
  {
      mexErrMsgTxt("brathlm_Julian2DSM Function needs 2 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_Julian2DSM Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesJulian, NFIELDS_JULIAN) == false)
  {
    mexErrMsgTxt("brathlm_Julian2DSM Input 0 must be a JULIAN structure.");
  }
  
  brathl_DateJulian dateJulian;
  brathl_DateDSM dateDSM;
  
  brathl_refDate refDate = static_cast<brathl_refDate>((int) mxGetScalar(prhs[1]));
  
  brathlm_FillStructJulianIn(prhs[0], &dateJulian);
  
  brathl_Julian2DSM(&dateJulian, refDate, &dateDSM);

  brathlm_Error();
  
  brathlm_FillStructDSMOut(&dateDSM, &(plhs[0]));
    
}

//-----------------------------

static void brathlm_DSM2Julian(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
 
  if (nrhs != 2)
  {
      mexErrMsgTxt("brathlm_DSM2Julian Function needs 2 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_DSM2Julian Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesDSM, NFIELDS_DSM) == false)
  {
    mexErrMsgTxt("brathlm_DSM2Julian Input 0 must be a DSM structure.");
  }
  
  brathl_DateJulian dateJulian;
  brathl_DateDSM dateDSM;
  
  brathl_refDate refDate = static_cast<brathl_refDate>((int) mxGetScalar(prhs[1]));
  
  brathlm_FillStructDSMIn(prhs[0], &dateDSM);
  
  brathl_DSM2Julian(&dateDSM, refDate, &dateJulian);

  brathlm_Error();
  
  brathlm_FillStructJulianOut(&dateJulian, &(plhs[0]));
    
}




//-----------------------------

static void brathlm_YMDHMSM2DSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 2)
  {
      mexErrMsgTxt("brathlm_YMDHMSM2DSM Function needs 2 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_YMDHMSM2DSM Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesYMDHMSM, NFIELDS_YMDHMSM) == false)
  {
    mexErrMsgTxt("brathlm_YMDHMSM2DSM Input 0 must be a YMDHMSM structure.");
  }
  
  brathl_DateYMDHMSM dateYMDHMSM;
  brathl_DateDSM dateDSM;
  
  brathl_refDate refDate = static_cast<brathl_refDate>((int) mxGetScalar(prhs[1]));
  
  brathlm_FillStructYMDHMSMIn(prhs[0], &dateYMDHMSM);
  
  brathl_YMDHMSM2DSM(&dateYMDHMSM, refDate, &dateDSM);

  brathlm_Error();
  
  brathlm_FillStructDSMOut(&dateDSM, &(plhs[0]));
    
}

//-----------------------------

static void brathlm_DSM2YMDHMSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
 
  if (nrhs != 1)
  {
      mexErrMsgTxt("brathl_DSM2YMDHMSM Function needs 1 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathl_DSM2YMDHMSM Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesDSM, NFIELDS_DSM) == false)
  {
    mexErrMsgTxt("brathlm_DSM2Julian Input 0 must be a DSM structure.");
  }
  
  brathl_DateYMDHMSM dateYMDHMSM;
  brathl_DateDSM dateDSM;
    
  brathlm_FillStructDSMIn(prhs[0], &dateDSM);
  
  brathl_DSM2YMDHMSM(&dateDSM, &dateYMDHMSM);

  brathlm_Error();
  
  brathlm_FillStructYMDHMSMOut(&dateYMDHMSM, &(plhs[0]));
    
}

//-----------------------------

static void brathlm_Seconds2Julian(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 2)
  {
      mexErrMsgTxt("brathlm_Seconds2Julian Function needs 2 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_Seconds2Julian Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesSeconds, NFIELDS_SECONDS) == false)
  {
    mexErrMsgTxt("brathlm_Seconds2Julian Input 0 must be a SECONDS structure.");
  }
  
  brathl_DateSecond dateSeconds;
  brathl_DateJulian dateJulian;
  
  brathl_refDate refDate = static_cast<brathl_refDate>((int) mxGetScalar(prhs[1]));
  
  brathlm_FillStructSecondsIn(prhs[0], &dateSeconds);
  
  brathl_Seconds2Julian(&dateSeconds, refDate, &dateJulian);

  brathlm_Error();
  
  brathlm_FillStructJulianOut(&dateJulian, &(plhs[0]));
}

//-----------------------------

static void brathlm_Julian2Seconds(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
 
  if (nrhs != 2)
  {
      mexErrMsgTxt("brathlm_Julian2Seconds Function needs 2 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_Julian2Seconds Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesJulian, NFIELDS_JULIAN) == false)
  {
    mexErrMsgTxt("brathlm_Julian2Seconds Input 0 must be a JULIAN structure.");
  }
  
  brathl_DateSecond dateSeconds;
  brathl_DateJulian dateJulian;
  
  brathl_refDate refDate = static_cast<brathl_refDate>((int) mxGetScalar(prhs[1]));
  
  brathlm_FillStructJulianIn(prhs[0], &dateJulian);
  
  brathl_Julian2Seconds(&dateJulian, refDate, &dateSeconds);

  brathlm_Error();
  
  brathlm_FillStructSecondsOut(&dateSeconds, &(plhs[0]));
    
}

//-----------------------------

static void brathlm_Seconds2YMDHMSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
 
  if (nrhs != 1)
  {
      mexErrMsgTxt("brathlm_Seconds2YMDHMSM Function needs 1 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_Seconds2YMDHMSM Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesSeconds, NFIELDS_SECONDS) == false)
  {
    mexErrMsgTxt("brathlm_Seconds2YMDHMSM Input 0 must be a SECONDS structure.");
  }
  
  brathl_DateYMDHMSM dateYMDHMSM;
  brathl_DateSecond dateSeconds;
    
  brathlm_FillStructSecondsIn(prhs[0], &dateSeconds);
  
  brathl_Seconds2YMDHMSM(&dateSeconds, &dateYMDHMSM);

  brathlm_Error();
  
  brathlm_FillStructYMDHMSMOut(&dateYMDHMSM, &(plhs[0]));
    
}

//-----------------------------

static void brathlm_YMDHMSM2Seconds(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 2)
  {
      mexErrMsgTxt("brathlm_YMDHMSM2Seconds Function needs 2 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_YMDHMSM2Seconds Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesYMDHMSM, NFIELDS_YMDHMSM) == false)
  {
    mexErrMsgTxt("brathlm_YMDHMSM2Seconds Input 0 must be a YMDHMSM structure.");
  }
  
  brathl_DateYMDHMSM dateYMDHMSM;
  brathl_DateSecond dateSeconds;
  
  brathl_refDate refDate = static_cast<brathl_refDate>((int) mxGetScalar(prhs[1]));
  
  brathlm_FillStructYMDHMSMIn(prhs[0], &dateYMDHMSM);
  
  brathl_YMDHMSM2Seconds(&dateYMDHMSM, refDate, &dateSeconds);

  brathlm_Error();
  
  brathlm_FillStructSecondsOut(&dateSeconds, &(plhs[0]));
    
}
//-----------------------------

static void brathlm_Julian2YMDHMSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
 
  if (nrhs != 1)
  {
      mexErrMsgTxt("brathlm_Julian2YMDHMSM Function needs 1 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_Julian2YMDHMSM Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesJulian, NFIELDS_JULIAN) == false)
  {
    mexErrMsgTxt("brathlm_Julian2YMDHMSM Input 0 must be a JULIAN structure.");
  }
  
  brathl_DateYMDHMSM dateYMDHMSM;
  brathl_DateJulian dateJulian;
    
  brathlm_FillStructJulianIn(prhs[0], &dateJulian);
  
  brathl_Julian2YMDHMSM(&dateJulian, &dateYMDHMSM);

  brathlm_Error();
  
  brathlm_FillStructYMDHMSMOut(&dateYMDHMSM, &(plhs[0]));
    
}

//-----------------------------

static void brathlm_YMDHMSM2Julian(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 2)
  {
      mexErrMsgTxt("brathlm_YMDHMSM2Julian Function needs 2 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_YMDHMSM2Julian Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesYMDHMSM, NFIELDS_YMDHMSM) == false)
  {
    mexErrMsgTxt("brathlm_YMDHMSM2Julian Input 0 must be a YMDHMSM structure.");
  }
  
  brathl_DateYMDHMSM dateYMDHMSM;
  brathl_DateJulian dateJulian;
  
  brathl_refDate refDate = static_cast<brathl_refDate>((int) mxGetScalar(prhs[1]));
  
  brathlm_FillStructYMDHMSMIn(prhs[0], &dateYMDHMSM);
  
  brathl_YMDHMSM2Julian(&dateYMDHMSM, refDate, &dateJulian);

  brathlm_Error();
  
  brathlm_FillStructJulianOut(&dateJulian, &(plhs[0]));
    
}

//-----------------------------

static void brathlm_NowYMDHMSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 0)
  {
      mexErrMsgTxt("brathlm_NowYMDHMSM Function needs 0 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_NowYMDHMSM Function needs 1 output arguments.");
  }
  
  brathl_DateYMDHMSM dateYMDHMSM;
  
 
  brathl_NowYMDHMSM(&dateYMDHMSM);

  brathlm_Error();
  
  brathlm_FillStructYMDHMSMOut(&dateYMDHMSM, &(plhs[0]));
    
}

//-----------------------------

static void brathlm_DayOfYear(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 1)
  {
      mexErrMsgTxt("brathlm_DayOfYear Function needs 1 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_DayOfYear Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesYMDHMSM, NFIELDS_YMDHMSM) == false)
  {
    mexErrMsgTxt("brathlm_DayOfYear Input 0 must be a YMDHMSM structure.");
  }
  
  uint32_t dayOfYear = 0;
  
  brathl_DateYMDHMSM dateYMDHMSM;
  
  brathlm_FillStructYMDHMSMIn(prhs[0], &dateYMDHMSM);
  
  brathl_DayOfYear(&dateYMDHMSM, &dayOfYear);

  brathlm_Error();

  plhs[0] = mxCreateDoubleScalar(dayOfYear);
    
}

//-----------------------------

static void brathlm_DiffYMDHMSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 2)
  {
      mexErrMsgTxt("brathlm_DiffYMDHMSM Function needs 2 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_DiffYMDHMSM Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesYMDHMSM, NFIELDS_YMDHMSM) == false)
  {
    mexErrMsgTxt("brathlm_DiffYMDHMSM Input 0 must be a YMDHMSM structure.");
  }
  
  if (brathlm_IsStruct(prhs[1], field_namesYMDHMSM, NFIELDS_YMDHMSM) == false)
  {
    mexErrMsgTxt("brathlm_DiffYMDHMSM Input 1 must be a YMDHMSM structure.");
  }
  
  double diff = 0.0;
  
  brathl_DateYMDHMSM d0;
  brathl_DateYMDHMSM d1;
  
  brathlm_FillStructYMDHMSMIn(prhs[0], &d0);
  brathlm_FillStructYMDHMSMIn(prhs[1], &d1);
  
  brathl_DiffYMDHMSM(&d0, &d1, &diff);

  brathlm_Error();

  plhs[0] = mxCreateDoubleScalar(diff);
    
}

//-----------------------------

static void brathlm_DiffDSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 2)
  {
      mexErrMsgTxt("brathlm_DiffDSM Function needs 2 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_DiffDSM Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesDSM, NFIELDS_DSM) == false)
  {
    mexErrMsgTxt("brathlm_DiffDSM Input 0 must be a DSM structure.");
  }
  
  if (brathlm_IsStruct(prhs[1], field_namesDSM, NFIELDS_DSM) == false)
  {
    mexErrMsgTxt("brathlm_DiffDSM Input 1 must be a DSM structure.");
  }
  
  double diff = 0.0;
  
  brathl_DateDSM d0;
  brathl_DateDSM d1;
  
  brathlm_FillStructDSMIn(prhs[0], &d0);
  brathlm_FillStructDSMIn(prhs[1], &d1);
  
  brathl_DiffDSM(&d0, &d1, &diff);

  brathlm_Error();

  plhs[0] = mxCreateDoubleScalar(diff);
    
}
//-----------------------------

static void brathlm_DiffJulian(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 2)
  {
      mexErrMsgTxt("brathlm_DiffJulian Function needs 2 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_DiffJulian Function needs 1 output arguments.");
  }
  
  if (brathlm_IsStruct(prhs[0], field_namesJulian, NFIELDS_JULIAN) == false)
  {
    mexErrMsgTxt("brathlm_DiffJulian Input 0 must be a JULIAN structure.");
  }
  
  if (brathlm_IsStruct(prhs[1], field_namesJulian, NFIELDS_JULIAN) == false)
  {
    mexErrMsgTxt("brathlm_DiffJulian Input 1 must be a JULIAN structure.");
  }
  
  double diff = 0.0;
  
  brathl_DateJulian d0;
  brathl_DateJulian d1;
  
  brathlm_FillStructJulianIn(prhs[0], &d0);
  brathlm_FillStructJulianIn(prhs[1], &d1);
  
  brathl_DiffJulian(&d0, &d1, &diff);

  brathlm_Error();

  plhs[0] = mxCreateDoubleScalar(diff);
    
}


//-----------------------------

static void brathlm_Cycle2YMDHMSM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 3)
  {
      mexErrMsgTxt("brathlm_Cycle2YMDHMSM Function needs 3 input arguments.");
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_Cycle2YMDHMSM Function needs 1 output arguments.");
  }
  
  brathl_DateYMDHMSM dateYMDHMSM;
  
  brathl_mission mission = static_cast<brathl_mission>((int) mxGetScalar(prhs[0]));
  int32_t cycle = static_cast<brathl_refDate>((int) mxGetScalar(prhs[1]));
  int32_t pass = static_cast<brathl_refDate>((int) mxGetScalar(prhs[2]));

 
  brathl_Cycle2YMDHMSM(mission, cycle, pass, &dateYMDHMSM);

  brathlm_Error();
  
  brathlm_FillStructYMDHMSMOut(&dateYMDHMSM, &(plhs[0]));
    
}

//-----------------------------

static void brathlm_YMDHMSM2Cycle(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  if (nrhs != 2)
  {
      mexErrMsgTxt("brathlm_YMDHMSM2Cycle Function needs 2 input arguments.");
  }

  if (nlhs != 2)
  {
      mexErrMsgTxt("brathlm_YMDHMSM2Cycle Function needs 2 output arguments.");
  }
  
  brathl_DateYMDHMSM dateYMDHMSM;

  brathlm_FillStructYMDHMSMIn(prhs[1], &dateYMDHMSM);
  
  brathl_mission mission = static_cast<brathl_mission>((int) mxGetScalar(prhs[0]));
  int32_t cycle = 0;
  int32_t pass = 0;

 
  brathl_YMDHMSM2Cycle(mission, &dateYMDHMSM, &cycle, &pass);
  
  brathlm_Error();

  plhs[0] = mxCreateDoubleScalar(cycle);
  plhs[1] = mxCreateDoubleScalar(pass);
  
     
}
//------------------------------------------------------
static void brathlm_ReadData(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{    
  int32_t result = BRATHL_SUCCESS;

  int32_t	num_files = 0;
  char	      	**fileNames = NULL;
  char	*recordName = NULL;
  char	*selection = NULL;
  int32_t	num_expr = 0;
  char  	**dataExpressions = NULL;
  int32_t	num_units = 0;
  char		**units = NULL;
  int32_t	num_data = 0;
  double	**data = NULL;
  int32_t	num_sizes = 0;
  int32_t	*sizes = NULL;
  size_t	actualSize = -1;
  int		ignoreOutOfRange = 0;
  int		statistics = 0;
  double	defaultValue = 0.0;

  int32_t        ifield, jstruct;
  int32_t        *classIDflags;
  int32_t        NStructElems, nfields, ndim;

  int32_t index = 0;
  
  const mxArray *cell_element_ptr = NULL;
  int32_t buflen = 0;
  mxClassID  category;

  if (nrhs != 8)
  {
      mexErrMsgTxt("brathlm_ReadData Function needs 7 input arguments :\n"
	      	  "1) Input files - std::string or an array of std::string cells\n"
	      	  "2) Record name - std::string\n"
	      	  "3) Selection expression - std::string - (empty std::string for no selection)\n"
	      	  "4) Data expression - std::string or an array of std::string cells\n"
	      	  "5) Units \n - std::string or an array of std::string cells - (empty std::string for default unit)\n"
	      	  "6) Ignore out of range flag - boolean\n"
	      	  "7) Statistics flag - boolean\n"
	      	  "8) Default value - double precesion, floating-points number\n"
	  );
  }

  if (nlhs != 1)
  {
      mexErrMsgTxt("brathlm_ReadData Function needs 1 output argument.");
  }

  //--------------------------------
  //-------------------- Input files 
  //--------------------------------
  const mxArray* mxFiles = prhs[0];
 
  num_files = mxGetNumberOfElements(mxFiles); 

  category = mxGetClassID(mxFiles);
  if ( (category != mxCELL_CLASS) && (category != mxCHAR_CLASS) )
  {
    mexPrintf("ERROR Parameter 1 (filenames) must be a std::string or an array of cell and found '%s' type.\n",
	      mxGetClassName(mxFiles));
    mexErrMsgTxt("BRATHL Error - see above");
  }
   
  if (category == mxCELL_CLASS) 
  {
    fileNames = static_cast<char**>(mxCalloc(num_files, sizeof(char *)));

    for(index = 0 ; index < num_files ; index++)
    {
      cell_element_ptr = mxGetCell(mxFiles, index);
      if ( (cell_element_ptr == NULL)  || 
	   (mxGetNumberOfElements(cell_element_ptr) == 0) )
      {
          mexPrintf("ERROR Parameter 1 (filenames) at index %d is an empty cell.\n", index);
          mexErrMsgTxt("BRATHL Error - see above");
      }

      category = mxGetClassID(cell_element_ptr);
      if (category != mxCHAR_CLASS)
      {
          mexPrintf("ERROR Parameter 1 (filenames) at index %d must be a std::string and found '%s' type.\n",
                    mxGetClassName(cell_element_ptr));
          mexErrMsgTxt("BRATHL Error - see above");
      }

      //buflen = mxGetNumberOfElements(cell_element_ptr) + 1;
      buflen = (mxGetM(cell_element_ptr) * mxGetN(cell_element_ptr)) + 1;
      // Input must be a row std::vector.
      if (mxGetM(cell_element_ptr) != 1)
      {
          mexPrintf("ERROR Parameter 1 (filenames) at index %d must be a row std::vector.\n", index);
          mexErrMsgTxt("BRATHL Error - see above");
      }

      fileNames[index] = static_cast<char*>(mxCalloc(buflen, sizeof(char)));

      if (mxGetString(cell_element_ptr, fileNames[index], buflen) != 0)
      {
          mexPrintf("ERROR parameter 1 (filenames) at index %d : could not convert std::string data\n", index);
          mexErrMsgTxt("BRATHL Error - see above");
      }

      //mexPrintf("fileNames%d %s\n", index, fileNames[index]);

    }
  }
  else // category is not mxCELL_CLASS
  {    
    // Input must be a std::string. 
    if (mxIsChar(mxFiles) == false)
    {
      mexErrMsgTxt("ERROR Parameter 1 (filenames) must be a std::string.");
    }
     // Input must be a row std::vector.
    if (mxGetM(mxFiles) != 1)
    {
      mexErrMsgTxt("ERROR if Parameter 1 (filenames) is a std::string, it must be a row std::vector.");
    }
	
	num_files = 1;
    fileNames = static_cast<char**>(mxCalloc(num_files, sizeof(char *)));

    // Get the length of the input std::string.
    buflen = (mxGetM(mxFiles) * mxGetN(mxFiles)) + 1;

    // Allocate memory for input and output strings. 
    fileNames[0] = static_cast<char*>(mxCalloc(buflen, sizeof(char)));

    if (mxGetString(mxFiles, fileNames[0], buflen) != 0)
    {
      mexPrintf("ERROR parameter 1 (fileNames) : could not convert std::string data\n");
      mexErrMsgTxt("BRATHL Error - see above");
    }
  }
      
  //--------------------------------
  //-------------------- Record name
  //-------------------------------
  const mxArray* mxRecordName = prhs[1];
      
  // Input must be a std::string. 
  if (mxIsChar(mxRecordName) == false)
  {
    mexErrMsgTxt("ERROR Parameter 2 (recordname) must be a std::string.");
  }
   // Input must be a row std::vector.
  if (mxGetM(mxRecordName) != 1)
  {
    mexErrMsgTxt("ERROR Parameter 2 (recordname) must be a row std::vector.");
  }
 
  // Get the length of the input std::string.
  buflen = (mxGetM(mxRecordName) * mxGetN(mxRecordName)) + 1;

  // Allocate memory for input and output strings. 
  recordName = static_cast<char*>(mxCalloc(buflen, sizeof(char)));
  //mexPrintf("RecordName '%s' len %d\n", mxRecordName, buflen);
  if (mxGetString(mxRecordName, recordName, buflen) != 0)
  {
    mexPrintf("ERROR parameter 2 (recordName) : could not convert std::string data\n");
    mexErrMsgTxt("BRATHL Error - see above");
  }

  //--------------------------------
  //-------------------- Selection name
  //-------------------------------
  const mxArray* mxSelection = prhs[2];
      
  // Input must be a std::string. 
  if (mxIsChar(mxSelection) == false)
  {
    mexErrMsgTxt("ERROR Parameter 3 (selection) must be a std::string.");
  }
   // Input must be a row std::vector.
  ////if (mxGetM(mxSelection) != 1)
  ////{
////    mexErrMsgTxt("ERROR Parameter 3 (selection) must be a row std::vector.");
////  }
  
 
  // Get the length of the input std::string.
  buflen = (mxGetM(mxSelection) * mxGetN(mxSelection)) + 1;

  // Allocate memory for input and output strings. 
  selection = static_cast<char*>(mxCalloc(buflen, sizeof(char)));
  if (mxGetString(mxSelection, selection, buflen) != 0)
  {
    mexPrintf("ERROR parameter 3 (selection) : could not convert std::string data\n");
    mexErrMsgTxt("BRATHL Error - see above");
  }
  
  //--------------------------------
  //-------------------- Data expression
  //--------------------------------
  const mxArray* mxDataExpressions = prhs[3];
 
  num_expr = mxGetNumberOfElements(mxDataExpressions);
  
  category = mxGetClassID(mxDataExpressions);
  if ( (category != mxCELL_CLASS) && (category != mxCHAR_CLASS) )
  {
    mexPrintf("ERROR Parameter 4 (data expressions) must be be a std::string or an array of cell and found '%s' type.\n",
              mxGetClassName(mxDataExpressions));
    mexErrMsgTxt("BRATHL Error - see above");
  }
   
  if (category == mxCELL_CLASS) 
  {
    dataExpressions = static_cast<char**>(mxCalloc(num_expr, sizeof(char *)));

    for(index = 0 ; index < num_expr ; index++)
    {
      cell_element_ptr = mxGetCell(mxDataExpressions, index);
      if ( (cell_element_ptr == NULL) || (mxGetNumberOfElements(cell_element_ptr) == 0) )
      {
          mexPrintf("ERROR Parameter 4 (data expressions) at index %d is an empty cell.\n", index);
          mexErrMsgTxt("BRATHL Error - see above");
      }

      category = mxGetClassID(cell_element_ptr);
      if (category != mxCHAR_CLASS)
      {
          mexPrintf("ERROR Parameter 4 (data expressions) at index %d must be a std::string and found '%s' type.\n",
                    mxGetClassName(cell_element_ptr));
          mexErrMsgTxt("BRATHL Error - see above");
      }

      buflen = (mxGetM(cell_element_ptr) * mxGetN(cell_element_ptr)) + 1;

      // Input must be a row std::vector.
      if (mxGetM(cell_element_ptr) != 1)
      {
          mexPrintf("ERROR Parameter 4 (data expressions) at index %d must be a row std::vector.\n", index);
          mexErrMsgTxt("BRATHL Error - see above");
      }

      dataExpressions[index] = static_cast<char*>(mxCalloc(buflen, sizeof(char)));

      if (mxGetString(cell_element_ptr, dataExpressions[index], buflen) != 0)
      {
          mexPrintf("ERROR parameter 4 (data expressions) at index %d : could not convert std::string data\n", index);
          mexErrMsgTxt("BRATHL Error - see above");
      }

      //mexPrintf("dataExpressions%d %s\n", index, dataExpressions[index]);

    }
  }
  else // category is not mxCELL_CLASS
  {    
    // Input must be a std::string. 
    if (mxIsChar(mxDataExpressions) == false)
    {
      mexErrMsgTxt("ERROR Parameter 4 (data expressions) must be a std::string.");
    }
     // Input must be a row std::vector.
    if (mxGetM(mxDataExpressions) != 1)
    {
      mexErrMsgTxt("ERROR if Parameter 4 (data expressions) is a std::string, it must be a row std::vector.");
    }

    num_expr = 1;
    dataExpressions = static_cast<char**>(mxCalloc(1, sizeof(char *)));

    // Get the length of the input std::string.
    buflen = (mxGetM(mxDataExpressions) * mxGetN(mxDataExpressions)) + 1;

    // Allocate memory for input and output strings. 
    dataExpressions[0] = static_cast<char*>(mxCalloc(buflen, sizeof(char)));

    if (mxGetString(mxDataExpressions, dataExpressions[0], buflen) != 0)
    {
      mexPrintf("ERROR parameter 4 (data expressions) : could not convert std::string data\n");
      mexErrMsgTxt("BRATHL Error - see above");
    }
  }

  //--------------------------------
  //-------------------- Units
  //--------------------------------
  const mxArray* mxUnits = prhs[4];
 
  num_units = mxGetNumberOfElements(mxUnits);
  
  category = mxGetClassID(mxUnits);
  if ( (category != mxCELL_CLASS) && (category != mxCHAR_CLASS) )
  {
    mexPrintf("ERROR Parameter 5 (units) must be be a std::string or an array of cell and found '%s' type.\n",
              mxGetClassName(mxFiles));
    mexErrMsgTxt("BRATHL Error - see above");
  }
   
  if (category == mxCELL_CLASS) 
  {
    units = static_cast<char**>(mxCalloc(num_units, sizeof(char *)));

    for(index = 0 ; index < num_units ; index++)
    {
      cell_element_ptr = mxGetCell(mxUnits, index);
      if ( (cell_element_ptr == NULL) || (mxGetNumberOfElements(cell_element_ptr) == 0) )
      {
        buflen = 1;
        units[index] = static_cast<char*>(mxCalloc(buflen, sizeof(char)));
      	memset(units[index], '\0', buflen);
        continue;
      }
      
      category = mxGetClassID(cell_element_ptr);
      if (category != mxCHAR_CLASS)
      {
          mexPrintf("ERROR Parameter 5 (units) at index %d must be a std::string and found '%s' type.\n",
                    mxGetClassName(cell_element_ptr));
          mexErrMsgTxt("BRATHL Error - see above");
      }

      //buflen = mxGetNumberOfElements(cell_element_ptr) + 1;
      buflen = (mxGetM(cell_element_ptr) * mxGetN(cell_element_ptr)) + 1;
      //mexPrintf("buflen %d :\n",
      //		buflen);
      // Input must be a row std::vector.
      if (mxGetM(cell_element_ptr) != 1)
      {
          mexPrintf("ERROR Parameter 5 (units) at index %d must be a row std::vector.\n", index);
          mexErrMsgTxt("BRATHL Error - see above");
      }

      units[index] = static_cast<char*>(mxCalloc(buflen, sizeof(char)));

      if (mxGetString(cell_element_ptr, units[index], buflen) != 0)
      {
          mexPrintf("ERROR parameter 5 (units) at index %d : could not convert std::string data\n", index);
          mexErrMsgTxt("BRATHL Error - see above");
      }

      //mexPrintf("units%d %s\n", index, units[index]);

    }
  }
  else // category is not mxCELL_CLASS
  {    
    // Input must be a std::string. 
    if (mxIsChar(mxUnits) == false)
    {
      mexErrMsgTxt("ERROR Parameter 5 (units) must be a std::string.");
    }
     // Input must be a row std::vector.
////    if (mxGetM(mxUnits) != 1)
////    {
////      mexErrMsgTxt("ERROR if Parameter 5 (units) is a std::string, it must be a row std::vector.");
////    }

    num_units = 1;
    units = static_cast<char**>(mxCalloc(1, sizeof(char *)));

    // Get the length of the input std::string.
    buflen = (mxGetM(mxUnits) * mxGetN(mxUnits)) + 1;

    // Allocate memory for input and output strings. 
    units[0] = static_cast<char*>(mxCalloc(buflen, sizeof(char)));

    if (mxGetString(mxUnits, units[0], buflen) != 0)
    {
      mexPrintf("ERROR parameter 5 (units) : could not convert std::string data\n");
      mexErrMsgTxt("BRATHL Error - see above");
    }
  }

  //--------------------------------
  //-------------------- Ignore out of range
  //--------------------------------
  const mxArray* mxIgoreOutOfRange = prhs[5];
//  if ( (mxIsLogical(mxIgoreOutOfRange) == false) || (mxGetNumberOfElements(mxIgoreOutOfRange) != 1) )
//  {
//    mexErrMsgTxt("ERROR Parameter 6 (ignore out of range) must be a logical (boolean) value : true or false .");
//  }
 
  if (mxGetNumberOfElements(mxIgoreOutOfRange) == 1) 
  {
	ignoreOutOfRange = static_cast<int32_t>(mxGetScalar(mxIgoreOutOfRange));
  }
  
  //--------------------------------
  //-------------------- Statistics
  //--------------------------------
  const mxArray* mxStatistics = prhs[6];
//  if ( (mxIsLogical(mxStatistics) == false) || (mxGetNumberOfElements(mxStatistics) != 1) )
//  {
//    mexErrMsgTxt("ERROR Parameter 7 (statistics) must be a logical (boolean) value : true or false .");
//  }
 
  if ( mxGetNumberOfElements(mxStatistics) == 1 ) 
  {
	statistics = static_cast<int32_t>(mxGetScalar(mxStatistics));
  }
 
       
  //--------------------------------
  //-------------------- Default value
  //--------------------------------
  const mxArray* mxDefaultValue = prhs[7];
  if (mxIsDouble(mxDefaultValue) == false)
  {
    mexErrMsgTxt("ERROR Parameter 8 (Default value) must be a double precsion, floating-points number.");
  }
  
  defaultValue = static_cast<double>(mxGetScalar(mxDefaultValue));

  //--------------------------------
  //-------------------- Results
  //--------------------------------
  num_data = num_expr;
  data = (double **)malloc(num_data * sizeof(double *));
  for (index = 0; index < num_data; index++)
  {
    data[index] = NULL;
  }
  
  //--------------------------------
  //-------------------- Sizes
  //--------------------------------

  num_sizes = num_data;
  sizes = (int32_t *)malloc(num_sizes * sizeof(int32_t));
  for (index = 0; index < num_sizes; index++)
  {
    sizes[index] = -1;
  }

  result = brathl_ReadData(num_files,
      	      	      	   fileNames,
      	      	      	   recordName,
      	      	      	   selection,
      	      	      	   num_expr,
      	      	      	   dataExpressions,
      	      	      	   units,
      	      	      	   data,
      	      	      	   sizes,
                           &actualSize,
      	      	      	   ignoreOutOfRange,
      	      	      	   statistics,
      	      	      	   defaultValue);

  brathlm_Error();

  if (actualSize > 0)
  { 
    //Warning : in the output array the first dimension is the number of record (ie actualsize)
    // and the second dimension is the number of variable (ie num_data)
    ndim = 2;
    int32_t dims[2];
    dims[0] = actualSize;
    dims[1] = num_data;

    plhs[0] = mxCreateNumericArray(ndim, dims, mxDOUBLE_CLASS, mxREAL);
    char* pdata = (char*)mxGetData(plhs[0]);

    mwSize sizebuf;
    sizebuf = mxGetElementSize(plhs[0]);

    for (int32_t r = 0 ; r < num_data ; r++)
    {
      double* vec = data[r];

      for (int32_t c = 0 ; c < actualSize ; c++)
      {    
        memcpy((void*)pdata, &(vec[c]), sizebuf);
        pdata += sizebuf;
      }
      // free memory allocated by brathl_readData.
      delete []vec;
    }
  }  
       
  //--------------------------------
  //-------------------- free allocated memory
  //--------------------------------
  //-------------
  for(index = 0 ; index < num_files ; index++)
  {
    mxFree(fileNames[index]);
    fileNames[index] = NULL;	
  }
  mxFree(fileNames);

  //-------------
  mxFree(recordName);
  
  //-------------
  mxFree(selection);
  
  //-------------
  for(index = 0 ; index < num_expr ; index++)
  {
    mxFree(dataExpressions[index]);
    dataExpressions[index] = NULL;	
  }
  mxFree(dataExpressions);
  
  if (sizes != NULL)
  {
    free(sizes);
    sizes = NULL;
  }
}

//-----------------------------

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  char funcname[MAX_FUNCNAME_LENGTH + 1];
  int32_t result;

  if (!brathlm_initialised)
  {
    brathlm_SetDataDir();
    brathlm_initialised = 1;
  }

  brathl_errno = 0;


  /* check parameters */
  if (!(nrhs >= 1 && mxIsChar(prhs[0]) && mxGetM(prhs[0]) == 1 && mxGetN(prhs[0]) <= MAX_FUNCNAME_LENGTH))
  {
      mexErrMsgTxt("Incorrect invocation of BRATHL_MATLAB gateway function.");
  }

  result = mxGetString(prhs[0], funcname, MAX_FUNCNAME_LENGTH + 1);
  if (result != 0)
  {
      mexErrMsgTxt("Error in BRATHL_MATLAB gateway function: Could not copy std::string.");
  }
  
  if (strcmp(funcname, "CREATESTRUCT") == 0)
  {
      brathlm_CreateStruct(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "SETREFUSER1") == 0)
  {
      brathlm_SetRefUser1(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "SETREFUSER2") == 0)
  {
      brathlm_SetRefUser2(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "SECONDS2DSM") == 0)
  {
      brathlm_Seconds2DSM(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "DSM2SECONDS") == 0)
  {
      brathlm_DSM2Seconds(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "JULIAN2DSM") == 0)
  {
      brathlm_Julian2DSM(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "DSM2JULIAN") == 0)
  {
      brathlm_DSM2Julian(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "YMDHMSM2DSM") == 0)
  {
      brathlm_YMDHMSM2DSM(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "DSM2YMDHMSM") == 0)
  {
      brathlm_DSM2YMDHMSM(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "SECONDS2JULIAN") == 0)
  {
      brathlm_Seconds2Julian(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "JULIAN2SECONDS") == 0)
  {
      brathlm_Julian2Seconds(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "SECONDS2YMDHMSM") == 0)
  {
      brathlm_Seconds2YMDHMSM(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "YMDHMSM2SECONDS") == 0)
  {
      brathlm_YMDHMSM2Seconds(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "JULIAN2YMDHMSM") == 0)
  {
      brathlm_Julian2YMDHMSM(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "YMDHMSM2JULIAN") == 0)
  {
      brathlm_YMDHMSM2Julian(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "NOWYMDHMSM") == 0)
  {
      brathlm_NowYMDHMSM(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "DAYOFYEAR") == 0)
  {
      brathlm_DayOfYear(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "DIFFYMDHMSM") == 0)
  {
      brathlm_DiffYMDHMSM(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "DIFFDSM") == 0)
  {
      brathlm_DiffDSM(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "DIFFJULIAN") == 0)
  {
      brathlm_DiffJulian(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "CYCLE2YMDHMSM") == 0)
  {
      brathlm_Cycle2YMDHMSM(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "YMDHMSM2CYCLE") == 0)
  {
      brathlm_YMDHMSM2Cycle(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else if (strcmp(funcname, "READDATA") == 0)
  {
      brathlm_ReadData(nlhs, plhs, nrhs - 1, &(prhs[1]));
  }
  else
  {
      mexErrMsgTxt("Error in BRATHL_MATLAB gateway function: Unknown function name.");
  }
}

#if defined( __cplusplus )
}
#endif  /* __cplusplus */
