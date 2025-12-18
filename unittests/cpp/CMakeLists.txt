# require CMake 2.8 or greater
cmake_minimum_required(VERSION 3.11)

project(unittest)
include(GNUInstallDirs)
include_directories(/usr/include)

find_package(Threads REQUIRED) 


set(CMAKE_CXX_STANDARD 17)
include_directories(${PROJECT_SOURCE_DIR})

find_package(PkgConfig REQUIRED)


pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)
pkg_search_module(GLIB REQUIRED glib-2.0) 

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# # framework lib
# find_library(DATALINK_LIB REQUIRED
#     NAMES datalink
#     HINTS /usr/local/lib
# )

SET(TARGET_H
  ../../include/datalink.h
)


add_executable(
  unittest
    cpp_test_base.cpp
    test_tcplink_read_write.cpp
    ../../src/datalink.cpp
    ../../src/tcp_link.cpp
    ../../src/utils.cpp
)


if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(unittest PUBLIC "-pthread")
  target_compile_options(tst_client PUBLIC "-pthread")
  target_compile_options(tst_server PUBLIC "-pthread")
  target_compile_options(tst_bridge PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(unittest "${CMAKE_THREAD_LIBS_INIT}")
  target_link_libraries(tst_client "${CMAKE_THREAD_LIBS_INIT}")
  target_link_libraries(tst_server "${CMAKE_THREAD_LIBS_INIT}")
  target_link_libraries(tst_bridge "${CMAKE_THREAD_LIBS_INIT}")
endif()

# link my-recognition to jetson-inference library
target_link_libraries(unittest PkgConfig::deps 
  ${GTEST_LIBRARIES} 
  #${DATALINK_LIB}
)
