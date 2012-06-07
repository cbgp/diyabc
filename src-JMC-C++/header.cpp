/*



*/
#include <cstdio>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
//#include <complex>
#include <utility>
#include <string>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <time.h>

#include "randomgenerator.h"
#include "mesutils.h"
#include "data.h"
#include "history.h"
#include "particuleC.h"


extern int debuglevel;
/*
#ifndef PARTICULEC
#include "particuleC.cpp"
#define PARTICULEC
#endif

#ifndef DATA
#include "data.cpp"
#define DATA
#endif
*/

#include "header.h"
extern string * stat_type;
extern int* stat_num;
extern string path;

void MutParameterC::ecris() {
	cout<<"    groupe="<<this->groupe<<"   category="<<this->category<<"\n";
	//prior.ecris();
}




void HeaderC::libere() {

	this->dataobs.libere();
	//cout<<"apres dataobs.libere\n";
	if(this->nconditions>0) delete []this->condition;
	// for(int i=0;i<this->nscenarios;i++) this->scenario[i].libere();
	//cout<<"apres scenario[i].libere\n";
	delete []this->scenario;
	// this->scen.libere();
	//for(int i=1;i<ngroupes+1;i++) this->groupe[i].libere();
	delete []this->groupe;
	//cout<<"apres delete group\n";
}





void HeaderC::assignloc(int gr){
	this->groupe[gr].nloc = 0;
	for (int loc=0;loc<dataobs.nloc;loc++) {
		if (dataobs.locus[loc].groupe==gr) this->groupe[gr].nloc++;
	}
	if (debuglevel==2) cout<<"assignloc nloc="<<this->groupe[gr].nloc<<"\n";
	this->groupe[gr].loc = new int[this->groupe[gr].nloc];
	int iloc=-1;
	for (int i=0;i<dataobs.nloc;i++) {
		if (dataobs.locus[i].groupe==gr) {iloc++;this->groupe[gr].loc[iloc] = i;}
	}
}

/*ScenarioC superscen() {
                int neventm,nn0m,nsampm,nparamm,nparamvarm,nconditionm;
                neventm=0;    for (int i=0;i<this->nscenarios;i++) {if (neventm<this->scenario[i].nevent)          neventm=this->scenario[i].nevent;}
                nn0m=0;       for (int i=0;i<this->nscenarios;i++) {if (nn0m<this->scenario[i].nn0)                nn0m=this->scenario[i].nn0;}
                nsampm=0;     for (int i=0;i<this->nscenarios;i++) {if (nsampm<this->scenario[i].nsamp)            nsampm=this->scenario[i].nsamp;}
                nparamm=0;    for (int i=0;i<this->nscenarios;i++) {if (nparamm<this->scenario[i].nparam)         nparamm=this->scenario[i].nparam;}
                nparamvarm=0; for (int i=0;i<this->nscenarios;i++) {if (nparamvarm<this->scenario[i].nparamvar)    nparamvarm=this->scenario[i].nparamvar;}
                nconditionm=0;for (int i=0;i<this->nscenarios;i++) {if (nconditionm<this->scenario[i].nconditions) nconditionm=this->scenario[i].nconditions;}
                cout<<"neventm="<<neventm<<"   nn0m="<<nn0m<<"   nsampm="<<nsampm<<"   nparamm="<<nparamm<<"nparamvarm="<<nparamvarm<<"   ncondm="<<nconditionm<<"\n";
                this->scen.event = new EventC[neventm];
                this->scen.ne0   = new Ne0C[nn0m];

                for (int i=0;i<nn0m;i++) this->scen.ne0[i].name="blabla";
                this->scen.time_sample = new int[nsampm];
                this->scen.histparam = new HistParameterC[nparamm];
                this->scen.paramvar = new double[nparamvarm+3];
                if (nconditionm>0) this->scen.condition = new ConditionC[nconditionm];

        }*/

int HeaderC::readHeaderDebut(ifstream & file){
	string s1;
	int nl;
	getline(file,this->datafilename);nl++;
	this->pathbase=path;
	int error = this->dataobs.loadfromfile(path+this->datafilename);
	if (error != 0) {
		this->message = this->dataobs.message;
		throw std::runtime_error(message);
		return error;
	}
	if (debuglevel==1) cout<<"lecture du fichier data terminée\n";
	getline(file,s1);nl++;
	this->nparamtot=getwordint(s1,1);		cout<<"nparamtot="<<this->nparamtot<<"\n";
	this->nstat=getwordint(s1,4);			cout<<"nstat="<<this->nstat<<"\n";
	//cout<<"avant scenarios\n";fflush(stdin);
	return 0;
}

int HeaderC::readHeaderScenarios(ifstream & file){
		//Partie Scenarios
	string s1;
	int nl = 0,nm,nf;
	getline(file,s1);nl++;   //cout<<s1<<"\n";     //ligne vide
	getline(file,s1);nl++;    //cout<<s1<<"\n";    // nombre de scenarios
	this->nscenarios=getwordint(s1,1); cout<<this->nscenarios<<" scenario(s)\n";
	string** sl; sl = new string*[this->nscenarios];
	int * nlscen; nlscen = new int[this->nscenarios];

	this->scenario = new ScenarioC[this->nscenarios];
	for (int i=0;i<this->nscenarios;i++) {nlscen[i]=getwordint(s1,3+i);}
	for (int i=0;i<this->nscenarios;i++) {
		sl[i] = new string[nlscen[i]];
		getline(file,s1); nl++;   cout<<s1<<"\n";
		if (s1.find("scenario")!=0) {
			this->message = "Error when reading header.txt file :keyword <scenario> expected at line "+IntToString(nl)+". Check the number of lines of each scenario at line 4.";
			throw std::runtime_error(message);
			return 1;
		}
		this->scenario[i].number = getwordint(s1,2);
		this->scenario[i].prior_proba = getwordfloat(s1,3);
		this->scenario[i].nparam = 0;
		this->scenario[i].nparamvar=0;
		for (int j=0;j<nlscen[i];j++) {getline(file,sl[i][j]);nl++;/*cout<<sl[i][j]<<"\n";*/}
		this->message = this->scenario[i].read_events(nlscen[i],sl[i]);
		if (this->message!="") {
			this->message = "Error when reading  header.txt file : "+this->message+" in scenario "+IntToString(i+1);
			return 1;
		}
		//this->scenario[i].ecris();
		cout<<"apres read_events\n";
		this->message = this->scenario[i].checklogic();
		if (this->message!="") {
			this->message = "Error when reading  header.txt file : "+this->message+" in scenario "+IntToString(i+1);
			return 1;
		}
	}
	this->reference=false;
	/*for (int ievent=0;ievent<this->scenario[0].nevent;ievent++){
		if (this->scenario[0].event[ievent].action == 'R') {
			this->dataobs.nsample++;
			this->dataobs.nind.resize(this->dataobs.nsample);
			nm = this->scenario[0].event[ievent].nindMref;
			nf = this->scenario[0].event[ievent].nindFref;
			this->dataobs.nind[this->dataobs.nsample-1] = nm+nf;
			this->dataobs.indivsexe.resize(this->dataobs.nsample);
			this->dataobs.indivsexe[this->dataobs.nsample-1].resize(this->dataobs.nind[this->dataobs.nsample-1]);
			for (int i=0;i<nm;i++) this->dataobs.indivsexe[this->dataobs.nsample-1][i] = 1;
			for (int i=nm;i<nm+nf;i++) this->dataobs.indivsexe[this->dataobs.nsample-1][i] = 2;
			for (int cat=0;cat<5;cat++) {
				if (this->dataobs.catexist[cat]) {
					this->dataobs.ss[cat].resize(this->dataobs.nsample);
					for (int sa=0;sa<this->dataobs.nsample;sa++) {
						if (this->scenario[0].event[ievent].pop == sa+1) {
							this->dataobs.ss[cat][sa]=0;
							for (int ind=0;ind<nm+nf;ind++) {
								if ((cat==0)or((cat==2)and(this->dataobs.indivsexe[sa][ind]==2))) this->dataobs.ss[cat][sa] +=2;
								else if (not((cat==3)and(this->dataobs.indivsexe[sa][ind]==2))) this->dataobs.ss[cat][sa] +=1;
							}
						}
					}
				}
			}
		}
		if (this->scenario[0].event[ievent].nindMref+this->scenario[0].event[ievent].nindFref>0) this->reference=true;
	}*/
	for (int sa=0;sa<this->dataobs.nsample;sa++) cout<<this->dataobs.ss[0][sa]<<"   ";cout<<"\n";

	for (int i=0;i<this->nscenarios;i++) delete []sl[i];
	delete [] sl;
	delete [] nlscen;
	if (debuglevel==2) cout<<"header.txt : fin de la lecture de la partie scénarios\n";
	return 0;
}

int HeaderC::readHeaderHistParam(ifstream & file){
	string s1, s2, *ss, *ss2;
	int nss, j, k;

	//cout <<"avant histparam\n";fflush(stdin);
	getline(file,s1);       //ligne vide
	getline(file,s1);
	//cout<<s1<<"\n";
	ss=splitwords(s1," ",&nss);
	s2=ss[3].substr(1,ss[3].length()-2);
	ss2=splitwords(s2,",",&nss);
	this->nparamtot = atoi(ss2[0].c_str());
	this->nconditions = atoi(ss2[1].c_str());
	cout<<"dans header nconditions="<<this->nconditions<<"\n";
	delete [] ss2;
	this->histparam = new HistParameterC[this->nparamtot];
	if (this->nconditions>0) this->condition = new ConditionC[this->nconditions];
	delete [] ss;
	for (int i=0;i<this->nparamtot;i++) {
		getline(file,s1);
		cout<<s1<<"\n";
		ss=splitwords(s1," ",&nss);
		this->histparam[i].name=ss[0];
		//cout<<this->histparam[i].name<<"\n";
		if (ss[1]=="N") this->histparam[i].category = 0;
		else if  (ss[1]=="T") this->histparam[i].category = 1;
		else if  (ss[1]=="A") this->histparam[i].category = 2;
		this->histparam[i].prior.readprior(ss[2]);
		//    cout<<"readHeader "<<this->histparam[i].name;
		//    if (this->histparam[i].prior.constant) cout<<"   constant\n"; else cout<<"   variable\n";
	}
	delete [] ss;
	this->drawuntil=true;
	if (this->nconditions>0) {
		this->condition = new ConditionC[this->nconditions];
		for (int i=0;i<this->nconditions;i++) {
			getline(file,s1);
			cout<<s1<<"\n";
			this->condition[i].readcondition(s1);
		}
		getline(file,s1);
		if (s1 != "DRAW UNTIL") this->drawuntil=false; else  this->drawuntil=true;
	}
	if (debuglevel==2) cout<<"header.txt : fin de la lecture de la partie priors des paramètres démographiques\n";
	//retour sur les parametres spécifiques à chaque scenario;
	//cout <<"avant retour sur histparam/scenario\n";fflush(stdin);
	for (int i=0;i<this->nscenarios;i++) {
		this->scenario[i].nparamvar=0;
		for (int j=0;j<this->scenario[i].nparam;j++) {
			int k=0;
			while (this->scenario[i].histparam[j].name!=this->histparam[k].name) {k++;}
			//this->scenario[i].histparam[j].prior = copyprior(this->histparam[k].prior);
			this->scenario[i].histparam[j].prior = this->histparam[k].prior;
			if (not this->histparam[k].prior.constant) {
				//this->scenario[i].paramvar[this->scenario[i].nparamvar].prior = copyprior(this->histparam[k].prior);
				this->scenario[i].nparamvar++;
			}
			this->scenario[i].histparam[j].ecris();
		}
		//cout<<"scenario "<<i<<"   "<<this->scenario[i].nparam<<" param et "<<this->scenario[i].nparamvar<<" paramvar\n "<<flush;
	}
	//retour sur les conditions spécifiques à chaque scenario
	//cout <<"avant retour sur conditions\n";fflush(stdin);
	if (this->nconditions>0) {
		for (int i=0;i<this->nscenarios;i++) {
			int nc=0;
			for (j=0;j<this->nconditions;j++) {
				int np=0;
				for (k=0;k<this->scenario[i].nparam;k++) {
					//cout<<this->condition[j].param1<<"   "<<this->condition[j].param2<<"   "<<this->scenario[i].histparam[k].name<<"  np="<<np<<"\n";
					if (this->condition[j].param1 == this->scenario[i].histparam[k].name) np++;
					if (this->condition[j].param2 == this->scenario[i].histparam[k].name) np++;
				}
				if (np==2) nc++;
			}
			this->scenario[i].nconditions=nc;
			//cout <<"header.scenario["<<i<<"].nconditions="<<this->scenario[i].nconditions<<"\n";
			this->scenario[i].condition = new ConditionC[nc];
			nc=0;
			while (nc<this->scenario[i].nconditions) {
				for (j=0;j<this->nconditions;j++) {
					int np=0;
					for (k=0;k<this->scenario[i].nparam;k++) {
						if (this->condition[j].param1 == this->scenario[i].histparam[k].name) np++;
						if (this->condition[j].param2 == this->scenario[i].histparam[k].name) np++;
					}
					if (np==2) {
						this->scenario[i].condition[nc] = this->condition[j];
						this->scenario[i].condition[nc].ecris();
						nc++;
					}
				}
			}
			//cout <<"dans readheader  \n";

		}
	} else for (int i=0;i<this->nscenarios;i++) this->scenario[i].nconditions=0;
	return 0;
}

int HeaderC::readHeaderLoci(ifstream & file){
	string s1, *ss;
	int k, k1, nss, gr, grm;
		//Partie loci description
	//cout <<"avant partie loci\n";fflush(stdin);
	getline(file,s1);       //ligne vide
	getline(file,s1);       //ligne "loci description"
	if (this->dataobs.filetype==0){ //fichier GENEPOP
		grm=1;                  //nombre maximal de groupes
		for (int loc=0;loc<this->dataobs.nloc;loc++){
			getline(file,s1);
			ss=splitwords(s1," ",&nss);
			k=0;while (ss[k].find("[")==string::npos) k++;
			if (ss[k]=="[M]") {
				s1=ss[k+1].substr(1,ss[k+1].length());gr=atoi(s1.c_str());this->dataobs.locus[loc].groupe=gr;if (gr>grm) grm=gr;
				this->dataobs.locus[loc].motif_size=atoi(ss[k+2].c_str());this->dataobs.locus[loc].motif_range=atoi(ss[k+3].c_str());
			}
			else if (ss[k]=="[S]") {
				s1=ss[k+1].substr(1,ss[k+1].length());gr=atoi(s1.c_str());this->dataobs.locus[loc].groupe=gr;if (gr>grm) grm=gr;
				this->dataobs.locus[loc].mutsit.resize(this->dataobs.locus[loc].dnalength);
				//cout<<this->dataobs.locus[loc].dnalength<<"\n";
				//this->dataobs.locus[loc].dnalength=atoi(ss[k+2].c_str());  //inutile variable déjà renseignée
			}
			delete [] ss;
		}
	} else {			//fichier SNP
		this->ngroupes=getwordint(s1,3); cout<<s1<<"\n";cout<<"this->ngroupes="<<this->ngroupes<<"\n";
		this->groupe = new LocusGroupC[this->ngroupes+1];
		this->groupe[0].nloc = this->dataobs.nloc;
		for (int loc=0;loc<this->dataobs.nloc;loc++) this->dataobs.locus[loc].groupe=0;
		for (gr=1;gr<=this->ngroupes;gr++) {
			getline(file,s1);
			this->groupe[gr].nloc=getwordint(s1,1);
			this->groupe[0].nloc -= this->groupe[gr].nloc;
			this->groupe[gr].loc = new int[this->groupe[gr].nloc];
			int prem = getwordint(s1,5)-1;
			//cout<<"prem="<<prem<<"\n";
			if (s1.find("<A>")!=string::npos) this->groupe[gr].type=10;
			else if (s1.find("<H>")!=string::npos) this->groupe[gr].type=11;
			else if (s1.find("<X>")!=string::npos) this->groupe[gr].type=12;
			else if (s1.find("<Y>")!=string::npos) this->groupe[gr].type=13;
			else if (s1.find("<M>")!=string::npos) this->groupe[gr].type=14;
			cout<<"this->groupe[gr].type = "<<this->groupe[gr].type<<"\n";
			k1=0;
			for (int loc=prem;loc<this->dataobs.nloc;loc++) {
				//cout<<this->dataobs.locus[loc].type<<" ";
				if (this->dataobs.locus[loc].type==this->groupe[gr].type) {
					this->groupe[gr].loc[k1]=loc;
					this->dataobs.locus[loc].groupe=gr;
					k1++;
				}
				if (k1 == this->groupe[gr].nloc) break;
			}
			cout<<"\napres la boucle  k1="<<k1<<"\n";
		}
		//cout<<"this->groupe[0].nloc = "<<this->groupe[0].nloc<<"\n";
		if (this->groupe[0].nloc>0) {
			this->groupe[0].loc = new int[this->groupe[0].nloc];
			k1=0;
			for (int loc=0;loc<this->dataobs.nloc;loc++) {
				//cout<<"loc="<<loc<<"\n";
				if (this->dataobs.locus[loc].groupe==0) {this->groupe[0].loc[k1]=loc;k1++;}
			}
		}
	}
	if (debuglevel==2) cout<<"header.txt : fin de la lecture de la partie description des locus\n";

	return 0;
}



int HeaderC::readHeaderGroupPrior(ifstream & file){
	string s1, *ss = NULL, *ss1 = NULL;
	int gr, nss, nss1, j;
	if (this->dataobs.filetype==0){

		//cout <<"avant partie group priors\n";fflush(stdin);
		getline(file,s1);       //ligne vide
		getline(file,s1);       //ligne "group prior"
		this->ngroupes=getwordint(s1,3);
		//cout<<"header.ngroupes="<<this->ngroupes;
		this->groupe = new LocusGroupC[this->ngroupes+1];
		this->assignloc(0);
		cout<<"on attaque les groupes : analyse des priors nombre de groupes="<<this->ngroupes <<"\n";
		for (gr=1;gr<=this->ngroupes;gr++){
			getline(file,s1);
			cout<<s1<<"\n";
			ss=splitwords(s1," ",&nss);
			this->assignloc(gr);
			if (ss[2]=="[M]") {
				this->groupe[gr].type=0;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormutmoy.readprior(ss1[1]);delete [] ss1;
				cout<<"readHeaderGroupPrior priormutmoy.constant="<<this->groupe[gr].priormutmoy.constant<<"\n";
				this->groupe[gr].priormutmoy.fixed=false;
				if (this->groupe[gr].priormutmoy.constant) this->groupe[gr].mutmoy=this->groupe[gr].priormutmoy.mini;
				else {this->groupe[gr].mutmoy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormutloc.readprior(ss1[1]);delete [] ss1;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorPmoy.readprior(ss1[1]);delete [] ss1;
				this->groupe[gr].priorPmoy.fixed=false;
				if (this->groupe[gr].priorPmoy.constant) this->groupe[gr].Pmoy=this->groupe[gr].priorPmoy.mini;
				else {this->groupe[gr].Pmoy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}

				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorPloc.readprior(ss1[1]);delete [] ss1;
				//cout<<"Ploc    ";this->groupe[gr].priorPloc.ecris();

				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorsnimoy.readprior(ss1[1]);delete [] ss1;
				this->groupe[gr].priorsnimoy.fixed=false;
				if (this->groupe[gr].priorsnimoy.constant) this->groupe[gr].snimoy=this->groupe[gr].priorsnimoy.mini;
				else {this->groupe[gr].snimoy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorsniloc.readprior(ss1[1]);delete [] ss1;
				//cout<<"sniloc  ";this->groupe[gr].priorsniloc.ecris();

			} else if (ss[2]=="[S]") {
				this->groupe[gr].type=1;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormusmoy.readprior(ss1[1]);delete [] ss1;
				this->groupe[gr].priormusmoy.fixed=false;
				if (this->groupe[gr].priormusmoy.constant) this->groupe[gr].musmoy=this->groupe[gr].priormusmoy.mini;
				else {this->groupe[gr].musmoy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}

				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormusloc.readprior(ss1[1]);delete [] ss1;

				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork1moy.readprior(ss1[1]);delete [] ss1;
				this->groupe[gr].priork1moy.fixed=false;
				if (this->groupe[gr].priork1moy.constant) this->groupe[gr].k1moy=this->groupe[gr].priork1moy.mini;
				else this->groupe[gr].k1moy=-1.0;

				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork1loc.readprior(ss1[1]);delete [] ss1;

				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork2moy.readprior(ss1[1]);delete [] ss1;
				this->groupe[gr].priork2moy.fixed=false;
				if (this->groupe[gr].priork2moy.constant) this->groupe[gr].k2moy=this->groupe[gr].priork2moy.mini;
				else this->groupe[gr].k2moy=-1.0;

				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork2loc.readprior(ss1[1]);delete [] ss1;

				getline(file,s1);ss1=splitwords(s1," ",&nss1);
				this->groupe[gr].p_fixe=atof(ss1[2].c_str());this->groupe[gr].gams=atof(ss1[3].c_str());
				if (ss1[1]=="JK") this->groupe[gr].mutmod=0;
				else if (ss1[1]=="K2P") this->groupe[gr].mutmod=1;
				else if (ss1[1]=="HKY") this->groupe[gr].mutmod=2;
				else if (ss1[1]=="TN") this->groupe[gr].mutmod=3;
				//cout<<"mutmod = "<<this->groupe[gr].mutmod <<"\n";
				if (this->groupe[gr].mutmod>0) {
					if (not this->groupe[gr].priork1moy.constant)for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}
				}
				if (this->groupe[gr].mutmod==3) {
					if (not this->groupe[gr].priork2moy.constant)for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}
				}
				if(ss1 != NULL) delete [] ss1; 
				ss1 = NULL;
			} // fin du if portant sur ss[2]
			if(ss != NULL) delete [] ss; ss = NULL;
		}
		cout<<"debuglevel = "<<debuglevel<<"\n";
		if (debuglevel==2) cout<<"header.txt : fin de la lecture de la partie définition des groupes\n";
		//Mise à jour des locus séquences
		MwcGen mwc;
		mwc.randinit(999,time(NULL));
		int nsv;
		bool nouveau;
		if (debuglevel==2) cout<<"avant la boucle sur les locus\n";
		for (int loc=0;loc<this->dataobs.nloc;loc++){
			gr=this->dataobs.locus[loc].groupe;
			if ((this->dataobs.locus[loc].type>4)and(this->dataobs.locus[loc].type<10)and(gr>0)){
				nsv = floor(this->dataobs.locus[loc].dnalength*(1.0-0.01*this->groupe[gr].p_fixe)+0.5);
				if (debuglevel==2) cout<<"nsv = "<<nsv<<"\n";
				if (debuglevel==2) cout<<"mutsit.size = "<<this->dataobs.locus[loc].mutsit.size()<<"\n";
				if (debuglevel==2) cout<<"this->groupe[gr].gams = "<<this->groupe[gr].gams<<"\n";
				if (debuglevel==2) cout<<"this->dataobs.locus[loc].dnalength = "<<this->dataobs.locus[loc].dnalength<<"\n";
				for (int i=0;i<this->dataobs.locus[loc].dnalength;i++) {this->dataobs.locus[loc].mutsit[i] = mwc.ggamma3(1.0,this->groupe[gr].gams);}
				if (debuglevel==2) cout<<"apres tirage dans gamma3\n";
				int *sitefix;
				sitefix=new int[this->dataobs.locus[loc].dnalength-nsv];
				for (int i=0;i<this->dataobs.locus[loc].dnalength-nsv;i++) {
					if (i==0) sitefix[i]=mwc.rand0(this->dataobs.locus[loc].dnalength);
					else {
						do {
							sitefix[i]=mwc.rand0(this->dataobs.locus[loc].dnalength);
							nouveau=true;j=0;
							while((nouveau)and(j<i)) {nouveau=(sitefix[i]!=sitefix[j]);j++;}
						} while (not nouveau);
					}
					this->dataobs.locus[loc].mutsit[i] = 0.0;
				}
				delete [] sitefix;
				double s=0.0;
				for (int i=0;i<this->dataobs.locus[loc].dnalength;i++) s += this->dataobs.locus[loc].mutsit[i];
				for (int i=0;i<this->dataobs.locus[loc].dnalength;i++) this->dataobs.locus[loc].mutsit[i] /=s;
			}
		}
		if (debuglevel==2) cout<<"header.txt : fin de la mise à jour des locus séquences\n";
		//cout<<"avant la mise à jour des paramvar\n";fflush(stdin);
		if(ss != NULL) delete [] ss; ss = NULL;
	} //fin de la condition fichier=GENEPOP
	//Mise à jour des paramvar
	for (int i=0;i<this->nscenarios;i++) {
		this->scenario[i].paramvar = new double[this->scenario[i].nparamvar];
		for (int j=0;j<this->scenario[i].nparamvar;j++)this->scenario[i].paramvar[j]=-1.0;
		//this->scenario[i].ecris();
	}

	return 0;
}

int HeaderC::readHeaderGroupStat(ifstream & file) {
	string s1, *ss, *ss1;
	int j, k, nss, nss1, gr;
	//Partie group statistics
	if (debuglevel==2) cout<<"debut des group stat\n";
	this->nstat=0;
	getline(file,s1);       //ligne vide
	getline(file,s1);       //ligne "group group statistics"
	//cout <<"s1="<<s1<<"\n";
	for (gr=1;gr<=this->ngroupes;gr++) {
		getline(file,s1);
		//cout <<"s1="<<s1<<"\n";
		ss=splitwords(s1," ",&nss);

		//cout <<"s1="<<s1<<"   ss[3]="<< ss[3] <<"   atoi = "<< atoi(ss[3].c_str()) <<"\n";
		this->groupe[gr].nstat = getwordint(ss[nss-1],0);
		//cout << this->groupe[gr].nstat <<"\n";
		this->nstat +=this->groupe[gr].nstat;
		this->groupe[gr].sumstat = new StatC[this->groupe[gr].nstat];
		delete [] ss;
		k=0;
		vector <StatsnpC> statsnp;
		StatsnpC stsnp;
		bool trouve;
		statsnp.resize(0);
		int catsnp;
		if (debuglevel==2) cout<<"nstat="<<this->groupe[gr].nstat<<"\n";
		while (k<this->groupe[gr].nstat) {
			if (debuglevel==2) cout <<"stat "<<k<<"    groupe "<<gr<<"\n";
			getline(file,s1);
			if (debuglevel==2) cout <<"s1="<<s1<<"\n";
			ss=splitwords(s1," ",&nss);
			if (debuglevel==2) cout << ss[0]<<"\n";
			j=0;while (ss[0]!=stat_type[j]) {
				//if (debuglevel==2)	cout << ss[0] << " " << j << " "<< stat_type[j] << endl;
				j++;
			}
			if (debuglevel==2) cout<<"j="<<j<<"\n";
			if (this->groupe[gr].type==0) {   //MICROSAT
				if (stat_num[j]<5) {
					for (int i=1;i<nss;i++) {
						this->groupe[gr].sumstat[k].cat=stat_num[j];
						this->groupe[gr].sumstat[k].samp=atoi(ss[i].c_str());
						k++;
					}
				} else if (stat_num[j]<12) {
					for (int i=1;i<nss;i++) {
						this->groupe[gr].sumstat[k].cat=stat_num[j];
						ss1=splitwords(ss[i],"&",&nss1);
						this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
						this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
						k++;
						delete [] ss1;
					}
				} else if (stat_num[j]==12) {
					for (int i=1;i<nss;i++) {
						this->groupe[gr].sumstat[k].cat=stat_num[j];
						ss1=splitwords(ss[i],"&",&nss1);
						this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
						this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
						this->groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
						k++;
						delete [] ss1;
					}
				}
			} else if (this->groupe[gr].type==1) {   //DNA SEQUENCE
				if (stat_num[j]>-9) {
					for (int i=1;i<nss;i++) {
						this->groupe[gr].sumstat[k].cat=stat_num[j];
						this->groupe[gr].sumstat[k].samp=atoi(ss[i].c_str());
						k++;
					}
				} else if (stat_num[j]>-14) {
					for (int i=1;i<nss;i++) {
						this->groupe[gr].sumstat[k].cat=stat_num[j];
						ss1=splitwords(ss[i],"&",&nss1);
						this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
						this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
						k++;
						delete [] ss1;
					}
				} else if (stat_num[j]==-14) {
					for (int i=1;i<nss;i++) {
						this->groupe[gr].sumstat[k].cat=stat_num[j];
						ss1=splitwords(ss[i],"&",&nss1);
						this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
						this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
						this->groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
						k++;
						delete [] ss1;
					}
				}
			} else if (this->groupe[gr].type>9) {   //SNP
				cout<<"statnum="<<stat_num[j]<<"\n";
				if (stat_num[j]<50) {
					catsnp = (stat_num[j]-21)/4;
					if (debuglevel==2) cout<<"stat_num["<<j<<"]="<<stat_num[j]<<"   catsnp="<<catsnp<<"\n";
				}
				if (stat_num[j]<25) {
					if (debuglevel==2) cout<<"nss="<<nss<<"\n";
					for (int i=1;i<nss;i++) {
						if (debuglevel==2) cout<<"i="<<i<<"\n";
						this->groupe[gr].sumstat[k].cat=stat_num[j];
						this->groupe[gr].sumstat[k].samp=atoi(ss[i].c_str());
						trouve=false;
						if (debuglevel==2) cout<<"statsnp.size="<<statsnp.size()<<"\n";
						if (statsnp.size()>0){
							for (int jj=0; jj < (int)statsnp.size();jj++) {
								trouve=((statsnp[jj].cat==catsnp)and(statsnp[jj].samp==this->groupe[gr].sumstat[k].samp));
								if (trouve) {this->groupe[gr].sumstat[k].numsnp=jj;break;}
							}
						}
						if (debuglevel==2) cout<<"trouve="<<trouve<<"\n";
						if (not trouve) {
							stsnp.cat=catsnp;
							stsnp.samp=this->groupe[gr].sumstat[k].samp;
							stsnp.defined=false;
							stsnp.sorted=false;
							this->groupe[gr].sumstat[k].numsnp=statsnp.size();
							statsnp.push_back(stsnp);
						}
						k++;
					}
				} else if ((stat_num[j]>24)and(stat_num[j]<33)) {
					for (int i=1;i<nss;i++) {
						this->groupe[gr].sumstat[k].cat=stat_num[j];
						ss1=splitwords(ss[i],"&",&nss1);
						this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
						this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
						delete [] ss1;
						trouve=false;
						if (statsnp.size()>0){
							for (int jj=0; jj < (int)statsnp.size(); jj++) {
								trouve=((statsnp[jj].cat==catsnp)and(statsnp[jj].samp==this->groupe[gr].sumstat[k].samp)and(statsnp[jj].samp1==this->groupe[gr].sumstat[k].samp1));
								if (trouve) {this->groupe[gr].sumstat[k].numsnp=jj;break;}
							}
						}
						//cout<<"statsnp.size = "<<statsnp.size()<<"   trouve = "<<trouve<<"\n";
						if (not trouve) {
							stsnp.cat=catsnp;
							stsnp.samp=this->groupe[gr].sumstat[k].samp;
							stsnp.samp1=this->groupe[gr].sumstat[k].samp1;
							stsnp.defined=false;
							stsnp.sorted=false;
							this->groupe[gr].sumstat[k].numsnp=statsnp.size();
							statsnp.push_back(stsnp);
						}
						//cout<<"numsnp = "<<statsnp.size()<<"\n";
						k++;
					}
				} else if ((stat_num[j]>32)and (stat_num[j]<50)) {
					for (int i=1;i<nss;i++) {
						this->groupe[gr].sumstat[k].cat=stat_num[j];
						ss1=splitwords(ss[i],"&",&nss1);
						this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
						this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
						this->groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
						delete [] ss1;
						trouve=false;
						if (statsnp.size()>0){
							for (int jj = 0; jj < (int)statsnp.size(); jj++) {
								trouve=((statsnp[jj].cat==catsnp)and(statsnp[jj].samp==this->groupe[gr].sumstat[k].samp)and(statsnp[jj].samp1==this->groupe[gr].sumstat[k].samp1)and(statsnp[jj].samp2==this->groupe[gr].sumstat[k].samp2));
								if (trouve) {this->groupe[gr].sumstat[k].numsnp=jj;break;}
							}
						}
						if (not trouve) {
							stsnp.cat=catsnp;
							stsnp.samp=this->groupe[gr].sumstat[k].samp;
							stsnp.samp1=this->groupe[gr].sumstat[k].samp1;
							stsnp.samp2=this->groupe[gr].sumstat[k].samp2;
							stsnp.defined=false;
							stsnp.sorted=false;
							this->groupe[gr].sumstat[k].numsnp=statsnp.size();
							statsnp.push_back(stsnp);
						}
						k++;
					} 
				} else if (stat_num[j]==50) {
					this->groupe[gr].sumstat[k].cat=stat_num[j];
					k++;
					cout<<"k="<<k<<"\n";
				}
			}
			if (debuglevel==2) cout<<"fin de la stat "<<k<<"\n";
		}
		delete [] ss;
		this->groupe[gr].nstatsnp=statsnp.size();
		//cout<<"this->groupe[gr].nstatsnp="<<this->groupe[gr].nstatsnp<<"\n";
		if (this->groupe[gr].nstatsnp>0){
			this->groupe[gr].sumstatsnp = new StatsnpC[this->groupe[gr].nstatsnp];
			for (int i=0;i<this->groupe[gr].nstatsnp;i++){
				this->groupe[gr].sumstatsnp[i].cat=statsnp[i].cat;
				this->groupe[gr].sumstatsnp[i].samp=statsnp[i].samp;
				this->groupe[gr].sumstatsnp[i].samp1=statsnp[i].samp1;
				this->groupe[gr].sumstatsnp[i].samp2=statsnp[i].samp2;
				this->groupe[gr].sumstatsnp[i].defined=statsnp[i].defined;
				this->groupe[gr].sumstatsnp[i].sorted=statsnp[i].sorted;
			}
			statsnp.clear();
		}
		//for (int i=0;i<this->groupe[gr].nstat;i++) cout<<this->groupe[gr].sumstat[i].cat<<"   "<<this->groupe[gr].sumstat[i].numsnp<<"\n";
	}
	if (debuglevel==2) cout<<"header.txt : fin de la lecture des summary stats\n";
	return 0;
}



int HeaderC::buildSuperScen(){
	//cout<<"avant superscen\n";
	this->scen.nevent=0;
	for (int i=0;i<this->nscenarios;i++) {if (this->scen.nevent<this->scenario[i].nevent) this->scen.nevent=this->scenario[i].nevent;}
	this->scen.nn0=0;
	for (int i=0;i<this->nscenarios;i++) {if (this->scen.nn0<this->scenario[i].nn0)       this->scen.nn0 =this->scenario[i].nn0;}
	this->scen.nsamp=0;
	for (int i=0;i<this->nscenarios;i++) {if (this->scen.nsamp<this->scenario[i].nsamp)   this->scen.nsamp=this->scenario[i].nsamp;}
	this->scen.nparam=0;
	for (int i=0;i<this->nscenarios;i++) {if (this->scen.nparam<this->scenario[i].nparam)  this->scen.nparam=this->scenario[i].nparam;}
	this->scen.nparamvar=0;
	for (int i=0;i<this->nscenarios;i++) {if (this->scen.nparamvar<this->scenario[i].nparamvar) this->scen.nparamvar=this->scenario[i].nparamvar;}
	this->scen.nconditions=0;
	for (int i=0;i<this->nscenarios;i++) {if (this->scen.nconditions<this->scenario[i].nconditions) this->scen.nconditions=this->scenario[i].nconditions;}
	this->scen.event = new EventC[this->scen.nevent];
	this->scen.ne0   = new Ne0C[this->scen.nn0];

	this->scen.time_sample = new int[this->scen.nsamp];
	this->scen.histparam = new HistParameterC[this->scen.nparam];
	this->scen.paramvar = new double[this->scen.nparamvar+3];
	if (this->scen.nconditions>0) this->scen.condition = new ConditionC[this->scen.nconditions];
	for (int i=0;i<this->scen.nsamp;i++) this->scen.time_sample[i]=0;
	for (int i=0;i<this->scen.nparamvar+3;i++) this->scen.paramvar[i]=-1;
	PriorC pr;pr.loi="uniforme";pr.mini=0.0;pr.maxi=1.0;pr.ndec=3;pr.constant=false;
	HistParameterC pp;pp.name="bidon";pp.category=0;pp.value=-1; pp.prior = pr; //pp.prior=copyprior(pr);
	for (int i=0;i<this->scen.nparam;i++)  this->scen.histparam[i] = pp;
	//this->scen.ecris();
	//cout<<"apres superscen\n";
	if (debuglevel==2) cout<<"header.txt : fin de l'établissement du superscen\n";
	//for(int i=0;i<this->nscenarios;i++) this->scenario[i].ecris();
	return 0;
}

int HeaderC::buildMutParam(){
	int gr;
	this->nparamut=0;
	for (gr=1;gr<=this->ngroupes;gr++) {
		if (this->groupe[gr].type==0) {
			if (not this->groupe[gr].priormutmoy.constant) this->nparamut++;
			if (not this->groupe[gr].priorPmoy.constant) this->nparamut++;
			if (not this->groupe[gr].priorsnimoy.constant) this->nparamut++;
		} else if (this->groupe[gr].type==1) {
			if (not this->groupe[gr].priormusmoy.constant) this->nparamut++;
			//cout<<"calcul de nparamut\n";
			if (this->groupe[gr].mutmod>0) {if (not this->groupe[gr].priork1moy.constant) this->nparamut++;}
			if (this->groupe[gr].mutmod==3){if (not this->groupe[gr].priork2moy.constant) this->nparamut++;}
			cout<<"fin du calcul de nparamut = "<<this->nparamut<<"\n";
		}
	}
	this->mutparam = new MutParameterC[nparamut];
	this->nparamut=0;
	for (gr=1;gr<=this->ngroupes;gr++) {
		if (this->groupe[gr].type==0) {
			if (not this->groupe[gr].priormutmoy.constant) {
				this->mutparam[nparamut].name="umic_"+IntToString(gr);
				this->mutparam[nparamut].groupe=gr;
				this->mutparam[nparamut].category=0;
				//this->mutparam[nparamut].prior = copyprior(this->groupe[gr].priormutmoy);
				this->mutparam[nparamut].prior = this->groupe[gr].priormutmoy;
				this->nparamut++;
			}
			if (not this->groupe[gr].priorPmoy.constant) {
				this->mutparam[nparamut].name="Pmic_"+IntToString(gr);
				this->mutparam[nparamut].groupe=gr;
				this->mutparam[nparamut].category=1;
				//this->mutparam[nparamut].prior = copyprior(this->groupe[gr].priorPmoy);
				this->mutparam[nparamut].prior = this->groupe[gr].priorPmoy;
				this->nparamut++;
			}
			if (not this->groupe[gr].priorsnimoy.constant) {
				this->mutparam[nparamut].name="snimic_"+IntToString(gr);
				this->mutparam[nparamut].groupe=gr;
				this->mutparam[nparamut].category=2;
				// this->mutparam[nparamut].prior = copyprior(this->groupe[gr].priorsnimoy);
				this->mutparam[nparamut].prior = this->groupe[gr].priorsnimoy;
				this->nparamut++;
			}
		} else if (this->groupe[gr].type==1) {
			if (not this->groupe[gr].priormusmoy.constant){
				this->mutparam[nparamut].name="useq_"+IntToString(gr);
				this->mutparam[nparamut].groupe=gr;
				this->mutparam[nparamut].category=3;
				// this->mutparam[nparamut].prior = copyprior(this->groupe[gr].priormusmoy);
				this->mutparam[nparamut].prior = this->groupe[gr].priormusmoy;
				this->nparamut++;
			}
			if ((this->groupe[gr].mutmod>0)and(not this->groupe[gr].priork1moy.constant)) {
				this->mutparam[nparamut].name="k1seq_"+IntToString(gr);
				this->mutparam[nparamut].groupe=gr;
				this->mutparam[nparamut].category=4;
				// this->mutparam[nparamut].prior = copyprior(this->groupe[gr].priork1moy);
				this->mutparam[nparamut].prior = this->groupe[gr].priork1moy;
				this->nparamut++;
			}
			if ((this->groupe[gr].mutmod==3)and(not this->groupe[gr].priork2moy.constant)) {
				this->mutparam[nparamut].name="k2seq_"+IntToString(gr);
				this->mutparam[nparamut].groupe=gr;
				this->mutparam[nparamut].category=5;
				// this->mutparam[nparamut].prior = copyprior(this->groupe[gr].priork2moy);
				this->mutparam[nparamut].prior = this->groupe[gr].priork2moy;
				this->nparamut++;
			}
		}
	}
	if (debuglevel==2) cout<<"header.txt : fin de l'établissement des paramètres mutationnels   nparamut="<<this->nparamut<<"\n";
	return 0;
}

int HeaderC::readHeaderEntete(ifstream & file){
	string s1, *ss;
	int nss;
	//Entete du fichier reftable
	getline(file,s1);       //ligne vide
	getline(file,this->entete);     //ligne entete
	this->statname =new string[this->nstat];
	ss=splitwords(entete," ",&nss);
	
	if (debuglevel==2) cout<<"nss="<<nss<<"   nparamtot="<<nparamtot<<"   nparamut="<<nparamut<<"   nstat="<<nstat<<"\n";
	for (int i=nstat-1;i>=0;i--) this->statname[i]=ss[--nss];
	this->entetehist=this->entete.substr(0,this->entete.length()-14*(nparamut+nstat));
	if (nparamut>0) this->entetemut=this->entete.substr(this->entetehist.length(),14*nparamut);else this->entetemut="";
	this->entetestat=this->entete.substr(this->entetehist.length()+this->entetemut.length(),14*nstat);
	if (debuglevel==2) cout<<"les trois entetes\n";
	if (debuglevel==2) cout<<">>>"<<this->entetehist<<"<<<\n";
	if (debuglevel==2) cout<<">>>"<<this->entetemut<<"<<<\n";
	if (debuglevel==2) cout<<">>>"<<this->entetestat<<"<<<\n";
	for (int i=0;i<this->nstat;i++) cout<<this->statname[i]<<"   ";cout<<"\n";
	delete []ss;
	//cout<<"scenarios à la fin de readheader\n";
	return 0;
}

int HeaderC::readHeader(string headerfilename){
	string s1; // string s2,**sl,*ss,*ss1,*ss2;
	int error = 0; // int *nlscen,nss,nss1,j,k,gr,grm,k1,cat,nl=0;
	cout<<"debut de readheader\n";
	//cout<<"readHeader headerfilename = "<<headerfilename<<"\n";
	ifstream file(headerfilename.c_str(), ios::in);
	if (file == NULL) {
		this->message = "Header file "+headerfilename+" not found";
		cout<<this->message<<"\n";
		throw std::runtime_error(this->message);
		return 1;
	} else this->message="";

	error = readHeaderDebut(file); cout<<"----------------------------------------apres readHeaderDebut\n";
	if(error != 0) return error;

	error = readHeaderScenarios(file);cout<<"----------------------------------------apres readHeaderScenarios\n";
	if(error != 0) return error;

	error = readHeaderHistParam(file);cout<<"----------------------------------------apres readHeaderHistParam\n";
	if(error != 0) return error;

	error = readHeaderLoci(file);cout<<"----------------------------------------apres readHeaderLoci\n";
	if(error != 0) return error;

	error = readHeaderGroupPrior(file);cout<<"----------------------------------------apres readHeaderGroupPrior\n";
	if(error != 0) return error;

	error = buildSuperScen();cout<<"----------------------------------------apres buildSuperScen\n";
	if(error != 0) return error;

	error = readHeaderGroupStat(file);cout<<"----------------------------------------apres readHeaderGroupStat\n";
	if(error != 0) return error;

	error = buildMutParam();cout<<"----------------------------------------apres buildMutParam\n";
	if(error != 0) return error;

	error = readHeaderEntete(file);cout<<"----------------------------------------apres readHeaderEntete\n";
	if(error != 0) return error;
    this->threshold = 0.;
	if (not file.eof()){
		getline(file,s1);
		getline(file,s1);
		this->reffreqmin = getwordfloat(s1,0);
		getline(file, s1);
		this->particuleobs.locpol = getwordfloat(s1,0);
		getline(file, s1);
		this->threshold = getwordfloat(s1, 0);
		cout <<"refreqmin="<<this->reffreqmin<<"   locpol="<<this->particuleobs.locpol<<"   threshold="<<this->threshold<<"\n";
	}
	return 0;
}

int HeaderC::readHeadersimDebut(ifstream & file){
	string s1,*ss;
	int nss,*nf,*nm;
	if (debuglevel==2) cout<<"debut de readheadersim\n";
	getline(file, s1);
	ss = splitwords(s1," ",&nss);
	this->datafilename=ss[0];
	this->nsimfile=atoi(ss[1].c_str());
	if (nss>2) this->dataobs.sexratio=atof(ss[2].c_str()); else this->dataobs.sexratio=0.5;
	getline(file,s1);
	delete []ss;
	ss = splitwords(s1," ",&nss);
	this->dataobs.nsample=atoi(ss[0].c_str());
	this->dataobs.nind.resize(this->dataobs.nsample);
	this->dataobs.indivsexe.resize(this->dataobs.nsample);
	nf=new int[this->dataobs.nsample];
	nm=new int[this->dataobs.nsample];
	for (int i=0;i<this->dataobs.nsample;i++) {
		getline(file,s1);
		delete []ss;
		ss = splitwords(s1," ",&nss);
		nf[i]=atoi(ss[0].c_str());
		nm[i]=atoi(ss[1].c_str());
		this->dataobs.nind[i]=nf[i]+nm[i];
		this->dataobs.indivsexe[i].resize(this->dataobs.nind[i]);
		for (int j=0;j<nf[i];j++) this->dataobs.indivsexe[i][j]=2;
		for (int j=nf[i];j<this->dataobs.nind[i];j++) this->dataobs.indivsexe[i][j]=1;
	}
	delete []ss;
	cout<<"nom générique : "<<this->datafilename<<"\n";
	cout<<"nombre de fichiers à simuler = "<<this->nsimfile<<"\n";
	cout<<"nombre d'échantillons = "<<this->dataobs.nsample<<"\n";
	for (int i=0;i<this->dataobs.nsample;i++) {
		cout<<"Echantillon "<<i+1<<"  :  "<<nf[i]<<" femelles et "<<this->dataobs.nind[i]-nf[i]<<" males\n";
	}
	this->dataobs.nmisshap=0;
	this->dataobs.nmissnuc=0;
	return 0;
}

int HeaderC::readHeadersimScenario(ifstream & file){
	string s1,*ss,*sl;
	int nss,nlscen;
	getline(file,s1);   //cout<<s1<<"\n";     //ligne vide
	getline(file,s1);    //cout<<s1<<"\n";    // nombre de scenarios
	this->nscenarios=1; //cout<<this->nscenarios<<"\n";
	ss=splitwords(s1," ",&nss);
	nlscen=getwordint(ss[1],1);
	this->scenario = new ScenarioC[this->nscenarios];
	sl = new string[nlscen];
	this->scenario[0].number = 1;
	this->scenario[0].prior_proba = 1.0;
	this->scenario[0].nparam = 0;
	this->scenario[0].nparamvar=0;
	for (int j=0;j<nlscen;j++) {getline(file,sl[j]);/*cout<<sl[j]<<"\n";*/}
	this->scenario[0].read_events(nlscen,sl);
	//cout<<"apres read_events\n";
	delete [] sl;
	delete [] ss;
	if (debuglevel==2) cout<<"header.txt : fin de la lecture de la partie scénario(s)\n";
	return 0;
}

int HeaderC::readHeadersimHistParam(std::ifstream & file){
	string s1,*ss;
	int nss;
	getline(file,s1);       //ligne vide
	getline(file,s1);
	//cout<<s1<<"\n";
	ss=splitwords(s1," ",&nss);
	this->nparamtot=getwordint(ss[2],1);
	this->scenario[0].nconditions=0;
	delete []ss;
	this->histparam = new HistParameterC[this->nparamtot];
	this->scenario[0].histparam = new HistParameterC[this->nparamtot];
	this->scenario[0].nparam = this->nparamtot;
	for (int i=0;i<this->nparamtot;i++) {
		getline(file,s1);  //cout<<s1<<"\n";
		ss=splitwords(s1," ",&nss);
		this->histparam[i].name=ss[0];
		if (ss[1]=="N") this->histparam[i].category = 0;
		else if  (ss[1]=="T") this->histparam[i].category = 1;
		else if  (ss[1]=="A") this->histparam[i].category = 2;
		this->histparam[i].value=atof(ss[2].c_str());
		this->scenario[0].histparam[i] = this->histparam[i];
		this->scenario[0].histparam[i].prior.constant=true;
		this->scenario[0].histparam[i].prior.fixed=true;
	}
	delete [] ss;
	if (debuglevel==2) cout<<"header.txt : fin de la lecture de la partie priors des paramètres démographiques\n";
	return 0;	
}

int HeaderC::readHeadersimLoci(std::ifstream & file){
	string s1,*ss;
	int nss,grm,gr,kloc,loctyp;
	double som;
	getline(file,s1); //cout<<s1<<"    ligne vide ? \n";       //ligne vide
	getline(file,s1);  //cout<<s1<<"     loci description ?\n";
	ss=splitwords(s1," ",&nss);
	this->dataobs.nloc=getwordint(s1,3);
	cout<<"nloc="<<this->dataobs.nloc<<"\n";
	this->dataobs.locus = new LocusC[this->dataobs.nloc];
	grm=1;
	this->dataobs.filetype=0;
	for (int loc=0;loc<this->dataobs.nloc;loc++){
		getline(file,s1);
		delete [] ss;
		ss=splitwords(s1," ",&nss);
		this->dataobs.locus[loc].name = strdup(ss[0].c_str());
		if (ss[1]=="<A>") this->dataobs.locus[loc].type =0;
		else if (ss[1]=="<H>")   this->dataobs.locus[loc].type =1;
		else if (ss[1]=="<X>")   this->dataobs.locus[loc].type =2;
		else if (ss[1]=="<Y>")   this->dataobs.locus[loc].type =3;
		else if (ss[1]=="<M>")   this->dataobs.locus[loc].type =4;
		this->dataobs.cal_coeffcoal(loc);
		if (ss[2]=="[M]") {
			s1=ss[3].substr(1);gr=atoi(s1.c_str());this->dataobs.locus[loc].groupe=gr;if (gr>grm) grm=gr;
			this->dataobs.locus[loc].motif_size=atoi(ss[4].c_str());
			this->dataobs.locus[loc].motif_range=atoi(ss[5].c_str());
			this->dataobs.locus[loc].mini=100;this->dataobs.locus[loc].maxi=300;
		}
		else if (ss[2]=="[S]") {
			this->dataobs.locus[loc].type +=5;
			s1=ss[3].substr(1);gr=atoi(s1.c_str());this->dataobs.locus[loc].groupe=gr;if (gr>grm) grm=gr;
			this->dataobs.locus[loc].dnalength=atoi(ss[4].c_str());
			this->dataobs.locus[loc].mutsit.resize(this->dataobs.locus[loc].dnalength);
			som=0.0;
			this->dataobs.locus[loc].pi_A=atof(ss[5].c_str());som +=this->dataobs.locus[loc].pi_A;
			this->dataobs.locus[loc].pi_C=atof(ss[6].c_str());som +=this->dataobs.locus[loc].pi_C;
			this->dataobs.locus[loc].pi_G=atof(ss[7].c_str());som +=this->dataobs.locus[loc].pi_G;
			this->dataobs.locus[loc].pi_T=atof(ss[8].c_str());som +=this->dataobs.locus[loc].pi_T;
			this->dataobs.locus[loc].pi_A /= som;
			this->dataobs.locus[loc].pi_C /= som;
			this->dataobs.locus[loc].pi_G /= som;
			this->dataobs.locus[loc].pi_T /= som;
			//cout<<this->dataobs.locus[loc].dnalength<<"\n";
			//this->dataobs.locus[loc].dnalength=atoi(ss[k+2].c_str());  //inutile variable déjà renseignée
		}
		else if (ss[2]=="[P]") {
			this->dataobs.filetype=1;
			kloc=getwordint(s1,1);
			loctyp=this->dataobs.locus[loc].type+10;
			s1=ss[3].substr(1);gr=atoi(s1.c_str());if (gr>grm) grm=gr;
			this->ngroupes=grm;
			for (int k=0;k<kloc;k++){
				this->dataobs.cal_coeffcoal(loc+k);
				this->dataobs.locus[loc+k].type =loctyp;
				this->dataobs.locus[loc+k].groupe=gr;
			}
			loc +=kloc;
		}
	}
	
	this->dataobs.catexist = new bool[5];
	this->dataobs.ss.resize(5);
	for (int i=0;i<5;i++) this->dataobs.catexist[i]=false;
	for (int locustype=0;locustype<5;locustype++) {
		if (not this->dataobs.catexist[locustype]) {
			for (int loc=0;loc<this->dataobs.nloc;loc++) {
				if ((this->dataobs.locus[loc].type % 5) == locustype) {
					this->dataobs.ss[locustype].resize(this->dataobs.nsample);
					for (int sa=0;sa<this->dataobs.nsample;sa++) {
						this->dataobs.ss[locustype][sa]=0;
						for (int ind=0;ind<this->dataobs.nind[sa];ind++) {
							//if ((this->dataobs.locus[loc].type==10)or((this->dataobs.locus[loc].type==12)and(this->dataobs.indivsexe[sa][ind]==2))) this->dataobs.ss[locustype][sa] +=2;
							if( (locustype == 0) or ( (locustype == 2)and(this->dataobs.indivsexe[sa][ind]==2) ) ) {
								this->dataobs.ss[locustype][sa] +=2;
							} else {
								this->dataobs.ss[locustype][sa] +=1;
							}
						}
					}
					this->dataobs.catexist[locustype] = true;
				}
			}
		}
	}
	if (debuglevel==2) cout<<"header.txt : fin de la lecture de la partie description des locus\n";
	return 0;
}

int HeaderC::readHeadersimGroupPrior(std::ifstream & file){
	string s1,*ss,*ss1;
	int nss,nss1;
	getline(file,s1); //cout<<s1<<"    ligne vide ? \n";      //ligne vide
	getline(file,s1);       //ligne "group prior"
	this->ngroupes=getwordint(s1,2);
	cout<<"header.ngroupes="<<this->ngroupes<<"\n";
	this->groupe = new LocusGroupC[this->ngroupes+1];
	this->assignloc(0);
	//cout<<"on attaque les groupes : analyse des priors nombre de groupes="<<this->ngroupes <<"\n";
	for (int gr=1;gr<=this->ngroupes;gr++){
		getline(file,s1);
		ss=splitwords(s1," ",&nss);
		this->assignloc(gr);
		if (ss[2]=="[M]") {
			this->groupe[gr].type=0;
			getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].mutmoy = atof(ss1[1].c_str());delete [] ss1;
			this->groupe[gr].priormutmoy.fixed=true;this->groupe[gr].priormutmoy.constant=true;
			getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormutloc.readprior(ss1[1]);delete [] ss1;

			getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].Pmoy   = atof(ss1[1].c_str());delete [] ss1;
			this->groupe[gr].priorPmoy.fixed=true;this->groupe[gr].priorPmoy.constant=true;
			getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorPloc.readprior(ss1[1]);delete [] ss1;

			getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].snimoy = atof(ss1[1].c_str());delete [] ss1;
			this->groupe[gr].priorsnimoy.fixed=true;this->groupe[gr].priorsnimoy.constant=true;
			getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorsniloc.readprior(ss1[1]);delete [] ss1;
			//cout<<"sniloc  ";this->groupe[gr].priorsniloc.ecris();

		} else if (ss[2]=="[S]") {
			this->groupe[gr].type=1;
			getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].musmoy = atof(ss1[1].c_str());delete [] ss1;
			this->groupe[gr].priormusmoy.fixed=true;this->groupe[gr].priormusmoy.constant=true;
			getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormusloc.readprior(ss1[1]);delete [] ss1;

			getline(file,s1);ss1=splitwords(s1," ",&nss1);
			if (ss1[0]!="MODEL") {
				this->groupe[gr].k1moy  = atof(ss1[1].c_str());delete [] ss1;
				this->groupe[gr].priork1moy.fixed=true;this->groupe[gr].priork1moy.constant=true;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork1loc.readprior(ss1[1]);delete [] ss1;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);
			}

			if (ss1[0]!="MODEL") {
				this->groupe[gr].k2moy  = atof(ss1[1].c_str());delete [] ss1;
				this->groupe[gr].priork2moy.fixed=true;this->groupe[gr].priork2moy.constant=true;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork2loc.readprior(ss1[1]);delete [] ss1;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);
			}

			this->groupe[gr].p_fixe=atof(ss1[2].c_str());this->groupe[gr].gams=atof(ss1[3].c_str());
			if (ss1[1]=="JK") this->groupe[gr].mutmod=0;
			else if (ss1[1]=="K2P") this->groupe[gr].mutmod=1;
			else if (ss1[1]=="HKY") this->groupe[gr].mutmod=2;
			else if (ss1[1]=="TN") this->groupe[gr].mutmod=3;
			delete [] ss1;
		}
		this->groupe[gr].nstat=0;
		delete [] ss;
	}
	if (debuglevel==2) cout<<"header.txt : fin de la lecture de la partie définition des groupes\n";
	return 0;
}

int HeaderC::readHeadersimGroupSNP(){
	this->groupe = new LocusGroupC[this->ngroupes+1];
	this->assignloc(0);
	for (int gr=1;gr<=this->ngroupes;gr++) {
		this->assignloc(gr);
		this->groupe[gr].type = 2;
	}
	return 0;
}

int HeaderC::readHeadersimFin(){
	//Mise à jour des locus séquences
	MwcGen mwc;
	mwc.randinit(999,time(NULL));
	int nsv,gr,j;
	bool nouveau;
	if (this->dataobs.filetype==0) {
		for (int loc=0;loc<this->dataobs.nloc;loc++){
			gr=this->dataobs.locus[loc].groupe;
			if ((gr>0)and(this->dataobs.locus[loc].type>4)){
				nsv = floor(this->dataobs.locus[loc].dnalength*(1.0-0.01*this->groupe[gr].p_fixe)+0.5);
				for (int i=0;i<this->dataobs.locus[loc].dnalength;i++) this->dataobs.locus[loc].mutsit[i] = mwc.ggamma3(1.0,this->groupe[gr].gams);
				int *sitefix;
				sitefix=new int[this->dataobs.locus[loc].dnalength-nsv];
				for (int i=0;i<this->dataobs.locus[loc].dnalength-nsv;i++) {
					if (i==0) sitefix[i]=mwc.rand0(this->dataobs.locus[loc].dnalength);
					else {
						do {
							sitefix[i]=mwc.rand0(this->dataobs.locus[loc].dnalength);
							nouveau=true;j=0;
							while((nouveau)and(j<i)) {nouveau=(sitefix[i]!=sitefix[j]);j++;}
						} while (not nouveau);
					}
					this->dataobs.locus[loc].mutsit[i] = 0.0;
				}
				delete [] sitefix;
				double s=0.0;
				for (int i=0;i<this->dataobs.locus[loc].dnalength;i++) s += this->dataobs.locus[loc].mutsit[i];
				for (int i=0;i<this->dataobs.locus[loc].dnalength;i++) this->dataobs.locus[loc].mutsit[i] /=s;
			}
		}
	}
	else {
		this->nparamut=0;
	}
	if (debuglevel==2) cout<<"header.txt : fin de la mise à jour des locus séquences\n";
	this->scen = this->scenario[0];
	//this->scen.ecris();
	if (debuglevel==2) cout<<"header.txt : apres la copie du scénario\n";
	return 0;
}

int HeaderC::readHeadersim(string headersimfilename){
	int error;
//	if (debuglevel==2) cout<<"debut de readheadersim\n";
	//cout<<"readHeader headerfilename = "<<headerfilename<<"\n";
	ifstream file(headersimfilename.c_str(), ios::in);
	if (file == NULL) {
		this->message = "HeaderSim  File "+string(headersimfilename)+" not found";
		cout<<this->message<<"\n";
		return 1;
	} else this->message="";
	

	error = readHeadersimDebut(file);
	if(error != 0) return error;

	//Partie Scenarios
	error = readHeadersimScenario(file);
	if(error != 0) return error;

	//Partie historical parameters
	error = readHeadersimHistParam(file);
	if(error != 0) return error;
	
	//Partie loci description
	error = readHeadersimLoci(file);
	if(error != 0) return error;

	//Partie group priors
	if (this->dataobs.filetype==0){
		error = readHeadersimGroupPrior(file);
		if(error != 0) return error;
	}
	if (this->dataobs.filetype==1){
		error = readHeadersimGroupSNP();
		if(error != 0) return error;
	}
	
	error = readHeadersimFin();
	return error;
}

void HeaderC::calstatobs(string statobsfilename) {
	stringstream erreur;
	int jstat,cat;
	//partie DATA
	if (debuglevel==2) cout<<"debut de calstatobs\n";
	this->particuleobs.threshold = this->threshold;
	this->particuleobs.dnatrue = true;
	this->particuleobs.nsample = this->dataobs.nsample0;
	if (debuglevel==2) cout<<"this->dataobs.nsample="<<this->dataobs.nsample<<"\n";
	this->particuleobs.data.nsample = this->dataobs.nsample;
	this->particuleobs.data.nind.resize(this->dataobs.nsample);
	this->particuleobs.data.indivsexe.resize(this->dataobs.nsample);
	for (int i=0;i<this->dataobs.nsample;i++) {
		this->particuleobs.data.nind[i] = this->dataobs.nind[i];
		this->particuleobs.data.indivsexe[i].resize(this->dataobs.nind[i]);
		for (int j=0;j<this->dataobs.nind[i];j++) this->particuleobs.data.indivsexe[i][j] = this->dataobs.indivsexe[i][j];
	}
	if (debuglevel==2) cout<<"avant l'affectation des catexist et des ss'\n";
	this->particuleobs.catexist = new bool[5];
	for (int i=0;i<5;i++) this->particuleobs.catexist[i] = this->dataobs.catexist[i];
	this->particuleobs.data.ss.resize(5);
	for (int locustype=0;locustype<5;locustype++){
		if (this->dataobs.catexist[locustype]) {
			this->particuleobs.data.ss[locustype].resize(this->dataobs.nsample);
			for (int sa=0;sa<this->dataobs.nsample;sa++) this->particuleobs.data.ss[locustype][sa] = this->dataobs.ss[locustype][sa];
		} 
	}
	if (this->dataobs.filetype==0){ 
		this->particuleobs.data.nmisshap = this->dataobs.nmisshap;
		this->particuleobs.data.nmissnuc = this->dataobs.nmissnuc;
		if (this->dataobs.nmisshap>0) {
			this->particuleobs.data.misshap = new MissingHaplo[this->particuleobs.data.nmisshap];
			for (int i=0;i<this->dataobs.nmisshap;i++) {
				this->particuleobs.data.misshap[i].locus  = this->dataobs.misshap[i].locus;
				this->particuleobs.data.misshap[i].sample = this->dataobs.misshap[i].sample;
				this->particuleobs.data.misshap[i].indiv  = this->dataobs.misshap[i].indiv;
			}
		}
		if (this->dataobs.nmissnuc>0) {
			this->particuleobs.data.missnuc = new MissingNuc[this->particuleobs.data.nmissnuc];
			for (int i=0;i<this->dataobs.nmissnuc;i++) {
				this->particuleobs.data.missnuc[i].locus  = this->dataobs.missnuc[i].locus;
				this->particuleobs.data.missnuc[i].sample = this->dataobs.missnuc[i].sample;
				this->particuleobs.data.missnuc[i].indiv  = this->dataobs.missnuc[i].indiv;
				this->particuleobs.data.missnuc[i].nuc    = this->dataobs.missnuc[i].nuc;
			}
		}
	} else {
		this->particuleobs.data.nmisssnp = this->dataobs.nmisssnp;
		this->particuleobs.data.nmisssnp = this->dataobs.nmisssnp;
		if (this->particuleobs.data.nmisssnp>0) {
			this->particuleobs.data.misssnp = new MissingHaplo[this->particuleobs.data.nmisssnp];
			for (int i=0;i<this->dataobs.nmisssnp;i++) {
				this->particuleobs.data.misssnp[i].locus  = this->dataobs.misssnp[i].locus;
				this->particuleobs.data.misssnp[i].sample = this->dataobs.misssnp[i].sample;
				this->particuleobs.data.misssnp[i].indiv  = this->dataobs.misssnp[i].indiv;
			}
		}
	}
	if (debuglevel==2) cout<<"avant l'affectation des dat et ref'\n";
	this->particuleobs.dat = new bool**[5];
	this->particuleobs.ref = new bool**[5];
	for (int locustype=0;locustype<5;locustype++){
		if (this->dataobs.catexist[locustype]) {
			this->particuleobs.dat[locustype] = new bool*[this->dataobs.nsample];
			this->particuleobs.ref[locustype] = new bool*[this->dataobs.nsample];
			for (int sa=0;sa<this->dataobs.nsample;sa++) {
				this->particuleobs.dat[locustype][sa] = new bool[this->dataobs.ss[locustype][sa]];
				this->particuleobs.ref[locustype][sa] = new bool[this->dataobs.ss[locustype][sa]];
				for (int i=0;i<this->dataobs.ss[locustype][sa];i++) {
					this->particuleobs.dat[locustype][sa][i] = true;
					this->particuleobs.ref[locustype][sa][i] = false;
				}
			}
		}
	}
	if (debuglevel==2) cout<<"apres DATA\n";
	//partie GROUPES
	int ngr = this->ngroupes;
	if (debuglevel==2) cout<<"ngr="<<ngr<<"\n";
	this->particuleobs.ngr = ngr;
	this->particuleobs.grouplist = new LocusGroupC[ngr+1];
	this->particuleobs.grouplist[0].nloc = this->groupe[0].nloc;
	if (this->groupe[0].nloc>0){
		this->particuleobs.grouplist[0].loc  = new int[this->groupe[0].nloc];
		for (int i=0;i<this->groupe[0].nloc;i++) this->particuleobs.grouplist[0].loc[i] = this->groupe[0].loc[i];
	}
	for (int gr=1;gr<=ngr;gr++) {
		if (debuglevel==2) cout <<"groupe "<<gr<<"\n";
		this->particuleobs.grouplist[gr].type =this->groupe[gr].type;
		this->particuleobs.grouplist[gr].nloc = this->groupe[gr].nloc;
		this->particuleobs.grouplist[gr].loc  = new int[this->groupe[gr].nloc];
		for (int i=0;i<this->groupe[gr].nloc;i++) this->particuleobs.grouplist[gr].loc[i] = this->groupe[gr].loc[i];
		this->particuleobs.grouplist[gr].nstat=this->groupe[gr].nstat;
		this->particuleobs.grouplist[gr].sumstat = new StatC[this->groupe[gr].nstat];
		//cout<<"calstatobs nstat["<<gr<<"]="<<this->groupe[gr].nstat<<"\n";
		for (int i=0;i<this->groupe[gr].nstat;i++){
			this->particuleobs.grouplist[gr].sumstat[i].cat   = this->groupe[gr].sumstat[i].cat;
			this->particuleobs.grouplist[gr].sumstat[i].samp  = this->groupe[gr].sumstat[i].samp;
			this->particuleobs.grouplist[gr].sumstat[i].samp1 = this->groupe[gr].sumstat[i].samp1;
			this->particuleobs.grouplist[gr].sumstat[i].samp2 = this->groupe[gr].sumstat[i].samp2;
			this->particuleobs.grouplist[gr].sumstat[i].numsnp = this->groupe[gr].sumstat[i].numsnp;
		}
		this->particuleobs.grouplist[gr].nstatsnp=this->groupe[gr].nstatsnp;
		if (this->groupe[gr].nstatsnp>0){
			this->particuleobs.grouplist[gr].sumstatsnp = new StatsnpC[this->groupe[gr].nstatsnp];
			for (int i=0;i<this->groupe[gr].nstatsnp;i++){
				this->particuleobs.grouplist[gr].sumstatsnp[i].cat   = this->groupe[gr].sumstatsnp[i].cat;
				this->particuleobs.grouplist[gr].sumstatsnp[i].samp  = this->groupe[gr].sumstatsnp[i].samp;
				this->particuleobs.grouplist[gr].sumstatsnp[i].samp1 = this->groupe[gr].sumstatsnp[i].samp1;
				this->particuleobs.grouplist[gr].sumstatsnp[i].samp2 = this->groupe[gr].sumstatsnp[i].samp2;
				this->particuleobs.grouplist[gr].sumstatsnp[i].defined = this->groupe[gr].sumstatsnp[i].defined;
				this->particuleobs.grouplist[gr].sumstatsnp[i].sorted = this->groupe[gr].sumstatsnp[i].sorted;
				this->particuleobs.grouplist[gr].sumstatsnp[i].x = new long double[this->groupe[gr].nloc];
				this->particuleobs.grouplist[gr].sumstatsnp[i].w = new long double[this->groupe[gr].nloc];
			}
		}
	}
	if (debuglevel==2) cout<<"apres GROUPS\n";
	//partie LOCUSLIST
	int kmoy;
	this->particuleobs.nloc = this->dataobs.nloc;
	cout<<this->dataobs.nloc<<"\n";
	//vector<LocusC> tmp(41751);
	//this->particuleobs.locuslist = &tmp[0]; //new LocusC[41752];
	if (debuglevel==2)cout<<"avant la partie locus\n";
	this->particuleobs.locuslist =new LocusC[this->dataobs.nloc];
	for (int kloc=0;kloc<this->dataobs.nloc;kloc++){
		this->particuleobs.locuslist[kloc].type = this->dataobs.locus[kloc].type;
		this->particuleobs.locuslist[kloc].groupe = this->dataobs.locus[kloc].groupe;
		this->particuleobs.locuslist[kloc].coeffcoal =  this->dataobs.locus[kloc].coeffcoal;
		cat = this->particuleobs.locuslist[kloc].type % 5;
		if (debuglevel==2) cout<<"kloc="<<kloc<<"   cat="<<cat<<"\n";
		//this->particuleobs.locuslist[kloc].name =  new char[strlen(this->dataobs.locus[kloc].name)+1];
		//strcpy(this->particuleobs.locuslist[kloc].name,this->dataobs.locus[kloc].name);
		if (debuglevel==2) cout<<"locus "<<kloc<<"   groupe "<<this->particuleobs.locuslist[kloc].groupe<<"\n";
		if (this->dataobs.locus[kloc].type < 5) {
			kmoy=(this->dataobs.locus[kloc].maxi+this->dataobs.locus[kloc].mini)/2;
			this->particuleobs.locuslist[kloc].kmin=kmoy-((this->dataobs.locus[kloc].motif_range/2)-1)*this->dataobs.locus[kloc].motif_size;
			this->particuleobs.locuslist[kloc].kmax=this->particuleobs.locuslist[kloc].kmin+(this->dataobs.locus[kloc].motif_range-1)*this->dataobs.locus[kloc].motif_size;
			this->particuleobs.locuslist[kloc].motif_size = this->dataobs.locus[kloc].motif_size;
			this->particuleobs.locuslist[kloc].motif_range = this->dataobs.locus[kloc].motif_range;
			if ((this->particuleobs.locuslist[kloc].kmin>this->dataobs.locus[kloc].mini)or(this->particuleobs.locuslist[kloc].kmax<this->dataobs.locus[kloc].maxi)) {
				erreur <<"Job aborted : motif range at locus "<<kloc+1<<" is not large enough to include all observed alleles.\n";
				throw std::range_error( erreur.str() ); //exit(1);
			}
			this->particuleobs.locuslist[kloc].haplomic = new int*[this->particuleobs.data.nsample];
			for (int sa=0;sa<this->particuleobs.data.nsample;sa++){
				this->particuleobs.locuslist[kloc].haplomic[sa] = new int[this->particuleobs.data.ss[cat][sa]];
				for (int i=0;i<this->particuleobs.data.ss[cat][sa];i++)this->particuleobs.locuslist[kloc].haplomic[sa][i]=this->dataobs.locus[kloc].haplomic[sa][i];
			}
		}else if (this->dataobs.locus[kloc].type < 10) {
			this->particuleobs.locuslist[kloc].dnalength =  this->dataobs.locus[kloc].dnalength;
			this->particuleobs.locuslist[kloc].pi_A = this->dataobs.locus[kloc].pi_A ;
			this->particuleobs.locuslist[kloc].pi_C =  this->dataobs.locus[kloc].pi_C;
			this->particuleobs.locuslist[kloc].pi_G =  this->dataobs.locus[kloc].pi_G;
			this->particuleobs.locuslist[kloc].pi_T =  this->dataobs.locus[kloc].pi_T;
			this->particuleobs.locuslist[kloc].haplodna = new string*[this->particuleobs.data.nsample];
			for (int sa=0;sa<this->particuleobs.data.nsample;sa++){
				this->particuleobs.locuslist[kloc].haplodna[sa] = new string[this->particuleobs.data.ss[cat][sa]];
				for (int i=0;i<this->particuleobs.data.ss[cat][sa];i++)this->particuleobs.locuslist[kloc].haplodna[sa][i] =this->dataobs.locus[kloc].haplodna[sa][i];
			}
		}else {
			this->particuleobs.locuslist[kloc].weight = 1.0;
			this->particuleobs.locuslist[kloc].haplosnp = new short int*[this->particuleobs.nsample];
			for (int cat=0;cat<5;cat++){
				if (this->particuleobs.catexist[cat]){
					for (int sa=0;sa<this->particuleobs.nsample;sa++){
						this->particuleobs.locuslist[kloc].haplosnp[sa] = new short int[this->particuleobs.data.ss[cat][sa]];
						for (int i=0;i<this->particuleobs.data.ss[cat][sa];i++)this->particuleobs.locuslist[kloc].haplosnp[sa][i] =this->dataobs.locus[kloc].haplosnp[sa][i];
					}
				}
			}
		}
	}
	if (debuglevel==2) cout<<"apres la partie locus\n";
	//string *sb,
	string ent;
	int j;
	if (debuglevel==2) cout<<"this->entetestat = "<<this->entetestat<<"\n";
	//sb = splitwords(this->entete," ",&j);
	//cout<<"this->entete = "<<this->entete<<"\nn="<<j<<"   this->nstat="<<this->nstat<<"\n";
	ent=entetestat+"\n";
	//for (int k=j-this->nstat;k<j;k++) ent=ent+centre(sb[k],14);
	//ent=ent+"\n";
	//if (debuglevel==2)cout<<"entete : "<<entete<<"\n";
	//delete []sb;
	jstat=0;
	if (debuglevel==2) cout<<"avant FILE *fobs\n";
	FILE *fobs;
	fobs=fopen(statobsfilename.c_str(),"w");
	fputs(ent.c_str(),fobs);
	for(int gr=1;gr<=this->particuleobs.ngr;gr++) {
		if (debuglevel==2)cout<<"avant calcul des statobs du groupe "<<gr<<"\n";
		this->particuleobs.docalstat(gr,1.0);
		jstat +=this->particuleobs.grouplist[gr].nstat;
		if (debuglevel==2)cout<<"apres calcul des statobs du groupe "<<gr<<"\n";
		for (int j=0;j<this->particuleobs.grouplist[gr].nstat;j++) fprintf(fobs,"%12.8Lf  ",this->particuleobs.grouplist[gr].sumstat[j].val);
	}
	fprintf(fobs,"\n");
	fclose(fobs);
	this->stat_obs = new float[jstat];
	jstat=0;
	for(int gr=1;gr<=this->particuleobs.ngr;gr++) {
		for (int j=0;j<this->particuleobs.grouplist[gr].nstat;j++) {
			this->stat_obs[jstat]=(float)this->particuleobs.grouplist[gr].sumstat[j].val;
			jstat++;
		}
	}
	if (debuglevel==2) cout<<"avant libere \n";
	this->particuleobs.libere(true);
	if (debuglevel==2) cout<<"fin de calstatobs\n";
}

/**
* lit le fichier des statistiques observées (placées dans double *stat_obs)
*/
/*    double* read_statobs(char *statobsfilename) {
       string entete,ligne,*ss;
       int ns;
       ifstream file(statobsfilename, ios::in);
       getline(file,entete);
       getline(file,ligne);
       file.close();
       ss=splitwords(ligne," ",&ns);
       //cout<<"statobs ns="<<ns<<"\n";
       if (ns!=this->nstat) exit(1);
       this->stat_obs = new double[ns];
       for (int i=0;i<ns;i++) this->stat_obs[i]=atof(ss[i].c_str());
       delete []ss;
       return this->stat_obs;
       //for (int i=0;i<ns;i++) cout<<stat_obs[i]<<"   ";
       //cout<<"\n";
    }*/



