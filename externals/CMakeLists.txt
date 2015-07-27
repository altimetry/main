# CMakeLists.txt - CMake configuration file for the external
# packages needed by BRAT
#

# TODO: comment this must be revised to include the 
# option of not specifying a library version

# function UNPACK_EXTERNAL_PACKAGE(PACKAGE_BASENAME PACKAGE_VERSION)
#
# Usage: UNPACK_EXTERNAL_PACKAGE(foo x.y.z)
#
# This function will take the following actions:
#
# - It will set the variable PACKAGE_SOURCE *in the parent scope*
# (i.e. available to the caller) to the directory
# externals/foo.brat/
#
# - It will set the variable PACKAGE_TARGET in the parent scope
# to the directory externals/foo/
#
# - It will place the contents of archive
# ${PACKAGE_SOURCE}/foo-x.y.z.tar.gz into ${PACKAGE_TARGET}.
#
# This function relies on the following assumptions:
# ${PACKAGE_SOURCE} exists, and foo-x.y.z.tar.gz will unpack to a
# directory named foo-x.y.z (i.e. with version number attached)
#
# Typical usage (see this file for examples) is to call
# UNPACK_EXTERNAL_PACKAGE on an external package and then:
#
# - Copy any BRAT-specific files (including CMakeLists.txr!) from
# ${PACKAGE_SOURCE} to ${PACKAGE_TARGET} using "cmake -E
# copy_if_different"
#
# - Call to include_directories() where necessary
#


####################################################
#		UNPACK_EXTERNAL_PACKAGE function
####################################################


function(UNPACK_EXTERNAL_PACKAGE PACKAGE_BASENAME PACKAGE_VERSION)

  set(EXT_PACKAGE_NAME ${PACKAGE_BASENAME})
  set(EXT_PACKAGE_VERSION ${PACKAGE_VERSION})

  if ("${EXT_PACKAGE_VERSION}" STREQUAL "")
	set(EXT_PACKAGE_FULLNAME ${EXT_PACKAGE_NAME})
  else("${EXT_PACKAGE_VERSION}" STREQUAL "")
	set(EXT_PACKAGE_FULLNAME ${EXT_PACKAGE_NAME}-${EXT_PACKAGE_VERSION})
  endif("${EXT_PACKAGE_VERSION}" STREQUAL "")


  set(EXT_PACKAGE_DIR ${EXT_PACKAGE_NAME}.brat)
  set(EXT_PACKAGE_ARCHIVE "${CMAKE_CURRENT_SOURCE_DIR}/${EXT_PACKAGE_DIR}/${EXT_PACKAGE_FULLNAME}.tar.gz")

  if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${EXT_PACKAGE_NAME})
    message(STATUS "Unpacking the external package ${EXT_PACKAGE_ARCHIVE} ...")
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xfz ${EXT_PACKAGE_ARCHIVE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      )
    if (NOT "${EXT_PACKAGE_VERSION}" STREQUAL "")
		execute_process(
		  COMMAND ${CMAKE_COMMAND} -E copy_directory ${EXT_PACKAGE_FULLNAME} ${EXT_PACKAGE_NAME}
		  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		  )
		execute_process(
		  COMMAND ${CMAKE_COMMAND} -E remove_directory ${EXT_PACKAGE_FULLNAME}
		  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		  )
    endif(NOT "${EXT_PACKAGE_VERSION}" STREQUAL "")
  endif(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${EXT_PACKAGE_NAME})

  set(PACKAGE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/${EXT_PACKAGE_DIR} PARENT_SCOPE)
  set(PACKAGE_TARGET ${CMAKE_CURRENT_SOURCE_DIR}/${EXT_PACKAGE_NAME} PARENT_SCOPE)

endfunction(UNPACK_EXTERNAL_PACKAGE)



####################################################
#		Variables for Externals Library Names
####################################################


IF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )

    IF (WIN32)
        set(ZLIB_LIB_PATH_END "Debug/vtkzlibd.lib")
        set(CURL_LIB_PATH_END "Debug/libcurld_imp.lib")
        set(SZIP_LIB_PATH_END "Debug/libszip_Dd.lib")
        set(HDF5_LIB_PATH_END "Debug/libhdf5_Dd.lib")
        set(HDF5_HL_LIB_PATH_END "Debug/libhdf5_hl_Dd.lib")
    ELSE (WIN32)
        set(ZLIB_LIB_PATH_END "libvtkzlib.a")
        # use system "libcurl.a"
        set(CURL_LIB_PATH_END "libcurl.so")
        set(SZIP_LIB_PATH_END "libszip_debug.a")
        set(HDF5_LIB_PATH_END "libhdf5_debug.a")
        set(HDF5_HL_LIB_PATH_END "libhdf5_hl_debug.a")
    ENDIF (WIN32)

ELSE ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )

    IF (WIN32)
        set(ZLIB_LIB_PATH_END "Release/vtkzlib.lib")
        set(CURL_LIB_PATH_END "Release/libcurl_imp.lib")
        set(SZIP_LIB_PATH_END "Release/libszip.lib")
        set(HDF5_LIB_PATH_END "Release/libhdf5.lib")
        set(HDF5_HL_LIB_PATH_END "Release/libhdf5_hl.lib")
    ELSE (WIN32)
        set(ZLIB_LIB_PATH_END "libvtkzlib.a")
        # use system "libcurl.a"
        set(CURL_LIB_PATH_END "libcurl.so")
        set(SZIP_LIB_PATH_END "libszip.a")
        set(HDF5_LIB_PATH_END "libhdf5.a")
        set(HDF5_HL_LIB_PATH_END "libhdf5_hl.a")
    ENDIF (WIN32)

ENDIF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )



####################################################
#				zlib (Using vtkzlib)
####################################################

# ZLib
#
#UNPACK_EXTERNAL_PACKAGE(ZLib "")
#add_subdirectory(${PACKAGE_TARGET})

set(ZLIB_FOUND ON CACHE BOOL "" FORCE)
#set(ZLIB_USE_EXTERNAL ON CACHE BOOL "" FORCE)
	
IF ("${ZLIB_LIBRARIES}" STREQUAL "")	# set by main CMakeList.txt

        set(ZLIB_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/../vtkzlib/lib/${ZLIB_LIB_PATH_END}" CACHE INTERNAL "")		# used by netcdf4
	set(ZLIB_LIBRARIES "${ZLIB_LIBRARY}" CACHE INTERNAL "")
        set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../vtkzlib" CACHE INTERNAL "")							# used by netcdf4
	set(H5_ZLIB_HEADER "${ZLIB_INCLUDE_DIR}/zlib.h" CACHE INTERNAL "")
	
	message(STATUS "Setting ZLIB_LIBRARIES to " ${ZLIB_LIBRARIES})
	
ELSE ("${ZLIB_LIBRARIES}" STREQUAL "")

	message(STATUS "ZLib found for BRAT: " ${ZLIB_LIBRARIES})
	message(STATUS "ZLIB_INCLUDE_DIR for BRAT: " ${ZLIB_INCLUDE_DIR})
	
	set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIR} CACHE INTERNAL "")			# used by netcdf4
	set(ZLIB_LIBRARIES ${ZLIB_LIBRARIES} CACHE INTERNAL "")				# used by netcdf4
	set(ZLIB_LIBRARY ${ZLIB_LIBRARIES} CACHE INTERNAL "")				# used by netcdf4
	set(H5_ZLIB_HEADER "${ZLIB_INCLUDE_DIR}/zlib.h" CACHE INTERNAL "")
	
ENDIF ("${ZLIB_LIBRARIES}" STREQUAL "")

message(STATUS "ZLib found for BRAT: " ${ZLIB_LIBRARIES})
message(STATUS "ZLIB_INCLUDE_DIR for BRAT: " ${ZLIB_INCLUDE_DIR})
	
#set(ZLIB_LIBRARY "${ZLIB_LIBRARIES}" CACHE BOOL "" FORCE)				# used by netcdf4, set by BRAT 3.1.0, without Debug subdir...


####################################################
#					curl
####################################################

IF (WIN32)

    UNPACK_EXTERNAL_PACKAGE(curl 7.43.0)

    set(BUILD_CURL_TESTS OFF CACHE BOOL "Set to ON to build cURL tests." FORCE)
    set(ENABLE_ARES OFF CACHE BOOL "Set to ON to enable c-ares support" FORCE)
    set(ENABLE_THREADED_RESOLVER OFF CACHE BOOL "Set to ON to enable POSIX threaded DNS lookup" FORCE)
    set(HTTP_ONLY OFF CACHE BOOL "disables all protocols except HTTP (This overrides all CURL_DISABLE_* options)" FORCE)
    set(CMAKE_USE_LIBSSH2 OFF CACHE BOOL "" FORCE)
    #
    # cannot be static: see curl CMakeLists.txt
    # set(CURL_STATICLIB ON CACHE BOOL "Set to ON to build libcurl with static linking." FORCE)
    #
    set(CURL_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/curl/lib/lib/${CURL_LIB_PATH_END}" CACHE INTERNAL "")	# used by netcfd4
    set(CURL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/curl/include" CACHE INTERNAL "")

    add_subdirectory(${PACKAGE_TARGET})

ELSE (WIN32)    # this is a hack to be able to work 32 bit on 64 bit systems; TODO transform in command line option

    IF ("${BRAT_TARGET_PROCESSOR}" STREQUAL "i386" AND "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" AND "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
        set(CURL_INCLUDE_DIR "/usr/inlude/i386-linux-gnu" CACHE INTERNAL "")
        include_directories(${CURL_INCLUDE_DIR})
        # message(STATUS "${CMAKE_C_FLAGS}")
        # message(STATUS "${CMAKE_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "-I${CURL_INCLUDE_DIR} ${CMAKE_C_FLAGS}" CACHE INTERNAL "")
        set(CMAKE_CXX_FLAGS "-I${CURL_INCLUDE_DIR} ${CMAKE_CXX_FLAGS}" CACHE INTERNAL "")
        # message(STATUS "${CMAKE_C_FLAGS}")
        # message(FATAL_ERROR "${CMAKE_CXX_FLAGS}")
    ENDIF ()

ENDIF (WIN32)


####################################################
#					szip
####################################################

UNPACK_EXTERNAL_PACKAGE(SZip "")

set(SZIP_ENABLE_ENCODING ON CACHE BOOL "Enable SZIP Encoding" FORCE)

IF (WIN32)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Shared Libraries" FORCE)
ELSE (WIN32)
    set(SZIP_BUILD_SHARED_LIBS OFF CACHE BOOL "Build Shared Libraries" FORCE)    # linux
ENDIF (WIN32)

add_subdirectory(${PACKAGE_TARGET})



####################################################
#					hdf5
####################################################

UNPACK_EXTERNAL_PACKAGE(hdf5 1.8.15-patch1)

set(HDF5_ENABLE_Z_LIB_SUPPORT ON CACHE BOOL "" FORCE)
set(HDF5_ENABLE_SZIP_SUPPORT ON CACHE BOOL "" FORCE)
set(HDF5_ENABLE_SZIP_ENCODING ON CACHE BOOL "" FORCE)

set(SZIP_FOUND ON CACHE BOOL "" FORCE)
IF (WIN32)
    set(SZIP_USE_EXTERNAL ON CACHE BOOL "" FORCE)		# ON avoids SZIP_EXPORT_LIBRARIES in hdf5 dependencies list in the cache
ELSE (WIN32)
    set(SZIP_USE_EXTERNAL ON CACHE BOOL "" FORCE)		# In linux fools hdf5 about finding szip
ENDIF (WIN32)

set(SZIP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SZip/src" CACHE INTERNAL "")		# used by netcdf-c CMakeLists.txt
set(SZIP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/SZip/src" CACHE INTERNAL "")
set(SZIP_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/SZip/src/lib/${SZIP_LIB_PATH_END}" CACHE INTERNAL "")

include_directories(${SZIP_INCLUDE_DIRS})
if (NOT WIN32)
    set(SZIP_DIR "${CMAKE_CURRENT_BINARY_DIR}/SZip/CMakeFiles" CACHE INTERNAL "")   # linux
endif (NOT WIN32)

#set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Shared Libraries" FORCE)
set(HDF5_ALLOW_EXTERNAL_SUPPORT "NO" CACHE INTERNAL "")

add_subdirectory(${PACKAGE_TARGET})



####################################################
#					netcdf-c-4
####################################################

UNPACK_EXTERNAL_PACKAGE(netcdf-c 4.3.3.1)

set(USE_SZIP ON CACHE BOOL "Use SZip" FORCE)
# set(USE_HDF5 ON CACHE BOOL "Use HDF5." FORCE)
set(FIND_SZIP_DEBUG ON CACHE BOOL "" FORCE) # linux

set(HDF5_LIB "${CMAKE_CURRENT_BINARY_DIR}/hdf5/src/lib/${HDF5_LIB_PATH_END}" CACHE INTERNAL "")
set(HDF5_HL_LIB "${CMAKE_CURRENT_BINARY_DIR}/hdf5/hl/src/lib/${HDF5_HL_LIB_PATH_END}" CACHE INTERNAL "")
set(HDF5_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/hdf5/src" CACHE INTERNAL "")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/hdf5/hl/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/hdf5)

# trick to help cmake of netcdf finding modules...
#
set(SAVE_CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR} CACHE INTERNAL "")
set(SAVE_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} CACHE INTERNAL "")
set(CMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/netcdf-c CACHE INTERNAL "")

# Ugly trick to get szip linked...
# Using NC_EXTRA_DEPS variable requires that the dependency exists already, but we are building it now...
#
set(ZLIB_LIBRARY ${ZLIB_LIBRARIES};${SZIP_LIBRARIES} CACHE INTERNAL "")     # used by netcdf4

IF (NOT WIN32)

    # set(ENABLE_DAP OFF CACHE BOOL "Use DAP" FORCE)
    set(USE_LIBDL OFF CACHE BOOL "")
    set(ENABLE_DYNAMIC_LOADING OFF CACHE BOOL "" FORCE) # THIS IS CRITICAL

    # Ugly trick to solve netcdf bug of adding the boolean HAVE_LIBM to the library lists, making the linker
    # look for a library 1.so, which of course fails
    #
    IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(NC_EXTRA_DEPS "-lcurl -lssl -lsasl2 -lldap -lz" CACHE INTERNAL "")
        set(HAVE_LIBM curl;m;z;ssl;dl CACHE STRING "" FORCE)
    ELSE ()
        set(NC_EXTRA_DEPS "-lssl -lidn -lssh2 -lsasl2 -lcrypt -lrtmp -lldap -lz" CACHE INTERNAL "")
        set(HAVE_LIBM curl;m;idn;z;ssl;crypt;dl CACHE STRING "" FORCE)
    ENDIF ()

    set(SZIP_LIBRARY ${SZIP_LIBRARIES} CACHE INTERNAL "")	    # apparently needed only when cod was set to use hdf5

ENDIF (NOT WIN32)

add_subdirectory(${PACKAGE_TARGET})



set(CMAKE_SOURCE_DIR ${SAVE_CMAKE_SOURCE_DIR} CACHE INTERNAL "")
set(CMAKE_MODULE_PATH ${SAVE_CMAKE_MODULE_PATH} CACHE INTERNAL "")

set(NETCDF_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/netcdf-c/include
  ${CMAKE_CURRENT_SOURCE_DIR}/hdf5/hl/src
  ${CMAKE_CURRENT_BINARY_DIR}/hdf5
  CACHE INTERNAL "Include directories for the netcdf library")

include_directories(
#  ${CMAKE_CURRENT_BINARY_DIR}/netcdf-c	this misleads coda, that includes netcdf's and not coda's config.h
  ${NETCDF_INCLUDE_DIRS})

set(NETCDF_INCLUDE_DIRS ${NETCDF_INCLUDE_DIRS} PARENT_SCOPE)




####################################################
#					geotiff
####################################################

UNPACK_EXTERNAL_PACKAGE(libgeotiff 1.2.5)
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/CMakeLists.txt.in ${PACKAGE_TARGET}/CMakeLists.txt
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/geo_config.h.cmake.in ${PACKAGE_TARGET}/geo_config.h.cmake.in
  )
add_subdirectory(${PACKAGE_TARGET})




####################################################
#					udunits
####################################################

UNPACK_EXTERNAL_PACKAGE(udunits 1.12.4)
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/CMakeLists.txt.in ${PACKAGE_TARGET}/CMakeLists.txt
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/utlib.c   ${PACKAGE_TARGET}/src/lib/utlib.c
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/limits.h.in   ${PACKAGE_TARGET}/src/port/misc/limits.h.in
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/search.h.in   ${PACKAGE_TARGET}/src/port/misc/search.h.in
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/stdarg.h.in   ${PACKAGE_TARGET}/src/port/misc/stdarg.h.in
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/stddef.h.in   ${PACKAGE_TARGET}/src/port/misc/stddef.h.in
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/stdlib.h.in   ${PACKAGE_TARGET}/src/port/misc/stdlib.h.in
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/string.h.in   ${PACKAGE_TARGET}/src/port/misc/string.h.in
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/tdelete.c     ${PACKAGE_TARGET}/src/port/misc/tdelete.c
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/tfind.c       ${PACKAGE_TARGET}/src/port/misc/tfind.c
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/time.h.in     ${PACKAGE_TARGET}/src/port/misc/time.h.in
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/tsearch.c     ${PACKAGE_TARGET}/src/port/misc/tsearch.c
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/twalk.c       ${PACKAGE_TARGET}/src/port/misc/twalk.c
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/udposix.h.in     ${PACKAGE_TARGET}/src/port/misc/udposix.h.in
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

IF (WIN32)
  set(CMAKE_C_FLAGS "/D_POSIX_PATH_MAX=_MAX_PATH ${CMAKE_C_FLAGS}" CACHE INTERNAL "")
  set(CMAKE_CXX_FLAGS "/D_POSIX_PATH_MAX=_MAX_PATH ${CMAKE_CXX_FLAGS}" CACHE INTERNAL "")
ENDIF (WIN32)

add_subdirectory(${PACKAGE_TARGET})




####################################################
#					proj
####################################################

UNPACK_EXTERNAL_PACKAGE(proj 4.4.9)
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/CMakeLists.txt.in ${PACKAGE_TARGET}/CMakeLists.txt
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/proj_config.h.in ${PACKAGE_TARGET}/src/proj_config.h.in
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
add_subdirectory(${PACKAGE_TARGET})




####################################################
#					coda
####################################################

# This is the only way I can discover of making the CODA options
# 'inherit' the corresponding BRAT options.
#
UNPACK_EXTERNAL_PACKAGE(coda 2.13)
set(HDF5_INCLUDE "${HDF5_INCLUDE_DIR}" CACHE INTERNAL "")
set(HDF5_INCLUDE_DIRS "${HDF5_INCLUDE_DIR}" CACHE INTERNAL "")
set(HDF5_LIBRARIES "${HDF5_LIB}" CACHE INTERNAL "")
#set(HDF5_LIB "${HDF5_LIB}" CACHE INTERNAL "")
set(HDF5_HL_LIBRARIES "${HDF5_HL_LIB}" CACHE INTERNAL "")

# ${HDF5_LIBRARIES}
set(LIBM_LIBRARY "${HAVE_LIBM}" CACHE INTERNAL "" FORCE)


#set(HDF5_LIB "${CMAKE_CURRENT_BINARY_DIR}/hdf5/src/lib/${HDF5_LIB_PATH_END}" CACHE INTERNAL "")
#set(HDF5_HL_LIB "${CMAKE_CURRENT_BINARY_DIR}/hdf5/hl/src/lib/${HDF5_HL_LIB_PATH_END}" CACHE INTERNAL "")

set(ZLIB_INCLUDE "${ZLIB_INCLUDE_DIR}/zlib.h" CACHE INTERNAL "")
set(ZLIB_LIBRARY ${ZLIB_LIBRARIES} CACHE INTERNAL "")     # used by netcdf4


#find_library(ZLIB_LIBRARY
#  NAMES ${ZLIB_NAMES}
#  PATHS ${ZLIB_LIB} ENV ZLIB_LIB)
#if (ZLIB_LIBRARY)
#  check_library_exists(${ZLIB_LIBRARY} deflate "" HAVE_ZLIB)
#  if (HAVE_ZLIB)
#    set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
#  endif(HAVE_ZLIB)
#endif (ZLIB_LIBRARY)


set(CODA_WITH_HDF5 ON CACHE INTERNAL "")
set(HDF5_FOUND ON CACHE INTERNAL "")
set(HAVE_HDF5 1 CACHE INTERNAL "")
set(HAVE_HDF5_H 1 CACHE INTERNAL "")


message(STATUS "HDF5_INCLUDE==" ${HDF5_INCLUDE} )
message(STATUS "HDF5_INCLUDE_DIR==" ${HDF5_INCLUDE_DIR} )
message(STATUS "HDF5_INCLUDE_DIRS==" ${HDF5_INCLUDE_DIRS} )
message(STATUS "HDF5_LIBRARIES==" ${HDF5_LIBRARIES} )
message(STATUS "HDF5_HL_LIBRARIES==" ${HDF5_HL_LIBRARIES} )
message(STATUS "HDF5_HL_LIBRARIES==" ${HDF5_HL_LIBRARIES} )
message(STATUS "HDF5_LIB==" ${HDF5_LIB} )
message(STATUS "ZLIB_INCLUDE==" ${ZLIB_INCLUDE} )
message(STATUS "ZLIB_LIBRARY==" ${ZLIB_LIBRARY} )
message(STATUS "LIBM_LIBRARY==" ${LIBM_LIBRARY} )
message(STATUS "HAVE_LIBM==" ${HAVE_LIBM} )


#HDF5_INCLUDE_DIRS - Location of the hdf5 includes
#HDF5_INCLUDE_DIR - Location of the hdf5 includes (deprecated)
#   HDF5_DEFINITIONS - Required compiler definitions for HDF5
#   HDF5_C_LIBRARIES - Required libraries for the HDF5 C bindings.
#   HDF5_CXX_LIBRARIES - Required libraries for the HDF5 C++ bindings
#   HDF5_Fortran_LIBRARIES - Required libraries for the HDF5 Fortran bindings
#HDF5_HL_LIBRARIES - Required libraries for the HDF5 high level API
#   HDF5_Fortran_HL_LIBRARIES - Required libraries for the high level Fortran
#                               bindings.
#HDF5_LIBRARIES - Required libraries for all requested bindings
#   HDF5_FOUND - true if HDF5 was found on the system
#HDF5_LIBRARY_DIRS - the full set of library directories
#   HDF5_IS_PARALLEL - Whether or not HDF5 was found with parallel IO support
#   HDF5_C_COMPILER_EXECUTABLE - the path to the HDF5 C wrapper compiler
#   HDF5_CXX_COMPILER_EXECUTABLE - the path to the HDF5 C++ wrapper compiler
#   HDF5_Fortran_COMPILER_EXECUTABLE - the path to the HDF5 Fortran wrapper compiler
#   HDF5_DIFF_EXECUTABLE - the path to the HDF5 dataset comparison tool


set(CODA_BUILD_IDL ${BRATHL_BUILD_IDL} CACHE BOOL "build CODA IDL Interface" FORCE)
set(CODA_BUILD_MATLAB ${BRATHL_BUILD_MATLAB} CACHE BOOL "build CODA MATLAB Interface" FORCE)
set(CODA_DEFINITION_IDL \"../data\" CACHE STRING "location of CODA definition files for IDL interface" FORCE)
set(CODA_DEFINITION_MATLAB \"../data\" CACHE STRING "location of CODA definition files for MATLAB interface" FORCE)
add_subdirectory(${PACKAGE_TARGET})



####################################################
#			clean_externals target
####################################################

# Remove all the directories generated by the
# UNPACK_EXTERNAL_PACKAGE calls made in this file.
#
add_custom_target(clean_externals
  COMMENT "Removing the unpacked external packages from ${CMAKE_CURRENT_SOURCE_DIR}"
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/coda
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/curl
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/SZip
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/hdf5
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/netcdf-c
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/libgeotiff
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/udunits
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/proj
)
