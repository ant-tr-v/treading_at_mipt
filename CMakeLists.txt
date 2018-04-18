cmake_minimum_required(VERSION 3.9)
project(treading_at_mipt)

set(CMAKE_CXX_STANDARD 11)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
find_package(Threads REQUIRED)


add_executable(all_at_one all_at_one.cpp)
add_executable(integrate_baseline integrate_baseline.cpp)
add_executable(integrate_advansed integrate_advansed.cpp)
target_link_libraries (all_at_one Threads::Threads)
target_link_libraries (integrate_baseline Threads::Threads)
target_link_libraries (integrate_advansed Threads::Threads)