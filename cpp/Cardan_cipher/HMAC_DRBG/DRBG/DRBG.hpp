#ifndef DRBG_HPP
#define DRBG_HPP
#define RESEED_NUMBER 281474976710656
#include <vector>
#include <optional>
#include "../HMAC/HMAC.hpp"


class HMAC_DRBG
{
 private:
    std::vector<uint8_t> key;
    std::vector<uint8_t> value;
    size_t reseedCounter;

 public:
    // Конструктор содания экземпляра генератора, определение начальных значений полей key и value  
    HMAC_DRBG(std::vector<uint8_t> entropyInput, std::vector<uint8_t> nonce, std::vector<uint8_t> personalizationString = {});
    HMAC_DRBG(const HMAC_DRBG& source);
    // Деструктор, очистка полей, в целях безопасности
    ~HMAC_DRBG();
    // Обновление значений полей key, value и reseedCounter
    void HMAC_DRBG_Update(std::vector<uint8_t> providedData);
    // Перезагрузка новых данных в генератор, повтороное создание экземпляра
    void HMAC_DRBG_Ressed(std::vector<uint8_t> entropyInput, std::vector<uint8_t> aditionInput = {});
    // Проверка на потребность перезапуска генератора
    bool HMAC_DRBG_Ressed_Check() const;
    // Получение псевдослучайной последовательности байт 
    std::optional<std::vector<uint8_t>> HMAC_DRBG_Generate_algorithm(size_t byteNumber, std::vector<uint8_t> aditionInput = {});
};

//Выбираются первые 64 бита из предоставленных 
uint64_t convert_bytes_to_ddword(std::vector<uint8_t> bytes);

// Получение энтропии
std::vector<uint8_t> get_entropy();


#endif // DRBG_HPP
