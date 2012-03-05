/*
 * modchec.cpp
 *
 *  Created on: 4 april 2011
 *      Author: cornuet
 */

#include <vector>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cmath>


#include "header.h"
#include "reftable.h"
#include "matrices.h"
#include "mesutils.h"
#include "particleset.h"
#include "acploc.h"
#include "estimparam.h"
/*
#ifndef HEADER
#include "header.cpp"
#define HEADER
#endif


#ifndef MATRICES
#include "matrices.cpp"
#define MATRICES
#endif

#ifndef MESUTILS
#include "mesutils.cpp"
#define MESUTILS
#endif
*/

using namespace std;

extern int debuglevel;
extern ParticleSetC ps;
extern HeaderC header;
extern ReftableC rt;
extern enregC* enreg;
extern int nenr, numtransf;
extern int nparamcom, nparcompo, nparscaled;
extern string progressfilename, statobsfilename, headerfilename, path, ident;
extern bool original, composite,scaled;
long double ** phistarOK;
extern string* stat_type;
extern int* stat_num;
extern bool multithread;
extern string scurfile;

long double **ssphistar,**ssref;

    bool resetstats(string s) {
		cout<<"debut de resetstats\n";
		cout<<s<<"\n";
        int j,ns,nq,nss1,gr,k=0;
        string *ss,*qq,*ss1;
		vector <StatsnpC> statsnp;
		StatsnpC stsnp;
		bool trouve;
		statsnp.resize(0);
		int nss,catsnp;
//comptage des stat
        for (gr=1;gr<=header.ngroupes;gr++) {header.groupe[gr].nstat=0;} header.nstat=0;
        cout<<header.ngroupes<<" groupe\n";
        ss =splitwords(s," ",&ns);
        if (debuglevel==2) cout<<"ns="<<ns<<"\n";
        for (int i=0;i<ns;i++) {
            qq=splitwords(ss[i],"_",&nq);
            if (nq!=3) return false;
            if (debuglevel==2) cout<<"nq="<<nq<<"   qq[1]="<<qq[1]<<"\n";
            gr=atoi(qq[1].c_str());
            if (debuglevel==2) cout<<"gr="<<gr<<"\n";
            header.groupe[gr].nstat++;
            header.nstat++;
            delete []qq;
       }
        if (debuglevel==2) cout <<"dans resetstat nstat = "<<header.nstat++<<"\n";

        for (gr=1;gr<=header.ngroupes;gr++) {delete []header.groupe[gr].sumstat;header.groupe[gr].sumstat = new StatC[header.groupe[gr].nstat];}
        ss =splitwords(s," ",&ns);
        delete []header.statname;header.statname = new string[ns];
        for (int i=0;i<ns;i++) header.statname[i]=ss[i];
        for (int i=0;i<ns;i++) {
            qq=splitwords(ss[i],"_",&nq);
            gr=atoi(qq[1].c_str());
            j=0;while (qq[0]!=stat_type[j]) j++;
            if (debuglevel==2) cout<<"\nss[i] = "<<ss[i]<<"   j="<<j<<"\n";
            if (header.groupe[gr].type==0) {   //MICROSAT
                if (stat_num[j]<5) {
                      header.groupe[gr].sumstat[k].cat=stat_num[j];
                      header.groupe[gr].sumstat[k].samp=atoi(qq[2].c_str());
                      k++;
                } else if (stat_num[j]<12) {
                      header.groupe[gr].sumstat[k].cat=stat_num[j];
                      ss1=splitwords(qq[2],"&",&nss1);
                      header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                      header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                      k++;
                      delete [] ss1;
                } else if (stat_num[j]==12) {
                      header.groupe[gr].sumstat[k].cat=stat_num[j];
                      ss1=splitwords(qq[2],"&",&nss1);
                      header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                      header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                      header.groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
                      k++;
                      delete [] ss1;
                }
            } else if (header.groupe[gr].type==1) {   //DNA SEQUENCE
                if (stat_num[j]>-5) {
                      header.groupe[gr].sumstat[k].cat=stat_num[j];
                      header.groupe[gr].sumstat[k].samp=atoi(qq[2].c_str());
                      k++;
                } else if (stat_num[j]>-14) {
                        header.groupe[gr].sumstat[k].cat=stat_num[j];
                        ss1=splitwords(qq[2],"&",&nss1);
                        header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                        header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                        k++;
                        delete [] ss1;
                } else if (stat_num[j]==-14) {
                        header.groupe[gr].sumstat[k].cat=stat_num[j];
                        ss1=splitwords(qq[2],"&",&nss1);
                        header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                        header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                        header.groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
                        k++;
                        delete [] ss1;
                }
            } else if (header.groupe[gr].type>9) {   //SNP
					catsnp = (stat_num[j]-21)/4;
					if (debuglevel==2) cout<<"snp catsnp="<<catsnp<<"   stat_num["<<j<<"]="<<stat_num[j]<<"\n";
                    if (stat_num[j]<25) {
						header.groupe[gr].sumstat[k].cat=stat_num[j];
						header.groupe[gr].sumstat[k].samp=atoi(qq[2].c_str());
						trouve=false;
						if (debuglevel==2) cout<<"statsnp.size="<<statsnp.size()<<"\n";
						if (statsnp.size()>0){
						for (size_t jj=0;jj<statsnp.size();jj++) {
								trouve=((statsnp[jj].cat==catsnp)and(statsnp[jj].samp==header.groupe[gr].sumstat[k].samp));
								if (trouve) {header.groupe[gr].sumstat[k].numsnp=jj;break;}
							}
						}
						if (debuglevel==2) cout<<"trouve="<<trouve<<"\n";
						if (not trouve) {
								stsnp.cat=catsnp;
								stsnp.samp=header.groupe[gr].sumstat[k].samp;
								stsnp.defined=false;
								stsnp.sorted=false;
								header.groupe[gr].sumstat[k].numsnp=statsnp.size();
								cout<<"statsnp["<<statsnp.size()<<"]   cat="<<stsnp.cat<<"   samp="<<stsnp.samp<<"\n";
								statsnp.push_back(stsnp);
						}
						k++;
                        
                    } else if ((stat_num[j]>24)and(stat_num[j]<33)) {
						header.groupe[gr].sumstat[k].cat=stat_num[j];
                        ss1=splitwords(qq[2],"&",&nss1);
                        header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                        header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
						trouve=false;
						if (debuglevel==2) cout<<"statsnp.size="<<statsnp.size()<<"\n";
						if (statsnp.size()>0){
						for (size_t jj=0;jj<statsnp.size();jj++) {
								trouve=((statsnp[jj].cat==catsnp)and(statsnp[jj].samp==header.groupe[gr].sumstat[k].samp)and(statsnp[jj].samp1==header.groupe[gr].sumstat[k].samp1));
								if (trouve) {header.groupe[gr].sumstat[k].numsnp=jj;break;}
							}
						}
						//cout<<"statsnp.size = "<<statsnp.size()<<"   trouve = "<<trouve<<"\n";
						if (debuglevel==2) cout<<"trouve="<<trouve<<"\n";
						if (not trouve) {
								stsnp.cat=catsnp;
								stsnp.samp=header.groupe[gr].sumstat[k].samp;
								stsnp.samp1=header.groupe[gr].sumstat[k].samp1;
								stsnp.defined=false;
								stsnp.sorted=false;
								header.groupe[gr].sumstat[k].numsnp=statsnp.size();
								cout<<"statsnp["<<statsnp.size()<<"]   cat="<<stsnp.cat<<"   samp="<<stsnp.samp<<"   samp1="<<stsnp.samp1<<"\n";
								statsnp.push_back(stsnp);
						}
						//cout<<"numsnp = "<<statsnp.size()<<"\n";
						delete [] ss1;
						k++;
                        
                    } else if (stat_num[j]>32) {
						header.groupe[gr].sumstat[k].cat=stat_num[j];
                        ss1=splitwords(qq[2],"&",&nss1);
                        header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                        header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                        header.groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
						trouve=false;
						if (debuglevel==2) cout<<"statsnp.size="<<statsnp.size()<<"\n";
						if (statsnp.size()>0){
						for (size_t jj=0;jj<statsnp.size();jj++) {
								trouve=((statsnp[jj].cat==catsnp)and(statsnp[jj].samp==header.groupe[gr].sumstat[k].samp)and(statsnp[jj].samp1==header.groupe[gr].sumstat[k].samp1)and(statsnp[jj].samp2==header.groupe[gr].sumstat[k].samp2));
								if (trouve) {header.groupe[gr].sumstat[k].numsnp=jj;break;}
							}
						}
						if (debuglevel==2) cout<<"trouve="<<trouve<<"\n";
						if (not trouve) {
								stsnp.cat=catsnp;
								stsnp.samp=header.groupe[gr].sumstat[k].samp;
								stsnp.samp1=header.groupe[gr].sumstat[k].samp1;
								stsnp.samp2=header.groupe[gr].sumstat[k].samp2;
								stsnp.defined=false;
								stsnp.sorted=false;
								header.groupe[gr].sumstat[k].numsnp=statsnp.size();
								cout<<"statsnp["<<statsnp.size()<<"]   cat="<<stsnp.cat<<"   samp="<<stsnp.samp<<"   samp1="<<stsnp.samp1<<"   samp2="<<stsnp.samp2<<"\n";
								statsnp.push_back(stsnp);
						}
						delete [] ss1;
						k++;
                    }
			}
            delete []qq;
        }
		header.groupe[gr].nstatsnp=statsnp.size();
		cout<<"header.groupe[gr].nstatsnp="<<header.groupe[gr].nstatsnp<<"\n";
		if (header.groupe[gr].nstatsnp>0){
			header.groupe[gr].sumstatsnp = new StatsnpC[header.groupe[gr].nstatsnp];
			for (int i=0;i<header.groupe[gr].nstatsnp;i++){
				header.groupe[gr].sumstatsnp[i].cat=statsnp[i].cat;
				header.groupe[gr].sumstatsnp[i].samp=statsnp[i].samp;
				header.groupe[gr].sumstatsnp[i].samp1=statsnp[i].samp1;
				header.groupe[gr].sumstatsnp[i].samp2=statsnp[i].samp2;
				header.groupe[gr].sumstatsnp[i].defined=statsnp[i].defined;
				header.groupe[gr].sumstatsnp[i].sorted=statsnp[i].sorted;
			}
			statsnp.clear();
		}
		cout<<"fin de resetstats\n";
        return true;
    }

    string pseudoprior2(long double x) {
        string spr;
        long double mini=0.99999*x,maxi=1.00001*x;
        spr="UN["+LongDoubleToString(mini)+","+LongDoubleToString(maxi)+",0.0,0.0";
        return spr;
    }

    int detphistarOK(int nsel, long double **phistar) {
		char c;
        bool OK;
        int npv,ip1,ip2,nphistarOK=0,scen=rt.scenteste-1,k1;
        npv = rt.nparam[scen];
        for (int i=0;i<nsel;i++) {
            OK=true;
            if (header.scenario[scen].nconditions>0) {
                for (int j=0;j<header.scenario[scen].nconditions;j++) {
                    ip1=0;
                    for(int k =0; k < rt.nparam[scen]; ++k){
                    	if(header.scenario[scen].condition[j].param1 == header.scenario[scen].histparam[k].name){
							k1=k;
                    		break;
                    	} else {
                    		if(not header.scenario[scen].histparam[k].prior.constant) ip1++;
                    	}
                    }

                    ip2=0;
                    for(int k =0; k < rt.nparam[scen]; ++k){
                    	if(header.scenario[scen].condition[j].param2 == header.scenario[scen].histparam[k].name){
                    		break;
                    	} else {
                    		if(not header.scenario[scen].histparam[k].prior.constant) ip2++;
                    	}
                    }
                    if (header.scenario[scen].histparam[k1].category<2){
						if (header.scenario[scen].condition[j].operateur==">")       OK=(floor(0.5+phistar[i][ip1]) >  floor(0.5+phistar[i][ip2]));
						else if (header.scenario[scen].condition[j].operateur=="<")  OK=(floor(0.5+phistar[i][ip1]) <  floor(0.5+phistar[i][ip2]));
						else if (header.scenario[scen].condition[j].operateur==">=") OK=(floor(0.5+phistar[i][ip1]) >= floor(0.5+phistar[i][ip2]));
						else if (header.scenario[scen].condition[j].operateur=="<=") OK=(floor(0.5+phistar[i][ip1]) <= floor(0.5+phistar[i][ip2]));
					} else {
						if (header.scenario[scen].condition[j].operateur==">")       OK=(phistar[i][ip1] >  phistar[i][ip2]);
						else if (header.scenario[scen].condition[j].operateur=="<")  OK=(phistar[i][ip1] <  phistar[i][ip2]);
						else if (header.scenario[scen].condition[j].operateur==">=") OK=(phistar[i][ip1] >= phistar[i][ip2]);
						else if (header.scenario[scen].condition[j].operateur=="<=") OK=(phistar[i][ip1] <= phistar[i][ip2]);
					}
                    if (not OK) break;
                }
            }
            if (OK) { //cout<<nphistarOK<<"    ";
                for (int j=0;j<npv;j++) {
                    phistarOK[nphistarOK][j] = phistar[i][j];
                    //cout <<phistarOK[nphistarOK][j]<<"  ";
                }
                //cout<<"\n";
                nphistarOK++;
				//if ((nphistarOK % 10)==0) cin >>c;
            }
        }
        return nphistarOK;
    }

    void call_loc(int npart,int nstat, int nrec,int nsel,long double **ss, float *stat_obs) {
        int *avant,*apres,*egal;
        long double *qobs;
		float diff;
        string *star;
        qobs = new long double[header.nstat];
        star = new string[header.nstat];
        avant = new int[header.nstat];for (int i=0;i<header.nstat;i++) avant[i] = 0;
        apres = new int[header.nstat];for (int i=0;i<header.nstat;i++) apres[i] = 0;
        egal  = new int[header.nstat];for (int i=0;i<header.nstat;i++) egal[i] = 0;
        for (int p=0;p<npart;p++) {
            for (int j=0;j<header.nstat;j++) {
                diff=stat_obs[j]-(float)ss[p][j];
                if (diff>0.001) avant[j]++;
                else if (diff<-0.001) apres[j]++; else egal[j]++;
            }
        }
        for (int j=0;j<header.nstat;j++) {
              qobs[j] = (long double)(avant[j]+apres[j]+egal[j]);
              if (qobs[j]>0.0) qobs[j] = (0.5*(long double)egal[j]+(long double)avant[j])/qobs[j]; else qobs[j]=-1;
              star[j]="      ";
              if ((qobs[j]>0.95)or(qobs[j]<0.05)) star[j]=" (*)  ";
              if ((qobs[j]>0.99)or(qobs[j]<0.01)) star[j]=" (**) ";
              if ((qobs[j]>0.999)or(qobs[j]<0.001)) star[j]=" (***)";
              cout<<setiosflags(ios::left)<<setw(15)<<header.statname[j]<<"    ("<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<stat_obs[j]<<")   ";
              cout<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<qobs[j]<<star[j]<<"  ";
              cout<<"\n";
        }
        string nomfiloc;
        nomfiloc = path + ident + "_mclocate.txt";
        //strcpy(nomfiloc,path);
        //strcat(nomfiloc,ident);
        //strcat(nomfiloc,"_mclocate.txt");
        cout <<nomfiloc<<"\n";
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        ofstream f12(nomfiloc.c_str(),ios::out);
        f12<<"DIYABC :                   POSTERIOR CHECKING                         "<<asctime(timeinfo)<<"\n";
        f12<<"Data file       : "<<header.datafilename<<"\n";
        f12<<"Reference table : "<<rt.filename<<"\n";
        f12<<"Chosen scenario : "<<rt.scenteste<<"\n";
        f12<<"Number of simulated data sets used to compute posterior    : "<<nrec<<"\n";
        f12<<"Number of simulated data sets used in the local regression : "<<nsel<<"\n";
        f12<<"Number of data sets simulated from the posterior           : "<<npart<<"\n";
        switch (numtransf) {
          case 1 : f12 << "No transformation of parameters\n\n";break;
          case 2 : f12 <<"Transformation of parameters : Log\n\n";break;
          case 3 : f12 <<"Transformation of parameters : Logit\n\n";break;
          case 4 : f12 <<"Transformation of parameters : Log(tg)\n\n";break;
        }

        f12<<" summary           observed       proportion   \n";
        f12<<"statistics           value    (simulated<observed)\n";
        for (int j=0;j<header.nstat;j++) {
             f12<<setiosflags(ios::left)<<setw(15)<<header.statname[j]<<"     "<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<stat_obs[j]<<"       ";
             f12<<setiosflags(ios::fixed)<<setw(6)<<setprecision(4)<<qobs[j]<<star[j]<<"   ";
             f12<<"\n";
        }
        f12.close();


    }

    void call_acp(int nr, int ns, int nstat, int *numscen,long double **ssref, long double **ssphistar, float *stat_obs) {
        resACPC rACP;
        long double *pca_statobs,**pca_ss;
        pca_statobs = new long double[nstat];
        pca_ss = new long double*[ns];
        for (int i=0;i<ns;i++) pca_ss[i] = new long double[nstat];
        rACP = ACP(nr,nstat,ssref,1.0,0);
        //for (int k=0;k<nstat;k++) cout<<stat_obs[k]<<"   "<<rACP.moy[k]<<"   "<<rACP.sd[k]<<"\n";
        for (int j=0;j<rACP.nlambda;j++) {
            pca_statobs[j]=0.0;
            for(int k=0;k<nstat;k++) if (rACP.sd[k]>0) pca_statobs[j] +=(stat_obs[k]-rACP.moy[k])/rACP.sd[k]*rACP.vectprop[k][j];
            cout<<"  "<<pca_statobs[j];
        }
        cout<<"\n";
        for (int i=0;i<ns;i++) {
            for (int j=0;j<rACP.nlambda;j++){
                pca_ss[i][j]=0.0;
                for(int k=0;k<nstat;k++) if (rACP.sd[k]>0) pca_ss[i][j] +=(ssphistar[i][k]-rACP.moy[k])/rACP.sd[k]*rACP.vectprop[k][j];
            }
        }
        string nomfiACP;
        nomfiACP = path + ident + "_mcACP.txt";
        //strcpy(nomfiACP,path);
        //strcat(nomfiACP,ident);
        //strcat(nomfiACP,"_mcACP.txt");
        cout <<nomfiACP<<"\n";
        FILE *f1;
        f1=fopen(nomfiACP.c_str(),"w");
        fprintf(f1,"%d %d",nr,rACP.nlambda);
        for (int i=0;i<rACP.nlambda;i++) fprintf(f1," %5.3Lf",rACP.lambda[i]/rACP.slambda);fprintf(f1,"\n");
        fprintf(f1,"%d",0);
        for (int i=0;i<rACP.nlambda;i++) fprintf(f1," %5.3Lf",pca_statobs[i]);fprintf(f1,"\n");
        for (int i=0;i<nr;i++){
            fprintf(f1,"%d",numscen[i]);
            for (int j=0;j<rACP.nlambda;j++) fprintf(f1," %5.3Lf",rACP.princomp[i][j]);fprintf(f1,"\n");
        }
        for (int i=0;i<ns;i++){
            fprintf(f1,"%d",rt.scenteste);
            for (int j=0;j<rACP.nlambda;j++) fprintf(f1," %5.3Lf",pca_ss[i][j]);fprintf(f1,"\n");
        }
        fclose(f1);
    }

    void domodchec(string opt,int seed){
        int nstatOK, nphistarOK, iprog,nprog;
        int nrec = 0,nsel = 0, ns,nrecpos = 0,newsspart = 0,npv,nss,nsr,newrefpart,*numscen,nparamax,bidon;
        string *ss,s,*ss1,s0,s1,snewstat;
        float  *stat_obs;
        bool usestats,firsttime,dopca = false,doloc = false, newstat=false;
        long double** matC, **phistar, **phistarcompo, **phistarscaled;
        FILE *flog;

        progressfilename = path + ident + "_progress.txt";
		scurfile = path + "courant_"+ ident +".log";
        cout<<"debut domodchec  options : "<<opt<<"\n";
		original=true;composite=false;scaled=false;
        ss = splitwords(opt,";",&ns);
        numtransf=3;
        for (int i=0;i<ns;i++) { cout<<ss[i]<<"\n";
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);
            if (s0=="s:") {
                ss1 = splitwords(s1,",",&rt.nscenchoisi);
                if (rt.nscenchoisi>1) rt.nscenchoisi=1;
                rt.scenchoisi = new int[rt.nscenchoisi];
                for (int j=0;j<rt.nscenchoisi;j++) rt.scenchoisi[j] = atoi(ss1[j].c_str());
                nrecpos=rt.nrecscen[rt.scenchoisi[0]-1];
                cout <<"scenario choisi : "<<rt.scenchoisi[0]<<"\n";
                rt.scenteste = rt.scenchoisi[0];
            } else if (s0=="n:") {
                nrec=atoi(s1.c_str());
                if(nrec>nrecpos) nrec=nrecpos;
                cout<<"nombre total de jeux de données considérés (pour le scénario choisi )= "<<nrec<<"\n";
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
            } else if (s0=="v:") {
                cout<<""<< "\n";
                snewstat=s1;
                newstat = (s1.length()>0);
            } else if (s0=="q:") {
                newsspart=atoi(s1.c_str());
                cout<<"nombre de particules à simuler à partir du posterior = "<<newsspart<<"\n";
            } else if (s0=="a:") {
                dopca=(s1.find("p")!=string::npos);
                doloc=(s1.find("l")!=string::npos);
                if (dopca) cout <<"Perform ACP  ";
                if ((s1=="pl")or(s1=="lp")) cout <<"et ";
                if (doloc) cout<<"locate  ";
                cout<< "\n";

            }
        }
        nprog=newsspart+100;
        if ((newstat)and(dopca)) nprog += header.nscenarios*10000;
        iprog=10;flog=fopen(progressfilename.c_str(),"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        nstatOK = rt.cal_varstat();                       cout<<"apres cal_varstat\n";
        //header.calstatobs(statobsfilename);
		stat_obs = header.stat_obs;  cout<<"apres read_statobs\n";
        cout<<"nrec="<<nrec<<"     nsel="<<nsel<<"\n";
        rt.alloue_enrsel(nsel);
        rt.cal_dist(nrec,nsel,stat_obs);                  cout<<"apres cal_dist\n";
        iprog+=40;flog=fopen(progressfilename.c_str(),"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        det_numpar();
		cout<<"apres det_numpar\n";
        rempli_mat(nsel,stat_obs);                        cout<<"apres rempli_mat\n";
		matC = cal_matC(nsel); 
        recalparamO(nsel);                                 cout<<"apres recalparam\n";
		rempli_parsim(nsel,nparamcom);            			cout<<"apres rempli_parsim(O)\n";
		local_regression(nsel,nparamcom,matC);              cout<<"apres local_regression\n";
        iprog+=20;flog=fopen(progressfilename.c_str(),"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
		phistar = new long double* [nsel];
		for (int i=0;i<nsel;i++) phistar[i] = new long double[nparamcom];
        calphistarO(nsel,phistar);                       cout<<"apres calphistar\n";
        det_nomparam();
		savephistar(nsel,path,ident,phistar,phistarcompo,phistarscaled);                     cout<<"apres savephistar\n";
        iprog+=20;flog=fopen(progressfilename.c_str(),"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        phistarOK = new long double*[nsel];
        for (int i=0;i<nsel;i++) phistarOK[i] = new long double[rt.nparam[rt.scenteste-1]];
		cout<<"header.scenario[rt.scenteste-1].nparam = "<<header.scenario[rt.scenteste-1].nparam<<"\n";
		nphistarOK=detphistarOK(nsel,phistar);               cout << "apres detphistarOK  nphistarOK="<<nphistarOK<<"\n";
		for (int i=0;i<nsel;i++) delete []phistar[i];delete phistar;
		
        cout//<<"naparamcom="<<nparamcom<<"   nparcompo="<<nparcompo<<"   nenr="<<nenr
            << "   nphistarOK="<< nphistarOK<<"   nstat="<<header.nstat<<"\n";
        //cout <<"DEBUG: j'arrête là." << endl; exit(1);
        if(nphistarOK < newsspart){
        	cout << "Not enough suitable particles to perform model checking. Stopping computations." << endl;
        	exit(1);
        }
        npv = rt.nparam[rt.scenteste-1];
		cout<<"npv="<<npv<<"    nenr="<<nenr<<"\n";
		//delete []enreg;
        enreg = new enregC[nenr];
		cout<<"apres new enregC[nenr]\n";
        for (int p=0;p<nenr;p++) {
            enreg[p].stat = new float[header.nstat];
            enreg[p].param = new float[npv];
            enreg[p].numscen = rt.scenteste;
        }
        cout<<"apres dimensionnement des enreg\n";
        nss=0;
        firsttime=true;
        cout<<"ns="<<ns<<"\n";
        //cout<<phistarOK[0][0]<<"\n";
        if (newstat) usestats = resetstats(snewstat);

        cout<<"header.nstat = "<<header.nstat<<"\n";
        ssphistar = new long double*[newsspart];
        for (int i=0;i<newsspart;i++) ssphistar[i] = new long double[header.nstat];
        cout<<"newstat ="<<newstat<<"   newsspart="<<newsspart<<"     nenr="<<nenr<<"\n";
		if (nenr>newsspart) nenr=newsspart;
        while (nss<newsspart) {
            ps.dosimulphistar(header,nenr,false,multithread,firsttime,rt.scenteste,seed,nphistarOK);
            for (int i=0;i<nenr;i++) {
                for (int j=0;j<header.nstat;j++) ssphistar[i+nss][j]=enreg[i].stat[j];
                for (int j=0;j<header.nstat;j++) cout<<ssphistar[i+nss][j]<<"   ";cout<<"\n";
            }
            firsttime=false;
            nss+=nenr;
            iprog+=nenr;flog=fopen(progressfilename.c_str(),"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
            cout<<nss<<"\n";
        }
        if (newstat) {header.calstatobs(statobsfilename);stat_obs = header.stat_obs;}
        if (doloc) call_loc(newsspart,header.nstat,nrec,nsel,ssphistar,stat_obs);
        if (dopca) {
            if (newstat) {
                header.readHeader(headerfilename);cout<<"apres readHeader nscenarios= "<<header.nscenarios<<"\n";
                usestats = resetstats(snewstat);
                newrefpart= header.nscenarios*10000; cout<<"newrefparts="<<newrefpart<<"\n";
                numscen = new int[newrefpart];
                ssref = new long double*[newrefpart];
                for (int i=0;i<newrefpart;i++) ssref[i] = new long double[header.nstat];
                nsr=0;
                firsttime=true;
				cout<<"avant le while (nsr<newrefpart)\n";
                while (nsr<newrefpart) {
                    ps.dosimultabref(header,nenr,false,multithread,firsttime,0,seed,3);
                    for (int i=0;i<nenr;i++) {
                      numscen[i+nsr] = enreg[i].numscen;
                      for (int j=0;j<header.nstat;j++) ssref[i+nsr][j]=enreg[i].stat[j];
                        for (int j=0;j<header.nstat;j++) cout<<ssref[i+nsr][j]<<"   ";cout<<"\n";
                    }
                    firsttime=false;
                    nsr+=nenr;
                    iprog+=nenr;flog=fopen(progressfilename.c_str(),"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
                    cout<<nsr<<"\n";
                }
            } else {
                rt.openfile2();
                enregC enr;
                nparamax=0;for (int i=0;i<rt.nscen;i++)  if (rt.nparam[i]>nparamax) nparamax=rt.nparam[i];
                enr.stat = new float[rt.nstat];
                enr.param = new float[nparamax];
                newrefpart= header.nscenarios*10000; cout<<"newrefparts="<<newrefpart<<"\n";
                numscen = new int[newrefpart];
                ssref = new long double*[newrefpart];
                for (int i=0;i<newrefpart;i++) ssref[i] = new long double[header.nstat];
                nsr=0;
                while (nsr<newrefpart) {
                    bidon = rt.readrecord(&enr);
                    numscen[nsr] = enr.numscen;
                    for (int j=0;j<rt.nstat;j++) ssref[nsr][j]=enr.stat[j];
                    nsr++;
                }
            }
            call_acp(newrefpart,newsspart,header.nstat,numscen,ssref,ssphistar,stat_obs);
        }
        iprog+=10;flog=fopen(progressfilename.c_str(),"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
    }
