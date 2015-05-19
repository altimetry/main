; BRATHL IDL interface examples

;-----------------------------------------------------------------
; Date diff example:

d1={BRATHL_DATEYMDHMSM}
d1.YEAR=2009
d1.MONTH=9
d1.DAY=3
d1.HOUR=12
d1.MINUTE=25
d1.SECOND=5
d1.MUSECOND=0

d2={BRATHL_DATEYMDHMSM}
d2.YEAR=2009
d2.MONTH=9
d2.DAY=3
d2.HOUR=12
d2.MINUTE=25
d2.SECOND=15
d2.MUSECOND=0

; Output should be: 10
diff = 0.0D
r =  BRATHL_DIFFYMDHMSM(d2, d1, diff)
print, "Date difference: ", diff


; -----------------------------------------------------------------
; CODA Product file ingestion example (adapt for your own file path)

print, "CODA version: ", coda_version()
; For a Windows system 
f = coda_open("E:\brat\data_sample\jason1\JA1_GDR_2PaP124_001.CNES")
; For a Linux system
; f = coda_open("/home/leo/brat-svn/datafiles/jason1/JA1_GDR_2PaP124_001.CNES")
print, "Result of coda_open(): ", f
r = coda_fetch(f)
print, "Struct resulting from coda_fetch():"
help, r, /struct
r = coda_close(f)


; -----------------------------------------------------------------
; Product file ingestion example (adapt for your own file paths) 

files=SINDGEN(3)
; For a Windows system
files[0] = "E:\brat\data_sample\jason1\JA1_GDR_2PaP124_001.CNES"
files[1] = "E:\brat\data_sample\jason1\JA1_GDR_2PaP124_002.CNES"
files[2] = "E:\brat\data_sample\jason1\JA1_GDR_2PaP124_003.CNES"
; For e.g a Linux system
; files[0] = "/home/leo/brat-svn/datafiles/jason1/JA1_GDR_2PaP124_001.CNES"
; files[1] = "/home/leo/brat-svn/datafiles/jason1/JA1_GDR_2PaP124_002.CNES"
; files[2] = "/home/leo/brat-svn/datafiles/jason1/JA1_GDR_2PaP124_003.CNES"

record="data"
selection="latitude > 20 && latitude < 30"
expr=SINDGEN(2)
expr[0]="latitude + longitude"
expr[1]="swh_ku"
units=SINDGEN(2)
units[0]="radians"
units[1]=""
dataResults=DINDGEN(2)
ignoreOutOfrange=0
statistics=0
defaultValue=0
r = BRATHL_READDATA(files, record, selection, expr, units, dataResults, ignoreOutOfrange, statistics, defaultValue)
print, "Return value from breathl_readdata():"
help, r, /struct
print, "Result struct from breathl_readdata():"
help, dataResults, /struct
end
