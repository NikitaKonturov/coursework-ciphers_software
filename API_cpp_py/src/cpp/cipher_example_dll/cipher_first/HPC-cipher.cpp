#include "HPC-cipher.hpp"
#include <random>

/*================================================================*/
/*=============== Шифр горизонтальной перестановки ===================*/
/*================================================================*/


std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys)
{
    if(keys.empty()) {
        throw InvalidKey("Keys not found...");
    }
    
    std::wstring text = L"";
    std::map<std::wstring, std::wstring> keysAndCipherTexts;
    
    Permutation permut;
    for (size_t i = 0; i < openTexts.size(); ++i) {
        std::wstringstream tempss;
        text = openTexts[i];
        
        permut = Permutation((keys[i]));
        if(text.size() % permut.size() != 0) {
            throw InvalidOpenText("The size of the plaintext must be divided by the size of the key...");
        }
        permut.apply(text);
        tempss << permut;
        keysAndCipherTexts[tempss.str()] = text;
        tempss.clear();
    }

    
    return keysAndCipherTexts;
}

std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndText)
{
    for (auto& [key, text]: keysAndText) {
        Permutation keyPermut(key);
        if(text.size() % keyPermut.size() != 0) {
            throw InvalidKey("Lenght cipher text must be multiple of the size permutation...");
        }
        keyPermut.inverse();
        keyPermut.apply(text);
    }

    return keysAndText;
}


std::vector<std::string> gen_keys(std::string keyPropertys, size_t count)
{
    nlohmann::json prop;
    try{
        std::replace(keyPropertys.begin(), keyPropertys.end(), '\'', '\"');
        std::cout << keyPropertys << std::endl;

        prop = nlohmann::json::parse(keyPropertys);
        chekRequest(prop);
      

        int32_t permut_size = prop["permutation_size"];
        std::vector<int32_t> trivial_permut(permut_size);
        for (size_t i = 0; i < permut_size; ++i) {
            trivial_permut[i] = i + 1;
        }
        
        
        std::vector<uint8_t> entropy = get_entropy();
        std::vector<uint8_t> nonce = get_entropy();

        HMAC_DRBG gen(entropy, nonce, {'H', 'P', 'C', '-', 'c', 'i', 'p', 'h', 'e', 'r'});

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
    nlohmann::json keyProp = nlohmann::json::parse(R"({"params": [{"name": "permutation_size", "min": 1, "max": null, "value": 0, "type": "number", "default": 0, "label": "Permutation Size"}]})");
   
    return keyProp.dump();
}

void chekRequest(nlohmann::json keyPropertys)
{
    try {
        if(!keyPropertys.at("permutation_size").is_number()) {
            throw KeyPropertyError("Key permutation_size must has int value...");
        }
        if(keyPropertys["permutation_size"] <= 0) {
            throw InvalidKey("Value permutation_size must be natural...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
