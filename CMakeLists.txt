cmake_minimum_required(VERSION 3.0.0)
project(monkvg VERSION 0.1.0)

# configuration
option(DO_VULKAN_BACKEND "Use Vulkan as the rendering backend.  NOT IMPLEMENTED" OFF) # Not Implemented Yet
option(DO_OPENGL_BACKEND "Use OpenGL as the rendering backend." ON)
option(DO_GLU_TESSELATION "Use GLU tesselator" ON)
option(DO_EXAMPLES "Build examples in the ./examples directory" ON)

# message(STATUS "Project Binary: ${PROJECT_BINARY_DIR}")

# require PkgConfig
find_package(PkgConfig REQUIRED)

if(DO_GLU_TESSELATION)
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
        CONFIGURE_COMMAND meson setup --prefix=${CMAKE_SOURCE_DIR}/thirdparty/glu -Ddefault_library=static  ${PROJECT_BINARY_DIR}/glu-build  ${PROJECT_BINARY_DIR}/glu-src
        BUILD_COMMAND ninja -C  ${PROJECT_BINARY_DIR}/glu-build
        INSTALL_COMMAND meson install -C  ${PROJECT_BINARY_DIR}/glu-build/
        TEST_COMMAND      ""
        LOG_DOWNLOAD      ON
        BUILD_ALWAYS 1
    )

    # now add the build GLU package
    # set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/thirdparty/glu/lib/pkgconfig")
    # find_package(GLU REQUIRED)
    pkg_check_modules(GLU REQUIRED glu)

    message(STATUS "MICAH GLU LIBRARIES ---\n: ${GLU_LIBRARIES}")
endif()

if(DO_OPENGL_BACKEND)
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
set(COMMON_INCLUDE ${COMMON_INCLUDE} ./src)

## Build the MonkVG library
add_library(monkvg ${COMMON_SOURCE} ${BACKEND_SOURCE})
target_include_directories(monkvg 
                            PUBLIC 
                            ./include 
                            PRIVATE 
                            ${COMMON_INCLUDE} 
                            ${BACKEND_INCLUDE})
target_link_libraries(monkvg ${GLU_LIBRARIES})



if (DO_EXAMPLES)
    ## Build Examples

    include(ExternalProject)

    # GLFW
    ExternalProject_Add(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        CMAKE_ARGS 
            -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/thirdparty/glfw
            -DCMAKE_BUILD_TYPE=Release

            LOG_DOWNLOAD 1 LOG_UPDATE 1 LOG_CONFIGURE 1 LOG_BUILD 1 LOG_INSTALL 1
    )

    # now add the build glfw3 OpenGL windowing package
    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/thirdparty/glfw/lib/cmake")
    find_package(glfw3 REQUIRED)
    set(GLFW_LIBRARIES
    ${CMAKE_SOURCE_DIR}/thirdparty/glfw/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX})

    # GLEW library
    ExternalProject_Add(glew PREFIX glew
        GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git

        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/thirdparty/glew
            -DCMAKE_BUILD_TYPE=Release

        LOG_DOWNLOAD 1 LOG_UPDATE 1 LOG_CONFIGURE 1 LOG_BUILD 1 LOG_INSTALL 1
    )    

    set(glew_DIR ${CMAKE_SOURCE_DIR}/thirdparty/glew/lib/cmake/glew)
    find_package(glew REQUIRED)
    
    set(GLEW_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/glew/include)
    set(GLEW_LIBRARIES
    ${CMAKE_SOURCE_DIR}/thirdparty/glew/lib/${CMAKE_STATIC_LIBRARY_PREFIX}GLEW${CMAKE_STATIC_LIBRARY_SUFFIX})

    # GLM Math Library
    ExternalProject_Add(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        CMAKE_ARGS 
            -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/thirdparty/glm 
            -DCMAKE_BUILD_TYPE=Release
            -DBUILD_TESTING=0

        LOG_DOWNLOAD 1 LOG_UPDATE 1 LOG_CONFIGURE 1 LOG_BUILD 1 LOG_INSTALL 1
    )

    # now add the build glm package
    set(glm_DIR ${CMAKE_SOURCE_DIR}/thirdparty/glm/lib/cmake/glm)
    find_package(glm REQUIRED)
    set(GLM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/glm/include)


    add_executable(glfw_hello_world examples/glfw_hello_world.cpp)
    add_dependencies(glm glew glfw glu monkvg)
    target_include_directories(glfw_hello_world 
                                PRIVATE 
                                ${CMAKE_SOURCE_DIR}/include/MonkVG
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
    
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
