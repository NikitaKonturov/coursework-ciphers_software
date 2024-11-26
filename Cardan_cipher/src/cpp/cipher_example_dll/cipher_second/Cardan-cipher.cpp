#include "Cardan-cipher.hpp"
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
        BoolMatrix matrix(keys[i]);
        if (text.size() % matrix.size() != 0) {
            size_t pad_length = matrix.size() - text.size() % matrix.size();
            text.append(pad_length, 'A'); 
        }


        matrix.encryption(text);
        keysAndCipherTexts[keys[i]] = text;
    }

    
    return keysAndCipherTexts;
}

std::map<std::string, std::string> decript(std::map<std::string, std::string> keysAndText) 
{
    for (auto& [key, text] : keysAndText) {
        BoolMatrix keyMatrix(key);  

        if (text.size() % keyMatrix.size() != 0) {
            throw InvalidKey("Length of cipher text must be multiple of the key size...");
        }

        std::string decryptedText = keyMatrix.decryption(text);
        text = decryptedText;
    }

    return keysAndText;
}


std::vector<std::string> gen_keys(std::string keyProperties, size_t count)
{
    nlohmann::json prop;
    try {
        std::replace(keyProperties.begin(), keyProperties.end(), '\'', '\"');
        std::cout << keyProperties << std::endl;

        prop = nlohmann::json::parse(keyProperties);
        chekRequest(prop);

        // Извлекаем размер матрицы
        int32_t matrix_size = prop["matrix_size"];
        if (matrix_size % 2 != 0) {
            throw InvalidKey("Matrix size must be even.");
        }

        std::minstd_rand generator(static_cast<uint64_t>(time(NULL)));

        std::vector<std::string> result;
        for (size_t i = 0; i < count; ++i) {
            BoolMatrix matrix(matrix_size);
            generatMatrix(matrix, generator);
            std::ostringstream oss;
            oss << matrix;
            result.push_back(oss.str());
        }

        return result;                              // Пример вывода функции:"1000111100000101" — это строковое представление 4x4 матрицы с элементами 0 и 1.
    } catch (nlohmann::json::parse_error &err) {
        throw KeyPropertyError(err.what());
    }
}



// сам шаблон как должен выглядеть .json запрос с параметрами
std::string get_key_propertys() 
{
    nlohmann::json keyProp = nlohmann::json::parse(R"({
        "params": [
            {
                "name": "matrix_size",
                "min": 2,
                "max": null,
                "value": 0,
                "type": "number",
                "default": 4,
                "label": "Matrix Size"
            }
        ]
    })");
    return keyProp.dump();
}

void chekRequest(nlohmann::json keyProperties) 
{
    try {
        if (!keyProperties.at("matrix_size").is_number()) {
            throw KeyPropertyError("Key matrix_size must have an int value...");
        }
        if (keyProperties["matrix_size"] <= 0) {
            throw InvalidKey("Value matrix_size must be natural...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
