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
#ifndef COMPARSCEN
#include "comparscen.cpp"
#define COMPARSCEN
#endif
#ifndef SYS_TIMEH
#include <sys/time.h>
#define SYS_TIMEH
#endif
#ifndef GLOBALH
#include "global.h"
#define GLOBALH
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


int readheaders() {
    int k;
    header.readHeader(headerfilename);
    header.calstatobs(statobsfilename);
    datafilename=strdup(header.datafilename.c_str());
    k=rt.readheader(reftablefilename,reftablelogfilename,datafilename);
    return k;
}

int main(int argc, char *argv[]){
    char *estpar,*compar;
    bool multithread=false,firsttime;
	int nrecneeded,nrectodo,k,seed;
	double **paramstat;
	int optchar;
    ParticleSetC ps;
    char action='a';
    bool flagp=false,flagi=false;
    //string a;
        //srand(time(NULL));
       
        debut=walltime(&clock_zero);

	while((optchar = getopt(argc,argv,"i:p:r:e:s:b:c:p:f:g:hmqs:")) !=-1) {
         
	  switch (optchar) {

        case 'h' : cout << "USAGE :\n";
            cout << "-p <directory of header.txt>\n-r <number of required data sets in the reftable>\n";
            cout << "-i <name given to the analysis\n";
            cout << "-g <minimum number of particles simulated in a single bunch (default=100)>\n";
            cout << "-m <multithreaded version of the program (default single threaded)\n";
            cout << "-e for ABC parameter estimation with parameters as defined below\n";
            cout << "-e s:<chosen scenarios separated by a comma>;n:<number of simulated datasets taken from reftable>;m:<number of simulated datasets used for the local regression>;t:<number of the transformation (1,2,3 or 4)>;p:<o for original, c for composite, oc for both>"<<"\n";
            cout << "-c for ABC computation of posterior probability of scenarios with parameters as defined below\n";
            cout << "-c s:<chosen scenarios separated by a comma;n:<number of simulated datasets taken from reftable>;d:<number of simulated datasets used in the direct approach>;l:<number of simulated datasets used in the logistic regression;m:<number of required logistic regressions>\n";
            cout << "-q to merge all reftable_$j.bin \n";
            cout << "-r for building/appending a reference table <required number of simulated datasets (default 10^6/scenario)>\n";
            cout << "-s <seed for the random generator>\n";
            break;
	
        case 'i' :
            ident=strdup(optarg);
            flagi=true;
            break;
        
        case 'p' : 
            headerfilename = new char[strlen(optarg)+13];
            reftablefilename = new char[strlen(optarg)+15];
            reftablelogfilename = new char[strlen(optarg)+15];
            statobsfilename = new char[strlen(optarg)+14];
            path = new char[strlen(optarg)+1];
            strcpy(path,optarg);
            strcpy(headerfilename,optarg);
            strcpy(reftablefilename,optarg);
            strcpy(reftablelogfilename,optarg);
            strcpy(statobsfilename,optarg);
            strcat(headerfilename,"header.txt");
            strcat(reftablefilename,"reftable.bin");
            strcat(reftablelogfilename,"reftable.log");
            strcat(statobsfilename,"statobs.txt");
            flagp=true;
            //cout<<headerfilename<<"\n"<<reftablefilename<<"\n"; 
            break;
		   
        case 's' :  
            seed=atoi(optarg);
            break;
            
        case 'r' :  
            nrecneeded = atoi(optarg);
            action='r';
            break;
                   
        case 'g' :  
            nenr = atoi(optarg);
            break;
          
        case 'm' :  
            multithread=true;
            break;

        case 'c' :  
            compar=strdup(optarg);
            action='c';
            break;        
                    
         case 'e' :  
            estpar=strdup(optarg);
            action='e';
            break;        
                    
       case 'q' : 
            header.readHeader(headerfilename);
            k=rt.readheader(reftablefilename,reftablelogfilename,datafilename);
            rt.concat();
            break;
	    }
	}
	 if (not flagp) {cout << "option -p is compulsory\n";exit(1);}
	 if (not flagi) {if (action=='e') ident=strdup("estim1");
                     if (action=='c') ident=strdup("compar1");
     }
	
	switch (action) {
    
      case 'r' :   k=readheaders(); 
                   cout << header.dataobs.title << "\n nloc = "<<header.dataobs.nloc<<"   nsample = "<<header.dataobs.nsample<<"\n";fflush(stdin);
                   //header.readHeader(headerfilename);
                   //cout<<"avant calstatobs\n";fflush(stdin);
                   //header.calstatobs(statobsfilename);
                   //cout << header.dataobs.title << "\n nloc = "<<header.dataobs.nloc<<"   nsample = "<<header.dataobs.nsample<<"\n";fflush(stdin);
                   //datafilename=strdup(header.datafilename.c_str());
                   //cout << datafilename<<"\n";
                   //k=rt.readheader(reftablefilename,reftablelogfilename,datafilename);
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
                                  rt.openfile();
                                  enreg = new enregC[nenr];
                                  for (int p=0;p<nenr;p++) {
                                      enreg[p].stat = new float[header.nstat];
                                      enreg[p].param = new float[header.nparamtot+3*header.ngroupes];
                                      enreg[p].numscen = 1;
                                  }
                                  //cout<<"nparammax="<<header.nparamtot+3*header.ngroupes<<"\n";
                                  firsttime=true;
                                  while (nrecneeded>rt.nrec) {
                                          ps.dosimultabref(header,nenr,false,rt.nrec,multithread,firsttime,seed);
                                          if (firsttime) firsttime=false;
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
                                  header.libere();
                                  
                          }
                      //delete []datafilename;
                      break;
                      
      case 'e'  : k=readheaders();
                  if (k==1) {cout <<"no file reftable.bin in the current directory\n";exit(1);} 
                  doestim(estpar,multithread);
                  //doestim(path,ident,headerfilename,reftablefilename,reftablelogfilename,statobsfilename,estpar,multithread);
                  break;
                  
      case 'c'  : k=readheaders();
                  if (k==1) {cout <<"no file reftable.bin in the current directory\n";exit(1);} 
                  docompscen(compar,multithread);
                  break;
    
    }
	//delete [] headerfilename;delete [] reftablefilename;
	duree=walltime(&debut);
    fprintf(stdout,"durée = %.2f secondes \n",duree);
    //cin >> a;
	return 0;
};
