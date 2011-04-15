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

char *headerfilename, *reftablefilename,*datafilename,*statobsfilename, *reftablelogfilename,*path,*ident,*stopfilename;
bool multithread=false;
int nrecneeded,nenr=100,niter=1000;
int debuglevel=0;
string sremtime;
double clock_zero=0.0,debut,duree,debutf,dureef,time_file=0.0,time_reftable=0.0,debutr,dureer,remtime;

/**
* lecture du fichier header.txt, calcul des stat_obs et lecture de l'entête de reftable.bin' 
*/

int readheaders() {
    int k;
    header.readHeader(headerfilename);                                   if (debuglevel==1) cout<<"apres header.readHeader\n";
    header.calstatobs(statobsfilename);                                  if (debuglevel==1) cout <<"apres header.calstatobs\n";
    datafilename=strdup(header.datafilename.c_str());                    if (debuglevel==1) cout<<"datafile name : "<<header.datafilename<<"\n";
    k=rt.readheader(reftablefilename,reftablelogfilename,datafilename);  if (debuglevel==1) cout<<"apres rt.readheader k="<<k<<"\n";
    return k;
}

/**
* programme principal : lecture et interprétation des options et lancement des calculs choisis
*/

int main(int argc, char *argv[]){
    char *dataname,*headername,*reftablename,aaa;
    bool firsttime;
	int k,seed,numtest;
	double **paramstat,hetmoy,hetheo,pidmoy,pidtheo;
	int optchar;
    char action='a';
    bool flagp=false,flagi=false,flags=false,simOK,stoprun=false;
    string message;
    FILE *flog;   
        
    debut=walltime(&clock_zero);
	while((optchar = getopt(argc,argv,"n:d:h:s:mp:")) !=-1) {
         
	  switch (optchar) {

        case 'n' : numtest = atoi(optarg);
                   break;
        
        case 'h' :headername = new char[strlen(optarg)+1];
                  strcpy(headername,optarg);
                  headerfilename = new char[strlen(path)+strlen(headername)+2];
                  strcpy(headerfilename,path);
                  strcat(headerfilename,headername);
                  cout<<"header = "<<headerfilename<<"\n";
                  break;
	
        case 'd' :dataname = new char[strlen(optarg)+1];
                  strcpy(dataname,optarg);
                  datafilename = new char[strlen(path)+strlen(dataname)+2];
                  strcpy(datafilename,path);
                  strcat(datafilename,dataname);
                  cout<<"data  = "<<datafilename<<"\n"; 
                  break;
           
        case 'p' :
                  headerfilename = new char[strlen(optarg)+13];
                  reftablefilename = new char[strlen(optarg)+15];
                  reftablelogfilename = new char[strlen(optarg)+15];
                  statobsfilename = new char[strlen(optarg)+14];
                  stopfilename = new char[strlen(optarg)+13];
                  path = new char[strlen(optarg)+1];
                  strcpy(path,optarg);
                  strcpy(headerfilename,optarg);
                  strcpy(reftablefilename,optarg);
                  strcpy(reftablelogfilename,optarg);
                  strcpy(statobsfilename,optarg);
                  strcpy(stopfilename,optarg);
                  strcat(headerfilename,"header.txt");
                  strcat(reftablefilename,"reftable.bin");
                  strcat(reftablelogfilename,"reftable.log");
                  strcat(statobsfilename,"statobs.txt");
                  strcat(stopfilename,".stop");
                  flagp=true;
                  if (stat(stopfilename,&stFileInfo)==0) remove(stopfilename);
                  break;
		   
        case 's' :seed=atoi(optarg);
                  flags=true;
                  break;
            
        case 'm' :multithread=true;
                  break;

	    }
	}
	 if (not flagp) {cout << "option -p is compulsory\n";exit(1);}
     if (not flags) seed=time(NULL);	
	switch (numtest) {
    
      case 1 :   k=readheaders(); 
                 if (k==1) {
                     rt.datapath = datafilename;
                     rt.nscen = header.nscenarios;
                     rt.nrec=0;
                     rt.nrecscen = new int[header.nscenarios];
                     for (int i=0;i<header.nscenarios;i++) rt.nrecscen[i]=0;
                     rt.nparam = new int[header.nscenarios];
                     for (int i=0;i<header.nscenarios;i++) rt.nparam[i]=header.scenario[i].nparamvar;
                     rt.nstat=header.nstat;
                 } else if (k==2) {cout<<"cannot create reftable file\n"; exit(1);}
                 enreg = new enregC[nenr];
                 for (int p=0;p<nenr;p++) {
                     enreg[p].stat = new float[header.nstat];
                     enreg[p].param = new float[header.nparamtot+3*header.ngroupes];
                     enreg[p].numscen = 1;
                                  }
                 cout<<"TEST DE l'HETEROZYGOTIE MOYENNE D'UN LOCUS SOUS SRICT SMM\n";
                 
                 hetheo=1.0-1.0/sqrt(1.04);pidtheo=0.9805595780769629;
                 cout<<"\ntheta=0.02"<<"\n";
                 firsttime=true;stoprun=false;
                 header.scenario[0].histparam[0].prior.mini=9.9;header.scenario[0].histparam[0].prior.maxi=10.1;
                 hetmoy=0.0;pidmoy=0.0;
                 for (k=0;k<niter;k++) { 
                     ps.dosimultabref(header,nenr,false,multithread,firsttime,0,seed,true,true);
                     firsttime=false;
                     seed +=nenr;
                     for (int i=0;i<nenr;i++) {hetmoy += enreg[i].stat[0];pidmoy += enreg[i].stat[1];}
                 }
                 hetmoy /= (nenr*niter);pidmoy /= (nenr*niter);
                 cout<<"hétérozygotie attendue = "<<hetheo<<"    hétérozygotie moyenne = "<<hetmoy;
                 if(abs(hetmoy-hetheo)<0.001) cout <<"   test OK\n"; else cout<<"   PROBLEME !!!\n";
                 cout<<"prob(identite) attendue = "<<pidtheo<<"    prob(identite) moyenne = "<<pidmoy;
                 if(abs(pidmoy-pidtheo)<0.001) cout <<"   test OK\n"; else cout<<"   PROBLEME !!!\n";
                 
                 hetheo=1.0-1.0/sqrt(1.4);pidtheo=0.845026245742127;
                 cout<<"\ntheta=0.2"<<"\n";
                 firsttime=true;stoprun=false;
                 header.scenario[0].histparam[0].prior.mini=99.9;header.scenario[0].histparam[0].prior.maxi=100.1;
                 hetmoy=0.0;pidmoy=0.0;
                 for (k=0;k<niter;k++) { 
                     ps.dosimultabref(header,nenr,false,multithread,firsttime,0,seed,true,true);
                     firsttime=false;
                     seed +=nenr;
                     for (int i=0;i<nenr;i++) {hetmoy += enreg[i].stat[0];pidmoy += enreg[i].stat[1];}
                 }
                 hetmoy /= (nenr*niter);pidmoy /= (nenr*niter);
                 cout<<"hétérozygotie attendue = "<<hetheo<<"    hétérozygotie moyenne = "<<hetmoy;
                 if(abs(hetmoy-hetheo)<0.001) cout <<"   test OK\n"; else cout<<"   PROBLEME !!!\n";
                 cout<<"prob(identite) attendue = "<<pidtheo<<"    prob(identite) moyenne = "<<pidmoy;
                 if(abs(pidmoy-pidtheo)<0.001) cout <<"   test OK\n"; else cout<<"   PROBLEME !!!\n";
                 
                 hetheo=1.0-1.0/sqrt(5);pidtheo=0.44707336192772973;
                 cout<<"\ntheta=2.0"<<"\n""         valeur attendue=1-1/sqrt(1+2theta) = "<<hetheo<<"\n";
                 firsttime=true;stoprun=false;
                 header.scenario[0].histparam[0].prior.mini=999.9;header.scenario[0].histparam[0].prior.maxi=1000.1;
                 hetmoy=0.0;pidmoy=0.0;
                 for (k=0;k<niter;k++) {  
                     ps.dosimultabref(header,nenr,false,multithread,firsttime,0,seed,true,true);
                     firsttime=false;
                     seed +=nenr;
                     for (int i=0;i<nenr;i++) {hetmoy += enreg[i].stat[0];pidmoy += enreg[i].stat[1];}
                 }
                 hetmoy /= (nenr*niter);pidmoy /= (nenr*niter);
                 cout<<"hétérozygotie attendue = "<<hetheo<<"    hétérozygotie moyenne = "<<hetmoy;
                 if(abs(hetmoy-hetheo)<0.001) cout <<"   test OK\n"; else cout<<"   PROBLEME !!!\n";
                 cout<<"prob(identite) attendue = "<<pidtheo<<"    prob(identite) moyenne = "<<pidmoy;
                 if(abs(pidmoy-pidtheo)<0.001) cout <<"   test OK\n"; else cout<<"   PROBLEME !!!\n";
                 
                  hetheo=1.0-1.0/sqrt(41);pidtheo=0.1561449771153935;
                 cout<<"\ntheta=20.0"<<"\n";
                 firsttime=true;stoprun=false;
                 header.scenario[0].histparam[0].prior.mini=9999.9;header.scenario[0].histparam[0].prior.maxi=10000.1;
                 hetmoy=0.0;pidmoy=0.0;
                 for (k=0;k<niter;k++) {  
                     ps.dosimultabref(header,nenr,false,multithread,firsttime,0,seed,true,true);
                     firsttime=false;
                     seed +=nenr;
                     for (int i=0;i<nenr;i++) {hetmoy += enreg[i].stat[0];pidmoy += enreg[i].stat[1];}
                 }
                 hetmoy /= (nenr*niter);pidmoy /= (nenr*niter);
                 cout<<"hétérozygotie attendue = "<<hetheo<<"    hétérozygotie moyenne = "<<hetmoy;
                 if(abs(hetmoy-hetheo)<0.001) cout <<"   test OK\n"; else cout<<"   PROBLEME !!!\n";
                 cout<<"prob(identite) attendue = "<<pidtheo<<"    prob(identite) moyenne = "<<pidmoy;
                 if(abs(pidmoy-pidtheo)<0.001) cout <<"   test OK\n"; else cout<<"   PROBLEME !!!\n";
              
                 break;
                      
                   
  }
	duree=walltime(&debut);
    fprintf(stdout,"durée = %.2f secondes (%.6f)\n",duree,time_file);
     //fprintf(stdout,"durée dans le remplissage de matC = %.2f secondes\n",time_matC);
     //fprintf(stdout,"durée dans call_polytom = %.2f secondes\n",time_call);
     //fprintf(stdout,"durée dans la lecture de la reftable et le tri des enregistrements = %.2f secondes\n",time_readfile);
	return 0;
};
