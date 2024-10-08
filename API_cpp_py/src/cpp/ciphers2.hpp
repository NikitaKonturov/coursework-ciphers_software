#ifndef CIPHERS2
#define CIPHERS2
#include <iostream>
#include <string>

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif


extern "C" {
    EXPORT char* title();
    EXPORT char* encryptTest(char* key, char* openText);
    EXPORT void freeStr(char* cstr);
}

    
#endif // CIPHERS