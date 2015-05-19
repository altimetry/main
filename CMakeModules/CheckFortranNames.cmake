#
# Check for the number of underscores added by fortran compiler
# May be 0, 1 or 2
#
# Will determine -DFORTRAN_NO_UNDERSCORE, -DFORTRAN_ONE_UNDERSCORE, -DFORTRAN_TWO_UNDERSCORES
# and -DFORTRAN_UNDERSCORES=n fortran defines flags according to the actual number
#
# FORTRAN_NAMES_DEFINITIONS - variable containing result as decribed above and
#		suitable for ADD_DEFINITIONS (i.e. a list)
#
# CHECK_FORTRAN_NAMES - Macro which returns FORTRAN_NAMES_DEFINITIONS but
#		as a single string suitable for SET_SOURCE_FILES_PROPERTIES
#		(semicolons replaced by spaces)
#
# If CMAKE_REQUIRED_FLAGS is set then those flags will be passed into the
# compile of the program likewise if CMAKE_REQUIRED_LIBRARIES is set then
# those libraries will be linked against the test program



ENABLE_LANGUAGE(Fortran)

IF (NOT DEFINED FORTRAN_NAMES_DEFINITIONS)
  SET(MACRO_CFN_FLAGS ${CMAKE_REQUIRED_FLAGS})
  IF(CMAKE_REQUIRED_LIBRARIES)
    SET(MACRO_CFN_LIBS "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
  ENDIF(CMAKE_REQUIRED_LIBRARIES)
  SET(MACRO_CFN_FILE    ${CMAKE_BINARY_DIR}/CMakeTmp/CheckFortranUnderScore.f)
  SET(MACRO_CFN_CONTENT "       PROGRAM P\n       call MY_FUNC\n       END\n")
  FILE(WRITE ${MACRO_CFN_FILE} ${MACRO_CFN_CONTENT})
# Don't care of result since it it always a link error, and that's what we
# want (we parse the linker error)
  TRY_COMPILE(FORTRAN_NAMES_DEFINITIONS
    ${CMAKE_BINARY_DIR} ${MACRO_CFN_FILE}
    CMAKE_FLAGS
    -DCOMPILE_DEFINITIONS:STRING=${MACRO_CFN_FLAGS}
    "${MACRO_CFN_LIBS}"
    OUTPUT_VARIABLE OUTPUT)

  MESSAGE(STATUS "Looking for fortran names case")
  STRING(REGEX MATCH "my_func_?_?" MACRO_CFN_NAME "${OUTPUT}")
  IF(MACRO_CFN_NAME STREQUAL "")
    STRING(REGEX MATCH "MY_FUNC_?_?" MACRO_CFN_NAME "${OUTPUT}")
    IF(MACRO_CFN_NAME STREQUAL "")
      FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
	"Determining case and number ofunderscores of fortran:\n"
	"${OUTPUT}\nFile ${MACRO_CFN_FILE}:\n"
	"${MACRO_CFN_CONTENT}\n")
      MESSAGE(FATAL_ERROR "Did not found how to know if fortran names are lowercase or uppercase")
    ELSE(MACRO_CFN_NAME STREQUAL "")
      MESSAGE(STATUS "Looking for fortran names case - uppercase")
      SET(MACRO_CFN_TMP "-DFORTRAN_UPPERCASE")
    ENDIF(MACRO_CFN_NAME STREQUAL "")
  ELSE(MACRO_CFN_NAME STREQUAL "")
    MESSAGE(STATUS "Looking for fortran names case - lowercase")
    SET(MACRO_CFN_TMP "-DFORTRAN_LOWERCASE")
  ENDIF(MACRO_CFN_NAME STREQUAL "")

  MESSAGE(STATUS "Looking for underscores appended to fortran names")
  IF (MACRO_CFN_NAME STREQUAL "my_func" OR MACRO_CFN_NAME STREQUAL "MY_FUNC")
    MESSAGE(STATUS "Looking for underscores appended to fortran names -- none")
    SET(MACRO_CFN_TMP ${MACRO_CFN_TMP} "-DFORTRAN_NO_UNDERSCORE" "-DFORTRAN_UNDERSCORES=0")
  ELSE (MACRO_CFN_NAME STREQUAL "my_func" OR MACRO_CFN_NAME STREQUAL "MY_FUNC")
    IF (MACRO_CFN_NAME STREQUAL "my_func_" OR MACRO_CFN_NAME STREQUAL "MY_FUNC_")
      MESSAGE(STATUS "Looking for underscores appended to fortran names -- 1")
      SET(MACRO_CFN_TMP ${MACRO_CFN_TMP} "-DFORTRAN_ONE_UNDERSCORE" "-DFORTRAN_UNDERSCORES=1")
    ELSE (MACRO_CFN_NAME STREQUAL "my_func_" OR MACRO_CFN_NAME STREQUAL "MY_FUNC_")
      IF (MACRO_CFN_NAME STREQUAL "my_func__" OR MACRO_CFN_NAME STREQUAL "MY_FUNC__")
	MESSAGE(STATUS "Looking for underscores appended to fortran names -- 2")
	SET(MACRO_CFN_TMP ${MACRO_CFN_TMP} "-DFORTRAN_TWO_UNDERSCORES" "-DFORTRAN_UNDERSCORES=2")
      ELSE (MACRO_CFN_NAME STREQUAL "my_func__" OR MACRO_CFN_NAME STREQUAL "MY_FUNC__")
	MESSAGE(FATAL_ERROR "CHECK_FORTRAN_NAMES macro error: this message should not be displayed (${MACRO_CFN_NAME})")
      ENDIF (MACRO_CFN_NAME STREQUAL "my_func__" OR MACRO_CFN_NAME STREQUAL "MY_FUNC__")
    ENDIF (MACRO_CFN_NAME STREQUAL "my_func_" OR MACRO_CFN_NAME STREQUAL "MY_FUNC_")
  ENDIF (MACRO_CFN_NAME STREQUAL "my_func" OR MACRO_CFN_NAME STREQUAL "MY_FUNC")
  SET(FORTRAN_NAMES_DEFINITIONS "${MACRO_CFN_TMP}" CACHE STRING "Cached parameters for C/Fortran interface" FORCE)
  MARK_AS_ADVANCED(FORTRAN_NAMES_DEFINITIONS)
ENDIF (NOT DEFINED FORTRAN_NAMES_DEFINITIONS)


MACRO(CHECK_FORTRAN_NAMES VARIABLE)
  IF("${VARIABLE}" MATCHES "^${VARIABLE}$")
    SET(MACRO_CFN_TMP "")
    FOREACH (MACRO_CFN_NAME ${FORTRAN_NAMES_DEFINITIONS})
      SET(MACRO_CFN_TMP "${MACRO_CFN_TMP} ${MACRO_CFN_NAME}")
    ENDFOREACH(MACRO_CFN_NAME)
    SET(${VARIABLE} ${MACRO_CFN_TMP})
  ENDIF("${VARIABLE}" MATCHES "^${VARIABLE}$")
ENDMACRO(CHECK_FORTRAN_NAMES)
