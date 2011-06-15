#include <string>
#include <iostream>
#include "../lib/comp_vector.hpp"

using namespace std;

int main(int argc, char* argv[]){
  int n = 10000;
  if (argc >= 2){
    n = atoi(argv[1]);
  }
  cout << "n=" << n << endl;

  dag::comp_vector<string> cv;
  vector<string> origs;
  char buf[256];
  for (int i = 0; i < n; ++i){
    snprintf(buf, 256, "%d", rand() % 1000);
    cv.push_back(string(buf));
    origs.push_back(buf);
  }
  
  for (size_t i = 0; i < origs.size(); ++i){
    if (origs[i] != cv[i]){
      cout << "Error () i=" << i << " " << origs[i] << " " << cv[i] << endl;
    }
  }

  uint64_t pos = 0;
  for (dag::comp_vector<string>::const_iterator it = cv.begin(); it != cv.end(); ++it, ++pos){
    if (origs[pos] != *it){
      cout << "Error iter i=" << pos << " " << origs[pos] << " " << *it << endl;
    }
  }

  cout << "test passed." << endl;
  return 0;
}
