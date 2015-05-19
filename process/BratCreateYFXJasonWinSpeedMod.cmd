#!/usr/bin/env BratCreateYFXJasonWinSpeedMod.cmd

FILE  = ${SRC_DATA_DIR}/JA1_GDR_2PaP124_001.CNES
#FILE = ${SRC_DATA_DIR}/JA1_GDR_2PaP124_002.CNES
#FILE = ${SRC_DATA_DIR}/JA1_GDR_2PaP124_003.CNES

RECORD = data

VERBOSE = 1


X	= latitude
X_TYPE	= latitude
X_NAME	= XXX
X_UNIT	= DV
X_TITLE	= Latitude

FIELD		= range_ku
FIELD_TYPE	= Data
FIELD_NAME	= range_ku
FIELD_UNIT	= DV
FIELD_TITLE	= 1 Hz Ku band range 

FIELD		= wind_speed_model_u
FIELD_TYPE	= Data
FIELD_NAME	= wind_speed_model_u
FIELD_UNIT	= cm/s
FIELD_TITLE	= U component of the model wind vector 

FIELD		= wind_speed_model_v
FIELD_TYPE	= Data
FIELD_NAME	= wind_speed_model_v
FIELD_UNIT	= cm/s
FIELD_TITLE	= V component of the model wind vector 

FIELD		= wind_speed_alt
FIELD_TYPE	= Data
FIELD_NAME	= wind_speed_alt
FIELD_UNIT	= cm/s
FIELD_TITLE	= altimeter wind speed 

FIELD		= sqrt(sqr(wind_speed_model_u) + sqr(wind_speed_model_v))
FIELD_TYPE	= Data
FIELD_NAME	= wind_speed
FIELD_UNIT	= m/s
FIELD_TITLE	= wind speed modulus


OUTPUT		= ${BRATHL_DATA_DIR}/JasonYFXWinSpeedMod.nc
OUTPUT_TITLE	= Wind speed modulus\nmodel vs altimeter

