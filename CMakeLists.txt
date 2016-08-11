# The main BRAT CMake configuration file.
#
# HDF5, SZip, ..., require cmake 3.1.0
#
cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
MACRO(DBG_MSG _MSG)
  #MESSAGE(STATUS
  #  "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}): ${_MSG}")
ENDMACRO(DBG_MSG)
MACRO(DBG_MSG_V _MSG)
  #  MESSAGE(STATUS
  #   "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}): ${_MSG}")
ENDMACRO(DBG_MSG_V)


project(BRAT)

set(BRAT_MAJOR_VERSION "4")
set(BRAT_MINOR_VERSION "0")
set(BRAT_REVISION_VERSION "0")
set(BRAT_VERSION "${BRAT_MAJOR_VERSION}.${BRAT_MINOR_VERSION}.${BRAT_REVISION_VERSION}")

message(STATUS "System type: ${CMAKE_SYSTEM_NAME}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin CACHE INTERNAL "Directory for all programs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib CACHE INTERNAL "Directory for all dynamic libraries")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib CACHE INTERNAL "Directory for all static libraries")
set(CMAKE_MODULE_PATH ${BRAT_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH} CACHE INTERNAL "Directories where to find module definitions")
set(CMAKE_BUILD_TYPE "" CACHE STRING "Build type option")

# Make sure to override /usr/local as default installation
# prefix, because BRAT needs to be installed in a subdirectory of
# its own.
if(NOT WIN32)
  if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX
      "/usr/local/brat" CACHE PATH "BRAT default install prefix" FORCE
      )
  endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
endif(NOT WIN32)


if(WIN32)

  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_DEBUG_POSTFIX "d")
  else(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_DEBUG_POSTFIX "")
  endif(CMAKE_BUILD_TYPE MATCHES "Debug")


  if (MSVC)

	  if(MSVC90)

		MESSAGE(STATUS "Processing MSVC90 redistributable DLLs...")
	
		# Microsoft's redistributable DLLs 
		# 

		get_filename_component(VC_DIR ${CMAKE_CXX_COMPILER} PATH)

		# If VC_DIR is empty try to find VC_DIR with ${CMAKE_MAKE_PROGRAM}
		if(NOT VC_DIR)
		  get_filename_component(VCEXPRESS_DIR ${CMAKE_MAKE_PROGRAM} PATH)
		  SET(VC_DIR "${VCEXPRESS_DIR}/../../VC/bin" )
		endif(NOT VC_DIR)

		install(DIRECTORY ${VC_DIR}/../redist/x86/Microsoft.VC90.CRT DESTINATION bin/)

		# Note: abusing configure_file again, just to get things in
		# the right location for InstallBuilder. This is now getting
		# out of hand however, and I will fix this properly with high
		# priority.
		# 

		set(VC_DIR ${VC_DIR}/../redist/x86/Microsoft.VC90.CRT)
		configure_file(${VC_DIR}/msvcp90.dll
					   ${BRAT_BINARY_DIR}/bin/Microsoft.VC90.CRT/msvcp90.dll
					   COPYONLY)
		configure_file(${VC_DIR}/msvcr90.dll
					   ${BRAT_BINARY_DIR}/bin/Microsoft.VC90.CRT/msvcr90.dll
					   COPYONLY)
		configure_file(${VC_DIR}/msvcm90.dll
					   ${BRAT_BINARY_DIR}/bin/Microsoft.VC90.CRT/msvcm90.dll
					   COPYONLY)


	  endif(MSVC90)

	  if(MSVC10)

		MESSAGE(STATUS "Processing MSVC10 redistributable DLLs...")
	
		# To disable deprecation C unsafe functions
		# Microsoft added some "safe" versions of many functions such as strcpy(), makepath(), etc.
		# They called them _strcpy_s() and _makepath_s() respectfully.
		# When compiling the old code, many warnings were displayed about using the "unsafe" version
		# and that the "safe" version should be used instead.
		# Again, there is some defines that could be set to suppress this warning.

		add_definitions (/D _CRT_SECURE_NO_WARNINGS)
		add_definitions (/D _CRT_SECURE_NO_DEPRECATE)
		add_definitions (/D _CRT_NONSTDC_NO_DEPRECATE)

		# Disable Checked Iterators in VC9 STL

		add_definitions(/D _SECURE_SCL=0)
		add_definitions(/D _HAS_ITERATOR_DEBUGGING=0)

		# Disable Runtime-Checks

		#add_definitions (/I /K)

		if(CMAKE_CXX_FLAGS_DEBUG MATCHES "/RTC1")
		  string(REPLACE "/RTC1" ""
		  CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
		  message(STATUS "info: removed '/RTC1' in CMAKE_CXX_FLAGS_DEBUG")
		endif()

		if(CMAKE_C_FLAGS_DEBUG MATCHES "/RTC1")
		  string(REPLACE "/RTC1" ""
		  CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
		  message(STATUS "info: removed '/RTC1' in CMAKE_CXX_FLAGS_DEBUG")
		endif()


		# Force "/FR"  options if CMAKE_MSVC_BROWSE_INFO is ON. CMake doesn't do it.
		if(CMAKE_MSVC_BROWSE_INFO)
		  add_definitions(/FR)
		endif(CMAKE_MSVC_BROWSE_INFO)


		# Microsoft's redistributable DLLs 
		# Get them from the Windows/System folder

		install(FILES "C:/WINDOWS/system32/msvcp100.dll" DESTINATION bin/)
		install(FILES "C:/WINDOWS/system32/msvcr100.dll" DESTINATION bin/)

		install(FILES "C:/WINDOWS/system32/msvcp100.dll" DESTINATION matlab/)
		install(FILES "C:/WINDOWS/system32/msvcr100.dll" DESTINATION matlab/)

		# Note: abusing configure_file again, just to get things in
		# the right location for InstallBuilder. This is now getting
		# out of hand however, and I will fix this properly (using
		# the installed files instead) with top priority.
		# 
		configure_file("C:/WINDOWS/system32/msvcp100.dll"
					   ${BRAT_BINARY_DIR}/bin/msvcp100.dll
					   COPYONLY)
		configure_file("C:/WINDOWS/system32/msvcr100.dll"
					   ${BRAT_BINARY_DIR}/bin/msvcr100.dll
					   COPYONLY)

		configure_file("C:/WINDOWS/system32/msvcp100.dll"
					   ${BRAT_BINARY_DIR}/matlab/msvcp100.dll
					   COPYONLY)
		configure_file("C:/WINDOWS/system32/msvcr100.dll"
					   ${BRAT_BINARY_DIR}/matlab/msvcr100.dll
					   COPYONLY)
  
	  endif(MSVC10)


	  IF(MSVC11 OR MSVC12)

		IF(MSVC11)
			SET(VC_VER 11)
		ELSE()
			SET(VC_VER 12)
		ENDIF()
	
		MESSAGE(STATUS "Processing MSVC${VC_VER} redistributable DLLs...")

		# Microsoft's redistributable DLLs 
		# Get them from the VC installation folder
	
		IF(CMAKE_BUILD_TYPE MATCHES "Debug")

			IF ("${BRAT_TARGET_PROCESSOR}" STREQUAL "x64")
				SET(VCREDIST_DIR $ENV{VCINSTALLDIR}/redist/Debug_NonRedist/x64/Microsoft.VC${VC_VER}0.DebugCRT)
			ELSE ()
				SET(VCREDIST_DIR $ENV{VCINSTALLDIR}/redist/Debug_NonRedist/x86/Microsoft.VC${VC_VER}0.DebugCRT)
			ENDIF ()	
		
		ELSE(CMAKE_BUILD_TYPE MATCHES "Debug")
	
			IF ("${BRAT_TARGET_PROCESSOR}" STREQUAL "x64")
				SET(VCREDIST_DIR $ENV{VCINSTALLDIR}/redist/x64/Microsoft.VC${VC_VER}0.CRT)
			ELSE ()
				SET(VCREDIST_DIR $ENV{VCINSTALLDIR}/redist/x86/Microsoft.VC${VC_VER}0.CRT)
			ENDIF ()	
		
		ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")
	
		file(TO_CMAKE_PATH ${VCREDIST_DIR} VCREDIST_DIR)

		INSTALL(FILES	"${VCREDIST_DIR}/msvcp${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll" 
						"${VCREDIST_DIR}/msvcr${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll" 
						"${VCREDIST_DIR}/vccorlib${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll" DESTINATION bin/)

		INSTALL(FILES	"${VCREDIST_DIR}/msvcp${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll"
						"${VCREDIST_DIR}/msvcr${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll"
						"${VCREDIST_DIR}/vccorlib${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll" DESTINATION matlab/)
	
		# Get things in the right location for InstallBuilder (in the build directory)

		configure_file("${VCREDIST_DIR}/msvcp${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll"
						${BRAT_BINARY_DIR}/bin/msvcp${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll
						COPYONLY)
		configure_file("${VCREDIST_DIR}/msvcr${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll"
						${BRAT_BINARY_DIR}/bin/msvcr${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll
						COPYONLY)
		configure_file("${VCREDIST_DIR}/vccorlib${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll"
						${BRAT_BINARY_DIR}/bin/vccorlib${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll
						COPYONLY)

		configure_file("${VCREDIST_DIR}/msvcp${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll"
						${BRAT_BINARY_DIR}/matlab/msvcp${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll
						COPYONLY)
		configure_file("${VCREDIST_DIR}/msvcr${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll"
						${BRAT_BINARY_DIR}/matlab/msvcr${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll
						COPYONLY)
		configure_file("${VCREDIST_DIR}/vccorlib${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll"
						${BRAT_BINARY_DIR}/matlab/vccorlib${VC_VER}0${CMAKE_DEBUG_POSTFIX}.dll
						COPYONLY)
  
	  ENDIF(MSVC11 OR MSVC12)

	set(CMAKE_C_FLAGS "/WX- ${CMAKE_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "/WX- ${CMAKE_CXX_FLAGS}")
	
  endif (MSVC)

endif(WIN32)


# Prefer static libraries over dynamic libraries:
# TODO: this needs to become optional.
if(WIN32)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib;.dll;${CMAKE_FIND_LIBRARY_SUFFIXES}")
else(WIN32)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so;${CMAKE_FIND_LIBRARY_SUFFIXES}")
endif(WIN32)

option(BRATHL_BUILD_FORTRAN "Build FORTRAN interface" OFF)
option(BRATHL_BUILD_IDL "Build IDL interface" OFF)
option(BRATHL_BUILD_MATLAB "Build MATLAB interface" OFF)
option(BRAT_BUILD_GUI "Build graphic user interface programs" OFF)
if(WIN32)
  if(MSVC)
    option(CMAKE_MSVC_BROWSE_INFO "Set the Browse Information flag" OFF)
  endif(MSVC)
endif(WIN32)


if(CMAKE_BUILD_TYPE MATCHES "^$")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(CMAKE_BUILD_TYPE MATCHES "^$")

# Global compilation settings
#
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++0x -Wl,--rpath=./" )
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--rpath=./" )
	SET(CMAKE_INSTALL_RPATH "$ORIGIN/.")
	set(BRAT_GNU_OPTIONS "-Wall")
endif(CMAKE_COMPILER_IS_GNUCXX)


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++11 -Wno-dangling-else -Wno-format-security" )
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++" )
endif()

IF (WIN32)
#    IF ("${BRAT_TARGET_PROCESSOR}" STREQUAL "Win32")
		string( REPLACE "/EHs" "/EHa" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} )
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _USING_V110_SDK71_ /D _CRT_SECURE_NO_WARNINGS" )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D _USING_V110_SDK71_ /D _CRT_SECURE_NO_WARNINGS" )
		IF ("${BRAT_TARGET_PROCESSOR}" STREQUAL "x64")
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS\",5.02\"")
		ELSE()
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS\",5.01\"")
		ENDIF ()
#    ENDIF ()
ENDIF ()



if(MSVC)
  if(NOT MSVC60)
    add_definitions(/wd4274)
  endif(NOT MSVC60)
endif(MSVC)


# RUNTIME ##################################################################################################


IF (BRAT_BUILD_GUI)

	# VC90 vcredist_x64.exe or vcredist_x86.exe still needed in XP
	
	IF (NOT APPLE)
		IF (NOT EXISTS ${PYTHON_BIN_DIR})
			MESSAGE(FATAL_ERROR "PYTHON_BIN_DIR not defined or not found." )
		ENDIF()
		IF (WIN32)
			IF (NOT EXISTS ${PYTHON_LIBRARY})
				MESSAGE(FATAL_ERROR "PYTHON_LIBRARY not defined or not found." )
			ENDIF()
		ENDIF()
	ENDIF (NOT APPLE)


	IF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
		MESSAGE(FATAL_ERROR "QT_QMAKE_EXECUTABLE not defined or not found." )
	ELSE()
	ENDIF()

	IF (NOT EXISTS ${OSG_BIN_DIR})
		MESSAGE(FATAL_ERROR "OSG_BIN_DIR not defined or not found." )
	ENDIF()

	IF (NOT EXISTS ${QWT_BIN_DIR})
		MESSAGE(FATAL_ERROR "QWT_BIN_DIR not defined or not found: ${QWT_BIN_DIR}." )
	ENDIF()

	IF (NOT EXISTS ${QWT3D_BIN_DIR})
		MESSAGE(FATAL_ERROR "QWT3D_BIN_DIR not defined or not found." )
	ENDIF()

	
	get_filename_component( QT_BINDIR ${QT_QMAKE_EXECUTABLE} PATH )
	get_filename_component( QT_BINDIR_PARENT ${QT_BINDIR} PATH )
	set( QT_LIBDIR ${QT_BINDIR_PARENT}/lib )
	IF(NOT WIN32)
		set( QT_BINDIR ${QT_LIBDIR} )
	ENDIF(NOT WIN32)

	get_filename_component( QTDIR ${QT_BINDIR} PATH )
	get_filename_component( QGIS_DIR ${QGIS_BIN_DIR} PATH )
	
ENDIF (BRAT_BUILD_GUI)





IF( WIN32)
	SET( DBG_SUFFIX ${CMAKE_DEBUG_POSTFIX} )
	SET( DLL_SUFFIX .dll )
	SET( EXEC_SUFFIX .exe )

ELSEIF(APPLE)
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		SET( DBG_SUFFIX _debug )
		SET( OSG_DBG_SUFFIX d )
	else(CMAKE_BUILD_TYPE MATCHES "Debug")
		SET( DBG_SUFFIX )
		SET( OSG_DBG_SUFFIX )
	endif(CMAKE_BUILD_TYPE MATCHES "Debug")

	SET( DLL_SUFFIX .dylib )
	SET( EXEC_SUFFIX )

ELSE(WIN32)
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		SET( OSG_DBG_SUFFIX d )
	else(CMAKE_BUILD_TYPE MATCHES "Debug")
		SET( OSG_DBG_SUFFIX )
	endif(CMAKE_BUILD_TYPE MATCHES "Debug")

	SET( DBG_SUFFIX )
	SET( DLL_SUFFIX .so )
	SET( EXEC_SUFFIX )
ENDIF(WIN32)




# Files for main binary directory or Frameworks directory in Mac
#
#	- Qt		(Frameworks)
#	- OSG		(Frameworks)
#	- qwt		(Frameworks)
#	- qwt3d		(Frameworks)
#	- QGIS		(Frameworks)
#	- osgEarth	(Frameworks)
#	- OSGeo4W
#	- trace scripts

IF( WIN32)
	SET( OTHER_FILES
		${OTHER_FILES}
		${QT_BINDIR}/QtCore${DBG_SUFFIX}4${DLL_SUFFIX}
		${QT_BINDIR}/QtGui${DBG_SUFFIX}4${DLL_SUFFIX}
		${QT_BINDIR}/QtOpenGL${DBG_SUFFIX}4${DLL_SUFFIX}
		${QT_BINDIR}/QtNetwork${DBG_SUFFIX}4${DLL_SUFFIX}
		${QT_BINDIR}/QtSvg${DBG_SUFFIX}4${DLL_SUFFIX}
		${QT_BINDIR}/QtXml${DBG_SUFFIX}4${DLL_SUFFIX}
		${QT_BINDIR}/QtWebKit${DBG_SUFFIX}4${DLL_SUFFIX}
		${OSG_BIN_DIR}/osg130-osg${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/osg130-osgDB${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/osg130-osgGA${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/osg130-osgUtil${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/osg130-osgQt${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/osg130-osgViewer${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/osg130-osgShadow${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/osg130-osgText${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/osg130-osgTerrain${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/osg130-osgManipulator${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/osg130-osgAnimation${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/osg130-osgSim${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/ot20-OpenThreads${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/zlib${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_BIN_DIR}/gdal110${DLL_SUFFIX}
		${QWT_BIN_DIR}/qwt${DBG_SUFFIX}5${DLL_SUFFIX}
		${QWT3D_BIN_DIR}/qwtplot3d${DLL_SUFFIX}
		${QGIS_BIN_DIR}/qgis_core${DLL_SUFFIX}
		${QGIS_BIN_DIR}/qgis_gui${DLL_SUFFIX}
		${OSG_EARTH_BIN_DIR}/osgEarth${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_EARTH_BIN_DIR}/osgEarthAnnotation${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_EARTH_BIN_DIR}/osgEarthFeatures${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_EARTH_BIN_DIR}/osgEarthQt${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_EARTH_BIN_DIR}/osgEarthSymbology${DBG_SUFFIX}${DLL_SUFFIX}
		${OSG_EARTH_BIN_DIR}/osgEarthUtil${DBG_SUFFIX}${DLL_SUFFIX}
		)

	IF (CMAKE_BUILD_TYPE MATCHES "Debug")
		SET( OTHER_FILES
			${OTHER_FILES}
			${QT_LIBDIR}/QtCore${DBG_SUFFIX}4.pdb
			${QT_LIBDIR}/QtGui${DBG_SUFFIX}4.pdb
			${QT_LIBDIR}/QtOpenGL${DBG_SUFFIX}4.pdb
			${QT_LIBDIR}/QtNetwork${DBG_SUFFIX}4.pdb
			${QT_LIBDIR}/QtSvg${DBG_SUFFIX}4.pdb
			${QT_LIBDIR}/QtXml${DBG_SUFFIX}4.pdb
			${QT_LIBDIR}/QtWebKit${DBG_SUFFIX}4.pdb
			${OSG_BIN_DIR}/osg130-osg${DBG_SUFFIX}.pdb
			${OSG_BIN_DIR}/osg130-osgDB${DBG_SUFFIX}.pdb
			${OSG_BIN_DIR}/osg130-osgGA${DBG_SUFFIX}.pdb
			${OSG_BIN_DIR}/osg130-osgUtil${DBG_SUFFIX}.pdb
			${OSG_BIN_DIR}/osg130-osgQt${DBG_SUFFIX}.pdb
			${OSG_BIN_DIR}/osg130-osgViewer${DBG_SUFFIX}.pdb
			${OSG_BIN_DIR}/osg130-osgShadow${DBG_SUFFIX}.pdb
			${OSG_BIN_DIR}/osg130-osgText${DBG_SUFFIX}.pdb
			${OSG_BIN_DIR}/osg130-osgTerrain${DBG_SUFFIX}.pdb
			${OSG_BIN_DIR}/osg130-osgManipulator${DBG_SUFFIX}.pdb
			${OSG_BIN_DIR}/osg130-osgAnimation${DBG_SUFFIX}.pdb
			${OSG_BIN_DIR}/osg130-osgSim${DBG_SUFFIX}.pdb
			# ${OSG_BIN_DIR}/ot20-OpenThreads${DBG_SUFFIX}.pdb
			${OSG_BIN_DIR}/zlib${DBG_SUFFIX}.pdb
			# ${OSG_BIN_DIR}/gdal110.pdb
			${QWT_BIN_DIR}/qwt${DBG_SUFFIX}5.pdb
			${QWT3D_BIN_DIR}/qwtplot3d.pdb
			${QGIS_BIN_DIR}/qgis_core.pdb
			${QGIS_BIN_DIR}/qgis_gui.pdb
			${OSG_EARTH_BIN_DIR}/osgEarth${DBG_SUFFIX}.pdb
			${OSG_EARTH_BIN_DIR}/osgEarthAnnotation${DBG_SUFFIX}.pdb
			${OSG_EARTH_BIN_DIR}/osgEarthFeatures${DBG_SUFFIX}.pdb
			${OSG_EARTH_BIN_DIR}/osgEarthQt${DBG_SUFFIX}.pdb
			${OSG_EARTH_BIN_DIR}/osgEarthSymbology${DBG_SUFFIX}.pdb
			${OSG_EARTH_BIN_DIR}/osgEarthUtil${DBG_SUFFIX}.pdb
			)
	ENDIF (CMAKE_BUILD_TYPE MATCHES "Debug")
	
ELSE(WIN32)

	IF(APPLE)
		SET( QT_FRAMEWORK_FILES
			${QT_FRAMEWORK_FILES}
			${QT_BINDIR}/libQtCore${DBG_SUFFIX}.4.8.6${DLL_SUFFIX}
			${QT_BINDIR}/libQtGui${DBG_SUFFIX}.4.8.6${DLL_SUFFIX}
			${QT_BINDIR}/libQtOpenGL${DBG_SUFFIX}.4.8.6${DLL_SUFFIX}
			${QT_BINDIR}/libQtNetwork${DBG_SUFFIX}.4.8.6${DLL_SUFFIX}
			${QT_BINDIR}/libQtSvg${DBG_SUFFIX}.4.8.6${DLL_SUFFIX}
			${QT_BINDIR}/libQtXml${DBG_SUFFIX}.4.8.6${DLL_SUFFIX}
			${QT_BINDIR}/libQtWebKit${DBG_SUFFIX}.4.9.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtCore${DBG_SUFFIX}.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtGui${DBG_SUFFIX}.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtOpenGL${DBG_SUFFIX}.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtNetwork${DBG_SUFFIX}.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtSvg${DBG_SUFFIX}.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtXml${DBG_SUFFIX}.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtWebKit${DBG_SUFFIX}.4${DLL_SUFFIX}
		)

		SET( FRAMEWORK_FILES
			${QT_FRAMEWORK_FILES}
			${FRAMEWORK_FILES}
			${QT_BINDIR}/libQtCore${DBG_SUFFIX}.4.8.6${DLL_SUFFIX}
			${QT_BINDIR}/libQtGui${DBG_SUFFIX}.4.8.6${DLL_SUFFIX}
			${QT_BINDIR}/libQtOpenGL${DBG_SUFFIX}.4.8.6${DLL_SUFFIX}
			${QT_BINDIR}/libQtNetwork${DBG_SUFFIX}.4.8.6${DLL_SUFFIX}
			${QT_BINDIR}/libQtSvg${DBG_SUFFIX}.4.8.6${DLL_SUFFIX}
			${QT_BINDIR}/libQtXml${DBG_SUFFIX}.4.8.6${DLL_SUFFIX}
			${QT_BINDIR}/libQtWebKit${DBG_SUFFIX}.4.9.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtCore${DBG_SUFFIX}.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtGui${DBG_SUFFIX}.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtOpenGL${DBG_SUFFIX}.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtNetwork${DBG_SUFFIX}.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtSvg${DBG_SUFFIX}.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtXml${DBG_SUFFIX}.4${DLL_SUFFIX}
			${QT_BINDIR}/libQtWebKit${DBG_SUFFIX}.4${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosg${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgDB${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgFX${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgGA${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgUtil${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgQt${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgWidget${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgViewer${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgShadow${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgText${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgTerrain${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgManipulator${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgAnimation${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgSim${OSG_DBG_SUFFIX}.3.4.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libOpenThreads${OSG_DBG_SUFFIX}.3.3.0${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosg${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgDB${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgFX${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgGA${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgUtil${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgQt${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgWidget${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgViewer${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgShadow${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgText${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgTerrain${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgManipulator${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgAnimation${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libosgSim${OSG_DBG_SUFFIX}.130${DLL_SUFFIX}
			${OSG_BIN_DIR}/libOpenThreads${OSG_DBG_SUFFIX}.20${DLL_SUFFIX}
			${QWT_BIN_DIR}/libqwt${DLL_SUFFIX}
			${QWT3D_BIN_DIR}/libqwtplot3d${DLL_SUFFIX}
			${OSG_EARTH_BIN_DIR}/libosgEarth${OSG_DBG_SUFFIX}.2.6.0${DLL_SUFFIX}
			${OSG_EARTH_BIN_DIR}/libosgEarthAnnotation${OSG_DBG_SUFFIX}.2.6.0${DLL_SUFFIX}
			${OSG_EARTH_BIN_DIR}/libosgEarthFeatures${OSG_DBG_SUFFIX}.2.6.0${DLL_SUFFIX}
			${OSG_EARTH_BIN_DIR}/libosgEarthQt${OSG_DBG_SUFFIX}.2.6.0${DLL_SUFFIX}
			${OSG_EARTH_BIN_DIR}/libosgEarthSymbology${OSG_DBG_SUFFIX}.2.6.0${DLL_SUFFIX}
			${OSG_EARTH_BIN_DIR}/libosgEarthUtil${OSG_DBG_SUFFIX}.2.6.0${DLL_SUFFIX}
			${OSG_EARTH_BIN_DIR}/libosgEarth${OSG_DBG_SUFFIX}.0${DLL_SUFFIX}
			${OSG_EARTH_BIN_DIR}/libosgEarthAnnotation${OSG_DBG_SUFFIX}.0${DLL_SUFFIX}
			${OSG_EARTH_BIN_DIR}/libosgEarthFeatures${OSG_DBG_SUFFIX}.0${DLL_SUFFIX}
			${OSG_EARTH_BIN_DIR}/libosgEarthQt${OSG_DBG_SUFFIX}.0${DLL_SUFFIX}
			${OSG_EARTH_BIN_DIR}/libosgEarthSymbology${OSG_DBG_SUFFIX}.0${DLL_SUFFIX}
			${OSG_EARTH_BIN_DIR}/libosgEarthUtil${OSG_DBG_SUFFIX}.0${DLL_SUFFIX}
		)
	ELSE(APPLE)
		SET( OTHER_FILES
			${OTHER_FILES}
			${QT_BINDIR}/libQtCore${DLL_SUFFIX}.4.8.6
			${QT_BINDIR}/libQtGui${DLL_SUFFIX}.4.8.6
			${QT_BINDIR}/libQtOpenGL${DLL_SUFFIX}.4.8.6
			${QT_BINDIR}/libQtNetwork${DLL_SUFFIX}.4.8.6
			${QT_BINDIR}/libQtSvg${DLL_SUFFIX}.4.8.6
			${QT_BINDIR}/libQtXml${DLL_SUFFIX}.4.8.6
			${QT_BINDIR}/libQtWebKit${DLL_SUFFIX}.4.9.4
			${QT_BINDIR}/libQtCore${DLL_SUFFIX}.4
			${QT_BINDIR}/libQtGui${DLL_SUFFIX}.4
			${QT_BINDIR}/libQtOpenGL${DLL_SUFFIX}.4
			${QT_BINDIR}/libQtNetwork${DLL_SUFFIX}.4
			${QT_BINDIR}/libQtSvg${DLL_SUFFIX}.4
			${QT_BINDIR}/libQtXml${DLL_SUFFIX}.4
			${QT_BINDIR}/libQtWebKit${DLL_SUFFIX}.4
			${OSG_BIN_DIR}/libosg${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libosgDB${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libosgFX${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libosgGA${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libosgUtil${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libosgQt${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libosgWidget${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libosgViewer${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libosgShadow${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libosgText${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libosgTerrain${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libosgManipulator${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libosgAnimation${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libosgSim${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.4.0
			${OSG_BIN_DIR}/libOpenThreads${OSG_DBG_SUFFIX}${DLL_SUFFIX}.3.3.0
			${OSG_BIN_DIR}/libosg${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libosgDB${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libosgFX${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libosgGA${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libosgUtil${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libosgQt${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libosgWidget${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libosgViewer${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libosgShadow${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libosgText${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libosgTerrain${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libosgManipulator${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libosgAnimation${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libosgSim${OSG_DBG_SUFFIX}${DLL_SUFFIX}.130
			${OSG_BIN_DIR}/libOpenThreads${OSG_DBG_SUFFIX}${DLL_SUFFIX}.20
			${QWT_BIN_DIR}/libqwt${DLL_SUFFIX}
			${QWT3D_BIN_DIR}/libqwtplot3d${DLL_SUFFIX}
			${QGIS_BIN_DIR}/libqgis_core${DLL_SUFFIX}.2.8.3
			${QGIS_BIN_DIR}/libqgis_gui${DLL_SUFFIX}.2.8.3
			${QGIS_BIN_DIR}/libqgis_analysis${DLL_SUFFIX}.2.8.3
			${OSG_EARTH_BIN_DIR}/libosgEarth${OSG_DBG_SUFFIX}${DLL_SUFFIX}.2.6.0
			${OSG_EARTH_BIN_DIR}/libosgEarthAnnotation${OSG_DBG_SUFFIX}${DLL_SUFFIX}.2.6.0
			${OSG_EARTH_BIN_DIR}/libosgEarthFeatures${OSG_DBG_SUFFIX}${DLL_SUFFIX}.2.6.0
			${OSG_EARTH_BIN_DIR}/libosgEarthQt${OSG_DBG_SUFFIX}${DLL_SUFFIX}.2.6.0
			${OSG_EARTH_BIN_DIR}/libosgEarthSymbology${OSG_DBG_SUFFIX}${DLL_SUFFIX}.2.6.0
			${OSG_EARTH_BIN_DIR}/libosgEarthUtil${OSG_DBG_SUFFIX}${DLL_SUFFIX}.2.6.0
			${OSG_EARTH_BIN_DIR}/libosgEarth${OSG_DBG_SUFFIX}${DLL_SUFFIX}.0
			${OSG_EARTH_BIN_DIR}/libosgEarthAnnotation${OSG_DBG_SUFFIX}${DLL_SUFFIX}.0
			${OSG_EARTH_BIN_DIR}/libosgEarthFeatures${OSG_DBG_SUFFIX}${DLL_SUFFIX}.0
			${OSG_EARTH_BIN_DIR}/libosgEarthQt${OSG_DBG_SUFFIX}${DLL_SUFFIX}.0
			${OSG_EARTH_BIN_DIR}/libosgEarthSymbology${OSG_DBG_SUFFIX}${DLL_SUFFIX}.0
			${OSG_EARTH_BIN_DIR}/libosgEarthUtil${OSG_DBG_SUFFIX}${DLL_SUFFIX}.0
			)
	ENDIF(APPLE)
ENDIF(WIN32)


SET( OTHER_FILES
	${OTHER_FILES}
	new-gui/brat/brat_trace.bat
	new-gui/brat/brat_trace.py
	new-gui/brat/brat_trace.sh
	new-gui/scheduler/scheduler_trace.bat
	new-gui/scheduler/scheduler_trace.py
	new-gui/scheduler/scheduler_trace.sh
	)


IF( WIN32)

	IF ("${BRAT_TARGET_PROCESSOR}" STREQUAL "x64")
	
        set( OTHER_FILES
			${OTHER_FILES}
            ${OSGeo4W_BIN_DIR}/expat${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/ogdi${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/libpng16${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/spatialite${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/spatialindex-64${DLL_SUFFIX} 
		)

    ELSE()

        set( OTHER_FILES
			${OTHER_FILES}
            ${OSGeo4W_BIN_DIR}/ogdi_32b1${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/spatialite4${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/spatialindex1${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/zlib_osgeo${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/libexpat${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/libiconv-2${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/hdf_fw${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/mfhdf_fw${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/libintl-8${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/krb5_32${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/comerr32${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/k5sprt32${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/gssapi32${DLL_SUFFIX}
            ${OSGeo4W_BIN_DIR}/libgcc_s_dw2-1${DLL_SUFFIX}
		)

    ENDIF()


	IF (CMAKE_BUILD_TYPE MATCHES "Debug")
		SET( OTHER_FILES
			${OTHER_FILES}
	        ${OSGeo4W_BIN_DIR}/libcurl${DLL_SUFFIX}
			)
	ENDIF()


    set( OTHER_FILES
		${OTHER_FILES}
		${OSGeo4W_BIN_DIR}/proj${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/geos_c${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/gdal111${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/iconv${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/xerces-c_3_1${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/libpq${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/netcdf${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/geotiff${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/libtiff${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/sqlite3${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/freexl${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/libxml2${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/libmysql${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/hdf5${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/zlib1${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/szip${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/openjp2${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/ssleay32${DLL_SUFFIX}
		${OSGeo4W_BIN_DIR}/libeay32${DLL_SUFFIX}
		${XSD_BIN_DIR}/xerces-c_3_1${DBG_SUFFIX}_vc120${DLL_SUFFIX}
	)

	file( GLOB JPEG_OTHER_FILES ${OSGeo4W_BIN_DIR}/jpeg*${DLL_SUFFIX} )
	file( GLOB MSVC_OTHER_FILES ${OSGeo4W_BIN_DIR}/msvc*${DLL_SUFFIX} )
	SET( OTHER_FILES
		${OTHER_FILES}
		${JPEG_OTHER_FILES}
		${MSVC_OTHER_FILES}
		${PYTHON_LIBRARY}
		)

ENDIF(WIN32)




IF (APPLE)
	set( RUNTIME_DESTINATION ${BRAT_BINARY_DIR}/bin/brat.app/Contents/MacOS )
	set( SCHEDULER_RUNTIME_DESTINATION ${BRAT_BINARY_DIR}/bin/scheduler.app/Contents/MacOS )
	set( RUNTIME_DESTINATION_QT_PLUGINS ${BRAT_BINARY_DIR}/bin/brat.app/Contents/Plugins )
	set( SCHEDULER_RUNTIME_DESTINATION_QT_PLUGINS ${BRAT_BINARY_DIR}/bin/scheduler.app/Contents/Plugins )
ELSE (APPLE)
	set( RUNTIME_DESTINATION bin )
	set( RUNTIME_DESTINATION_QT_PLUGINS bin )
ENDIF (APPLE)





ADD_CUSTOM_TARGET( brat_runtime )
ADD_CUSTOM_TARGET( scheduler_runtime )

#######################################################
# Python
#######################################################


#IF (NOT APPLE)
#
#	IF (WIN32)
#		file( GLOB PYTHON_EXECS ${PYTHON_BIN_DIR}/python*${EXEC_SUFFIX} )
#		INSTALL(
#			FILES
#			${PYTHON_EXECS}
#			DESTINATION ${RUNTIME_DESTINATION}/Python
#			COMPONENT brat_runtime
#			)
#	ENDIF(WIN32)
#
#ENDIF(NOT APPLE)



file( GLOB BRAT_EMBEDDED_PYTHON_FILES process/python/*.* )

INSTALL(
	FILES
	${BRAT_EMBEDDED_PYTHON_FILES}
	DESTINATION ${RUNTIME_DESTINATION}/Python
	COMPONENT brat_runtime
	)


IF(WIN32)
	INSTALL(
		DIRECTORY
		${PYTHON_BIN_DIR}/DLLs
		${PYTHON_BIN_DIR}/Lib
		DESTINATION bin/Python
		COMPONENT brat_runtime
		PATTERN __pycache__ EXCLUDE
		)
ELSE(WIN32)
	IF (APPLE)
		INSTALL(
			DIRECTORY
			${PYTHON_BIN_DIR}/
			DESTINATION ${RUNTIME_DESTINATION}/Python
			USE_SOURCE_PERMISSIONS
			COMPONENT brat_runtime
			PATTERN __pycache__ EXCLUDE
			)
	ELSE (APPLE)
		INSTALL(
			DIRECTORY
			${PYTHON_BIN_DIR}
			DESTINATION ${RUNTIME_DESTINATION}/Python/lib
			COMPONENT brat_runtime
			PATTERN __pycache__ EXCLUDE
			)
	ENDIF(APPLE)
ENDIF(WIN32)


#######################################################
# Libraries | Frameworks
#######################################################

INSTALL(
	FILES 
	${OTHER_FILES}
	DESTINATION ${RUNTIME_DESTINATION}
	COMPONENT brat_runtime
	)


IF (APPLE)
	INSTALL(
		FILES
		${QT_FRAMEWORK_FILES}
		DESTINATION ${SCHEDULER_RUNTIME_DESTINATION}/../Frameworks
		COMPONENT scheduler_runtime
		)

	INSTALL(
		FILES
		${FRAMEWORK_FILES}
		DESTINATION ${RUNTIME_DESTINATION}/../Frameworks
		COMPONENT brat_runtime
		)


	INSTALL(
		DIRECTORY
		${QGIS_BIN_DIR}
		DESTINATION ${RUNTIME_DESTINATION}/../
		COMPONENT brat_runtime
		)
ENDIF(APPLE)


#######################################################
# Qt/QGIS plugins
#######################################################

IF (APPLE)

	IF (CMAKE_BUILD_TYPE MATCHES "Debug")
		INSTALL(
			DIRECTORY
			${QTDIR}/plugins/
			DESTINATION ${RUNTIME_DESTINATION_QT_PLUGINS}
			COMPONENT brat_runtime
			PATTERN *_debug.dylib
			)
	ELSE (CMAKE_BUILD_TYPE MATCHES "Debug")
		INSTALL(
			DIRECTORY
			${QTDIR}/plugins/
			DESTINATION ${RUNTIME_DESTINATION_QT_PLUGINS}
			COMPONENT brat_runtime
			PATTERN *_debug.dylib EXCLUDE
			)
	ENDIF (CMAKE_BUILD_TYPE MATCHES "Debug")
	
	IF (CMAKE_BUILD_TYPE MATCHES "Debug")
		INSTALL(
			DIRECTORY
			${QTDIR}/plugins/
			DESTINATION ${SCHEDULER_RUNTIME_DESTINATION_QT_PLUGINS}
			COMPONENT scheduler_runtime
			PATTERN *_debug.dylib
			)
	ELSE (CMAKE_BUILD_TYPE MATCHES "Debug")
		INSTALL(
			DIRECTORY
			${QTDIR}/plugins/
			DESTINATION ${SCHEDULER_RUNTIME_DESTINATION_QT_PLUGINS}
			COMPONENT scheduler_runtime
			PATTERN *_debug.dylib EXCLUDE
			)
	ENDIF (CMAKE_BUILD_TYPE MATCHES "Debug")

ELSE(APPLE)

	INSTALL(
		DIRECTORY
		${QTDIR}/plugins
		DESTINATION ${RUNTIME_DESTINATION_QT_PLUGINS}
		COMPONENT brat_runtime
		)
	
ENDIF(APPLE)



INSTALL(
	DIRECTORY
	${QGIS_PLUGINS_DIR}
	DESTINATION ${RUNTIME_DESTINATION_QT_PLUGINS}
	COMPONENT brat_runtime
	)



#######################################################
# OSG & osgEarth plugins
#######################################################

IF (APPLE)
	file( GLOB OSG_PLUGIN_FILES ${OSG_BIN_DIR}/osgPlugins-3.4.0/*.so )					# .so, amazing...
	file( GLOB OSG_EARTH_PLUGIN_FILES ${OSG_EARTH_BIN_DIR}/osgPlugins-3.4.0/*.so )
ELSE (APPLE)
	IF (WIN32)
		file( GLOB OSG_PLUGIN_FILES ${OSG_BIN_DIR}/osgPlugins-3.4.0/*${DBG_SUFFIX}${DLL_SUFFIX} )
		file( GLOB OSG_PLUGIN_FILES_DEBUG ${OSG_BIN_DIR}/osgPlugins-3.4.0/*.pdb )
		file( GLOB OSG_EARTH_PLUGIN_FILES ${OSG_EARTH_BIN_DIR}/osgPlugins-3.4.0/*${DBG_SUFFIX}${DLL_SUFFIX} )
		file( GLOB OSG_EARTH_PLUGIN_FILES_DEBUG ${OSG_EARTH_BIN_DIR}/osgPlugins-3.4.0/*.pdb )
		IF (CMAKE_BUILD_TYPE MATCHES "Debug")
			SET( OSG_PLUGIN_FILES ${OSG_PLUGIN_FILES} ${OSG_PLUGIN_FILES_DEBUG} )
			SET( OSG_EARTH_PLUGIN_FILES ${OSG_EARTH_PLUGIN_FILES} ${OSG_EARTH_PLUGIN_FILES_DEBUG} )
		ENDIF (CMAKE_BUILD_TYPE MATCHES "Debug")
	ELSE (WIN32)
		file( GLOB OSG_PLUGIN_FILES ${OSG_BIN_DIR}/osgPlugins-3.4.0/*${DLL_SUFFIX} )
		file( GLOB OSG_EARTH_PLUGIN_FILES ${OSG_EARTH_BIN_DIR}/osgPlugins-3.4.0/*${DLL_SUFFIX} )
	ENDIF (WIN32)
ENDIF (APPLE)


INSTALL(
	FILES
	${OSG_PLUGIN_FILES}
	${OSG_EARTH_PLUGIN_FILES}
	DESTINATION ${RUNTIME_DESTINATION}/osgPlugins-3.4.0
	COMPONENT brat_runtime
	)


#######################################################
# Globe
#######################################################

IF(WIN32)

	INSTALL(
		DIRECTORY
		${QGIS_DIR}/globe
		DESTINATION ${RUNTIME_DESTINATION}
		COMPONENT brat_runtime
		)

ELSE(WIN32)

	IF (APPLE)

		INSTALL(
			DIRECTORY
			${QGIS_DIR}/Resources/globe
			DESTINATION ${RUNTIME_DESTINATION}
			COMPONENT brat_runtime
			)

	ELSE (APPLE)
		INSTALL(
			DIRECTORY
			${QGIS_DIR}/share/qgis/globe
			DESTINATION ${RUNTIME_DESTINATION}
			COMPONENT brat_runtime
			)
	ENDIF(APPLE)

ENDIF(WIN32)

	
#######################################################
# Data
#######################################################

INSTALL( 
	DIRECTORY 
	data
	DESTINATION ${RUNTIME_DESTINATION}
	COMPONENT brat_runtime
	)
	

# in mac internal data is in both bundles (and in API...)
#
IF (APPLE)
	INSTALL(
		DIRECTORY
		data
		DESTINATION ${SCHEDULER_RUNTIME_DESTINATION}
		COMPONENT scheduler_runtime
		)
ENDIF(APPLE)




#######################################################
# Demo
#######################################################

IF (APPLE)
	INSTALL(
		DIRECTORY
		Demo
		DESTINATION ${RUNTIME_DESTINATION}/../
		COMPONENT brat_runtime
		)
ELSE(APPLE)
	INSTALL(
		DIRECTORY
		Demo
		DESTINATION Demo
		COMPONENT brat_runtime
		)
ENDIF(APPLE)



#######################################################
# doc
#######################################################

# doc in mac bundles
#
IF (APPLE)
	INSTALL(
		DIRECTORY
		${BRAT_BINARY_DIR}/doc
		DESTINATION ${RUNTIME_DESTINATION}/../
		COMPONENT brat_runtime
		)
	INSTALL(
		DIRECTORY
		${BRAT_BINARY_DIR}/doc
		DESTINATION ${SCHEDULER_RUNTIME_DESTINATION}/../
		COMPONENT scheduler_runtime
		)
ENDIF(APPLE)





# RETURN()


###################################################################################################



# ---------------------------------
# Cmake modules needed
# ---------------------------------
find_package(Perl)
if(NOT PERL_FOUND)
  message(FATAL_ERROR "Perl program not found. It is necessary for some operations.")
endif(NOT PERL_FOUND)

include(BratUtilities)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFile)
include(TestBigEndian)




if(WIN32)
  link_libraries(ws2_32)
endif(WIN32)

# ---------------------------------
# check for external but provided libraries
# ---------------------------------

# Built-in ZLIB
#
add_subdirectory(vtkzlib)

set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vtkzlib" CACHE INTERNAL "")			# used by netcdf4

set(ZLIB_INCLUDE_DIRS
  "${ZLIB_INCLUDE_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}/vtkzlib"
  )
include_directories(${ZLIB_INCLUDE_DIRS})
if(WIN32)
  set(VTK_EXTLIB vtkzlib)
  set(VTK_EXTLIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/${VTK_EXTLIB}/lib)

  if(MSVC)
    set(ZLIB_LIBRARIES "${VTK_EXTLIB_PATH}/${CMAKE_BUILD_TYPE}/${VTK_EXTLIB}${CMAKE_DEBUG_POSTFIX}.lib")
  else(MSVC)
    set(ZLIB_LIBRARIES "${VTK_EXTLIB_PATH}/${VTK_EXTLIB}${CMAKE_DEBUG_POSTFIX}.lib")
  endif(MSVC)

else(WIN32)
  set(ZLIB_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/vtkzlib/lib/libvtkzlib.a")
endif(WIN32)

# Built-in JPEG
#
add_subdirectory(vtkjpeg)
set(JPEG_INCLUDE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/vtkjpeg"
  "${CMAKE_CURRENT_BINARY_DIR}/vtkjpeg"
  )
include_directories(${JPEG_INCLUDE_DIRS})
if(WIN32)
  set(VTK_EXTLIB vtkjpeg)
  set(VTK_EXTLIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/${VTK_EXTLIB}/lib)

  if(MSVC)
    set(JPEG_LIBRARIES "${VTK_EXTLIB_PATH}/${CMAKE_BUILD_TYPE}/${VTK_EXTLIB}${CMAKE_DEBUG_POSTFIX}.lib")
  else(MSVC)
    set(JPEG_LIBRARIES "${VTK_EXTLIB_PATH}/${VTK_EXTLIB}${CMAKE_DEBUG_POSTFIX}.lib")
  endif(MSVC)

else(WIN32)
#  set(JPEG_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/vtkjpeg/lib/libvtkjpeg.a")
endif(WIN32)

# Built-in TIFF
#
add_subdirectory(vtktiff)
add_dependencies(vtktiff vtkzlib vtkjpeg)
set(TIFF_INCLUDE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/vtktiff"
  "${CMAKE_CURRENT_BINARY_DIR}/vtktiff"
  )
include_directories(${TIFF_INCLUDE_DIRS})
if(WIN32)
  set(VTK_EXTLIB vtktiff)
  set(VTK_EXTLIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/${VTK_EXTLIB}/lib)

  if(MSVC)
    set(TIFF_LIBRARIES "${VTK_EXTLIB_PATH}/${CMAKE_BUILD_TYPE}/${VTK_EXTLIB}${CMAKE_DEBUG_POSTFIX}.lib")
  else(MSVC)
    set(TIFF_LIBRARIES "${VTK_EXTLIB_PATH}/${VTK_EXTLIB}${CMAKE_DEBUG_POSTFIX}.lib")
  endif(MSVC)

else(WIN32)
  set(TIFF_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/vtktiff/lib/libvtktiff.a")
endif(WIN32)



add_subdirectory(externals)

IF (WIN32)
    add_dependencies(libcurl vtkzlib)
ENDIF (WIN32)

# add_dependencies(curl vtkzlib)
add_dependencies(hdf5 vtkzlib szip)
IF (WIN32)
    add_dependencies(netcdf hdf5 hdf5_hl libcurl)
ELSE (WIN32)
    add_dependencies(netcdf hdf5 hdf5_hl)
ENDIF (WIN32)

IF(APPLE)
	add_dependencies(coda vtkzlib szip)
ENDIF(APPLE)

add_dependencies(geotiff vtktiff)
include_directories(${CURL_INCLUDES})
include_directories(${LIBGEOTIFF_INCLUDE_DIRS})
include_directories(${NETCDF_INCLUDE_DIRS})
include_directories(${UDUNITS_INCLUDE_DIRS})
include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${CODA_INCLUDE_DIRS})

if (WIN32)
    set(MAKE_COMMAND nmake)
else (WIN32)
    set(MAKE_COMMAND make)
endif (WIN32)

add_custom_target(externals-build
  COMMENT "Build only external packages from ${CMAKE_CURRENT_SOURCE_DIR}/externals"
  COMMAND ${MAKE_COMMAND}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/externals
)
add_custom_target(externals-install
  COMMENT "Build only external packages from ${CMAKE_CURRENT_SOURCE_DIR}/externals"
  COMMAND  ${MAKE_COMMAND} install
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/externals
)


# The data/ subdirectory

set(CODADEF_FILES
	#data/CRYOSAT-20120130.codadef
	data/CRYOSAT-20150601.codadef
	#data/ENVISAT_RA2MWR-20110125.codadef
	data/ENVISAT_RA2MWR-20120623.codadef
	data/ERS_MWR-20110820.codadef
	data/ERS_RA-20110820.codadef
	data/GFO-20110125.codadef
	data/JASON-20110125.codadef
	data/PODAAC-20110125.codadef
	data/RADS-20080701.codadef
	data/River_Lake-20110125.codadef
	data/Topex_Poseidon-20110125.codadef
  )

set(LIBBRATHL_DATA_FILES
  data/brathl_units.dat
  data/brathl_refmission.txt
  data/brathl_aliasmission.txt
  data/brathl_transposefieldvalues.txt
  data/aliases.xml
  data/aliases.xsd
  data/aliases.xsd.html
  )

set(GUI_DATA_FILES
  data/BratIcon.ico
  data/BratIcon.bmp
  )



# The BRAT core tools library  ############################################################################################################

set(BRAT_TOOLS_SOURCES
    new-gui/Common/tools/stdafx.cpp
    new-gui/Common/tools/Exception.cpp
    new-gui/Common/tools/ExceptionWin.cpp
    new-gui/Common/tools/Trace.cpp
    new-gui/Common/tools/TraceLog.cpp
    new-gui/Common/tools/CoreTypes.cpp
  )

set(BRAT_TOOLS_SOURCES_H
    new-gui/Common/tools/stdafx.h
    new-gui/Common/tools/Exception.h
    new-gui/Common/tools/ExceptionWin.h
    new-gui/Common/tools/Trace.h
    new-gui/Common/tools/TraceLog.h
    new-gui/Common/tools/brathl_error.h
    new-gui/Common/tools/CoreTypes.h
    new-gui/Common/+Utils.h

  )



add_library(brat-tools STATIC ${BRAT_TOOLS_SOURCES} ${BRAT_TOOLS_SOURCES_H})


# The libbrathl/ subdirectory  ############################################################################################################

set(LIBBRATHL_CXX_SOURCES
  libbrathl/Aliases.cpp
  libbrathl/AliasesDictionary.cpp
  libbrathl/BratAlgoFilter.cpp
  libbrathl/BratAlgoFilterKernel.cpp
  libbrathl/BratAlgoFilterGaussian.cpp
  libbrathl/BratAlgoFilterGaussian1D.cpp
  libbrathl/BratAlgoFilterGaussian2D.cpp
  libbrathl/BratAlgoFilterLanczos.cpp
  libbrathl/BratAlgoFilterLanczos1D.cpp
  libbrathl/BratAlgoFilterLanczos2D.cpp
  libbrathl/BratAlgoFilterLoess.cpp
  libbrathl/BratAlgoFilterLoess1D.cpp
  libbrathl/BratAlgoFilterLoess2D.cpp
  libbrathl/BratAlgoFilterMedian.cpp
  libbrathl/BratAlgoFilterMedian1D.cpp
  libbrathl/BratAlgoFilterMedian2D.cpp
  libbrathl/BratAlgorithmBase.cpp
  libbrathl/BratAlgorithmGeosVel.cpp
  libbrathl/BratAlgorithmGeosVelAtp.cpp
  libbrathl/BratAlgorithmGeosVelGrid.cpp
  libbrathl/Criteria.cpp
  libbrathl/CriteriaCycle.cpp
  libbrathl/CriteriaDatetime.cpp
  libbrathl/CriteriaInfo.cpp
  libbrathl/CriteriaLatLon.cpp
  libbrathl/CriteriaPass.cpp
  libbrathl/Date.cpp
  libbrathl/DatePeriod.cpp
  libbrathl/Expression.cpp
  libbrathl/ExternalFiles.cpp
  libbrathl/ExternalFilesNetCDF.cpp
  libbrathl/ExternalFilesAvisoGrid.cpp
  libbrathl/ExternalFilesATP.cpp
  libbrathl/ExternalFilesFactory.cpp
  libbrathl/ExternalFilesJason2.cpp
  libbrathl/Field.cpp
  libbrathl/File.cpp
  libbrathl/FileParams.cpp
  libbrathl/InternalFiles.cpp
  libbrathl/InternalFilesFactory.cpp
  libbrathl/InternalFilesYFX.cpp
  libbrathl/InternalFilesZFXY.cpp
  libbrathl/LatLonPoint.cpp
  libbrathl/LatLonRect.cpp
  libbrathl/List.cpp
  libbrathl/MapParameter.cpp
  libbrathl/Mission.cpp
  libbrathl/NetCDFFiles.cpp
  libbrathl/ObjectTree.cpp
  libbrathl/Parameter.cpp
  libbrathl/Product.cpp
  libbrathl/ProductAop.cpp
  libbrathl/ProductCryosat.cpp
  libbrathl/ProductEnvisat.cpp
  libbrathl/ProductErs.cpp
  libbrathl/ProductErsWAP.cpp
  libbrathl/ProductGfo.cpp
  libbrathl/ProductJason.cpp
  libbrathl/ProductJason2.cpp
  libbrathl/ProductNetCdf.cpp
  libbrathl/ProductNetCdfCF.cpp
  libbrathl/ProductPodaac.cpp
  libbrathl/ProductRads.cpp
  libbrathl/ProductRiverLake.cpp
  libbrathl/ProductTopex.cpp
  libbrathl/ProductTopexSDR.cpp
  libbrathl/ProductReaper.cpp
  libbrathl/Tools.cpp
  libbrathl/TreeField.cpp
  libbrathl/Unit.cpp
  libbrathl/Xml.cpp
  )


# brathl_c  ############################################################################################################
#

set(LIBBRATHL_C_SOURCES
  libbrathl/brathl_c/arg_dbl.c
  libbrathl/brathl_c/arg_end.c
  libbrathl/brathl_c/arg_file.c
  libbrathl/brathl_c/arg_int.c
  libbrathl/brathl_c/arg_lit.c
  libbrathl/brathl_c/arg_rem.c
  libbrathl/brathl_c/arg_str.c
  libbrathl/brathl_c/argtable2.c
  libbrathl/brathl_c/brathlc.cpp
  libbrathl/brathl_c/getopt.c
  libbrathl/brathl_c/getopt1.c
  )

set(LIBBRATHL_C_SOURCES_H
  libbrathl/brathl_c/argtable2.h
  libbrathl/brathl_c/brathlc.h
  libbrathl/brathl_c/getopt.h
  )

include(UseCWithCXX)

include_directories(${BRAT_SOURCE_DIR})
include_directories(${BRAT_SOURCE_DIR}/libbrathl/brathl_c)
set(SAMPLE BratSampleCProgram)
add_executable(${SAMPLE} libbrathl/brathl_c/${SAMPLE}.c)
target_link_libraries(${SAMPLE} brathl)
install(FILES libbrathl/brathl_c/${SAMPLE}.c DESTINATION examples/c)

# brathl_python; 
#	- requires brathl_module, specifically created for Python API support;
#	- in windows, to allow immediate example use without meddling with the 
#		user's system, curl dll is installed also near the example files
#
install(FILES libbrathl/brathl_python/brathl.py DESTINATION bin)
install(FILES libbrathl/brathl_python/example.py DESTINATION examples/python)
install(FILES libbrathl/brathl_python/example.nc DESTINATION examples/python)


# brathl_fortran  ############################################################################################################
#
if(BRATHL_BUILD_FORTRAN)
  include(CheckFortranNames)
  include(UseFortranWithCXX)
  check_fortran_names(BRATHL_FORTRAN_DEFS)
  set(LIBBRATHL_FORTRAN_SOURCES libbrathl/brathl_fortran/brathl_fortran.c)
  set(LIBBRATHL_FORTRAN_SOURCES_H libbrathl/brathl_fortran/brathl_fortran.h)

  set_source_files_properties(${LIBBRATHL_FORTRAN_SOURCES} PROPERTIES COMPILE_FLAGS "${BRATHL_FORTRAN_DEFS}")

  set(SAMPLE BratSampleFortranProgram)
  add_executable(${SAMPLE} libbrathl/brathl_fortran/${SAMPLE}.f)
  target_link_libraries(${SAMPLE} brathl)

  install(FILES libbrathl/brathl_fortran/brathlf.inc DESTINATION include/)
  install(FILES libbrathl/brathl_fortran/BratSampleFortranProgram.f DESTINATION examples/fortran)

endif(BRATHL_BUILD_FORTRAN)

# brathl_idl ############################################################################################################
#
if(BRATHL_BUILD_IDL)

  if(NOT IDL_DIR)
    if($ENV{IDL_DIR} MATCHES ".+")
      file(TO_CMAKE_PATH $ENV{IDL_DIR} IDL_DIR)
    endif($ENV{IDL_DIR} MATCHES ".+")
  endif(NOT IDL_DIR)

  if(IDL_DIR)
    set(IDL_DIR ${IDL_DIR} CACHE STRING "Root directory of an IDL installation" FORCE)
    set(IDL_INSTALL_DIR "idl")
    set(IDL_DEF "${CMAKE_CURRENT_SOURCE_DIR}/libbrathl/brathl_idl/libbrathl_idl.def")

    set(HAVE_IDL_SYSFUN_DEF2 1)
    set(HAVE_IDL_SYSRTN_UNION 1)
    if(WIN32)
      set(IDL_COMPILE_FLAGS "-nologo -I. -I${CODA_BINARY_DIR} -I${CODA_SOURCE_DIR}/libcoda -DWIN32_LEAN_AND_MEAN -DWIN32 -DHAVE_CONFIG_H -DIDL_V5_4")
      set(IDL_LIBDIR "${IDL_DIR}/bin/bin.x86/")
      set(IDL_LIB "${IDL_LIBDIR}/idl.lib")
      set(IDL_LINK_FLAGS "/DEF:${IDL_DEF}")
    else(WIN32)
      # Add the -Wno-write-strings because the IDL export.h header
      # defines a non-const-safe struct that causes tons of warnings.
      if(CMAKE_COMPILER_IS_GNUCXX)
        set(IDL_COMPILE_FLAGS "-Wno-write-strings")
      endif(CMAKE_COMPILER_IS_GNUCXX)
      set(IDL_LINK_FLAGS "-module -shared -avoid-version")
      if(APPLE)
        set(IDL_LINK_FLAGS "${IDL_LINK_FLAGS} -undefined dynamic_lookup")
      endif(APPLE)
    endif(WIN32)

    set(IDL_INCLUDE_DIR "${IDL_DIR}/external")
    include_directories(${IDL_INCLUDE_DIR})

    set(LIBBRATHL_IDL_SOURCES
      libbrathl/brathl_idl/libbrathl_idl.cpp
      )
    set_source_files_properties(${LIBBRATHL_IDL_SOURCES}
      PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} ${IDL_COMPILE_FLAGS}"
      )
    # Visual Studio does not like targets with a dash in the
    # name, hence brathl_idl rather than coda-idl.
    add_library(brathl_idl MODULE
      ${LIBBRATHL_IDL_SOURCES}
      )
    target_link_libraries(brathl_idl brathl ${IDL_LIB})
    set_target_properties(brathl_idl
      PROPERTIES LINK_FLAGS "${IDL_LINK_FLAGS}"
      OUTPUT_NAME "brathl-idl"
      PREFIX ""
      )

    install(TARGETS brathl_idl DESTINATION ${IDL_INSTALL_DIR})

    configure_file(
      ${BRAT_SOURCE_DIR}/libbrathl/brathl_idl/libbrathl_idl.dlm.in
      ${BRAT_BINARY_DIR}/libbrathl/brathl_idl/brathl-idl.dlm
      )

    install(FILES ${BRAT_BINARY_DIR}/libbrathl/brathl_idl/brathl-idl.dlm DESTINATION ${IDL_INSTALL_DIR})
    install(FILES libbrathl/brathl_idl/examples.pro DESTINATION examples/idl)

  else(IDL_DIR)
    message(FATAL_ERROR "IDL not found. Try setting the IDL_DIR environment or CMake variable to the root directory of an IDL installation.")
  endif(IDL_DIR)

endif(BRATHL_BUILD_IDL)  ############################################################################################################


# ------------------------------------------------------
# Just to setup 'Header Files' file tab in Visual Studio
# ------------------------------------------------------
string(REGEX REPLACE "\\.cpp" .h LIBBRATHL_CXX_SOURCES_H
  "${LIBBRATHL_CXX_SOURCES}")

set(LIBBRATHL_CXX_SOURCES_H
  ${LIBBRATHL_CXX_SOURCES_H}
  libbrathl/brathl.h
  new-gui/Common/tools/brathl_error.h
  )

set(LIBBRATHL_SOURCES_H
  ${LIBBRATHL_CXX_SOURCES_H}
  ${LIBBRATHL_C_SOURCES_H}
  ${LIBBRATHL_FORTRAN_SOURCES_H})

if(CMAKE_COMPILER_IS_GNUCXX)
  # Only for C++
  set_source_files_properties(
    ${LIBBRATHL_CXX_SOURCES}
    ${LIBBRATHL_C_SOURCES}
    PROPERTIES COMPILE_FLAGS ${BRAT_GNU_OPTIONS})

  set_source_files_properties(
    libbrathl/List.cpp
    PROPERTIES COMPILE_FLAGS "-Wall")
endif(CMAKE_COMPILER_IS_GNUCXX)


include_directories(
  ${BRAT_BINARY_DIR})


set(CREATED)

include_directories(${BRAT_SOURCE_DIR}/libbrathl)
include_directories(${BRAT_BINARY_DIR}/libbrathl)

set(ALL_BRATHL_SOURCES
  ${LIBBRATHL_CXX_SOURCES}
  ${LIBBRATHL_SOURCES_H}
  ${LIBBRATHL_C_SOURCES}
  ${LIBBRATHL_FORTRAN_SOURCES})

add_library(brathl STATIC ${ALL_BRATHL_SOURCES})

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(brathl brat-tools netcdf coda_static hdf5 ssh2 udunits)
else()
    target_link_libraries(brathl brat-tools netcdf coda_static hdf5 udunits)
endif()

# required by brathl_python
#
add_library(brathl_module MODULE ${ALL_BRATHL_SOURCES})
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(brathl_module brat-tools netcdf coda_static hdf5 ssh2 udunits)
else()
    target_link_libraries(brathl_module brat-tools netcdf coda_static hdf5 udunits)
endif()

set_target_properties(brathl_module PROPERTIES 
	OUTPUT_NAME brathl_module
	DEFINE_SYMBOL LIBRATHLDLL_EXPORTS
	DEBUG_POSTFIX ""
	PREFIX "")

IF(WIN32)
	set_target_properties(brathl_module PROPERTIES COMPILE_FLAGS "/D LIBRATHLDLL")
ELSE(WIN32)
    IF(APPLE)
	    set_target_properties(brathl_module PROPERTIES COMPILE_FLAGS "-fvisibility=default")
	    set_target_properties(brathl_module PROPERTIES LINK_FLAGS "-fvisibility=default")
    ENDIF(APPLE)
	set_target_properties(brathl_module PROPERTIES SUFFIX "")
ENDIF(WIN32)



install(TARGETS brathl DESTINATION lib/)
install(TARGETS brathl_module DESTINATION bin/)				# see also apple section to install brathl_module in mac
install(FILES ${LIBBRATHL_SOURCES_H} DESTINATION include)


# Add CODA/EXPAT definitions (XML parsing within brathl)
add_definitions(-DCODA_EXPAT_NAME_MANGLE=1)
add_definitions(-DXML_LARGE_SIZE=1)
add_definitions(-DXML_DTD=1)
add_definitions(-DXML_NS=1)
add_definitions(-DXML_CONTEXT_BYTES=1024)
include_directories(${CODA_SOURCE_DIR}/libcoda/expat)


# brathl_matlab ###########################################################################################

if(BRATHL_BUILD_MATLAB)

  # If not in cache, try from environment
  if(NOT MATLAB_DIR)
    if($ENV{MATLAB_DIR} MATCHES ".+")
      file(TO_CMAKE_PATH $ENV{MATLAB_DIR} MATLAB_DIR)
    endif($ENV{MATLAB_DIR} MATCHES ".+")
  endif(NOT MATLAB_DIR)

  if(MATLAB_DIR)

    if(NOT EXISTS "${MATLAB_DIR}")
      message(FATAL_ERROR "Specified MATLAB directory '${MATLAB_DIR}' does not exist")
    endif(NOT EXISTS "${MATLAB_DIR}")

    if(NOT IS_DIRECTORY "${MATLAB_DIR}")
      message(FATAL_ERROR "Specified MATLAB directory '${MATLAB_DIR}' exists, but is not a directory")
    endif(NOT IS_DIRECTORY "${MATLAB_DIR}")

    set(MATLAB_DIR ${MATLAB_DIR} CACHE STRING "Root directory of MATLAB installation" FORCE)
    set(MATLAB_INSTALL_DIR "matlab")

    find_path(MATLAB_INCLUDE_DIR "mex.h" ${MATLAB_DIR}/extern/include)
    mark_as_advanced(MATLAB_INCLUDE_DIR)

    set(LIBBRATHL_MATLAB_SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/libbrathl/brathl_matlab/brathl_matlab.cpp
      )

    # Configure the mex compiler
    set(MEXFLAGS -I${MATLAB_INCLUDE_DIR} -I. -I${CMAKE_CURRENT_BINARY_DIR}/libbrathl -I${CODA_BINARY_DIR} -I${CODA_SOURCE_DIR}/libcoda -I${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}/libbrathl -I${CMAKE_CURRENT_SOURCE_DIR}/libbrathl/brathl_c -DSTL_USING_ALL)

    # Note: The explicit mex.bat assignment is necessary because
    # only with the .bat attached will name invoke the command
    # using 'call' -- and only that works if the path name
    # contains spaces (as in 'Program Files', the default for
    # MATLAB 2007 and beyond). So relying (as we did before) on
    # the fact that on Windows you can theoretically also invoke
    # a batchfile without the extension will fail in that case.
    if(WIN32)
      set(MEX "${MATLAB_DIR}/bin/mex.bat")
    #  set(MEXEXT "mexw32")
      set(MEXFLAGS ${MEXFLAGS} COMPFLAGS="-c -MD -Zp8 -GR -W3 -EHs -DMATLAB_MEX_FILE -nologo" -DWIN32_LEAN_AND_MEAN -DWIN32 -DHAVE_CONFIG_H -v)
    else(WIN32)
      set(MEX "${MATLAB_DIR}/bin/mex")
      set(MEXEXT_PROG "mexext")
      set(MEXFLAGS ${MEXFLAGS} -g -O)
      file(TO_NATIVE_PATH "${MATLAB_DIR}/bin/${MEXEXT_PROG}" MEXEXT_CMD)
      execute_process(
        COMMAND "${MEXEXT_CMD}"
        OUTPUT_VARIABLE MEXEXT
        )
      string(STRIP ${MEXEXT} MEXEXT)
    endif(WIN32)

    set(MEX_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/brathl_matlab.${MEXEXT})

    # Because we are essentially bypassing CMake with the mex command,
    # we need to specify exactly where the build libraries can be found.
    # Since the user may have overridden CMAKE_ARCHIVE_OUTPUT_DIRECTORY,
    # we have to take that possibility into account. Just prefixing is
    # not enough, because the variable can also be empty, and then you'd
    # get names starting with a slash.
    set(INT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    set(CODA_INT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/coda/${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    set(NETCDF_INT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/netcdf-c/liblib/${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    set(UDUNITS_INT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/udunits/${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    set(HDF5_INT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/hdf5/src/${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    set(HDF5_HL_INT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/hdf5/hl/src/${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    set(CURL_INT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/curl/lib/${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    set(SZIP_INT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/SZip/src/${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

    set(HDF5_LIBRARIES 
		${HDF5_INT_LIB_DIR}/libhdf5.lib 
		${HDF5_HL_INT_LIB_DIR}/libhdf5_hl.lib 
		${CURL_INT_LIB_DIR}/libcurl_imp.lib
		${SZIP_INT_LIB_DIR}/libszip.lib
		${ZLIB_LIBRARIES}
		)
	
    if(WIN32)
      set(MEX_CMD ${MEX} ${MEXFLAGS} ${LIBBRATHL_MATLAB_SOURCE_FILES} ${INT_LIB_DIR}/brathl.lib ${INT_LIB_DIR}/brat-tools.lib ${CODA_INT_LIB_DIR}/libcoda.lib ${NETCDF_INT_LIB_DIR}/netcdf.lib ${UDUNITS_INT_LIB_DIR}/udunits.lib ${HDF4_LIBRARIES} ${HDF5_LIBRARIES}  -output ${MEX_OUTPUT})
    else(WIN32)
      set(MEX_CMD ${MEX} ${MEXFLAGS} ${LIBBRATHL_MATLAB_SOURCE_FILES} ${INT_LIB_DIR}/libbrathl.a ${INT_LIB_DIR}/libbrat-tools.a ${CODA_INT_LIB_DIR}/libcoda.a ${NETCDF_INT_LIB_DIR}/libnetcdf.a ${UDUNITS_INT_LIB_DIR}/libudunits.a ${HDF4_LIBRARIES} ${HDF5_LIBRARIES} -o ${MEX_OUTPUT})
    endif(WIN32)

    add_custom_command(
      COMMENT "Compiling the BRATHL MATLAB interface..."
      DEPENDS ${LIBBRATHL_MATLAB_SOURCE_FILES}
      OUTPUT ${MEX_OUTPUT}
      COMMAND ${MEX_CMD}
      )

    add_custom_target(brathl_matlab ALL DEPENDS
      ${MEX_OUTPUT}
      )
    add_dependencies(brathl_matlab brathl )

    install(FILES ${MEX_OUTPUT} DESTINATION ${MATLAB_INSTALL_DIR})

    # *** matlab data ***

    set(LIBBRATHL_MATLAB_MFILES
      libbrathl/brathl_matlab/brathl_CreateStruct.m
      libbrathl/brathl_matlab/brathl_Cycle2YMDHMSM.m
      libbrathl/brathl_matlab/brathl_DSM2Julian.m
      libbrathl/brathl_matlab/brathl_DSM2Seconds.m
      libbrathl/brathl_matlab/brathl_DSM2YMDHMSM.m
      libbrathl/brathl_matlab/brathl_DayOfYear.m
      libbrathl/brathl_matlab/brathl_DiffDSM.m
      libbrathl/brathl_matlab/brathl_DiffJulian.m
      libbrathl/brathl_matlab/brathl_DiffYMDHMSM.m
      libbrathl/brathl_matlab/brathl_Julian2DSM.m
      libbrathl/brathl_matlab/brathl_Julian2Seconds.m
      libbrathl/brathl_matlab/brathl_Julian2YMDHMSM.m
      libbrathl/brathl_matlab/brathl_NowYMDHMSM.m
      libbrathl/brathl_matlab/brathl_ReadData.m
      libbrathl/brathl_matlab/brathl_Seconds2DSM.m
      libbrathl/brathl_matlab/brathl_Seconds2Julian.m
      libbrathl/brathl_matlab/brathl_Seconds2YMDHMSM.m
      libbrathl/brathl_matlab/brathl_SetRefUser1.m
      libbrathl/brathl_matlab/brathl_SetRefUser2.m
      libbrathl/brathl_matlab/brathl_YMDHMSM2Cycle.m
      libbrathl/brathl_matlab/brathl_YMDHMSM2DSM.m
      libbrathl/brathl_matlab/brathl_YMDHMSM2Julian.m
      libbrathl/brathl_matlab/brathl_YMDHMSM2Seconds.m
      libbrathl/brathl_matlab/brathl_test.m
      )

    install(FILES ${LIBBRATHL_MATLAB_MFILES} DESTINATION ${MATLAB_INSTALL_DIR})
    install(FILES libbrathl/brathl_matlab/examples.m DESTINATION examples/matlab)

  else(MATLAB_DIR)
    message(FATAL_ERROR "MATLAB not found. Try setting the MATLAB_DIR environment or CMake variable to the root directory of a MATLAB installation.")
  endif(MATLAB_DIR)


endif(BRATHL_BUILD_MATLAB) ###############################################################################





# Check for Qt ###########################################################################################

SET(QT_MIN_VERSION 4.8.0)
SET (ENABLE_QT5 FALSE CACHE BOOL "If enabled will try to find Qt5 before looking for Qt4")
IF (ENABLE_QT5)
	FIND_PACKAGE(Qt5Core QUIET)
	FIND_PACKAGE(Qt5Gui REQUIRED)
	FIND_PACKAGE(Qt5Widgets REQUIRED)
	FIND_PACKAGE(Qt5Network REQUIRED)
	FIND_PACKAGE(Qt5Xml REQUIRED)
	FIND_PACKAGE(Qt5Svg REQUIRED)
	FIND_PACKAGE(Qt5Concurrent REQUIRED)
	FIND_PACKAGE(Qt5PrintSupport REQUIRED)
	FIND_PACKAGE(Qt5WebKit REQUIRED)
	FIND_PACKAGE(Qt5WebKitWidgets REQUIRED)
	FIND_PACKAGE(Qt5Test REQUIRED)
	FIND_PACKAGE(Qt5UiTools REQUIRED)
	FIND_PACKAGE(Qt5Script REQUIRED)
	FIND_PACKAGE(Qt5Sql REQUIRED)
	SET(QT5_BUILD TRUE)
	INCLUDE("cmake/modules/ECMQt4To5Porting.cmake")
	MESSAGE(STATUS "Found Qt version: ${Qt5Core_VERSION_STRING}")
ELSE()
	FIND_PACKAGE(Qt4 ${QT_MIN_VERSION} REQUIRED)
	SET(QT_USE_QTXML 1)
	SET(QT_USE_QTNETWORK 1)
	SET(QT_USE_QTSVG 1)
	SET(QT_USE_QTSQL 1)
	SET(QT_USE_QTWEBKIT 1)
	IF (NOT QT_QTXML_FOUND OR NOT QT_QTNETWORK_FOUND OR NOT QT_QTSVG_FOUND OR NOT QT_QTSQL_FOUND OR NOT QT_QTWEBKIT_FOUND OR (WITH_CUSTOM_WIDGETS AND NOT QT_QTDESIGNER_FOUND))
	MESSAGE(SEND_ERROR "Some Qt4 modules haven't been found!")
	ENDIF (NOT QT_QTXML_FOUND OR NOT QT_QTNETWORK_FOUND OR NOT QT_QTSVG_FOUND OR NOT QT_QTSQL_FOUND OR NOT QT_QTWEBKIT_FOUND OR (WITH_CUSTOM_WIDGETS AND NOT QT_QTDESIGNER_FOUND))

	INCLUDE( ${QT_USE_FILE} )

	MESSAGE(STATUS "Found Qt version: ${QTVERSION}")
ENDIF()


SET(QT_USE_QTOPENGL 1)
INCLUDE_DIRECTORIES( 
	${QT_INCLUDE_DIR} 
	${QT_INCLUDE_DIR}/QtCore
	${QT_INCLUDE_DIR}/QtGui
	${QT_INCLUDE_DIR}/QtXml
	${QT_INCLUDE_DIR}/QtOpenGL
	${QT_INCLUDE_DIR}/ActiveQt
	${QT_INCLUDE_DIR}/QtNetwork
	new-gui/Common
	)


set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)




# process ##########################################################################################################


set(PROCESS_LIB_SOURCES
  process/BratProcess.cpp
  process/BratProcessExportAscii.cpp
  process/BratProcessStats.cpp
  process/ProcessCommonTools.cpp
  new-gui/Common/ApplicationStaticPaths.cpp
  )


set(PROCESS_LIB_SOURCES_H  
    process/BratProcess.h
    process/BratProcessExportAscii.h
    process/BratProcessStats.h
    process/ProcessCommonTools.h
    new-gui/Common/ApplicationStaticPaths.h
	)

set(PROCESS_SOURCES "")

macro(DEFINE_PROCESS PNAME)
  set(PROCESS_SOURCES "process/${PNAME}.cpp")
  add_executable(${PNAME} "${PROCESS_SOURCES}")
  target_link_libraries(${PNAME} brathlProcessUtilities brathl ${QT_QTCORE_LIBRARY} ${ADDITIONAL_LIBS})
  if (APPLE)
	  set_target_properties(
				${PNAME} PROPERTIES
				LINK_FLAGS "-rpath @executable_path/../Frameworks/"
				)
  endif(APPLE)
  install(TARGETS ${PNAME} DESTINATION bin/)
endmacro(DEFINE_PROCESS)


DEFINE_PROCESS(BratExportAscii)
DEFINE_PROCESS(BratShowInternalFile)
DEFINE_PROCESS(BratStats)
DEFINE_PROCESS(BratListFieldNames)
DEFINE_PROCESS(BratCheckAliases)



set(ADDITIONAL_PROCESS_SOURCES "")

macro(DEFINE_PROCESS_WITH_PYTHON PNAME)
  set(PROCESS_SOURCES "process/${PNAME}.cpp" process/BratEmbeddedPythonProcess.h process/BratEmbeddedPythonProcess.cpp ${ADDITIONAL_PROCESS_SOURCES} )
  add_executable(${PNAME} "${PROCESS_SOURCES}")
  target_link_libraries(${PNAME} brathlProcessUtilities brathl ${QT_QTCORE_LIBRARY} ${ADDITIONAL_LIBS})
  if (APPLE)
	  set_target_properties(
				${PNAME} PROPERTIES
				LINK_FLAGS "-rpath @executable_path/../Frameworks/"
				)
  endif(APPLE)
  install(TARGETS ${PNAME} DESTINATION bin/)
endmacro(DEFINE_PROCESS_WITH_PYTHON)



# Embedded Python #

set(PY_VER_SUFFIX 3.2)

include_directories(

	${PYTHON_INCLUDE_DIR}
)


IF (NOT WIN32)
	SET( PYTHON_IMPORT_LIBRARY ${PYTHON_LIBRARY})
ENDIF (NOT WIN32)

set(ADDITIONAL_LIBS ${PYTHON_IMPORT_LIBRARY})
IF(UNIX AND NOT APPLE)
	set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} util expat )
ENDIF(UNIX AND NOT APPLE)

DEFINE_PROCESS(RunPythonAlgorithm)

add_custom_target(run_py_algo DEPENDS RunPythonAlgorithm)


SET( ADDITIONAL_PROCESS_SOURCES process/BratProcessYFX.cpp process/BratProcessYFX.h )
DEFINE_PROCESS_WITH_PYTHON( BratCreateYFX )

SET( ADDITIONAL_PROCESS_SOURCES process/BratProcessZFXY.cpp process/BratProcessZFXY.h )
DEFINE_PROCESS_WITH_PYTHON( BratCreateZFXY )


###################





#set(ADDITIONAL_LIBS ${LIBGEOTIFF_LIBRARIES} ${JPEG_LIBRARIES} ${ZLIB_LIBRARIES})
set(ADDITIONAL_LIBS geotiff vtktiff vtkjpeg vtkzlib ${QT_QTCORE_LIBRARY})
DEFINE_PROCESS(BratExportGeoTiff)

add_custom_target(command-line-tools DEPENDS 
  BratExportAscii
  BratCreateYFX 
  BratCreateZFXY
  BratShowInternalFile
  BratStats
  BratListFieldNames
  BratCheckAliases)

# install(TARGETS command_line_tools DESTINATION bin/)
add_custom_target(command-line-tools-install
  COMMENT "Build only command line tools"
  COMMAND  ${MAKE_COMMAND} install command-line-tools
)



if(CMAKE_COMPILER_IS_GNUCXX)
  # Only for C++
  set_source_files_properties(
    ${PROCESS_LIB_SOURCES} ${PROCESS_SOURCES}
    PROPERTIES COMPILE_FLAGS "${BRAT_GNU_OPTIONS}")
endif(CMAKE_COMPILER_IS_GNUCXX)

add_library(brathlProcessUtilities STATIC ${PROCESS_LIB_SOURCES} ${PROCESS_LIB_SOURCES_H})





# GUI Tools ###########################################################################################################
#######################################################################################################################

if(BRAT_BUILD_GUI)

	SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH})

	SET(XERCES_MIN_VERSION 3.1.1)
    FIND_PACKAGE(XercesC ${XERCES_MIN_VERSION} REQUIRED)
	IF (NOT XercesC_FOUND)
		MESSAGE(SEND_ERROR "Xerces not found!")
	ELSE()
		INCLUDE_DIRECTORIES(${XercesC_INCLUDE_DIRS} ${XSD_INCLUDE_DIR})
	ENDIF()

	
#	if(VTK_USE_X)
#	find_package(GTK REQUIRED)
#	if(GTK_FOUND)
#		set(CMAKE_CXX_FLAGS "${CMAKE_GTK_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
#		add_definitions(-DWXVTK_GTK)
#	endif(GTK_FOUND)
#	endif(VTK_USE_X)


	FIND_PACKAGE(GEOS REQUIRED)
	INCLUDE_DIRECTORIES(
		${GEOS_INCLUDE_DIR}
	)

	SET(QGIS_MIN_VERSION 2.8.3)
	FIND_PACKAGE(QGIS ${QGIS_MIN_VERSION} REQUIRED)
	IF (QGIS_FOUND)
		INCLUDE_DIRECTORIES( ${QGIS_INCLUDE_DIR} )
		IF(APPLE)
			INCLUDE_DIRECTORIES( ${QGIS_GUI_INCLUDE_DIR} ${QGIS_ANALYSIS_INCLUDE_DIR} )
		ELSEIF(UNIX)
			INCLUDE_DIRECTORIES( /usr/include/gdal )
		ENDIF(APPLE)
	ENDIF(QGIS_FOUND)

	FIND_PACKAGE(Qwt REQUIRED)
	INCLUDE_DIRECTORIES(
		${QWT_INCLUDE_DIR}
	)

	FIND_PACKAGE(QwtPlot3d REQUIRED)
		INCLUDE_DIRECTORIES(
		${QWTPLOT3D_INCLUDE_DIR}
	)

	FIND_PACKAGE(OSG REQUIRED)

	FIND_PACKAGE(OsgEarth REQUIRED)
	IF (OSGEARTHQT_LIBRARY)
		# following variable is used in qgsconfig.h
		SET(HAVE_OSGEARTHQT TRUE)
	ENDIF (OSGEARTHQT_LIBRARY)

	FIND_PACKAGE(Threads)

	INCLUDE_DIRECTORIES(
		${OSGEARTH_INCLUDE_DIR}
		${OSG_INCLUDE_DIR}
	)

	INCLUDE_DIRECTORIES(
		${OSGEARTH_SOURCE_DIR}/src
		${OSG_INCLUDE_DIR}
	)


	# scheduler ########################################################################################################

	# -----------------------------------------------------
	# Set scheduler sources
	# -----------------------------------------------------
	set(SCHEDULER_SOURCES
	    new-gui/scheduler/stdafx.cpp
		new-gui/scheduler/main.cpp
		new-gui/scheduler/SchedulerDialog.cpp
		new-gui/scheduler/SchedulerApplication.cpp
		new-gui/scheduler/EditTasksFileDialog.cpp
		new-gui/Common/DataModels/TaskProcessor.cpp
		new-gui/Common/DataModels/BratTask.cpp
		new-gui/Common/ApplicationStaticPaths.cpp
		new-gui/Common/GUI/ApplicationUserPaths.cpp
		new-gui/Common/ApplicationLogger.cpp
		new-gui/scheduler/SchedulerLogger.cpp
		new-gui/Common/GUI/TextWidget.cpp
		new-gui/Common/GUI/BasicLogShell.cpp
		new-gui/Common/GUI/ProcessesTable.cpp
		new-gui/Common/GUI/ControlPanel.cpp
		new-gui/Common/SingleApplication/qtsingleapplication.cpp
		new-gui/Common/SingleApplication/qtlocalpeer.cpp
		new-gui/Common/SingleApplication/qtlockedfile.cpp
	)


	# -----------------------------------------------------
	# Just to setup 'Header Files' file tab in Visual Studio
	# -----------------------------------------------------

	set(SCHEDULER_SOURCES_H
	    new-gui/scheduler/stdafx.h
		new-gui/Common/BratVersion.h
		new-gui/Common/+Utils.h
		new-gui/Common/QtUtilsIO.h
		new-gui/Common/QtStringUtils.h
		new-gui/Common/QtUtils.h
		new-gui/scheduler/SchedulerDialog.h
		new-gui/scheduler/SchedulerApplication.h
		new-gui/scheduler/EditTasksFileDialog.h
		new-gui/Common/ScheduledTasksList.hxx
		new-gui/Common/DataModels/TaskProcessor.h
		new-gui/Common/DataModels/BratTask.h
		new-gui/Common/ApplicationStaticPaths.h
		new-gui/Common/GUI/ApplicationUserPaths.h
		new-gui/Common/ApplicationLogger.h
		new-gui/scheduler/SchedulerLogger.h
		new-gui/Common/XmlSerializer.h
		new-gui/Common/GUI/TextWidget.h
		new-gui/Common/GUI/BasicLogShell.h
		new-gui/Common/GUI/ProcessesTable.h
		new-gui/Common/GUI/ControlPanel.h
		new-gui/Common/SingleApplication/qtsingleapplication.h
		new-gui/Common/SingleApplication/qtlocalpeer.h
		new-gui/Common/SingleApplication/qtlockedfile.h
	)

	QT4_WRAP_UI(schedulerUISrcs new-gui/scheduler/SchedulerDialog.ui)
	QT4_ADD_RESOURCES( SCHEDULER_RCC_SOURCES new-gui/Common/Resources/brat.qrc OPTIONS -o ${BRAT_SOURCE_DIR}/new-gui/scheduler/tmp/qrc_brat.cpp )

	  
	# -----------------------------------------------------
	# Add RC file in WIN32
	# Add 'Resource Files' file tab in Visual Studio
	# -----------------------------------------------------
	if(WIN32)
		set(SCHEDULER_SOURCES_RC new-gui/Common/Resources/scheduler.rc)
		SOURCE_GROUP("Resource Files" FILES ${SCHEDULER_SOURCES_RC})
	endif(WIN32)

	if(APPLE)
		set(SCHEDULER_BUNDLE_DATA_FILES ${GUI_DATA_FILES} ${CODADEF_FILES} ${LIBBRATHL_DATA_FILES})
		set_source_files_properties(${SCHEDULER_BUNDLE_DATA_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION data)
		set_source_files_properties(new-gui/Common/Resources/brat.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		set(SCHEDULER_SOURCES_RC ${SCHEDULER_BUNDLE_DATA_FILES} new-gui/Common/Resources/brat.icns)
	endif(APPLE)

	include_directories(
		${BRAT_SOURCE_DIR}/process 
    )
  
	
	add_executable(scheduler
	WIN32 MACOSX_BUNDLE
	${SCHEDULER_SOURCES} ${SCHEDULER_SOURCES_RC} ${SCHEDULER_RCC_SOURCES} ${SCHEDULER_SOURCES_H} ${schedulerUISrcs} )
	# ${schedulerMOCSrcs} 

	target_include_directories( scheduler BEFORE PRIVATE new-gui/scheduler )
	
	target_link_libraries( scheduler 
		${QT_QTCORE_LIBRARY} 
		${QT_QTMAIN_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
		${XercesC_LIBRARIES} 
		brathlProcessUtilities 
		brathl 
	)
	
	target_compile_definitions( scheduler 
		PRIVATE
		-DUNICODE
		-D_UNICODE
		)
	
	install(TARGETS scheduler DESTINATION bin BUNDLE DESTINATION .)
	

	if(APPLE)
		add_dependencies(scheduler BratCreateYFX BratCreateZFXY BratExportAscii BratExportGeoTiff BratStats)
		set_target_properties(
			scheduler PROPERTIES
			MACOSX_BUNDLE_ICON_FILE brat
			MACOSX_BUNDLE_BUNDLE_NAME scheduler
			LINK_FLAGS "-rpath @executable_path/../Frameworks/"
			)
		add_custom_command(TARGET scheduler
			POST_BUILD COMMAND
			COMMAND cp -f ../../BratCreateYFX MacOS/BratCreateYFX
			COMMAND cp -f ../../BratCreateZFXY MacOS/BratCreateZFXY
			COMMAND cp -f ../../BratExportAscii MacOS/BratExportAscii
			COMMAND cp -f ../../BratExportGeoTiff MacOS/BratExportGeoTiff
			COMMAND cp -f ../../BratStats MacOS/BratStats
			WORKING_DIRECTORY ${BRAT_BINARY_DIR}/bin/scheduler.app/Contents)
	endif(APPLE)
   
	add_dependencies( scheduler scheduler_runtime )



	# brat ##############################################################################################################


	# -----------------------------------------------------
	# Set brat sources
	# -----------------------------------------------------
	set(GUI_SOURCES
		new-gui/brat/main.cpp
		new-gui/brat/BratApplication.cpp
		new-gui/brat/BratMainWindow.cpp
		new-gui/brat/stdafx.cpp
		new-gui/brat/DataModels/MapTypeDisp.cpp
		new-gui/brat/GUI/DisplayEditors/AbstractDisplayEditor.cpp
		new-gui/brat/DataModels/Workspaces/Dataset.cpp
		new-gui/brat/DataModels/Workspaces/Operation.cpp
		new-gui/brat/DataModels/Workspaces/Formula.cpp
		new-gui/brat/DataModels/Workspaces/Function.cpp
		new-gui/brat/DataModels/Workspaces/Display.cpp
		new-gui/brat/DataModels/Workspaces/Workspace.cpp
		new-gui/brat/DataModels/Workspaces/TreeWorkspace.cpp
		new-gui/brat/GUI/DisplayWidgets/Globe/qgsosgearthtilesource.cpp
		new-gui/brat/GUI/DisplayWidgets/2DPlotWidget.cpp
		new-gui/brat/GUI/DisplayWidgets/3DPlotWidget.cpp
		new-gui/brat/GUI/DisplayWidgets/GlobeWidget.cpp
		new-gui/brat/GUI/DisplayWidgets/MapWidget.cpp
		new-gui/brat/GUI/DesktopManager.cpp
		new-gui/Common/GUI/TextWidget.cpp
		new-gui/brat/GUI/TabbedDock.cpp
		new-gui/Common/GUI/ControlPanel.cpp
		new-gui/brat/GUI/ControlPanels/DatasetBrowserControls.cpp
		new-gui/brat/DataModels/Model.cpp
		new-gui/brat/GUI/ActionsTable.cpp
		new-gui/Common/ConfigurationKeywords.cpp
		new-gui/Common/FileSettings.cpp
		new-gui/Common/ApplicationSettings.cpp
		new-gui/brat/BratSettings.cpp
		new-gui/brat/DataModels/Workspaces/WorkspaceSettings.cpp
		new-gui/brat/GUI/WorkspaceDialog.cpp
		new-gui/brat/GUI/WorkspaceElementsDialog.cpp
		new-gui/brat/ApplicationSettingsDlg.cpp
		new-gui/Common/GUI/ApplicationUserPaths.cpp
		new-gui/brat/ApplicationPaths.cpp
		new-gui/Common/ApplicationLogger.cpp
		new-gui/brat/BratLogger.cpp
		new-gui/brat/GUI/LogShell.cpp
		new-gui/brat/DataModels/PlotData/MapProjection.cpp
		new-gui/brat/DataModels/PlotData/Plots.cpp
		new-gui/brat/DataModels/PlotData/PlotColor.cpp
		new-gui/brat/DataModels/PlotData/GeoPlotField.cpp
		new-gui/brat/DataModels/DisplayFilesNetCDFParameters.cpp
		new-gui/brat/DataModels/DisplayFilesProcessor.cpp
		new-gui/brat/DataModels/PlotData/XYPlotField.cpp
		new-gui/brat/DataModels/PlotData/ZFXYPlotField.cpp
		new-gui/brat/DataModels/PlotData/MapColor.cpp
		new-gui/brat/DataModels/PlotData/PlotValues.cpp
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapDecorationGrid.cpp
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapDecorationItem.cpp
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapToolMeasure.cpp
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapToolMeasureDialog.cpp
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapToolSelectPolygon.cpp
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapToolSelectRectangle.cpp
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapToolSelectUtils.cpp
		new-gui/brat/GUI/ControlPanels/DatasetFilterControls.cpp
		new-gui/brat/GUI/DisplayWidgets/Globe/GlobeControls.cpp
		new-gui/brat/GUI/ControlPanels/OperationControls.cpp
		new-gui/Common/GUI/ProcessesTable.cpp
		new-gui/brat/GUI/DisplayEditors/MapEditor.cpp
		new-gui/brat/GUI/DisplayEditors/PlotEditor.cpp
		new-gui/brat/GUI/ControlPanels/OperationControls-Quick.cpp
		new-gui/brat/GUI/ControlPanels/Views/ViewControlPanels.cpp
		new-gui/brat/GUI/ControlPanels/Views/ViewControlPanelsCommon.cpp
		new-gui/brat/GUI/ControlPanels/DataExpressionsTree.cpp
		new-gui/brat/GUI/WorkspaceViewsDialog.cpp
		new-gui/brat/DataModels/Filters/BratFilters.cpp
		new-gui/brat/GUI/ControlPanels/Dialogs/FormulaDialog.cpp
		new-gui/brat/GUI/ControlPanels/Dialogs/InsertAlgorithmDialog.cpp
		new-gui/brat/GUI/ControlPanels/Dialogs/InsertFunctionDialog.cpp
		new-gui/brat/GUI/ControlPanels/Dialogs/ShowAliasesDialog.cpp
		new-gui/brat/GUI/ControlPanels/Dialogs/ShowInfoDialog.cpp
		new-gui/brat/GUI/ControlPanels/Dialogs/DelayExecutionDialog.cpp
		new-gui/brat/GUI/ControlPanels/Dialogs/ExportDialog.cpp
		new-gui/brat/GUI/ControlPanels/Dialogs/RegionSettingsDialog.cpp
		new-gui/brat/DataModels/Filters/BratAreas.cpp
		new-gui/brat/GUI/ControlPanels/Dialogs/SelectRecordDialog.cpp
		new-gui/brat/GUI/DisplayWidgets/Histogram.cpp
		new-gui/brat/GUI/ActiveViewsDialog.cpp
		new-gui/brat/GUI/StackedWidget.cpp
		new-gui/brat/DataModels/PlotData/BratLookupTable.cpp
		new-gui/brat/DataModels/PlotData/BratLookupTableSettings.cpp
		new-gui/brat/GUI/ControlPanels/Dialogs/EditExportAsciiDialog.cpp
		new-gui/brat/GUI/ControlPanels/ColorMapWidget.cpp
		new-gui/Common/DataModels/BratTask.cpp
		new-gui/Common/DataModels/TaskProcessor.cpp
		new-gui/brat/GUI/DisplayEditors/Dialogs/ExportImageDialog.cpp
		process/BratEmbeddedPythonProcess.cpp
		new-gui/brat/GUI/ControlPanels/DesktopControlPanel.cpp
		new-gui/brat/DataModels/PlotData/Contour/BratContour.cpp
		new-gui/brat/DataModels/PlotData/Contour/ListContour.cpp
		new-gui/brat/GUI/ControlPanels/Dialogs/DataDisplayPropertiesDialog.cpp
		new-gui/brat/GUI/ControlPanels/ColorButton.cpp
		new-gui/brat/GUI/DisplayWidgets/SublayersDialog.cpp
		new-gui/brat/GUI/DisplayWidgets/MapTip.cpp
		new-gui/brat/GUI/OperationViewsDialog.cpp
		new-gui/brat/GUI/ControlPanels/Dialogs/SaveAsFormulaDialog.cpp
		new-gui/brat/DataModels/PlotData/FieldData.cpp
		new-gui/brat/DataModels/DisplayDataProcessor.cpp
		new-gui/brat/GUI/ProgressDialog.cpp
		new-gui/Common/SingleApplication/qtlockedfile.cpp
	)

		
	# -----------------------------------------------------
	# Just to setup 'Header Files' file tab in Visual Studio
	# -----------------------------------------------------
	set(GUI_SOURCES_H
		new-gui/brat/stdafx.h
		new-gui/Common/BratVersion.h
		new-gui/brat/BratApplication.h
		new-gui/brat/BratMainWindow.h
		new-gui/brat/DataModels/Workspaces/BitSet32.h
		new-gui/brat/DataModels/MapTypeDisp.h
		new-gui/brat/GUI/DisplayEditors/AbstractDisplayEditor.h
		new-gui/brat/GUI/DisplayWidgets/Globe/qgsosgearthtilesource.h
		new-gui/brat/GUI/DisplayWidgets/2DPlotWidget.h
		new-gui/brat/GUI/DisplayWidgets/3DPlotWidget.h
		new-gui/brat/GUI/DisplayWidgets/GlobeWidget.h
		new-gui/brat/GUI/DisplayWidgets/MapWidget.h
		new-gui/brat/DataModels/Workspaces/Constants.h
		new-gui/brat/DataModels/Workspaces/Dataset.h
		new-gui/brat/DataModels/Workspaces/Operation.h
		new-gui/brat/DataModels/Workspaces/Formula.h
		new-gui/brat/DataModels/Workspaces/Function.h
		new-gui/brat/DataModels/Workspaces/Display.h
		new-gui/brat/DataModels/Workspaces/Workspace.h
		new-gui/brat/DataModels/Workspaces/TreeWorkspace.h
		new-gui/Common/+Utils.h
		new-gui/Common/+UtilsIO.h
		new-gui/Common/QtUtilsIO.h
		new-gui/Common/QtStringUtils.h
		new-gui/Common/QtUtils.h
		new-gui/brat/GUI/DesktopManager.h
		new-gui/Common/GUI/TextWidget.h
		new-gui/brat/GUI/TabbedDock.h
		new-gui/Common/GUI/ControlPanel.h
		new-gui/brat/GUI/ControlPanels/DatasetBrowserControls.h
		new-gui/brat/DataModels/Model.h
		new-gui/brat/GUI/ActionsTable.h
		new-gui/Common/ConfigurationKeywords.h
		new-gui/Common/FileSettings.h
		new-gui/Common/ApplicationSettings.h
		new-gui/brat/BratSettings.h
		new-gui/brat/DataModels/Workspaces/WorkspaceSettings.h
		new-gui/brat/GUI/WorkspaceDialog.h
		new-gui/brat/GUI/WorkspaceElementsDialog.h
		new-gui/brat/ApplicationSettingsDlg.h
		new-gui/Common/GUI/ApplicationUserPaths.h
		new-gui/brat/ApplicationPaths.h
		new-gui/Common/ApplicationLogger.h
		new-gui/brat/BratLogger.h
		new-gui/brat/GUI/LogShell.h
		new-gui/brat/DataModels/PlotData/PlotColor.h
		new-gui/brat/DataModels/PlotData/GeoPlotField.h
		new-gui/brat/DataModels/DisplayFilesProcessor.h
		new-gui/brat/DataModels/PlotData/XYPlotField.h
		new-gui/brat/DataModels/PlotData/ZFXYPlotField.h
		new-gui/brat/DataModels/PlotData/MapColor.h
		new-gui/brat/DataModels/PlotData/PlotValues.h
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapDecorationGrid.h
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapDecorationItem.h
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapToolMeasure.h
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapToolMeasureDialog.h
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapToolSelectPolygon.h
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapToolSelectRectangle.h
		new-gui/brat/GUI/DisplayWidgets/QGISapp/MapToolSelectUtils.h
		new-gui/brat/GUI/ControlPanels/DatasetFilterControls.h
		new-gui/brat/GUI/DisplayWidgets/Globe/GlobeControls.h
		new-gui/brat/GUI/ControlPanels/OperationControls.h
		new-gui/Common/GUI/ProcessesTable.h
		new-gui/brat/GUI/DisplayEditors/MapEditor.h
		new-gui/brat/GUI/DisplayEditors/PlotEditor.h
		new-gui/brat/GUI/ControlPanels/Views/ViewControlPanels.h
		new-gui/brat/GUI/ControlPanels/Views/ViewControlPanelsCommon.h
		new-gui/brat/GUI/ControlPanels/DataExpressionsTree.h
		new-gui/brat/GUI/WorkspaceViewsDialog.h
		new-gui/brat/DataModels/Filters/BratFilters.h
		new-gui/brat/GUI/ControlPanels/Dialogs/FormulaDialog.h
		new-gui/brat/GUI/ControlPanels/Dialogs/InsertAlgorithmDialog.h
		new-gui/brat/GUI/ControlPanels/Dialogs/InsertFunctionDialog.h
		new-gui/brat/GUI/ControlPanels/Dialogs/ShowAliasesDialog.h
		new-gui/brat/GUI/ControlPanels/Dialogs/ShowInfoDialog.h
		new-gui/brat/GUI/ControlPanels/Dialogs/DelayExecutionDialog.h
		new-gui/brat/GUI/ControlPanels/Dialogs/ExportDialog.h
		new-gui/brat/GUI/ControlPanels/Dialogs/RegionSettingsDialog.h
		new-gui/brat/DataModels/Filters/BratAreas.h
		new-gui/brat/GUI/ControlPanels/Dialogs/SelectRecordDialog.h
		new-gui/brat/GUI/DisplayWidgets/Histogram.h
		process/PythonEngine.hpp
		new-gui/brat/GUI/ActiveViewsDialog.h
		new-gui/brat/GUI/StackedWidget.h
		new-gui/brat/DataModels/PlotData/BratLookupTable.h
		new-gui/brat/DataModels/PlotData/BratLookupTableSettings.h
		new-gui/brat/GUI/ControlPanels/Dialogs/EditExportAsciiDialog.h
		new-gui/brat/GUI/ControlPanels/ColorMapWidget.h
		new-gui/Common/DataModels/BratTask.h
		new-gui/Common/DataModels/TaskProcessor.h
		new-gui/brat/GUI/DisplayEditors/Dialogs/ExportImageDialog.h
		process/BratEmbeddedPythonProcess.h
		new-gui/brat/GUI/ControlPanels/DesktopControlPanel.h
		new-gui/brat/DataModels/PlotData/Contour/BratContour.h
		new-gui/brat/DataModels/PlotData/Contour/ListContour.h
		new-gui/brat/GUI/ControlPanels/Dialogs/DataDisplayPropertiesDialog.h
		new-gui/brat/GUI/ControlPanels/ColorButton.h
		new-gui/brat/GUI/DisplayWidgets/SublayersDialog.h
		new-gui/brat/GUI/DisplayWidgets/MapTip.h
		new-gui/brat/GUI/OperationViewsDialog.h
		new-gui/brat/GUI/ControlPanels/Dialogs/SaveAsFormulaDialog.h
		new-gui/brat/DataModels/PlotData/FieldData.h
		new-gui/brat/DataModels/DisplayDataProcessor.h
		new-gui/brat/DataModels/PlotData/Plots.h
		new-gui/brat/DataModels/PlotData/WidgetField.h
		new-gui/brat/GUI/ProgressDialog.h
		new-gui/brat/DataModels/ProgressInterface.h
		new-gui/Common/SingleApplication/qtlockedfile.h
	)


	# -o is ignored
	QT4_WRAP_UI( BRAT_UI_SOURCES new-gui/brat/BratMainWindow.ui OPTIONS -o ${BRAT_SOURCE_DIR}/new-gui/brat/tmp/ui_BratMainWindow.h )
	QT4_ADD_RESOURCES( BRAT_RCC_SOURCES new-gui/Common/Resources/brat.qrc OPTIONS -o ${BRAT_SOURCE_DIR}/new-gui/brat/tmp/qrc_brat.cpp )
	QT4_ADD_RESOURCES( BRAT_RCC_SOURCES new-gui/brat/GUI/DisplayWidgets/Globe/globe_plugin.qrc OPTIONS -o ${BRAT_SOURCE_DIR}/new-gui/brat/tmp/qrc_globe_plugin.cpp )
	QT4_ADD_RESOURCES( BRAT_RCC_SOURCES new-gui/brat/Resources/images/qgis-themes-1.qrc OPTIONS -o ${BRAT_SOURCE_DIR}/new-gui/brat/tmp/qrc_qgis-themes-1.cpp )
	QT4_ADD_RESOURCES( BRAT_RCC_SOURCES new-gui/brat/Resources/images/qgis-themes-2.qrc OPTIONS -o ${BRAT_SOURCE_DIR}/new-gui/brat/tmp/qrc_qgis-themes-2.cpp )

	include_directories(
		${BRAT_SOURCE_DIR}/new-gui/brat/tmp
	)
	include_directories(
		${BRAT_SOURCE_DIR}/process
	)

	
	# -----------------------------------------------------
	# Add RC file in WIN32
	# Add 'Resource Files' file tab in Visual Studio
	# -----------------------------------------------------
	if(WIN32)
		set(GUI_SOURCES_RC new-gui/Common/Resources/brat.rc)
		SOURCE_GROUP("Resource Files" FILES ${GUI_SOURCES_RC})
	endif(WIN32)
  
	
	# -----------------------------------------------------
	# Mac OS X application bundle specific rules
	# -----------------------------------------------------
	if(APPLE)
		foreach(DATA_FILE ${DATA_FILES})
			set(GUI_BUNDLE_DATA_FILES ${GUI_BUNDLE_DATA_FILES} ${DATA_FILE})
		endforeach(DATA_FILE ${DATA_FILES})

		set_source_files_properties(${GUI_BUNDLE_DATA_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION data)
		set_source_files_properties(new-gui/Common/Resources/brat.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		set(GUI_SOURCES_RC ${GUI_BUNDLE_DATA_FILES} new-gui/Common/Resources/brat.icns)
	endif(APPLE)



	add_executable(brat WIN32 MACOSX_BUNDLE ${GUI_SOURCES} ${GUI_SOURCES_RC} ${BRAT_RCC_SOURCES} ${GUI_SOURCES_H} ${BRAT_UI_SOURCES} )
	
	target_include_directories( brat BEFORE PRIVATE new-gui/brat )
	
	add_dependencies( brat brat_runtime )

	target_link_libraries( brat
		 
		${QT_QTCORE_LIBRARY} 
		${QT_QTMAIN_LIBRARY}
		${QT_QTXML_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
		${QT_QTSVG_LIBRARY}
		${QT_QTOPENGL_LIBRARY}
		${QT_QTWEBKIT_LIBRARY}
		${QWT_LIBRARY}
		${QWTPLOT3D_LIBRARY}
		${QGIS_CORE_LIBRARY}
		${QGIS_GUI_LIBRARY}
		${QGIS_ANALYSIS_LIBRARY}
		${GEOS_LIBRARY}
		${OSGDB_LIBRARY}
		${OSGGA_LIBRARY}
		${OSGUTIL_LIBRARY}
		${OSG_LIBRARY}
		${OSGQT_LIBRARY}
		${OSGVIEWER_LIBRARY}
		${OSGSHADOW_LIBRARY}
		${OSGEARTH_LIBRARY}
		${OSGEARTHFEATURES_LIBRARY}
		${OSGEARTHUTIL_LIBRARY}
		${OSGEARTHQT_LIBRARY}
		${OSGEARTHSYMBOLOGY_LIBRARY}
		${OSGEARTHANNOTATION_LIBRARY}
		${OPENTHREADS_LIBRARY}
		${PYTHON_IMPORT_LIBRARY}
		${XercesC_LIBRARIES} 
		brathlProcessUtilities 
		brathl 
		proj 
		brat-tools
	)
	
	target_compile_definitions( brat 
		PRIVATE
		-DHAVE_OSGEARTH_ELEVATION_QUERY 
		-DHAVE_OSGEARTHQT 
		-DQWT_DLL 
		-DQWT3D_DLL 
		-DSPATIALITE_VERSION_GE_4_0_0 
		-DSPATIALITE_HAS_INIT_EX 
		-DPYTHON_EXPORT 
		-DANALYSIS_EXPORT 
		-DAPP_EXPORT 
		-DCUSTOMWIDGETS_EXPORT 
		-DSERVER_EXPORT
		-DUNICODE
		-D_UNICODE
		)
		
	IF (WIN32)	
		target_link_libraries( brat opengl32.lib )
        set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D CORE_EXPORT=__declspec(dllimport) /D GUI_EXPORT=__declspec(dllimport)" )
		set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D CORE_EXPORT=__declspec(dllimport) /D GUI_EXPORT=__declspec(dllimport)" )
	ELSE()
		set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCORE_EXPORT= -DGUI_EXPORT=" )
		set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCORE_EXPORT= -DGUI_EXPORT=" )

		IF (APPLE)
			target_link_libraries( brat

				"-framework OpenGL"
			)
		ELSE(APPLE)
			target_link_libraries( brat

				${OSGTEXT_LIBRARY}
				${OSGWIDGET_LIBRARY}
				${OSGSIM_LIBRARY}
				${OSGTERRAIN_LIBRARY}
				${OSGFX_LIBRARY}
				${OSGMANIPULATOR_LIBRARY}
				util
			)
		ENDIF (APPLE)
	ENDIF (WIN32)
	
	install(TARGETS brat DESTINATION bin BUNDLE DESTINATION .)




	if(APPLE)

		add_dependencies(brat BratCreateYFX BratCreateZFXY BratExportAscii BratExportGeoTiff BratStats scheduler)
		set_target_properties(
			brat PROPERTIES
			MACOSX_BUNDLE_ICON_FILE brat
			MACOSX_BUNDLE_BUNDLE_NAME brat
			LINK_FLAGS "-rpath @executable_path/../Frameworks/"
			)

		add_custom_command(TARGET brat
			POST_BUILD
			COMMAND cp -f ../../BratCreateYFX MacOS/BratCreateYFX
			COMMAND cp -f ../../BratCreateZFXY MacOS/BratCreateZFXY
			COMMAND cp -f ../../BratExportAscii MacOS/BratExportAscii
			COMMAND cp -f ../../BratExportGeoTiff MacOS/BratExportGeoTiff
			COMMAND cp -f ../../BratStats MacOS/BratStats
			# COMMAND cp -f ${BRAT_SOURCE_DIR}/doc/brat_user_manual.pdf doc/brat_user_manual_${BRAT_VERSION}.pdf
			WORKING_DIRECTORY ${BRAT_BINARY_DIR}/bin/brat.app/Contents)

	endif(APPLE)


endif(BRAT_BUILD_GUI)


#######################################################################################################################
#######################################################################################################################






# documentation
#

add_custom_target(doc ALL)

# Deprecated legacy target
add_custom_target(documentation)
add_dependencies(documentation doc)

set(DOC_OUTPUT_DIRECTORY ${BRAT_BINARY_DIR}/doc)

# The user manual

configure_file(
  ${BRAT_SOURCE_DIR}/doc/brat_user_manual.pdf
  ${BRAT_BINARY_DIR}/doc/brat_user_manual_${BRAT_VERSION}.pdf
  COPYONLY)

install(
  FILES ${BRAT_BINARY_DIR}/doc/brat_user_manual_${BRAT_VERSION}.pdf
  DESTINATION doc)

# The reference manual - precompiled version

configure_file(
  ${BRAT_SOURCE_DIR}/doc/brat_reference_manual.pdf
  ${BRAT_BINARY_DIR}/doc/brat_reference_manual_${BRAT_VERSION}.pdf
  COPYONLY)

install(
  FILES ${BRAT_BINARY_DIR}/doc/brat_reference_manual_${BRAT_VERSION}.pdf
  DESTINATION doc)

# The reference manual - custom target for creating new version

find_package(Doxygen)

if(DOXYGEN_FOUND)

  # Configure Doxygen

  find_package(HTMLHelp)

  if(HTML_HELP_COMPILER)
    set(DOXYGEN_HTMLHELP YES)
  else(HTML_HELP_COMPILER)
    set(DOXYGEN_HTMLHELP NO)
  endif(HTML_HELP_COMPILER)

  find_package(LATEX)

  if(LATEX_COMPILER OR PDFLATEX_COMPILER)
    set(DOXYGEN_LATEX YES)
  else(LATEX_COMPILER OR PDFLATEX_COMPILER)
    set(DOXYGEN_LATEX NO)
  endif(LATEX_COMPILER OR PDFLATEX_COMPILER)

  if(PDFLATEX_COMPILER)
    set(DOXYGEN_PDFLATEX YES)
  else(PDFLATEX_COMPILER)
    set(DOXYGEN_PDFLATEX NO)
  endif(PDFLATEX_COMPILER)

  if(DOXYGEN_DOT_EXECUTABLE)
    set(HAVE_DOT YES)
  else(DOXYGEN_DOT_EXECUTABLE)
    set(HAVE_DOT NO)
  endif(DOXYGEN_DOT_EXECUTABLE)

  set(DOXYGEN_SOURCE_DIRS
    ${CMAKE_SOURCE_DIR}/libbrathl
    ${CMAKE_SOURCE_DIR}/libbrathl/brathl_c
    ${CMAKE_SOURCE_DIR}/process
    )

  if(BRATHL_BUILD_FORTRAN)
    set(DOXYGEN_SOURCE_DIRS ${DOXYGEN_SOURCE_DIRS} ${CMAKE_SOURCE_DIR}/libbrathl/brathl_fortran)
  endif(BRATHL_BUILD_FORTRAN)
  if(BRATHL_BUILD_MATLAB)
    set(DOXYGEN_SOURCE_DIRS ${DOXYGEN_SOURCE_DIRS} ${CMAKE_SOURCE_DIR}/libbrathl/brathl_matlab)
  endif(BRATHL_BUILD_MATLAB)
  if(BRATHL_BUILD_IDL)
    set(DOXYGEN_SOURCE_DIRS ${DOXYGEN_SOURCE_DIRS} ${CMAKE_SOURCE_DIR}/libbrathl/brathl_idl)
  endif(BRATHL_BUILD_IDL)

  string(REGEX REPLACE ";" " " DOXYGEN_INPUT_LIST "${DOXYGEN_SOURCE_DIRS}")

  configure_file(
    ${BRAT_SOURCE_DIR}/doc/doxygen.conf.in
    ${BRAT_BINARY_DIR}/doxygen.conf)


  # HTML format

  set(HTML_TARGET ${DOC_OUTPUT_DIRECTORY}/refman-html/index.html)

  add_custom_target(html_reference_manual
    DEPENDS ${HTML_TARGET})

  file(TO_NATIVE_PATH ${BRAT_BINARY_DIR}/doxygen.conf DOXYGEN_CONF)
  add_custom_command(OUTPUT ${HTML_TARGET}
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${DOC_OUTPUT_DIRECTORY}/refman-html"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${DOC_OUTPUT_DIRECTORY}/refman-latex"
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONF}
    DEPENDS ${BRAT_BINARY_DIR}/doxygen.conf
    COMMENT "Building HTML documentation")

  # LaTeX/PDF format

  set(REFERENCE_MANUAL_PDF_OUT ${DOC_OUTPUT_DIRECTORY}/brat_reference_manual_${BRAT_VERSION}.pdf)

  add_custom_target(refman
    DEPENDS ${REFERENCE_MANUAL_PDF_OUT})

  # Deprecated legacy target
  add_custom_target(reference_manual)
  add_dependencies(reference_manual refman)

  add_custom_command(OUTPUT "${REFERENCE_MANUAL_PDF_OUT}"
    COMMAND ${PERL_EXECUTABLE}
    "${BRAT_SOURCE_DIR}/doc/CompileLatex.pl"
    "${PDFLATEX_COMPILER}"
    "${DOC_OUTPUT_DIRECTORY}/refman-latex"
    refman.tex
    refman.pdf
    "${REFERENCE_MANUAL_PDF_OUT}"
    DEPENDS ${HTML_TARGET}
    COMMENT "Building the reference manual")

else(DOXYGEN_FOUND)
  message(STATUS "WARNING: Doxygen not found - 'reference-manual' target will not be available")
endif(DOXYGEN_FOUND)



# The Coda product format definition documentation
#
set(DD_HTML_DIR ${CMAKE_BINARY_DIR}/doc/codadef)

add_custom_command(OUTPUT ${DD_HTML_DIR}
  COMMENT "Creating the dd_html directory..."
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DD_HTML_DIR})

add_custom_target(dd_html_dir
  DEPENDS ${DD_HTML_DIR})

add_custom_command(OUTPUT ${DD_HTML_DIR}/index.html
  COMMENT "Generating the dd_html contents..."
  COMMAND externals/coda/bin/codadd -D ${CMAKE_SOURCE_DIR}/data doc ${DD_HTML_DIR}
  DEPENDS externals/coda/bin/codadd ${CODADEF_FILES})

add_custom_target(dd_html_contents
  DEPENDS ${DD_HTML_DIR}/index.html)

add_dependencies(dd_html_contents dd_html_dir)
add_custom_target(dd_html)
add_dependencies(dd_html dd_html_contents)
add_dependencies(doc dd_html)

configure_file(
  ${BRAT_SOURCE_DIR}/externals/coda/doc/html/css/codadef.css
  ${DD_HTML_DIR}/codadef.css
  COPYONLY)

install(DIRECTORY ${DD_HTML_DIR} DESTINATION doc)

# We install the coda documentation from within BRAT. This should
# be done more elegantly from within coda itself.
install(
  DIRECTORY externals/coda/doc/html/
  DESTINATION doc/coda
  PATTERN .svn EXCLUDE
  )

# We could also install the coda documentation from the source
# location in the packager, but that can only be done by renaming
# the directory from doc/html to doc/coda in the InstallBuilder
# config file (using a postinstall rename action). However, if
# you do that, then InstallBuilder loses track of what it has
# installed, and is no longer able to uninstall things correctly.
# So, we prepare things during the build:
#
set(CODA_HTML_DIR ${CMAKE_BINARY_DIR}/doc/coda)

add_custom_command(OUTPUT ${CODA_HTML_DIR}/index.html
  COMMENT "Copying the CODA documentation"
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${BRAT_SOURCE_DIR}/externals/coda/doc/html ${CODA_HTML_DIR}
)

add_custom_target(codadoc
  DEPENDS ${CODA_HTML_DIR}/index.html
)

add_dependencies(doc codadoc)

# packager
#

# Source distribution (== tarball)

set(SOURCE_DISTRIB_FILE ${BRAT_BINARY_DIR}/brat-${BRAT_VERSION}.tar.gz)

add_custom_target(sdist
  COMMENT "Creating source package tar file ${SOURCE_DISTRIB_FILE}"
  COMMAND ${BRAT_SOURCE_DIR}/packager/CreateSourcePackage.pl ${BRAT_SOURCE_DIR} ${BRAT_BINARY_DIR} brat-${BRAT_VERSION}
  COMMAND ${CMAKE_COMMAND} -E remove ${SOURCE_DISTRIB_FILE}
  COMMAND ${CMAKE_COMMAND} -E tar cfz ${SOURCE_DISTRIB_FILE} brat-${BRAT_VERSION}
  )

# Deprecated legacy target
add_custom_target(source_package)
add_dependencies(source_package sdist)


# DVD tree

find_program(MD5SUM_COMMAND md5sum)
find_program(SHA1SUM_COMMAND sha1sum)

add_custom_target(dvd
  COMMENT "Creating the BRAT DVD directory structure"
  COMMAND ${BRAT_SOURCE_DIR}/packager/CreateDvdDirectory.pl
  ${BRAT_BINARY_DIR}/brat-dvd-${BRAT_VERSION}
  ${BRAT_VERSION}
  ${MD5SUM_COMMAND}
  ${SHA1SUM_COMMAND}
  COMMAND ${CMAKE_COMMAND} -E echo "The brat DVD directory structure has been built in brat-dvd-${BRAT_VERSION}"
  COMMAND ${CMAKE_COMMAND} -E echo "You may add some external files such as the 'data_sample' directory"
  DEPENDS ${INSTALL_BUILDER_CONF}
  )

add_dependencies(dvd sdist)


# bdist ############################################################################################

if(APPLE)
  set(APPLE_CPU ${BRAT_TARGET_PROCESSOR})
  string(STRIP ${APPLE_CPU} APPLE_CPU)
  set(BDIST_PKGDIR "${BRAT_BINARY_DIR}/brat-${BRAT_VERSION}-macosx-${APPLE_CPU}")
  set(BDIST_ROOT "${BDIST_PKGDIR}/brat")

  add_custom_target(bdist_stage_macos
    COMMENT "Copying the files needed for Mac OS X distribution"
    COMMAND rm -rf ${BDIST_PKGDIR}
    COMMAND mkdir ${BDIST_PKGDIR}
	COMMAND cp -R ${BRAT_BINARY_DIR}/bin/brat.app ${BDIST_PKGDIR}
    COMMAND cp -RL ${BRAT_BINARY_DIR}/bin/scheduler.app ${BDIST_PKGDIR}

    COMMAND mkdir -p ${BDIST_ROOT}/bin
    COMMAND cp ${BRAT_BINARY_DIR}/externals/coda/bin/coda* ${BDIST_ROOT}/bin
    COMMAND cp ${BRAT_BINARY_DIR}/externals/netcdf-c/ncdump/bin/nc* ${BDIST_ROOT}/bin
    COMMAND cp ${BRAT_BINARY_DIR}/externals/netcdf-c/ncgen/bin/nc* ${BDIST_ROOT}/bin
    COMMAND cp ${BRAT_BINARY_DIR}/externals/netcdf-c/ncgen3/bin/nc* ${BDIST_ROOT}/bin
    COMMAND cp ${BRAT_BINARY_DIR}/lib/brathl_module ${BDIST_ROOT}/bin
    COMMAND cp ${BRAT_BINARY_DIR}/bin/BratCreateYFX ${BDIST_ROOT}/bin
    COMMAND cp ${BRAT_BINARY_DIR}/bin/BratCreateZFXY ${BDIST_ROOT}/bin
    COMMAND cp ${BRAT_BINARY_DIR}/bin/BratExportAscii ${BDIST_ROOT}/bin
    COMMAND cp ${BRAT_BINARY_DIR}/bin/BratExportGeoTiff ${BDIST_ROOT}/bin
    COMMAND cp ${BRAT_BINARY_DIR}/bin/BratListFieldNames ${BDIST_ROOT}/bin
    COMMAND cp ${BRAT_BINARY_DIR}/bin/BratShowInternalFile ${BDIST_ROOT}/bin
    COMMAND cp ${BRAT_BINARY_DIR}/bin/BratCheckAliases ${BDIST_ROOT}/bin
    COMMAND cp ${BRAT_BINARY_DIR}/bin/BratStats ${BDIST_ROOT}/bin

    COMMAND mkdir -p ${BDIST_ROOT}/lib
    COMMAND cp ${BRAT_BINARY_DIR}/lib/libbrathl.a ${BDIST_ROOT}/lib
    COMMAND cp ${BRAT_BINARY_DIR}/externals/coda/lib/libcoda.a ${BDIST_ROOT}/lib
    COMMAND cp ${BRAT_BINARY_DIR}/externals/netcdf-c/liblib/lib/libnetcdf.a ${BDIST_ROOT}/lib
    COMMAND cp ${BRAT_BINARY_DIR}/externals/udunits/lib/libudunits.a ${BDIST_ROOT}/lib
    COMMAND cp ${BRAT_SOURCE_DIR}/libbrathl/brathl_python/brathl.py ${BDIST_ROOT}/bin

    COMMAND mkdir -p ${BDIST_ROOT}/include
    COMMAND cp ${BRAT_TOOLS_SOURCES_H} ${BDIST_ROOT}/include
    COMMAND cp ${LIBBRATHL_CXX_SOURCES_H} ${BDIST_ROOT}/include
    COMMAND cp libbrathl/brathl.h new-gui/Common/tools/brathl_error.h ${BDIST_ROOT}/include
    COMMAND cp ${LIBBRATHL_C_SOURCES_H} ${BDIST_ROOT}/include
    COMMAND cp ${BRAT_BINARY_DIR}/externals/coda/coda.h ${BDIST_ROOT}/include
    COMMAND cp ${BRAT_SOURCE_DIR}/externals/netcdf-c/include/netcdf.h ${BDIST_ROOT}/include
    COMMAND cp ${BRAT_SOURCE_DIR}/externals/udunits/src/lib/udunits.h ${BDIST_ROOT}/include

	COMMAND mkdir -p ${BDIST_ROOT}/data
	COMMAND cp -R ${BRAT_SOURCE_DIR}/data ${BDIST_ROOT}/data

    COMMAND mkdir ${BDIST_PKGDIR}/doc
    COMMAND cp ${BRAT_SOURCE_DIR}/doc/brat_user_manual.pdf ${BDIST_PKGDIR}/doc/brat_user_manual_${BRAT_VERSION}.pdf
    COMMAND cp ${BRAT_SOURCE_DIR}/doc/brat_reference_manual.pdf ${BDIST_PKGDIR}/doc/brat_reference_manual_${BRAT_VERSION}.pdf
    COMMAND cp -R ${DD_HTML_DIR}/ ${BDIST_PKGDIR}/doc/codadef
    COMMAND cp ${BRAT_SOURCE_DIR}/externals/coda/doc/html/css/codadef.css ${BDIST_PKGDIR}/doc/codadef
    COMMAND cp -R ${BRAT_SOURCE_DIR}/externals/coda/doc/html ${BDIST_PKGDIR}/doc/coda

    COMMAND mkdir -p ${BDIST_ROOT}/examples
    COMMAND mkdir -p ${BDIST_ROOT}/examples/c
    COMMAND cp ${BRAT_SOURCE_DIR}/libbrathl/brathl_c/BratSampleCProgram.c ${BDIST_ROOT}/examples/c
    COMMAND mkdir -p ${BDIST_ROOT}/examples/python
    COMMAND cp ${BRAT_SOURCE_DIR}/libbrathl/brathl_python/example.py ${BDIST_ROOT}/examples/python
    COMMAND cp ${BRAT_SOURCE_DIR}/libbrathl/brathl_python/example.nc ${BDIST_ROOT}/examples/python
    COMMAND mkdir -p ${BDIST_ROOT}/examples/idl
    COMMAND cp ${BRAT_SOURCE_DIR}/libbrathl/brathl_idl/examples.pro ${BDIST_ROOT}/examples/idl
    COMMAND mkdir -p ${BDIST_ROOT}/examples/matlab
    COMMAND cp ${BRAT_SOURCE_DIR}/libbrathl/brathl_matlab/examples.m ${BDIST_ROOT}/examples/matlab

    COMMAND cp ${BRAT_SOURCE_DIR}/INSTALL ${BDIST_PKGDIR}
    COMMAND cp ${BRAT_SOURCE_DIR}/README ${BDIST_PKGDIR}
    COMMAND cp ${BRAT_SOURCE_DIR}/COPYING ${BDIST_PKGDIR}
    COMMAND cp ${BRAT_SOURCE_DIR}/COPYING.LESSER ${BDIST_PKGDIR}

	COMMAND cp -R ${BRAT_SOURCE_DIR}/externals/OsGeo4Mac/lib ${BDIST_PKGDIR}/lib
	WORKING_DIRECTORY ${BRAT_SOURCE_DIR}
    )

  set(BDIST_DMGFILE "${BDIST_PKGDIR}.dmg")
  add_custom_target(bdist
    COMMENT "Creating the Mac OS X disk image"
    COMMAND rm -f ${BDIST_DMGFILE}
	# -megabytes: to avoid error -5341; use a size that is comfortably bigger than expected (more than 1Gb)
	COMMAND hdiutil create -megabytes 2000 -srcfolder ${BDIST_PKGDIR} -volname "BRAT-${BRAT_VERSION}-${APPLE_CPU}" ${BDIST_DMGFILE}
    )

  add_dependencies(bdist bdist_stage_macos)
  add_dependencies(bdist_stage_macos doc)
else(APPLE)
  find_package(InstallBuilder)
endif(APPLE)


if(INSTALL_BUILDER_FOUND)

  set(INSTALL_BUILDER_DIR ${INSTALL_BUILDER_DIR} CACHE STRING "Root directory of the InstallBuilder installation" FORCE)

  # InstallBuilder configuration

  set(WINDOWS_64_BIT_MODE 0)
  set(START_MENU_GROUP_NAME "Broadview Radar Altimetry Toolbox ${BRAT_VERSION}-${BRAT_TARGET_PROCESSOR}")

  if(WIN32)
    set(INSTALLER_PLATFORM windows)
    set(INSTALLER_FILENAME brat-${BRAT_VERSION}-${BRAT_TARGET_PROCESSOR}-installer.exe)
 	if ("${BRAT_TARGET_PROCESSOR}" STREQUAL "x64")
	  set(WINDOWS_64_BIT_MODE 1)
	endif()
  else(WIN32)
    if ("${BRAT_TARGET_PROCESSOR}" MATCHES "i386")
        set(INSTALLER_PLATFORM linux)
    else()
        set(INSTALLER_PLATFORM linux-x64)
    endif()
    set(INSTALLER_FILENAME brat-${BRAT_VERSION}-${BRAT_TARGET_PROCESSOR}-installer.run)
  endif (WIN32)

  set(PACKAGE_FILE ${BRAT_BINARY_DIR}/${INSTALLER_FILENAME})

  configure_file(
    ${BRAT_SOURCE_DIR}/packager/BratMainPackage.in.xml
    ${BRAT_BINARY_DIR}/BratMainPackage.xml
    @ONLY)

  set(INSTALL_BUILDER_CONF ${BRAT_BINARY_DIR}/BratMainPackage.xml)


  # Copy the header files that need to be installed. I beg
  # forgiveness for abusing 'configure_file()' as a glorified
  # copying mechanism, but it does work.


  foreach(SOURCE ${BRAT_TOOLS_SOURCES_H})
    string(REGEX REPLACE "^new-gui/Common/tools" "" TARGET ${SOURCE})
    configure_file(
      ${BRAT_SOURCE_DIR}/${SOURCE}
      ${BRAT_BINARY_DIR}/include/${TARGET}
      COPYONLY)
  endforeach(SOURCE)

  foreach(SOURCE ${LIBBRATHL_CXX_SOURCES_H})
    string(REGEX REPLACE "^libbrathl" "" TARGET ${SOURCE})
    configure_file(
      ${BRAT_SOURCE_DIR}/${SOURCE}
      ${BRAT_BINARY_DIR}/include/${TARGET}
      COPYONLY)
  endforeach(SOURCE)

#  configure_file(
#    ${BRAT_BINARY_DIR}/libbrathl/brathl_config.h
#    ${BRAT_BINARY_DIR}/include/brathl_config.h COPYONLY)

  configure_file(
    ${BRAT_SOURCE_DIR}/libbrathl/brathl_c/brathlc.h
    ${BRAT_BINARY_DIR}/include/brathlc.h COPYONLY)

  if(BRATHL_BUILD_FORTRAN)
    configure_file(
      ${BRAT_SOURCE_DIR}/libbrathl/brathl_fortran/brathl_fortran.h
      ${BRAT_BINARY_DIR}/include/brathl_fortran.h COPYONLY)

    configure_file(
      ${BRAT_SOURCE_DIR}/libbrathl/brathl_fortran/brathlf.inc
      ${BRAT_BINARY_DIR}/include/brathlf.inc COPYONLY)
  endif(BRATHL_BUILD_FORTRAN)

  configure_file(
    ${BRAT_BINARY_DIR}/externals/coda/coda.h
    ${BRAT_BINARY_DIR}/include/coda.h COPYONLY)

  configure_file(
    ${BRAT_SOURCE_DIR}/externals/netcdf-c/include/netcdf.h
    ${BRAT_BINARY_DIR}/include/netcdf.h COPYONLY)

  configure_file(
    ${BRAT_SOURCE_DIR}/externals/udunits/src/lib/udunits.h
    ${BRAT_BINARY_DIR}/include/udunits.h COPYONLY)

  # Ditto for the matlab m files

  foreach(SOURCE ${LIBBRATHL_MATLAB_MFILES})
    string(REGEX REPLACE "^libbrathl/brathl_matlab" "" TARGET ${SOURCE})
    configure_file(
      ${BRAT_SOURCE_DIR}/${SOURCE}
      ${BRAT_BINARY_DIR}/matlab/${TARGET}
      COPYONLY)
  endforeach(SOURCE)

  file(GLOB CODA_MFILES
    ${BRAT_SOURCE_DIR}/externals/coda/matlab/*.m)
  foreach(SOURCE ${CODA_MFILES})
    # string(REGEX REPLACE "^${BRAT_SOURCE_DIR}/externals/coda/matlab" "" TARGET ${SOURCE})
    get_filename_component(TARGET ${SOURCE} NAME)
    configure_file(
      ${SOURCE}
      ${BRAT_BINARY_DIR}/matlab/${TARGET}
      COPYONLY)
  endforeach(SOURCE)

  # Binary distribution (== installer)

  file(TO_NATIVE_PATH ${INSTALL_BUILDER_CONF} INSTALL_BUILDER_CONF_NATIVE)

  add_custom_target(bdist
    COMMENT "Running InstallBuilder"
    COMMAND ${INSTALL_BUILDER_EXECUTABLE} build ${INSTALL_BUILDER_CONF_NATIVE} ${INSTALLER_PLATFORM}
    )

  add_dependencies(bdist doc)

  # Deprecated legacy target
  add_custom_target(bundle)
  add_dependencies(bundle bdist)


else(INSTALL_BUILDER_FOUND)
  if (NOT APPLE)
    message(STATUS "WARNING: InstallBuilder not found - 'bdist' target will not be available. Try setting the INSTALL_BUILDER_DIR environment or CMake variable to the location of an installed InstallBuilder executable.")
  endif(NOT APPLE)
endif(INSTALL_BUILDER_FOUND)
