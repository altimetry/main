#
# Utilities macros defined for CMake
#

INCLUDE(FindPerl)

MACRO (DUMP_VARIABLES)
  GET_CMAKE_PROPERTY(DUMPV_VARS VARIABLES)
  FILE(WRITE  "${PROJECT_BINARY_DIR}/CMakeTmp/DumpVar.log" "Dumping CMAKE variables:\n")
  FOREACH (VARNAME ${DUMPV_VARS})
    MESSAGE("${VARNAME}: ${${VARNAME}}")
    FILE(APPEND "${PROJECT_BINARY_DIR}/CMakeTmp/DumpVar.log" "${VARNAME}: ${${VARNAME}}\n")
  ENDFOREACH(VARNAME)
ENDMACRO (DUMP_VARIABLES)

MACRO (NOW RESULT)
  IF (PERL_FOUND)
    EXECUTE_PROCESS(COMMAND "${PERL_EXECUTABLE}" "-e" "@T=localtime; print sprintf \"%04d-%02d-%02d %02d:%02d:%02d\",$T[5]+1900,$T[4]+1,$T[3],$T[2],$T[1],$T[0]"
		    OUTPUT_VARIABLE ${RESULT})
  ELSE (PERL_FOUND)
    MESSAGE(SEND_ERROR "Perl needed for some operations")
  ENDIF (PERL_FOUND)
ENDMACRO (NOW)

MACRO (WIN32_POSSIBLE_DIR_LIST RESULT ROOT SUFFIX)
  IF (WIN32)

    IF ("${SUFFIX}" MATCHES ".+")
      SET(W32PDL_SUFFIX "/${SUFFIX}")
    ELSE ("${SUFFIX}" MATCHES ".+")
      SET(W32PDL_SUFFIX "")
    ENDIF ("${SUFFIX}" MATCHES ".+")

#   The new paths are added to the existing ones
    SET(W32PDL_PATH ${${RESULT}})

    IF ("${CMAKE_BUILD_TYPE}" MATCHES ".+")
      SET(W32PDL_LIST ${CMAKE_BUILD_TYPE} ${CMAKE_BUILD_TYPE}/lib "." "lib")
    ELSE ("${CMAKE_BUILD_TYPE}" MATCHES ".+")
      SET(W32PDL_LIST
			"."
			"lib"
			"release"
			"release/lib"
	)
    ENDIF ("${CMAKE_BUILD_TYPE}" MATCHES ".+")

    FOREACH (W32PDL ${W32PDL_LIST})
      SET(W32PDL_PATH ${W32PDL_PATH}
		${ROOT}/${W32PDL}${W32PDL_SUFFIX}
		${ROOT}/lib/${W32PDL}${W32PDL_SUFFIX}
		${ROOT}/win32/${W32PDL}${W32PDL_SUFFIX}
		${ROOT}/win32/lib/${W32PDL}${W32PDL_SUFFIX})
    ENDFOREACH (W32PDL)
    FILE(TO_CMAKE_PATH "${W32PDL_PATH}" ${RESULT})
  ENDIF (WIN32)
ENDMACRO (WIN32_POSSIBLE_DIR_LIST)

MACRO (DIR_GLOBING RESULT DIRS)
  IF (PERL_FOUND)
    EXECUTE_PROCESS(COMMAND ${PERL_EXECUTABLE} -e "
     sub globsp{
       my @Motifs = @_;
       s/\\s/\\\\$&/g foreach @Motifs;
       return map glob($_), @Motifs;
     }
     $R = join(\";\", globsp(split m/;/, \"${DIRS}\"));
     $R =~ s%\\\\%/%g;
     print $R"
			OUTPUT_VARIABLE ${RESULT})
  ELSE (PERL_FOUND)
    MESSAGE(FATAL_ERROR "Perl is needed for some operations")
  ENDIF (PERL_FOUND)
ENDMACRO (DIR_GLOBING)
