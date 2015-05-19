#!/usr/bin/env BratCreateZFXY

FILE = JA1_GDR_2PaP124_001.CNES
FILE = JA1_GDR_2PaP124_002.CNES
FILE = JA1_GDR_2PaP124_003.CNES
RECORD = data
#SELECT = Clip1
#SELECT = Clip2
VERBOSE = 1

X		= longitude
X_TYPE		= longitude
X_NAME		= Longitude
X_UNIT		= DV
X_TITLE		= Le titre de l'axe des X
X_MIN		= DV
X_MAX		= DV
X_INTERVALS	= 180

Y		= latitude
Y_TYPE		= latitude
Y_NAME		= Latitude
Y_UNIT		= DV
Y_TITLE		= Le titre de l'axe des Y
Y_MIN		= DV
Y_MAX		= DV
Y_INTERVALS	= 2

FIELD		= time_day
FIELD_TYPE	= X
FIELD_NAME	= MaDonnee
FIELD_UNIT	= degrees_north
FIELD_TITLE	= Le petit nom de ma donnee

FIELD		= longitude
FIELD_TYPE	= data
FIELD_NAME	= MaLon
FIELD_UNIT	= degrees_east
FIELD_TITLE	= Longitude

OUTPUT		= xx.nc
OUTPUT_TITLE	= Mon titre qui me plait bien

