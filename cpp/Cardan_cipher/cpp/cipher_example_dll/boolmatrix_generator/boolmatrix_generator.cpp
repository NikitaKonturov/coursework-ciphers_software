#include "boolmatrix_generator.hpp"

void generatMatrix(BoolMatrix& mtrx, HMAC_DRBG& gen)
{
    uint32_t n = mtrx.size();
    uint32_t half_n = n / 2;  // Размер верхней левой четверти

    for (uint32_t i = 0; i < half_n; ++i) {
        for (uint32_t j = 0; j < half_n; ++j) {
            // Генерируем случайный индекс [0,3] для выбора варианта поворота
            auto randomBytes = gen.HMAC_DRBG_Generate_algorithm(8);
            if (!randomBytes.has_value()) {
                throw std::runtime_error("Ошибка генерации случайных чисел");
            }
            uint32_t variant = convert_bytes_to_ddword(randomBytes.value()) % 4;

            uint32_t x, y;

            // Выбираем только одну позицию для прокола
            switch (variant) {
                case 0: x = i; y = j; break;                    // Оригинальная позиция
                case 1: x = j; y = n - i - 1; break;            // Поворот на 90°
                case 2: x = n - i - 1; y = n - j - 1; break;    // Поворот на 180°
                case 3: x = n - j - 1; y = i; break;            // Поворот на 270°
            }

            // Устанавливаем прокол только в одну выбранную позицию
            mtrx[x][y] = 1;
        }
    }
}



