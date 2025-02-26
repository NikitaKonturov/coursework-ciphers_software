#include "Vigener_cipher.hpp"
#include <random>
#include <algorithm>
#include <fstream>

/*================================================================*/
/*======================== Шифр Виженера =========================*/
/*================================================================*/

bool is_ru_alpha(uint8_t symbol) {
    return (symbol >= 192 && symbol <= 255) || symbol == 184 || symbol == 168;
}

bool is_en_alpha(uint8_t symbol) {
    return (symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z');
}

std::map<size_t, size_t> count_words(const std::string& keyFilePath, const std::string& language)
{
    bool (*is_alpha) (uint8_t);
    if (language == "ru") {
        setlocale(LC_ALL, ".1251");
        is_alpha = is_ru_alpha;
    }
    else if (language == "en") {
        is_alpha = is_en_alpha;
    }
    
    
    std::ifstream keyFile(keyFilePath, std::ios::binary);
    keyFile.seekg(0, keyFile.end);
    size_t fileSize = keyFile.tellg();
    keyFile.close();
    bool isWordMode = false;
    size_t wordLength = 0;
    std::map<size_t, size_t> wordsCount;
    uint8_t buff;
    for (size_t i = 0; i < fileSize; ++i)
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
        } else if (isWordMode)
        {
            ++wordsCount[wordLength];
            isWordMode = false;
            wordLength = 0;
        }
        keyFile.close();
    }
    return wordsCount;
}

void set_final_key_word_num(size_t& keyWordNum, bool*& bannedWords)
{
    for (size_t i = 0; i < keyWordNum; ++i) {
        if (bannedWords[i] == true) {
            ++keyWordNum;
        }
    }
    bannedWords[keyWordNum - 1] = true;
}

std::string give_word(
    const std::string& keyFilePath, 
    const std::string& language, 
    std::map<size_t, size_t>& wordsCount,
    const size_t& keyWordLength,
    bool*& bannedWords,
    const size_t& allWordsCount
)
{   
    if (wordsCount.at(keyWordLength) == 0) {
        return "End of words";
    }

    bool (*is_alpha) (uint8_t);
    if (language == "ru") {
        setlocale(LC_ALL, ".1251");
        is_alpha = is_ru_alpha;
    }
    else if (language == "en") {
        is_alpha = is_en_alpha;
    }

    srand(time(NULL));
    size_t keyWordNum = (rand() % wordsCount.at(keyWordLength)) + 1;
    set_final_key_word_num(keyWordNum, bannedWords);
    wordsCount.at(keyWordLength) -= 1;

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
        } else if (isWordMode)
        {
            if (wordLength == keyWordLength) {
                --keyWordNum;
                if (keyWordNum == 0) { 
                    keyFile.close(); 
                    std::ifstream keyFile(keyFilePath, std::ios::binary);
                    keyFile.seekg(i - keyWordLength); 
                    std::string keyBuff(keyWordLength, ' ');
                    for (size_t i = 0; i < keyWordLength; ++i)
                    {
                        keyFile.read((char*)&buff, 1);
                        keyBuff[i] = (char) buff;
                    }
                    return keyBuff;
                }
            }
            wordLength = 0; 
            isWordMode = false;
        }
        keyFile.close(); 
    }
    return " "; 
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

std::string to_upper(const std::string& line, const std::string& language) {
    std::string result;

    if (language == "ru") {
        std::locale loc("ru_RU.UTF-8");
        for (char ch : line) {
            result.push_back(std::use_facet<std::ctype<char>>(loc).toupper(ch));
        }
    } 
    else {
        for (char ch : line) {
            result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
        }
    }
    return result;
}

std::vector<std::string> extract_words(const std::string& keyFilePath, size_t keyWordLength, const std::string& language) {
    std::ifstream file(keyFilePath);
    
    if (!file) {
        throw std::runtime_error("Не удалось открыть файл с ключевыми словами.");
    }

    std::vector<std::string> wordsVector;
    std::string word;
    std::wstring alphabet = get_alphabet(language);
    std::string allowed;
    for (wchar_t wc : alphabet) {
        allowed.push_back(static_cast<char>(wc));
    }
    
    while (file >> word) {
        std::string filtered;
        for (char ch : word) {
            std::string uppendCh = to_upper(std::string(1, ch), language);
            if (allowed.find(uppendCh) != std::string::npos) {
                filtered.append(uppendCh);
            }
        }
        if (filtered.size() == keyWordLength) {
            wordsVector.push_back(filtered);
        }
    }
    return wordsVector;
}

std::string determine_language_from_key(const std::wstring& key) {
    if (key.empty()) {
        throw std::invalid_argument("Ключ пуст.");
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
    if(keys.size() < openTexts.size()) {
        throw InvalidKey("Количество ключей должно быть равно количеству открытого текста...");
    }
    
    std::vector<uint8_t> entropy = get_entropy();
    std::vector<uint8_t> nonce = get_entropy();
        
    HMAC_DRBG gen(entropy, nonce, { 'V', 'I', 'G', 'E', 'N', 'E', 'R', '-', 'c', 'i', 'p', 'h', 'e', 'r' });
    std::optional<std::vector<uint8_t>> randomBytesOpt = gen.HMAC_DRBG_Generate_algorithm(4);

    if (!randomBytesOpt.has_value()) {
        throw std::runtime_error("Ошибка генерации случайного числа через HMAC_DRBG.");
    }

    std::vector<uint8_t> randomBytes = randomBytesOpt.value();

    uint32_t randomIndex = *reinterpret_cast<uint32_t*>(randomBytes.data()) % keys.size();

    std::wstring chosenKey = keys[randomIndex];

    std::map<std::wstring, std::wstring> keysAndCiphersTexts;
    for (size_t i = 0; i < openTexts.size(); ++i) {
        std::wstring text = openTexts[i];
        std::wstring alphabet = get_alphabet(define_language(text));
        std::wstring cipher = vigenere_encrypt(text, chosenKey, alphabet);
        std::wstringstream wss;
        wss << L"Vigenere key: " << chosenKey;
        keysAndCiphersTexts[wss.str()] = cipher;
    }
    return keysAndCiphersTexts;
}

std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndCipherTexts)
{
    std::map<std::wstring, std::wstring> keysAndOpenTexts;
    for (const auto& pair : keysAndCipherTexts) {
        std::wstring keyStr = pair.first;
        std::wstring cipherText = pair.second;
        std::wstring delimiter = L"Vigenere key: ";
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
        size_t keyWordLength = 6;
        if (prop.contains("key_word_length") && prop["key_word_length"].is_number_integer()) {
            keyWordLength = prop["key_word_length"].get<size_t>();
            if (keyWordLength == 0) {
            throw std::invalid_argument("Длина ключевого слова должна быть положительной.");
            }
        }

        std::vector<std::string> keys;

        std::map<size_t, size_t> countedWords = count_words(prop["key_file_path"], language);
        size_t allWordsCount = countedWords[keyWordLength];

        bool* bannedWords = new bool[allWordsCount];
        for (size_t i = 0; i < allWordsCount; ++i) {
            bannedWords[i] = false;
        }

        for (size_t i = 0; i < count; ++i) {
            std::string word = give_word(prop["key_file_path"], language, countedWords, keyWordLength, bannedWords, allWordsCount);
            keys.push_back(word);
        }
        
        return keys;
        delete[] bannedWords;

    } catch(nlohmann::json::parse_error &err) {
        throw KeyPropertyError(err.what());
    }
}

std::string get_key_propertys()
{
// сам шаблон как должен выглядеть .json запрос с параметрами
    nlohmann::json keyProp = nlohmann::json::parse(R"({"text_language": "en", "params": []})");
    
    return keyProp.dump();
}

void chekRequest(nlohmann::json keyPropertys)
{
    try {
        if(!keyPropertys.at("text_language").is_string()) {
            throw KeyPropertyError("Значение text_language должен иметь строковое значение...");
        }
        if(keyPropertys["text_language"] != "ru" && keyPropertys["text_language"] != "en") {
            throw InvalidKey("Значение text_language должно быть ru или en...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
