#include "viginer_cipher.hpp"
#include <random>

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

std::vector<std::string> gen_keys(std::string keyPropertys, size_t count)
{
    nlohmann::json prop;
    try{
        std::vector<std::string> keys;
    } catch(nlohmann::json::parse_error &err) {
        throw KeyPropertyError(err.what());
    }
    
}

std::string get_key_propertys()
{
    // сам шаблон как должен выглядеть .json запрос с параметрами
    nlohmann::json keyProp = nlohmann::json::parse(R"({"params": [{"name": "key_length", "min": 2, "max": null, "value": 0, "type": "number", "default": 0, "label": "Длина ключа"}]})");
    
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
        if(!keyPropertys.at("disk_count").is_number()) {
            throw KeyPropertyError("Ключ \"Количество дисков\" должен иметь числовое значение...");
        }
        if(keyPropertys.at("disk_count") <= 0) {
            throw InvalidKey("Значение \"Количество дисков\" должно быть больше 0...");
        }
        
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
