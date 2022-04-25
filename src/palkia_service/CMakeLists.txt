cmake_minimum_required(VERSION 3.0)
project(palkia_service)

FILE(GLOB_RECURSE SOURCE "*.cc")

# the executable
add_executable(palkia_service ${SOURCE})
include_directories(palkia_service
    "${PROJECT_BINARY_DIR}/src/palkia_service"
)

# googlelog
add_dependencies(palkia_service googlelog)
include_directories(palkia_service ${GLOG_INCLUDE_DIRS})
target_link_libraries(palkia_service glog)

# ibverbs
target_link_libraries(palkia_service ${IBVERBS_LIBRARIES})

# palkia common
add_dependencies(palkia_service palkia)
include_directories(palkia_service ${palkia_INCLUDE_DIRS})
target_link_libraries(palkia_service ${palkia_LINKER_LIBS})
