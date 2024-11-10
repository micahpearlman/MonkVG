/**
 * @file monkvg_py.cpp
 * @author Micah Pearlman (micahpearlman@gmail.com)
 * @brief Python bindings for MonkVG
 * @version 0.1
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <pybind11/pybind11.h>


namespace py = pybind11;

int add(int i, int j) {
    return i + j;
}

PYBIND11_MODULE(monkvg_py, m) {
    m.def("add", &add, "A function that adds two numbers");
}
