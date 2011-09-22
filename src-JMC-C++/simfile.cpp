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



    void dosimfile(char *options,  int seed){
        char *datafilename, *progressfilename, *courantfilename, *nomfigp,*numero;
        int nstatOK, iprog,nprog;
        int nrec,nsel,ns,ns1,nrecpos,ntest,np,ng,sc,npv,nn,ncond;
        string opt,*ss,s,*ss1,s0,s1,sg;
        double  *stat_obs,st,pa;
       bool usepriorhist,usepriormut;
        string *sgp;
        string bidon;
        bidon = new char[1000];
        FILE *flog, *fcur, *fgp;

        progressfilename = new char[strlen(path)+20];
        strcpy(progressfilename,path);
        strcat(progressfilename,"_progress.txt");

        cout<<"options : "<<options<<"\n";
        opt=char2string(options);
        ss = splitwords(opt,";",&ns);
        for (int i=0;i<ns;i++) { //cout<<ss[i]<<"\n";
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);
            if (s0=="d:") {
                ntest=atoi(s1.c_str());
                cout<<"nombre de fichiers genepop à simuler = "<<ntest<<"\n";
            } else if (s0=="h:") {
                ss1 = splitwords(s1," ",&np);
                if (np < header.scenario[0].nparam) {
                    cout<<"le nombre de paramètres transmis ("<<np<<") est incorrect. Le nombre attendu pour le scénario est de "<<header.scenario[0].nparam<<"\n";
                    exit(1);
                }
                ncond=np-header.scenario[0].nparam;
                for (int j=0;j<header.scenario[0].nparam;j++) usepriorhist = resethistparam(ss1[j]);
                if (ncond>0) {
                  cout<<header.scenario[0].nconditions<<"\n";
                    if (header.scenario[0].nconditions != ncond) {
                        if (header.scenario[0].nconditions>0) delete []header.scenario[0].condition;
                        header.scenario[0].condition = new ConditionC[ncond];
                    }
                    for (int j=0;j<ncond;j++)
                         header.scenario[0].condition[j] = header.readcondition(ss1[j+header.scenario[0].nparam]);
                }
            } else if (s0=="u:") {
                cout<<s1<<"\n";
                ss1 = splitwords(s1,"*",&ng);
                if (ng != header.ngroupes) {
                    cout<<"le nombre de groupes transmis ("<<ng<<") est incorrect. Le nombre attendu  est de "<< header.ngroupes<<"\n";
                    //exit(1);
                }
                for (int j=0;j<ng;j++) usepriormut = resetmutparam(ss1[j]);
            }
        }
        sgp = ps.simulgenepop(header, ntest, multithread, seed, usepriormut);
        nomfigp = new char[strlen(path)+strlen(ident)+20];
        numero = new char[strlen(str2char(IntToString(ntest)))+2];
        numero[0] = '_';
        for (int i=0;i<ntest;i++) {
            string sn=IntToString(i+1);
            if (i<9)  sn = "0"+sn;
            if (i<99) sn="0"+sn;
            strcat(numero,str2char(sn));
            if (sgp[i]=="") cout<<"une erreur s'est produite lors de la simulation du fichier numero "<<i+1<<"\n";
            else {
                strcpy(nomfigp,path);
                strcat(nomfigp,ident);
                strcat(nomfigp,numero);
                fgp = fopen(nomfigp,"w");fprintf(fgp,"%s",str2char(sgp[i]));fclose(fgp);
            }
        }
   }
