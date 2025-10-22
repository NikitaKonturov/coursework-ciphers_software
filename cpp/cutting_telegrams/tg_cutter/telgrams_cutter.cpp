#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <random>
#include <algorithm>
#include <codecvt>
#include <iterator>
#include "telgrams_cutter.hpp"

//функция для проверки файла
void checkFile(std::ifstream& fileIn)
{
  if(!fileIn.is_open())
  {
    throw std::runtime_error("The file could not be opened!\n");
  }

  if(!fileIn.good())
  {
    throw std::runtime_error("The file does not exist!\n");
  }

  if(fileIn.peek() == EOF)
  {
    throw std::runtime_error("The file is empty!\n");
  }
}


// функция для генерации случайных непересекающихся телеграмм
std::vector<std::wstring> generateTelegrams(std::string pathToFile, int telegramLength, int telegramCount)
{
    // Открываем файл
    std::ifstream file(pathToFile, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("The file could not be opened!");
    }

    // Вычисляем размер файла в символах UTF-16 (без BOM)
    std::streamsize fileSizeBytes = file.tellg();
    std::streamsize fileSizeChars = (fileSizeBytes - 2) / 2;
    
    file.seekg(2, std::ios::beg); // Пропускаем BOM

    // Проверка возможности генерации телеграмм
    if (telegramLength * telegramCount > fileSizeChars) {
        throw std::runtime_error("There is not enough space in the file to generate the specified number of telegrams.");
    }

    // Генерация непересекающихся начальных позиций (в символах UTF-16)
    std::set<int> startPositions;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, fileSizeChars - telegramLength);

    while (startPositions.size() < static_cast<size_t>(telegramCount)) {
        int startPos = dis(gen); // Позиция в символах UTF-16
        bool intersects = false;

        for (int pos : startPositions) {
            if (std::abs(pos - startPos) < telegramLength) { // Проверка пересечения в символах
                intersects = true;
                break;
            }
        }

        if (!intersects) {
            startPositions.insert(startPos);
        }
    }

    // Читаем телеграммы
    std::vector<std::wstring> telegrams;
    std::vector<uint16_t> buffer(telegramLength); // Буфер для UTF-16 символов

    for (int startPos : startPositions) {
        // Пересчитываем позицию в байтах: каждый символ UTF-16 = 2 байта
        std::streampos bytePos = 2 + (startPos * 2); // BOM + позиция в байтах
        
        file.seekg(bytePos, std::ios::beg);
        file.read(reinterpret_cast<char*>(buffer.data()), telegramLength * 2); // Читаем telegramLength * 2 байт

        if (!file) {
            throw std::runtime_error("Error reading telegram data.");
        }

        // Создаем wstring из UTF-16 данных
        // На Linux wchar_t обычно 4 байта, поэтому мы можем напрямую преобразовать UTF-16
        std::wstring telegram;
        telegram.reserve(telegramLength);
        
        for (int i = 0; i < telegramLength; ++i) {
            telegram += static_cast<wchar_t>(buffer[i]);
        }

        telegrams.push_back(telegram);
    }
 
    std::cout << telegrams.size() << '\n'; 

    file.close();
    return telegrams;
}
