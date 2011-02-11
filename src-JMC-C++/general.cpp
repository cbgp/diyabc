#include <iostream>
#include <string.h>

#include "simuldatC.cpp"



int main(int argc, char *argv[]){
	char *myheaderfilename;
	double **paramstat;
	std::cout << argv[1]<<"\n";
	myheaderfilename = new char[strlen(argv[1])+12];
	strcat(myheaderfilename,"header.txt");
	HeaderC header;
	header.readHeader(myheaderfilename);
	cout << header.dataobs.title << "\n nloc = "<<header.dataobs.nloc<<"   nsample = "<<header.dataobs.nsample<<"\n";
	ParticleSetC ps;
	paramstat = ps.dosimultabref(header,1,true);
	return 0;
}
