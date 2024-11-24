#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <exception>

void checkFile(std::ifstream& file)//функция для проверки файла 
{
  if (!file.good())
  {
    throw std::exception("File does not exist.\n");
  }

  if (!file)
  {
    throw std::exception("File is not opened.\n");
  }

  if (file.peek() == EOF)
  {
    throw std::exception("File is empty.\n");
  }
}


std::vector<std::string> splitFileToTelegrams(int32_t telegramSize)//функция для разделения на телеграммы
{
    std::ifstream inFile(/*путь к файлу*/);//открываем поток
    
    checkFile(inFile);//проверяем файл

    std::vector<std::string> telegrams;//создаем вектор для хранения наших телеграмм
    std::string content((std::istreambuf_iterator<char>(inFile)), (std::istreambuf_iterator<char>()));
    //в строке content будет хранится содержимое нашего файла
    inFile.close();

    for(size_t i = 0; i < telegramSize; i += telegramSize)
    {
        telegrams.push_back(content.substr(i, telegramSize));//добавляем учатки текста определенного размера в наш вектор
    }       

    return telegrams;
}

