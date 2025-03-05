#include "viginer_key_gen/viginer_key_gen.hpp"
#include "viginer_cipher/viginer_cipher.hpp"

int main()
{
    std::locale::global(std::locale("ru_RU.UTF-8"));
    std::wcout.imbue(std::locale());
    setlocale(LC_ALL, "ru_RU.UTF-8");
    std::vector<std::wstring> texts;
    std::wifstream openTextFile("C:/CourseWork/coursework-ciphers_software/src/small_ru.txt");
    std::wstring buff;
    for (size_t i = 0; i < 5; ++i) {
        std::getline(openTextFile, buff);
        texts.push_back(buff);
    }
    
    std::vector<std::wstring> keys;
    for (size_t i = 0; i < 5; ++i) {
        keys.push_back(give_random_key(std::string("ru"), 4));
    }
    
    decript(encript(texts, keys));
    
    return 0;
}
