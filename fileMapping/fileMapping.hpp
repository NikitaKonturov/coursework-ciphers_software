#ifndef FILEMAPPING_HPP
#define FILEMAPPING_HPP

#include <iostream>
#include <string>
#include <cstdint>
#include <Windows.h>

std::string getLastErrorCodeMessage(DWORD _dwErrCode);

bool read_and_markup_big_file(const std::string strFilePath, std::string& res);

#endif //FILEMAPPING_HPP