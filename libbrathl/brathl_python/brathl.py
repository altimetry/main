#!/usr/bin/python -tt
# <example.py>

import ctypes
#from ctypes.util import find_library

# brathl_lib = "/home/brat/s3-altb/project/dev/bin/i386/Debug/libbrathl.so"
brathl_lib = "/home/brat/s3-altb/project/dev/bin/x86_64/Debug/libbrathl-dyn.so"

# Load every library that is used by your library:
#ctypes.CDLL("/fullpath/lib.so", mode = ctypes.RTLD_GLOBAL)
lib = ctypes.cdll.LoadLibrary(brathl_lib)   #find_library(brathl_lib)
#getattr(lib, "brathl_DiffYMDHMSM")


#------------------------------------
#      BRATHL Data structures
#------------------------------------

## Date reference enumeration - Used to give a date a a start reference
class brathl_refDate():
    REF19500101 = ctypes.c_int(0) # reference to 1950-01-01 00:00:00:00
    REF19580101 = ctypes.c_int(1) # reference to 1958-01-01 00:00:00:00
    REF19850101 = ctypes.c_int(2) # reference to the 1985-01-01 00:00:00:00
    REF19900101 = ctypes.c_int(3) # reference to the 1990-01-01 00:00:00:00
    REF20000101 = ctypes.c_int(4) # reference to the 2000-01-01 00:00:00:00
    REFUSER1    = ctypes.c_int(5) # reference to a user-defined date #brathl_refDateUser1
    REFUSER2    = ctypes.c_int(6) # reference to a second user-defined date #brathl_refDateUser2


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



#------------------------------------
#        BRATHL Functions
#------------------------------------

## Retrieves the day of the year of a date.
# @param[in] date      -> date object (Type: brathl_DateYMDHMSM)
# @return    dayOfYear -> day of year (Type: long integer)
# @return    errno     -> brathl error number (Optional)
def brathl_DayOfYear(date):
    dayOfYear = ctypes.c_uint()
    error = lib.brathl_DayOfYear(ctypes.pointer(date),
                                 ctypes.pointer(dayOfYear))
    return dayOfYear.value, error


## Computes the difference between two dates (date1 - date2)
# @param[in] dateDSM1  -> date object (Type: brathl_DateDSM)
# @param[in] dateDSM2  -> date object (Type: brathl_DateDSM)
# @return    diff      -> difference in seconds (Type: float)
# @return    errno     -> brathl error number (Optional)
def brathl_DiffDSM(dateDSM1, dateDSM2):
    diff = ctypes.c_double()
    error = lib.brathl_DiffDSM(ctypes.pointer(dateDSM1),
                               ctypes.pointer(dateDSM2),
                               ctypes.pointer(diff))
    return diff.value, error


## Computes the difference between two dates (date1 - date2)
# @param[in] dateJulian1 -> date object (Type: brathl_DateJulian)
# @param[in] dateJulian2 -> date object (Type: brathl_DateJulian)
# @return    diff        -> difference in seconds (Type: float)
# @return    errno       -> brathl error number (Optional)
def brathl_DiffJulian(dateJulian1, dateJulian2):
    diff = ctypes.c_double()
    error = lib.brathl_DiffJulian(ctypes.pointer(dateJulian1),
                                  ctypes.pointer(dateJulian2),
                                  ctypes.pointer(diff))
    return diff.value, error


## Computes the difference, in seconds, between two dates (date1 - date2)
# @param[in] date1 -> date object (Type: brathl_DateYMDHMSM)
# @param[in] date2 -> date object (Type: brathl_DateYMDHMSM)
# @return    diff  -> difference in seconds (Type: float)
# @return    errno -> brathl error number (Optional)
def brathl_DiffYMDHMSM(date1, date2):
    diff = ctypes.c_double()
    error = lib.brathl_DiffYMDHMSM(ctypes.pointer(date1),
                                   ctypes.pointer(date2), 
                                   ctypes.pointer(diff))
    return diff.value, error


## Converts a days-seconds-microseconds date into a decimal julian date, 
# according to refDate parameter.
# @param[in] dateDSM    -> date to convert (Type: brathl_DateDSM)
# @param[in] refDate    -> date reference conversion (see brathl_refDate)
# @return    dateJulian -> result of the conversion (Type: brathl_DateJulian)
# @return    errno      -> brathl error number (Optional)
def brathl_DSM2Julian(dateDSM, refDate):
    dateJulian = brathl_DateJulian()
    error = lib.brathl_DSM2Julian(ctypes.pointer(dateDSM),
                                  refDate, 
                                  ctypes.pointer(dateJulian))
    return dateJulian, error


## Converts a days-seconds-microseconds date into seconds, according to refDate
# parameter.
# @param[in] dateDSM     -> date to convert (Type: brathl_DateDSM)
# @param[in] refDate     -> date reference conversion (see brathl_refDate)
# @return    dateSeconds -> result of the conversion (Type: brathl_DateSecond)
# @return    errno       -> brathl error number (Optional)
def brathl_DSM2Seconds(dateDSM, refDate):
    dateSeconds = brathl_DateSecond()
    error = lib.brathl_DSM2Seconds(ctypes.pointer(dateDSM),
                                   refDate, 
                                   ctypes.pointer(dateSeconds))
    return dateSeconds, error


## Converts a days-seconds-microseconds date into into a year, month, day, hour,
# minute, second, microsecond date.
# @param[in] dateDSM     -> date to convert (Type: brathl_DateDSM)
# @return    dateYMDHMSM -> result of the conversion (Type: brathl_DateYMDHMSM)
# @return    errno       -> brathl error number (Optional)
def brathl_DSM2YMDHMSM(dateDSM):
    dateYMDHMSM = brathl_DateYMDHMSM()
    error = lib.brathl_DSM2YMDHMSM(ctypes.pointer(dateDSM),
                                   ctypes.pointer(dateYMDHMSM))
    return dateYMDHMSM, error


## Converts a decimal julian date into a days-seconds-microseconds date, 
# according to refDate parameter
# @param[in] dateJulian -> date to convert (Type: brathl_DateJulian)
# @param[in] refDate    -> date reference conversion (see brathl_refDate)
# @return    dateDSM    -> result of the conversion (Type: brathl_DateDSM)
# @return    errno      -> brathl error number (Optional)
def brathl_Julian2DSM(dateJulian, refDate):
    dateDSM = brathl_DateDSM()
    error = lib.brathl_Julian2DSM(ctypes.pointer(dateJulian),
                                  refDate,
                                  ctypes.pointer(dateDSM))
    return dateDSM, error


## Converts a decimal julian date into seconds, according to refDate parameter
# @param[in] dateJulian -> date to convert (Type: brathl_DateJulian)
# @param[in] refDate    -> date reference conversion (see brathl_refDate)
# @return    dateSeconds-> result of the conversion (Type: brathl_DateSecond)
# @return    errno      -> brathl error number (Optional)
def brathl_Julian2Seconds(dateJulian, refDate):
    dateSeconds = brathl_DateSecond()
    error = lib.brathl_Julian2Seconds(ctypes.pointer(dateJulian),
                                      refDate,
                                      ctypes.pointer(dateSeconds))
    return dateSeconds, error


## Converts a decimal julian date into a year, month, day, hour, minute, second,
# microsecond date.
# @param[in] dateJulian  -> date to convert (Type: brathl_DateJulian)
# @return    dateYMDHMSM -> result of the conversion (Type: brathl_DateYMDHMSM)
# @return    errno       -> brathl error number (Optional)
def brathl_Julian2YMDHMSM(dateJulian):
    dateYMDHMSM = brathl_DateYMDHMSM()
    error = lib.brathl_Julian2YMDHMSM(ctypes.pointer(dateJulian),
                                      ctypes.pointer(dateYMDHMSM))
    return dateYMDHMSM, error


## Converts seconds into a days-seconds-microseconds date, according to refDate 
# parameter.
# @param[in] dateSeconds -> date to convert (Type: brathl_DateSecond)
# @param[in] refDate     -> date reference conversion (see brathl_refDate)
# @return    dateDMS     -> result of the conversion (Type: brathl_DateDSM)
# @return    errno       -> brathl error number (Optional)
def brathl_Seconds2DSM(dateSeconds, refDate):
    dateDSM = brathl_DateDSM()
    error = lib.brathl_Seconds2DSM(ctypes.pointer(dateSeconds),
                                   refDate,
                                   ctypes.pointer(dateDSM))
    return dateDSM, error


## Converts seconds into a decimal julian date, according to refDate parameter.
# @param[in] dateSeconds -> date to convert (Type: brathl_DateSecond)
# @param[in] refDate     -> date reference conversion (see brathl_refDate)
# @return    dateJulian  -> result of the conversion (Type: brathl_DateJulian)
# @return    errno       -> brathl error number (Optional)
def brathl_Seconds2Julian(dateSeconds, refDate):
    dateJulian = brathl_DateJulian()
    error = lib.brathl_Seconds2Julian(ctypes.pointer(dateSeconds),
                                      refDate,
                                      ctypes.pointer(dateJulian))
    return dateJulian, error


## Converts seconds into a year, month, day, hour, minute, second, microsecond 
# date.
# @param[in] dateSeconds -> date to convert (Type: brathl_DateSecond)
# @return    dateYMDHMSM -> result of the conversion (Type: brathl_DateYMDHMSM)
# @return    errno       -> brathl error number (Optional)
def brathl_Seconds2YMDHMSM(dateSeconds):
    dateYMDHMSM = brathl_DateYMDHMSM()
    error = lib.brathl_Seconds2YMDHMSM(ctypes.pointer(dateSeconds),
                                       ctypes.pointer(dateYMDHMSM))
    return dateYMDHMSM, error


## Gets the current year, month, day, hour, minute, second, microsecond date.
# @return    dateYMDHMSM -> current date/time (Type: brathl_DateYMDHMSM)
# @return    errno       -> brathl error number (Optional)
def brathl_NowYMDHMSM():
    dateYMDHMSM = brathl_DateYMDHMSM()
    error = lib.brathl_NowYMDHMSM(ctypes.pointer(dateYMDHMSM))
    return dateYMDHMSM, error




def main():

    
    # EXAMPLE: brathl_DayOfYear
    print ('\nRunning function: brathl_DayOfYear...')
    d = brathl_DateYMDHMSM (2009, 2, 1, 12, 25, 5, 0)

    dayOfYear, error = brathl_DayOfYear (d)

    print ("The day of Year is: ", dayOfYear)   # should be: 32
    print ("The error is : ", error)


    # EXAMPLE: brathl_DiffDSM
    print ('\nRunning function: brathl_DiffDSM...')
    d1 = brathl_DateDSM (brathl_refDate.REF19580101, 0, 0, 0)
    d2 = brathl_DateDSM (brathl_refDate.REF19500101, 0, 0, 0)

    diff, error = brathl_DiffDSM (d1, d2)             # seconds   sec/day   days
    print ("The difference is: ", diff)   # should be: 252460800 = 86400*(365*8+2)
    print ("The error is : ", error)


    # EXAMPLE: brathl_DiffJulian
    print ('\nRunning function: brathl_DiffJulian...')
    d1 = brathl_DateJulian (brathl_refDate.REF20000101, 1.5)
    d2 = brathl_DateJulian (brathl_refDate.REF19900101, 0)

    diff, error = brathl_DiffJulian (d1, d2)      # seconds    sec/day  days
    print ("The difference is: ", diff) # should be: 315662400 = 86400*(365*10+2+1.5)
    print ("The error is : ", error)


    # EXAMPLE: brathl_DiffYMDHMSM
    print ('\nRunning function: brathl_DiffYMDHMSM...')
    d1 = brathl_DateYMDHMSM (2009, 9, 3, 12, 25, 15, 0)
    d2 = brathl_DateYMDHMSM (2009, 9, 3, 12, 25,  5, 0)

    diff, error = brathl_DiffYMDHMSM (d1, d2)
    print ("The difference is: ", diff)   # should be: 10
    print ("The error is : ", error)


    # EXAMPLE: brathl_DSM2Julian
    print ('\nRunning function: brathl_DSM2Julian...')
    dateDSM = brathl_DateDSM(brathl_refDate.REF20000101, 1, 43200, 0) # 1.5 day

    dateJulian, error = brathl_DSM2Julian(dateDSM, brathl_refDate.REF19900101)
    print ("The REFDATE is: ", dateJulian.REFDATE)
    print ("and JULIAN is: ", dateJulian.JULIAN) # should be: 3653.5 = 365*10+2+1.5
    print ("The error is : ", error)


    # EXAMPLE: brathl_DSM2Seconds
    print ('\nRunning function: brathl_DSM2Seconds...')
    dateDSM = brathl_DateDSM(brathl_refDate.REF19900101, 0, 0, 0)

    dateSeconds, error = brathl_DSM2Seconds(dateDSM, brathl_refDate.REF19850101)
    print ("The REFDATE is: ", dateSeconds.REFDATE)
    print ("and SECOND is: ", dateSeconds.SECOND) # should be: 157766400 = 86400*(365*5+1)
    print ("The error is : ", error)


    # EXAMPLE: brathl_DSM2YMDHMSM
    print ('\nRunning function: brathl_DSM2YMDHMSM...')
    dateDSM = brathl_DateDSM(brathl_refDate.REF19500101, 1, 62, 100000)

    dateYMDHMSM, error = brathl_DSM2YMDHMSM(dateDSM)
    print ("The dateYMDHMSM is: %s-%s-%s %s:%s:%s" % (dateYMDHMSM.YEAR,
                                                      dateYMDHMSM.MONTH,
                                                      dateYMDHMSM.DAY,
                                                      dateYMDHMSM.HOUR,
                                                      dateYMDHMSM.MINUTE,
                                                      (dateYMDHMSM.SECOND + 
                                                      dateYMDHMSM.MUSECOND/1e6))
           ) # should be: 1950-1-2 0:1:2.1
    print ("The error is : ", error)


    # EXAMPLE: brathl_Julian2DSM
    print ('\nRunning function: brathl_Julian2DSM...')
    dateJulian = brathl_DateJulian (brathl_refDate.REF20000101, 1.5)

    dateDSM, error = brathl_Julian2DSM (dateJulian, brathl_refDate.REF19850101)
    print ("The DateDSM is: %s days and %s secs. " % (dateDSM.DAY,
                                                      (dateDSM.SECOND + 
                                                      dateDSM.MUSECOND/1e6))
           ) # should be: 5479 days and 43200 secs.
    print ("The error is : ", error)


    # EXAMPLE: brathl_Julian2Seconds
    print ('\nRunning function: brathl_Julian2Seconds...')
    dateJulian = brathl_DateJulian (brathl_refDate.REF20000101, 1.5)

    dateSeconds, error = brathl_Julian2Seconds (dateJulian, brathl_refDate.REF19900101)
    print ("The result is: %s secs." % dateSeconds.SECOND) # should be: 315662400 = 86400*(365*10+2+1.5)
    print ("The error is : ", error)


    # EXAMPLE: brathl_Julian2YMDHMSM
    print ('\nRunning function: brathl_Julian2YMDHMSM...')
    dateJulian = brathl_DateJulian (brathl_refDate.REF20000101, 1.5)

    dateYMDHMSM, error = brathl_Julian2YMDHMSM(dateJulian)
    print ("The dateYMDHMSM is: %s-%s-%s %s:%s:%s" % (dateYMDHMSM.YEAR,
                                                      dateYMDHMSM.MONTH,
                                                      dateYMDHMSM.DAY,
                                                      dateYMDHMSM.HOUR,
                                                      dateYMDHMSM.MINUTE,
                                                      (dateYMDHMSM.SECOND + 
                                                      dateYMDHMSM.MUSECOND/1e6))
           ) # should be: 2000-1-2 12:0:0.0
    print ("The error is : ", error)


    # EXAMPLE: brathl_Seconds2DSM
    print ('\nRunning function: brathl_Seconds2DSM...')
    dateSeconds = brathl_DateSecond (brathl_refDate.REF19500101, 86401.01)

    dateDSM, error = brathl_Seconds2DSM (dateSeconds, brathl_refDate.REF19500101)
    print ("The DateDSM is: %s days and %s secs. " % (dateDSM.DAY,
                                                     (dateDSM.SECOND + 
                                                      dateDSM.MUSECOND/1e6))
        ) # should be: 1 days and 1.01 secs.
    print ("The error is : ", error)


    # EXAMPLE: brathl_Seconds2Julian
    print ('\nRunning function: brathl_Seconds2Julian...')
    dateSeconds = brathl_DateSecond (brathl_refDate.REF19580101, 86400)

    dateJulian, error = brathl_Seconds2Julian (dateSeconds, brathl_refDate.REF19500101)
    print ("The REFDATE is: ", dateJulian.REFDATE)
    print ("and JULIAN is: ", dateJulian.JULIAN) # should be: 2923 = (365*8+2) + 1
    print ("The error is : ", error)


    # EXAMPLE: brathl_Seconds2YMDHMSM
    print ('\nRunning function: brathl_Seconds2YMDHMSM...')
    dateSeconds = brathl_DateSecond (brathl_refDate.REF19580101, 86400)

    dateYMDHMSM, error = brathl_Seconds2YMDHMSM(dateSeconds)
    print ("The dateYMDHMSM is: %s-%s-%s %s:%s:%s" % (dateYMDHMSM.YEAR,
                                                      dateYMDHMSM.MONTH,
                                                      dateYMDHMSM.DAY,
                                                      dateYMDHMSM.HOUR,
                                                      dateYMDHMSM.MINUTE,
                                                      (dateYMDHMSM.SECOND + 
                                                      dateYMDHMSM.MUSECOND/1e6))
        ) # should be: 1958-1-2 0:0:0.0
    print ("The error is : ", error)


    # EXAMPLE: brathl_NowYMDHMSM
    print ('\nRunning function: brathl_NowYMDHMSM...')

    dateYMDHMSM, error = brathl_NowYMDHMSM()
    print ("The current date is: %s-%s-%s %s:%s:%s" % (dateYMDHMSM.YEAR,
                                                       dateYMDHMSM.MONTH,
                                                       dateYMDHMSM.DAY,
                                                       dateYMDHMSM.HOUR,
                                                       dateYMDHMSM.MINUTE,
                                                      (dateYMDHMSM.SECOND + 
                                                      dateYMDHMSM.MUSECOND/1e6))
          ) # should be: current date/time in YYYY-MM-DD HH:MM:SS.MU
    print ("The error is : ", error)




    print ('\nWELL DONE!\n')

    
    
if __name__ == '__main__':
    main()
