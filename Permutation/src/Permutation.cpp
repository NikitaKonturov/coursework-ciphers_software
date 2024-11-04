#include "Permutation.hpp"


/*================================================================================*/
/*================================= Конструкторы =================================*/
/*================================================================================*/

Permutation::Permutation() {}

Permutation::Permutation(uint32_t initSize) 
{ 
    for (uint32_t i = 0; i < initSize; ++i)
    {
        SourcePermut.insert({ i, i });
    }
}

Permutation::Permutation(const Permutation& rhs)
{
    for (auto pair : rhs.SourcePermut)
    {
        this->SourcePermut.insert({pair.first, pair.second });
    }
}


Permutation::Permutation(const std::vector<int32_t>& rhs)
{
    std::multiset<uint32_t> check;
    for (uint32_t i = 0; i < rhs.size(); ++i)
    {
        if (rhs[i] - 1 != abs(rhs[i] - 1) || rhs[i] == 0) throw std::invalid_argument("Numbers must be positive and not zero!!!"); // Проверка на наличие отрицательных чисел и числа 0 в векторе
        check.find(rhs[i] - 1) == check.end()? check.insert(rhs[i] - 1) : throw std::invalid_argument("Row elements are different!!!"); // Проверка повторяющихся символов во второй строке
        this->SourcePermut.insert({i,( rhs[i] - 1)});
    }
    this->checkPermutation();
}


Permutation::Permutation(const std::string& rhs)
{
    std::istringstream iss(rhs);
    int32_t value;
    uint32_t index = 0;
    std::multiset<uint32_t> check;  // Для проверки дубликатов
    
    // Разбираем строку на числа
    while (iss >> value)
    {
        if (value <= 0) 
            throw std::invalid_argument("Numbers must be positive and not zero!!!");
        
        if (check.find(value - 1) != check.end()) 
            throw std::invalid_argument("Row elements are different!!!");
        
        check.insert(value - 1);
        this->SourcePermut.insert({ index++, value - 1 });
    }
    
    this->checkPermutation();
}

Permutation::~Permutation() {}




/*================================================================================*/
/*==================================== Функции ===================================*/
/*================================================================================*/

// Функция для проверки корректности подстановки
void Permutation::checkPermutation()
{
    std::multiset<uint32_t> check;
    for (const auto& [key, object] : SourcePermut)
    {
        if (SourcePermut.count(object) == 0 ) throw std::invalid_argument("Row elements are different!!!");  // Проверка совместимости строк перестановки
    } 
}

// Функция применения перестановки к строке
void Permutation::apply(std::string& str) 
{
    size_t permSize = SourcePermut.size();

    // Проверка: длина строки должна быть кратна размеру перестановки
    if (str.size() % permSize != 0) {
        throw std::invalid_argument("The length of the string must be a multiple of the size of the permutation!");
    }

    // Создание нового порядка символов
    std::string permutedStr = str;
    for (size_t i = 0; i < str.size(); i += permSize) {
        for (size_t j = 0; j < permSize; ++j) {
            permutedStr[i + j] = str[i + SourcePermut.at(j)];
        }
    }

    // Замена исходной строки на переставленную
    str = permutedStr;
}

void Permutation::inverse()
{
    Permutation inversePermut(SourcePermut.size());

    // Строим обратную перестановку
    for (const auto& [key, value] : SourcePermut) {
        inversePermut.SourcePermut[value] = key;
    }

    SourcePermut = inversePermut.SourcePermut;
}

uint32_t Permutation::operator()(int32_t& index)
{
    // Проверка на допустимый индекс
    if (SourcePermut.find(index) == SourcePermut.end()) 
        throw std::out_of_range("Index is out of range for the permutation.");
    if (index <= 0) 
            throw std::invalid_argument("Numbers must be positive and not zero!!!");
    // Возвращаем элемент по индексу
    return SourcePermut.at(index);
}


void Permutation::operator*(const Permutation& rhs)
{
    for (auto& [up, down] : this->SourcePermut)
    {
        down = rhs.SourcePermut.at(up);
    }
}

void Permutation::compose(const Permutation& rhs)
{
    Permutation temp(this->SourcePermut.size()); 

    // Композиция перестановок: сначала rhs (справа), затем this (слева)
    for (const auto& [up, down] : rhs.SourcePermut)  
    {
        temp.SourcePermut[up] = this->SourcePermut.at(down);
    }

    this->SourcePermut = temp.SourcePermut;  
}

std::ostream& operator<<(std::ostream& out, const Permutation& obj)
{
    for (const auto& pair : obj.SourcePermut)
    {
        out << pair.first + 1 << '\t';
    }
    out << '\n';
    for (const auto& pair : obj.SourcePermut)
    {
        out << pair.second + 1 << '\t';
    }
    return out;
}

std::istream& operator>>(std::istream& in, Permutation& obj)
{
    std::multiset<uint32_t> check;
    int32_t Second;
    for (uint32_t i = 0; i < obj.SourcePermut.size(); ++i)
    {
        in >> Second;
        if (in.fail() || Second != abs(Second) || Second == 0) throw std::invalid_argument("Numbers must be positive and not zero!!!"); // Проверка введенного значения(Значение не доллжно быть: словом/буквой, отрицвтельным число, нулём)
        check.find(Second) == check.end()? check.insert(Second) : throw std::invalid_argument("Row elements are different!!!"); // Проверка повторяющихся символов во второй строке
        obj.SourcePermut[i] = Second - 1;
    }
    obj.checkPermutation();
    return in;
}
