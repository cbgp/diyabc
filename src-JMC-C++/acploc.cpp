/*
 * acploc.cpp
 *
 *  Created on: 29 march 2011
 *      Author: cornuet
 */

#include <vector>
#include <cmath>
#include "matrices.h"
#include "mesutils.h"
#include "reftable.h"
#include "acploc.h"
#include "header.h"

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
extern enregC* enreg;
extern HeaderC header;
extern ReftableC rt;
extern string ident, path;

int nacp=100000;

/**
* Structure resACP : contient les résultats d'une ACP 
*/ 

/**
* Effectue une ACP et renvoie les résultats dans une structure resACP 
 */ 


    resACPC ACP(int nli,int nco, long double **X, long double prop,int index) {
        resACPC res;
        res.proportion = prop;
        res.index = index;
        long double **matX,**matXT,**matM,*valprop,**vcprop,**matXTX;
        long double *y,anli,piv,sl;
        anli = 1.0/(long double)nli;
        y = new long double[nli];
        res.moy = new long double[nco];
        res.sd  = new long double[nco];
        
        for (int j=0;j<nco;j++) {
            for (int i=0;i<nli;i++) y[i] = X[i][j];
            res.moy[j] = cal_moyL(nli,y);
            res.sd[j]  = cal_sdL(nli,y)*sqrt((long double)(nli-1)/(long double)(nli));
        }
        matX = new long double*[nli];for (int i=0;i<nli;i++)matX[i] = new long double[nco];
        if (index==0) {
            for (int i=0;i<nli;i++) {
                for (int j=0;j<nco;j++){ 
                    if (res.sd[j]>0.0) matX[i][j] = (X[i][j]-res.moy[j])/res.sd[j]; else matX[i][j]=0.0;
                }
            }
        } else {
            for (int i=0;i<nli;i++) {
                for (int j=0;j<nco;j++) matX[i][j] = X[i][j]-res.moy[j];
            }
        }
		/*for (int i=0;i<10;i++) {
		  for (int j=0;j<10;j++) cout<<setiosflags(ios::fixed)<<setw(10)<<setprecision(6)<< matX[i][j]<<"  ";
		  cout<<"\n";
		}
		cout<<"\n";*/
        matXT = transposeL(nli,nco,matX);
        matXTX = prodML(nco,nli,nco,matXT,matX);
        matM = prodMsL(nco,nco,matXTX,anli);
        for (int i=0;i<nco;i++) {for (int j=0;j<nco;j++) matM[i][j]=matXTX[i][j]*anli;}
        vcprop  = new long double*[nco];for (int i=0;i<nco;i++) vcprop[i]=new long double [nco];
        valprop = new long double[nco];
        //ecrimat("matM",nco,nco,matM);
		/*for (int i=0;i<10;i++) {
		  for (int j=0;j<10;j++) cout<<setiosflags(ios::fixed)<<setw(10)<<setprecision(6)<< matM[i][j]<<"  ";
		  cout<<"\n";
		}
		cout<<"\n";*/
		int nrot=jacobiL(nco,matM,valprop,vcprop);
        //cout <<"nrot = "<<nrot<<"\n";
        //cout<<"valeurs propres :\n";
        //for (int i=0;i<nco;i++) cout<<valprop[i]<<"   ";cout<<"\n";
        for (int i=0;i<nco-1;i++) {
            for (int j=i+1;j<nco;j++) {
                if (valprop[i]<valprop[j]) {
                    piv=valprop[i];valprop[i]=valprop[j];valprop[j]=piv;
                    for (int k=0;k<nco;k++) {
                        piv=vcprop[k][i];vcprop[k][i]=vcprop[k][j];vcprop[k][j]=piv;
                    }
                }
            }
        }
        res.slambda=0.0;
        for (int i=0;i<nco;i++) res.slambda +=valprop[i];
        //for (int i=0;i<nco;i++) cout<<valprop[i]<<"   ";cout <<"\n";
        //cout<<res.slambda<<"\n";
        res.nlambda=1;sl=valprop[0];
        while ((sl/res.slambda<prop)and(res.nlambda<nco)) {sl+=valprop[res.nlambda];res.nlambda++;}
        //cout<<"nombre de composantes : "<<res.nlambda<<"\n";
        res.lambda = new long double[res.nlambda];
        for (int i=0;i<res.nlambda;i++) res.lambda[i]=valprop[i];
        res.vectprop = new long double*[nco];
        for (int i=0;i<nco;i++) {
            res.vectprop[i] = new long double[res.nlambda];
            for (int j=0;j<res.nlambda;j++) res.vectprop[i][j] = vcprop[i][j];
        }
        res.princomp = new long double*[nli];
        for (int i=0;i<nli;i++) {
            res.princomp[i] = new long double[res.nlambda];
            for (int j=0;j<res.nlambda;j++) {
                res.princomp[i][j]=0.0;
                for (int k=0;k<nco;k++) res.princomp[i][j] +=matX[i][k]*res.vectprop[k][j];
            } 
        }  
        delete []valprop;
        for (int i=0;i<nco;i++) delete []vcprop[i]; delete []vcprop;
        return res;
    }

/**
*   Prépare et appelle une ACP sur les summary stats
*/
	void cal_acp(){
        long double **matstat,*pca_statobs;
		float *stat_obs;
        enregC enr;
        int bidon,*numscen,k;
        resACPC rACP;
        //header.calstatobs(statobsfilename);
		stat_obs = header.stat_obs;  //cout<<"apres read_statobs\n";
        int nparamax = 0;
        for (int i=0;i<rt.nscen;i++) if (rt.nparam[i]>nparamax) nparamax=rt.nparam[i];
        //cout<<nparamax<<"\n";
        enr.param = new float[nparamax];
        enr.stat  = new float[rt.nstat];
        if (nacp>rt.nrec) nacp=rt.nrec;
        matstat = new long double*[nacp];
        numscen = new int [nacp];
        pca_statobs = new long double[rt.nstat];
        rt.openfile2();
        for (int p=0;p<nacp;p++) {
                bidon=rt.readrecord(&enr);
                matstat[p] = new long double[rt.nstat];
                numscen[p] = enr.numscen;
                for (int j=0;j<rt.nstat;j++) matstat[p][j] = enr.stat[j];
        }
        rt.closefile();   //cout<<"apres la lecture des "<<nacp<<" enregistrements\n";
        
        cout<<"avant ACP\n";
        rACP = ACP(nacp,rt.nstat,matstat,1.0,0);
        cout<<"apres ACP  path ="<<path<<"\n";
        for (int j=0;j<rACP.nlambda;j++){
            pca_statobs[j]=0.0;
            for(k=0;k<rt.nstat;k++) if (rACP.sd[k]>0.0) pca_statobs[j] +=(stat_obs[k]-rACP.moy[k])/rACP.sd[k]*rACP.vectprop[k][j];
        }
        string nomfiACP;
        nomfiACP = path + ident + "_ACP.txt";
        //strcpy(nomfiACP,path);
        //strcat(nomfiACP,ident);
        //strcat(nomfiACP,"_ACP.txt");
        cout <<nomfiACP<<"\n";
        FILE *f1;
        f1=fopen(nomfiACP.c_str(),"w");
        fprintf(f1,"%d %d",nacp,rACP.nlambda);
        for (int i=0;i<rACP.nlambda;i++) fprintf(f1," %5.3Lf",rACP.lambda[i]/rACP.slambda);fprintf(f1,"\n");
        fprintf(f1,"%d",0);
        for (int i=0;i<rACP.nlambda;i++) fprintf(f1," %5.3Lf",pca_statobs[i]);fprintf(f1,"\n");
        for (int i=0;i<nacp;i++){
            fprintf(f1,"%d",numscen[i]);
            for (int j=0;j<rACP.nlambda;j++) fprintf(f1," %5.3Lf",rACP.princomp[i][j]);fprintf(f1,"\n");
        }
        fclose(f1);
    }

/**
*  Effectue les calculs qui localisent summary stat par summary stat le jeu de données observées par rapport à la table de référence
*/ 
    void cal_loc() {
        long double **qobs;
		float *stat_obs,diff;
        int scen,**avant,**apres,**egal,bidon,nparamax = 0;
        enregC enr;
        string **star;
        //header.calstatobs(statobsfilename);
		stat_obs = header.stat_obs;  //cout<<"apres read_statobs\n";
        for (int i=0;i<rt.nscen;i++) if (rt.nparam[i]>nparamax) nparamax=rt.nparam[i];
        cout<<nparamax<<"\n";
        enr.param = new float[nparamax];
        enr.stat  = new float[rt.nstat];
        qobs = new long double*[rt.nscen];
        for (int i=0;i<rt.nscen;i++) qobs[i]=new long double[rt.nstat];
        star = new string*[rt.nscen];
        for (int i=0;i<rt.nscen;i++) star[i]=new string[rt.nstat];
        avant = new int*[rt.nscen];for (int i=0;i<rt.nscen;i++) {avant[i] = new int[rt.nstat];for (int j=0;j<rt.nstat;j++) avant[i][j]=0;}
        apres = new int*[rt.nscen];for (int i=0;i<rt.nscen;i++) {apres[i] = new int[rt.nstat];for (int j=0;j<rt.nstat;j++) apres[i][j]=0;}
        egal = new int*[rt.nscen]; for (int i=0;i<rt.nscen;i++) {egal[i]  = new int[rt.nstat];for (int j=0;j<rt.nstat;j++)  egal[i][j]=0;}
        rt.openfile2();
        for (int p=0;p<rt.nrec;p++) {
            bidon=rt.readrecord(&enr);
            scen=enr.numscen-1;
            for (int j=0;j<rt.nstat;j++) {
                diff=stat_obs[j]-enr.stat[j];
                if (diff>0.001) avant[scen][j]++;
                else if (diff<-0.001) apres[scen][j]++; else egal[scen][j]++;
            }
        }
        rt.closefile();   cout<<"apres la lecture des "<<rt.nrec<<" enregistrements\n";
        for (int j=0;j<rt.nstat;j++) {
             for (int i=0;i<rt.nscen;i++) {
                 qobs[i][j] = (long double)(avant[i][j]+apres[i][j]+egal[i][j]);
                 if (qobs[i][j]>0.0) qobs[i][j] = (0.5*(long double)egal[i][j]+(long double)avant[i][j])/qobs[i][j]; else qobs[i][j]=-1;
                 star[i][j]="      ";
                 if ((qobs[i][j]>0.95)or(qobs[i][j]<0.05)) star[i][j]=" (*)  ";
                 if ((qobs[i][j]>0.99)or(qobs[i][j]<0.01)) star[i][j]=" (**) ";
                 if ((qobs[i][j]>0.999)or(qobs[i][j]<0.001)) star[i][j]=" (***)";
            }
            cout<<setiosflags(ios::left)<<setw(15)<<header.statname[j]<<"    ("<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<stat_obs[j]<<")   ";
            for (int i=0;i<rt.nscen;i++) cout<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<qobs[i][j]<<star[i][j]<<"  ";
            cout<<"\n";
        }
        string nomfiloc;
        nomfiloc = path + ident + "_locate.txt";
        //strcpy(nomfiloc,path);
        //strcat(nomfiloc,ident);
        //strcat(nomfiloc,"_locate.txt");
        cout <<nomfiloc<<"\n";
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        ofstream f12(nomfiloc.c_str(),ios::out);
        f12<<"DIYABC :                   PRIOR CHECKING                         "<<asctime(timeinfo)<<"\n";
        f12<<"Data file                     : "<<header.datafilename<<"\n";
        f12<<"Reference table               : "<<rt.filename<<"\n";
        f12<<"Number of simulated data sets : "<<rt.nrec<<"\n\n";
        f12<<"Values indicate for each summary statistics the proportion \nof simulated data sets which have a value below the observed one\n";
        f12<<" Summary           observed";for (int i=0;i<rt.nscen;i++) f12<< "    scenario   ";f12<<"\n";
        f12<<"statistics           value ";for (int i=0;i<rt.nscen;i++) f12<< "      "<<setw(3)<<i+1<< "      ";f12<<"\n";
        for (int j=0;j<rt.nstat;j++) {
             f12<<setiosflags(ios::left)<<setw(15)<<header.statname[j]<<"    ("<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<stat_obs[j]<<")   ";
             for (int i=0;i<rt.nscen;i++) {
                 f12<<setiosflags(ios::fixed)<<setw(6)<<setprecision(4)<<qobs[i][j]<<star[i][j]<<"   ";
             }
             f12<<"\n";
        }
        f12.close();
    }

/**
 * interprète la commande de l'option pre-evaluate prior-scenario combination et lance les calculs correspondants
 */
    void doacpl(string opt,bool multithread, int seed){
        string *ss,s,s0,s1;
        bool dopca = false,doloc = false;
        int ns;
 		cout<<"doacpl "<<opt<<"\n";
       ss = splitwords(opt,";",&ns);
        for (int i=0;i<ns;i++) { cout<<ss[i]<<"\n";
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);
            if (s0=="a:") {
                dopca=(s1.find("p")!=string::npos);
                doloc=(s1.find("l")!=string::npos);
                if (dopca) cout <<"Perform ACP  ";
                if ((s1=="pl")or(s1=="lp")) cout <<"et ";
                if (doloc) cout<<"locate  ";
                cout<< "\n";
            }            
        }
        if (dopca) cal_acp();
        if (doloc) cal_loc();
   }
