#include <Rcpp.h>

// [[Rcpp::export]]
std::vector<int> sieve(bool original = true) {
  if(original){
    bool notPrimesArr[100007];
    std::vector<int> primes;
    notPrimesArr[0] = notPrimesArr[1] = 1;
    for (int i=2; i<1e5; ++i) {
      if (notPrimesArr[i]) continue;
      
      primes.push_back(i);
      for (int j=i*2; j<1e5; j += i) {
        notPrimesArr[j] = 1;
      }
    }
    return primes;
  }else{
    bool notPrimesArr[100007];
    std::vector<int> primes;
    for (int i=0; i<1e5; ++i) {
      notPrimesArr[i] = false;
    }
    notPrimesArr[0] = notPrimesArr[1] = true;
    for (int i=2; i<1e5; ++i) {
      if (notPrimesArr[i]) continue;
      
      primes.push_back(i);
      for (int j=i*2; j<1e5; j += i) {
        notPrimesArr[j] = true;
      }
    }
    return primes;
  }
}
