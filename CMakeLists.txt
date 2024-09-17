cmake_minimum_required(VERSION 3.0.0)
project(monkvg VERSION 0.1.0)

## configuration
# Set CMP0077 policy to NEW
cmake_policy(SET CMP0077 NEW)
option(MKVG_DO_VULKAN_BACKEND "Use Vulkan as the rendering backend.  NOT IMPLEMENTED" OFF) # Not Implemented Yet
option(MKVG_DO_OPENGLES_BACKEND "Use OpenGL ES as the rendering backend. NOT IMPLEMENTED" OFF)
option(MKVG_DO_OPENGL_BACKEND "Use OpenGL as the rendering backend." ON)
option(MKVG_DO_GLU_TESSELATION "Use GLU tesselator" ON)
option(MKVG_DO_EXAMPLES "Build examples in the ./examples directory" ON)

if (MKVG_DO_VULKAN_BACKEND)
    message(FATAL_ERROR "Vulkan Backend Not Implemented Yet")
endif()
if (MKVG_DO_OPENGLES_BACKEND)
    message(FATAL_ERROR "OpenGL ES Backend Not Implemented Yet")
endif()

if (MKVG_DO_OPENGLES_BACKEND AND MKVG_DO_OPENGL_BACKEND)
    message(FATAL_ERROR "Cannot build both OpenGL and OpenGL ES backends MKVG_DO_OPENGLES_BACKEND: ${MKVG_DO_OPENGLES_BACKEND} MKVG_DO_OPENGL_BACKEND: ${MKVG_DO_OPENGL_BACKEND}" )
endif()

# C++ 20
set(CMAKE_CXX_STANDARD 20)

# require PkgConfig
find_package(PkgConfig REQUIRED)


## GLU Tesselation
if(MKVG_DO_GLU_TESSELATION)
    add_subdirectory(thirdparty/glu)
    set(GLU_LIBRARIES glu_tess glu_util)
    set(GLU_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glu/include)
    message(STATUS "GLU Tesselation Enabled")
    message(STATUS "GLU_INCLUDE_DIRS: ${GLU_INCLUDE_DIRS}")
    message(STATUS "GLU_LIBRARIES: ${GLU_LIBRARIES}")
endif()


## OpenGL Backend
if(MKVG_DO_OPENGL_BACKEND)
    if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
        # use vendor version of OpenGL if available
        set(OpenGL_GL_PREFERENCE "GLVND")
    endif()

    find_package(OpenGL REQUIRED)
    find_package(glm REQUIRED)
    if (APPLE)
        # for whatever reason apple requires explicit include path
        set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIRS} /opt/homebrew/include)
    endif(APPLE)


    set(BACKEND_SOURCE ${BACKEND_SOURCE}
        ./src/opengl/glBatch.cpp
        ./src/opengl/glContext.cpp
        ./src/opengl/glFont.cpp
        ./src/opengl/glImage.cpp
        ./src/opengl/glPaint.cpp
        ./src/opengl/glPath.cpp
        ./src/opengl/glShader.cpp)
    set(BACKEND_INCLUDE ${BACKEND_INCLUDE}
        ./src/opengl
        ${GLU_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS})

    set(MNKVG_COMPILE_DEFS ${MNKVG_COMPILE_DEFS} MNKVG_GL_BACKEND)


endif()



## setup common source 
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
set(COMMON_INCLUDE ${COMMON_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/src)


## Build the MonkVG library
message(STATUS "MNKVG_COMPILE_DEFS: ${MNKVG_COMPILE_DEFS}")
add_library(monkvg ${COMMON_SOURCE} ${BACKEND_SOURCE})
add_dependencies(monkvg ${GLU_LIBRARIES} ${MNKVG_DEPENDENCIES})
target_compile_definitions(monkvg PRIVATE ${MNKVG_COMPILE_DEFS})

target_include_directories(monkvg 
                            PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR}/include 
                            PRIVATE 
                            ${COMMON_INCLUDE} 
                            ${BACKEND_INCLUDE})
target_link_libraries(monkvg ${GLU_LIBRARIES} ${MNKVG_LIBRARIES})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(monkvg PRIVATE -Wno-deprecated-enum-enum-conversion)
endif()

if (MKVG_DO_EXAMPLES)
    ## Build Examples

    # suppress annoying C++ 20 deprecation warnings
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-Wno-deprecated-enum-enum-conversion)
    endif()

    # Platform Specific  librarys like GLX on Linux
    set(PLATFORM_LIBS )
    if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
        set(PLATFORM_LIBS ${PLATFORM_LIBS} OpenGL::GLX)
        find_package(glfw3 REQUIRED)

    endif()



    if (APPLE)
        find_package(glfw3 3.4 REQUIRED)
 
        # M1s have an issue with GLFW includes.  See: https://stackoverflow.com/questions/67373307/macos-m1-fatal-error-glfw-glfw3-h-file-not-found
        set(PLATFORM_LIBS ${PLATFORM_LIBS} "-framework Cocoa" "-framework OpenGL" "-framework IOKit")
        set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIRS} /opt/homebrew/include)
    endif(APPLE)

    
    
    # GLFW Hello World
    add_executable(glfw_hello_world examples/glfw_hello_world.cpp)
    add_dependencies(glfw_hello_world monkvg)
    target_include_directories(glfw_hello_world 
                                PRIVATE 
                                ${CMAKE_CURRENT_SOURCE_DIR}/include/MonkVG
                                ${GLM_INCLUDE_DIRS}
                                ${GLFW_INCLUDE_DIRS}
                                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb                               
                                )
    target_link_libraries(glfw_hello_world  PUBLIC
                            monkvg
                            ${GLU_LIBRARIES} # Required by MonkVG
                            glfw   
                            OpenGL::GL
                            ${PLATFORM_LIBS}
                            ${CMAKE_DL_LIBS}
                            pthread
                            )

    # Tiger
    add_executable(tiger examples/tiger.cpp examples/tiger_paths.c)
    add_dependencies(tiger monkvg)
    target_include_directories(tiger 
                                PRIVATE 
                                ${CMAKE_CURRENT_SOURCE_DIR}/.
                                ${CMAKE_CURRENT_SOURCE_DIR}/include/MonkVG
                                ${GLM_INCLUDE_DIRS}
                                ${GLFW_INCLUDE_DIRS}
                                )
    target_link_libraries(tiger  PUBLIC
                            monkvg
                            ${GLU_LIBRARIES} # Required by MonkVG
                            glfw   
                            OpenGL::GL
                            ${PLATFORM_LIBS}
                            ${CMAKE_DL_LIBS}
                            pthread
                            )


endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
