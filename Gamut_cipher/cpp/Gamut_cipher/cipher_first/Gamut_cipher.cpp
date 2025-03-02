#include "Gamut_cipher.hpp"
#include <random>
#include <algorithm>

/*================================================================*/
/*======================== Шифр Гаммирования =========================*/
/*================================================================*/

std::wstring get_en_completion()
{
    return L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

std::wstring get_ru_completion()
{
    return L"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯ";
}

std::string define_language(const std::wstring& text)
{
    bool has_ru = false, has_en = false;

    for (wchar_t ch : text) {
        if (ch >= L'А' && ch <= L'Я') {
            has_ru = true;
        } else if (ch >= L'A' && ch <= L'Z') {
            has_en = true;
        }
        if (has_ru && has_en) {
            throw InvalidOpenText("Ошибка: неверный язык в тексте.");
        }
    }

    if (has_en) return "en";
    if (has_ru) return "ru";
    throw InvalidOpenText("Ошибка: текст не содержит допустимых символов.");
}

std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys)
{
    std::locale::global(std::locale("ru_RU.UTF-8")); 
    std::wcout.imbue(std::locale()); 
    if (keys.size() < openTexts.size()) {
        throw InvalidKey("Количество ключей должно быть как минимум равно количеству открытых текстов...");
    }

    std::map<std::wstring, std::wstring> keysAndCiphersTexts;
    
    for (size_t i = 0; i < openTexts.size(); ++i)
    {
        std::wstring text = openTexts[i];

        if (keys[i].size() != text.size())
        {
            throw KeyPropertyError("Размер гаммы должен соответствовать размеру открытого текста!");
        }

        std::string lang = define_language(text);
        std::wstring alphabet = (lang == "ru") ? get_ru_completion() : get_en_completion();
        size_t alphabetSize = alphabet.size();

        std::wstring gamma(keys[i].begin(), keys[i].end());
        if (gamma.empty())
        {
            throw InvalidKey("Гамма-ключ не может быть пустым.");
        }

        std::wstringstream cipherStream;
        for (size_t j = 0; j < text.size(); ++j)
        {
            wchar_t plainCh = text[j];
            size_t plainChPos = alphabet.find(plainCh);
            if (plainChPos == std::wstring::npos)
            {
                throw InvalidOpenText("Открытый текст содержит недопустимый символ.");
            }

            wchar_t gammaCh = gamma[j];
            size_t gammaChPos = alphabet.find(gammaCh);
            if (gammaChPos == std::wstring::npos)
            {
                throw InvalidKey("Гамма-ключ содержит недопустимый символ.");
            }

            size_t cipherPos = (plainChPos + gammaChPos) % alphabetSize;
            cipherStream << alphabet[cipherPos];
        }

        keysAndCiphersTexts[gamma] = cipherStream.str();

    }

    return keysAndCiphersTexts;
}

std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndCipherTexts)
{
    std::map<std::wstring, std::wstring> keysAndOpenTexts;
    
    for (auto &pair : keysAndCipherTexts)
    {
        std::wstring gamma = pair.first;
        std::wstring cipherText = pair.second;

        if (gamma.size() != cipherText.size())
        {
            throw KeyPropertyError("Размер гаммы должен соответствовать размеру открытого текста!");
        }

        if (gamma.empty())
        {
            throw InvalidKey("Гамма-ключ не может быть пустым.");
        }

        std::string lang = define_language(cipherText);
        std::wstring alphabet = (lang == "ru") ? get_ru_completion() : get_en_completion();
        size_t alphabetSize = alphabet.size();

        std::wstringstream openTextStream;
        for (size_t j = 0; j < cipherText.size(); ++j)
        {
            wchar_t cipherCh = cipherText[j];
            size_t cipherChPos = alphabet.find(cipherCh);
            if (cipherChPos == std::wstring::npos)
            {
                throw InvalidOpenText("Зашифрованный текст содержит недопустимый символ.");
            }

            wchar_t gammaCh = gamma[j];
            size_t gammaChPos = alphabet.find(gammaCh);
            if (gammaChPos == std::wstring::npos)
            {
                throw InvalidKey("Гамма-ключ содержит недопустимый символ.");
            }

            size_t openPos = (cipherChPos + alphabetSize - gammaChPos) % alphabetSize;
            openTextStream << alphabet[openPos];
        }

        keysAndOpenTexts[gamma] = openTextStream.str();
    }

    return keysAndOpenTexts;
}


std::vector<std::string> gen_keys(std::string keyPropertys, size_t count)
{
    nlohmann::json prop;
    try
    {
        std::replace(keyPropertys.begin(), keyPropertys.end(), '\'', '\"');
        prop = nlohmann::json::parse(keyPropertys);
        chekRequest(prop);

        std::vector<uint8_t> entropy = get_entropy();
        std::vector<uint8_t> nonce = get_entropy();
        HMAC_DRBG gen(entropy, nonce, {'G', 'a', 'm', 'u', 't', '-', 'c', 'i', 'p', 'h', 'e', 'r'});

        std::vector<std::string> result;

        std::wstring alphabet = (prop["text_language"] == "en") ? get_en_completion() : get_ru_completion();
        size_t alphabetSize = alphabet.size();
        size_t gamutSize = prop["gamut_size"].get<size_t>();
        
        for (size_t key = 0; key < count; ++key) 
        {
            if (gen.HMAC_DRBG_Ressed_Check()) 
            {
                gen.HMAC_DRBG_Ressed(get_entropy());
            }

            std::wstringstream ss;
            for (size_t i = 0; i < gamutSize; ++i) 
            {
                auto random_opt = gen.HMAC_DRBG_Generate_algorithm(256);
                if (!random_opt.has_value()) {
                    throw KeyPropertyError("Не удалось сгенерировать случайное значение.");
                }
                uint64_t random_value = convert_bytes_to_ddword(random_opt.value());
                ss << alphabet[random_value % alphabetSize];
            }

            std::wstring wskey = ss.str();
            result.push_back(std::string(wskey.begin(), wskey.end()));
        }

        return result;
    } 
    catch (nlohmann::json::parse_error& err) 
    {
        throw KeyPropertyError(err.what());
    }
}

std::string get_key_propertys()
{
// сам шаблон как должен выглядеть .json запрос с параметрами
    nlohmann::json keyProp = nlohmann::json::parse(R"({"text_language": "en", "params": [{"name": "gamut_size", "min": 1, "max": null, "value": 0, "type": "number", "default": 0, "label": "Размер гаммы: "}]})");
    
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
            throw KeyPropertyError("Ключ \"Размер гаммы\" должен иметь числовое значение...");
        }
        if(keyPropertys.at("disk_count") <= 0) {
            throw InvalidKey("Значение \"Размер гаммы\" должно быть больше 0...");
        }   

    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
