#
# Set parameters to link fortran calling C++ code (generally needs
# the Std C++ library)
#


ENABLE_LANGUAGE(Fortran)

IF (WIN32)
ENDIF (WIN32)

IF (UNIX)
  if (APPLE)
    SET (CMAKE_Fortran_LINK_FLAGS "${CMAKE_Fortran_LINK_FLAGS} -lstdc++-static")
  else(APPLE)
    SET (CMAKE_Fortran_LINK_FLAGS "${CMAKE_Fortran_LINK_FLAGS} -lstdc++")
  endif(APPLE)
  SET (CMAKE_Fortran_FLAGS_DEBUG "${CMAKE_Fortran_FLAGS_DEBUG} -g")
ENDIF (UNIX)

