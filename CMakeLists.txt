#https://cmake.org/cmake/help/book/mastering-cmake/chapter/Getting%20Started.html

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS ON) #...with compiler extensions like gnu++11

cmake_minimum_required(VERSION 3.0)

file(GLOB SOURCES
    *.h
    *.cpp
)

add_executable(depotMgr ${SOURCES})
project(depotMgr)

find_package(OpenCV)
find_package(aruco REQUIRED)

#Locating OpenCV_... isn't an echo $ thing, ask cmake if you want to know
message(STATUS "OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBS = ${OpenCV_LIBS}")
message(STATUS "aruco_INCLUDE_DIRS: " ${aruco_INCLUDE_DIRS})

#will build w/o OpenCV incl/links, must already be in aruco .so
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS})
target_link_libraries( depotMgr ${OpenCV_LIBS})

include_directories(../../aruco-3.1.9/src)
target_link_libraries( depotMgr libaruco.so )




