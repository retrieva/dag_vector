#include <vector>
#include <iostream>
#include <stdint.h>
#include "criterion.h"
#include "../lib/dag_vector.hpp"
#include "../lib/sparse_vector.hpp"

using namespace std;

int main(int argc, char* argv[]){
  if (argc != 3){
    cerr << argv[0] << " size maxval" << endl;
    return -1;
  }

  int size = atoi(argv[1]);
  int maxval = atoi(argv[2]);

  vector<uint64_t> vals;
  dag::dag_vector dagv;
  dag::bit_vector bv;
  dag::sparse_vector ds(size, maxval / 2 * size);
  uint64_t sum = 0;
  for (int i = 0; i < size; ++i){
    uint64_t v = rand() % maxval;
    sum += v;
    vals.push_back(sum);
    dagv.push_back(v);
    for (uint64_t j = 0; j < v; ++j){
      bv.push_back(0, 1);
    }
    bv.push_back(1, 1);
    ds.set_next_bit(sum);
  }

  cout << "vals:" << vals.size() * sizeof(uint64_t) << endl;
  cout << "dagv:" << dagv.get_alloc_byte_num() << endl;
  cout << "  bv:" << bv.get_alloc_byte_num() << endl;
  cout << "  ds:" << ds.get_alloc_byte_num() << endl;

  bgroup("sequeantial") {
    bench("vector"){
      for (size_t i = 0; i < vals.size(); ++i){
        sum += vals[i];
      }
    }

    bench("dag_vector []"){
      for (size_t i = 0; i < dagv.size(); ++i){
        sum += dagv[i];
      }
    }

    bench("dag_vector iterator"){
      dag::dag_vector::const_iterator end = dagv.end();
      for (dag::dag_vector::const_iterator it = dagv.begin(); it != end; ++it){
        sum += *it;
      }
    }

    bench("bit vector getbit"){
      for (size_t i = 0; i < bv.size(); ++i){
        if (bv.get_bit(i)){
          sum += i;
        }
      }
    } 

    bench("sparse_vector select"){
      for (size_t i = 0; i < ds.one_num(); ++i){
        sum += ds.select(i);
      }
    }

    bench("sparse_vector iterator"){
      dag::sparse_vector::const_iterator end = ds.end();
      dag::sparse_vector::const_iterator it = ds.begin();
      for (size_t i = 0; i < vals.size(); ++i, ++it){
        //for (; it != end; ++it){
        sum += *it;
      }
    }

  }


  

  return 0;
}
