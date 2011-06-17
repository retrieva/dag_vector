#include <vector>
#include <iostream>
#include <stdint.h>
#include "criterion.h"
#include "../lib/dag_vector.hpp"

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
  uint64_t sum = 0;
  for (int i = 0; i < size; ++i){
    uint64_t v = rand() % maxval;
    sum += v;
    vals.push_back(sum);
    dagv.push_back(v);
  }

  bgroup("sequeantial") {
    bench("vector"){
      for (size_t i = 0; i < vals.size(); ++i){
        sum += vals[i];
      }
    }

    bench("dag_vector"){
      for (size_t i = 0; i < dagv.size(); ++i){
        sum += dagv[i];
      }
    }

    dag::dag_vector::const_iterator end = dagv.end();
    bench("dag_vector"){
      for (dag::dag_vector::const_iterator it = dagv.begin(); it != end; ++it){
        sum += *it;
      }
    }
  }


  

  return 0;
}
