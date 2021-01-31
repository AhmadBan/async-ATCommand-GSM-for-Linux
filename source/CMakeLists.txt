cmake_minimum_required(VERSION 3.10)
project(gsm)
set(CMAKE_C_STANDARD 11)#C11
set(CMAKE_CXX_STANDARD 17)#C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
TARGET_LINK_LIBRARIES(  gsm pthread)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/inc)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/inc/commands)
file(GLOB_RECURSE INCLUDES "inc/*.h"   "inc/commands/*.h"  )
file(GLOB_RECURSE SOURCES  "src/*.c"   "src/commands/*.c" )

add_executable(gsm  ${SOURCES} ${INCLUDES})


