#include <iostream>
#include "../lib/dag_select.hpp"

using namespace std;

int main(int argc, char* argv[]){
  int n = 10000;
  int vmax = 100;
  if (argc >= 2){
    n = atoi(argv[1]);
  }
  if (argc >= 3){
    vmax = atoi(argv[2]);
  }

  cout << "n=" << n << " vmax=" << vmax << endl;

  dag::dag_select ds(n, vmax / 2 * n + n);
  uint64_t sum = 0;
  vector<uint64_t> results;
  vector<uint64_t> bv;
  for (int i = 0; i < n; ++i){
    int dif = rand() % vmax;
    sum += dif;
    ds.push_back(sum);
    results.push_back(sum);
    for (int j = 0; j < dif; ++j){
      bv.push_back(0);
    }
    bv.push_back(1);
    ++sum;
  }

  cout << " alloc_byte_num:" << ds.get_alloc_byte_num() << endl; 

  for (size_t i = 0; i < results.size(); ++i){
    if (results[i] != ds.select(i)){
      cout << "Error ds.select i=" << i 
           << " results[i]=" << results[i] 
           << " ds.select(i)=" << ds.select(i) << endl;  
    }
  }

  size_t i = 0;
  dag::dag_select::const_iterator end = ds.end();
  for (dag::dag_select::const_iterator it = ds.begin(); it != end; ++it, ++i){
    if (results[i] != *it){
      cout << "Error ds.iterator i=" << i 
           << " results[i]=" << results[i] 
           << " ds.select(i)=" << *it << endl;  
    }
  }

  for (size_t i = 0; i < bv.size(); ++i){
    if (bv[i] != ds.get_bit(i)){
      cout << "Error ds.get_bit i =" << i
           << " bv[i]=" << bv[i]
           << " ds.get_bit(i)=" << ds.get_bit(i) << endl;
    }
  }
  
  return 0;
}
