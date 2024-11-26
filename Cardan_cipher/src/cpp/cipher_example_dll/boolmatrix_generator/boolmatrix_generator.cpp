#include "boolmatrix_generator.hpp"

void generatMatrix(BoolMatrix& mtrx, std::minstd_rand& gen)
{
    std::map<uint32_t, std::set<uint32_t>> usingBigrams;
    // Создаём генератор на основе мультипликативной конгруэнтной последовательности
    
    // Создаём распределение от 0 до 2n-1
    std::uniform_int_distribution<int32_t> distr(0, (mtrx.size()-1));

    mtrx = BoolMatrix(mtrx.size()/2);

    // n^2
    uint32_t onesCount = mtrx.size() * mtrx.size() / 4; 
    uint32_t n = mtrx.size()/2;
    for (size_t k = 0; k < onesCount; ++k) {
        uint32_t i = 0;
        uint32_t j = 0;
        do {
            i = distr(gen);
            j = distr(gen);
            // 0 поворот
            if(usingBigrams.find(i) != usingBigrams.end() && usingBigrams.find(i)->second.find(j) != usingBigrams.find(i)->second.end()){
                usingBigrams[i].insert(j);
                continue;
            }
            uint32_t temp = i;
            i = j;
            j = n - temp + 1;
            // 90 поворот
            if(usingBigrams.find(i) != usingBigrams.end() && usingBigrams.find(i)->second.find(j) != usingBigrams.find(i)->second.end()){
                usingBigrams[i].insert(j);
                continue;
            }
            temp = i;
            i = j;
            j = n - temp + 1;
            // 180 поворот
            if(usingBigrams.find(i) != usingBigrams.end() && usingBigrams.find(i)->second.find(j) != usingBigrams.find(i)->second.end()){
                usingBigrams[i].insert(j);
                continue;
            }
            temp = i;
            i = j;
            j = n - temp + 1;
            // 270 поворот
            if(usingBigrams.find(i) != usingBigrams.end() && usingBigrams.find(i)->second.find(j) != usingBigrams.find(i)->second.end()){
                usingBigrams[i].insert(j);
                continue;
            }
            temp = i;
            i = j;
            j = n - temp + 1;
            usingBigrams[i].insert(j);
            mtrx[i][j] = 1;
            break;
        }while(true);
    }
}
