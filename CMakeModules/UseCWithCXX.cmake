#
# Set parameters to link C calling C++ code (generally needs
# the Std C++ library)
#


IF (WIN32)
ENDIF (WIN32)

IF (UNIX)
  SET (CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -lstdc++")
ENDIF (UNIX)

