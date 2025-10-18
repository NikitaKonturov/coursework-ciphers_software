#include "cipher_exceptions/cipher_exceptions.hpp"
#include "viginer_cipher/viginer_cipher.hpp"
// основная библиотека библиотека для экспорта с++ в python 
#include <pybind11/pybind11.h>
// библиотека для экспорта stl библиотеки
#include <pybind11/stl.h>



// главный макрос позволяющий экспортировать функции и типы данных в python
PYBIND11_MODULE(Viginer_cipher, m) {
    m.doc() = "Шифр Виженера";
    m.def("encript", &encript);
    m.def("decript", &decript);
    m.def("gen_keys", &gen_keys);
    m.def("get_key_propertys", &get_key_propertys);
    pybind11::register_exception<KeyPropertyError>(m, "KeyPropertyError"); // регистрация класса ошибки валидности свойств ключа
    pybind11::register_exception<InvalidKey>(m, "InvalidKey"); // регистрация класса ошибки валидности ключа
    pybind11::register_exception<InvalidOpenText>(m, "InvalidOpenText"); // регистрация класса валидности открытого текста 
}