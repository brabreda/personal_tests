#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>

#include <immintrin.h>
#include <stdint.h>

// same basic debug functionskm
void PrintRawBytes(const uint8_t* data, const size_t n_bytes){
  for(int i = 0; i<n_bytes; i++, data++){
    std::cout << std::setw(4) << static_cast<uint32_t>(*data);
    if(i && (i % 32 == 0)){ std::cout << std::endl; };

  }
}

void FillWithRandomData(std::vector<uint8_t> &v, const size_t n_bytes){
  std::srand(std::time(nullptr));
  for(int i = 0; i<n_bytes; i++){
    v.push_back(static_cast<uint8_t>(std::rand() & 0xff));
  }
}

void PrintByteWithDiff(std::vector<uint8_t> &v1, std::vector<uint8_t> &v2){
  auto it1 = v1.begin();
  auto it2 = v2.begin();
  
  for(int i = 0; it1<v1.end() && it2<v2.end(); i++, it1++, it2++){
    if(i && (i % 32 == 0)){ std::cout << std::endl; };
    std::cout << std::setw(4) << (*it1 == *it2 ? 0 : 1);   
  }
  std::cout << std::endl;
}

int main() {
    std::srand(std::time(nullptr));
    constexpr uint32_t n_bytes = 2560;
    std::vector<uint8_t> sour;
    std::vector<uint8_t> dest(2560);

    FillWithRandomData(sour, n_bytes);
    //PrintRawBytes(&sour[0], n_bytes);

    size_t nVec = n_bytes / sizeof(__m256i);
    __m256i* sVec = (__m256i*) (&sour[0]);
    __m256i* dVec = (__m256i*) (&dest[0]);
    for(;nVec> 0; nVec--, sVec++, dVec++) {
      const __m256i L2 = _mm256_loadu_si256(sVec);
      _mm256_storeu_si256(dVec, L2);
    }

    PrintByteWithDiff(sour, dest);

    return 0;
}