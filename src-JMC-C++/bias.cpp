/*
 * bias.cpp
 *
 *  Created on: 23 march 2011
 *      Author: cornuet
 */

#include <vector>
#include <cstdlib>
#include <cmath>
#include <time.h>
#include <iomanip>


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
extern int nparamcom, nparcompo, nparscaled,**numpar, numtransf, npar,nstatOKsel;
extern string ident, headerfilename;
extern bool original, composite,scaled;
extern string* nomparamO,*nomparamC,*nomparamS;
//extern long double **phistar,**phistarcompo,**phistarscaled;
extern int debuglevel;
extern ofstream fprog;


parstatC **paramest, **paramestcompo, **paramestscaled;
enreC *enreg2;
long double **br_O,*rrmise_O,*rmad_O,**rmse_O,*cov50_O,*cov95_O,**fac2_O,**rmb_O,*rmedad_O,**rmae_O,***bmed_O,**bmedr_O,***bmeda_O;
long double **br_C,*rrmise_C,*rmad_C,**rmse_C,*cov50_C,*cov95_C,**fac2_C,**rmb_C,*rmedad_C,**rmae_C,***bmed_C,**bmedr_C,***bmeda_C;
long double **br_S,*rrmise_S,*rmad_S,**rmse_S,*cov50_S,*cov95_S,**fac2_S,**rmb_S,*rmedad_S,**rmae_S,***bmed_S,**bmedr_S,***bmeda_S;
long double **paretoil,**paretoilcompo,**paretoilscaled;

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
        //cout<<"resethistparam   parametre "<<name<<"  ("<<i<<")   "<<ss[1]<<"\n";
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
				if (ss[0].find("[")==string::npos) {
					header.groupe[gr].mutmoy =atof(ss[0].c_str());
					header.groupe[gr].priormutmoy.fixed=true;
					cout<<"mutmoy="<<header.groupe[gr].mutmoy<<"\n";
				} else {
					header.groupe[gr].priormutmoy.readprior(ss[0]);header.groupe[gr].priormutmoy.fixed=false;
					
				}
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
* initialise les tableaux et les différentes statistiques de biais, rmse... pour les paramètres originaux
*/
    void initbiasO(int ntest, int nsel,int nparamcom) {
		paramest = new parstatC*[ntest];
		paretoil = new long double*[nsel];
		for (int i=0;i<nsel;i++)paretoil[i] = new long double[nparamcom];
//////////////// mean relative bias
        br_O = new long double* [3];
        for (int k=0;k<3;k++) {
            br_O[k] = new long double[nparamcom];
            for (int j=0;j<nparamcom;j++) br_O[k][j]=0.0;
		}
////////////  RRMISE
        rrmise_O = new long double [nparamcom];
        for (int j=0;j<nparamcom;j++) rrmise_O[j]=0.0;
////////////  RMAD
        rmad_O = new long double [nparamcom];
        for (int j=0;j<nparamcom;j++) rmad_O[j]=0.0;
////////////  RMSE
        rmse_O = new long double*[3];
        for (int k=0;k<3;k++) {
            rmse_O[k] = new long double[nparamcom];
            for (int j=0;j<nparamcom;j++) rmse_O[k][j]=0.0;
		}
/////////////// coverages
        cov50_O = new long double[nparamcom];
        cov95_O = new long double[nparamcom];
        for (int j=0;j<nparamcom;j++) {
             cov50_O[j]=0.0;
             cov95_O[j]=0.0;
		}
///////////////// factors 2
        fac2_O = new long double*[3];
        for (int k=0;k<3;k++) {
            fac2_O[k] = new long double[nparamcom];
            for (int j=0;j<nparamcom;j++) fac2_O[k][j]=0.0;
		}
/////////////////// medianes du biais relatif
		bmed_O = new long double**[3];
        for (int k=0;k<3;k++) {
			bmed_O[k] = new long double*[nparamcom];
			for (int j=0;j<nparamcom;j++) {
				bmed_O[k][j] = new long double[ntest];
			}
		}
		rmb_O = new long double*[3];
		for (int k=0;k<3;k++) rmb_O[k] = new long double[nparamcom];
 ///////////////// Relative Median Absolute Deviation
		bmedr_O = new long double*[nparamcom];
		for (int j=0;j<nparamcom;j++) bmedr_O[j] = new long double[ntest];
        rmedad_O = new long double [nparamcom];
////////////  RMAE
        rmae_O = new long double*[3];
		for (int k=0;k<3;k++) rmae_O[k] = new long double[nparamcom];
		bmeda_O = new long double**[3];
		for (int k=0;k<3;k++){
			bmeda_O[k] = new long double*[nparamcom];
			for (int j=0;j<nparamcom;j++) bmeda_O[k][j] = new long double[ntest];
		}
	}
	
/**
* initialise les tableaux et les différentes statistiques de biais, rmse... pour les paramètres composites
*/
    void initbiasC(int ntest, int nsel,int nparamcom) {
		paramestcompo = new parstatC*[ntest];
		paretoilcompo = new long double*[nsel];
		for (int i=0;i<nsel;i++)paretoilcompo[i] = new long double[nparcompo];
//////////////// mean relative bias
        br_C = new long double* [3];
        for (int k=0;k<3;k++) {
            br_C[k] = new long double[nparcompo];
            for (int j=0;j<nparcompo;j++) br_C[k][j]=0.0;
		}
////////////  RRMISE
        rrmise_C = new long double [nparcompo];
        for (int j=0;j<nparcompo;j++) rrmise_C[j]=0.0;
////////////  RMAD
        rmad_C = new long double [nparcompo];
        for (int j=0;j<nparcompo;j++) rmad_C[j]=0.0;
////////////  RMSE
        rmse_C = new long double*[3];
        for (int k=0;k<3;k++) {
            rmse_C[k] = new long double[nparcompo];
            for (int j=0;j<nparcompo;j++) rmse_C[k][j]=0.0;
		}
/////////////// coverages
        cov50_C = new long double[nparcompo];
        cov95_C = new long double[nparcompo];
        for (int j=0;j<nparcompo;j++) {
             cov50_C[j]=0.0;
             cov95_C[j]=0.0;
		}
///////////////// factors 2
        fac2_C = new long double*[3];
        for (int k=0;k<3;k++) {
            fac2_C[k] = new long double[nparcompo];
            for (int j=0;j<nparcompo;j++) fac2_C[k][j]=0.0;
		}
/////////////////// medianes du biais relatif
		bmed_C = new long double**[3];
        for (int k=0;k<3;k++) {
			bmed_C[k] = new long double*[nparcompo];
			for (int j=0;j<nparcompo;j++) {
				bmed_C[k][j] = new long double[ntest];
			}
		}
		rmb_C = new long double*[3];
		for (int k=0;k<3;k++) rmb_C[k] = new long double[nparcompo];
 ///////////////// Relative Median Absolute Deviation
		bmedr_C = new long double*[nparcompo];
		for (int j=0;j<nparcompo;j++) bmedr_C[j] = new long double[ntest];
        rmedad_C = new long double [nparcompo];
////////////  RMAE
        rmae_C = new long double*[3];
		for (int k=0;k<3;k++) rmae_C[k] = new long double[nparcompo];
		bmeda_C = new long double**[3];
		for (int k=0;k<3;k++){
			bmeda_C[k] = new long double*[nparcompo];
			for (int j=0;j<nparcompo;j++) bmeda_C[k][j] = new long double[ntest];
		}
	}
	
    void initbiasS(int ntest, int nsel,int nparamcom) {
		paramestscaled = new parstatC*[ntest];
		paretoilscaled = new long double*[nsel];
		for (int i=0;i<nsel;i++)paretoilscaled[i] = new long double[nparscaled];
//////////////// mean relative bias
        br_S = new long double* [3];
        for (int k=0;k<3;k++) {
            br_S[k] = new long double[nparscaled];
            for (int j=0;j<nparscaled;j++) br_S[k][j]=0.0;
		}
////////////  RRMISE
        rrmise_S = new long double [nparscaled];
        for (int j=0;j<nparscaled;j++) rrmise_S[j]=0.0;
////////////  RMAD
        rmad_S = new long double [nparscaled];
        for (int j=0;j<nparscaled;j++) rmad_S[j]=0.0;
////////////  RMSE
        rmse_S = new long double*[3];
        for (int k=0;k<3;k++) {
            rmse_S[k] = new long double[nparscaled];
            for (int j=0;j<nparscaled;j++) rmse_S[k][j]=0.0;
		}
/////////////// coverages
        cov50_S = new long double[nparscaled];
        cov95_S = new long double[nparscaled];
        for (int j=0;j<nparscaled;j++) {
             cov50_S[j]=0.0;
             cov95_S[j]=0.0;
		}
///////////////// factors 2
        fac2_S = new long double*[3];
        for (int k=0;k<3;k++) {
            fac2_S[k] = new long double[nparscaled];
            for (int j=0;j<nparscaled;j++) fac2_S[k][j]=0.0;
		}
/////////////////// medianes du biais relatif
		bmed_S = new long double**[3];
        for (int k=0;k<3;k++) {
			bmed_S[k] = new long double*[nparscaled];
			for (int j=0;j<nparscaled;j++) {
				bmed_S[k][j] = new long double[ntest];
			}
		}
		rmb_S = new long double*[3];
		for (int k=0;k<3;k++) rmb_S[k] = new long double[nparscaled];
 ///////////////// Relative Median Absolute Deviation
		bmedr_S = new long double*[nparscaled];
		for (int j=0;j<nparscaled;j++) bmedr_S[j] = new long double[ntest];
        rmedad_S = new long double [nparscaled];
////////////  RMAE
        rmae_S = new long double*[3];
		for (int k=0;k<3;k++) rmae_S[k] = new long double[nparscaled];
		bmeda_S = new long double**[3];
		for (int k=0;k<3;k++){
			bmeda_S[k] = new long double*[nparscaled];
			for (int j=0;j<nparscaled;j++) bmeda_S[k][j] = new long double[ntest];
		}
	}
	
/**
* calcule les différentes statistiques de biais, rmse...
*/
    void biaisrelO(int ntest,int nsel,int npv,int p) {
        long double s,d;
//////////////// mean relative bias
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparamcom;j++) {
				switch (k) {
					case 0 : d=paramest[p][j].moy-enreg2[p].paramvv[j];break;
					case 1 : d=paramest[p][j].med-enreg2[p].paramvv[j];break;
					case 2 : d=paramest[p][j].mod-enreg2[p].paramvv[j];break;
				}
				br_O[k][j] +=d/enreg2[p].paramvv[j]; // il restera à diviser par ntest
			}
		}
		//cout<<("1\n");
////////////  RRMISE
		for (int j=0;j<nparamcom;j++) {
			s=0.0;
			for (int i=0;i<nsel;i++) {d=paretoil[i][j]-enreg2[p].paramvv[j];s += d*d;}
			rrmise_O[j] +=s/enreg2[p].paramvv[j]/enreg2[p].paramvv[j]/double(nsel);// il restera à diviser par ntest
		}
		//cout<<("2\n");
////////////  RMAD
		for (int j=0;j<nparamcom;j++) {
			s=0.0;
                for (int i=0;i<nsel;i++) s += fabs(paretoil[i][j]-enreg2[p].paramvv[j]);
                rmad_O[j] +=s/fabs(enreg2[p].paramvv[j])/(long double)nsel;// il restera à diviser par ntest
		}
		//cout<<("3\n");
////////////  RMSE
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparamcom;j++) {
				switch (k) {
					case 0 : d=paramest[p][j].moy-enreg2[p].paramvv[j];break;
					case 1 : d=paramest[p][j].med-enreg2[p].paramvv[j];break;
					case 2 : d=paramest[p][j].mod-enreg2[p].paramvv[j];break;
				}
				rmse_O[k][j] += d*d/enreg2[p].paramvv[j]/enreg2[p].paramvv[j];
				// il restera à prendre la racine carrée et à diviser par ntest
            }
        }
		//cout<<("4\n");
/////////////// coverages
        long double atest=1.0/(long double)ntest;
        for (int j=0;j<nparamcom;j++) {
			if((paramest[p][j].q025<=enreg2[p].paramvv[j])and(paramest[p][j].q975>=enreg2[p].paramvv[j])) cov95_O[j] += atest;
			if((paramest[p][j].q250<=enreg2[p].paramvv[j])and(paramest[p][j].q750>=enreg2[p].paramvv[j])) cov50_O[j] += atest;
		}
		//cout<<("5\n");
///////////////// factors 2
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparamcom;j++) {
				switch (k) {
					case 0 : if((paramest[p][j].moy>=0.5*enreg2[p].paramvv[j])and(paramest[p][j].moy<=2.0*enreg2[p].paramvv[j])) fac2_O[k][j] += atest;break;
					case 1 : if((paramest[p][j].med>=0.5*enreg2[p].paramvv[j])and(paramest[p][j].med<=2.0*enreg2[p].paramvv[j])) fac2_O[k][j] += atest;break;
					case 2 : if((paramest[p][j].mod>=0.5*enreg2[p].paramvv[j])and(paramest[p][j].mod<=2.0*enreg2[p].paramvv[j])) fac2_O[k][j] += atest;break;
				}
            }
        }
		//cout<<("6\n");
/////////////////// medianes du biais relatif
        for (int k=0;k<3;k++) {
            for (int j=0;j<nparamcom;j++) {
                switch (k) {
                  case 0 : bmed_O[k][j][p]=(paramest[p][j].moy-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];break;
                  case 1 : bmed_O[k][j][p]=(paramest[p][j].med-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];break;
                  case 2 : bmed_O[k][j][p]=(paramest[p][j].mod-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];break;
                }
            }
            //Il restera à calculer la médiane des bmed[k][j]
        }
		//cout<<("7\n");
///////////////// Relative Median Absolute Deviation
        long double *cc;
        cc = new long double[nsel];

        for (int j=0;j<nparamcom;j++) {
            for (int i=0;i<nsel;i++) cc[i] = (fabs(paretoil[i][j]-enreg2[p].paramvv[j]))/enreg2[p].paramvv[j];
			bmedr_O[j][p] = cal_medL(nsel,cc);
            //Il restera à calculer la médiane des bmedr_O[j]
        }            
        delete []cc;
		//cout<<("8\n");
////////////  RMAE
		for (int k=0;k<3;k++) {
			for (int j=0;j<nparamcom;j++) {
				switch (k) {
					case 0 : bmeda_O[k][j][p]=fabs(paramest[p][j].moy-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];break;
					case 1 : bmeda_O[k][j][p]=fabs(paramest[p][j].med-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];break;
					case 2 : bmeda_O[k][j][p]=fabs(paramest[p][j].mod-enreg2[p].paramvv[j])/enreg2[p].paramvv[j];break;
				}
			}
		}
            //Il restera à calculer la médiane des bmeda_O[k][j]

    }

/**
* calcule les différentes statistiques de biais, rmse... des paramètres compo
*/
    void biaisrelC(int ntest,int nsel,int npv,int p) {
        long double s,d;		
//////////////// mean relative bias
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparcompo;j++) {
				switch (k) {
					case 0 : d=paramestcompo[p][j].moy-enreg2[p].paramvvC[j];break;
					case 1 : d=paramestcompo[p][j].med-enreg2[p].paramvvC[j];break;
					case 2 : d=paramestcompo[p][j].mod-enreg2[p].paramvvC[j];break;
				}
				br_C[k][j] +=d/enreg2[p].paramvvC[j]; // il restera à diviser par ntest
				//if ((j<3)and(k==0)) cout <<"paramestcompo["<<p<<"]["<<j<<"].moy="<<paramestcompo[p][j].moy;
				//if ((j<3)and(k==1)) cout <<"paramestcompo["<<p<<"]["<<j<<"].med="<<paramestcompo[p][j].med;
				//if ((j<3)and(k==2)) cout <<"paramestcompo["<<p<<"]["<<j<<"].mod="<<paramestcompo[p][j].mod;
				//cout<<"   ("<<enreg2[p].paramvvC[j]<<")\n";
			}
		}
////////////  RRMISE
		for (int j=0;j<nparcompo;j++) {
			s=0.0;
			for (int i=0;i<nsel;i++) {d=paretoilcompo[i][j]-enreg2[p].paramvvC[j];s += d*d;}
			rrmise_C[j] +=s/enreg2[p].paramvvC[j]/enreg2[p].paramvvC[j]/double(nsel);// il restera à diviser par ntest
		}
////////////  RMAD
		for (int j=0;j<nparcompo;j++) {
			s=0.0;
                for (int i=0;i<nsel;i++) s += fabs(paretoilcompo[i][j]-enreg2[p].paramvvC[j]);
                rmad_C[j] +=s/fabs(enreg2[p].paramvvC[j])/(long double)nsel;// il restera à diviser par ntest
		}
////////////  RMSE
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparcompo;j++) {
				switch (k) {
					case 0 : d=paramestcompo[p][j].moy-enreg2[p].paramvvC[j];break;
					case 1 : d=paramestcompo[p][j].med-enreg2[p].paramvvC[j];break;
					case 2 : d=paramestcompo[p][j].mod-enreg2[p].paramvvC[j];break;
				}
				rmse_C[k][j] += d*d/enreg2[p].paramvvC[j]/enreg2[p].paramvvC[j];
				// il restera à prendre la racine carrée et à diviser par ntest
            }
        }
/////////////// coverages
        long double atest=1.0/(long double)ntest;
        for (int j=0;j<nparcompo;j++) {
			if((paramestcompo[p][j].q025<=enreg2[p].paramvvC[j])and(paramestcompo[p][j].q975>=enreg2[p].paramvvC[j])) cov95_C[j] += atest;
			if((paramestcompo[p][j].q250<=enreg2[p].paramvvC[j])and(paramestcompo[p][j].q750>=enreg2[p].paramvvC[j])) cov50_C[j] += atest;
		}
///////////////// factors 2
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparcompo;j++) {
				switch (k) {
					case 0 : if((paramestcompo[p][j].moy>=0.5*enreg2[p].paramvvC[j])and(paramestcompo[p][j].moy<=2.0*enreg2[p].paramvvC[j])) fac2_C[k][j] += atest;break;
					case 1 : if((paramestcompo[p][j].med>=0.5*enreg2[p].paramvvC[j])and(paramestcompo[p][j].med<=2.0*enreg2[p].paramvvC[j])) fac2_C[k][j] += atest;break;
					case 2 : if((paramestcompo[p][j].mod>=0.5*enreg2[p].paramvvC[j])and(paramestcompo[p][j].mod<=2.0*enreg2[p].paramvvC[j])) fac2_C[k][j] += atest;break;
				}
            }
        }
/////////////////// medianes du biais relatif
        for (int k=0;k<3;k++) {
            for (int j=0;j<nparcompo;j++) {
                switch (k) {
                  case 0 : bmed_C[k][j][p]=(paramestcompo[p][j].moy-enreg2[p].paramvvC[j])/enreg2[p].paramvvC[j];break;
                  case 1 : bmed_C[k][j][p]=(paramestcompo[p][j].med-enreg2[p].paramvvC[j])/enreg2[p].paramvvC[j];break;
                  case 2 : bmed_C[k][j][p]=(paramestcompo[p][j].mod-enreg2[p].paramvvC[j])/enreg2[p].paramvvC[j];break;
                }
            }
            //Il restera à calculer la médiane des bmed[k][j]
        }
///////////////// Relative Median Absolute Deviation
        long double *cc;
        cc = new long double[nsel];

        for (int j=0;j<nparcompo;j++) {
            for (int i=0;i<nsel;i++) cc[i] = (fabs(paretoilcompo[i][j]-enreg2[p].paramvvC[j]))/enreg2[p].paramvvC[j];
			bmedr_C[j][p] = cal_medL(nsel,cc);
            //Il restera à calculer la médiane des bmedr_C[j]
        }            
        delete []cc;
////////////  RMAE
		for (int k=0;k<3;k++) {
			for (int j=0;j<nparcompo;j++) {
				switch (k) {
					case 0 : bmeda_C[k][j][p]=fabs(paramestcompo[p][j].moy-enreg2[p].paramvvC[j])/enreg2[p].paramvvC[j];break;
					case 1 : bmeda_C[k][j][p]=fabs(paramestcompo[p][j].med-enreg2[p].paramvvC[j])/enreg2[p].paramvvC[j];break;
					case 2 : bmeda_C[k][j][p]=fabs(paramestcompo[p][j].mod-enreg2[p].paramvvC[j])/enreg2[p].paramvvC[j];break;
				}
			}
		}
            //Il restera à calculer la médiane des bmeda_O[k][j]
    }


/**
* calcule les différentes statistiques de biais, rmse... des paramètres scaled
*/
    void biaisrelS(int ntest,int nsel,int npv,int p) {
        long double s,d;
//////////////// mean relative bias
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparscaled;j++) {
				switch (k) {
					case 0 : d=paramestscaled[p][j].moy-enreg2[p].paramvvS[j];break;
					case 1 : d=paramestscaled[p][j].med-enreg2[p].paramvvS[j];break;
					case 2 : d=paramestscaled[p][j].mod-enreg2[p].paramvvS[j];break;
				}
				br_S[k][j] +=d/enreg2[p].paramvvS[j]; // il restera à diviser par ntest
			}
		}
		
////////////  RRMISE
		for (int j=0;j<nparscaled;j++) {
			s=0.0;
			for (int i=0;i<nsel;i++) {d=paretoilscaled[i][j]-enreg2[p].paramvvS[j];s += d*d;}
			rrmise_S[j] +=s/enreg2[p].paramvvS[j]/enreg2[p].paramvvS[j]/double(nsel);// il restera à diviser par ntest
		}

////////////  RMAD
		for (int j=0;j<nparscaled;j++) {
			s=0.0;
                for (int i=0;i<nsel;i++) s += fabs(paretoilscaled[i][j]-enreg2[p].paramvvS[j]);
                rmad_S[j] +=s/fabs(enreg2[p].paramvvS[j])/(long double)nsel;// il restera à diviser par ntest
		}
////////////  RMSE
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparscaled;j++) {
				switch (k) {
					case 0 : d=paramestscaled[p][j].moy-enreg2[p].paramvvS[j];break;
					case 1 : d=paramestscaled[p][j].med-enreg2[p].paramvvS[j];break;
					case 2 : d=paramestscaled[p][j].mod-enreg2[p].paramvvS[j];break;
				}
				rmse_S[k][j] += d*d/enreg2[p].paramvvS[j]/enreg2[p].paramvvS[j];
				// il restera à prendre la racine carrée et à diviser par ntest
            }
        }
/////////////// coverages
        long double atest=1.0/(long double)ntest;
        for (int j=0;j<nparscaled;j++) {
			if((paramestscaled[p][j].q025<=enreg2[p].paramvvS[j])and(paramestscaled[p][j].q975>=enreg2[p].paramvvS[j])) cov95_S[j] += atest;
			if((paramestscaled[p][j].q250<=enreg2[p].paramvvS[j])and(paramestscaled[p][j].q750>=enreg2[p].paramvvS[j])) cov50_S[j] += atest;
		}
///////////////// factors 2
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparscaled;j++) {
				switch (k) {
					case 0 : if((paramestscaled[p][j].moy>=0.5*enreg2[p].paramvvS[j])and(paramestscaled[p][j].moy<=2.0*enreg2[p].paramvvS[j])) fac2_S[k][j] += atest;break;
					case 1 : if((paramestscaled[p][j].med>=0.5*enreg2[p].paramvvS[j])and(paramestscaled[p][j].med<=2.0*enreg2[p].paramvvS[j])) fac2_S[k][j] += atest;break;
					case 2 : if((paramestscaled[p][j].mod>=0.5*enreg2[p].paramvvS[j])and(paramestscaled[p][j].mod<=2.0*enreg2[p].paramvvS[j])) fac2_S[k][j] += atest;break;
				}
            }
        }
/////////////////// medianes du biais relatif
        for (int k=0;k<3;k++) {
            for (int j=0;j<nparscaled;j++) {
                switch (k) {
                  case 0 : bmed_S[k][j][p]=(paramestscaled[p][j].moy-enreg2[p].paramvvS[j])/enreg2[p].paramvvS[j];break;
                  case 1 : bmed_S[k][j][p]=(paramestscaled[p][j].med-enreg2[p].paramvvS[j])/enreg2[p].paramvvS[j];break;
                  case 2 : bmed_S[k][j][p]=(paramestscaled[p][j].mod-enreg2[p].paramvvS[j])/enreg2[p].paramvvS[j];break;
                }
            }
            //Il restera à calculer la médiane des bmed[k][j]
        }
///////////////// Relative Median Absolute Deviation
        long double *cc;
        cc = new long double[nsel];

        for (int j=0;j<nparscaled;j++) {
            for (int i=0;i<nsel;i++) cc[i] = (fabs(paretoilscaled[i][j]-enreg2[p].paramvvS[j]))/enreg2[p].paramvvS[j];
			bmedr_S[j][p] = cal_medL(nsel,cc);
            //Il restera à calculer la médiane des bmedr_S[j]
        }            
        delete []cc;
////////////  RMAE
		for (int k=0;k<3;k++) {
			for (int j=0;j<nparscaled;j++) {
				switch (k) {
					case 0 : bmeda_S[k][j][p]=fabs(paramestscaled[p][j].moy-enreg2[p].paramvvS[j])/enreg2[p].paramvvS[j];break;
					case 1 : bmeda_S[k][j][p]=fabs(paramestscaled[p][j].med-enreg2[p].paramvvS[j])/enreg2[p].paramvvS[j];break;
					case 2 : bmeda_S[k][j][p]=fabs(paramestscaled[p][j].mod-enreg2[p].paramvvS[j])/enreg2[p].paramvvS[j];break;
				}
			}
		}
            //Il restera à calculer la médiane des bmeda_O[k][j]
    }

	void finbiaisrelO(int ntest) {
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparamcom;j++) br_O[k][j] /=(long double)ntest;
		}
		for (int j=0;j<nparamcom;j++) rrmise_O[j] /=(long double)ntest;
		for (int j=0;j<nparamcom;j++) rmad_O[j] /=(long double)ntest;
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparamcom;j++) rmse_O[k][j] = sqrt(rmse_O[k][j])/(long double)ntest;
		}
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparamcom;j++) rmb_O[k][j] =cal_medL(ntest,bmed_O[k][j]);
		}
		for (int j=0;j<nparamcom;j++) rmedad_O[j] = cal_medL(ntest,bmedr_O[j]);
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparamcom;j++) rmae_O[k][j] =cal_medL(ntest,bmeda_O[k][j]);
		}
	}
	
	void finbiaisrelC(int ntest) {
		//cout<<"0\n";
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparcompo;j++) br_C[k][j] /=(long double)ntest;
		}
		//cout<<"1\n";
		for (int j=0;j<nparcompo;j++) rrmise_C[j] /=(long double)ntest;cout<<"2\n";
		for (int j=0;j<nparcompo;j++) rmad_C[j] /=(long double)ntest;cout<<"3\n";
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparcompo;j++) rmse_C[k][j] = sqrt(rmse_C[k][j])/(long double)ntest;
		}
		//cout<<"4\n";
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparcompo;j++) rmb_C[k][j] =cal_medL(ntest,bmed_C[k][j]);
		}
		//cout<<"5\n";
		for (int j=0;j<nparcompo;j++) rmedad_C[j] = cal_medL(ntest,bmedr_C[j]);cout<<"6\n";
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparcompo;j++) rmae_C[k][j] =cal_medL(ntest,bmeda_C[k][j]);
		}
		//cout<<"7\n";
	}
	
	void finbiaisrelS(int ntest) {
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparscaled;j++) br_S[k][j] /=(long double)ntest;
		}
		for (int j=0;j<nparscaled;j++) rrmise_S[j] /=(long double)ntest;
		for (int j=0;j<nparscaled;j++) rmad_S[j] /=(long double)ntest;
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparscaled;j++) rmse_S[k][j] = sqrt(rmse_S[k][j])/(long double)ntest;
		}
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparscaled;j++) rmb_S[k][j] =cal_medL(ntest,bmed_S[k][j]);
		}
		for (int j=0;j<nparscaled;j++) rmedad_S[j] = cal_medL(ntest,bmedr_S[j]);
		for (int k=0;k<3;k++) {
            for (int j=0;j<nparscaled;j++) rmae_S[k][j] =cal_medL(ntest,bmeda_S[k][j]);
		}
	}
    
/**
 *   Mise en forme des résultats pour les paramètres originaux
 */ 
	void ecriresO(int ntest,int npv,int nsel) {
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        string nomfiresult;
        long double *x,*mo;
        x=new long double[ntest];
        mo=new long double[4];
        nomfiresult = path + ident + "_bias.txt";
        cout <<"Les résultats sont dans "<<nomfiresult<<"\n";
        ofstream f1;
        f1.open(nomfiresult.c_str());
        f1<<"DIYABC :                 Bias and Mean Square Error Analysis                         "<<asctime(timeinfo)<<"\n";
        f1<<"Data file       : "<<header.datafilename<<"\n";
        f1<<"Reference table : "<<rt.filename<<"\n";
        switch (numtransf) {
              case 1 : f1<<"No transformation of parameters\n";break;
              case 2 : f1<<"Transformation LOG of parameters\n";break;
              case 3 : f1<<"Transformation LOGIT of parameters\n";break;
              case 4 : f1<<"Transformation LOG(TG) of parameters\n";break;
        }
        f1<<"Chosen scenario : "<<rt.scenteste<<"\n";
        f1<<"Number of simulated data sets : "<<rt.nreclus<<"\n";
        f1<<"Number of selected data sets  : "<<nsel<<"\n";
        f1<<"Results based on "<<ntest<<" test data sets\n\n";
        f1<<"                                               Averages\n";
        f1<<"Parameter                True values           Means             Medians             Modes\n";
        for (int j=0;j<nparamcom;j++) {
            //cout<<nomparam[j]<<"\n";
            f1<<nomparamO[j];
            for (int i=0;i<24-(int)nomparamO[j].length();i++) f1<<" ";
            for (int i=0;i<ntest;i++) x[i]=enreg2[i].paramvv[j];mo[0]=cal_moyL(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramest[i][j].moy;mo[1]=cal_moyL(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramest[i][j].med;mo[2]=cal_moyL(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramest[i][j].mod;mo[3]=cal_moyL(ntest,x);
            f1<<setw(10)<<setprecision(3)<<mo[0];
			f1<<setw(18)<<setprecision(3)<<mo[1];
			f1<<setw(18)<<setprecision(3)<<mo[2];
			f1<<setw(18)<<setprecision(3)<<mo[3]<<"\n";
        }
        f1<<"\n                                           Mean Relative Bias\n";
        f1<<"Parameter                           Means          Medians        Modes\n";
        for (int j=0;j<nparamcom;j++) {
            //cout<<nomparam[j]<<"\n";
            f1<<nomparamO[j];
            for(int i=0;i<33-(int)nomparamO[j].length();i++) f1<<" ";
			f1<<setiosflags(ios::fixed)<<setw(8)<<setprecision(3)<<br_O[0][j];
			f1<<setw(15)<<setprecision(3)<<br_O[1][j];
			f1<<setw(15)<<setprecision(3)<<br_O[2][j]<<"\n";
        }
        f1<<"\n                            RRMISE            RMeanAD            Square root of mean square error/true value\n";
        f1<<"Parameter                                                         Mean             Median             Mode\n";
        for (int j=0;j<nparamcom;j++) {
            //cout<<nomparam[j]<<"\n";
            f1<<nomparamO[j];
            for(int i=0;i<24-(int)nomparamO[j].length();i++) f1<<" ";
			f1<<setw(10)<<setprecision(3)<<rrmise_O[j];
			f1<<setw(18)<<setprecision(3)<<rmad_O[j];
			f1<<setw(18)<<setprecision(3)<<rmse_O[0][j];
			f1<<setw(19)<<setprecision(3)<<rmse_O[1][j];
			f1<<setw(17)<<setprecision(3)<<rmse_O[2][j]<<"\n";
        }
        f1<<"\n                                                                 Factor 2        Factor 2        Factor 2\n";
        f1<<"Parameter               50%% Coverage        95%% Coverage         (Mean)          (Median)        (Mode)  \n";
        for (int j=0;j<nparamcom;j++) {
            //cout<<nomparam[j]<<"\n";
            f1<<nomparamO[j];
            for(int i=0;i<24-(int)nomparamO[j].length();i++) f1<<" ";
			f1<<setw(10)<<setprecision(3)<<cov50_O[j];
			f1<<setw(18)<<setprecision(3)<<cov95_O[j];
			f1<<setw(18)<<setprecision(3)<<fac2_O[0][j];
			f1<<setw(19)<<setprecision(3)<<fac2_O[1][j];
			f1<<setw(17)<<setprecision(3)<<fac2_O[2][j]<<"\n";
        }
        f1<<"\n                                          Median Relative Bias\n";
        f1<<"Parameter                                     Means          Medians        Modes\n";
        for (int j=0;j<nparamcom;j++) {
            //cout<<nomparam[j]<<"\n";
			f1<<nomparamO[j];
            for(int i=0;i<43-(int)nomparamO[j].length();i++) f1<<" ";
				f1<<setw(8)<<setprecision(3)<<rmb_O[0][j];
				f1<<setw(15)<<setprecision(3)<<rmb_O[1][j];
				f1<<setw(15)<<setprecision(3)<<rmb_O[2][j]<<"\n";
        }
        f1<<"\n                             RMedAD        Median of the absolute error/true value\n";
        f1<<"Parameter                                     Means          Medians        Modes\n";
        for (int j=0;j<nparamcom;j++) {
            //cout<<nomparam[j]<<"\n";
            f1<<nomparamO[j];
            for(int i=0;i<24-(int)nomparamO[j].length();i++)f1<<" ";
			f1<<setw(11)<<setprecision(3)<<rmedad_O[j];
			f1<<setw(16)<<setprecision(3)<<rmae_O[0][j];
			f1<<setw(16)<<setprecision(3)<<rmae_O[1][j];
			f1<<setw(16)<<setprecision(3)<<rmae_O[2][j]<<"\n";
        }
         f1.close();

    }

/**
 *   Mise en forme des résultats pour les paramètres composites
 */ 
	void ecriresC(int ntest,int npv,int nsel) {
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        string nomfiresult;
        long double *x,*mo;
        x=new long double[ntest];
        mo=new long double[4];
        nomfiresult = path + ident + "_biascompo.txt";
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
        for (int j=0;j<nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparamC[j].c_str());
            for (int i=0;i<24-(int)nomparamC[j].length();i++)fprintf(f1," ");
            for (int i=0;i<ntest;i++) x[i]=enreg2[i].paramvvC[j];mo[0]=cal_moyL(ntest,x); 
            for (int i=0;i<ntest;i++) x[i]=paramestcompo[i][j].moy;mo[1]=cal_moyL(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramestcompo[i][j].med;mo[2]=cal_moyL(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramestcompo[i][j].mod;mo[3]=cal_moyL(ntest,x);
            fprintf(f1,"  %8.3Le          %8.3Le          %8.3Le          %8.3Le\n",mo[0],mo[1],mo[2],mo[3]);
        }
        fprintf(f1,"\n                                           Mean Relative Bias\n");
        fprintf(f1,"Parameter                           Means          Medians        Modes\n");
        for (int j=0;j<nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparamC[j].c_str());
            for(int i=0;i<33-(int)nomparamC[j].length();i++)fprintf(f1," ");
            if (br_C[0][j]<0) fprintf(f1,"  %6.3Lf",br_C[0][j]); else fprintf(f1,"  %6.3Lf",br_C[0][j]);
            if (br_C[1][j]<0) fprintf(f1,"         %6.3Lf",br_C[1][j]); else fprintf(f1,"         %6.3Lf",br_C[1][j]);
            if (br_C[2][j]<0) fprintf(f1,"         %6.3Lf\n",br_C[2][j]); else fprintf(f1,"         %6.3Lf\n",br_C[2][j]);
        }
        fprintf(f1,"\n                            RRMISE            RMeanAD            Square root of mean square error/true value\n");
        fprintf(f1,"Parameter                                                         Mean             Median             Mode\n");
        for (int j=0;j<nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparamC[j].c_str());
            for(int i=0;i<24-(int)nomparamC[j].length();i++)fprintf(f1," ");
            fprintf(f1,"    %6.3Lf            %6.3Lf            %6.3Lf             %6.3Lf           %6.3Lf\n",rrmise_C[j],rmad_C[j],rmse_C[0][j],rmse_C[1][j],rmse_C[2][j]);
        }
        fprintf(f1,"\n                                                                 Factor 2        Factor 2        Factor 2\n");
        fprintf(f1,"Parameter               50%% Coverage        95%% Coverage         (Mean)          (Median)        (Mode)  \n");
        for (int j=0;j<nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparamC[j].c_str());
            for(int i=0;i<24-(int)nomparamC[j].length();i++)fprintf(f1," ");
            fprintf(f1,"    %6.3Lf            %6.3Lf            %6.3Lf             %6.3Lf           %6.3Lf\n",cov50_C[j],cov95_C[j],fac2_C[0][j],fac2_C[1][j],fac2_C[2][j]);
        }
        fprintf(f1,"\n                                          Median Relative Bias\n");
        fprintf(f1,"Parameter                                     Means          Medians        Modes\n");
        for (int j=0;j<nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparamC[j].c_str());
            for(int i=0;i<43-(int)nomparamC[j].length();i++)fprintf(f1," ");
            if (rmb_C[0][j]<0) fprintf(f1,"  %6.3Lf",rmb_C[0][j]); else fprintf(f1,"  %6.3Lf",rmb_C[0][j]);
            if (rmb_C[1][j]<0) fprintf(f1,"         %6.3Lf",rmb_C[1][j]); else fprintf(f1,"         %6.3Lf",rmb_C[1][j]);
            if (rmb_C[2][j]<0) fprintf(f1,"         %6.3Lf\n",rmb_C[2][j]); else fprintf(f1,"         %6.3Lf\n",rmb_C[2][j]);
        }
        fprintf(f1,"\n                             RMedAD        Median of the absolute error/true value\n");
        fprintf(f1,"Parameter                                     Means          Medians        Modes\n");
        for (int j=0;j<nparcompo;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparamC[j].c_str());
            for(int i=0;i<24-(int)nomparamC[j].length();i++)fprintf(f1," ");
            fprintf(f1,"     %6.3Lf          %6.3Lf         %6.3Lf         %6.3Lf\n",rmedad_C[j],rmae_C[0][j],rmae_C[1][j],rmae_C[2][j]);
        }
         fclose(f1);

    }

/**
 *   Mise en forme des résultats pour les paramètres scaled
 */ 
	void ecriresS(int ntest,int npv,int nsel) {
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        string nomfiresult;
        long double *x,*mo;
        x=new long double[ntest];
        mo=new long double[4];
        nomfiresult = path + ident + "_biasscaled.txt";
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
        for (int j=0;j<nparscaled;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparamS[j].c_str());
            for (int i=0;i<24-(int)nomparamS[j].length();i++)fprintf(f1," ");
            for (int i=0;i<ntest;i++) x[i]=enreg2[i].paramvvS[j];mo[0]=cal_moyL(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramestscaled[i][j].moy;mo[1]=cal_moyL(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramestscaled[i][j].med;mo[2]=cal_moyL(ntest,x);
            for (int i=0;i<ntest;i++) x[i]=paramestscaled[i][j].mod;mo[3]=cal_moyL(ntest,x);
            fprintf(f1,"  %8.3Le          %8.3Le          %8.3Le          %8.3Le\n",mo[0],mo[1],mo[2],mo[3]);
        }
        fprintf(f1,"\n                                           Mean Relative Bias\n");
        fprintf(f1,"Parameter                           Means          Medians        Modes\n");
        for (int j=0;j<nparscaled;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparamS[j].c_str());
            for(int i=0;i<33-(int)nomparamS[j].length();i++)fprintf(f1," ");
            if (br_S[0][j]<0) fprintf(f1,"  %6.3Lf",br_S[0][j]); else fprintf(f1,"  %6.3Lf",br_S[0][j]);
            if (br_S[1][j]<0) fprintf(f1,"         %6.3Lf",br_S[1][j]); else fprintf(f1,"         %6.3Lf",br_S[1][j]);
            if (br_S[2][j]<0) fprintf(f1,"         %6.3Lf\n",br_S[2][j]); else fprintf(f1,"         %6.3Lf\n",br_S[2][j]);
        }
        fprintf(f1,"\n                            RRMISE            RMeanAD            Square root of mean square error/true value\n");
        fprintf(f1,"Parameter                                                         Mean             Median             Mode\n");
        for (int j=0;j<nparscaled;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparamS[j].c_str());
            for(int i=0;i<24-(int)nomparamS[j].length();i++)fprintf(f1," ");
            fprintf(f1,"    %6.3Lf            %6.3Lf            %6.3Lf             %6.3Lf           %6.3Lf\n",rrmise_S[j],rmad_S[j],rmse_S[0][j],rmse_S[1][j],rmse_S[2][j]);
        }
        fprintf(f1,"\n                                                                 Factor 2        Factor 2        Factor 2\n");
        fprintf(f1,"Parameter               50%% Coverage        95%% Coverage         (Mean)          (Median)        (Mode)  \n");
        for (int j=0;j<nparscaled;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparamS[j].c_str());
            for(int i=0;i<24-(int)nomparamS[j].length();i++)fprintf(f1," ");
            fprintf(f1,"    %6.3Lf            %6.3Lf            %6.3Lf             %6.3Lf           %6.3Lf\n",cov50_S[j],cov95_S[j],fac2_S[0][j],fac2_S[1][j],fac2_S[2][j]);
        }
        fprintf(f1,"\n                                          Median Relative Bias\n");
        fprintf(f1,"Parameter                                     Means          Medians        Modes\n");
        for (int j=0;j<nparscaled;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparamS[j].c_str());
            for(int i=0;i<43-(int)nomparamS[j].length();i++)fprintf(f1," ");
            if (rmb_S[0][j]<0) fprintf(f1,"  %6.3Lf",rmb_S[0][j]); else fprintf(f1,"  %6.3Lf",rmb_S[0][j]);
            if (rmb_S[1][j]<0) fprintf(f1,"         %6.3Lf",rmb_S[1][j]); else fprintf(f1,"         %6.3Lf",rmb_S[1][j]);
            if (rmb_S[2][j]<0) fprintf(f1,"         %6.3Lf\n",rmb_S[2][j]); else fprintf(f1,"         %6.3Lf\n",rmb_S[2][j]);
        }
        fprintf(f1,"\n                             RMedAD        Median of the absolute error/true value\n");
        fprintf(f1,"Parameter                                     Means          Medians        Modes\n");
        for (int j=0;j<nparscaled;j++) {
            //cout<<nomparam[j]<<"\n";
            fprintf(f1,"%s",nomparamS[j].c_str());
            for(int i=0;i<24-(int)nomparamS[j].length();i++)fprintf(f1," ");
            fprintf(f1,"     %6.3Lf          %6.3Lf         %6.3Lf         %6.3Lf\n",rmedad_S[j],rmae_S[0][j],rmae_S[1][j],rmae_S[2][j]);
        }
         fclose(f1);

    }

/*
 * Traitement des paramètres composites (microsat et séquences uniquement)
 */ 
    void setcompo(int p) {
		long double pmut;
        int kk,qq,k=0;
        for (int gr=1;gr<header.ngroupes+1;gr++) {
            if (header.groupe[gr].type==0) {
                if (header.groupe[gr].priormutmoy.constant) {
                    if (header.groupe[gr].priorsnimoy.constant) {
                        pmut = (long double)(header.groupe[gr].mutmoy+header.groupe[gr].snimoy);
                        for (int j=0;j<npar;j++) {
                            if (header.scenario[rt.scenteste-1].histparam[numpar[0][j]].category<2){
                                  enreg2[p].paramvvC[k]=pmut*enreg2[p].paramvv[j];
                                k++;
                            }
                        }
                    } else {
                        kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==2))) kk++;
                        for (int j=0;j<npar;j++) {
                            if (header.scenario[rt.scenteste-1].histparam[numpar[0][j]].category<2){
                                pmut = (long double)header.groupe[gr].mutmoy+(long double)enreg2[p].paramvv[npar+kk];
                                enreg2[p].paramvvC[k]=pmut*enreg2[p].paramvv[j];
                                k++;
                            }
                        }
                    }
                } else {
                    if (header.groupe[gr].priorsnimoy.constant) {
                        kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==0))) kk++;
                        for (int j=0;j<npar;j++) {
                            if (header.scenario[rt.scenteste-1].histparam[numpar[0][j]].category<2){
                                pmut =(long double)enreg2[p].paramvv[npar+kk] +(long double)header.groupe[gr].snimoy;
                                enreg2[p].paramvvC[k]=pmut*enreg2[p].paramvv[j];
                                k++;
                            }
                        }
                    } else {
                        kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==0))) kk++;
                        qq=0;while (not ((header.mutparam[qq].groupe == gr)and(header.mutparam[qq].category ==2))) qq++;
                        for (int j=0;j<npar;j++) {
                            if (header.scenario[rt.scenteste-1].histparam[numpar[0][j]].category<2){
                                pmut =(long double)enreg2[p].paramvv[npar+kk]+(long double)enreg2[p].paramvv[npar+qq];
                                enreg2[p].paramvvC[k]=pmut*enreg2[p].paramvv[j];
                                k++;
                            }
                        }
                    }
                }
            }
            if (header.groupe[gr].type==1) {
                if (header.groupe[gr].priormusmoy.constant) {
                    pmut = (long double)header.groupe[gr].musmoy;
                    for (int j=0;j<npar;j++) {
                        if (header.scenario[rt.scenteste-1].histparam[numpar[0][j]].category<2){
                            enreg2[p].paramvvC[k]=pmut*enreg2[p].paramvv[j];
                            k++;
                        }
                    }
                } else {
                    kk=0;while (not ((header.mutparam[kk].groupe == gr)and(header.mutparam[kk].category ==3))) kk++;
                    for (int j=0;j<npar;j++) {
                        if (header.scenario[rt.scenteste-1].histparam[numpar[0][j]].category<2){
                            pmut = enreg2[p].paramvv[npar+kk];
                            enreg2[p].paramvvC[k]=pmut*enreg2[p].paramvv[j];
                            k++;
                        }
                    }
                }
            }
        }
    }

/**
 * Traitement des paramètres scaled 
 */ 
    void setscaled(int p) {
		long double Ne;
		int k=0,n,nNe=0;
		Ne=0.0;
		for (int j=0;j<header.scenario[rt.scenchoisi[0]-1].npop;j++) {
			for (int ievent=0;ievent<header.scenario[rt.scenchoisi[0]-1].nevent;ievent++) {
				if ((header.scenario[rt.scenchoisi[0]-1].event[ievent].action=='E')and(header.scenario[rt.scenchoisi[0]-1].event[ievent].pop==j+1)){
					if (header.scenario[rt.scenchoisi[0]-1].histparam[j].prior.constant) {
						n=0;
						while (header.scenario[rt.scenchoisi[0]-1].histparam[n].name!=header.scenario[rt.scenchoisi[0]-1].ne0[j].name) n++;
						Ne += (long double)header.scenario[rt.scenchoisi[0]-1].histparam[n].prior.mini;
						//cout<<"j="<<j<< "   Ne="<<header.scenario[rt.scenchoisi[0]-1].histparam[n].prior.mini<<"\n";
					}
					else {
						//cout<<"  prior.variable"<< header.scenario[rt.scenchoisi[0]-1].ne0[j].name  <<"\n";
						n=0;
						while(enreg2[p].name[n].compare(header.scenario[rt.scenchoisi[0]-1].ne0[j].name)!=0) n++;
						Ne +=(long double)enreg2[p].paramvv[n];
						nNe++;
						//cout<<"j="<<j<<"  "<< "   Ne="<< enreg2[p].paramvv[n] <<"\n";
					}
				}
			}
		}
		//cout<<"Ne="<<Ne<<"\n";
		Ne = Ne/(long double)nNe;
		cout<<"Ne="<<Ne<<"\n";
		k=0;
		for (int j=0;j<npar;j++) {
			if (header.scenario[rt.scenteste-1].histparam[numpar[0][j]].category<2){
				enreg2[p].paramvvS[k]=enreg2[p].paramvv[j]/Ne;
				k++;
			}
		}
	}
/**
 * Interprête la ligne de paramètres de l'option biais et lance les calculs correspondants
 */
	void dobias(string opt,  int seed){
		cout<<"debut de dobias\n";
        int nstatOK, iprog,nprog,bidule;
        int nrec = 0, nsel = 0,ns,nrecpos = 0,ntest = 0,np,ng,npv,nn,ncond,nt,*paordre,*paordreabs;
        string *ss,s,*ss1,s0,s1,sg, entetelog, *paname;
		float *stat_obs;
		long double **matC;
        string bidon;
		long double **phistar, **phistarcompo, **phistarscaled;
        progressfilename = path + ident + "_progress.txt";
		scurfile = path + "pseudo-observed_datasets_"+ ident +".txt";
        cout<<scurfile<<"\n";
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
				scaled=(s1.find("s")!=string::npos);
                if ((original)and(not composite)and(not scaled)) cout <<"paramètres  originaux  ";
                if ((not original)and(composite)and(not scaled)) cout <<"paramètres  composites  ";
                if ((not original)and(not composite)and(scaled)) cout <<"paramètres  scaled  ";
				if ((original)and(composite)and(not scaled)) cout <<"paramètres  originaux et composites ";
				if ((original)and(not composite)and(scaled)) cout <<"paramètres  originaux et scaled ";
				if ((not original)and(composite)and(scaled)) cout <<"paramètres  composites et scaled ";
				if ((original)and(composite)and(scaled)) cout <<"paramètres  originaux, composites et scaled ";
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
            }
        }
        npv = rt.nparam[rt.scenteste-1];
        enreg = new enregC[ntest];
        for (int p=0;p<ntest;p++) {
            enreg[p].stat = new float[header.nstat];
            enreg[p].param = new float[npv];
            enreg[p].numscen = rt.scenteste;
        }
		cout<<"avant dosimultabref\n";
        ps.dosimultabref(header,ntest,false,multithread,true,rt.scenteste,seed,1);
		cout<<"apres dosimultabref\n";
		//header.entete=header.entetehist+header.entetemut0+header.entetestat;
        nprog=10*ntest+6;iprog=5;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
        header.readHeader(headerfilename);cout<<"apres readHeader\n";
        for (int p=0;p<ntest;p++) {delete []enreg[p].param;delete []enreg[p].stat;}delete []enreg;
        rt.nscenchoisi=1;rt.scenchoisi = new int[rt.nscenchoisi];rt.scenchoisi[0]=rt.scenteste;
        det_numpar();
        cout<<"naparmcom = "<<nparamcom<<"   nparcomp = "<<nparcompo<<"   nparscaled = "<<nparscaled<<"\n";
		cout<<"header.nstat="<<header.nstat<<"    rt.nstat="<<rt.nstat<<"\n";
        enreg2 = new enreC[ntest];
        for (int p=0;p<ntest;p++) {
            enreg2[p].stat = new double[header.nstat];
            enreg2[p].paramvv = new double[nparamcom];
			enreg2[p].name = new string[nparamcom];
			if (composite) enreg2[p].paramvvC = new double[nparcompo];
			if (scaled) enreg2[p].paramvvS = new double[nparscaled];
            enreg2[p].numscen = rt.scenchoisi[0];
        }
        cout<<"courantfilename="<<scurfile<<"\n";
        ifstream file(scurfile.c_str(), ios::in);
        getline(file,entetelog);
		cout<<"entetelog=\n"<<entetelog<<"\n";
		paname = splitwords(entetelog," ",&nt);
		paordreabs = new int[npv];
		paordre =new int[npv];
		for (int i=0;i<npv;i++)  {
			if (i<rt.nhistparam[rt.scenteste-1]) {
				int k=0;
				while ((paname[k].compare(rt.histparam[rt.scenteste-1][i].name)!=0)and(k<nt-1)) k++;
				paordreabs[i]=k;
				cout <<"i="<<i<<"    k="<<k<<"      ";
				cout <<rt.histparam[rt.scenteste-1][i].name<<"   "<<paname[k]<<"\n";
			} else {
				cout <<"i="<<i<<"   "<<paname[i+1]<<"\n";
				paordreabs[i]=i+1;
			}
			cout <<"paordreabs["<<i<<"]="<<paordreabs[i]<<"\n";
		}		
		for (int i=0;i<npv;i++) {
			paordre[i] = i;
			if (i<rt.nhistparam[rt.scenteste-1]) {
				paordre[i]=0;
				for (int j=0;j<npv;j++) if (paordreabs[i]>paordreabs[j]) paordre[i]++;
			}
			cout<<"paordre["<<i<<"]="<<paordre[i]<<"\n";
		}
        for (int p=0;p<ntest;p++) {
			getline(file,bidon);
			cout<<"ligne "<<p+1<<" du fichier\n";
			cout<<bidon<<"\n";
			//cout<<bidon<<"\n";
			ss = splitwords(bidon," ",&ns);
			cout<<"ns="<<ns<<"    npv="<<npv<<"\n";
			enreg2[p].numscen=atoi(ss[0].c_str());
			cout<<"bias.cpp   npv="<<npv<<"\n";
			for (int i=0;i<npv;i++)  {
				enreg2[p].paramvv[i]=atof(ss[paordre[i]+1].c_str());
				if (i<rt.nhistparam[rt.scenteste-1]) enreg2[p].name[i]=rt.histparam[rt.scenteste-1][i].name;
				else enreg2[p].name[i]=paname[i+1];
				if(p==0)cout<<"enreg2[p].name["<<i<<"]="<<enreg2[p].name[i]<<" = "<<enreg2[p].paramvv[i]<<"\n";
			}
			cout<<"avant setcompo\n";
			if(composite) setcompo(p);
			cout<<"avant setscaled\n";
			if(scaled) setscaled(p);
			cout<<"apres setscaled\n";
			for (int i=0;i<rt.nstat;i++) enreg2[p].stat[i]=atof(ss[i+1+npv].c_str());
			cout<<"apres copie des stat\n";
        }
        file.close();
        nstatOK = rt.cal_varstat();                       cout<<"apres cal_varstat\n";
        stat_obs = new float[rt.nstat];
        rt.alloue_enrsel(nsel);
        cout<<"apres rt.alloue_enrsel\n";
		if (original) {
			initbiasO(ntest,nsel,nparamcom);
			phistar = new long double* [nsel];
			for (int i=0;i<nsel;i++) phistar[i] = new long double[nparamcom];
		}
		if (composite) {
			initbiasC(ntest,nsel,nparcompo);
			phistarcompo = new long double* [nsel];
			for (int i=0;i<nsel;i++) phistarcompo[i] = new long double[nparcompo];
		}
		if (scaled) {
			initbiasS(ntest,nsel,nparscaled);
			phistarscaled = new long double* [nsel];
			for (int i=0;i<nsel;i++) phistarscaled[i] = new long double[nparscaled];
		}
        for (int p=0;p<ntest;p++) {
            printf("\nanalysing data test %3d \n",p+1);
            for (int j=0;j<rt.nstat;j++) stat_obs[j]=enreg2[p].stat[j];
            rt.cal_dist(nrec,nsel,stat_obs);          if (debuglevel==11)   cout<<"apres cal_dist\n";
            iprog +=6;fprog.open(progressfilename.c_str(),ios::out);fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
            if (p<1) det_nomparam();
			rempli_mat(nsel,stat_obs);            	if (debuglevel==11)	cout<<"apres rempli_mat\n";
			matC = cal_matC(nsel);               	if (debuglevel==11)	cout<<"cal_matC\n";
			
			if (original) {
				recalparamO(nsel);                    	if (debuglevel==11)	cout<<"apres recalparamO\n";
				rempli_parsim(nsel,nparamcom);  		if (debuglevel==11)   cout<<"apres rempli_mat\n";
				local_regression(nsel,nparamcom,matC);     	if (debuglevel==11)	cout<<"apres local_regression\n";
				calphistarO(nsel,phistar);          	if (debuglevel==11)   cout<<"apres calphistarO\n";
				paramest[p] = calparstatO(nsel,phistar);      	if (debuglevel==11)   cout<<"apres calparstatO\n";
				for (int i=0;i<nsel;i++) {
					for (int j=0;j<nparamcom;j++) paretoil[i][j] = phistar[i][j];
					//for (int j=0;j<nparamcom;j++) cout<<"  "<<phistar[i][j]<<" ("<<enreg2[p].paramvv[j] <<")";
					//cout<<"\n";
				}
				biaisrelO(ntest,nsel,npv,p);			if (debuglevel==11)   cout<<"apres biaisrelO\n";
			}
			if (composite) {
				recalparamC(nsel);                  	if (debuglevel==11)	cout<<"apres recalparamC\n";
				rempli_parsim(nsel,nparcompo); 			if (debuglevel==11)    cout<<"apres rempli_mat\n";
				local_regression(nsel,nparcompo,matC); 	if (debuglevel==11)	cout<<"apres local_regression\n";
				//phistarcompo = new long double*[nsel];
				//for (int i=0;i<nsel;i++) phistarcompo[i] = new long double[nparcompo];
				calphistarC(nsel,phistarcompo);	 		if (debuglevel==11)	cout<<"apres calphistarC\n";
				paramestcompo[p] = calparstatC(nsel,phistarcompo);	if (debuglevel==11)    cout<<"apres calparstatC\n";
				for (int i=0;i<nsel;i++) {
					for (int j=0;j<nparcompo;j++) paretoilcompo[i][j] = phistarcompo[i][j];
				}
				//cout<<"avant delete phistarcompo\n";
				//for (int i=0;i<nsel;i++) delete []phistarcompo[i];delete phistarcompo;
				//cout<<"apres delete phistarcompo\n";
														if (debuglevel==11) cout <<"avant biaisrelC\n";
				biaisrelC(ntest,nsel,npv,p);			if (debuglevel==11) cout <<"après biaisrelC\n";
				
			}
			if (scaled) {
				recalparamS(nsel);                    	if (debuglevel==11)	cout<<"apres recalparamS\n";
				rempli_parsim(nsel,nparscaled); 		if (debuglevel==11)   cout<<"apres rempli_mat\n";
				local_regression(nsel,nparscaled,matC); if (debuglevel==11)	cout<<"apres local_regression\n";
				calphistarS(nsel,phistarscaled);	  	if (debuglevel==11)	cout<<"apres calphistarS\n";
				paramestscaled[p] = calparstatS(nsel,phistarscaled);	if (debuglevel==11)	cout<<"apres calparstatS\n";
				for (int i=0;i<nsel;i++) {
					for (int j=0;j<nparscaled;j++) paretoilscaled[i][j] = phistarscaled[i][j];
					//for (int j=0;j<nparscaled;j++) cout<<"  "<<phistarscaled[i][j]<<" ("<<enreg2[p].paramvvS[j] <<")";
					//cout<<"\n";
				}
				biaisrelS(ntest,nsel,npv,p);			if (debuglevel==11)   cout<<"apres biaisrelS\n";
			}
			for (int i=0;i<nstatOKsel+1;i++) delete [] matC[i];delete [] matC;
			delete_mat(nsel);
            //printf("\nanalysing data test %3d \n",p+1);
            //for (int j=0;j<npar;j++) printf(" %6.0e (%8.2e %8.2e %8.2e) ",enreg2[p].paramvv[j],paramest[p][j].moy,paramest[p][j].med,paramest[p][j].mod);cout<<"\n";
            iprog +=4;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
        }
        rt.desalloue_enrsel(nsel);
//		for (int i=0;i<nsel;i++) delete []phistar[i];delete phistar;
        if (original) {
			finbiaisrelO(ntest);
			ecriresO(ntest,npv,nsel);
		}
         if (composite) {
			finbiaisrelC(ntest);
			ecriresC(ntest,npv,nsel);
		}
        if (scaled) {
			finbiaisrelS(ntest);			if (debuglevel==11)	cout<<"apres biaisrelS\n";
			ecriresS(ntest,npv,nsel);		if (debuglevel==11)	cout<<"apres ecriresS\n";
		}
       iprog +=1;fprog.open(progressfilename.c_str());fprog<<iprog<<"   "<<nprog<<"\n";fprog.close();
    }
