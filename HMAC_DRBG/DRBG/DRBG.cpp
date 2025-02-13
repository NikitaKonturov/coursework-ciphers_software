#include "DRBG.hpp"


                            /*=====================================================================*/
                            /*== Реализация генератора псевдослучайной последовательности байтов ==*/
                            /*=====================================================================*/
                        
HMAC_DRBG::HMAC_DRBG(std::vector<uint8_t> entropyInput, 
                     std::vector<uint8_t> nonce, 
                     std::vector<uint8_t> personalizationString) : key(std::vector<uint8_t>(32, 0x00)), 
                                                                   value(std::vector<uint8_t>(32, 0xff)),
                                                                   reseedCounter(0)
{
    this->HMAC_DRBG_Update(concatination(concatination(entropyInput, nonce), personalizationString));
}

HMAC_DRBG::~HMAC_DRBG()
{
    this->reseedCounter = 0;
    this->key.clear();
    this->value.clear();
}

void HMAC_DRBG::HMAC_DRBG_Update(std::vector<uint8_t> providedData)
{
    this->key = convert_to_bytes(hash_message_authentication_code(this->key, concatination(concatination(this->value, {0x00}), providedData)));
    this->value =convert_to_bytes(hash_message_authentication_code(this->key, this->value));

    if(providedData.size() != 0) {
        this->key = convert_to_bytes(hash_message_authentication_code(this->key, concatination(concatination(this->value, {0x01}), providedData)));
        this->value =convert_to_bytes(hash_message_authentication_code(this->key, this->value));
    }
    return;
}

void HMAC_DRBG::HMAC_DRBG_Ressed(std::vector<uint8_t> entropyInput, std::vector<uint8_t> aditionInput)
{
    this->HMAC_DRBG_Update(concatination(entropyInput, aditionInput));
    this->reseedCounter = 1;
    return;
}

std::optional<std::vector<uint8_t>> HMAC_DRBG::HMAC_DRBG_Generate_algorithm(size_t byteNumber, std::vector<uint8_t> aditionInput)
{
    if(this->reseedCounter > 281474976710656) {
        return std::nullopt;
    }

    if(aditionInput.size() != 0) {
        this->HMAC_DRBG_Update(aditionInput);
    }

    std::vector<uint8_t> temp = {};

    while (temp.size() < byteNumber) {
        this->value = convert_to_bytes(hash_message_authentication_code(this->key, this->value));
        temp = concatination(temp, this->value);
    }

    this->HMAC_DRBG_Update(aditionInput);
    ++this->reseedCounter;

    return std::vector<uint8_t>(temp.begin(), temp.begin() + byteNumber);     
}


// Получение энтропии из КГСЧ которые зависят от ОС
#ifdef _WIN32
#include <Windows.h>
#include <wincrypt.h>
std::vector<uint8_t> get_entropy(){
    std::vector<uint8_t> entropy(32);
    HCRYPTPROV hCryptProv = 0;
    
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        throw std::runtime_error("Error during CryptAcquireContext"); 
    }

    
    if (!CryptGenRandom(hCryptProv, 32, entropy.data())) {
        CryptReleaseContext(hCryptProv, 0); // Освобождаем хэндл
        throw std::runtime_error("Error during CryptGenRandom");
    }

    CryptReleaseContext(hCryptProv, 0);
    return entropy;
}

#elif __linux__
#include <fstream>
std::vector<uint8_t> get_entropy() {
    std::vector<uint8_t> entropy(16); 
    std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);

    if (!urandom) {
        throw std::runtime_error("Failed to open /dev/urandom");
        
    }

    urandom.read(reinterpret_cast<char*>(entropy.data()), entropy.size());
    urandom.close();
    return entropy;
}
#error "Unsupported operating system"
#endif
