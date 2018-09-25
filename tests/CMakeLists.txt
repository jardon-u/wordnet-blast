CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

ADD_CUSTOM_TARGET(check
  COMMAND ./check/check.sh ./check/list.txt)
  
  
# Executable
#--------------------------------------------------
ADD_EXECUTABLE (wntest main.cc)
TARGET_INCLUDE_DIRECTORIES(wntest PUBLIC ${PROJECT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(wntest wnb)
SET_TARGET_PROPERTIES(wntest PROPERTIES DEBUG_POSTFIX d)


# Install targets
#--------------------------------------------------
install(TARGETS wntest
  RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" COMPONENT bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" COMPONENT shlib
  ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" COMPONENT lib  
  COMPONENT wntest)