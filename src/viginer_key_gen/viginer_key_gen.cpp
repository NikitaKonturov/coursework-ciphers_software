#include "viginer_key_gen.hpp"

bool is_rus_alpha(uint8_t symbol) {
    return (symbol >= 192 && symbol <= 255) || symbol == 184 || symbol == 168;
} //Проверка на русский символ

bool is_eng_alpha(uint8_t symbol) {
    return (symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z');
} //Проверка на английский символ

std::map<size_t, size_t> count_words(const std::string& keyFilePath, const std::string& lang)
{
    bool (*is_alpha) (uint8_t); //Выбор языка
    if (lang == "ru") {
        setlocale(LC_ALL, ".1251");
        is_alpha = is_rus_alpha;
    }
    else if (lang == "en") {
        is_alpha = is_eng_alpha;
    }
    
    
    std::ifstream keyFile(keyFilePath, std::ios::binary); //Открытие файла с ключами
    keyFile.seekg(0, keyFile.end); //Переход в конец файла
    size_t fileSize = keyFile.tellg(); //Подсчёт длины файла
    keyFile.close(); //Закрытие файла
    bool isWordMode = false; //Флаг индикации режима "Слово"
    size_t wordLength = 0; //Длина читаемого слова
    std::map<size_t, size_t> wordsCount; //Результирующий map
    uint8_t buff; //Буфер для чтения символов
    for (size_t i = 0; i < fileSize; i++)
    {
        std::ifstream keyFile(keyFilePath, std::ios::binary); //Открытие файла
        
        keyFile.seekg(i); //Перевод файла на i-тую позицию

        keyFile.read((char*)&buff, 1); //Чтение i-того символа

        if (is_alpha(buff)) //Попалась буква
        {
            ++wordLength; //Увеличение длины слова
            if (!isWordMode) 
            {
                isWordMode = true; //Включение режима "Слово", если не был включён
            }
        } else if (isWordMode) //Попался знак
        {
            ++wordsCount[wordLength]; //Прибавляем единицу к количеству слов данной длины
            isWordMode = false; //Выключаем режим слово
            wordLength = 0; //Восстанавливаем значение длины читаемого слова
        }
        keyFile.close(); //Закрываем файл
    }
    return wordsCount;
}

void set_final_key_word_num(size_t& keyWordNum, bool*& bunnedWords)
{
    for (size_t i = 0; i < keyWordNum; ++i) {
        if (bunnedWords[i] == true) {
            ++keyWordNum; //Добавляем по еденице за каждое использованное слово идущее перед данным
        }
    }
    bunnedWords[keyWordNum - 1] = true; //Добавляем слово в список использованных
}

std::string give_word(
    const std::string& keyFilePath, 
    const std::string& lang, 
    std::map<size_t, size_t>& wordsCount,
    const size_t& keyWordLength,
    bool*& bannedWords,
    const size_t& allWordsCount
)
{   
    if (wordsCount.at(keyWordLength) == 0) {
        return "End of words"; //Не осталось слов данной длины
    }

    bool (*is_alpha) (uint8_t); //Выбор языка
    if (lang == "ru") {
        setlocale(LC_ALL, ".1251");
        is_alpha = is_rus_alpha;
    }
    else if (lang == "en") {
        is_alpha = is_eng_alpha;
    }

    srand(time(NULL));
    size_t keyWordNum = (rand() % wordsCount.at(keyWordLength)) + 1; //Генерация изначальной позиции слова
    set_final_key_word_num(keyWordNum, bannedWords); //Генерация итоговой позиции слова
    wordsCount.at(keyWordLength) -= 1; //Уменьшение числа доступных слов

    //Чтение файла по одному символу аналогичное предыдущему
    std::ifstream keyFile(keyFilePath, std::ios::binary);
    keyFile.seekg(0, keyFile.end);
    size_t fileSize = keyFile.tellg();
    keyFile.close();
    bool isWordMode = false;
    uint8_t buff;
    size_t wordLength = 0;
    for (size_t i = 0; i < fileSize; i++)
    {
        std::ifstream keyFile(keyFilePath, std::ios::binary);
        
        keyFile.seekg(i);

        keyFile.read((char*)&buff, 1);

        if (is_alpha(buff)) 
        {
            ++wordLength;
            if (!isWordMode) 
            {
                isWordMode = true;
            }
        } else if (isWordMode) //Закончилось слово
        {
            if (wordLength == keyWordLength) { //Слово подходит
                --keyWordNum; //Уменьшаем счётчик слов до нужного нам
                if (keyWordNum == 0) { //Если слово нам подходит
                    keyFile.close(); //Закраем файл для очистки памяти
                    std::ifstream keyFile(keyFilePath, std::ios::binary); //Заново его открываем
                    keyFile.seekg(i - keyWordLength); //Переходим к началу слова
                    std::string keyBuff(keyWordLength, ' '); //Создаём контейнер для слова
                    for (size_t i = 0; i < keyWordLength; ++i) //Читаем слово из файла
                    {
                        keyFile.read((char*)&buff, 1);
                        keyBuff[i] = (char) buff;
                    }
                    return keyBuff; //Возврацаем слово
                }
            }
            wordLength = 0; //Если слово не подходит зануляем длину текущего слова
            isWordMode = false; //Выключаем режим "Слово"
        }
        keyFile.close(); //Закрываем файл
    }
    return " "; //Возвращаемое значение на случай необычных ветвлений
}
