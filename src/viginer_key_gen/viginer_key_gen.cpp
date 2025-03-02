#include "viginer_key_gen.hpp"

bool is_rus_alpha(uint8_t symbol) {
    return (symbol >= 192 && symbol <= 255) || symbol == 184 || symbol == 168;
} //Проверка на русский символ

bool is_eng_alpha(uint8_t symbol) {
    return (symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z');
} //Проверка на английский символ

void sort_dict_words(const std::string& dictPath, const std::string& lang)
{
    std::wifstream dictIn(dictPath);
    std::wstring buff;
    std::string res_dict_part_path;
    dictIn.imbue(std::locale("ru_RU.UTF-8"));
    setlocale(LC_ALL, "ru_RU.UTF-8");
    while (std::getline(dictIn, buff)) {
        if (buff.find(L'-') != std::wstring::npos) {
            continue;
        }
        if (buff.find(L'.') != std::wstring::npos) {
            continue;
        }
        res_dict_part_path = 
        "../../src/dictionaries/" + lang + '/' + 
        lang + "_dict_" + std::to_string(buff.length()) + ".txt";
        std::wofstream res_dict_part(res_dict_part_path, std::ios::app);
        res_dict_part.imbue(std::locale("ru_RU.UTF-8"));
        res_dict_part << buff << '\n';
        res_dict_part.close();
    }
}
