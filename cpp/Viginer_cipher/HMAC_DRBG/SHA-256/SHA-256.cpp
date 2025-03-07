#include "SHA-256.hpp"


std::vector<std::array<uint32_t, 16>> data_expansion(std::vector<uint8_t> data) 
{
    size_t dataSize = data.size() * 8;
    data.push_back(0x80);
    size_t quantityToSupplement = (448 - (dataSize + 1) % 512)%512;
    
    data.insert(data.end(), quantityToSupplement / 8, 0x00);

    uint64_t bitLenght = dataSize;
    for (int i = 7; i >= 0; --i) {
        data.push_back((bitLenght >> (i * 8)) & 0xff);
    }
    
    std::vector<std::array<uint32_t, 16>> res;

    for (size_t i = 0; i < data.size(); i += 64) {
        std::array<uint32_t, 16> block = {0};

        // Заполняем 16 слов по 32 бита
        for (size_t j = 0; j < 16; ++j) {
            uint32_t word = 0;
            for (size_t k = 0; k < 4; ++k) { // 4 байта = 32 бита
                word = word << 8;
                word |= data[i + j * 4 + k];
            }
            block[j] = word;
        }

        res.push_back(block);
    }
    
    return res;
}

uint32_t right_rotate(uint32_t data, uint32_t count)
{
    return (data >> count | data << (32 - count));
}

uint32_t c0(uint32_t word)
{
    return (right_rotate(word, 7) ^ right_rotate(word, 18) ^ (word >> 3));
}

uint32_t c1(uint32_t word)
{
    return (right_rotate(word, 17) ^ right_rotate(word, 19) ^ (word >> 10));
}

std::vector<std::array<uint32_t, 64>> block_expansion(std::vector<std::array<uint32_t, 16>> rawBlocks) 
{   
    std::vector<std::array<uint32_t, 64>> res;

    for(auto rawBlock: rawBlocks) {
        std::array<uint32_t, 64> newBlock;
        
        for (size_t i = 0; i < 16; ++i) {
            newBlock[i] = rawBlock[i];
        }

        for (size_t i = 16; i < 64; ++i) {
            newBlock[i] = newBlock[i-16] + c0(newBlock[i-15]) + newBlock[i-7] + c1(newBlock[i-2]);
        }
        
        res.push_back(newBlock);
    } 

    return res;
}

uint32_t sum0(uint32_t word)
{
    return right_rotate(word, 2) ^ right_rotate(word, 13) ^ right_rotate(word, 22);
}

uint32_t sum1(uint32_t word)
{
    return right_rotate(word, 6) ^ right_rotate(word, 11) ^ right_rotate(word, 25);
}

uint32_t choice(uint32_t e, uint32_t f, uint32_t g)
{
    return (e & f) ^ ((~e) & g);
}

uint32_t majority(uint32_t a, uint32_t b, uint32_t c)
{
    return (a & b) ^ (a & c) ^ (b & c);
}

void compress(std::array<uint32_t, 64> block, std::array<uint32_t, 8> &hash)
{
    // Дробная часть корней из первых 64 простых чисел 
    const std::array<uint32_t, 64> constans = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
		0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
		0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
		0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
		0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
		0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
		0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
		0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
		0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
		0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
		0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
		0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
		0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
		0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
		0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
		0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };

    uint32_t a = hash[0];
    uint32_t b = hash[1];
    uint32_t c = hash[2];
    uint32_t d = hash[3];
    uint32_t e = hash[4];
    uint32_t f = hash[5];
    uint32_t g = hash[6];
    uint32_t h = hash[7];

    for (size_t i = 0; i < 64; ++i) {
        uint32_t temp1 = h + sum1(e) + choice(e, f, g) + constans[i] + block[i];
        uint32_t temp2 = sum0(a) + majority(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2; 
    }
    
    hash[0] += a;
	hash[1] += b;
	hash[2] += c;
	hash[3] += d;
	hash[4] += e;
	hash[5] += f;
	hash[6] += g;
	hash[7] += h;
}

std::array<uint32_t, 8> Hash(std::vector<uint8_t> data)
{
    std::vector<std::array<uint32_t, 64>> blocks(block_expansion(data_expansion(data)));

    std::array<uint32_t, 8> hash = {
        0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
		0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19
    };

    for(auto& block: blocks){
        compress(block, hash);
    }
    
    return hash;
}
