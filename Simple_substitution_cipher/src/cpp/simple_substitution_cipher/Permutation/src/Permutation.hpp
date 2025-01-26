#ifndef PERMUTATION_H
#define PERMUTATION_H
#include <iostream>
#include <exception>
#include <map>
#include <vector>
#include <iterator>
#include <set>
#include <regex>
#include <algorithm>
#include <sstream>

class Permutation
{
private:
    std::map<uint32_t, uint32_t> SourcePermut;
public:
    // Констуркторы
    Permutation();
    Permutation(uint32_t);
    Permutation(const std::vector<int32_t>&);
    Permutation(const Permutation&);
    Permutation(std::string);
    Permutation(std::wstring);
    ~Permutation();

    // Функция
    void checkPermutation();
    void apply(std::string&);
    void apply(std::wstring&);
    void inverse();
    size_t size();

    // Перегрузка операторов
    void operator*(const Permutation&); // Умножение
    void compose(const Permutation&);   // Композиция
    uint32_t operator()(int32_t&);          // Применение подстановки к числу
    friend std::wostream& operator<<(std::wostream&, const Permutation&);
    friend std::wistream& operator>>(std::wistream&, Permutation&);
};

#endif //!PERMUTATION_H