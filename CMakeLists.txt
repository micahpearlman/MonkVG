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
    # OpenGL GLU project for the tesselation
    # Uses Meson for building
    include(ExternalProject)

    # Require Meson for GLU building
    find_program(Meson_EXECUTABLE meson)
    if(NOT Meson_EXECUTABLE)
    message(FATAL_ERRORO
        "Meson is required!")
    
    endif()

    ExternalProject_Add(glu
        GIT_REPOSITORY  https://gitlab.freedesktop.org/mesa/glu.git
        GIT_TAG           master
        SOURCE_DIR        "glu-src"
        BINARY_DIR        "glu-build"    
        CONFIGURE_COMMAND meson setup --prefix=${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glu -Ddefault_library=static  ${PROJECT_BINARY_DIR}/glu-build  ${PROJECT_BINARY_DIR}/glu-src
        BUILD_COMMAND ninja -C  ${PROJECT_BINARY_DIR}/glu-build
        INSTALL_COMMAND meson install -C  ${PROJECT_BINARY_DIR}/glu-build/
        TEST_COMMAND      ""
        LOG_DOWNLOAD      ON
        BUILD_ALWAYS 1
    )

    # now add the build GLU package
    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glu/lib/pkgconfig")
    # find_package(GLU REQUIRED PATHS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glu/lib/pkgconfig)
    pkg_check_modules(GLU REQUIRED glu)

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
add_dependencies(monkvg glu gles2-bc)
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

    # GLFW Library
    set(GLFW_OPTIONS "")
    list(APPEND GLFW_OPTIONS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw)
    list(APPEND GLFW_OPTIONS -DCMAKE_BUILD_TYPE=Release)
    build_external_git_project(glfw https://github.com/glfw/glfw.git ${GLFW_OPTIONS})

    # now add the build glfw3 OpenGL windowing package
    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw/lib/cmake")
    find_package(glfw3 REQUIRED)
    set(GLFW_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX})

    
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

    # GLM Math Library
    set(GLM_OPTIONS "")
    list(APPEND GLM_OPTIONS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm)
    list(APPEND GLM_OPTIONS -DCMAKE_BUILD_TYPE=Release)
    list(APPEND GLM_OPTIONS -DBUILD_TESTING=0)
    build_external_git_project(glm https://github.com/g-truc/glm.git ${GLM_OPTIONS})

    # now add the build glm package
    set(glm_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm/lib/cmake/glm)
    find_package(glm REQUIRED)
    set(GLM_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm/include)

    # GLFW Hello World
    add_executable(glfw_hello_world examples/glfw_hello_world.cpp)
    add_dependencies(glfw_hello_world monkvg)
    target_include_directories(glfw_hello_world 
                                PRIVATE 
                                ${CMAKE_CURRENT_SOURCE_DIR}/include/MonkVG
                                ${GLEW_INCLUDE_DIRS}
                                ${GLM_INCLUDE_DIRS}
                                )
    target_link_libraries(glfw_hello_world  PUBLIC
                            monkvg
                            ${GLU_LIBRARIES} # Required by MonkVG
                            ${GLFW_LIBRARIES}   
                            ${GLEW_LIBRARIES}                     
                            OpenGL::GL
                            OpenGL::GLX
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
                                )
    target_link_libraries(tiger  PUBLIC
                            monkvg
                            ${GLU_LIBRARIES} # Required by MonkVG
                            ${GLFW_LIBRARIES}   
                            ${GLEW_LIBRARIES}                     
                            OpenGL::GL
                            OpenGL::GLX
                            ${CMAKE_DL_LIBS}
                            pthread
                            )

    
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
