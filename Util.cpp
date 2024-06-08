#include <cmath>
#include <climits>
#include "Util.hpp"

#define LC_A 48271
#define LC_C 0
#define LC_M 0x7FFFFFFF
#define LFSR32_MASK 0x80200003

namespace {
    unsigned int MiddleSquare32(unsigned int x, unsigned int n) {
        unsigned int xx = x * x;
        unsigned int u = static_cast<unsigned int>(pow(10, static_cast<int>(n / 2)));
        unsigned int v = static_cast<unsigned int>(pow(10, n));

        return (xx / u) % v;
    }

    unsigned int LinearCongruential32(unsigned int A, unsigned int C, unsigned int M, unsigned int x) {
        unsigned int y = (A * x + C) % M;

        return y;
    }

    unsigned int LinearFeedbackShiftRegister32(unsigned int x, unsigned int mask) {
        return (x >> 1) ^ (mask * (x & 1));
    }
}

Random::Random(RANDOM_METHOD method, unsigned int seed) {
    this->mLastValue = seed;
    this->mMethod = method;
}

unsigned int Random::GetRandom() {
    unsigned int v = this->mLastValue;

    switch(this->mMethod) {
        case RANDOM_METHOD::RM_MIDDLE_SQUARE:
            v = MiddleSquare32(this->mLastValue, 4);
            break;
        case RANDOM_METHOD::RM_LINEAR_CONGRUENTIAL:
            v = LinearCongruential32(LC_A, LC_C, LC_M, this->mLastValue);
            break;
        case RANDOM_METHOD::RM_LFSR:
            v = LinearFeedbackShiftRegister32(this->mLastValue, LFSR32_MASK);
            break;
    }

    this->mLastValue = v;
    return v;
}
