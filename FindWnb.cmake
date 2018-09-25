
# - Try to find WordNet-Blast
# Once done this will define
#  WNB_FOUND - System has WordNet-Blast
#  WNB_INCLUDE_DIRS - The WordNet-Blast include directories
#  WNB_LIBRARIES - The libraries needed to use WordNet-Blast
#  WNB_LIBRARY_DEBUG - Debug version of Library
#  WNB_LIBRARY_RELEASE - Release Version of Library
#  WNB_DEFINITIONS - Compiler switches required for using WordNet-Blast


# This macro was taken directly from the FindQt4.cmake file that is included
# with the CMake distribution. This is NOT my work. All work was done by the
# original authors of the FindQt4.cmake file. Only minor modifications were
# made to remove references to Qt and make this file more generally applicable
# 
# Link found: http://www.cmake.org/pipermail/cmake/2010-February/035055.html
#########################################################################
 
MACRO (ADJUST_LIB_VARS basename)
  IF (${basename}_INCLUDE_DIR)

  # if only the release version was found, set the debug variable also to the release version
  IF (${basename}_LIBRARY_RELEASE AND NOT ${basename}_LIBRARY_DEBUG)
    SET(${basename}_LIBRARY_DEBUG ${${basename}_LIBRARY_RELEASE})
    SET(${basename}_LIBRARY       ${${basename}_LIBRARY_RELEASE})
    SET(${basename}_LIBRARIES     ${${basename}_LIBRARY_RELEASE})
  ENDIF (${basename}_LIBRARY_RELEASE AND NOT ${basename}_LIBRARY_DEBUG)

  # if only the debug version was found, set the release variable also to the debug version
  IF (${basename}_LIBRARY_DEBUG AND NOT ${basename}_LIBRARY_RELEASE)
    SET(${basename}_LIBRARY_RELEASE ${${basename}_LIBRARY_DEBUG})
    SET(${basename}_LIBRARY         ${${basename}_LIBRARY_DEBUG})
    SET(${basename}_LIBRARIES       ${${basename}_LIBRARY_DEBUG})
  ENDIF (${basename}_LIBRARY_DEBUG AND NOT ${basename}_LIBRARY_RELEASE)
  IF (${basename}_LIBRARY_DEBUG AND ${basename}_LIBRARY_RELEASE)
    # if the generator supports configuration types then set
    # optimized and debug libraries, or if the CMAKE_BUILD_TYPE has a value
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(${basename}_LIBRARY optimized ${${basename}_LIBRARY_RELEASE}
                              debug ${${basename}_LIBRARY_DEBUG})
    ELSE(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      # if there are no configuration types and CMAKE_BUILD_TYPE has no value
      # then just use the release libraries
      SET(${basename}_LIBRARY ${${basename}_LIBRARY_RELEASE} )
    ENDIF(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
    SET(${basename}_LIBRARIES optimized ${${basename}_LIBRARY_RELEASE} 
                              debug ${${basename}_LIBRARY_DEBUG})
  ENDIF (${basename}_LIBRARY_DEBUG AND ${basename}_LIBRARY_RELEASE)

  SET(${basename}_LIBRARY ${${basename}_LIBRARY} CACHE FILEPATH "The ${basename} library")

  IF (${basename}_LIBRARY)
    SET(${basename}_FOUND 1)
  ENDIF (${basename}_LIBRARY)

  ENDIF (${basename}_INCLUDE_DIR )

  # Make variables changeble to the advanced user
  MARK_AS_ADVANCED(${basename}_LIBRARY
                   ${basename}_LIBRARY_RELEASE
                   ${basename}_LIBRARY_DEBUG
                   ${basename}_INCLUDE_DIR )
ENDMACRO (ADJUST_LIB_VARS)


set(WNB_DEBUG 0)
set(WNB_INCLUDE_SEARCH_DIRS $ENV{WNB_INSTALL}/include ${CMAKE_INSTALL_PREFIX}/include)
set(WNB_SEARCH_DEBUG_NAMES "wnbd;libwnbd")
set(WNB_SEARCH_RELEASE_NAMES "wnb;libwnb")
set(WNB_LIB_SEARCH_DIRS $ENV{EXPAT_INSTALL}/lib ${CMAKE_INSTALL_PREFIX}/lib)

if(WNB_DEBUG)
    message(STATUS "Finding wordnet-blast library and headers..." )
    message(STATUS "WNB_INCLUDE_SEARCH_DIRS: ${WNB_INCLUDE_SEARCH_DIRS}")
    message(STATUS "WNB_SEARCH_DEBUG_NAMES: ${WNB_SEARCH_DEBUG_NAMES}")
    message(STATUS "WNB_SEARCH_RELEASE_NAMES: ${WNB_SEARCH_RELEASE_NAMES}")
    message(STATUS "WNB_LIB_SEARCH_DIRS: ${WNB_LIB_SEARCH_DIRS}")
endif()

# Look for the header file
find_path(WNB_INCLUDE_DIR 
          NAMES wnb/core/wordnet.hh
          PATHS ${WNB_INCLUDE_SEARCH_DIRS}
          NO_DEFAULT_PATH)
          
# Look for the library
find_library(WNB_LIBRARY_DEBUG
             NAMES ${WNB_SEARCH_DEBUG_NAMES}
             PATHS ${WNB_LIB_SEARCH_DIRS}
             NO_DEFAULT_PATH
             )
             
find_library(WNB_LIBRARY_RELEASE
             NAMES ${WNB_SEARCH_RELEASE_NAMES}
             PATHS ${WNB_LIB_SEARCH_DIRS}
             NO_DEFAULT_PATH
             )

IF (WNB_DEBUG)
    message(STATUS "WNB_INCLUDE_DIR: ${WNB_INCLUDE_DIR}")
    message(STATUS "WNB_LIBRARY_DEBUG: ${WNB_LIBRARY_DEBUG}")
    message(STATUS "WNB_LIBRARY_RELEASE: ${WNB_LIBRARY_RELEASE}")
    message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif(WNB_DEBUG)

ADJUST_LIB_VARS(WNB)

# message( STATUS "WNB_LIBRARY: ${WNB_LIBRARY}")

# Copy the results to the output variables.
if(WNB_INCLUDE_DIR AND WNB_LIBRARY)
  set(WNB_FOUND 1)
  set(WNB_LIBRARIES ${WNB_LIBRARY})
  set(WNB_INCLUDE_DIRS ${WNB_INCLUDE_DIR})
  if(WNB_LIBRARY_DEBUG)
    get_filename_component(WNB_LIBRARY_PATH ${WNB_LIBRARY_DEBUG} PATH)
    set(WNB_LIB_DIR  ${WNB_LIBRARY_PATH})
  elseif(WNB_LIBRARY_RELEASE)
    get_filename_component(WNB_LIBRARY_PATH ${WNB_LIBRARY_RELEASE} PATH)
    set(WNB_LIB_DIR  ${WNB_LIBRARY_PATH})
  endif(WNB_LIBRARY_DEBUG)
else(WNB_INCLUDE_DIR AND WNB_LIBRARY)
  set(WNB_FOUND 0)
  set(WNB_LIBRARIES)
  set(WNB_INCLUDE_DIRS)
endif(WNB_INCLUDE_DIR AND WNB_LIBRARY)

# Report the results.
if(NOT WNB_FOUND)
  SET(WNB_DIR_MESSAGE
    "WNB (WordNet-Blast) was not found. Make sure WNB_LIBRARY and WNB_INCLUDE_DIR are set or set the WNB_INSTALL environment variable.")
  if(NOT WNB_FIND_QUIETLY)
    message(STATUS "${WNB_DIR_MESSAGE}")
  else(NOT EXPAT_FIND_QUIETLY)
    if(WNB_FIND_REQUIRED)
      message(FATAL_ERROR "Wnb was NOT found and is Required by this project")
    endif(WNB_FIND_REQUIRED)
  endif(NOT WNB_FIND_QUIETLY)
endif(NOT WNB_FOUND)
