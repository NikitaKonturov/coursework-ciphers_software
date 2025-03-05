#include "viginer_key_gen/viginer_key_gen.hpp"

int main()
{
    //setlocale(LC_ALL, "ru-RU.UTF-8");
    setlocale(LC_ALL, "en_US.UTF-8");
    std::wstring res = give_random_key(std::string("en"), 4);
    std::wcout << res << '\n';
    clear_cache();
    return 0;
}
