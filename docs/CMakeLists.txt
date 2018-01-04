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

# define a variable of project name
set(project_name openql-docs)

# set the project as the startup project
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${project_name} )

# define a project name
project (${project_name})

# define a variable SRC with file GLOB
file(GLOB SRC *.c *.h)

# add include directory
include_directories(../src)

# add include directory for ${project_name}_Export.h
include_directories(${PROJECT_BINARY_DIR}/../src)

# define sources files of an executable
add_executable(${project_name} ${SRC})

# link a library
target_link_libraries(${project_name} openql)
