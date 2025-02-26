#include "viginer_key_gen/viginer_key_gen.hpp"

int main()
{
    std::string strFilePath("C:/CourseWork/small_en.txt"); //Путь к файлу с ключами
    std::map<size_t, size_t> wordsCount = count_words(strFilePath, std::string("en")); //Подсчёт количества слов с группировкой по количеству букв
    size_t keyWordLen = 5; //Длинна желаемого слова
    size_t allWordsCount = wordsCount[keyWordLen]; //Изначальное количество возможных ключевых слов
    bool* bannedWords = new bool[allWordsCount]; //Массив под уже использованные слова
    for (size_t i = 0; i < allWordsCount; ++i) {
        bannedWords[i] = false; //Инициализация массива
    }

    for (size_t i = 0; i < 100; ++i)
    {
        std::string word = give_word(strFilePath, std::string("en"), wordsCount, keyWordLen, bannedWords, allWordsCount); //Чтение слова
        std::cout << word << '\n'; //Вывод слова или информации о окончании таковых
        if (word == "End of words") {
            break;
        }
    }

    delete[] bannedWords;

    return 0;
}
