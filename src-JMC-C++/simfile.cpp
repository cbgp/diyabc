/*
 * simfile.cpp
 *
 *  Created on: 20 september 2011
 *      Author: cornuet
 */
#include <vector>
#include <string>

#include "header.h"
#include "reftable.h"
#include "matrices.h"
#include "mesutils.h"
#include "particleset.h"
/*
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
*/
#include "simfile.h"

extern ParticleSetC ps;
extern enregC* enreg;
extern bool multithread;
extern string progressfilename, path;
extern HeaderC header;



void dosimfile(int seed){
	string nomfigp;
	int  ntest;
	string opt,s,s0,s1,sg;
	string *sgp;
	string nomfisim;
	FILE *flog, *fgp;
	cout<<"path="<<path<<"\n";
	progressfilename =path + "progress.txt";
	nomfisim=header.datafilename;
	ntest = header.nsimfile;
	cout<<"nombre de fichiers genepop à simuler = "<<ntest<<"\n";
	if (header.dataobs.filetype==0) sgp = ps.simulgenepop(header, ntest, multithread, seed);
	if (header.dataobs.filetype==1) sgp = ps.simuldataSNP(header, ntest, multithread, seed);
	cout<<"apres les simulations\n";
	for (int i=0;i<ntest;i++) {
		string sn=IntToString(i+1);
		if (i<9)  sn ="0"+sn;
		if (i<99) sn="0"+sn;
		sn="_"+sn;
		if (sgp[i]=="") cout<<"une erreur s'est produite lors de la simulation du fichier numero "<<i+1<<"\n";
		else {
			nomfigp = path + nomfisim + sn;
			cout<<"écriture du fichier "<<nomfigp<<"\n";
			sgp[i] +="\n";
			fgp = fopen(nomfigp.c_str(),"w");fprintf(fgp,"%s", sgp[i].c_str());fclose(fgp);
		}
	}
	flog=fopen(progressfilename.c_str(),"w");fprintf(flog,"OK");fclose(flog);
}
