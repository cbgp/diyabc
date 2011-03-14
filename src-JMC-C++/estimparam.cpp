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

enregC *enrsel;
ReftableC rt;
HeaderC header;
double *var_stat,*stat_obs;
int nparamax;

using namespace std;

struct compenreg
{
   bool operator() (const enregC & lhs, const enregC & rhs) const
   {
      return lhs.dist < rhs.dist;
   }
};
    int cal_varstat() {
        int nrecutil,iscen,nsOK;
        double *sx,*sx2,x,an;
        enregC enr;
        nrecutil=300000;if (nrecutil>rt.nrec) nrecutil=rt.nrec;
        an=1.0*(double)nrecutil;
        sx  = new double[rt.nstat];
        sx2 = new double[rt.nstat];
        var_stat = new double[rt.nstat];
        for (int j=0;j<rt.nstat;j++) {sx[j]=0.0;sx2[j]=0.0;}
        enr.stat = new float[rt.nstat];
        nparamax = 0;for (int i=0;i<rt.nscen;i++) if (rt.nparam[i]>nparamax) nparamax=rt.nparam[i];
        enr.param = new float[nparamax];
        for (int i=0;i<nrecutil;i++) {
            rt.readrecord(&enr);
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
            //cout<<"var_stat["<<j<<"]="<<var_stat[j]<<"\n";
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

    void cal_dist(int nrec, int nsel, int nscenchoisi,int *scenchoisi) {
        int nn=10000,nreclus=0,nparamax,nrecOK=0,iscen;
        bool firstloop=true,scenOK;
        double diff;
        if (nn<nsel) nn=nsel;
        nparamax = 0;for (int i=0;i<rt.nscen;i++) if (rt.nparam[i]>nparamax) nparamax=rt.nparam[i];
        enrsel = new enregC[2*nn]; 
        while (nreclus<nrec) {
            if (firstloop) {nrecOK=0;firstloop=false;}
            else nrecOK=nn;
            while (nrecOK<2*nn) {
                enrsel[nrecOK].param = new float[nparamax];
                enrsel[nrecOK].stat  = new float[rt.nstat];
                rt.readrecord(&enrsel[nrecOK]);
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
                }
            }
            sort(&enrsel[0],&enrsel[2*nn],compenreg()); 
            cout<<"distmin = "<<enrsel[0].dist/rt.nstat<<"    distmax = "<<enrsel[nsel-1].dist/rt.nstat<<"\n";
        }
    }
    
    void det_numpar(int nscenchoisi,int *scenchoisi,int **numpar) {
        vector <string>  parname;
        int npar=0,ii;
        bool commun,trouve;
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
                }
            }
            cout << "noms des parametres communs : ";
            for (int i=0;i<npar;i++) cout<<parname[i]<<"   ";
            cout <<"\n";
        }     
        for (int j=0;j<nscenchoisi;j++) {
            numpar[j] = new int[npar+rt.nparam[scenchoisi[j]-1]-header.scenario[scenchoisi[j]-1].nparam];
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
    
    
    
    
  /*  void recalparam(int n,int numtransf, **alpsimrat) {
       
    
        alpsimrat = new double*[n];
        for(int i=0;i<n;i++) alpsimrat[i] = new double[rt.]
    
    } */  
    
    


    





    void doestim(char *headerfilename,char *reftablefilename,char *statobsfilename,char *options) {
        char *datafilename;
        int rtOK,nstatOK;
        int nscenchoisi,*scenchoisi,nrec,nsel,numtransf,ns,ns1, **numpar;
        bool original,composite;
        string opt,*ss,s,*ss1,s0,s1;
        double **matX0, *vecW, **alpsimrat;
        
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
        rtOK=rt.readheader(reftablefilename,header.nscenarios,datafilename);
        if (rtOK==1) {cout <<"no file reftable.bin in the current directory\n";exit(1);}          
        rt.openfile2();
        nstatOK = cal_varstat();
        rt.closefile();
        read_statobs(statobsfilename);
        rt.openfile2();
        cal_dist(nrec,nsel,nscenchoisi,scenchoisi);
        det_numpar(nscenchoisi,scenchoisi,numpar);
        //recalparam(nsel,numtransf,alpsimrat);
        //rempli_mat(n,matX0,vecW,parsim)
    
    }