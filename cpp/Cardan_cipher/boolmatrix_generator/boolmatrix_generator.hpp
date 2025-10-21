#ifndef PERMUTATION_GENERATOR
#define PERMUTATION_GENERATOR
#include "../BoolMatrix/src/BoolMatrix.hpp"
#include "../HMAC_DRBG/DRBG/DRBG.hpp"
#include <vector>
#include <random>
#include <map>
#include <set>

void generatMatrix(BoolMatrix& mtrx, HMAC_DRBG& gen);

#endif //PERMUTATION_GENERATOR
