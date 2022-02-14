cmake_minimum_required(VERSION 3.0.0)
project(monkvg VERSION 0.1.0)

# configuration
set(DO_VULKAN_BACKEND FALSE) # Not Implemented Yet
set(DO_OPENGL_BACKEND TRUE)
set(DO_GLU_TESSELATION TRUE)
set(DO_EXAMPLES TRUE)

message("Project Binary: ${PROJECT_BINARY_DIR}")

find_program(Meson_EXECUTABLE meson)
if(NOT Meson_EXECUTABLE)
   message(FATAL_ERRORO
       "Meson is required!")
   
endif()

if(DO_GLU_TESSELATION)
    # OpenGL GLU project for the tesselation
    # Uses Meson for building
    include(ExternalProject)
    ExternalProject_Add(glu
        GIT_REPOSITORY  https://gitlab.freedesktop.org/mesa/glu.git
        GIT_TAG           master
        SOURCE_DIR        "glu-src"
        BINARY_DIR        "glu-build"    
        CONFIGURE_COMMAND meson setup --prefix=${CMAKE_SOURCE_DIR}/thirdparty/glu -Ddefault_library=static  ${PROJECT_BINARY_DIR}/glu-build  ${PROJECT_BINARY_DIR}/glu-src
        BUILD_COMMAND ninja -C  ${PROJECT_BINARY_DIR}/glu-build
        INSTALL_COMMAND meson install -C  ${PROJECT_BINARY_DIR}/glu-build/
        TEST_COMMAND      ""
        LOG_DOWNLOAD      ON
        BUILD_ALWAYS 1
    )

    # now add the build GLU package
    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/thirdparty/glu/lib/pkgconfig")
    find_package(GLU REQUIRED)
endif()

if(DO_OPENGL_BACKEND)
    set(BACKEND_SOURCE ${BACKEND_SOURCE}
        ./src/opengl/glBatch.cpp
        ./src/opengl/glContext.cpp
        ./src/opengl/glFont.cpp
        ./src/opengl/glImage.cpp
        ./src/opengl/glPaint.cpp
        ./src/opengl/glPath.cpp)
    set(BACKEND_INCLUDE ${BACKEND_INCLUDE}
        ./src/opengl
        ${GLU_INCLUDE_DIRS}
        ./thirdparty/gles2-bc/Sources/OpenGLES
        ./thirdparty/gles2-bc/Sources/OpenGLES/OpenGLES11
        ./thirdparty/gles2-bc/Sources/OpenGLES/OpenGLES20)    
endif()

set(COMMON_SOURCE ${COMMON_SOURCE}
    ./src/mkBaseObject.cpp
    ./src/mkBatch.cpp
    ./src/mkContext.cpp
    ./src/mkFont.cpp
    ./src/mkImage.cpp
    ./src/mkMath.cpp
    ./src/mkPaint.cpp
    ./src/mkParameter.cpp
    ./src/mkPath.cpp
    ./src/mkVGU.cpp
)
set(COMMON_INCLUDE ${COMMON_INCLUDE} ./src)

## Build the MonkVG library
add_library(monkvg ${BACKEND_SOURCE})
target_include_directories(monkvg 
                            PUBLIC 
                            ./include 
                            PRIVATE 
                            ${COMMON_INCLUDE} 
                            ${BACKEND_INCLUDE})
target_link_libraries(monkvg ${GLU_LIBRARIES})


if (DO_EXAMPLES)
    ## Build Examples
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
