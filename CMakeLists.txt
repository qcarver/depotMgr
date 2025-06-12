cmake_minimum_required(VERSION 3.10)
project(depotMgr)

# needed for `concept` support. Else? just rewrite to use common base class
set(CMAKE_CXX_STANDARD 20)

# comment out for debugging, 
# Eg building with gdb:
# cmake -DCMAKE_BUILD_TYPE=Debug ..
# Eg cmds:
# gdb ./depotMgr            # info threads
# break depotSort.cpp:135   # Thread <thread number> 
# run -b 28 ../capture.jpg
# print args.bin
# info breakpoints
# delete <breakpoint number>
# next # Step Over
# step # Step Into
# continue
set(CMAKE_BUILD_TYPE Debug)

# This is where our own include files are located
include_directories(${CMAKE_SOURCE_DIR}/include)

# With OpenCV and Aruco 'find_package' assUmes they are installed on the local system. 
# If you built them from source, be sure to run 'cmake install' in their build directories.

# Locate OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Locate Aruco 
find_package(aruco REQUIRED)
include_directories(${aruco_INCLUDE_DIRS})

# Add this project's header files 
link_directories(${CMAKE_SOURCE_DIR}/include)

# Add this project's source files
add_executable(depotMgr
    src/depotSort.cpp
    src/bin.cpp
    src/group.cpp
    src/rack.cpp
    src/argc_argv.hpp
)

# Link against Aruco and OpenCV
target_link_libraries(depotMgr
    aruco
    ${OpenCV_LIBS}
)

# I always forget this, and I'm like: "Where's my executable?"
message("Remember if cmake builds successfully, you still have to call 'make' in the buid dir")
