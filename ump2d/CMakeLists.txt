cmake_minimum_required(VERSION 3.5)
project(ump2d)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

find_package(VTK REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})

find_package(Boost COMPONENTS program_options REQUIRED)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" "${CMAKE_ROOT}/Modules")
find_package(METIS REQUIRED)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${METIS_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${METIS_LIBRARIES})