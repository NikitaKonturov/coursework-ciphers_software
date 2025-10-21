#include "Hill-cipher.hpp"
#include <random>

/*================================================================*/
/*========================= Шифр Хила ============================*/
/*================================================================*/


std::string define_language(std::wstring text) {
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
    throw InvalidOpenText("Ошибка: неверный язык в тексте.");
}

Matrix get_cipher_block(std::wstring block, std::string lang) {
    std::vector<int64_t> cipher_block;
    int64_t alfabetOffset = (lang == "ru" ? 1040 : 65);

    for (size_t i = 0; i < block.size(); ++i) {
        cipher_block.push_back(static_cast<int64_t>(block[i]) - alfabetOffset);
        std::cout << static_cast<int64_t>(block[i]) - alfabetOffset << " ";
    }
    std::cout << std::endl;
    
    Matrix res(std::vector<std::vector<int64_t>>{cipher_block});
    res.transpose();
    std::cout << res << std::endl;
    return res;
}

std::wstring get_string_cipher_block(double* intCipherBlock, size_t lenght, std::string lang) {
    size_t alfabetSize = (lang == "ru" ? 32 : 26);
    size_t alfabetOffset = (lang == "ru" ? 1040 : 65);
    std::wstring res;

    for (size_t i = 0; i < lenght; ++i) {
        int64_t rounded_value = static_cast<int64_t>(std::round(intCipherBlock[i])); // Округление
        int64_t mod_value = ((rounded_value % alfabetSize) + alfabetSize) % alfabetSize; // Коррекция по модулю
        res.push_back(static_cast<wchar_t>(alfabetOffset + mod_value));
    }

    return res;
}


std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys)
{
    if(keys.empty()) {
        throw InvalidKey("Ключи не найдены...");
    }
    
    if(openTexts.size() > keys.size()) {
        throw InvalidKey("Колличство ключей должно быть больше либо равно колличеству открытых текстов...");
    }
    
    std::map<std::wstring, std::wstring> keysAndCipherTexts;

    for (size_t i = 0; i < keys.size(); ++i) {
        Matrix keyMtrx(keys[i]);        
        if(keyMtrx.get_columns() != keyMtrx.get_lines() || openTexts[i].size() % keyMtrx.get_columns() != 0) {
            throw InvalidKey("Размер открытого текста должен быть кратен размерности матрицы ключа...");
        }
        std::string lang = define_language(openTexts[i]);
        
        size_t blockCount = openTexts[i].size() / keyMtrx.get_columns();

        std::wstring cipherText;
        for (size_t j = 0; j < blockCount; ++j) {
            std::wstring temp = openTexts[i].substr((j*keyMtrx.get_columns()), keyMtrx.get_columns());
            Matrix intCipherBlock = keyMtrx * get_cipher_block(temp, lang);
            intCipherBlock.transpose();
            std::cout << intCipherBlock << std::endl;
            cipherText.append(get_string_cipher_block(intCipherBlock[0], intCipherBlock.get_columns(), lang));
        }
        
        keysAndCipherTexts[keys[i]] = cipherText;
    }
    
    return keysAndCipherTexts;
}

std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndText)
{
    std::map<std::wstring, std::wstring> keysAndOpenText;
    for (const auto &[key, text]: keysAndText) {
        Matrix keyMtrx(key);
        std::cout << "Key mtrx: " << keyMtrx << std::endl;
        std::string lang = define_language(text);
        keyMtrx = keyMtrx.inverse((lang == "ru" ? 32 : 26));
        std::cout << "Inverse mtrx: " << keyMtrx << std::endl;

        if(keyMtrx.get_columns() != keyMtrx.get_lines() || text.size() % keyMtrx.get_columns() != 0) {
            throw InvalidKey("Размер открытого текста должен быть кратен размерности матрицы ключа...");
        }
        
        size_t blockCount = text.size() / keyMtrx.get_columns();

        std::wstring openText;
        for (size_t j = 0; j < blockCount; ++j) {
            Matrix intCipherBlock = keyMtrx * get_cipher_block(text.substr(j*keyMtrx.get_columns(), keyMtrx.get_columns()), lang);
            intCipherBlock.transpose();
            std::cout << "Open int block: " << intCipherBlock << std::endl;
            openText.append(get_string_cipher_block(intCipherBlock[0], intCipherBlock.get_columns(), lang));
        }
        
        keysAndOpenText[key] = openText;
    }

    return keysAndOpenText;
}

std::vector<std::string> gen_keys(std::string keyPropertys, size_t count)
{
    nlohmann::json prop;
    try{
        std::replace(keyPropertys.begin(), keyPropertys.end(), '\'', '\"');
        std::cout << keyPropertys << std::endl;

        prop = nlohmann::json::parse(keyPropertys);
        chekRequest(prop);
        
        uint8_t upperLimit = (prop["text_language"] == "en" ? 26 : 32);
        uint32_t matrixSize = prop["matrix_size"];

        std::vector<std::string> keys(count);
        for (size_t i = 0; i < count; ++i){
            Matrix mtrx(matrixSize, matrixSize, upperLimit);
            mtrx.matrix_random(0, upperLimit, upperLimit);
            std::stringstream ss;
            ss << mtrx;
            keys[i] = ss.str();
        }

        return keys;
    } catch(nlohmann::json::parse_error &err) {
        throw KeyPropertyError(err.what());
    }
}

std::string get_key_propertys()
{
// сам шаблон как должен выглядеть .json запрос с параметрами
    nlohmann::json keyProp = nlohmann::json::parse(R"({"params": [{"name": "matrix_size", "min": 1, "max": null, "value": 0, "type": "number", "default": 0, "label": "Размер матрицы: "}]})");
   
    return keyProp.dump();
}

void chekRequest(nlohmann::json keyPropertys)
{
    try {
        if(!keyPropertys.at("matrix_size").is_number()) {
            throw KeyPropertyError("Поле permutation_size должно иметь целочисленное значение...");
        }
        if(keyPropertys["matrix_size"] <= 0) {
            throw InvalidKey("Значение permutation_size должно быть натуральным...");
        }
        if(!keyPropertys.at("text_language").is_string()) {
            throw KeyPropertyError("Поле text_language must должно иметь тип строка...");
        }
        if(keyPropertys["text_language"] != "ru" && keyPropertys["text_language"] != "en") {
            throw InvalidKey("Значение permutation_size должно быть ru или en...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
