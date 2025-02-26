#ifndef VIGINER_CIPHER_HPP
#define VIGINER_CIPHER_HPP

#include <map>
#include <string>
#include <vector>
#include "..\..\include\nlohmann\json.hpp"

std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys);

std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndText);

std::vector<std::string> gen_keys(std::string keyPropertys, size_t count);

std::string get_key_propertys();

void chekRequest(nlohmann::json keyPropertys);

#endif //VIGINER_CIPHER_HPP