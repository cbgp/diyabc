/*
 * acploc.cpp
 *
 *  Created on: 29 march 2011
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

#ifndef MATRICES
#include "matrices.cpp"
#define MATRICES
#endif


using namespace std;
extern enregC* enreg;
int nacp=100000;

struct resACPC
{
   int index,nlambda;      //index=0 pour matrice de corrélation et !=0 pour matrice de covariance
   double proportion,*lambda,slambda,**vectprop,**princomp,*moy,*sd;
   
};

    int jacobi(int n, double **A, double *D, double **V) {
        int nrot=0;
        double *b,*z,tresh,theta,tau,t,sm,s,h,g,cc;
        V = new double*[n];for (int i=0;i<n;i++) V[i]=new double [n];
        D = new double[n];
        b = new double[n];
        z = new double[n];
        for (int ip=0;ip<n;ip++) {for (int iq=0;iq<n;iq++) V[ip][iq]=0.0; V[ip][ip]=1.0;}
        for (int ip=0;ip<n;ip++) {b[ip]=A[ip][ip];D[ip]=b[ip];z[ip]=0.0;}
        for (int i=0;i<51;i++) {
            sm=0.0;
            for (int ip=0;ip<n-1;ip++) {for (int iq=ip+1;iq<n;iq++) sm += abs(A[ip][iq]);}
            if (sm==0.0) {delete []b;delete []z;return nrot;}
            if (i<4) tresh=0.2*sm/(double)n/(double)n; else tresh=0.0;
            for (int ip=0;ip<n-1;ip++) {
                for (int iq=ip+1;iq<n;iq++) {
                    g=100.0*abs(A[ip][iq]);
                    if (((i>4)and(abs(D[ip]+g)==abs(D[ip])))and(abs(D[iq]+g)==abs(D[iq]))) A[ip][iq]=0.0;
                    else if (abs(A[ip][iq])>tresh) {
                        h=D[iq]-D[ip];
                        if (abs(h)+g == abs(h)) t=A[ip][iq]/h;
                        else  {
                            theta=0.5*h/A[ip][iq];
                            t=1.0/(abs(theta)+sqrt(1.0+theta*theta));
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
                        for (int j=0;j<ip-1;j++) {
                            g=A[j][ip];
                            h=A[j][iq];
                            A[j][ip] = g-s*(h+g*tau);
                            A[j][iq] = h+s*(g-h*tau);
                        }
                        for (int j=ip;j<iq-1;j++) {
                            g=A[ip][j];
                            h=A[j][iq];
                            A[ip][j] = g-s*(h+g*tau);
                            A[j][iq] = h+s*(g-h*tau);
                        }
                        for (int j=iq;j<n;j++) {
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
                        nrot++;
                    }
                }  
            }
        }
        for (int ip=0;ip<n-1;ip++) {
            b[ip]=b[ip]+z[ip];
            D[ip]=b[ip];
            z[ip]=0.0;
        }
        delete []b;delete []z;
        return nrot;    
    }

    resACPC ACP(int nli,int nco, double **X,double prop,int index) {
        resACPC res;
        res.proportion = prop;
        res.index = index;
        double **matX,**matXT,**matM,*valprop,**vcprop,**matXTX;
        double *y,anli,piv,sl;
        anli = 1.0/(double)nli;
        y = new double[nli];
        res.moy = new double[nco];
        res.sd  = new double[nco];
        for (int j=0;j<nco;j++) {
            for (int i=0;i<nli;i++) y[i] = X[i][j];
            res.moy[j] = cal_moy(nli,y);
            res.sd[j]  = cal_sd(nli,y)*sqrt((double)(nli-1)/(double(nli)));
        }
        if (index==0) {
            for (int i=0;i<nli;i++) {
                for (int j=0;j<nco;j++){ 
                    if (res.sd[j]>0.0) matX[i][j] = (X[i][j]-res.moy[j])/res.sd[j]; else matX[i][j]=0.0;
                }
            }
        } else {
            for (int i=0;i<nli;i++) {
                for (int j=0;j<nco;j++) matX[i][j] = X[i][j]-res.moy[j];
            }
        }
        matXT = transpose(nli,nco,matX);
        matXTX = prodM(nco,nli,nco,matXT,matX);
        matM = prodMs(nco,nco,matXTX,anli);
        for (int i=0;i<nco;i++) {for (int j=0;j<nco;j++) matM[i][j]=matXTX[i][j]*anli;}
        jacobi(nco,matM,valprop,vcprop);
        for (int i=0;i<nco-1;i++) {
            for (int j=i+1;j<nco;j++) {
                if (valprop[i]<valprop[j]) {
                    piv=valprop[i];valprop[i]=valprop[j];valprop[j]=piv;
                    for (int k=0;k<nco;k++) {
                        piv=vcprop[k][i];vcprop[k][i]=vcprop[k][j];vcprop[k][j]=piv;
                    }
                }
            }
        }
        res.slambda=0.0;
        for (int i=0;i<nco-1;i++) res.slambda +=valprop[i];
        res.nlambda=1;sl=valprop[0];
        while (sl/res.slambda<prop) {sl+=valprop[res.nlambda];res.nlambda++;}
        res.lambda = new double[res.nlambda];
        for (int i=0;i<res.nlambda;i++) res.lambda[i]=valprop[i];
        res.vectprop = new double*[nco];
        for (int i=0;i<nco;i++) {
            res.vectprop[i] = new double[res.nlambda];
            for (int j=0;j<res.nlambda;j++) res.vectprop[i][j] = vcprop[i][j];
        }
        res.princomp = new double*[nli];
        for (int i=0;i<nli;i++) {
            for (int j=0;j<res.nlambda;j++) {
                res.princomp[i][j]=0.0;
                for (int k=0;k<nco;k++) res.princomp[i][j] +=matX[i][k]*res.vectprop[k][j];
            } 
        }  
        delete []valprop;
        for (int i=0;i<nco;i++) delete []vcprop; delete []vcprop;
    }

    void cal_acp(){
        double *stat_obs,**matstat;
        int bidon;
        resACPC rACP;
        stat_obs = header.read_statobs(statobsfilename);  cout<<"apres read_statobs\n";
        int nparamax = 0;
        for (int i=0;i<rt.nscen;i++) if (rt.nparam[i]>nparamax) nparamax=rt.nparam[i];
        cout<<nparamax<<"\n";
        if (nacp>rt.nrec) nacp=rt.nrec;
        enreg = new enregC[nacp];
        rt.openfile2();
        for (int p=0;p<nacp;p++) {
                enreg[p].param = new float[nparamax];
                enreg[p].stat  = new float[rt.nstat];
                bidon=rt.readrecord(&(enreg[p]));
            
        }
        rt.closefile();
        matstat = new double*[nacp];
        for (int i=0;i<nacp;i++) {
            matstat[i] = new double[rt.nstat];
            for (int j=0;j<rt.nstat;j++) matstat[i][j] = enreg[i].stat[j];
        }
        rACP = ACP(nacp,rt.nstat,matstat,1.0,0);
    }

    void doacpl(char *options,bool multithread, int seed){
        string opt,*ss,s,*ss1,s0,s1;
        bool dopca,doloc;
        int ns;
        opt=char2string(options);
        ss = splitwords(opt,";",&ns);
        for (int i=0;i<ns;i++) { //cout<<ss[i]<<"\n";
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);
            if (s0=="a:") {
                dopca=(s1.find("p")!=string::npos);
                doloc=(s1.find("l")!=string::npos);
                if (dopca) cout <<"Perform ACP  ";
                if ((s1=="pl")or(s1=="lp")) cout <<"et ";
                if (doloc) cout<<"locate  ";
                cout<< "\n";
            }            
        }
        if (dopca) cal_acp();
   }