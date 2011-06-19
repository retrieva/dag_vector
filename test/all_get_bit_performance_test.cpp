#include <vector>
#include <iostream>
#include <algorithm>
#include <stdint.h>
#include "criterion.h"
#include "../lib/sparse_vector.hpp"
#include "../lib/bit_vector.hpp"

using namespace std;

int main(int argc, char* argv[]){
  if (argc != 3){
    cerr << argv[0] << " size maxval" << endl;
    return -1;
  }

  int size = atoi(argv[1]);
  int maxval = atoi(argv[2]);

  vector<uint64_t> vals;
  dag::bit_vector bv;
  dag::dag_vector dv;
  dag::sparse_vector ds(size, (maxval-1) * size / 2 + size);
  cout << "estimated_sum:" << maxval / 2 * size + size << endl;
  uint64_t one_num = 0;
  uint64_t sum = 0;
  for (int i = 0; i < size; ++i){
    uint64_t v = rand() % maxval;
    sum += v;
    vals.push_back(sum);
    dv.push_back(v);
    ds.set_next_bit(sum);
    for (uint64_t j = 0; j < v; ++j){
      bv.push_back(0, 1);
    }
    one_num++;
    bv.push_back(1, 1);
    ++sum;
  }

  cout << "m * (1.44 + lg(n/m)) = " << size * (1.44 + log((float)sum / size) / log(2.f)) / 8.f << endl;
  uint64_t bv_len = sum;

  cout << "     size:" << size << " maxval:" << maxval << " one_num:" << one_num << " sum:" << sum << endl;
  cout << "     vals:" << vals.size() * sizeof(uint64_t) << endl;
  cout << "       bv:" << bv.get_alloc_byte_num() << endl;
  cout << "       ds:" << ds.get_alloc_byte_num() << endl;
  cout << "       dv:" << dv.get_alloc_byte_num() << endl;

  vector<uint64_t> queries;
  for (uint64_t i = 0; i < 10000; ++i){
    queries.push_back(rand() % bv_len);
  }

  bgroup("sequeantial") {
    bench("vector binary_search"){
      for (size_t i = 0; i < queries.size(); ++i){
        vector<uint64_t>::const_iterator it = lower_bound(vals.begin(), vals.end(), queries[i]);
        if (it != vals.end()) ++sum;
      }
    }

    bench("bit_vector get_bit"){
      for (size_t i = 0; i < queries.size(); ++i){
        sum += bv.get_bit(queries[i]);
      }
    }

    bench("sparse_vector get_bit"){
      for (size_t i = 0; i < queries.size(); ++i){
        sum += ds.get_bit(queries[i]);
      }
    }

  }


  

  return 0;
}
