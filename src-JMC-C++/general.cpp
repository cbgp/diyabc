#include <iostream>
#include <string.h>
#include <unistd.h>
#include "simuldatC.cpp"
#include "reftable.cpp"
#include <sys/time.h>

double walltime( double *t0 )
{
  double mic, time;
  double mega = 0.000001;
  struct timeval tp;
  struct timezone tzp;
  static long base_sec = 0;
  static long base_usec = 0;

  (void) gettimeofday(&tp,&tzp);
  if (base_sec == 0)
    {
      base_sec = tp.tv_sec;
      base_usec = tp.tv_usec;
    }

  time = (double) (tp.tv_sec - base_sec);
  mic = (double) (tp.tv_usec - base_usec);
  time = (time + mic * mega) - *t0;
  return(time);
}

double clock_zero=0.0,debut,duree;

int main(int argc, char *argv[]){
	char *headerfilename, *reftablefilename,*datafilename;
	int nrecneeded,nrectodo,k,nenr=100;
	double **paramstat;
        enregC *enr;
	int optchar;
        ReftableC rt;
	srand(time(NULL));
        
        debut=walltime(&clock_zero);

	while((optchar = getopt(argc,argv,"p:r:e:s:b:c:p:f:h")) !=-1) {
         
	  switch (optchar) {

        case 'h' : cout << "USAGE :\n";
            cout << "-p directory_of_header.txt (choose the header directory)\n-r nb (indicate number of required data sets in the reftable)\n";
            break;
	
	    case 'p' : headerfilename = new char[strlen(optarg)+13];
		       reftablefilename = new char[strlen(optarg)+15];
		       strcpy(headerfilename,optarg);
		       strcpy(reftablefilename,optarg);
		       strcat(headerfilename,"header.txt");
		       strcat(reftablefilename,"reftable.bin");
		       cout<<headerfilename<<"\n"<<reftablefilename<<"\n"; 
		       break;
		   
	    case 'r' : nrecneeded = atoi(optarg);
	               cout <<"nrecneeded = "<<nrecneeded<<"\n";
		       HeaderC header;
		       header.readHeader(headerfilename);
                       //cout<<"avant calstatobs\n";fflush(stdin);
                       header.calstatobs();
		       cout << header.dataobs.title << "\n nloc = "<<header.dataobs.nloc<<"   nsample = "<<header.dataobs.nsample<<"\n";fflush(stdin);
		       datafilename=strdup(header.datafilename.c_str());
                       //cout << datafilename<<"\n";
                       ParticleSetC ps;
		       k=rt.readheader(reftablefilename,header.nscenarios,datafilename);
                       //cout<<header.scenario[0].nparamvar<<"    "<<header.scenario[1].nparamvar<<"    "<<header.scenario[2].nparamvar<<"\n";
                       cout <<"k="<<k<<"\n";
		       if (k==1) {
                           rt.datapath = datafilename;
                           rt.nscen = header.nscenarios;
                           rt.nrec=0;
                           rt.nrecscen = new int[header.nscenarios];
                           for (int i=0;i<header.nscenarios;i++) rt.nrecscen[i]=0;
                           rt.nparam = new int[header.nscenarios];
                           for (int i=0;i<header.nscenarios;i++) rt.nparam[i]=header.scenario[i].nparamvar;
                           rt.nstat=header.nstat;
                           rt.writeheader();
                       } else if (k==2) {cout<<"cannot create reftable file\n"; exit(1);} 
                       ps.defined=false;
                       rt.openfile();
                       while (nrecneeded>rt.nrec) {
                               enr = ps.dosimultabref(header,nenr,false);
                               //cout<<"avant writerecords\n";fflush(stdin);
                               rt.writerecords(nenr,enr);
                               rt.nrec +=nenr;
                               cout<<rt.nrec<<"\n";
                               for (int i=0;i<nenr;i++) {
                                   delete [] enr[i].param;
                               }
                       }
                       for (int i=0;i<nenr;i++) {
                            //delete [] enr[i].param;
                            delete [] enr[i].stat;
                       }
                       delete [] enr;
                       rt.closefile();
	               break;
	
	    }
	}
	
	duree=walltime(&debut);
        fprintf(stdout,"durée = %.2f secondes \n",duree);
	return 0;
};
