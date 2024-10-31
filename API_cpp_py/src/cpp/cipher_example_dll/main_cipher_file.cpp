#include "cipher_first\HPC-cipher.hpp"
// основная библиотека библиотека для экспорта с++ в python 
#include <pybind11/pybind11.h>
// библиотека для экспорта stl библиотеки
#include <pybind11/stl.h>



// главный макрос позволяющий экспортировать функции и типы данных в python
PYBIND11_MODULE(HPC_cipher, m) {
    m.doc() = "HPC_cipher"; // документация к модулю питона, обязана содержать имя библиотеки на английском которое будет отображаться во frontend
    m.def("encript", &encript); // экспортирует функцию из нашей библиотеки в python она отабражаться в python по имени указываемым первым параметром 
    m.def("gen_keys", &gen_keys); // аналогично
    m.def("get_key_propertys", &get_key_propertys); // аналогично
    m.def("decript", &decript); // аналогично
    pybind11::register_exception<KeyPropertyError>(m, "keyPropertyError"); // регистрация класса ошибки валидности свойств ключа
    pybind11::register_exception<InvalidKey>(m, "InvalidKey"); // регистрация класса ошибки не валидного ключа
}

