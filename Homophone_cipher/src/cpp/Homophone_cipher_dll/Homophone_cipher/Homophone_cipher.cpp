#include "homophone_cipher.hpp"
#include <iomanip>
#include <codecvt>
#include <locale>


// Шифрует открытый текст с использованием ключей.
// Каждая буква текста заменяется на случайный ключ из списка, связанного с этой буквой.
// Индекс для выбора ключа обновляется после каждого использования.
// Возвращает map, где ключ — обновленный ключ шифрования, а значение — зашифрованный текст.
std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys) {
    if (keys.empty()) {
        throw InvalidKey("Keys not found...");
    }

    std::map<std::wstring, std::wstring> keysAndCipherTexts;
    
    for (size_t i = 0; i < openTexts.size(); ++i) {
        std::wstring text = openTexts[i];
        std::wstring key = keys[i];
        std::wistringstream keyStream(key);
        std::map<wchar_t, std::vector<std::wstring>> substitutionMap;
        std::map<wchar_t, size_t> letterIndices;
        std::wstring letter, number;
        
        while (keyStream >> letter) {
            std::vector<std::wstring> numbers;
            while (keyStream >> number) {
                numbers.push_back(number);
            }
            wchar_t charKey = letter[0];
            letterIndices[charKey] = std::stoi(numbers.back()); // Последниц элемент - индекс
            numbers.pop_back();                                 // Убираем индекс из списка замен
            substitutionMap[charKey] = numbers;
        }

        std::wstring cipherText;
        for (wchar_t ch : text) {
            if (substitutionMap.count(ch) && !substitutionMap[ch].empty()) {
                size_t& index = letterIndices[ch];
                cipherText += substitutionMap[ch][index] + L" ";
                
                // Обновляет индекс для выбора следующего ключа.
                // Если индекс достигает конца списка, сбрасывается в 0.
                index = (index + 1) % substitutionMap[ch].size();
            } else {
                cipherText += ch;
            }
        }

        // Собираем обновленный ключ с новым индексом
        std::wstringstream updatedKey;
        for (const auto& [charKey, numbers] : substitutionMap) {
            updatedKey << charKey << L" ";
            for (const auto& num : numbers) {
                updatedKey << num << L" ";
            }
            updatedKey << letterIndices[charKey] << L"\n"; // Добавляем новый индекс
        }

        keysAndCipherTexts[updatedKey.str()] = cipherText;
    }

    return keysAndCipherTexts;
}

// Расшифровывает текст с использованием ключей.
// Каждое число в зашифрованном тексте заменяется на соответствующую букву.
// Возвращает map, где ключ — ключ шифрования, а значение — расшифрованный текст.
std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndText) {
    std::map<std::wstring, std::wstring> decryptedTexts;
    
    for (auto& [key, cipherText] : keysAndText) {
        std::wistringstream keyStream(key);
        std::map<std::wstring, wchar_t> reverseMap;
        std::wstring letter, number;
        
        while (keyStream >> letter) {
            std::vector<std::wstring> numbers;
            while (keyStream >> number) {
                numbers.push_back(number);
            }
            numbers.pop_back(); // Убираем последний элемент (индекс)
            for (const auto& num : numbers) {
                reverseMap[num] = letter[0];
            }
        }

        std::wistringstream cipherStream(cipherText);
        std::wstring decryptedText;
        while (cipherStream >> number) {
            if (reverseMap.count(number)) {
                decryptedText += reverseMap[number];
            } else {
                decryptedText += number;
            }
        }
        decryptedTexts[key] = decryptedText;
    }
    
    return decryptedTexts;
}


std::vector<std::wstring> gen_keys(std::wstring keyPropertys, size_t count) {
    nlohmann::json prop;
    try {
        std::replace(keyPropertys.begin(), keyPropertys.end(), L'\'', L'"');
        prop = nlohmann::json::parse(keyPropertys);
        chekRequest(prop);


        int64_t leftBoarder = prop["leftBoarder"].get<int64_t>();
        int64_t rightBoarder = prop["rightBoarder"].get<int64_t>();
        std::string text_language = prop["text_language"].get<std::string>(); // извоекаем используемый язык


        std::vector<std::wstring> result;
        for (size_t i = 0; i < count; ++i) {
            std::map<wchar_t, std::vector<std::wstring>> keys;
            
            try {
                keys = generate_keys(leftBoarder, rightBoarder, text_language);
            } catch (const std::exception& e) {
                throw KeyPropertyError(e.what());
            }

            std::wstringstream wss;
            for (const auto& [letter, keyList] : keys) {
                wss << letter << L" ";
                for (const auto& key : keyList) {
                    wss << key << L" ";
                }
                wss << L"\n";
            }

            result.push_back(wss.str());
        }
        return result;

    } catch (const std::exception& e) {
        throw KeyPropertyError(e.what());
    }
}


// Возвращает свойства ключей в формате JSON.
// Свойства включают диапазон чисел (leftBoarder и rightBoarder) и язык текста.
// Используется для настройки генерации ключей.
std::string get_key_propertys() {
    nlohmann::json keyProp = nlohmann::json::parse(R"({
        "params": [
            { "name": "leftBoarder", "min": 0, "max": null, "value": 0, "type": "number", "default": 0 },
            { "name": "rightBoarder", "min": 1, "max": null, "value": 99, "type": "number", "default": 99 }
        ]
    })");
   return keyProp.dump();
}

void chekRequest(nlohmann::json keyPropertys) {
    try {
        if (!keyPropertys.contains("leftBoarder") || !keyPropertys.contains("rightBoarder")) {
            throw KeyPropertyError("Missing required parameters.");
        }
        if (!keyPropertys["leftBoarder"].is_number() || !keyPropertys["rightBoarder"].is_number()) {
            throw KeyPropertyError("Borders must be numbers.");
        }
        if (keyPropertys["leftBoarder"] >= keyPropertys["rightBoarder"]) {
            throw InvalidKey("Left border must be less than right border.");
        }
        if(!keyPropertys.at("text_language").is_string()) {
            throw KeyPropertyError("Key text_language must has string value...");
        }
        if(keyPropertys["text_language"] != "ru" && keyPropertys["text_language"] != "en") {
            throw InvalidKey("Value language must be ru or en...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
