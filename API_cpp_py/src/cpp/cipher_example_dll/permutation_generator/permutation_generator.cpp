#include "permutation_generator.hpp"


uint32_t x_n = 1225;

// Функция для определение первого эллемента последовательности
void setRand(uint32_t seed)
{
    x_n = seed;
}

// Функция для получения случайного числа в диапазоне [0, 2^(32)]
uint64_t randomNumber()
{
    const uint64_t a = 16807; // Рекомендованное значение Кнутом
    const uint64_t c = 0;
    const uint64_t m = 2147483647;
   
    x_n = (a * x_n + c) % m; // вычисление нового элемена последовательности

    return x_n;
}

std::vector<int32_t> generat_permutation(std::vector<int32_t> sourcePermut, HMAC_DRBG &gen)
{
    // Процесс перемешивания
    for (size_t i = 0; i < sourcePermut.size(); ++i) {
        // Проверка на необходимость перезапуска генератора
        if(gen.HMAC_DRBG_Ressed_Check()) {
            gen.HMAC_DRBG_Ressed(get_entropy());
        }
        uint64_t temp = convert_bytes_to_ddword(gen.HMAC_DRBG_Generate_algorithm(256).value()) % sourcePermut.size();
        std::cout << temp << std::endl;
        std::swap(sourcePermut[i], sourcePermut[temp]);
    }
    
    return sourcePermut;
}
