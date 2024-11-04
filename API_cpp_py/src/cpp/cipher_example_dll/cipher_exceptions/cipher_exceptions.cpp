#include "cipher_exceptions.hpp"



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
