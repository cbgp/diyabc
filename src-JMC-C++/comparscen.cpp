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

using namespace std;

extern char *progressfilename;

struct matligneC
{
    double* x;
};

struct posteriorscenC
{
    double x,inf,sup;
};

/**
* définit l'opérateur de comparaison de deux lignes de la matrice cmatA
* pour l'utilisation de la fonction sort du module algorithm
*/
struct complignes
{
   bool operator() (const matligneC & lhs, const matligneC & rhs) const
   {
      return lhs.x[0] < rhs.x[0];
   }
};

extern double time_readfile;

int ncs=100;
double time_loglik=0.0,time_matC=0.0,time_call=0.0;
long double **cmatA,**cmatB,**cmatX,**cmatXT,**cmatC,*cdeltabeta,*cbeta0,*cbeta,**cmatP,**cmatY,*cmatYP,*cloglik,*csd,*cbet,*cpx0,*csmatY,*csmatP,**cgdb ;
long double *vecY,*vecYY,*cvecW,**cmatX0;
matligneC *matA;

    void allouecmat(int nmodel, int nli, int nco) {
        int nmodnco = nmodel*(nco+1);

        cmatA = new long double*[nmodel];for (int i=0;i<nmodel;i++) cmatA[i]= new long double[nmodel];
        cmatB = new long double*[nmodnco];for (int i=0;i<nmodnco;i++) cmatB[i]= new long double[nmodnco];
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

    void liberecmat(int nmodel, int nli, int nco) {
        int nmodnco = nmodel*(nco+1);

        for (int i=0;i<nmodel;i++) delete []cmatA[i];delete cmatA;
        for (int i=0;i<nmodnco;i++) delete []cmatB[i];delete cmatB;
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

    posteriorscenC** comp_direct(int n) {
        int nts,bidon,iscen,k,i;
        posteriorscenC **posts;
        double d,p,delta;
        bool scenOK;
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

    void  save_comp_direct(int n, posteriorscenC** posts, char *path, char *ident){
        //cout<<"apres calcul des postinf/postsup\n";
        int nts;
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
            if (nts%50==0){printf(" %6d   ",nts);for (int j=0;j<rt.nscenchoisi;j++) printf("   %6.4f [%6.4f,%6.4f]  ",posts[i][j].x,posts[i][j].inf,posts[i][j].sup);printf("\n");}
            fprintf(f1," %6d   ",nts);for (int j=0;j<rt.nscenchoisi;j++) fprintf(f1,"   %6.4f [%6.4f,%6.4f]  ",posts[i][j].x,posts[i][j].inf,posts[i][j].sup);fprintf(f1,"\n");
        }
        fclose(f1);
        for (int i=0;i<rt.nscenchoisi;i++) delete []posts[i];
        delete []posts;
        delete []nomfiparstat;

    }

    void save_comp_logistic(int nlogreg,int nselr,posteriorscenC** postscenlog,char *path, char *ident) {
        int nts;
        char *nomfiparstat;
        nomfiparstat = new char[strlen(path)+strlen(ident)+20];
        strcpy(nomfiparstat,path);
        strcat(nomfiparstat,ident);
        strcat(nomfiparstat,"_complogreg.txt");
        cout <<nomfiparstat<<"\n";
        FILE *f1;
        f1=fopen(nomfiparstat,"w");
        printf("     %s   ","n");for (int i=0;i<rt.nscenchoisi;i++) printf("          scenario %d       ",rt.scenchoisi[i]);printf("\n");
        fprintf(f1,"   %s   ","n");for (int i=0;i<rt.nscenchoisi;i++) fprintf(f1,"          scenario %d       ",rt.scenchoisi[i]);fprintf(f1,"\n");
        for (int i=0;i<nlogreg;i++) {
            nts = (nselr/nlogreg)*(i+1);
            printf(" %6d   ",nts);for (int j=0;j<rt.nscenchoisi;j++) printf("   %6.4f [%6.4f,%6.4f]  ",postscenlog[i][j].x,postscenlog[i][j].inf,postscenlog[i][j].sup);printf("\n");
            fprintf(f1," %6d   ",nts);for (int j=0;j<rt.nscenchoisi;j++) fprintf(f1,"   %6.4f [%6.4f,%6.4f]  ",postscenlog[i][j].x,postscenlog[i][j].inf,postscenlog[i][j].sup);fprintf(f1,"\n");
        }
        fclose(f1);
        for (int i=0;i<nlogreg;i++) delete []postscenlog[i];
        delete []postscenlog;
        delete []nomfiparstat;
    }


/**
* effectue le remplissage de la matrice cmatX0 et du vecteur des poids cvecW
*/
    void rempli_mat0(int n,double* stat_obs) {
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
            //cout <<"var_statsel["<<j<<"]="<<var_statsel[j]<<"\n";
            if (var_statsel[j]>0.0) nstatOKsel++;
            mo[j] = sx[j]/nn;
        }
        //cout <<"hello\n";
        som=0.0;
        for (int i=0;i<n;i++) {
            icc=-1;
            for (int j=0;j<rt.nstat;j++) {
                if (var_statsel[j]>0.0) {
                    icc++;
                    cmatX0[i][icc]=(rt.enrsel[i].stat[j]-stat_obs[j])/sqrt(var_statsel[j]);
                }
            }
            x=rt.enrsel[i].dist/delta;
            cvecW[i]=(1.5/delta)*(1.0-x*x);
            som = som + cvecW[i];
        }
        //cout <<"\n cmatX0:\n";
        /*for (int i=0;i<5;i++) { cout<<rt.enrsel[i].numscen<<"   ";
            for (int j=0;j<nstatOKsel;j++) cout<<cmatX0[i][j]<<"  ";cout<<"\n";
        }*/
        for (int i=0;i<n;i++) cvecW[i]/=som;
        //cout <<"\ncvecW:\n";for (int i=0;i<10;i++) cout<<cvecW[i]<<"  ";
        //cout<<"\n";
        delete []sx;delete []sx2;delete []mo;delete []var_statsel;
    }

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
                for (i=0;i<nli;i++) cmatYP[imod*(nco+1)+j]+=(cmatY[i][imod]-cmatP[i][imod])*cmatX[i][j]*cvecW[i];
            }
        }
    }

    bool cal_loglik(int nli, int nmodel, int rep, long double *cloglik, long double **cmatY, long double **cmatP, long double *vecW, long double *csmatY, long double *csmatP)
    {
        int i,imod;
        long double a,llik=0.0;
        double debut,duree,clock_zero=0.0;
        debut=walltime(&clock_zero);
        bool OK=true;
//    #pragma omp parallel for private(a,imod) if(multithread)\
//      reduction( + : llik )
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
        if (rep>1) {if(cloglik[rep-1]<cloglik[rep-2]-0.000001) {std::cout << "loglik decroit\n";return false;}}
        duree=walltime(&debut);time_loglik += duree;
        return OK;
    }

    void calcul_psd(int nmodel, long double *b0, long double **matV, long double *sd, double *px)
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

    void ordonne(int nmodel,int nli,int nco,long double *vecY,int *numod) {
        long double swm,*sw,xpiv;
        int k,ii,ki,kj;
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

    void reordonne(int nmodel,int *numod,double *px, double *pxi,double *pxs) {
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

    int polytom_logistic_regression(int nli, int nco, long double **cmatX0, long double *vecY, long double *cvecW, double *px, double *pxi, double *pxs)
    {
        double clock_zero;
        int nmodel=0;
        double debut,duree,de1,du1,sx,sx2;
        int *numod;
        for (int i=1;i<nli;i++) {if (vecY[i]>1.0*nmodel) nmodel=vecY[i]+0.1;}
        if (nmodel<1) {px[0]=1.0;pxi[0]=1.0;pxs[0]=1.0;return 0;}
        //cout << "   nmodel="<<nmodel<<"   nco="<<nco<<"   nmodnco="<<nmodel*(nco+1)<<endl;
        numod=new int[nmodel+1];
        ordonne(nmodel,nli,nco,vecY,numod);
        //cout <<"apres ordonne\n";
        int i,j,l,m,n,nmodnco=nmodel*(nco+1),imod,rep;
        long double imody,betmin,betmax;
        bool fin,caloglik;
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
              clock_zero=0.0;debut=walltime(&clock_zero);
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
        duree=walltime(&debut);time_matC += duree;
            inverse(nmodnco,cmatC,cmatB);
            for (i=0;i<nmodnco;i++) {cdeltabeta[i]=0.0;for (j=0;j<nmodnco;j++) cdeltabeta[i]+=cmatB[i][j]*cmatYP[j];}
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
            for (i=0;i<nmodnco;i++)cbeta0[i]=cbeta[i];
            for (i=0;i<nmodel;i++) {for (j=0;j<nmodel;j++) cmatA[i][j]=cmatB[i*(nco+1)][j*(nco+1)];}
            cbet[0]=0.0;for(i=0;i<nmodel;i++) cbet[i+1]=cbeta0[i*(nco+1)];
            if (rep==1) {for (i=0;i<nmodel+1;i++) cpx0[i]=2.0;} else {for (i=0;i<nmodel+1;i++) cpx0[i]=px[i];}
            calcul_psd(nmodel,cbet,cmatA,csd,px);
            betmin=cbet[0];betmax=cbet[0];
            for (i=0;i<nmodel+1;i++) {if (betmin>cbet[i]) betmin=cbet[i];}
            for (i=0;i<nmodel+1;i++) {if (betmax<cbet[i]) betmax=cbet[i];}
            fin=true;i=0;while ((fin==true)&&(i<nmodel+1)) {fin= (fabs(px[i]-cpx0[i])<0.0001);i++;}
            fin=(fin||(betmax-betmin>50));
/////////////////////////
            if (rep<=20) {
                cout<<"\ncbet       : ";
                for(i=0;i<nmodel;i++) cout<< setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<cbet[i+1]<<"  ";cout<<"\n";
                cout<<"cdeltabeta : ";
                for(i=0;i<nmodel;i++) cout<< setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<cdeltabeta[i*(nco+1)]<<"  ";cout<<"\n\n";
                //for(i=0;i<nmodel;i++) {for (j=0;j<nmodel;j++) cout<<cmatB[i*(nco+1)][i*(nco+1)]<<"  ";cout<<"\n";}
            }

            cout<<"\niteration "<<rep;
            for (i=0;i<nmodel+1;i++) {cout<<"  ";cout<< setiosflags(ios::fixed)<<setw(9)<<setprecision(3)<<px[i];}
            sx=0.0;sx2=0.0;for(i=0;i<nmodel+1;i++) {sx+=px[i];sx2+=px[i]*px[i];}
            //if ((sx<0.001)or(sx2>0.999)) {
                cout<<"\nsx="<<sx<<"   sx2="<<sx2<<"   loglik="<<cloglik[rep-1]<<"\n";
                //for (i=0;i<nmodnco;i++) cout<<cbeta[i]<<"\n";
            //    exit(1);
            //}
//////////////////////////
            }
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
        if ((rep==2)and(sx2>0.9999)) exit(1);
   }

    posteriorscenC* call_polytom_logistic_regression(int nts, double *stat_obs, int nscenutil,int *scenchoisiutil) {
        posteriorscenC *postlog;
        bool trouve;
        int ntt,j,dtt,k,kk;
        double som, *px,*pxi,*pxs,somw,duree,debut,clock_zero;
        clock_zero=0.0;debut=walltime(&clock_zero);
        postlog = new posteriorscenC[rt.nscenchoisi];
        for (int i=0;i<rt.nscenchoisi;i++) {
              if (i==0) {postlog[i].x=1.0;postlog[i].inf=1.0;postlog[i].sup=1.0;}
              else      {postlog[i].x=0.0;postlog[i].inf=0.0;postlog[i].sup=0.0;}
        }
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
            for (int i=0;i<nts;i++) matA[i].x = new double[nstatOKsel+2];
            kk=0;
            for (int k=0;k<nscenutil;k++) {
                for (int i=0;i<nts;i++) {
                    if (vecY[i]==k) {
                        matA[kk].x[0]=(double)vecY[i];
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
        }
        //cout<<"call_polytom_logistic_regression 2\n";
        som=0.0;for (int i=0;i<ntt;i++) som += cvecW[i];
        for (int i=0;i<ntt;i++) cvecW[i] = cvecW[i]/som*(double)ntt;
        px = new double[rt.nscenchoisi];pxi = new double[rt.nscenchoisi];pxs = new double[rt.nscenchoisi];
        polytom_logistic_regression(nts, nstatOKsel, cmatX0, vecYY, cvecW, px, pxi, pxs);
        if (nscenutil==rt.nscenchoisi) for (int i=0;i<nscenutil;i++) {postlog[i].x=px[i];postlog[i].inf=pxi[i];postlog[i].sup=pxs[i];}
        else {
            for (int i=0;i<rt.nscenchoisi;i++) {postlog[i].x=0.0;postlog[i].inf=0.0;postlog[i].sup=0.0;}
            for  (int i=0;i<nscenutil;i++)  {
                kk=0;
                while (scenchoisiutil[i]!=rt.scenchoisi[kk]) kk++;
                postlog[kk].x=px[i];postlog[kk].inf=pxi[i];postlog[kk].sup=pxs[i];
            }
        }
        delete [] px; delete []pxi; delete []pxs;
        duree=walltime(&debut);time_call += duree;
        //cout<<"call_polytom_logistic_regression 3\n";
        return postlog;
    }

    posteriorscenC* comp_logistic(int nts,double *stat_obs) {
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
              if((postdir[i]>2)and(postdir[i]>nts/1000)) {postlog[i].x=1.0;postlog[i].inf=1.0;postlog[i].sup=1.0;}
              else                                       {postlog[i].x=0.0;postlog[i].inf=0.0;postlog[i].sup=0.0;}
            }
        } else postlog = call_polytom_logistic_regression(nts,stat_obs,nscenutil,scenchoisiutil);
        //cout<<"\n";
        //for (int i=0;i<rt.nscenchoisi;i++) cout<<"scenario "<<rt.scenchoisi[i]<<"   "<<postlog[i].x<<"   ["<<postlog[i].inf<<","<<postlog[i].sup<<"]\n";
        delete []postdir;
        delete []scenchoisiutil;
        return postlog;
    }

    void docompscen(char *compar){
        char *datafilename;
        int nstatOK,iprog,nprog;;
        int nrec,nseld,nselr,nsel,ns,ns1,nlogreg,k,kk,nts;
        string opt,*ss,s,*ss1,s0,s1;
        double  *stat_obs,duree,debut,clock_zero;
        FILE *flog;
        posteriorscenC **postscendir,**postscenlog;

        progressfilename = new char[strlen(path)+strlen(ident)+20];
        strcpy(progressfilename,path);
        strcat(progressfilename,ident);
        strcat(progressfilename,"_progress.txt");

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
            }
        }
        nsel=nseld;if(nsel<nselr)nsel=nselr;
        nprog=6+nlogreg;
        iprog=1;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        nstatOK = rt.cal_varstat();
        stat_obs = header.read_statobs(statobsfilename);
        rt.alloue_enrsel(nsel);
        clock_zero=0.0;debut=walltime(&clock_zero);
        rt.cal_dist(nrec,nsel,stat_obs);
        duree=walltime(&debut);time_readfile += duree;
        iprog+=4;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        postscendir = comp_direct(nseld);
        save_comp_direct(nseld,postscendir,path,ident);

        postscenlog = new posteriorscenC*[nlogreg];
        if (nlogreg>0) {
            allouecmat(rt.nscenchoisi, nselr, rt.nstat);
            k=0;
            while(k<nlogreg){
                nts=(nselr/nlogreg)*(k+1);
                postscenlog[k] = comp_logistic(nts,stat_obs);
                k++;
                iprog+=1;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
            }
            save_comp_logistic(nlogreg,nselr,postscenlog,path,ident);
            liberecmat(rt.nscenchoisi, nselr, rt.nstat);
        }
        delete []ss;
        delete []ss1;
        iprog+=1;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
    }
