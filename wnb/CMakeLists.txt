set(WNB_SRCS wordnet.cc load_wordnet.cc info_helper.cc)

# executable
#--------------------------------------------------
add_executable (wntest main.cc ${WNB_SRCS})
SET(EXECUTABLE_OUTPUT_PATH ${WNB_BINARY_DIR}/bin)

# static library
#--------------------------------------------------
add_library (wnb ${WNB_SRCS})
SET(LIBRARY_OUTPUT_PATH ${WNB_BINARY_DIR}/lib)

#IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#ENDIF()


