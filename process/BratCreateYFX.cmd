
FILE = JA1_GDR_2PaP124_001.CNES
#FILE = JA1_GDR_2PaP124_002.CNES
#FILE = JA1_GDR_2PaP124_003.CNES
RECORD = data
#SELECT = Clip1
#SELECT = Clip2
VERBOSE = 1

X	= latitude
X_TYPE	= latitude
X_NAME	= XXX
X_UNIT	= DV
X_TITLE	= Le titre de l'axe des X

FIELD		= time_day
FIELD_TYPE	= T
FIELD_NAME	= MaDonnee
FIELD_UNIT	= DV
FIELD_TITLE	= Le petit nom de ma donnee

FIELD		= longitude
FIELD_TYPE	= longitude
FIELD_NAME	= MaLon
FIELD_UNIT	= dv
FIELD_TITLE	= Longitude

OUTPUT		= xx.nc
OUTPUT_TITLE	= Mon titre qui me plait bien

