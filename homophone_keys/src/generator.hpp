#ifndef GENERATOR_HPP
#define GENERATOR_HPP
#include <iostream> //для ввода и вывода
#include <exception> //для выброса исключений
#include <typeinfo> //для обработки типа выбрасываемох ошибок
#include <fstream> //для работы с файлами
#include <ctime> //для генерации рандомного числа
#include <string> //для корректной работы с данными типа std::string
#include <map> //для работы с контейнером std::map
#include <vector> //для работы с контейнером std::vector
#include <algorithm> //для использования алгоритмов с++


bool check_language(std::string);
bool check_stream();
int64_t get_positive_number();
int64_t get_random_number(const int64_t&, const int64_t&);
bool get_homophones_amount();
void get_boarders(int64_t&, int64_t&, const std::string&);
std::map<char, double> get_frequencies(const std::string&);
std::map<char, std::vector<int64_t>> generate_keys(const int64_t&, const int64_t&, const std::string&);
void print_keys_info(const std::map<char, std::vector<int64_t>>&);

#endif //GENERATOR_HPP