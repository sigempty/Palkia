cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mtune=native")

# the executable
add_executable(stage1 stage1.cc)
add_dependencies(stage1 googlelog)
target_link_libraries(stage1
    glog
)
