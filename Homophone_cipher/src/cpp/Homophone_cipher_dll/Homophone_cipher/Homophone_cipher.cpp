#include "homophone_cipher.hpp"
#include <iomanip>
#include <codecvt>
#include <locale>
#include <regex>
#include <set>

bool is_valid_key_format(const std::wstring& key) {
    // Проверяем, что ключ не пустой
    if (key.empty()) {
        return false;
    }
    
    std::wistringstream keyStream(key);
    std::wstring line;
    std::set<wchar_t> seen_letters;
    std::set<std::wstring> seen_numbers;
    bool has_valid_lines = false;
    
    // Регулярное выражение для проверки строки: буква + пробел + числа через пробел
    std::wregex line_pattern(L"^[A-ZА-ЯЁ]\\s+([0-9]+\\s+)*[0-9]*$");
    std::wregex number_pattern(L"^[0-9]+$");
    
    while (std::getline(keyStream, line)) {
        // Убираем пробелы в начале и конце строки
        size_t start = line.find_first_not_of(L" \t");
        if (start == std::wstring::npos) continue; // Пропускаем пустые строки
        size_t end = line.find_last_not_of(L" \t");
        line = line.substr(start, end - start + 1);
        
        // Проверяем общий формат строки
        if (!std::regex_match(line, line_pattern)) {
            return false;
        }
        
        std::wistringstream lineStream(line);
        std::wstring letter;
        if (!(lineStream >> letter)) {
            return false;
        }
        
        // Проверяем, что первый токен - одна буква
        if (letter.length() != 1) {
            return false;
        }
        
        wchar_t charKey = letter[0];
        
        // Проверяем, что буква не повторяется
        if (seen_letters.count(charKey)) {
            return false;
        }
        seen_letters.insert(charKey);
        
        // Проверяем числа
        std::wstring number;
        bool has_numbers = false;
        while (lineStream >> number) {
            // Проверяем формат числа
            if (!std::regex_match(number, number_pattern)) {
                return false;
            }
            
            // Проверяем, что числа не повторяются
            if (seen_numbers.count(number)) {
                return false;
            }
            seen_numbers.insert(number);
            has_numbers = true;
        }
        
        // Каждая буква должна иметь хотя бы одно число
        if (!has_numbers) {
            return false;
        }
        
        has_valid_lines = true;
    }
    
    // Ключ должен содержать хотя бы одну корректную строку
    return has_valid_lines;
}

// Шифрует открытый текст с использованием ключей.
// Каждая буква текста заменяется на случайный ключ из списка, связанного с этой буквой.
// Индекс для выбора ключа обновляется после каждого использования.
// Возвращает map, где ключ — обновленный ключ шифрования, а значение — зашифрованный текст.
std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys)  {
    std::map<std::wstring, std::wstring> result;

    for (size_t i = 0; i < openTexts.size(); ++i) {
        std::wstring text = openTexts[i];
        std::wstring key = keys[i];
        
        // Проверяем формат ключа
        if (!is_valid_key_format(key)) {
            throw std::invalid_argument("Неверный формат ключа. Ожидается формат: буква пробел числа_через_пробел. Проверьте отсутствие повторений букв а так же шифробозначений");
        }

        std::wistringstream keyStream(key);
        std::map<wchar_t, std::vector<std::wstring>> substitutionMap;
        std::map<wchar_t, size_t> letterIndices;

        // Чтение ключа (если в исходном ключе был индекс в конце строки — распознаём его и НЕ добавляем в список подстановок)
        std::wstring line;
        while (std::getline(keyStream, line)) {
            std::wistringstream lineStream(line);
            std::wstring letter;
            if (!(lineStream >> letter)) continue;

            wchar_t charKey = letter[0];
            std::vector<std::wstring> tokens;
            std::wstring token;
            while (lineStream >> token) {
                tokens.push_back(token);
            }

            // Если нет никаких токенов — просто инициализируем пустой вектор и индекс 0
            if (tokens.empty()) {
                substitutionMap[charKey] = {};
                letterIndices[charKey] = 0;
                continue;
            }

            // Попробуем определить, является ли последний токен индексом:
            // индекс рассматриваем как числовой токен, значение которого < (количество токенов - 1)
            size_t detectedIndex = 0;
            bool hasIndex = false;
            if (tokens.size() >= 2) {
                bool alldigits = true;
                for (wchar_t wc : tokens.back()) {
                    if (!iswdigit(wc)) { alldigits = false; break; }
                }
                if (alldigits) {
                    try {
                        unsigned long val = std::stoul(tokens.back());
                        // если значение индекса меньше числа реальных ключей (tokens.size()-1), считаем его индексом
                        if (val < tokens.size() - 1) {
                            hasIndex = true;
                            detectedIndex = static_cast<size_t>(val);
                        }
                    } catch (...) {
                        hasIndex = false;
                    }
                }
            }

            // Заполняем substitutionMap ключами (без индекса)
            size_t limit = tokens.size();
            if (hasIndex) limit = tokens.size() - 1; // не включаем последний токен
            for (size_t k = 0; k < limit; ++k) {
                substitutionMap[charKey].push_back(tokens[k]);
            }

            // Инициализация индекса: либо найденный, либо 0
            letterIndices[charKey] = hasIndex ? detectedIndex : 0;
        }

        // Шифрование текста
        std::wstring cipherText;
        for (wchar_t ch : text) {
            if (substitutionMap.count(ch) && !substitutionMap[ch].empty()) {
                size_t& index = letterIndices[ch];
                if (index >= substitutionMap[ch].size()) index = 0;
                cipherText += substitutionMap[ch][index];
                index = (index + 1) % substitutionMap[ch].size();
            } else {
                // Если символа нет в ключе, выбрасываем ошибку
                std::wstring error_msg = L"Символ '";
                error_msg += ch;
                error_msg += L"' отсутствует в ключе шифрования";
                throw std::invalid_argument(std::string(error_msg.begin(), error_msg.end()));
            }
        }

        // Формируем ключ (без индексов в конце строк)
        std::wstringstream updatedKey;
        for (const auto& [charKey, numbers] : substitutionMap) {
            updatedKey << charKey << L" ";
            for (const auto& num : numbers) {
                updatedKey << num << L" ";
            }
            updatedKey << L"\n";
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
        // Проверяем формат ключа
        if (!is_valid_key_format(key)) {
            throw std::invalid_argument("Неверный формат ключа при дешифровании");
        }

        std::wistringstream keyStream(key);
        std::map<std::wstring, wchar_t> reverseMap;  // Число → буква
        std::wstring line;

        // Строим обратный map и находим максимальную длину чисел
        size_t maxNumberLength = 0;
        while (std::getline(keyStream, line)) {
            std::wistringstream lineStream(line);
            std::wstring letter;
            if (!(lineStream >> letter)) continue;

            wchar_t charKey = letter[0];
            std::wstring number;
            while (lineStream >> number) {
                reverseMap[number] = charKey;
                if (number.length() > maxNumberLength) {
                    maxNumberLength = number.length();
                }
            }
        }
        // Проверяем, что обратный словарь не пустой
        if (reverseMap.empty()) {
            throw std::invalid_argument("Ключ не содержит чисел для дешифрования");
}
        std::wstring originalText;
        size_t pos = 0;
        size_t cipherLen = cipherText.length();
        
        // Делим шифртекст на блоки фиксированной длины
        while (pos + maxNumberLength <= cipherLen) {
            std::wstring numberBlock = cipherText.substr(pos, maxNumberLength);
            
            // Ищем число ТОЧНО как в ключах (с ведущими нулями)
            if (reverseMap.count(numberBlock)) {
                originalText += reverseMap[numberBlock];
            } else {
                // Если не нашли, пробуем найти без ведущих нулей
                std::wstring numberWithoutZeros = numberBlock;
                size_t firstNonZero = numberBlock.find_first_not_of(L'0');
                if (firstNonZero != std::wstring::npos) {
                    numberWithoutZeros = numberBlock.substr(firstNonZero);
                } else {
                    numberWithoutZeros = L"0";
                }
                
                if (reverseMap.count(numberWithoutZeros)) {
                    originalText += reverseMap[numberWithoutZeros];
                }
                // Если и так не нашли, пропускаем символ
            }
            
            pos += maxNumberLength;
        }

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

         if (leftBoarder < 0 || rightBoarder < 0) {
            throw std::invalid_argument("Неверное значение: граница должна быть натуральным числом.");
        }
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
            
            std::wstring generated_key = wss.str();
            
            // Проверяем сгенерированный ключ
            if (!is_valid_key_format(generated_key)) {
                throw std::runtime_error("Сгенерирован некорректный ключ");
            }
            
            result.push_back(generated_key);
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
