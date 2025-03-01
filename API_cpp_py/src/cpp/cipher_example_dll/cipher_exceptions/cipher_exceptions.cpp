#include "cipher_exceptions.hpp"



/*==============================================================================*/
/*============================= Не валидный ключ ===============================*/
/*==============================================================================*/

InvalidKey::InvalidKey() noexcept : std::exception()
{
    this->message = L"Key property error";
}

InvalidKey::InvalidKey(const wchar_t* mes) noexcept
{
    this->message = mes;
}

InvalidKey::InvalidKey(const InvalidKey& source) noexcept 
{
    this->message = source.message;
}

const wchar_t *InvalidKey::what() const noexcept
{
    return this->message.c_str();
}


/*==============================================================================*/
/*======================== Не валидные параметры ключа =========================*/
/*==============================================================================*/

KeyPropertyError::KeyPropertyError() noexcept : std::exception()
{
    this->message = L"Key property error";
}

KeyPropertyError::KeyPropertyError(const wchar_t* mes) noexcept 
{
    this->message = mes;
}

KeyPropertyError::KeyPropertyError(const KeyPropertyError& source) noexcept : std::exception() 
{
    this->message = source.message;
}

const wchar_t *KeyPropertyError::what() const noexcept
{
    return this->message.c_str();
}


/*===============================================================================*/
/*========================= Не валидный открытый текст ==========================*/
/*===============================================================================*/


InvalidOpenText::InvalidOpenText() noexcept
{
    this->message = L"Key property error";
}

InvalidOpenText::InvalidOpenText(const wchar_t* mes) noexcept
{
    this->message = mes;
}

InvalidOpenText::InvalidOpenText(const InvalidOpenText& source) noexcept  
{
    this->message = source.message;
}

const wchar_t *InvalidOpenText::what() const noexcept
{
    return this->message.c_str();
}
