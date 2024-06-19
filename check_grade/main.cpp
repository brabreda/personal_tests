#include <iostream>
#include <iomanip>
#include <vector>

#include <immintrin.h>
#include <stdint.h>


/* Check grade
 * A -> 90 +
 * B -> 80 +
 * C -> 70 +
 * D -> 60 +
 * F -> 0 - 60
*/

template <typename T> 
void _mm256_print( 
  const __m256i d
){
  const size_t n_i16 = sizeof(__m256i) / sizeof(T);
  const T * data =(T *) &d;
  for(int i = 0; i < n_i16; i++, data++){
    std::cout << std::setw(8) << *data;
  }
  std::cout << std::endl;
}

int main() {

  for(int i = 0; i <=100; i ++){
    std::cout << ((i * ((1<<15)/10) >> 15));

    const __m256i vec_grade_base_100 = _mm256_set1_epi16(i);
    // Add 5 to compensate for the rounding of _mm256_mulhrs_epi16
    const __m256i t1 = _mm256_set1_epi16(5); 
    const __m256i t2 = _mm256_add_epi16(vec_grade_base_100, t1);

    // multiplying by 32768/divisor gives you a divide by divisor
    const __m256i vec_mul = _mm256_set1_epi16(((1<<15)/10));
    const __m256i t3 = _mm256_mulhrs_epi16(t2, vec_mul);
    
    const uint16_t * data = (uint16_t *) &t3; 
    std::cout << std::setw(8) << *data << std::endl;
  }
  



  //_mm256_print<uint16_t>(vec_grade_base_10);


  // _mm256_print_i16(mul_vec);

  //__m256i vb = _mm256__mm256_set1_epi8( 10);
  return 0;
}