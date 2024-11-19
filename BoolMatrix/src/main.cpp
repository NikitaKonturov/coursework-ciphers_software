#include "BoolMatrix.hpp"
#include <windows.h>

int main()
{
    try
    {
        std::string OT;
        std::string ShT("aipeorgypdlcrpat");
        std::string key("1010000010100000");
        BoolMatrix grate(key);
        std::cout << grate;

        std::cout << "ShT:\t" << ShT << '\n';

        OT = grate.decryption(ShT);
        std::cout << "OT:\t" << OT << '\n';

        ShT = grate.encryption(OT);
        std::cout << "ShT:\t" << ShT << '\n';

        OT = grate.decryption(ShT);
        std::cout << "OT:\t" << OT << '\n';

        ShT = grate.encryption(OT);
        std::cout << "ShT:\t" << ShT << '\n';
       


        BoolMatrix A(1);
        std::cout << A;
        A[0][0]=1;
        A[0][1]=1;
        A[1][0]=1;
        A[1][1]=1;
        std::cout << A;

    }
    catch(const std::exception& e)
    {
        std::cerr << '\n' << e.what() << '\n';
    }
    ;
}