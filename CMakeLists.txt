#   Copyright 2018 OpenQL Project developers.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

cmake_minimum_required(VERSION 2.8.7)
if(POLICY CMP0046)
  cmake_policy(SET CMP0046 NEW)
endif()
if(POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})

# ---[ OpenQL project
project(OpenQL C CXX)

# ---[ OpenQL version
set (OpenQL_VERSION_MAJOR 1)
set (OpenQL_VERSION_MINOR 0)

# ---[ Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -O2 -DNDEBUG")

if(CMAKE_COMPILER_IS_GNUCXX)
  if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  endif()
endif()

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
  set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -undefined dynamic_lookup")
endif()

if(WIN32)
  add_definitions(-DUNICODE -D_UNICODE)
  set(TARGET_ARCHITECTURE "x86")
endif()

if(UNIX OR APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall")
endif()

# ---[ settings
option (BUILD_SHARED_LIBS "Build Shared Libraries" ON)

# Set the LIB_TYPE variable to STATIC
set (CMAKE_MACOSX_RPATH 1)
# set (LIB_TYPE STATIC)
set (LIB_TYPE SHARED)
if (BUILD_SHARED_LIBS)
  # User wants to build Dynamic Libraries, so change the LIB_TYPE variable to CMake keyword 'SHARED'
  set (LIB_TYPE SHARED)
endif (BUILD_SHARED_LIBS)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "sample_program" )

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---[ OpenQL config file
# configure_file (
#   "${PROJECT_SOURCE_DIR}/openql_config.h.in"
#   "${PROJECT_BINARY_DIR}/openql_config.h"
#   )

# ---[ Includes
include_directories(include)

add_definitions("-Wall")

add_subdirectory(src)
add_subdirectory(samples)
add_subdirectory(tools)

# turn on solution folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# create solution folder
set_target_properties(openql          PROPERTIES FOLDER library)
set_target_properties(openql-samples  PROPERTIES FOLDER executable)
set_target_properties(openql-tools    PROPERTIES FOLDER executable)

