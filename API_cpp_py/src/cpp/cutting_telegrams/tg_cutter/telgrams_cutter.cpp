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

    // Вычисляем размер файла
    std::streamsize fileSize = (file.tellg()) / 2 - 1; // Размер файла в символах UTF-16 (без BOM)
    file.seekg(2, std::ios::beg); // Пропускаем BOM

    // Проверка возможности генерации телеграмм
    if (telegramLength * telegramCount > fileSize) {
        throw std::runtime_error("There is not enough space in the file to generate the specified number of telegrams.");
    }

    // Генерация непересекающихся начальных позиций
    std::set<int> startPositions;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, fileSize - telegramLength);

    while (startPositions.size() < static_cast<size_t>(telegramCount)) {
        int startPos = dis(gen) * 2; // Смещение в байтах
        bool intersects = false;

        for (int pos : startPositions) {
            if (std::abs(pos - startPos) < telegramLength * 2) { // Проверка пересечения
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
    std::vector<char> buffer(telegramLength * 2); // Буфер для одного телеграмма

    for (int startPos : startPositions) {
        file.seekg(startPos + 2, std::ios::beg); // Начало телеграммы
        file.read(buffer.data(), telegramLength * 2); // Чтение телеграммы

        if (!file) {
            throw std::runtime_error("Error reading telegram data.");
        }

        // Декодируем из UTF-16 в wstring
        std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10FFFF, std::little_endian>> converter;
        std::wstring decodeTelegram = converter.from_bytes(buffer.data(), buffer.data() + telegramLength * 2);

        // Сохраняем результат
        telegrams.push_back(decodeTelegram);
    }
 
    std::cout << telegrams.size() << '\n'; 

    file.close();
    return telegrams;
}
