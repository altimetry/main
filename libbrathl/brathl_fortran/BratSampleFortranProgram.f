	PROGRAM P
	IMPLICIT NONE
	CHARACTER*(100) NAMES(10)
	CHARACTER*(10)	Record
	CHARACTER*(120) Selection
	CHARACTER*(200) Expressions(20)
	CHARACTER*(20)	Units(20)
	REAL*8		Result(1000,20)
	LOGICAL*4	Ignore
	LOGICAL*4	Statistics
	REAL*8		Default

	INTEGER*4	NbValues
	INTEGER*4	NbResults
	INTEGER*4	ReturnCode


	INCLUDE "brathlf.inc"

	NAMES(1)	= 'JA1_GDR_2PaP124_001.CNES'
	NAMES(2)	= 'JA1_GDR_2PaP124_002.CNES'
	NAMES(3)	= 'JA1_GDR_2PaP124_003.CNES'
	Record		= 'data'
	Selection	= 'latitude > 20'
	Expressions(1)	= 'latitude + longitude'
	Units(1)	= 'radians'
	Expressions(2)	= 'swh_ku'
	Units(2)	= 'm'
	NbValues	= 1000
	NbResults	= -1
        Ignore		= .false.
	Statistics	= .false.
	Default		= 18446744073709551616.0

	ReturnCode	= brathlf_ReadData(3,
     $					   NAMES,
     $					   Record,
     $					   Selection,
     $					   2,
     $					   Expressions,
     $					   Units,
     $					   Result,
     $					   NbValues,
     $					   NbResults,
     $					   Ignore,
     $					   Statistics,
     $					   Default)
	print *, NbResults
	print *, ReturnCode
	END
