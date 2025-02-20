#ifndef FILEMAPPING_HPP
#define FILEMAPPING_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <map>
#include <random>
#include <set>

bool is_rus_alpha(uint8_t symbol);

std::map<size_t, size_t> count_words(const std::string& keyFilePath, const std::string& lang);

void set_final_key_word_num(size_t& keyWordNum, bool*& bunnedWords);

std::string give_word(
    const std::string& keyFilePath, 
    const std::string& lang, 
    std::map<size_t, size_t>& wordsCount,
    const size_t& keyWordLength,
    bool*& bannedWords,
    const size_t& allWordsCount
);

#endif //FILEMAPPING_HPP