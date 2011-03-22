/*
 * comparscenm.cpp
 *
 *  Created on: 18 march 2011
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

#ifndef GLOBALH
#include "global.h"
#define GLOBALH
#endif

using namespace std;

    void comp_direct(int n, char *path, char *ident) {
        int ncs=100,nts,bidon,iscen,k,i;
        double **postscen, **postinf, **postsup,d,p;
        bool scenOK;
        postscen = new double*[ncs];postinf = new double*[ncs];postsup = new double*[ncs];
        for (int i=0;i<ncs;i++) {
            postscen[i] = new double[rt.nscenchoisi];
            postinf[i] = new double[rt.nscenchoisi];
            postsup[i] = new double[rt.nscenchoisi];
        }
        for (int i=0;i<ncs;i++) {
            for (int j=0;j<rt.nscenchoisi;j++) postscen[i][j]=0.0;
            nts = (n/ncs)*(i+1);
            for (int j=0;j<nts;j++) {
                k=0;
                while ((k<rt.nscenchoisi)and(rt.scenchoisi[k]!=rt.enrsel[j].numscen)) k++;
                postscen[i][k] +=(1.0/(double)nts);
            }
        }
        cout<<"apres calcul des postscen\n";
        for (int i=0;i<ncs;i++) {
             nts=n/ncs;
             for (k=0;k<rt.nscenchoisi;k++) {
                 p=postscen[i][k];
                 if ((abs(p)<0.00001)or(abs(1.0-p)<0.00001)) d=0.0;
                 else d=1.96*sqrt(p*(1.0-p)/(double)nts);
                 postinf[i][k] =p-d;if(postinf[i][k]<0.0)postinf[i][k]=0.0; 
                 postsup[i][k] =p+d;if(postsup[i][k]>1.0)postsup[i][k]=1.0;
             }
        }
        cout<<"apres calcul des postinf/postsup\n";
        char *nomfiparstat;
        nomfiparstat = new char[strlen(path)+strlen(ident)+20];
        strcpy(nomfiparstat,path);
        strcat(nomfiparstat,ident);
        strcat(nomfiparstat,"_compdirect.txt");
        cout <<nomfiparstat<<"\n";
        FILE *f1;
        f1=fopen(nomfiparstat,"w");
        printf("     %s   ","n");for (int i=0;i<rt.nscenchoisi;i++) printf("          scenario %d       ",rt.scenchoisi[i]);printf("\n");
        fprintf(f1,"   %s   ","n");for (int i=0;i<rt.nscenchoisi;i++) fprintf(f1,"          scenario %d       ",rt.scenchoisi[i]);fprintf(f1,"\n");
        for (int i=0;i<ncs;i++) {
            nts = (n/ncs)*(i+1);
            if (nts%50==0){printf(" %6d   ",nts);for (int j=0;j<rt.nscenchoisi;j++) printf("   %6.4f [%6.4f,%6.4f]  ",postscen[i][j],postinf[i][j],postsup[i][j]);printf("\n");}
            fprintf(f1," %6d   ",nts);for (int j=0;j<rt.nscenchoisi;j++) fprintf(f1,"   %6.4f [%6.4f,%6.4f]  ",postscen[i][j],postinf[i][j],postsup[i][j]);fprintf(f1,"\n");
        }
        fclose(f1);        
    }

    void expbeta(int bsize, double *b, double *eb)
    {
        double mi=b[0],ma=b[0],s=0;
        int i;
        for (i=1;i<bsize;i++)
            {if (mi>b[i]) mi=b[i];if (ma<b[i]) ma=b[i];}
        for (i=0;i<bsize;i++)
            {if (ma-b[i]>1000.0) eb[i]=0.0; else eb[i]=exp(b[i]-ma);s+=eb[i];}
        if (s>0.0) 
            {for (i=0;i<bsize;i++) eb[i]=eb[i]/s;}
    }

    void remplimatriceYP(int nli, int nco, int nmodel, double **matP, double *matYP, double *beta, double **matX, double *vecW, double **matY, double *smatP)
    {
        double betax[nmodel+1],ebetax[nmodel+1];
        int i,j,imod;
        
        for (i=0;i<nli;i++)
          {betax[nmodel]=0.0;smatP[i]=0.0;
            for (imod=0;imod<nmodel;imod++)
                {betax[imod]=0.0;
                for (j=0;j<nco+1;j++) betax[imod]+=beta[imod*(nco+1)+j]*matX[i][j];
                } 
            expbeta((nmodel+1),betax,ebetax);
            for (imod=0;imod<nmodel;imod++) {matP[i][imod]=ebetax[imod];smatP[i]+=matP[i][imod];}
            }
        for (i=0;i<nmodel*(nco+1);i++) matYP[i]=0.0;
        for (imod=0;imod<nmodel;imod++)
            {for (j=0;j<nco+1;j++)
                {for (i=0;i<nli;i++) matYP[imod*(nco+1)+j]+=(matY[i][imod]-matP[i][imod])*matX[i][j]*vecW[i];}
            }   
    }

    bool cal_loglik(int nli, int nmodel, int rep, double *loglik, double **matY, double **matP, double *vecW, double *smatY, double *smatP)
    {
        int i,imod;
        double a,llik;
        double debut,duree;
        llik=0.0;
        //omp_set_num_threads(1);
    #pragma omp parallel for private(a,imod)\
      reduction( + : llik )
        for (i=0;i<nli;i++)
            {a=0.0;
            for (imod=0;imod<nmodel;imod++)
              {//if ((matY[i][imod]>0.5)&&(matP[i][imod]<=0.0)) {std::cout << "caloglik=false\n";return false;}
                  if (matY[i][imod]>0.5) a+=log(matP[i][imod]);
                }
            //if ((smatP[i]>=1.0)&&(smatY[i]<0.9)) {std::cout << "caloglik=false\n";return false;}
            if (smatY[i]<0.9) a+=log(1.0-smatP[i]);
            llik=llik+vecW[i]*a;
                    //if (rep>1) {if(loglik[rep-1]<loglik[rep-2]-0.000001) {std::cout << "loglik decroit\n";return false;}}
            }
        loglik[rep-1]=llik;
        return true;
    }

    void calcul_psd(int nmodel, double *b0, double **matV, double *sd, double *px)
    {
        int imod,i,j,l;
        double esd=0, ebma, sum=0.0, mi=b0[0], ma=b0[0];
        double vecD[nmodel];
        if (nmodel>0) {for (i=1;i<nmodel;i++) {if (mi>b0[i]) mi=b0[i];if (ma<b0[i]) ma=b0[i];}}
        if (ma-mi<1000.0)
            {sum=0.0;for (i=0;i<nmodel+1;i++) sum+=exp(b0[i]-ma);
            for (imod=1;imod<nmodel+1;imod++)
                {ebma=exp(b0[imod]-ma);
                px[imod]=ebma/sum;
                esd=ebma/pow(sum,2);
                for (j=1;j<nmodel+1;j++) {if (imod==j) vecD[j-1]=esd*(sum-ebma); else vecD[j-1]=-esd*ebma;}
                sd[imod]=0.0;
                for (j=0;j<nmodel;j++) {for (l=0;l<nmodel;l++) sd[imod]+=matV[j][l]*vecD[j]*vecD[l];}
                if (sd[imod]>0.0) sd[imod]=sqrt(sd[imod]); else sd[imod]=0.0;   
                }
            px[0]=exp(b0[0]-ma)/sum;
            for (j=0;j<nmodel;j++) vecD[j]=-pow(exp(0.5*b0[j+1]-ma)/sum,2);
            sd[0]=0.0;
            for (j=0;j<nmodel;j++) {for (l=0;l<nmodel;l++) sd[0]+=matV[j][l]*vecD[j]*vecD[l];}
            if (sd[0]>0.0) sd[0]=sqrt(sd[0]); else sd[0]=0.0;
            }
            else
            {sum=0.0;for (i=1;i<nmodel+1;i++) {if (b0[i]>ma-1000) sum+=exp(b0[i]-ma);}
            for (imod=1;imod<nmodel+1;imod++)
                {if (b0[imod]>ma-1000)
                    {ebma=exp(b0[imod]-ma);
                    px[imod]=ebma/sum;
                    esd=ebma/pow(sum,2);
                    for (j=1;j<nmodel+1;j++) 
                        {if (imod==j) vecD[j-1]=esd*(sum-ebma); 
                                      else if (b0[j]>ma-1000) vecD[j-1]=-esd*ebma; else vecD[j-1]=0.0;
                        }
                    sd[imod]=0.0;
                    for (j=0;j<nmodel;j++) {for (l=0;l<nmodel;l++) sd[imod]+=matV[j][l]*vecD[j]*vecD[l];}
                    if (sd[imod]>0.0) sd[imod]=sqrt(sd[imod]); else sd[imod]=0.0;   
        }
                    else
                    {px[imod]=0.0;
                    sd[imod]=0.0;
                    }
                }
            if (b0[0]>ma-1000)
                    {px[0]=exp(b0[0]-ma)/sum;
                    for (j=0;j<nmodel;j++) 
                        {if (0.5*b0[j+1]>ma-1000) vecD[j]=-pow(exp(0.5*b0[j+1]-ma)/sum,2); else vecD[j]=0.0;}
                    sd[0]=0.0;
                    for (j=0;j<nmodel;j++) {for (l=0;l<nmodel;l++) sd[0]+=matV[j][l]*vecD[j]*vecD[l];}
                    if (sd[0]>0.0) sd[0]=sqrt(sd[0]); else sd[0]=0.0;
                    }
                    else
                    {px[0]=0.0;
                    sd[0]=0.0;
                    }
            }
    } 

    int polytom_logistic_regression(int nli, int nco, double **matX0, double *vecY, double *vecW, double *px, double *pxi, double *pxs)
    {
        int nmodel=0;
        double debut,duree,de1,du1;
        
        for (int i=1;i<nli;i++) {if (vecY[i]>1.0*nmodel) nmodel=vecY[i]+0.1;}
        if (nmodel<1) {px[0]=1.0;pxi[0]=1.0;pxs[0]=1.0;return 0;}
        cout << "   nmodel="<<nmodel<<endl;
        
        int i,j,l,m,n,nmodnco=nmodel*(nco+1),imod,rep;
        double **matA,**matB,**matX,**matXT,**matC,*deltabeta,*beta0,*beta,**matP,**matY,*matYP,*loglik,*sd,*bet,*px0;
        double imody,betmin,betmax,*smatY,*smatP;
        bool fin,caloglik;
        
        matA = new double*[nmodel];for (i=0;i<nmodel;i++) matA[i]= new double[nmodel];
        matB = new double*[nmodnco];for (i=0;i<nmodnco;i++) matB[i]= new double[nmodnco];
        matC = new double*[nmodnco];for (i=0;i<nmodnco;i++) matC[i]= new double[nmodnco];
        matX = new double*[nli];for (i=0;i<nli;i++) matX[i]= new double[nco+1];
        matXT = new double*[nco+1];for (i=0;i<nco+1;i++) matXT[i]= new double[nli];
        deltabeta = new double[nmodnco];
        beta0  = new double[nmodnco];
        beta   = new double[nmodnco];
        matYP  = new double[nmodnco];
        matP   = new double*[nli];;for (i=0;i<nli;i++) matP[i]= new double[nmodel];
        matY   = new double*[nli];;for (i=0;i<nli;i++) matY[i]= new double[nmodel];
        loglik = new double[50];
        sd     = new double[nmodel+1];
        bet    = new double[nmodel+1];
        px0    = new double[nmodel+1];
        smatY  = new double[nli];
        smatP  = new double[nli];
        
        for (i=0;i<nli;i++) {matX[i][0]=1.0; for (j=0;j<nco;j++) matX[i][j+1]=matX0[i][j];}
        //save_mat(nli,nco+1,matX0,"matX0.txt");
        for (imod=0;imod<nmodel;imod++) 
            {imody=imod+1;
            for (i=0;i<nli;i++) {if(fabs(vecY[i]-imody)<0.1) matY[i][imod]=1.0; else matY[i][imod]=0.0;}
            }
        for (i=0;i<nli;i++) {smatY[i]=0.0; for (j=0;j<nmodel;j++)smatY[i]+=matY[i][j];}
        for (i=0;i<nli;i++) {for (j=0;j<nco+1;j++) matXT[j][i]=matX[i][j];}  //transposition
        for (i=0;i<nmodnco;i++) {beta0[i]=0.0;beta[i]=0.0;}
        rep=0;fin=false;
        while (fin==false)
            {rep++;
              remplimatriceYP(nli,nco,nmodel,matP,matYP,beta,matX,vecW,matY,smatP);
            for (i=0;i<nmodnco;i++) {for (j=0;j<nmodnco;j++) matC[i][j]=0.0;}
            #pragma omp parallel shared(matC,vecW,matP,matX) private(i,m,n,j,l)
            {
            #pragma omp for schedule(dynamic)
            
              for (j=0;j<nco+1;j++)
                {for (l=j;l<nco+1;l++)
                    {for (m=0;m<nmodel;m++)
                        {for (n=m;n<nmodel;n++)
                            {for (i=0;i<nli;i++)
                                {if (m!=n) matC[m*(nco+1)+j][n*(nco+1)+l]-=vecW[i]*matP[i][m]*matP[i][n]*matX[i][j]*matX[i][l];
                                    else   matC[m*(nco+1)+j][n*(nco+1)+l]+=vecW[i]*matP[i][m]*(1.0-matP[i][m])*matX[i][j]*matX[i][l];   
                                }
                            matC[m*(nco+1)+l][n*(nco+1)+j]=matC[m*(nco+1)+j][n*(nco+1)+l];
                            if (m!=n) 
                                {matC[n*(nco+1)+j][m*(nco+1)+l]=matC[m*(nco+1)+j][n*(nco+1)+l];
                                matC[n*(nco+1)+l][m*(nco+1)+j]=matC[n*(nco+1)+j][m*(nco+1)+l];
                                }
                            }
                        }               
                    }           
                }
              }
            inverse(nmodnco,matC,matB);
            //InvdiagMat(nmodnco,matC,matB);
            for (i=0;i<nmodnco;i++) {deltabeta[i]=0.0;for (j=0;j<nmodnco;j++) deltabeta[i]+=matB[i][j]*matYP[j];}
            for (i=0;i<nmodnco;i++) beta[i]=beta0[i]+deltabeta[i];
            remplimatriceYP(nli,nco,nmodel,matP,matYP,beta,matX,vecW,matY,smatP);
            caloglik=false;
            if (rep==1)
                {while (caloglik==false) 
                    {caloglik=cal_loglik(nli,nmodel,rep,loglik,matY,matP,vecW,smatY,smatP);
                    if (caloglik==false)
                        {for (i=0;i<nmodnco;i++) deltabeta[i]*=0.5;
                        for (i=0;i<nmodnco;i++) beta[i]=beta0[i]+deltabeta[i];
                        remplimatriceYP(nli,nco,nmodel,matP,matYP,beta,matX,vecW,matY,smatP);
                        }
                    }
                }else
                {while ((caloglik==false)||(loglik[rep-1]<loglik[rep-2]-0.000001))
                    {caloglik=cal_loglik(nli,nmodel,rep,loglik,matY,matP,vecW,smatY,smatP);
                    if ((caloglik==false)||(loglik[rep-1]<loglik[rep-2]-0.000001))
                        {for (i=0;i<nmodnco;i++) deltabeta[i]*=0.5;
                        for (i=0;i<nmodnco;i++) beta[i]=beta0[i]+deltabeta[i];
                        remplimatriceYP(nli,nco,nmodel,matP,matYP,beta,matX,vecW,matY,smatP);                   
                        }
                    }
                }       
            for (i=0;i<nmodnco;i++)beta0[i]=beta[i];
            std::cout << "iteration " << rep <<"   log-lik= " << loglik[rep-1] << "\n";
            for (i=0;i<nmodel;i++) {for (j=0;j<nmodel;j++) matA[i][j]=matB[i*(nco+1)][j*(nco+1)];}
            bet[0]=0.0;for(i=0;i<nmodel;i++) bet[i+1]=beta0[i*(nco+1)];
            if (rep==1) {for (i=0;i<nmodel+1;i++) px0[i]=2.0;} else {for (i=0;i<nmodel+1;i++) px0[i]=px[i];}
            calcul_psd(nmodel,bet,matA,sd,px);
            betmin=bet[0];betmax=bet[0];
            for (i=0;i<nmodel+1;i++) {if (betmin>bet[i]) betmin=bet[i];}   
            for (i=0;i<nmodel+1;i++) {if (betmax<bet[i]) betmax=bet[i];}
            fin=true;i=0;while ((fin==true)&&(i<nmodel+1)) {fin= (fabs(px[i]-px0[i])<0.0001);i++;}
            fin=(fin||(betmax-betmin>50));
            for (i=0;i<nmodel+1;i++) {std::cout << bet[i] <<"   ";}
            std::cout << "\n";
            for (i=0;i<nmodel+1;i++) {std::cout << px[i] <<"   ";}
            std::cout << "\n";std::cout << "\n";
            }
        for (imod=0;imod<nmodel+1;imod++)
                {pxi[imod]=px[imod]-1.96*sd[imod];
                if (pxi[imod]<0.0) pxi[imod]=0.0;
                pxs[imod]=px[imod]+1.96*sd[imod];
                if (pxs[imod]>1.0) pxs[imod]=1.0;
                }
            
        for (i=0;i<nmodel;i++) delete[] matA[i];delete[] matA;
        for (i=0;i<nmodnco;i++) delete[] matB[i];delete[] matB;
        for (i=0;i<nmodnco;i++) delete[] matC[i];delete[] matC;
        for (i=0;i<nli;i++) delete[] matX[i];delete[] matX;
        for (i=0;i<nco+1;i++) delete[] matXT[i];delete[] matXT;
        for (i=0;i<nli;i++) delete[] matP[i];delete[] matP;
        for (i=0;i<nli;i++) delete[] matY[i];delete[] matY;
        delete[] deltabeta;
        delete[] beta;
        delete[] beta0;
        delete[] matYP;
        delete[] loglik;
        delete[] sd;
        delete[] bet;
        delete[] px0;
    }
        
    void docompscen(char *compar,bool multithread){
        char *datafilename;
        int rtOK,nstatOK;
        int nrec,nseld,nselr,nsel,ns,ns1,nlogreg,k,nts,nscenutil;
        string opt,*ss,s,*ss1,s0,s1;
        double **matX0, *vecW, **alpsimrat,**parsim, *stat_obs;
        
        opt=char2string(compar);
        ss = splitwords(opt,";",&ns);
        for (int i=0;i<ns;i++) { //cout<<ss[i]<<"\n";
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);
            if (s0=="s:") {
                ss1 = splitwords(s1,",",&rt.nscenchoisi);
                rt.scenchoisi = new int[rt.nscenchoisi];
                for (int j=0;j<rt.nscenchoisi;j++) rt.scenchoisi[j] = atoi(ss1[j].c_str());
                cout <<"scenario(s) choisi(s) : ";
                for (int j=0;j<rt.nscenchoisi;j++) {cout<<rt.scenchoisi[j]; if (j<rt.nscenchoisi-1) cout <<",";}cout<<"\n";
            } else {
                if (s0=="n:") {
                    nrec=atoi(s1.c_str());    
                    cout<<"nombre total de jeux de données considérés (tous scénarios confondus)= "<<nrec<<"\n";
                } else {
                    if (s0=="d:") {
                        nseld=atoi(s1.c_str());    
                        cout<<"nombre de jeux de données considérés pour la méthode directe = "<<nseld<<"\n";
                    } else {
                        if (s0=="l:") {
                        nselr=atoi(s1.c_str());    
                        cout<<"nombre de jeux de données considérés pour la régression logistique = "<<nselr<<"\n";
                        } else {
                            if (s0=="m:") {
                            	nlogreg=atoi(s1.c_str());
                                if (nlogreg==0) nselr=0;
                                cout<< "nombre de régressions logistiques à effectuer\n";
                            }            
                        }
                    }
                }
            }
        }
        nsel=nseld;if(nsel<nselr)nsel=nselr;
        nstatOK = rt.cal_varstat();
        stat_obs = header.read_statobs(statobsfilename);
        rt.cal_dist(nrec,nsel,stat_obs);
        comp_direct(nseld,path,ident);
        if (nlogreg>0) {
            k=0;
            while(k<nlogreg) {
                nts=(nselr/nlogreg)*(k+1);
                nscenutil=0;
                
            }
        }
    
    }