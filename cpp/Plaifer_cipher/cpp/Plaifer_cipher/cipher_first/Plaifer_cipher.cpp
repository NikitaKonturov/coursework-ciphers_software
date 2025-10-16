#include "Plaifer_cipher.hpp"
#include <random>

/*================================================================*/
/*======================== Шифр Плейфера =========================*/
/*================================================================*/


std::wstring get_trivial_completion(){
    std::wstring completion;

    for (size_t i = 65; i < 91; ++i) {
        if(i != 74) {
            completion.push_back(static_cast<wchar_t>(i));
        }
    }

    return completion;
}

std::string define_language(std::wstring text)
{
    bool has_ru = false, has_en = false;

    for (wchar_t ch : text) {
        if (static_cast<uint16_t>(ch) >= 1040 && static_cast<uint16_t>(ch) <= 1071) {
            has_ru = true;
        } else if (static_cast<uint16_t>(ch) >= 65 && static_cast<uint16_t>(ch) <= 90) {
            has_en = true;
        }
        if (has_ru && has_en) {
            throw InvalidOpenText("Ошибка: текст содержит несколько языков.");
        }
    }

    if (has_en) return "en";
    if (has_ru) throw InvalidOpenText("Ошибка: недопустимый язык в тексте. Язык должен быть английский...");
    throw InvalidOpenText("Ошибка: недопустимый язык в тексте.");
}

std::pair<wchar_t, wchar_t> get_cipher_bigram(wchar_t firstCh, wchar_t secondCh, std::wstring key) {
    // Замена J на I
    if(static_cast<uint16_t>(firstCh) == 74) {
        firstCh = static_cast<wchar_t>(73);
    }
    if(static_cast<uint16_t>(secondCh) == 74) {
        secondCh = static_cast<wchar_t>(73);
    }

    // Поиск позиций с проверкой
    size_t firstPosition = std::wstring::npos;
    size_t secondPosition = std::wstring::npos;
    
    for (size_t i = 0; i < key.size(); ++i) {
        if(key[i] == firstCh && firstPosition == std::wstring::npos) {
            firstPosition = i;
        }
        if(key[i] == secondCh && secondPosition == std::wstring::npos) {
            secondPosition = i;
        }
        if(firstPosition != std::wstring::npos && secondPosition != std::wstring::npos) {
            break;
        }
    }
    
    // Проверка что символы найдены
    if(firstPosition == std::wstring::npos || secondPosition == std::wstring::npos) {
        throw std::runtime_error("Символы не найдены в ключе");
    }
    
    size_t lineOfFirstCh = firstPosition / 5;
    size_t lineOfSecondCh = secondPosition / 5;

    size_t columnOfFirstCh = firstPosition % 5;
    size_t columnOfSecondCh = secondPosition % 5;

    // Одинаковая строка
    if(lineOfFirstCh == lineOfSecondCh) {
        columnOfFirstCh = (columnOfFirstCh + 1) % 5;
        columnOfSecondCh = (columnOfSecondCh + 1) % 5;
        
        return std::pair<wchar_t, wchar_t>(
            key[lineOfFirstCh * 5 + columnOfFirstCh], 
            key[lineOfSecondCh * 5 + columnOfSecondCh]
        );
    } 
    // Одинаковый столбец
    else if(columnOfFirstCh == columnOfSecondCh) {
        lineOfFirstCh = (lineOfFirstCh + 1) % 5;
        lineOfSecondCh = (lineOfSecondCh + 1) % 5;
        
        return std::pair<wchar_t, wchar_t>(
            key[lineOfFirstCh * 5 + columnOfFirstCh], 
            key[lineOfSecondCh * 5 + columnOfSecondCh]
        );
    } 
    // Разные строка и столбец
    else {
        // Меняем столбцы местами
        return std::pair<wchar_t, wchar_t>(
            key[lineOfFirstCh * 5 + columnOfSecondCh], 
            key[lineOfSecondCh * 5 + columnOfFirstCh]
        );        
    }   
}

std::pair<wchar_t, wchar_t> get_revers_cipher_bigram(wchar_t firstCh, wchar_t secondCh, std::wstring key) {
    // Замена J на I
    if(static_cast<uint16_t>(firstCh) == 74) {
        firstCh = static_cast<wchar_t>(73);
    }
    if(static_cast<uint16_t>(secondCh) == 74) {
        secondCh = static_cast<wchar_t>(73);
    }

    // Поиск позиций с проверкой
    size_t firstPosition = std::wstring::npos;
    size_t secondPosition = std::wstring::npos;
    
    for (size_t i = 0; i < key.size(); ++i) {
        if(key[i] == firstCh && firstPosition == std::wstring::npos) {
            firstPosition = i;
        }
        if(key[i] == secondCh && secondPosition == std::wstring::npos) {
            secondPosition = i;
        }
        if(firstPosition != std::wstring::npos && secondPosition != std::wstring::npos) {
            break;
        }
    }
    
    // Проверка что символы найдены
    if(firstPosition == std::wstring::npos || secondPosition == std::wstring::npos) {
        throw std::runtime_error("Символы не найдены в ключе");
    }
    
    size_t lineOfFirstCh = firstPosition / 5;
    size_t lineOfSecondCh = secondPosition / 5;

    size_t columnOfFirstCh = firstPosition % 5;
    size_t columnOfSecondCh = secondPosition % 5;

    // Одинаковая строка
    if(lineOfFirstCh == lineOfSecondCh) {
        // Двигаемся влево с циклическим переходом
        columnOfFirstCh = (columnOfFirstCh + 4) % 5; // +4 вместо -1 для избежания отрицательных чисел
        columnOfSecondCh = (columnOfSecondCh + 4) % 5;
        
        return std::pair<wchar_t, wchar_t>(
            key[lineOfFirstCh * 5 + columnOfFirstCh], 
            key[lineOfSecondCh * 5 + columnOfSecondCh]
        );
    } 
    // Одинаковый столбец
    else if(columnOfFirstCh == columnOfSecondCh) {
        // Двигаемся вверх с циклическим переходом
        lineOfFirstCh = (lineOfFirstCh + 4) % 5; // +4 вместо -1
        lineOfSecondCh = (lineOfSecondCh + 4) % 5;
        
        return std::pair<wchar_t, wchar_t>(
            key[lineOfFirstCh * 5 + columnOfFirstCh], 
            key[lineOfSecondCh * 5 + columnOfSecondCh]
        );
    } 
    // Разные строка и столбец
    else {
        // Меняем столбцы местами (так же как при шифровании)
        return std::pair<wchar_t, wchar_t>(
            key[lineOfFirstCh * 5 + columnOfSecondCh], 
            key[lineOfSecondCh * 5 + columnOfFirstCh]
        );        
    }   
}

std::wstring key_conversions(std::wstring key) {
    std::wstringstream wss;

    for (size_t i = 0; i < key.size(); ++i) {
        if(i % 5 == 0) {
            wss << L"[";
        }
        wss << key[i];
        if((i + 1) % 5 == 0) {
            wss << L"]\n";
        }
    }
    
    return wss.str();
}

bool checkUniqueNumbers(const std::wstring& key) {
    std::wregex number_pattern(L"\\d+");
    std::wsregex_iterator it(key.begin(), key.end(), number_pattern);
    std::wsregex_iterator end;
    
    std::unordered_set<std::wstring> unique_numbers;
    
    for (; it != end; ++it) {
        std::wstring number = it->str();
        if (unique_numbers.find(number) != unique_numbers.end()) {
            return false;
        }
        unique_numbers.insert(number);
    }
    return true;
}

bool checkUniqueLetters(const std::wstring& key) {
    std::wregex letter_pattern(L"[A-Z]");
    std::wsregex_iterator it(key.begin(), key.end(), letter_pattern);
    std::wsregex_iterator end;
    
    std::unordered_set<wchar_t> unique_letters;
    
    for (; it != end; ++it) {
        wchar_t letter = it->str()[0];
        if (unique_letters.find(letter) != unique_letters.end()) {
            return false;
        }
        unique_letters.insert(letter);
    }
    return unique_letters.size() == 25; // Должно быть ровно 25 уникальных букв
}


std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys)
{
    if(keys.size() < openTexts.size()) {
        throw InvalidKey("Количество ключей должно быть равно количеству открытого текста...");
    }
    std::map<std::wstring, std::wstring> keysAndCiphersTexts;
    
    std::wregex completionReg(LR"(^[\s\n]*(?:\[[A-Z]{5}\]\n*)+$)");
    std::wregex keyPermutReg(LR"(^\[\d+(?: \d+)*\]$)");
    for (size_t i = 0; i < openTexts.size(); ++i) {
        std::wstring text = openTexts[i];
        if(text.size() % 2 != 0) {
            text.push_back(static_cast<wchar_t>(65));
        }
        if(define_language(text) != "en") {
            throw InvalidOpenText("Неверный язык, должен быть английский...");
        }

        bool isPermut = std::regex_match(keys[i], keyPermutReg) && checkUniqueNumbers(keys[i]);
        bool isCompletion = std::regex_match(keys[i], completionReg) && checkUniqueLetters(keys[i]);
        
        std::wstring completion = get_trivial_completion();
        if(isPermut) {
            Permutation key_permutation(keys[i]);
            key_permutation.apply(completion);
        } else if(isCompletion) {
            completion = std::regex_replace(keys[i], std::wregex(L"\\W+"), L"");
        } else {
            throw std::runtime_error("Неверный формат ключей...");
        }

        for (size_t j = 0; j < text.size(); j += 2) {
            std::pair<wchar_t, wchar_t> newBigram = get_cipher_bigram(text[j], text[j+1], completion);
            text[j] = newBigram.first;
            text[j+1] = newBigram.second;
        }
        
        keysAndCiphersTexts[key_conversions(completion)] = text;
    }
    
    return keysAndCiphersTexts;
}

std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndCipherTexts)
{
    std::map<std::wstring, std::wstring> keysAndOpenTexts;
    
    for(auto& pair: keysAndCipherTexts) {
        if(define_language(pair.second) != "en") {
            throw InvalidOpenText("Неверный язык, должен быть английский...");
        }
        std::wregex cleaner(L"\\W+");
        
        std::wstring completion = std::regex_replace(pair.first, cleaner, L"");
        std::wcout << "Filling in the square: " << completion << std::endl;
        for (size_t i = 0; i < pair.second.size(); i += 2) {
            std::pair<wchar_t, wchar_t> newBigram = get_revers_cipher_bigram(pair.second[i], pair.second[i+1], completion);
            std::wcout << L"New bigram: " << newBigram.first << newBigram.second << std::endl;
            pair.second[i] = newBigram.first;
            pair.second[i+1] = newBigram.second;
        }
    }

    return keysAndCipherTexts;
}


std::vector<std::string> gen_keys(std::string keyPropertys, size_t count)
{
    nlohmann::json prop;
    try{
        std::replace(keyPropertys.begin(), keyPropertys.end(), '\'', '\"');
        std::cout << keyPropertys << std::endl;

        prop = nlohmann::json::parse(keyPropertys);
        chekRequest(prop);
      

        std::vector<int32_t> trivial_permut(25);
        for (size_t i = 0; i < 25; ++i) {
            trivial_permut[i] = i + 1;
        }
        
        std::vector<uint8_t> entropy = get_entropy();
        std::vector<uint8_t> nonce = get_entropy();

        HMAC_DRBG gen(entropy, nonce, {'P', 'l', 'a', 'i', 'f', 'e', 'r', '-', 'c', 'i', 'p', 'h', 'e', 'r'});


        std::vector<std::vector<int32_t>> all_permut(count);
        for (size_t i = 0; i < count; ++i) {
            all_permut[i] = generat_permutation(trivial_permut, gen);
        }

        std::vector<std::string> result;
        for (auto permut: all_permut) {
            std::ostringstream oss;
            std::copy(permut.begin(), permut.end(), std::ostream_iterator<int32_t>(oss, " "));
            result.push_back(oss.str().substr(0, oss.str().size() - 1));
        }
        
        return result;
    } catch(nlohmann::json::parse_error &err) {
        throw KeyPropertyError(err.what());
    }
}

std::string get_key_propertys()
{
// сам шаблон как должен выглядеть .json запрос с параметрами
    nlohmann::json keyProp = nlohmann::json::parse(R"({"params": []})");
   
    return keyProp.dump();
}

void chekRequest(nlohmann::json keyPropertys)
{
    try {
        if(!keyPropertys.at("text_language").is_string()) {
            throw KeyPropertyError("Значение \"Язык текста\" должен иметь строковое значение...");
        }
        if(keyPropertys["text_language"] != "ru" && keyPropertys["text_language"] != "en") {
            throw InvalidKey("Значение \"Язык текста\" должно быть ru или en...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}