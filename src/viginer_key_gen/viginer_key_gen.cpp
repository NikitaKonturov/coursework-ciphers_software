#include "viginer_key_gen.hpp"

void sort_dict_words(const std::string& dictPath, const std::string& lang)
{
    std::locale::global(std::locale("ru_RU.UTF-8"));
    std::wcout.imbue(std::locale());
    std::wifstream dictIn(dictPath);
    std::wstring buff;
    std::string res_dict_part_path;
    if (lang == "ru") {
        dictIn.imbue(std::locale("ru_RU.UTF-8"));
        setlocale(LC_ALL, "ru_RU.UTF-8");
    }
    else if (lang == "en") {
        dictIn.imbue(std::locale("en-US.UTF-8"));
        setlocale(LC_ALL, "en_US.UTF-8");
    }
    while (std::getline(dictIn, buff)) {
        if (buff.find_first_of(std::wstring(L"-.ёЁ`")) != std::wstring::npos) {
            continue;
        }
        bool found = false;
        for (wchar_t ch : buff) {
            if (static_cast<int>(ch) == 0x451 || static_cast<int>(ch) == 0x401) { // 0x451 = 'ё', 0x401 = 'Ё'
                found = true;
                break;
            }
        }
        if (found) {
            continue;
        }
        
        res_dict_part_path = 
        "../../src/dictionaries/" + lang + '/' + 
        lang + "_dict_" + std::to_string(buff.length()) + ".txt";
        std::wofstream res_dict_part(res_dict_part_path, std::ios::app);
        if (lang == "ru") {
            res_dict_part.imbue(std::locale("ru_RU.UTF-8"));
        }
        else if(lang == "en") {
            res_dict_part.imbue(std::locale("en_US.UTF-8"));
        }
        res_dict_part << buff << '\n';
        res_dict_part.close();
    }
}

size_t give_words_count(std::wifstream& keyFile, const size_t& length, const std::string& lang)
{
    keyFile.seekg(0, std::ios::end);
    size_t wordsCount;
    if (lang == "ru") {
        wordsCount = keyFile.tellg() / 2 / (length + 1);
    }
    else if (lang == "en") {
        wordsCount = keyFile.tellg() / (length + 2);
    }
    keyFile.seekg(0, std::ios::beg);
    return wordsCount;
}

std::wstring give_random_key(const std::string& lang, const size_t& length)
{
    std::string keyFilePath = "../../src/dictionaries/" + lang + '/' + lang + "_dict_"
     + std::to_string(length) + ".txt";
    
    std::wifstream keyFile(keyFilePath, std::ios::binary);
    size_t wordsCount = give_words_count(keyFile, length, lang);
    if (lang == "ru") {
        keyFile.imbue(std::locale("ru-RU.UTF-8"));
    }
    else if (lang == "en") {
        keyFile.imbue(std::locale("en_US.UTF-8"));
    }

    std::string bannedWordsPath = "../../src/dictionaries/" + lang + '/' + lang + "_ban_words.txt";
    std::wifstream bannedWordsIn(bannedWordsPath);
    if (!bannedWordsIn.is_open()) {
        bannedWordsIn.close();
        std::wofstream bannedWordsOut(bannedWordsPath);
        bannedWordsOut.close();
        std::wifstream bannedWordsIn(bannedWordsPath);
    }
    if (lang == "ru") {
        bannedWordsIn.imbue(std::locale("ru-RU.UTF-8"));
    }
    else if (lang == "en") {
        bannedWordsIn.imbue(std::locale("en_US.UTF-8"));
    }
    
    wchar_t* buff = new wchar_t[length + 1];
    std::wstring buffStr;
    std::wstring bannedWord;

    while (true) {
        size_t randomPos =  rand() % wordsCount;
        keyFile.seekg(keyFile.beg);
        if (lang == "ru") {
            keyFile.seekg(2 * randomPos * (length + 1));
        }
        else if (lang == "en") {
            keyFile.seekg(randomPos * (length + 2));
        }
        keyFile.read(buff, length);
        buff[length] = L'\0';
        buffStr = buff;
        while (std::getline(bannedWordsIn, bannedWord)) {
            if (buffStr == bannedWord) {
                continue;
            }
        }
        bannedWordsIn.close();
        std::wofstream bannedWordsOut(bannedWordsPath, std::ios::app);
        if (lang == "ru") {
            bannedWordsOut.imbue(std::locale("ru-RU.UTF-8"));
        }
        else if (lang == "en") {
            bannedWordsOut.imbue(std::locale("en_US.UTF-8"));
        }
        bannedWordsOut << buffStr << '\n';
        bannedWordsOut.close();
        delete[] buff;
        for (size_t i = 0; i < buffStr.length(); ++i) {
                buffStr[i] = towupper(buffStr[i]);
        }
        
        return buffStr;
    }
    
}

void sort_key_file(const std::string& keyFilePath, const std::string& lang)
{
    std::wifstream keyFile(keyFilePath);
    std::wstring buff;
    std::string res_dict_part_path;
    if (lang == "ru") {
        keyFile.imbue(std::locale("ru_RU.UTF-8"));
        setlocale(LC_ALL, "ru_RU.UTF-8");
    }
    else if (lang == "en") {
        keyFile.imbue(std::locale("en-US.UTF-8"));
        setlocale(LC_ALL, "en_US.UTF-8");
    }
    while (std::getline(keyFile, buff)) {
        if (buff.find(L'-') != std::wstring::npos) {
            continue;
        }
        if (buff.find(L'.') != std::wstring::npos) {
            continue;
        }
        res_dict_part_path =  "../../src/dictionaries/custom/custom_dict_"
         + std::to_string(buff.length()) + ".txt";
        std::wofstream res_dict_part(res_dict_part_path, std::ios::app);
        if (lang == "ru") {
            res_dict_part.imbue(std::locale("ru_RU.UTF-8"));
        }
        else if(lang == "en") {
            res_dict_part.imbue(std::locale("en_US.UTF-8"));
        }
        res_dict_part << buff << '\n';
        res_dict_part.close();
    }
}

void clear_custom()
{
    for (size_t i = 2; i < 50; ++i) {
        std::string custom_path =
        "../../src/dictionaries/custom/custom_dict_" + std::to_string(i) + ".txt";
        std::ofstream clear_file(custom_path);
        clear_file.close();
    }
}

std::wstring give_random_custom_key(const std::string& lang, const size_t& length)
{
    srand(time(NULL));

    std::string keyFilePath = "../../src/dictionaries/custom/custom_dict_"
     + std::to_string(length) + ".txt";
    
    std::wifstream keyFile(keyFilePath, std::ios::binary);
    size_t wordsCount = give_words_count(keyFile, length, lang);
    if (lang == "ru") {
        keyFile.imbue(std::locale("ru-RU.UTF-8"));
    }
    else if (lang == "en") {
        keyFile.imbue(std::locale("en_US.UTF-8"));
    }

    std::string bannedWordsPath = "../../src/dictionaries/custom/custom_ban_words.txt";
    std::wifstream bannedWordsIn(bannedWordsPath);
    if (!bannedWordsIn.is_open()) {
        bannedWordsIn.close();
        std::wofstream bannedWordsOut(bannedWordsPath);
        bannedWordsOut.close();
        std::wifstream bannedWordsIn(bannedWordsPath);
    }
    if (lang == "ru") {
        bannedWordsIn.imbue(std::locale("ru-RU.UTF-8"));
    }
    else if (lang == "en") {
        bannedWordsIn.imbue(std::locale("en_US.UTF-8"));
    }
    
    wchar_t* buff = new wchar_t[length + 1];
    std::wstring buffStr;
    std::wstring bannedWord;

    while (true) {
        size_t randomPos = rand() % wordsCount;
        keyFile.seekg(keyFile.beg);
        if (lang == "ru") {
            keyFile.seekg(2 * randomPos * (length + 1));
        }
        else if (lang == "en") {
            keyFile.seekg(randomPos * (length + 2));
        }
        keyFile.read(buff, length);
        buff[length] = L'\0';
        buffStr = buff;
        while (std::getline(bannedWordsIn, bannedWord)) {
            if (buffStr == bannedWord) {
                srand(randomPos);
                continue;
            }
        }
        bannedWordsIn.close();
        std::wofstream bannedWordsOut(bannedWordsPath, std::ios::app);
        if (lang == "ru") {
            bannedWordsOut.imbue(std::locale("ru-RU.UTF-8"));
        }
        else if (lang == "en") {
            bannedWordsOut.imbue(std::locale("en_US.UTF-8"));
        }
        bannedWordsOut << buffStr << '\n';
        bannedWordsOut.close();
        delete[] buff;
        return buffStr;
    }
    
}

void clear_cache()
{
    std::wofstream clear_ru("../../src/dictionaries/ru/ru_ban_words.txt");
    std::wofstream clear_en("../../src/dictionaries/en/en_ban_words.txt");
    std::wofstream clear_custom("../../src/dictionaries/custom/custom_ban_words.txt");
}
