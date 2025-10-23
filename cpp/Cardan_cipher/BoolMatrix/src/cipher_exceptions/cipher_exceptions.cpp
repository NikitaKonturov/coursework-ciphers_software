#include "cipher_exceptions.hpp"



/*==============================================================================*/
/*============================= Не валидный ключ ===============================*/
/*==============================================================================*/

InvalidKey::InvalidKey() noexcept : std::runtime_error("")
{
    this->message = "Ошибка в свойстве ключа";
}

InvalidKey::InvalidKey(const char* mes) noexcept : std::runtime_error(mes)
{
    this->message = mes;
}

InvalidKey::InvalidKey(const InvalidKey& source) noexcept : std::runtime_error("") 
{
    this->message = source.message;
}

const char *InvalidKey::what() const noexcept
{
    return this->message.c_str();
}


/*==============================================================================*/
/*======================== Не валидные параметры ключа =========================*/
/*==============================================================================*/

KeyPropertyError::KeyPropertyError() noexcept : std::runtime_error("")
{
    this->message = "Ошибка в свойстве ключа";
}

KeyPropertyError::KeyPropertyError(const char* mes) noexcept : std::runtime_error(mes)
{
    this->message = mes;
}

KeyPropertyError::KeyPropertyError(const KeyPropertyError& source) noexcept : std::runtime_error("") 
{
    this->message = source.message;
}

const char *KeyPropertyError::what() const noexcept
{
    return this->message.c_str();
}


/*===============================================================================*/
/*========================= Не валидный открытый текст ==========================*/
/*===============================================================================*/


InvalidOpenText::InvalidOpenText() noexcept : std::runtime_error("")
{
    this->message = "Ошибка в свойстве ключа";
}

InvalidOpenText::InvalidOpenText(const char* mes) noexcept : std::runtime_error(mes)
{
    this->message = mes;
}

InvalidOpenText::InvalidOpenText(const InvalidOpenText& source) noexcept : std::runtime_error("") 
{
    this->message = source.message;
}

const char *InvalidOpenText::what() const noexcept
{
    return this->message.c_str();
}