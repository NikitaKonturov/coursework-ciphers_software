#include "cipher_exceptions/cipher_exceptions.hpp"
// основная библиотека библиотека для экспорта с++ в python 
#include <pybind11/pybind11.h>
// библиотека для экспорта stl библиотеки
#include <pybind11/stl.h>



// главный макрос позволяющий экспортировать функции и типы данных в python
PYBIND11_MODULE(cpp_exeptions, m) {
    m.doc() = "cpp_exeptions"; // документация к модулю питона, обязана содержать имя библиотеки на английском которое будет отображаться во frontend
    pybind11::register_exception<KeyPropertyError>(m, "KeyPropertyError"); // регистрация класса ошибки валидности свойств ключа
    pybind11::register_exception<InvalidKey>(m, "InvalidKey"); // регистрация класса ошибки валидности ключа
    pybind11::register_exception<InvalidOpenText>(m, "InvalidOpenText"); // регистрация класса валидности открытого текста
}

