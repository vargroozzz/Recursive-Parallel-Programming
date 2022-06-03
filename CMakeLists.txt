cmake_minimum_required(VERSION 3.21)
project(RPP)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost)
find_package(OpenMP)
find_package(MPI)

add_compile_options("-O3")

add_executable(ublas src/ublas.cpp)
target_include_directories(ublas PRIVATE ${Boost_INCLUDE_DIRS} include)

add_executable(sequential src/sequntial.cpp)
target_include_directories(sequential PRIVATE ${Boost_INCLUDE_DIRS} include)

add_executable(openmp src/openmp.cpp)
target_include_directories(openmp PRIVATE ${Boost_INCLUDE_DIRS} include)
target_link_libraries(openmp PRIVATE OpenMP::OpenMP_CXX)

add_executable(openmpi src/openmpi.cpp)
target_include_directories(openmpi PRIVATE ${Boost_INCLUDE_DIRS} include)
target_link_libraries(openmpi PUBLIC MPI::MPI_CXX)
