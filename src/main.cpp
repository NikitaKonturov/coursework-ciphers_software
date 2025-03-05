#include "viginer_key_gen/viginer_key_gen.hpp"

int main()
{
    //setlocale(LC_ALL, "ru-RU.UTF-8");
    setlocale(LC_ALL, "en_US.UTF-8");
    sort_key_file("C:/CourseWork/coursework-ciphers_software/src/dictionaries/custom/custom_words.txt", std::string("ru"));
    std::wstring res = give_random_custom_key(std::string("en"), 4);
    std::wcout << res << '\n';
    return 0;
}
