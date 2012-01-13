/*
 * bias.cpp
 *
 *  Created on: 23 march 2011
 *      Author: cornuet
 */

#include <vector>
#include <cstdlib>
#include <cmath>


#include "bias.h"
#include "estimparam.h"
#include "reftable.h"
#include "header.h"
#include "matrices.h"
#include "mesutils.h"
#include "particleset.h"
/*
#ifndef HEADER
#include "header.cpp"
#define HEADER
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
*/
using namespace std;

extern ReftableC rt;
extern ParticleSetC ps;
extern enregC* enreg;
extern bool multithread;
extern string progressfilename, path;
extern string scurfile;
extern HeaderC header;
extern int nparamcom, nparcompo, nparscaled,**numpar, numtransf, npar;
extern string ident, headerfilename;
extern bool original, composite;
extern string* nomparam;
extern long double ** phistar;



parstatC **paramest;
enreC *enreg2;
long double **br,*rrmise,*rmad,**rmse,*cov50,*cov95,**fac2,**medbr,*rmedad,**rmae;
long double ***paretoil;



/*
    string pseudoprior(string s) {
        string spr;
        long double x=atof(s.c_str());
        long double mini=0.99999*x,maxi=1.00001*x;
        spr="UN["+DoubleToString(mini)+","+DoubleToString(maxi)+",0.0,0.0";
        return spr;
    }
*/

/**
 *  modifie les paramètres historiques en accord avec les priors/valeurs des pseudo-observés
 */ 
    void resethistparam(string s) {
        string *ss,name,sprior,smini,smaxi;
        int n,i;
        ss = splitwords(s,"=",&n);
        name=ss[0];
        i=0;while((i<header.scenario[rt.scenteste-1].nparam)and(name != header.scenario[rt.scenteste-1].histparam[i].name)) i++;
        //cout<<"parametre "<<name<<"  ("<<i<<")   "<<ss[1]<<"\n";
        if (ss[1].find("[")!=string::npos) {
            header.scenario[rt.scenteste-1].histparam[i].prior.readprior(ss[1]);
            header.scenario[rt.scenteste-1].histparam[i].prior.fixed=false;
            header.scenario[rt.scenteste-1].histparam[i].prior.ecris();
        }
        else {
            header.scenario[rt.scenteste-1].histparam[i].value = atof(ss[1].c_str());
            header.scenario[rt.scenteste-1].histparam[i].prior.fixed=true;
            cout<<header.scenario[rt.scenteste-1].histparam[i].value<<"\n";
        }
    }

/**
 *  modifie les conditions sur les paramètres historiques en accord avec les priors des pseudo-observés
 */ 
    void resetcondition(int j,string s) {
       header.scenario[rt.scenteste-1].condition[j].readcondition(s);

    }

/**
 *  modifie les paramètres mutationnels en accord avec les priors des pseudo-observés
 */ 
    void resetmutparam(string s) {
	    cout<<"debut de resetmutparam\n";
        string *ss,numgr,s1,sg;
        int n,gr,i0,i1;
        numgr = s.substr(1,s.find("(")-1);  gr=atoi(numgr.c_str());
        i0=s.find("(");i1=s.find(")"); cout <<"i0="<<i0<<"  i1="<<i1<<"\n";
        s1 = s.substr(i0+1,i1-i0-1); cout <<"groupe "<<gr<<"  "<<s1<<"\n";
        ss = splitwords(s1," ",&n);
        if (header.groupe[gr].type==0) {
		    //cout<<"mutmoy : \n";
			//header.groupe[gr].priormutmoy.ecris();
            if (header.groupe[gr].priormutmoy.constant) header.groupe[gr].mutmoy=header.groupe[gr].priormutmoy.mini;
			else {
				if (ss[0].find("[")==string::npos) {header.groupe[gr].mutmoy =atof(ss[0].c_str());header.groupe[gr].priormutmoy.fixed=true;}
				else {header.groupe[gr].priormutmoy.readprior(ss[0]);header.groupe[gr].priormutmoy.fixed=false;}
			}
		    //cout<<"mutmoy : \n";
			//header.groupe[gr].priormutmoy.ecris();
            if (ss[1].find("[")==string::npos) header.groupe[gr].priormutloc.sdshape=atof(ss[1].c_str());
            else header.groupe[gr].priormutloc.readprior(ss[1]);

		    //cout<<"Pmoy : \n";
			//header.groupe[gr].priorPmoy.ecris();
			if (header.groupe[gr].priorPmoy.constant) header.groupe[gr].Pmoy=header.groupe[gr].priorPmoy.mini;
			else{
				if (ss[2].find("[")==string::npos) {header.groupe[gr].Pmoy=atof(ss[2].c_str());header.groupe[gr].priorPmoy.fixed=true;}
				else {header.groupe[gr].priorPmoy.readprior(ss[2]);header.groupe[gr].priorPmoy.fixed=false;}
			}
		    //cout<<"Pmoy : \n";
			//header.groupe[gr].priorPmoy.ecris();
            if (ss[3].find("[")==string::npos) header.groupe[gr].priorPloc.sdshape=atof(ss[3].c_str());
            else header.groupe[gr].priorPloc.readprior(ss[3]);
			
		    //cout<<"snimoy : \n";
			//header.groupe[gr].priorsnimoy.ecris();
			if (header.groupe[gr].priorsnimoy.constant) header.groupe[gr].snimoy=header.groupe[gr].priorsnimoy.mini;
			else {
				if (ss[4].find("[")==string::npos) {header.groupe[gr].snimoy=atof(ss[4].c_str());header.groupe[gr].priorsnimoy.fixed=true;}
				else {header.groupe[gr].priorsnimoy.readprior(ss[4]);header.groupe[gr].priorsnimoy.fixed=false;}
			}
		    //cout<<"snimoy : \n";
			//header.groupe[gr].priorsnimoy.ecris();
            if (ss[5].find("[")==string::npos) header.groupe[gr].priorsniloc.sdshape=atof(ss[5].c_str());
            else header.groupe[gr].priorsniloc.readprior(ss[5]);
       } else if (header.groupe[gr].type==1){
		    //cout<<"resetmutparam type sequence\n";
			//cout<<"modele "<< header.groupe[gr].mutmod<<"\n";
			if (header.groupe[gr].priormusmoy.constant) header.groupe[gr].musmoy=header.groupe[gr].priormusmoy.mini;
			else {
				if (ss[0].find("[")==string::npos) {header.groupe[gr].musmoy=atof(ss[0].c_str());header.groupe[gr].priormusmoy.fixed=true;}
				else {header.groupe[gr].priormusmoy.readprior(ss[0]);header.groupe[gr].priormusmoy.fixed=false;}
			}
            //cout<<"ss[0] : >"<<ss[0]<<"<      musmoy="<<header.groupe[gr].musmoy<<"\n";
			if (ss[1].find("[")==string::npos) header.groupe[gr].priormusloc.sdshape=atof(ss[1].c_str());
            else header.groupe[gr].priormusloc.readprior(ss[1]);
            //cout<<"musloc"<<"\n";
			if (header.groupe[gr].mutmod>0) {
                if (header.groupe[gr].priork1moy.constant) header.groupe[gr].k1moy=header.groupe[gr].priork1moy.mini;
                else {
					if (ss[2].find("[")==string::npos) {header.groupe[gr].k1moy=atof(ss[2].c_str());header.groupe[gr].priork1moy.fixed=true;}
					else {header.groupe[gr].priork1moy.readprior(ss[2]);header.groupe[gr].priork1moy.fixed=false;}
                }
				//cout<<"k1moy="<<header.groupe[gr].k1moy<<"\n";
				if (ss[3].find("[")==string::npos) header.groupe[gr].priork1loc.sdshape=atof(ss[3].c_str());
				else header.groupe[gr].priork1loc.readprior(ss[3]);
				//cout<<"k1loc\n";
				if (header.groupe[gr].mutmod>2) {
					if (header.groupe[gr].priork1moy.constant) header.groupe[gr].k1moy=header.groupe[gr].priork1moy.mini;
					else {
						if (ss[4].find("[")==string::npos) {header.groupe[gr].k2moy=atof(ss[4].c_str());header.groupe[gr].priork2moy.fixed=true;}
						else {header.groupe[gr].priork2moy.readprior(ss[4]);header.groupe[gr].priork2moy.fixed=false;}
                    }
					//cout<<"k2moy="<<header.groupe[gr].k2moy<<"\n";
					if (ss[5].find("[")==string::npos) header.groupe[gr].priork2loc.sdshape=atof(ss[5].c_str());
					else header.groupe[gr].priork2loc.readprior(ss[5]);
				}
				//cout<<"fin\n";
			}
       }
    }

/**
* calcule les différentes statistiques de biais, rmse...
*/
    void biaisrel(int ntest,int nsel,int npv) {
        long double s,d;
//////////////// mean relative bias
        br = new long double* [3];
        for (int k=0;k<3;k++) {
            br[k] = new long double[nparamcom+nparcompo];
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
                br[k][j] /= (long double)ntest;
                //cout << "br["<<k<<"]["<<j<<"]="<<br[k][j]<<"\n";
            }
        }
////////////  RRMISE
        rrmise = new long double [nparamcom+nparcompo];
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
        rmad = new long double [nparamcom+nparcompo];
        for (int j=0;j<nparamcom+nparcompo;j++) {
            rmad[j]=0.0;
            for (int p=0;p<ntest;p++) {
                s=0.0;
                for (int i=0;i<nsel;i++) s += fabs(paretoil[p][i][j]-enreg2[p].paramvv[j]);
                rmad[j] +=s/fabs(enreg2[p].paramvv[j])/(long double)nsel;
            }
            rmad[j] = rmad[j]/(long double)ntest;
        }
////////////  RMSE
        rmse = new long double*[3];
        for (int k=0;k<3;k++) {
            rmse[k] = new long double[nparamcom+nparcompo];
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
                rmse[k][j] =sqrt(rmse[k][j]/(long double)ntest);
            }
        }

/////////////// coverages
        cov50 = new long double[nparamcom+nparcompo];
        cov95 = new long double[nparamcom+nparcompo];
        long double atest=1.0/(long double)ntest;
        for (int j=0;j<nparamcom+nparcompo;j++) {
             cov50[j]=0.0;
             cov95[j]=0.0;
             for (int p=0;p<ntest;p++) {
                 if((paramest[p][j].q025<=enreg2[p].paramvv[j])and(paramest[p][j].q975>=enreg2[p].paramvv[j])) cov95[j] += atest;
                 if((paramest[p][j].q250<=enreg2[p].paramvv[j])and(paramest[p][j].q750>=enreg2[p].paramvv[j])) cov50[j] += atest;
             }
        }
///////////////// factors 2
        fac2 = new long double*[3];
        for (int k=0;k<3;k++) {
            fac2[k] = new long double[nparamcom+nparcompo];
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
        medbr = new long double*[3];
        long double *b;
        b = new long double[ntest];
        for (int k=0;k<3;k++) {
            medbr[k] = new long double[nparamcom+nparcompo];
            for (int j=0;j<nparamcom+nparcompo;j++) {
                switch (k) {
                  case 0 : for (int p=0;p<ntest;p++) b[p]=(paramest[p][j].moy-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];medbr[k][j] = cal_medL(ntest,b);break;
                  case 1 : for (int p=0;p<ntest;p++) b[p]=(paramest[p][j].med-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];medbr[k][j] = cal_medL(ntest,b);break;
                  case 2 : for (int p=0;p<ntest;p++) b[p]=(paramest[p][j].mod-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];medbr[k][j] = cal_medL(ntest,b);break;
                }
            }
        }
///////////////// Relative Median Absolute Deviation
        rmedad = new long double [nparamcom+nparcompo];
        long double *cc;
        cc = new long double[nsel];

        for (int j=0;j<nparamcom+nparcompo;j++) {
            for (int p=0;p<ntest;p++) {
                for (int i=0;i<nsel;i++) cc[i] = (fabs(paretoil[p][i][j]-enreg2[p].paramvv[j]))/enreg2[p].paramvv[j];b[p] = cal_medL(nsel,cc);
            }
            rmedad[j] = cal_medL(ntest,b);
        }
////////////  RMAE
        rmae = new long double*[3];
        long double *e;
        e = new long double [ntest];
        for (int k=0;k<3;k++) {
            rmae[k] = new long double[nparamcom+nparcompo];
            for (int j=0;j<nparamcom+nparcompo;j++) {
                for (int p=0;p<ntest;p++) {
                    switch (k) {
                      case 0 : e[p]=fabs(paramest[p][j].moy-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];break;
                      case 1 : e[p]=fabs(paramest[p][j].med-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];break;
                      case 2 : e[p]=fabs(paramest[p][j].mod-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];break;
                    }
                }
                rmae[k][j] = cal_medL(ntest,e);
            }
        }
        //cout<<br[1][0]<<"   "<<br[1][1]<<"   "<<br[1][2]<<"\n";
    }

/**
 *   Mise en forme des résultats
 */ 
	void ecrires(int ntest,int npv,int nsel) {
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        string nomfiresult;
        long double *x,*mo;
        x=new long double[ntest];
        mo=new long double[4];
        nomfiresult = path + ident + "_bias.txt";
        //strcpy(nomfiresult,path);
        //strcat(nomfiresult,ident);
        //strcat(nomfiresult,"_bias.txt");
        cout <<"Les résultats sont dans "<<nomfiresult<<"\n";
        FILE *f1;
        f1=fopen(nomfiresult.c_str(),"w");
        fprintf(f1,"DIYABC :                 Bias and Mean Square Error Analysis                         %s\n",asctime(timeinfo));
        fprintf(f1,"Data file       : %s\n",header.datafilename.c_str());
        fprintf(f1,"Reference table : %s\n",rt.filename.c_str());
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
            for (int i=0;i<24-(int)nomparam[j].length();i++)fprintf(f1," ");
            for (int i=0;i<ntest;i++) x[i]=enreg2[i].paramvv[j];mo[0]=cal_moyL(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramest[i][j].moy;mo[1]=cal_moyL(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramest[i][j].med;mo[2]=cal_moyL(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramest[i][j].mod;mo[3]=cal_moyL(ntest,x);
            fprintf(f1,"  %8.3Le          %8.3Le          %8.3Le          %8.3Le\n",mo[0],mo[1],mo[2],mo[3]);
        }
        fprintf(f1,"\n                                           Mean Relative Bias\n");
        fprintf(f1,"Parameter                           Means          Medians        Modes\n");
        for (int j=0;j<nparamcom+nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparam[j].c_str());
            for(int i=0;i<33-(int)nomparam[j].length();i++)fprintf(f1," ");
            if (br[0][j]<0) fprintf(f1,"  %6.3Lf",br[0][j]); else fprintf(f1,"  %6.3Lf",br[0][j]);
            if (br[1][j]<0) fprintf(f1,"         %6.3Lf",br[1][j]); else fprintf(f1,"         %6.3Lf",br[1][j]);
            if (br[2][j]<0) fprintf(f1,"         %6.3Lf\n",br[2][j]); else fprintf(f1,"         %6.3Lf\n",br[2][j]);
        }
        fprintf(f1,"\n                            RRMISE            RMeanAD            Square root of mean square error/true value\n");
        fprintf(f1,"Parameter                                                         Mean             Median             Mode\n");
        for (int j=0;j<nparamcom+nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparam[j].c_str());
            for(int i=0;i<24-(int)nomparam[j].length();i++)fprintf(f1," ");
            fprintf(f1,"    %6.3Lf            %6.3Lf            %6.3Lf             %6.3Lf           %6.3Lf\n",rrmise[j],rmad[j],rmse[0][j],rmse[1][j],rmse[2][j]);
        }
        fprintf(f1,"\n                                                                 Factor 2        Factor 2        Factor 2\n");
        fprintf(f1,"Parameter               50%% Coverage        95%% Coverage         (Mean)          (Median)        (Mode)  \n");
        for (int j=0;j<nparamcom+nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparam[j].c_str());
            for(int i=0;i<24-(int)nomparam[j].length();i++)fprintf(f1," ");
            fprintf(f1,"    %6.3Lf            %6.3Lf            %6.3Lf             %6.3Lf           %6.3Lf\n",cov50[j],cov95[j],fac2[0][j],fac2[1][j],fac2[2][j]);
        }
        fprintf(f1,"\n                                          Median Relative Bias\n");
        fprintf(f1,"Parameter                                     Means          Medians        Modes\n");
        for (int j=0;j<nparamcom+nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparam[j].c_str());
            for(int i=0;i<43-(int)nomparam[j].length();i++)fprintf(f1," ");
            if (medbr[0][j]<0) fprintf(f1,"  %6.3Lf",medbr[0][j]); else fprintf(f1,"  %6.3Lf",medbr[0][j]);
            if (medbr[1][j]<0) fprintf(f1,"         %6.3Lf",medbr[1][j]); else fprintf(f1,"         %6.3Lf",medbr[1][j]);
            if (medbr[2][j]<0) fprintf(f1,"         %6.3Lf\n",medbr[2][j]); else fprintf(f1,"         %6.3Lf\n",medbr[2][j]);
        }
        fprintf(f1,"\n                             RMedAD        Median of the absolute error/true value\n");
        fprintf(f1,"Parameter                                     Means          Medians        Modes\n");
        for (int j=0;j<nparamcom+nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparam[j].c_str());
            for(int i=0;i<24-(int)nomparam[j].length();i++)fprintf(f1," ");
            fprintf(f1,"     %6.3Lf          %6.3Lf         %6.3Lf         %6.3Lf\n",rmedad[j],rmae[0][j],rmae[1][j],rmae[2][j]);
        }
         fclose(f1);

    }

/**
 * Traitement des paramètres composites (microsat et séquences uniquement)
 */ 
    void setcompo(int p) {
		long double pmut;
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

/**
 * Interprête la ligne de paramètres de l'option biais et lance les calculs correspondants
 */
	void dobias(string opt,  int seed){
        string courantfilename;
        int nstatOK, iprog,nprog;
        int nrec = 0, nsel = 0,ns,nrecpos = 0,ntest = 0,np,ng,npv,nn,ncond;
        string *ss,s,*ss1,s0,s1,sg; 
		float *stat_obs;
        string bidon;
        bidon = new char[1000];
        FILE *flog;

        progressfilename = path + ident + "_progress.txt";
        courantfilename = path + "biascourant.log";
        cout<<courantfilename<<"\n";
        cout<<"options : "<<opt<<"\n";
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
                if (np < header.scenario[rt.scenteste-1].nparam) {
                    cout<<"le nombre de paramètres transmis ("<<np<<") est incorrect. Le nombre attendu pour le scénario "<<rt.scenteste<<" est de "<<header.scenario[rt.scenteste-1].nparam<<"\n";
                    exit(1);
                }
                ncond=np-header.scenario[rt.scenteste-1].nparam;
                for (int j=0;j<header.scenario[rt.scenteste-1].nparam;j++) resethistparam(ss1[j]);
                if (ncond>0) {
                  cout<<header.scenario[rt.scenteste-1].nconditions<<"\n";
                    if (header.scenario[rt.scenteste-1].nconditions != ncond) {
                        if (header.scenario[rt.scenteste-1].nconditions>0) delete []header.scenario[rt.scenteste-1].condition;
                        header.scenario[rt.scenteste-1].condition = new ConditionC[ncond];
                    }
                    for (int j=0;j<ncond;j++)
                         header.scenario[rt.scenteste-1].condition[j].readcondition(ss1[j+header.scenario[rt.scenteste-1].nparam]);
                }
            } else if (s0=="u:") {
                cout<<s1<<"\n";
                ss1 = splitwords(s1,"*",&ng);
                if (ng != header.ngroupes) {
                    cout<<"le nombre de groupes transmis ("<<ng<<") est incorrect. Le nombre attendu  est de "<< header.ngroupes<<"\n";
                    //exit(1);
                }
                for (int j=1;j<=ng;j++) resetmutparam(ss1[j-1]);
				/*if (not usepriormut) {
				    header.entetemut0=header.entetemut;
					header.entetemut="";
				    for (int j=1;j<=ng;j++){
					    sg=IntToString(j);
						if (header.groupe[j].type==0) {
							header.entetemut=header.entetemut+"µmic_"+sg+"        pmic_"+sg+"        snimic_"+sg+"       ";
						}
						if (header.groupe[j].type==1) {
							header.entetemut=header.entetemut+"µseq_"+sg+"        ";
							if (header.groupe[j].mutmod>0){
								header.entetemut=header.entetemut+"k1seq_"+sg+"       ";
								if (header.groupe[j].mutmod>2){
									header.entetemut=header.entetemut+"k2seq_"+sg+"       ";
								}
							}
						}

					}
					header.entete=header.entetehist+header.entetemut+header.entetestat;
					cout<<"dans bias.cpp l'entete devient \n";
					cout<<header.entete<<"\n";
				}*/
            }
        }
        npv = rt.nparam[rt.scenteste-1];
        enreg = new enregC[ntest];
        for (int p=0;p<ntest;p++) {
            enreg[p].stat = new float[header.nstat];
            enreg[p].param = new float[npv];
            enreg[p].numscen = rt.scenteste;
        }



        //cout<<header.entete<<"\n";
        ps.dosimultabref(header,ntest,false,multithread,true,rt.scenteste,seed);
		//header.entete=header.entetehist+header.entetemut0+header.entetestat;
        nprog=10*ntest+6;iprog=5;flog=fopen(progressfilename.c_str(),"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
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
        cout<<"courantfilename="<<scurfile<<"\n";
        ifstream file(scurfile.c_str(), ios::in);
        getline(file,bidon);
        for (int p=0;p<ntest;p++) {
          getline(file,bidon);
          //cout<<"bidon="<<bidon<<"\n";
          //cout<<bidon<<"\n";
          ss = splitwords(bidon," ",&ns);
          //cout<<"ns="<<ns<<"\n";
          enreg2[p].numscen=atoi(ss[0].c_str());
		  //cout<<"bias.cpp   npv="<<npv<<"\n";
          for (int i=0;i<npv;i++) enreg2[p].paramvv[i]=atof(ss[i+1].c_str());
          if(nparcompo>0) setcompo(p);
          for (int i=0;i<rt.nstat;i++) enreg2[p].stat[i]=atof(ss[i+1+npv].c_str());
        }
        file.close();
        nstatOK = rt.cal_varstat();                       cout<<"apres cal_varstat\n";
        stat_obs = new float[rt.nstat];
        paramest = new parstatC*[ntest];
        paretoil = new long double**[ntest];
        for (int p=0;p<ntest;p++) {paretoil[p] = new long double* [nsel];for (int i=0;i<nsel;i++)paretoil[p][i] = new long double[nparamcom+nparcompo];}
        rt.alloue_enrsel(nsel);
        cout<<"apres rt.alloue_enrsel\n";
        for (int p=0;p<ntest;p++) {
            for (int j=0;j<rt.nstat;j++) stat_obs[j]=enreg2[p].stat[j];
            rt.cal_dist(nrec,nsel,stat_obs);                  cout<<"apres cal_dist\n";
            iprog +=6;flog=fopen(progressfilename.c_str(),"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
            if (p<1) det_nomparam();
			if (original) {
				recalparamO(nsel);                                 cout<<"apres recalparam\n";
				rempli_mat(nsel,stat_obs,nparamcom);                        cout<<"apres rempli_mat\n";
				local_regression(nsel,nparamcom);               cout<<"apres local_regression\n";
				phistar = calphistarO(nsel);                                 cout<<"apres calphistar\n";
			}
            paramest[p] = calparstat(nsel);                             cout<<"apres calparstat\n";
            for (int i=0;i<nsel;i++) {
                for (int j=0;j<nparamcom+nparcompo;j++) paretoil[p][i][j] = phistar[i][j];
            }
            for (int i=0;i<nsel;i++) delete []phistar[i];delete phistar;
            printf("\nanalysing data test %3d \n",p+1);
            //for (int j=0;j<npar;j++) printf(" %6.0e (%8.2e %8.2e %8.2e) ",enreg2[p].paramvv[j],paramest[p][j].moy,paramest[p][j].med,paramest[p][j].mod);cout<<"\n";
            iprog +=4;flog=fopen(progressfilename.c_str(),"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        }
        rt.desalloue_enrsel(nsel);
        biaisrel(ntest,nsel,npv);
        ecrires(ntest,npv,nsel);
        iprog +=1;flog=fopen(progressfilename.c_str(),"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
    }
