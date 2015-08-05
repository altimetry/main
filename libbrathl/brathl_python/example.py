#!/usr/bin/python -tt
# <example.py>
# This module contains examples to test the Brathl Python API.
# @version v0.1

import sys
# Add the lib folder path to the sys.path list
sys.path.append('../../lib/')

from brathl import *



def main():
    
    
    # EXAMPLE: brathl_ReadData
    print ('\nRunning function: brathl_ReadData...')

    fileNames = ['example.nc']
    recordName = 'data'
    selection  = ''               # 'lon_mwr_l1b > 10 && lon_mwr_l1b < 40'
    expressions = ['lon_mwr_l1b'] # ['lat_mwr_l1b', 'lon_mwr_l1b']
    units       = ['degrees']     # ['radians', 'radians']
    ignoreOutOfRange = False
    statistics       = False
    defaultValue = 0

    dataResults = brathl_ReadData(fileNames,
                                  recordName,
                                  selection,
                                  expressions,
                                  units,
                                  ignoreOutOfRange,
                                  statistics,
                                  defaultValue)

    print ("--------------- Printing data values ---------------")
    for i in range(len(dataResults)):
        print (expressions[i], "(", len(dataResults[i]), " values) =", dataResults[i])
    print ("----------------------------------------------------")


    '''
    # EXAMPLE: brathl_SetRefDateUser1 and brathl_SetRefDateUser2
    print ('\nDefining user data references (REFUSER1 and REFUSER2)...')
    dateRef1 = "2000 01 01 00:00:00.000"
    dateRef2 = "2000 01 03 00:00:00.000"

    brathl_SetRefDateUser1 (dateRef1)
    brathl_SetRefDateUser2 (dateRef2)

    print ('\nTesting with function: brathl_DSM2Julian...')
    dateDSM = brathl_DateDSM(brathl_refDate.REFUSER2, 1, 43200, 0) # 1.5 day

    dateJulian = brathl_DSM2Julian(dateDSM, brathl_refDate.REFUSER1)
    print ("The REFDATE is: ", dateJulian.REFDATE)
    print ("and JULIAN is: ", dateJulian.JULIAN) # should be: 3.5 = 1.5 + 2



    # EXAMPLE: brathl_DayOfYear
    print ('\nRunning function: brathl_DayOfYear...')
    d = brathl_DateYMDHMSM (2009, 2, 1, 12, 25, 5, 0)

    dayOfYear = brathl_DayOfYear (d)
    print ("The day of Year is: ", dayOfYear)   # should be: 32



    # EXAMPLE: brathl_DiffDSM
    print ('\nRunning function: brathl_DiffDSM...')
    d1 = brathl_DateDSM (brathl_refDate.REF19580101, 0, 0, 0)
    d2 = brathl_DateDSM (brathl_refDate.REF19500101, 0, 0, 0)

    diff = brathl_DiffDSM (d1, d2)             # seconds   sec/day   days
    print ("The difference is: ", diff)   # should be: 252460800 = 86400*(365*8+2)



    # EXAMPLE: brathl_DiffJulian
    print ('\nRunning function: brathl_DiffJulian...')
    d1 = brathl_DateJulian (brathl_refDate.REF20000101, 1.5)
    d2 = brathl_DateJulian (brathl_refDate.REF19900101, 0)

    diff = brathl_DiffJulian (d1, d2)      # seconds    sec/day  days
    print ("The difference is: ", diff) # should be: 315662400 = 86400*(365*10+2+1.5)



    # EXAMPLE: brathl_DiffYMDHMSM
    print ('\nRunning function: brathl_DiffYMDHMSM...')
    d1 = brathl_DateYMDHMSM (2009, 9, 3, 12, 25, 15, 0)
    d2 = brathl_DateYMDHMSM (2009, 9, 3, 12, 25,  5, 0)

    diff = brathl_DiffYMDHMSM (d1, d2)
    print ("The difference is: ", diff)   # should be: 10



    # EXAMPLE: brathl_DSM2Julian
    print ('\nRunning function: brathl_DSM2Julian...')
    dateDSM = brathl_DateDSM(brathl_refDate.REF20000101, 1, 43200, 0) # 1.5 day

    dateJulian = brathl_DSM2Julian(dateDSM, brathl_refDate.REF19900101)
    print ("The REFDATE is: ", dateJulian.REFDATE)
    print ("and JULIAN is: ", dateJulian.JULIAN) # should be: 3653.5 = 365*10+2+1.5



    # EXAMPLE: brathl_DSM2Seconds
    print ('\nRunning function: brathl_DSM2Seconds...')
    dateDSM = brathl_DateDSM(brathl_refDate.REF19900101, 0, 0, 0)

    dateSeconds = brathl_DSM2Seconds(dateDSM, brathl_refDate.REF19850101)
    print ("The REFDATE is: ", dateSeconds.REFDATE)
    print ("and SECOND is: ", dateSeconds.SECOND) # should be: 157766400 = 86400*(365*5+1)



    # EXAMPLE: brathl_DSM2YMDHMSM
    print ('\nRunning function: brathl_DSM2YMDHMSM...')
    dateDSM = brathl_DateDSM(brathl_refDate.REF19500101, 1, 62, 100000)

    dateYMDHMSM = brathl_DSM2YMDHMSM(dateDSM)
    print ("The dateYMDHMSM is: %s-%s-%s %s:%s:%s" % (dateYMDHMSM.YEAR,
                                                      dateYMDHMSM.MONTH,
                                                      dateYMDHMSM.DAY,
                                                      dateYMDHMSM.HOUR,
                                                      dateYMDHMSM.MINUTE,
                                                      (dateYMDHMSM.SECOND + 
                                                      dateYMDHMSM.MUSECOND/1e6))
           ) # should be: 1950-1-2 0:1:2.1



    # EXAMPLE: brathl_Julian2DSM
    print ('\nRunning function: brathl_Julian2DSM...')
    dateJulian = brathl_DateJulian (brathl_refDate.REF20000101, 1.5)

    dateDSM = brathl_Julian2DSM (dateJulian, brathl_refDate.REF19850101)
    print ("The DateDSM is: %s days and %s secs. " % (dateDSM.DAY,
                                                      (dateDSM.SECOND + 
                                                      dateDSM.MUSECOND/1e6))
           ) # should be: 5479 days and 43200 secs.



    # EXAMPLE: brathl_Julian2Seconds
    print ('\nRunning function: brathl_Julian2Seconds...')
    dateJulian = brathl_DateJulian (brathl_refDate.REF20000101, 1.5)

    dateSeconds = brathl_Julian2Seconds (dateJulian, brathl_refDate.REF19900101)
    print ("The result is: %s secs." % dateSeconds.SECOND) # should be: 315662400 = 86400*(365*10+2+1.5)



    # EXAMPLE: brathl_Julian2YMDHMSM
    print ('\nRunning function: brathl_Julian2YMDHMSM...')
    dateJulian = brathl_DateJulian (brathl_refDate.REF20000101, 1.5)

    dateYMDHMSM = brathl_Julian2YMDHMSM(dateJulian)
    print ("The dateYMDHMSM is: %s-%s-%s %s:%s:%s" % (dateYMDHMSM.YEAR,
                                                      dateYMDHMSM.MONTH,
                                                      dateYMDHMSM.DAY,
                                                      dateYMDHMSM.HOUR,
                                                      dateYMDHMSM.MINUTE,
                                                      (dateYMDHMSM.SECOND + 
                                                      dateYMDHMSM.MUSECOND/1e6))
           ) # should be: 2000-1-2 12:0:0.0



    # EXAMPLE: brathl_Seconds2DSM
    print ('\nRunning function: brathl_Seconds2DSM...')
    dateSeconds = brathl_DateSecond (brathl_refDate.REF19500101, 86401.01)

    dateDSM = brathl_Seconds2DSM (dateSeconds, brathl_refDate.REF19500101)
    print ("The DateDSM is: %s days and %s secs. " % (dateDSM.DAY,
                                                     (dateDSM.SECOND + 
                                                      dateDSM.MUSECOND/1e6))
        ) # should be: 1 days and 1.01 secs.



    # EXAMPLE: brathl_Seconds2Julian
    print ('\nRunning function: brathl_Seconds2Julian...')
    dateSeconds = brathl_DateSecond (brathl_refDate.REF19580101, 86400)

    dateJulian = brathl_Seconds2Julian (dateSeconds, brathl_refDate.REF19500101)
    print ("The REFDATE is: ", dateJulian.REFDATE)
    print ("and JULIAN is: ", dateJulian.JULIAN) # should be: 2923 = (365*8+2) + 1



    # EXAMPLE: brathl_Seconds2YMDHMSM
    print ('\nRunning function: brathl_Seconds2YMDHMSM...')
    dateSeconds = brathl_DateSecond (brathl_refDate.REF19580101, 86400)

    dateYMDHMSM = brathl_Seconds2YMDHMSM(dateSeconds)
    print ("The dateYMDHMSM is: %s-%s-%s %s:%s:%s" % (dateYMDHMSM.YEAR,
                                                      dateYMDHMSM.MONTH,
                                                      dateYMDHMSM.DAY,
                                                      dateYMDHMSM.HOUR,
                                                      dateYMDHMSM.MINUTE,
                                                      (dateYMDHMSM.SECOND + 
                                                      dateYMDHMSM.MUSECOND/1e6))
        ) # should be: 1958-1-2 0:0:0.0



    # EXAMPLE: brathl_NowYMDHMSM
    print ('\nRunning function: brathl_NowYMDHMSM...')

    dateYMDHMSM = brathl_NowYMDHMSM()
    print ("The current date is: %s-%s-%s %s:%s:%s" % (dateYMDHMSM.YEAR,
                                                       dateYMDHMSM.MONTH,
                                                       dateYMDHMSM.DAY,
                                                       dateYMDHMSM.HOUR,
                                                       dateYMDHMSM.MINUTE,
                                                      (dateYMDHMSM.SECOND + 
                                                      dateYMDHMSM.MUSECOND/1e6))
          ) # should be: current date/time in YYYY-MM-DD HH:MM:SS.MU



    # EXAMPLE: brathl_YMDHMSM2DSM
    print ('\nRunning function: brathl_YMDHMSM2DSM...')
    dateYMDHMSM = brathl_DateYMDHMSM (2000, 1, 1, 12, 25, 0, 100000)

    dateDSM = brathl_YMDHMSM2DSM(dateYMDHMSM, brathl_refDate.REF20000101)
    print ("The DateDSM is: %s days and %s secs. " % (dateDSM.DAY,
                                                     (dateDSM.SECOND + 
                                                      dateDSM.MUSECOND/1e6))
    ) # should be: 0 days and 44700.1 secs. (44700.1 = 12*3600+25*60 + 0.1)



    # EXAMPLE: brathl_YMDHMSM2Julian
    print ('\nRunning function: brathl_YMDHMSM2Julian...')
    dateYMDHMSM = brathl_DateYMDHMSM (2000, 1, 1, 12, 25, 0, 100000)

    dateJulian = brathl_YMDHMSM2Julian(dateYMDHMSM, brathl_refDate.REF20000101)
    print ("The REFDATE is: ", dateJulian.REFDATE)
    print ("and JULIAN is: ", dateJulian.JULIAN) # should be: 0.5173622685185185



    # EXAMPLE: brathl_YMDHMSM2Seconds
    print ('\nRunning function: brathl_YMDHMSM2Seconds...')
    dateYMDHMSM = brathl_DateYMDHMSM (2000, 1, 1, 12, 25, 0, 0)

    dateSeconds = brathl_YMDHMSM2Seconds(dateYMDHMSM, brathl_refDate.REF20000101)
    print ("The result is: %s secs." % dateSeconds.SECOND) # should be: 44700 = 3600*12 + 60*25



    # EXAMPLE: brathl_Cycle2YMDHMSM
    print ('\nRunning function: brathl_Cycle2YMDHMSM...')
    cycle  = 1
    nbPass = 2

    dateYMDHMSM = brathl_Cycle2YMDHMSM(brathl_mission.JASON1, cycle, nbPass)
    print ("The current date is: %s-%s-%s %s:%s:%s" % (dateYMDHMSM.YEAR,
                                                       dateYMDHMSM.MONTH,
                                                       dateYMDHMSM.DAY,
                                                       dateYMDHMSM.HOUR,
                                                       dateYMDHMSM.MINUTE,
                                                      (dateYMDHMSM.SECOND + 
                                                       dateYMDHMSM.MUSECOND/1e6))
          ) # should be: 2002-1-15 6:35:43.261871



    # EXAMPLE: brathl_YMDHMSM2Cycle
    print ('\nRunning function: brathl_YMDHMSM2Cycle...')
    dateYMDHMSM = brathl_DateYMDHMSM (2002, 1, 15, 6, 35, 43, 261871)

    cycle, nbPass = brathl_YMDHMSM2Cycle(brathl_mission.JASON1, dateYMDHMSM)
    print ("The cycle is : ", cycle)  # should be: 1
    print ("The nbPass is: ", nbPass) # should be: 2
    '''


    print ('\nSUCCESS!\n')


if __name__ == '__main__':
    main()
