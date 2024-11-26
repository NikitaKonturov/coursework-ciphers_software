#ifndef TELEGRAMS_CUTTER
#define TELEGRAMS_CUTTER
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <random>
#include <algorithm>


//функция для проверки файла
void checkFile(std::ifstream& fileIn);
// функция для генерации случайных непересекающихся телеграмм
std::vector<std::string> generateTelegrams(const std::string& filename, int telegramLength, int telegramCount);


#endif // TELEGRAMS_CUTTER
