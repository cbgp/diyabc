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

extern char *progressfilename;

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

extern ReftableC rt;
extern HeaderC header;
double **matX0, *vecW, **alpsimrat,**parsim;
int nstatOKsel;
extern char *headerfilename, *reftablefilename,*datafilename,*statobsfilename, *reftablelogfilename,*path,*ident;
extern bool multithread;
pardensC *pardens;
double *var_stat, *parmin, *parmax, *diff;
double **beta, **phistar,**simpar;
int nparamcom,nparcompo,**numpar,numtransf,npar,npar0;
double borne=10.0,xborne;
bool composite,original;
string entete;
int nsimpar=100000;
string *nomparam;
parstatC *parstat;


/**
* compte les paramètres communs aux scénarios choisis (nparamcom), le nombre de paramètres composites
* remplit le tableau numpar des numéros de paramètres communs par scénario
*/
    void det_numpar() {
        cout<<"debut de det_numpar\n";
        vector <string>  parname;
        int ii,iscen;
        bool commun,trouve;
        npar=0;npar0=0;
        numpar = new int*[rt.nscenchoisi];
        if (rt.nscenchoisi==1) {
            iscen=rt.scenchoisi[0]-1;
            //cout<<"rt.nparam[rt.scenchoisi[0]-1] = "<<rt.nparam[iscen]<<"\n";
            numpar[0] = new int[rt.nparam[iscen]];
            for (int i=0;i<rt.nhistparam[iscen];i++){
                numpar[0][i]=i;
                npar++;
                parname.push_back(rt.histparam[rt.scenchoisi[0]-1][i].name);
                if (rt.histparam[iscen][i].category<2) npar0++;
            }
            nparamcom = npar+rt.nparamut;//rt.nparam[rt.scenchoisi[0]-1]-header.scenario[rt.scenchoisi[0]-1].nparam;
            for (int i=rt.nhistparam[iscen];i<nparamcom;i++) numpar[0][i]=i;
            cout << "noms des parametres communs : ";
            for (int i=0;i<rt.nhistparam[iscen];i++) cout<<rt.histparam[iscen][i].name<<"   ";
            for (int i=0;i<rt.nparamut;i++) cout<<rt.mutparam[i].name<<"   ";
            cout <<"\n";
        } else {
            iscen=rt.scenchoisi[0]-1;  //on prend les paramètres du premier scénario choisi et on les compare à ceux des autres scénarios
            for (int i=0;i<rt.nhistparam[iscen];i++) {
                commun=true;
                //cout<<"parametre teste : "<<rt.histparam[rt.scenchoisi[0]-1][i].name<<"\n";
                for (int j=1;j<rt.nscenchoisi;j++) {
                    trouve=false;
                    //cout<<"rt.nhistparam[rt.scenchoisi["<<j<<"]-1] = "<<rt.nhistparam[rt.scenchoisi[j]-1]<<"\n";
                    for (int k=0;k<rt.nhistparam[rt.scenchoisi[j]-1];k++) {
                        //cout<<"   "<<rt.histparam[rt.scenchoisi[j]-1][k].name;
                        trouve=(rt.histparam[rt.scenchoisi[0]-1][i].name.compare(rt.histparam[rt.scenchoisi[j]-1][k].name)==0);
                        if (trouve) {/*cout<< "\nparam commun : "<<rt.histparam[rt.scenchoisi[0]-1][i].name<<"\n";*/ break;}
                    }
                    commun=trouve;
                    if (not commun) break;
                }
                if (commun) {
                    if (not rt.histparam[rt.scenchoisi[0]-1][i].prior.constant){
                        npar++;//cout<<"npar = "<<npar<<"\n";
                        parname.push_back(rt.histparam[rt.scenchoisi[0]-1][i].name);
                        //cout<<parname[npar-1]<<"\n\n";
                        if (rt.histparam[rt.scenchoisi[0]-1][i].category<2) npar0++;
                    }
                }// else cout<<"\nnon commun\n\n";
            }
            //cout<<"avant affichage des parname\n";
            //for (int i=0;i<npar;i++) cout<<parname[i]<<"   ";
            //cout<<"\n";
            nparamcom = npar+rt.nparamut;//rt.nparam[rt.scenchoisi[0]-1]-header.scenario[rt.scenchoisi[0]-1].nparam;
            for (int j=0;j<rt.nscenchoisi;j++) {
                numpar[j] = new int[nparamcom];
                ii=0;
                for (int i=0;i<npar;i++) {
                    for (int k=0;k<rt.nhistparam[rt.scenchoisi[j]-1];k++) {
                        if (rt.histparam[rt.scenchoisi[j]-1][k].name.compare(parname[i])==0) {numpar[j][ii]=k;ii++;}
                    }
                 }
                for (int k=rt.nhistparam[rt.scenchoisi[j]-1];k<rt.nparam[rt.scenchoisi[j]-1];k++) {numpar[j][ii]=k;ii++;}
                for (int kk=0;kk<npar+header.nparamut;kk++) {
                  cout <<numpar[j][kk]<<"  ";
                  if (kk<npar) cout<<"("<<rt.histparam[rt.scenchoisi[j]-1][numpar[j][kk]].name<<")  ";
                  else cout<<"("<<rt.mutparam[kk-npar].name<<")  ";
                }
                cout<<"\n";
            }
            /*cout << "noms des parametres communs : ";
            for (int i=0;i<npar;i++) cout<<parname[i]<<"   ";
            cout <<"\n";*/
        }
        /*cout << "noms des parametres communs : ";
        for (int j=0;j<rt.nscenchoisi;j++) {
            cout<<"scenario "<<rt.scenchoisi[j]<<"\n";
            for (int i=0;i<nparamcom;i++) {cout<<numpar[j][i]<<" ";
                if (i<npar) cout<<"("<<rt.histparam[rt.scenchoisi[j]-1][numpar[j][i]].name<<")  ";
                else cout<<"("<<rt.mutparam[i-npar].name<<")  ";
            }
            cout <<"\n";
        }*/
        if (composite) nparcompo=npar0*header.ngroupes; else nparcompo=0;
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
                           k=0;while(rt.enrsel[i].numscen!=rt.scenchoisi[k])k++;
                           alpsimrat[i][j] = rt.enrsel[i].param[numpar[k][j]];
                       }
                   }
                   break;
          case 2 : // log transform
                   for (int i=0;i<n;i++) {
                       for (int j=0;j<nparamcom;j++) {
                           k=0;while(rt.enrsel[i].numscen!=rt.scenchoisi[k])k++;
                           if (rt.enrsel[i].param[numpar[k][j]]<=0.00000000001) rt.enrsel[i].param[numpar[k][j]]=1E-11;
                           alpsimrat[i][j] = log(rt.enrsel[i].param[numpar[k][j]]);
                       }
                   }
                   break;
          case 3 : //logit transform
                   //cout<<"borne="<<borne<<"\n";
                   parmin = new double[nparamcom]; parmax = new double[nparamcom]; diff = new double[nparamcom];
                   if (borne<0.0000000001) {
                       xborne=1E100;
                       for (int j=0;j<nparamcom;j++) {parmin[j]=1E100;parmax[j]=-1E100;}
                       for (int i=0;i<n;i++) {
                            for (int j=0;j<nparamcom;j++) {
                                k=0;while(rt.enrsel[i].numscen!=rt.scenchoisi[k])k++;
                                if (rt.enrsel[i].param[numpar[k][j]]<parmin[j]) parmin[j]=rt.enrsel[i].param[numpar[k][j]];
                                if (rt.enrsel[i].param[numpar[k][j]]>parmax[j]) parmax[j]=rt.enrsel[i].param[numpar[k][j]];
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
                       //k=0;while(rt.enrsel[i].numscen!=rt.scenchoisi[k]) k++;
                       //k=rt.scenchoisi[0]-1;
                       //cout<<"k="<<k<<"\n";
                       for (int j=0;j<nparamcom-rt.nparamut;j++) {
                           cout<<"j="<<j<<"   nparamcom="<<nparamcom<<"\n";
                           if (rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].category<2) {
                               parmin[j] = rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.mini-0.5;
                               parmax[j] = rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.maxi+0.5;
                           } else {
                               parmin[j] = rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.mini-0.0005;
                               parmax[j] = rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.maxi+0.0005;
                           }
                           diff[j]=parmax[j]-parmin[j];
                           cout<<rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].name<<"   ";
                           cout<<"parmin = "<<parmin[j]<<"   parmax="<<parmax[j]<<"\n";
                       }
                       cout<<rt.nhistparam[rt.scenchoisi[0]-1]<<"\n";
                       cout<<nparamcom<<"\n";
                       for (int j=nparamcom-rt.nparamut;j<nparamcom;j++) {
                               jj=j+rt.nparamut-nparamcom;
                               cout<<"jj="<<jj<<"\n";
                               parmin[j]=0.95*rt.mutparam[jj].prior.mini;
                               parmax[j]=1.05*rt.mutparam[jj].prior.maxi;
                               cout<<header.mutparam[jj].category<<"   ";
                               cout<<"parmin = "<<parmin[j]<<"   parmax="<<parmax[j]<<"\n";
                               diff[j]=parmax[j]-parmin[j];
                       }
                   }
                   cout <<"fin du calcul de parmin/parmax rt.scenchoisi[0] = "<<rt.scenchoisi[0]<<"\n";
                   for (int i=0;i<nparamcom;i++) cout <<parmin[i]<<"   "<<parmax[i]<<"\n";
                   for (int i=0;i<n;i++) {
                       for (int j=0;j<nparamcom;j++) {
                           //cout<<"numscen = "<<rt.enrsel[i].numscen<<"  ";
                           //for (int k0=0;k0<rt.nscenchoisi;k0++) cout<<rt.scenchoisi[k0]<<"  ";cout<<"\n";
                           k=0;while(rt.enrsel[i].numscen != rt.scenchoisi[k])k++;
                           //k = rt.enrsel[i].numscen-1;
                           //cout <<"k="<<k<<"   j="<<j<<"\n";
                           //cout<<numpar[k][j]<<"\n";
                           if (rt.enrsel[i].param[numpar[k][j]]<=parmin[j]) alpsimrat[i][j] = -xborne;
                           else if (rt.enrsel[i].param[numpar[k][j]]>=parmax[j]) alpsimrat[i][j] = xborne;
                           else {
                             //cout<<rt.enrsel[i].param[numpar[k][j]]<<"   "<<parmin[j]<<"   "<<parmax[j]<<"\n";
                             alpsimrat[i][j] =log((rt.enrsel[i].param[numpar[k][j]]-parmin[j])/(parmax[j]-rt.enrsel[i].param[numpar[k][j]]));
                           }
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
                                k=0;while(rt.enrsel[i].numscen!=rt.scenchoisi[k])k++;
                                //k = rt.enrsel[i].numscen-1;
                                if (rt.enrsel[i].param[numpar[k][j]]<parmin[j]) parmin[j]=rt.enrsel[i].param[numpar[k][j]];
                                if (rt.enrsel[i].param[numpar[k][j]]>parmax[j]) parmax[j]=rt.enrsel[i].param[numpar[k][j]];
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
                       //k=0;while(rt.enrsel[i].numscen!=rt.scenchoisi[k])k++;
                       //k=rt.scenchoisi[0]-1;
                       for (int j=0;j<nparamcom-rt.nparamut;j++) {
                           if (rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].category<2) {
                               parmin[j] = rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.mini-0.5;
                               parmax[j] = rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.maxi+0.5;
                           } else {
                               parmin[j] = rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.mini-0.0005;
                               parmax[j] = rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.maxi+0.0005;
                           }
                           diff[j]=parmax[j]-parmin[j];
                           //cout<<header.scenario[k].histparam[numpar[k][j]].name<<"   ";
                           //cout<<"parmin = "<<parmin[j]<<"   parmax="<<parmax[j]<<"\n";
                       }
                       for (int j=nparamcom-rt.nparamut;j<nparamcom;j++) {
                               jj=j-(nparamcom-rt.nparamut);
                               //cout<<"jj="<<jj<<"\n";
                               parmin[j]=0.95*rt.mutparam[jj].prior.mini;
                               parmax[j]=1.05*rt.mutparam[jj].prior.maxi;
                               //cout<<rt.mutparam[jj].category<<"   ";
                               //cout<<"parmin = "<<parmin[j]<<"   parmax="<<parmax[j]<<"\n";
                               //diff[j]=parmax[j]-parmin[j];
                       }
                   }
                   //cout <<"fin du calcul de parmin/parmax\n";
                   for (int i=0;i<n;i++) {
                       for (int j=0;j<nparamcom;j++) {
                           k=0;while(rt.enrsel[i].numscen!=rt.scenchoisi[k])k++;
                           //k = rt.enrsel[i].numscen-1;
                           if (rt.enrsel[i].param[numpar[k][j]]<=parmin[j]) alpsimrat[i][j] = -xborne;
                           else if (rt.enrsel[i].param[numpar[k][j]]>=parmax[j]) alpsimrat[i][j] = xborne;
                           else alpsimrat[i][j] =log(tan((rt.enrsel[i].param[numpar[k][j]]-parmin[j])*c/diff[j]));
                       }
                   }
                   break;
        }
    }

/**
* effectue le remplissage de la matrice matX0, du vecteur des poids vecW et
* de la matrice des paramètres parsim (éventuellement transformés)
*/
    void rempli_mat(int n,double* stat_obs) {
        int icc;
        double delta,som,x,*var_statsel,nn;
        double *sx,*sx2,*mo;
        nn=(double)n;
        delta = rt.enrsel[n-1].dist;
        //cout<<"delta="<<delta<<"\n";
        sx  = new double[rt.nstat];
        sx2 = new double[rt.nstat];
        mo  = new double[rt.nstat];
        var_statsel = new double[rt.nstat];
        for (int i=0;i<rt.nstat;i++){sx[i]=0.0;sx2[i]=0.0;mo[i]=0.0;}
        for (int i=0;i<n;i++){
            for (int j=0;j<rt.nstat;j++) {
                x = rt.enrsel[i].stat[j];
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
        //cout <<"hello\n";
        som=0.0;
        for (int i=0;i<n;i++) {
            icc=-1;
            for (int j=0;j<rt.nstat;j++) {
                if (var_statsel[j]>0.0) {
                    icc++;
                    matX0[i][icc]=(rt.enrsel[i].stat[j]-stat_obs[j])/sqrt(var_statsel[j]);
                }
            }
            x=rt.enrsel[i].dist/delta;
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
    void local_regression(int n) {
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
        matB = invM(nstatOKsel+1,matAA);
        matC = prodM(nstatOKsel+1,nstatOKsel+1,n,matB,matA);
        beta = prodM(nstatOKsel+1,n,nparamcom,matC,parsim);
        //ecrimat("beta",nstatOKsel+1,nparamcom,beta);
        libereM(nstatOKsel+1,matA);
        libereM(n,matX);
        libereM(nstatOKsel+1,matB);
        libereM(nstatOKsel+1,matAA);
        libereM(nstatOKsel+1,matC);
        for (int i=0;i<n;i++) delete []parsim[i];delete []parsim;
    }

/**
* calcule les phistars pour les paramètres originaux et composites
*/
    double** calphistar(int n){
        //cout<<"debut de calphistar\n";
        int k,kk,qq;
        double pmut;
        double **phista;
        phista = new double*[n];
        for (int i=0;i<n;i++) phista[i] = new double[nparamcom+nparcompo];
        for (int i=0;i<n;i++) {
            for (int j=0;j<nparamcom;j++) {
                phista[i][j] = alpsimrat[i][j];
                if (i==0) cout<< phista[i][j]<<"   ";
                for (int k=0;k<nstatOKsel;k++) phista[i][j] -= matX0[i][k]*beta[k+1][j];
                if(i==0) cout<< phista[i][j]<<"   ";
                switch(numtransf) {
                  case 1 : break;
                  case 2 : if (phista[i][j]<100.0) phista[i][j] = exp(phista[i][j]); else phista[i][j]=exp(100.0);
                           break;
                  case 3 : if (phista[i][j]<=-xborne) phista[i][j] = parmin[j];
                           else if (phista[i][j]>=xborne) phista[i][j] = parmax[j];
                           else phista[i][j] = (exp(phista[i][j])*parmax[j]+parmin[j])/(1.0+exp(phista[i][j]));
                           if(i==0) cout<< phista[i][j]<<"   ("<<parmin[j]<<","<<parmax[j]<<")\n";
                           break;
                  case 4 : if (phista[i][j]<=-xborne) phista[i][j] = parmin[j];
                           else if (phista[i][j]>=xborne) phista[i][j] = parmax[j];
                           else phista[i][j] =parmin[j] +(diff[j]/c*atan(exp(phista[i][j])));
                           break;
                }
            }
        }
        delete []parmin; delete []parmax;delete []diff;
        if (nparcompo>0) {
            k=0;
            for (int gr=1;gr<header.ngroupes+1;gr++) {
                if (header.groupe[gr].type==0) {
                    if (header.groupe[gr].priormutmoy.constant) {
                        if (header.groupe[gr].priorsnimoy.constant) {
                            pmut = header.groupe[gr].mutmoy+header.groupe[gr].snimoy;
                            for (int j=0;j<npar;j++) {
                                if (header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                    for (int i=0;i<n;i++) {
                                          phista[i][nparamcom+k] = pmut*phista[i][j];
                                    }
                                    k++;
                                }
                            }
                        } else {
                            kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==2))) kk++;
                            for (int j=0;j<npar;j++) {
                                if (header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                    for (int i=0;i<n;i++) {
                                        pmut = header.groupe[gr].mutmoy+phista[i][npar+kk];
                                        phista[i][nparamcom+k] = pmut*phista[i][j];
                                    }
                                    k++;
                                }
                            }
                        }
                    } else {
                        if (header.groupe[gr].priorsnimoy.constant) {
                            kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==0))) kk++;
                            for (int j=0;j<npar;j++) {
                                if (header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                    for (int i=0;i<n;i++) {
                                        pmut =phista[i][npar+kk] +header.groupe[gr].snimoy;
                                        phista[i][nparamcom+k] = pmut*phista[i][j];
                                    }
                                    k++;
                                }
                            }
                        } else {
                            kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==0))) kk++;
                            qq=0;while (not ((header.mutparam[qq].groupe == gr)and(header.mutparam[qq].category ==2))) qq++;
                            for (int j=0;j<npar;j++) {
                                if (header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                    for (int i=0;i<n;i++) {
                                        pmut =phista[i][npar+kk]+phista[i][npar+qq];
                                        phista[i][nparamcom+k] = pmut*phista[i][j];
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
                            if (header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                for (int i=0;i<n;i++) {
                                    phista[i][nparamcom+k] = pmut*phista[i][j];
                                }
                                k++;
                            }
                        }
                    } else {
                        kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==3))) kk++;
                        for (int j=0;j<npar;j++) {
                            if (header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                for (int i=0;i<n;i++) {
                                    pmut = phista[i][npar+kk];
                                    phista[i][nparamcom+k] = pmut*phista[i][j];
                                }
                                k++;
                            }
                        }
                    }
                }
            }
        }
        //cout<<"nparcompo = "<<nparcompo<<"   k="<<k<<"\n";
        for (int i=0;i<n;i++) delete []alpsimrat[i];delete []alpsimrat;
        for (int i=0;i<n;i++) delete []matX0[i]; delete []matX0;
        for (int i=0;i<nstatOKsel+1;i++) delete []beta[i]; delete []beta;
        return phista;
    }

    void det_nomparam() {
        int k;
        cout<<"debut de det_nomparam\n";
        nomparam =new string[nparamcom+nparcompo];
        string pp;
        entete="scenario";
        for (int j=0;j<npar;j++) {
            nomparam[j]=string(rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].name);
            entete += centre(rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].name,15);}
        for (int j=npar;j<nparamcom;j++) {
            nomparam[j]=string(rt.mutparam[j-npar].name);
            entete += centre(rt.mutparam[j-npar].name,15);}
        if (nparcompo>0) {
            k=nparamcom;
            for (int gr=1;gr<header.ngroupes+1;gr++) {
                for (int j=0;j<npar;j++) {
                    if (rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].category<2){
                        pp=rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].name;
                        if (header.groupe[gr].type==0) pp = pp+"(u+sni)_"+IntToString(gr)+" ";
                        if (header.groupe[gr].type==1) pp = pp+"useq_"+IntToString(gr)+" ";
                        nomparam[k]=pp;k++;
                        entete += centre(pp,16);
                    }
                }
            }
        }
        for (int j=0;j<nparamcom+nparcompo;j++) cout<<nomparam[j]<<"\n";
        cout<<"nparamcom="<<nparamcom<<"   nparcompo="<<nparcompo<<"\n";

    }


/**
* effectue la sauvegarde des phistars dans le fichier path/ident/phistar.txt
*/
    void savephistar(int n, char* path,char* ident) {
        char *nomphistar ;
        /*int k;
        nomparam =new string[nparamcom+nparcompo];
        string pp;
        entete="scenario";
        for (int j=0;j<npar;j++) {
            nomparam[j]=string(header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].name);
            entete += centre(header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].name,15);}
        for (int j=npar;j<nparamcom;j++) {
            nomparam[j]=string(header.mutparam[j-npar].name);
            entete += centre(header.mutparam[j-npar].name,15);}
        if (nparcompo>0) {
            k=nparamcom;
            for (int gr=1;gr<header.ngroupes+1;gr++) {
                for (int j=0;j<npar;j++) {
                    if (header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                        pp=header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].name;
                        if (header.groupe[gr].type==0) pp = pp+"(u+sni)_"+IntToString(gr)+" ";
                        if (header.groupe[gr].type==1) pp = pp+"useq_"+IntToString(gr)+" ";
                        nomparam[k]=pp;k++;
                        entete += centre(pp,16);
                    }
                }
            }
        }*/
        nomphistar = new char[strlen(path)+strlen(ident)+20];
        strcpy(nomphistar,path);
        strcat(nomphistar,ident);
        strcat(nomphistar,"_phistar.txt");
        //cout <<nomphistar<<"\n";
        FILE *f1;
        f1=fopen(nomphistar,"w");
        fprintf(f1,"%s\n",entete.c_str());
        for (int i=0;i<n;i++) {
            fprintf(f1,"  %d    ",rt.enrsel[i].numscen);
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
      int bidon,iscen,m,k,kk,qq,nr;
        bool scenOK;
        double pmut;
        if (nsimpar>rt.nrec) nsimpar=rt.nrec;
        nr=0;for (int i=0;i<rt.nscenchoisi;i++) nr+=rt.nrecscen[rt.scenchoisi[i]-1];
        if (nsimpar>nr) nsimpar=nr;
        simpar = new double*[nsimpar];
        enregC enr;
        enr.stat = new float[rt.nstat];
        enr.param = new float[rt.nparamax];
        int i=-1;
        rt.openfile2();
        while (i<nsimpar-1) {
            bidon=rt.readrecord(&enr);
            scenOK=false;iscen=0;
            while((not scenOK)and(iscen<rt.nscenchoisi)) {
                scenOK=(enr.numscen==rt.scenchoisi[iscen]);
                iscen++;
            }
            if (scenOK) {
              i++;
              simpar[i] = new double[nparamcom+nparcompo];
              m=0;while(enr.numscen!=rt.scenchoisi[m]) m++;
              for (int j=0;j<nparamcom;j++) simpar[i][j] = enr.param[numpar[m][j]];
              if (nparcompo>0) {
                  k=0;
                  for (int gr=1;gr<header.ngroupes+1;gr++) {
                      if (header.groupe[gr].type==0) {
                          if (header.groupe[gr].priormutmoy.constant) {
                              if (header.groupe[gr].priorsnimoy.constant) {
                                  pmut = header.groupe[gr].mutmoy+header.groupe[gr].snimoy;
                                  for (int j=0;j<npar;j++) {
                                      if (header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                          simpar[i][nparamcom+k] = pmut*enr.param[numpar[m][j]];
                                          k++;
                                      }
                                  }
                              } else {
                                  kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==2))) kk++;
                                  for (int j=0;j<npar;j++) {
                                      if (header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                          pmut = header.groupe[gr].mutmoy+enr.param[numpar[m][npar+kk]];
                                          simpar[i][nparamcom+k] = pmut*enr.param[numpar[m][j]];
                                          k++;
                                      }
                                  }
                              }
                          } else {
                              if (header.groupe[gr].priorsnimoy.constant) {
                                  kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==0))) kk++;
                                  for (int j=0;j<npar;j++) {
                                      if (header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                          pmut =enr.param[numpar[0][npar+kk]]+header.groupe[gr].snimoy;
                                          simpar[i][nparamcom+k] = pmut*enr.param[numpar[0][j]];
                                          k++;
                                      }
                                  }
                              } else {
                                  kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==0))) kk++;
                                  qq=0;while (not ((header.mutparam[qq].groupe == gr)and(header.mutparam[qq].category ==2))) qq++;
                                  for (int j=0;j<npar;j++) {
                                      if (header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                          pmut =enr.param[numpar[m][npar+kk]]+enr.param[numpar[m][npar+qq]];
                                          simpar[i][nparamcom+k] = pmut*enr.param[numpar[m][j]];
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
                                  if (header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                      simpar[i][nparamcom+k] = pmut*enr.param[numpar[m][j]];
                                      k++;
                                  }
                              }
                          } else {
                              kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==3))) kk++;
                              for (int j=0;j<npar;j++) {
                                  if (header.scenario[rt.scenchoisi[0]-1].histparam[numpar[0][j]].category<2){
                                      pmut = enr.param[numpar[m][npar+kk]];
                                      simpar[i][nparamcom+k] = pmut*enr.param[numpar[m][j]];
                                      k++;
                                  }
                              }
                          }
                      }
                  }
               }
            }
        }
        rt.closefile();
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
* calcule un histogramme à partir de la loi fournie dans le prior
*/
    double* calhistexact(int ncl,double *x,PriorC pr) {
        double *dens,xb,som,z,pas=x[1]-x[0];
        int ncl2=11;
        dens = new double[ncl];
        if (pr.loi=="UN") for(int i=0;i<ncl;i++) dens[i]=1.0/(pr.maxi-pr.mini);
        if (pr.loi=="LU") for(int i=0;i<ncl;i++) {
            dens[i]=0.0;
            for (int j=0;j<ncl2-1;j++){
                z=x[i]+(double)j*pas/(double)(ncl2-1);
                dens[i]+=1.0/(pr.maxi-pr.mini)/z/(ncl2-1);
            }
        }
        if (pr.loi=="NO") for(int i=0;i<ncl;i++){
            dens[i]=0.0;
            for (int j=0;j<ncl2-1;j++){
                z=x[i]+(double)j*pas/(double)(ncl2-1);
                dens[i]+=exp(-(z-pr.mean)*(z-pr.mean)/2.0/(pr.sdshape*pr.sdshape))/(pr.sdshape*co);
            }
        }
        if (pr.loi=="LN") for(int i=0;i<ncl;i++) {
            dens[i]=0.0;
            for (int j=0;j<ncl2-1;j++){
                z=x[i]+(double)j*pas/(double)(ncl2-1);
                dens[i]+=exp(-(log(z)/pr.mean)*(log(z)/pr.mean)/2.0/(pr.sdshape*pr.sdshape))/(pr.sdshape*co*x[i]);
            }
        }
        if (pr.loi=="GA") for(int i=0;i<ncl;i++) {
            dens[i]=0.0;
            for (int j=0;j<ncl2-1;j++){
                z=x[i]+(double)j*pas/(double)(ncl2-1);
                xb=z*pr.sdshape/pr.mean;
                dens[i]+=exp((pr.sdshape-1.0)*log(xb)-xb);
            }
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
        //cout<<"sd="<<sd<<"    bw="<<bw<<"\n";
#pragma omp parallel for shared(dens,z,x,bw) private(denom,i) if(multithread)
        for (int j=0;j<ncl;j++) {
            dens[j]=0.0;
            for (int i=0;i<n;i++) dens[j] +=exp(lnormal_dens(z[i],x[j],bw));
            //if(2*j==ncl-1) cout<<" avant denom dens["<<j<<"] = "<< dens[j]<<"\n";
            denom=pnorm5((x[ncl-1]-x[j])/bw,0.0,1.0)-pnorm5((x[0]-x[j])/bw,0.0,1.0);
            if (denom>0.0) dens[j] /=denom;
            //if(2*j==ncl-1) cout<<" apres denom dens["<<j<<"] = "<< dens[j]<<"   (denom="<<denom<<")\n";
        }
        som=0.0;for (int j=0;j<ncl;j++) {som +=dens[j];}
        if (som>0.0) for (int j=0;j<ncl;j++) dens[j] /=som;
        delete []z;
        return dens;
    }

/**
* calcule un histogramme à partir d'un échantillon de valeurs'
*/
    double* calculhisto(int ncl, int n, double *x, double **y,int j,bool multithread) {
        int k;
        double som,*dens,*z,d,demipas=0.5*(x[1]-x[0]);
        dens = new double[ncl];
        z = new double[n];
        for (int i=0;i<n;i++) z[i]=y[i][j];
        //for (int i=0;i<n;i++) cout <<"  "<<z[i];cout<<"\n";
        for (int i=0;i<ncl;i++) dens[i]=0.0;
        d=1.0/(double)n;
#pragma omp parallel for shared(dens,z,x) private(k) if(multithread)
        for (int i=0;i<n;i++) {
            for (k=0;k<ncl;k++) if (fabs(z[i]-x[k])<=demipas) break;
            dens[k] += d;
            if((k==0)or(k==ncl-1)) dens[k] += d;
        }
        som=0.0;for (int k=0;k<ncl;k++) {som +=dens[k];}
        if (som>0.0) for (int k=0;k<ncl;k++) dens[k] /=som;
        delete []z;
        return dens;
    }

/**
* traitement global du calcul de la densité des paramètres variables
* si le parametre est à valeurs entières avec moins de 30 classes, la densité est remplacée par un histogramme
* sinon la densité est évaluée pour 1000 points du min au max
*/
    void histodens(int n, bool multithread, char* progressfilename,int* iprog,int* nprog) {
        bool condition;
        double *densprior,*denspost,*x,delta;
        int ncl,ii;
        FILE *flog;
        pardens = new pardensC[nparamcom+nparcompo];
        for (int j=0;j<nparamcom+nparcompo;j++) {
            pardens[j].ncl=1001;
            condition=false;
            if (j<npar) {  //histparam
                if (rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].category<2) {
                    if (rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.maxi-rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.mini<=30)
                        pardens[j].ncl=rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.maxi-rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.mini+1;
                }
                pardens[j].xmin=rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.mini;
                pardens[j].xmax=rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior.maxi;
                if (header.nconditions>0) {
                    for (int i=0;i<header.nconditions;i++) {
                        condition=((rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].name==header.condition[i].param1)or(rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].name==header.condition[i].param2));
                        if (condition) break;
                    }
                }
            } else if (j<nparamcom){  //mutparam
                pardens[j].xmin=rt.mutparam[j-npar].prior.mini;
                pardens[j].xmax=rt.mutparam[j-npar].prior.maxi;
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
                else pardens[j].priord =caldensexact(pardens[j].ncl,pardens[j].x,rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior);
                pardens[j].postd = calculdensite(pardens[j].ncl,n,pardens[j].x,phistar,j,multithread);
            } else {
                /*if ((condition)or(j>=npar)) pardens[j].priord = calculhisto(pardens[j].ncl,nsimpar,pardens[j].x,simpar,j,multithread);
                else pardens[j].priord =calhistexact(pardens[j].ncl,pardens[j].x,rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior);
                pardens[j].postd = calculhisto(pardens[j].ncl,n,pardens[j].x,phistar,j,multithread);*/
                if ((condition)or(j>=npar)) densprior = calculhisto(pardens[j].ncl,nsimpar,pardens[j].x,simpar,j,multithread);
                else densprior =calhistexact(pardens[j].ncl,pardens[j].x,rt.histparam[rt.scenchoisi[0]-1][numpar[0][j]].prior);
                denspost = calculhisto(pardens[j].ncl,n,pardens[j].x,phistar,j,multithread);
                ncl=pardens[j].ncl;
                for (int k=0;k<ncl;k++) cout<<"   "<<pardens[j].x[k];cout<<"\n";
                for (int k=0;k<ncl;k++) cout<<"   "<<densprior[k];cout<<"\n";
                for (int k=0;k<ncl;k++) cout<<"   "<<denspost[k];cout<<"\n";
                x=new double[ncl];
                for (int k=0;k<ncl;k++) x[k]=pardens[j].x[k];
                delete []pardens[j].x;
                pardens[j].ncl = 1001;
                pardens[j].x = new double[pardens[j].ncl];
                pardens[j].priord = new double[pardens[j].ncl];
                pardens[j].postd = new double[pardens[j].ncl];
                pardens[j].xdelta = (x[ncl-1]-x[0])/(double)(pardens[j].ncl-1);
                delta = x[1]-x[0];
                for (int k=0;k<pardens[j].ncl;k++) {
                    pardens[j].x[k]=x[0]+k*pardens[j].xdelta;
                    for (ii=0;ii<ncl-1;ii++) {
                        if ((pardens[j].x[k]>=x[ii])and(pardens[j].x[k]<=x[ii+1])) break;
                    }
                    pardens[j].priord[k]=0.5*(densprior[ii]+densprior[ii+1]);
                    pardens[j].postd[k]=0.5*(denspost[ii]+denspost[ii+1]);
                    //cout<<"x="<<pardens[j].x[k]<<"   priord="<<pardens[j].priord[k]<<"   postd="<<pardens[j].postd[k]<<"   ii="<<ii<<"\n";
                }
                delete [] densprior;
                delete [] denspost;
            }
           cout <<"fin du calcul du parametre "<<j+1<<"  sur "<<nparamcom+nparcompo<<"\n";
        *iprog+=10;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",*iprog,*nprog);fclose(flog);
        }
     }

/**
*calcule les statistiques des paramètres
*/
    parstatC* calparstat(int n) {
        double sx,*x;
        parstat = new parstatC[nparamcom+nparcompo];
        x = new double[n];
        //cout <<"avant la boucle sur les parametres nparamcom="<<nparamcom<<"  nparcompo="<<nparcompo<<"   nsel="<<n<<"\n";
        for (int j=0;j<nparamcom+nparcompo;j++) {
            for (int i=0;i<n;i++) x[i] = phistar[i][j];
            //if (j==0) for (int i=0;i<20;i++) cout <<phistar[i][j]<<"  "; cout<<"\n";
            //cout<<"allocation des x du parametre "<<j<<"\n";
            sort(&x[0],&x[n]);
            //cout<<"apres le sort\n";
            parstat[j].med = x[(int)floor(0.5*n+0.5)-1];
            parstat[j].q025 = x[(int)floor(0.025*n+0.5)-1];
            parstat[j].q050 = x[(int)floor(0.050*n+0.5)-1];
            parstat[j].q250 = x[(int)floor(0.250*n+0.5)-1];
            parstat[j].med  = x[(int)floor(0.500*n+0.5)-1];
            parstat[j].q750 = x[(int)floor(0.750*n+0.5)-1];
            parstat[j].q950 = x[(int)floor(0.950*n+0.5)-1];
            parstat[j].q975 = x[(int)floor(0.975*n+0.5)-1];
            parstat[j].moy = cal_moy(n,x);
            //cout<<"apres cal_moy\n";
            parstat[j].mod = cal_mode(n,x);
            //cout<<"apres cal_mode\n";
            //for (int i=0;i<16-nomparam[j].length();i++) cout<<" ";
            //cout<<nomparam[j];
            //printf(" %7.2e  %7.2e  %7.2e  %7.2e  %7.2e  %7.2e  %7.2e\n",parstat[j].moy,parstat[j].med,parstat[j].mod,parstat[j].q025,parstat[j].q050,parstat[j].q950,parstat[j].q975);
        }
        return parstat;
    }

/**
*sauvegarde les statistiques et les densités des paramètres
*/
    void saveparstat(int nsel,char *path, char *ident) {
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        char *nomfiparstat;
        nomfiparstat = new char[strlen(path)+strlen(ident)+20];
        strcpy(nomfiparstat,path);
        strcat(nomfiparstat,ident);
        strcat(nomfiparstat,"_paramstatdens.txt");
        cout <<nomfiparstat<<"\n";
        FILE *f1;
        f1=fopen(nomfiparstat,"w");
        for (int j=0;j<nparamcom+nparcompo;j++) {
          cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s\n",nomparam[j].c_str());cout<<"1\n";
            fprintf(f1,"%8.5e  %8.5e  %8.5e  %8.5e  %8.5e  %8.5e  %8.5e  %8.5e  %8.5e\n",parstat[j].moy,parstat[j].med,parstat[j].mod,parstat[j].q025,parstat[j].q050,parstat[j].q250,parstat[j].q750,parstat[j].q950,parstat[j].q975);
            cout<<"2\n";
            fprintf(f1,"%d\n",pardens[j].ncl);cout<<"3\n";
            for (int i=0;i<pardens[j].ncl;i++) fprintf(f1,"  %8.5e",pardens[j].x[i]);fprintf(f1,"\n");cout<<"3\n";
            for (int i=0;i<pardens[j].ncl;i++) fprintf(f1,"  %8.5e",pardens[j].priord[i]);fprintf(f1,"\n");cout<<"4\n";
            for (int i=0;i<pardens[j].ncl;i++) fprintf(f1,"  %8.5e",pardens[j].postd[i]);fprintf(f1,"\n");cout<<"5\n";
        }
        fclose(f1);
        cout<<"apres close(f1)\n";
        strcpy(nomfiparstat,path);
        strcat(nomfiparstat,ident);
        strcat(nomfiparstat,"_mmmq.txt");
        cout <<nomfiparstat<<"\n";
        f1=fopen(nomfiparstat,"w");
        fprintf(f1,"DIYABC :                      ABC parameter estimation                         %s\n",asctime(timeinfo));
        fprintf(f1,"Data file       : %s\n",header.datafilename.c_str());
        fprintf(f1,"Reference table : %s\n",rt.filename);
        switch (numtransf) {
              case 1 : fprintf(f1,"No transformation of parameters\n");break;
              case 2 : fprintf(f1,"Transformation LOG of parameters\n");break;
              case 3 : fprintf(f1,"Transformation LOGIT of parameters\n");break;
              case 4 : fprintf(f1,"Transformation LOG(TG) of parameters\n");break;
        }
        fprintf(f1,"Chosen scenario(s) : ");for (int i=0;i<rt.nscenchoisi;i++) fprintf(f1,"%d ",rt.scenchoisi[i]);fprintf(f1,"\n");
        fprintf(f1,"Number of simulated data sets : %d\n",rt.nreclus);
        fprintf(f1,"Number of selected data sets  : %d\n\n",nsel);
        fprintf(f1,"Parameter          mean     median    mode      q025      q050      q250      q750      q950      q975\n");
        fprintf(f1,"------------------------------------------------------------------------------------------------------\n");
        for (int j=0;j<nparamcom+nparcompo;j++) {
            fprintf(f1,"%s",nomparam[j].c_str());
            for(int i=0;i<16-nomparam[j].length();i++)fprintf(f1," ");
            fprintf(f1,"%8.2e  %8.2e  %8.2e  %8.2e  %8.2e  %8.2e  %8.2e  %8.2e  %8.2e\n",parstat[j].moy,parstat[j].med,parstat[j].mod,parstat[j].q025,parstat[j].q050,parstat[j].q250,parstat[j].q750,parstat[j].q950,parstat[j].q975);
        }
        fclose(f1);
    }

/**
* effectue l'estimation ABC des paramètres (directe + régression locale)
*/
    void doestim(char *options) {
        int nstatOK, iprog,nprog;
        int nrec,nsel,ns,ns1,nrecpos;
        string opt,*ss,s,*ss1,s0,s1;
        double  *stat_obs;

        FILE *flog;

        progressfilename = new char[strlen(path)+strlen(ident)+20];
        strcpy(progressfilename,path);
        strcat(progressfilename,ident);
        strcat(progressfilename,"_progress.txt");
        cout<<"debut doestim  options : "<<options<<"\n";
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
                cout <<"scenario(s) choisi(s) : ";
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
            }
        }

        nstatOK = rt.cal_varstat();                       cout<<"apres cal_varstat\n";
        stat_obs = header.read_statobs(statobsfilename);  cout<<"apres read_statobs\n";
        nprog=100;iprog=1;
        flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        rt.alloue_enrsel(nsel);
        rt.cal_dist(nrec,nsel,stat_obs);                  cout<<"apres cal_dist\n";
        iprog+=8;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        det_numpar();                                     cout<<"apres det_numpar\n";
        nprog=(nparamcom+nparcompo)*10+14;
        recalparam(nsel);                                 cout<<"apres recalparam\n";
        rempli_mat(nsel,stat_obs);                        cout<<"apres rempli_mat\n";
        local_regression(nsel);               cout<<"apres local_regression\n";
        iprog+=1;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        phistar = calphistar(nsel);                                 cout<<"apres calphistar\n";
        det_nomparam();
        savephistar(nsel,path,ident);                     cout<<"apres savephistar\n";
        iprog+=1;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        lisimpar(nsel);                                   cout<<"apres lisimpar\n";
        iprog+=2;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        histodens(nsel,multithread,progressfilename,&iprog,&nprog);                      cout<<"apres histodens\n";
        parstat = calparstat(nsel);                                 cout<<"apres calparstat\n";
        saveparstat(nsel,path,ident);
        rt.desalloue_enrsel(nsel);
        iprog+=1;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
    }
