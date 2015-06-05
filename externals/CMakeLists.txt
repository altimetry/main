# CMakeLists.txt - CMake configuration file for the external
# packages needed by BRAT
#

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
function(UNPACK_EXTERNAL_PACKAGE PACKAGE_BASENAME PACKAGE_VERSION)

  set(EXT_PACKAGE_NAME ${PACKAGE_BASENAME})
  set(EXT_PACKAGE_VERSION ${PACKAGE_VERSION})

  set(EXT_PACKAGE_FULLNAME ${EXT_PACKAGE_NAME}-${EXT_PACKAGE_VERSION})
  set(EXT_PACKAGE_DIR ${EXT_PACKAGE_NAME}.brat)
  set(EXT_PACKAGE_ARCHIVE "${CMAKE_CURRENT_SOURCE_DIR}/${EXT_PACKAGE_DIR}/${EXT_PACKAGE_FULLNAME}.tar.gz")

  if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${EXT_PACKAGE_NAME})
    message(STATUS "Unpacking the external package ${EXT_PACKAGE_ARCHIVE} ...")
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xfz ${EXT_PACKAGE_ARCHIVE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      )
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E copy_directory ${EXT_PACKAGE_FULLNAME} ${EXT_PACKAGE_NAME}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      )
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E remove_directory ${EXT_PACKAGE_FULLNAME}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      )
  endif(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${EXT_PACKAGE_NAME})

  set(PACKAGE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/${EXT_PACKAGE_DIR} PARENT_SCOPE)
  set(PACKAGE_TARGET ${CMAKE_CURRENT_SOURCE_DIR}/${EXT_PACKAGE_NAME} PARENT_SCOPE)

endfunction(UNPACK_EXTERNAL_PACKAGE)


# libgeotiff
#
UNPACK_EXTERNAL_PACKAGE(libgeotiff 1.2.5)
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/CMakeLists.txt.in ${PACKAGE_TARGET}/CMakeLists.txt
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/geo_config.h.cmake.in ${PACKAGE_TARGET}/geo_config.h.cmake.in
  )
add_subdirectory(${PACKAGE_TARGET})


# netcdf
#
UNPACK_EXTERNAL_PACKAGE(netcdf 3.6.2)
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/CMakeLists.txt.in ${PACKAGE_TARGET}/CMakeLists.txt
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/config.h.cmake.in ${PACKAGE_TARGET}/config.h.cmake.in
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/getopt.c ${PACKAGE_TARGET}/ncgen/getopt.c
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/getopt.c ${PACKAGE_TARGET}/ncdump/getopt.c
  )
add_subdirectory(${PACKAGE_TARGET})


# udunits
#
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
add_subdirectory(${PACKAGE_TARGET})


# proj
#
UNPACK_EXTERNAL_PACKAGE(proj 4.4.9)
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/CMakeLists.txt.in ${PACKAGE_TARGET}/CMakeLists.txt
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PACKAGE_SOURCE}/proj_config.h.in ${PACKAGE_TARGET}/src/proj_config.h.in
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
add_subdirectory(${PACKAGE_TARGET})


# coda
#
# This is the only way I can discover of making the CODA options
# 'inherit' the corresponding BRAT options.
#
set(CODA_BUILD_IDL ${BRATHL_BUILD_IDL} CACHE BOOL "build CODA IDL Interface" FORCE)
set(CODA_BUILD_MATLAB ${BRATHL_BUILD_MATLAB} CACHE BOOL "build CODA MATLAB Interface" FORCE)
set(CODA_DEFINITION_IDL \"../data\" CACHE STRING "location of CODA definition files for IDL interface" FORCE)
set(CODA_DEFINITION_MATLAB \"../data\" CACHE STRING "location of CODA definition files for MATLAB interface" FORCE)
UNPACK_EXTERNAL_PACKAGE(coda 2.13)
add_subdirectory(${PACKAGE_TARGET})


# Remove all the directories generated by the
# UNPACK_EXTERNAL_PACKAGE calls made in this file.
#
add_custom_target(clean_externals
  COMMENT "Removing the unpacked external packages from ${CMAKE_CURRENT_SOURCE_DIR}"
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/libgeotiff
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/netcdf
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/udunits
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/proj
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/coda
)
