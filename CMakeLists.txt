cmake_minimum_required(VERSION 3.9)
project(treading_at_mipt)

set(CMAKE_CXX_STANDARD 11)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


add_executable(all_at_one all_at_one.cpp)
target_link_libraries (all_at_one Threads::Threads)