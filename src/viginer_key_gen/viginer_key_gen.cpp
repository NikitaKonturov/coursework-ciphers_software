#include "viginer_key_gen.hpp"

bool is_rus_alpha(uint8_t symbol) {
    return (symbol >= 192 && symbol <= 255) || symbol == 184 || symbol == 168;
} //Проверка на русский символ

bool is_eng_alpha(uint8_t symbol) {
    return (symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z');
} //Проверка на английский символ
