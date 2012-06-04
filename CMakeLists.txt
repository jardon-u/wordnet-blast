CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( WNB )

LINK_DIRECTORIES(${WNB_SOURCE_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

set(PROJECT_VERSION "0.5")
set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})
add_custom_target(dist
    COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD
        | bzip2 > ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

## Compiler flags
IF (CMAKE_COMPILER_IS_GNUCXX)
  list(APPEND CMAKE_CXX_FLAGS " -O3 -DNDEBUG -Wall -Wextra")
  #list(APPEND CMAKE_CXX_FLAGS " -g -Wall -Wextra")
ENDIF()


set(WNB_SRCS wnb/core/wordnet.cc
  wnb/core/load_wordnet.cc wnb/core/info_helper.cc)

# executable
#--------------------------------------------------
add_executable (wntest wnb/main.cc ${WNB_SRCS})
SET(EXECUTABLE_OUTPUT_PATH ${WNB_BINARY_DIR}/bin)

# static library
#--------------------------------------------------
add_library (wnb ${WNB_SRCS})
SET(LIBRARY_OUTPUT_PATH ${WNB_BINARY_DIR}/lib)

#IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#ENDIF()

#ADD_SUBDIRECTORY( wnb )
#ADD_SUBDIRECTORY( check )
#ADD_SUBDIRECTORY( matlab )

