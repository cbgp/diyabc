#ifndef IOSTREAM
#include <iostream>
#define IOSTREAM
#endif
#ifndef STRINGH
#include <string.h>
#define STRINGH
#endif
#ifndef UNISTDH
#include <unistd.h>
#define UNISTDH
#endif
#ifndef PARTICLESET
#include "particleset.cpp"
#define PARTICLESET
#endif
#ifndef REFTABLE
#include "reftable.cpp"
#define REFTABLE
#endif
#ifndef ESTIMPARAM
#include "estimparam.cpp"
#define ESTIMPARAM
#endif
#ifndef SYS_TIMEH
#include <sys/time.h>
#define SYS_TIMEH
#endif

int nenr=100;

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
	char *headerfilename, *reftablefilename,*datafilename,*statobsfilename;
    bool multithread=false;
	int nrecneeded,nrectodo,k;
	double **paramstat;
	int optchar;
        ReftableC rt;
	HeaderC header;
        ParticleSetC ps;
        //srand(time(NULL));
       
        debut=walltime(&clock_zero);

	while((optchar = getopt(argc,argv,"p:r:e:s:b:c:p:f:g:hm")) !=-1) {
         
	  switch (optchar) {

        case 'h' : cout << "USAGE :\n";
            cout << "-p <directory of header.txt>\n-r <number of required data sets in the reftable>\n";
            cout <<"-e for ABC parameter estimation with parameters as defined below\n";
            cout <<"-e s:<chosen scenarios separated by a comma>;n:<number of simulated datasets taken from reftable>;m:<number of simulated datasets used for the local regression>;t:<number of the transformation (1,2,3 or 4)>;p:<o for original, c for composite, oc for both>"<<"\n";
            break;
	
	    case 'p' : headerfilename = new char[strlen(optarg)+13];
		       reftablefilename = new char[strlen(optarg)+15];
                       statobsfilename = new char[strlen(optarg)+14];
		       strcpy(headerfilename,optarg);
		       strcpy(reftablefilename,optarg);
                       strcpy(statobsfilename,optarg);
		       strcat(headerfilename,"header.txt");
		       strcat(reftablefilename,"reftable.bin");
                       strcat(statobsfilename,"statobs.txt");
		       //cout<<headerfilename<<"\n"<<reftablefilename<<"\n"; 
		       break;
		   
	    case 'r' : nrecneeded = atoi(optarg);
	               cout <<"nrecneeded = "<<nrecneeded<<"\n";
		       header.readHeader(headerfilename);
                       //cout<<"avant calstatobs\n";fflush(stdin);
                       header.calstatobs(statobsfilename);
		       cout << header.dataobs.title << "\n nloc = "<<header.dataobs.nloc<<"   nsample = "<<header.dataobs.nsample<<"\n";fflush(stdin);
		       datafilename=strdup(header.datafilename.c_str());
                       //cout << datafilename<<"\n";
		       k=rt.readheader(reftablefilename,header.nscenarios,datafilename);
                       //cout<<header.scenario[0].nparamvar<<"    "<<header.scenario[1].nparamvar<<"    "<<header.scenario[2].nparamvar<<"\n";
                       //cout <<"k="<<k<<"\n";
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
                       if (nrecneeded>rt.nrec) {
                              ps.defined=false;
                              rt.openfile();
                              enreg = new enregC[nenr];
                              for (int p=0;p<nenr;p++) {
                                  enreg[p].stat = new float[header.nstat];
                                  enreg[p].param = new float[header.nparamtot+3*header.ngroupes];
                                  enreg[p].numscen = 1;
                              }
                              //cout<<"nparammax="<<header.nparamtot+3*header.ngroupes<<"\n";
                              while (nrecneeded>rt.nrec) {
                                      ps.dosimultabref(header,nenr,false,rt.nrec,multithread);
                                      rt.writerecords(nenr,enreg);
                                      rt.nrec +=nenr;
                                      cout<<rt.nrec<<"\n";
                              }
                              for (int i=0;i<nenr;i++) {
                                    delete [] enreg[i].param;
                                    delete [] enreg[i].stat;
                              }
                              delete [] enreg;
                              rt.closefile();
                       }
	               break;
                   
            case 'g' : nenr = atoi(optarg);
                       cout<<"minimum ="<<nenr;
                       break;
              
            case 'm' : 
                       multithread=true;
	               break;
            case 'e' : doestim(headerfilename,reftablefilename,statobsfilename,optarg);
                       
                       break;
        
	    }
	}
	
	duree=walltime(&debut);
        fprintf(stdout,"durée = %.2f secondes \n",duree);
	return 0;
};
