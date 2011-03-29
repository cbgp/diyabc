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
char *nomficonfresult;

    void ecrientete(int nrec, int ntest,int nseld, int nselr,int nlogreg,bool usepriorhist, bool usepriormut,string shist,string smut) {
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        double *x,*mo;
        x=new double[ntest];
        mo=new double[4];
        nomficonfresult = new char[strlen(path)+strlen(ident)+20];
        strcpy(nomficonfresult,path);
        strcat(nomficonfresult,ident);
        strcat(nomficonfresult,"_conf.txt");
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
/*
data set  scen 1  scen 2  scen 3          scenario  1                         scenario  2                         scenario  3               
    1      1.000   0.000   0.000  0.99998783 [0.99997648,0.99999918]  0.00001120 [0.00000000,0.00002249]  0.00000097 [0.00000
Number of simulated data sets :3000000
Direct approach : number of selected data sets 500
Logistic regression  : number of selected data sets 30000
Results obtained with plain summary statistics
Peudo-observed data sets simulated with scenario 1 and parameter values drawn from priors : 
N1:UN[100,5000,0,0,1], N2:UN[100,5000,0,0,1], N3:UN[100,5000,0,0,1], N4:UN[100,5000,0,0,1], N5:UN[100,5000,0,0,1], N6:UN[100,5000,0,0,1], t4:UN[1,200,0,0,1], r1:UN[0.001,0.999,0.000,0.000,0.001], ta:UN[200,10000,0,0,1], t3:UN[200,10000,0,0,1], r2:UN[0.001,0.999,0.000,0.000,0.001], t2:UN[10000,100000,0,0,1], t1:UN[100000,500000,0,0,1].
Autosomaldiploidmicrosatellites, MEANMU:LU[1.00E-005,1.00E-003,-9,-9], GAMMU:GA[1.00E-005,1.00E-002,-9.00E+000,2.000], MEANP:UN[0.100,0.60,-9.00,-9.00], GAMP:GA[1.00E-002,9.00E-001,-9.00E+000,2.000], MEANSNILU[1.00E-008,1.00E-004,-9,-9], GAMSNI:GA[1.00E-009,1.00E-003,-9.00E+000,2.000].
Candidate scenarios : 1, 2, 3.*/
        
        
        
    void doconf(char *options, bool multithread, int seed) {
        char *datafilename, *progressfilename, *courantfilename;
        int rtOK,nstatOK, iprog,nprog;
        int nrec,nsel,nseld,nselr,ns,ns1,nrecpos,ntest,np,ng,sc,npv,nlogreg;
        string opt,*ss,s,*ss1,s0,s1;
        double  *stat_obs,st,pa;
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
        ps.dosimultabref(header,ntest,false,multithread,true,rt.scenchoisi[0],seed);
        header.readHeader(headerfilename);cout<<"apres readHeader\n";
        nstatOK = rt.cal_varstat();                       
        stat_obs = new double[rt.nstat];
        nsel=nseld;if(nsel<nselr) nsel=nselr;
        ecrientete(nrec,ntest,nseld,nselr,nlogreg,usepriorhist,usepriormut,shist,smut);
        ofstream f1(nomficonfresult,ios::app);
        for (int p=0;p<ntest;p++) {
            for (int j=0;j<rt.nstat;j++) stat_obs[j]=enreg[p].stat[j];
            rt.cal_dist(nrec,nsel,stat_obs);
            postsd = comp_direct(nseld);
            for (int i=0;i<rt.nscenchoisi;i++) printf(" %5.3f ",postsd[ncs-1][i].x);
            if (nlogreg==1) postsr = comp_logistic(nselr,stat_obs);
            for (int i=0;i<rt.nscenchoisi;i++) printf("  %6.4f [%6.4f,%6.4f] ",postsr[i].x,postsr[i].inf,postsr[i].sup);printf("\n");
            f1<<setw(5)<<setprecision(0)<<p+1;f1<<"   ";
            for (int i=0;i<rt.nscenchoisi;i++) f1<< setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<postsd[ncs-1][i].x;
            for (int i=0;i<rt.nscenchoisi;i++) f1<<"  "<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<postsr[i].x<<" ["<<setiosflags(ios::fixed)<<setw(6)<<setprecision(4)<<postsr[i].inf<<","<<setiosflags(ios::fixed)<<setw(6)<<setprecision(4)<<postsr[i].sup<<"]";
            f1<<"\n";
        }        
        f1.close();
    }