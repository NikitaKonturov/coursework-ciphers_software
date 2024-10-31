#ifndef HPC_CIPHER
#define HPC_CIPHER
#include "../include/nlohmann/json.hpp"
#include <vector>
#include <map>
#include <regex>
#include <string>
#include <exception>
#include <iostream>

// функция зашифрования о.т., сигнатура и название должны строго соблюдаться, 
// возвращает в качестве значения std::map где ключ для map это ключ зашифрования о.т. а сам ш.т. это значение map
// openText вектор о.т., keys вектор ключей зашифрование 
std::map<std::string, std::string> encript(std::vector<std::string> openTexts, std::vector<std::string> keys);
// функция рашифрование ш.т., сигнатура и название должны строго соблюдаться,
// возвращает в качестве значение std::map где ключ map ключ расшифрование ш.т. а сам о.т. значение map
// ketsAndCipherText map где ключ для map это ключ расшифрования ш.т. значение сам ш.т.
std::map<std::string, std::string> decript(std::map<std::string, std::string> keysAndCipherText);
// функция генерации ключей по параметрам ключа, возвращает вектор строк, сигнатура и названия строго  соблюдаются
// keyPropertys строка в которой записаны параметры в формате .json, должны быть заполнены все поля влияющие на генерацию ключа
// count числовой параметр обозначающий количество ключей 
std::vector<std::string> gen_keys(std::string keyPropertys, size_t count);
// фунция возвращающая шаблон заапроса на шифрование в формате .json, также записанный в строку  
std::string get_key_propertys();
// функция проверки строку на корректное шаблон
void chekRequest(nlohmann::json keyPropertys);


class KeyPropertyError : public std::exception  
{
protected:
    std::string message;
public:
    /*============== Конструкторы ==============*/
    KeyPropertyError() noexcept;
    KeyPropertyError(const char* mes) noexcept;
    KeyPropertyError(const KeyPropertyError& source) noexcept;
    /*============== Деструктор ===============*/
    ~KeyPropertyError() noexcept = default;
    /*============ Переопределение ============*/
    inline const char* what() const noexcept override;
};


class InvalidKey : public std::exception  
{
protected:
    std::string message;
public:
    /*============== Конструкторы ==============*/
    InvalidKey() noexcept;
    InvalidKey(const char* mes) noexcept;
    InvalidKey(const InvalidKey& source) noexcept;
    /*============== Деструктор ===============*/
    ~InvalidKey() noexcept = default;
    /*============ Переопределение ============*/
    inline const char* what() const noexcept override;
};



#endif //HPC_CIPHER
