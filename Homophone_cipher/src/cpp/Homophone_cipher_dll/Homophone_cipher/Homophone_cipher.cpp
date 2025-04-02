#include "homophone_cipher.hpp"
#include <iomanip>
#include <codecvt>
#include <locale>


// Шифрует открытый текст с использованием ключей.
// Каждая буква текста заменяется на случайный ключ из списка, связанного с этой буквой.
// Индекс для выбора ключа обновляется после каждого использования.
// Возвращает map, где ключ — обновленный ключ шифрования, а значение — зашифрованный текст.
std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys) {
    std::map<std::wstring, std::wstring> result;

    for (size_t i = 0; i < openTexts.size(); ++i) {
        std::wstring text = openTexts[i];
        std::wstring key = keys[i];
        std::wistringstream keyStream(key);
        std::map<wchar_t, std::vector<std::wstring>> substitutionMap;
        std::map<wchar_t, size_t> letterIndices;

        // Чтение ключа (без нулей в конце)
        std::wstring line;
        while (std::getline(keyStream, line)) {
            std::wistringstream lineStream(line);
            std::wstring letter;
            if (!(lineStream >> letter)) continue;
            
            wchar_t charKey = letter[0];
            std::wstring number;
            while (lineStream >> number) {  // Читаем до конца строки
                substitutionMap[charKey].push_back(number);
            }
            letterIndices[charKey] = 0;  // Инициализация индекса
        }

        // Шифрование текста
        std::wstring cipherText;
        for (wchar_t ch : text) {
            if (substitutionMap.count(ch)) {
                size_t& index = letterIndices[ch];
                cipherText += substitutionMap[ch][index] + L" ";
                index = (index + 1) % substitutionMap[ch].size();
            } else {
                cipherText += ch;
            }
        }

        // Формируем ключ
        std::wstringstream updatedKey;
        for (const auto& [charKey, numbers] : substitutionMap) {
            updatedKey << charKey << L" ";
            for (const auto& num : numbers) {
                updatedKey << num << L" ";
            }
            updatedKey << L"\n";  // Просто перевод строки
        }

        result[updatedKey.str()] = cipherText;
    }

    return result;
}



// Расшифровывает текст с использованием ключей.
// Каждое число в зашифрованном тексте заменяется на соответствующую букву.
// Возвращает map, где ключ — ключ шифрования, а значение — расшифрованный текст.
std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndText) {
    std::map<std::wstring, std::wstring> decryptedTexts;

    for (const auto& [key, cipherText] : keysAndText) {
        std::wistringstream keyStream(key);
        std::map<std::wstring, wchar_t> reverseMap;  // Число → буква
        std::wstring line;

        // Строим обратный map (число → буква)
        while (std::getline(keyStream, line)) {
            std::wistringstream lineStream(line);
            std::wstring letter;
            if (!(lineStream >> letter)) continue;

            wchar_t charKey = letter[0];
            std::wstring number;
            while (lineStream >> number) {
                reverseMap[number] = charKey;  // Например, "45" → 'А'
            }
        }

        // Расшифровываем текст
        std::wistringstream cipherStream(cipherText);
        std::wstring token;
        std::wstring originalText;

        while (cipherStream >> token) {
            if (reverseMap.count(token)) {
                originalText += reverseMap[token];  // Заменяем число на букву
            } else {
                originalText += token;  // Оставляем как есть (если число не найдено)
            }
        }

        // Возвращаем результат с исходным ключом (без изменений)
        decryptedTexts[key] = originalText;
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

            std::vector<uint8_t> entropy = get_entropy();
            std::vector<uint8_t> nonce = get_entropy();

            HMAC_DRBG gen(entropy, nonce, {'H', 'o', 'm', 'o', 'p', 'h', 'o', 'n', 'e', '-', 'c', 'i', 'p', 'h', 'e', 'r'});
            try {
                keys = generate_keys(leftBoarder, rightBoarder, text_language, gen);
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
            { "name": "leftBoarder", "min": 0, "max": null, "value": 0, "type": "number", "default": 0, "label" : "Левая граница" },
            { "name": "rightBoarder", "min": 1, "max": null, "value": 99, "type": "number", "default": 99, "label" : "Правая граница" }
        ]
    })");
   return keyProp.dump();
}

void chekRequest(nlohmann::json keyPropertys) {
    try {
        if (!keyPropertys.contains("leftBoarder") || !keyPropertys.contains("rightBoarder")) {
            throw KeyPropertyError("Отсутствуют необходимые параметры.");
        }
        if (!keyPropertys["leftBoarder"].is_number() || !keyPropertys["rightBoarder"].is_number()) {
            throw KeyPropertyError("Границы должны быть числами.");
        }
        if (keyPropertys["leftBoarder"] >= keyPropertys["rightBoarder"]) {
            throw InvalidKey("Левая граница должна быть меньше правой границы.");
        }
        if(!keyPropertys.at("text_language").is_string()) {
            throw KeyPropertyError("Значени text_language должно иметь строковое значение...");
        }
        if(keyPropertys["text_language"] != "ru" && keyPropertys["text_language"] != "en") {
            throw InvalidKey("Значение language должно быть или ru или en...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
