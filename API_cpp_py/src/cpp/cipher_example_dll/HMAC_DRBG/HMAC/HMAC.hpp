#ifndef HMAC_HPP
#define HMAC_HPP
#include <iostream>
#include "../SHA-256/SHA-256.hpp"
#include <bitset>
#include <vector>
#include <array>

// Ковертация в вектор байтов
std::vector<uint8_t> convert_to_bytes(std::array<uint32_t, 8> data);
// Подготовка ключа
std::vector<uint8_t> key_preparing(std::vector<uint8_t> key);
// Вычисление xor с opad
std::vector<uint8_t> opad(std::vector<uint8_t> key);
// Вычисление xor с ipad
std::vector<uint8_t> ipad(std::vector<uint8_t> key);
// Конкатинация
std::vector<uint8_t> concatination(std::vector<uint8_t> first, std::vector<uint8_t> second);
// Вычисление самого значение HMAC
std::array<uint32_t, 8> hash_message_authentication_code(std::vector<uint8_t> key, std::vector<uint8_t> message);
#endif // HMAC_HPP
