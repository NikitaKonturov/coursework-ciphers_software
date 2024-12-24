#ifndef CARDAN_CIPHER
#define CARDAN_CIPHER
#include "../include/nlohmann/json.hpp"
#include "../BoolMatrix/src/BoolMatrix.hpp"
#include "../cipher_exceptions/cipher_exceptions.hpp"
#include "../boolmatrix_generator/boolmatrix_generator.hpp"
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <vector>
#include <cmath>
#include <regex>
#include <map>

// функция зашифрования о.т., сигнатура и название должны строго соблюдаться, 
// возвращает в качестве значения std::map где ключ для map это ключ зашифрования о.т. а сам ш.т. это значение map
// openText вектор о.т., keys вектор ключей зашифрование 
std::map<std::string, std::string> encript(std::vector<std::string> openTexts, std::vector<std::string> keys);
// функция рашифрование ш.т., сигнатура и название должны строго соблюдаться,
// возвращает в качестве значение std::map где ключ map ключ расшифрование ш.т. а сам о.т. значение map
// ketsAndCipherText map где ключ для map это ключ расшифрования ш.т. значение сам ш.т.
std::map<std::string, std::string> decript(std::map<std::string, std::string> keysAndText);
// функция генерации ключей по параметрам ключа, возвращает вектор строк, сигнатура и названия строго  соблюдаются
// keyPropertys строка в которой записаны параметры в формате .json, должны быть заполнены все поля влияющие на генерацию ключа
// count числовой параметр обозначающий количество ключей 
std::vector<std::string> gen_keys(std::string keyPropertys, size_t count);
// фунция возвращающая шаблон заапроса на шифрование в формате .json, также записанный в строку  
std::string get_key_propertys();
// функция проверки строку на корректное шаблон
void chekRequest(nlohmann::json keyPropertys);


#endif //CARDAN_CIPHER
