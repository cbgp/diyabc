/*
 * comparscenm.cpp
 *
 *  Created on: 18 march 2011
 *      Author: cornuet
 */

#include <algorithm>
#include <vector>
#include <cmath>
#include <iomanip>

#include "comparscen.h"
#include "reftable.h"
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

extern string progressfilename, path;
extern ReftableC rt;
extern double time_readfile;
extern string ident;
extern HeaderC header;
extern int nstatOKsel;
extern ofstream fprog;

int ncs=100;
//double time_loglik=0.0, time_matC=0.0,time_call=0.0;
long double **cmatA,**cmatB,**cmatB0,**cmatX,**cmatXT,**cmatC,*cdeltabeta,*cbeta0,*cbeta,**cmatP,**cmatY,*cmatYP,*cloglik,*csd,*cbet,*cpx0,*csmatY,*csmatP,**cgdb ;
long double *vecY,*vecYY,*cvecW,**cmatX0;
matligneC *matA;




/**
 * Alloue la mémoire pour les nombreuses matrices (et vecteurs) utiles à la régression logistique
 */ 
    void allouecmat(int nmodel, int nli, int nco) {
        int nmodnco = nmodel*(nco+1);

        cmatA = new long double*[nmodel];for (int i=0;i<nmodel;i++) cmatA[i]= new long double[nmodel];
        cmatB = new long double*[nmodnco];for (int i=0;i<nmodnco;i++) cmatB[i]= new long double[nmodnco];
        cmatB0 = new long double*[nmodnco];for (int i=0;i<nmodnco;i++) cmatB0[i]= new long double[nmodnco];
        cmatC = new long double*[nmodnco];for (int i=0;i<nmodnco;i++) cmatC[i]= new long double[nmodnco];
        cmatX = new long double*[nli];for (int i=0;i<nli;i++) cmatX[i]= new long double[nco+1];
        cmatXT = new long double*[nco+1];for (int i=0;i<nco+1;i++) cmatXT[i]= new long double[nli];
        cdeltabeta = new long double[nmodnco];
        cbeta0  = new long double[nmodnco];
        cbeta   = new long double[nmodnco];
        cmatYP  = new long double[nmodnco];
        cmatP   = new long double*[nli];;for (int i=0;i<nli;i++) cmatP[i]= new long double[nmodel];
        cmatY   = new long double*[nli];;for (int i=0;i<nli;i++) cmatY[i]= new long double[nmodel];
        cloglik = new long double[50];
        csd     = new long double[nmodel+1];
        cbet    = new long double[nmodel+1];
        cpx0    = new long double[nmodel+1];
        csmatY  = new long double[nli];
        csmatP  = new long double[nli];
        cmatX0 = new long double*[nli];for (int i=0;i<nli;i++)cmatX0[i]=new long double[nco];
        matA = new matligneC[nli];
        cvecW = new long double[nli];
        vecY  = new long double[nli];
        vecYY = new long double[nli];
    }

/**
 * Libère la mémoire pour les nombreuses matrices (et vecteurs) utiles à la régression logistique
 */ 
    void liberecmat(int nmodel, int nli, int nco) {
        int nmodnco = nmodel*(nco+1);

        for (int i=0;i<nmodel;i++) delete []cmatA[i];delete cmatA;
        for (int i=0;i<nmodnco;i++) delete []cmatB[i];delete cmatB;
         for (int i=0;i<nmodnco;i++) delete []cmatB0[i];delete cmatB0;
       for (int i=0;i<nmodnco;i++) delete []cmatC[i];delete cmatC;
        for (int i=0;i<nli;i++) delete []cmatX[i];delete cmatX;
        for (int i=0;i<nco+1;i++) delete []cmatXT[i];delete cmatXT;
        delete []cdeltabeta;
        delete []cbeta0;
        delete []cbeta;
        delete []cmatYP;
        for (int i=0;i<nli;i++) delete []cmatP[i];delete cmatP;
        for (int i=0;i<nli;i++) delete []cmatY[i];delete cmatY;
        delete []cloglik;
        delete []csd;
        delete []cbet;
        delete []cpx0;
        delete []csmatY;
        delete []csmatP;
        for (int i=0;i<nli;i++) delete []cmatX0[i];delete cmatX0;
        delete [] matA;
        delete []cvecW;
        delete [] vecY;
        delete [] vecYY;
    }

/**
 * Effectue les calculs de l'approche directe et renvoie les résultats dans une structure posteriorscenC** [scenario][nombre de jeux]
 */ 
    posteriorscenC** comp_direct(int n) {
        int nts,k;
        posteriorscenC **posts;
        double d,p,delta;
        delta = rt.enrsel[n-1].dist;
        posts = new posteriorscenC*[ncs];
        for (int i=0;i<ncs;i++) {
            posts[i] = new posteriorscenC[rt.nscenchoisi];
        }
        for (int i=0;i<ncs;i++) {
            for (int j=0;j<rt.nscenchoisi;j++) posts[i][j].x=0.0;
            nts = (n/ncs)*(i+1);
            for (int j=0;j<nts;j++) {
                k=0;
                while ((k<rt.nscenchoisi)and(rt.scenchoisi[k]!=rt.enrsel[j].numscen)) k++;
                posts[i][k].x +=(1.0/(double)nts);
            }
        }
        //cout<<"apres calcul des posts\n";
        for (int i=0;i<ncs;i++) {
             nts=n/ncs;
             for (k=0;k<rt.nscenchoisi;k++) {
                 p=posts[i][k].x;
                 if ((fabs(p)<0.00001)or(fabs(1.0-p)<0.00001)) d=0.0;
                 else d=1.96*sqrt(p*(1.0-p)/(double)nts);
                 posts[i][k].inf =p-d;if(posts[i][k].inf<0.0)posts[i][k].inf=0.0;
                 posts[i][k].sup =p+d;if(posts[i][k].sup>1.0)posts[i][k].sup=1.0;
             }
        }
        return posts;
    }

/**
 * Sauvegarde des résultats de l'approche directe
 */ 
    void  save_comp_direct(int n, posteriorscenC** posts, string path, string ident){
        //cout<<"apres calcul des postinf/postsup\n";
        int nts;
        string nomfiparstat;
        nomfiparstat = path + ident + "_compdirect.txt";
        //strcpy(nomfiparstat,path);
        //strcat(nomfiparstat,ident);
        //strcat(nomfiparstat,"_compdirect.txt");
        cout <<nomfiparstat<<"\n";
        FILE *f1;
        f1=fopen(nomfiparstat.c_str(),"w");
        printf("     %s   ","n");for (int i=0;i<rt.nscenchoisi;i++) printf("          scenario %d       ",rt.scenchoisi[i]);printf("\n");
        fprintf(f1,"   %s   ","n");for (int i=0;i<rt.nscenchoisi;i++) fprintf(f1,"          scenario %d       ",rt.scenchoisi[i]);fprintf(f1,"\n");
        for (int i=0;i<ncs;i++) {
            nts = (n/ncs)*(i+1);
            if (nts%50==0){printf(" %6d   ",nts);for (int j=0;j<rt.nscenchoisi;j++) printf("   %6.4Lf [%6.4Lf,%6.4Lf]  ",posts[i][j].x,posts[i][j].inf,posts[i][j].sup);printf("\n");}
            fprintf(f1," %6d   ",nts);for (int j=0;j<rt.nscenchoisi;j++) fprintf(f1,"   %6.4Lf [%6.4Lf,%6.4Lf]  ",posts[i][j].x,posts[i][j].inf,posts[i][j].sup);fprintf(f1,"\n");
        }
        fclose(f1);
        for (int i=0;i<rt.nscenchoisi;i++) delete []posts[i];
        delete []posts;

    }

/**
 * Sauvegarde des résultats de l'approche régression logistique
 */ 
    void save_comp_logistic(int nlogreg,int nselr,posteriorscenC** postscenlog,string path, string ident) {
        int nts,sum;
		double som;
        string nomfiparstat;
        nomfiparstat = path + ident + "_complogreg.txt";
        //strcpy(nomfiparstat,path);
        //strcat(nomfiparstat,ident);
        //strcat(nomfiparstat,"_complogreg.txt");
        cout <<nomfiparstat<<"\n";
        FILE *f1;
        f1=fopen(nomfiparstat.c_str(),"w");
        printf("     %s   ","n");for (int i=0;i<rt.nscenchoisi;i++) printf("          scenario %d       ",rt.scenchoisi[i]);printf("\n");
        fprintf(f1,"   %s   ","n");for (int i=0;i<rt.nscenchoisi;i++) fprintf(f1,"          scenario %d       ",rt.scenchoisi[i]);fprintf(f1,"\n");
        for (int i=0;i<nlogreg;i++) {
			som=0.0;for (int j=0;j<rt.nscenchoisi;j++) som+=postscenlog[i][j].x;
			if (fabs(som-1.0)<0.001*rt.nscenchoisi) {
				nts = (nselr/nlogreg)*(i+1);
				sum=0;for (int j=0;j<rt.nscenchoisi;j++) sum+=postscenlog[i][j].err;
				if (sum==0){
					printf(" %6d   ",nts);for (int j=0;j<rt.nscenchoisi;j++) printf("   %6.4Lf [%6.4Lf,%6.4Lf]  ",postscenlog[i][j].x,postscenlog[i][j].inf,postscenlog[i][j].sup);printf("\n");
					fprintf(f1," %6d   ",nts);for (int j=0;j<rt.nscenchoisi;j++) fprintf(f1,"   %6.4Lf [%6.4Lf,%6.4Lf]  ",postscenlog[i][j].x,postscenlog[i][j].inf,postscenlog[i][j].sup);fprintf(f1,"\n");
				} else {
					printf(" %6d   ",nts);for (int j=0;j<rt.nscenchoisi;j++) printf("   %6.4Lf [%6.4Lf,%6.4Lf]* ",postscenlog[i][j].x,postscenlog[i][j].inf,postscenlog[i][j].sup);
					fprintf(f1," %6d   ",nts);for (int j=0;j<rt.nscenchoisi;j++) fprintf(f1,"   %6.4Lf [%6.4Lf,%6.4Lf]* ",postscenlog[i][j].x,postscenlog[i][j].inf,postscenlog[i][j].sup);					
					if (postscenlog[i][0].err==8) {
						printf(" LOGISTIC REGRESSION DID NOT CONVERGE AFTER 50 ITERATIONS\n");
						fprintf(f1," LOGISTIC REGRESSION DID NOT CONVERGE AFTER 50 ITERATIONS\n");
						
					} else {printf("\n");fprintf(f1,"\n");}
				}
			}
		}
        fclose(f1);
        for (int i=0;i<nlogreg;i++) delete []postscenlog[i];
        delete []postscenlog;
    }


/**
 * Appelle l'analyse factorielle discriminante et remplace les summary stats par les composantes des jeux simulés
 * sur les axes discriminants
 */
    void transAFD(int nrec,int nsel, float* stat_obs) {
        long double delta,rdelta,*w,a,**X,*statpiv;
		float *sp;
        int *scenar;
        resAFD afd;
        //cout<<"debut transfAFD\n";
        delta=rt.enrsel[nsel-1].dist;
        rdelta=1.5/delta;
        w = new long double[nsel];
        for (int i=0;i<nsel;i++) {a=rt.enrsel[i].dist/delta;w[i]=rdelta*(1.0-a*a);}
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
// remplacement des stat observées par ses composantes sur les axes discriminants
        for (int j=0;j<afd.nlambda;j++) {
            statpiv[j]=0.0;
            for (int k=0;k<rt.nstat;k++) statpiv[j] += ((long double)stat_obs[k]-afd.moy[k])*afd.vectprop[k][j];
            }
//calcul des composantes pour le jeu observé
        for (int j=0;j<afd.nlambda;j++) stat_obs[j] = statpiv[j];
//mise à zéro des stats au delà des composantes
        for (int j=afd.nlambda;j<rt.nstat;j++) stat_obs[j] = 0.0;
       delete []w;
        delete []statpiv;
        for (int i=0;i<nsel;i++) delete []X[i];delete []X;
    }
/**
* effectue le remplissage de la matrice cmatX0 et du vecteur des poids cvecW
*/
    void rempli_mat0(int n, float* stat_obs) {
        int icc;
        long double delta,som,x,*var_statsel,nn;
        long double *sx,*sx2,*mo;
        nn=(long double)n;
        delta = rt.enrsel[n-1].dist;
        //cout<<"delta="<<delta<<"\n";
        sx  = new long double[rt.nstat];
        sx2 = new long double[rt.nstat];
        mo  = new long double[rt.nstat];
        var_statsel = new long double[rt.nstat];
        for (int i=0;i<rt.nstat;i++){sx[i]=0.0;sx2[i]=0.0;mo[i]=0.0;}
        for (int i=0;i<n;i++){
            for (int j=0;j<rt.nstat;j++) {
                x =(long double)rt.enrsel[i].stat[j];
                sx[j] +=x;
                sx2[j] +=x*x;
				//cout<<"  "<<x;
            }
            //cout<<"\n";
        }
        nstatOKsel=0;
        for (int j=0;j<rt.nstat;j++) {
            var_statsel[j]=(sx2[j]-sx[j]/nn*sx[j])/(nn-1.0);
            //cout <<"var_statsel["<<j<<"]="<<var_statsel[j]<<"   nn="<<nn<<"    sx2="<<sx2[j]<<"   sx="<<sx[j]<<"\n";
            if (var_statsel[j]>1E-20) nstatOKsel++;
            mo[j] = sx[j]/nn;
        }
        //cout <<"hello\n";
        som=0.0;
        for (int i=0;i<n;i++) {
            icc=-1;
            for (int j=0;j<rt.nstat;j++) {
                if (var_statsel[j]>1E-20) {
                    icc++;
                    cmatX0[i][icc]=(long double)(rt.enrsel[i].stat[j]-stat_obs[j])/sqrt(var_statsel[j]);
                }
            }
            x=rt.enrsel[i].dist/delta;
            cvecW[i]=(1.5/delta)*(1.0-x*x);
            som = som + cvecW[i];
        }
        /*cout <<"\n cmatX0:\n";
        for (int i=0;i<5;i++) { cout<<rt.enrsel[i].numscen<<"   ";
            for (int j=0;j<nstatOKsel;j++) cout<<cmatX0[i][j]<<"  ";cout<<"\n";
        }*/
        for (int i=0;i<n;i++) cvecW[i]/=som;
        /*cout <<"\ncvecW:\n";for (int i=0;i<10;i++) cout<<cvecW[i]<<"  ";
        cout<<"\n";
		exit(1);*/
        delete []sx;delete []sx2;delete []mo;delete []var_statsel;
    }

/**
* Calcule l'exponentielle des coefficients béta tels que leur somme soit égale à 1 et qu'il n'y ait pas de dépassement 
* de la taille maximale du type long double dans l'exponentielle
*/
    void expbeta(int bsize, long double *b, long double *eb)
    {
        long double mi=b[0],ma=b[0],s=0;
        int i;
        for (i=1;i<bsize;i++)
            {if (mi>b[i]) mi=b[i];if (ma<b[i]) ma=b[i];}
        for (i=0;i<bsize;i++)
            {if (ma-b[i]>1000.0) eb[i]=0.0; else eb[i]=exp(b[i]-ma);s+=eb[i];}
        if (s>0.0)
            {for (i=0;i<bsize;i++) eb[i]=eb[i]/s;}
        for (i=0;i<bsize;i++) if (eb[i]==0.0) eb[i]=0.00000001;
    }

/**
* Remplit la matrice YP = Y - PXW
*/
    void remplimatriceYP(int nli, int nco, int nmodel, long double **cmatP, long double *cmatYP, long double *cbeta, long double **cmatX, long double *cvecW, long double **cmatY, long double *csmatP)
    {
        long double betax[nmodel+1],ebetax[nmodel+1];
        int i,j,imod;

        for (i=0;i<nli;i++)
          {betax[nmodel]=0.0;csmatP[i]=0.0;
            for (imod=0;imod<nmodel;imod++)
                {betax[imod]=0.0;
                for (j=0;j<nco+1;j++) betax[imod]+=cbeta[imod*(nco+1)+j]*cmatX[i][j];
                }
            expbeta((nmodel+1),betax,ebetax);
            for (imod=0;imod<nmodel;imod++) {cmatP[i][imod]=ebetax[imod];csmatP[i]+=cmatP[i][imod];}
            if (csmatP[i]==1.0) {
                  //for (imod=0;imod<nmodel;imod++) cout<<cmatP[i][imod]<<"  ";cout<<"  csmatP["<<i<<"]="<<csmatP[i]<<"\n";
                  //for (imod=0;imod<nmodel;imod++)  cout<<betax[imod]<<"  ";cout<<"\n";
                  csmatP[i]=0.99999999;
            }
            }
        for (i=0;i<nmodel*(nco+1);i++) cmatYP[i]=0.0;
        for (imod=0;imod<nmodel;imod++){
            for (j=0;j<nco+1;j++){
                for (i=0;i<nli;i++) {
					cmatYP[imod*(nco+1)+j]+=(cmatY[i][imod]-cmatP[i][imod])*cmatX[i][j]*cvecW[i];
					if (cmatYP[imod*(nco+1)+j] != cmatYP[imod*(nco+1)+j]) {
						cout <<"probleme NAN dans cmatYP pour i="<<i<<" et j="<<j<<"\n";
						cout<<" cmatY="<<cmatY[i][imod]<<"   cmatP="<<cmatP[i][imod]<<"   cmatX="<<cmatX[i][j]<<"   cvecW="<<cvecW[i]<<"\n";
					}
				  
				}
            }
        }
    }

/**
* Calcule la vraisemblance du modèle logistique et renvoit false si la vraisemblance a diminué
*/
    bool cal_loglik(int nli, int nmodel, int rep, long double *cloglik, long double **cmatY, long double **cmatP, long double *vecW, long double *csmatY, long double *csmatP)
    {
        int i,imod;
        long double a,llik=0.0;
//        double debut,duree,clock_zero=0.0;
//        debut=walltime(&clock_zero);
        bool OK=true;
//    #pragma omp parallel for private(a,imod) if(multithread) reduction( + : llik )
        for (i=0;i<nli;i++)
            {a=0.0;
            for (imod=0;imod<nmodel;imod++) {
              if ((cmatY[i][imod]>0.5)and(cmatP[i][imod]<=0.0)) {std::cout << "caloglik=false\n";return false;}
                  //if (cmatP[i][imod]<=0.0) {cout<<"cmatP["<<i<<"]["<<imod<<"]="<<cmatP[i][imod]<<"\n";OK=false;}
              if (cmatY[i][imod]>0.5) a+=log(cmatP[i][imod]);
                }
            if ((csmatP[i]>=1.0)and(csmatY[i]<0.9)) {
                //cout<<"csmatP["<<i<<"]="<< setiosflags(ios::fixed)<<setw(12)<<setprecision(9)<<csmatP[i]<<"   csmatY["<<i<<"]="<<csmatY[i]<<"\n";
                csmatP[i]=0.99999999;
            }
            if (csmatY[i]<0.9) a+=log(1.0-csmatP[i]);
            llik=llik+cvecW[i]*a;
            }
        cloglik[rep-1]=llik;
        if (rep>1) {
			if(cloglik[rep-1]<cloglik[rep-2]) {
				printf("rep = %d  loglik decroit : %12.6Lf  %12.6Lf\n",rep,cloglik[rep-1],0.999999*cloglik[rep-2]);
				//std::cout << "loglik decroit : "<<cloglik[rep-1]<<"   "<<cloglik[rep-2]<<"\n";
				return false;
			}
		}
//        duree=walltime(&debut);//time_loglik += duree;
        return OK;
    }

/**
* Calcule la valeur de la probabilité a posteriori de chaque modèle ainsi que les bornes inf et sup de leur intervalle 
* de crédibilité, avec le souci de ne pas dépasser les limites du type long double dans les exponentiations
*/
    void calcul_psd(int nmodel, long double *b0, long double **matV, long double *sd, long double *px)
    {
        int imod,i,j,l;
        long double esd=0, ebma, sum=0.0, mi=b0[0], ma=b0[0];
        long double vecD[nmodel];
        //cout<<"debut calcul_psd nmodel="<<nmodel<<"\n";
        if (nmodel>0) {for (i=1;i<nmodel;i++) {if (mi>b0[i]) mi=b0[i];if (ma<b0[i]) ma=b0[i];}}
        if (ma-mi<1000.0){
            //cout<<"ma-mi="<<ma-mi<<"\n";
            sum=0.0;for (i=0;i<nmodel+1;i++) sum+=exp(b0[i]-ma);
            //cout<<"sum="<<sum<<"\n";
            for (imod=1;imod<nmodel+1;imod++) {
                //cout<<"imod="<<imod;fflush(stdout);
                //cout<<"   b0[imod]="<<b0[imod]<<"\n";
                ebma=exp(b0[imod]-ma);
                //cout<<"ebma="<<ebma<<"   sum="<<sum<<"   ebma/sum="<<ebma/sum<<"\n";
                px[imod]=ebma/sum;
                //cout<<"px["<<imod<<"]="<< px[imod]<<"\n";
                esd=ebma/pow(sum,2);
                for (j=1;j<nmodel+1;j++) {if (imod==j) vecD[j-1]=esd*(sum-ebma); else vecD[j-1]=-esd*ebma;}
                //cout<<"esd="<<esd<<"\n";
                sd[imod]=0.0;
                for (j=0;j<nmodel;j++) {for (l=0;l<nmodel;l++) sd[imod]+=matV[j][l]*vecD[j]*vecD[l];}
                if (sd[imod]>0.0) sd[imod]=sqrt(sd[imod]); else sd[imod]=0.0;
                //cout<<"sd["<<imod<<"]="<<sd[imod]<<"\n";
            }
            px[0]=exp(b0[0]-ma)/sum;
            for (j=0;j<nmodel;j++) vecD[j]=-pow(exp(0.5*b0[j+1]-ma)/sum,2);
            sd[0]=0.0;
            for (j=0;j<nmodel;j++) {for (l=0;l<nmodel;l++) sd[0]+=matV[j][l]*vecD[j]*vecD[l];}
            if (sd[0]>0.0) sd[0]=sqrt(sd[0]); else sd[0]=0.0;
            //cout<<"px[0]="<<px[0]<<"   sd[0]"<<sd[0]<<"\n";
        }
        else{
            //cout<<"ma-mi="<<ma-mi<<"\n";
            sum=0.0;for (i=1;i<nmodel+1;i++) {if (b0[i]>ma-1000) sum+=exp(b0[i]-ma);}
            for (imod=1;imod<nmodel+1;imod++){
                if (b0[imod]>ma-1000){
                    ebma=exp(b0[imod]-ma);
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
                else {
                    px[imod]=0.0;
                    sd[imod]=0.0;
                }
            }
            if (b0[0]>ma-1000) {
                px[0]=exp(b0[0]-ma)/sum;
                for (j=0;j<nmodel;j++)
                    {if (0.5*b0[j+1]>ma-1000) vecD[j]=-pow(exp(0.5*b0[j+1]-ma)/sum,2); else vecD[j]=0.0;}
                sd[0]=0.0;
                for (j=0;j<nmodel;j++) {for (l=0;l<nmodel;l++) sd[0]+=matV[j][l]*vecD[j]*vecD[l];}
                if (sd[0]>0.0) sd[0]=sqrt(sd[0]); else sd[0]=0.0;
            }
            else {
                px[0]=0.0;
                sd[0]=0.0;
            }
        }
        //cout<<"fin calcul_psd\n";
    }

/**
* Numérote les scénarios (vecteur numod) dans l'attribution des probabilités a posteriori
*/
    void ordonne(int nmodel,int nli,int nco,long double *vecY,int *numod) {
        long double swm,*sw,xpiv;
        int k,ki;
        //cout<<"debut de ordonne\n";
        sw =new long double[nmodel+1];
        for (int i=0;i<nmodel+1;i++) sw[i]=0.0;
        swm=0.0;
        for (int i=0;i<nli;i++) sw[int(vecY[i])] +=cvecW[i];
        for (int i=0;i<nmodel+1;i++) numod[i]=-1;
        for (int i=0;i<nmodel+1;i++) {
            swm +=sw[i];
            k=0;
            for (int j=0;j<nmodel+1;j++) if((i!=j)and(sw[i]<sw[j])) k++;
            while (numod[k]!=-1) k++;
            numod[k]=i;
        }
        //for (int i=0;i<nmodel+1;i++) cout<<"numod["<<i<<"]="<<numod[i]<<"\n";
        //cout<<"dans ordonne nmodel="<<nmodel<<"\n";
        for (int i=0;i<nli-1;i++) {
            ki=0;while (int(vecY[i])!=numod[ki]) ki++;
            vecY[i]=(double)ki;
        }
        //cout<<"dans ordonne avant la permutation\n";
        for (int i=0;i<nli-1;i++) {
            for (int j=i+1;j<nli;j++) {
                if (vecY[i]>vecY[j]) {
                    xpiv=vecY[i];vecY[i]=vecY[j];vecY[j]=xpiv;
                    xpiv=cvecW[i];cvecW[i]=cvecW[j];cvecW[j]=xpiv;
                    for (int k=0;k<nco;k++) {xpiv=cmatX0[i][k];cmatX0[i][k]=cmatX0[j][k];cmatX0[j][k]=xpiv;}
                }
            }
        }
        delete []sw;

        /*for (int i=0;i<nmodel+1;i++) cout<<"numod["<<i<<"]="<<numod[i]<<"\n";
        cout<<"\n";
        ii=-1;
        for (k=0;k<nmodel+1;k++) {
            for (int i=0;i<nli;i++){
                if(int(vecY[i])==numod[k]) {
                    ii++;
                    vecY2[ii]=(double)k;
                    vecW2[ii]=vecW[i];
                    for (int j=0;j<nco;j++) matX2[ii][j]=cmatX0[i][j];
                }
            }
        }
        cout<<"apres la première recopie\n";
        for (int i=0;i<nli;i++) {
            vecY[i]=vecY2[i];
            vecW[i]=vecW2[i];
            for(int j=0;j<nco;j++) cmatX0[i][j]=matX2[i][j];
        }
        cout<<"avant les delete\n";
        delete []sw;           cout<<"sw OK\n";
        delete []vecW2;        cout<<"vecW2 OK\n";
        delete []vecY2;        cout<<"vecY2 OK\n";
        for (int i=0;i<nli;i++) delete []matX2[i];cout<<"matX2[i] OK\n";
        delete []matX2;cout<<"matX2 OK\n";*/
        //cout<<"apres delete sw\n";

    }

/**
* attribution des probabilités a posteriori des scénarios en tenant compte de leur renumérotation (vecteur numod)
*/
    void reordonne(int nmodel,int *numod,long double *px, long double *pxi,long double *pxs) {
        long double *qx,*qxi,*qxs;
        qx = new long double[nmodel+1];qxi = new long double[nmodel+1];qxs = new long double[nmodel+1];
        for (int i=0;i<nmodel+1;i++) {
            qx[numod[i]]=px[i];qxi[numod[i]]=pxi[i];qxs[numod[i]]=pxs[i];
        }
        for (int i=0;i<nmodel+1;i++) {
            px[i]=qx[i];pxi[i]=qxi[i];pxs[i]=qxs[i];
        }
        delete []qx;delete []qxi;delete []qxs;
    }

/**
* Effectue les calculs de la régression logistique 
*/
    int polytom_logistic_regression(int nli, int nco, long double **cmatX0, long double *vecY, long double *cvecW, long double *px, long double *pxi, long double *pxs)
    {
//        double clock_zero;
        int err,nmodel=0; // int no;
//        double debut, duree;
        double sx,sx2;
        int *numod;
        for (int i=1;i<nli;i++) {if (vecY[i]>1.0*nmodel) nmodel=vecY[i]+0.1;} // transforme les .99999 en (int)1
        if (nmodel<1) {px[0]=1.0;pxi[0]=1.0;pxs[0]=1.0;return 0;}
        //cout << "   nmodel="<<nmodel<<"   nco="<<nco<<"   nmodnco="<<nmodel*(nco+1)<<endl;
        numod=new int[nmodel+1];
        ordonne(nmodel,nli,nco,vecY,numod);
        //cout <<"apres ordonne\n";
        int i,j,l,m,n,nmodnco=nmodel*(nco+1),imod,rep;
        long double imody,betmin,betmax,coeff,mdiff,mdiff0;
        bool fin,caloglik,invOK;
        for (i=0;i<nli;i++) {cmatX[i][0]=1.0; for (j=0;j<nco;j++) cmatX[i][j+1]=cmatX0[i][j];}
        //cout<<"\n";cout<<"\n";for (i=0;i<10;i++) {for (j=0;j<11;j++) cout<<cmatX[i][j]<<"  "; cout<<"\n";} cout<<"\n";
        for (imod=0;imod<nmodel;imod++)
            {imody=imod+1;
            for (i=0;i<nli;i++) {if(fabs(vecY[i]-imody)<0.1) cmatY[i][imod]=1.0; else cmatY[i][imod]=0.0;}
            }
        for (i=0;i<nli;i++) {csmatY[i]=0.0; for (j=0;j<nmodel;j++)csmatY[i]+=cmatY[i][j];}
        for (i=0;i<nli;i++) {for (j=0;j<nco+1;j++) cmatXT[j][i]=cmatX[i][j];} //transposition
        for (i=0;i<nmodnco;i++) {cbeta0[i]=0.0;cbeta[i]=0.0;}
        rep=0;fin=false;

        while (fin==false)
            {
//              clock_zero=0.0;debut=walltime(&clock_zero);
              rep++;
              remplimatriceYP(nli,nco,nmodel,cmatP,cmatYP,cbeta,cmatX,cvecW,cmatY,csmatP);
            for (i=0;i<nmodnco;i++) {for (j=0;j<nmodnco;j++) cmatC[i][j]=0.0;}
            #pragma omp parallel shared(cmatC,cvecW,cmatP,cmatX) private(i,m,n,j,l)
            {
            #pragma omp for schedule(dynamic)

              for (j=0;j<nco+1;j++)
                {for (l=j;l<nco+1;l++)
                    {for (m=0;m<nmodel;m++)
                        {for (n=m;n<nmodel;n++)
                            {for (i=0;i<nli;i++)
                                {if (m!=n) cmatC[m*(nco+1)+j][n*(nco+1)+l]-=cvecW[i]*cmatP[i][m]*cmatP[i][n]*cmatX[i][j]*cmatX[i][l];
                                    else   cmatC[m*(nco+1)+j][n*(nco+1)+l]+=cvecW[i]*cmatP[i][m]*(1.0-cmatP[i][m])*cmatX[i][j]*cmatX[i][l];
                                }
                            cmatC[m*(nco+1)+l][n*(nco+1)+j]=cmatC[m*(nco+1)+j][n*(nco+1)+l];
                            if (m!=n)
                                {cmatC[n*(nco+1)+j][m*(nco+1)+l]=cmatC[m*(nco+1)+j][n*(nco+1)+l];
                                cmatC[n*(nco+1)+l][m*(nco+1)+j]=cmatC[n*(nco+1)+j][m*(nco+1)+l];
                                }
                            }
                        }
                    }
                }
              }
        //duree=walltime(&debut);time_matC += duree;
			//kap = kappa(nmodnco,cmatC);
			//printf("\nkappa(cmatC) = %10Le",kap);
			//if(kap>1.0E99) cout <<"   MATRICE SINGULIERE\n"; else cout<<"\n";
			//if(kap>1.0E99) coeff=1.0E-15; else coeff=1.0E-20;
			coeff=1.0E-15;
			err=inverse_Tik2(nmodnco,cmatC,cmatB,coeff);
			mdiff=1.0;
			do {
				coeff *=sqrt(10.0);/*mdiff = 1.0;*/
				for (i=0;i<nmodnco;i++) {for (j=0;j<nmodnco;j++) cmatB0[i][j] = cmatB[i][j];}
				mdiff0=mdiff;
				err=inverse_Tik2(nmodnco,cmatC,cmatB,coeff);
				if (err==0) {
				    mdiff=0.0;
					for (i=0;i<nmodnco;i++) {
						for (j=0;j<nmodnco;j++) mdiff += fabs(cmatB[i][j]-cmatB0[i][j])/(fabs(cmatB[i][j])+fabs(cmatB0[i][j]));
					}
					mdiff /=(long double)nmodnco;
					mdiff /=(long double)nmodnco;
				}
				//printf("coeff = %8Le   mdiff = %8.5Lf   mdiff0 = %8.5Lf   err=%d   nmodnco=%d\n",coeff,mdiff,mdiff0,err,nmodnco);
				invOK = ((err==0)and/*(mdiff<0.02)*/((mdiff>mdiff0)or(mdiff<0.001)));
			} while ((not invOK)and(coeff<0.01));
			for (i=0;i<nmodnco;i++) {for (j=0;j<nmodnco;j++) cmatB[i][j] = cmatB0[i][j];}			
			if (not invOK) {err=9;cout <<"Echec de l'inversion de la matrice cmatC\n";}
			for (i=0;i<nmodnco;i++) {cdeltabeta[i]=0.0;for (j=0;j<nmodnco;j++) cdeltabeta[i]+=cmatB[i][j]*cmatYP[j];}
            for (i=0;i<nmodnco;i++) {if (cdeltabeta[i] != cdeltabeta[i]) err=10;}
			for (i=0;i<nmodnco;i++) cbeta[i]=cbeta0[i]+cdeltabeta[i];
            remplimatriceYP(nli,nco,nmodel,cmatP,cmatYP,cbeta,cmatX,cvecW,cmatY,csmatP);
            caloglik=false;
            if (rep==1)
                {while (caloglik==false)
                    {caloglik=cal_loglik(nli,nmodel,rep,cloglik,cmatY,cmatP,cvecW,csmatY,csmatP);
                    if (caloglik==false)
                        {for (i=0;i<nmodnco;i++) cdeltabeta[i]*=0.5;
                        for (i=0;i<nmodnco;i++) cbeta[i]=cbeta0[i]+cdeltabeta[i];
                        remplimatriceYP(nli,nco,nmodel,cmatP,cmatYP,cbeta,cmatX,cvecW,cmatY,csmatP);
                        }
                    }
                }else
                {while ((caloglik==false)||(cloglik[rep-1]<cloglik[rep-2]-0.000001))
                    {caloglik=cal_loglik(nli,nmodel,rep,cloglik,cmatY,cmatP,cvecW,csmatY,csmatP);
                    if ((caloglik==false)||(cloglik[rep-1]<cloglik[rep-2]-0.000001))
                        {for (i=0;i<nmodnco;i++) cdeltabeta[i]*=0.5;
                        for (i=0;i<nmodnco;i++) cbeta[i]=cbeta0[i]+cdeltabeta[i];
                        remplimatriceYP(nli,nco,nmodel,cmatP,cmatYP,cbeta,cmatX,cvecW,cmatY,csmatP);
                        }
                    }
                }
            //cout<<"apres if rep=1\n";
            for (i=0;i<nmodnco;i++)cbeta0[i]=cbeta[i];
            for (i=0;i<nmodel;i++) {for (j=0;j<nmodel;j++) cmatA[i][j]=cmatB[i*(nco+1)][j*(nco+1)];}
            cbet[0]=0.0;for(i=0;i<nmodel;i++) cbet[i+1]=cbeta0[i*(nco+1)];
            if (rep==1) {for (i=0;i<nmodel+1;i++) cpx0[i]=2.0;} else {for (i=0;i<nmodel+1;i++) cpx0[i]=px[i];}
            //cout<<"avant calcul_psd\n";
            calcul_psd(nmodel,cbet,cmatA,csd,px);
			//cout<<"apres calcul_psd\n";
            betmin=cbet[0];betmax=cbet[0];
            for (i=0;i<nmodel+1;i++) {if (betmin>cbet[i]) betmin=cbet[i];}
            for (i=0;i<nmodel+1;i++) {if (betmax<cbet[i]) betmax=cbet[i];}
            //cout<<"apres calcul des betmin, betmax     rep="<<rep<<"\n";
			if (rep>1){
				fin=true;i=0;while ((fin==true)&&(i<nmodel+1)) {
					fin= (fabs(px[i]-cpx0[i])<0.0001)or(fabs(cloglik[rep-1]/cloglik[rep-2]-1.0)<0.000001);
					printf("abs(px-px0) = %12.7Lf\n",fabs(px[i]-cpx0[i]));i++;
				}
				fin=(fin or (betmax-betmin>50));
				if ((not fin) and (rep>50)) {err=8;fin=true;}
			} else {
				fin=true;i=0;while ((fin==true)&&(i<nmodel+1)) fin= (fabs(px[i]-cpx0[i])<0.0001);
			}
/////////////////////////
            /*if (rep<=20) {
                cout<<"\ncbet       : ";
                for(i=0;i<nmodel;i++) cout<< setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<cbet[i+1]<<"  ";cout<<"\n";
                cout<<"cdeltabeta : ";
                for(i=0;i<nmodel;i++) cout<< setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<cdeltabeta[i*(nco+1)]<<"  ";cout<<"\n\n";
                //for(i=0;i<nmodel;i++) {for (j=0;j<nmodel;j++) cout<<cmatB[i*(nco+1)][i*(nco+1)]<<"  ";cout<<"\n";}
            }*/

            cout<<"\niteration "<<rep<<"   fin="<<fin<<"\n";
            for (i=0;i<nmodel+1;i++) {cout<<"  ";cout<< setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<px[i];}
            sx=0.0;sx2=0.0;for(i=0;i<nmodel+1;i++) {sx+=px[i];sx2+=px[i]*px[i];}
            //if ((sx<0.001)or(sx2>0.999)) {
                //cout<<"\nsx="<<sx<<"   sx2="<<sx2<<"   loglik="<<cloglik[rep-1]<<"\n";
				//printf("\nsx= %6.3f   sx2= %6.3f     loglik= %15.8Lf\n",sx,sx2,cloglik[rep-1]);
                //for (i=0;i<nmodnco;i++) cout<<cbeta[i]<<"\n";
            //    exit(1);
            //}
//////////////////////////
            }
            if (err>1) return err;
        cout<<"\n";
        for (imod=0;imod<nmodel+1;imod++)
                {pxi[imod]=px[imod]-1.96*csd[imod];
                if (pxi[imod]<0.0) pxi[imod]=0.0;
                pxs[imod]=px[imod]+1.96*csd[imod];
                if (pxs[imod]>1.0) pxs[imod]=1.0;
                }
        reordonne(nmodel,numod,px,pxi,pxs);
        cout<<"\niteration "<<rep;
        for (i=0;i<nmodel+1;i++) {cout<<"  ";cout<< setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<px[i];}
        cout<<"\n";
        //if ((rep==2)and(sx2<0.9999)) exit(1); // FIXME
		return err;
   }

/**
* Prépare et lance les calculs de la régression logistique 
*/
    posteriorscenC* call_polytom_logistic_regression(int nts, float *stat_obs, int nscenutil,int *scenchoisiutil) {
        posteriorscenC *postlog;
        bool trouve;
        int ntt,j,kk,err;
        long double som, *px,*pxi,*pxs;
//		double duree,debut,clock_zero;
//        clock_zero=0.0;debut=walltime(&clock_zero);
        postlog = new posteriorscenC[rt.nscenchoisi];
        for (int i=0;i<rt.nscenchoisi;i++) {
		      postlog[i].err=0;
              if (i==0) {postlog[i].x=1.0;postlog[i].inf=1.0;postlog[i].sup=1.0;}
              else      {postlog[i].x=0.0;postlog[i].inf=0.0;postlog[i].sup=0.0;}
        }
        if (rt.enrsel[nts-1].dist>0.0) {
			rempli_mat0(nts,stat_obs);
			//cout<<"call_polytom_logistic_regression 0  nts="<<nts<<"   nscenutil="<<nscenutil<<"\n";
			//for (j=0;j<nscenutil;j++) cout<<scenchoisiutil[j]<<"\n";
			ntt=nts;
			for (int i=0;i<nts;i++)  {
				//cout<<"rt.enrsel["<<i<<"].numscen="<<rt.enrsel[i].numscen<<"\n";
				trouve=false;
				for (j=0;j<nscenutil;j++) {trouve=(scenchoisiutil[j]==rt.enrsel[i].numscen);if (trouve) break;}
				if (trouve) vecY[i]=j;
				else {vecY[i]=-1;ntt--;}
			}
			//cout<<"call_polytom_logistic_regression 1\n";
			if (ntt<=nts) {
				for (int i=0;i<nts;i++) matA[i].x = new long double[nstatOKsel+2];
				kk=0;
				for (int k=0;k<nscenutil;k++) {
					for (int i=0;i<nts;i++) {
						if (vecY[i]==k) {
							matA[kk].x[0]=(long double)vecY[i];
							matA[kk].x[1]= cvecW[i];
							for (int j=0;j<nstatOKsel;j++) matA[kk].x[2+j] = cmatX0[i][j];
							kk++;
						}
					}
				}
				for (int i=0;i<ntt;i++) {
					vecYY[i]=matA[i].x[0];
					cvecW[i]=matA[i].x[1];
					for (int j=0;j<nstatOKsel;j++) cmatX0[i][j]=matA[i].x[j+2];
				}
				for (int i=0;i<nts;i++) delete [] matA[i].x; 
			}
			//cout<<"call_polytom_logistic_regression 2\n";
			som=0.0;for (int i=0;i<ntt;i++) som += cvecW[i];
			for (int i=0;i<ntt;i++) cvecW[i] = cvecW[i]/som*(long double)ntt;
			px = new long double[rt.nscenchoisi];pxi = new long double[rt.nscenchoisi];pxs = new long double[rt.nscenchoisi];
			err = polytom_logistic_regression(nts, nstatOKsel, cmatX0, vecYY, cvecW, px, pxi, pxs);
			if (err>1) {
				long double d,a=1.0/(long double)nts;
				for (int i=0;i<rt.nscenchoisi;i++) {
					postlog[i].x=0.0;
					for (int j=0;j<nts;j++) if (rt.scenchoisi[i]==rt.enrsel[j].numscen) postlog[i].x +=a;
					if ((fabs(postlog[i].x)<0.00001)or(fabs(1.0-postlog[i].x)<0.00001)) d=0.0;
					else d=1.96*sqrt(postlog[i].x*(1.0-postlog[i].x)/(long double)nts);
					postlog[i].inf =postlog[i].x-d;if(postlog[i].inf<0.0)postlog[i].inf=0.0;
					postlog[i].sup =postlog[i].x+d;if(postlog[i].sup>1.0)postlog[i].sup=1.0;
					postlog[i].err=err;
				}
			  
			} else {
				if (nscenutil==rt.nscenchoisi) for (int i=0;i<nscenutil;i++) {postlog[i].x=px[i];postlog[i].inf=pxi[i];postlog[i].sup=pxs[i];}
				else {
					for (int i=0;i<rt.nscenchoisi;i++) {postlog[i].x=0.0;postlog[i].inf=0.0;postlog[i].sup=0.0;}
					for  (int i=0;i<nscenutil;i++)  {
						kk=0;
						while (scenchoisiutil[i]!=rt.scenchoisi[kk]) kk++;
						postlog[kk].x=px[i];postlog[kk].inf=pxi[i];postlog[kk].sup=pxs[i];
					}
				}
			}
			delete [] px; delete []pxi; delete []pxs;
//			duree=walltime(&debut);time_call += duree;
			//cout<<"call_polytom_logistic_regression 3\n";
		} else {
		    long double d,a=1.0/(long double)nts;
			for (int i=0;i<rt.nscenchoisi;i++) {
				postlog[i].x=0.0;
				for (int j=0;j<nts;j++) if (rt.scenchoisi[i]==rt.enrsel[j].numscen) postlog[i].x +=a;
                if ((fabs(postlog[i].x)<0.00001)or(fabs(1.0-postlog[i].x)<0.00001)) d=0.0;
                else d=1.96*sqrt(postlog[i].x*(1.0-postlog[i].x)/(long double)nts);
                postlog[i].inf =postlog[i].x-d;if(postlog[i].inf<0.0)postlog[i].inf=0.0;
                postlog[i].sup =postlog[i].x+d;if(postlog[i].sup>1.0)postlog[i].sup=1.0;
				postlog[i].err=5; 
			}
		}
        return postlog;
    }
/**
 *  Sélectionne les scénarios participant à la régression logistique et
 * appelle cette dernière s'il y en a au moins deux
 */ 
    posteriorscenC* comp_logistic(int nts,float *stat_obs) {
        int *postdir,nscenutil,*scenchoisiutil,kk;
        posteriorscenC *postlog;
        postdir= new int[rt.nscenchoisi];
        for (int i=0;i<rt.nscenchoisi;i++) {
            postdir[i]=0;
            for (int j=0;j<nts;j++) if (rt.scenchoisi[i]==rt.enrsel[j].numscen) postdir[i]++;
        }
        //for (int i=0;i<rt.nscenchoisi;i++) cout<<"\nscenario "<<rt.scenchoisi[i]<<"  : "<<postdir[i]<<"\n";
        //cout<<"\n";
        nscenutil=0;
        for (int i=0;i<rt.nscenchoisi;i++) if((postdir[i]>2)and(postdir[i]>nts/1000)) nscenutil++;
        scenchoisiutil = new int[nscenutil];
        kk=0;for (int i=0;i<rt.nscenchoisi;i++) if((postdir[i]>2)and(postdir[i]>nts/1000)) {scenchoisiutil[kk]=rt.scenchoisi[i];kk++;}
        //cout <<"nscenutil="<<nscenutil<<"\n";
        if (nscenutil==1) {
            postlog = new posteriorscenC[rt.nscenchoisi];
            for (int i=0;i<rt.nscenchoisi;i++) {
              if((postdir[i]>2)and(postdir[i]>nts/1000)) {postlog[i].x=1.0;postlog[i].inf=1.0;postlog[i].sup=1.0;postlog[i].err=0;}
              else                                       {postlog[i].x=0.0;postlog[i].inf=0.0;postlog[i].sup=0.0;postlog[i].err=0;}
            }
        } else postlog = call_polytom_logistic_regression(nts,stat_obs,nscenutil,scenchoisiutil);
        //cout<<"\n";
        //for (int i=0;i<rt.nscenchoisi;i++) cout<<"scenario "<<rt.scenchoisi[i]<<"   "<<postlog[i].x<<"   ["<<postlog[i].inf<<","<<postlog[i].sup<<"]\n";
        delete []postdir;
        delete []scenchoisiutil;
        return postlog;
    }

/**
 * Interprête la ligne de paramètres de l'option "comparaison de scenarios" et lance les calculs correspondants
 */ 
    void docompscen(string opt){
        int nstatOK,iprog,nprog;;
        int nrec = 0,nseld = 0,nselr = 0,nsel = 0,ns,nlogreg = 0,k,nts;
        string *ss = NULL,s,*ss1 = NULL,s0,s1;
        float  *stat_obs;
        bool AFD=false;
		/*double duree,debut,clock_zero;*/
        posteriorscenC **postscendir,**postscenlog;
        progressfilename = path + ident + "_progress.txt";
        //strcpy(progressfilename,path);
        //strcat(progressfilename,ident);
        //strcat(progressfilename,"_progress.txt");

        ss = splitwords(opt,";",&ns);
        for (int i=0;i<ns;i++) { //cout<<ss[i]<<"\n";
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);
            if (s0=="s:") {
                ss1 = splitwords(s1,",",&rt.nscenchoisi);
                rt.scenchoisi = new int[rt.nscenchoisi];
                for (int j=0;j<rt.nscenchoisi;j++) rt.scenchoisi[j] = atoi(ss1[j].c_str());
                delete [] ss1; ss1 = NULL;
                cout <<"scenario(s) choisi(s) : ";
                for (int j=0;j<rt.nscenchoisi;j++) {cout<<rt.scenchoisi[j]; if (j<rt.nscenchoisi-1) cout <<",";}cout<<"\n";
            } else if (s0=="n:") {
                nrec=atoi(s1.c_str());
                cout<<"nombre total de jeux de données considérés (tous scénarios confondus)= "<<nrec<<"\n";
            } else if (s0=="d:") {
                nseld=atoi(s1.c_str());
                cout<<"nombre de jeux de données considérés pour la méthode directe = "<<nseld<<"\n";
            } else if (s0=="l:") {
                nselr=atoi(s1.c_str());
                cout<<"nombre de jeux de données considérés pour la régression logistique = "<<nselr<<"\n";
            } else if (s0=="m:") {
                nlogreg=atoi(s1.c_str());
                if (nlogreg==0) nselr=0;
                cout<< "nombre de régressions logistiques à effectuer = "<<nlogreg<<"\n";
            } else if (s0=="f:") {
                AFD=(s1=="1");
                if (AFD) cout<<"Factorial Discriminant Analysis\n";
            }
        }
        nsel=nseld;if(nsel<nselr)nsel=nselr;
        nprog=6+nlogreg;
        iprog=1;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
        nstatOK = rt.cal_varstat();
        //header.calstatobs(statobsfilename);
		stat_obs = header.stat_obs;
        rt.alloue_enrsel(nsel);
//        clock_zero=0.0;debut=walltime(&clock_zero);
        rt.cal_dist(nrec,nsel,stat_obs);
//        duree=walltime(&debut);time_readfile += duree;
		if (AFD) transAFD(nrec,nsel,stat_obs);
        iprog+=4;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
        postscendir = comp_direct(nseld);
        save_comp_direct(nseld,postscendir,path,ident);  cout<<"apres save_comp_direct\n";

        postscenlog = new posteriorscenC*[nlogreg];
        if (nlogreg>0) {
            allouecmat(rt.nscenchoisi, nselr, rt.nstat);cout<<"apres allouecmat\n";
            k=0;
            while(k<nlogreg){
                nts=(nselr/nlogreg)*(k+1);
                postscenlog[k] = comp_logistic(nts,stat_obs);cout <<"apres la regression logistique n° "<<k<<"\n";
                k++;
                iprog+=1;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
            }
            save_comp_logistic(nlogreg,nselr,postscenlog,path,ident);
            liberecmat(rt.nscenchoisi, nselr, rt.nstat);
        }
        if(ss != NULL) delete []ss;
        if(ss1 != NULL) delete []ss1;
        iprog+=1;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
    }
