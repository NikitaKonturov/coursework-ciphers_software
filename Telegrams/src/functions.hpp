#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <random>
#include <algorithm>
#include <locale>


void checkFile(std::wifstream& fileIn);
std::vector<std::wstring> generateTelegrams(const std::wstring& filename, int telegramLength, int telegramCount);
#endif //FUNCTIONS_HPP