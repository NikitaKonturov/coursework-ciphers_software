#include "ciphers1.hpp"
#include <cstring>

extern "C" {
    char* encryptTest(char* key, char* openText) {
        char* res = new char[strlen(key) + strlen(openText) + 2];
        strcpy(res, openText);
        strcat(res, key);
        return res;
    }

    void freeStr(char* cstr) {
        delete[] cstr;
    }

    char* title() {
        return "first_cipher";
    }

}
   

