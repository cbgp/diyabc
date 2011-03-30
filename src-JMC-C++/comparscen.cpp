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

struct matligneC
{
    double* x;
};

struct posteriorscenC
{
    double x,inf,sup;
};

/** 
* définit l'opérateur de comparaison de deux lignes de la matrice matA
* pour l'utilisation de la fonction sort du module algorithm
*/
struct complignes
{
   bool operator() (const matligneC & lhs, const matligneC & rhs) const
   {
      return lhs.x[0] < rhs.x[0];
   }
};

int ncs=100;

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
                 if ((abs(p)<0.00001)or(abs(1.0-p)<0.00001)) d=0.0;
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
    }
   
    
/** 
* effectue le remplissage de la matrice matX0 et du vecteur des poids vecW 
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
        /*cout <<"\n matX0:\n";
        for (int i=0;i<5;i++) { cout<<rt.enrsel[i].numscen<<"   ";
            for (int j=0;j<nstatOKsel;j++) cout<<matX0[i][j]<<"  ";cout<<"\n";
        }*/
        for (int i=0;i<n;i++) vecW[i]/=som;
        /*cout <<"\nvecW:\n";for (int i=0;i<10;i++) cout<<vecW[i]<<"  ";
        cout<<"\n";*/
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
        for (imod=0;imod<nmodel;imod++){
            for (j=0;j<nco+1;j++){
                for (i=0;i<nli;i++) matYP[imod*(nco+1)+j]+=(matY[i][imod]-matP[i][imod])*matX[i][j]*vecW[i];
            }
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

    void ordonne(int nmodel,int nli,int nco,double *vecY,int *numod) {
        double swm,*sw,*vecW2,*vecY2,**matX2;
        int k,ii;
        //cout<<"debut de ordonne\n";
        sw =new double[nmodel+1];
        vecW2=new double[nli];vecY2 = new double[nli];
        matX2 = new double*[nli];for (int i=0;i<nli;i++) matX2[i] = new double[nco];
        for (int i=0;i<nmodel+1;i++) sw[i]=0.0;
        swm=0.0;
        for (int i=0;i<nli;i++) sw[int(vecY[i])] +=vecW[i];
        for (int i=0;i<nmodel+1;i++) numod[i]=-1;
        for (int i=0;i<nmodel+1;i++) {
            swm +=sw[i];
            k=0;
            for (int j=0;j<nmodel+1;j++) if((i!=j)and(sw[i]<sw[j])) k++;
            while (numod[k]!=-1) k++;
            numod[k]=i;    
        }
        //for (int i=0;i<nmodel+1;i++) cout<<"numod["<<i<<"]="<<numod[i]<<"\n";
        ii=-1;
        for (k=0;k<nmodel+1;k++) {
            for (int i=0;i<nli;i++){
                if(int(vecY[i])==numod[k]) {
                    ii++;
                    vecY2[ii]=(double)k;
                    vecW2[ii]=vecW[i];
                    for (int j=0;j<nco;j++) matX2[ii][j]=matX0[i][j];
                }
            }
        }
        //cout<<"apres la première recopie\n";
        for (int i=0;i<nli;i++) {
            vecY[i]=vecY2[i];
            vecW[i]=vecW2[i];
            for(int j=0;j<nco;j++) matX0[i][j]=matX2[i][j];
        }
        delete []sw;delete []vecW2;delete []vecY2;
        for (int i=0;i<nli;i++) delete []matX2[i];delete []matX2;
    }

    void reordonne(int nmodel,int *numod,double *px, double *pxi,double *pxs) {
        double *qx,*qxi,*qxs;
        qx = new double[nmodel+1];qxi = new double[nmodel+1];qxs = new double[nmodel+1];
        for (int i=0;i<nmodel+1;i++) {
            qx[numod[i]]=px[i];qxi[numod[i]]=pxi[i];qxs[numod[i]]=pxs[i];
        }
        for (int i=0;i<nmodel+1;i++) {
            px[i]=qx[i];pxi[i]=qxi[i];pxs[i]=qxs[i];
        }
        delete []qx;delete []qxi;delete []qxs;
    }

    int polytom_logistic_regression(int nli, int nco, double **matX0, double *vecY, double *vecW, double *px, double *pxi, double *pxs)
    {
        int nmodel=0;
        double debut,duree,de1,du1;
        int *numod;
        for (int i=1;i<nli;i++) {if (vecY[i]>1.0*nmodel) nmodel=vecY[i]+0.1;}
        if (nmodel<1) {px[0]=1.0;pxi[0]=1.0;pxs[0]=1.0;return 0;}
        //cout << "   nmodel="<<nmodel<<endl;
        numod=new int[nmodel+1];
        ordonne(nmodel,nli,nco,vecY,numod);
        //cout <<"apres ordonne\n";
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
            /*cout<<"\n matC:\n";
            for (i=0;i<5;i++){
                 for (j=0;j<9;j++) cout<<matC[i][j]<<"   ";
                 cout<<"\n";
            }*/
            inverse(nmodnco,matC,matB);
            //InvdiagMat(nmodnco,matC,matB);
            /*cout<<"\n matB:\n";
            for (i=0;i<5;i++){
                 for (j=0;j<9;j++) cout<<matC[i][j]<<"   ";
                 cout<<"\n";
            }*/
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
            //std::cout << "iteration " << rep <<"   log-lik= " << loglik[rep-1] << "\n";
            for (i=0;i<nmodel;i++) {for (j=0;j<nmodel;j++) matA[i][j]=matB[i*(nco+1)][j*(nco+1)];}
            bet[0]=0.0;for(i=0;i<nmodel;i++) bet[i+1]=beta0[i*(nco+1)];
            if (rep==1) {for (i=0;i<nmodel+1;i++) px0[i]=2.0;} else {for (i=0;i<nmodel+1;i++) px0[i]=px[i];}
            calcul_psd(nmodel,bet,matA,sd,px);
            betmin=bet[0];betmax=bet[0];
            for (i=0;i<nmodel+1;i++) {if (betmin>bet[i]) betmin=bet[i];}   
            for (i=0;i<nmodel+1;i++) {if (betmax<bet[i]) betmax=bet[i];}
            fin=true;i=0;while ((fin==true)&&(i<nmodel+1)) {fin= (fabs(px[i]-px0[i])<0.0001);i++;}
            fin=(fin||(betmax-betmin>50));
            //cout <<"\nbet:\n";
            //for (i=0;i<nmodel+1;i++) {std::cout << bet[i] <<"   ";}
            //std::cout << "\npx:\n";
            //for (i=0;i<nmodel+1;i++) {std::cout << px[i] <<"   ";}
            //std::cout << "\n";std::cout << "\n";
            }
        for (imod=0;imod<nmodel+1;imod++)
                {pxi[imod]=px[imod]-1.96*sd[imod];
                if (pxi[imod]<0.0) pxi[imod]=0.0;
                pxs[imod]=px[imod]+1.96*sd[imod];
                if (pxs[imod]>1.0) pxs[imod]=1.0;
                //cout<<"model "<<imod<<"  "<<px[imod]<<"  ["<<pxi[imod]<<","<<pxs[imod]<<"]   sd="<<sd[imod]<<"\n";
                }
        reordonne(nmodel,numod,px,pxi,pxs);    
        //for (imod=0;imod<nmodel+1;imod++) cout<<"model "<<imod<<"  "<<px[imod]<<"  ["<<pxi[imod]<<","<<pxs[imod]<<"]\n";
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
        delete[] smatY;delete[] smatP;
   }
      
    posteriorscenC* call_polytom_logistic_regression(int nts, double *stat_obs, int nscenutil,int *scenchoisiutil) {
        posteriorscenC *postlog;
        int *vecY,ntt,j,dtt,k,kk;
        double som,*vecYY, *px,*pxi,*pxs,somw;
        matligneC *matA;
        postlog = new posteriorscenC[rt.nscenchoisi];
        for (int i=0;i<rt.nscenchoisi;i++) {
              if (i==0) {postlog[i].x=1.0;postlog[i].inf=1.0;postlog[i].sup=1.0;}
              else      {postlog[i].x=0.0;postlog[i].inf=0.0;postlog[i].sup=0.0;}  
        }
        cout<<"\ncall_polytom_logistic_regression avant rempli_mat0\n";
        rempli_mat0(nts,stat_obs);
        vecY = new int[nts];
        ntt=nts;
        for (int i=0;i<nts;i++)  {
            j=0;while ((j<nscenutil)and(scenchoisiutil[j]!=rt.enrsel[i].numscen)) j++;
            if (scenchoisiutil[j]==rt.enrsel[i].numscen) vecY[i]=j;
            else {vecY[i]=-1;ntt--;}
        }
        if (ntt<=nts) {
            matA = new matligneC[ntt];
            for (int i=0;i<nts;i++) matA[i].x = new double[nstatOKsel+2];
            kk=0;
            for (int k=0;k<nscenutil;k++) {
                for (int i=0;i<nts;i++) { 
                    if (vecY[i]==k) {
                        matA[kk].x[0]=(double)vecY[i];
                        matA[kk].x[1]= vecW[i];
                        for (int j=0;j<nstatOKsel;j++) matA[kk].x[2+j] = matX0[i][j];
                        kk++;
                    }
                }
            }
            vecYY = new double[ntt];
            for (int i=0;i<ntt;i++) {
                vecYY[i]=matA[i].x[0];
                vecW[i]=matA[i].x[1];
                for (int j=0;j<nstatOKsel;j++) matX0[i][j]=matA[i].x[j+2];
            }
        }
        som=0.0;for (int i=0;i<ntt;i++) som += vecW[i];
        for (int i=0;i<ntt;i++) vecW[i] = vecW[i]/som*(double)ntt;
        px = new double[nscenutil];pxi = new double[nscenutil];pxs = new double[nscenutil];
        polytom_logistic_regression(nts, nstatOKsel, matX0, vecYY, vecW, px, pxi, pxs);
        if (nscenutil==rt.nscenchoisi) for (int i=0;i<nscenutil;i++) {postlog[i].x=px[i];postlog[i].inf=pxi[i];postlog[i].sup=pxs[i];}
        else {
            for (int i=0;i<rt.nscenchoisi;i++) {postlog[i].x=0.0;postlog[i].inf=0.0;postlog[i].sup=0.0;}
            for  (int i=0;i<nscenutil;i++)  {
                kk=0;
                while (scenchoisiutil[i]!=rt.scenchoisi[kk]) kk++;
                postlog[kk].x=px[i];postlog[kk].inf=pxi[i];postlog[kk].sup=pxs[i];
            }
        }
        for (int i=0;i<nts;i++) delete [] matA[i].x; delete [] matA;
        for (int i=0;i<nts;i++) delete [] matX0[i]; delete [] matX0;
        delete [] vecY;
        delete [] vecYY;
        delete [] vecW;
        delete [] px; delete []pxi; delete []pxs;
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
        //for (int i=0;i<rt.nscenchoisi;i++) cout<<"scenario "<<rt.scenchoisi[i]<<"  : "<<postdir[i]<<"\n";
        //cout<<"\n";
        nscenutil=0;
        for (int i=0;i<rt.nscenchoisi;i++) if((postdir[i]>2)and(postdir[i]>nts/1000)) nscenutil++;
        scenchoisiutil = new int[nscenutil];
        kk=0;for (int i=0;i<rt.nscenchoisi;i++) if((postdir[i]>2)and(postdir[i]>nts/1000)) {scenchoisiutil[kk]=rt.scenchoisi[i];kk++;}
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
      
    void docompscen(char *compar,bool multithread){
        char *datafilename, *progressfilename;
        int rtOK,nstatOK,iprog,nprog;;
        int nrec,nseld,nselr,nsel,ns,ns1,nlogreg,k,kk,nts;
        string opt,*ss,s,*ss1,s0,s1;
        double  *stat_obs;
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
        nprog=5+nlogreg;
        iprog=1;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        nstatOK = rt.cal_varstat();
        stat_obs = header.read_statobs(statobsfilename);
        rt.alloue_enrsel(nsel);
        rt.cal_dist(nrec,nsel,stat_obs);
        iprog+=4;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        postscendir = comp_direct(nseld);
        save_comp_direct(nseld,postscendir,path,ident);
        
        postscenlog = new posteriorscenC*[nlogreg];
        if (nlogreg>0) {
            k=0;
            while(k<nlogreg){ 
                nts=(nselr/nlogreg)*(k+1);
                postscenlog[k] = comp_logistic(nts,stat_obs);
                k++;
                iprog+=1;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
            }
            save_comp_logistic(nlogreg,nselr,postscenlog,path,ident);
        }
    
    }