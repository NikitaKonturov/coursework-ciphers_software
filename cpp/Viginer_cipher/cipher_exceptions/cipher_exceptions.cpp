#include "cipher_exceptions.hpp"



/*==============================================================================*/
/*============================= Не валидный ключ ===============================*/
/*==============================================================================*/

InvalidKey::InvalidKey() noexcept : std::exception()
{
    this->message = "Ошибка в свойстве ключа";
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
/*======================== Не валидные параметры ключа =========================*/
/*==============================================================================*/

KeyPropertyError::KeyPropertyError() noexcept : std::exception()
{
    this->message = "Ошибка в свойстве ключа";
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


/*===============================================================================*/
/*========================= Не валидный открытый текст ==========================*/
/*===============================================================================*/


InvalidOpenText::InvalidOpenText() noexcept : std::exception()
{
    this->message = "Ошибка в свойстве ключа";
}

InvalidOpenText::InvalidOpenText(const char* mes) noexcept : std::exception(mes)
{
    this->message = mes;
}

InvalidOpenText::InvalidOpenText(const InvalidOpenText& source) noexcept : std::exception() 
{
    this->message = source.message;
}

const char *InvalidOpenText::what() const noexcept
{
    return this->message.c_str();
}