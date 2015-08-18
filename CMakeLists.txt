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

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

# Project
#--------------------------------------------------
ADD_DEFINITIONS(-D_UNICODE -DUNICODE)
LINK_DIRECTORIES(${wnb_SOURCE_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR})

SET(PROJECT_VERSION "0.6")
SET(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})

ADD_CUSTOM_TARGET(dist
  COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD
  | bzip2 > ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

ADD_CUSTOM_TARGET(check
  COMMAND ./check/check.sh ./check/list.txt)


## Compiler flags
IF (CMAKE_COMPILER_IS_GNUCXX)
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
			 )

# Executable
#--------------------------------------------------
ADD_EXECUTABLE (wntest wnb/main.cc ${WNB_SRCS})
SET(EXECUTABLE_OUTPUT_PATH ${wnb_BINARY_DIR}/bin)

# Static library
#--------------------------------------------------
ADD_LIBRARY(wnb ${WNB_SRCS})
SET(LIBRARY_OUTPUT_PATH ${wnb_BINARY_DIR}/lib)

IF (Boost_FOUND)
  TARGET_LINK_LIBRARIES(wntest ${Boost_LIBRARIES})
  TARGET_LINK_LIBRARIES(wnb ${Boost_LIBRARIES})
ENDIF()

# Install targets
#--------------------------------------------------
# Applying workaround to preserve hierarchy of include files (see http://cmake.3232098.n2.nabble.com/Install-header-directory-hierarchy-td5638507.html)
set_target_properties(wnb PROPERTIES DEBUG_POSTFIX d)
set(HEADERS   wnb/core/info_helper.hh
              wnb/core/wordnet.hh
              wnb/core/pos_t.hh
              wnb/core/load_wordnet.hh
              wnb/bfs.hh
              wnb/nltk_similarity.hh
              wnb/std_ext.hh
              ) 
               
install(TARGETS wnb
  RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" COMPONENT bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" COMPONENT shlib
  ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" COMPONENT lib  
  COMPONENT wnb)
  
install(TARGETS wntest
  RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" COMPONENT bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" COMPONENT shlib
  ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" COMPONENT lib  
  COMPONENT wntest)
  
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