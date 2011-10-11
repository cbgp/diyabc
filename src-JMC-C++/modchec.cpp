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
extern char  *progressfilename;

long double **ssphistar,**ssref;

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

        for (gr=1;gr<=header.ngroupes;gr++) {delete []header.groupe[gr].sumstat;header.groupe[gr].sumstat = new StatC[header.groupe[gr].nstat];}
        ss =splitwords(s," ",&ns);
        delete []header.statname;header.statname = new string[ns];
        for (int i=0;i<ns;i++) header.statname[i]=ss[i];
        for (int i=0;i<ns;i++) {
            qq=splitwords(ss[i],"_",&nq);
            gr=atoi(qq[1].c_str());
            j=0;while (qq[0]!=stat_type[j]) j++;
            //cout<<"qq[0] = "<<qq[0]<<"   j="<<j<<"\n";
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

    string pseudoprior2(long double x) {
        string spr;
        long double mini=0.99999*x,maxi=1.00001*x;
        spr="UN["+LongDoubleToString(mini)+","+LongDoubleToString(maxi)+",0.0,0.0";
        return spr;
    }

    int detphistarOK(int nsel,long double **phistar,long double **phistarOK) {
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

    void call_loc(int npart,int nstat, int nrec,int nsel,long double **ss, float *stat_obs) {
        int *avant,*apres,*egal;
        long double *qobs,quant; float diff;
        string *star;
        qobs = new long double[header.nstat];
        star = new string[header.nstat];
        avant = new int[header.nstat];for (int i=0;i<header.nstat;i++) avant[i] = 0;
        apres = new int[header.nstat];for (int i=0;i<header.nstat;i++) apres[i] = 0;
        egal  = new int[header.nstat];for (int i=0;i<header.nstat;i++) egal[i] = 0;
        for (int p=0;p<npart;p++) {
            for (int j=0;j<header.nstat;j++) {
                diff=stat_obs[j]-(float)ss[p][j];
                if (diff>0.001) avant[j]++;
                else if (diff<-0.001) apres[j]++; else egal[j]++;
            }
        }
        for (int j=0;j<header.nstat;j++) {
              qobs[j] = (long double)(avant[j]+apres[j]+egal[j]);
              if (qobs[j]>0.0) qobs[j] = (0.5*(long double)egal[j]+(long double)avant[j])/qobs[j]; else qobs[j]=-1;
              star[j]="      ";
              if ((qobs[j]>0.95)or(qobs[j]<0.05)) star[j]=" (*)  ";
              if ((qobs[j]>0.99)or(qobs[j]<0.01)) star[j]=" (**) ";
              if ((qobs[j]>0.999)or(qobs[j]<0.001)) star[j]=" (***)";
              cout<<setiosflags(ios::left)<<setw(15)<<header.statname[j]<<"    ("<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<stat_obs[j]<<")   ";
              cout<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<qobs[j]<<star[j]<<"  ";
              cout<<"\n";
        }
        char *nomfiloc;
        nomfiloc = new char[strlen(path)+strlen(ident)+20];
        strcpy(nomfiloc,path);
        strcat(nomfiloc,ident);
        strcat(nomfiloc,"_mclocate.txt");
        cout <<nomfiloc<<"\n";
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        ofstream f12(nomfiloc,ios::out);
        f12<<"DIYABC :                   POSTERIOR CHECKING                         "<<asctime(timeinfo)<<"\n";
        f12<<"Data file       : "<<header.datafilename<<"\n";
        f12<<"Reference table : "<<rt.filename<<"\n";
        f12<<"Chosen scenario : "<<rt.scenteste<<"\n";
        f12<<"Number of simulated data sets used to compute posterior    : "<<nrec<<"\n";
        f12<<"Number of simulated data sets used in the local regression : "<<nsel<<"\n";
        f12<<"Number of data sets simulated from the posterior           : "<<npart<<"\n";
        switch (numtransf) {
          case 1 : f12 << "No transformation of parameters\n\n";break;
          case 2 : f12 <<"Transformation of parameters : Log\n\n";break;
          case 3 : f12 <<"Transformation of parameters : Logit\n\n";break;
          case 4 : f12 <<"Transformation of parameters : Log(tg)\n\n";break;
        }

        f12<<" summary           observed       proportion   \n";
        f12<<"statistics           value    (simulated<observed)\n";
        for (int j=0;j<header.nstat;j++) {
             f12<<setiosflags(ios::left)<<setw(15)<<header.statname[j]<<"     "<<setiosflags(ios::fixed)<<setw(8)<<setprecision(4)<<stat_obs[j]<<"       ";
             f12<<setiosflags(ios::fixed)<<setw(6)<<setprecision(4)<<qobs[j]<<star[j]<<"   ";
             f12<<"\n";
        }
        f12.close();


    }

    void call_acp(int nr, int ns, int nstat, int *numscen,long double **ssref, long double **ssphistar, float *stat_obs) {
        resACPC rACP;
        long double *pca_statobs,**pca_ss;
        pca_statobs = new long double[nstat];
        pca_ss = new long double*[ns];
        for (int i=0;i<ns;i++) pca_ss[i] = new long double[nstat];
        rACP = ACP(nr,nstat,ssref,1.0,0);
        //for (int k=0;k<nstat;k++) cout<<stat_obs[k]<<"   "<<rACP.moy[k]<<"   "<<rACP.sd[k]<<"\n";
        for (int j=0;j<rACP.nlambda;j++) {
            pca_statobs[j]=0.0;
            for(int k=0;k<nstat;k++) if (rACP.sd[k]>0) pca_statobs[j] +=(stat_obs[k]-rACP.moy[k])/rACP.sd[k]*rACP.vectprop[k][j];
            cout<<"  "<<pca_statobs[j];
        }
        cout<<"\n";
        for (int i=0;i<ns;i++) {
            for (int j=0;j<rACP.nlambda;j++){
                pca_ss[i][j]=0.0;
                for(int k=0;k<nstat;k++) if (rACP.sd[k]>0) pca_ss[i][j] +=(ssphistar[i][k]-rACP.moy[k])/rACP.sd[k]*rACP.vectprop[k][j];
            }
        }
        char *nomfiACP;
        nomfiACP = new char[strlen(path)+strlen(ident)+20];
        strcpy(nomfiACP,path);
        strcat(nomfiACP,ident);
        strcat(nomfiACP,"_mcACP.txt");
        cout <<nomfiACP<<"\n";
        FILE *f1;
        f1=fopen(nomfiACP,"w");
        fprintf(f1,"%d %d",nr,rACP.nlambda);
        for (int i=0;i<rACP.nlambda;i++) fprintf(f1," %5.3Lf",rACP.lambda[i]/rACP.slambda);fprintf(f1,"\n");
        fprintf(f1,"%d",0);
        for (int i=0;i<rACP.nlambda;i++) fprintf(f1," %5.3Lf",pca_statobs[i]);fprintf(f1,"\n");
        for (int i=0;i<nr;i++){
            fprintf(f1,"%d",numscen[i]);
            for (int j=0;j<rACP.nlambda;j++) fprintf(f1," %5.3Lf",rACP.princomp[i][j]);fprintf(f1,"\n");
        }
        for (int i=0;i<ns;i++){
            fprintf(f1,"%d",rt.scenteste);
            for (int j=0;j<rACP.nlambda;j++) fprintf(f1," %5.3Lf",pca_ss[i][j]);fprintf(f1,"\n");
        }
        fclose(f1);
    }

    void domodchec(char *options,int seed){
        int nstatOK, iprog,nprog;
        int nrec,nsel,ns,ns1,nrecpos,newsspart,npv,nphistarOK,nss,nsr,newrefpart,*numscen,nparamax,bidon;
        string opt,*ss,s,*ss1,s0,s1,snewstat;
        float  *stat_obs;
        bool usestats,firsttime,dopca,doloc,newstat=false;

        FILE *flog;

        progressfilename = new char[strlen(path)+strlen(ident)+20];
        strcpy(progressfilename,path);
        strcat(progressfilename,ident);
        strcat(progressfilename,"_progress.txt");
        cout<<"debut domodchec  options : "<<options<<"\n";
        opt=char2string(options);
        ss = splitwords(opt,";",&ns);
        numtransf=3;
        for (int i=0;i<ns;i++) { cout<<ss[i]<<"\n";
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);
            if (s0=="s:") {
                ss1 = splitwords(s1,",",&rt.nscenchoisi);
                if (rt.nscenchoisi>1) rt.nscenchoisi=1;
                rt.scenchoisi = new int[rt.nscenchoisi];
                for (int j=0;j<rt.nscenchoisi;j++) rt.scenchoisi[j] = atoi(ss1[j].c_str());
                nrecpos=rt.nrecscen[rt.scenchoisi[0]-1];
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
                snewstat=s1;
                newstat = (s1.length()>0);
            } else if (s0=="q:") {
                newsspart=atoi(s1.c_str());
                cout<<"nombre de particules à simuler à partir du posterior = "<<newsspart<<"\n";
            } else if (s0=="a:") {
                dopca=(s1.find("p")!=string::npos);
                doloc=(s1.find("l")!=string::npos);
                if (dopca) cout <<"Perform ACP  ";
                if ((s1=="pl")or(s1=="lp")) cout <<"et ";
                if (doloc) cout<<"locate  ";
                cout<< "\n";

            }
        }
        nprog=newsspart+100;
        if ((newstat)and(dopca)) nprog += header.nscenarios*10000;
        iprog=10;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        original=true;composite=false;
        nstatOK = rt.cal_varstat();                       cout<<"apres cal_varstat\n";
        header.calstatobs(statobsfilename);stat_obs = header.stat_obs;  cout<<"apres read_statobs\n";
        cout<<"nrec="<<nrec<<"     nsel="<<nsel<<"\n";
        rt.alloue_enrsel(nsel);
        rt.cal_dist(nrec,nsel,stat_obs);                  cout<<"apres cal_dist\n";
        iprog+=40;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        det_numpar();                                     cout<<"apres det_numpar\n";
        recalparam(nsel);                                 cout<<"apres recalparam\n";
        rempli_mat(nsel,stat_obs);                        cout<<"apres rempli_mat\n";
        local_regression(nsel);                           cout<<"apres local_regression\n";
        iprog+=20;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        phistar = calphistar(nsel);                       cout<<"apres calphistar\n";
        det_nomparam();
        savephistar(nsel,path,ident);                     cout<<"apres savephistar\n";
        iprog+=20;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        //phistarOK = new double*[nsel];
        //for (int i=0;i<nsel;i++) phistarOK[i] = new double[header.scenario[rt.scenteste-1].nparam];
        //nphistarOK=detphistarOK(nsel,phistar,phistarOK);
        //cout<<"naparamcom="<<nparamcom<<"   nparcompo="<<nparcompo<<"   nenr="<<nenr<<"\n";
        npv = rt.nparam[rt.scenteste-1];
        enreg = new enregC[nenr];
        for (int p=0;p<nenr;p++) {
            enreg[p].stat = new float[header.nstat];
            enreg[p].param = new float[npv];
            enreg[p].numscen = rt.scenteste;
        }
        nss=0;
        firsttime=true;
        cout<<"ns="<<ns<<"\n";
        //cout<<phistarOK[0][0]<<"\n";
        if (newstat) usestats = resetstats(snewstat);

        cout<<"header.nstat = "<<header.nstat<<"\n";
        ssphistar = new long double*[newsspart];
        for (int i=0;i<newsspart;i++) ssphistar[i] = new long double[header.nstat];
        cout<<"newstat ="<<newstat<<"   newsspart="<<newsspart<<"     nenr="<<nenr<<"\n";
		
        while (nss<newsspart) {
            ps.dosimulphistar(header,nenr,false,multithread,firsttime,rt.scenteste,seed,nsel);
            for (int i=0;i<nenr;i++) {
                for (int j=0;j<header.nstat;j++) ssphistar[i+nss][j]=enreg[i].stat[j];
                for (int j=0;j<header.nstat;j++) cout<<ssphistar[i+ns][j]<<"   ";cout<<"\n";
            }
            firsttime=false;
            nss+=nenr;
            iprog+=nenr;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
            cout<<nss<<"\n";
        }
        if (newstat) {header.calstatobs(statobsfilename);stat_obs = header.stat_obs;}
        if (doloc) call_loc(newsspart,header.nstat,nrec,nsel,ssphistar,stat_obs);
        if (dopca) {
            if (newstat) {
                header.readHeader(headerfilename);cout<<"apres readHeader nscenarios= "<<header.nscenarios<<"\n";
                usestats = resetstats(snewstat);
                newrefpart= header.nscenarios*10000; cout<<"newrefparts="<<newrefpart<<"\n";
                numscen = new int[newrefpart];
                ssref = new long double*[newrefpart];
                for (int i=0;i<newrefpart;i++) ssref[i] = new long double[header.nstat];
                nsr=0;
                firsttime=true;
                while (nsr<newrefpart) {
                    ps.dosimultabref(header,nenr,false,multithread,firsttime,0,seed);
                    for (int i=0;i<nenr;i++) {
                      numscen[i+nsr] = enreg[i].numscen;
                      for (int j=0;j<header.nstat;j++) ssref[i+nsr][j]=enreg[i].stat[j];
                        //for (int j=0;j<header.nstat;j++) cout<<ssref[i+nsr][j]<<"   ";cout<<"\n";
                    }
                    firsttime=false;
                    nsr+=nenr;
                    iprog+=nenr;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
                    cout<<nsr<<"\n";
                }
            } else {
                rt.openfile2();
                enregC enr;
                nparamax=0;for (int i=0;i<rt.nscen;i++)  if (rt.nparam[i]>nparamax) nparamax=rt.nparam[i];
                enr.stat = new float[rt.nstat];
                enr.param = new float[nparamax];
                newrefpart= header.nscenarios*10000; cout<<"newrefparts="<<newrefpart<<"\n";
                numscen = new int[newrefpart];
                ssref = new long double*[newrefpart];
                for (int i=0;i<newrefpart;i++) ssref[i] = new long double[header.nstat];
                nsr=0;
                while (nsr<newrefpart) {
                    bidon = rt.readrecord(&enr);
                    numscen[nsr] = enr.numscen;
                    for (int j=0;j<rt.nstat;j++) ssref[nsr][j]=enr.stat[j];
                    nsr++;
                }
            }
            call_acp(newrefpart,newsspart,header.nstat,numscen,ssref,ssphistar,stat_obs);
        }
        iprog+=10;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
    }
