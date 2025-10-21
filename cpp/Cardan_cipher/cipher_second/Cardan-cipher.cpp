#include "Cardan-cipher.hpp"
#include <random>

/*================================================================*/
/*=============== Реализация примерных функций ===================*/
/*================================================================*/

/*все алгоритмы лишь пример, и не один из них не являеться шифром*/

std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys)
{
    if (keys.empty()) {
        throw InvalidKey("Ключи не найдены...");
    }

    std::map<std::wstring, std::wstring> keysAndCipherTexts;

    for (size_t i = 0; i < openTexts.size(); ++i) {
        std::wstring text = openTexts[i];
        BoolMatrix matrix(keys[i]);
        matrix.check();
        std::wcout << L"Open text: " << text << std::endl;
        text = matrix.encryption(text);
        keysAndCipherTexts[keys[i]] = text;
    }

    return keysAndCipherTexts;
}


std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndText) 
{
    for (auto& [key, text] : keysAndText) {
        BoolMatrix keyMatrix(key);  
        keyMatrix.check();
        if (text.size() % keyMatrix.size() != 0) {
            throw InvalidKey("Длина зашифрованного текста должна быть кратна размеру ключа...");
        }

        std::wstring decryptedText = keyMatrix.decryption(text);
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

        // Инициализация HMAC_DRBG
        std::vector<uint8_t> entropy = get_entropy(); // Получаем энтропию
        std::vector<uint8_t> nonce = get_entropy();   // Получаем nonce
        HMAC_DRBG gen(entropy, nonce, {'C', 'A', 'R', 'D', 'A', 'N', '-', 'c', 'i', 'p', 'h', 'e', 'r'}); 

        std::vector<std::string> result;

        // Извлекаем размер матрицы
        int32_t matrix_size = prop["matrix_size"];
        if (matrix_size % 2 != 0) {
            throw InvalidKey("Размер матрицы должен быть четным.");
        }

        for (size_t key = 0; key < count; ++key) {
            // Проверяем, нужно ли выполнить повторную инициализацию
            if (gen.HMAC_DRBG_Ressed_Check()) {
                gen.HMAC_DRBG_Ressed(get_entropy());
            }

            // Создаем матрицу
            BoolMatrix matrix(matrix_size);
            generatMatrix(matrix, gen); // Используем HMAC_DRBG для генерации матрицы

            // Преобразуем матрицу в строку
            std::ostringstream oss;
            oss << matrix;
            result.push_back(oss.str());
        }

        return result; // Возвращаем сгенерированные матрицы
    } catch (nlohmann::json::parse_error& err) {
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
                "label": "Размер матрицы (Размер матрицы должен быть чётным) "
            }
        ]
    })");
    return keyProp.dump();
}

void chekRequest(nlohmann::json keyProperties) 
{
    try {
        if (!keyProperties.at("matrix_size").is_number()) {
            throw KeyPropertyError("Значение \"Размер матрицы\" должен иметь целое значение...");
        }
        if (keyProperties["matrix_size"] <= 0) {
            throw InvalidKey("Значение \"Размер матрицы\" должно быть натуральным...");
        }
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
