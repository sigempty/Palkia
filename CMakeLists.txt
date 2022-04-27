# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required(VERSION 3.0)
project(palkia)

# for clang's compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER_ID "GNU")
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Glog
add_subdirectory(src/vendor/googlelog)
include_directories(${GLOG_INCLUDE_DIRS})
link_directories(${GLOG_LINK_DIRS})

# ibverbs
find_package(ibverbs REQUIRED)
include_directories(${IBVERBS_INCLUDE_DIRS})
link_directories(${IBVERBS_LIB_DIR})

# dialga
add_subdirectory(src/dialga)

include_directories(include)

add_subdirectory(src/palkia)
add_subdirectory(src/palkia_service)
add_subdirectory(app/stage1)
add_subdirectory(app/bench)
