#include "simple_substitution_cipher.hpp"
#include <random>

/*================================================================*/
/*===================== Шифр простой замены ======================*/
/*================================================================*/

std::string define_language(std::wstring text)
{
    std::wregex ru_template(L"[а-яА-Я]");
    std::wregex en_template(L"[a-zA-Z]");

    if(std::regex_search(text, ru_template) && !std::regex_search(text, en_template)) {
        return "ru";
    }
    if(std::regex_search(text, en_template) && !std::regex_search(text, ru_template)) {
        return "en";
    }
    throw InvalidOpenText("Error invalid languge in text...");
}

std::map<wchar_t, wchar_t> get_alfabet_substitution(Permutation& permut, std::string language)
{
    std::wstring ru_alfabet = L"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    std::wstring en_alfabet = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

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
        for (size_t i = 0; i < ru_alfabet.size(); ++i) {
            alfabetSubstitution[ru_alfabet[i]] = temp[i];
        }
        return alfabetSubstitution;
    }

    throw std::invalid_argument("Permutation size was not equal to alfabet size...");
}


std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys)
{

    if(keys.size() < openTexts.size()) {
        throw InvalidKey("Count of keys must be unless then count of open text...");
    }
    
    std::map<std::wstring, std::wstring> keysAndCiphersTexts;

    for (size_t i = 0; i < keys.size(); ++i) {
        Permutation key(keys[i]);
        std::map<wchar_t, wchar_t> substitution = get_alfabet_substitution(key, define_language(openTexts[i]));
        std::wstring cipherText = openTexts[i];
        for(wchar_t& symbol: cipherText) {
            symbol = substitution[symbol];
        }
        keys[i].insert(0, L"[");
        keys[i].push_back(L']');
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
        for(wchar_t& symbol: openText) {
            symbol = substitution[symbol];
        }
        keysAndOpenTexts[keyAndCipherText.first] = openText;
    }

    return keysAndCipherTexts;
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
            throw KeyPropertyError("Key text_language must has string value...");
        }
        if(keyPropertys["text_language"] != "ru" && keyPropertys["text_language"] != "en") {
            throw InvalidKey("Value permutation_size must be ru or en...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
