#ifndef VIGINER_KEY_GEN_HPP
#define VIGINER_KEY_GEN_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <map>
#include <random>
#include <set>
#include <filesystem>
#include "../HMAC_DRBG/DRBG/DRBG.hpp"
#include "../include/nlohmann/json.hpp"

void sort_dict_words(const std::string& dictPath, const std::string& lang);

std::wstring give_random_key(nlohmann::json prop);

void sort_key_file(nlohmann::json prop);

std::wstring give_random_custom_key(nlohmann::json prop);

void clear_custom(nlohmann::json prop);

void clear_cache(nlohmann::json prop);

#endif //VIGINER_KEY_GEN_HPP