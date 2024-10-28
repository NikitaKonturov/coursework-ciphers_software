#include "HPC-cipher.hpp"

/*================================================================*/
/*=============== Реализация примерных функций ===================*/
/*================================================================*/
/*все алгоритмы лишь пример, и не один из них не являеться шифром*/




std::map<std::string, std::string> encript(std::vector<std::string> openTexts, std::vector<std::string> keys)
{
    std::map<std::string, std::string> result;
    for (size_t i = 0; i < openTexts.size(); ++i) {
        result[openTexts[i]] = keys[i];
    }
    return result;
}

std::map<std::string, std::string> decript(std::map<std::string, std::string> keysAndCipherText)
{
    for (auto &pair: keysAndCipherText) {
        pair.second = "decript_complite";
    }
    
    return keysAndCipherText;
}

std::vector<std::string> gen_keys(std::string keyPropertys, size_t count)
{
    nlohmann::json prop;
    try{
    std::replace(keyPropertys.begin(), keyPropertys.end(), '\'', '\"');
    std::cout << keyPropertys << std::endl;

    prop = nlohmann::json::parse(keyPropertys);

    chekRequest(prop);
    } catch(nlohmann::json::parse_error &err) {
        throw std::invalid_argument(err.what());
    }
    return std::vector<std::string>(count, prop["permutation_size"].dump());
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
            throw std::invalid_argument("Key permutation_size must has int value...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw std::invalid_argument(err.what());
    }
}
