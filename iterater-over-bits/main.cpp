#include <chrono>
#include <vector>
#include <initializer_list>

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cassert>


class bitvector {

    const size_t n;
    uint64_t* array;

public:
    bitvector(size_t n_) : n(n_) {
        assert(n > 0);
        array = new uint64_t[n];
    }

    ~bitvector() {
        delete[] array;
    }

    void fill(uint64_t word) {
        for (size_t i=0; i < n; i++)
            array[i] = word;
    }

    void fill_random(int threshold) {
        assert(threshold >= 0);
        assert(threshold <= 100);

        srand(0);
        for (size_t i=0; i < n; i++) {
            if ((rand() % 100) > threshold) {
                array[i] = -1;
            } else {
                array[i] = 0;
            }
        }
    }

    size_t size() const {
        return n * 64;
    }

    size_t cardinality() const {
        size_t k = 0;
        for (size_t i=0; i < n; i++) {
            k += __builtin_popcountll(array[i]);
        }

        return k;
    }

public:
    template <typename CALLBACK>
    void iterate_naive(CALLBACK callback) const {
        for (size_t i=0; i < n; i++) {
            uint64_t tmp = array[i];

            size_t k = i * 64;
            while (tmp) {
                if (tmp & 0x1) {
                    callback(k);
                }

                tmp >>= 1;
                k += 1;
            }
        }
    }

    template <typename CALLBACK>
    void iterate_better(CALLBACK callback) const {
        for (size_t i=0; i < n; i++) {
            uint64_t tmp = array[i];

            size_t k = i * 64;
            while (tmp) {
                const uint64_t t = tmp & (~tmp + 1);
                const uint64_t r = __builtin_ctzll(t);
                callback(k + r);
                tmp ^= t;
            }
        }
    }

    template <typename CALLBACK>
    void iterate_block3(CALLBACK callback) const {
        for (size_t i=0; i < n; i++) {
            uint64_t tmp = array[i];

            size_t k = i * 64;
            while (tmp) {
                switch (tmp & 0x7) {
                    case 0:
                        break;

                    case 1:
                        callback(k);
                        break;

                    case 2:
                        callback(k + 1);
                        break;

                    case 3:
                        callback(k);
                        callback(k + 1);
                        break;

                    case 4:
                        callback(k + 2);
                        break;

                    case 5:
                        callback(k);
                        callback(k + 2);
                        break;

                    case 6:
                        callback(k + 1);
                        callback(k + 2);
                        break;

                    case 7:
                        callback(k);
                        callback(k + 1);
                        callback(k + 2);
                        break;
                }

                tmp >>= 3;
                k += 3;
            }
        }
    }

    template <typename CALLBACK>
    void iterate_block4(CALLBACK callback) const {
        for (size_t i=0; i < n; i++) {
            uint64_t tmp = array[i];

            size_t k = i * 64;
            while (tmp) {
                switch (tmp & 0xf) {
                    case 0:
                        break;

                    case 1:
                        callback(k);
                        break;

                    case 2:
                        callback(k + 1);
                        break;

                    case 3:
                        callback(k);
                        callback(k + 1);
                        break;

                    case 4:
                        callback(k + 2);
                        break;

                    case 5:
                        callback(k);
                        callback(k + 2);
                        break;

                    case 6:
                        callback(k + 1);
                        callback(k + 2);
                        break;

                    case 7:
                        callback(k);
                        callback(k + 1);
                        callback(k + 2);
                        break;

                    case 8:
                        callback(k + 3);
                        break;

                    case 9:
                        callback(k);
                        callback(k + 3);
                        break;

                    case 10:
                        callback(k + 1);
                        callback(k + 3);
                        break;

                    case 11:
                        callback(k);
                        callback(k + 1);
                        callback(k + 3);
                        break;

                    case 12:
                        callback(k + 2);
                        callback(k + 3);
                        break;

                    case 13:
                        callback(k);
                        callback(k + 2);
                        callback(k + 3);
                        break;

                    case 14:
                        callback(k + 1);
                        callback(k + 2);
                        callback(k + 3);
                        break;

                    case 15:
                        callback(k);
                        callback(k + 1);
                        callback(k + 2);
                        callback(k + 3);
                        break;
                }

                tmp >>= 4;
                k += 4;
            }
        }
    }
};

#ifdef __GNUC__
#   define NOINLINE __attribute__((noline))
#else
#   define NOINLINE
#endif

size_t callback_function(size_t index) {
    return index;
}


void testcase(const bitvector& bv, int iterations, size_t (*cb_fun)(size_t)) {
    using clock = std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::microseconds;

    uint64_t ta;
    uint64_t tb;

    uint64_t n;
    auto callback = [&n, cb_fun](size_t i) {
#if 0
        n += cb_fun(i);
#else
        n += i;
#endif
    };

    {
        int tmp = iterations;
        volatile uint64_t k = 0;

        const auto t1 = clock::now();
        while (tmp--) {
            n = 0;
            bv.iterate_naive(callback);
            k += n;
        }
        const auto t2 = clock::now();

        ta = duration_cast<microseconds>(t2 - t1).count();
        printf("\t\tnaive:  %10ld us [%ld]\n", ta, k);
    }

    {
        int tmp = iterations;
        volatile uint64_t k = 0;

        const auto t1 = clock::now();
        while (tmp--) {
            n = 0;
            bv.iterate_better(callback);
            k += n;
        }
        const auto t2 = clock::now();

        tb = duration_cast<microseconds>(t2 - t1).count();
        printf("\t\tbetter: %10ld us [%ld]", tb, k);
        printf(" (%0.2f)\n", ta/double(tb));
    }


    {
        int tmp = iterations;
        volatile uint64_t k = 0;

        const auto t1 = clock::now();
        while (tmp--) {
            n = 0;
            bv.iterate_block3(callback);
            k += n;
        }
        const auto t2 = clock::now();

        tb = duration_cast<microseconds>(t2 - t1).count();
        printf("\t\tblock3: %10ld us [%ld]", tb, k);
        printf(" (%0.2f)\n", ta/double(tb));
    }

    {
        int tmp = iterations;
        volatile uint64_t k = 0;

        const auto t1 = clock::now();
        while (tmp--) {
            n = 0;
            bv.iterate_block4(callback);
            k += n;
        }
        const auto t2 = clock::now();

        tb = duration_cast<microseconds>(t2 - t1).count();
        printf("\t\tblock4: %10ld us [%ld]", tb, k);
        printf(" (%0.2f)\n", ta/double(tb));
    }
}


template <typename INITIALIZER>
void test(const char* info, const std::vector<size_t>& sizes, int iterations, INITIALIZER initalizer) {

    bool print = true;
    for (size_t i: sizes) {
        bitvector bv(i);   
        initalizer(bv);
        if (print) {
            printf("%s\n", info);
            print = false;
        }
        printf("\tsize=%lu, cardinality=%lu\n", bv.size(), bv.cardinality());

        testcase(bv, iterations, callback_function);
    }
}


int main() {
 
    const std::vector<size_t> sizes = {64, 256, 1024, 4096, 8192};

    test("empty", sizes, 10000, [](bitvector& bv) {bv.fill(0);});
    test("1/4",   sizes,  1000, [](bitvector& bv) {bv.fill(0x000000000000ffff);});
    test("1/2",   sizes,  1000, [](bitvector& bv) {bv.fill(0x00000000ffffffff);});
    test("3/4",   sizes,  1000, [](bitvector& bv) {bv.fill(0x0000ffffffffffff);});
    test("full",  sizes,  1000, [](bitvector& bv) {bv.fill(0xffffffffffffffff);});
    test("rand",  sizes,  1000, [](bitvector& bv) {bv.fill_random(80);});
    test("rand2", sizes,  1000, [](bitvector& bv) {bv.fill_random(5);});
    return 0;
}

