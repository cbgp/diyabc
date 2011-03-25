/*
 * bias.cpp
 *
 *  Created on: 23 march 2011
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


#ifndef PARTICLESET
#include "particleset.cpp"
#define PARTICLESET
#endif

extern ParticleSetC ps;
extern enregC* enreg;

struct enreC {
    int numscen;
    double *paramvv,*stat;
};

parstatC **paramest;
enreC *enreg2;
double **br,rrmise;
double ***paretoil;

using namespace std;

    string pseudoprior(string s) {
        string spr;
        double x=atof(s.c_str());
        double mini=0.99999*x,maxi=1.00001*x;
        spr="UN["+DoubleToString(mini)+","+DoubleToString(maxi)+",0.0,0.0";
        return spr;
    }

    void resethistparam(string s) {
        string *ss,name,sprior,smini,smaxi;
        int n,i;
        double mini,maxi;
        ss = splitwords(s,"=",&n);
        name=ss[0];
        i=0;while((i<header.scenario[rt.scenchoisi[0]-1].nparam)and(name != header.scenario[rt.scenchoisi[0]-1].histparam[i].name)) i++;
        //cout<<"parametre "<<name<<"  ("<<i<<")   "<<ss[1]<<"\n";
        if (ss[1].find("[")!=string::npos) {
            header.scenario[rt.scenchoisi[0]-1].histparam[i].prior = header.readprior(ss[1]);
             header.scenario[rt.scenchoisi[0]-1].histparam[i].prior.ecris();
        }
        else {
            header.scenario[rt.scenchoisi[0]-1].histparam[i].prior = header.readprior(pseudoprior(ss[1]));
            //header.scenario[rt.scenchoisi[0]-1].histparam[i].value = atof(ss[1].c_str()) ;
        }
    }

    void resetmutparam(string s) {
        string *ss,numgr,s1;
        int n,i,gr,i0,i1;
        numgr = s.substr(1,s.find("(")-1);  gr=atoi(numgr.c_str());
        i0=s.find("(");i1=s.find(")"); cout <<"i0="<<i0<<"  i1="<<i1<<"\n";
        s1 = s.substr(i0+1,i1-i0-1); cout <<"groupe "<<gr<<"  "<<s1<<"\n";
        ss = splitwords(s1," ",&n);
        if (header.groupe[gr].type==0) {
            if (ss[0].find("[")==string::npos) header.groupe[gr].priormutmoy =header.readpriormut(pseudoprior(ss[0]));
            else header.groupe[gr].priormutmoy = header.readpriormut(ss[0]);
            if (ss[1].find("[")==string::npos) header.groupe[gr].priormutloc.sdshape=atof(ss[1].c_str());
            else header.groupe[gr].priormutloc = header.readpriormut(ss[1]);
            
            if (ss[2].find("[")==string::npos) header.groupe[gr].priorPmoy=header.readpriormut(pseudoprior(ss[2]));
            else header.groupe[gr].priorPmoy = header.readpriormut(ss[2]);
             if (ss[3].find("[")==string::npos) header.groupe[gr].priorPloc.sdshape=atof(ss[3].c_str());
            else header.groupe[gr].priorPloc = header.readpriormut(ss[3]);
            
            if (ss[4].find("[")==string::npos) header.groupe[gr].priorsnimoy=header.readpriormut(pseudoprior(ss[4]));
            else header.groupe[gr].priorsnimoy = header.readpriormut(ss[4]);
            if (ss[5].find("[")==string::npos) header.groupe[gr].priorsniloc.sdshape=atof(ss[5].c_str());
            else header.groupe[gr].priorsniloc = header.readpriormut(ss[5]);
       } else {
             if (ss[0].find("[")==string::npos) header.groupe[gr].priormusmoy=header.readpriormut(pseudoprior(ss[0]));
            else header.groupe[gr].priormusmoy = header.readpriormut(ss[0]);
            if (ss[1].find("[")==string::npos) header.groupe[gr].priormusloc.sdshape=atof(ss[1].c_str());
            else header.groupe[gr].priormusloc = header.readpriormut(ss[1]);
            
            if (ss[2].find("[")==string::npos) header.groupe[gr].priork1moy=header.readpriormut(pseudoprior(ss[2]));
            else header.groupe[gr].priork1moy = header.readpriormut(ss[2]);
             if (ss[3].find("[")==string::npos) header.groupe[gr].priork1loc.sdshape=atof(ss[3].c_str());
            else header.groupe[gr].priork1loc = header.readpriormut(ss[3]);
            
            if (ss[4].find("[")==string::npos) header.groupe[gr].priork2moy=header.readpriormut(pseudoprior(ss[4]));
            else header.groupe[gr].priork2moy = header.readpriormut(ss[4]);
            if (ss[5].find("[")==string::npos) header.groupe[gr].priork2loc.sdshape=atof(ss[5].c_str());
            else header.groupe[gr].priork2loc = header.readpriormut(ss[5]);
       } 
       cout<<"fin de resetmutparam\n";
    }
    
    void biaisrel(int ntest,int npv) {
        int ntestOK;
        br = new double* [npv];
        for (int j=0;j<npv;j++) {
            br[j] = new double[3];
            br[j][0]=0.0;br[j][1]=0.0;br[j][2]=0.0;ntestOK=0.0;
            for (int i=0;i<ntest;i++) if (enreg2[i].paramvv[j]>0.0) {
                 br[j][0] +=(paramest[i][j].moy-enreg2[i].paramvv[j])/enreg2[i].paramvv[j];
                 br[j][1] +=(paramest[i][j].med-enreg2[i].paramvv[j])/enreg2[i].paramvv[j];
                 br[j][2] +=(paramest[i][j].mod-enreg2[i].paramvv[j])/enreg2[i].paramvv[j];
                 ntestOK++;
            }
            if (ntestOK>0) br[j][0] /= (double)ntestOK; else br[j][0]=-9.0;
            if (ntestOK>0) br[j][1] /= (double)ntestOK; else br[j][1]=-9.0;
            if (ntestOK>0) br[j][2] /= (double)ntestOK; else br[j][2]=-9.0;
        }
        
    }
    
    void ecrires(int ntest,int npv,int nsel) {
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        char *nomfiresult;
        double *x,*mo;
        x=new double[ntest];
        mo=new double[4];
        nomfiresult = new char[strlen(path)+strlen(ident)+20];
        strcpy(nomfiresult,path);
        strcat(nomfiresult,ident);
        strcat(nomfiresult,"_bias.txt");
        cout <<nomfiresult<<"\n";
        FILE *f1;
        f1=fopen(nomfiresult,"w");
        fprintf(f1,"DIYABC :                 Bias and Mean Square Error Analysis                         %s\n",asctime(timeinfo));
        fprintf(f1,"Data file       : %s\n",header.datafilename.c_str());
        fprintf(f1,"Reference table : %s\n",rt.filename);
        switch (numtransf) {
              case 1 : fprintf(f1,"No transformation of parameters\n");break;
              case 2 : fprintf(f1,"Transformation LOG of parameters\n");break;
              case 3 : fprintf(f1,"Transformation LOGIT of parameters\n");break;
              case 4 : fprintf(f1,"Transformation LOG(TG) of parameters\n");break;
        }
        fprintf(f1,"Chosen scenario(s) : ");for (int i=0;i<rt.nscenchoisi;i++) fprintf(f1,"%d ",rt.scenchoisi[i]);fprintf(f1,"\n");
        fprintf(f1,"Number of simulated data sets : %d\n",rt.nreclus);
        fprintf(f1,"Number of selected data sets  : %d\n",nsel);
        fprintf(f1,"Results based on %d test data sets\n\n",ntest);
        fprintf(f1,"                                               Averages\n");
        fprintf(f1,"Parameter                True values           Means             Medians             Modes\n");
        for (int j=0;j<npv;j++) {
            fprintf(f1,"%s",nomparam[j].c_str());
            for (int i=0;i<24-nomparam[j].length();i++)fprintf(f1," ");
            for (int i=0;i<ntest;i++) x[i]=enreg2[i].paramvv[j];mo[0]=cal_moy(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramest[i][j].moy;mo[1]=cal_moy(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramest[i][j].med;mo[2]=cal_moy(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramest[i][j].mod;mo[3]=cal_moy(ntest,x);
            fprintf(f1,"  %8.3e          %8.3e          %8.3e          %8.3e\n",mo[0],mo[1],mo[2],mo[3]);
        }
        fprintf(f1,"\n                                           Mean Relative Bias\n");
        fprintf(f1,"Parameter                           Means          Medians        Modes\n");
        for (int j=0;j<npv;j++) {
            fprintf(f1,"%s",nomparam[j].c_str());
            for(int i=0;i<33-nomparam[j].length();i++)fprintf(f1," ");
            if (br[j][0]<0) fprintf(f1,"  %5.3f",br[j][0]); else fprintf(f1,"   %5.3f",br[j][0]); 
            if (br[j][1]<0) fprintf(f1,"         %5.3f",br[j][0]); else fprintf(f1,"          %5.3f",br[j][0]); 
            if (br[j][2]<0) fprintf(f1,"         %5.3f\n",br[j][0]); else fprintf(f1,"          %5.3f\n",br[j][0]); 
        }
        
        fclose(f1);
    }
    
    void dobias(char *options, bool multithread, int seed){
        char *datafilename, *progressfilename, *courantfilename;
        int rtOK,nstatOK, iprog,nprog;
        int nrec,nsel,ns,ns1,nrecpos,ntest,np,ng,sc,npv;
        string opt,*ss,s,*ss1,s0,s1;
        double  *stat_obs,st,pa;
        string bidon;
        bidon = new char[1000];
        FILE *flog, *fcur;
        progressfilename = new char[strlen(path)+strlen(ident)+20];
        strcpy(progressfilename,path);
        strcat(progressfilename,ident);
        strcat(progressfilename,"_progress.txt");
        courantfilename = new char[strlen(path)+strlen(ident)+20];
        strcpy(courantfilename,path);
        strcat(courantfilename,"courant.log");
        cout<<courantfilename<<"\n";
        cout<<"options : "<<options<<"\n";
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
                cout <<"scenario choisi : ";
                for (int j=0;j<rt.nscenchoisi;j++) {cout<<rt.scenchoisi[j]; if (j<rt.nscenchoisi-1) cout <<",";}cout<<"\n";
            } else if (s0=="n:") {
                nrec=atoi(s1.c_str());
                if(nrec>nrecpos) nrec=nrecpos;
                cout<<"nombre total de jeux de données considérés (pour le(s) scénario(s) choisi(s) )= "<<nrec<<"\n";
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
            } else if (s0=="p:") {
                original=(s1.find("o")!=string::npos);
                composite=(s1.find("c")!=string::npos);
                if (original) cout <<"paramètres  originaux  ";
                if ((s1=="oc")or(s1=="co")) cout <<"et ";
                if (composite) cout<<"paramètres  composite  ";
                cout<< "\n";
            } else if (s0=="d:") {
                ntest=atoi(s1.c_str());
                cout<<"nombre de jeux-tests à simuler = "<<ntest<<"\n";
            } else if (s0=="h:") {
                ss1 = splitwords(s1," ",&np);
                if (np != header.scenario[rt.scenchoisi[0]-1].nparam) {
                    cout<<"le nombre de paramètres transmis ("<<np<<") est incorrect. Le nombre attendu pour le scénario "<<rt.scenchoisi[0]<<" est de "<<header.scenario[rt.scenchoisi[0]-1].nparam<<"\n";
                    exit(1);
                }
                for (int j=0;j<np;j++) resethistparam(ss1[j]);
            } else if (s0=="u:") {
                cout<<s1<<"\n";
                ss1 = splitwords(s1,"-",&ng);
                if (ng != header.ngroupes) {
                    cout<<"le nombre de groupes transmis ("<<ng<<") est incorrect. Le nombre attendu  est de "<< header.ngroupes<<"\n";
                    //exit(1);
                }
                for (int j=0;j<ng;j++) resetmutparam(ss1[j]);
            }
        }
        npv = rt.nparam[rt.scenchoisi[0]-1];
        enreg = new enregC[ntest];
        for (int p=0;p<ntest;p++) {
            enreg[p].stat = new float[header.nstat];
            enreg[p].param = new float[npv];
            enreg[p].numscen = rt.scenchoisi[0];
        }
        ps.dosimultabref(header,ntest,false,multithread,true,rt.scenchoisi[0],seed);
        header.readHeader(headerfilename);cout<<"apres readHeader\n";
        for (int p=0;p<ntest;p++) {delete []enreg[p].param;delete []enreg[p].stat;}delete []enreg;

        enreg2 = new enreC[ntest];
        for (int p=0;p<ntest;p++) {
            enreg2[p].stat = new double[header.nstat];
            enreg2[p].paramvv = new double[npv];
            enreg2[p].numscen = rt.scenchoisi[0];
        }
        ifstream file(courantfilename, ios::in);
        getline(file,bidon);
        for (int p=0;p<ntest;p++) {
          getline(file,bidon);
          //cout<<bidon<<"\n";
          ss = splitwords(bidon," ",&ns);
          //cout<<"ns="<<ns<<"\n";
          enreg2[p].numscen=atoi(ss[0].c_str());
          for (int i=0;i<npv;i++) enreg2[p].paramvv[i]=atof(ss[i+1].c_str());
          for (int i=0;i<rt.nstat;i++) enreg2[p].stat[i]=atof(ss[i+1+npv].c_str());
        }
        file.close();
        /*cout<<"nombre de parametres dans courant.log = "<<rt.nparam[rt.scenchoisi[0]-1]<<"\n";
        for (int p=0;p<ntest;p++) {
            cout <<"numscen="<<enreg2[p].numscen<<"\n";
            cout <<"stat   =";for (int j=0;j<rt.nstat;j++) cout<<enreg2[p].stat[j]<<"  ";cout<<"\n";
            cout <<"param  =";for (int j=0;j<rt.nparam[rt.scenchoisi[0]-1];j++) cout<<enreg2[p].param[j]<<"  ";cout<<"\n";
        }*/
        nstatOK = rt.cal_varstat();                       //cout<<"apres cal_varstat\n";
        stat_obs = new double[rt.nstat];
        paramest = new parstatC*[ntest];
        paretoil = new double**[ntest];
        for (int p=0;p<ntest;p++) {
            for (int j=0;j<rt.nstat;j++) stat_obs[j]=enreg2[p].stat[j];
            rt.cal_dist(nrec,nsel,stat_obs);                  //cout<<"apres cal_dist\n";
            if (p<1) det_numpar();                            //cout<<"apres det_numpar\n";
            if (p<1) det_nomparam();
            recalparam(nsel);                                 //cout<<"apres recalparam\n";
            rempli_mat(nsel,stat_obs);                        //cout<<"apres rempli_mat\n";
            local_regression(nsel,multithread);               //cout<<"apres local_regression\n";
            iprog+=1;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
            phistar = calphistar(nsel);                                 //cout<<"apres calphistar\n";
            if(p<1) det_nomparam();
            paramest[p] = calparstat(nsel);                             //cout<<"apres calparstat\n";
            paretoil[p] = new double* [nsel];
            for (int i=0;i<nsel;i++) {
                paretoil[p][i] = new double[nparamcom+nparcompo];
                for (int j=0;j<nparamcom+nparcompo;j++) paretoil[p][i][j] = phistar[i][j];
            }
            printf(" test n° %3d   ",p+1);
            for (int j=0;j<npar;j++) printf(" %8.3e (%8.3e)   ",enreg2[p].paramvv[j],paramest[p][j].med);cout<<"\n";    
        }
        biaisrel(ntest,npv);
        ecrires(ntest,npv,nsel);
    }