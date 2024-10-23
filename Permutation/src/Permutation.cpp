#include "Permutation.hpp"


/*================================================================================*/
/*================================= Конструкторы =================================*/
/*================================================================================*/

Permutation::Permutation() {}

Permutation::Permutation(uint32_t initSize) 
{ 
    for (uint32_t i = 0; i < initSize; ++i)
    {
        map.insert({ i, i });
    }
}

Permutation::Permutation(const Permutation& rhs)
{
    for (auto pair : rhs.map)
    {
        this->map.insert({pair.first, pair.second });
    }
}


Permutation::Permutation(const std::vector<uint32_t>& rhs)
{
    for (uint32_t i = 0; i < rhs.size(); ++i)
    {
        this->map.insert({i,( rhs[i] - 1)});
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
    for (const auto& [key, object] : map)
    {
        if (map.count(object) == 0 ) throw std::invalid_argument("Row elements are different!!!");  // Проверка совместимости строк перестановки
        check.find(object) == check.end()? check.insert(object) : throw std::invalid_argument("Row elements are different!!!"); // Проверка повторяющихся символов во второй строке
    } 
}

// Функция применения перестановки к строке
void Permutation::apply(std::string& string)
{
    std::vector<std::string> parts;
    std::istringstream iss(string);
    std::string element;

    // Считываем все элементы строки в вектор
    while (iss >> element) 
    {
        parts.push_back(element);
    }

    // Проверяем, что количество элементов делится на размер перестановки
    if(parts.size() % map.size() != 0) 
        throw std::invalid_argument("The size of the string is not a multiple of the size of the permutation!");

    std::string openText;

    uint32_t blockCount = parts.size() / map.size(); // Количество блоков
    for (uint32_t i = 0; i < blockCount; ++i)
    {
        for (uint32_t j = 0; j < map.size(); ++j)
        {
            // Индекс в parts: (i * размер блока) + новый индекс по подстановке
            openText += parts[i * map.size() + map.at(j)] + ' ';
        }
    }

    if (!openText.empty()) 
    {
        openText.pop_back();
    }

    string = openText;
}


/*================================================================================*/
/*==================================== Функции ===================================*/
/*================================================================================*/

void Permutation::operator*(const Permutation& rhs)
{
    for (auto& [up, down] : this->map)
    {
        down = rhs.map.at(up);
    }
}

void Permutation::compose(const Permutation& rhs)
{
    Permutation temp(this->map.size()); 

    // Композиция перестановок: сначала rhs (справа), затем this (слева)
    for (const auto& [up, down] : rhs.map)  
    {
        temp.map[up] = this->map.at(down);
    }

    this->map = temp.map;  
}

std::ostream& operator<<(std::ostream& out, const Permutation& obj)
{
    for (const auto& pair : obj.map)
    {
        out << pair.first + 1 << '\t';
    }
    out << '\n';
    for (const auto& pair : obj.map)
    {
        out << pair.second + 1 << '\t';
    }
    return out;
}

std::istream& operator>>(std::istream& in, Permutation& obj)
{
    uint32_t Second;
    for (uint32_t i = 0; i < obj.map.size(); ++i)
    {
        in >> Second;
        obj.map[i] = Second - 1;
    }
    obj.checkPermutation();
    return in;
}
