/** 
* libere la mémoire occupée par une matrice de doubles 
*/
void libereM(int n,double **A) {
    for (int i=0;i<n;i++) delete[] A[i];
    delete [] A;
}

/** 
* écrit les n premières lignes et j premières colonnes d'une matrice de doubles' 
*/
void ecrimat(string nomat, int n, int m, double **A) {
    cout<<"\n"<<nomat<<"\n";
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) cout<< A[i][j]<<"  ";
        cout<<"\n";
    }
    cout<<"\n";
}


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
  double** invM(int n, double **A, bool multithread)
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

int inverse(int n, double **A, double **C)
{
    int i,j,k,l,err=0;
    double max,pivot,coef, **T;
    double debut,duree;
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
    return err; 
}

  
