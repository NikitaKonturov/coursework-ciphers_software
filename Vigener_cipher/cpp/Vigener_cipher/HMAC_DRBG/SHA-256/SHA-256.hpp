#ifndef SHA_256_HPP
#define SHA_256_HPP
#include <exception>
#include <iostream>
#include <bitset>
#include <vector>
#include <array>


// Расширение данных до размера кратного размеру 512
std::vector<std::array<uint32_t, 16>> data_expansion(std::vector<uint8_t> data);
// Цикличекий сдвиг в право
uint32_t right_rotate(uint32_t data, uint32_t count);
uint32_t c0(uint32_t word);
uint32_t c1(uint32_t word);
// Расширение блоков с 16 до 64 слов (каждое слово размером 32 бита)
std::vector<std::array<uint32_t, 64>> block_expansion(std::vector<std::bitset<512>> raw_blocks);
// Сумма 0
uint32_t sum0(uint32_t word);
// Сумма 1
uint32_t sum1(uint32_t word);
// Выбор 
uint32_t choice(uint32_t e, uint32_t f, uint32_t g);
// Мажоранта
uint32_t majority(uint32_t a, uint32_t b, uint32_t c);
// Функция сжатия блоков
void compress(std::array<uint32_t, 64> block, std::array<uint32_t, 8> &hash);
// Сама функция SHA-256
std::array<uint32_t, 8> Hash(std::vector<uint8_t> data);


#endif // HMAC_HPP
