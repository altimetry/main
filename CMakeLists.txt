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

set(BRAT_MAJOR_VERSION "3")
set(BRAT_MINOR_VERSION "3")
set(BRAT_REVISION_VERSION "0")
set(BRAT_VERSION "${BRAT_MAJOR_VERSION}.${BRAT_MINOR_VERSION}.${BRAT_REVISION_VERSION}")

message(STATUS "System type: ${CMAKE_SYSTEM_NAME}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin CACHE INTERNAL "Directory for all programs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib CACHE INTERNAL "Directory for all dynamic libraries")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib CACHE INTERNAL "Directory for all static libraries")
set(CMAKE_MODULE_PATH ${BRAT_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH} CACHE INTERNAL "Directories where to find modules defs")
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

	  if(MSVC60)
		set(MSPROJECT_TEMPLATE_DIRECTORY ${BRAT_SOURCE_DIR}/CMakeMsvc6Templates)

		MESSAGE(STATUS "Processing MSVC60 redistributable DLLs...")
	
		# Microsoft's redistributable DLLs
		# 
		install(FILES "C:/WINDOWS/system32/msvcp60.dll" DESTINATION bin/)
		install(FILES "C:/WINDOWS/system32/msvcrt.dll" DESTINATION bin/)

		# Note: abusing configure_file again, just to get things in
		# the right location for InstallBuilder. This is now getting
		# out of hand however, and I will fix this properly (using
		# the installed files instead) with top priority.
		# 
		configure_file("C:/WINDOWS/system32/msvcp60.dll"
					   ${BRAT_BINARY_DIR}/bin/msvcp60.dll
					   COPYONLY)
		configure_file("C:/WINDOWS/system32/msvcrt.DLL"
					   ${BRAT_BINARY_DIR}/bin/msvcrt.DLL
					   COPYONLY)

		configure_file("C:/WINDOWS/system32/msvcp60.dll"
					   ${BRAT_BINARY_DIR}/matlab/msvcp60.dll
					   COPYONLY)
		configure_file("C:/WINDOWS/system32/msvcrt.DLL"
					   ${BRAT_BINARY_DIR}/matlab/msvcrt.DLL
					   COPYONLY)

	  endif(MSVC60)

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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++0x" )
  set(BRAT_GNU_OPTIONS "-Wall")
endif(CMAKE_COMPILER_IS_GNUCXX)


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
endif()

IF (WIN32)
#    IF ("${BRAT_TARGET_PROCESSOR}" STREQUAL "Win32")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _USING_V110_SDK71_" )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D _USING_V110_SDK71_" )
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS,5.01")
#    ENDIF ()
ENDIF ()



if(MSVC)
  if(NOT MSVC60)
    add_definitions(/wd4274)
  endif(NOT MSVC60)
endif(MSVC)


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


# ---------------------------------
# defines for STL include. see explanation in Stl.h
# ---------------------------------
add_definitions(-DSTL_USING_ALL)

# ---------------------------------
# Check for various functions..
# ---------------------------------

check_function_exists(snprintf HAVE_SNPRINTF)
check_function_exists(_snprintf HAVE_USCORE_SNPRINTF)
check_function_exists(vsnprintf HAVE_VSNPRINTF)
check_function_exists(_vsnprintf HAVE_USCORE_VSNPRINTF)
check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(_stricmp HAVE_USCORE_STRICMP)
check_function_exists(isinf HAVE_ISINF)
check_function_exists(isnan HAVE_ISNAN)
check_function_exists(_isnan HAVE_USCORE_ISNAN)
check_function_exists(_fpclass HAVE_FPCLASS)
check_function_exists(_fullpath HAVE_FULLPATH)
check_function_exists(realpath HAVE_REALPATH)
check_function_exists(dirname HAVE_DIRNAME)
check_function_exists(basename HAVE_BASENAME)
check_function_exists(_splitpath HAVE_SPLITPATH)
check_function_exists(stat HAVE_STAT)

# ---------------------------------
# check for optional include files
# ---------------------------------

check_include_file("unistd.h" HAVE_UNISTD_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("libgen.h" HAVE_LIBGEN_H)

# ---------------------------------
# check for endiannes
# ---------------------------------

test_big_endian(WORDS_BIGENDIAN)



# ---------------------------------
# Some system dependant settings
# ---------------------------------

if(WIN32)
  set(PATH_SEPARATOR "\\\\")
  set(PATH_LIST_SEPARATOR ";")
else(WIN32)
  set(PATH_SEPARATOR "/")
  set(PATH_LIST_SEPARATOR ":")
endif(WIN32)

set(BRATHL_ENVVAR "BRAT_DATA_DIR")



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

set(BRATHL_DEFDATADIR  "${CMAKE_INSTALL_PREFIX}/${INSTALL_DIR}/data" CACHE INTERNAL "Default data directory")
set(BRATHL_UNITFILE "brathl_units.dat" CACHE INTERNAL "Units file")


set(CODADEF_FILES
	data/Altimeter_Ocean_Pathfinder-20100826.codadef
	#data/CRYOSAT-20120130.codadef
	data/CRYOSAT-20150601.codadef
	data/ENVISAT_AATSR-20110923.codadef
	data/ENVISAT_ASAR-20150331.codadef
	data/ENVISAT_AUX-20121219.codadef
	data/ENVISAT_DORIS-20100826.codadef
	data/ENVISAT_GOMOS-20121004.codadef
	data/ENVISAT_MERIS-20100826.codadef
	data/ENVISAT_MIPAS-20140120.codadef
	#data/ENVISAT_RA2MWR-20110125.codadef
	data/ENVISAT_RA2MWR-20120623.codadef
	data/ENVISAT_SCIAMACHY-20140916.codadef
	data/EPS-20150324.codadef
	data/ERS_ATSR-20110923.codadef
	data/ERS_GOME-20110920.codadef
	data/ERS_MWR-20110820.codadef
	data/ERS_RA-20110820.codadef
	data/GFO-20110125.codadef
	data/JASON-20110125.codadef
	data/PODAAC-20110125.codadef
	data/RADS-20080701.codadef
	data/River_Lake-20110125.codadef
	data/SWARM-20150126.codadef
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

set(BRAT_DISPLAY_DATA_FILES
  data/gshhs_l.b
  )

set(BRAT_GUI_DATA_FILES
  data/function_descr.txt
  data/BratIcon.png
  data/BratLogo.png
  data/Formulas.ini
  )
  
set(DATA_FILES
  ${CODADEF_FILES}
  ${LIBBRATHL_DATA_FILES}
  ${GUI_DATA_FILES}
  ${BRAT_DISPLAY_DATA_FILES}
  ${BRAT_GUI_DATA_FILES}
  )

install(FILES ${DATA_FILES} DESTINATION data/)

# The libbrathl/ subdirectory

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
  libbrathl/Exception.cpp
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
  libbrathl/Tools.cpp
  libbrathl/Trace.cpp
  libbrathl/TraceLog.cpp
  libbrathl/TreeField.cpp
  libbrathl/Unit.cpp
  libbrathl/Xml.cpp
  )


# brathl_c
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


# brathl_fortran
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

# brathl_idl
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

endif(BRATHL_BUILD_IDL)


# ------------------------------------------------------
# Just to setup 'Header Files' file tab in Visual Studio
# ------------------------------------------------------
string(REGEX REPLACE "\\.cpp" .h LIBBRATHL_CXX_SOURCES_H
  "${LIBBRATHL_CXX_SOURCES}")

set(LIBBRATHL_CXX_SOURCES_H
  ${LIBBRATHL_CXX_SOURCES_H}
  libbrathl/Stl.h
  libbrathl/brathl.h
  libbrathl/brathl_error.h
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

configure_file(
  ${BRAT_SOURCE_DIR}/libbrathl/brathl_config.h.in
  ${BRAT_BINARY_DIR}/libbrathl/brathl_config.h)

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
    target_link_libraries(brathl netcdf coda_static hdf5 ssh2 udunits)
else()
    target_link_libraries(brathl netcdf coda_static hdf5 udunits)
endif()

# required by brathl_python
#
add_library(brathl_module MODULE ${ALL_BRATHL_SOURCES})
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(brathl_module netcdf coda_static hdf5 ssh2 udunits)
else()
    target_link_libraries(brathl_module netcdf coda_static hdf5 udunits)
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

# include_directories(${BRAT_SOURCE_DIR}/libbrathl/brathl_c)
# set(SAMPLEXML BratSampleXMLProgram)
# add_executable(${SAMPLEXML} libbrathl/brathl_c/${SAMPLEXML}.c)
# target_link_libraries(${SAMPLEXML} brathl)

# brathl_matlab
#
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
    set(MEXFLAGS -I${MATLAB_INCLUDE_DIR} -I. -I${CMAKE_CURRENT_BINARY_DIR}/libbrathl -I${CODA_BINARY_DIR} -I${CODA_SOURCE_DIR}/libcoda -I${CMAKE_CURRENT_SOURCE_DIR}/libbrathl -I${CMAKE_CURRENT_SOURCE_DIR}/libbrathl/brathl_c -DSTL_USING_ALL)

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
      set(MEX_CMD ${MEX} ${MEXFLAGS} ${LIBBRATHL_MATLAB_SOURCE_FILES} ${INT_LIB_DIR}/brathl.lib ${CODA_INT_LIB_DIR}/libcoda.lib ${NETCDF_INT_LIB_DIR}/netcdf.lib ${UDUNITS_INT_LIB_DIR}/udunits.lib ${HDF4_LIBRARIES} ${HDF5_LIBRARIES}  -output ${MEX_OUTPUT})
    else(WIN32)
      set(MEX_CMD ${MEX} ${MEXFLAGS} ${LIBBRATHL_MATLAB_SOURCE_FILES} ${INT_LIB_DIR}/libbrathl.a ${CODA_INT_LIB_DIR}/libcoda.a ${NETCDF_INT_LIB_DIR}/libnetcdf.a ${UDUNITS_INT_LIB_DIR}/libudunits.a ${HDF4_LIBRARIES} ${HDF5_LIBRARIES} -o ${MEX_OUTPUT})
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


endif(BRATHL_BUILD_MATLAB)


# process
#

set(PROCESS_LIB_SOURCES
  process/BratProcess.cpp
  process/BratProcessYFX.cpp
  process/BratProcessZFXY.cpp
  process/BratProcessExportAscii.cpp
  process/BratProcessStats.cpp
  process/ProcessCommonTools.cpp
  )

string(REGEX REPLACE "\\.cpp" .h PROCESS_LIB_SOURCES_H  "${PROCESS_LIB_SOURCES}")

set(PROCESS_SOURCES "")

macro(DEFINE_PROCESS PNAME)
  set(PROCESS_SOURCES "process/${PNAME}.cpp")
  add_executable(${PNAME} "${PROCESS_SOURCES}")
  target_link_libraries(${PNAME} brathlProcessUtilities brathl ${ADDITIONAL_LIBS})
  install(TARGETS ${PNAME} DESTINATION bin/)
endmacro(DEFINE_PROCESS)

DEFINE_PROCESS(BratExportAscii)
DEFINE_PROCESS(BratCreateYFX)
DEFINE_PROCESS(BratCreateZFXY)
DEFINE_PROCESS(BratShowInternalFile)
DEFINE_PROCESS(BratStats)
DEFINE_PROCESS(BratListFieldNames)
DEFINE_PROCESS(BratCheckAliases)


set(PY_VER_SUFFIX 3.2)

#IF(BUILD_RunPythonAlgorithm)
#
#	IF (WIN32)
#		IF ("${BRAT_TARGET_PROCESSOR}" MATCHES "Win32")
#			SET(PY_SUBDIR x86/default)
#		ELSE()
#			SET(PY_SUBDIR default)
#		ENDIF()
#		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /I$ENV{S3ALTB_ROOT}/compilers/Python/${PY_SUBDIR}/include")
#		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /L$ENV{S3ALTB_ROOT}/compilers/Python/default/libs")
#	ELSEIF(APPLE)
#		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/Library/Frameworks/Python.framework/Versions/${PY_VER_SUFFIX}/include/python${PY_VER_SUFFIX}m")
#		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/Library/Frameworks/Python.framework/Versions/${PY_VER_SUFFIX}/lib/python${PY_VER_SUFFIX}/config-${PY_VER_SUFFIX}m")
#		 SET(PY_LIB python${PY_VER_SUFFIX})
#	ELSE(APPLE)
#		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/python${PY_VER_SUFFIX}")
#		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/lib/python${PY_VER_SUFFIX}/config-${PY_VER_SUFFIX}mu")
#		SET(PY_LIB python${PY_VER_SUFFIX}mu)
#	ENDIF(WIN32)
#	
#	set(ADDITIONAL_LIBS ${PY_LIB})
#	DEFINE_PROCESS(RunPythonAlgorithm)
#
#	add_custom_target(run_py_algo DEPENDS RunPythonAlgorithm)
#	
#ENDIF()



#set(ADDITIONAL_LIBS ${LIBGEOTIFF_LIBRARIES} ${JPEG_LIBRARIES} ${ZLIB_LIBRARIES})
set(ADDITIONAL_LIBS geotiff vtktiff vtkjpeg vtkzlib)
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


# GUI Tools

# display
#

if(BRAT_BUILD_GUI)

  # ---------------------------------
  # Check for VTK..
  # ---------------------------------
  find_package(VTK5 REQUIRED)
  if(VTK_USE_FILE)
    set(SAVE_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(SAVE_CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
    # set(SKIP_VTK_BUILD_SETTINGS_FILE ON)
    # set(VTK_NO_LIBRARY_DEPENDS ON)
    include(${VTK_USE_FILE})                            #..../lib/vtk-5.6/UseVTK.cmake
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND "${BRAT_TARGET_PROCESSOR}" MATCHES "i386" )
      #
      # Not nice but necessary: get rid of undesired, deprecated, flags that cause errors 
      # in recent clang versions, which do not allow uknown arguments, like -mlong-branch
      #
      set(CMAKE_CXX_FLAGS ${SAVE_CMAKE_CXX_FLAGS})
      set(CMAKE_C_FLAGS ${SAVE_CMAKE_C_FLAGS})
    endif()
  endif(VTK_USE_FILE)
  set(VTK_DIR ${VTK_DIR} CACHE STRING "VTK installation directory containing VTKConfig.cmake" FORCE)
  # set(VTK_NO_LIBRARY_DEPENDS ON)

  # ---------------------------------
  # Check for GTK..
  # ---------------------------------
  if(VTK_USE_X)
    find_package(GTK REQUIRED)
    if(GTK_FOUND)
      set(CMAKE_CXX_FLAGS "${CMAKE_GTK_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
      add_definitions(-DWXVTK_GTK)
    endif(GTK_FOUND)
  endif(VTK_USE_X)


  # ---------------------------------
  # Check for wxWidgets..
  # ---------------------------------
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

  # find_package(wxWidgets COMPONENTS base core net xml adv gl xrc)
  find_package(wxWidgets COMPONENTS base core net xml adv gl xrc)
  if(NOT wxWidgets_FOUND)
    if(WIN32)
      message(FATAL_ERROR "WxWidgets not found. Try setting the wxWidgets_ROOT_DIR environment or CMake variable to the root of a wxWidgets build tree or installation.")
    endif(WIN32)
    if(UNIX)
      message(FATAL_ERROR "WxWidgets not found. Try adding the location of the wx-config utility from the wxWidgets installation you want to use to your executable search PATH.")
    endif(UNIX)
  endif(NOT wxWidgets_FOUND)
  include(${wxWidgets_USE_FILE})

  # Use BRATS's embedded vtktiff, avoiding duplicate symbol errors
  list(REMOVE_ITEM wxWidgets_LIBRARIES -lwxtiff-3.0)


  # -----------------------------------------------------
  # Set common gui object sources library
  # -----------------------------------------------------
  set(DISPLAY_LIB_SOURCES
    display/BitSet32.cpp
    display/BratTask.cpp
    display/CheckListBox.cpp
    display/ColorPicker.cpp
    display/ColorPalette.cpp
    display/DirTraverser.cpp
    display/Dnd.cpp
    display/LabeledTextCtrl.cpp
    display/MapColor.cpp
    display/MapImageType.cpp
    display/MapProjection.cpp
    display/MapTypeDisp.cpp
    display/Process.cpp
    display/SchedulerTaskConfig.cpp
    display/SchedulerTimer.cpp
    display/TimeCtrl.cpp
    display/TreeCtrl.cpp
    display/Validators.cpp
    display/VtkColor.cpp
    display/vtkTools.cpp
    display/wxList.cpp
    display/wxBratTools.cpp
    display/WindowHandler.cpp
    )


  # -----------------------------------------------------
  # Set BratDisplay sources
  # -----------------------------------------------------
  set(DISPLAY_SOURCES
    display/AnimationToolbar.cpp
    display/AxisPropertyPanel.cpp
    display/BratDisplay.cpp
    display/BratDisplay_wdr.cpp
    display/BratLookupTable.cpp
    display/ContourPropFrame.cpp
    display/DatasetPanel.cpp
    display/LUTFrame.cpp
    display/LUTPanel.cpp
    display/PlotBase.cpp
    display/Plot.cpp
    display/PlotField.cpp
    display/TypedSaveFileDialog.cpp
    display/WPlot.cpp
    display/WPlotPropertyPanel.cpp
    display/WorldPlotData.cpp
    display/WorldPlotFrame.cpp
    display/WorldPlotPanel.cpp
    display/XYPlotData.cpp
    display/XYPlotFrame.cpp
    display/XYPlotPanel.cpp
    display/XYPlotPropertyPanel.cpp
    display/ZFXYContourPropFrame.cpp
    display/ZFXYPlot.cpp
    display/ZFXYPlotData.cpp
    display/ZFXYPlotFrame.cpp
    display/ZFXYPlotPanel.cpp
    display/ZFXYPlotPropertyPanel.cpp
    display/vtkCameraState.cpp
    display/vtkDataArrayPlotData.cpp
    display/vtkGSHHSReader.cpp
    display/vtkGeoGridSource.cpp
    display/vtkGeoMapFilter.cpp
    display/vtkInteractorStyleWPlot.cpp
    display/vtkInteractorStyle3DWPlot.cpp
    display/vtkInteractorStyleXYPlot.cpp
    display/vtkInteractorStyleZFXYPlot.cpp
    display/vtkList.cpp
    display/vtkNewAxisActor2D.cpp
    display/vtkPlotData.cpp
    display/vtkPlotDataCollection.cpp
    display/vtkPointLocatorBrat.cpp
    display/vtkProj2DFilter.cpp
    display/vtkXYPlotActor.cpp
    display/vtkZFXYPlotActor.cpp
    display/vtkZFXYPlotFilter.cpp
    display/vtkZFXYPlotFilterCollection.cpp
    display/Config.cpp
    display/vtkVelocityGlyphFilter.cpp
    display/vtkBratArrowSource.cpp
    )

  # -----------------------------------------------------
  # Just to setup 'Header Files' file tab in Visual Studio
  # -----------------------------------------------------
  string(REGEX REPLACE "\\.cpp" .h DISPLAY_SOURCES_H_TEMP
    "${DISPLAY_SOURCES}"
    )
  string(REGEX REPLACE "\\.cxx" .h DISPLAY_SOURCES_H
    "${DISPLAY_SOURCES_H_TEMP}"
    )
  string(REGEX REPLACE "\\.cpp" .h DISPLAY_LIB_SOURCES_H
    "${DISPLAY_LIB_SOURCES}"
    )

  # -----------------------------------------------------
  # Set VTK libraries used by BratDisplay
  # -----------------------------------------------------
  set(BRAT_VTK_LIBRARIES
    vtkCommon
    vtkRendering
    vtkHybrid
    )

  if(CMAKE_COMPILER_IS_GNUCXX)
    # Only for C++
    set_source_files_properties(
      ${DISPLAY_SOURCES} ${C_SOURCES}
      PROPERTIES
      COMPILE_FLAGS "-Wall -Wno-write-strings"
      )
  endif(CMAKE_COMPILER_IS_GNUCXX)


  # -----------------------------------------------------
  # Add RC file in WIN32
  # Add 'Resource Files' file tab in Visual Studio
  # -----------------------------------------------------
  if(WIN32)
    set(DISPLAY_SOURCES_RC display/BratDisplayRC.rc)
    SOURCE_GROUP("Resource Files" FILES ${DISPLAY_SOURCES_RC})

    set_source_files_properties(display/vtkProj2DFilter.cpp
                                PROPERTIES COMPILE_FLAGS "/Od")
  endif(WIN32)

  if(APPLE)
    set(DISPLAY_BUNDLE_DATA_FILES data/brathl_units.dat ${GUI_DATA_FILES} ${BRAT_DISPLAY_DATA_FILES})
    set_source_files_properties(${DISPLAY_BUNDLE_DATA_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION data)
    set_source_files_properties(gui/BratGui.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(DISPLAY_SOURCES_RC ${DISPLAY_BUNDLE_DATA_FILES} gui/BratGui.icns)
    # Add MakeApplicationActive.mm for Mac OS X build
    set(DISPLAY_SOURCES ${DISPLAY_SOURCES} display/MakeApplicationActive.mm display/wxVTKRenderWindowInteractor.mm)
    # set_source_files_properties(display/wxVTKRenderWindowInteractor.cxx PROPERTIES COMPILE_FLAGS "-x objective-c++")
    # if (APPLE)
    #  set(DISPLAY_SOURCES ${DISPLAY_SOURCES} display/wxVTKRenderWindowInteractor.mm)
    # else(APPLE)
    #  set(DISPLAY_SOURCES ${DISPLAY_SOURCES} display/wxVTKRenderWindowInteractor.cxx)
    # endif(APPLE)
  else(APPLE)
    set(DISPLAY_SOURCES ${DISPLAY_SOURCES} display/wxVTKRenderWindowInteractor.cxx)
  endif(APPLE)

  include_directories(
    ${BRAT_SOURCE_DIR}/process ${wxWidgets_INCLUDE_DIRS}
    )

  add_executable(BratDisplay
    WIN32 MACOSX_BUNDLE
    ${DISPLAY_SOURCES} ${DISPLAY_SOURCES_RC} ${DISPLAY_SOURCES_H}
    )

  target_link_libraries(BratDisplay brathl proj brathlProcessUtilities brathlGuiUtilities ${BRAT_VTK_LIBRARIES} ${wxWidgets_LIBRARIES} vtktiff vtkjpeg )

  if(APPLE)
    # On Mac OS X we install a script that invokes the BratDisplay binary that is located in BratGui.app
    install(PROGRAMS display/BratDisplayMac.sh RENAME BratDisplay DESTINATION bin/)
  else(APPLE)
    install(TARGETS BratDisplay DESTINATION bin/)
  endif(APPLE)

  add_library(brathlGuiUtilities STATIC ${DISPLAY_LIB_SOURCES} ${DISPLAY_LIB_SOURCES_H})

  if(APPLE)
    set_target_properties(BratDisplay PROPERTIES
      MACOSX_BUNDLE_ICON_FILE BratGui
      MACOSX_BUNDLE_BUNDLE_NAME BratDisplay)
    add_custom_command(TARGET BratDisplay
      POST_BUILD COMMAND cp -f ${BRAT_SOURCE_DIR}/display/BratDisplay BratDisplay
      WORKING_DIRECTORY ${BRAT_BINARY_DIR}/bin)
  endif(APPLE)


  # scheduler
  #

  # -----------------------------------------------------
  # Set BratScheduler sources
  # -----------------------------------------------------
  set(SCHEDULER_SOURCES
    scheduler/BratScheduler.cpp
    scheduler/BratScheduler_wdr.cpp
    scheduler/EndedPanel.cpp
    scheduler/LogPanel.cpp
    scheduler/PendingPanel.cpp
    scheduler/ProcessingPanel.cpp
    scheduler/RichTextFrame.cpp
    scheduler/SchedulerFrame.cpp
    scheduler/SchedulerPanel.cpp
    scheduler/TaskListView.cpp
    )
  
  # -----------------------------------------------------
  # Just to setup 'Header Files' file tab in Visual Studio
  # -----------------------------------------------------
  string(REGEX REPLACE "\\.cpp" .h SCHEDULER_SOURCES_H
    "${SCHEDULER_SOURCES}"
    )

  # -----------------------------------------------------
  # Add RC file in WIN32
  # Add 'Resource Files' file tab in Visual Studio
  # -----------------------------------------------------
  if(WIN32)
    set(SCHEDULER_SOURCES_RC scheduler/BratSchedulerRC.rc)
    SOURCE_GROUP("Resource Files" FILES ${SCHEDULER_SOURCES_RC})
  endif(WIN32)

  if(APPLE)
    set(SCHEDULER_BUNDLE_DATA_FILES ${GUI_DATA_FILES} ${CODADEF_FILES} ${LIBBRATHL_DATA_FILES})
    set_source_files_properties(${SCHEDULER_BUNDLE_DATA_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION data)
    set_source_files_properties(gui/BratGui.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(SCHEDULER_SOURCES_RC ${SCHEDULER_BUNDLE_DATA_FILES} gui/BratGui.icns)
    # Add MakeApplicationActive.mm for Mac OS X build
    set(SCHEDULER_SOURCES ${SCHEDULER_SOURCES} scheduler/MakeApplicationActive.mm)
  endif(APPLE)

  include_directories(
    ${BRAT_SOURCE_DIR}/process 
    ${BRAT_SOURCE_DIR}/display 
    ${wxWidgets_INCLUDE_DIRS}
    )
  
  
  add_executable(BratScheduler
    WIN32 MACOSX_BUNDLE
    ${SCHEDULER_SOURCES} ${SCHEDULER_SOURCES_RC} ${SCHEDULER_SOURCES_H})

  target_link_libraries(BratScheduler brathlGuiUtilities brathlProcessUtilities brathl ${wxWidgets_LIBRARIES})
  install(TARGETS BratScheduler DESTINATION bin BUNDLE DESTINATION .)

  if(APPLE)
    # On Mac OS X we install a script that invokes the BratScheduler application
    install(PROGRAMS scheduler/BratScheduler RENAME BratScheduler DESTINATION bin/)

    add_dependencies(BratScheduler BratCreateYFX BratCreateZFXY BratExportAscii BratExportGeoTiff BratStats)
    set_target_properties(BratScheduler PROPERTIES
      MACOSX_BUNDLE_ICON_FILE BratGui
      MACOSX_BUNDLE_BUNDLE_NAME BratScheduler)
    add_custom_command(TARGET BratScheduler
      POST_BUILD COMMAND cp -f ${BRAT_SOURCE_DIR}/scheduler/BratScheduler BratScheduler
      COMMAND cp -f ../../BratCreateYFX MacOS/BratCreateYFX
      COMMAND cp -f ../../BratCreateZFXY MacOS/BratCreateZFXY
      COMMAND cp -f ../../BratExportAscii MacOS/BratExportAscii
      COMMAND cp -f ../../BratExportGeoTiff MacOS/BratExportGeoTiff
      COMMAND cp -f ../../BratStats MacOS/BratStats
      WORKING_DIRECTORY ${BRAT_BINARY_DIR}/bin/BratScheduler.app/Contents)  
  endif(APPLE)
   
  
  # gui
  #

  # -----------------------------------------------------
  # Set BratGui sources
  # -----------------------------------------------------
  set(GUI_SOURCES
    gui/AlgorithmDlg.cpp
    gui/BratGui.cpp
    gui/BratGui_wdr.cpp
    gui/Dataset.cpp
    gui/DatasetPanel.cpp
    gui/DatasetTreeCtrl.cpp
    gui/DelayDlg.cpp
    gui/Dictionary.cpp
    gui/Display.cpp
    gui/DisplayPanel.cpp
    gui/DisplayDataTreeCtrl.cpp
    gui/ExportDlg.cpp
    gui/FieldsTreeCtrl.cpp
    gui/Formula.cpp
    gui/FormulaDlg.cpp
    gui/Function.cpp
    gui/FunctionDlg.cpp
    gui/GuiPanel.cpp
    gui/GuiFrame.cpp
    gui/LogPanel.cpp
    gui/Operation.cpp
    gui/OperationPanel.cpp
    gui/OperationTreeCtrl.cpp
    gui/ResolutionDlg.cpp
    gui/RichTextFrame.cpp
    gui/SelectionCriteriaDlg.cpp
    gui/TaskList.cpp
    gui/TreeWorkspace.cpp
    gui/Workspace.cpp
    gui/WorkspaceDlg.cpp
    )

  # -----------------------------------------------------
  # Just to setup 'Header Files' file tab in Visual Studio
  # -----------------------------------------------------
  string(REGEX REPLACE "\\.cpp" .h GUI_SOURCES_H
    "${GUI_SOURCES}"
    )

  # -----------------------------------------------------
  # Add RC file in WIN32
  # Add 'Resource Files' file tab in Visual Studio
  # -----------------------------------------------------
  if(WIN32)
    set(GUI_SOURCES_RC gui/BratGuiRC.rc)
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
    set_source_files_properties(gui/BratGui.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(GUI_SOURCES_RC ${GUI_BUNDLE_DATA_FILES} gui/BratGui.icns)
  endif(APPLE)

  include_directories(
    ${BRAT_SOURCE_DIR}/process
    ${BRAT_SOURCE_DIR}/display
    )

  add_executable(BratGui WIN32 MACOSX_BUNDLE ${GUI_SOURCES} ${GUI_SOURCES_RC} ${GUI_SOURCES_H})

  target_link_libraries(BratGui brathlGuiUtilities brathlProcessUtilities brathl proj ${wxWidgets_LIBRARIES})
  install(TARGETS BratGui DESTINATION bin BUNDLE DESTINATION .)

  if(APPLE)
    add_dependencies(BratGui BratCreateYFX BratCreateZFXY BratExportAscii BratExportGeoTiff BratStats BratDisplay BratScheduler)
    set_target_properties(BratGui PROPERTIES
      MACOSX_BUNDLE_ICON_FILE BratGui
      MACOSX_BUNDLE_BUNDLE_NAME BratGui)
    add_custom_command(TARGET BratGui
      POST_BUILD
      COMMAND cp -f ../../BratCreateYFX MacOS/BratCreateYFX
      COMMAND cp -f ../../BratCreateZFXY MacOS/BratCreateZFXY
      COMMAND cp -f ../../BratExportAscii MacOS/BratExportAscii
      COMMAND cp -f ../../BratExportGeoTiff MacOS/BratExportGeoTiff
      COMMAND cp -f ../../BratStats MacOS/BratStats
      COMMAND cp -f ${BRAT_SOURCE_DIR}/display/BratDisplay MacOS/BratDisplay
      COMMAND rm -rf MacOS/BratDisplay.app
      COMMAND cp -RL ../../BratDisplay.app MacOS/BratDisplay.app
      COMMAND cp -f ${BRAT_SOURCE_DIR}/scheduler/BratScheduler MacOS/BratScheduler
      COMMAND cp -f ${BRAT_SOURCE_DIR}/doc/brat_user_manual.pdf data/brat_user_manual_${BRAT_VERSION}.pdf
      WORKING_DIRECTORY ${BRAT_BINARY_DIR}/bin/BratGui.app/Contents)
  endif(APPLE)


endif(BRAT_BUILD_GUI)


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
    ${CMAKE_SOURCE_DIR}/display
    ${CMAKE_SOURCE_DIR}/gui
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



if(APPLE)
  set(APPLE_CPU ${BRAT_TARGET_PROCESSOR})
  string(STRIP ${APPLE_CPU} APPLE_CPU)
  set(BDIST_PKGDIR "${BRAT_BINARY_DIR}/brat-${BRAT_VERSION}-macosx-${APPLE_CPU}")
  set(BDIST_ROOT "${BDIST_PKGDIR}/brat")

  add_custom_target(bdist_stage_macos
    COMMENT "Copying the files needed for Mac OS X distribution"
    COMMAND rm -rf ${BDIST_PKGDIR}
    COMMAND mkdir ${BDIST_PKGDIR}
    COMMAND cp -RL ${BRAT_BINARY_DIR}/bin/BratGui.app ${BDIST_PKGDIR}
    COMMAND cp -RL ${BRAT_BINARY_DIR}/bin/BratScheduler.app ${BDIST_PKGDIR}

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
    COMMAND cp ${BRAT_SOURCE_DIR}/display/BratDisplayMac.sh ${BDIST_ROOT}/bin/BratDisplay
    COMMAND chmod 755 ${BDIST_ROOT}/bin/BratDisplay
    COMMAND cp ${BRAT_SOURCE_DIR}/scheduler/BratScheduler ${BDIST_ROOT}/bin/BratScheduler
    COMMAND chmod 755 ${BDIST_ROOT}/bin/BratScheduler

    COMMAND mkdir -p ${BDIST_ROOT}/lib
    COMMAND cp ${BRAT_BINARY_DIR}/lib/libbrathl.a ${BDIST_ROOT}/lib
    COMMAND cp ${BRAT_BINARY_DIR}/externals/coda/lib/libcoda.a ${BDIST_ROOT}/lib
    COMMAND cp ${BRAT_BINARY_DIR}/externals/netcdf-c/liblib/lib/libnetcdf.a ${BDIST_ROOT}/lib
    COMMAND cp ${BRAT_BINARY_DIR}/externals/udunits/lib/libudunits.a ${BDIST_ROOT}/lib
    COMMAND cp ${BRAT_SOURCE_DIR}/libbrathl/brathl_python/brathl.py ${BDIST_ROOT}/bin

    COMMAND mkdir -p ${BDIST_ROOT}/include
    COMMAND cp ${LIBBRATHL_CXX_SOURCES_H} ${BDIST_ROOT}/include
    COMMAND cp libbrathl/Stl.h ${BDIST_ROOT}/include
    COMMAND cp libbrathl/brathl.h libbrathl/brathl_error.h ${BDIST_ROOT}/include
    COMMAND cp ${LIBBRATHL_C_SOURCES_H} ${BDIST_ROOT}/include
    COMMAND cp ${BRAT_BINARY_DIR}/externals/coda/coda.h ${BDIST_ROOT}/include
    COMMAND cp ${BRAT_SOURCE_DIR}/externals/netcdf-c/include/netcdf.h ${BDIST_ROOT}/include
    COMMAND cp ${BRAT_SOURCE_DIR}/externals/udunits/src/lib/udunits.h ${BDIST_ROOT}/include

    COMMAND mkdir -p ${BDIST_ROOT}/data
    COMMAND cp ${BRAT_SOURCE_DIR}/data/* ${BDIST_ROOT}/data

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

    COMMAND cp ${BRAT_SOURCE_DIR}/README ${BDIST_PKGDIR}
    COMMAND cp ${BRAT_SOURCE_DIR}/COPYING ${BDIST_PKGDIR}
    COMMAND cp ${BRAT_SOURCE_DIR}/COPYING.LESSER ${BDIST_PKGDIR}
    WORKING_DIRECTORY ${BRAT_SOURCE_DIR}
    )

  set(BDIST_DMGFILE "${BDIST_PKGDIR}.dmg")
  add_custom_target(bdist
    COMMENT "Creating the Mac OS X disk image"
    COMMAND rm -f ${BDIST_DMGFILE}
    # -megabytes: to avoid error -5341; use a size that is comfortably bigger than expected (about 120 Mb)
    COMMAND hdiutil create -megabytes 500 -srcfolder ${BDIST_PKGDIR} -volname "BRAT-${BRAT_VERSION}-${APPLE_CPU}" ${BDIST_DMGFILE}
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
  set(START_MENU_GROUP_NAME BRAT-${BRAT_VERSION}-${BRAT_TARGET_PROCESSOR})

  if(WIN32)
    set(INSTALLER_PLATFORM windows)
    set(INSTALLER_FILENAME brat-${BRAT_VERSION}-${BRAT_TARGET_PROCESSOR}-installer.exe)
 	if ("${BRAT_TARGET_PROCESSOR}" STREQUAL "x64")
	  set(WINDOWS_64_BIT_MODE 1)
	endif()
  else(WIN32)
    set(INSTALLER_PLATFORM linux)
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

  foreach(SOURCE ${LIBBRATHL_CXX_SOURCES_H})
    string(REGEX REPLACE "^libbrathl" "" TARGET ${SOURCE})
    configure_file(
      ${BRAT_SOURCE_DIR}/${SOURCE}
      ${BRAT_BINARY_DIR}/include/${TARGET}
      COPYONLY)
  endforeach(SOURCE)

  configure_file(
    ${BRAT_BINARY_DIR}/libbrathl/brathl_config.h
    ${BRAT_BINARY_DIR}/include/brathl_config.h COPYONLY)

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
