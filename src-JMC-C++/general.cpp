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
#ifndef SIMF
#include "simfile.cpp"
#define SIMF
#endif
#ifndef SYS_TIMEH
#include <sys/time.h>
#define SYS_TIMEH
#endif
#ifndef ACPLOC
#include "acploc.cpp"
#define ACPLOC
#endif
#ifndef MODCHEC
#include "modchec.cpp"
#define MODCHEC
#endif
#ifndef SYSSTATH
#include <sys/stat.h>
#define SYSSTATH
#endif

ReftableC rt;
HeaderC header;
ParticleSetC ps;
struct stat stFileInfo;

char *headerfilename, *reftablefilename,*datafilename,*statobsfilename, *reftablelogfilename,*path,*ident,*stopfilename, *progressfilename;
char *headersimfilename;
bool multithread=false;
int nrecneeded,nenr=100;
int debuglevel=0;
int num_threads=0;
string sremtime;
double clock_zero=0.0,debut,duree,debutf,dureef,time_file=0.0,time_reftable=0.0,debutr,dureer,remtime;

/**
* lecture du fichier header.txt, calcul des stat_obs et lecture de l'entête de reftable.bin
*/

int readheaders() {
    int k;
    if (debuglevel==1) cout<<"lecture des entêtes\n";
    header.readHeader(headerfilename);                                   if (debuglevel==1) cout<<"apres header.readHeader\n";
    header.calstatobs(statobsfilename);                                  if (debuglevel==1) cout <<"apres header.calstatobs\n";
    datafilename=strdup(header.datafilename.c_str());                    if (debuglevel==1) cout<<"datafile name : "<<header.datafilename<<"\n";
    k=rt.readheader(reftablefilename,reftablelogfilename,datafilename);  if (debuglevel==1) cout<<"apres rt.readheader k="<<k<<"\n";
    if (k==0) {rt.sethistparamname(header);if (debuglevel==1) cout<<"sethistparamname"<<"\n";}
    return k;
}

/**
* lecture du fichier headersim.txt
*/
int readheadersim() {
    int k;
	cout<<"avant header.readHeadersim    headersimfilename="<<headersimfilename<<"\n";
	header.readHeadersim(headersimfilename);
	return k;
}

/**
* programme principal : lecture et interprétation des options et lancement des calculs choisis
*/

int main(int argc, char *argv[]){
	try {
  
	char *estpar,*compar,*biaspar,*confpar,*priorpar,*testpar;
    bool firsttime;
	int k,seed;
	double **paramstat;
	int optchar;
    char action='a';
    bool flagp=false,flagi=false,flags=false,simOK,stoprun=false;
    string message;
    FILE *flog;

    debut=walltime(&clock_zero);
	while((optchar = getopt(argc,argv,"i:p:r:e:s:b:c:qkf:g:d:hmqj:a:t:")) !=-1) {

	  switch (optchar) {

        case 'h' : cout << "USAGE :\n";
            cout << "-p <directory of header.txt>\n";
            cout << "-r <number of required data sets in the reftable>\n";
            cout << "-i <name given to the analysis\n";
            cout << "-g <minimum number of particles simulated in a single bunch (default=100)>\n";
            cout << "-m <multithreaded version of the program\n";
            cout << "-q to merge all reftable_$j.bin \n";
            cout << "-s <seed for the random generator>\n";
            cout << "-t <required number of threads>\n";

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
            cout << "           f:<0 if logistic regression on SS, 1 if logistic regression on FDA components>\n";

            cout << "\n-d for ABC PRIOR/SCENARIO CHECKING (idem)\n";
            cout << "           a:<p for PCA, l for locate observed, pl for both>\n";

            cout << "\n-j for ABC MODEL CHECKING (idem)\n";
            cout << "           s:<chosen scenario[s separated by a comma]>\n";
            cout << "           n:<number of simulated datasets taken from reftable>\n";
            cout << "           m:<number of simulated datasets used for the local regression>\n";
            cout << "           q:<number of datasets simulated from posterior>\n";
            cout << "           t:<number of the transformation (1,2,3 or 4)>\n";
            cout << "           v:<list of summary stat names separated by a comma (if empty keep those of reftable)>\n";

            cout << "\n-k for SIMULATE GENEPOP DATA FILES\n";

           break;

        case 'a' :
            debuglevel=atoi(optarg);
            break;

        case 'i' :
            ident=strdup(optarg);
            flagi=true;
            break;

        case 'p' :
            headerfilename = new char[strlen(optarg)+13];
            headersimfilename = new char[strlen(optarg)+16];
            reftablefilename = new char[strlen(optarg)+15];
            reftablelogfilename = new char[strlen(optarg)+15];
            statobsfilename = new char[strlen(optarg)+14];
            stopfilename = new char[strlen(optarg)+13];
            path = new char[strlen(optarg)+1];
            strcpy(path,optarg);
            strcpy(headerfilename,optarg);
            strcpy(headersimfilename,optarg);
            strcpy(reftablefilename,optarg);
            strcpy(reftablelogfilename,optarg);
            strcpy(statobsfilename,optarg);
            strcpy(stopfilename,optarg);
            strcat(headerfilename,"header.txt");
            strcat(headersimfilename,"headersim.txt");
            strcat(reftablefilename,"reftable.bin");
            strcat(reftablelogfilename,"reftable.log");
            strcat(statobsfilename,"statobs.txt");
            strcat(stopfilename,".stop");
            flagp=true;
            if (stat(stopfilename,&stFileInfo)==0) remove(stopfilename);
			cout<<headersimfilename<<"\n";
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

        case 'k' :
            action='k';
            break;

        case 'q' :
            header.readHeader(headerfilename);
            k=rt.readheader(reftablefilename,reftablelogfilename,datafilename);
            rt.concat();
            break;

        case 'j' :
            estpar=strdup(optarg);
            action='j';
            break;

        case 't' :
            num_threads = atoi(optarg);
            multithread=true;
            break;

	    }
	}
	 if (not flagp) {cout << "option -p is compulsory\n";exit(1);}
	 if (not flagi) {if (action=='e') ident=strdup("estim1");
                     if (action=='c') ident=strdup("compar1");
                     if (action=='b') ident=strdup("bias1");
                     if (action=='f') ident=strdup("conf1");
                     if (action=='d') ident=strdup("pcaloc1");
                     if (action=='j') ident=strdup("modchec1");
     }
     if (not flags) seed=time(NULL);
     if (num_threads>0) omp_set_num_threads(num_threads);
	switch (action) {
      case 'r' :   k=readheaders();
                   cout << header.dataobs.title << "\n nloc = "<<header.dataobs.nloc<<"   nsample = "<<header.dataobs.nsample<<"   ";fflush(stdin);
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
                              rt.sethistparamname(header);
                          } else if (k==2) {cout<<"cannot create reftable file\n"; exit(1);}
                          if (nrecneeded>rt.nrec) {
                                  rt.openfile();
                                  enreg = new enregC[nenr];
                                  for (int p=0;p<nenr;p++) {
                                      enreg[p].stat = new float[header.nstat];
                                      enreg[p].param = new float[header.nparamtot+3*header.ngroupes];
                                      enreg[p].numscen = 1;
                                  }
                                  cout<<"nparammax="<<header.nparamtot+3*header.ngroupes<<"\n";
                                  firsttime=true;stoprun=false;
                                  debutr=walltime(&clock_zero);
                                  while ((not stoprun)and(nrecneeded>rt.nrec)) {
                                          //cout<<"avant dosimultabref rt.nrec="<<rt.nrec<<"\n";
                                          ps.dosimultabref(header,nenr,false,multithread,firsttime,0,seed,true,true);
                                          //cout<<"retour de dosimultabref\n";
                                          simOK=true;
                                          for (int i=0;i<nenr;i++) if (enreg[i].message!="OK") {simOK=false;message=enreg[i].message;}
                                          if (simOK) {
											  //cout<<"simOK=true   nenr="<<nenr<<"\n";
                                              //debutf=walltime(&clock_zero);
                                              rt.writerecords(nenr,enreg);
                                              //dureef=walltime(&debutf);time_file += dureef;
                                              rt.nrec +=nenr;
                                              cout<<rt.nrec;
                                              //if (firsttime) writecourant();
                                              //cout<<"à la place de writecourant\n";
                                              if (((rt.nrec%1000)==0)and(rt.nrec<nrecneeded))cout<<"   ("<<TimeToStr(remtime)<<")""\n"; else cout<<"\n";
                                              stoprun = (stat(stopfilename,&stFileInfo)==0);
                                              if (stoprun) remove(stopfilename);
                                          } else {
                                              flog=fopen(reftablelogfilename,"w");fprintf(flog,"%s",message.c_str());fclose(flog);
                                              stoprun=true;
                                          }
                                          if (firsttime) firsttime=false;
                                  }
                                  for (int i=0;i<nenr;i++) {
                                        delete [] enreg[i].param;
                                        delete [] enreg[i].stat;
                                  }
                                  delete [] enreg;
                                  rt.closefile();
                                  if (nrecneeded==rt.nrec) {ofstream f1(reftablelogfilename,ios::out);f1<<"END\n"<<rt.nrec<<"\n";f1.close();}
                                  header.libere();
                                  //exit(1);
                          } else {ofstream f1(reftablelogfilename,ios::out);f1<<"END\n\n";f1.close();}
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

       case 'k'  : k=readheadersim();
                  dosimfile(seed);
                  break;

       case 'd'  : k=readheaders();
                  if (k==1) {cout <<"no file reftable.bin in the current directory\n";exit(1);}
                  doacpl(priorpar,multithread,seed);
                  break;

       case 'j'  : k=readheaders();
                   if (k==1) {cout <<"no file reftable.bin in the current directory\n";exit(1);}
                   domodchec(estpar,seed);
                   break;

  }
	duree=walltime(&debut);
    cout<<"durée ="<<TimeToStr(duree)<<"\n";
    //int aaa;
    //cin>>aaa;
    //fprintf(stdout,"durée = %.2f secondes (%.6f)\n",duree,time_file);
     //fprintf(stdout,"durée dans le remplissage de matC = %.2f secondes\n",time_matC);
     //fprintf(stdout,"durée dans call_polytom = %.2f secondes\n",time_call);
     //fprintf(stdout,"durée dans la lecture de la reftable et le tri des enregistrements = %.2f secondes\n",time_readfile);
	return 0;
	  
	}
catch (exception& e)
  {
    cout << e.what() << endl;
  }
	
};
