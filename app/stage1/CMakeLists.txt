cmake_minimum_required(VERSION 3.0)

set(stage1_DEPENDENCIES googlelog palkia)
set(stage1_INCLUDE_DIRS ${palkia_INCLUDE_DIRS})
set(stage1_LIBS ${palkia_LINKER_LIBS})

# the executable
add_executable(test_pod test_pod.cc)
add_dependencies(test_pod ${stage1_DEPENDENCIES})
include_directories(test_pod ${stage1_INCLUDE_DIRS})
target_link_libraries(test_pod ${stage1_LIBS})

add_executable(test_swap test_swap.cc)
add_dependencies(test_swap ${stage1_DEPENDENCIES})
include_directories(test_swap ${stage1_INCLUDE_DIRS})
target_link_libraries(test_swap ${stage1_LIBS})
