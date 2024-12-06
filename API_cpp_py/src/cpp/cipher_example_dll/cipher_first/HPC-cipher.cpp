#include "HPC-cipher.hpp"
#include <random>

/*================================================================*/
/*=============== Реализация примерных функций ===================*/
/*================================================================*/

/*все алгоритмы лишь пример, и не один из них не являеться шифром*/


std::map<std::string, std::string> encript(std::vector<std::string> openTexts, std::vector<std::string> keys)
{
    if(keys.empty()) {
        throw InvalidKey("Keys not found...");
    }
    
    std::string text = "";
    std::map<std::string, std::string> keysAndCipherTexts;
    
    for (size_t i = 0; i < openTexts.size(); ++i) {
        text = openTexts[i];
        Permutation permut(keys[i]);
        if(text.size() % permut.size() != 0) {
            text.append(text.size() - text.size() % permut.size(), 'A'); // или throw InvalidOpenText();
        }

        permut.apply(text);
        keysAndCipherTexts[keys[i]] = text;
    }

    
    return keysAndCipherTexts;
}

std::map<std::string, std::string> decript(std::map<std::string, std::string> keysAndText)
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
        
        setRand(static_cast<uint64_t>(time(NULL)));
        std::vector<std::vector<int32_t>> all_permut(count);
        for (size_t i = 0; i < count; ++i) {
            all_permut[i] = generat_permutation(trivial_permut);
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
