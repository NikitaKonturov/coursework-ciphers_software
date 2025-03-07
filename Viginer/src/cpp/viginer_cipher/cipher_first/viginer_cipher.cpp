#include "viginer_cipher.hpp"
#include <random>


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

std::wstring give_random_key(nlohmann::json prop)
{
    HMAC_DRBG generator(get_entropy(), get_entropy());
    std::string keyFilePath = prop["viginer_path_to_dir"];
    keyFilePath.push_back('/');
    keyFilePath.append(prop["text_language"]);
    keyFilePath.append("_dict_");
    keyFilePath.append(prop["key_length"]);
    keyFilePath.append(".txt");

    std::wifstream keyFile(keyFilePath, std::ios::binary);
    size_t wordsCount = give_words_count(keyFile, prop["key_length"], prop["text_language"]);
    if (prop["text_language"] == "ru") {
        keyFile.imbue(std::locale("ru-RU.UTF-8"));
    }
    else if (prop["text_language"] == "en") {
        keyFile.imbue(std::locale("en_US.UTF-8"));
    }

    std::string bannedWordsPath = prop["viginer_path_to_dir"];
    bannedWordsPath.push_back('/');
    bannedWordsPath.append(prop["text_language"]);
    bannedWordsPath.append("_ban_words.txt");
    std::wifstream bannedWordsIn(bannedWordsPath);
    if (!bannedWordsIn.is_open()) {
        bannedWordsIn.close();
        std::wofstream bannedWordsOut(bannedWordsPath);
        bannedWordsOut.close();
        std::wifstream bannedWordsIn(bannedWordsPath);
    }
    if (prop["text_language"] == "ru") {
        bannedWordsIn.imbue(std::locale("ru-RU.UTF-8"));
    }
    else if (prop["text_language"] == "en") {
        bannedWordsIn.imbue(std::locale("en_US.UTF-8"));
    }
    
    wchar_t* buff = new wchar_t[prop["key_length"] + 1];
    std::wstring buffStr;
    std::wstring bannedWord;

    while (true) {
        std::optional<std::vector<uint8_t>> generatedBytes = generator.HMAC_DRBG_Generate_algorithm(8);
        if (generator.HMAC_DRBG_Ressed_Check()) {
            generator.HMAC_DRBG_Ressed(get_entropy());
        }
        size_t generatedNum = convert_bytes_to_ddword(generatedBytes.value());
        size_t randomPos = generatedNum % wordsCount;
        keyFile.seekg(keyFile.beg);
        if (prop["text_language"] == "ru") {
            keyFile.seekg(2 * randomPos * (prop["key_length"] + 1));
        }
        else if (prop["text_language"] == "en") {
            keyFile.seekg(randomPos * (prop["key_length"] + 2));
        }
        keyFile.read(buff, prop["key_length"]);
        buff[prop["key_length"]] = L'\0';
        buffStr = buff;
        while (std::getline(bannedWordsIn, bannedWord)) {
            if (buffStr == bannedWord) {
                continue;
            }
        }
        bannedWordsIn.close();
        std::wofstream bannedWordsOut(bannedWordsPath, std::ios::app);
        if (prop["text_language"] == "ru") {
            bannedWordsOut.imbue(std::locale("ru-RU.UTF-8"));
        }
        else if (prop["text_language"] == "en") {
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

void sort_key_file(nlohmann::json prop)
{
    std::string keyFilePath = prop["viginer_path_to_dir"];
    keyFilePath.push_back('/');
    keyFilePath.append("keys.txt");
    std::wifstream keyFile(keyFilePath);
    std::wstring buff;
    std::string res_dict_part_path;
    if (prop["text_language"] == "ru") {
        keyFile.imbue(std::locale("ru_RU.UTF-8"));
        setlocale(LC_ALL, "ru_RU.UTF-8");
    }
    else if (prop["text_language"] == "en") {
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
        if (prop["text_language"] == "ru") {
            res_dict_part.imbue(std::locale("ru_RU.UTF-8"));
        }
        else if(prop["text_language"] == "en") {
            res_dict_part.imbue(std::locale("en_US.UTF-8"));
        }
        res_dict_part << buff << '\n';
        res_dict_part.close();
    }
}

void clear_custom(nlohmann::json prop)
{
    for (size_t i = 2; i < 50; ++i) {
        std::string custom_path =
        prop["viginer_path_to_dir"] + "custom_dict_" + std::to_string(i) + ".txt";
        std::ofstream clear_file(custom_path);
        clear_file.close();
    }
}

std::wstring give_random_custom_key(nlohmann::json prop)
{
    HMAC_DRBG generator(get_entropy(), get_entropy());

    std::string keyFilePath = prop["viginer_path_to_dir"];
    keyFilePath.push_back('/');
    keyFilePath.append("custom");
    keyFilePath.append("_dict_");
    keyFilePath.append(prop["key_length"]);
    keyFilePath.append(".txt");
    
    std::wifstream keyFile(keyFilePath, std::ios::binary);
    size_t wordsCount = give_words_count(keyFile, prop["key_length"], prop["text_language"]);
    if (prop["text_language"] == "ru") {
        keyFile.imbue(std::locale("ru-RU.UTF-8"));
    }
    else if (prop["text_language"] == "en") {
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
    if (prop["text_language"] == "ru") {
        bannedWordsIn.imbue(std::locale("ru-RU.UTF-8"));
    }
    else if (prop["text_language"] == "en") {
        bannedWordsIn.imbue(std::locale("en_US.UTF-8"));
    }
    
    wchar_t* buff = new wchar_t[prop["key_length"] + 1];
    std::wstring buffStr;
    std::wstring bannedWord;

    while (true) {
        std::optional<std::vector<uint8_t>> generatedBytes = generator.HMAC_DRBG_Generate_algorithm(8);
        if (generator.HMAC_DRBG_Ressed_Check()) {
            generator.HMAC_DRBG_Ressed(get_entropy());
        }
        size_t generatedNum = convert_bytes_to_ddword(generatedBytes.value());
        size_t randomPos = generatedNum % wordsCount;
        keyFile.seekg(keyFile.beg);
        if (prop["text_language"] == "ru") {
            keyFile.seekg(2 * randomPos * (prop["key_length"] + 1));
        }
        else if (prop["text_language"] == "en") {
            keyFile.seekg(randomPos * (prop["key_length"] + 2));
        }
        keyFile.read(buff, prop["key_length"]);
        buff[prop["key_length"]] = L'\0';
        buffStr = buff;
        while (std::getline(bannedWordsIn, bannedWord)) {
            if (buffStr == bannedWord) {
                srand(randomPos);
                continue;
            }
        }
        bannedWordsIn.close();
        std::wofstream bannedWordsOut(bannedWordsPath, std::ios::app);
        if (prop["text_language"] == "ru") {
            bannedWordsOut.imbue(std::locale("ru-RU.UTF-8"));
        }
        else if (prop["text_language"] == "en") {
            bannedWordsOut.imbue(std::locale("en_US.UTF-8"));
        }
        bannedWordsOut << buffStr << '\n';
        bannedWordsOut.close();
        delete[] buff;
        return buffStr;
    }
    
}

void clear_cache(nlohmann::json prop)
{
    std::wofstream clear_ru(prop["viginer_path_to_dir"] + "/ru_ban_words.txt");
    std::wofstream clear_en(prop["viginer_path_to_dir"] + "/en_ban_words.txt");
    std::wofstream clear_custom(prop["viginer_path_to_dir"] + "/custom_ban_words.txt");
}

/*================================================================*/
/*===================== Шифр вижинера ======================*/
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

    if (has_ru) return "ru";
    if (has_en) return "en";
    throw InvalidOpenText("Ошибка: неверный язык в тексте.");
}

wchar_t put_viginer_on_char(const wchar_t& openTextChar, const wchar_t& keyChar, const std::string& lang)
{
    wchar_t cipherTextChar;
    if (lang == "ru") {
        uint16_t buff = static_cast<uint16_t>(openTextChar) - 1040 + static_cast<uint16_t>(keyChar);
        if (buff > 1071) { 
            buff -= 32;
        }
        cipherTextChar = static_cast<wchar_t>(buff);
    }
    else if (lang == "en") {
        int16_t buff = static_cast<uint16_t>(openTextChar) - 65 + static_cast<uint16_t>(keyChar);
        if (buff > 90) { buff -= 26;};
        cipherTextChar = static_cast<wchar_t>(buff);
    }
    return cipherTextChar;
}

std::wstring put_viginer_on_text(const std::wstring& openText, const std::wstring& key)
{
    size_t openTextLen = openText.length();
    size_t keyLen = key.length();
    std::wstring cipherText;
    size_t keyCycle = 0;
    std::string lang = define_language(openText);
    for (size_t i = 0; i < openTextLen; ++i) {
        cipherText += put_viginer_on_char(openText[i], key[keyCycle], lang);
        ++keyCycle;
        if (keyCycle >= keyLen) {
            keyCycle -= keyLen;
        }
    }
    return cipherText;
}

std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys)
{
    std::locale::global(std::locale("ru_RU.UTF-8"));
    std::wcout.imbue(std::locale());   
    if (keys.size() < openTexts.size()) {
        throw InvalidKey("Количество ключей должно быть как минимум равно количеству открытых текстов...");
    }

    std::map<std::wstring, std::wstring> keysAndCiphersTexts;

    for (size_t i = 0; i < keys.size(); ++i) {
        keysAndCiphersTexts[keys[i]] = put_viginer_on_text(openTexts[i], keys[i]);
    }

    return keysAndCiphersTexts;
}

wchar_t put_viginer_off_char(const wchar_t& cipherTextChar, const wchar_t& keyChar, const std::string& lang)
{
    wchar_t openTextChar;
    if (lang == "ru") {
        uint16_t buff = static_cast<uint16_t>(cipherTextChar) + 1040 - static_cast<uint16_t>(keyChar);
        if (buff < 1040) { 
            buff += 32;
        }
        openTextChar = static_cast<wchar_t>(buff);
    }
    else if (lang == "en") {
        int16_t buff = static_cast<uint16_t>(cipherTextChar) - 65 + static_cast<uint16_t>(keyChar);
        if (buff < 65) { buff += 26;};
        openTextChar = static_cast<wchar_t>(buff);
    }
    return openTextChar;
}

std::wstring put_viginer_off_text(const std::wstring& cipherText, const std::wstring& key)
{
    size_t cipherTextLen = cipherText.length();
    size_t keyLen = key.length();
    std::wstring openText;
    size_t keyCycle = 0;
    std::string lang = define_language(cipherText);
    for (size_t i = 0; i < cipherTextLen; ++i) {
        openText += put_viginer_off_char(cipherText[i], key[keyCycle], lang);
        ++keyCycle;
        if (keyCycle >= keyLen) {
            keyCycle -= keyLen;
        }
    }
    return openText;
}

std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndCipherTexts)
{
    std::map<std::wstring, std::wstring> keysAndOpenTexts;
    for(auto& keyAndCipherText : keysAndCipherTexts) {
        std::wstring key = keyAndCipherText.first;
        std::wstring cipherText = keyAndCipherText.second;  
        
        keysAndOpenTexts[key] = put_viginer_off_text(cipherText, key);

    }
    for (auto i : keysAndOpenTexts) {
        std::wcout << i.first << '\n' << i.second << '\n' << '\n';
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
        std::vector<std::wstring> keys;
        
        std::cout << prop["viginer_path_to_dir"] << '\n' << '\n';
        std::cout << prop["text_language"] << '\n' << '\n';
        prop["key_length"] = (int) prop["key_length"];

        if (std::filesystem::exists(prop["viginer_path_to_dir"] + "/keys.txt")) {
            clear_custom(prop);
            clear_cache(prop);
            sort_key_file(prop);
            keys.push_back(give_random_custom_key(prop));
        }
        else {
            clear_cache(prop);
            keys.push_back(give_random_key(prop));
        }
        return keys;
    } catch(nlohmann::json::parse_error &err) {
        throw KeyPropertyError(err.what());
    }
}

std::string get_key_propertys()
{
    // сам шаблон как должен выглядеть .json запрос с параметрами
    nlohmann::json keyProp = nlohmann::json::parse(
        R"({"params": [
        {"name": "key_length", "label" : "Длина ключа"}
            ]
        })");

        std::cout << "OK";
    
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
        if(!keyPropertys.at("key_length").is_number()) {
            throw KeyPropertyError("Ключ \"Длина ключа\" должен иметь числовое значение...");
        }
        if(keyPropertys.at("key_length") <= 1) {
            throw InvalidKey("Значение \"Длина ключа\" должно быть больше 1...");
        }

        std::cout << "Double OK";
        
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
