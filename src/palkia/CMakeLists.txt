cmake_minimum_required(VERSION 3.0)
project(palkia)

FILE(GLOB SOURCE "*.cc")

# local variables
set(palkia_LINKER_LIBS_L "")
set(palkia_INCLUDE_DIR_L "")

# googlelog
list(APPEND palkia_INCLUDE_DIR_L ${GLOG_INCLUDE_DIRS})
list(APPEND palkia_LINKER_LIBS_L glog)

# dialga
list(APPEND palkia_INCLUDE_DIR_L ${dialga_INCLUDE_DIRS})
list(APPEND palkia_LINKER_LIBS_L ${dialga_LINKER_LIBS})

# palkia
list(APPEND palkia_INCLUDE_DIR_L "${PROJECT_BINARY_DIR}/include")
list(APPEND palkia_INCLUDE_DIR_L "${PROJECT_BINARY_DIR}/src/palkia")

# the library
add_library(palkia ${SOURCE})
include_directories(palkia ${palkia_INCLUDE_DIR_L})
target_link_libraries(palkia ${palkia_LINKER_LIBS_L})

# exports
list(APPEND palkia_LINKER_LIBS_L palkia)
set(palkia_LINKER_LIBS ${palkia_LINKER_LIBS_L} PARENT_SCOPE)
set(palkia_INCLUDE_DIRS ${palkia_INCLUDE_DIR_L} PARENT_SCOPE)
