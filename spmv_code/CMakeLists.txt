cmake_minimum_required(VERSION 3.0)
project (ws7)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

add_executable(sparse_matrix_mult.out
    sparse_matrix_mult.c
    matrix.c
    vector.c
    coordinate_form.c
    csc_format.c
    csr_format.c
    csr_extraction_format.c
    ellpack_format.c
    jds_format.c
)

target_include_directories(sparse_matrix_mult.out
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
 )

target_link_libraries(sparse_matrix_mult.out m)
