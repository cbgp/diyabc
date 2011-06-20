#include <iostream>
#include <algorithm>

int main(int argc, char *argv[]){
  
  double fTab [5] = {5.5, 6.4, 1.57, 1.58, 4.2};
  std::sort (&fTab [0], &fTab [4]);
  for (int i=0;i<5;i++) std::cout << fTab[i] <<"\n";
}