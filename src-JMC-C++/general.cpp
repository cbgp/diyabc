#include <iostream>
#include <string.h>
#include <unistd.h>
#include "simuldatC.cpp"
#include "reftable.cpp"



int main(int argc, char *argv[]){
	char *headerfilename, *reftablefilename,*datafilename;
	int nrecneeded,nrectodo,k,nenr=100;
	double **paramstat;
        enregC *enr;
	int optchar;
        ReftableC rt;
	
	while((optchar = getopt(argc,argv,"p:r:e:s:b:c:p:f:")) !=-1) {
	  switch (optchar) {
	
	    case 'p' : headerfilename = new char[strlen(optarg)+13];
		       reftablefilename = new char[strlen(optarg)+15];
		       strcpy(headerfilename,optarg);
		       strcpy(reftablefilename,optarg);
		       strcat(headerfilename,"/header.txt");
		       strcat(reftablefilename,"/reftable.bin");
		       cout<<headerfilename<<"   "<<reftablefilename<<"\n"; 
		       break;
		   
	    case 'r' : nrecneeded = atoi(optarg);
	               cout <<"nrecneeded = "<<nrecneeded<<"\n";
		       HeaderC header;
		       header.readHeader(headerfilename);
		       cout << header.dataobs.title << "\n nloc = "<<header.dataobs.nloc<<"   nsample = "<<header.dataobs.nsample<<"\n";fflush(stdin);
		       datafilename=strdup(header.datafilename.c_str());
                       ParticleSetC ps;
		       k=rt.readheader(reftablefilename,header.nscenarios,datafilename);
		       if (k==1) {
                           rt.datapath = datafilename;
                           rt.nrec=0;
                           rt.nrecscen = new int[header.nscenarios];
                           for (int i=0;i<header.nscenarios;i++) rt.nrecscen[i]=0;
                           rt.nparam = new int[header.nscenarios];
                           for (int i=0;i<header.nscenarios;i++) rt.nparam[i]=header.scenario[i].nparamvar;
                           rt.nstat=header.nstat;
                           rt.writeheader();
                       } else if (k==2) {cout<<"cannot create reftable file\n"; exit(1);} 
                       rt.openfile();
                       while (nrecneeded>rt.nrec) {
                               enr = ps.dosimultabref(header,nenr,false);
                               rt.writerecords(nenr,enr);
                               delete [] enr;
                       }
                       rt.closefile();
	               break;
	
	    }
	}
	
	return 0;
};
