#include "functions.hpp"

//функция для проверки файла
void checkFile(std::wifstream& fileIn)
{
  if(!fileIn.is_open())
  {
    throw std::runtime_error("!The file could not be opened!\n");
  }

  if(!fileIn.good())
  {
    throw std::runtime_error("!The file does not exist!\n");
  }

  if(fileIn.peek() == EOF)
  {
    throw std::runtime_error("!The file is empty!\n");
  }
}


// функция для генерации случайных непересекающихся телеграмм
std::vector<std::wstring> generateTelegrams(const std::wstring& filename, int telegramLength, int telegramCount)
{
    // открываем файл в режиме чтения в бинарном режиме, чтобы считать размер
    std::wifstream fileIn(filename);
    checkFile(fileIn);
    fileIn.close();

    std::wifstream file(filename, std::ios::binary | std::ios::ate);
    file.imbue(std::locale("ru_RU.UTF-8"));

    // вычисляем размер файла
    std::wifstream::pos_type fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // проверка возможности генерации телеграмм
    if (telegramLength * telegramCount > fileSize) 
    {
        throw std::runtime_error("There is not enough space in the file to generate the specified number of telegrams");
    }

    // создаем набор случайных начальных позиций телеграмм
    std::set<int> startPositions;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, static_cast<int>(fileSize) - telegramLength);

    // генерируем непересекающиеся начальные позиции
    while (startPositions.size() < static_cast<size_t>(telegramCount)) 
    {
        int startPos = dis(gen);
        
        // проверяем, пересекается ли новая позиция с существующими
        bool intersects = false;
        for (int pos : startPositions) 
        {
            if (std::abs(pos - startPos) < telegramLength) 
            {
                intersects = true;
                break;
            }
        }

        if (!intersects) 
        {
            startPositions.insert(startPos);
        }
    }

    // Сортируем позиции для удобства чтения
    std::vector<int> positions(startPositions.begin(), startPositions.end());
    std::sort(positions.begin(), positions.end());

    // Считываем телеграммы
    std::vector<std::wstring> telegrams;
    wchar_t* buffer = new wchar_t[telegramLength];

    for (int startPos : positions) 
    {
        file.seekg(startPos, std::ios::beg);
        file.read(buffer, telegramLength);
        telegrams.emplace_back(buffer, telegramLength);
    }

    delete[] buffer;
    file.close();

    return telegrams;
}

