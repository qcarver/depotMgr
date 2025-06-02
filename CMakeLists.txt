cmake_minimum_required(VERSION 3.10)
project(depotMgr)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/../)

# Locate OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Add aruco headers directly
include_directories(${CMAKE_SOURCE_DIR}/../aruco-3.1.9/src)

# Add aruco library path
link_directories(${CMAKE_SOURCE_DIR}/../aruco-3.1.9/build/src)

# Add project's header files 
include_directories(${CMAKE_SOURCE_DIR}/..)

# Add project's source files
add_executable(depotMgr
    depotSort.cpp
    bin.cpp
    group.cpp
    rack.cpp
)

# Link against aruco and OpenCV
target_link_libraries(depotMgr
    aruco
    ${OpenCV_LIBS}
)
