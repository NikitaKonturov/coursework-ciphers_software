#include "Vigener_cipher.hpp"
#include <random>
#include <algorithm>
#include <fstream>
#include <filesystem>

/*================================================================*/
/*======================== Шифр Виженера =========================*/
/*================================================================*/

std::string find_word_file(const std::string& directory) {
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.path().extension() == ".txt") {
            return entry.path().string();
        }
    }
    throw std::runtime_error("Не найден файл со словами (.txt) в директории: " + directory);
}

std::vector<std::string> load_words(const std::string& filePath, size_t wordLength) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filePath);
    }

    std::vector<std::string> words;
    std::string word;
    while (file >> word) {
        if (word.length() == wordLength) {
            words.push_back(word);
        }
    }

    file.close();
    if (words.empty()) {
        throw std::runtime_error("Не найдено слов длины " + std::to_string(wordLength));
    }
    
    return words;
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
    if (has_ru) return "ru";
    throw InvalidOpenText("Ошибка: недопустимый язык в тексте.");
}

std::wstring get_alphabet(const std::string& language) {
    return language == "en" ? L"ABCDEFGHIJKLMNOPQRSTUVWXYZ" : L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    throw std::invalid_argument("Неподдерживаемый язык.");
}

std::wstring vigenere_encrypt(const std::wstring& text, const std::wstring& key, const std::wstring& alphabet) {
    
    if (key.empty()) {
        throw InvalidKey("Ключ не может быть пустым.");
    }
    
    std::wstring ciphertext;
    for (size_t i = 0; i < text.size(); ++i) {
        wchar_t symbol = text[i];
        if (alphabet.find(symbol) == std::wstring::npos) {
            throw InvalidOpenText("Открытый текст содержит недопустимый символ.");
        }
        wchar_t keySymbol = key[i % key.size()];
        if (alphabet.find(keySymbol) == std::wstring::npos) {
            throw InvalidKey("Ключ содержит недопустимый символ.");
        }
        size_t symbolIndex = alphabet.find(symbol);
        size_t keyIndex = alphabet.find(keySymbol);
        size_t cipherIndex = (symbolIndex + keyIndex) % alphabet.size();
        ciphertext.push_back(alphabet[cipherIndex]);
    }
    return ciphertext;
}

std::wstring vigenere_decrypt(const std::wstring& ciphertext, const std::wstring& key, const std::wstring& alphabet) {
    
    if (key.empty()) {
        throw InvalidKey("Ключ не может быть пустым.");
    }
    
    std::wstring openText;
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        wchar_t cipherSymbol = ciphertext[i];
        if (alphabet.find(cipherSymbol) == std::wstring::npos) {
            throw InvalidOpenText("Зашифрованный текст содержит недопустимый символ.");
        }
        wchar_t keySymbol = key[i % key.size()];
        if (alphabet.find(keySymbol) == std::wstring::npos) {
            throw InvalidKey("Ключ содержит недопустимый символ.");
        }
        size_t cipherIndex = alphabet.find(cipherSymbol);
        size_t keyIndex = alphabet.find(keySymbol);
        size_t openTextIndex = (cipherIndex + alphabet.size() - keyIndex) % alphabet.size();
        openText.push_back(alphabet[openTextIndex]);
    }
    return openText;
}

std::string determine_language_from_key(const std::wstring& key) {
    if (key.empty()) {
        throw InvalidKey("Ключ пуст.");
    }

    wchar_t firstCh = key[0];
    std::wstring ruAlphabet = get_alphabet("ru");
    std::wstring enAlphabet = get_alphabet("en");
    if (ruAlphabet.find(firstCh) != std::wstring::npos) {
        return "ru";
    }
    else if (enAlphabet.find(firstCh) != std::wstring::npos) {
        return "en";
    }
    else {
        throw InvalidKey("Невозможно определить язык ключа.");
    }
}

std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys)
{
    std::locale::global(std::locale("ru_RU.UTF-8")); 
    std::wcout.imbue(std::locale()); 
    if (keys.size() < openTexts.size()) {
        throw InvalidKey("Количество ключей должно быть как минимум равно количеству открытых текстов...");
    }

    std::map<std::wstring, std::wstring> keysAndCiphersTexts;

    for (size_t i = 0; i < openTexts.size(); ++i) {
        const std::wstring text = openTexts[i];
        const std::wstring key = keys[i];
        std::wstring alphabet = get_alphabet(define_language(text));

        std::wstring cipherText = vigenere_encrypt(text, key, alphabet);

        std::wstringstream wss;
        wss << L"Ключ для шифра: " << key;
        keysAndCiphersTexts[wss.str()] = cipherText;
    }
    return keysAndCiphersTexts;
}

std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndCipherTexts)
{
    std::map<std::wstring, std::wstring> keysAndOpenTexts;
    for (const auto& pair : keysAndCipherTexts) {
        std::wstring keyStr = pair.first;
        std::wstring cipherText = pair.second;
        std::wstring delimiter = L"Ключ для шифра: ";
        size_t pos = keyStr.find(delimiter);
        if (pos == std::wstring::npos) {
            throw InvalidKey("Ключ не найден в строке.");
        }
        std::wstring key = keyStr.substr(pos + delimiter.size());
        std::wstring alphabet = get_alphabet(determine_language_from_key(key));
        std::wstring openText = vigenere_decrypt(cipherText, key, alphabet);
        keysAndOpenTexts[keyStr] = openText;
    }
    return keysAndOpenTexts;
}


std::vector<std::string> gen_keys(std::string keyPropertys, size_t count)
{
    nlohmann::json prop;
    try{
        std::replace(keyPropertys.begin(), keyPropertys.end(), '\'', '\"');
        std::cout << keyPropertys << std::endl;

        prop = nlohmann::json::parse(keyPropertys);
        chekRequest(prop);
        
        std::vector<uint8_t> entropy = get_entropy();
        std::vector<uint8_t> nonce = get_entropy();
        
        HMAC_DRBG gen(entropy, nonce, { 'V', 'I', 'G', 'E', 'N', 'E', 'R', '-', 'c', 'i', 'p', 'h', 'e', 'r' });
        
        std::string language = prop["text_language"];
        std::string directory = prop["viginer_path_to_dir"];
        size_t keyWordLength = 6;

        if (prop.contains("vigener_key_size") && prop["vigener_key_size"].is_number_integer()) {
            keyWordLength = prop["vigener_key_size"].get<size_t>();
            if (keyWordLength <= 0) {
                throw InvalidKey("Длина ключевого слова должна быть положительной.");
            }
        }

        std::string filePath = find_word_file(directory);
        std::vector<std::string> words = load_words(filePath, keyWordLength);

        std::vector<std::string> result;
        for (size_t i = 0; i < count; ++i) {
            uint64_t randomIndex = convert_bytes_to_ddword(gen.HMAC_DRBG_Generate_algorithm(256).value()) % words.size();
            result.push_back(words[randomIndex]);
        }

        return result;

    } catch(nlohmann::json::parse_error &err) {
        throw KeyPropertyError(err.what());
    }
}

std::string get_key_propertys()
{
// сам шаблон как должен выглядеть .json запрос с параметрами
    nlohmann::json keyProp = nlohmann::json::parse(R"({"text_language": "en", "params": [{"name": "vigener_key_size", "min": 1, "max": null, "value": 0, "type": "number", "default": 0, "label": "Длина ключа"}]})");
    
    return keyProp.dump();
}

void chekRequest(nlohmann::json keyPropertys)
{
    try {
        if(!keyPropertys.at("text_language").is_string()) {
            throw KeyPropertyError("Ключ text_language должен иметь строковое значение...");
        }
        if(keyPropertys["text_language"] != "ru" && keyPropertys["text_language"] != "en") {
            throw InvalidKey("Значение \"Язык текста\" должно быть ru или en...");
        }
        if(!keyPropertys.at("gamut_size").is_number()) {
            throw KeyPropertyError("Ключ \"Длина ключа\" должен иметь числовое значение...");
        }
        if(keyPropertys.at("disk_count") <= 0) {
            throw InvalidKey("Значение \"Длина ключа\" должно быть больше 0...");
        }   
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
