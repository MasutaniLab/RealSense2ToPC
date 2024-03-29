set(REALSENSE2_INC_SEARCH_PATH /usr/local/include)
set(REALSENSE2_LIB_SEARCH_PATH /usr/local/lib)

if(MSVC)
  list(APPEND REALSENSE2_INC_SEARCH_PATH "C:/librealsense2/include")

  list(APPEND REALSENSE2_INC_SEARCH_PATH $ENV{REALSENSE2_HOME}/include)
  list(APPEND REALSENSE2_INC_SEARCH_PATH $ENV{REALSENSE2_DIR}/include)
  list(APPEND REALSENSE2_INC_SEARCH_PATH "C:/Program Files (x86)/Intel RealSense SDK 2.0/include")

  list(APPEND REALSENSE2_LIB_SEARCH_PATH $ENV{REALSENSE2_HOME}/lib)
  list(APPEND REALSENSE2_LIB_SEARCH_PATH $ENV{REALSENSE2_DIR}/lib)
  
  if(CMAKE_CL_64)
    list(APPEND REALSENSE2_LIB_SEARCH_PATH "C:/Program Files (x86)/Intel RealSense SDK 2.0/lib/x64")
  else()
    list(APPEND REALSENSE2_LIB_SEARCH_PATH "C:/Program Files (x86)/Intel RealSense SDK 2.0/lib/x86")
  endif()
else()
  list(APPEND REALSENSE2_INC_SEARCH_PATH /usr/include)
  list(APPEND REALSENSE2_LIB_SEARCH_PATH /usr/lib)

  list(APPEND REALSENSE2_INC_SEARCH_PATH $ENV{REALSENSE2_HOME}/include)
  list(APPEND REALSENSE2_LIB_SEARCH_PATH $ENV{REALSENSE2_HOME}/lib)
  list(APPEND REALSENSE2_INC_SEARCH_PATH $ENV{REALSENSE2_DIR}/include)
  list(APPEND REALSENSE2_LIB_SEARCH_PATH $ENV{REALSENSE2_DIR}/lib)
endif()

find_path(REALSENSE2_INCLUDE_DIRS librealsense2/rs.hpp
  PATHS
    ${REALSENSE2_INC_SEARCH_PATH}
)

find_library(REALSENSE2_LIBRARIES
  NAMES realsense2
  PATHS
    ${REALSENSE2_LIB_SEARCH_PATH}
)

if(REALSENSE2_LIBRARIES AND REALSENSE2_INCLUDE_DIRS)
  set(REALSENSE2_FOUND TRUE)
else()
  set(REALSENSE2_FOUND FALSE)
endif()
  
mark_as_advanced(
  REALSENSE2_INCLUDE_DIRS
  REALSENSE2_LIBRARIES
  REALSENSE2_INC_SEARCH_PATH
  REALSENSE2_LIB_SEARCH_PATH
)
