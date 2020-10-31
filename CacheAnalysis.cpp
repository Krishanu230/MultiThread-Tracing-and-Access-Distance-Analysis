#include "Cache.h"
#include <fstream>
#include <stdio.h>
using namespace std;
int main(int argc, char** argv){
  config L1config = {1,64,16,(2*1024*1024)}; //{level, blocksize, assoc, total cache size}
  Cache L1c(L1config);
  //L2c.Init();
  cout<< "-----------CACHE INFO-----------"<<endl;
  L1c.PrintConfig();
  cout<<"---------"<<endl;
  L1c.Init();
  //L1c.PrintStats();

  std::ifstream file(argv[1]);
  if(!file) {
    cout << "Cannot open input file.\n";
    return 1;
  }

  string str;
  while (getline(file, str)) {
    if (str=="#eof"){
      break;
    };
    string memAddrStr = str.substr(5);
    u_int64_t memAddr = stoull(memAddrStr);
    //cout<<memAddr<<endl;
    cacheStatus c1 = L1c.Read(memAddr);
    if (c1==MISS){
      //cout<<"miss for address"<<memAddr<<endl;
      L1c.Load(memAddr);
    }
    }
  L1c.PrintStats();

}
