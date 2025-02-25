#include "ADFGVX_cipher.hpp"
#include <random>
#include <algorithm>

/*================================================================*/
/*======================== Шифр ADFGVX =========================*/
/*================================================================*/

std::wstring get_trivial_completion(){
    return L"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
}

std::map<wchar_t, wchar_t> get_alfabet_substitution(Permutation& permut, std::string language)
{
    std::wstring en_alfabet;
    
    for (size_t i = 65; i < 91; ++i){
        en_alfabet.push_back(static_cast<wchar_t>(i));
    }

    std::map<wchar_t, wchar_t> alfabetSubstitution; 

    if(language == "en" && permut.size() == en_alfabet.size()) {
        std::wstring temp = en_alfabet;
        permut.apply(temp);
        for (size_t i = 0; i < en_alfabet.size(); ++i) {
            alfabetSubstitution[en_alfabet[i]] = temp[i];
        }
        return alfabetSubstitution;
    }
    
    if(language == "ru") {
        throw InvalidOpenText("Ошибка: недопустимый язык в тексте. Язык должен быть английский...");
    }

    throw std::invalid_argument("Permutation size was not equal to alfabet size...");
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
    if (has_ru) throw InvalidOpenText("Ошибка: недопустимый язык в тексте. Язык должен быть английский...");
    throw InvalidOpenText("Ошибка: недопустимый язык в тексте.");
}

static const std::wstring adfgvx = L"ADFGVX";

std::pair<wchar_t, wchar_t> get_cipher_bigram(wchar_t plainCh, const std::wstring& substitutionTable) {
    size_t pos = substitutionTable.find(plainCh);
    if(pos == std::wstring::npos) {
        throw InvalidOpenText("В открытом тексте обнаружен недопустимый символ.");
    }
    size_t row = pos / 6;
    size_t col = pos % 6;
    
    return std::make_pair(adfgvx[row], adfgvx[col]);
}

wchar_t get_revers_cipher_bigram(wchar_t firstCh, wchar_t secondCh, const std::wstring& substitutionTable) {
    size_t row = adfgvx.find(firstCh);
    size_t col = adfgvx.find(secondCh);
    if(row == std::wstring::npos || col == std::wstring::npos) {
        throw InvalidOpenText("Недопустимые символы ADFGVX для обратной замены.");
    }

    size_t pos = row * 6 + col;
    if(pos >= substitutionTable.size()) {
        throw InvalidOpenText("Неправильная позиция в таблице замен.");
    }
    return substitutionTable[pos];
}

std::wstring key_conversions(const std::wstring &key) {
    std::wstringstream wss;

    for (size_t i = 0; i < key.size(); ++i) {
        if(i % 6 == 0)
            wss << L"[";
        wss << key[i];
        if((i + 1) % 6 == 0)
            wss << L"]\n";
    }

    return wss.str();
}

std::wstring parse_substitution_table(const std::wstring& keyStr) {
    std::wstringstream ss(keyStr);
    std::wstring line;
    std::wstring table;
    while (std::getline(ss, line)) {
        if (!line.empty() && line.front() == L'[' && line.back() == L']') {
            std::wstring content = line.substr(1, line.size() - 2);
            content.erase(std::remove(content.begin(), content.end(), L' '), content.end());
            table += content;
        }
    }
    return table;
}

std::wstring parse_transposition_key(const std::wstring& keyStr) {
    std::wstring delimiter = L"The transpositional key: ";
    size_t pos = keyStr.find(delimiter);
    if (pos == std::wstring::npos)
        throw InvalidKey("Транспозиционный ключ не найден в строке ключа.");
    std::wstring transKey = keyStr.substr(pos + delimiter.size());
    while (!transKey.empty() && std::isspace(transKey.front()))
        transKey.erase(transKey.begin());
    while (!transKey.empty() && std::isspace(transKey.back()))
        transKey.pop_back();
    return transKey;
}

/*======================== Структура для хранения ключей ADFGVX =========================*/
struct ADFGVXKeys {
    std::wstring substitutionKeyData;
    std::wstring transpositionKey;
};
/*=======================================================================================*/

std::vector<ADFGVXKeys> parse_keys(const std::vector<std::wstring>& keysV) {
    std::vector<ADFGVXKeys> keys;
    for (const auto& line : keysV) {
        if (line.empty()) {
            continue;
        }

        size_t delimPos = line.find(L'|');
        if (delimPos == std::wstring::npos) {
            throw InvalidKey("Неверный формат ключа. Разделитель '|' ожидается.");
        }

        ADFGVXKeys key;
        key.substitutionKeyData = line.substr(0, delimPos);
        key.transpositionKey = line.substr(delimPos + 1);
        keys.push_back(key);
    }
    return keys;
}

std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys)
{
    if(keys.size() < openTexts.size()) {
        throw InvalidKey("Количество ключей должно быть равно количеству открытого текста...");
    }
    auto keysE = parse_keys(keys);

    std::map<std::wstring, std::wstring> keysAndCiphersTexts;
    
    for (size_t i = 0; i < openTexts.size(); ++i) {
        std::wstring text = openTexts[i];
        if(define_language(text) != "en") {
            throw InvalidOpenText("Недопустимый язык, должен быть...");
        }

        std::wstring substitutionTable = get_trivial_completion();
        Permutation key_permutation(keysE[i].substitutionKeyData);
        key_permutation.apply(substitutionTable);
        
        std::wstring intermediate;
        for (wchar_t ch : text) {
            auto pair = get_cipher_bigram(ch, substitutionTable);
            intermediate.push_back(pair.first);
            intermediate.push_back(pair.second);
        }
        
        Permutation transpositionKey(keysE[i].transpositionKey);
        std::map<wchar_t, wchar_t> substitution = get_alfabet_substitution(transpositionKey, define_language(openTexts[i]));
        std::wstring cipherText = openTexts[i];
        std::wcout << cipherText << std::endl;
        for(wchar_t& symbol : cipherText) {
            symbol = substitution[symbol];
        }
        
        std::wstringstream wss;
        wss << key_conversions(substitutionTable)
            << L"The transpositional key: " << keysE[i].transpositionKey;
        keysAndCiphersTexts[wss.str()] = cipherText;
    }
    
    return keysAndCiphersTexts;
}

std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndCipherTexts)
{
    std::map<std::wstring, std::wstring> keysAndOpenTexts;
    
    for (auto& pair : keysAndCipherTexts) {

        if(define_language(pair.second) != "en") {
            throw InvalidOpenText("Неверный язык, должен быть английский...");
        }
        std::wstring cipherText = pair.second;
        std::wstring keyStr = pair.first;

        std::wstring substitutionTable = parse_substitution_table(keyStr);
        std::wstring transpositionKey = parse_transposition_key(keyStr);
        
        if (substitutionTable.empty()) {
            throw InvalidKey("Извлечена пустая таблица подстановки.");
        }
        
        Permutation key(transpositionKey);
        key.inverse();
        std::map<wchar_t, wchar_t> substitution = get_alfabet_substitution(key, define_language(cipherText));
        std::wstring openText = cipherText;
        for(wchar_t& symbol: openText) {
            symbol = substitution[symbol];
        }
        
        if (openText.size() % 2 != 0) {
            throw InvalidOpenText("Промежуточный текст имеет нечетную длину.");
        }

        std::wstring text;
        Permutation key_permutation(substitutionTable);
        std::wstring printedSubstitution = get_trivial_completion();
        key_permutation.apply(printedSubstitution);
        

        for (size_t i = 0; i < openText.size(); i += 2) {
            wchar_t plainCh = get_revers_cipher_bigram(openText[i], openText[i+1], substitutionTable);
            text.push_back(plainCh);
        }
        
        keysAndOpenTexts[keyStr] = text;
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
      
        std::vector<int32_t> trivialSub(36);
        for (size_t i = 0; i < 36; ++i) {
            trivialSub[i] = static_cast<int32_t>(i + 1);
        }
        
        std::vector<int32_t> trivialLetters(26);
        for (size_t i = 0; i < 26; ++i) {
            trivialLetters[i] = static_cast<int32_t>(i);
        }
        
        std::vector<uint8_t> entropy = get_entropy();
        std::vector<uint8_t> nonce = get_entropy();
        
        HMAC_DRBG gen(entropy, nonce, { 'A','D','F','G','V','X','-','c','i','p','h','e','r' });
        
        std::vector<std::vector<int32_t>> substitutionKeys(count);
        for (size_t i = 0; i < count; ++i) {
            substitutionKeys[i] = generat_permutation(trivialSub, gen);
        }
        
        std::vector<std::vector<int32_t>> transpositionKeys(count);
        for (size_t i = 0; i < count; ++i) {
            transpositionKeys[i] = generat_permutation(trivialLetters, gen);
        }
        
        std::vector<std::string> result;
        for (size_t i = 0; i < count; ++i) {
            std::ostringstream subStream;
            for (int num : substitutionKeys[i]) {
                subStream << num << " ";
            }
            std::string subKey = subStream.str();
            if (!subKey.empty()) {
                subKey.pop_back();
            }
            
            std::string transKey;
            for (auto permut: transpositionKeys) {
                std::ostringstream oss;
                std::copy(permut.begin(), permut.end(), std::ostream_iterator<int32_t>(oss, " "));
                transKey = oss.str().substr(0, oss.str().size() - 1);
            }    
            
            std::string combined = subKey + "|" + transKey;
            result.push_back(combined);
        }
        
        return result;
        
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
            throw KeyPropertyError("Key text_language must has string value...");
        }
        if(keyPropertys["text_language"] != "ru" && keyPropertys["text_language"] != "en") {
            throw InvalidKey("Значение permutation_size должно быть ru или en...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
