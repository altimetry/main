C
C Constants
C

      INTEGER BRATHL_SUCCESS
      PARAMETER(BRATHL_SUCCESS  =                    0)
      INTEGER BRATHL_ERROR_INVALID_DATE
      PARAMETER(BRATHL_ERROR_INVALID_DATE  =         -101)
      INTEGER BRATHL_ERROR_INVALID_DATE_REF
      PARAMETER(BRATHL_ERROR_INVALID_DATE_REF =      -102)
      INTEGER BRATHL_ERROR_INVALID_DATE_REF_CONV
      PARAMETER(BRATHL_ERROR_INVALID_DATE_REF_CONV = -103)
      INTEGER BRATHL_ERROR_INVALID_DSM
      PARAMETER(BRATHL_ERROR_INVALID_DSM =           -104)
      INTEGER BRATHL_ERROR_INVALID_YEAR
      PARAMETER(BRATHL_ERROR_INVALID_YEAR =          -105)
      INTEGER BRATHL_ERROR_INVALID_MONTH
      PARAMETER(BRATHL_ERROR_INVALID_MONTH =         -106)
      INTEGER BRATHL_ERROR_INVALID_DAY
      PARAMETER(BRATHL_ERROR_INVALID_DAY =           -107)
      INTEGER BRATHL_ERROR_INVALID_HOUR
      PARAMETER(BRATHL_ERROR_INVALID_HOUR =          -108)
      INTEGER BRATHL_ERROR_INVALID_MINUTE
      PARAMETER(BRATHL_ERROR_INVALID_MINUTE =        -109)
      INTEGER BRATHL_ERROR_INVALID_SECOND
      PARAMETER(BRATHL_ERROR_INVALID_SECOND =        -110)
      INTEGER BRATHL_ERROR_INVALID_MUSECOND
      PARAMETER(BRATHL_ERROR_INVALID_MUSECOND =      -111)
      INTEGER BRATHL_ERROR_INVALID_DATE_NEGATIVE
      PARAMETER(BRATHL_ERROR_INVALID_DATE_NEGATIVE = -112)

      INTEGER REF19500101
      PARAMETER(REF19500101 =                        0)
      INTEGER REF19580101
      PARAMETER(REF19580101 =                        1)
      INTEGER REF19900101
      PARAMETER(REF19900101 =                        2)
      INTEGER REF20000101
      PARAMETER(REF20000101 =                        3)
      INTEGER REFUSER1
      PARAMETER(REFUSER1 =                           4)
      INTEGER REFUSER2
      PARAMETER(REFUSER2 =                           5)


      INTEGER TOPEX
      PARAMETER(TOPEX =                              0)
      INTEGER JASON1
      PARAMETER(JASON1 =                             1)
      INTEGER ERS2
      PARAMETER(ERS2 =                               2)
      INTEGER ENVISAT
      PARAMETER(ENVISAT =                            3)
      INTEGER ERS1_A
      PARAMETER(ERS1_A =                             4)
      INTEGER ERS1_B
      PARAMETER(ERS1_B =                             5)
      INTEGER GFO
      PARAMETER(GFO =                                6)

C
C Functions in alphabetical order
C
      INTEGER brathlf_Cycle2YMDHMSM
      INTEGER brathlf_DayOfYear
      INTEGER brathlf_DSM2Seconds
      INTEGER brathlf_DSM2YMDHMSM
      INTEGER brathlf_DiffDSM
      INTEGER brathlf_DiffJulian
      INTEGER brathlf_DiffYMDHMSM
      INTEGER brathlf_GetErrno
      INTEGER brathlf_Julian2DSM
      INTEGER brathlf_Julian2Seconds
      INTEGER brathlf_Julian2YMDHMSM
      INTEGER brathlf_NowYMDHMSM
      INTEGER brathlf_ReadData
      INTEGER brathlf_Seconds2DSM
      INTEGER brathlf_Seconds2Julian
      INTEGER brathlf_Seconds2YMDHMSM
      INTEGER brathlf_YMDHMSM2Cycle
      INTEGER brathlf_YMDHMSM2DSM
      INTEGER brathlf_YMDHMSM2Julian
      INTEGER brathlf_YMDHMSM2Seconds


