CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4 FATAL_ERROR)
project (3DTerrain)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")


FIND_PACKAGE(OpenGL)

FIND_PACKAGE(CURL)
FIND_PACKAGE(GDAL)
FIND_PACKAGE(GEOS)
FIND_PACKAGE(Sqlite3)
FIND_PACKAGE(ZLIB)
FIND_PACKAGE(V8)
FIND_PACKAGE(SDL)
find_package (OpenThreads)
find_package (OSG)
#find_package (osgDB)
#find_package (osgUtil)
#find_package (osgViewer)
#find_package (osgGA)
find_package (osgEarth)
find_package (SDL)
find_package (SDL_image)

macro ( config_project PROJNAME LIBNAME)
    include_directories( ${${LIBNAME}_INCLUDE_DIR})
    TARGET_LINK_LIBRARIES( ${PROJNAME} ${${LIBNAME}_LIBRARY} )
endmacro()

IF (ZLIB_FOUND)
    FIND_PACKAGE(MiniZip)
ENDIF (ZLIB_FOUND)

IF(UNIX)
    # Not sure what this will do on Cygwin and Msys
    # Also, remember OS X X11 is a user installed option so it may not exist.
    FIND_PACKAGE(X11)
    # Some Unicies need explicit linkage to the Math library or the build fails.
    FIND_LIBRARY(MATH_LIBRARY m)
ENDIF(UNIX)

INCLUDE_DIRECTORIES(
    ${OSGEARTH_SOURCE_DIR}/src
    ${OSG_INCLUDE_DIR}
)


IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/lib)
ENDIF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

IF(UNIX AND NOT WIN32 AND NOT APPLE)
  IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET(LIB_POSTFIX "64" CACHE STRING "suffix for 32/64 dir placement")
      MARK_AS_ADVANCED(LIB_POSTFIX)
  ENDIF(CMAKE_SIZEOF_VOID_P MATCHES "8")
ENDIF(UNIX AND NOT WIN32 AND NOT APPLE)
IF(NOT DEFINED LIB_POSTFIX)
    SET(LIB_POSTFIX "")
ENDIF(NOT DEFINED LIB_POSTFIX)

#SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME})
SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_BINDIR})

#SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib/${CMAKE_SYSTEM_NAME})
SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})
SET(LIBRARY_OUTPUT_PATH ${OUTPUT_LIBDIR})

LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${OUTPUT_LIBDIR} )

SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")

INCLUDE(OsgEarthMacroUtils)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
  
SET(TARGET_COMMON_LIBRARIES
    osgEarth
    osgEarthFeatures
    osgEarthUtil
    osgEarthSymbology
    osgEarthAnnotation
    osgGA
)

INCLUDE_DIRECTORIES(${OSG_INCLUDE_DIRS} )
SET(TARGET_LIBRARIES_VARS OSG_LIBRARY OSGDB_LIBRARY OSGUTIL_LIBRARY OSGVIEWER_LIBRARY OPENTHREADS_LIBRARY)

SET(TARGET_EXTERNAL_LIBRARIES ${SDL_LIBRARY} )
        INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR} )



SET(TARGET_SRC 3DTerrain.cpp SDLIntegration.cpp)

SET(TARGET_H
    SDLIntegration.h
)

        ADD_DEFINITIONS(-DUSE_SDL)

#### end var setup  ###
SETUP_APPLICATION(3DTerrain)

#add_executable (3DTerrain 3DTerrain.cpp)
#config_project (3DTerrain OPENTHREADS)
#config_project (3DTerrain OSG)
#config_project (3DTerrain OSGDB)
#config_project (3DTerrain OSGUTIL)
#config_project (3DTerrain OSGVIEWER)
#config_project (3DTerrain OSGGA)
#config_project (3DTerrain OSGEARTH)
#config_project (3DTerrain SDL)
#config_project (3DTerrain SDL_IMAGE)
