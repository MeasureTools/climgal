cmake_minimum_required(VERSION 3.8)

# Project Information
project (climgal VERSION 0.1.0)

# Set compiler options
set (CMAKE_C_STANDARD 11)
set (CMAKE_C_COMPILER             "/usr/bin/clang")
set (CMAKE_C_FLAGS                "-fPIC -Weverything -pthread")
set (CMAKE_C_FLAGS_DEBUG          "-g")
set (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
set (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
set (CMAKE_CXX_FLAGS                "-fPIC -Wno-c++98-compat -Wno-padded -Wno-covered-switch-default -pthread")
set (CMAKE_CXX_FLAGS_DEBUG          "-g")
set (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

set (CMAKE_SHARED_LINKER_FLAGS "-v")

# Source
set (SOURCES
	climgal/main.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} docopt_s)
target_link_libraries(${PROJECT_NAME} readerlib_s)
