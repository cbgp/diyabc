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
extern string scurfile, path, ident, headerfilename,progressfilename;
extern HeaderC header;
extern ReftableC rt;
extern int ncs;
extern bool multithread;
extern int nparamcom;
extern int iprog,nprog;
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
        //long double *x = NULL, *mo = NULL;
        string s;
        //x=new long double[ntest];
        //mo=new long double[4];
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
        }
        f1<<"Historical parameters are drawn from the following priors and/or are given the following values : "<<shist<<"\n";
        f1<<"Mutation parameters are drawn from the following priors and/or are given the following values : "<<smut<<"\n";
        f1<<"Candidate scenarios : ";
        for (int i=0;i<rt.nscenchoisi;i++) {f1<<rt.scenchoisi[i];if (i<rt.nscenchoisi-1) f1<<", "; else f1<<"\n";}
        if (AFD) f1<<"Summary statistics have been replaced by components of a Linear Discriminant Analysis\n\n"; else 
			f1<<"Results obtained with plain summary statistics\n\n";
        //f1<<"         ");
		if (ntest>0){
			string aprdir,aprlog;
			f1<<"\nPeudo-observed data sets simulated with scenario "<<rt.scenteste<<" \n";
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
		}
        f1.close();
        //if(x != NULL) delete [] x;
        //if(mo != NULL) delete [] mo;
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
        //cout<<"apres AFD\n";
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
 * Appelle l'analyse factorielle discriminante et remplace les summary stats par les composantes des jeux simulés
 * sur les axes discriminants
 */
    float* transfAFD2(int nrec,int nsel, float* stat_obs) {
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
        //cout<<"apres AFD\n";
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
            for (int k=0;k<rt.nstat;k++) statpiv[j] += ((long double)stat_obs[k]-afd.moy[k])*afd.vectprop[k][j];
            }
//calcul des composantes pour le p-ème pods
        for (int j=0;j<afd.nlambda;j++) stat_obs[j] = statpiv[j];
//mise à zéro des stats au delà des composantes
        for (int j=afd.nlambda;j<rt.nstat;j++) {stat_obs[j] = 0.0;statpiv[j] = 0.0;}
// transfert dans la structure de retour        
        for (int j=0;j<rt.nstat;j++) sp[j]=float(statpiv[j]);
        delete []w;
        //delete []statpiv;
        for (int i=0;i<nsel;i++) delete []X[i];delete []X;
        return sp;
    }
 
	void traitenreg(string s, int nrec, int nseld, int nselr,int nrecp, int nlogreg, double *prop) {
 		float *stat_obs;
		int ncs1,nsel;
		nsel=nseld;if(nsel<nselr) nsel=nselr;
        posteriorscenC **postsd,*postsr;
		int ncdir,nclog;
		stat_obs = new float[rt.nstat];
		rt.alloue_enrsel(nsel);
		if (nlogreg==1) allouecmat(rt.nscen, nselr, rt.nstat);
		ncdir = nclog = 0;
		for (int p=0;p<nrecp;p++) {
			for (int j=0;j<rt.nstat;j++) stat_obs[j]=enreg[p].stat[j];
			cout<<"\nComputing "<<s<<" predictive error\njeu test "<<p+1<<"   nsel="<<nsel<<"     bon scenario="<<enreg[p].numscen<<"\n";
			rt.cal_dist(nrec,nsel,stat_obs,false,true);
			iprog +=6;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
			if (AFD) stat_obs = transfAFD(nrec,nsel,p);
			postsd = comp_direct(nseld);
			ncs1=ncs-1;
			int s=0;for (int i=1;i<rt.nscen;i++) {if (postsd[ncs1][i].x>postsd[ncs1][s].x) s=i;}
			if (s==enreg[p].numscen-1) ncdir++;
			cout<<"scenario estimé par l'approche directe : "<<s+1<<"\n";
			if (nlogreg==1) {
				postsr = comp_logistic(nselr,stat_obs);
				int s=0;for (int i=1;i<rt.nscen;i++) {if (postsr[i].x>postsr[s].x) s=i;}
				if (s==enreg[p].numscen-1) nclog++;
				cout<<"scenario estimé par l'approche logistique : "<<s+1<<"\n";
				iprog +=4;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
				cout<<"                                                       --->"<<setprecision(2)<<(double)(100*iprog)/(double)nprog<<" %\n";
				delete []postsd;
				delete []postsr;
			}
			cout<<"ncordir="<<ncdir<<"   ncorlog="<<nclog<<"\n";
		}
		rt.desalloue_enrsel(nsel);
		if (nlogreg==1) liberecmat(rt.nscen, nselr, rt.nstat);
		prop[0]=(double)ncdir/(double)nrecp;
		prop[1]=(double)nclog/(double)nrecp;
		delete [] stat_obs;
	}
 
	void pperror(bool prior, int nrec, int nrecp, int nsel0, int nseld, int nselr, int nlogreg, int seed,double *prop) {
		int nstatOK,ncdir,nclog,ns,nsel,*num;
		long double **matC,**phistar;
		double **stat;
		enreg = new enregC[nrecp];
		nstatOK = rt.cal_varstat();
		int nphistarOK;
		MwcGen mw;
		if (prior) {
			rt.openfile2();
			for (int p=0;p<nrecp;p++) {
				enreg[p].stat = new float[header.nstat];
				enreg[p].param = new float[rt.nparamax];
				enreg[p].numscen = rt.readparam(enreg[p].param);
			}
			rt.closefile();
			for (int iscen=0;iscen<rt.nscen;iscen++) {
				//cout<<rt.nparam[iscen]<<"\n";
				nphistarOK=0;
				for (int p=0;p<nrecp;p++) if (enreg[p].numscen-1==iscen) nphistarOK++;
				if (nphistarOK>0){
					phistarOK = new long double*[nphistarOK];
					stat = new double*[nphistarOK];
					ns=0;
					for (int p=0;p<nrecp;p++) if (enreg[p].numscen-1==iscen) {
						phistarOK[ns] = new long double[rt.nparam[iscen]];
						stat[ns] = new double[rt.nstat];
						for (int j=0;j<rt.nparam[iscen];j++) phistarOK[ns][j]=enreg[p].param[j];
						ns++;
					}
					//for (int j=0;j<rt.nparam[iscen];j++) cout<<phistarOK[0][j]<<"\n";
					//cout<<"pperror ns="<<ns<<" sur "<<nrecp<<" pour le scenario "<<iscen+1<<"\n";
					ps.dosimulstat(header,0,ns,false,multithread,iscen+1,seed,stat);
					ns=0;
					for (int p=0;p<nrecp;p++) if (enreg[p].numscen-1==iscen) {
						for (int j=0;j<rt.nstat;j++) enreg[p].stat[j] = stat[ns][j];
						delete [] phistarOK[ns];delete [] stat[ns];
						ns++;
					}
					delete [] phistarOK;delete [] stat;
				}
			}
			iprog+=9;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
			traitenreg("prior",nrec,nseld,nselr,nrecp,nlogreg,prop);
		} else {
			rt.alloue_enrsel(nsel0);
			rt.cal_dist(nrec,nsel0,&header.stat_obs[0],false,true);
			num = new int[nrecp];
			for (int i=0;i<nrecp;i++) num[i]=mw.rand0(nsel0);
			for (int p=0;p<nrecp;p++) {
				enreg[p].stat = new float[rt.nstat];
				enreg[p].param = new float[rt.nparamax];
				enreg[p].numscen = rt.enrsel[num[p]].numscen;
				for (int j=0;j<rt.nparam[enreg[p].numscen-1];j++) enreg[p].param[j] = rt.enrsel[num[p]].param[j];
				for (int j=0;j<rt.nstat;j++) enreg[p].stat[j] = rt.enrsel[num[p]].stat[j];
			}
			for (int iscen=0;iscen<rt.nscen;iscen++) {
				nphistarOK=0;
				for (int p=0;p<nrecp;p++) if (enreg[p].numscen-1==iscen) nphistarOK++;
				if (nphistarOK>0){
					phistarOK = new long double*[nphistarOK];
					stat = new double*[nphistarOK];
					ns=0;
					for (int p=0;p<nrecp;p++) if (enreg[p].numscen-1==iscen) {
						phistarOK[ns] = new long double[rt.nparam[iscen]];
						stat[ns] = new double[rt.nstat];
						for (int j=0;j<rt.nparam[iscen];j++) phistarOK[ns][j]=(long double)enreg[p].param[j];
						ns++;
					}
					ps.dosimulstat(header,0,ns,false,multithread,iscen+1,seed,stat);
					ns=0;
					for (int p=0;p<nrecp;p++) if (enreg[p].numscen-1==iscen) {
						for (int j=0;j<rt.nstat;j++) {enreg[p].stat[j]=(float)stat[ns][j];}
						delete [] phistarOK[ns];delete [] stat[ns];
						ns++;
					} 
					delete [] phistarOK;delete [] stat;
				}
			}
			rt.desalloue_enrsel(nsel0);
			iprog+=9;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
			traitenreg("posterior",nrec,nseld,nselr,nrecp,nlogreg,prop);
		}
		for (int p=0;p<nrecp;p++) {
			delete [] enreg[p].stat;
			delete [] enreg[p].param;
		}
		delete [] enreg;
	}
    
/**
 * Interprête la ligne de paramètres de l'option "confiance dans le choix du scénario" et lance les calculs correspondants
 */
    void doconf(string opt, int seed) {
        int nstatOK,ncs1,*nbestdir,*nbestlog,*scenchoibackup,nscenchoibackup;
        int nrec = 0,nreca, nsel=0,nsel0=0,nseld = 0,nselr = 0,ns,nr, nrecpos = 0,ntest = 0, np,ng,npv, nlogreg = 0, ncond,nrecb,nrecc;
		int ncordir,ncorlog,ncdir,nclog,ii,jj;
        string s,s0,s1;
        vector<string> ss, ss1;
		float *stat_obs;
		long double **matC;
		double duree,debut,clock_zero,**stat,prop[2],propcordirprior,propcorlogprior,propcordirposterior,propcorlogposterior;
        bool AFD=false,posterior=false;
		long double **phistar;
		int nphistarOK;
        posteriorscenC **postsd,*postsr;
        string shist,smut;
		nrecb=nrecc=0;
		clock_zero=0.0;debut=walltime(&clock_zero);
        //cout <<"debut de doconf\n";
        progressfilename = path + ident + "_progress.txt";
        //strcpy(progressfilename,path);
        //strcat(progressfilename,ident);
        //strcat(progressfilename,"_progress.txt");

		scurfile = path + "pseudo-observed_datasets_"+ ident +".txt";
        cout<<scurfile<<"\n";
        cout<<"options : "<<opt<<"\n";
        splitwords(opt,";",ss); ns = ss.size();
        for (int i=0;i<ns;i++) {
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);cout<<ss[i]<<"   s0="<<s0<<"s1="<<s1<<"\n";
            if (s0=="s:") {
                splitwords(s1,",",ss1); rt.nscenchoisi=ss1.size();
                rt.scenchoisi = new int[rt.nscenchoisi];
                for (int j=0;j<rt.nscenchoisi;j++) rt.scenchoisi[j] = atoi(ss1[j].c_str());
                nrecpos=0;for (int j=0;j<rt.nscenchoisi;j++) nrecpos +=rt.nrecscen[rt.scenchoisi[j]-1];
				nreca=nrecb=nrecc=nrecpos;
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
			} else if (s0=="b:") {
				nrecb=atoi(s1.c_str());
                if(nrecb>nrecpos) nrecb=nrecpos;
				cout<<"nombre total de jeux de données considérés pour le calcul de la prior predictive error= "<<nrecb<<"\n";
			}else if (s0=="c:") {
				nrecc=atoi(s1.c_str());
                if(nrecc>nrecpos) nrecc=nrecpos;
				cout<<"nombre total de jeux de données considérés pour le calcul de la posterior predictive error= "<<nrecc<<"\n";
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
                nsel0=atoi(s1.c_str());
                cout<<"nombre de jeux de données considérés pour la régression locale = "<<nsel0<<"\n";
            } else if (s0=="h:") {
                shist = s1;
                splitwords(s1," ",ss1); np = ss1.size();
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
                        header.scenario[rt.scenteste-1].condition = vector<ConditionC>(ncond);
                    }
                    for (int j=0;j<ncond;j++)
                         header.scenario[rt.scenteste-1].condition[j].readcondition(ss1[j+header.scenario[rt.scenteste-1].nparam]);
                }
            } else if ((s0=="u:")and(s1!="")) {
                smut = s1;
                cout<<s1<<"\n";
                splitwords(s1,"*",ss1); ng = ss1.size();
                if (ng != header.ngroupes) {
                    cout<<"le nombre de groupes transmis ("<<ng<<") est incorrect. Le nombre attendu  est de "<< header.ngroupes<<"\n";
                    //exit(1);
                }
                //cout<<"avant resetmutparam\n";
				
                for (int j=1;j<ng+1;j++) resetmutparam(ss1[j-1]);
                //cout<<"apres resetmutparam\n";
            } else if (s0=="f:") {
                AFD=(s1=="1");
                if (AFD) cout<<"Factorial Discriminant Analysis\n";
            } else if (s0=="po") {
				cout<<"paramètres tirés dans les posteriors\n";
				posterior=true;
			}
        }
        cout<<"fin de l'analyse de confpar\n";
        if (nlogreg==1){
			nprog=10*(ntest+nrecb+nrecc)+2;
			if (ntest>0) nprog +=9;if (nrecb>0) nprog +=9;if (nrecc>0) nprog +=9;
			iprog=1;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
		} else {
			nprog=6*(ntest+nrecb+nrecc)+2;
			if (ntest>0) nprog +=9;if (nrecb>0) nprog +=9;if (nrecc>0) nprog +=9;
			iprog=1;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
		}
		
//Calcul de la prior predictive error
		if (nrecb>0) {
			cout<<"\n---------------------------------------------------------------\n";
			cout<<"\nDebut du calcul de la prior predictive error  nrecb="<<nrecb<<"\n";
			nscenchoibackup = rt.nscenchoisi;
			scenchoibackup = new int[rt.nscenchoisi];
			for (int j=0;j<nscenchoibackup;j++) scenchoibackup[j]=rt.scenchoisi[j];
			rt.nscenchoisi = rt.nscen;
			delete [] rt.scenchoisi;
			rt.scenchoisi = new int[rt.nscen];
			for (int j=0;j<rt.nscen;j++) rt.scenchoisi[j]=j+1;
			pperror(true,nrec,nrecb,nsel0,nseld,nselr,nlogreg,seed,prop);
			propcordirprior=prop[0];
			propcorlogprior=prop[1];
			cout<<"\nProportion of times the scenario is correctly chosen\n";
			cout<<"Approche directe"<<setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<propcordirprior<<"\n";
			cout<<"Approche logistique"<<setiosflags(ios::fixed)<<setw(6)<<setprecision(3)<<propcorlogprior<<"\n";
			rt.nscenchoisi = nscenchoibackup;
			delete [] rt.scenchoisi;
			rt.scenchoisi = new int[rt.nscenchoisi];
			for (int j=0;j<rt.nscenchoisi;j++) rt.scenchoisi[j]=scenchoibackup[j];
			cout<<"---------------------------------------------------------------\n";
		}
//FIN du calcul de la prior predictive error
		
//DEBut du calcul de la posterior predictive error
		if (nrecc>0) {
			cout<<"\n---------------------------------------------------------------\n";
			cout<<"\nDebut du calcul de la posterior predictive error  nrecc="<<nrecc<<"\n";
			nscenchoibackup = rt.nscenchoisi;
			scenchoibackup = new int[rt.nscenchoisi];
			for (int j=0;j<nscenchoibackup;j++) scenchoibackup[j]=rt.scenchoisi[j]; 
			rt.nscenchoisi = rt.nscen;
			delete [] rt.scenchoisi;
			rt.scenchoisi = new int[rt.nscen];
			for (int j=0;j<rt.nscen;j++) rt.scenchoisi[j]=j+1;
			pperror(false,nrec,nrecc,nsel0,nseld,nselr,nlogreg,seed,prop);
			propcordirposterior=prop[0];
			propcorlogposterior=prop[1];
			cout<<"\nProportion of times the scenario is correctly chosen\n";
			cout<<"Approche directe"<<setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<propcordirposterior<<"\n";
			cout<<"Approche logistique"<<setiosflags(ios::fixed)<<setw(6)<<setprecision(3)<<propcorlogposterior<<"\n";
			rt.nscenchoisi = nscenchoibackup;
			delete [] rt.scenchoisi;
			rt.scenchoisi = new int[rt.nscenchoisi];
			for (int j=0;j<rt.nscenchoisi;j++) rt.scenchoisi[j]=scenchoibackup[j];
			cout<<"---------------------------------------------------------------\n";
		}
//FIN du calcul de la posterior predictive error
		ecrientete(nrec,ntest,nseld,nselr,nlogreg,shist,smut,AFD); //cout<<"apres ecrientete\n";
		ofstream f11(nomficonfresult.c_str(),ios::app);
		if (ntest>0) {
			cout<<ntest<<" test data sets\n";
			if (posterior) {
				//calcul des posteriors
				nsel=nsel0;
				nscenchoibackup = rt.nscenchoisi;
				scenchoibackup = new int[rt.nscenchoisi];
				for (int j=0;j<nscenchoibackup;j++) scenchoibackup[j]=rt.scenchoisi[j]; 
				rt.nscenchoisi=1;rt.scenchoisi = new int[rt.nscenchoisi];rt.scenchoisi[0]=rt.scenteste;
				cout<<"rt.nrec="<<rt.nrec<<"\n";
				nstatOK = rt.cal_varstat();                       cout<<"apres cal_varstat\n";
				cout<<"nrec="<<nrec<<"     nsel="<<nsel<<"\n";
				rt.alloue_enrsel(nsel);
				cout<<"avant le cal_dist de posterior\n";fflush(stdout);
			rt.cal_dist(nreca,nsel,&header.stat_obs[0],true,false);                  cout<<"apres cal_dist\n";
				det_numpar();
				cout<<"apres det_numpar\n";
			rempli_mat(nsel,&header.stat_obs[0]);                        cout<<"apres rempli_mat\n";
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
				delete [] rt.scenchoisi;
				rt.scenchoisi = new int[rt.nscenchoisi];
				for (int j=0;j<nscenchoibackup;j++) rt.scenchoisi[j]=scenchoibackup[j];
			}
			cout<<"\n---------------------------------------------------------------\n";
			cout<<"\nDebut du calcul de la confiance  pour le scenario "<<rt.scenchoisi[0]<<"\n";
			
			
			npv = rt.nparam[rt.scenteste-1];
			enreg = new enregC[ntest];
			for (int p=0;p<ntest;p++) {
				enreg[p].stat = new float[header.nstat];
				enreg[p].param = new float[npv];
				enreg[p].numscen = rt.scenteste;
			}
			nsel=nseld;if(nsel<nselr) nsel=nselr;
			if (posterior) {
				cout<<"\n\navant dosimulphistar\n";
				ps.dosimulphistar(header,ntest,false,multithread,true,rt.scenteste,seed,nphistarOK);
				cout<<"apres dosimulphistar\n";
			} else {
				cout<<"avant ps.dosimultabref\n";
				ps.dosimultabref(header,ntest,false,multithread,true,rt.scenteste,seed,2);
				cout<<"apres ps.dosimultabref\n";
			}
			iprog+=9;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
			//cout<<"apres ecriture dans progress\n";
			header.readHeader(headerfilename);//cout<<"apres readHeader\n";
			//nstatOK = rt.cal_varstat();
			stat_obs = new float[rt.nstat];
			rt.alloue_enrsel(nsel);
			if (nlogreg==1) allouecmat(rt.nscenchoisi, nselr, rt.nstat);
			nbestdir = new int[rt.nscenchoisi];nbestlog = new int[rt.nscenchoisi];
        vector<int> nbestlog(rt.nscenchoisi);
			for(int s=0;s<rt.nscenchoisi;s++){nbestdir[s]=0;nbestlog[s]=0;}
			nstatOK = rt.cal_varstat();
			for (int p=0;p<ntest;p++) {
				for (int j=0;j<rt.nstat;j++) stat_obs[j]=enreg[p].stat[j];
				cout<<"\nComputing confidence in scenario choice for scenario "<<rt.scenchoisi[0]<<"\n";
				cout<<"jeu test "<<p+1<<"\n";
				//cout<<"nstatOK="<<nstatOK<<"\n";
				rt.cal_dist(nrec,nsel,stat_obs,false,false);
				//cout<<"apres cal_dist\n";
				iprog +=6;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
				//cout<<"avant transfAFD\n";
				if (AFD) stat_obs = transfAFD(nrec,nsel,p);
				//if (ite==1) stat_obs = transfAFD(nrec,nsel,p);
				//cout<<"avant postsd\n";
				postsd = comp_direct(nseld);
				//cout<<"test"<<setiosflags(ios::fixed)<<setw(4)<<p+1<<" \n";
				if (p<9)  f11<<"    "<<(p+1); else if (p<99)  f11<<"   "<<(p+1); else if (p<999)  f11<<"  "<<(p+1);else  f11<<" "<<(p+1);
				f11<<"   ";
				ncs1=ncs-1;
				int s=0;for (int i=1;i<rt.nscenchoisi;i++) {if (postsd[ncs1][i].x>postsd[ncs1][s].x) s=i;}
				nbestdir[s]++;
				cout<<"scenario estimé par l'approche directe : "<<s+1<<"\n";
				for (int i=0;i<rt.nscenchoisi;i++) f11<< setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<postsd[ncs1][i].x;
				//for (int i=0;i<rt.nscenchoisi;i++) cout<< setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<postsd[ncs1][i].x;cout<<"\n";
				if (nlogreg==1) {
					postsr = comp_logistic(nselr,stat_obs);
					int s=0;for (int i=1;i<rt.nscenchoisi;i++) {if (postsr[i].x>postsr[s].x) s=i;}
					nbestlog[s]++;
					cout<<"scenario estimé par l'approche logistique : "<<s+1<<"\n";
					iprog +=4;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
					cout<<"                                                       --->"<<setprecision(2)<<(double)(100*iprog)/(double)nprog<<" %\n";
					for (int i=0;i<rt.nscenchoisi;i++) {
						//cout<<"  "<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<postsr[i].x;
						//cout<<" ["<<setiosflags(ios::fixed)<<setw(6)<<setprecision(4)<<postsr[i].inf;
						//cout<<","<<setiosflags(ios::fixed)<<setw(6)<<setprecision(4)<<postsr[i].sup<<"] ";
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
			}
			rt.desalloue_enrsel(nsel);
			if (nlogreg==1) liberecmat(rt.nscenchoisi, nselr, rt.nstat);
			f11<<"\nNumber of times the scenario has the highest posterior probability\nTotal  ";
			for (int i=0;i<rt.nscenchoisi;i++) f11<<setiosflags(ios::fixed)<<setw(9)<<nbestdir[i];
			if (nlogreg==1) {for (int i=0;i<rt.nscenchoisi;i++) f11<<setiosflags(ios::fixed)<<setw(17)<<nbestlog[i]<<"         ";}
		}
		f11<<"\n\nPrior predictive error (computed over "<<nrecb<<" data sets):\n";
		f11<<"Direct approach :   "<<fixed<<setw(9)<<setprecision(3)<<1.0-propcordirprior<<"\n";
		f11<<"Logistic approach : "<<fixed<<setw(9)<<setprecision(3)<<1.0-propcorlogprior<<"\n";
		
		f11<<"\n\nPosterior predictive error (computed over "<<nrecc<<" data sets):\n";
		f11<<"Direct approach :   "<<fixed<<setw(9)<<setprecision(3)<<1.0-propcordirposterior<<"\n";
		f11<<"Logistic approach : "<<fixed<<setw(9)<<setprecision(3)<<1.0-propcorlogposterior<<"\n";
		duree=walltime(&debut);
		f11<<"\nTotal duration ="<<TimeToStr(duree)<<"\n";
		
		f11.close();
        iprog +=1;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
		cout<<"durée ="<<TimeToStr(duree)<<"\n";
}
