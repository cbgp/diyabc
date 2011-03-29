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


using namespace std;
extern enregC* enreg;
int nacp=100000;

    void cal_acp(){
        double *stat_obs;
        int bidon;
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