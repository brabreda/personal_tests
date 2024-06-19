/* benchmark is the most ideal scenerio for a personal case.
*  Goal: Best theorical performance benifit compared to normal memcpy and normal vectorized cpy  
*/

#include <benchmark/benchmark.h>
#include <stdint.h>
#include <VectorCpy.h>
#include <bits/stdc++.h> 

#include <vector>
#include <random>

// static constexpr uint64_t steps = 1000;

// std::random_device seed;
// std::mt19937 gen(seed());
// std::uniform_int_distribution<std::mt19937::result_type> dist(1,100);
// uint32_t stepsize = 5900 + (dist(gen) * 2);

// template <typename T>
// static void BM_cpy(benchmark::State& state) {


//   std::vector<uint8_t> SourceData(stepsize * steps, 0); 
//   std::vector<uint8_t> DestData(stepsize * steps * 3, 0);
//   uint8_t* pSource = &(*SourceData.begin());
//   uint8_t* pDest = &(*DestData.begin());

//   srand(time(0));
//   generate(SourceData.begin(), SourceData.end(), rand);

//   for (auto _ : state) {
//     uint8_t* pSource = &(*SourceData.begin());
//     uint8_t* pDest = &(*DestData.begin());

//     for(size_t i=0; i <steps; i++){
//       T::cpy(pSource, pDest, stepsize);
//       pSource += stepsize;
//       pDest += stepsize * 3;
//     }
//   }
// }

// struct Cpy { static inline void *cpy(void *dest, const void *src, size_t n){ return std::memcpy(dest, src, n); } };
// struct VecCpy { static inline void *cpy(void *dest, const void *src, size_t n){ return local_memcpy(dest, src, n); } };
// struct VecStreamCpy { static inline void *cpy(void *dest, const void *src, size_t n){ return local_memcpy(dest, src, n); } };
// struct VecLocalCpy { static inline void *cpy(void *dest, const void *src, size_t n){ return local_memcpy(dest, src, n); } };

// BENCHMARK(BM_cpy<Cpy>);
// BENCHMARK(BM_cpy<VecCpy>);
// BENCHMARK(BM_cpy<VecStreamCpy>);
// BENCHMARK(BM_cpy<VecLocalCpy>);

void *align_pointer(void *ptr, unsigned alignment, unsigned offset) {
  size_t p = (size_t)ptr;
  while (p % alignment)
    ++p;
  return (void *)(p + (size_t)offset);
}


using time_point = std::chrono::steady_clock::time_point;

class Stopwatch {
  /// The time of the last sample;
  time_point begin_;
  /// A list of recorded intervals.
  std::vector<uint64_t> intervals_;

public:
  NO_INLINE
  Stopwatch() : begin_() {}

  NO_INLINE
  void start() { begin_ = std::chrono::steady_clock::now(); }

  NO_INLINE
  void stop() {
    time_point end = std::chrono::steady_clock::now();
    uint64_t interval =
        std::chrono::duration_cast<std::chrono::microseconds>(end - begin_)
            .count();
    intervals_.push_back(interval);
  }

  NO_INLINE
  uint64_t get_median() {
    std::sort(intervals_.begin(), intervals_.end());
    return intervals_[intervals_.size() / 2];
  }
};

uint8_t random_bytes[320] = {
    227, 138, 244, 198, 73,  247, 185, 248, 229, 75,  24,  215, 159, 230, 136,
    246, 200, 144, 65,  67,  109, 86,  118, 61,  209, 103, 188, 213, 187, 8,
    210, 121, 214, 178, 232, 59,  153, 92,  209, 239, 44,  85,  156, 172, 237,
    41,  150, 195, 247, 202, 249, 142, 208, 133, 21,  204, 114, 38,  51,  150,
    194, 46,  184, 138, 50,  250, 190, 180, 161, 5,   211, 191, 62,  137, 142,
    122, 63,  72,  233, 125, 189, 51,  238, 51,  116, 10,  44,  18,  240, 41,
    157, 81,  183, 252, 214, 17,  81,  12,  44,  119, 77,  97,  101, 80,  106,
    128, 190, 89,  160, 104, 244, 192, 46,  69,  73,  255, 45,  213, 190, 86,
    18,  89,  34,  46,  134, 145, 166, 128, 87,  97,  192, 71,  105, 94,  51,
    30,  7,   9,   0,   40,  0,   187, 205, 189, 151, 159, 107, 105, 180, 182,
    233, 52,  209, 108, 186, 31,  184, 254, 170, 71,  162, 31,  80,  226, 75,
    125, 214, 125, 247, 197, 149, 132, 247, 157, 253, 101, 107, 1,   127, 236,
    249, 242, 152, 169, 123, 240, 129, 230, 135, 25,  57,  227, 130, 189, 76,
    254, 33,  193, 39,  82,  177, 143, 31,  17,  20,  195, 219, 165, 171, 198,
    125, 119, 216, 143, 55,  210, 17,  88,  150, 126, 38,  160, 71,  214, 10,
    162, 158, 6,   234, 233, 119, 221, 167, 62,  146, 50,  150, 176, 142, 167,
    201, 250, 195, 26,  156, 96,  36,  177, 95,  23,  7,   63,  55,  142, 80,
    227, 73,  124, 93,  211, 231, 166, 182, 57,  145, 55,  242, 213, 246, 30,
    146, 247, 19,  229, 34,  210, 37,  147, 242, 103, 125, 91,  171, 51,  22,
    126, 248, 149, 19,  60,  89,  5,   241, 132, 72,  217, 195, 11,  173, 247,
    47,  144, 222, 94,  51,  166, 192, 50,  109, 62,  42,  126, 111, 204, 141,
    66,
};

/// Implements a doom-style random number generator.
struct DoomRNG {
  // Points to the current random number.
  unsigned rand_curr = 0;

  void rand_reset() { rand_curr = 0; }

  uint8_t next_u8_random() { return random_bytes[rand_curr++ % 320]; }
};



// BENCHMARK_MAIN();

using memset_ty = void *(void *s, int c, size_t n);
using memcpy_ty = void *(void *dest, const void *src, size_t n);

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <vector>

#include <immintrin.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// This is a small program that compares two memcpy implementations and records
// the output in a csv file.
////////////////////////////////////////////////////////////////////////////////

#define ITER (1000L * 1000L * 10L)
#define SAMPLES (20)

DoomRNG RNG;

/// Measure a single implementation \p handle.
uint64_t measure(memcpy_ty handle, void *dest, void *src, unsigned size) {
  Stopwatch T;
  for (unsigned i = 0; i < SAMPLES; i++) {
    T.start();
    for (size_t j = 0; j < ITER; j++) {
      (handle)(dest, src, size);
    }
    T.stop();
  }
  return T.get_median();
}

inline void* vec_copy(void *dest, const void *source, const size_t nBytes){
  const uint8_t* pSource = (uint8_t*) source;
  const uint8_t* pDest = (uint8_t*) dest;

  __m256i* pSourceVec = (__m256i*) (pSource);
  __m256i* pDestVec   = (__m256i*) (pDest);

  const size_t OffsetSecondVector = (sizeof(__m256i) * 4) - ((uintptr_t) pDest % (sizeof(__m256i) * 4));
  const uint32_t nVec = nBytes / (sizeof(__m256i) * 4);

  {
    const __m256i L0 = _mm256_loadu_si256(pSourceVec++);
    const __m256i L1 = _mm256_loadu_si256(pSourceVec++);
    const __m256i L2 = _mm256_loadu_si256(pSourceVec++);

    _mm256_storeu_si256(pDestVec++, L0);
    _mm256_storeu_si256(pDestVec++, L1);
    _mm256_storeu_si256(pDestVec++, L2);
  }

  pSourceVec = (__m256i*) (pSource + OffsetSecondVector);
  pDestVec   = (__m256i*) (pDest + OffsetSecondVector);

  for(uint32_t i=0; i<nVec; i++) {
    const __m256i L0 = _mm256_loadu_si256(pSourceVec++);
    const __m256i L1 = _mm256_loadu_si256(pSourceVec++);
    const __m256i L2 = _mm256_loadu_si256(pSourceVec++);
    const __m256i L3 = _mm256_loadu_si256(pSourceVec++);

    _mm256_storeu_si256(pDestVec++, L0);
    _mm256_storeu_si256(pDestVec++, L1);
    _mm256_storeu_si256(pDestVec++, L2);
    _mm256_storeu_si256(pDestVec++, L3);
  }
  return nullptr;
}

// Allocate memory and benchmark each implementation at a specific size \p size.
void bench_impl(const std::vector<memcpy_ty *> &toTest, unsigned size,
                unsigned align, unsigned offset) {
  std::vector<char> dest(size + 256, 0);
  std::vector<char> src(size + 256, 0);

  char *src_ptr = (char *)align_pointer(&src[0], align, offset);
  char *dest_ptr = (char *)align_pointer(&dest[0], align, offset);

  std::cout << size << ", ";
  for (auto handle : toTest) {
    u_int64_t res = measure(handle, dest_ptr, src_ptr, size);
    std::cout << res << ", ";
  }
  std::cout << std::endl;
}

/// Allocate and copy buffers at random offsets and in random sizes.
/// The sizes and the offsets are in the range 0..256.
void bench_rand_range(const std::vector<memcpy_ty *> &toTest) {
  std::vector<char> dest(4096, 1);
  std::vector<char> src(4096, 0);
  const char *src_p = &src[0];
  char *dest_p = &dest[0];

  for (auto handle : toTest) {
    Stopwatch T;
    sleep(1);
    for (unsigned i = 0; i < SAMPLES; i++) {
      RNG.rand_reset();
      T.start();
      for (size_t j = 0; j < ITER; j++) {
        char *to = dest_p + RNG.next_u8_random();
        const char *from = src_p + RNG.next_u8_random();
        (handle)(to, from, RNG.next_u8_random());
      }
      T.stop();
    }

    std::cout << T.get_median() << ", ";
  }
  std::cout << std::endl;
}

// To measure the call overhead.
void *nop(void *dest, const void *src, size_t n) { return dest; }

int main(int argc, char **argv) {
  std::cout << std::setprecision(3);
  std::cout << std::fixed;

  std::vector<memcpy_ty *> toTest = { &memcpy, &local_memcpy, &vec_copy};

  std::cout << "Batches of random sizes:\n";
  std::cout << "libc@plt, libc, folly, c_memcpy, asm_memcpy, nop, myimpl\n";

  bench_rand_range(toTest);

  std::cout << "\nFixed size:\n";
  std::cout << "size, libc@plt, libc, folly, c_memcpy, asm_memcpy, nop, myimpl\n";

  for (int i = 0; i < 512; i++) {
    bench_impl(toTest, i, 16, 0);
  }

  return 0;
}