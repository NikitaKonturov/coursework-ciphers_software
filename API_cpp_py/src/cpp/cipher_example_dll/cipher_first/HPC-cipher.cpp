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

// конвертация строки в формат .json для удобства работы, крайне рекомендую для использования
    nlohmann::json prop = nlohmann::json::parse(keyPropertys);
    
    return std::vector<std::string>(count, prop["params"].dump(1));
}

std::string get_key_propertys()
{
// сам шаблон как должен выглядеть .json запрос с параметрами
    nlohmann::json keyProp = nlohmann::json::parse(R"({"params": [{"name": "permutation_size", "min": 1, "max": null "type": "number", "default": 0, "label": "Permutation Size"}]})");
   
    return keyProp.dump();
}
