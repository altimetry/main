#!/usr/bin/python -tt
#==============================================================================
#                              <brathl.py>
#==============================================================================
# Project   : S3-ALTB
# Company   : Deimos Engenharia
# Component : Brathl
# Language  : Python (v.3)
#------------------------------------------------------------------------------
# Scope : (see the following docstring)
# Usage : ------
#==============================================================================
# $Revision:
# $LastChangedBy:
# $LastChangedDate:
#==============================================================================

## @namespace brathl
# This module contains the brathl Python API.
# @version v0.1

import ctypes, os, sys 
import platform
#from ctypes.util import find_library


#-------------------------------------
#      Loading BRATHL Lib
#-------------------------------------

_OS = platform.system()
if _OS == 'Linux':
    brathl_lib = os.path.dirname(os.path.realpath(__file__)) + "/brathl_module"
elif _OS == 'Windows':
    brathl_lib = os.path.dirname(os.path.realpath(__file__)) + "/brathl_module"
elif _OS == "Darwin":  # Mac OS X
    brathl_lib = os.path.dirname(os.path.realpath(__file__)) + "/brathl_module"
else:
    sys.exit("Brathl Python API - ERROR: Unknown operating system!\n")

# Load every library that is used by your library:
#ctypes.CDLL("/fullpath/lib.so", mode = ctypes.RTLD_GLOBAL)
lib = ctypes.cdll.LoadLibrary(brathl_lib)   #find_library(brathl_lib)
#getattr(lib, "brathl_refDateUser1")



#------------------------------------
#      BRATHL Data structures
#------------------------------------

## Date reference enumeration - Used to give a start reference to a date
class brathl_refDate():
    REF19500101 = ctypes.c_int(0) # reference to 1950-01-01 00:00:00:00
    REF19580101 = ctypes.c_int(1) # reference to 1958-01-01 00:00:00:00
    REF19850101 = ctypes.c_int(2) # reference to the 1985-01-01 00:00:00:00
    REF19900101 = ctypes.c_int(3) # reference to the 1990-01-01 00:00:00:00
    REF20000101 = ctypes.c_int(4) # reference to the 2000-01-01 00:00:00:00
    REFUSER1    = ctypes.c_int(5) # reference to a user-defined date #brathl_SetRefDateUser1
    REFUSER2    = ctypes.c_int(6) # reference to a second user-defined date #brathl_SetRefDateUser2


## Creates a class for brathl_DateYMDHMSM (YYYY-MM-DD HH:MN:SS:MS date structure)
class brathl_DateYMDHMSM(ctypes.Structure):
    _fields_ = [("YEAR",     ctypes.c_int),   # numbers of years
                ("MONTH",    ctypes.c_int),   # numbers of months
                ("DAY",      ctypes.c_int),   # numbers of days
                ("HOUR",     ctypes.c_int),   # numbers of hours
                ("MINUTE",   ctypes.c_int),   # numbers of minutes
                ("SECOND",   ctypes.c_int),   # numbers of seconds
                ("MUSECOND", ctypes.c_int)]   # numbers of microseconds


## Creates a class for brathl_DateDSM (Day/seconds/microseconds date structure)
class brathl_DateDSM(ctypes.Structure):
    _fields_ = [("REFDATE",  ctypes.c_int), # date reference (see #brathl_refDate)
                ("DAY",      ctypes.c_int), # numbers of days
                ("SECOND",   ctypes.c_int), # numbers of seconds
                ("MUSECOND", ctypes.c_int)] # numbers of microseconds


## Creates a class for brathl_DateSecond (Decimal seconds date structure)
class brathl_DateSecond(ctypes.Structure):
    _fields_ = [("REFDATE", ctypes.c_int),    # date reference (see #brathl_refDate)
                ("SECOND",  ctypes.c_double)] # number of seconds


## Creates a class for brathl_DateJulian (Decimal julian date structure)
class brathl_DateJulian(ctypes.Structure):
    _fields_ = [("REFDATE", ctypes.c_int),    # date reference (see #brathl_refDate)
                ("JULIAN",  ctypes.c_double)] # decimal julian day


## Satellite (mission) enumeration
class brathl_mission():
    TOPEX   = ctypes.c_int(0) # Topex/Poseidon
    JASON2  = ctypes.c_int(1) # Jason-2
    JASON1  = ctypes.c_int(2) # Jason-1
    ERS2    = ctypes.c_int(3) # ERS2
    ENVISAT = ctypes.c_int(4) # Envisat
    ERS1_A  = ctypes.c_int(5) # ERS1-A
    ERS1_B  = ctypes.c_int(6) # ERS1-B
    GFO     = ctypes.c_int(7) # GFO



#------------------------------------
#        BRATHL Functions
#------------------------------------


#
#------ Error Brathl-Python Functions ------
#

## Retrieves a string with the error description.
# @param[in] errno    -> brathl error code (Type: int).
# @return    errorMsg -> string error description (Type: string) 
def brathl_Errno2String(errno):
    errorMsg       = ctypes.c_char_p()
    errorMsg.value = lib.brathl_Errno2String(ctypes.c_int(errno))
    return (errorMsg.value.decode('utf8')) 


## Prints the brathl error message.
# @param[in] errno    -> brathl error code (Type: int).
# @return    ----
def brathl_Error(errno):
    if errno != 0:
        print ("ERROR: Brathl errno: #%d - "%errno + brathl_Errno2String(errno))
    
    
#
#---- Reference date definition Python APIs ----
#

## Set first user defined reference date: REFUSER1 (See #brathl_refDate).
# @param[in] dateRef    -> date to set in format: YYYY MM DD HH:MN:SS.MS
#                          (Type: string).
# @return    ----
def brathl_SetRefDateUser1(dateRef):
    c_dateRef = ctypes.create_string_buffer (bytes(dateRef ,'utf8'))
    c_strLen  = ctypes.sizeof(c_dateRef)

    c_hook = (ctypes.c_char * c_strLen).in_dll (lib , "brathl_refDateUser1")
    c_hook.value = c_dateRef.value  #ctypes.memmove(c_hook, c_dateRef, c_strLen)
    return


## Set second user defined reference date: REFUSER2 (See #brathl_refDate).
# @param[in] dateRef    -> date to set in format: YYYY MM DD HH:MN:SS.MS
#                          (Type: string).
# @return    ----
def brathl_SetRefDateUser2(dateRef):
    c_dateRef = ctypes.create_string_buffer (bytes(dateRef ,'utf8'))
    c_strLen  = ctypes.sizeof(c_dateRef)

    c_hook = (ctypes.c_char * c_strLen).in_dll (lib , "brathl_refDateUser2")
    c_hook.value = c_dateRef.value  #ctypes.memmove(c_hook, c_dateRef, c_strLen)
    return


#
#--- Date Conversion Python APIs ----
#

## Retrieves the day of the year of a date.
# @param[in] date      -> date object (Type: brathl_DateYMDHMSM)
# @return    dayOfYear -> day of year (Type: long integer)
def brathl_DayOfYear(date):
    dayOfYear = ctypes.c_uint()
    errno = lib.brathl_DayOfYear(ctypes.pointer(date),
                                 ctypes.pointer(dayOfYear))
    brathl_Error(errno)
    return dayOfYear.value


## Computes the difference between two dates (date1 - date2)
# @param[in] dateDSM1  -> date object (Type: brathl_DateDSM)
# @param[in] dateDSM2  -> date object (Type: brathl_DateDSM)
# @return    diff      -> difference in seconds (Type: float)
def brathl_DiffDSM(dateDSM1, dateDSM2):
    diff = ctypes.c_double()
    errno = lib.brathl_DiffDSM(ctypes.pointer(dateDSM1),
                               ctypes.pointer(dateDSM2),
                               ctypes.pointer(diff))
    brathl_Error(errno)
    return diff.value


## Computes the difference between two dates (date1 - date2)
# @param[in] dateJulian1 -> date object (Type: brathl_DateJulian)
# @param[in] dateJulian2 -> date object (Type: brathl_DateJulian)
# @return    diff        -> difference in seconds (Type: float)
def brathl_DiffJulian(dateJulian1, dateJulian2):
    diff = ctypes.c_double()
    errno = lib.brathl_DiffJulian(ctypes.pointer(dateJulian1),
                                  ctypes.pointer(dateJulian2),
                                  ctypes.pointer(diff))
    brathl_Error(errno)
    return diff.value


## Computes the difference, in seconds, between two dates (date1 - date2)
# @param[in] date1 -> date object (Type: brathl_DateYMDHMSM)
# @param[in] date2 -> date object (Type: brathl_DateYMDHMSM)
# @return    diff  -> difference in seconds (Type: float)
def brathl_DiffYMDHMSM(date1, date2):
    diff = ctypes.c_double()
    errno = lib.brathl_DiffYMDHMSM(ctypes.pointer(date1),
                                   ctypes.pointer(date2), 
                                   ctypes.pointer(diff))
    brathl_Error(errno)
    return diff.value


## Converts a days-seconds-microseconds date into a decimal julian date, 
# according to refDate parameter.
# @param[in] dateDSM    -> date to convert (Type: brathl_DateDSM)
# @param[in] refDate    -> date reference conversion (see brathl_refDate)
# @return    dateJulian -> result of the conversion (Type: brathl_DateJulian)
def brathl_DSM2Julian(dateDSM, refDate):
    dateJulian = brathl_DateJulian()
    errno = lib.brathl_DSM2Julian(ctypes.pointer(dateDSM),
                                  refDate, 
                                  ctypes.pointer(dateJulian))
    brathl_Error(errno)
    return dateJulian


## Converts a days-seconds-microseconds date into seconds, according to refDate
# parameter.
# @param[in] dateDSM     -> date to convert (Type: brathl_DateDSM)
# @param[in] refDate     -> date reference conversion (see brathl_refDate)
# @return    dateSeconds -> result of the conversion (Type: brathl_DateSecond)
def brathl_DSM2Seconds(dateDSM, refDate):
    dateSeconds = brathl_DateSecond()
    errno = lib.brathl_DSM2Seconds(ctypes.pointer(dateDSM),
                                   refDate, 
                                   ctypes.pointer(dateSeconds))
    brathl_Error(errno)
    return dateSeconds


## Converts a days-seconds-microseconds date into into a year, month, day, hour,
# minute, second, microsecond date.
# @param[in] dateDSM     -> date to convert (Type: brathl_DateDSM)
# @return    dateYMDHMSM -> result of the conversion (Type: brathl_DateYMDHMSM)
def brathl_DSM2YMDHMSM(dateDSM):
    dateYMDHMSM = brathl_DateYMDHMSM()
    errno = lib.brathl_DSM2YMDHMSM(ctypes.pointer(dateDSM),
                                   ctypes.pointer(dateYMDHMSM))
    brathl_Error(errno)
    return dateYMDHMSM


## Converts a decimal julian date into a days-seconds-microseconds date, 
# according to refDate parameter
# @param[in] dateJulian -> date to convert (Type: brathl_DateJulian)
# @param[in] refDate    -> date reference conversion (see brathl_refDate)
# @return    dateDSM    -> result of the conversion (Type: brathl_DateDSM)
def brathl_Julian2DSM(dateJulian, refDate):
    dateDSM = brathl_DateDSM()
    errno = lib.brathl_Julian2DSM(ctypes.pointer(dateJulian),
                                  refDate,
                                  ctypes.pointer(dateDSM))
    brathl_Error(errno)
    return dateDSM


## Converts a decimal julian date into seconds, according to refDate parameter
# @param[in] dateJulian -> date to convert (Type: brathl_DateJulian)
# @param[in] refDate    -> date reference conversion (see brathl_refDate)
# @return    dateSeconds-> result of the conversion (Type: brathl_DateSecond)
def brathl_Julian2Seconds(dateJulian, refDate):
    dateSeconds = brathl_DateSecond()
    errno = lib.brathl_Julian2Seconds(ctypes.pointer(dateJulian),
                                      refDate,
                                      ctypes.pointer(dateSeconds))
    brathl_Error(errno)
    return dateSeconds


## Converts a decimal julian date into a year, month, day, hour, minute, second,
# microsecond date.
# @param[in] dateJulian  -> date to convert (Type: brathl_DateJulian)
# @return    dateYMDHMSM -> result of the conversion (Type: brathl_DateYMDHMSM)
def brathl_Julian2YMDHMSM(dateJulian):
    dateYMDHMSM = brathl_DateYMDHMSM()
    errno = lib.brathl_Julian2YMDHMSM(ctypes.pointer(dateJulian),
                                      ctypes.pointer(dateYMDHMSM))
    brathl_Error(errno)
    return dateYMDHMSM


## Converts seconds into a days-seconds-microseconds date, according to refDate 
# parameter.
# @param[in] dateSeconds -> date to convert (Type: brathl_DateSecond)
# @param[in] refDate     -> date reference conversion (see brathl_refDate)
# @return    dateDSM     -> result of the conversion (Type: brathl_DateDSM)
def brathl_Seconds2DSM(dateSeconds, refDate):
    dateDSM = brathl_DateDSM()
    errno = lib.brathl_Seconds2DSM(ctypes.pointer(dateSeconds),
                                   refDate,
                                   ctypes.pointer(dateDSM))
    brathl_Error(errno)
    return dateDSM


## Converts seconds into a decimal julian date, according to refDate parameter.
# @param[in] dateSeconds -> date to convert (Type: brathl_DateSecond)
# @param[in] refDate     -> date reference conversion (see brathl_refDate)
# @return    dateJulian  -> result of the conversion (Type: brathl_DateJulian)
def brathl_Seconds2Julian(dateSeconds, refDate):
    dateJulian = brathl_DateJulian()
    errno = lib.brathl_Seconds2Julian(ctypes.pointer(dateSeconds),
                                      refDate,
                                      ctypes.pointer(dateJulian))
    brathl_Error(errno)
    return dateJulian


## Converts seconds into a year, month, day, hour, minute, second, microsecond 
# date.
# @param[in] dateSeconds -> date to convert (Type: brathl_DateSecond)
# @return    dateYMDHMSM -> result of the conversion (Type: brathl_DateYMDHMSM)
def brathl_Seconds2YMDHMSM(dateSeconds):
    dateYMDHMSM = brathl_DateYMDHMSM()
    errno = lib.brathl_Seconds2YMDHMSM(ctypes.pointer(dateSeconds),
                                       ctypes.pointer(dateYMDHMSM))
    brathl_Error(errno)
    return dateYMDHMSM


## Gets the current year, month, day, hour, minute, second, microsecond date.
# @return    dateYMDHMSM -> current date/time (Type: brathl_DateYMDHMSM)
def brathl_NowYMDHMSM():
    dateYMDHMSM = brathl_DateYMDHMSM()
    errno = lib.brathl_NowYMDHMSM(ctypes.pointer(dateYMDHMSM))
    brathl_Error(errno)
    return dateYMDHMSM


## Converts a year, month, day, hour, minute, second, microsecond date into a 
# days-seconds-microseconds date.
# @param[in] dateYMDHMSM -> date to convert (Type: brathl_DateYMDHMSM)
# @param[in] refDate     -> date reference conversion (see brathl_refDate)
# @return    dateDSM     -> current date/time (Type: brathl_DateDSM)
def brathl_YMDHMSM2DSM(dateYMDHMSM, refDate):
    dateDSM = brathl_DateDSM()
    errno = lib.brathl_YMDHMSM2DSM(ctypes.pointer(dateYMDHMSM),
                                   refDate,
                                   ctypes.pointer(dateDSM))
    brathl_Error(errno)
    return dateDSM


## Converts a year, month, day, hour, minute, second, microsecond date into a
# decimal julian date, according to refDate parameter.
# @param[in] dateYMDHMSM -> date to convert (Type: brathl_DateYMDHMSM)
# @param[in] refDate     -> date reference conversion (see brathl_refDate)
# @return    dateJulian  -> current date/time (Type: brathl_DateJulian)
def brathl_YMDHMSM2Julian(dateYMDHMSM, refDate):
    dateJulian = brathl_DateJulian()
    errno = lib.brathl_YMDHMSM2Julian(ctypes.pointer(dateYMDHMSM),
                                      refDate,
                                      ctypes.pointer(dateJulian))
    brathl_Error(errno)
    return dateJulian


## Converts a year, month, day, hour, minute, second, microsecond date  into
# seconds, according to refDate parameter.
# @param[in] dateYMDHMSM -> date to convert (Type: brathl_DateYMDHMSM)
# @param[in] refDate     -> date reference conversion (see brathl_refDate)
# @return    dateSeconds -> current date/time (Type: brathl_DateSecond)
def brathl_YMDHMSM2Seconds(dateYMDHMSM, refDate):
    dateSeconds = brathl_DateSecond()
    errno = lib.brathl_YMDHMSM2Seconds(ctypes.pointer(dateYMDHMSM),
                                       refDate,
                                       ctypes.pointer(dateSeconds))
    brathl_Error(errno)
    return dateSeconds


#
#--- Cycle/Date Conversion Python APIs ----
#

## Converts a cyle/pass into a date
# @param[in] mission     -> mission type (Type: brathl_mission)
# @param[in] cycle       -> number of cycle to convert (Type: int)
# @param[in] nbPass      -> number of pass in the cycle to convert (Type: int)
# @return    dateYMDHMSM -> date/time corresponding to the cycle/pass (Type: brathl_DateYMDHMSM)
def brathl_Cycle2YMDHMSM(mission, cycle, nbPass):
    cycle       = ctypes.c_int(cycle)
    nbPass      = ctypes.c_int(nbPass)
    dateYMDHMSM = brathl_DateYMDHMSM()
    errno = lib.brathl_Cycle2YMDHMSM(mission,
                                     cycle,
                                     nbPass,
                                     ctypes.pointer(dateYMDHMSM))
    brathl_Error(errno)
    return dateYMDHMSM


## Converts a date into a cyle/pass
# @param[in] mission     -> mission type (Type: brathl_mission)
# @param[in] dateYMDHMSM -> date/time to convert (Type: brathl_DateYMDHMSM)
# @return    cycle       -> number of cycle (Type: int)
# @return    nbPass      -> number of pass in the cycle (Type: int)
def brathl_YMDHMSM2Cycle(mission, dateYMDHMSM):
    cycle       = ctypes.c_int()
    nbPass      = ctypes.c_int()
    errno = lib.brathl_YMDHMSM2Cycle(mission,
                                     ctypes.pointer(dateYMDHMSM),
                                     ctypes.pointer(cycle),
                                     ctypes.pointer(nbPass))
    brathl_Error(errno)
    return cycle.value, nbPass.value


#
#--- Data Reading Python API ----
#

## Read data from a set of files
# @param[in] fileNames   -> File name list. Empty strings are ignored.
#                           (Type: list of strings)
# @param[in] recordName  -> Name of the fields record. For netCDF files 
#                           'recordName' is 'data'. (Type: string)
# @param[in] selection   -> Expression for selecting data fields. If empty 
#                           string, all data is selected. (Type: string )
# @param[in] expressions -> Expressions applyed to data fields to build wanted
#                           value. If empty string, the returned data are always
#                           default values. (Type: list of strings)
# @param[in] units       -> Wanted unit for each expression. Must be None or
#                           of 'expressions' size. If None, no unit conversion
#                           is done. If an entry is None or an empty string, no
#                           unit conversion is applyed to the data of the
#                           corresponding expression. (Type: list of strings)
# @param[in] ignoreOutOfRange -> Skip excess data. If there are too much values
#                              to store they are ignored (case is set True).
#                              Must be False if statistics is True. (Type: bool)
# @param[in] statistics -> Returns statistics on data instead of data 
#                          themselves. (Type: bool)
#                          The returned values for each expression are:
#                        - Count of valid data taken into account;
#                        - Mean of the valid data;
#                        - Standard deviation of the valid data;
#                        - Minimum value of the valid data;
#                        - Maximum value of the valid data.
# @param[in] defaultValue -> Value to use for default/missing values. 
#                            (Type: float or int)
# @return  dataResults -> Data read. Must contain a number of entries to
#                         values to read equal to expressions size. (Type: list)
def brathl_ReadData(fileNames,
                    recordName,
                    selection,
                    expressions,
                    units,
                    ignoreOutOfRange,
                    statistics,
                    defaultValue):

    ## Local function. Converts a Python List of Strings into a C Array of Char.
    # @param[in] ListOfStrings -> List of strings to be converted.
    #                             (Type: list of strings)
    # @return     ArrayOfChars -> Array of Char pointers in C. (Type: char*)
    def ListStrings2ArrayChars (ListOfStrings):
        nbStrings    = len(ListOfStrings)
        ArrayOfChars = (ctypes.c_char_p * nbStrings)()
        for i in range(nbStrings):
            ArrayOfChars[i] = ctypes.c_char_p (bytes(ListOfStrings[i], 'utf8'))
        return ArrayOfChars


    #---------------- Creating 'brathl_ReadData' variables ----------------#
    nbFiles            = ctypes.c_int( len(fileNames) )
    c_fileNames        = ListStrings2ArrayChars( fileNames )
    c_recordName       = ctypes.c_char_p( bytes(recordName, 'utf8') )

    if selection:  c_selection = ctypes.c_char_p(bytes(selection, 'utf8'))
    else:          c_selection = ctypes.c_char_p()

    nbData             = ctypes.c_int( len(expressions) )
    c_expressions      = ListStrings2ArrayChars( expressions )
    c_units            = ListStrings2ArrayChars( units )
    dataRead           = (ctypes.POINTER(ctypes.c_double) * len(expressions))()

    sizes              = (ctypes.c_int * len(expressions))()
    for i in range(len(expressions)):
        sizes[i] = -1

    actualSize         = ctypes.c_int()
    c_ignoreOutOfRange = ctypes.c_int(ignoreOutOfRange)
    c_statistics       = ctypes.c_int(statistics)
    c_defaultValue     = ctypes.c_double(defaultValue)


    #-----------------     Running 'brathl_ReadData'     -----------------#
    errno = lib.brathl_ReadData(nbFiles,       # Nb. of files in fileNames
                                c_fileNames,   # File name list
                                c_recordName,  # Name of record
                                c_selection,   # Selection string
                                nbData,        # Nb. of data expressions
                                c_expressions, # Expressions
                                c_units,       # Each expression units
                                dataRead,      # Data read
                                sizes,         # Nb. allocated values in results
                                ctypes.byref(actualSize), # Nb. actual data 
                                c_ignoreOutOfRange,       # Skip excess data
                                c_statistics,             # Return statistics
                                c_defaultValue )          # Default/missing value

    #-- Converting C array into a Python list with the 'dataRead' values --#
    dataResults = [[dataRead[j][i] for i in range(sizes[j])] 
                                   for j in range (len(expressions))]

    #brathl_Error(errno) # Not required: it already prints the error messages
    return dataResults


