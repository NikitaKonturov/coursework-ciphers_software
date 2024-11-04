#ifndef CIPHER_EXCEPTIONS
#define CIPHER_EXCEPTIONS
#include <exception>
#include <iostream>
#include <string>


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




#endif // CIPHER_EXCEPTIONS