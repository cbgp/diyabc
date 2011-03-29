/*
 * conf.cpp
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


#ifndef PARTICLESET
#include "particleset.cpp"
#define PARTICLESET
#endif

extern ParticleSetC ps;
extern enregC* enreg;











    void doconf(char *options, bool multithread, int seed) {
        cout<<"debut de doconf\n";
        char *datafilename, *progressfilename, *courantfilename;
        int rtOK,nstatOK, iprog,nprog;
        int nrec,nsel,nseld,nselr,ns,ns1,nrecpos,ntest,np,ng,sc,npv,nlogreg;
        string opt,*ss,s,*ss1,s0,s1;
        double  *stat_obs,st,pa;
        string bidon;
        posteriorscenC **postsd,**postsr;
        
        bidon = new char[1000];
        FILE *flog, *fcur;
        progressfilename = new char[strlen(path)+strlen(ident)+20];
        strcpy(progressfilename,path);
        strcat(progressfilename,ident);
        strcat(progressfilename,"_progress.txt");
        courantfilename = new char[strlen(path)+strlen(ident)+20];
        strcpy(courantfilename,path);
        strcat(courantfilename,"courant.log");
        cout<<courantfilename<<"\n";
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
                cout <<"scenarios à tester : ";
                for (int j=0;j<rt.nscenchoisi;j++) {cout<<rt.scenchoisi[j]; if (j<rt.nscenchoisi-1) cout <<",";}cout<<"\n";
            } else if (s0=="r:") {
                rt.scenteste=atoi(s1.c_str());
                cout<<"scenario choisi pour le test : "<<rt.scenteste<<"\n";
             } else if (s0=="n:") {
                nrec=atoi(s1.c_str());
                if(nrec>nrecpos) nrec=nrecpos;
                cout<<"nombre total de jeux de données considérés (pour les scénarios choisis )= "<<nrec<<"\n";
           } else if (s0=="d:") {
                nseld=atoi(s1.c_str());    
                cout<<"nombre de jeux de données considérés pour l'approche directe = "<<nseld<<"\n";
            } else if (s0=="l:") {
                nselr=atoi(s1.c_str());    
                cout<<"nombre de jeux de données considérés pour la régression logistique = "<<nselr<<"\n";
           } else if (s0=="t:") {
                ntest=atoi(s1.c_str());
                cout<<"nombr de jeux de données tests"<<ntest<<"\n";
            } else if (s0=="m:") {
                nlogreg=atoi(s1.c_str());
                cout<<"nombre de régressions logistiques = "<<nlogreg<<"\n";
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
        npv = rt.nparam[rt.scenchoisi[0]-1];
        enreg = new enregC[ntest];
        for (int p=0;p<ntest;p++) {
            enreg[p].stat = new float[header.nstat];
            enreg[p].param = new float[npv];
            enreg[p].numscen = rt.scenchoisi[0];
        }
        ps.dosimultabref(header,ntest,false,multithread,true,rt.scenchoisi[0],seed);
        header.readHeader(headerfilename);cout<<"apres readHeader\n";
        
        nstatOK = rt.cal_varstat();                       //cout<<"apres cal_varstat\n";
        stat_obs = new double[rt.nstat];
        nsel=nseld;if(nsel<nselr) nsel=nselr;
        for (int p=0;p<ntest;p++) {
            for (int j=0;j<rt.nstat;j++) stat_obs[j]=enreg[p].stat[j];
            cout<<"avant cal_dist\n";
            rt.cal_dist(nrec,nsel,stat_obs);cout<<"apres cal_dist\n";
            postsd = comp_direct(nseld);
            cout<<"test n° "<<p+1;
            for (int i=0;i<rt.nscenchoisi;i++) cout<<"   "<< postsd[ncs-1][i].x;cout<<"\n";
            
        
        }    
    
    
    
    }