#include "Vigener_cipher.hpp"
#include <random>
#include <algorithm>
#include <fstream>
#include <filesystem>


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

std::wstring give_random_key(std::string keyFilePath, const std::string& language, const size_t& keyLength, HMAC_DRBG gen)
{
    keyFilePath.push_back('/');
    keyFilePath.append(language);
    keyFilePath.append("_dict_");
    keyFilePath.append(std::to_string(keyLength));
    keyFilePath.append(".txt");

    std::wifstream keyFile(keyFilePath, std::ios::binary);
    size_t wordsCount = give_words_count(keyFile, keyLength, language);
    if (language == "ru") {
        keyFile.imbue(std::locale("ru-RU.UTF-8"));
    }
    else if (language == "en") {
        keyFile.imbue(std::locale("en_US.UTF-8"));
    }

    std::string bannedWordsPath = keyFilePath;
    bannedWordsPath.push_back('/');
    bannedWordsPath.append(language);
    bannedWordsPath.append("_ban_words.txt");
    std::wifstream bannedWordsIn(bannedWordsPath);
    if (!bannedWordsIn.is_open()) {
        bannedWordsIn.close();
        std::wofstream bannedWordsOut(bannedWordsPath);
        bannedWordsOut.close();
        std::wifstream bannedWordsIn(bannedWordsPath);
    }
    if (language == "ru") {
        bannedWordsIn.imbue(std::locale("ru-RU.UTF-8"));
    }
    else if (language == "en") {
        bannedWordsIn.imbue(std::locale("en_US.UTF-8"));
    }
    
    wchar_t* buff = new wchar_t[keyLength + 1];
    std::wstring buffStr;
    std::wstring bannedWord;

    while (true) {
        std::optional<std::vector<uint8_t>> generatedBytes = gen.HMAC_DRBG_Generate_algorithm(8);
        if (gen.HMAC_DRBG_Ressed_Check()) {
            gen.HMAC_DRBG_Ressed(get_entropy());
        }
        size_t generatedNum = convert_bytes_to_ddword(generatedBytes.value());
        size_t randomPos = generatedNum % wordsCount;
        keyFile.seekg(keyFile.beg);
        if (language == "ru") {
            keyFile.seekg(2 * randomPos * (keyLength + 1));
        }
        else if (language == "en") {
            keyFile.seekg(randomPos * (keyLength + 2));
        }
        keyFile.read(buff, keyLength);
        buff[keyLength] = L'\0';
        buffStr = buff;
        while (std::getline(bannedWordsIn, bannedWord)) {
            if (buffStr == bannedWord) {
                continue;
            }
        }
        bannedWordsIn.close();
        std::wofstream bannedWordsOut(bannedWordsPath, std::ios::app);
        if (language == "ru") {
            bannedWordsOut.imbue(std::locale("ru-RU.UTF-8"));
        }
        else if (language == "en") {
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

void sort_key_file(std::string keyFilePath, const std::string& language)
{
    keyFilePath.push_back('/');
    keyFilePath.append("keys.txt");
    std::wifstream keyFile(keyFilePath);
    std::wstring buff;
    std::string res_dict_part_path;
    if (language == "ru") {
        keyFile.imbue(std::locale("ru_RU.UTF-8"));
        setlocale(LC_ALL, "ru_RU.UTF-8");
    }
    else if (language == "en") {
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
        if (language == "ru") {
            res_dict_part.imbue(std::locale("ru_RU.UTF-8"));
        }
        else if(language == "en") {
            res_dict_part.imbue(std::locale("en_US.UTF-8"));
        }
        res_dict_part << buff << '\n';
        res_dict_part.close();
    }
}

void clear_custom(std::string keyFilePath)
{
    for (size_t i = 2; i < 50; ++i) {
        std::string custom_path =
        keyFilePath + "custom_dict_" + std::to_string(i) + ".txt";
        std::ofstream clear_file(custom_path);
        clear_file.close();
    }
}

std::wstring give_random_custom_key(std::string keyFilePath, const std::string& language, const size_t& keyLength, HMAC_DRBG gen)
{
    keyFilePath.push_back('/');
    keyFilePath.append("custom");
    keyFilePath.append("_dict_");
    keyFilePath.append(std::to_string(keyLength));
    keyFilePath.append(".txt");
    
    std::wifstream keyFile(keyFilePath, std::ios::binary);
    size_t wordsCount = give_words_count(keyFile, keyLength, language);
    if (language == "ru") {
        keyFile.imbue(std::locale("ru-RU.UTF-8"));
    }
    else if (language == "en") {
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
    if (language == "ru") {
        bannedWordsIn.imbue(std::locale("ru-RU.UTF-8"));
    }
    else if (language == "en") {
        bannedWordsIn.imbue(std::locale("en_US.UTF-8"));
    }
    
    wchar_t* buff = new wchar_t[keyLength + 1];
    std::wstring buffStr;
    std::wstring bannedWord;

    while (true) {
        std::optional<std::vector<uint8_t>> generatedBytes = gen.HMAC_DRBG_Generate_algorithm(8);
        if (gen.HMAC_DRBG_Ressed_Check()) {
            gen.HMAC_DRBG_Ressed(get_entropy());
        }
        size_t generatedNum = convert_bytes_to_ddword(generatedBytes.value());
        size_t randomPos = generatedNum % wordsCount;
        keyFile.seekg(keyFile.beg);
        if (language == "ru") {
            keyFile.seekg(2 * randomPos * (keyLength + 1));
        }
        else if (language == "en") {
            keyFile.seekg(randomPos * (keyLength + 2));
        }
        keyFile.read(buff, keyLength);
        buff[keyLength] = L'\0';
        buffStr = buff;
        while (std::getline(bannedWordsIn, bannedWord)) {
            if (buffStr == bannedWord) {
                srand(randomPos);
                continue;
            }
        }
        bannedWordsIn.close();
        std::wofstream bannedWordsOut(bannedWordsPath, std::ios::app);
        if (language == "ru") {
            bannedWordsOut.imbue(std::locale("ru-RU.UTF-8"));
        }
        else if (language == "en") {
            bannedWordsOut.imbue(std::locale("en_US.UTF-8"));
        }
        bannedWordsOut << buffStr << '\n';
        bannedWordsOut.close();
        delete[] buff;
        return buffStr;
    }
    
}

void clear_cache(std::string keyFilePath)
{
    std::wofstream clear_ru(keyFilePath + "/ru_ban_words.txt");
    std::wofstream clear_en(keyFilePath + "/en_ban_words.txt");
    std::wofstream clear_custom(keyFilePath + "/custom_ban_words.txt");
}

/*================================================================*/
/*======================== Шифр Виженера =========================*/
/*================================================================*/

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
    if (language == "ru") {
        
        std::wstring ru_alphabet;

        for (size_t i = 1040; i < 1072; ++i){
            ru_alphabet.push_back(static_cast<wchar_t>(i));
        }

        return ru_alphabet;
    }

    if (language == "en") {

        std::wstring en_alphabet;

        for (size_t i = 65; i < 91; ++i){
            en_alphabet.push_back(static_cast<wchar_t>(i));
        }

        return en_alphabet;
    }
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


std::vector<std::wstring> gen_keys(std::string keyPropertys, size_t count)
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
        
        std::string keyDirectory = prop["viginer_key_path"].get<std::string>();
        std::string language = prop["text_language"].get<std::string>();
        size_t keyLength = prop["key_length"].get<size_t>();
        if (keyLength == 0) {
            throw KeyPropertyError("Длина ключевого слова должна быть положительной.");
        }

        std::string keysFilePath = keyDirectory + "/keys.txt";

        std::vector<std::wstring> keys;

        if (std::filesystem::exists(keysFilePath)) {
            clear_custom(keyDirectory);
            clear_cache(keyDirectory);
            sort_key_file(keyDirectory, language);
            for (size_t i = 0; i < count; ++i) {
                keys.push_back(give_random_custom_key(keyDirectory, language, keyLength, gen));
            }
        }
        else {
            clear_cache(keyDirectory);
            for (size_t i = 0; i < count; ++i) {
                keys.push_back(give_random_key(keyDirectory, language, keyLength, gen));
            }
        }

        return keys;

    } catch(nlohmann::json::parse_error &err) {
        throw KeyPropertyError(err.what());
    }
}

std::string get_key_propertys()
{
// сам шаблон как должен выглядеть .json запрос с параметрами
    nlohmann::json keyProp = nlohmann::json::parse(R"({
                "name": "viginer_key_path",
                "type": "string",
                "default": "../dictionaries",
                "label": "Путь к директории с ключевыми файлами: "
            },
            {
                "name": "text_language",
                "type": "string",
                "default": "en",
                "label": "Язык текста (ru/en): "
            },
            {
                "name": "key_length",
                "type": "number",
                "default": 6,
                "label": "Длина ключа: "
            }]})");
    
    return keyProp.dump();
}

void chekRequest(nlohmann::json keyPropertys)
{
    try {
        if (!keyPropertys.contains("viginer_key_path") || !keyPropertys["viginer_key_path"].is_string()) {
            throw KeyPropertyError("Значение \"Путь к директории с ключевыми файлами\" должно иметь строковое значение.");
        }
        
        if (!keyPropertys.contains("text_language") || !keyPropertys["text_language"].is_string()) {
            throw KeyPropertyError("Значение \"Язык текста\" должно иметь строковое значение.");
        }
        std::string language = keyPropertys["text_language"];
        if (language != "ru" && language != "en") {
            throw InvalidKey("Значение \"Язык текста\" должно быть либо \"ru\", либо \"en\".");
        }
        
        if (!keyPropertys.contains("key_length") || !keyPropertys["key_length"].is_number()) {
            throw KeyPropertyError("Значение \"Длина ключа\" должен иметь числовое значение.");
        }
        if (keyPropertys["key_length"].get<int>() <= 0) {
            throw InvalidKey("Значение \"Длина ключа\" должно быть больше 0.");
        }  
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
