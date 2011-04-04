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
extern bool multithread;

struct enreC {
    int numscen;
    double *paramvv,*stat;
};

parstatC **paramest;
enreC *enreg2;
double **br,*rrmise,*rmad,**rmse,*cov50,*cov95,**fac2,**medbr,*rmedad,**rmae;
double ***paretoil;

using namespace std;

    string pseudoprior(string s) {
        string spr;
        double x=atof(s.c_str());
        double mini=0.99999*x,maxi=1.00001*x;
        spr="UN["+DoubleToString(mini)+","+DoubleToString(maxi)+",0.0,0.0";
        return spr;
    }

    bool resethistparam(string s) {
        string *ss,name,sprior,smini,smaxi;
        bool useprior=true;
        int n,i;
        double mini,maxi;
        ss = splitwords(s,"=",&n);
        name=ss[0];
        i=0;while((i<header.scenario[rt.scenteste-1].nparam)and(name != header.scenario[rt.scenteste-1].histparam[i].name)) i++;
        //cout<<"parametre "<<name<<"  ("<<i<<")   "<<ss[1]<<"\n";
        if (ss[1].find("[")!=string::npos) {
            header.scenario[rt.scenteste-1].histparam[i].prior = header.readprior(ss[1]);
             //header.scenario[rt.scenteste-1].histparam[i].prior.ecris();
        }
        else {
            header.scenario[rt.scenteste-1].histparam[i].prior = header.readprior(pseudoprior(ss[1]));
            useprior=false;
            //header.scenario[rt.scenteste-1].histparam[i].value = atof(ss[1].c_str()) ;
        }
        return useprior;
    }

    bool resetmutparam(string s) {
        string *ss,numgr,s1;
        bool useprior=true;
        int n,i,gr,i0,i1;
        numgr = s.substr(1,s.find("(")-1);  gr=atoi(numgr.c_str());
        i0=s.find("(");i1=s.find(")"); cout <<"i0="<<i0<<"  i1="<<i1<<"\n";
        s1 = s.substr(i0+1,i1-i0-1); cout <<"groupe "<<gr<<"  "<<s1<<"\n";
        ss = splitwords(s1," ",&n);
        if (header.groupe[gr].type==0) {
            if (ss[0].find("[")==string::npos) {header.groupe[gr].priormutmoy =header.readpriormut(pseudoprior(ss[0]));useprior=false;}
            else header.groupe[gr].priormutmoy = header.readpriormut(ss[0]);
            if (ss[1].find("[")==string::npos) header.groupe[gr].priormutloc.sdshape=atof(ss[1].c_str());
            else header.groupe[gr].priormutloc = header.readpriormut(ss[1]);
            
            if (ss[2].find("[")==string::npos) {header.groupe[gr].priorPmoy=header.readpriormut(pseudoprior(ss[2]));useprior=false;}
            else header.groupe[gr].priorPmoy = header.readpriormut(ss[2]);
             if (ss[3].find("[")==string::npos) header.groupe[gr].priorPloc.sdshape=atof(ss[3].c_str());
            else header.groupe[gr].priorPloc = header.readpriormut(ss[3]);
            
            if (ss[4].find("[")==string::npos) {header.groupe[gr].priorsnimoy=header.readpriormut(pseudoprior(ss[4]));useprior=false;}
            else header.groupe[gr].priorsnimoy = header.readpriormut(ss[4]);
            if (ss[5].find("[")==string::npos) header.groupe[gr].priorsniloc.sdshape=atof(ss[5].c_str());
            else header.groupe[gr].priorsniloc = header.readpriormut(ss[5]);
       } else {
             if (ss[0].find("[")==string::npos) {header.groupe[gr].priormusmoy=header.readpriormut(pseudoprior(ss[0]));useprior=false;}
            else header.groupe[gr].priormusmoy = header.readpriormut(ss[0]);
            if (ss[1].find("[")==string::npos) header.groupe[gr].priormusloc.sdshape=atof(ss[1].c_str());
            else header.groupe[gr].priormusloc = header.readpriormut(ss[1]);
            
            if (ss[2].find("[")==string::npos) {header.groupe[gr].priork1moy=header.readpriormut(pseudoprior(ss[2]));useprior=false;}
            else header.groupe[gr].priork1moy = header.readpriormut(ss[2]);
             if (ss[3].find("[")==string::npos) header.groupe[gr].priork1loc.sdshape=atof(ss[3].c_str());
            else header.groupe[gr].priork1loc = header.readpriormut(ss[3]);
            
            if (ss[4].find("[")==string::npos) {header.groupe[gr].priork2moy=header.readpriormut(pseudoprior(ss[4]));useprior=false;}
            else header.groupe[gr].priork2moy = header.readpriormut(ss[4]);
            if (ss[5].find("[")==string::npos) header.groupe[gr].priork2loc.sdshape=atof(ss[5].c_str());
            else header.groupe[gr].priork2loc = header.readpriormut(ss[5]);
       } 
       return useprior;
    }

/**
* calcule les différentes statistiques de biais, rmse...
*/
    void biaisrel(int ntest,int nsel,int npv) {
        double s,d;
//////////////// mean relative bias
        br = new double* [3];
        for (int k=0;k<3;k++) {
            br[k] = new double[nparamcom+nparcompo];
            for (int j=0;j<nparamcom+nparcompo;j++) {
                br[k][j]=0.0;
                for (int p=0;p<ntest;p++) {
                    switch (k) {
                      case 0 : d=paramest[p][j].moy-enreg2[p].paramvv[j];break; 
                      case 1 : d=paramest[p][j].med-enreg2[p].paramvv[j];break; 
                      case 2 : d=paramest[p][j].mod-enreg2[p].paramvv[j];break; 
                    }
                    br[k][j] +=d/enreg2[p].paramvv[j];
                }
                br[k][j] /= (double)ntest;
                //cout << "br["<<k<<"]["<<j<<"]="<<br[k][j]<<"\n";
            }
        }
////////////  RRMISE
        rrmise = new double [nparamcom+nparcompo];
        for (int j=0;j<nparamcom+nparcompo;j++) {
            rrmise[j]=0.0;
            for (int p=0;p<ntest;p++) {
                s=0.0;
                for (int i=0;i<nsel;i++) {d=paretoil[p][i][j]-enreg2[p].paramvv[j];s += d*d;}
                rrmise[j] +=s/enreg2[p].paramvv[j]/enreg2[p].paramvv[j]/double(nsel);
            }
            rrmise[j] = sqrt(rrmise[j]/(double)ntest);
        } 
////////////  RMAD
        rmad = new double [nparamcom+nparcompo];
        for (int j=0;j<nparamcom+nparcompo;j++) {
            rmad[j]=0.0;
            for (int p=0;p<ntest;p++) {
                s=0.0;
                for (int i=0;i<nsel;i++) s += abs(paretoil[p][i][j]-enreg2[p].paramvv[j]);
                rmad[j] +=s/abs(enreg2[p].paramvv[j])/double(nsel);
            }
            rmad[j] = rmad[j]/(double)ntest;
        } 
////////////  RMSE
        rmse = new double*[3];
        for (int k=0;k<3;k++) {
            rmse[k] = new double[nparamcom+nparcompo];
            for (int j=0;j<nparamcom+nparcompo;j++) {
                rmse[k][j]=0.0;
                for (int p=0;p<ntest;p++)  if (enreg2[p].paramvv[j]>0.0) {
                    switch (k) {
                      case 0 : d=paramest[p][j].moy-enreg2[p].paramvv[j];break; 
                      case 1 : d=paramest[p][j].med-enreg2[p].paramvv[j];break; 
                      case 2 : d=paramest[p][j].mod-enreg2[p].paramvv[j];break; 
                    }
                    rmse[k][j] += d*d/enreg2[p].paramvv[j]/enreg2[p].paramvv[j];
                }
                rmse[k][j] =sqrt(rmse[k][j]/double(ntest));
            } 
        }
/////////////// coverages
        cov50 = new double[nparamcom+nparcompo];
        cov95 = new double[nparamcom+nparcompo];
        double atest=1.0/(double)ntest;
        for (int j=0;j<nparamcom+nparcompo;j++) {
             cov50[j]=0.0;
             cov95[j]=0.0;
             for (int p=0;p<ntest;p++) {
                 if((paramest[p][j].q025<=enreg2[p].paramvv[j])and(paramest[p][j].q975>=enreg2[p].paramvv[j])) cov95[j] += atest;
                 if((paramest[p][j].q250<=enreg2[p].paramvv[j])and(paramest[p][j].q750>=enreg2[p].paramvv[j])) cov50[j] += atest;
             }
        }
///////////////// factors 2
        fac2 = new double*[3];
        for (int k=0;k<3;k++) {
            fac2[k] = new double[nparamcom+nparcompo];
            for (int j=0;j<nparamcom+nparcompo;j++) {
                fac2[k][j]=0.0;
                for (int p=0;p<ntest;p++) {
                    switch (k) {
                      case 0 : if((paramest[p][j].moy>=0.5*enreg2[p].paramvv[j])and(paramest[p][j].moy<=2.0*enreg2[p].paramvv[j])) fac2[k][j] += atest;break; 
                      case 1 : if((paramest[p][j].med>=0.5*enreg2[p].paramvv[j])and(paramest[p][j].med<=2.0*enreg2[p].paramvv[j])) fac2[k][j] += atest;break; 
                      case 2 : if((paramest[p][j].mod>=0.5*enreg2[p].paramvv[j])and(paramest[p][j].mod<=2.0*enreg2[p].paramvv[j])) fac2[k][j] += atest;break; 
                    }
                }
            }
        }
/////////////////// medianes du biais relatif
        medbr = new double*[3];
        double *b;
        b = new double[ntest];
        for (int k=0;k<3;k++) {
            medbr[k] = new double[nparamcom+nparcompo];
            for (int j=0;j<nparamcom+nparcompo;j++) {
                switch (k) {
                  case 0 : for (int p=0;p<ntest;p++) b[p]=(paramest[p][j].moy-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];medbr[k][j] = cal_med(ntest,b);break; 
                  case 1 : for (int p=0;p<ntest;p++) b[p]=(paramest[p][j].med-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];medbr[k][j] = cal_med(ntest,b);break; 
                  case 2 : for (int p=0;p<ntest;p++) b[p]=(paramest[p][j].mod-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];medbr[k][j] = cal_med(ntest,b);break; 
                }
            }
        }
///////////////// Relative Median Absolute Deviation
        rmedad = new double [nparamcom+nparcompo];
        double *cc;
        cc = new double[nsel];
        
        for (int j=0;j<nparamcom+nparcompo;j++) {
            for (int p=0;p<ntest;p++) {
                for (int i=0;i<nsel;i++) cc[i] = (abs(paretoil[p][i][j]-enreg2[p].paramvv[j]))/enreg2[p].paramvv[j];b[p] = cal_med(nsel,cc);
            }
            rmedad[j] = cal_med(ntest,b);
        } 
////////////  RMAE
        rmae = new double*[3];
        double *e;
        e = new double [ntest];
        for (int k=0;k<3;k++) {
            rmae[k] = new double[nparamcom+nparcompo];
            for (int j=0;j<nparamcom+nparcompo;j++) {
                for (int p=0;p<ntest;p++) {
                    switch (k) {
                      case 0 : e[p]=abs(paramest[p][j].moy-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];break; 
                      case 1 : e[p]=abs(paramest[p][j].med-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];break; 
                      case 2 : e[p]=abs(paramest[p][j].mod-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];break; 
                    }
                }
                rmae[k][j] = cal_med(ntest,e);
            } 
        }
        //cout<<br[1][0]<<"   "<<br[1][1]<<"   "<<br[1][2]<<"\n";
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
        cout <<"Les résultats sont dans "<<nomfiresult<<"\n";
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
        fprintf(f1,"Chosen scenario : %d\n",rt.scenteste);
        fprintf(f1,"Number of simulated data sets : %d\n",rt.nreclus);
        fprintf(f1,"Number of selected data sets  : %d\n",nsel);
        fprintf(f1,"Results based on %d test data sets\n\n",ntest);
        fprintf(f1,"                                               Averages\n");
        fprintf(f1,"Parameter                True values           Means             Medians             Modes\n");
        for (int j=0;j<nparamcom+nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
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
        for (int j=0;j<nparamcom+nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparam[j].c_str());
            for(int i=0;i<33-nomparam[j].length();i++)fprintf(f1," ");
            if (br[0][j]<0) fprintf(f1,"  %5.3f",br[0][j]); else fprintf(f1,"   %5.3f",br[0][j]); 
            if (br[1][j]<0) fprintf(f1,"         %5.3f",br[1][j]); else fprintf(f1,"          %5.3f",br[1][j]); 
            if (br[2][j]<0) fprintf(f1,"         %5.3f\n",br[2][j]); else fprintf(f1,"          %5.3f\n",br[2][j]); 
        }
        fprintf(f1,"\n                            RRMISE            RMeanAD            Square root of mean square error/true value\n");
        fprintf(f1,"Parameter                                                         Mean             Median             Mode\n");
        for (int j=0;j<nparamcom+nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparam[j].c_str());
            for(int i=0;i<24-nomparam[j].length();i++)fprintf(f1," ");
            fprintf(f1,"     %5.3f             %5.3f             %5.3f              %5.3f            %5.3f\n",rrmise[j],rmad[j],rmse[0][j],rmse[1][j],rmse[2][j]);
        }
        fprintf(f1,"\n                                                                 Factor 2        Factor 2        Factor 2\n");
        fprintf(f1,"Parameter               50%% Coverage        95%% Coverage         (Mean)          (Median)        (Mode)  \n");
        for (int j=0;j<nparamcom+nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparam[j].c_str());
            for(int i=0;i<24-nomparam[j].length();i++)fprintf(f1," ");
            fprintf(f1,"     %5.3f             %5.3f             %5.3f              %5.3f            %5.3f\n",cov50[j],cov95[j],fac2[0][j],fac2[1][j],fac2[2][j]);
        }
        fprintf(f1,"\n                                          Median Relative Bias\n");
        fprintf(f1,"Parameter                                     Means          Medians        Modes\n");
        for (int j=0;j<nparamcom+nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparam[j].c_str());
            for(int i=0;i<43-nomparam[j].length();i++)fprintf(f1," ");
            if (medbr[0][j]<0) fprintf(f1,"  %5.3f",medbr[0][j]); else fprintf(f1,"   %5.3f",medbr[0][j]); 
            if (medbr[1][j]<0) fprintf(f1,"         %5.3f",medbr[1][j]); else fprintf(f1,"          %5.3f",medbr[1][j]); 
            if (medbr[2][j]<0) fprintf(f1,"         %5.3f\n",medbr[2][j]); else fprintf(f1,"          %5.3f\n",medbr[2][j]); 
        }
        fprintf(f1,"\n                             RMedAD        Median of the absolute error/true value\n");
        fprintf(f1,"Parameter                                     Means          Medians        Modes\n");
        for (int j=0;j<nparamcom+nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparam[j].c_str());
            for(int i=0;i<24-nomparam[j].length();i++)fprintf(f1," ");
            fprintf(f1,"      %5.3f           %5.3f          %5.3f          %5.3f\n",rmedad[j],rmae[0][j],rmae[1][j],rmae[2][j]);
        }
         fclose(f1);
        
    }
    
    void setcompo(int p) {
      double pmut;
        int kk,qq,k=0;
        for (int gr=1;gr<header.ngroupes+1;gr++) {
            if (header.groupe[gr].type==0) {
                if (header.groupe[gr].priormutmoy.constant) {
                    if (header.groupe[gr].priorsnimoy.constant) {
                        pmut = header.groupe[gr].mutmoy+header.groupe[gr].snimoy;
                        for (int j=0;j<npar;j++) {
                            if (header.scenario[rt.scenteste-1].histparam[numpar[0][j]].category<2){
                                  enreg2[p].paramvv[nparamcom+k]=pmut*enreg2[p].paramvv[j];
                                k++;
                            }
                        }
                    } else {
                        kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==2))) kk++;
                        for (int j=0;j<npar;j++) {
                            if (header.scenario[rt.scenteste-1].histparam[numpar[0][j]].category<2){
                                pmut = header.groupe[gr].mutmoy+enreg2[p].paramvv[npar+kk];
                                enreg2[p].paramvv[nparamcom+k]=pmut*enreg2[p].paramvv[j];
                                k++;
                            }
                        }
                    }
                } else {
                    if (header.groupe[gr].priorsnimoy.constant) {
                        kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==0))) kk++;
                        for (int j=0;j<npar;j++) {
                            if (header.scenario[rt.scenteste-1].histparam[numpar[0][j]].category<2){
                                pmut =enreg2[p].paramvv[npar+kk] +header.groupe[gr].snimoy;
                                enreg2[p].paramvv[nparamcom+k]=pmut*enreg2[p].paramvv[j];
                                k++;
                            }
                        }
                    } else {
                        kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==0))) kk++;
                        qq=0;while (not ((header.mutparam[qq].groupe == gr)and(header.mutparam[qq].category ==2))) qq++;
                        for (int j=0;j<npar;j++) {
                            if (header.scenario[rt.scenteste-1].histparam[numpar[0][j]].category<2){
                                pmut =enreg2[p].paramvv[npar+kk]+enreg2[p].paramvv[npar+qq];
                                enreg2[p].paramvv[nparamcom+k]=pmut*enreg2[p].paramvv[j];
                                k++;
                            }
                        }
                    }
                }
            } 
            if (header.groupe[gr].type==1) {    
                if (header.groupe[gr].priormusmoy.constant) {
                    pmut = header.groupe[gr].musmoy;
                    for (int j=0;j<npar;j++) {
                        if (header.scenario[rt.scenteste-1].histparam[numpar[0][j]].category<2){
                            enreg2[p].paramvv[nparamcom+k]=pmut*enreg2[p].paramvv[j];
                            k++;
                        }
                    }
                } else {
                    kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==3))) kk++;
                    for (int j=0;j<npar;j++) {
                        if (header.scenario[rt.scenteste-1].histparam[numpar[0][j]].category<2){
                            pmut = enreg2[p].paramvv[npar+kk];
                            enreg2[p].paramvv[nparamcom+k]=pmut*enreg2[p].paramvv[j];
                            k++;
                        }
                    }
                }
            }
        }
    }

    void dobias(char *options,  int seed){
        char *datafilename, *progressfilename, *courantfilename;
        int nstatOK, iprog,nprog;
        int nrec,nsel,ns,ns1,nrecpos,ntest,np,ng,sc,npv,nn;
        string opt,*ss,s,*ss1,s0,s1;
        double  *stat_obs,st,pa;
        bool usepriorhist,usepriormut;
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
                ss1 = splitwords(s1,",",&nn);
                rt.scenteste = atoi(ss1[0].c_str());
                nrecpos=0;nrecpos +=rt.nrecscen[rt.scenteste-1];
                cout <<"scenario choisi : "<<rt.scenteste<<"\n";
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
                if (np != header.scenario[rt.scenteste-1].nparam) {
                    cout<<"le nombre de paramètres transmis ("<<np<<") est incorrect. Le nombre attendu pour le scénario "<<rt.scenteste<<" est de "<<header.scenario[rt.scenteste-1].nparam<<"\n";
                    exit(1);
                }
                for (int j=0;j<np;j++) usepriorhist = resethistparam(ss1[j]);
            } else if (s0=="u:") {
                cout<<s1<<"\n";
                ss1 = splitwords(s1,"*",&ng);
                if (ng != header.ngroupes) {
                    cout<<"le nombre de groupes transmis ("<<ng<<") est incorrect. Le nombre attendu  est de "<< header.ngroupes<<"\n";
                    //exit(1);
                }
                for (int j=0;j<ng;j++) usepriormut = resetmutparam(ss1[j]);
            }
        }
        npv = rt.nparam[rt.scenteste-1];
        enreg = new enregC[ntest];
        for (int p=0;p<ntest;p++) {
            enreg[p].stat = new float[header.nstat];
            enreg[p].param = new float[npv];
            enreg[p].numscen = rt.scenteste;
        }
        ps.dosimultabref(header,ntest,false,multithread,true,rt.scenteste,seed);
        nprog=10*ntest+5;iprog=5;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        header.readHeader(headerfilename);cout<<"apres readHeader\n";
        for (int p=0;p<ntest;p++) {delete []enreg[p].param;delete []enreg[p].stat;}delete []enreg;
        rt.nscenchoisi=1;rt.scenchoisi = new int[rt.nscenchoisi];rt.scenchoisi[0]=rt.scenteste;
        det_numpar();                                   
        cout<<"naparmcom = "<<nparamcom<<"   nparcomp = "<<nparcompo<<"\n";
        enreg2 = new enreC[ntest];
        for (int p=0;p<ntest;p++) {
            enreg2[p].stat = new double[header.nstat];
            enreg2[p].paramvv = new double[nparamcom+nparcompo];
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
          if(nparcompo>0) setcompo(p);
          for (int i=0;i<rt.nstat;i++) enreg2[p].stat[i]=atof(ss[i+1+npv].c_str());
        }
        file.close();
        nstatOK = rt.cal_varstat();                       //cout<<"apres cal_varstat\n";
        stat_obs = new double[rt.nstat];
        paramest = new parstatC*[ntest];
        paretoil = new double**[ntest];
        for (int p=0;p<ntest;p++) {paretoil[p] = new double* [nsel];for (int i=0;i<nsel;i++)paretoil[p][i] = new double[nparamcom+nparcompo];} 
        rt.alloue_enrsel(nsel);
        cout<<"apres rt.alloue_enrsel\n";
        for (int p=0;p<ntest;p++) {
            for (int j=0;j<rt.nstat;j++) stat_obs[j]=enreg2[p].stat[j];
            rt.cal_dist(nrec,nsel,stat_obs);                  //cout<<"apres cal_dist\n";
            iprog +=6;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
            if (p<1) det_nomparam();
            recalparam(nsel);                                 //cout<<"apres recalparam\n";
            rempli_mat(nsel,stat_obs);                        //cout<<"apres rempli_mat\n";
            local_regression(nsel);               //cout<<"apres local_regression\n";
            phistar = calphistar(nsel);                                 //cout<<"apres calphistar\n";
            paramest[p] = calparstat(nsel);                             //cout<<"apres calparstat\n";
            for (int i=0;i<nsel;i++) {
                for (int j=0;j<nparamcom+nparcompo;j++) paretoil[p][i][j] = phistar[i][j];
            }
            for (int i=0;i<nsel;i++) delete []phistar[i];delete phistar;
            printf("analysing data test %3d \n",p+1);
            //for (int j=0;j<npar;j++) printf(" %6.0e (%8.2e %8.2e %8.2e) ",enreg2[p].paramvv[j],paramest[p][j].moy,paramest[p][j].med,paramest[p][j].mod);cout<<"\n";    
            iprog +=4;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        }
        rt.desalloue_enrsel(nsel);
        biaisrel(ntest,nsel,npv);
        ecrires(ntest,npv,nsel);
    }
