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
std::vector<std::string> generateTelegrams(std::ifstream& file, int telegramLength, int telegramCount);


#endif // TELEGRAMS_CUTTER
