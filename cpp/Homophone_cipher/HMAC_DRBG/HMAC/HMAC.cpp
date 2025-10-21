#include "HMAC.hpp"


std::vector<uint8_t> convert_to_bytes(std::array<uint32_t,8> data)
{   
    std::vector<uint8_t> res;
    for(uint32_t word : data) {
        for (size_t i = 4; i > 0; --i) {
            res.push_back((word >> ((i-1) * 8)) & 0xff);
        }
    }

    return res;
}

std::vector<uint8_t> key_preparing(std::vector<uint8_t> key)
{   
    size_t keySize = key.size() * 8;

    if(keySize > 512) {
        key = convert_to_bytes(Hash(key));
    }
    
    keySize = key.size() * 8;

    if(keySize < 512) {
        key.insert(key.end(), (512-keySize) / 8, 0x00);
    }

    return key;
}

std::vector<uint8_t> opad(std::vector<uint8_t> key)
{
    for(uint8_t& byte: key) {
        byte ^= 0x5c;
    }

    return key;
}

std::vector<uint8_t> ipad(std::vector<uint8_t> key)
{
    for(uint8_t& byte: key) {
        byte ^= 0x36;
    }

    return key;
}

std::vector<uint8_t> concatination(std::vector<uint8_t> first, std::vector<uint8_t> second)
{
    first.insert(first.end(), second.begin(), second.end());

    return first;
}

std::array<uint32_t,8> hash_message_authentication_code(std::vector<uint8_t> key, std::vector<uint8_t> message)
{
    std::vector<uint8_t> prepared_key = key_preparing(key);

    std::array<uint32_t, 8> hash = Hash(concatination(opad(prepared_key), convert_to_bytes(Hash(concatination(ipad(prepared_key), message)))));

    return hash;
}
