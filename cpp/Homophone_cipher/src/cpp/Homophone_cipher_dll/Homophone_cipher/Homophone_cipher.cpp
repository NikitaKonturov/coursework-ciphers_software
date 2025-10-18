#include "homophone_cipher.hpp"
#include <iomanip>
#include <codecvt>
#include <locale>
#include <regex>
#include <set>

// Функция для определения языка текста (только верхний регистр, без Ё)
std::string detect_language(const std::wstring& text) {
    bool has_cyrillic = false;
    bool has_latin = false;
    
    for (wchar_t c : text) {
        if (iswalpha(c)) {
            // Кириллические символы в верхнем регистре (А-Я, без Ё)
            if (c >= L'А' && c <= L'Я') {
                has_cyrillic = true;
            }
            // Латинские символы в верхнем регистре
            else if (c >= L'A' && c <= L'Z') {
                has_latin = true;
            }
        }
    }
    
    if (has_cyrillic && has_latin) {
        return "mixed";
    } else if (has_cyrillic) {
        return "ru";
    } else if (has_latin) {
        return "en";
    } else {
        return "unknown";
    }
}

// Функция для проверки совпадения языков
void check_language_compatibility(const std::wstring& text, const std::wstring& key) {
    std::string text_lang = detect_language(text);
    std::string key_lang = detect_language(key);
    
    // Если оба текста содержат смешанные языки или несовместимые языки
    if ((text_lang == "mixed" && key_lang != "mixed") || 
        (key_lang == "mixed" && text_lang != "mixed") ||
        (text_lang == "ru" && key_lang == "en") || 
        (text_lang == "en" && key_lang == "ru")) {
        throw std::invalid_argument("Язык текста и ключа не совпадают. Текст: " + text_lang + ", Ключ: " + key_lang);
    }
}

std::wstring normalize_key(const std::wstring& key) {
    std::wstringstream normalized;
    std::wistringstream keyStream(key);
    std::wstring line;
    
    while (std::getline(keyStream, line)) {
        if (line.empty()) continue;
        
        // Убираем пробелы в начале и конце
        size_t start = line.find_first_not_of(L" \t");
        if (start == std::wstring::npos) continue;
        size_t end = line.find_last_not_of(L" \t");
        std::wstring cleaned_line = line.substr(start, end - start + 1);
        
        // Обрабатываем посимвольно
        for (size_t i = 0; i < cleaned_line.length(); ) {
            wchar_t ch = cleaned_line[i];
            
            if (iswalpha(ch)) {
                // Нашли букву - начинаем новую строку
                normalized << ch << L" ";
                
                // Ищем числа после буквы
                i++;
                std::wstring numbers;
                while (i < cleaned_line.length()) {
                    wchar_t next_ch = cleaned_line[i];
                    if (iswdigit(next_ch)) {
                        numbers += next_ch;
                        i++;
                    } else if (iswalpha(next_ch)) {
                        // Новая буква - заканчиваем текущую строку
                        break;
                    } else {
                        // Пробел или другой символ - добавляем пробел между числами
                        if (!numbers.empty() && numbers.back() != L' ') {
                            numbers += L' ';
                        }
                        i++;
                    }
                }
                
                // Убираем лишний пробел в конце и добавляем строку
                if (!numbers.empty()) {
                    if (numbers.back() == L' ') {
                        numbers.pop_back();
                    }
                    normalized << numbers << L"\n";
                }
            } else {
                i++; // Пропускаем не-буквенные символы в начале
            }
        }
    }
    
    return normalized.str();
}

bool is_valid_key_format(const std::wstring& key) {
    if (key.empty()) {
        return false;
    }
    
    // Сначала нормализуем ключ
    std::wstring normalized_key = normalize_key(key);
    
    // Если после нормализации ключ пустой - невалидный
    if (normalized_key.empty()) {
        return false;
    }
    
    std::set<wchar_t> seen_letters;
    std::set<std::wstring> seen_numbers;
    bool has_valid_lines = false;
    
    std::wistringstream keyStream(normalized_key);
    std::wstring line;
    
    while (std::getline(keyStream, line)) {
        if (line.empty()) continue;
        
        std::wistringstream lineStream(line);
        std::wstring letter;
        if (!(lineStream >> letter)) {
            continue;
        }
        
        if (letter.length() != 1) {
            return false;
        }
        
        wchar_t charKey = letter[0];
        
        if (!iswalpha(charKey)) {
            return false;
        }
        
        if (seen_letters.count(charKey)) {
            return false;
        }
        seen_letters.insert(charKey);
        
        std::wstring number;
        bool has_numbers = false;
        while (lineStream >> number) {
            bool all_digits = true;
            for (wchar_t c : number) {
                if (!iswdigit(c)) {
                    all_digits = false;
                    break;
                }
            }
            if (!all_digits) {
                return false;
            }
            
            if (seen_numbers.count(number)) {
                return false;
            }
            seen_numbers.insert(number);
            has_numbers = true;
        }
        
        if (!has_numbers) {
            return false;
        }
        
        has_valid_lines = true;
    }
    
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
        
        // Проверяем совместимость языков
        check_language_compatibility(text, key);
        
        // Нормализуем ключ перед использованием
        std::wstring normalized_key = normalize_key(key);
        
        // Проверяем формат нормализованного ключа
        if (!is_valid_key_format(normalized_key)) {
            throw std::invalid_argument("Неверный формат ключа. Ожидается формат: буква пробел числа_через_пробел. Проверьте отсутствие повторений букв а так же шифробозначений");
        }

        // ИСПРАВЛЕНИЕ: используем ТОЛЬКО нормализованный ключ
        std::wistringstream keyStream(normalized_key);
        std::map<wchar_t, std::vector<std::wstring>> substitutionMap;
        std::map<wchar_t, size_t> letterIndices;

        // Чтение ключа
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

            // Попробуем определить, является ли последний токен индексом
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
            if (hasIndex) limit = tokens.size() - 1;
            for (size_t k = 0; k < limit; ++k) {
                substitutionMap[charKey].push_back(tokens[k]);
            }

            // Инициализация индекса
            letterIndices[charKey] = hasIndex ? detectedIndex : 0;
        }

        std::set<wchar_t> missing_letters;
        for (wchar_t ch : text) {
            if (substitutionMap.count(ch) == 0 || substitutionMap[ch].empty()) {
                missing_letters.insert(ch);
            }
        }
        
        if (!missing_letters.empty()) {
            std::wstring error_msg = L"В ключе отсутствуют шифробозначения для букв: ";
            for (wchar_t missing : missing_letters) {
                error_msg += missing;
                error_msg += L", ";
            }
            error_msg.pop_back(); // Убираем последнюю запятую
            error_msg.pop_back(); // Убираем последний пробел
            throw std::invalid_argument(std::string(error_msg.begin(), error_msg.end()));
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
        // Нормализуем ключ перед использованием
        std::wstring normalized_key = normalize_key(key);
        
        // Проверяем формат нормализованного ключа
        if (!is_valid_key_format(normalized_key)) {
            throw std::invalid_argument("Неверный формат ключа при дешифровании");
        }

        // ИСПРАВЛЕНИЕ: используем ТОЛЬКО нормализованный ключ
        std::wistringstream keyStream(normalized_key);
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
            throw KeyPropertyError("Значение text_language должно иметь строковое значение...");
        }
        if(keyPropertys["text_language"] != "ru" && keyPropertys["text_language"] != "en") {
            throw InvalidKey("Значение language должно быть или ru или en...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
