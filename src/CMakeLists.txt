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
set(project_name openql)

# define a project name
project (${project_name})

# define a variable SRC with file GLOB
file(GLOB SRC *.c *.h)

# add include directory for ${project_name}
include_directories(${PROJECT_BINARY_DIR})

# define sources files of an executable
add_library(${project_name} ${LIB_TYPE} ${SRC})

include (GenerateExportHeader)
GENERATE_EXPORT_HEADER( ${project_name}
  BASE_NAME OPENQL_LIB
  EXPORT_MACRO_NAME __QL_OPENQL
  EXPORT_FILE_NAME openql.h
  STATIC_DEFINE OPENQL_LIB_BUILT_AS_STATIC
)
