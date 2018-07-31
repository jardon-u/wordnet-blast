CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(wnb)

# Boost dependency
#--------------------------------------------------

IF (NOT DEFINED BOOST_ROOT)
    IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      SET (BOOST_ROOT /Developer/boost_build/) # Suggested path
    ELSE()
      SET (BOOST_ROOT "/usr/include")
    ENDIF()
ENDIF()
IF (NOT DEFINED BOOST_LIBRARYDIR)
    set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")
ENDIF()
MESSAGE(STATUS "** Search Boost prefix (BOOST_ROOT): ${BOOST_ROOT}")
MESSAGE(STATUS "** Search Boost libraries (BOOST_LIBRARYDIR): ${BOOST_LIBRARYDIR}")
FIND_PACKAGE(Boost COMPONENTS graph REQUIRED)
MESSAGE(STATUS "** Boost Include: ${Boost_INCLUDE_DIR}")
MESSAGE(STATUS "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
MESSAGE(STATUS "** Boost Libraries: ${Boost_LIBRARIES}")


# Project
#--------------------------------------------------
ADD_DEFINITIONS(-D_UNICODE -DUNICODE)

SET(PROJECT_VERSION "0.6")
SET(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})

ADD_CUSTOM_TARGET(dist
  COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD
  | bzip2 > ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


## Compiler flags
if(CMAKE_CXX_COMPILER MATCHES ".*clang")
    set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()
IF (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
  list(APPEND CMAKE_CXX_FLAGS " --std=c++11 -O3 -DNDEBUG -Wall -Wextra")
  #list(APPEND CMAKE_CXX_FLAGS " -g -Wall -Wextra")
ENDIF()

SET(WNB_SRCS wnb/core/wordnet.cc
			 wnb/core/load_wordnet.cc
			 wnb/core/info_helper.cc
		     wnb/core/wordnet.hh
			 wnb/core/load_wordnet.hh
			 wnb/core/info_helper.hh
			 wnb/core/pos_t.hh
             wnb/core/std_ext.hh
			 )

# Static library
#--------------------------------------------------
ADD_LIBRARY(wnb ${WNB_SRCS})
TARGET_INCLUDE_DIRECTORIES(wnb PUBLIC ${Boost_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(wnb ${Boost_LIBRARIES})
SET_TARGET_PROPERTIES(wnb PROPERTIES 
    DEBUG_POSTFIX d
    PUBLIC_HEADER "wnb/core/wordnet.hh;wnb/core/pos_t.hh;wnb/core/info_helper.hh;${CMAKE_CURRENT_BINARY_DIR}/wnb_export.h;"
    LINKER_LANGUAGE CXX)

include_directories("${PROJECT_BINARY_DIR}")
include (GenerateExportHeader) 
generate_export_header(wnb)

OPTION(BUILD_TESTS "Build wnb tests/examples" OFF)
IF (BUILD_TESTS)
    ADD_SUBDIRECTORY(tests)
ENDIF()


# Install targets
#--------------------------------------------------
install(TARGETS wnb
  RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" COMPONENT bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" COMPONENT shlib
  ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" COMPONENT lib  
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include/wnb/core"
  COMPONENT wnb)

  
# Applying workaround to preserve hierarchy of include files (see http://cmake.3232098.n2.nabble.com/Install-header-directory-hierarchy-td5638507.html)
set(HEADERS   wnb/core/load_wordnet.hh
              wnb/core/std_ext.hh
              wnb/utils/bfs.hh
              wnb/utils/nltk_similarity.hh
              ) 
               
foreach(HEADER ${HEADERS}) 
   string(REGEX MATCH "(.*)[/\\]" DIR ${HEADER}) 
   install(FILES ${HEADER} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${DIR}) 
endforeach(HEADER HEADERS) 


# Data
#------------------------
option(DOWNLOAD_WORDNET "Download WordNet 3.1" OFF)
if(DOWNLOAD_WORDNET)
    if(IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/data/wordnet3.1")
              message(STATUS "WordNet 3.1 is already on disk")
    else()
              message(STATUS "Downloading WordNet 3.1 database to ${CMAKE_INSTALL_PREFIX}/data/wordnet3.1")
              file(DOWNLOAD http://wordnetcode.princeton.edu/wn3.1.dict.tar.gz ${CMAKE_INSTALL_PREFIX}/data/wordnet3.1/wn3.1.dict.tar.gz                            SHOW_PROGRESS)

              message(STATUS "Uncompressing database...")
              execute_process(COMMAND ${CMAKE_COMMAND} -E tar xvzf ${CMAKE_INSTALL_PREFIX}/data/wordnet3.1/wn3.1.dict.tar.gz
                            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/data/wordnet3.1)
    endif()
endif()