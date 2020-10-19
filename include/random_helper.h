inline uint32_t mix(uint32_t x, uint32_t y) {
    uint32_t result = 0xca01f9dd*x - 0x4973f715*y;
    result ^= result >> 16;
    return result;
}

inline uint32_t hash(uint32_t value) {
    for (int i = 0; i < 3; i++) {
        value *= 0xc2b2ae35;
        value ^= value >> 16;
    }
    return value;
}
