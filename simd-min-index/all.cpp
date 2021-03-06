#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <algorithm> // std::min

#include <immintrin.h>

#define common_assertions                                                       \
    do {                                                                        \
        assert(array);                                                          \
        assert(size > 0);                                                       \
        assert((size % 32) == 0); /* just don't care about odd-sized arrays */  \
    } while (0)


#include "scalar.cpp"
#include "sse.cpp"
#include "sse_unrolled.cpp"
#ifdef HAVE_AVX2
#include "avx2.cpp"
#endif
#ifdef HAVE_AVX512
#include "avx512f.cpp"
#endif
