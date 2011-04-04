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

#ifndef IOMANIP
#include <iomanip>
#define IOMANIP
#endif


extern ParticleSetC ps;
extern enregC* enreg;
double time_readfile=0.0;

char *nomficonfresult;

    void ecrientete(int nrec, int ntest,int nseld, int nselr,int nlogreg,bool usepriorhist, bool usepriormut,string shist,string smut) {
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        double *x,*mo;
        x=new double[ntest];
        mo=new double[4];
        nomficonfresult = new char[strlen(path)+strlen(ident)+30];
        strcpy(nomficonfresult,path);
        strcat(nomficonfresult,ident);
        strcat(nomficonfresult,"_confidence.txt");
        cout <<nomficonfresult<<"\n";
        FILE *f1;
        f1=fopen(nomficonfresult,"w");
        fprintf(f1,"DIYABC :                 Confidence in scenario choice                         %s\n",asctime(timeinfo));
        fprintf(f1,"Data file       : %s\n",header.datafilename.c_str());
        fprintf(f1,"Reference table : %s\n",rt.filename);
        fprintf(f1,"Number of simulated data sets : %d\n",nrec);
        fprintf(f1,"Direct approach : number of selected data sets : %d\n",nseld);
        if (nlogreg==1) {
            fprintf(f1,"Logistic regression  : number of selected data sets : %d\n",nselr);
            fprintf(f1,"Results obtained with plain summary statistics\n");
        }
        fprintf(f1,"Peudo-observed data sets simulated with scenario %d \n",rt.scenteste);
        if (usepriorhist) {fprintf(f1,"Historical parameters are drawn from the following priors : ");}
        else  {fprintf(f1,"Historical parameters are given the following values :");}
        fprintf(f1,"%s\n",shist.c_str());
        if (usepriormut) {fprintf(f1,"Mutation parameters are drawn from the following priors : ");}
        else  {fprintf(f1,"Mutation parameters are given the following values :");}
        fprintf(f1,"%s\n",smut.c_str());
        fprintf(f1,"Candidate scenarios : ");
        for (int i=0;i<rt.nscenchoisi;i++) {fprintf(f1,"%d",rt.scenchoisi[i]);if (i<rt.nscenchoisi-1) fprintf(f1,", "); else fprintf(f1,"\n\n");}
        fprintf(f1,"data set ");
        for (int i=0;i<rt.nscenchoisi;i++) fprintf(f1,"  scen %2d",rt.scenchoisi[i]);
        for (int i=0;i<rt.nscenchoisi;i++) fprintf(f1,"        scenario %2d       ",rt.scenchoisi[i]);
        fprintf(f1,"\n");
        fclose(f1);
    }
        
    void doconf(char *options, int seed) {
        char *datafilename, *progressfilename, *courantfilename;
        int nstatOK, iprog,nprog,ncs1;
        int nrec,nsel,nseld,nselr,ns,ns1,nrecpos,ntest,np,ng,sc,npv,nlogreg;
        string opt,*ss,s,*ss1,s0,s1;
        double  *stat_obs,st,pa,duree,debut,clock_zero;
        bool usepriorhist,usepriormut;
        string bidon;
        posteriorscenC **postsd,*postsr;
        string shist,smut;
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
                cout<<"nombr de jeux de données tests = "<<ntest<<"\n";
            } else if (s0=="m:") {
                nlogreg=atoi(s1.c_str());
                cout<<"nombre de régressions logistiques = "<<nlogreg<<"\n";
            } else if (s0=="h:") {
                shist = s1;  
                ss1 = splitwords(s1," ",&np);
                if (np != header.scenario[rt.scenteste-1].nparam) {
                    cout<<"le nombre de paramètres transmis ("<<np<<") est incorrect. Le nombre attendu pour le scénario "<<rt.scenteste<<" est de "<<header.scenario[rt.scenteste-1].nparam<<"\n";
                    exit(1);
                }
                for (int j=0;j<np;j++) usepriorhist = resethistparam(ss1[j]);
            } else if (s0=="u:") {
                smut = s1;
                cout<<s1<<"\n";
                ss1 = splitwords(s1,"*",&ng);
                if (ng != header.ngroupes) {
                    cout<<"le nombre de groupes transmis ("<<ng<<") est incorrect. Le nombre attendu  est de "<< header.ngroupes<<"\n";
                    //exit(1);
                }
                for (int j=0;j<ng;j++) usepriormut = resetmutparam(ss1[j]);
            }
        }
        npv = rt.nparam[rt.scenchoisi[0]-1];
        enreg = new enregC[ntest];
        for (int p=0;p<ntest;p++) {
            enreg[p].stat = new float[header.nstat];
            enreg[p].param = new float[npv];
            enreg[p].numscen = rt.scenchoisi[0];
        }
        if (nlogreg==1){nprog=10*(ntest+1);iprog=1;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);}
        else           {nprog=6*ntest+10;iprog=1;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);}
        ps.dosimultabref(header,ntest,false,multithread,true,rt.scenchoisi[0],seed);
        iprog=10;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        header.readHeader(headerfilename);cout<<"apres readHeader\n";
        nstatOK = rt.cal_varstat();                       
        stat_obs = new double[rt.nstat];
        nsel=nseld;if(nsel<nselr) nsel=nselr;
        ecrientete(nrec,ntest,nseld,nselr,nlogreg,usepriorhist,usepriormut,shist,smut); cout<<"apres ecrientete\n";
        ofstream f11(nomficonfresult,ios::app);
        rt.alloue_enrsel(nsel);
        if (nlogreg==1) allouecmat(rt.nscenchoisi, nselr, rt.nstat);
        for (int p=0;p<ntest;p++) {
            for (int j=0;j<rt.nstat;j++) stat_obs[j]=enreg[p].stat[j];
            clock_zero=0.0;debut=walltime(&clock_zero);
            rt.cal_dist(nrec,nsel,stat_obs); 
            iprog +=6;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
            duree=walltime(&debut);time_readfile += duree;
            postsd = comp_direct(nseld);
            cout<<"test"<<setiosflags(ios::fixed)<<setw(4)<<p+1<<"  ";
            for (int i=0;i<rt.nscenchoisi;i++) f11<<setiosflags(ios::fixed)<<setw(5)<<setprecision(3)<<postsd[ncs-1][i].x;
            if (p<9)  f11<<"    "<<(p+1); else if (p<99)  f11<<"   "<<(p+1); else if (p<999)  f11<<"  "<<(p+1);else  f11<<" "<<(p+1);
            f11<<"   ";
            ncs1=ncs-1;
            for (int i=0;i<rt.nscenchoisi;i++) f11<< setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<postsd[ncs1][i].x;
            if (nlogreg==1) {
                postsr = comp_logistic(nselr,stat_obs);
                iprog +=4;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
                for (int i=0;i<rt.nscenchoisi;i++) printf("  %6.4f [%6.4f,%6.4f] ",postsr[i].x,postsr[i].inf,postsr[i].sup);printf("\n");
                for (int i=0;i<rt.nscenchoisi;i++) f11<<"  "<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<postsr[i].x<<" ["<<setiosflags(ios::fixed)<<setw(6)<<setprecision(4)<<postsr[i].inf<<","<<setiosflags(ios::fixed)<<setw(6)<<setprecision(4)<<postsr[i].sup<<"]";
                f11<<"\n";
                delete []postsd;
                delete []postsr;
            } else {
                cout<<"\n";
            }
        }        
        rt.desalloue_enrsel(nsel);
        f11.close();
    }