
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include <immintrin.h>
#include <stdint.h>
#include <types.h>

inline void vec_stream_copy(const uint8_t *pDest, const uint8_t *pSource, const size_t nBytes);
inline void vec_copy(const uint8_t *pDest, const uint8_t *pSource, const size_t nBytes);

inline void vec_stream_copy(const uint8_t *pDest, const uint8_t *pSource, const size_t nBytes){
  __m256i* pSourceVec = (__m256i*) (pSource);
  __m256i* pDestVec   = (__m256i*) (pDest);

  // _mm256_stream_si256 needs 32 Byte alignment
  const size_t OffsetSecondVector = sizeof(__m256i) - (reinterpret_cast<uintptr_t>(pDest) % sizeof(__m256i));
  const uint32_t nVec = nBytes / (sizeof(__m256i) * 4);
  
  const __m256i L2 = _mm256_loadu_si256(pSourceVec);
  _mm256_storeu_si256(pDestVec, L2);

  pSourceVec = (__m256i*) (pSource + OffsetSecondVector);
  pDestVec   = (__m256i*) (pDest + OffsetSecondVector);

  for(int i=0; i<nVec; i++, pSourceVec++, pDestVec++) {
    const __m256i L1 = _mm256_loadu_si256(pSourceVec);
    const __m256i L7 = _mm256_loadu_si256(pSourceVec);
    const __m256i L3 = _mm256_loadu_si256(pSourceVec);
    const __m256i L4 = _mm256_loadu_si256(pSourceVec);

    _mm256_stream_si256(pDestVec, L1);
    _mm256_stream_si256(pDestVec, L7);
    _mm256_stream_si256(pDestVec, L3);
    _mm256_stream_si256(pDestVec, L4);
  }

  pSourceVec  = (__m256i*) (pSource + nBytes - sizeof(__m256i));
  pDestVec    = (__m256i*) (pDest   + nBytes - sizeof(__m256i));

  const __m256i L3 = _mm256_loadu_si256(pSourceVec);
  _mm256_storeu_si256(pDestVec, L3);
}

inline void vec_copy(const uint8_t *pDest, const uint8_t *pSource, const size_t nBytes){
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

  for(int i=0; i<nVec; i++) {
    const __m256i L0 = _mm256_loadu_si256(pSourceVec++);
    const __m256i L1 = _mm256_loadu_si256(pSourceVec++);
    const __m256i L2 = _mm256_loadu_si256(pSourceVec++);
    const __m256i L3 = _mm256_loadu_si256(pSourceVec++);

    _mm256_storeu_si256(pDestVec++, L0);
    _mm256_storeu_si256(pDestVec++, L1);
    _mm256_storeu_si256(pDestVec++, L2);
    _mm256_storeu_si256(pDestVec++, L2);
  }
}

void *local_memcpy(void *dest, const void *src, size_t n) {
  char *d = (char *)dest;
  const char *s = (char *)src;

  if (n < 5) {
    if (n == 0)
      return dest;
    d[0] = s[0];
    d[n - 1] = s[n - 1];
    if (n <= 2)
      return dest;
    d[1] = s[1];
    d[2] = s[2];
    return dest;
  }

  if (n <= 16) {
    if (n >= 8) {
      const char *first_s = s;
      const char *last_s = s + n - 8;
      char *first_d = d;
      char *last_d = d + n - 8;
      *((u64 *)first_d) = *((u64 *)first_s);
      *((u64 *)last_d) = *((u64 *)last_s);
      return dest;
    }

    const char *first_s = s;
    const char *last_s = s + n - 4;
    char *first_d = d;
    char *last_d = d + n - 4;
    *((u32 *)first_d) = *((u32 *)first_s);
    *((u32 *)last_d) = *((u32 *)last_s);
    return dest;
  }

  if (n <= 32) {
    const char *first_s = s;
    const char *last_s = s + n - 16;
    char *first_d = d;
    char *last_d = d + n - 16;

    *((char16 *)first_d) = *((char16 *)first_s);
    *((char16 *)last_d) = *((char16 *)last_s);
    return dest;
  }

  const char *last_word_s = s + n - 32;
  char *last_word_d = d + n - 32;

  // Stamp the 32-byte chunks.
  do {
    *((char32 *)d) = *((char32 *)s);
    d += 32;
    s += 32;
  } while (d < last_word_d);

  // Stamp the last unaligned 32 bytes of the buffer.
  *((char32 *)last_word_d) = *((char32 *)last_word_s);
  return dest;
}