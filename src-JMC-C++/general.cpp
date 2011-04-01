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
#ifndef BIAS
#include "bias.cpp"
#define BIAS
#endif
#ifndef CONF
#include "conf.cpp"
#define CONF
#endif
#ifndef SYS_TIMEH
#include <sys/time.h>
#define SYS_TIMEH
#endif
#ifndef ACPLOC
#include "acploc.cpp"
#define ACPLOC
#endif

ReftableC rt;
HeaderC header;    
ParticleSetC ps;

char *headerfilename, *reftablefilename,*datafilename,*statobsfilename, *reftablelogfilename,*path,*ident;
bool multithread=false;
int nenr=100;

double clock_zero=0.0,debut,duree;

/**
* lecture du fichier header.txt, calcul des stat_obs et lecture de l'entête de reftable.bin' 
*/

int readheaders() {
    int k;
    header.readHeader(headerfilename);
    header.calstatobs(statobsfilename);
    datafilename=strdup(header.datafilename.c_str());
    k=rt.readheader(reftablefilename,reftablelogfilename,datafilename);
    return k;
}

/**
* programme principal : lecture et interprétation des options et lancement des calculs choisis
*/

int main(int argc, char *argv[]){
    char *estpar,*compar,*biaspar,*confpar,*priorpar;
    bool firsttime;
	int nrecneeded,nrectodo,k,seed;
	double **paramstat;
	int optchar;
    char action='a';
    bool flagp=false,flagi=false,flags=false;
       
        debut=walltime(&clock_zero);

	while((optchar = getopt(argc,argv,"i:p:r:e:s:b:c:q:f:g:d:hmq")) !=-1) {
         
	  switch (optchar) {

        case 'h' : cout << "USAGE :\n";
            cout << "-p <directory of header.txt>\n";
            cout << "-r <number of required data sets in the reftable>\n";
            cout << "-i <name given to the analysis\n";
            cout << "-g <minimum number of particles simulated in a single bunch (default=100)>\n";
            cout << "-m <multithreaded version of the program (default single threaded)\n";
            cout << "-q to merge all reftable_$j.bin \n";
            cout << "-s <seed for the random generator>\n";

            cout << "\n-e for ABC PARAMETER ESTIMATION (with parameters as a string including the following options separated par a semi-colon)\n";
            cout << "           s:<chosen scenario[s separated by a comma]>\n";
            cout << "           n:<number of simulated datasets taken from reftable>\n";
            cout << "           m:<number of simulated datasets used for the local regression>\n";
            cout << "           t:<number of the transformation (1,2,3 or 4)>\n";
            cout << "           p:<o for original, c for composite, oc for both>\n";

            cout << "\n-c for ABC COMPUTATION OF POSTERIOR PROBALITY OF SCENARIOS (idem)\n";
            cout << "           s:<chosen scenarios separated by a comma>\n";
            cout << "           n:<number of simulated datasets taken from reftable>\n";
            cout << "           d:<number of simulated datasets used in the direct approach>\n";
            cout << "           l:<number of simulated datasets used in the logistic regression>\n";
            cout << "           m:<number of requested logistic regressions>\n";
            
            cout << "\n-b for BIAS/PRECISION COMPUTATIONS (idem)\n";
            cout << "           s:<chosen scenario<\n";
            cout << "           n:<number of simulated datasets taken from reftable>\n";
            cout << "           m:<number of simulated datasets used for the local regression>\n";
            cout << "           t:<number of the transformation (1,2,3 or 4)>\n";
            cout << "           p:<o for original, c for composite, oc for both>\n";
            cout << "           d:<number of requested test data sets>\n";
            cout << "           h:<histparameter values/priors (see below)>\n";
            cout << "                histparameter values (separated by a space): <parameter name>=<parameter value>\n";
            cout << "                histparameter priors (separated by a space): <parameter name>=<parameter prior as in header.txt>\n";
            cout << "           u:<mutparameter values/priors for successive groups (see below)> groups are named G1, G2 and separated by a star : G1*G2-...\n";
            cout << "                mutparameter values/priors of a given as a set of 6 values/priors : Gx(vx1,vx2,vx3,vx4,vx5,vx6) with :\n";
            cout << "                vx1=<mean mutation rate/prior for group x>    vx2=<shape value/locus mutation rate prior for group x>\n";
            cout << "                vx3=<mean P value/mean P prior for group x>   vx4=<shape value/locus P prior for group x>\n";
            cout << "                vx5 and vx6 correspond to sni mutation rate.\n";
            cout << "                For a DNA sequence group, replace P and sni by k1 and k2 respectively\n";
            
            cout << "\n-f for CONFIDENCE IN SCENARIO CHOICE COMPUTATIONS (idem)\n";
            cout << "           s:<chosen scenarios among which to choose<\n";
            cout << "           r:<chosen scenario to be tested<\n";
            cout << "           n:<number of simulated datasets taken from reftable>\n";
            cout << "           t:<number of requested test data sets>\n";
            cout << "           d:<number of simulated datasets used in the direct approach>\n";
            cout << "           l:<number of simulated datasets used in the logistic regression>\n";
            cout << "           m:<number of requested logistic regressions>\n";
            cout << "           h:<histparameter values/priors (as in bias/precision)>\n";
            cout << "           u:<mutparameter values/priors for successive groups (as in bias/precision)\n";
            
            cout << "\n-d for ABC PRIOR/SCENARIO CHECKING (idem)\n";
            cout << "           a:<p for PCA, l for locate observed, pl for both>\n";
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
            break;
		   
        case 's' :  
            seed=atoi(optarg);
            flags=true;
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
                    
        case 'b' :  
            biaspar=strdup(optarg);
            action='b';
            break;
            
        case 'f' :
            confpar=strdup(optarg);
            action='f';
            break;
                    
        case 'd' :
            priorpar=strdup(optarg);
            action='d';
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
                     if (action=='b') ident=strdup("bias1");
                     if (action=='f') ident=strdup("conf1");
                     if (action=='d') ident=strdup("pcaloc1");
     }
     if (not flags) seed=time(NULL);	
	switch (action) {
    
      case 'r' :   k=readheaders(); 
                   cout << header.dataobs.title << "\n nloc = "<<header.dataobs.nloc<<"   nsample = "<<header.dataobs.nsample<<"\n";fflush(stdin);
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
                                          ps.dosimultabref(header,nenr,false,multithread,firsttime,0,seed);
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
                      break;
                      
      case 'e'  : k=readheaders();
                  if (k==1) {cout <<"no file reftable.bin in the current directory\n";exit(1);} 
                  doestim(estpar);
                  break;
                  
      case 'c'  : k=readheaders();
                  if (k==1) {cout <<"no file reftable.bin in the current directory\n";exit(1);} 
                  docompscen(compar);
                  break;
    
       case 'b'  : k=readheaders();
                  if (k==1) {cout <<"no file reftable.bin in the current directory\n";exit(1);} 
                  dobias(biaspar,seed);
                  break;
 
       case 'f'  : k=readheaders();
                  if (k==1) {cout <<"no file reftable.bin in the current directory\n";exit(1);} 
                  doconf(confpar,seed);
                  break;
       case 'd'  : k=readheaders();
                  if (k==1) {cout <<"no file reftable.bin in the current directory\n";exit(1);} 
                  doacpl(priorpar,multithread,seed);
                  break;
                  
   }
	duree=walltime(&debut);
    fprintf(stdout,"durée = %.2f secondes (%.2f)\n",duree,time_loglik);
     //fprintf(stdout,"durée dans le remplissage de matC = %.2f secondes\n",time_matC);
     //fprintf(stdout,"durée dans call_polytom = %.2f secondes\n",time_call);
     //fprintf(stdout,"durée dans la lecture de la reftable et le tri des enregistrements = %.2f secondes\n",time_readfile);
	return 0;
};
