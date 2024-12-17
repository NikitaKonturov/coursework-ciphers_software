#include "functions.hpp"



int main() 
{
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    std::wstring filename = L"D:/MyWorkDesk/2course/courseWork2/new_txt_to_telegrams/src/testText.txt";
    int telegramLength = 10; 
    int telegramCount = 10;   


    try 
    {
        std::vector<std::wstring> telegrams = generateTelegrams(filename, telegramLength, telegramCount);

        
        for (size_t i = 0; i < telegrams.size(); ++i) 
        {
            std::wcout << L"Telegram:\n " << i + 1 << ":\n" << telegrams[i] << L"\n\n";    
        }
    } 
    
    catch (const std::exception& e) 
    {
        std::cerr << "Eror: " << e.what() << std::endl;
    }

    return 0;
}

