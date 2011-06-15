#include <stdlib.h>
#include <iostream>
#include "../lib/dag_vector.hpp"

using namespace std;

int main(int argc, char* argv[]){
  int n = 10000;
  int vmax = 10000;
  if (argc >= 2){
    n = atoi(argv[1]);
  }
  if (argc >= 3){
    vmax = atoi(argv[2]);
  }
  cout << "n=" << n << " vmax=" << vmax << endl; 
  dag::dag_vector dagv;
  vector<uint64_t> vals;
  vector<uint64_t> sums;
  uint64_t sum = 0;
  for (int i = 0; i < n; ++i){
    uint64_t val = rand() % vmax;
    dagv.push_back(val);
    vals.push_back(val);
    sums.push_back(sum);
    sum += val;
  }
  
  if (dagv.size() != (size_t)n){
    cout << "Error dagv.size()=" << dagv.size() << " n=" << n << endl;
    return -1;
  }

  for (uint64_t i = vals.size()-1; i < vals.size(); ++i){
    if (dagv[i] != vals[i]){
      cout << "Error [] i=" << i << " dagv[i]=" << dagv[i] << " vals[i]=" << vals[i] << endl;
    }
    if (dagv.sum(i) != sums[i]){
      cout << "Error sum i=" << i << " dagv.sum(i)=" << dagv.sum(i) << " sums[i]=" << sums[i] << endl;
    }
  }
  
  size_t pos = 0;
  for (dag::dag_vector::const_iterator it = dagv.begin(); it != dagv.end(); ++it, ++pos){
    if (*it != vals[pos]){
      cout << "Error Iter i=" << pos << " dagv[i]=" << *it << " vals[i]=" << vals[pos] << endl;
    }
  }

  cout << "test passed." << endl;
  return 0;
}
