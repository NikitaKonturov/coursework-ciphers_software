#ifndef HOMOPHONE_GENERATOR_HPP
#define HOMOPHONE_GENERATOR_HPP

#include <map>
#include <vector>
#include <string>
#include <random>
#include <stdexcept>
#include <iostream>

// Функция для генерации случайного числа в заданном диапазоне
int64_t get_random_number(const int64_t& leftBorder, const int64_t& rightBorder);

// Функция для вычисления частоты символов в тексте
std::map<wchar_t, double> get_frequencies(const std::string& text);

// Функция для генерации ключей для символов текста
std::map<wchar_t, std::vector<std::wstring>> generate_keys(
    int64_t leftBoarder,
    int64_t rightBoarder,
    std::string& text
);

#endif // HOMOPHONE_GENERATOR_HPP
