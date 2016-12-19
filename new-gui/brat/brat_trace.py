import os
 
# get current env vars
original_env = dict(os.environ)
 
#do stuff here
print ("\n", os.environ.get( 'QGIS_DEBUG' ) )
print ("\n", os.environ.get( 'QGIS_LOG_FILE' ) )

os.environ["QGIS_DEBUG"] = "1"
os.environ["QGIS_LOG_FILE"] = "brat_log.txt"
os.system( "brat" )

# clean os environment variables
# os.environ.clear()
# set previously saved variables (preserves)
os.environ.update(original_env)
# also make sure we haven't destroyed our development environment
# print ("\n", os.environ.get('QGIS_DEBUG'))
