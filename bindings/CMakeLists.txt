

## Build MonkVG Python Bindings

# make sure Python3 is found
find_package(Python3 COMPONENTS Interpreter Development)
if (Python3_FOUND)
    message(STATUS "Python3 Found: ${Python3_EXECUTABLE}")
    message(STATUS "Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
    message(STATUS "Python3_LIBRARIES: ${Python3_LIBRARIES}")
    message(STATUS "Python3_VERSION: ${Python3_VERSION}")
else()
    message(FATAL_ERROR "Python3 Not Found")
endif()

## Setup a virtual environment for the Python bindings
set(VENV_PATH ${CMAKE_BINARY_DIR}/venv)

# Create a virtual environment if it does not exist
if (NOT EXISTS ${VENV_PATH})
    execute_process(COMMAND ${Python3_EXECUTABLE} -m venv ${VENV_PATH})
endif()

# install pybind11 in the virtual environment
execute_process(COMMAND ${VENV_PATH}/bin/pip install pybind11)

# Add the virtual environment to the path
set(PYTHON_EXECUTABLE ${VENV_PATH}/bin/python)
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_path('include'))"
    OUTPUT_VARIABLE PYTHON_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))"
    OUTPUT_VARIABLE PYTHON_LIBRARY_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

## add pybind11
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/pybind ${CMAKE_BINARY_DIR}/thirdparty/pybind_build)
pybind11_add_module(monkvg_py ${CMAKE_CURRENT_SOURCE_DIR}/python/monkvg_py.cpp)

target_include_directories(monkvg_py PRIVATE ${PYTHON_INCLUDE_DIR})
target_link_directories(monkvg_py PRIVATE ${PYTHON_LIBRARY_DIR})
target_link_libraries(monkvg_py PRIVATE monkvg pybind11::module)

# add_library(monkvg_py MODULE ${CMAKE_CURRENT_SOURCE_DIR}/python/monkvg_py.cpp)
# target_link_libraries(monkvg_py PRIVATE monkvg pybind11::module)
# # Set target properties for compatibility with Python
# set_target_properties(monkvg_py PROPERTIES
#     PREFIX ""
#     SUFFIX ".so"  # Adjust for your platform
# )