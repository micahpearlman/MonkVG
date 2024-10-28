## Build Examples

# suppress annoying C++ 20 deprecation warnings
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-deprecated-enum-enum-conversion)
endif()

# Platform Specific  librarys like GLX on Linux
set(PLATFORM_LIBS )
if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
    # set(PLATFORM_LIBS ${PLATFORM_LIBS} OpenGL::GLX)
    find_package(glfw3 REQUIRED)
endif()



if (APPLE)
    find_package(glfw3 3.4 REQUIRED)

    # M1s have an issue with GLFW includes.  See: https://stackoverflow.com/questions/67373307/macos-m1-fatal-error-glfw-glfw3-h-file-not-found
    set(PLATFORM_LIBS ${PLATFORM_LIBS} "-framework Cocoa" "-framework OpenGL" "-framework IOKit")
    set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIRS} /opt/homebrew/include)
endif(APPLE)


if (MKVG_DO_OPENGL_BACKEND)
    ## GLFW Hello World
    add_executable(glfw_hello_world glfw_hello_world.cpp)
    add_dependencies(glfw_hello_world monkvg)
    target_include_directories(glfw_hello_world 
                                PRIVATE 
                                ${GLM_INCLUDE_DIRS}
                                ${GLFW_INCLUDE_DIRS}
                                ${CMAKE_SOURCE_DIR}/thirdparty/stb                               
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

    ## Font Example
    add_executable(font_example font_example.cpp)
    add_dependencies(font_example monkvg)
    target_include_directories(font_example 
                                PRIVATE 
                                ${GLM_INCLUDE_DIRS}
                                ${GLFW_INCLUDE_DIRS}
                                ${CMAKE_SOURCE_DIR}/thirdparty/stb                               
                                )
    target_link_libraries(font_example  PUBLIC
                            monkvg
                            ${GLU_LIBRARIES} # Required by MonkVG
                            glfw   
                            OpenGL::GL
                            ${PLATFORM_LIBS}
                            ${CMAKE_DL_LIBS}
                            pthread
                            )

    ## Tiger
    add_executable(tiger tiger.cpp tiger_paths.c)
    add_dependencies(tiger monkvg)
    target_include_directories(tiger 
                                PRIVATE 
                                ${CMAKE_SOURCE_DIR}/.
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
endif() # MKVG_DO_OPENGL_BACKEND

## Vulkan Hello World
if (MKVG_DO_VULKAN_BACKEND)
    add_executable(vulkan_hello_world vulkan_hello_world.cpp)
    add_dependencies(vulkan_hello_world monkvg)
    
    target_include_directories(vulkan_hello_world 
                                PRIVATE 
                                ${GLM_INCLUDE_DIRS}
                                ${GLFW_INCLUDE_DIRS}
                                ${CMAKE_SOURCE_DIR}/thirdparty/stb                               
                                )
    target_link_libraries(vulkan_hello_world  PRIVATE
                            monkvg
                            ${GLU_LIBRARIES} # Required by MonkVG
                            glfw                               
                            ${PLATFORM_LIBS}
                            ${CMAKE_DL_LIBS}
                            pthread
                            )
endif() # MKVG_DO_VULKAN_BACKEND
                            