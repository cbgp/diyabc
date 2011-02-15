#include <iostream>
#include <string.h>

#include "simuldatC.cpp"



int main(int argc, char *argv[]){
	char *myheaderfilename = new char[strlen(argv[1])+13];
	double **paramstat;
	std::cout << argv[1]<<"\n";
	//for (int i=0;i<strlen(argv[1]);i++) myheaderfinename[i]
	myheaderfilename=argv[1];
	strcat(myheaderfilename,"/header.txt");
	cout<<myheaderfilename<<"\n";
	HeaderC header;
	header.readHeader(myheaderfilename);
	cout << header.dataobs.title << "\n nloc = "<<header.dataobs.nloc<<"   nsample = "<<header.dataobs.nsample<<"\n";fflush(stdin);
	ParticleSetC ps;
	paramstat = ps.dosimultabref(header,1,true);
	return 0;
}
