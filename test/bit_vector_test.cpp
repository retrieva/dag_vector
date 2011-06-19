#include <iostream>
#include "../lib/bit_vector.hpp"

using namespace std;

int main(int argc, char* argv[]){
  if (argc != 3){
    cerr << argv[0] << " num bitwidth" << endl;
    return -1;
  }
  uint64_t n = atoi(argv[1]);
  uint64_t w = atoi(argv[2]);

  cout << "num=" << n << " width=" << w << endl;
  dag::bit_vector bv;
  for (uint64_t i = 0; i < n; ++i){
    bv.push_back(i, w);
  }

  for (uint64_t i = 0; i < n; ++i){
    if (i != bv.get_bits(i * w, w)){
      cerr << "error i=" << i << " bv=" << bv.get_bits(i * w, w) << endl;
    }
  }

  return 0;
}
