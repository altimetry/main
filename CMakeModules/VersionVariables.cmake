


set( VERSION_FILE "${${PROJECT_NAME}_SOURCE_DIR}/${INSTALLERS_SUB_DIR}/version.txt" )	# no underscore, is not temporary, ships with product sources
set( BETA_VERSION_FILE "${${PROJECT_NAME}_SOURCE_DIR}/_beta" )							# no file type, it's only a tag
set( RC_VERSION_FILE "${${PROJECT_NAME}_SOURCE_DIR}/_rc.txt" )							# idem

set( VERSION_PATTERN ^[0-9]+$ )


if( EXISTS ${VERSION_FILE} )

	file( READ ${VERSION_FILE} VERSION_FILE_CONTENT LIMIT 128 )

	string( STRIP ${VERSION_FILE_CONTENT} VERSION_FILE_CONTENT )
	string( REPLACE "." ";" VERSION_FILE_LIST ${VERSION_FILE_CONTENT} )

	list( LENGTH VERSION_FILE_LIST len )

	# message( STATUS "string = ${VERSION_FILE_CONTENT}" )
	# message( STATUS "list = ${VERSION_FILE_LIST}" )
	# message( STATUS "len = ${len}" )

	if ( ${len} EQUAL 3 )
		list( GET VERSION_FILE_LIST 0 MAJOR_VERSION_READ )
		list( GET VERSION_FILE_LIST 1 MINOR_VERSION_READ )
		list( GET VERSION_FILE_LIST 2 PATCH_VERSION_READ )
	endif()

	if ( ( ${MAJOR_VERSION_READ} MATCHES ${VERSION_PATTERN} ) AND ( ${MINOR_VERSION_READ} MATCHES ${VERSION_PATTERN} ) AND ( ${PATCH_VERSION_READ} MATCHES ${VERSION_PATTERN} ) )
		set( MAJOR_VERSION ${MAJOR_VERSION_READ} )
		set( MINOR_VERSION ${MINOR_VERSION_READ} )
		set( PATCH_VERSION ${PATCH_VERSION_READ} )
	else()
		message( FATAL_ERROR "${VERSION_FILE} must contain only a version, with format 'M.m.p', where 'M', 'm' and 'p' are numbers." )
	endif()

endif()


if( EXISTS ${BETA_VERSION_FILE} )

	set( BETA_VERSION TRUE )	# any value, just create the variable

endif()


if( EXISTS ${RC_VERSION_FILE} )

	file( READ ${RC_VERSION_FILE} RC_VERSION_FILE_CONTENT LIMIT 32 )

	string( STRIP ${RC_VERSION_FILE_CONTENT} RC_VERSION_FILE_CONTENT )

	if ( ${RC_VERSION_FILE_CONTENT} MATCHES ${VERSION_PATTERN} )
		set( RC_VERSION ${RC_VERSION_FILE_CONTENT} )
	else()
		message( FATAL_ERROR "${RC_VERSION_FILE} must contain only a number." )
	endif()

endif()


# It is assumed that, if any of the following version values is defined
# in the version file or in the command line, the purpose is to assign
# (or override) macros in Version.h

IF (NOT DEFINED MAJOR_VERSION)
	SET(MAJOR_VERSION 1)
ELSE ()
	ADD_DEFINITIONS(-DMAJOR_VERSION=${MAJOR_VERSION})
ENDIF ()

IF (NOT DEFINED MINOR_VERSION)
	SET(MINOR_VERSION 0)
ELSE ()
	ADD_DEFINITIONS(-DMINOR_VERSION=${MINOR_VERSION})
ENDIF ()

IF (NOT DEFINED PATCH_VERSION)
	SET(PATCH_VERSION 0)
ELSE ()
	ADD_DEFINITIONS(-DPATCH_VERSION=${PATCH_VERSION})
ENDIF ()

# Beta and RC version suffixes are always defined externally
# and used by Version.h

IF (DEFINED BETA_VERSION)
	SET(BETA_VERSION_SUFFIX -beta)
	ADD_DEFINITIONS(-DBETA_VERSION)
ENDIF ()

IF (DEFINED RC_VERSION)
	SET(RC_VERSION_SUFFIX -rc${RC_VERSION})
	ADD_DEFINITIONS(-DRC_VERSION=${RC_VERSION})
ENDIF ()
