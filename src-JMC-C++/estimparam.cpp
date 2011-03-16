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

enregC *enrsel;
ReftableC rt;
HeaderC header;
double *var_stat,*stat_obs, *parmin, *parmax, *diff;
double **alpsimrat,**parsim,**matX0,*vecW,**beta, **phistar;
int nparamax,nparamcom,nparcompo,**numpar,nstatOKsel,numtransf,npar,npar0;
double borne=10.0,xborne;
bool composite,original;
int nscenchoisi,*scenchoisi;

using namespace std;

#define c 1.5707963267948966192313216916398

struct compenreg
{
   bool operator() (const enregC & lhs, const enregC & rhs) const
   {
      return lhs.dist < rhs.dist;
   }
};
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
            cout<<"var_stat["<<j<<"]="<<var_stat[j]<<"\n";
        }
        return nsOK;
    }

    void read_statobs(char *statobsfilename) {
       string entete,ligne,*ss;
       int ns;
       ifstream file(statobsfilename, ios::in);
       getline(file,entete);
       getline(file,ligne);
       file.close();
       ss=splitwords(ligne," ",&ns);
       cout<<"statobs ns="<<ns<<"\n";
       if (ns!=rt.nstat) exit(1);
       stat_obs = new double[ns];
       for (int i=0;i<ns;i++) stat_obs[i]=atof(ss[i].c_str());
       delete []ss;
       //for (int i=0;i<ns;i++) cout<<stat_obs[i]<<"   ";
       //cout<<"\n";
    }

    void cal_dist(int nrec, int nsel) {
        int nn=10000,nreclus=0,nparamax,nrecOK=0,iscen,bidon;
        bool firstloop=true,scenOK;
        double diff;
        if (nn<nsel) nn=nsel;
        nparamax = 0;for (int i=0;i<rt.nscen;i++) if (rt.nparam[i]>nparamax) nparamax=rt.nparam[i];
        cout<<"cal_dist nsel="<<nsel<<"   nparamax="<<nparamax<<"   nrec="<<nrec<<"   nreclus="<<nreclus<<"   nstat="<<rt.nstat<<"   2*nn="<<2*nn<<"\n";
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

    void rempli_mat(int n) {
        int icc;
        double delta,som,x,*var_statsel,nn;
        double *sx,*sx2,*mo;
        nn=(double)n;
        delta = enrsel[n-1].dist;
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
        for (int i;i<n;i++) {
            icc=-1;
            for (int j=0;j<rt.nstat;j++) {
                if (var_statsel[j]>0.0) {
                    icc++;
                    matX0[icc][j]=(enrsel[i].stat[j]-stat_obs[j])/sqrt(var_statsel[j]);
                }
            }
            x=enrsel[i].dist/delta;
            vecW[i]=(1.5/delta)*(1.0-x*x);
            som +=vecW[i];
        }
        for (int i;i<n;i++) vecW[i]/=som;
        parsim = new double*[n];
        for (int i;i<n;i++) parsim[i] = new double[nparamcom];
        for (int i;i<n;i++) {
            for (int j=0;j<nparamcom;j++)parsim[i][j]=alpsimrat[i][j];
        }
    }

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
        matXT = transpose(n,nstatOKsel+1,matX);
        for (int i=0;i<n;i++) {
            for (int j=0;j<nstatOKsel+1;j++) matA[j][i] = matXT[j][i]*vecW[i];
        }
        matAA = prodM(nstatOKsel+1,n,nstatOKsel+1,matA,matX);
        matB = inverse(nstatOKsel+1,matAA,multithread);
        matC = prodM(nstatOKsel+1,nstatOKsel+1,n,matB,matA);
        beta = prodM(nstatOKsel+1,n,nparamcom,matC,parsim);
        libereM(nstatOKsel+1,matA);
        libereM(n,matX);
        libereM(nstatOKsel+1,matB);
        libereM(nstatOKsel+1,matAA);
        libereM(nstatOKsel+1,matC);
    
    }

    void calphistar(int n){
        int k,kk,qq;
        double pmut;
        phistar = new double*[n];
        for (int i=0;i<n;i++) phistar[i] = new double[nparamcom+nparcompo];
        for (int i=0;i<n;i++) {
            for (int j=0;j<nparamcom;j++) {
                phistar[i][j] = alpsimrat[i][j];
                for (int k=0;k<nstatOKsel;k++) phistar[i][j] -= matX0[i][k]*beta[k+1][j];
                switch(numtransf) {
                  case 1 : break;
                  case 2 : if (phistar[i][j]<100.0) phistar[i][j] = exp(phistar[i][j]); else phistar[i][j]=exp(100.0);
                           break; 
                  case 3 : if (phistar[i][j]<=-xborne) phistar[i][j] = parmin[j];
                           else if (phistar[i][j]>=xborne) phistar[i][j] = parmax[j];
                           else phistar[i][j] = (exp(phistar[i][j])*parmax[j]+parmin[j])/(1.0+exp(phistar[i][j]));
                  case 4 : if (phistar[i][j]<=-xborne) phistar[i][j] = parmin[j];
                           else if (phistar[i][j]>=xborne) phistar[i][j] = parmax[j];
                           else phistar[i][j] =parmin[j] +(diff[j]/c*atan(exp(phistar[i][j])));                
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
                                        pmut = header.groupe[gr].mutmoy+phistar[i][kk];
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
                                        pmut =phistar[i][kk] +header.groupe[gr].snimoy;
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
                                        pmut =phistar[i][kk]+phistar[i][qq];
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
                                    pmut = phistar[i][kk];
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
        





    void doestim(char *headerfilename,char *reftablefilename,char *reftablelogfilename,char *statobsfilename,char *options,bool multithread) {
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
        det_numpar();
        recalparam(nsel);
        cout<<"apres recalparam\n";
        rempli_mat(nsel);
        cout<<"apres rempli_mat\n";
        local_regression(nsel,multithread);
        cout<<"apres local_regression\n";
        calphistar(nsel);
        cout<<"apres calphistar\n";
    }