#include <iostream>

//using namespace boost::python;
//using namespace std;
//#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "particuleC.cpp"
//#include "data.cpp"

/*  Numérotation des stat
 * 	1 : nal			-1 : nha			-13 : fst
 *  2 : het			-2 : nss            -14 : aml
 *  3 : MGW			-3 : mpd
 *  4 : var			-4 : vpd
 *  5 : Fst			-5 : dta
 *  6 : lik			-6 : pss
 *  7 : dm2			-7 : mns
 *  8 : n2P			-8 : vns
 *  9 : h2P			-9 : nh2
 * 10 : v2P		   -10 : ns2
 * 11 : das        -11 : mp2
 * 12 : Aml        -12 : mpb
 *
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HeaderC
{
public:
	string message,datafilename,entete;
	DataC dataobs;
	int nparamtot,nstat,nscenarios,nconditions,ngroupes;
	ScenarioC *scenario;
	HistParameterC *histparam;
	ConditionC *condition;
	LocusGroupC *groupe;

	PriorC readprior(string ss) {
		double step;
		PriorC prior;
		string s1,*sb;
		int j;
		s1 = ss.substr(3,ss.length()-4);
		sb = splitwords(s1,",",&j);
		step=atof(sb[4].c_str());
		prior.ndec=ndecimales(step);
		prior.mini=atof(sb[0].c_str());
		prior.maxi=atof(sb[1].c_str());
		if (ss.find("UN[")!=string::npos) {prior.loi="uniforme";}
		else if (ss.find("LU[")!=string::npos) {prior.loi="loguniforme";}
		else if (ss.find("NO[")!=string::npos) {prior.loi="normale";prior.mean=atof(sb[2].c_str());prior.sdshape=atof(sb[3].c_str());}
		else if (ss.find("LN[")!=string::npos) {prior.loi="lognormale";prior.mean=atof(sb[2].c_str());prior.sdshape=atof(sb[3].c_str());}
		else if (ss.find("GA[")!=string::npos) {prior.loi="gamma";prior.mean=atof(sb[2].c_str());prior.sdshape=atof(sb[3].c_str());}
		prior.constant = ((prior.maxi-prior.mini)/prior.maxi<0.000001);
		return prior;
	}

	ConditionC readcondition(string ss){
		ConditionC cond;
		if (ss.find(">=")!=string::npos){
			cond.operateur=">=";cond.param1=ss.substr(0,ss.find(">="));cond.param2=ss.substr(ss.find(">=")+2,ss.length()-(ss.find(">=")+2));}
		else if (ss.find("<=")!=string::npos){
			cond.operateur="<=";cond.param1=ss.substr(0,ss.find("<="));cond.param2=ss.substr(ss.find("<=")+2,ss.length()-(ss.find("<=")+2));}
		if (ss.find(">")!=string::npos){
			cond.operateur=">";cond.param1=ss.substr(0,ss.find(">"));cond.param2=ss.substr(ss.find(">")+1,ss.length()-(ss.find(">")+1));}
		else if (ss.find("<=")!=string::npos){
			cond.operateur="<";cond.param1=ss.substr(0,ss.find("<"));cond.param2=ss.substr(ss.find("<")+1,ss.length()-(ss.find("<")+1));}
	}

	void assignloc(int gr){
		this->groupe[gr].nloc = 0;
		for (int loc=0;loc<dataobs.nloc;loc++) {
			if (dataobs.locus[loc].groupe==gr+1) this->groupe[gr].nloc++;
		}
		this->groupe[gr].loc = new int[this->groupe[gr].nloc];
		int iloc=-1;
		for (int i=0;i<dataobs.nloc;i++) {
			if (dataobs.locus[i].groupe==gr+1) {iloc++;this->groupe[gr].loc[iloc] = i;}
		}
	}

	HeaderC* readHeader(char* headerfilename){
		string s1,**sl,*ss,*ss1;
		int *nlscen,nss,nss1,j,k,l,gr,grm;
		ifstream file(headerfilename, ios::in);
		if (file == NULL) {
			this->message = "File ReftableHeader.txt not found";
			return this;
		} else this->message="";
		getline(file,this->datafilename);
		this->dataobs.loadfromfile(this->datafilename);
		getline(file,s1);
		this->nparamtot=getwordint(s1,1);
		this->nstat=getwordint(s1,4);
//Partie Scenarios
		getline(file,s1);		//ligne vide
		getline(file,s1);       // nombre de scenarios
		this->nscenarios=getwordint(s1,1);
		sl = new string*[this->nscenarios];
		nlscen = new int[this->nscenarios];
		this->scenario = new ScenarioC[this->nscenarios];
		for (int i=0;i<this->nscenarios;i++) nlscen[i]=getwordint(s1,3+i);
		for (int i=0;i<this->nscenarios;i++) {
			sl[i] = new string[nlscen[i]];
			getline(file,s1);
			scenario[i].number = getwordint(s1,2);
			scenario[i].prior_proba = getwordint(s1,3);
			scenario[i].nparam = getwordint(s1,4);
			for (int j=0;j<nlscen[i];j++) getline(file,sl[i][j]);
			scenario[i].read_events(nlscen[i],sl[i]);
		}
		for (int i=0;i<this->nscenarios;i++) delete []sl[i];
		delete [] sl;
//Partie historical parameters
		getline(file,s1);		//ligne vide
		getline(file,s1);
		ss=splitwords(s1," ",&nss);
		this->nparamtot = atoi(ss[3].c_str());
		if (nss>4) this->nconditions =  atoi(ss[4].c_str()); else  this->nconditions = 0;
		this->histparam = new HistParameterC[this->nparamtot];
		if (this->nconditions>0) this->condition = new ConditionC[this->nconditions];
		delete [] ss;
		for (int i=0;i<this->nparamtot;i++) {
			getline(file,s1);
			ss=splitwords(s1," ",&nss);
			this->histparam[i].name=ss[0];
			if (ss[1]=="N") this->histparam[i].category = 0;
			else if  (ss[1]=="T") this->histparam[i].category = 1;
			else if  (ss[1]=="A") this->histparam[i].category = 2;
			this->histparam[i].prior = this->readprior(ss[2]);
		}
		delete [] ss;
		if (this->nconditions>0) {
			this->condition = new ConditionC[this->nconditions];
			for (int i=0;i<this->nconditions;i++) {
				getline(file,s1);
				this->condition[i] = this->readcondition(s1);
			}
		}
//Partie loci description
		getline(file,s1);		//ligne vide
		getline(file,s1);		//ligne "loci description"
		grm=1;
		for (int loc=0;loc<dataobs.nloc;loc++){
			getline(file,s1);
			ss=splitwords(s1," ",&nss);
			k=0;while (ss[k].find("[")==string::npos) k++;
			if (ss[k]=="[M]") {
				s1=ss[k+1].substr(1,ss[k+1].length());gr=atoi(s1.c_str());dataobs.locus[loc].groupe=gr;if (gr>grm) grm=gr;
				dataobs.locus[loc].motif_size=atoi(ss[k+2].c_str());dataobs.locus[loc].motif_range=atoi(ss[k+3].c_str());
			}
			else if (ss[k]=="[S]") {
				s1=ss[k+1].substr(1,ss[k+1].length());gr=atoi(s1.c_str());dataobs.locus[loc].groupe=gr;if (gr>grm) grm=gr;
				dataobs.locus[loc].dnalength=atoi(ss[k+2].c_str());
			}
		}
		this->ngroupes=grm;
		delete [] ss;
//Partie group priors
		getline(file,s1);		//ligne vide
		getline(file,s1);		//ligne "group prior"
		this->groupe = new LocusGroupC[grm];
		for (gr=0;gr<grm;gr++){
			getline(file,s1);
			ss=splitwords(s1," ",&nss);
			this->assignloc(gr);
			if (ss[2]=="[M]") {this->groupe[gr].type=0;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormutmoy = this->readprior(ss1[1]);delete [] ss1;
				if (this->groupe[gr].priormutmoy.constant) this->groupe[gr].mutmoy=this->groupe[gr].priormutmoy.mini; else this->groupe[gr].mutmoy=-1.0;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormutloc = this->readprior(ss1[1]);delete [] ss1;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorPmoy   = this->readprior(ss1[1]);delete [] ss1;
				if (this->groupe[gr].priorPmoy.constant) this->groupe[gr].Pmoy=this->groupe[gr].priorPmoy.mini; else this->groupe[gr].Pmoy=-1.0;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorPloc   = this->readprior(ss1[1]);delete [] ss1;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorsnimoy = this->readprior(ss1[1]);delete [] ss1;
				if (this->groupe[gr].priorsnimoy.constant) this->groupe[gr].snimoy=this->groupe[gr].priorsnimoy.mini; else this->groupe[gr].snimoy=-1.0;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorsniloc = this->readprior(ss1[1]);delete [] ss1;
			} else if (ss[2]=="[S]") {this->groupe[gr].type=1;
				this->groupe[gr].p_fixe = atof(ss[3].c_str());
				this->groupe[gr].gams = atof(ss[4].c_str());
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormusmoy = this->readprior(ss1[1]);delete [] ss1;
				if (this->groupe[gr].priormusmoy.constant) this->groupe[gr].musmoy=this->groupe[gr].priormusmoy.mini; else this->groupe[gr].musmoy=-1.0;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormusloc = this->readprior(ss1[1]);delete [] ss1;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork1moy  = this->readprior(ss1[1]);delete [] ss1;
				if (this->groupe[gr].priork1moy.constant) this->groupe[gr].k1moy=this->groupe[gr].priork1moy.mini; else this->groupe[gr].k1moy=-1.0;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork1loc  = this->readprior(ss1[1]);delete [] ss1;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork2moy  = this->readprior(ss1[1]);delete [] ss1;
				if (this->groupe[gr].priork2moy.constant) this->groupe[gr].k2moy=this->groupe[gr].priork2moy.mini; else this->groupe[gr].k2moy=-1.0;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork2loc  = this->readprior(ss1[1]);delete [] ss1;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);
				this->groupe[gr].p_fixe=atof(ss1[2].c_str());this->groupe[gr].gams=atof(ss1[3].c_str());
				if (ss1[0]=="JK") this->groupe[gr].mutmod=0;
				else if (ss1[0]=="K2P") this->groupe[gr].mutmod=1;
				else if (ss1[0]=="HKY") this->groupe[gr].mutmod=2;
				else if (ss1[0]=="TN") this->groupe[gr].mutmod=3;
			}
		}
		delete [] ss;
//Partie group statistics
		getline(file,s1);		//ligne vide
		getline(file,s1);		//ligne "group group statistics"
		for (gr=0;gr<grm;gr++) {
			getline(file,s1);
			ss=splitwords(s1," ",&nss);
			this->groupe[gr].nstat = atoi(ss[3].c_str());
			this->groupe[gr].sumstat = new StatC[this->groupe[gr].nstat];
			delete [] ss;
			k=0;
			while (k<this->groupe[gr].nstat) {
				getline(file,s1);
				ss=splitwords(s1," ",&nss);
				j=0;while (ss[0]!=stat_type[j]) j++;
				this->groupe[gr].sumstat[k].cat=stat_num[j];
				if (this->groupe[gr].type==0) {
					if (stat_num[j]<5) {
						for (int i=1;i<nss;i++) {this->groupe[gr].sumstat[k].samp=atoi(ss[i].c_str());k++;}
					} else if (stat_num[j]<12) {
						for (int i=1;i<nss;i++) {
							ss1=splitwords(ss[i],"&",&nss1);
							this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
							this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
							k++;
						}
						delete [] ss1;
					} else if (stat_num[j]==12) {
						for (int i=1;i<nss;i++) {
							ss1=splitwords(ss[i],"&",&nss1);
							this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
							this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
							k++;
						}
					}
					delete [] ss1;
				} else if (this->groupe[gr].type==1) {
					if (stat_num[j]>-5) {
						for (int i=1;i<nss;i++) {this->groupe[gr].sumstat[k].samp=atoi(ss[i].c_str());k++;}
					} else if (stat_num[j]>-14) {
						for (int i=1;i<nss;i++) {
							ss1=splitwords(ss[i],"&",&nss1);
							this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
							this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
							k++;
						}
						delete [] ss1;
					} else if (stat_num[j]==-14) {
						for (int i=1;i<nss;i++) {
							ss1=splitwords(ss[i],"&",&nss1);
							this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
							this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
							this->groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
							k++;
						}
					}
					delete [] ss1;
				}
			}
			delete [] ss;
		}
//Entete du fichier reftable
		getline(file,s1);		//ligne vide
		getline(file,this->entete);		//ligne entete
		return this;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ParticleSetC
{
	HeaderC header;
	ParticleC *particule;
	LocusC *locuslist;
	LocusGroupC *grouplist;
	int npart,nloc,ngr;
	int nsample,*nind,**indivsexe,nscenarios;
	ScenarioC *scenario;
	double sexratio;

	void setdata(int p) {
		this->particule[p].data.nsample = header.dataobs.nsample;
		this->particule[p].data.nind = new int[header.dataobs.nsample];
		this->particule[p].data.indivsexe = new int*[header.dataobs.nsample];
		for (int i=0;i<header.dataobs.nsample;i++) {
			this->particule[p].data.nind[i] = header.dataobs.nind[i];
			this->particule[p].data.indivsexe[i] = new int[header.dataobs.nind[i]];
			for (int j=0;j<header.dataobs.nind[i];j++) this->particule[p].data.indivsexe[i][j] = header.dataobs.indivsexe[i][j];
		}
	}

	void setgroup(int p) {
		int ngr = header.ngroupes;
		this->particule[p].ngr = ngr;
		this->particule[p].grouplist = new LocusGroupC[ngr];
		for (int gr=0;gr<this->ngr;gr++) {
			this->particule[p].grouplist[gr].type =header.groupe[gr].type;
			this->particule[p].grouplist[gr].nloc = header.groupe[gr].nloc;
			this->particule[p].grouplist[gr].loc  = new int[header.groupe[gr].nloc];
			for (int i=0;i<header.groupe[gr].nloc;i++) this->particule[p].grouplist[gr].loc[i] = header.groupe[gr].loc[i];
			if (header.groupe[gr].type==0) {	//MICROSAT
				this->particule[p].grouplist[gr].mutmoy = header.groupe[gr].mutmoy;
				if (header.groupe[gr].mutmoy<0.0) this->particule[p].grouplist[gr].priormutmoy = copyprior(header.groupe[gr].priormutmoy);
				else this->particule[p].grouplist[gr].priormutmoy.constant=true;
				this->particule[p].grouplist[gr].priormutloc = copyprior(header.groupe[gr].priormutloc);
				this->particule[p].grouplist[gr].Pmoy = header.groupe[gr].Pmoy;
				if (header.groupe[gr].Pmoy<0.0) this->particule[p].grouplist[gr].priorPmoy = copyprior(header.groupe[gr].priorPmoy);
				else this->particule[p].grouplist[gr].priorPmoy.constant=true;
				this->particule[p].grouplist[gr].priorPloc = copyprior(header.groupe[gr].priorPloc);
				this->particule[p].grouplist[gr].snimoy = header.groupe[gr].snimoy;
				if (header.groupe[gr].snimoy<0.0) this->particule[p].grouplist[gr].priorsnimoy = copyprior(header.groupe[gr].priorsnimoy);
				else this->particule[p].grouplist[gr].priorsnimoy.constant=true;
				this->particule[p].grouplist[gr].priorsniloc = copyprior(header.groupe[gr].priorsniloc);
			}
			else {							//SEQUENCES
				this->particule[p].grouplist[gr].mutmod = header.groupe[gr].mutmod;	//mutmod
				this->particule[p].grouplist[gr].p_fixe = header.groupe[gr].p_fixe;	//p_fixe
				this->particule[p].grouplist[gr].gams   = header.groupe[gr].gams;	//gams
				this->particule[p].grouplist[gr].musmoy = header.groupe[gr].musmoy;	//musmoy
				if (header.groupe[gr].musmoy<0.0) this->particule[p].grouplist[gr].priormusmoy = copyprior(header.groupe[gr].priormusmoy);
				else this->particule[p].grouplist[gr].priormusmoy.constant=true;
				this->particule[p].grouplist[gr].priormusloc = copyprior(header.groupe[gr].priormusloc);
				if (header.groupe[gr].mutmod>0){
					this->particule[p].grouplist[gr].k1moy = header.groupe[gr].k1moy ;	//k1moy
					if (header.groupe[gr].k1moy<0) this->particule[p].grouplist[gr].priork1moy = copyprior(header.groupe[gr].priork1moy);
					else this->particule[p].grouplist[gr].priork1moy.constant=true;
					this->particule[p].grouplist[gr].priork1loc = copyprior(header.groupe[gr].priork1loc);
				}
				if (header.groupe[gr].mutmod>2){
					this->particule[p].grouplist[gr].k2moy = header.groupe[gr].k2moy ;	//k2moy
					if (header.groupe[gr].k2moy<0) this->particule[p].grouplist[gr].priork2moy = copyprior(header.groupe[gr].priork2moy);
					else this->particule[p].grouplist[gr].priork2moy.constant=true;
					this->particule[p].grouplist[gr].priork2loc = copyprior(header.groupe[gr].priork2loc);
				}
			}
			this->particule[p].grouplist[gr].nstat=header.groupe[gr].nstat;
			this->particule[p].grouplist[gr].sumstat = new StatC[header.groupe[gr].nstat];
			for (int i=0;i<header.groupe[gr].nstat;i++){
				this->particule[p].grouplist[gr].sumstat[i].cat   = header.groupe[gr].sumstat[i].cat;
				this->particule[p].grouplist[gr].sumstat[i].samp  = header.groupe[gr].sumstat[i].samp;
				this->particule[p].grouplist[gr].sumstat[i].samp1 = header.groupe[gr].sumstat[i].samp1;
				this->particule[p].grouplist[gr].sumstat[i].samp2 = header.groupe[gr].sumstat[i].samp2;

			}
		}
	}

	void setloci(int p) {
		this->particule[p].nloc = header.dataobs.nloc;
		this->particule[p].locuslist = new LocusC[header.dataobs.nloc];
		for (int kloc=0;kloc<header.dataobs.nloc;kloc++){
			this->particule[p].locuslist[kloc].type = header.dataobs.locus[kloc].type;
			this->particule[p].locuslist[kloc].groupe = header.dataobs.locus[kloc].groupe;
			if (header.dataobs.locus[kloc].type < 5) {
				this->particule[p].locuslist[kloc].kmin = header.dataobs.locus[kloc].kmin;
				this->particule[p].locuslist[kloc].kmax = header.dataobs.locus[kloc].kmax;
				this->particule[p].locuslist[kloc].motif_size = header.dataobs.locus[kloc].motif_size;
				this->particule[p].locuslist[kloc].motif_range = header.dataobs.locus[kloc].motif_range;
				this->particule[p].locuslist[kloc].mut_rate = header.dataobs.locus[kloc].mut_rate;
				this->particule[p].locuslist[kloc].Pgeom = header.dataobs.locus[kloc].Pgeom;
				this->particule[p].locuslist[kloc].sni_rate = header.dataobs.locus[kloc].sni_rate;
				}
			else {
				this->particule[p].locuslist[kloc].dnalength =  header.dataobs.locus[kloc].dnalength;
				this->particule[p].locuslist[kloc].mus_rate =  header.dataobs.locus[kloc].mus_rate;
				this->particule[p].locuslist[kloc].k1 =  header.dataobs.locus[kloc].k1;
				this->particule[p].locuslist[kloc].k2 =  header.dataobs.locus[kloc].k2;
				this->particule[p].locuslist[kloc].pi_A = header.dataobs.locus[kloc].pi_A ;
				this->particule[p].locuslist[kloc].pi_C =  header.dataobs.locus[kloc].pi_C;
				this->particule[p].locuslist[kloc].pi_G =  header.dataobs.locus[kloc].pi_G;
				this->particule[p].locuslist[kloc].pi_T =  header.dataobs.locus[kloc].pi_T;
				this->particule[p].locuslist[kloc].mutsit = new double[ header.dataobs.locus[kloc].dnalength];
				for (int i=0;i<locuslist[kloc].dnalength;i++) this->particule[p].locuslist[kloc].mutsit[i] = header.dataobs.locus[kloc].mutsit[i];
				//std::cout <<"\n";
				}
			this->particule[p].locuslist[kloc].coeff =  header.dataobs.locus[kloc].coeff;
			this->particule[p].locuslist[kloc].ss = new int[ header.dataobs.nsample];
			for (int sa=0;sa<this->nsample;sa++) this->particule[p].locuslist[kloc].ss[sa] =  header.dataobs.locus[kloc].ss[sa];
			this->particule[p].locuslist[kloc].samplesize = new int[ header.dataobs.nsample];
			for (int sa=0;sa<this->nsample;sa++) this->particule[p].locuslist[kloc].samplesize[sa] =  header.dataobs.locus[kloc].samplesize[sa];
		}
	}

	void setscenarios (int p) {
		this->particule[p].nscenarios=header.nscenarios;
		this->particule[p].scenario = new ScenarioC[header.nscenarios];
		for (int i=0;i<header.nscenarios;i++) this->particule[p].scenario[i] = copyscenario(header.scenario[i]);
	}

/*	void setparamn0(vector<int>hpi,vector<double>hpd,vector<string>hps) {
		PriorC *prior;
		Ne0C *ne0;
		int *category;
		string *name;
		double *value;
		int *paramvar;
		int ni=-1,nd=-1,ns=-1;
		int nparam,nparamvar=0,nn0;
		ni++;nparam = hpi[ni];
		prior = new PriorC[nparam];
		category = new int[nparam];
		name = new string[nparam];
		value = new double[nparam];
		paramvar =new int[nparam];
		for (int k=0;k<nparam;k++){
			ni++;category[k] = hpi[ni];
			ns++;name[k] = hps[ns];
			nd++;value[k] = hpd[nd];
			if (value[k]<0.0){
				ns++;prior[k].loi = hps[ns];
				nd++;prior[k].mini =hpd[nd];
				nd++;prior[k].maxi =hpd[nd];
				if (prior[k].maxi!=prior[k].mini) {paramvar[nparamvar]=k;nparamvar++;prior[k].constant=false;}
				else prior[k].constant=true;
				nd++;prior[k].mean = hpd[nd];
				nd++;prior[k].sdshape = hpd[nd];
				ni++;prior[k].ndec = hpi[ni];
			} else prior[k].constant=true;
		}
		ni++;nn0 = hpi[ni];
		ne0 = new Ne0C[nn0];
		for (int k=0;k<nn0;k++) {
			ni++;ne0[k].val = hpi[ni];
			ns++;ne0[k].name =hps[ns];
		}
		for (int p=0;p<this->npart;p++) {
			this->particule[p].nparamvar=nparamvar;
			this->particule[p].paramvar = new int[nparamvar];
			for(int i=0;i<nparamvar;i++)this->particule[p].paramvar[i] =paramvar[i];
			this->particule[p].nparam = nparam;
			this->particule[p].parameterlist = new HistParameterC [nparam];
			for (int k=0;k<nparam;k++){
				this->particule[p].parameterlist[k].category = category[k];
				this->particule[p].parameterlist[k].name = name[k];
				this->particule[p].parameterlist[k].value = value[k];
				//if (p==0) cout << this->particule[p].parameterlist[k].name << "  " << this->particule[p].parameterlist[k].value << "  " << this->particule[p].parameterlist[k].category <<"   ";
				this->particule[p].parameterlist[k].prior.constant = prior[k].constant;
				if (not prior[k].constant) {
					this->particule[p].parameterlist[k].prior.loi = prior[k].loi;
					//if (p==0) cout << this->particule[p].parameterlist[k].prior.loi << "  [";
					this->particule[p].parameterlist[k].prior.mini =prior[k].mini;
					//if (p==0) cout << this->particule[p].parameterlist[k].prior.mini << "  ";
					this->particule[p].parameterlist[k].prior.maxi =prior[k].maxi;
					//if (p==0) cout << this->particule[p].parameterlist[k].prior.maxi << "  ";
					this->particule[p].parameterlist[k].prior.mean = prior[k].mean;
					//if (p==0) cout <<this->particule[p].parameterlist[k].prior.mean << "  ";
					this->particule[p].parameterlist[k].prior.sdshape = prior[k].sdshape;
					//if (p==0) cout << this->particule[p].parameterlist[k].prior.sdshape << "  ";
					this->particule[p].parameterlist[k].prior.ndec = prior[k].ndec;
					//if (p==0) cout <<this->particule[p].parameterlist[k].prior.ndec <<	"]\n";
				}
			}
			this->particule[p].nn0 = nn0;
			//if (p==0) cout  << "\nnombre de Ne = " << this->particule[p].nn0 <<"\n";
			this->particule[p].ne0list = new Ne0C[nn0];
			for (int k=0;k<nn0;k++){
				this->particule[p].ne0list[k].val = ne0[k].val;
				this->particule[p].ne0list[k].name = ne0[k].name;
				//if (p==0) cout << this->particule[p].ne0list[k].name <<"   " << this->particule[p].ne0list[k].val << "\n";
			}
		}
		delete []prior;
		delete []ne0;
		delete []category;
		delete []name;
		delete []value;
		delete []paramvar;
	}

	void printevents() {
		std::cout << "printevents : " << this->nevent << "\n";
		for (int kloc=0;kloc<this->nevent;kloc++){
			std::cout << "event " << kloc << "\n";
			if (this->eventlist[kloc].action  == 0)
			{std::cout << this->eventlist[kloc].time << "  SAMPLE   pop=" << this->eventlist[kloc].pop << "   sample =" << this->eventlist[kloc].sample << "  numevent0 =" << this->eventlist[kloc].numevent0 << "\n";}
			else if (this->eventlist[kloc].action  == 1)
			{std::cout << this->eventlist[kloc].time << "  VARNE    pop=" << this->eventlist[kloc].pop << "   Ne =" << this->eventlist[kloc].Ne << "  numevent0 =" << this->eventlist[kloc].numevent0 << "\n";}
			else if (this->eventlist[kloc].action  == 2)
			{std::cout << this->eventlist[kloc].time << "  MERGE    pop=" << this->eventlist[kloc].pop << "   pop1 =" << this->eventlist[kloc].pop1 << "  numevent0 =" << this->eventlist[kloc].numevent0 << "\n";}
			else if (this->eventlist[kloc].action  == 3)
			{std::cout << this->eventlist[kloc].time << "  SPLIT    pop=" << this->eventlist[kloc].pop << "   pop1 =" << this->eventlist[kloc].pop1 << "   pop2 =" << this->eventlist[kloc].pop2 << " admixrate =" << this->eventlist[kloc].admixrate << "  numevent0 =" << this->eventlist[kloc].numevent0 << "\n";}

			}
		}
*/

/*
	list gethaplostate(int loc) {
		list state;
//		for (int i=0;i<)
//		return this->locuslist[loc].haplostate;
		return state;
	}

	list gethaplodna(int loc) {
		list dna;
//		return this->locuslist[loc].haplodna;
		return dna;
	}
*/
	void cleanParticle(int ipart){
//nettoyage de locuslist
		for (int loc=0;loc<this->particule[ipart].nloc;loc++) {
			if (this->particule[ipart].locuslist[loc].type>4) {
				delete []this->particule[ipart].locuslist[loc].mutsit;
			    delete []this->particule[ipart].locuslist[loc].tabsit;
			}
			for(int sa=0;sa<this->particule[ipart].data.nsample;sa++) {
				if (this->particule[ipart].locuslist[loc].type<5) delete []this->particule[ipart].locuslist[loc].haplomic[sa];
				if (this->particule[ipart].locuslist[loc].type>4) delete []this->particule[ipart].locuslist[loc].haplodna[sa];
				delete []this->particule[ipart].locuslist[loc].freq[sa];
			}
			if (this->particule[ipart].locuslist[loc].type<5) delete []this->particule[ipart].locuslist[loc].haplomic;
			if (this->particule[ipart].locuslist[loc].type>4) delete []this->particule[ipart].locuslist[loc].haplodna;
			delete []this->particule[ipart].locuslist[loc].ss;
			delete []this->particule[ipart].locuslist[loc].samplesize;
			delete []this->particule[ipart].locuslist[loc].freq;
		}
		delete []this->particule[ipart].locuslist;
//nettoyage de grouplist		
		for (int gr=0;gr<this->particule[ipart].ngr;gr++) {
			delete []this->particule[ipart].grouplist[gr].loc;
			delete []this->particule[ipart].grouplist[gr].sumstat;
		}
		delete []this->particule[ipart].grouplist;
//nettoyage de data
		delete []this->particule[ipart].data.nind;
		for(int sa=0;sa<this->particule[ipart].data.nsample;sa++) delete []this->particule[ipart].data.indivsexe[sa];
		delete []this->particule[ipart].data.indivsexe;
//nettoyage de scenario
		for (int i=0;i<this->particule[ipart].nscenarios;i++) {
			delete []this->particule[ipart].scenario[i].parameters;
			delete []this->particule[ipart].scenario[i].time_sample;
			delete []this->particule[ipart].scenario[i].event;
			delete []this->particule[ipart].scenario[i].ne0;
			delete []this->particule[ipart].scenario[i].histparam;
		}
		delete []this->particule[ipart].scenario;
//nettoyage de paramvar
		if (this->particule[ipart].nparamvar>0) delete []this->particule[ipart].paramvar;
//nettoyage de matQ
		for (int i=0;i<4;i++) delete []this->particule[ipart].matQ[i];
		delete []this->particule[ipart].matQ;
//nettoage des données manquantes		
	        delete []this->particule[ipart].mhap;
	        delete []this->particule[ipart].mnuc;
	}
	
	void cleanParticleSet(){
		for (int i=0;i<this->npart;i++) this->cleanParticle(i);
		delete []this->particule;
		for (int loc=0;loc<this->nloc;loc++) {if (this->locuslist[loc].type>4)   delete []this->locuslist[loc].mutsit;}
		delete []this->locuslist;
		for (int gr=0;gr<this->ngr;gr++) delete []this->grouplist[gr].loc;
		delete []this->grouplist;
		delete []this->nind;
		for (int i=0;i<this->nsample;i++) delete[]this->indivsexe[i];
		delete []this->indivsexe;
	}

	double ** dosimultabref(HeaderC header,int npart, bool dnatrue)
		{
		//cout << "debut de dosimultabref\n";
		this->npart = npart;
		this->particule = new ParticleC[this->npart];
		//cout << "apres le new ParticleC\n";
		for (int p=0;p<this->npart;p++) {
			this->particule[p].dnatrue = dnatrue;
			this->setdata(p);
			this->setgroup(p);
			this->setloci(p);
			this->setscenarios(p);
		}
		int ipart,tid=0,jpart=0;
		int *sOK;
		sOK = new int[npart];
		double **paramstat;
		int nparamstat,k;
		//cout << "avant pragma\n";
//debut du pragma
//#pragma omp parallel for shared(sOK) private(ipart,tid) schedule(static)
		paramstat = new double*[npart];
		for (ipart=0;ipart<this->npart;ipart++){
			tid = 0;//omp_get_thread_num();
			sOK[ipart]=this->particule[ipart].dosimulpart(ipart,tid);
			//cout << "\nretour de dosimulpart sOK["<<ipart<<"] = "<<sOK[ipart] <<"\n";
			nparamstat = this->particule[ipart].scen.nparamvar;
			for (int gr=0;gr<this->particule[ipart].ngr;gr++) nparamstat +=this->particule[ipart].grouplist[gr].nstat;
			if (this->particule[ipart].nscenarios>1) nparamstat +=1;
			paramstat[ipart] = new double[nparamstat];
			if (sOK[ipart]==0) {
			 	for(int gr=0;gr<this->particule[ipart].ngr;gr++) this->particule[ipart].docalstat(gr);
			//cout << "retour de docalstat \n";
			}
		}
//fin du pragma
		//cout << "remplissage de paramstat \n";
		for (int ipart=0;ipart<this->npart;ipart++) {
			if (sOK[ipart]==0){
				k=0;
				if (this->particule[ipart].nscenarios>1) {paramstat[ipart][k]=this->particule[ipart].scen.number;k++;}
				for (int j=0;j<this->particule[ipart].scen.nparamvar;j++) {paramstat[ipart][k]=this->particule[ipart].scen.histparamvar[j].value;k++;}
				for(int gr=0;gr<this->particule[ipart].ngr;gr++){
					for (int st=0;st<this->particule[ipart].grouplist[gr].nstat;st++){paramstat[ipart][k]=this->particule[ipart].grouplist[gr].sumstat[st].val;k++;}
				}
				
				 
			}
		}
		//cout <<"fin du remplissage \n";
		cleanParticleSet();
		//cout << "fin de dosimultabref\n";
		return paramstat;
	}

/*	list getsumstat(){
//		cout << "npart = " << this->npart << "   nstat = " << this->particule[0].nstat << "\n";
		list stat;
		for (int ipart=0;ipart<this->npart;ipart++) {
			for (int j=0;j<this->particule[ipart].nstat;j++) {
				cout << "stat[" << ipart;
				cout << "][" << j;
				cout << "] = " ;
				cout << this->particule[ipart].stat[j].val;
				cout << "\n";
				stat.append(this->particule[ipart].stat[j].val);
			}
		}
		return stat;
	}

	void setoneloci(list loc_i) {
		this->particule[0].nloc = extract<int>(loc_i[0]);
//		cout <<"nloc="<<this->particule[0].nloc<<"\n";
		this->particule[0].locuslist = new LocusC[nloc];
		int n = 0;
		for (int kloc=0;kloc<this->particule[0].nloc;kloc++){
			n +=1;
			this->particule[0].locuslist[kloc].type =extract<int>(loc_i[n]);
			if (this->particule[0].locuslist[kloc].type < 5) {
				n +=1;this->particule[0].locuslist[kloc].kmin = extract<int>(loc_i[n]);
				n +=1;this->particule[0].locuslist[kloc].kmax = extract<int>(loc_i[n]);
				n +=1;this->particule[0].locuslist[kloc].motif_size = extract<int>(loc_i[n]);
				n +=1;this->particule[0].locuslist[kloc].motif_range = extract<int>(loc_i[n]);
				}
			else {
				n +=1;this->particule[0].locuslist[kloc].dnalength = extract<int>(loc_i[n]);
				n +=1;this->particule[0].locuslist[kloc].p_fixe = extract<double>(loc_i[n]);
				n +=1;this->particule[0].locuslist[kloc].gams = extract<double>(loc_i[n]);
				n +=1;this->particule[0].locuslist[kloc].mus_rate = extract<double>(loc_i[n]);
				n +=1;this->particule[0].locuslist[kloc].k1 = extract<double>(loc_i[n]);
				n +=1;this->particule[0].locuslist[kloc].k2 = extract<double>(loc_i[n]);
				n +=1;this->particule[0].locuslist[kloc].mutmod = extract<int>(loc_i[n]);
				n +=1;this->particule[0].locuslist[kloc].pi_A = extract<double>(loc_i[n]);
				n +=1;this->particule[0].locuslist[kloc].pi_C = extract<double>(loc_i[n]);
				n +=1;this->particule[0].locuslist[kloc].pi_G = extract<double>(loc_i[n]);
				n +=1;this->particule[0].locuslist[kloc].pi_T = extract<double>(loc_i[n]);
				this->particule[0].locuslist[kloc].mutsit = new double[locuslist[kloc].dnalength];
				for (int i=0;i<this->particule[0].locuslist[kloc].dnalength;i++) {
					n +=1;this->particule[0].locuslist[kloc].mutsit[i] = extract<double>(loc_i[n]);
					//std::cout << locuslist[kloc].mutsit[i]<< "   ";
					}
				//std::cout <<"\n";
				}
			double coeff=0.0;
//			cout << "sex-ratio = " << this->sexratio << "\n";
			switch (this->particule[0].locuslist[kloc].type % 5)
			{	case 0 :  coeff = 16.0*this->sexratio*(1.0-this->sexratio);break;
				case 1 :  coeff = 2.0;break;
				case 2 :  coeff = 18.0*this->sexratio*(1.0-this->sexratio)/(1.0+this->sexratio);break;
				case 3 :  coeff = 2.0*this->sexratio;break;
				case 4 :  coeff = 2.0*(1.0-this->sexratio);break;
			}
			this->particule[0].locuslist[kloc].coeff=coeff;
//			cout << "locus " << kloc << "   type=" << this->particule[0].locuslist[kloc].type << "   coeff=" << this->particule[0].locuslist[kloc].coeff << "\n";
		}
	}

	void sethaplo(list haplo) {
		int n=0;
		for (int loc=0;loc<this->particule[0].nloc;loc++){
			this->particule[0].locuslist[loc].ss = new int[this->nsample];
			this->particule[0].locuslist[loc].samplesize = new int[this->nsample];
			for (int sa=0;sa<this->nsample;sa++) {
				this->particule[0].locuslist[loc].ss[sa]=extract<int>(haplo[n]);n++;
//				cout << this->particule[0].locuslist[loc].ss[sa]<<", ";
			}
		}
//		cout << "\n";
		for (int loc=0;loc<this->particule[0].nloc;loc++){
			this->particule[0].locuslist[loc].haplomic = new int*[this->nsample];
			for (int sa=0;sa<this->nsample;sa++) {
				this->particule[0].locuslist[loc].samplesize[sa]=0;
				this->particule[0].locuslist[loc].haplomic[sa] = new int[this->particule[0].locuslist[loc].ss[sa]];

				for (int i=0;i<this->particule[0].locuslist[loc].ss[sa];i++) {
					this->particule[0].locuslist[loc].haplomic[sa][i] = extract<int>(haplo[n]);n++;
					if (this->particule[0].locuslist[loc].haplomic[sa][i]!=MISSING) this->particule[0].locuslist[loc].samplesize[sa]++;

				}
//				cout <<"samplesize["<<loc<<"]["<<sa<<"]="<< this->particule[0].locuslist[loc].samplesize[sa]<<"\n";
			}
		}

	}

	void setonepart(list da, list loci,list haplo) {
		setdata(da);
		setoneloci(loci);
		sethaplo(haplo);
	}

	list getonestat(){
//		cout << "npart = " << this->npart << "   nstat = " << this->particule[0].nstat << "\n";
		this->particule[0].docalstat();
		list stat;
		for (int j=0;j<this->particule[0].nstat;j++) stat.append(this->particule[0].stat[j].val);
		return stat;
	}
*/

};

//BOOST_PYTHON_MODULE(simuldatC)
//{
//	class_<ParticleSetC>("ParticleSet")
////		.def("setdnatrue", &ParticleSetC::setdnatrue)
////		.def("setevents", &ParticleSetC::setevents)
//		.def("dosimultabref", &ParticleSetC::dosimultabref)
//		.def("gethaplostate", &ParticleSetC::gethaplostate)
//		.def("gethaplodna", &ParticleSetC::gethaplodna)
////		.def("setstat", &ParticleSetC::setstat)
////		.def("setdataloc", &ParticleSetC::setdataloc)
////		.def("setparamn0", &ParticleSetC::setparamn0)
////		.def("setnpart", &ParticleSetC::setnpart)
//		.def("getsumstat", &ParticleSetC::getsumstat)
//		.def("setonepart",  &ParticleSetC::setonepart)
//		.def("getonestat", &ParticleSetC::getonestat)
//    ;
//}

/*void do_main(int npart) {
	int a[]={3, 30, 30, 30, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
	vector <int> va;
	va.assign(a,a+94);
	int b01[]={2,0,5,0,1,2,3,4,6,3,0,5,5,6,7,8,9};
	vector <int> vb01;
	vb01.assign(b01,b01+17);
	double b02[]={-1.0,0.0001,0.001,-1.0,-1.0,-1.0,0.01,0.3,-1.0,-1.0,0.0,0.0005,0.22,0.0};
	vector<double> vb02;
	vb02.assign(b02,b02+14);
	vector <string> vb03;
	vb03.resize(2);vb03[0]="UN";vb03[1]="UN";


	int    b1[]={10, 0, 0, 100, 180, 2, 40, 0, 2, 200, 280, 2, 40, 0, 4, 100, 180, 2, 40,  0, 0, 200, 280, 2, 40,  0, 0, 100, 180, 2, 40,  1, 0, 200, 280, 2, 40, 1, 0, 100, 180, 2, 40,  1, 0, 200, 280, 2, 40, 1, 0, 200, 280, 2, 40,1,0,200,280,2,40};
	vector <int> vb1;
	vb1.assign(b1,b1+61);
	double b2[]={ -1.0,0.00001,0.01,2.0,-1.0,0.01,0.5,2.0, 0.0,
			      -1.0,0.00001,0.01,2.0,-1.0,0.01,0.5,2.0, 0.0,
			      -1.0,0.00001,0.01,2.0,-1.0,0.01,0.5,2.0, 0.0,
			      -1.0,0.00001,0.01,2.0,-1.0,0.01,0.5,2.0, 0.0,
			      -1.0,0.00001,0.01,2.0,-1.0,0.01,0.5,2.0, 0.0,
			      0.0005,0.22,0.0,0.0005,0.22,0.0,0.0005,0.22,0.0,0.0005,0.22,0.0,0.0005,0.22,0.0};
	vector<double> vb2;
	vb2.assign(b2,b2+60);

	int    c1[]={8,0,0,0,0,0,0,1,0,1,0,0,0,2,4,1,0,3,-1,-1,-1};
	vector <int> vc1;
	vc1.assign(c1,c1+21);
	double c2[]={-1.0,1000.0,20000.0,0.0,0.0,-1.0,1000.0,20000.0, 0.0,0.0,-1.0,1000.0,20000.0,0.0,0.0,-1.0,100.0,1000.0,0.0,0.0,-1.0,1.0,10.0,0.0,0.0,-1.0,8.0,80.0,0.0,0.0,-1.0,0.001,0.999,0.0,0.0,-1.0,1000.0,100000.0,0.0,0.0};
	vector<double> vc2;
	vc2.assign(c2,c2+40);
	vector <string> vc3;
	vc3.resize(19);
	for (int i=0;i<8;i++){vc3[2*i+1]="UN";}
	vc3[0]="N1";vc3[2]="N2";vc3[4]="N3";vc3[6]="t1";vc3[8]="db";vc3[10]="Nb";vc3[12]="lambda";vc3[14]="t2";vc3[16]="N1";vc3[17]="N2";vc3[18]="N3";

	int    d1[]={6, 2, 0, 1, -1, -1, 1, -1,  0, 0, 0, 2, -1, -1, 2, -1, 1, 4, 0, 3, -1, -1, 3, -1,  2, -1, 1, 2, -1, -1, -1, -1,   3, -1,  3, 3, 1, 2, -1, -1, 4, -1, 2, 1, 2, -1, -1, -1, 5};
	vector <int> vd1;
	vd1.assign(d1,d1+49);
	double d2[]={-1.0,-1.0,-1.0,-1.0, -1.0, -1.0};
	vector<double> vd2;
	vd2.assign(d2,d2+6);
	vector <string> vd3;
	vd3.resize(10);
	vd3[0]="N1";vd3[1]="N2";vd3[2]="N3";vd3[3]="t1-db";vd3[4]="Nb";vd3[5]="t1";vd3[6]="N3";vd3[7]="lambda";vd3[8]="t2";vd3[9]="N1";

	int e[]={20, 1, 0, 0, 0, 0,
				 1, 1, 0, 0, 0,
				 1, 2, 0, 0, 0,
				 2, 0, 0, 0, 0,
				 2, 1, 0, 0, 0,
				 2, 2, 0, 0, 0,
				 3, 0, 0, 0, 0,
				 3, 1, 0, 0, 0,
				 3, 2, 0, 0, 0,
				 4, 0, 0, 0, 0,
				 4, 1, 0, 0, 0,
				 4, 2, 0, 0, 0,
				 5, 0, 2, 0, 1,
				 6, 0, 2, 0, 1,
				 7, 0, 2, 0, 1,
				 8, 0, 2, 0, 1,
				 9, 0, 2, 0, 1,
				 10, 0, 2, 0, 1,
				 11, 0, 2, 0, 1,
				 12, 2, 0, 1, 1};
	vector <int> ve;
	ve.assign(e,e+101);

	ParticleSetC pc;
	vector<double> resu;
	resu = pc.dosimultabref(npart, false, va,vb01,vb02,vb03,vb1,vb2,vc1,vc2,vc3,vd1,vd2,vd3,ve);
	cout << resu[0] <<" particules simulées avec succès\n";
}*/
