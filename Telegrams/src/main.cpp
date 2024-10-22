#include <iostream>
#include <fstream>
#include <string>
#include <vector>


std::vector<std::string> splitFileToTelegrams(int32_t telegramSize)
{
    std::ifstream inFile("D:\ Рабочий стол\2 курс\ курсовая работа 2 курс\ Курсова работа(разбиение txt на телеграммы)\ src\ inputTxt.txt");
    if(!inFile.is_open())
    {
        std::cerr << "Error openning input file!" << '\n';
    }

    std::vector<std::string> telegrams;
    std::string content((std::istreambuf_iterator<char>(inFile)), (std::istreambuf_iterator<char>()));
    
    inFile.close();

    for(size_t i = 0; i < telegramSize; i += telegramSize)
    {
        telegrams.push_back(content.substr(i, telegramSize));
    }       

    return telegrams;
}


int main() 
{
    int32_t sizeOfTelegram;

    std::cout << "Enter size of telegram: ";
    std::cin >> sizeOfTelegram;

    std::vector<std::string> telegrams = splitFileToTelegrams(sizeOfTelegram);

    int32_t length = telegrams.size();
    
    for(size_t i = 0; i < length; ++i)
    {
        std::cout << "Telegram " << i + 1 << ":\n" << telegrams[i] << '\n';
    }
    return 0;
}
