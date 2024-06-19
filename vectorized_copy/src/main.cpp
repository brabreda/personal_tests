#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include <immintrin.h>
#include <stdint.h>
#include <vector>

// same basic debug functions
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
  constexpr uint32_t n_bytes = 2560;
  std::vector<uint8_t> sour;
  FillWithRandomData(sour, n_bytes-100);

  std::vector<uint8_t> dest(2560, 0);
  std::vector<uint8_t> dest1(2560, 0);
  std::vector<uint8_t> dest2(2560, 0);

  std::memcpy(&dest[0], &sour[0], n_bytes-100);
  vec_copy(&dest1[0], &sour[0], n_bytes-100);
  vec_stream_copy(&dest2[0], &sour[0], n_bytes-100);

  PrintByteWithDiff(dest, dest1);
  PrintByteWithDiff(dest, dest2);

  return 0;
}