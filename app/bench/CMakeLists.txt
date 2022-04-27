set(bench_DEPENDENCIES googlelog palkia)
set(bench_INCLUDE_DIRS ${palkia_INCLUDE_DIRS})
set(bench_LIBS ${palkia_LINKER_LIBS})

# the executables
# bench_sequential_swap
add_executable(bench_sequential_swap bench_sequential_swap.cc)
add_dependencies(bench_sequential_swap ${bench_DEPENDENCIES})
include_directories(bench_sequential_swap ${bench_INCLUDE_DIRS})
target_link_libraries(bench_sequential_swap ${bench_LIBS})

# bench_sequential_remote
add_executable(bench_sequential_remote bench_sequential_remote.cc)
add_dependencies(bench_sequential_remote ${bench_DEPENDENCIES})
include_directories(bench_sequential_remote ${bench_INCLUDE_DIRS})
target_link_libraries(bench_sequential_remote ${bench_LIBS})
