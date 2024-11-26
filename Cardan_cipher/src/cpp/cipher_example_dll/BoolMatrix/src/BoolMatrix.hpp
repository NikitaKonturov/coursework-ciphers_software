#ifndef BOOLMATRIX_H
#define BOOLMATRIX_H

#include <iostream>
#include <vector>
#include <string>
#include "cipher_exceptions/cipher_exceptions.hpp"

class BoolMatrix
{
private:
    std::vector<std::vector<bool>> b_matrix;
    size_t m_size;
public:
    // Конструкторы
    BoolMatrix();
    BoolMatrix(int32_t n);                         // Формула для расчета количества столбцов/строк: L=2n, где n - передаваемое в аргумент значение
    BoolMatrix(std::vector<std::vector<bool>>&);   
    BoolMatrix(BoolMatrix&);            
    BoolMatrix(const std::string&);                // Для заполнения матрицы из строки вида "0010001000101", рассчет длины стороны матрицы = корень из длины строки, добавить проверку: сторона^2 == sqrt(str.size())
    ~BoolMatrix();
    
    // Методы и функции
    uint32_t size();
    void check();                                  // Для проверки корректности матрицы Кордано
    void rotation();                               // Для поворота матрицы на 90° направо
    std::string decryption(const std::string&);    // Для применения матрицы к строке (Расшифрование)
    std::string encryption(const std::string&);    // Для применения матрицы к строке (Зашифрование)
    
    // Перегрузка операторов
    std::vector<bool>& operator[](int32_t);
    friend std::ostream& operator<<(std::ostream&, const BoolMatrix&);
    friend std::istream& operator>>(std::istream&, BoolMatrix&);
};


#endif // !BOOLMATRIX_H