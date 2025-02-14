#ifndef PERMUTATION_GENERATOR
#define PERMUTATION_GENERATOR
#include <iostream>
#include "../HMAC_DRBG/DRBG/DRBG.hpp"
#include <vector>


// генерация случайной подстановки размерности permutSize
std::vector<int32_t> generat_permutation(std::vector<int32_t> sourcePermut, HMAC_DRBG &gen);
// первый элемент линейной конгруэнтной последовательности, для корректной работы его нужно инициализировать функцей setRand(uint64_t seed)
extern uint32_t x_n;
// Функция для инициализации первого элемента линейной конгруэнтной последовательности
void setRand(uint32_t seed);
// Функция генерации случайного числа в диапазоне [0, 2^(32)] 
uint64_t randomNumber();

#endif //PERMUTATION_GENERATOR
