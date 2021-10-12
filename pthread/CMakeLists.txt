project("pthread")

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories($(CMAKE_CURRENT_SOURCE_DIR)/src)
#add_executable(app src/main.cpp)
#target_link_libraries(app ${CMAKE_THREAD_LIBS_INIT})
MACRO(add_example name)
  ADD_EXECUTABLE(${name} src/${name}.cpp)
  TARGET_LINK_LIBRARIES(${name} ${CMAKE_THREAD_LIBS_INIT})
ENDMACRO()

add_example(main)

