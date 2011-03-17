/*
 * estimparam.cpp
 *
 *  Created on: 9 march 2011
 *      Author: cornuet
 */
#ifndef HEADER
#include "header.cpp"
#define HEADER
#endif

#ifndef ALGORITHM
#include <algorithm>
#define ALGORITHM
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

using namespace std;

#define c 1.5707963267948966192313216916398
#define co 2.506628274631000502415765284811   //sqrt(pi)
#define coefbw 1.8                            //coefficient multiplicateur de la bandwidth pour le calcul de la densité 


/**
* définition de la densité d'un paramètre
*/
struct pardensC {
    double *x,*priord,*postd;
    double xmin,xmax,xdelta;
    int ncl;
};

struct parstatC {
    double moy,med,mod,q025,q050,q250,q750,q950,q975;
};

//variables globales du module;
pardensC *pardens;
enregC *enrsel;
ReftableC rt;
HeaderC header;
double *var_stat,*stat_obs, *parmin, *parmax, *diff;
double **alpsimrat,**parsim,**matX0,*vecW,**beta, **phistar,**simpar;
int nparamax,nparamcom,nparcompo,**numpar,nstatOKsel,numtransf,npar,npar0;
double borne=10.0,xborne;
bool composite,original;
int nscenchoisi,*scenchoisi;
string entete;
int nsimpar=100000;
string *nomparam;
parstatC *parstat;

/** 
* définit l'opérateur de comparaison de deux enregistrements de type enregC
* pour l'utilisation de la fonction sort du module algorithm
*/
struct compenreg
{
   bool operator() (const enregC & lhs, const enregC & rhs) const
   {
      return lhs.dist < rhs.dist;
   }
};

/** 
* calcule les variances des statistiques résumées 
* sur les 100000 premiers enregistrements de la table de référence
*/
    int cal_varstat() {
        int nrecutil,iscen,nsOK,bidon;
        double *sx,*sx2,x,an;
        enregC enr;
        nrecutil=100000;if (nrecutil>rt.nrec) nrecutil=rt.nrec;
        an=1.0*(double)nrecutil;
        sx  = new double[rt.nstat];
        sx2 = new double[rt.nstat];
        var_stat = new double[rt.nstat];
        for (int j=0;j<rt.nstat;j++) {sx[j]=0.0;sx2[j]=0.0;}
        enr.stat = new float[rt.nstat];
        nparamax = 0;for (int i=0;i<rt.nscen;i++) if (rt.nparam[i]>nparamax) nparamax=rt.nparam[i];
        enr.param = new float[nparamax];
        //for (int i=0;i<20000;i++) bidon=rt.readrecord(&enr);
        for (int i=0;i<nrecutil;i++) {
            bidon=rt.readrecord(&enr);
            iscen=enr.numscen;
            for (int j=0;j<rt.nstat;j++) {
                x = (double)enr.stat[j];
                sx[j] += x;
                sx2[j] += x*x;
            }
        }
        nsOK=0;
        for (int j=0;j<rt.nstat;j++) {
            var_stat[j]=(sx2[j] -sx[j]*sx[j]/an)/(an-1.0);
            if (var_stat[j]>0) nsOK++;
            //cout<<"var_stat["<<j<<"]="<<var_stat[j]<<"\n";
        }
        return nsOK;
    }

/** 
* lit le fichier des statistiques observées (placées dans double *stat_obs)
*/
    void read_statobs(char *statobsfilename) {
       string entete,ligne,*ss;
       int ns;
       ifstream file(statobsfilename, ios::in);
       getline(file,entete);
       getline(file,ligne);
       file.close();
       ss=splitwords(ligne," ",&ns);
       //cout<<"statobs ns="<<ns<<"\n";
       if (ns!=rt.nstat) exit(1);
       stat_obs = new double[ns];
       for (int i=0;i<ns;i++) stat_obs[i]=atof(ss[i].c_str());
       delete []ss;
       //for (int i=0;i<ns;i++) cout<<stat_obs[i]<<"   ";
       //cout<<"\n";
    }

/** 
* calcule la distance de chaque jeu de données simulé au jeu observé
* et sélectionne les nsel enregistrements les plus proches (copiés dans enregC *enrsel)
*/
    void cal_dist(int nrec, int nsel) {
        int nn=10000,nreclus=0,nparamax,nrecOK=0,iscen,bidon;
        bool firstloop=true,scenOK;
        double diff;
        if (nn<nsel) nn=nsel;
        nparamax = 0;for (int i=0;i<rt.nscen;i++) if (rt.nparam[i]>nparamax) nparamax=rt.nparam[i];
        //cout<<"cal_dist nsel="<<nsel<<"   nparamax="<<nparamax<<"   nrec="<<nrec<<"   nreclus="<<nreclus<<"   nstat="<<rt.nstat<<"   2*nn="<<2*nn<<"\n";
        enrsel = new enregC[2*nn];
        //cout<<" apres allocation de enrsel\n";
        while (nreclus<nrec) {
            if (firstloop) {nrecOK=0;firstloop=false;}
            else nrecOK=nn;
            //cout<<"nrecOK = "<<nrecOK<<"\n";
            while ((nrecOK<2*nn)and(nreclus<nrec)) {
                enrsel[nrecOK].param = new float[nparamax];
                enrsel[nrecOK].stat  = new float[rt.nstat];
                //cout<<"avant readrecord\n";
                bidon=rt.readrecord(&enrsel[nrecOK]);
                //cout<<"apres readrecord\n";
                //cout<<enrsel[nrecOK].numscen<<"   "<<scenchoisi[0]<<"\n";
                //for (int j=0;j<rt.nstat;j++) cout <<enrsel[nrecOK].stat[j]<<"  ";
                //cout <<"\n";
                //cin >>bidon;
                nreclus++;
                scenOK=false;iscen=0;
                while((not scenOK)and(iscen<nscenchoisi)) {
                    scenOK=(enrsel[nrecOK].numscen==scenchoisi[iscen]);
                    iscen++;
                }
                if (scenOK) {
                    enrsel[nrecOK].dist=0.0; 
                    for (int j=0;j<rt.nstat;j++) if (var_stat[j]>0.0) {
                        diff =(double)enrsel[nrecOK].stat[j] - (double)stat_obs[j]; 
                      enrsel[nrecOK].dist += diff*diff/var_stat[j];
                    }
                    nrecOK++;
                if (nreclus==nrec) break;
                }
            }
            sort(&enrsel[0],&enrsel[2*nn],compenreg()); 
            //cout<<"nrec_lus = "<<nreclus<<"    distmin = "<<enrsel[0].dist/rt.nstat<<"    distmax = "<<enrsel[nsel-1].dist/rt.nstat<<"\n";
        }
            cout<<"nrec_lus = "<<nreclus<<"    distmin = "<<enrsel[0].dist/rt.nstat<<"    distmax = "<<enrsel[nsel-1].dist/rt.nstat<<"\n";
    }
    
/** 
* compte les paramètres communs aux scénarios choisis (nparamcom), le nombre de paramètres composites
* rempli le tableau numpar des numéros de paramètres communs par scénario
*/
    void det_numpar() {
        vector <string>  parname;
        int ii;
        bool commun,trouve;
        npar=0;npar0=0;
        numpar = new int*[nscenchoisi]; 
        if (nscenchoisi==1) {
            numpar[0] = new int[rt.nparam[scenchoisi[0]-1]];
            for (int i=0;i<rt.nparam[scenchoisi[0]-1];i++) numpar[0][i]=i;
        } else {
            for (int i=0;i<header.scenario[scenchoisi[0]-1].nparam;i++) {
                commun=true;
                for (int j=1;j<nscenchoisi;j++) {
                    trouve=false;
                    for (int k=0;k<header.scenario[scenchoisi[j]-1].nparam;k++) {
                        trouve=(header.scenario[scenchoisi[0]-1].histparam[i].name.compare(header.scenario[scenchoisi[j]-1].histparam[k].name)==0);
                        if (trouve) break;
                    }
                    commun=trouve;
                    if (not commun) break;
                }
                if (commun) {
                    npar++;
                    parname.push_back(header.scenario[scenchoisi[0]-1].histparam[i].name);
                    if (header.scenario[scenchoisi[0]-1].histparam[i].category<2) npar0++;
                }
            }
            cout << "noms des parametres communs : ";
            for (int i=0;i<npar;i++) cout<<parname[i]<<"   ";
            cout <<"\n";
        } 
        if (composite) nparcompo=npar0*header.ngroupes; else nparcompo=0;
        cout<<"nombre de parametres composites : "<<nparcompo<<"\n";
        nparamcom = npar+rt.nparam[scenchoisi[0]-1]-header.scenario[scenchoisi[0]-1].nparam;
        for (int j=0;j<nscenchoisi;j++) {
            numpar[j] = new int[nparamcom];
            ii=0;
            for (int i=0;i<npar;i++) {
                for (int k=0;k<header.scenario[scenchoisi[j]-1].nparam;k++) {
                    if (header.scenario[scenchoisi[j]-1].histparam[k].name.compare(parname[i])==0) {numpar[j][ii]=k;ii++;}  
                }
            }
            for (int k=header.scenario[scenchoisi[j]-1].nparam;k<rt.nparam[scenchoisi[j]-1];k++) {numpar[j][ii]=k;ii++;}
            for (int kk=0;kk<npar+rt.nparam[scenchoisi[j]-1]-header.scenario[scenchoisi[j]-1].nparam;kk++) {
              cout <<numpar[j][kk]<<"  ";
              if (kk<npar) cout<<"("<<header.scenario[scenchoisi[j]-1].histparam[numpar[j][kk]].name<<")  ";
            }
            cout<<"\n";
        }
        
    }

/** 
* recalcule les valeurs de paramètres en fonction de la transformation choisie 
*/
    void recalparam(int n) {
        double coefmarge=0.001,marge;
        int jj,k,kk;
        alpsimrat = new double*[n];
        for(int i=0;i<n;i++) alpsimrat[i] = new double[nparamcom];
        switch (numtransf) {
        
          case 1 :  //no transform
                   for (int i=0;i<n;i++) {
                       for (int j=0;j<nparamcom;j++) {
                           k = enrsel[i].numscen-1;
                           alpsimrat[i][j] = enrsel[i].param[numpar[k][j]];
                       }    
                   }
                   break;
          case 2 : // log transform
                   for (int i=0;i<n;i++) {
                       for (int j=0;j<nparamcom;j++) {
                           k = enrsel[i].numscen-1;
                           if (enrsel[i].param[numpar[k][j]]<=0.00000000001) enrsel[i].param[numpar[k][j]]=1E-11;
                           alpsimrat[i][j] = log(enrsel[i].param[numpar[k][j]]);
                       }    
                   }
                   break;
          case 3 : //logit transform
                   parmin = new double[nparamcom]; parmax = new double[nparamcom]; diff = new double[nparamcom];
                   if (borne<0.0000000001) {
                       xborne=1E100;
                       for (int j=0;j<nparamcom;j++) {parmin[j]=1E100;parmax[j]=-1E100;}
                       for (int i=0;i<n;i++) {
                            for (int j=0;j<nparamcom;j++) {
                                k = enrsel[i].numscen-1;
                                if (enrsel[i].param[numpar[k][j]]<parmin[j]) parmin[j]=enrsel[i].param[numpar[k][j]];
                                if (enrsel[i].param[numpar[k][j]]>parmax[j]) parmax[j]=enrsel[i].param[numpar[k][j]];
                           }
                       }
                       for (int j=0;j<nparamcom;j++) {
                           diff[j]=parmax[j]-parmin[j];
                           marge=coefmarge*diff[j];
                           parmin[j] -=marge;
                           parmax[j] +=marge;
                           diff[j]=parmax[j]-parmin[j];
                       }
                   } else{
                       xborne=borne;
                       k=scenchoisi[0]-1;
                       for (int j=0;j<nparamcom-header.nparamut;j++) {
                           if (header.scenario[k].histparam[numpar[k][j]].category<2) {
                               parmin[j] = header.scenario[k].histparam[numpar[k][j]].prior.mini-0.5;
                               parmax[j] = header.scenario[k].histparam[numpar[k][j]].prior.maxi+0.5;
                           } else {
                               parmin[j] = header.scenario[k].histparam[numpar[k][j]].prior.mini-0.0005;
                               parmax[j] = header.scenario[k].histparam[numpar[k][j]].prior.maxi+0.0005;
                           }
                           diff[j]=parmax[j]-parmin[j];
                           //cout<<header.scenario[k].histparam[numpar[k][j]].name<<"   ";
                           //cout<<"parmin = "<<parmin[j]<<"   parmax="<<parmax[j]<<"\n";
                       }
                       for (int j=nparamcom-header.nparamut;j<nparamcom;j++) {
                               jj=j-(nparamcom-header.nparamut);
                               //cout<<"jj="<<jj<<"\n";
                               parmin[j]=0.95*header.mutparam[jj].prior.mini;
                               parmax[j]=1.05*header.mutparam[jj].prior.maxi;
                               //cout<<header.mutparam[jj].category<<"   ";
                               //cout<<"parmin = "<<parmin[j]<<"   parmax="<<parmax[j]<<"\n";
                               //diff[j]=parmax[j]-parmin[j];
                       }
                   }
                   //cout <<"fin du calcul de parmin/parmax\n";
                   for (int i=0;i<n;i++) {
                       for (int j=0;j<nparamcom;j++) {
                           k = enrsel[i].numscen-1;
                           if (enrsel[i].param[numpar[k][j]]<=parmin[j]) alpsimrat[i][j] = -xborne;
                           else if (enrsel[i].param[numpar[k][j]]>=parmax[j]) alpsimrat[i][j] = xborne;
                           else alpsimrat[i][j] =log((enrsel[i].param[numpar[k][j]]-parmin[j])/(parmax[j]-enrsel[i].param[numpar[k][j]]));
                       }
                   }
                   break;
          case 4 : //log(tg) transform
                   parmin = new double[nparamcom]; parmax = new double[nparamcom]; diff = new double[nparamcom];
                   if (borne<0.0000000001) {
                       xborne=1E100;
                       for (int j=0;j<nparamcom;j++) {parmin[j]=1E100;parmax[j]=-1E100;}
                       for (int i=0;i<n;i++) {
                            for (int j=0;j<nparamcom;j++) {
                                k = enrsel[i].numscen-1;
                                if (enrsel[i].param[numpar[k][j]]<parmin[j]) parmin[j]=enrsel[i].param[numpar[k][j]];
                                if (enrsel[i].param[numpar[k][j]]>parmax[j]) parmax[j]=enrsel[i].param[numpar[k][j]];
                           }
                       }
                       for (int j=0;j<nparamcom;j++) {
                           diff[j]=parmax[j]-parmin[j];
                           marge=coefmarge*diff[j];
                           parmin[j] -=marge;
                           parmax[j] +=marge;
                           diff[j]=parmax[j]-parmin[j];
                       }
                   } else{
                       xborne=borne;
                       k=scenchoisi[0]-1;
                       for (int j=0;j<nparamcom-header.nparamut;j++) {
                           if (header.scenario[k].histparam[numpar[k][j]].category<2) {
                               parmin[j] = header.scenario[k].histparam[numpar[k][j]].prior.mini-0.5;
                               parmax[j] = header.scenario[k].histparam[numpar[k][j]].prior.maxi+0.5;
                           } else {
                               parmin[j] = header.scenario[k].histparam[numpar[k][j]].prior.mini-0.0005;
                               parmax[j] = header.scenario[k].histparam[numpar[k][j]].prior.maxi+0.0005;
                           }
                           diff[j]=parmax[j]-parmin[j];
                           //cout<<header.scenario[k].histparam[numpar[k][j]].name<<"   ";
                           //cout<<"parmin = "<<parmin[j]<<"   parmax="<<parmax[j]<<"\n";
                       }
                       for (int j=nparamcom-header.nparamut;j<nparamcom;j++) {
                               jj=j-(nparamcom-header.nparamut);
                               cout<<"jj="<<jj<<"\n";
                               parmin[j]=0.95*header.mutparam[jj].prior.mini;
                               parmax[j]=1.05*header.mutparam[jj].prior.maxi;
                               cout<<header.mutparam[jj].category<<"   ";
                               //cout<<"parmin = "<<parmin[j]<<"   parmax="<<parmax[j]<<"\n";
                               //diff[j]=parmax[j]-parmin[j];
                       }
                   }
                   //cout <<"fin du calcul de parmin/parmax\n";
                   for (int i=0;i<n;i++) {
                       for (int j=0;j<nparamcom;j++) {
                           k = enrsel[i].numscen-1;
                           if (enrsel[i].param[numpar[k][j]]<=parmin[j]) alpsimrat[i][j] = -xborne;
                           else if (enrsel[i].param[numpar[k][j]]>=parmax[j]) alpsimrat[i][j] = xborne;
                           else alpsimrat[i][j] =log(tan((enrsel[i].param[numpar[k][j]]-parmin[j])*c/diff[j]));
                       }
                   }
                   break;
        }
    }   

/** 
* effectue le remplissage de la matrice matX0, du vecteur des poids vecW et 
* de la matrice des paramètres parsim (éventuellement transformés)
*/
    void rempli_mat(int n) {
        int icc;
        double delta,som,x,*var_statsel,nn;
        double *sx,*sx2,*mo;
        nn=(double)n;
        delta = enrsel[n-1].dist;
        //cout<<"delta="<<delta<<"\n";
        sx  = new double[rt.nstat];
        sx2 = new double[rt.nstat];
        mo  = new double[rt.nstat];
        var_statsel = new double[rt.nstat];
        for (int i=0;i<rt.nstat;i++){sx[i]=0.0;sx2[i]=0.0;mo[i]=0.0;}
        for (int i=0;i<n;i++){
            for (int j=0;j<rt.nstat;j++) {
                x = enrsel[i].stat[j];
                sx[j] +=x;
                sx2[j] +=x*x;
            }
        }
        nstatOKsel=0;
        for (int j=0;j<rt.nstat;j++) {
            var_statsel[j]=(sx2[j]-sx[j]/nn*sx[j])/(nn-1.0);
            if (var_statsel[j]>0.0) nstatOKsel++;
            mo[j] = sx[j]/nn;
        }
        matX0 = new double*[n];
        for (int i=0;i<n;i++)matX0[i]=new double[nstatOKsel];
        vecW = new double[n];

        som=0.0;
        for (int i=0;i<n;i++) {
            icc=-1;
            for (int j=0;j<rt.nstat;j++) {
                if (var_statsel[j]>0.0) {
                    icc++;
                    matX0[i][icc]=(enrsel[i].stat[j]-stat_obs[j])/sqrt(var_statsel[j]);
                }
            }
            x=enrsel[i].dist/delta;
            vecW[i]=(1.5/delta)*(1.0-x*x);
            som = som + vecW[i];
        }

        for (int i=0;i<n;i++) vecW[i]/=som;
        //for (int i=0;i<10;i++) cout<<vecW[i]<<"  ";
        //cout<<"\n";
        parsim = new double*[n];
        for (int i=0;i<n;i++) parsim[i] = new double[nparamcom];
        for (int i=0;i<n;i++) {
            for (int j=0;j<nparamcom;j++)parsim[i][j]=alpsimrat[i][j];
        }
    }

/** 
* effectue la régression locale à partir de la matrice matX0 et le vecteur des poids vecW
*/
    void local_regression(int n, bool multithread) {
        double **matX,**matXT,**matA,**matB,**matAA,**matC;
        
        matA = new double*[nstatOKsel+1];
        for (int j=0;j<nstatOKsel+1;j++) matA[j] = new double[n];
        matX = new double*[n];
        for (int i=0;i<n;i++) {
            matX[i] = new double[nstatOKsel+1];
            matX[i][0] = 1.0;
            for (int j=1;j<nstatOKsel+1;j++) matX[i][j] = matX0[i][j-1];
        }
        //ecrimat("matX0",10,nstatOKsel,matX0);
        //ecrimat("matX",10,nstatOKsel+1,matX);
        matXT = transpose(n,nstatOKsel+1,matX);
        for (int i=0;i<n;i++) {
            for (int j=0;j<nstatOKsel+1;j++) matA[j][i] = matXT[j][i]*vecW[i];
        }
        //ecrimat("matA",nstatOKsel+1,nstatOKsel+1,matA);
        matAA = prodM(nstatOKsel+1,n,nstatOKsel+1,matA,matX);
        //ecrimat("matAA",nstatOKsel+1,nstatOKsel+1,matAA);
        matB = inverse(nstatOKsel+1,matAA,multithread);
        matC = prodM(nstatOKsel+1,nstatOKsel+1,n,matB,matA);
        beta = prodM(nstatOKsel+1,n,nparamcom,matC,parsim);
        //ecrimat("beta",nstatOKsel+1,nparamcom,beta);
        libereM(nstatOKsel+1,matA);
        libereM(n,matX);
        libereM(nstatOKsel+1,matB);
        libereM(nstatOKsel+1,matAA);
        libereM(nstatOKsel+1,matC);
    }

/** 
* calcule les phistars pour les paramètres originaux et composites
*/
    void calphistar(int n){
        cout<<"debut de calphistar\n";
        int k,kk,qq;
        double pmut;
        phistar = new double*[n];
        for (int i=0;i<n;i++) phistar[i] = new double[nparamcom+nparcompo];
        for (int i=0;i<n;i++) {
            for (int j=0;j<nparamcom;j++) {
                phistar[i][j] = alpsimrat[i][j];
                //if (i==0) cout<< phistar[i][j]<<"   ";
                for (int k=0;k<nstatOKsel;k++) phistar[i][j] -= matX0[i][k]*beta[k+1][j];
                //if(i==0) cout<< phistar[i][j]<<"   ";
                switch(numtransf) {
                  case 1 : break;
                  case 2 : if (phistar[i][j]<100.0) phistar[i][j] = exp(phistar[i][j]); else phistar[i][j]=exp(100.0);
                           break; 
                  case 3 : if (phistar[i][j]<=-xborne) phistar[i][j] = parmin[j];
                           else if (phistar[i][j]>=xborne) phistar[i][j] = parmax[j];
                           else phistar[i][j] = (exp(phistar[i][j])*parmax[j]+parmin[j])/(1.0+exp(phistar[i][j]));
                           //if(i==0) cout<< phistar[i][j]<<"\n";
                           break;
                  case 4 : if (phistar[i][j]<=-xborne) phistar[i][j] = parmin[j];
                           else if (phistar[i][j]>=xborne) phistar[i][j] = parmax[j];
                           else phistar[i][j] =parmin[j] +(diff[j]/c*atan(exp(phistar[i][j])));
                           break;
                }
            }
        }
        if (nparcompo>0) {
            k=0;
            for (int gr=1;gr<header.ngroupes+1;gr++) {
                if (header.groupe[gr].type==0) {
                    if (header.groupe[gr].priormutmoy.constant) {
                        if (header.groupe[gr].priorsnimoy.constant) {
                            pmut = header.groupe[gr].mutmoy+header.groupe[gr].snimoy;
                            for (int j=0;j<npar;j++) {
                                if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                    for (int i=0;i<n;i++) {
                                          phistar[i][nparamcom+k] = pmut*phistar[i][j];
                                    }    
                                    k++;
                                }
                            }
                        } else {
                            kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==2))) kk++;
                            for (int j=0;j<npar;j++) {
                                if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                    for (int i=0;i<n;i++) {
                                        pmut = header.groupe[gr].mutmoy+phistar[i][npar+kk];
                                        phistar[i][nparamcom+k] = pmut*phistar[i][j];
                                    }    
                                    k++;
                                }
                            }
                        }
                    } else {
                        if (header.groupe[gr].priorsnimoy.constant) {
                            kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==0))) kk++;
                            for (int j=0;j<npar;j++) {
                                if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                    for (int i=0;i<n;i++) {
                                        pmut =phistar[i][npar+kk] +header.groupe[gr].snimoy;
                                        phistar[i][nparamcom+k] = pmut*phistar[i][j];
                                    }    
                                    k++;
                                }
                            }
                        } else {
                            kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==0))) kk++;
                            qq=0;while (not ((header.mutparam[qq].groupe == gr)and(header.mutparam[qq].category ==2))) qq++;
                            for (int j=0;j<npar;j++) {
                                if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                    for (int i=0;i<n;i++) {
                                        pmut =phistar[i][npar+kk]+phistar[i][npar+qq];
                                        phistar[i][nparamcom+k] = pmut*phistar[i][j];
                                    }    
                                    k++;
                                }
                            }
                        }
                    }
                } 
                if (header.groupe[gr].type==1) {    
                    if (header.groupe[gr].priormusmoy.constant) {
                        pmut = header.groupe[gr].musmoy;
                        for (int j=0;j<npar;j++) {
                            if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                for (int i=0;i<n;i++) {
                                    phistar[i][nparamcom+k] = pmut*phistar[i][j];
                                }    
                                k++;
                            }
                        }
                    } else {
                        kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==3))) kk++;
                        for (int j=0;j<npar;j++) {
                            if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                for (int i=0;i<n;i++) {
                                    pmut = phistar[i][npar+kk];
                                    phistar[i][nparamcom+k] = pmut*phistar[i][j];
                                }    
                                k++;
                            }
                        }
                    }
                }
            }
        }
        cout<<"nparcompo = "<<nparcompo<<"   k="<<k<<"\n";
    }
        
/** 
* effectue la sauvegarde des phistars dans le fichier path/ident/phistar.txt
*/
    void savephistar(int n, char* path,char* ident) {
        char *nomphistar ;
        int k;
        nomparam =new string[nparamcom+nparcompo];
        string pp;
        entete="scenario";
        for (int j=0;j<npar;j++) {
            nomparam[j]=string(header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].name);
            entete += centre(header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].name,15);}
        for (int j=npar;j<nparamcom;j++) {
            nomparam[j]=string(header.mutparam[j-npar].name);
            entete += centre(header.mutparam[j-npar].name,15);}
        if (nparcompo>0) {
            k=nparamcom;
            for (int gr=1;gr<header.ngroupes+1;gr++) {
                for (int j=0;j<npar;j++) {
                    if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                        pp=header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].name;
                        if (header.groupe[gr].type==0) pp = pp+"(µ+sni)_"+IntToString(gr);
                        if (header.groupe[gr].type==1) pp = pp+"µseq_"+IntToString(gr);
                        nomparam[k]=pp;k++;
                        entete += centre(pp,16);
                    }
                }
            }
        }
        nomphistar = new char[strlen(path)+strlen(ident)+20];
        strcpy(nomphistar,path);
        strcat(nomphistar,ident);
        strcat(nomphistar,"_phistar.txt");
        cout <<nomphistar<<"\n";
        FILE *f1;
        f1=fopen(nomphistar,"w");
        fprintf(f1,"%s\n",entete.c_str());
        for (int i=0;i<n;i++) {
            fprintf(f1,"  %d    ",enrsel[i].numscen);
            for (int j=0;j<nparamcom+nparcompo;j++) fprintf(f1,"  %8.5e  ",phistar[i][j]);
            //if(i<10) for (int j=0;j<nparamcom+nparcompo;j++) printf(" %8.5e",phistar[i][j]);
            fprintf(f1,"\n");
        }
        fclose(f1);
    }

/**
* calcul de la densité par noyau gaussien 
* x : vecteur des abcisses
* y vecteur des 
*/


/**
* lit les paramètres des enregistrements simulés pour l'établissement des distributions a priori'
*/
    void lisimpar(int nsel){
        int bidon,iscen,k,kk,qq;
        bool scenOK;
        double pmut;
        if (nsimpar>rt.nrec) nsimpar=rt.nrec;
        simpar = new double*[nsimpar];
        enregC enr;
        enr.stat = new float[rt.nstat];
        enr.param = new float[nparamax];
        int i=-1;
        while (i<nsimpar-1) {
            bidon=rt.readrecord(&enr);
            scenOK=false;iscen=0;
            while((not scenOK)and(iscen<nscenchoisi)) {
                scenOK=(enr.numscen==scenchoisi[iscen]);
                iscen++;
            }
            if (scenOK) {
              i++;
              simpar[i] = new double[nparamcom+nparcompo];
              for (int j=0;j<nparamcom;j++) simpar[i][j] = enr.param[numpar[enr.numscen-1][j]];
              if (nparcompo>0) {
                  k=0;
                  for (int gr=1;gr<header.ngroupes+1;gr++) {
                      if (header.groupe[gr].type==0) {
                          if (header.groupe[gr].priormutmoy.constant) {
                              if (header.groupe[gr].priorsnimoy.constant) {
                                  pmut = header.groupe[gr].mutmoy+header.groupe[gr].snimoy;
                                  for (int j=0;j<npar;j++) {
                                      if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                          simpar[i][nparamcom+k] = pmut*enr.param[numpar[enr.numscen-1][j]];
                                          k++;
                                      }
                                  }
                              } else {
                                  kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==2))) kk++;
                                  for (int j=0;j<npar;j++) {
                                      if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                          pmut = header.groupe[gr].mutmoy+enr.param[numpar[enr.numscen-1][npar+kk]];
                                          simpar[i][nparamcom+k] = pmut*enr.param[numpar[enr.numscen-1][j]];
                                          k++;
                                      }
                                  }
                              }
                          } else {
                              if (header.groupe[gr].priorsnimoy.constant) {
                                  kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==0))) kk++;
                                  for (int j=0;j<npar;j++) {
                                      if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                          pmut =enr.param[numpar[enr.numscen-1][npar+kk]]+header.groupe[gr].snimoy;
                                          simpar[i][nparamcom+k] = pmut*enr.param[numpar[enr.numscen-1][j]];
                                          k++;
                                      }
                                  }
                              } else {
                                  kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==0))) kk++;
                                  qq=0;while (not ((header.mutparam[qq].groupe == gr)and(header.mutparam[qq].category ==2))) qq++;
                                  for (int j=0;j<npar;j++) {
                                      if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                          pmut =enr.param[numpar[enr.numscen-1][npar+kk]]+enr.param[numpar[enr.numscen-1][npar+qq]];
                                          simpar[i][nparamcom+k] = pmut*enr.param[numpar[enr.numscen-1][j]];
                                          k++;
                                      }
                                  }
                              }
                          }
                      } 
                      if (header.groupe[gr].type==1) {    
                          if (header.groupe[gr].priormusmoy.constant) {
                              pmut = header.groupe[gr].musmoy;
                              for (int j=0;j<npar;j++) {
                                  if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                      simpar[i][nparamcom+k] = pmut*enr.param[numpar[enr.numscen-1][j]];
                                      k++;
                                  }
                              }
                          } else {
                              kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==3))) kk++;
                              for (int j=0;j<npar;j++) {
                                  if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                      pmut = enr.param[numpar[enr.numscen-1][npar+kk]];
                                      simpar[i][nparamcom+k] = pmut*enr.param[numpar[enr.numscen-1][j]];
                                      k++;
                                  }
                              }
                          }
                      }
                  }
               }
            }
        }
        //ecrimat("simpar",10,nparamcom+nparcompo,simpar);
    }

/**
* calcule la densité à partir de la loi fournie dans le prior
*/
    double* caldensexact(int ncl,double *x,PriorC pr) {
      
        double *dens,xb,som;
        dens = new double[ncl];
        if (pr.loi=="UN") for(int i=0;i<ncl;i++) dens[i]=1.0/(pr.maxi-pr.mini);
        if (pr.loi=="LU") for(int i=0;i<ncl;i++) dens[i]=1.0/(pr.maxi-pr.mini)/x[i];
        if (pr.loi=="NO") for(int i=0;i<ncl;i++) dens[i]=exp(-(x[i]-pr.mean)*(x[i]-pr.mean)/2.0/(pr.sdshape*pr.sdshape))/(pr.sdshape*co);
        if (pr.loi=="LN") for(int i=0;i<ncl;i++) dens[i]=exp(-(log(x[i])/pr.mean)*(log(x[i])/pr.mean)/2.0/(pr.sdshape*pr.sdshape))/(pr.sdshape*co*x[i]);
        if (pr.loi=="GA") for(int i=0;i<ncl;i++) {
            xb=x[i]*pr.sdshape/pr.mean;
            dens[i]=exp((pr.sdshape-1.0)*log(xb)-xb);
        }
        som=0.0;for(int i=0;i<ncl;i++) som +=dens[i];
        for(int i=0;i<ncl;i++) dens[i] /= som;
        return dens;
    }

/**
* calcule la densité à partir d'un échantillon de valeurs'
*/
    double* calculdensite(int ncl,int n, double *x, double **y,int j,bool multithread) { 
        double bw,*dens,sd,*z,denom,som;
        dens = new double[ncl];
        z = new double[n];
        for (int i=0;i<n;i++) z[i]=y[i][j];
        sd =cal_sd(n,z);
        if (sd>0.0) bw=coefbw*exp(-0.2*log((double)n))*sd;
        else bw=1.0;
        cout<<"sd="<<sd<<"    bw="<<bw<<"\n";
#pragma omp parallel for shared(dens,z,x,bw) private(denom,i) if(multithread)
        for (int j=0;j<ncl;j++) {
            dens[j]=0.0;
            for (int i=0;i<n;i++) dens[j] +=exp(lnormal_dens(z[i],x[j],bw));
            if(2*j==ncl-1) cout<<" avant denom dens["<<j<<"] = "<< dens[j]<<"\n"; 
            denom=pnorm5((x[ncl-1]-x[j])/bw,0.0,1.0)-pnorm5((x[0]-x[j])/bw,0.0,1.0);
            if (denom>0.0) dens[j] /=denom;
            if(2*j==ncl-1) cout<<" apres denom dens["<<j<<"] = "<< dens[j]<<"   (denom="<<denom<<")\n"; 
        }
        som=0.0;for (int j=0;j<ncl;j++) {som +=dens[j];}
        if (som>0.0) for (int j=0;j<ncl;j++) dens[j] /=som;
        return dens;
    }

/**
* traitement global du calcul de la densité des paramètres variables
* si le parametre est à valeurs entières avec moins de 30 classes, la densité est remplacée par un histogramme
* sinon la densité est évaluée pour 1000 points du min au max 
*/
    void histodens(int n, bool multithread) {
        bool condition;
        pardens = new pardensC[nparamcom+nparcompo];
        for (int j=0;j<nparamcom+nparcompo;j++) {
            pardens[j].ncl=1001;
            condition=false;
            if (j<npar) {  //histparam
                if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].category<2) {
                    if (header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].prior.maxi-header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].prior.mini<=30) 
                        pardens[j].ncl=header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].prior.maxi-header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].prior.mini+1;
                }
                pardens[j].xmin=header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].prior.mini;
                pardens[j].xmax=header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].prior.maxi;
                if (header.nconditions>0) {
                    for (int i=0;i<header.nconditions;i++) {
                        condition=((header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].name==header.condition[i].param1)or(header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].name==header.condition[i].param2));
                        if (condition) break;
                    }
                }
            } else if (j<nparamcom){  //mutparam
                pardens[j].xmin=header.mutparam[j-npar].prior.mini;
                pardens[j].xmax=header.mutparam[j-npar].prior.maxi;
            } else {  //parametres composites
                pardens[j].xmin=1E100;pardens[j].xmax=0;
                for (int i=0;i<nsimpar;i++) {
                    if (pardens[j].xmin>simpar[i][j]) pardens[j].xmin=simpar[i][j];
                    if (pardens[j].xmax<simpar[i][j]) pardens[j].xmax=simpar[i][j];
                }
                for (int i=0;i<n;i++) {
                    if (pardens[j].xmin>phistar[i][j]) pardens[j].xmin=phistar[i][j];
                    if (pardens[j].xmax<phistar[i][j]) pardens[j].xmax=phistar[i][j];
                }
            } 
            pardens[j].xdelta = (pardens[j].xmax-pardens[j].xmin)/(double)(pardens[j].ncl-1);
            cout<<nomparam[j]<<"   xmin="<<pardens[j].xmin<<"   xmax="<<pardens[j].xmax<<"   xdelta="<<pardens[j].xdelta<<"\n";
            pardens[j].x = new double[pardens[j].ncl];
            for (int k=0;k<pardens[j].ncl;k++) pardens[j].x[k]=pardens[j].xmin+k*pardens[j].xdelta;
            if (pardens[j].ncl>31) {
                if ((condition)or(j>=npar)) pardens[j].priord = calculdensite(pardens[j].ncl,nsimpar,pardens[j].x,simpar,j,multithread);
                else pardens[j].priord =caldensexact(pardens[j].ncl,pardens[j].x,header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].prior);
                pardens[j].postd = calculdensite(pardens[j].ncl,n,pardens[j].x,phistar,j,multithread);
            } else {
/*                if ((condition)or(j>=npar)) pardens[j].priord = calculhisto(pardens[j].x,simpar,j,pardens[j].ncl);
                else pardens[j].priord =calhistsexact(header.scenario[scenchoisi[0]-1].histparam[numpar[0][j]].prior,pardens[j].ncl);
                pardens[j].postd = calculhisto(pardens[j].x,phistar,j,pardens[j].ncl);
*/            }
           cout <<"fin du calcul du parametre "<<j+1<<"  sur "<<nparamcom+nparcompo<<"\n";
        }
     }
 
/**
*calcule les statistiques des paramètres
*/
    void calparstat(int n) {
        double sx,*x;  
        parstat = new parstatC[nparamcom+nparcompo];
        x = new double[n];
        for (int j=0;j<nparamcom+nparcompo;j++) {
            for (int i=0;i<n;i++) x[i] = phistar[i][j];
            sort(&x[0],&x[n]);
            parstat[j].med = x[(int)floor(0.5*n+0.5)-1];
            parstat[j].q025 = x[(int)floor(0.025*n+0.5)-1];
            parstat[j].q050 = x[(int)floor(0.050*n+0.5)-1];
            parstat[j].q250 = x[(int)floor(0.250*n+0.5)-1];
            parstat[j].q750 = x[(int)floor(0.750*n+0.5)-1];
            parstat[j].q950 = x[(int)floor(0.950*n+0.5)-1];
            parstat[j].q975 = x[(int)floor(0.975*n+0.5)-1];
            parstat[j].moy = cal_moy(n,x);
            parstat[j].mod = cal_mode(n,x);
            cout<<nomparam[j]<<"   "<<parstat[j].moy<<"   "<< parstat[j].med<<"   "<<parstat[j].mod<<"\n";
            cout<<nomparam[j]<<"   "<<parstat[j].q025<<"   "<< parstat[j].q050<<"   "<<parstat[j].q250<<"   "<<parstat[j].q750<<"   "<< parstat[j].q950<<"   "<<parstat[j].q975<<"\n";
        }
    }

/**
*sauvegarde les statistiques et les densités des paramètres
*/
    void saveparstat(char *path, char *ident) {
        char *nomfiparstat;
        nomfiparstat = new char[strlen(path)+strlen(ident)+20];
        strcpy(nomfiparstat,path);
        strcat(nomfiparstat,ident);
        strcat(nomfiparstat,"_paramstatdens.txt");
        cout <<nomfiparstat<<"\n";
        FILE *f1;
        f1=fopen(nomfiparstat,"w");
        for (int j=0;j<nparamcom+nparcompo;j++) {
            fprintf(f1,"%s\n",nomparam[j].c_str());
            fprintf(f1,"%8.5e  %8.5e  %8.5e  %8.5e  %8.5e  %8.5e  %8.5e  %8.5e  %8.5e\n",parstat[j].moy,parstat[j].med,parstat[j].mod,parstat[j].q025,parstat[j].q050,parstat[j].q250,parstat[j].q750,parstat[j].q950,parstat[j].q975);
            fprintf(f1,"%d\n",pardens[j].ncl);
            for (int i=0;i<pardens[j].ncl;i++) fprintf(f1,"  %8.5e",pardens[j].x[i]);fprintf(f1,"\n");
            for (int i=0;i<pardens[j].ncl;i++) fprintf(f1,"  %8.5e",pardens[j].priord[i]);fprintf(f1,"\n");
            for (int i=0;i<pardens[j].ncl;i++) fprintf(f1,"  %8.5e",pardens[j].postd[i]);fprintf(f1,"\n");
        }
        fclose(f1);
        strcpy(nomfiparstat,path);
        strcat(nomfiparstat,ident);
        strcat(nomfiparstat,"_psd.txt");
        cout <<nomfiparstat<<"\n";
        f1=fopen(nomfiparstat,"w");
        for (int i=0;i<pardens[0].ncl;i++) {
            for (int j=0;j<nparamcom+nparcompo;j++) fprintf(f1,"%8.5e   %8.5e   %8.5e   ",pardens[j].x[i],pardens[j].priord[i],pardens[j].postd[i]);
            fprintf(f1,"\n");
        }
        fclose(f1);
    }

/** 
* effectue l'estimation ABC des paramètres (directe + régression locale)
*/
    void doestim(char* path, char* ident, char *headerfilename,char *reftablefilename,char *reftablelogfilename,char *statobsfilename,char *options,bool multithread) {
        char *datafilename;
        int rtOK,nstatOK;
        int nrec,nsel,ns,ns1;
        string opt,*ss,s,*ss1,s0,s1;
        double **matX0, *vecW, **alpsimrat,**parsim;
        
        opt=char2string(options);
        ss = splitwords(opt,";",&ns);
        for (int i=0;i<ns;i++) { cout<<ss[i]<<"\n";
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);
            if (s0=="s:") {
                ss1 = splitwords(s1,",",&nscenchoisi);
                scenchoisi = new int[nscenchoisi];
                for (int j=0;j<nscenchoisi;j++) scenchoisi[j] = atoi(ss1[j].c_str());
                cout <<"scenario(s) choisi(s) ";for (int j=0;j<nscenchoisi;j++) cout<<scenchoisi[j]<<",";cout<<"\n";
            } else {
                if (s0=="n:") {
                    nrec=atoi(s1.c_str());    
                    cout<<"nrec = "<<nrec<<"\n";
                } else {
                    if (s0=="m:") {
                        nsel=atoi(s1.c_str());    
                        cout<<"nsel = "<<nsel<<"\n";
                    } else {
                        if (s0=="t:") {
                            numtransf=atoi(s1.c_str());    
                            cout<<"numtransf = "<<numtransf<<"\n";
                        } else {
                            if (s0=="p:") {
                                original=(s1.find("o")!=string::npos);
                                composite=(s1.find("c")!=string::npos);
                                if (original) cout <<"  original  ";
                                if (composite) cout<<"  composite  ";
                                cout<< "\n";
                            }            
                        }
                    }
                }
            }
        }
        
        header.readHeader(headerfilename);
        for (int i=0;i<header.nscenarios;i++) cout<<"scenario "<<i<<"    nparam = "<<header.scenario[i].nparam<<"\n";
        datafilename=strdup(header.datafilename.c_str());
        rtOK=rt.readheader(reftablefilename,reftablelogfilename,datafilename);
        cout <<"\napres rt.readheader\n";
        if (rtOK==1) {cout <<"no file reftable.bin in the current directory\n";exit(1);}          
        rt.openfile2();
        cout<<"apres rt.openfile2\n";
        nstatOK = cal_varstat();
        rt.closefile();
        cout<<"fermeture du fichier apres cal_varstat\n";
        read_statobs(statobsfilename);
        cout <<"avant rt.openfiles2\n";
        rt.openfile2();
        cout <<"apres rt.openfiles2\n";
        cal_dist(nrec,nsel);
        rt.closefile();
        det_numpar();
        recalparam(nsel);
        cout<<"apres recalparam\n";
        rempli_mat(nsel);
        cout<<"apres rempli_mat\n";
        local_regression(nsel,multithread);
        cout<<"apres local_regression\n";
        calphistar(nsel);
        cout<<"apres calphistar\n";
        savephistar(nsel,path,ident);
        cout<<"apres savephistar\n";
        rt.openfile2();
        lisimpar(nsel);
        rt.closefile();
        histodens(nsel,multithread);
        calparstat(nsel);
        saveparstat(path,ident);
    }