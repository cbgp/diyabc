/*
 * simfile.cpp
 *
 *  Created on: 20 september 2011
 *      Author: cornuet
 */
#ifndef HEADER
#include "header.cpp"
#define HEADER
#endif

#ifndef VECTOR
#include <vector>
#define VECTOR
#endif

#ifndef MATRICES
#include "matrices.cpp"
#define MATRICES
#endif

#ifndef MESUTILS
#include "mesutils.cpp"
#define MESUTILS
#endif


#ifndef PARTICLESET
#include "particleset.cpp"
#define PARTICLESET
#endif

extern ParticleSetC ps;
extern enregC* enreg;
extern bool multithread;
extern char *progressfilename;



void dosimfile(int seed){
  char *nomfigp;
  int  ntest;
  string opt,s,s0,s1,sg;
  string *sgp;
  string nomfisim;
  FILE *flog, *fgp;
  cout<<"path="<<path<<"\n";
  progressfilename = new char[strlen(path)+20];
  cout<<"apres dimension de progressfilename\n";
  strcpy(progressfilename,path);
  strcat(progressfilename,"progress.txt");

  nomfisim=header.datafilename;
  ntest = header.nsimfile;
  cout<<"nombre de fichiers genepop à simuler = "<<ntest<<"\n";
  sgp = ps.simulgenepop(header, ntest, multithread, seed);
  cout<<"apres les simulations\n";
  nomfigp = new char[strlen(path)+nomfisim.length()+20];
  for (int i=0;i<ntest;i++) {
    string sn=IntToString(i+1);
    if (i<9)  sn ="0"+sn;
    if (i<99) sn="0"+sn;
    sn="_"+sn;
    if (sgp[i]=="") cout<<"une erreur s'est produite lors de la simulation du fichier numero "<<i+1<<"\n";
    else {
      strcpy(nomfigp,path);
      strcat(nomfigp,nomfisim.c_str());
      strcat(nomfigp,sn.c_str());
      cout<<"écriture du fichier "<<nomfigp<<"\n";
      sgp[i] +="\n";
      fgp = fopen(nomfigp,"w");fprintf(fgp,"%s", sgp[i].c_str());fclose(fgp);
    }
  }
  flog=fopen(progressfilename,"w");fprintf(flog,"OK");fclose(flog);
}
