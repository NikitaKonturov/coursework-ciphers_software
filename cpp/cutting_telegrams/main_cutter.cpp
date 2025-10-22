#include "tg_cutter/telgrams_cutter.hpp"
// основная библиотека библиотека для экспорта с++ в python 
#include <pybind11/pybind11.h>
// библиотека для экспорта stl библиотеки
#include <pybind11/stl.h>

PYBIND11_MODULE(telegrams_cutter, m) {
    m.doc() = "telegrams cutter";
    m.def("cut_telegrams", &generateTelegrams);
}