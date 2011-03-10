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

enregC *enr;


using namespace std;

    int cal_varstat(char* reftablefilename,double *var_stat) {
      
        
        return 0;
    }












    void doestim(char *headerfilename,char *reftablefilename,char *statobsfilename,char *options) {
        char *datafilename;
        HeaderC header;
        ReftableC rt;
        double *var_stat;
        int rtOK,nstatOK;
        int nscenchoisi,*scenchoisi,nrec,nsel,numtransf,ns,ns1;
        bool original,composite;
        string opt,*ss,s,*ss1,s0,s1;
        
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
        exit(1);
        header.readHeader(headerfilename);
        datafilename=strdup(header.datafilename.c_str());
        rtOK=rt.readheader(reftablefilename,header.nscenarios,datafilename);
        if (rtOK==1) {cout <<"no file reftable.bin in the current directory\n";exit(1);}          
        
        
        nstatOK = cal_varstat(reftablefilename,var_stat);
    
    
    
    }