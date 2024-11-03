#ifndef PERMUTATION_H
#define PERMUTATION_H
#include <iostream>
#include <exception>
#include <map>
#include <vector>
#include <set>
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
    Permutation(const std::string&);
    ~Permutation();

    // Функция
    void checkPermutation();
    void apply(std::string&);

    // Перегрузка операторов
    void operator*(const Permutation&); // Умножение
    void compose(const Permutation&);   // Композиция
    friend std::ostream& operator<<(std::ostream&, const Permutation&);
    friend std::istream& operator>>(std::istream&, Permutation&);
};

#endif //!PERMUTATION_H
