#
# Find a VTK5 installation or build tree.
# Based upon FindVTK.cmake found in Cmake 2.2.3 distrib
#
# The following variables are set if VTK is found.  If VTK is not
# found, VTK_FOUND is set to false.
#
# VTK_FOUND          - Set to true when VTK is found.
# VTK_USE_FILE       - CMake source file to setup a project to use VTK.
# VTK_MAJOR_VERSION  - The VTK major version number.
# VTK_MINOR_VERSION  - The VTK minor version number (odd for non-release).
# VTK_BUILD_VERSION  - The VTK patch level (meaningless for odd minor).
# VTK_INCLUDE_DIRS   - Include directories for VTK headers.
# VTK_LIBRARY_DIRS   - Link directories for VTK libraries.
# VTK_KITS           - List of VTK kits, in all CAPS (COMMON, IO, ...).
# VTK_LANGUAGES      - List of wrapped languages, in all CAPS (TCL, ...).
#
# The following cache entries must be set by the user to locate VTK:
#
# VTK_DIR      - The directory containing VTKConfig.cmake.  This is either
#                the root of the build tree, or the lib/vtk
#                directory.  This is the only cache entry.
#
# The following variables are set for backward compatability and
# should not be used in new code:
#
# USE_VTK_FILE - The full path to the UseVTK.cmake file.  This is provided
#                for backward compatability.  Use VTK_USE_FILE instead.
#

SET(VTK5_DIR_DESCRIPTION "directory containing VTKConfig.cmake.  This is either the root of the build tree, or PREFIX/lib/vtk-5.0 for an installation.")
SET(VTK5_DIR_MESSAGE "VTK5 not found. Try setting the VTK_DIR environment or Cmake variable to the ${VTK5_DIR_DESCRIPTION}")

# Search only if the location is not already known.
IF(NOT VTK_DIR)

  # Get the system search path as a list.
  IF(UNIX)
    STRING(REGEX MATCHALL "[^:]+" VTK_DIR_SEARCH1 "$ENV{PATH}")
  ELSE(UNIX)
    STRING(REGEX REPLACE "\\\\" "/" VTK_DIR_SEARCH1 "$ENV{PATH}")
  ENDIF(UNIX)
  STRING(REGEX REPLACE "/;" ";" VTK_DIR_SEARCH2 "${VTK_DIR_SEARCH1}")

  # Construct a set of paths relative to the system search path.
  SET(VTK_DIR_SEARCH "")
  FOREACH(dir ${VTK_DIR_SEARCH2})
    SET(VTK_DIR_SEARCH ${VTK_DIR_SEARCH} "${dir}/../lib/vtk-5.0")
  ENDFOREACH(dir)

  #
  # Look for an installation or build tree.
  #
  FIND_PATH(VTK_DIR UseVTK.cmake
    # Look for an environment variable VTK_DIR.
    $ENV{VTK_DIR}

    # Look in places relative to the system executable search path.
    ${VTK_DIR_SEARCH}
    # Help the user find it if we cannot.
    DOC "The ${VTK5_DIR_DESCRIPTION}"
  )
ENDIF(NOT VTK_DIR)

FIND_PACKAGE(VTK QUIET)

IF(NOT VTK_FOUND)
  # VTK not found, explain to the user how to specify its location.
  IF(NOT VTK5_FIND_QUIETLY)
    MESSAGE(FATAL_ERROR ${VTK5_DIR_MESSAGE})
  ELSE(NOT VTK5_FIND_QUIETLY)
    IF(VTK5_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR ${VTK5_DIR_MESSAGE})
    ENDIF(VTK5_FIND_REQUIRED)
  ENDIF(NOT VTK5_FIND_QUIETLY)
ENDIF(NOT VTK_FOUND)
