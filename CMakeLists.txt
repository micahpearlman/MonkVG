cmake_minimum_required(VERSION 3.0.0)
project(monkvg VERSION 0.1.0)

# configuration
option(MKVG_DO_VULKAN_BACKEND "Use Vulkan as the rendering backend.  NOT IMPLEMENTED" OFF) # Not Implemented Yet
option(MKVG_DO_OPENGL_BACKEND "Use OpenGL as the rendering backend." ON)
option(MKVG_DO_GLU_TESSELATION "Use GLU tesselator" ON)
option(MKVG_DO_EXAMPLES "Build examples in the ./examples directory" ON)


# require PkgConfig
find_package(PkgConfig REQUIRED)

# External Project Handler
include(build_external_proj.cmake)

if(MKVG_DO_GLU_TESSELATION)
    add_subdirectory(thirdparty/glu)
    set(GLU_LIBRARIES glu_tess glu_util)
    set(GLU_INCLUDE_DIRS thirdparty/glu/include)
endif()

if(MKVG_DO_OPENGL_BACKEND)
    find_package(OpenGL REQUIRED)

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
set(COMMON_INCLUDE ${COMMON_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/src)

# GLES2-BC library 
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gles2-bc)

## Build the MonkVG library
add_library(monkvg ${COMMON_SOURCE} ${BACKEND_SOURCE})
add_dependencies(monkvg ${GLU_LIBRARIES} gles2-bc)
target_include_directories(monkvg 
                            PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR}/include 
                            PRIVATE 
                            ${COMMON_INCLUDE} 
                            ${BACKEND_INCLUDE})
target_link_libraries(monkvg ${GLU_LIBRARIES} gles2-bc)



if (MKVG_DO_EXAMPLES)
    ## Build Examples

    include(ExternalProject)

    # Platform Specific  librarys like GLX on Linux
    set(PLATFORM_LIBS )
    if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
        set(PLATFORM_LIBS ${PLATFORM_LIBS} OpenGL::GLX)
    endif()


    find_package(glfw3 REQUIRED)
    if (${CMAKE_SYSTEM_NAME} MATCHES Darwin) 
        # M1s have an issue with GLFW includes.  See: https://stackoverflow.com/questions/67373307/macos-m1-fatal-error-glfw-glfw3-h-file-not-found
        set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIRS} /opt/homebrew/include)
    endif()

    
    # GLEW library
    set(GLEW_OPTIONS "")
    list(APPEND GLEW_OPTIONS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew)
    list(APPEND GLEW_OPTIONS -DCMAKE_BUILD_TYPE=Release)
    build_external_git_project(glew https://github.com/Perlmint/glew-cmake.git ${GLEW_OPTIONS})

    # set(glew_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew/lib/cmake/glew)
    find_package(glew REQUIRED PATHS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew/lib/cmake/glew)
    
    set(GLEW_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew/include)
    set(GLEW_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew/lib/${CMAKE_STATIC_LIBRARY_PREFIX}GLEW${CMAKE_STATIC_LIBRARY_SUFFIX})

    find_package(glm REQUIRED)

    # GLFW Hello World
    add_executable(glfw_hello_world examples/glfw_hello_world.cpp)
    add_dependencies(glfw_hello_world monkvg)
    target_include_directories(glfw_hello_world 
                                PRIVATE 
                                ${CMAKE_CURRENT_SOURCE_DIR}/include/MonkVG
                                ${GLEW_INCLUDE_DIRS}
                                ${GLM_INCLUDE_DIRS}
                                ${GLFW_INCLUDE_DIRS}                                
                                )
    target_link_libraries(glfw_hello_world  PUBLIC
                            monkvg
                            ${GLU_LIBRARIES} # Required by MonkVG
                            glfw   
                            ${GLEW_LIBRARIES}                     
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
                                ${GLEW_INCLUDE_DIRS}
                                ${GLM_INCLUDE_DIRS}
                                ${GLFW_INCLUDE_DIRS}
                                )
    target_link_libraries(tiger  PUBLIC
                            monkvg
                            ${GLU_LIBRARIES} # Required by MonkVG
                            glfw   
                            ${GLEW_LIBRARIES}                     
                            OpenGL::GL
                            ${PLATFORM_LIBS}
                            ${CMAKE_DL_LIBS}
                            pthread
                            )

    
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
