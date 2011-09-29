#ifndef IOMANIP
#include <iomanip>
#define IOMANIP
#endif

extern bool multithread;

/** 
* libere la mémoire occupée par une matrice de doubles 
*/
void libereM(int n,double **A) {
    for (int i=0;i<n;i++) delete[] A[i];
    delete [] A;
}
/*
/** 
* écrit les n premières lignes et m premières colonnes d'une matrice de doubles' 
*/
/*int ecrimat(string nomat, int n, int m, double **A) {
    cout<<"\n"<<nomat<<"\n";
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) cout<<setiosflags(ios::fixed)<<setw(10)<<setprecision(6)<< A[i][j]<<"  ";
        cout<<"\n";
    }
    cout<<"\n";
	return 0;
}*/


/** 
* transpose une matrice de doubles 
*/
double** transpose(int n, int m, double **A) {
    double **C;
    C = new double*[m];
    for (int i=0;i<m;i++) C[i] = new double[n];
    for (int i=0;i<m;i++) {
        for (int j=0;j<n;j++) C[i][j] = A[j][i];
    }
    return C;
}

/** 
* effectue le produit matriciel de deux matrices de doubles 
*/
double** prodM(int n, int m, int p, double **A,double **B) {
    double **C;
    C = new double*[n];
    for (int i=0;i<n;i++) {
        C[i] = new double[p];
        for(int j=0;j<p;j++) C[i][j]=0.0;
    }
    for (int i=0;i<n;i++) {
        for (int j=0;j<p;j++) {
            for (int k=0;k<m;k++) C[i][j] += A[i][k]*B[k][j];
        }  
    }
    return C;
}

/**
*effectue le produit d'une matrice (**double) par un scalaire (double)'
*/
double** prodMs(int n,int m, double **A, double b) {
    double **C;
    C = new double*[n];
    for (int i=0;i<n;i++) {
        C[i] = new double[m];
        for(int j=0;j<m;j++) C[i][j]=A[i][j]*b;
    }
    return C;
}

/** 
* effectue l'inversion matricielle d'une matrice (carrée) de doubles 
*/
  double** invM(int n, double **A)
{
    int i,j,k,l,err=0;
    double max,pivot,coef, **T, **C;
    double debut,duree; 
    T = new double*[n];for (i=0;i<n;i++) T[i]= new double[2*n]; 
    C = new double*[n];for (i=0;i<n;i++) C[i]= new double[n];
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
#pragma omp parallel for private(coef,j) if(multithread)
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
    return C; 
}

/** 
* effectue l'inversion matricielle d'une matrice (carrée) de doubles 
*/
int inverse(int n, long double **A, long double **C)
{
    int i,j,k,l,err=0;
    long double max,pivot,coef, **T;
    double debut,duree;
    T = new long double*[n];for (i=0;i<n;i++) T[i]= new long double[2*n]; 

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
    if (err==4) {
		std::cout<<"k+1="<<k+1<<"      n="<<n<<"\n";
		for  (i=k+1;i<n;i++) std::cout<<"T["<<i<<"]["<<k<<"]="<<T[i][k]<<"\n";
		exit(1);
	}
    for (i=0;i<n;i++) delete[] T[i];delete[] T;
    return err; 
}


    int jacobi(int n, double **A, double *D, double **V) {
        char bidon;
        int nrot=0,ng=0;
        double *b,*z,tresh,theta,tau,t,sm,s,h,g,cc;
        b = new double[n];
        z = new double[n];
        for (int ip=0;ip<n;ip++) {for (int iq=0;iq<n;iq++) V[ip][iq]=0.0; V[ip][ip]=1.0;}
        for (int ip=0;ip<n;ip++) {b[ip]=A[ip][ip];D[ip]=b[ip];z[ip]=0.0;}
        std::cout<<"matrice A dans jacobi  n="<<n<<"\n"; 
		for (int i=0;i<10;i++) {
		  for (int j=0;j<10;j++) std::cout<< A[i][j]<<"  ";
		  std::cout<<"\n";
		}
		std::cout<<"\n";
        
        for (int i=0;i<51;i++) {
            sm=0.0;
            for (int ip=0;ip<n-1;ip++) {for (int iq=ip+1;iq<n;iq++) sm += fabs(A[ip][iq]);}
            std::cout<<"jacobi sm="<<sm<<"\n";
            if (sm==0.0) {delete []b;delete []z;return nrot;}
            if (i<4) tresh=0.2*sm/(double)n/(double)n; else tresh=0.0;
            for (int ip=0;ip<n-1;ip++) {
                for (int iq=ip+1;iq<n;iq++) {
                    g=100.0*fabs(A[ip][iq]);
                    if (((i>4)and(fabs(D[ip]+g)==fabs(D[ip])))and(fabs(D[iq]+g)==fabs(D[iq]))) A[ip][iq]=0.0;
                    else if (fabs(A[ip][iq])>tresh) {
                        h=D[iq]-D[ip];
                        if (fabs(h)+g == fabs(h)) t=A[ip][iq]/h;
                        else  {
                            theta=0.5*h/A[ip][iq];
                            t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
                            if (theta<0.0) t=-t;
                        }
                        cc=1.0/sqrt(1+t*t);
                        s=t*cc;
                        tau=s/(1.0+cc);
                        h=t*A[ip][iq];
                        z[ip] -=h;
                        z[iq] +=h;
                        D[ip] -=h;
                        D[iq] +=h;
                        A[ip][iq]=0.0;
                        for (int j=0;j<ip;j++) {
                            g=A[j][ip];
                            h=A[j][iq];
                            A[j][ip] = g-s*(h+g*tau);
                            A[j][iq] = h+s*(g-h*tau);
                        }
                        for (int j=ip+1;j<iq;j++) {
                            g=A[ip][j];
                            h=A[j][iq];
                            A[ip][j] = g-s*(h+g*tau);
                            A[j][iq] = h+s*(g-h*tau);
                        }
                       for (int j=iq+1;j<n;j++) {
                            g=A[ip][j];  
                            h=A[iq][j];  
                            A[ip][j] = g-s*(h+g*tau);   
                            A[iq][j] = h+s*(g-h*tau);   
                        }
                        for (int j=0;j<n;j++) {
                            g=V[j][ip];
                            h=V[j][iq];
                            V[j][ip] = g-s*(h+g*tau);
                            V[j][iq] = h+s*(g-h*tau);
                        }
                    }
                }  
            } 
            nrot++;
        }
        for (int ip=0;ip<n-1;ip++) {
            b[ip]=b[ip]+z[ip];
            D[ip]=b[ip];
            z[ip]=0.0;
        }
        delete []b;delete []z;
        return nrot;    
    }

  
