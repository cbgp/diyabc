/*
 * conf.cpp
 *
 *  Created on: 23 march 2011
 *      Author: cornuet
 */


#include <iomanip>
#include <cstdlib>
#include <vector>
#include <time.h>
#include "bias.h"
#include "reftable.h"
#include "header.h"
#include "matrices.h"
#include "mesutils.h"
#include "particleset.h"
#include "comparscen.h"
#include "estimparam.h"
#include "modchec.h"

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


extern ParticleSetC ps;
extern enregC* enreg;
double time_readfile=0.0;
extern string scurfile, path, ident, headerfilename;
extern HeaderC header;
extern ReftableC rt;
extern int ncs;
extern bool multithread;
extern int nparamcom;
extern ofstream fprog;
extern long double **phistarOK;
extern bool deltanul;

string nomficonfresult;

/**
 * Ecriture de l'entete du fichier confidence.txt contenant les résultats
 */
	void ecrientete(int nrec, int ntest,int nseld, int nselr,int nlogreg,string shist,string smut, bool AFD) {
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        long double *x = NULL, *mo = NULL;
        string s;
        x=new long double[ntest];
        mo=new long double[4];
        string aprdir,aprlog;
        nomficonfresult = path + ident + "_confidence.txt";
        //strcpy(nomficonfresult,path);
        //strcat(nomficonfresult,ident);
        //strcat(nomficonfresult,"_confidence.txt");
        cout <<nomficonfresult<<"\n";
		ofstream f1;
        f1.open(nomficonfresult.c_str());
        f1<<"DIYABC :                 Confidence in scenario choice                         "<<asctime(timeinfo)<<"\n";
        f1<<"Data file       : "<<header.datafilename<<"\n";
        f1<<"Reference table : "<<rt.filename<<"\n";
        f1<<"Number of simulated data sets : "<<nrec<<"\n";
        f1<<"Direct approach : number of selected data sets : "<<nseld<<"\n";
        if (nlogreg==1) {
            f1<<"Logistic regression  : number of selected data sets : "<<nselr<<"\n";
            f1<<"Results obtained with plain summary statistics\n";
        }
        f1<<"Peudo-observed data sets simulated with scenario "<<rt.scenteste<<" \n";
        f1<<"Historical parameters are drawn from the following priors and/or are given the following values : "<<shist<<"\n";
        f1<<"Mutation parameters are drawn from the following priors and/or are given the following values : "<<smut<<"\n";
        f1<<"Candidate scenarios : ";
        for (int i=0;i<rt.nscenchoisi;i++) {f1<<rt.scenchoisi[i];if (i<rt.nscenchoisi-1) f1<<", "; else f1<<"\n";}
        if (AFD) f1<<"Summary statistics have been replaced by components of a Linear Discriminant Analysis\n\n"; else f1<<"\n";
        //f1<<"         ");
        aprdir="direct approach";aprlog="logistic approach";
        s=centre(aprdir,9*rt.nscenchoisi);
        f1<<"data set "<<s;
        if (nlogreg>0) {
            s=centre(aprlog,26*rt.nscenchoisi);
            f1<<s;
        }
        f1<<"\n         ";
        for (int i=0;i<rt.nscenchoisi;i++) f1<<"  scen "<<setiosflags(ios::fixed)<<setw(3)<<rt.scenchoisi[i];
        if (nlogreg>0) for (int i=0;i<rt.nscenchoisi;i++) f1<<"        scenario "<<rt.scenchoisi[i]<<"       ";
        f1<<"\n";
        f1.close();
        if(x != NULL) delete [] x;
        if(mo != NULL) delete [] mo;
    }

/**
 * Appelle l'analyse factorielle discriminante et remplace les summary stats par les composantes des jeux simulés
 * sur les axes discriminants
 */
    float* transfAFD(int nrec,int nsel, int p) {
        long double delta,rdelta,*w,a,**X,*statpiv;
		float *sp;
        int *scenar;
        resAFD afd;
        //cout<<"debut transfAFD\n";
        delta=rt.enrsel[nsel-1].dist;
		w = new long double[nsel];
		if (delta>0.0){
			rdelta=1.5/delta;
			for (int i=0;i<nsel;i++) {a=rt.enrsel[i].dist/delta;w[i]=rdelta*(1.0-a*a);}
		} else for (int i=0;i<nsel;i++) w[i]=1.0;
        scenar = new int[nsel];for (int i=0;i<nsel;i++) scenar[i] = rt.enrsel[i].numscen;
        X = new long double*[nsel];for (int i=0;i<nsel;i++) X[i] = new long double[rt.nstat];
        statpiv = new long double[rt.nstat];
		sp = new float[rt.nstat];
        for (int i=0;i<nsel;i++) {for (int j=0;j<rt.nstat;j++) X[i][j]=(long double)rt.enrsel[i].stat[j];}
        //cout<<"avant AFD\n";
        afd = AFD(nsel,rt.nstat,scenar,w,X,1.0);
        //cout<<"apresAFD\n";
// remplacement des stat des jeux simulés par leurs composantes sur les axes discriminants
		for (int i=0;i<nsel;i++) {
//calcul des composantes pour le i-ème jeu
            for (int j=0;j<afd.nlambda;j++) {
                statpiv[j]=0.0;
                for (int k=0;k<rt.nstat;k++) statpiv[j] += ((long double)rt.enrsel[i].stat[k]-afd.moy[k])*afd.vectprop[k][j];
            }
//remplacement des composantes
            for (int j=0;j<afd.nlambda;j++) rt.enrsel[i].stat[j] = statpiv[j];
//mise à zéro des stats au delà des composantes
            for (int j=afd.nlambda;j<rt.nstat;j++) rt.enrsel[i].stat[j] = 0.0;
        }
// remplacement des stat du p_ième pods par ses composantes sur les axes discriminants
        for (int j=0;j<afd.nlambda;j++) {
            statpiv[j]=0.0;
            for (int k=0;k<rt.nstat;k++) statpiv[j] += ((long double)enreg[p].stat[k]-afd.moy[k])*afd.vectprop[k][j];
            }
//calcul des composantes pour le p-ème pods
        for (int j=0;j<afd.nlambda;j++) enreg[p].stat[j] = statpiv[j];
//mise à zéro des stats au delà des composantes
        for (int j=afd.nlambda;j<rt.nstat;j++) {enreg[p].stat[j] = 0.0;statpiv[j] = 0.0;}
// transfert dans la structure de retour        
        for (int j=0;j<rt.nstat;j++) sp[j]=float(statpiv[j]);
        delete []w;
        //delete []statpiv;
        for (int i=0;i<nsel;i++) delete []X[i];delete []X;
        return sp;
    }

/**
 * Interprête la ligne de paramètres de l'option "confiance dans le choix du scénario" et lance les calculs correspondants
 */
    void doconf(string opt, int seed) {
        string progressfilename;
        int nstatOK, iprog,nprog,ncs1,*nbestdir,*nbestlog,*scenchoibackup,nscenchoibackup;
        int nrec = 0,nreca, nsel=0,nseld = 0,nselr = 0,ns, nrecpos = 0,ntest = 0, np,ng,npv, nlogreg = 0, ncond;
        string *ss,s,*ss1,s0,s1; 
		float *stat_obs;
		long double **matC;
		double duree,debut,clock_zero;
        bool AFD=false,posterior=false;
			long double **phistar;
			int nphistarOK;
        posteriorscenC **postsd,*postsr;
        string shist,smut;
        //cout <<"debut de doconf\n";
        progressfilename = path + ident + "_progress.txt";
        //strcpy(progressfilename,path);
        //strcat(progressfilename,ident);
        //strcat(progressfilename,"_progress.txt");

		scurfile = path + "pseudo-observed_datasets_"+ ident +".txt";
        cout<<scurfile<<"\n";
        cout<<"options : "<<opt<<"\n";
        ss = splitwords(opt,";",&ns);
        for (int i=0;i<ns;i++) {
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);cout<<ss[i]<<"   s0="<<s0<<"s1="<<s1<<"\n";
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
              } else if (s0=="a:") {
                nreca=atoi(s1.c_str());
                if(nreca>nrecpos) nreca=nrecpos;
                cout<<"nombre total de jeux de données considérés (pour le calcul des posteriors)= "<<nreca<<"\n";
          } else if (s0=="d:") {
                nseld=atoi(s1.c_str());
                cout<<"nombre de jeux de données considérés pour l'approche directe = "<<nseld<<"\n";
            } else if (s0=="l:") {
                nselr=atoi(s1.c_str());
                cout<<"nombre de jeux de données considérés pour la régression logistique = "<<nselr<<"\n";
           } else if (s0=="t:") {
                ntest=atoi(s1.c_str());
                cout<<"nombre de jeux de données tests = "<<ntest<<"\n";
            } else if (s0=="m:") {
                nlogreg=atoi(s1.c_str());
                cout<<"nombre de régressions logistiques = "<<nlogreg<<"\n";
            } else if (s0=="z:") {
                nsel=atoi(s1.c_str());
                cout<<"nombre de jeux de données considérés pour la régression locale = "<<nsel<<"\n";
            } else if (s0=="h:") {
                shist = s1;
                ss1 = splitwords(s1," ",&np);
                if (np < header.scenario[rt.scenteste-1].nparam) {
                    //cout<<"le nombre de paramètres transmis ("<<np<<") est incorrect. Le nombre attendu pour le scénario "<<rt.scenteste<<" est de "<<header.scenario[rt.scenteste-1].nparam<<"\n";
                    cout<<"the number of parameter transmitted ("<<np<<") is incorrect. The expected number for scenario "<<rt.scenteste<<" is "<<header.scenario[rt.scenteste-1].nparam<<"\n";
                    exit(1);
                }
                ncond=np-header.scenario[rt.scenteste-1].nparam;
                for (int j=0;j<header.scenario[rt.scenteste-1].nparam;j++) resethistparam(ss1[j]);
                if (ncond>0) {
                  cout<<header.scenario[rt.scenteste-1].nconditions<<"\n";
                    if (header.scenario[rt.scenteste-1].nconditions != ncond) {
                        if (header.scenario[rt.scenteste-1].nconditions>0) delete []header.scenario[rt.scenteste-1].condition;
                        header.scenario[rt.scenteste-1].condition = new ConditionC[ncond];
                    }
                    for (int j=0;j<ncond;j++)
                         header.scenario[rt.scenteste-1].condition[j].readcondition(ss1[j+header.scenario[rt.scenteste-1].nparam]);
                }
            } else if ((s0=="u:")and(s1!="")) {
                smut = s1;
                cout<<s1<<"\n";
                ss1 = splitwords(s1,"*",&ng);
                if (ng != header.ngroupes) {
                    cout<<"le nombre de groupes transmis ("<<ng<<") est incorrect. Le nombre attendu  est de "<< header.ngroupes<<"\n";
                    //exit(1);
                }
                cout<<"avant resetmutparam\n";
				
                for (int j=1;j<ng+1;j++) resetmutparam(ss1[j-1]);
                cout<<"apres resetmutparam\n";
            } else if (s0=="f:") {
                AFD=(s1=="1");
                if (AFD) cout<<"Factorial Discriminant Analysis\n";
            } else if (s0=="po") {
				cout<<"paramètres tirés dans les posteriors\n";
				posterior=true;
			}
        }
        cout<<"fin de l'analyse de confpar\n";
       if (posterior) {
			//calcul des posteriors
			nscenchoibackup = rt.nscenchoisi;
			scenchoibackup = new int[rt.nscenchoisi];
			for (int j=0;j<nscenchoibackup;j++) scenchoibackup[j]=rt.scenchoisi[j]; 
			rt.nscenchoisi=1;rt.scenchoisi = new int[rt.nscenchoisi];rt.scenchoisi[0]=rt.scenteste;
			cout<<"rt.nrec="<<rt.nrec<<"\n";
			nstatOK = rt.cal_varstat();                       cout<<"apres cal_varstat\n";
			cout<<"nrec="<<nrec<<"     nsel="<<nsel<<"\n";
			rt.alloue_enrsel(nsel);
			cout<<"avant le cal_dist de posterior\n";fflush(stdout);
			rt.cal_dist(nreca,nsel,header.stat_obs,true);                  cout<<"apres cal_dist\n";
			det_numpar();
			cout<<"apres det_numpar\n";
			rempli_mat(nsel,header.stat_obs);                        cout<<"apres rempli_mat\n";
			if (not deltanul) matC = cal_matC(nsel); 
			recalparamO(nsel);                                 cout<<"apres recalparam\n";
			if (not deltanul) {
				rempli_parsim(nsel,nparamcom);            			cout<<"apres rempli_parsim(O)\n";
				local_regression(nsel,nparamcom,matC);              cout<<"apres local_regression\n";
			}
			phistar = new long double* [nsel];
			for (int i=0;i<nsel;i++) phistar[i] = new long double[nparamcom];
			if (not deltanul) calphistarO(nsel,phistar); else copyphistar(nsel,nparamcom,phistar); cout<<"apres calphistar\n";
			det_nomparam();
			//savephistar(nsel,path,ident,phistar,phistarcompo,phistarscaled);                     cout<<"apres savephistar\n";
			phistarOK = new long double*[nsel];
			for (int i=0;i<nsel;i++) phistarOK[i] = new long double[rt.nparam[rt.scenteste-1]];
			cout<<"header.scenario[rt.scenteste-1].nparam = "<<header.scenario[rt.scenteste-1].nparam<<"\n";
			nphistarOK=detphistarOK(nsel,phistar);               cout << "apres detphistarOK  nphistarOK="<<nphistarOK<<"\n";
			cout<< "   nphistarOK="<< nphistarOK<<"   nstat="<<header.nstat<<"\n";
			if(10*nphistarOK < ntest){
				cout << "Not enough suitable particles ("<<nphistarOK<<")to perform model checking. Stopping computations." << endl;
				exit(1);
			}
			rt.desalloue_enrsel(nsel);
			rt.nscenchoisi = nscenchoibackup;
			for (int j=0;j<nscenchoibackup;j++) rt.scenchoisi[j]=scenchoibackup[j];
		}
        npv = rt.nparam[rt.scenteste-1];
        enreg = new enregC[ntest];
        for (int p=0;p<ntest;p++) {
            enreg[p].stat = new float[header.nstat];
            enreg[p].param = new float[npv];
            enreg[p].numscen = rt.scenteste;
        }
        nsel=nseld;if(nsel<nselr) nsel=nselr;
        if (nlogreg==1){nprog=10*(ntest+1)+1;iprog=1;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();}
        else           {nprog=6*ntest+11;iprog=1;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();}
		if (posterior) {
			cout<<"avant dosimulphistar\n";
			ps.dosimulphistar(header,ntest,false,multithread,true,rt.scenteste,seed,nphistarOK);
			cout<<"apres dosimulphistar\n";
		} else {
			cout<<"avant ps.dosimultabref\n";
			ps.dosimultabref(header,ntest,false,multithread,true,rt.scenteste,seed,2);
			cout<<"apres ps.dosimultabref\n";
		}
        iprog=10;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
        cout<<"apres ecriture dans progress\n";
        header.readHeader(headerfilename);cout<<"apres readHeader\n";
        //nstatOK = rt.cal_varstat();
        stat_obs = new float[rt.nstat];
        ecrientete(nrec,ntest,nseld,nselr,nlogreg,shist,smut,AFD); cout<<"apres ecrientete\n";
        ofstream f11(nomficonfresult.c_str(),ios::app);
        rt.alloue_enrsel(nsel);
        if (nlogreg==1) allouecmat(rt.nscenchoisi, nselr, rt.nstat);
		nbestdir = new int[rt.nscenchoisi];nbestlog = new int[rt.nscenchoisi];
		for(int s=0;s<rt.nscenchoisi;s++){nbestdir[s]=0;nbestlog[s]=0;}
		nstatOK = rt.cal_varstat();
        for (int p=0;p<ntest;p++) {
            clock_zero=0.0;debut=walltime(&clock_zero);
            for (int j=0;j<rt.nstat;j++) stat_obs[j]=enreg[p].stat[j];
			cout<<"\n\njeu test "<<p+1<<"\n";
			//cout<<"nstatOK="<<nstatOK<<"\n";
            rt.cal_dist(nrec,nsel,stat_obs,false);
            //cout<<"apres cal_dist\n";
            iprog +=6;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
			//cout<<"avant transfAFD\n";
            if (AFD) stat_obs = transfAFD(nrec,nsel,p);
            //if (ite==1) stat_obs = transfAFD(nrec,nsel,p);
            //cout<<"avant postsd\n";
            postsd = comp_direct(nseld);
            cout<<"test"<<setiosflags(ios::fixed)<<setw(4)<<p+1<<"  ";
            if (p<9)  f11<<"    "<<(p+1); else if (p<99)  f11<<"   "<<(p+1); else if (p<999)  f11<<"  "<<(p+1);else  f11<<" "<<(p+1);
            f11<<"   ";
            ncs1=ncs-1;
			int s=0;for (int i=1;i<rt.nscenchoisi;i++) {if (postsd[ncs1][i].x>postsd[ncs1][s].x) s=i;}nbestdir[s]++;
            for (int i=0;i<rt.nscenchoisi;i++) f11<< setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<postsd[ncs1][i].x;
            for (int i=0;i<rt.nscenchoisi;i++) cout<< setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<postsd[ncs1][i].x;cout<<"\n";
            if (nlogreg==1) {
                postsr = comp_logistic(nselr,stat_obs);
				int s=0;for (int i=1;i<rt.nscenchoisi;i++) {if (postsr[i].x>postsr[s].x) s=i;}nbestlog[s]++;
                iprog +=4;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
				for (int i=0;i<rt.nscenchoisi;i++) {
					cout<<"  "<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<postsr[i].x;
					cout<<" ["<<setiosflags(ios::fixed)<<setw(6)<<setprecision(4)<<postsr[i].inf;
					cout<<","<<setiosflags(ios::fixed)<<setw(6)<<setprecision(4)<<postsr[i].sup<<"] ";
					f11<<"  "<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<postsr[i].x;
					f11<<" ["<<setiosflags(ios::fixed)<<setw(6)<<setprecision(4)<<postsr[i].inf;
					f11<<","<<setiosflags(ios::fixed)<<setw(6)<<setprecision(4)<<postsr[i].sup<<"] ";
				}
				if (postsr[0].err==1) {
					cout<<" Tikhonov regularisation of the Hessian matrix";
					f11<<" Tikhonov regularisation of the Hessian matrix";
				}
				if (postsr[0].err>1) {
					cout<<" WARNING : Computation of the logistic failed (error code="<<postsr[0].err<<". Results replaced by those of the direct approach";
					f11<<" WARNING : Computation of the logistic failed (error code="<<postsr[0].err<<"). Results replaced by those of the direct approach";
				}
                delete []postsd;
                delete []postsr;
            }
            f11<<"\n";f11.flush();
            cout<<"\n";
            duree=walltime(&debut);
            cout<<"durée ="<<TimeToStr(duree)<<"\n\n\n";
        }
        rt.desalloue_enrsel(nsel);
        if (nlogreg==1) liberecmat(rt.nscenchoisi, nselr, rt.nstat);
		f11<<"\nNumber of times the scenario has the highest posterior probability\nTotal  ";
		for (int i=0;i<rt.nscenchoisi;i++) f11<<setiosflags(ios::fixed)<<setw(9)<<nbestdir[i];
		if (nlogreg==1) {for (int i=0;i<rt.nscenchoisi;i++) f11<<setiosflags(ios::fixed)<<setw(17)<<nbestlog[i]<<"         ";}
        f11.close();
        iprog +=1;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
}
