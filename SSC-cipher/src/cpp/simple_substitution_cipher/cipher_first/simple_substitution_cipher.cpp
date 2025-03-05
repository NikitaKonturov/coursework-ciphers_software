#include "simple_substitution_cipher.hpp"
#include <random>

/*================================================================*/
/*===================== Шифр простой замены ======================*/
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

std::map<wchar_t, wchar_t> get_alfabet_substitution(Permutation& permut, std::string language)
{
    std::wstring ru_alfabet;
    std::wstring en_alfabet;

    for (size_t i = 1040; i < 1072; ++i){
        ru_alfabet.push_back(static_cast<wchar_t>(i));
    }
    
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
    
    if(language == "ru" && permut.size() == ru_alfabet.size()) {
        std::wstring temp = ru_alfabet;
        permut.apply(temp);
        for (size_t i = 0; i < permut.size(); ++i) {
            alfabetSubstitution[ru_alfabet[i]] = temp[i];
        }
        return alfabetSubstitution;
    }

    throw std::invalid_argument("Размер перестановки не был равен размеру алфавита...");
}

std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys)
{
    std::locale::global(std::locale("ru_RU.UTF-8")); 
    std::wcout.imbue(std::locale()); 
    if(keys.size() < openTexts.size()) {
        throw InvalidKey("Количество ключей должно быть равным, если только количество открытого текста...");
    }
    
    std::map<std::wstring, std::wstring> keysAndCiphersTexts;

    for (size_t i = 0; i < keys.size(); ++i) {
        Permutation key(keys[i]);
        std::map<wchar_t, wchar_t> substitution = get_alfabet_substitution(key, define_language(openTexts[i]));
        std::wstring cipherText = openTexts[i];
        std::wcout << cipherText << std::endl;
        for(wchar_t& symbol: cipherText) {
            symbol = substitution[symbol];
        }
        keysAndCiphersTexts[keys[i]] = cipherText;
    }
    
    return keysAndCiphersTexts;
}

std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndCipherTexts)
{
    std::map<std::wstring, std::wstring> keysAndOpenTexts;
    for(auto& keyAndCipherText : keysAndCipherTexts) {
        Permutation key(keyAndCipherText.first);
        key.inverse();
        std::map<wchar_t, wchar_t> substitution = get_alfabet_substitution(key, define_language(keyAndCipherText.second));
        std::wstring openText = keyAndCipherText.second;
        for (size_t i = 0; i < openText.size(); ++i) {
            openText[i] = substitution[openText[i]];
        }
        
        std::wcout << "Open text: " << openText << std::endl;

        keysAndOpenTexts[keyAndCipherText.first] = openText;
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
      
        int32_t permut_size = (prop["text_language"] == "ru" ? 32 : 26);
        std::vector<int32_t> trivial_permut(permut_size);
        for (size_t i = 0; i < permut_size; ++i) {
            trivial_permut[i] = i + 1;
        }
        
        std::vector<uint8_t> entropy = get_entropy();
        std::vector<uint8_t> nonce = get_entropy();

        HMAC_DRBG gen(entropy, nonce, {'S', 'S', 'C', '-', 'c', 'i', 'p', 'h', 'e', 'r'});
        std::vector<std::vector<int32_t>> all_permut(count);
        for (size_t i = 0; i < count; ++i) {
            all_permut[i] = generat_permutation(trivial_permut, gen);
        }

        std::vector<std::string> result;
        for (auto permut: all_permut) {
            std::ostringstream oss;
            std::copy(permut.begin(), permut.end(), std::ostream_iterator<int32_t>(oss, " "));
            result.push_back(oss.str().substr(0, oss.str().size() - 1));
        }
        
        return result;
    } catch(nlohmann::json::parse_error &err) {
        throw KeyPropertyError(err.what());
    }
}

std::string get_key_propertys()
{
// сам шаблон как должен выглядеть .json запрос с параметрами
    nlohmann::json keyProp = nlohmann::json::parse(R"({"params": []})");
   
    return keyProp.dump();
}

void chekRequest(nlohmann::json keyPropertys)
{
    try {
        if(!keyPropertys.at("text_language").is_string()) {
            throw KeyPropertyError("Ключ text_language должен иметь строковое значение...");
        }
        if(keyPropertys["text_language"] != "ru" && keyPropertys["text_language"] != "en") {
            throw InvalidKey("Значение permutation_size должно быть ru или en...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
