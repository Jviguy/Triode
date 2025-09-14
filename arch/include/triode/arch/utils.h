#ifndef TRIODE_UTILS_H
#define TRIODE_UTILS_H
namespace triode::arch {
    constexpr int64_t pow3(const int exp) {
        int64_t result = 1;
        for (int i = 0; i < exp; ++i) {
            result *= 3;
        }
        return result;
    }
}

#endif //TRIODE_UTILS_H