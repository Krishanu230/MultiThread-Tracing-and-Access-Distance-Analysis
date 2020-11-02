#include "Cache.h"
#include <fstream>
#include <stdio.h>
using namespace std;

int main(int argc, char** argv){
  config L1config = {1,64,16,(2*1024*1024)}; //{level, blocksize, assoc, total cache size}
  Cache L1c(L1config);
  cout<< "-----------CACHE INFO-----------"<<endl;
  L1c.PrintConfig();
  cout<<"---------"<<endl;
  L1c.Init();

  std::ifstream file(argv[1]);
  std::ofstream savefile("L1filtered.txt");
  //validation
  if(!file) {
    cout << "Cannot open input file.\n";
    return 1;
  }
  if(!savefile) {
    cout << "Cannot open output file.\n";
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
      //if its a miss also store the trace to the file as we need to filter out the L1 misses and ofc load the miss in the cache
      L1c.Load(memAddr);
      savefile<<str<<"\n";
    }
    }
  L1c.PrintStats();
  file.close();
  savefile<<"#eof\n";
  savefile.close();
}
