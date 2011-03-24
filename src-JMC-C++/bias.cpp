/*
 * bias.cpp
 *
 *  Created on: 23 march 2011
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

#ifndef GLOBALH
#include "global.h"
#define GLOBALH
#endif

using namespace std;


    void resethistparam(string s) {
        string *ss,name;
        int n,i;
        ss = splitwords(s,"=",&n);
        name=ss[0];
        i=0;while((i<header.scenario[rt.scenchoisi[0]-1].nparam)and(name != header.scenario[rt.scenchoisi[0]-1].histparam[i].name)) i++;
        cout<<"parametre "<<name<<"  ("<<i<<")   "<<ss[1]<<"\n";
        if (ss[1].find("[")!=string::npos) {
            header.scenario[rt.scenchoisi[0]-1].histparam[i].prior = header.readprior(ss[1]);
             header.scenario[rt.scenchoisi[0]-1].histparam[i].prior.ecris();
        }
        else {
            header.scenario[rt.scenchoisi[0]-1].histparam[i].value = atof(ss[1].c_str()) ;
        }
    }

    void resetmutparam(string s) {
        string *ss,numgr,s1;
        int n,i,gr,i0,i1;
        numgr = s.substr(1,s.find("(")-1);  gr=atoi(numgr.c_str());
        i0=s.find("(");i1=s.find(")"); cout <<"i0="<<i0<<"  i1="<<i1<<"\n";
        s1 = s.substr(i0+1,i1-i0-1); cout <<"groupe "<<gr<<"  "<<s1<<"\n";
        ss = splitwords(s1," ",&n);
        if (header.groupe[gr].type==0) {
 /*           if (ss[0].find("[")==string::npos) header.groupe[gr].mutmoy=atof(ss[0].c_str());
            else header.groupe[gr].priormutmoy = header.readprior(ss[0]);
            if (ss[1].find("[")==string::npos) header.groupe[gr].mutloc=atof(ss[1].c_str());
            else header.groupe[gr].priormutloc = header.readprior(ss[1]);
            if (ss[2].find("[")==string::npos) header.groupe[gr].Pmoy=atof(ss[2].c_str());
            else header.groupe[gr].priorPmoy = header.readprior(ss[2]);
             if (ss[3].find("[")==string::npos) header.groupe[gr].Ploc=atof(ss[3].c_str());
            else header.groupe[gr].priorPloc = header.readprior(ss[3]);
            if (ss[4].find("[")==string::npos) header.groupe[gr].mutmoy=atof(ss[4].c_str());
            else header.groupe[gr].priormutmoy = header.readprior(ss[4]);
            if (ss[5].find("[")==string::npos) header.groupe[gr].mutmoy=atof(ss[5].c_str());
            else header.groupe[gr].priormutmoy = header.readprior(ss[5]);*/
       }  
    }
    
    void dobias(char *options, bool multithread){
        char *datafilename, *progressfilename;
        int rtOK,nstatOK, iprog,nprog;
        int nrec,nsel,ns,ns1,nrecpos,ntest,np,ng;
        string opt,*ss,s,*ss1,s0,s1;
        double  *stat_obs;
       
        FILE *flog;
        progressfilename = new char[strlen(path)+strlen(ident)+20];
        strcpy(progressfilename,path);
        strcat(progressfilename,ident);
        strcat(progressfilename,"_progress.txt");

        cout<<"options : "<<options<<"\n";
        opt=char2string(options);
        ss = splitwords(opt,";",&ns);
        for (int i=0;i<ns;i++) { //cout<<ss[i]<<"\n";
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);
            if (s0=="s:") {
                ss1 = splitwords(s1,",",&rt.nscenchoisi);
                rt.scenchoisi = new int[rt.nscenchoisi];
                for (int j=0;j<rt.nscenchoisi;j++) rt.scenchoisi[j] = atoi(ss1[j].c_str());
                nrecpos=0;for (int j=0;j<rt.nscenchoisi;j++) nrecpos +=rt.nrecscen[rt.scenchoisi[j]-1];
                cout <<"scenario choisi : ";
                for (int j=0;j<rt.nscenchoisi;j++) {cout<<rt.scenchoisi[j]; if (j<rt.nscenchoisi-1) cout <<",";}cout<<"\n";
            } else if (s0=="n:") {
                nrec=atoi(s1.c_str());
                if(nrec>nrecpos) nrec=nrecpos;
                cout<<"nombre total de jeux de données considérés (pour le(s) scénario(s) choisi(s) )= "<<nrec<<"\n";
            } else if (s0=="m:") {
                nsel=atoi(s1.c_str());    
                cout<<"nombre de jeux de données considérés pour la régression locale = "<<nsel<<"\n";
            } else if (s0=="t:") {
                numtransf=atoi(s1.c_str()); 
                switch (numtransf) {
                  case 1 : cout <<" pas de transformation des paramètres\n";break;
                  case 2 : cout <<" transformation log des paramètres\n";break;
                  case 3 : cout <<" transformation logit des paramètres\n";break;
                  case 4 : cout <<" transformation log(tg) des paramètres\n";break;
                }
            } else if (s0=="p:") {
                original=(s1.find("o")!=string::npos);
                composite=(s1.find("c")!=string::npos);
                if (original) cout <<"paramètres  originaux  ";
                if ((s1=="oc")or(s1=="co")) cout <<"et ";
                if (composite) cout<<"paramètres  composite  ";
                cout<< "\n";
            } else if (s0=="d:") {
                ntest=atoi(s1.c_str());
                cout<<"nombre de jeux-tests à simuler = "<<ntest<<"\n";
            } else if (s0=="h:") {
                ss1 = splitwords(s1," ",&np);
                if (np != header.scenario[rt.scenchoisi[0]-1].nparam) {
                    cout<<"le nombre de paramètres transmis ("<<np<<") est incorrect. Le nombre attendu pour le scénario "<<rt.scenchoisi[0]<<" est de "<<header.scenario[rt.scenchoisi[0]-1].nparam<<"\n";
                    exit(1);
                }
                for (int j=0;j<np;j++) resethistparam(ss1[j]);
            } else if (s0=="u:") {
                cout<<s1<<"\n";
                ss1 = splitwords(s1,"-",&ng);
                if (ng != header.ngroupes) {
                    cout<<"le nombre de groupes transmis ("<<ng<<") est incorrect. Le nombre attendu  est de "<< header.ngroupes<<"\n";
                    //exit(1);
                }
                for (int j=0;j<ng;j++) resetmutparam(ss1[j]);
            }
        } 

    }