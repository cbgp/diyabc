/*
 * modchec.cpp
 *
 *  Created on: 4 april 2011
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

extern ParticleSetC ps;
extern enregC* enreg;
extern int nenr;

double **ssphistar;

    bool resetstats(string s) {
      cout<<s<<"\n";
        int j,ns,nq,nss1,gr,k=0;
        string *ss,*qq,*ss1;
//comptage des stat        
        for (gr=1;gr<=header.ngroupes;gr++) {header.groupe[gr].nstat=0;} header.nstat=0;
        //cout<<header.ngroupes<<" groupe\n";
        ss =splitwords(s," ",&ns);
        cout<<"ns="<<ns<<"\n";
        for (int i=0;i<ns;i++) { 
            qq=splitwords(ss[j],"_",&nq);
            if (nq!=3) return false;
            //cout<<"nq="<<nq<<"   qq[1]="<<qq[1]<<"\n";
            gr=atoi(qq[1].c_str());
            //cout<<"gr="<<gr<<"\n";
            header.groupe[gr].nstat++;
            header.nstat++;
            delete []qq;
       } 
        //cout <<"dans resetstat nstat = "<<header.nstat++<<"\n";
        for (gr=1;gr<=header.ngroupes;gr++) header.groupe[gr].sumstat = new StatC[header.groupe[gr].nstat];     
        ss =splitwords(s," ",&ns);
        for (int i=0;i<ns;i++) { 
            qq=splitwords(ss[i],"_",&nq);
            gr=atoi(qq[1].c_str());
            j=0;while (qq[0]!=stat_type[j]) j++;
            cout<<"qq[0] = "<<qq[0]<<"   j="<<j<<"\n";
            if (header.groupe[gr].type==0) {   //MICROSAT
                if (stat_num[j]<5) {
                      header.groupe[gr].sumstat[k].cat=stat_num[j];
                      header.groupe[gr].sumstat[k].samp=atoi(qq[2].c_str());
                      k++;
                } else if (stat_num[j]<12) {
                      header.groupe[gr].sumstat[k].cat=stat_num[j];
                      ss1=splitwords(qq[2],"&",&nss1);
                      header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                      header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                      k++;
                      delete [] ss1;
                } else if (stat_num[j]==12) {
                      header.groupe[gr].sumstat[k].cat=stat_num[j];
                      ss1=splitwords(qq[2],"&",&nss1);
                      header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                      header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                      header.groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
                      k++;
                      delete [] ss1;
                }
            } else if (header.groupe[gr].type==1) {   //DNA SEQUENCE
                if (stat_num[j]>-5) {
                      header.groupe[gr].sumstat[k].cat=stat_num[j];
                      header.groupe[gr].sumstat[k].samp=atoi(qq[2].c_str());
                      k++;
                } else if (stat_num[j]>-14) {
                        header.groupe[gr].sumstat[k].cat=stat_num[j];
                        ss1=splitwords(qq[2],"&",&nss1);
                        header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                        header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                        k++;
                        delete [] ss1;
                } else if (stat_num[j]==-14) {
                        header.groupe[gr].sumstat[k].cat=stat_num[j];
                        ss1=splitwords(qq[2],"&",&nss1);
                        header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                        header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                        header.groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
                        k++;
                        delete [] ss1;
                }
            }
            delete []qq;
        }
        return true;
    }

    string pseudoprior2(double x) {
        string spr;
        double mini=0.99999*x,maxi=1.00001*x;
        spr="UN["+DoubleToString(mini)+","+DoubleToString(maxi)+",0.0,0.0";
        return spr;
    }

    int detphistarOK(int nsel,double **phistar,double **phistarOK) {
        MwcGen MWC;
        bool OK;
        int k,npv,ip1,ip2,nphistarOK=0,scen=rt.scenteste-1;
        npv = rt.nparam[scen];
       for (int i=0;i<nsel;i++) {
            OK=true;
            if (header.scenario[scen].nconditions>0) {
                for (int j=0;j<header.scenario[scen].nconditions;j++) {
                    ip1=0;while (header.scenario[scen].condition [j].param1!=header.scenario[scen].histparam[ip1].name) ip1++;
                    ip2=0;while (header.scenario[scen].condition [j].param2!=header.scenario[scen].histparam[ip2].name) ip2++;
                    if (header.scenario[scen].condition[j].operateur==">")       OK=(phistar[i][ip1] >  phistar[i][ip2]);
                    else if (header.scenario[scen].condition[j].operateur=="<")  OK=(phistar[i][ip1] <  phistar[i][ip2]);
                    else if (header.scenario[scen].condition[j].operateur==">=") OK=(phistar[i][ip1] >= phistar[i][ip2]);
                    else if (header.scenario[scen].condition[j].operateur=="<=") OK=(phistar[i][ip1] <= phistar[i][ip2]);
                    if (not OK) break;
                }
            }
            if (OK) { cout<<nphistarOK<<"    ";
                for (int j=0;j<npv;j++) {
                    phistarOK[nphistarOK][j] = phistar[i][j];
                    cout <<phistarOK[nphistarOK][j]<<"  ";               
                } 
                cout<<"\n";
                nphistarOK++;
            }
        }
        cout<<"nphistarOK="<<nphistarOK<<"\n";
        return nphistarOK;
    }

    void domodchec(char *options,int seed){
        char  *progressfilename;
        int nstatOK, iprog,nprog;
        int nrec,nsel,ns,ns1,nrecpos,newpart,npv,nphistarOK;
        string opt,*ss,s,*ss1,s0,s1,newstat;
        double  *stat_obs;
        bool usestats,firsttime,dopca,doloc;
        
        FILE *flog;
        
        progressfilename = new char[strlen(path)+strlen(ident)+20];
        strcpy(progressfilename,path);
        strcat(progressfilename,ident);
        strcat(progressfilename,"_progress.txt");
        cout<<"debut domodchec  options : "<<options<<"\n";
        opt=char2string(options);
        ss = splitwords(opt,";",&ns);
        for (int i=0;i<ns;i++) { //cout<<ss[i]<<"\n";
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);
            if (s0=="s:") {
                ss1 = splitwords(s1,",",&rt.nscenchoisi);
                rt.scenchoisi = new int[rt.nscenchoisi];
                for (int j=0;j<rt.nscenchoisi;j++) rt.scenchoisi[j] = atoi(ss1[j].c_str());
                nrecpos=0;for (int j=0;j<rt.nscenchoisi;j++) nrecpos +=rt.nrecscen[rt.scenchoisi[j]-1];
                cout <<"scenario choisi : "<<rt.scenchoisi[0]<<"\n";
                rt.scenteste = rt.scenchoisi[0]; 
            } else if (s0=="n:") {
                nrec=atoi(s1.c_str());
                if(nrec>nrecpos) nrec=nrecpos;
                cout<<"nombre total de jeux de données considérés (pour le scénario choisi )= "<<nrec<<"\n";
            } else if (s0=="m:") {
                nsel=atoi(s1.c_str());    
                cout<<"nombre de jeux de données considérés pour la régression locale = "<<nsel<<"\n";
            } else if (s0=="t:") {
                numtransf=atoi(s1.c_str()); 
                switch (numtransf) {
                  case 1 : cout <<" pas de transformation des paramètres\n";break;
                  case 2 : cout <<" transformation log des paramètres\n";break;
                  case 3 : cout <<" transformation logit des paramètres\n";break;
                  case 4 : cout <<" transformation log(tg) des paramètres\n";break;
                }
            } else if (s0=="v:") {
                cout<<""<< "\n";
                newstat=s1;
            } else if (s0=="q:") {
                newpart=atoi(s1.c_str());
                cout<<"nombre de particules à simuler à partir du posterior = "<<newpart<<"\n";
            } else if (s0=="a:") {
                dopca=(s1.find("p")!=string::npos);
                doloc=(s1.find("l")!=string::npos);
                if (dopca) cout <<"Perform ACP  ";
                if ((s1=="pl")or(s1=="lp")) cout <<"et ";
                if (doloc) cout<<"locate  ";
                cout<< "\n";
                 
            }           
        }
        original=true;composite=false;
        nstatOK = rt.cal_varstat();                       cout<<"apres cal_varstat\n";
        stat_obs = header.read_statobs(statobsfilename);  cout<<"apres read_statobs\n";
        nprog=100;iprog=1;
        flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        rt.alloue_enrsel(nsel);
        rt.cal_dist(nrec,nsel,stat_obs);                  cout<<"apres cal_dist\n";
        iprog+=8;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        det_numpar();                                     cout<<"apres det_numpar\n";
        nprog=nparamcom*10+14;
        recalparam(nsel);                                 cout<<"apres recalparam\n";
        rempli_mat(nsel,stat_obs);                        cout<<"apres rempli_mat\n";
        local_regression(nsel);               cout<<"apres local_regression\n";
        iprog+=1;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        phistar = calphistar(nsel);                                 cout<<"apres calphistar\n";
        det_nomparam();
        savephistar(nsel,path,ident);                     cout<<"apres savephistar\n";
        //phistarOK = new double*[nsel];
        //for (int i=0;i<nsel;i++) phistarOK[i] = new double[header.scenario[rt.scenteste-1].nparam];
        //nphistarOK=detphistarOK(nsel,phistar,phistarOK);
        cout<<"naparamcom="<<nparamcom<<"   nparcompo="<<nparcompo<<"   nenr="<<nenr<<"\n";
        npv = rt.nparam[rt.scenteste-1];
        enreg = new enregC[nenr];
        for (int p=0;p<nenr;p++) {
            enreg[p].stat = new float[header.nstat];
            enreg[p].param = new float[npv];
            enreg[p].numscen = rt.scenteste;
        }
        ns=0;
        firsttime=true;
        cout<<"ns="<<ns<<"\n";
        //cout<<phistarOK[0][0]<<"\n";
        usestats = resetstats(newstat);
        cout<<"header.nstat = "<<header.nstat<<"\n";
        ssphistar = new double*[newpart];
        for (int i=0;i<newpart;i++) ssphistar[i] = new double[header.nstat];
        while (ns<newpart) {
            ps.dosimulphistar(header,(rt.scenteste-1),nenr,false,multithread,firsttime,rt.scenteste,seed,false,false,nsel);
            for (int i=0;i<nenr;i++) {
                for (int j=0;j<header.nstat;j++) ssphistar[i+ns][j]=enreg[i].stat[j];
                for (int j=0;j<header.nstat;j++) cout<<ssphistar[i+ns][j]<<"   ";cout<<"\n";
            }
            firsttime=false;
            ns+=nenr;
            cout<<ns<<"\n";
        }
    }