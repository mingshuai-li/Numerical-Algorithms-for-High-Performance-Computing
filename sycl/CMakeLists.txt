cmake_minimum_required(VERSION 3.5)
project(sparse)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(DEVICE "cpu" CACHE STRING "compute device")
set(DEVICE_OPTIONS "cpu" "sm_50" "sm_60" "sm_70" "sm_75" "sm_86")
set_property(CACHE DEVICE PROPERTY STRINGS ${DEVICE_OPTIONS})

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS program_options filesystem)
message(STATUS "Boost version: ${Boost_VERSION}")


add_executable(${CMAKE_PROJECT_NAME} src/main.cpp
                                     src/util.cpp
                                     src/auxiliary.cpp
                                     src/external/mmio.c
                                     src/solvers/mat_vec_mult_crs.cpp
                                     src/solvers/mat_vec_milt_ellpack.cpp)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC src src/solvers src/external)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC Boost::program_options Boost::filesystem)


if (DEVICE MATCHES "sm_*")
    set(HIPSYCL_TARGETS "cuda:${DEVICE}")
else()
    set(HIPSYCL_TARGETS "omp")
endif()

find_package(hipSYCL REQUIRED)

add_sycl_to_target(TARGET ${CMAKE_PROJECT_NAME}
                   SOURCES src/solvers/mat_vec_mult_crs.cpp
                           src/solvers/mat_vec_milt_ellpack.cpp)
