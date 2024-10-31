#include "HPC-cipher.hpp"

/*================================================================*/
/*=============== Реализация примерных функций ===================*/
/*================================================================*/
/*все алгоритмы лишь пример, и не один из них не являеться шифром*/





std::map<std::string, std::string> encript(std::vector<std::string> openTexts, std::vector<std::string> keys)
{
    if(keys.empty()) {
        throw InvalidKey("Keys not found...");
    }

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
        throw KeyPropertyError(err.what());
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
            throw KeyPropertyError("Key permutation_size must has int value...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}

/*==============================================================================*/
/*============================= Не валидный ключ ===============================*/
/*==============================================================================*/

InvalidKey::InvalidKey() noexcept : std::exception()
{
    this->message = "Key property error";
}

InvalidKey::InvalidKey(const char* mes) noexcept : std::exception(mes)
{
    this->message = mes;
}

InvalidKey::InvalidKey(const InvalidKey& source) noexcept : std::exception() 
{
    this->message = source.message;
}

const char *InvalidKey::what() const noexcept
{
    return this->message.c_str();
}


/*==============================================================================*/
/*=========================== Не валидные параметры ============================*/
/*==============================================================================*/

KeyPropertyError::KeyPropertyError() noexcept : std::exception()
{
    this->message = "Key property error";
}

KeyPropertyError::KeyPropertyError(const char* mes) noexcept : std::exception(mes)
{
    this->message = mes;
}

KeyPropertyError::KeyPropertyError(const KeyPropertyError& source) noexcept : std::exception() 
{
    this->message = source.message;
}

const char *KeyPropertyError::what() const noexcept
{
    return this->message.c_str();
}
