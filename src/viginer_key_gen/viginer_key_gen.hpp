#ifndef VIGINER_KEY_GEN_HPP
#define VIGINER_KEY_GEN_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <map>
#include <random>
#include <set>

void sort_dict_words(const std::string& dictPath, const std::string& lang);

std::wstring give_random_key(const std::string& lang, const size_t& length);

void sort_key_file(const std::string& keyFilePath, const std::string& lang);

std::wstring give_random_custom_key(const std::string& lang, const size_t& length);

void clear_custom();

void clear_cache();

#endif //VIGINER_KEY_GEN_HPP