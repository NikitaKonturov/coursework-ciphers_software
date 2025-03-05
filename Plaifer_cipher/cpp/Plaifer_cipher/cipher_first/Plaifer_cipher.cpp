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
    if(static_cast<uint16_t>(firstCh) == 74) {
        firstCh = static_cast<wchar_t>(73);
    }
    if(static_cast<uint16_t>(secondCh) == 74) {
        secondCh = static_cast<wchar_t>(73);
    }

    size_t firstPosition;
    for (size_t i = 0; i < key.size(); ++i) {
        if(key[i] == firstCh) {
            firstPosition = i;
            break;
        }
    }
    
    size_t secondPosition;
    for (size_t i = 0; i < key.size(); ++i) {
        if(key[i] == secondCh) {
            secondPosition = i;
            break;
        }
    }
    
    size_t lineOfFirstCh = firstPosition / 5;
    size_t lineOfSecondCh = secondPosition / 5;

    size_t columnOfFirstCh = firstPosition % 5;
    size_t columnOfSecondCh = secondPosition % 5;

    if(lineOfFirstCh == lineOfSecondCh) {
        ++firstPosition;
        ++secondPosition;
        
        if(firstPosition / 5 != lineOfFirstCh) {
            firstPosition -= 5;
        }
        if(secondPosition / 5 != lineOfSecondCh) {
            secondPosition -= 5;
        }

        return std::pair<wchar_t, wchar_t>(key[firstPosition], key[secondPosition]);
    } else if(columnOfFirstCh == columnOfSecondCh) {
        firstPosition += 5;
        secondPosition += 5;

        if(firstPosition >= 25) {
            firstPosition = columnOfFirstCh;
        }
        if(secondPosition >= 25) {
            secondPosition = columnOfSecondCh;
        }

        return std::pair<wchar_t, wchar_t>(key[firstPosition], key[secondPosition]);
    } else {
        if(columnOfFirstCh < columnOfSecondCh) {
            firstPosition += columnOfSecondCh - columnOfFirstCh;
            secondPosition -= columnOfSecondCh - columnOfFirstCh;
        } else {
            secondPosition += columnOfFirstCh - columnOfSecondCh;
            firstPosition -= columnOfFirstCh - columnOfSecondCh;    
        }

        return std::pair<wchar_t, wchar_t>(key[firstPosition], key[secondPosition]);        
    }   
}

std::pair<wchar_t, wchar_t> get_revers_cipher_bigram(wchar_t firstCh, wchar_t secondCh, std::wstring key) {
    if(static_cast<uint16_t>(firstCh) == 74) {
        firstCh = static_cast<wchar_t>(73);
    }
    if(static_cast<uint16_t>(secondCh) == 74) {
        secondCh = static_cast<wchar_t>(73);
    }

    size_t firstPosition;
    for (size_t i = 0; i < key.size(); ++i) {
        if(key[i] == firstCh) {
            firstPosition = i;
            break;
        }
    }
    
    size_t secondPosition;
    for (size_t i = 0; i < key.size(); ++i) {
        if(key[i] == secondCh) {
            secondPosition = i;
            break;
        }
    }

    size_t lineOfFirstCh = firstPosition / 5;
    size_t lineOfSecondCh = secondPosition / 5;

    size_t columnOfFirstCh = firstPosition % 5;
    size_t columnOfSecondCh = secondPosition % 5;


    if(lineOfFirstCh == lineOfSecondCh) {
        --firstPosition;
        --secondPosition;
        
        if(firstPosition / 5 != lineOfFirstCh) {
            firstPosition += 5;
        }
        if(secondPosition / 5 != lineOfSecondCh) {
            secondPosition += 5;
        }

        return std::pair<wchar_t, wchar_t>(key[firstPosition], key[secondPosition]);
    } else if(columnOfFirstCh == columnOfSecondCh) {
        firstPosition -= 5;
        secondPosition -= 5;

        if(firstPosition >= 25) {
            firstPosition = 20 + columnOfFirstCh;
        }
        if(secondPosition >= 25) {
            secondPosition = 20 + columnOfSecondCh;
        }

        return std::pair<wchar_t, wchar_t>(key[firstPosition], key[secondPosition]);
    } else {
        if(columnOfFirstCh < columnOfSecondCh) {
            firstPosition += columnOfSecondCh - columnOfFirstCh;
            secondPosition -= columnOfSecondCh - columnOfFirstCh;
        } else {
            secondPosition += columnOfFirstCh - columnOfSecondCh;
            firstPosition -= columnOfFirstCh - columnOfSecondCh;    
        }

        return std::pair<wchar_t, wchar_t>(key[firstPosition], key[secondPosition]);        
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


std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys)
{
    if(keys.size() < openTexts.size()) {
        throw InvalidKey("Количество ключей должно быть равно количеству открытого текста...");
    }
    std::map<std::wstring, std::wstring> keysAndCiphersTexts;
    
    for (size_t i = 0; i < openTexts.size(); ++i) {
        std::wstring text = openTexts[i];
        if(text.size() % 2 != 0) {
            text.push_back(static_cast<wchar_t>(65));
        }
        if(define_language(text) != "en") {
            throw InvalidOpenText("Неверный язык, должен быть английский...");
        }
        Permutation key_permutation(keys[i]);
        std::wstring completion = get_trivial_completion();
        key_permutation.apply(completion);

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