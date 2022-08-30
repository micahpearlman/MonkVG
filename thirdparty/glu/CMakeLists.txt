cmake_minimum_required(VERSION 3.0.0)
project(glu VERSION 0.1.0)

find_package(OpenGL REQUIRED)

## Tesselation Library
add_library(glu_tess
    ${PROJECT_SOURCE_DIR}/src/libtess/dict.c
    ${PROJECT_SOURCE_DIR}/src/libtess/geom.c
    ${PROJECT_SOURCE_DIR}/src/libtess/memalloc.c
    ${PROJECT_SOURCE_DIR}/src/libtess/mesh.c
    ${PROJECT_SOURCE_DIR}/src/libtess/normal.c
    ${PROJECT_SOURCE_DIR}/src/libtess/priorityq-heap.c
    ${PROJECT_SOURCE_DIR}/src/libtess/priorityq.c
    ${PROJECT_SOURCE_DIR}/src/libtess/render.c
    ${PROJECT_SOURCE_DIR}/src/libtess/sweep.c
    ${PROJECT_SOURCE_DIR}/src/libtess/tess.c
    ${PROJECT_SOURCE_DIR}/src/libtess/tessmono.c
)


target_include_directories(glu_tess PRIVATE 
                            ${PROJECT_SOURCE_DIR}/src/libtess
                            ${PROJECT_SOURCE_DIR}/src/include
                            ${PROJECT_SOURCE_DIR}/include)

add_library(glu_util
    ${PROJECT_SOURCE_DIR}/src/libutil/error.c
    ${PROJECT_SOURCE_DIR}/src/libutil/glue.c
)

target_include_directories(glu_util PRIVATE 
                            ${PROJECT_SOURCE_DIR}/src/libutil
                            ${PROJECT_SOURCE_DIR}/src/include
                            ${PROJECT_SOURCE_DIR}/include)
