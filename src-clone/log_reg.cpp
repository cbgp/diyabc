/*
 *  log_reg.cpp
 *  Post_Weight
 *
 *  Created by INRA on 28/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include "log_reg.h"
#include "math.h"
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <sys/time.h>

#ifdef _OPENMP
  #include <omp.h>
#endif

#include <sys/time.h>

double walltime( double *t0 )
{
  double mic, time;
  double mega = 0.000001;
  struct timeval tp;
  struct timezone tzp;
  static long base_sec = 0;
  static long base_usec = 0;

  (void) gettimeofday(&tp,&tzp);
  if (base_sec == 0)
    {
      base_sec = tp.tv_sec;
      base_usec = tp.tv_usec;
    }

  time = (double) (tp.tv_sec - base_sec);
  mic = (double) (tp.tv_usec - base_usec);
  time = (time + mic * mega) - *t0;
  return(time);
}

using namespace std;
double time_loglik=0.0, clock_zero=0.0, time_inverse=0.0, time_matriceYP=0.0, time_matC=0.0,time_poly=0.0,time_loop1=0.0;

int save_mat(int n, int m, double **A, char* name)
{
	int i,j;
	ofstream ofs(name);
	for (i=0;i<n;i++)
		{for (j=0;j<m;j++) ofs<< A[i][j]<<"  ";
		ofs << "\n";
		}
	ofs.close();
	return 0;	
}

int save_vec(int n, double *V, char* name)
{
	int i;
	ofstream ofs(name);
	for (i=0;i<n;i++) ofs<<V[i]<<"\n";
	ofs.close();
	return 0;
}

/*
void invert(int n, double **A, double **C)
{
  int i,j,k;
  double **L,sum

    L= new double[n];for (i=0;i<n;i++) L[i]=new double[n];
  for (i=0;i<n;i++) {for (j=0;j<n;j++) L[i][j]=0.0;}
  for (j=0;j<n;j++)
    {for (i=j;i<n;i++)
	{if (i==j) 
	    {sum=A[j,j];
	    for (k=0;k<i;k++) sum-=L[i][k];
	    L[i,j]=sqrt(sum);
	    }else
	    {sum=A[i][j];
	      for (k=0;k<i


  for (i=0;i<n;i++)
}
*/
void InvdiagMat(int NbElement, double **Mat, double **C)
{
  //float ** temp;
	int i;
	//temp=new float*[NbElement];

	for(i=0;i<NbElement;i++)
		{
		  //temp[i]=new float [NbElement];
		for(int j=0;j<NbElement;j++)
			{
			C[i][j]=0;
			}
		}

	for(i=0;i<NbElement;i++)
		{
		for(int j=0;j<NbElement;j++)
			{
			C[i][i]=1/Mat[i][i];
			if(j!=i)
				{
				C[i][j]=-Mat[i][j]/Mat[i][i];
				}
			for(int k=0;k<NbElement;k++)
				{
				if(k!=i)
					{
					C[k][i]=Mat[k][i]/Mat[i][i];
					}
				if(j!=i &&k!=i)
					{
					C[k][j]=Mat[k][j]-Mat[i][j]*Mat[k][i]/Mat[i][i];
					}
				}
			}
		}
}

int inverse(int n, double **A, double **C)
{
	int i,j,k,l,err=0;
	double max,pivot,coef, **T;
	double debut,duree;
	debut=walltime(&clock_zero);
    T = new double*[n];for (i=0;i<n;i++) T[i]= new double[2*n];	

	for (i=0;i<n;i++) 
		{for (j=0;j<n;j++) 
			{T[i][j]=A[i][j];
			 if(i==j) T[i][j+n]=1.0; else T[i][j+n]=0.0;
			}
		}
	k=0;
	while ((err==0)&&(k<n))
		{max=fabs(T[k][k]);
		 l=k;
		 for (i=k+1;i<n;i++)
			{if (max<fabs(T[i][k])) {max=fabs(T[i][k]);l=i;}
			}
		 if (max!=0.0)
			{for (j=k;j<2*n;j++)
				{pivot=T[k][j];
				 T[k][j]=T[l][j];
				 T[l][j]=pivot;
				}
			 pivot=T[k][k];
			 if (pivot!=0.0)
				{
				 for (j=k+1;j<2*n;j++) T[k][j]/=pivot;
#pragma omp parallel for private(coef,j)
				 for (i=0;i<n;i++)
					{if (i!=k) 
						{coef=T[i][k];
						 for (j=k+1;j<2*n;j++) T[i][j]-=coef*T[k][j];
						}
					}
				} else err=3;
			} else err=4;
		 k++;	
		}
	if (err==0)
		{for (i=0;i<n;i++)
			{for (j=0;j<n;j++) C[i][j]=T[i][j+n];}
		}
	for (i=0;i<n;i++) delete[] T[i];delete[] T;
	duree=walltime(&debut);
	time_inverse += duree;
	return err;	
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
	double debut,duree;
	
	debut=walltime(&clock_zero);
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
	duree=walltime(&debut);
	time_matriceYP += duree;
}

bool cal_loglik(int nli, int nmodel, int rep, double *loglik, double **matY, double **matP, double *vecW, double *smatY, double *smatP)
{
	int i,imod;
	double a,llik;
	double debut,duree;
	llik=0.0;
	debut=walltime(&clock_zero);
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
	duree=walltime(&debut);
	//	fprintf(stdout,"duration log_lik = %.3f seconde(s)\n",duree);
	time_loglik += duree;
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
	
	debut=walltime(&clock_zero);
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
	save_mat(nli,nco+1,matX0,"matX0.txt");
	for (imod=0;imod<nmodel;imod++) 
		{imody=imod+1;
		 for (i=0;i<nli;i++) {if(fabs(vecY[i]-imody)<0.1) matY[i][imod]=1.0; else matY[i][imod]=0.0;}
		}
	for (i=0;i<nli;i++) {smatY[i]=0.0; for (j=0;j<nmodel;j++)smatY[i]+=matY[i][j];}
	for (i=0;i<nli;i++) {for (j=0;j<nco+1;j++) matXT[j][i]=matX[i][j];}  //transposition
	for (i=0;i<nmodnco;i++) {beta0[i]=0.0;beta[i]=0.0;}
	rep=0;fin=false;
	time_t sum_time=0;
	while (fin==false)
		{rep++;
		  remplimatriceYP(nli,nco,nmodel,matP,matYP,beta,matX,vecW,matY,smatP);
		 for (i=0;i<nmodnco;i++) {for (j=0;j<nmodnco;j++) matC[i][j]=0.0;}
		 de1=walltime(&clock_zero);
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
		 du1=walltime(&de1);
		 time_loop1 += du1;
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
	
	duree=walltime(&debut);
	time_poly += duree;
	fprintf(stdout,"total time in loop 1  = %.6f \n",time_loop1);
	fprintf(stdout,"total time in log_lik = %.6f \n",time_loglik);
	fprintf(stdout,"total time in inverse = %.6f \n",time_inverse);
	fprintf(stdout,"total time in matriYP = %.6f \n",time_matriceYP);
	fprintf(stdout,"total time in polytom = %.6f \n",time_poly);
	return 0;	
		
}
