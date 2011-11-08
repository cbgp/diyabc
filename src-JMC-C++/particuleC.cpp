/*
 * particuleC.cpp
 *
 *  Created on: 23 sept. 2010
 *      Author: cornuet
 */
#ifndef MATHH
#include <math.h>
#define MATHH
#endif
#ifndef CMATH
#include <cmath>
#define CMATH
#endif
#ifndef RANDOMGENERATOR
#include "randomgenerator.cpp"
#define RANDOMGENERATOR
#endif
#ifndef VECTOR
#include <vector>
#define VECTOR
#endif
#ifndef IOSTREAM
#include <iostream>
#define IOSTREAM
#endif
#ifndef COMPLEX
#include <complex>
#define COMPLEX
#endif
#ifndef STRING
#include <string>
#define STRING
#endif
#ifndef MESUTILS
#include "mesutils.cpp"
#define MESUTILS
#endif
#ifndef DATA
#include "data.cpp"
#define DATA
#endif

#ifdef _OPENMP
#ifndef OMPH
#include <omp.h>
#define OMPH
#endif
#endif

using namespace std;
#define MICMISSING -9999
#define SEQMISSING ""
#define SNPMISSING 9
#define NUCMISSING 'N'
#define NSTAT 43

string stat_type[NSTAT] = {"PID","NAL","HET","VAR","MGW","N2P","H2P","V2P","FST","LIK","DAS","DM2","AML","NHA","NSS","MPD","VPD","DTA","PSS","MNS","VNS","NH2","NS2","MP2","MPB","HST","SML","SHM","SHV","SHF","SHT","SNM","SNV","SNF","SNT","SFM","SFV","SFF","SFT","SAM","SAV","SAF","SAT"};
int stat_num[NSTAT]     = {  0  ,  1  ,  2  ,  3  ,  4  ,  5  ,  6  ,  7  ,  8  ,  9  ,  10 ,  11 ,  12 , -1  , -2  , -3  , -4  , -5  , -6  , -7  , -8  , -9  , -10 , -11 , -12 , -13 , -14 ,  21 ,  22 ,  23 ,  24 ,  25 ,  26 ,  27 ,  28 ,  29 ,  30 ,  31 ,  32 ,  33 ,  34 ,  35 ,  36};
/*  Numérotation des stat
 * 	1 : nal			-1 : nha			 21 : moyenne des genic diversities
 *  2 : het			-2 : nss             22 : variance des genic diversities
 *  3 : var			-3 : mpd			 23 : premier quartile des genic diversities
 *  4 : MGW			-4 : vpd			 24 : troisième quartile des genic diversities
 *  5 : Fst			-5 : dta			 25 : moyenne des distances de Nei
 *  6 : lik			-6 : pss			 26 : variance des distances de Nei
 *  7 : dm2			-7 : mns			 27 : premier quartile des distances de Nei
 *  8 : n2P			-8 : vns			 28 : troisième quartile des distances de Nei
 *  9 : h2P			-9 : nh2			 29 : moyenne des distances Fst 
 * 10 : v2P		   -10 : ns2			 30 : variance des distances Fst
 * 11 : das        -11 : mp2			 31 : premier quartile des distances Fst
 * 12 : Aml        -12 : mpb			 32 : troisième quartile des distances Fst
 * 				   -13 : fst			 33 : moyenne des estimations d'admixture
 * 				   -14 : aml			 34 : variance des estimations d'admixture
 * 										 35 : premier quartile des estimations d'admixture
 * 										 36 : troisième quartile des estimations d'admixture
 */
    vector <string> histparname;
    vector <int> histparcat;

extern int debuglevel;
extern char *reftablelogfilename;

/**
* retourne un ordre aléatoire pour les éléments d'un vecteur
*/
vector <int> melange(MwcGen mw, int n) {
	vector <double> ra;
	vector <int> ord;
	ord.resize(n);ra.resize(n);
	for (int i=0;i<n;i++) ra[i] = mw.random();
	for (int i=0;i<n;i++) {
		ord[i] = 0;
		for (int j=0;j<n;j++) {if (ra[i]>ra[j]) ord[i]++;}
	}
	return ord;
}

/**
* Classe PriorC :éléments de définition d'un prior de paramètre historique
*/
class PriorC
{
public:
	string loi;
	double mini,maxi,mean,sdshape;
	int ndec;
	bool constant,fixed;

	void ecris(){
		cout <<"    loi="<<this->loi<<"   min="<<this->mini<<"   max="<<this->maxi<<"   ndec="<<this->ndec;
		if (this->loi=="GA") cout <<"    shape="<<this->sdshape;
		if (this->constant) cout<<"   constant"; else cout<<"   non constant";
		if (this->fixed) cout<<"   fixed\n";else cout<<"   non fixed\n";
	}

};

/**
* Classe ConditionC :éléments de définition d'une condition sur un couple de paramètres historiques
*/

struct ConditionC
{
	string param1,param2,operateur;

	void ecris(){cout<<this->param1<<"  "<<this->operateur<<"  "<<this->param2<<"\n";}

};

/**
* struct StatC :éléments de définition d'une summary statistic
*/
struct StatC
{
	int cat,samp,samp1,samp2,group,numsnp;
	long double val;
};

/**
* struct StatsnpC :éléments de définition d'une summary statistic pour les snp
*/
struct StatsnpC
{
	int cat,samp,samp1,samp2,group,n;
	long double *x;
	bool defined,sorted;
};



/**
* Classe EventC :éléments de définition d'un événement populationnel
*/
class EventC
{
public:
	char action;   //"V"=VarNe "M"=Merge  "S"=Split  "E" = sample/echantillon
	int pop,pop1,pop2,sample,Ne,time;
    	double admixrate;
    	int numevent0;
    	char *stime,*sNe,*sadmixrate;
        int ltime,lNe,ladmixrate;

    void libere() {
        if (ltime>0) delete []stime;
        if (lNe>0) delete []sNe;
        if (ladmixrate>0) delete []sadmixrate;
    }

    void ecris(){
        string sstime;
        if (this->time!=-9999) sstime=IntToString(this->time);
        else {cout<<this->stime<<"\n";sstime=char2string(this->stime);}
		//cout <<"ltime="<<this->ltime<<"\n";
             if (this->action=='V') cout<<"    "<<sstime<<"   VarNe  pop="<<this->pop<<"   "<<this->sNe<<"\n";
        else if (this->action=='M') cout<<"    "<<sstime<<"   merge  pop="<<this->pop<<"   pop1="<<this->pop1<<"\n";
        else if (this->action=='S') cout<<"    "<<sstime<<"   split  pop="<<this->pop<<"   pop1="<<this->pop1<<"   pop2="<<this->pop2<<"   "<<this->sadmixrate<<"\n";
        else if (this->action=='E') cout<<"    "<<sstime<<"   samp   pop="<<this->pop<<"\n";

	}
};

/**
* Définition de l'opérateur pour le tri des événements populationnels
*/
struct compevent
{
   bool operator() (const EventC & lhs, const EventC & rhs) const
   {
      return lhs.time < rhs.time;
   }
};

/**
* struct Ne0C : éléments de définition d'un effectif efficace à la ligne 1 d'un scénario
*/
struct Ne0C
{
	int val,lon;
	char *name;

    void libere() {
        delete []this->name;
    }
};

/**
* Classe HistparameterC :éléments de définition d'un paramètre historique
*/
class HistParameterC
{
public:
	string name;
	int category;   //0 pour N, 1 pour T et 2 pour A
	double value;
	PriorC prior;

    void libere(){
        //delete &(this->prior);
    }

	void ecris() {
		cout<<"    name="<<this->name<<"   val="<<this->value<<"   category="<<this->category<<"\n";
		prior.ecris();
	}
};

/**
* struct LocusGroupC : éléments de définition d'un groupe de locus
*/
struct LocusGroupC
{
	int *loc,nloc,nstat,nstatsnp;           // *loc=numeros des locus du groupe
	int type;                      //O= microsat, 1=sequence
	double p_fixe,gams;
	double musmoy,mutmoy,Pmoy,snimoy;
	double k1moy,k2moy;
	int  mutmod;
	PriorC priormusmoy,priork1moy,priork2moy,priormusloc,priork1loc,priork2loc;
	PriorC priormutmoy,priorPmoy,priorsnimoy,priormutloc,priorPloc,priorsniloc;
	StatC *sumstat;
	StatsnpC *sumstatsnp;

    void libere() {
        delete []loc;
        for (int i=0;i<this->nstat;i++) free(&(this->sumstat[i]));
        delete []this->sumstat;

    }

};

/**
* Classe ScenarioC :éléments de définition d'un scénario
*/
class ScenarioC
{
public:
	double *paramvar,prior_proba;
    int number,popmax,npop,nsamp,*time_sample,nparam,nevent,nn0,nparamvar,nconditions,ipv;
    EventC *event;
    Ne0C *ne0;
    HistParameterC *histparam;
    ConditionC *condition;


    /* action = 0 (varne), 1 (merge), 2 (split), 3 (adsamp)
     * category=0 (Ne)   , 1 (time),  3 (admixrate)
    */

    void libere() {
        //cout<<"avant delete paramvar\n";
        delete []this->paramvar;
        //cout<<"avant delete event\n";
        for (int i=0;i<this->nevent;i++) this->event[i].libere();
        delete []this->event;
        //cout<<"avant delete ne0\n";
        for (int i=0;i<this->nn0;i++) this->ne0[i].libere();
        delete []this->ne0;
        //cout<<"avant delete histparam\n";
        for (int i=0;i<this->nparam;i++) this->histparam[i].libere();
        delete []this->histparam;
        //cout<<"avant delete condition\n";
        if (this->nconditions>0) delete []this->condition;
        //cout<<"         apres libere scenario\n";
        delete []this->time_sample;
    }

/**
* Classe ScenarioC : détermination du ou des paramètres contenus dans la string s
*/
    void detparam(string s,int cat)
    {	string s1;
        int i,j;
        size_t plus,minus,posigne;
        vector <string> ss;
        bool trouve;
        s1=s;
        //cout<<"detparam s1="<<s1<<"\n";
        while (s1.length()>0) {
                plus=s1.find("+");
                minus=s1.find("-"); //cout<<"minus="<<minus<<"\n";
                if ((plus == string::npos)and(minus == string::npos)) {ss.push_back(s1);s1="";}
                else {
                        if (plus!= string::npos) posigne=plus;
                        else		 posigne=minus;
                        //cout<<"posigne = "<<posigne<<"\n";
                        ss.push_back(s1.substr(0,posigne));
                        s1=s1.substr(posigne+1);
                        //cout<<"s1="<<s1<<"\n";;
                }
        }
        for (i=0;i<ss.size();i++) {
            //cout<<"ss["<<i<<"]="<<ss[i]<<"\n";
                //cout<<"   this->nparam = "<<this->nparam<<"\n";
                if (this->nparam>0) {
                        trouve=false;
                        j=0;
                        while ((not trouve)and(j<this->nparam)) {
                                trouve= (ss[i].compare(histparname[j])==0);
                                //(pr.name == this->histpar[j]->name);
                                //cout<<"histparname["<<j <<"] = "<<histparname[j]<<"   trouve="<<trouve<<"\n";
                                j++;
                        }
                        if (not trouve) {histparname.push_back(ss[i]);histparcat.push_back(cat);this->nparam++;}
                        //else cout<<"deja\n";
                } else {histparname.push_back(ss[i]);histparcat.push_back(cat);this->nparam++;}
        }
        /*cout<<"dans detparam  size = "<<histparname.size()<<"   nparam="<<this->nparam<<"\n";
        for (int i=0;i<histparname.size();i++) cout<<"histparname = "<<histparname[i]<<"\n";
        cout<<"\n";*/
    }


/**
* Classe ScenarioC : lecture/interprétation des lignes d'un scénario
*/
    void read_events(int nl,string *ls){
    	string *ss,sevent;
    	int n;
    	ss = splitwords(ls[0]," ",&n);
    	this->nn0=n;this->nparam=0;
    	this->ne0 = new Ne0C[this->nn0];
    	for (int i=0;i<this->nn0;i++) {
    		if (atoi(ss[i].c_str())==0) {
                    this->ne0[i].lon = ss[i].length()+1;
                    this->ne0[i].name = new char[this->ne0[i].lon];
                   strcpy(this->ne0[i].name,ss[i].c_str());this->ne0[i].val=-1;this->detparam(ss[i],0);}
    		else               {strcpy(this->ne0[i].name,"");this->ne0[i].val=atoi(ss[i].c_str());this->ne0[i].lon=1;}
    	}
    	this->nevent = nl-1;
    	this->event = new EventC[nl-1];
    	this->nsamp=0;this->npop=this->nn0;this->popmax=this->nn0;
		delete []ss;
    	for (int i=0;i<nl-1;i++) {
    		ss = splitwords(ls[i+1]," ",&n);
            this->event[i].ltime=0;this->event[i].lNe=0;this->event[i].ladmixrate=0;
    		//this->event[i].stime=new char[this->event[i].ltime];this->event[i].stime[0]='\0';
            //this->event[i].sNe=new char[this->event[i].lNe];this->event[i].sNe[0]='\0';
            //this->event[i].sadmixrate=new char[this->event[i].ladmixrate];this->event[i].sadmixrate[0]='\0';
     		if (ss[0]=="0") {this->event[i].time=0;}
    		else if (atoi(ss[0].c_str())==0) {
		     this->event[i].time=-9999;
                     this->event[i].ltime=ss[0].length()+1;
					 //cout<<"read_events ltime="<<this->event[i].ltime<<"\n";
                     this->event[i].stime=new char[this->event[i].ltime];
					 //cout<<"read_events avant le strcpy\n";
                     strcpy(this->event[i].stime,ss[0].c_str());
					 //cout<<"avant this->detparam("<<ss[0]<<",1)\n";
					 this->detparam(ss[0],1);
					 //cout<<"apres this->detparam("<<ss[0]<<",1)\n";
			}
    		else {this->event[i].time=atoi(ss[0].c_str());}
    		sevent=majuscules(ss[1]);
			//cout<<"apres majuscules(ss[1])";
    		if (sevent=="SAMPLE") {
    			this->event[i].action='E';
    			this->event[i].pop=atoi(ss[2].c_str());
    			this->nsamp++;
			    this->event[i].sample=this->nsamp;
    			//cout <<this->event[i].time<<"  SAMPLE"<<"   "<<this->event[i].pop<<"\n";
    		} else if (sevent=="MERGE") {
    			this->event[i].action='M';
    			this->event[i].pop=atoi(ss[2].c_str());
    			this->event[i].pop1=atoi(ss[3].c_str());
    			//cout <<this->event[i].stime<<"  MERGE"<<"   "<<this->event[i].pop<<"   "<<this->event[i].pop1<<"\n";

    		} else if (sevent=="SPLIT") {
    			this->event[i].action='S';
    			this->event[i].pop=atoi(ss[2].c_str());
    			this->event[i].pop1=atoi(ss[3].c_str());
    			this->event[i].pop2=atoi(ss[4].c_str());
    			if (atof(ss[5].c_str())!=0.0) this->event[i].admixrate=atof(ss[5].c_str());
    			else {
                              this->event[i].admixrate=-1.0;
                              this->event[i].ladmixrate=ss[5].length()+1;
                              this->event[i].sadmixrate=new char[this->event[i].ladmixrate];
                              strcpy(this->event[i].sadmixrate,ss[5].c_str());
                              this->detparam(ss[5],2);
                        }
    			//cout <<this->event[i].stime<<"  SPLIT"<<"   "<<this->event[i].pop<<"   "<<this->event[i].pop1<<"   "<<this->event[i].pop2<<"   "<<this->event[i].sadmixrate<<"\n";
    		} else if (sevent=="VARNE") {
    			this->event[i].action='V';
    			//cout<<"this->event[i].action = "<<this->event[i].action<<"\n";
    			this->event[i].pop=atoi(ss[2].c_str());
				//cout<<"this->event[i].pop = "<<this->event[i].pop<<"\n";
				//cout<<"ss[3] = "<<ss[3]<<"\n";
				//cout<<"atoi(ss[3].c_str()) = "<<atoi(ss[3].c_str())<<"\n";
    			if (atoi(ss[3].c_str())!=0) this->event[i].Ne=atoi(ss[3].c_str());
    			else {   
                              this->event[i].Ne=-1;
							  //cout<<"this->event[i].Ne = "<<this->event[i].Ne<<"\n";
                              this->event[i].lNe=ss[3].length()+1;
							  //cout<<"this->event[i].lNe="<<this->event[i].lNe<<"\n";
                              this->event[i].sNe=new char[this->event[i].lNe];
							  //cout<<"avant le strcpy\n";
                              strcpy(this->event[i].sNe,ss[3].c_str());
							  //cout<<"avant this->detparam("<<ss[3]<<",0)\n";
                              this->detparam(ss[3],0);
                        }
                //cout <<this->event[i].stime<<"  VARNE"<<"   "<<this->event[i].pop<<"\n";
    		}
    		delete []ss;
    	}
    	this->histparam = new HistParameterC[this->nparam];
    	//this->paramvar = new HistParameterC[this->nparamvar];
        //cout << "this->nparam = "<<this->nparam<<"   size= "<<histparname.size()<<"\n";
    	for (int i=0;i<this->nparam;i++){
                //cout<<"i = "<<i<<"\n";
    		this->histparam[i].name=histparname[i];
                //cout << this->histparam[i].name<<"\n";
    		this->histparam[i].category=histparcat[i];
		if (this->histparam[i].category=='T')this->histparam[i].value=-9999; else this->histparam[i].value=-1;
    	}
    	histparname.clear();histparcat.clear();
    	this->time_sample = new int[this->nsamp];
    	n=-1;
    	for (int i=0;i<this->nevent;i++) {if (this->event[i].action=='E') {n++;this->time_sample[n]=this->event[i].time;}}
    }

    void ecris() {
    	cout <<"scenario "<<this->number<<"   ("<<this->prior_proba<<")\n";
    	cout <<"    nevent="<<this->nevent<<"   nne0="<<nn0<<"\n";
    	cout <<"    nsamp ="<<this->nsamp<<"   npop="<<this->npop<<"   popmax="<<this->popmax<< "\n";
    	cout <<"    nparam="<<this->nparam<<"    nparamvar="<<this->nparamvar<<"\n";
    	for (int i=0;i<this->nsamp;i++) cout<<"    samp "<<i+1<<"    time_sample = "<<this->time_sample[i]<<"\n";
    	cout<<"    events:\n";
    	for (int i=0;i<this->nevent;i++) this->event[i].ecris();
    	cout<<"    histparam:\n";
    	for (int i=0;i<this->nparam;i++) this->histparam[i].ecris();
    	cout<<"    nparamvar = "<<this->nparamvar<<"\n";cout<<" paramvar : ";
     	for (int i=0;i<this->nparamvar;i++) cout<<this->paramvar[i]<<"   ";cout<<"\n";
     	cout <<"   nconditions="<<this->nconditions<<"\n";
     	if (this->nconditions>0) for (int i=0;i<this->nconditions;i++) this->condition[i].ecris();
    }
};

/**
* Copie du contenu d'une classe PriorC
*/
PriorC copyprior(PriorC source) {
	PriorC dest;
	dest.loi = source.loi;
	dest.mini = source.mini;
	dest.maxi = source.maxi;
	dest.mean = source.mean;
	dest.sdshape = source.sdshape;
	dest.constant = source.constant;
	dest.ndec  = source.ndec;
	dest.fixed = source.fixed;
	return dest;
}

/**
* Copie du contenu d'une classe EventC
*/
EventC copyevent(EventC source) {
	EventC dest;
	dest.action =source.action;
	dest.pop = source.pop;
	dest.pop1 = source.pop1;
	dest.pop2 = source.pop2;
	dest.sample = source.sample;
	dest.Ne = source.Ne;
	dest.time = source.time;
	dest.admixrate = source.admixrate;
	dest.numevent0 = source.numevent0;
    dest.ltime = source.ltime;
    if (source.ltime>0) {
        dest.stime = new char[source.ltime];
        for (int i=0;i<dest.ltime;i++)dest.stime[i]=source.stime[i];
    }
    dest.lNe = source.lNe;
	if (source.lNe>0) {
        dest.sNe = new char[source.lNe];
        for (int i=0;i<dest.lNe;i++)dest.sNe[i]=source.sNe[i];
    }
    dest.ladmixrate = source.ladmixrate;
    if (source.ladmixrate>0) {
        dest.sadmixrate = new char[source.ladmixrate];
        for (int i=0;i<dest.ladmixrate;i++)dest.sadmixrate[i]=source.sadmixrate[i];
    }
	return dest;
}

/**
* Copie du contenu d'une classe Ne0C
*/
Ne0C copyne0(Ne0C source) {
	Ne0C dest;
    dest.lon=source.lon;
    dest.name = new char[source.lon];
    if (dest.lon>0) for (int i=0;i<dest.lon;i++) dest.name[i]=source.name[i];
	dest.val = source.val;
	return dest;
}

/**
* Copie du contenu d'une classe HistParameterC
*/
HistParameterC copyhistparameter(HistParameterC source) {
	HistParameterC dest;
	dest.name = source.name;
	dest.category = source.category;
	dest.value = source.value;
	dest.prior = copyprior(source.prior);
	return dest;
}

/**
* Copie du contenu d'une classe ConditionC
*/
ConditionC copycondition(ConditionC source) {
    ConditionC dest;
    dest.param1 = source.param1;
    dest.param2 = source.param2;
    dest.operateur = source.operateur;
    return dest;
}

/**
* Copie du contenu d'une classe ScenarioC
*/
ScenarioC copyscenario(ScenarioC source) {
  //cout<<"debut de copyscenario\n";
	ScenarioC dest;
	dest.prior_proba = source.prior_proba;
	dest.number = source.number;
	dest.popmax = source.popmax;
	dest.npop = source.npop;
	dest.nsamp = source.nsamp;
	dest.nparam = source.nparam;
	dest.nparamvar = source.nparamvar;
	dest.nevent = source.nevent;
	dest.nn0 = source.nn0;
	dest.nconditions = source.nconditions;
    //cout<<"    fin des copy simples  dest.nevent ="<<dest.nevent<<"\n";
    dest.event = new EventC[dest.nevent];
	for (int i=0;i<dest.nevent;i++) dest.event[i] = copyevent(source.event[i]);
       // cout<<"   apres copyevent\n";
	dest.ne0 = new Ne0C[dest.nn0];
	for (int i=0;i<dest.nn0;i++) dest.ne0[i] = copyne0(source.ne0[i]);
      //  cout<<"   apres copyne0\n";
	dest.time_sample = new int[dest.nsamp];
	for (int i=0;i<dest.nsamp;i++) dest.time_sample[i] = source.time_sample[i];
       // cout<<"   apres copytime_sample\n";
	dest.histparam = new HistParameterC[dest.nparam];
	for (int i=0;i<dest.nparam;i++) {dest.histparam[i] = copyhistparameter(source.histparam[i]);/*cout<<dest.histparam[i].name<<"\n"<<flush;*/}
      //  cout<<"   apres copyhistparam\n";
	dest.paramvar = new double[dest.nparamvar];
	for (int i=0;i<dest.nparamvar;i++) {dest.paramvar[i] = source.paramvar[i];/*cout<<dest.histparam[i].name<<"\n"<<flush;*/}
      //  cout<<"   apres copyparamvar\n";
    if (dest.nconditions>0) {
            dest.condition = new ConditionC[dest.nconditions];
            for (int i=0;i<dest.nconditions;i++) dest.condition[i] = copycondition(source.condition[i]);
        }
	return dest;
}


/**
* Struct SequenceBitC : éléments de définition d'un segment (vertical) de l'arbre de coalescence'
*/
struct SequenceBitC
{
    /* action = "C" (coal), "M" (merge), "S" (split), "A" (adsamp)
	*/
    char action;
    int pop,pop1,pop2,sample;
    int N,t0,t1;
    double admixrate;
    bool *popfull;

    void ecris() {
        switch (action){
          case 'C' : cout <<"coalescence dans pop "<<this->pop<<"   N="<<this->N<<"   t0="<<this->t0<<"   t1="<<this->t1<<"\n";break;
          case 'M' : cout << "merge pop="<<this->pop<<"  pop1="<<this->pop1<<"  à t0="<<this->t0<<"\n";break;
          case 'S' : cout <<"split de pop="<<this->pop<<"  vers pop1="<<this->pop1<<" et pop2="<<this->pop2<<"  à t0="<<this->t0<<"\n";break;
          case 'A' : cout <<"add sample à pop="<<this->pop<<"  à t0="<<this->t0<<"\n";break;
        }
    }

};

/**
* Struct NodeC : éléments de définition d'un noeud de l'arbre de coalescence
*/
struct NodeC
{
	int pop,sample,state,brhaut,brbas;
    double height;
	string dna;
	bool OK;
};

/**
* Struct BranchC : éléments de définition d'une branche de l'arbre de coalescence
*/
struct BranchC
{
	int bottom,top,nmut;
	double length;
	bool OK;
};

/**
* Struct BranchC : éléments de définition d'un arbre de coalescence
*/
struct GeneTreeC
{
	NodeC *nodes;
	BranchC *branches;
	int nmutot,nnodes,nbranches,ngenes;
};


/**
* Struct ParticleC : éléments de définition d'une particule
*/
struct ParticleC
{
	LocusC *locuslist;
	LocusGroupC *grouplist;
	DataC  data;
	ScenarioC *scenario,scen;
	SequenceBitC *seqlist;
	GeneTreeC *gt;
	MwcGen mw;
	ConditionC *condition;
	bool dnatrue,drawuntil;
    vector < vector <bool> > afsdone;
    vector < vector < vector <int> > > t_afs;
    vector < vector <int> > n_afs;

	int npart,nloc,ngr,nparam,nseq,nstat,nsample,*nind,**indivsexe,nscenarios,nconditions,**numvar,*nvar;
	double matQ[4][4];

    void libere(bool obs) {
         for (int i=0;i<this->nloc;i++) locuslist[i].libere(obs,this->nsample);
         delete []locuslist;

    }

    void ecris(){
		for (int i=0;i<this->nscenarios;i++) this->scenario[i].ecris();
	}

/**
* Struct ParticleC : calcule le nombre de copies de gènes de l'individu i de l'échantillon sa au locus loc
*/
	int calploidy(int loc, int sa,int i) {
      int typ = this->locuslist[loc].type%5;
		if ((typ == 0)  //AUTOSOMAL DIPLOID
				or((typ == 2)and(this->data.indivsexe[sa][i] == 2))) //X-LINKED + FEMALE
					{return 2;}
		else if ((typ == 3)and(this->data.indivsexe[sa][i] == 2))  //Y-LINKED + FEMALE
					{return 1;}
		else 		{return 1;}   //AUTOSOMAL HAPLOID or X-LINKED MALE or MITOCHONDRIAL

	}

/**
* Struct ParticleC : recopie le scénario numscen dans this->scen.
* Si numscen<1, tirage au sort préalable du scenario dans le prior
*/
	void drawscenario(int numscen) {
		double ra,sp=0.0;
        int iscen;
        if (numscen<1) {
            ra = this->mw.random();
            iscen=-1;
            while ((ra>sp)and(iscen<this->nscenarios-1)) {
                iscen++;sp +=this->scenario[iscen].prior_proba;
            }
        } else iscen=numscen-1;
        this->scen.prior_proba = this->scenario[iscen].prior_proba;
        this->scen.number = this->scenario[iscen].number;
        this->scen.popmax = this->scenario[iscen].popmax;
        this->scen.npop = this->scenario[iscen].npop;
        this->scen.nsamp = this->scenario[iscen].nsamp;
        this->scen.nparam = this->scenario[iscen].nparam;
        this->scen.nparamvar = this->scenario[iscen].nparamvar;
        this->scen.nevent = this->scenario[iscen].nevent;
        this->scen.nn0 = this->scenario[iscen].nn0;
        this->scen.nconditions = this->scenario[iscen].nconditions;
        for (int i=0;i<this->scen.nsamp;i++) this->scen.time_sample[i] = this->scenario[iscen].time_sample[i];
        for (int i=0;i<this->scen.nn0;i++) {
              //if ((i>5)or(iscen>2)) cout <<"i="<<i<<"   iscen="<<iscen<<"\n";
              this->scen.ne0[i].val = this->scenario[iscen].ne0[i].val;
              strcpy(this->scen.ne0[i].name,this->scenario[iscen].ne0[i].name);
        }
        //for (int i=0;i<this->scen.nn0;i++) this->scen.ne0[i] = copyne0(this->scenario[iscen].ne0[i]);
        for (int i=0;i<this->scen.nevent;i++) {
              this->scen.event[i].libere();
              this->scen.event[i] = copyevent(this->scenario[iscen].event[i]);
        }
        for (int i=0;i<this->scen.nparam;i++) {this->scen.histparam[i] = copyhistparameter(this->scenario[iscen].histparam[i]);/*cout<<this->scen.histparam[i].name<<"\n"<<flush;*/}
        for (int i=0;i<this->scen.nparamvar;i++) {this->scen.paramvar[i] = this->scenario[iscen].paramvar[i];/*cout<<this->scen.histparam[i].name<<"\n"<<flush;*/}
        if (this->scen.nconditions>0) {
            for (int i=0;i<this->scen.nconditions;i++) this->scen.condition[i] = copycondition(this->scenario[iscen].condition[i]);
        }
		//cout<<"drawscenario nparamvar="<<this->scen.nparamvar<<"\n";
		//this->scen.ecris();
	}

/**
* Struct ParticleC : copie le contenu d'une structure GeneTreeC
*/
	GeneTreeC copytree(GeneTreeC source) {
		GeneTreeC dest;
		dest.nnodes = source.nnodes;
		dest.ngenes = source.ngenes;
		dest.nbranches = source.nbranches;
		dest.branches = new BranchC[dest.nbranches];
		dest.nodes = new NodeC[dest.nnodes];
		for (int b=0;b<dest.nbranches;b++) {
			dest.branches[b].top = source.branches[b].top;
			dest.branches[b].bottom = source.branches[b].bottom;
			dest.branches[b].length = source.branches[b].length;
			dest.branches[b].OK=source.branches[b].OK;
		}
		for (int n=0;n<dest.nnodes;n++) {
			dest.nodes[n].pop = source.nodes[n].pop;
			dest.nodes[n].height = source.nodes[n].height;
			dest.nodes[n].sample = source.nodes[n].sample;
		}
		return dest;
	}

/**
* Struct ParticleC : libère le contenu d'une structure GeneTreeC
*/
	void deletetree(GeneTreeC tree,bool dna) {
        if (dna) for (int i=0;i<tree.nnodes;i++) tree.nodes[i].dna.clear();
		delete [] tree.nodes;
		delete [] tree.branches;
	}

/**
* Struct ParticleC : tire une valeurde type double dans un prior
*/
	double drawfromprior(PriorC prior) {
		double r;
		if (prior.mini==prior.maxi) return prior.mini;
		if (prior.loi=="UN") return this->mw.gunif(prior.mini,prior.maxi);
		if (prior.loi=="LU") return this->mw.glogunif(prior.mini,prior.maxi);
		if (prior.loi=="NO") {
			do {r = this->mw.gnorm(prior.mean,prior.sdshape);}
			while ((r<prior.mini)or(r>prior.maxi));
			return r;
		}
		if (prior.loi=="LN") {
			do {r = this->mw.glognorm(prior.mean,prior.sdshape);}
			while ((r<prior.mini)or(r>prior.maxi));
			return r;
		}
		if (prior.loi=="GA") {
			if (prior.mean<1E-12) return 0;
			if (prior.sdshape<1E-12) return prior.mean;
			if (prior.maxi<1E-12) return prior.maxi;
			do {r = this->mw.ggamma3(prior.mean,prior.sdshape);}
			while ((r<prior.mini)or(r>prior.maxi));
			return r;
		}
		return -1.0;
	}

/**
* Struct ParticleC : retourne la valeur d'un paramètre à partir de son nom (propriété name)
*/
	double param2val(string paramname){
		int ip;
		//cout<<"paramname = "<<paramname<<"\n";
		for (ip=0;ip<this->scen.nparam;ip++) {if (paramname==this->scen.histparam[ip].name) break;}
		//cout<<"param "<<ip<<"    value="<<this->scen.histparam[ip].value<<"\n";fflush(stdin);
		return this->scen.histparam[ip].value;

	}

/**
* Struct ParticleC : retourne la valeur d'une expression du genre param1 +param2-param3
*/
	double getvalue(string line) {
		double result;
		bool fin=false;
		//cout<<"line="<<line<<"\n";
		string operateur,plus="+",minus="-";
		size_t posign,posplus,posminus;
		posplus=line.find('+');posminus=line.find('-');
		if ((posplus==string::npos)and(posminus==string::npos)) {return  this->param2val(line);}
		else {if ((posplus>=0)and(posminus>line.size())){posign=posplus;}
			else {if ((posminus>=0)and(posplus>line.size())){posign=posminus;}
				else {if (posplus<posminus) {posign=posplus;} else {posign=posminus;}}}
		}
		result=this->param2val(line.substr(0,posign));
		//cout << result;
		operateur=line.substr(posign,1);
		//cout <<"   "<<operateur<<"   ";
		line=line.substr(posign+1);
		//cout << line << "\n";
		while (not fin) {
			posplus=line.find('+');posminus=line.find('-');
			if ((posplus>line.size())and(posminus>line.size())) {
				if (operateur==plus) {result +=this->param2val(line);}
				if (operateur==minus) {result -=this->param2val(line);}
				//cout << "result = " << result <<"\n";
				fin=true;break;
			}
			else {if ((posplus>=0)and(posminus>line.size())){posign=posplus;}
			else {if ((posminus>=0)and(posplus>line.size())){posign=posminus;}
				else {if (posplus<posminus) {posign=posplus;} else {posign=posminus;}}}
			}
			if (operateur==plus) {result +=this->param2val(line.substr(0,posign));}
			if (operateur==minus) {result -=this->param2val(line.substr(0,posign));}
			operateur=line.substr(posign,1);
			line=line.substr(posign+1);
		}
		return result;
	}

/**
* Struct ParticleC : indique si toutes les conditions sur les paramètres sont vérifiées
*/
	bool conditionsOK() {
	    bool OK=true;
	    int ip1,ip2;
	    int i=0;
        if (debuglevel==10) cout<<this->scen.nconditions<<" conditions à remplir\n";
	    while ((OK)and(i<this->scen.nconditions)){
	        //this->scen.condition[i].ecris();

	        ip1=0;while (this->scen.condition [i].param1!=this->scen.histparam[ip1].name) ip1++;
	        ip2=0;while (this->scen.condition [i].param2!=this->scen.histparam[ip2].name) ip2++;
	        if (this->scen.condition[i].operateur==">")       OK=(this->scen.histparam[ip1].value > this->scen.histparam[ip2].value);
	        else if (this->scen.condition[i].operateur=="<")  OK=(this->scen.histparam[ip1].value < this->scen.histparam[ip2].value);
	        else if (this->scen.condition[i].operateur==">=") OK=(this->scen.histparam[ip1].value >= this->scen.histparam[ip2].value);
	        else if (this->scen.condition[i].operateur=="<=") OK=(this->scen.histparam[ip1].value <= this->scen.histparam[ip2].value);
	        if (debuglevel==10) cout<<this->scen.condition[i].param1<<this->scen.condition[i].operateur<<this->scen.condition[i].param2<<"  "<<OK<<"\n";
            i++;
	    }
	    return OK;
	}

/**
*Struct ParticleC : vérifie l'ordre des événements quand il a y en a plusieurs à la même génération
*/

    void checkorder() {
        vector < vector <int> > exaequo;
        vector <int> bonordre;
        int ng=0,kscen,g,gg,kk;
        EventC piv;
        bool deja,trouve;
        for (int iev=0;iev<this->scen.nevent-1;iev++) {
            for (int jev=iev+1;jev<this->scen.nevent;jev++) {
                if (this->scen.event[iev].time==this->scen.event[jev].time) {
                    //cout<<"iev="<<iev<<"   jev="<<jev<<"\n";
                    if (ng==0) {
                        ng++;
                        exaequo.resize(ng);
                        exaequo[ng-1].clear();
                        exaequo[ng-1].push_back(iev);
                        exaequo[ng-1].push_back(jev);
                        //cout<<"debut ng="<<ng;
                        //for (int k=0;k<exaequo[ng-1].size();k++) cout<<"  "<<exaequo[ng-1][k];cout<<"\n";
                    } else {
                        deja=false;
                        for (g=0;g<ng;g++) {
                            for (int k=0;k<exaequo[g].size();k++) if (exaequo[g][k]==iev) {deja=true;gg=g;kk=k;break;}
                        }
                        //cout<<"deja="<<deja<<"   g="<<gg<<"   k="<<kk<<"\n";
                        if (deja) {
                          trouve=false;
                          //cout<<"exaequo[g].size()="<<exaequo[gg].size()<<"\n";
                          for (int k=0;k<exaequo[gg].size();k++)if (exaequo[gg][k]==jev){trouve=true;break;}
                          //cout<<"trouve="<<trouve<<"\n";
                          if (not trouve) exaequo[gg].push_back(jev);
                          //cout<<"deja ng="<<ng;
                          //for (int k=0;k<exaequo[gg].size();k++) cout<<"  "<<exaequo[gg][k];cout<<"\n";
                        }
                        else {
                            ng++;
                            exaequo.resize(ng);
                            exaequo[ng-1].clear();
                            exaequo[ng-1].push_back(iev);
                            exaequo[ng-1].push_back(jev);
                            //cout<<"nouveau groupe ng="<<ng;
                            //for (int k=0;k<exaequo[ng-1].size();k++) cout<<"  "<<exaequo[ng-1][k];cout<<"\n";
                        }
                    }
                }
            }
        }
        if (ng==0) return;
        for (kscen=0;kscen<this->nscenarios;kscen++) if (this->scenario[kscen].number==this->scen.number) break;
        //cout<<"scenario de référence = "<<kscen+1<<"\n";
        //cout<<"avant de réordonner\n";
        /*for (g=0;g<ng;g++) {
            for (int iev=0;iev<exaequo[g].size();iev++) this->scen.event[exaequo[g][iev]].ecris();
            cout<<"\n";
        }*/
        //cout<<"apres remise en ordre\n";
        for (g=0;g<ng;g++) {
            //cout<<"dans le groupe "<<g<<"\n";
            bonordre.clear();
            for (int iev=0;iev<exaequo[g].size();iev++) {
                for (int jev=0;jev<this->scen.nevent;jev++) {
                    if (this->scen.event[exaequo[g][iev]].action==this->scenario[kscen].event[jev].action) {
                          if (this->scen.event[exaequo[g][iev]].pop==this->scenario[kscen].event[jev].pop) {
                              if (this->scen.event[exaequo[g][iev]].action!='M') bonordre.push_back(jev);
                              else {if (this->scen.event[exaequo[g][iev]].pop1==this->scenario[kscen].event[jev].pop1) bonordre.push_back(jev);}
                          }
                    }
                }
            }
            //for (int iev=0;iev<exaequo[g].size();iev++) cout<<"iev="<<iev<<"   exaequo="<<exaequo[g][iev]<<"   bonordre="<<bonordre[iev]<<"\n";
            for (int iev=0;iev<exaequo[g].size()-1;iev++) {
                for (int jev=iev+1;jev<exaequo[g].size();jev++) {
                    if (bonordre[iev]>bonordre[jev]) {
                        //cout<<"echange entre "<<iev<<" et "<<jev<<"\n";
                        piv = copyevent(this->scen.event[exaequo[g][iev]]);
                        this->scen.event[exaequo[g][iev]] = copyevent(this->scen.event[exaequo[g][jev]]);
                        this->scen.event[exaequo[g][jev]] =copyevent(piv);
                        kk=bonordre[iev];bonordre[iev]=bonordre[jev];bonordre[jev]=kk;
                    }
                }
            }
            //for (int iev=0;iev<exaequo[g].size();iev++) this->scen.event[exaequo[g][iev]].ecris();cout<<"\n";
        }
        //cout<<"--------------------------------------------------\n";
        bonordre.clear();
    }


/**
* Struct ParticleC : génère les valeurs des paramètres historiques d'un scénario donné'
*/
/*	bool setHistParamValue(bool *usepriorhist) {
        //cout<<"dans sethistparamvalue nconditions="<<this->scen.nconditions<<"   drawuntil="<<drawuntil<<"\n";
		bool OK=true;
		if (this->scen.nconditions>0) {
			if (drawuntil) {
				//cout <<"drawuntil\n";
				OK=false;
				while (not OK) {
					for (int p=0;p<this->scen.nparam;p++) {
						if (usepriorhist[p]) this->scen.histparam[p].value = drawfromprior(this->scen.histparam[p].prior);
						if (this->scen.histparam[p].category<2) this->scen.histparam[p].value = floor(0.5+this->scen.histparam[p].value);
						//cout<<this->scen.histparam[p].name <<" = "<<this->scen.histparam[p].value;
						//cout<<"    "<<this->scen.histparam[p].prior.loi <<"  ["<<this->scen.histparam[p].prior.mini<<","<<this->scen.histparam[p].prior.maxi <<"]\n";
					}
					//cout <<"avant test conditions\n";
					OK = conditionsOK();
					//if (OK) cout <<"condition OK\n";
				}
			} else {
				for (int p=0;p<this->scen.nparam;p++) {
					if (usepriorhist[p]) this->scen.histparam[p].value = drawfromprior(this->scen.histparam[p].prior);
					if (this->scen.histparam[p].category<2) this->scen.histparam[p].value = floor(0.5+this->scen.histparam[p].value);
				}
				OK = conditionsOK();
			}
		}else {
			for (int p=0;p<this->scen.nparam;p++) {
				if (usepriorhist[p]) this->scen.histparam[p].value = drawfromprior(this->scen.histparam[p].prior);
				if (this->scen.histparam[p].category<2) this->scen.histparam[p].value = floor(0.5+this->scen.histparam[p].value);
				//cout << this->scen.histparam[p].name<<" = "<<this->scen.histparam[p].value<<"\n";
			}
			OK=true;
		}
		this->scen.ipv=0;
		if (OK) {
			for (int p=0;p<this->scen.nparam;p++) {
                //cout<<this->scen.histparam[p].name;
                //if (this->scen.histparam[p].prior.constant) cout<<"   constant\n"; else cout<<"   variable\n";
				if (not this->scen.histparam[p].prior.constant) {this->scen.paramvar[this->scen.ipv]=this->scen.histparam[p].value;this->scen.ipv++;}
			}
		}
		//cout<<"fin du tirage des parametres\n";
		if (debuglevel==10) {for (int p=0;p<this->scen.nparam;p++) cout<<this->scen.histparam[p].name<<" = " <<this->scen.histparam[p].value<<"\n";cout <<"\n";}
		if (OK) {
			for (int ievent=0;ievent<this->scen.nevent;ievent++) {
				if (this->scen.event[ievent].action=='V') { if (this->scen.event[ievent].Ne<0) this->scen.event[ievent].Ne= (int)(0.5+this->getvalue(this->scen.event[ievent].sNe));}
				if (this->scen.event[ievent].time==-9999) {this->scen.event[ievent].time = (int)(0.5+this->getvalue(this->scen.event[ievent].stime));}
				if (this->scen.event[ievent].action=='S') {if (this->scen.event[ievent].admixrate<0) this->scen.event[ievent].admixrate = this->getvalue(this->scen.event[ievent].sadmixrate);}
			}
//TRI SUR LES TEMPS DES EVENEMENTS
            sort(&this->scen.event[0],&this->scen.event[this->scen.nevent],compevent());
            checkorder();


			//cout<<"fin des events\n";
			for (int i=0;i<this->scen.nn0;i++) {
				if (this->scen.ne0[i].val<0) this->scen.ne0[i].val = (int)this->getvalue(this->scen.ne0[i].name);
                //cout<<this->scen.ne0[i].name<<" = "<<this->scen.ne0[i].val<<"\n";
			}
		}
		//cout<<OK<<"\n";
		return OK;
	}*/

/**
* Struct ParticleC : génère les valeurs des paramètres historiques d'un scénario donné'
*/
	bool setHistParamValue() {
        //cout<<"dans sethistparamvalue nconditions="<<this->scen.nconditions<<"   drawuntil="<<drawuntil<<"\n";
		bool OK=true;
		if (this->scen.nconditions>0) {
			if (drawuntil) {
				//cout <<"drawuntil\n";
				OK=false;
				while (not OK) {
					for (int p=0;p<this->scen.nparam;p++) {
					    if (not this->scen.histparam[p].prior.fixed) this->scen.histparam[p].value = drawfromprior(this->scen.histparam[p].prior);
						if (this->scen.histparam[p].category<2) this->scen.histparam[p].value = floor(0.5+this->scen.histparam[p].value);
						//cout<<this->scen.histparam[p].name <<" = "<<this->scen.histparam[p].value;
						//cout<<"    "<<this->scen.histparam[p].prior.loi <<"  ["<<this->scen.histparam[p].prior.mini<<","<<this->scen.histparam[p].prior.maxi <<"]\n";
					}
					//cout <<"avant test conditions\n";
					OK = conditionsOK();
					//if (OK) cout <<"condition OK\n";
				}
			} else {
				for (int p=0;p<this->scen.nparam;p++) {
					if (not this->scen.histparam[p].prior.fixed) this->scen.histparam[p].value = drawfromprior(this->scen.histparam[p].prior);
					if (this->scen.histparam[p].category<2) this->scen.histparam[p].value = floor(0.5+this->scen.histparam[p].value);
				}
				OK = conditionsOK();
			}
		}else {
			for (int p=0;p<this->scen.nparam;p++) {
				if (not this->scen.histparam[p].prior.fixed) this->scen.histparam[p].value = drawfromprior(this->scen.histparam[p].prior);
				if (this->scen.histparam[p].category<2) this->scen.histparam[p].value = floor(0.5+this->scen.histparam[p].value);
				//cout << this->scen.histparam[p].name<<" = "<<this->scen.histparam[p].value<<"\n";
			}
			OK=true;
		}
		this->scen.ipv=0;
		if (OK) {
			for (int p=0;p<this->scen.nparam;p++) {
                //cout<<this->scen.histparam[p].name;
                //if (this->scen.histparam[p].prior.constant) cout<<"   constant\n"; else cout<<"   variable\n";
				if (not this->scen.histparam[p].prior.constant) {
					this->scen.paramvar[this->scen.ipv]=this->scen.histparam[p].value;
					this->scen.ipv++;
				}
			}
		}
		//cout<<"fin du tirage des parametres\n";
		if (debuglevel==10) {for (int p=0;p<this->scen.nparam;p++) cout<<this->scen.histparam[p].name<<" = " <<this->scen.histparam[p].value<<"\n";cout <<"\n";}
		if (OK) {
			for (int ievent=0;ievent<this->scen.nevent;ievent++) {
				if (this->scen.event[ievent].action=='V') { if (this->scen.event[ievent].Ne<0) this->scen.event[ievent].Ne= (int)(0.5+this->getvalue(this->scen.event[ievent].sNe));}
				if (this->scen.event[ievent].time==-9999) {this->scen.event[ievent].time = (int)(0.5+this->getvalue(this->scen.event[ievent].stime));}
				if (this->scen.event[ievent].action=='S') {if (this->scen.event[ievent].admixrate<0) this->scen.event[ievent].admixrate = this->getvalue(this->scen.event[ievent].sadmixrate);}
			}
//TRI SUR LES TEMPS DES EVENEMENTS
            sort(&this->scen.event[0],&this->scen.event[this->scen.nevent],compevent());
            checkorder();


			//cout<<"fin des events\n";
			for (int i=0;i<this->scen.nn0;i++) {
				if (this->scen.ne0[i].val<0) this->scen.ne0[i].val = (int)this->getvalue(this->scen.ne0[i].name);
                //cout<<this->scen.ne0[i].name<<" = "<<this->scen.ne0[i].val<<"\n";
			}
		}
		//cout<<OK<<"\n";
		return OK;
	}

/**
* Struct ParticleC : génère les valeurs des paramètres mutationnels moyens des différents groupes de locus
*/
	void setMutParammoyValue(){
		int gr;
		//cout<<"this->scen.ipv="<<this->scen.ipv<<"\n";
		for (gr=1;gr<=this->ngr;gr++) {
		    //cout<<"groupe "<<gr<<"   type="<<this->grouplist[gr].type<<"   usepriormut="<<usepriormut<<"\n";
		    if (this->grouplist[gr].type==0) {  //microsat
		        if (not this->grouplist[gr].priormutmoy.fixed) this->grouplist[gr].mutmoy = this->drawfromprior(this->grouplist[gr].priormutmoy);
				if (not this->grouplist[gr].priormutmoy.constant) {
					this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].mutmoy;
					this->scen.ipv++;
					cout<<"mutmoy ipv++\n";
				}
					cout<<"mutmoy="<<this->grouplist[gr].mutmoy<<"\n";
				if (not this->grouplist[gr].priorPmoy.fixed) this->grouplist[gr].Pmoy = this->drawfromprior(this->grouplist[gr].priorPmoy);
				if (not this->grouplist[gr].priorPmoy.constant) {
					this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].Pmoy;
					this->scen.ipv++;
					cout<<"Pmoy ipv++\n";
				}
					cout<<"Pmoy="<<this->grouplist[gr].Pmoy<<"\n";
				if (not this->grouplist[gr].priorsnimoy.fixed) this->grouplist[gr].snimoy = this->drawfromprior(this->grouplist[gr].priorsnimoy);
				if (not this->grouplist[gr].priorsnimoy.constant) {
					this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].snimoy;
					this->scen.ipv++;
					cout<<"snimoy ipv++\n";
				}
		    }
		    if (this->grouplist[gr].type==1) {  //sequence
				if (not this->grouplist[gr].priormusmoy.fixed) this->grouplist[gr].musmoy = this->drawfromprior(this->grouplist[gr].priormusmoy);
				if (not this->grouplist[gr].priormusmoy.constant) {		
					this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].musmoy;
					cout<<"musmoy ipv++\n";
					this->scen.ipv++;
				}
				if (this->grouplist [gr].mutmod>0){
					if (not this->grouplist[gr].priork1moy.fixed) this->grouplist[gr].k1moy = this->drawfromprior(this->grouplist[gr].priork1moy);
					if (not this->grouplist[gr].priork1moy.constant) {
						this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].k1moy;
						cout<<"k1moy ipv++\n";
						this->scen.ipv++;
					}
				}
				if (this->grouplist [gr].mutmod>2){
					if (not this->grouplist[gr].priork2moy.fixed) this->grouplist[gr].k2moy = this->drawfromprior(this->grouplist[gr].priork2moy);
					if (not this->grouplist[gr].priork2moy.constant) {
						this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].k2moy;
						this->scen.ipv++;
						cout<<"k2moy ipv++\n";
					}
				}
		    }

		}
		this->scen.nparamvar=this->scen.ipv;
		//cout<<"Dans particule C  nparamvar = "<<this->scen.nparamvar<<"\n";
	}

/**
* Struct ParticleC : génère les valeurs des paramètres mutationnels moyens des différents groupes de locus
*/
/*	void setMutParammoyValue(bool **usepriormut){
		int gr;
		//cout<<"this->scen.ipv="<<this->scen.ipv<<"\n";
		for (gr=1;gr<=this->ngr;gr++) {
		    //cout<<"groupe "<<gr<<"   type="<<this->grouplist[gr].type<<"   usepriormut="<<usepriormut<<"\n";
		    if (this->grouplist[gr].type==0) {  //microsat
		        if (not this->grouplist[gr].priormutmoy.constant) {
				    cout<<"usepriormut["<<gr<<"][0]="<<usepriormut[gr][0]<<"\n";
                    if ((this->grouplist[gr].mutmoy<0)and(usepriormut[gr][0])){ 
						this->grouplist[gr].mutmoy = this->drawfromprior(this->grouplist[gr].priormutmoy);
						this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].mutmoy;
						this->scen.ipv++;
					}
		        }
		        cout<<"juste avant mutmoy\n";
				cout<<"mutmoy="<<this->grouplist[gr].mutmoy;
				if (this->scen.ipv>0) cout<<"    "<<this->scen.paramvar[this->scen.ipv-1]<<"\n";else cout<<"\n";
					
				if (not this->grouplist[gr].priorPmoy.constant) {
					cout<<"usepriormut["<<gr<<"][1]="<<usepriormut[gr][1]<<"\n";
					if ((this->grouplist[gr].Pmoy<0)and(usepriormut[gr][1])) {
						this->grouplist[gr].Pmoy = this->drawfromprior(this->grouplist[gr].priorPmoy);
						this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].Pmoy;
						this->scen.ipv++;
					}
				}
				cout<<"Pmoy="<<this->grouplist[gr].Pmoy;
				if (this->scen.ipv>0) cout<<"    "<<this->scen.paramvar[this->scen.ipv-1]<<"\n";else cout<<"\n";
				
				if (not this->grouplist[gr].priorsnimoy.constant) {
					cout<<"usepriormut["<<gr<<"][é]="<<usepriormut[gr][2]<<"\n";
					if ((this->grouplist[gr].snimoy<0)and(usepriormut[gr][2])) {
						this->grouplist[gr].snimoy = this->drawfromprior(this->grouplist[gr].priorsnimoy);
						this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].snimoy;
						this->scen.ipv++;
					}
				}
				cout<<"snimoy="<<this->grouplist[gr].snimoy;
				if (this->scen.ipv>0) cout<<"    "<<this->scen.paramvar[this->scen.ipv-1]<<"\n";else cout<<"\n";
		    }
		    if (this->grouplist[gr].type==1) {  //sequence
				if (not this->grouplist[gr].priormusmoy.constant) {
					if ((this->grouplist[gr].musmoy<0)and(usepriormut[gr][0])) {
						this->grouplist[gr].musmoy = this->drawfromprior(this->grouplist[gr].priormusmoy);
						this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].musmoy;
						this->scen.ipv++;
					}
				}
				//cout<<"musmoy="<<this->grouplist[gr].musmoy;
				//if (this->scen.ipv>0) cout<<"    "<<this->scen.paramvar[this->scen.ipv-1]<<"\n";else cout<<"\n";
				if (this->grouplist [gr].mutmod>0){
					if (not this->grouplist[gr].priork1moy.constant) {
						if ((this->grouplist[gr].k1moy<0)and(usepriormut[gr][1])) {
							this->grouplist[gr].k1moy = this->drawfromprior(this->grouplist[gr].priork1moy);
							this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].k1moy;
							this->scen.ipv++;
						  }
					  }
					//cout<<"k1moy="<<this->grouplist[gr].k1moy;
					//if (this->scen.ipv>0) cout<<"    "<<this->scen.paramvar[this->scen.ipv-1]<<"\n";else cout<<"\n";
				}
				if (this->grouplist [gr].mutmod>2){
					if (not this->grouplist[gr].priork2moy.constant) {
						if ((this->grouplist[gr].k2moy<0)and(usepriormut[gr][2])) {
							this->grouplist[gr].k2moy = this->drawfromprior(this->grouplist[gr].priork2moy);
							this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].k2moy;
							this->scen.ipv++;
						}
					}
				}
		    }

		}
		this->scen.nparamvar=this->scen.ipv;
		cout<<"Dans particule C  nparamvar = "<<this->scen.nparamvar<<"\n";
	}*/

/**
* Struct ParticleC : génère les valeurs des paramètres mutationnels du locus loc
*/
	void setMutParamValue(int loc){
		int gr = this->locuslist[loc].groupe;
		cout <<"\n SetMutParamValue pour le locus "<<loc<< " (groupe "<< gr <<")\n";
		if (this->locuslist[loc].type<5) {  //MICROSAT
			this->grouplist[gr].priormutloc.mean = this->grouplist[gr].mutmoy;
			if ((this->grouplist[gr].priormutloc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].mut_rate = this->drawfromprior(this->grouplist[gr].priormutloc);
			else this->locuslist[loc].mut_rate =this->grouplist[gr].mutmoy;
			cout << "mutloc["<<loc<<"]="<<this->locuslist[loc].mut_rate <<"\n";

			this->grouplist[gr].priorPloc.mean = this->grouplist[gr].Pmoy;
			if ((this->grouplist[gr].priorPloc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].Pgeom = this->drawfromprior(this->grouplist[gr].priorPloc);
			else this->locuslist[loc].Pgeom =this->grouplist[gr].Pmoy;
			cout << "Ploc["<<loc<<"]="<<this->locuslist[loc].Pgeom <<"\n";

			this->grouplist[gr].priorsniloc.mean = this->grouplist[gr].snimoy;
			//cout <<"coucou\n";fflush(stdin);
			if ((this->grouplist[gr].priorsniloc.sdshape>0.001 )and(this->grouplist[gr].nloc>1)) this->locuslist[loc].sni_rate = this->drawfromprior(this->grouplist[gr].priorsniloc);
			else this->locuslist[loc].sni_rate =this->grouplist[gr].snimoy;
			cout << "sniloc["<<loc<<"]="<<this->locuslist[loc].sni_rate <<"\n";


		 }
		else		                    //DNA SEQUENCE
		{
			//cout << "musmoy = "<<this->grouplist[gr].musmoy <<" (avant)";
			if (this->grouplist[gr].musmoy<0) this->grouplist[gr].musmoy = this->drawfromprior(this->grouplist[gr].priormusmoy);
			this->grouplist[gr].priormusloc.mean = this->grouplist[gr].musmoy;
			//cout << "   et "<<this->grouplist[gr].musmoy <<" (apres)\n";

			if ((this->grouplist[gr].priormusloc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].mus_rate = this->drawfromprior(this->grouplist[gr].priormusloc);
			else this->locuslist[loc].mus_rate =this->grouplist[gr].musmoy;
			//cout << "   et "<<this->locuslist[loc].mut_rate <<" (apres)\n";

			if (this->grouplist [gr].mutmod>0) {
			//cout << "Pmoy = "<<this->grouplist[gr].Pmoy <<" (avant)";
			if (this->grouplist[gr].k1moy<0) this->grouplist[gr].k1moy = this->drawfromprior(this->grouplist[gr].priork1moy);
			this->grouplist[gr].priork1loc.mean = this->grouplist[gr].k1moy;
			//cout << "   et "<<this->grouplist[gr].Pmoy <<" (apres)\n";

			if ((this->grouplist[gr].priork1loc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].k1 = this->drawfromprior(this->grouplist[gr].priork1loc);
			else this->locuslist[loc].k1 =this->grouplist[gr].k1moy;
			//cout << "   et "<<this->locuslist[loc].Pgeom <<" (apres)\n";
			}

			if (this->grouplist [gr].mutmod>2) {
			if (this->grouplist[gr].k2moy<0) this->grouplist[gr].k2moy = this->drawfromprior(this->grouplist[gr].priork2moy);
			this->grouplist[gr].priork2loc.mean = this->grouplist[gr].k2moy;

			if (this->grouplist[gr].priork2loc.sdshape>0.001 ) this->locuslist[loc].k2 = this->drawfromprior(this->grouplist[gr].priork2loc);
			else this->locuslist[loc].k2 =this->grouplist[gr].k2moy;
			//cout <<"mutmoy = "<< this->grouplist[gr].mutmoy << "  Pmoy = "<<this->grouplist[gr].Pmoy <<"  snimoy="<<this->grouplist[gr].snimoy<<"\n";
			//cout <<"locus "<<loc<<"  groupe "<<gr<< "  mut_rate="<<this->locuslist[loc].mut_rate<<"  Pgeom="<<this->locuslist[loc].Pgeom<<"  sni_rate="<<this->locuslist[loc].sni_rate << "\n";
			}
		}
	}

//////////////////////////////////////////
//  Création de la séquence d'(événements)
/////////////////////////////////////////

/**
* Struct ParticleC : retourne vrai si et seulement si l'événement ievent est basal (??)
*/
	bool firstEvent(int ievent) {
		if (ievent == 0) {return 1;} //1 pour TRUE
		bool found;
		int jevent=ievent;
		while (jevent>0) {
			jevent --;
			if (this->scen.event[jevent].action == 'S') {
				found = (this->scen.event[jevent].pop1 == (this->scen.event[ievent].pop)or(this->scen.event[jevent].pop2 ==this->scen.event[ievent].pop));}
			else {found = this->scen.event[jevent].pop == this->scen.event[ievent].pop;}
			if (found) {break;}
			}
		return (not found);
	}

/**
* Struct ParticleC : retourne la valeur du Ne pour l'événement ievent et la pop refpop
*/
	int findNe(int ievent, int refpop) {
		if (ievent == 0) {return this->scen.ne0[refpop-1].val;}
		bool found;
		while (ievent>0) {
			ievent --;
			found = (this->scen.event[ievent].action == 'V')and(this->scen.event[ievent].pop == refpop);
			if (found) {break;}
			}
		if (found) {return this->scen.event[ievent].Ne;}
		else       {return this->scen.ne0[refpop-1].val;}
	}

/**
* Struct ParticleC : retourne la valeur du time pour l'événement ievent et la pop refpop
*/
	int findT0(int ievent, int refpop) {
		int jevent = ievent;
		bool found = 0; //FALSE
		while (jevent>0) {
			jevent --;
			if (this->scen.event[jevent].action == 'S') {
				found = (this->scen.event[jevent].pop1 == refpop)or(this->scen.event[jevent].pop2 == refpop);}
			else {found = (this->scen.event[jevent].pop == refpop);}
			if (found) {break;}
			}
		if (found) {return this->scen.event[jevent].time;}
		else       {return this->scen.event[ievent].time;}
	}

/**
* Struct ParticleC : établit les paramètres d'un segment de l'arbre de coalescence
*/
	void seqCoal(int iseq, int ievent, int refpop) {
		this->seqlist[iseq].action = 'C';
		this->seqlist[iseq].N = findNe(ievent,refpop);
		this->seqlist[iseq].pop = refpop;
		this->seqlist[iseq].t0 = findT0(ievent,refpop);
		this->seqlist[iseq].t1 = this->scen.event[ievent].time;
	}

/**
* Struct ParticleC : établit les paramètres d'un événement "ajout d'un échantllon" à l'arbre de coalescence
*/
	void seqSamp(int iseq, int ievent) {
		this->seqlist[iseq].action = 'A';
		this->seqlist[iseq].pop = this->scen.event[ievent].pop;
		this->seqlist[iseq].t0 = this->scen.event[ievent].time;
		this->seqlist[iseq].sample = this->scen.event[ievent].sample;
		//cout << " seqlist["<<iseq<<"].sample = "<<this->seqlist[iseq].sample<<"\n";
	}

/**
* Struct ParticleC : établit les paramètres d'un événement "merge" à l'arbre de coalescence
*/
	void seqMerge(int iseq, int ievent) {
		this->seqlist[iseq].action = 'M';
		this->seqlist[iseq].pop = this->scen.event[ievent].pop;
		this->seqlist[iseq].pop1 = this->scen.event[ievent].pop1;
		this->seqlist[iseq].t0 = this->scen.event[ievent].time;
	}

/**
* Struct ParticleC : établit les paramètres d'un événement "split" à l'arbre de coalescence
*/
	void seqSplit(int iseq, int ievent) {
		this->seqlist[iseq].action = 'S';
		this->seqlist[iseq].pop = this->scen.event[ievent].pop;
		this->seqlist[iseq].pop1 = this->scen.event[ievent].pop1;
		this->seqlist[iseq].pop2 = this->scen.event[ievent].pop2;
		this->seqlist[iseq].t0 = this->scen.event[ievent].time;
		this->seqlist[iseq].admixrate = this->scen.event[ievent].admixrate;
	}

/**
* Struct ParticleC : compte le nombre nécessaires d'éléments de la séquence définissant l'arbre de coalescence
*/
	int compteseq() {
    	int n = 0;
    	for (int k=0;k<this->scen.nevent;k++){
    		if (this->scen.event[k].action == 'E') {n +=2;}  //SAMPLE
    		else if (this->scen.event[k].action == 'V') {n +=1;} //VARNE
    		else if (this->scen.event[k].action == 'M') {n +=3;} //MERGE
    		else if (this->scen.event[k].action == 'S') {n +=4;} //SPLIT
    	}
    	return n;
    }

/**
* Struct ParticleC : établit la séquence d'événements populationnels définissant l'arbre de coalescence
*/
	void setSequence() {
		int kseq = compteseq();
		//cout<<"kseq="<<kseq<<"\n";
		this->seqlist = new SequenceBitC[kseq];
		int iseq = 0;
		for (int ievent=0;ievent<this->scen.nevent;ievent++){
		     //cout<<"event["<<ievent<<"]="<<this->scen.event[ievent].action<<"\n";
			if (firstEvent(ievent) == 0) {  // If NOT FirstEvent
				if (this->scen.event[ievent].action == 'V') {   // if action = VARNE
					seqCoal(iseq,ievent,this->scen.event[ievent].pop);iseq++;
				}
				else if (this->scen.event[ievent].action == 'E') {   // if action = SAMPLE
					seqCoal(iseq,ievent,this->scen.event[ievent].pop);iseq++;
					seqSamp(iseq,ievent);iseq++;
					//cout<<this->seqlist[iseq-1].action<<"\n";
				}
			}
			else if (this->scen.event[ievent].action == 'E') {   // if action = SAMPLE
				seqSamp(iseq,ievent);iseq++;
				//cout<<this->seqlist[iseq-1].action<<"\n";
			}
			if (this->scen.event[ievent].action == 'M') {   // if action = MERGE
				seqCoal(iseq,ievent,this->scen.event[ievent].pop);
				if (this->seqlist[iseq].t0 <  this->seqlist[iseq].t1) {iseq++;}
				seqCoal(iseq,ievent,this->scen.event[ievent].pop1);
				if (this->seqlist[iseq].t0 <  this->seqlist[iseq].t1) {iseq++;}
				seqMerge(iseq,ievent);iseq++;
				//cout<<this->seqlist[iseq-1].action<<"\n";
			}
			if (firstEvent(ievent) == 0) {  // If NOT FirstEvent
				if(this->scen.event[ievent].action == 'S') {   // if action = SPLIT
					//cout<<"avant seqcoal\n";fflush(stdin);
					seqCoal(iseq,ievent,this->scen.event[ievent].pop);
					//cout<<"apres seqcoal\n";fflush(stdin);
					if (this->seqlist[iseq].t0 <  this->seqlist[iseq].t1) {iseq++;}
					//cout<<"coalpop\n";
					seqCoal(iseq,ievent,this->scen.event[ievent].pop1);
					if (this->seqlist[iseq].t0 <  this->seqlist[iseq].t1) {iseq++;}
					//cout<<"coalpop1\n";
					seqCoal(iseq,ievent,this->scen.event[ievent].pop2);
					if (this->seqlist[iseq].t0 <  this->seqlist[iseq].t1) {iseq++;}
					//cout<<"coalpop2\n";
					seqSplit(iseq,ievent);iseq++;
					//cout<<this->seqlist[iseq-1].action<<"\n";
				}
			}
		}
		if ((this->scen.event[this->scen.nevent-1].action == 'M')or(this->scen.event[this->scen.nevent-1].action == 'E')) {
			this->seqlist[iseq].N = findNe(this->scen.nevent-1,this->scen.event[this->scen.nevent-1].pop);
            //cout<<"coucou\n  this->seqlist[iseq].N ="<<this->seqlist[iseq].N<<"  event="<<this->scen.nevent-1<<"   pop="<<this->scen.event[this->scen.nevent-1].pop <<"\n";
		}
		else if (this->scen.event[this->scen.nevent-1].action == 'V') {
			this->seqlist[iseq].N = this->scen.event[this->scen.nevent-1].Ne;
		}
		this->seqlist[iseq].pop = this->scen.event[this->scen.nevent-1].pop;
		this->seqlist[iseq].t0 = this->scen.event[this->scen.nevent-1].time;
		this->seqlist[iseq].t1 = -1;
		this->seqlist[iseq].action = 'C';
		this->nseq=iseq+1;
		/*std::cout << "nombre de SequenceBits alloués = " << kseq << "\n";
		std::cout << "nombre de SequenceBits utilisés = " << nseq << "\n";
		for (int i=0;i<this->nseq;i++){
			if (this->seqlist[i].action == 'C') {
				std::cout << seqlist[i].t0 << " à " << seqlist[i].t1 << "   coal  pop=" << seqlist[i].pop;
				std::cout << "  N=" << seqlist[i].N << "\n";
			}
			else if (this->seqlist[i].action == 'M') {
				std::cout << seqlist[i].t0 << "   Merge  " << "pop " << seqlist[i].pop << " et " << seqlist[i].pop1 << "\n";
			}
			else if (this->seqlist[i].action == 'S') {
				std::cout << seqlist[i].t0 << "   Split  " << "pop " << seqlist[i].pop << " vers " << seqlist[i].pop1 << " et " <<seqlist[i].pop2 << "\n";
			}
			else if (this->seqlist[i].action == 'A') {
				std::cout << seqlist[i].t0 << "   Adsamp  " << "à la pop " << seqlist[i].pop << "\n";
			}

		}*/
	}

/**
* Struct ParticleC : calcule les éléments de la matrice de transition Q qui fournit
*                    la probabilité de mutation d'un nucléotide à un autre en fonction
*                    du modèle mutationnel choisi
*/
    void comp_matQ(int loc) {
	    double s;
		int gr=this->locuslist[loc].groupe;
		this->matQ[0][0] = this->matQ[1][1] = this->matQ[2][2] = this->matQ[3][3] = 0.0;
		this->matQ[0][1] = this->matQ[0][2] = this->matQ[0][3] = 1.0;
		this->matQ[1][0] = this->matQ[1][2] = this->matQ[1][3] = 1.0;
		this->matQ[2][0] = this->matQ[2][1] = this->matQ[2][3] = 1.0;
		this->matQ[3][0] = this->matQ[3][1] = this->matQ[3][2] = 1.0;
		if (this->grouplist[gr].mutmod == 1) {
			this->matQ[0][2] = this->matQ[1][3] = this->matQ[2][0] = this->matQ[3][1] = this->locuslist[loc].k1;
		}
		else if (this->grouplist[gr].mutmod == 2) {
			this->matQ[0][1] = this->locuslist[loc].pi_C;
			this->matQ[0][2] = this->locuslist[loc].k1*this->locuslist[loc].pi_G;
			this->matQ[0][3] = this->locuslist[loc].pi_T;
			this->matQ[1][0] = this->locuslist[loc].pi_A;
			this->matQ[1][2] = this->locuslist[loc].pi_G;
			this->matQ[1][3] = this->locuslist[loc].k1*this->locuslist[loc].pi_T;
			this->matQ[2][0] = this->locuslist[loc].k1*this->locuslist[loc].pi_A;
			this->matQ[2][1] = this->locuslist[loc].pi_C;
			this->matQ[2][3] = this->locuslist[loc].pi_T;
			this->matQ[3][0] = this->locuslist[loc].pi_A;
			this->matQ[3][1] = this->locuslist[loc].k1*this->locuslist[loc].pi_C;
			this->matQ[3][2] = this->locuslist[loc].pi_G;
		}
		else if (this->grouplist[gr].mutmod == 3) {
			this->matQ[0][1] = this->locuslist[loc].pi_C;
			this->matQ[0][2] = this->locuslist[loc].k1*this->locuslist[loc].pi_G;
			this->matQ[0][3] = this->locuslist[loc].pi_T;
			this->matQ[1][0] = this->locuslist[loc].pi_A;
			this->matQ[1][2] = this->locuslist[loc].pi_G;
			this->matQ[1][3] = this->locuslist[loc].k2*this->locuslist[loc].pi_T;
			this->matQ[2][0] = this->locuslist[loc].k1*this->locuslist[loc].pi_A;
			this->matQ[2][1] = this->locuslist[loc].pi_C;
			this->matQ[2][3] = this->locuslist[loc].pi_T;
			this->matQ[3][0] = this->locuslist[loc].pi_A;
			this->matQ[3][1] = this->locuslist[loc].k2*this->locuslist[loc].pi_C;
			this->matQ[3][2] = this->locuslist[loc].pi_G;
		}
		
		for (int i=0;i<4;i++) {  //normalisation par ligne  somme=1
			s=0.0;for (int j=0;j<4;j++) {s += this->matQ[i][j];}
			for (int j=0;j<4;j++) {this->matQ[i][j] /= s;}
		}
	}


/**
* Struct ParticleC : initialise l'arbre de coalescence, i.e. alloue la mémoire pour les noeuds et les branches,
*                    initialise les propriétés "sample" et "height" des noeuds terminaux
*                    initialise à 0 la propriété "pop" de tous les noeuds et à 0 la propriété "sample" des noeuds non-terminaux
*/
	GeneTreeC init_tree(int loc) {
		//cout << "début de init_tree pour le locus " << loc  <<"\n";
		GeneTreeC gt;
		int nnod=0,nn,n=0;
		//cout << "nsample = " << this->data.nsample << "   samplesize[0] = " << this->locuslist[loc].samplesize[0] << "\n";
		for (int sa=0;sa<this->data.nsample;sa++) {nnod +=this->locuslist[loc].ss[sa];}
		//cout << "nombre initial de noeuds = " << nnod  <<"\n";exit(6);
		gt.ngenes = nnod;
		gt.nnodes=nnod;
		gt.nbranches = 0;
		gt.nodes = new NodeC[2*nnod+1];
		gt.branches = new BranchC[2*nnod];
		for (int sa=0;sa<this->data.nsample;sa++) {
		    //cout<<"    this->data.nind["<<sa<<"]="<<this->data.nind[sa]<<"\n";
			for (int ind=0;ind<this->data.nind[sa];ind++) {
				nn=calploidy(loc,sa,ind);
				//cout << "n=" << n << "     nn=" << nn << "\n";
				if (nn>0) {
					for (int i=n;i<n+nn;i++){
						gt.nodes[i].sample=sa+1;
						//cout << gt.nodes[i].sample  << "\n";
						gt.nodes[i].height=this->scen.time_sample[sa];
						//cout << gt.nodes[i].height  << "\n";
                        gt.nodes[i].pop=0;
					}
				n +=nn;
				}
			}
		}
		//cout<<"avant la deuxième boucle   départ="<<gt.nnodes<<"  arrivée="<<2*nnod<< "\n";
		for (int i=gt.nnodes;i<2*nnod+1;i++) {gt.nodes[i].sample=0;gt.nodes[i].pop=0;}
		//cout<<"avant la troisième boucle  départ=0  arrivée="<<gt.nnodes-1<<"\n";
		/*for (int i=0;i<gt.nnodes;i++){
			cout << "node " << i << "   sample = " << gt.nodes[i].sample << "\n";
		}*/
		return gt;

	}

/**
* Struct ParticleC : évalue la pertinence de l'approximation continue pour le traitement de la coalescence
*/
	int evalcriterium(int iseq,int nLineages) {
		if(this->seqlist[iseq].t1<0) {return 1;}
		int nGen;
		double ra;
		int OK=0;
		nGen=this->seqlist[iseq].t1-this->seqlist[iseq].t0;
		ra= (float) nLineages/ (float)this->seqlist[iseq].N;
		if (nGen<=30) {
			if (ra < (0.0031*nGen*nGen - 0.053*nGen + 0.7197)) {OK=1;}
		}
		else if (nGen<=100){
			if (ra < (0.033*nGen + 1.7)) {OK=1;}
		}
		else if (ra<0.5) {OK=1;}
		return OK;
	}

/**
* Struct ParticleC : tire au hasard (uniformément) une lignée ancestrale parmi celles de la population requise
*/
	int draw_node(int loc,int iseq,int nLineages) {
		int k = this->mw.rand0(nLineages);
		//cout << "draw_node nLineages = " << nLineages << "   k=" << k << "\n";
		int inode=0,i0=-1;
		while (inode<this->gt[loc].nnodes){
			//cout << "inode=" << inode << "\n";
			if (this->gt[loc].nodes[inode].pop==this->seqlist[iseq].pop) {i0++;}
			//cout << "i0=" << i0 << "\n";
			if (i0==k) {
				this->gt[loc].nodes[inode].pop=0;
				//cout << "numéro de noeud tiré = " << inode <<"\n";
				return inode;
			}
			inode++;
		}
		return -1;
	}

/**
* Struct ParticleC : coalesce les lignées ancestrales de la population requise
*/
	void coal_pop(int loc,int iseq) {
		/*cout <<"\n";
		cout << "debut COAL pop="<<this->seqlist[iseq].pop<<"  nbranches=" << this->gt[loc].nbranches <<"   nnodes="<<this->gt[loc].nnodes ;
		cout<<"   Ne="<<this->seqlist[iseq].N<<"   t0="<<this->seqlist[iseq].t0<<"   t1="<<this->seqlist[iseq].t1;
		cout<<"    coeff="<<this->locuslist[loc].coeff <<"\n";*/
		int nLineages=0;
		bool final=false;
        double lra;
		for (int i=0;i<this->gt[loc].nnodes;i++){
			if (this->gt[loc].nodes[i].pop == this->seqlist[iseq].pop) {nLineages +=1;}
		}
		//cout<<"debut coal_pop nbranches="<< this->gt[loc].nbranches << "  nLineages=" << nLineages << "\n";
		if (this->seqlist[iseq].t1<0) {final=true;}
		if (this->seqlist[iseq].N<1) {std::cout << "coal_pop : population size <1 ("<<this->seqlist[iseq].N<<") \n" ;exit(100);}
        //if (trace) cout<<"pop size>=1\n";
		if (evalcriterium(iseq,nLineages) == 1) {		//CONTINUOUS APPROXIMATION
			//if (trace) cout << "Approximation continue final="<< final << "   nLineages=" << nLineages << "  pop=" <<this->seqlist[iseq].pop << "\n";
			double start = this->seqlist[iseq].t0;
			//if (trace) cout << "start initial= " << start << "\n";
			while ((nLineages>1)and((final)or((not final)and(start<this->seqlist[iseq].t1)))) {
				double ra = this->mw.random();
				while (ra == 0.0) {ra = this->mw.random();}
				lra = log(ra);
				start -= (this->locuslist[loc].coeff*this->seqlist[iseq].N/nLineages/(nLineages-1.0))*lra;
				//if (trace)  cout << "coeff = " << this->locuslist[loc].coeff << "   N = " << this->seqlist[iseq].N << "nl*(nl-1) = " << nLineages/(nLineages-1.0) << "\n";
				//if (trace) cout << "start courant= " << start << "  log(ra)=" << lra << "\n";
				if ((final)or((not final)and(start<this->seqlist[iseq].t1))) {
					this->gt[loc].nodes[this->gt[loc].nnodes].pop=this->seqlist[iseq].pop;
					this->gt[loc].nodes[this->gt[loc].nnodes].height=start;
					this->gt[loc].nnodes++;//if (trace) cout <<"nouveau noeud = "<<this->gt[loc].nnodes-1<<"    nLineages = "<<nLineages<<"\n";
					this->gt[loc].branches[this->gt[loc].nbranches].top=this->gt[loc].nnodes-1;
					this->gt[loc].branches[this->gt[loc].nbranches].bottom=draw_node(loc,iseq,nLineages);
					//if (trace) cout << "retour premier noeud tiré : " << this->gt[loc].branches[this->gt[loc].nbranches].bottom <<"\n";
					nLineages--;
					this->gt[loc].branches[this->gt[loc].nbranches].length=this->gt[loc].nodes[this->gt[loc].branches[this->gt[loc].nbranches].top].height-this->gt[loc].nodes[this->gt[loc].branches[this->gt[loc].nbranches].bottom].height;
					this->gt[loc].nbranches++;
					this->gt[loc].branches[this->gt[loc].nbranches].top=this->gt[loc].nnodes-1;
					this->gt[loc].branches[this->gt[loc].nbranches].bottom=draw_node(loc,iseq,nLineages);
					//if (trace) cout << "retour second noeud tiré : " << this->gt[loc].branches[this->gt[loc].nbranches].bottom <<"\n";
					this->gt[loc].branches[this->gt[loc].nbranches].length=this->gt[loc].nodes[this->gt[loc].branches[this->gt[loc].nbranches].top].height-this->gt[loc].nodes[this->gt[loc].branches[this->gt[loc].nbranches].bottom].height;
                    this->gt[loc].nbranches++;
					//if (trace) cout << "nbranches = " << this->gt[loc].nbranches << "\n";
				}
			}
		}
		else {											//GENERATION PER GENERATION
			//cout << "Génération par génération pour le locus "<<loc<<"\n";
			//int *numtire,*num,*knum;
			int gstart= (int)(this->seqlist[iseq].t0+0.5);
			int Ne= (int) (0.5*this->locuslist[loc].coeff*this->seqlist[iseq].N+0.5);
			int nnum;
			//numtire = new int[this->gt[loc].nnodes];
			//num     = new int[this->gt[loc].nnodes];
			//knum    = new int[this->gt[loc].nnodes];
			vector <int> numtire,num,knum;
			numtire.resize(this->gt[loc].nnodes+nLineages);num.resize(this->gt[loc].nnodes+nLineages);knum.resize(this->gt[loc].nnodes+nLineages);
			while ((nLineages>1)and((final)or((not final)and(gstart<this->seqlist[iseq].t1)))) {
				gstart++;
				if ((final)or((not final)and(gstart<this->seqlist[iseq].t1))) {
					nnum=0;
					for (int i=0;i<this->gt[loc].nnodes;i++){
						if (this->gt[loc].nodes[i].pop == this->seqlist[iseq].pop) {
							num[i] = this->mw.rand1(Ne);
							if (nnum==0) {
								numtire[nnum]=num[i];
								nnum++;
							}
							else {
								int nn=-1;
								bool trouve=false;
								do {nn++;trouve=(num[i]==numtire[nn]);}
								while ((not trouve)and(nn<nnum));
								if (not trouve) {numtire[nnum]=num[i];nnum++;}
							}
						}
						else num[i] =0;
						//if (num[i]!=0) cout << i<<"("<<num[i]<<")  ";
					}
					//cout <<"\n nnum="<<nnum<<"\n";
					for (int i=0;i<nnum;i++) {
						int nn=0;
						for (int j=0;j<this->gt[loc].nnodes;j++){
							if (num[j]==numtire[i]) {knum[nn]=j;nn++;}
						}
						//if (nnum==1){cout << "nn="<<nn<<"   ";for (int k=0;k<nn;k++){cout << knum[k]<<"  ";}}
						if (nn>1) {
							//cout << nn <<" noeuds ont tiré " << numtire[i]<<"   :";
							//for (int k=0;k<nn;k++){cout << knum[k]<<"  ";}
							//cout <<"nombre de lineages = "<<nLineages<<"\n";
							this->gt[loc].nodes[this->gt[loc].nnodes].pop=this->seqlist[iseq].pop;
							this->gt[loc].nodes[this->gt[loc].nnodes].height=(double)gstart;
							this->gt[loc].nnodes++;
							nLineages++;
							for (int k=0;k<nn;k++){
								this->gt[loc].branches[this->gt[loc].nbranches].top=this->gt[loc].nnodes-1;
								this->gt[loc].branches[this->gt[loc].nbranches].bottom=knum[k];
								this->gt[loc].branches[this->gt[loc].nbranches].length=this->gt[loc].nodes[this->gt[loc].nnodes-1].height - this->gt[loc].nodes[knum[k]].height;
								this->gt[loc].nbranches++;
								this->gt[loc].nodes[knum[k]].pop=0;
								nLineages--;
							}
						}
						//if (nnum==1) cout <<"nombre de lineages = "<<nLineages<<"\n";
					}
				}
			}
			//cout << "avant les delete \n";
			//delete []num;
			//delete []knum;
			//delete[] numtire;
			//cout << "fin des delete\n";
			//cout << "fin coal_pop nbranches=" << this->gt[loc].nbranches << "  nLineages=" << nLineages << "\n";
		}
		//cout << "fin coal_pop nbranches=" << this->gt[loc].nbranches << "  nLineages=" << nLineages << "\n";flush(cout);
	}

	void pool_pop(int loc,int iseq) {
		for (int i=0;i<this->gt[loc].nnodes;i++){
			if (this->gt[loc].nodes[i].pop == this->seqlist[iseq].pop1) {
				this->gt[loc].nodes[i].pop = this->seqlist[iseq].pop;
			}
		}
	}

	void split_pop(int loc,int iseq) {
		for (int i=0;i<this->gt[loc].nnodes;i++){
			if (this->gt[loc].nodes[i].pop == this->seqlist[iseq].pop){
				if (this->mw.random()<this->seqlist[iseq].admixrate) {this->gt[loc].nodes[i].pop = this->seqlist[iseq].pop1;}
				else                                                 {this->gt[loc].nodes[i].pop = this->seqlist[iseq].pop2;}
			}
		}
	}

	void add_sample(int loc,int iseq) {
		//cout <<"add sample "<<this->seqlist[iseq].sample<<"   nnodes="<<this->gt[loc].nnodes<<"\n";
        //int nn=0,j=0;
        bool nodeOK;
		for (int i=0;i<this->gt[loc].nnodes;i++){
            //cout<<"\n a "<<j;;fflush(stdin);
            //j++;
            //cout<<"  "<<this->seqlist[iseq].sample;fflush(stdin);
            //cout<<"  "<<this->gt[loc].nodes[i].sample;;fflush(stdin);
			if (this->gt[loc].nodes[i].sample == this->seqlist[iseq].sample) {
				this->gt[loc].nodes[i].pop = this->seqlist[iseq].pop;
				//cout<<i<<"  ";
                //nn++;
                //cout<<"  "<<i;
			}
		}
		//cout<<"\n";
        //cout<<"\naddsample nn="<<nn<<"   sample = "<<this->seqlist[iseq].sample<<"\n";
	}

	int gpoisson(double lambda) {
		if (lambda<=0) {return 0;}
		int i=0;
		double p=0.0;
		double ra;
		while (p<lambda) {
			i++;
			do {ra = this->mw.random();} while (ra==0.0);
			p -= log(ra);
		}
		return (i-1);
	}

	void put_mutations(int loc) {
		this->gt[loc].nmutot=0;
		double mutrate;
		if (this->locuslist[loc].type <5) mutrate=this->locuslist[loc].mut_rate+this->locuslist[loc].sni_rate;
		else                        mutrate=this->locuslist[loc].mus_rate*(double)this->locuslist[loc].dnalength;
		for (int b=0;b<this->gt[loc].nbranches;b++) {
		    this->gt[loc].branches[b].nmut = this->mw.poisson(this->gt[loc].branches[b].length*mutrate);
			this->gt[loc].nmutot += this->gt[loc].branches[b].nmut;
		}
        //cout<<"Locus "<<loc<<"   mutrate = "<<mutrate<<"   nmutot="<<this->gt[loc].nmutot<<"\n";
	}

	void cherche_branchesOK(int loc) {
		for (int b=0;b<this->gt[loc].nbranches;b++) this->gt[loc].branches[b].OK = true;
		if (this->data.npopref<1) return;
		int j,nodemrca=0,f1,f2,b;
		bool mrca;
//mise à true de nodes[i].OK pour les gènes des populations de référence
		//printf("npopref = %3d     popref[0]=%3d\n",this->data.npopref,this->data.popref[0]);
		for (int i=0;i<this->gt[loc].ngenes;i++) {
			this->gt[loc].nodes[i].OK = false;
			j=0;
			while ((j<this->data.npopref)and(not this->gt[loc].nodes[i].OK)) {
				this->gt[loc].nodes[i].OK = (this->gt[loc].nodes[i].sample == this->data.popref[j]);
				if (not this->gt[loc].nodes[i].OK) j++;
			}
			//printf("node %3d  sample=%3d   popref[0]=%3d   OK=%1d\n",i,this->gt[loc].nodes[i].sample,this->data.popref[0],this->gt[loc].nodes[i].OK);
		}
//mise à false des branches de l'arbre
		for (b=0;b<this->gt[loc].nbranches;b++) this->gt[loc].branches[b].OK = false;
//remplissage des brhaut et brbas des nodes
		for (b=0;b<this->gt[loc].nbranches;b++) {
			this->gt[loc].nodes[this->gt[loc].branches[b].bottom].brhaut = b;
			this->gt[loc].nodes[this->gt[loc].branches[b].top].brbas = b;
		}
//recherche du mrca des gènes des pop de référence et mise à true des branches.OK y conduisant
		for (int i=0;i<this->gt[loc].ngenes-1;i++) {
			if (this->gt[loc].nodes[i].OK) {
				for (int j=i+1;j<this->gt[loc].ngenes;j++) {
					if (this->gt[loc].nodes[j].OK) {
						f1=i;f2=j,b=0;
						do {
							if (this->gt[loc].branches[b].bottom == f1) {
								this->gt[loc].branches[b].OK=true;
								f1 = this->gt[loc].branches[b].top;
							}
							if (this->gt[loc].branches[b].bottom == f2) {
								this->gt[loc].branches[b].OK=true;
								f2 = this->gt[loc].branches[b].top;
							}
							mrca = (f1==f2);
							b++;
						} while (not mrca);
						//printf("i = %3d   j = %3d      f1 = %3d   f2 = %3d     mrca = %3d\n",i,j,f1,f2,nodemrca);
						if (nodemrca<f1) nodemrca=f1;
					} 
				}
			}
		}
// on complète la mise à true des branches.OK de tous les descendants du mrca
		for (int i=0;i<this->gt[loc].ngenes;i++) {
			f1=i;
			do {
				b = this->gt[loc].nodes[f1].brhaut;
				f1 = this->gt[loc].branches[b].top;
			} while (f1<nodemrca);
			if (f1==nodemrca) {  // le gène i descend du mrca
				f1=i;
				do {
					b = this->gt[loc].nodes[f1].brhaut;
					f1 = this->gt[loc].branches[b].top;
					this->gt[loc].branches[b].OK = true;
				} while (f1<nodemrca);
			}
		}
		/*for (b=0;b<this->gt[loc].nbranches;b++){
			printf("branche %3d   bas %3d   haut %3d   (nodemrca %3d)",b,this->gt[loc].branches[b].bottom,this->gt[loc].branches[b].top,nodemrca);
			if (this->gt[loc].branches[b].OK) printf("   OK\n"); else printf("\n");
		}*/
	}

	void put_one_mutation(int loc) {
		this->gt[loc].nmutot=1;
			cherche_branchesOK(loc);
		double r,s=0.0,lengthtot=0.0;
		int b,nOK=0;
		for (b=0;b<this->gt[loc].nbranches;b++) {
			if (this->gt[loc].branches[b].OK) {lengthtot +=this->gt[loc].branches[b].length;nOK++;}
			this->gt[loc].branches[b].nmut = 0;
		}
		r=this->mw.random()*lengthtot;
		b=0;s=this->gt[loc].branches[b].length;
		while (s<r) {b++;if (this->gt[loc].branches[b].OK) s +=this->gt[loc].branches[b].length;};
		this->gt[loc].branches[b].nmut = 1;
		//cout<<nOK<<" branches mutables sur "<<this->gt[loc].nbranches<<"\n";
		//cout<<"numero de la branche mutée : "<<b<<" ("<<this->gt[loc].nbranches<<")"<<"   longueur = "<<this->gt[loc].branches[b].length<<" sur "<<lengthtot<<"\n";
	} 


	void mute(int loc, int numut, int b) {
		if (this->locuslist[loc].type<5) {   //MICROSAT
			int d;
			int g1= this->gt[loc].nodes[this->gt[loc].branches[b].bottom].state;
			if (this->mw.random() < this->locuslist[loc].sni_rate/(this->locuslist[loc].sni_rate+this->locuslist[loc].mut_rate)) {
				if (this->mw.random()<0.5) g1++;
				else            g1--;
			}
			else {
				if (this->locuslist[loc].Pgeom>0.001) {
					double ra;
					do {ra = this->mw.random();} while (ra==0.0);
					d = (1.0 + log(ra)/log(this->locuslist[loc].Pgeom));
				}
				else d=1;
				if (this->mw.random()<0.5)  g1 += d*this->locuslist[loc].motif_size;
				else             g1 -= d*this->locuslist[loc].motif_size;
			}
			if (g1>this->locuslist[loc].kmax) g1=this->locuslist[loc].kmax;
			if (g1<this->locuslist[loc].kmin) g1=this->locuslist[loc].kmin;
			this->gt[loc].nodes[this->gt[loc].branches[b].bottom].state = g1;
		}
		else  if (this->locuslist[loc].type<10) {	//DNA SEQUENCE
			string dna=this->gt[loc].nodes[this->gt[loc].branches[b].bottom].dna;
			char dnb;
			double ra=this->mw.random();
			int n =this->locuslist[loc].sitmut[numut];
            //cout<<"numut="<<numut<<"   n="<<n<<"\n";
			dnb = dna[n];
            if (n<0) {cout<<"probleme n="<<n<<"\n";exit(1);}
            if ((dna[n]!='A')and(dna[n]!='C')and(dna[n]!='G')and(dna[n]!='T')) {
                cout<<"probleme dna[n]="<<dna[n]<<"\n";
                exit(1);
            }
			switch (dna[n])
			{  	case 'A' :
					if (ra<this->matQ[0][1])                        dnb = 'C';
					else if (ra<this->matQ[0][1]+this->matQ[0][2])  dnb = 'G';
					else                                            dnb = 'T';
					break;
				case 'C' :
					if (ra<this->matQ[1][0])                        dnb = 'A';
					else if (ra<this->matQ[1][0]+this->matQ[1][2])  dnb = 'G';
					else                                            dnb = 'T';
					break;
				case 'G' :
					if (ra<this->matQ[2][0])                        dnb = 'A';
					else if (ra<this->matQ[2][0]+this->matQ[2][1])  dnb = 'C';
					else                                            dnb = 'T';
					break;
				case 'T' :
					if (ra<this->matQ[3][0])                        dnb = 'A';
					else if (ra<this->matQ[3][0]+this->matQ[3][1])  dnb = 'C';
					else                                            dnb = 'G';
					break;
			}
            if((debuglevel==9)and(loc==10)) cout<<"mutation "<<numut<<"    site mute = "<<n<<"  de "<<dna[n]<<"  vers "<<dnb<<"\n";
           if (debuglevel==8) cout<<"mutation "<<numut<<"   dna["<<n<<"]="<<dna[n]<<"   dnb="<<dnb<<"\n";
            if (debuglevel==8) cout<<dna<<"\n";
            dna[n] = dnb;
			this->gt[loc].nodes[this->gt[loc].branches[b].bottom].dna = dna;
            if (debuglevel==8) cout<<dna<<"\n";
		}
		else {		//SNP 
			this->gt[loc].nodes[this->gt[loc].branches[b].bottom].state=1;
		}
	}

	char draw_nuc(int loc) {
		double ra=this->mw.random();
		if (ra<this->locuslist[loc].pi_A)															return 'A';
		else if (ra<this->locuslist[loc].pi_A+this->locuslist[loc].pi_C)							return 'C';
		else if (ra<this->locuslist[loc].pi_A+this->locuslist[loc].pi_C+this->locuslist[loc].pi_G)	return 'G';
		else																						return 'T';
	}

	string init_dnaseq(int loc) {
      if (debuglevel==9) cout<<"debut de init_dnaseq("<<loc<<")  dnatrue = "<<this->dnatrue<<"   dnalength="<<this->locuslist[loc].dnalength<<"\n";
	if (debuglevel==9) cout<<"nmutot="<<this->gt[loc].nmutot<<"\n";
	  this->locuslist[loc].tabsit.resize(this->locuslist[loc].dnalength);
		//int *sitmut2;
		string dna="";
        if (this->gt[loc].nmutot>0) {
            //sitmut2 = new int[this->gt[loc].nmutot];
             this->locuslist[loc].sitmut2.resize(this->gt[loc].nmutot);
            this->locuslist[loc].sitmut.resize(this->gt[loc].nmutot);
            for (int i=0;i<this->gt[loc].nmutot;i++){
                double ra=this->mw.random();
                double s=0.0;
                int k=-1;
                while (s<ra) {k++;s+=this->locuslist[loc].mutsit[k];}
                this->locuslist[loc].sitmut2[i]=k;
            }
            if (this->dnatrue) {			//TRUE DNA SEQUENCE
                for (int i=0;i<this->gt[loc].nmutot;i++) this->locuslist[loc].sitmut[i]=this->locuslist[loc].sitmut2[i];
                dna.resize(this->locuslist[loc].dnalength);
                for (int i=0;i<this->locuslist[loc].dnalength;i++){
                    this->locuslist[loc].tabsit[i] = i;
                    dna[i]=draw_nuc(loc);
                }
            }
            else {							//ARTIFICIAL SEQUENCE MADE ONLY OF VARIABLE SITES
                string dna2;
                if (debuglevel==9) cout<<"nmutot="<<this->gt[loc].nmutot<<"\n";
                dna.resize(this->gt[loc].nmutot+1);
                dna2.resize(this->locuslist[loc].dnalength);
                for (int i=0;i<this->locuslist[loc].dnalength;i++){
                    dna2[i] = 'z';
                    this->locuslist[loc].tabsit[i] = -1;
                }
               int k=0;
                for (int i=0;i<this->gt[loc].nmutot;i++) {
                    if (dna2[this->locuslist[loc].sitmut2[i]] == 'z') {
                        this->locuslist[loc].tabsit[this->locuslist[loc].sitmut2[i]] = k;
                        //this->locuslist[loc].sitmut[i]=k;
                        k++;
                        dna2[this->locuslist[loc].sitmut2[i]] = draw_nuc(loc);
                        dna[k-1]=dna2[this->locuslist[loc].sitmut2[i]];
                        //cout<<"i="<<i<<"   sitmut2="<<this->locuslist[loc].sitmut2[i]<<"   dna2="<<dna2[this->locuslist[loc].sitmut2[i]]<<"\n";
                    }
                }
                for (int i=0;i<this->gt[loc].nmutot;i++) this->locuslist[loc].sitmut[i]=this->locuslist[loc].tabsit[this->locuslist[loc].sitmut2[i]];
                dna.resize(k);
                dna2.clear();
            }
            if (debuglevel==10) cout <<dna<<"\n";
            if ((debuglevel==9)and(loc==10)) {
                cout<<"sitmut2\n";
                for (int i=0;i<this->gt[loc].nmutot;i++) cout<<this->locuslist[loc].sitmut2[i]<<"  ";cout<<"\n";

                cout<<"sitmut\n";
                for (int i=0;i<this->gt[loc].nmutot;i++) cout<<this->locuslist[loc].sitmut[i]<<"  ";cout<<"\n";
                 cout<<"tabsit\n";
                for (int i=0;i<this->locuslist[loc].dnalength;i++) if (this->locuslist[loc].tabsit[i]!=-1)cout<<this->locuslist[loc].tabsit[i]<<"  ";cout<<"\n";
           }
            //delete []sitmut2;
            this->locuslist[loc].tabsit.clear();
            this->locuslist[loc].mutsit.clear();
        } else {
            if (this->dnatrue) {			//TRUE DNA SEQUENCE
                for (int i=0;i<this->gt[loc].nmutot;i++) this->locuslist[loc].sitmut[i]=this->locuslist[loc].sitmut2[i];
                dna.resize(this->locuslist[loc].dnalength);
                for (int i=0;i<this->locuslist[loc].dnalength;i++){
                    this->locuslist[loc].tabsit[i] = i;
                    dna[i]=draw_nuc(loc);
                }
			}
		}
        if ((debuglevel==9)and(loc==8)) cout<<dna<<"\n";
        return dna;
	}

	int cree_haplo(int loc) {
		if (debuglevel==10) cout <<"CREE_HAPLO\n";
		vector < vector <int> > ordre;
        if (debuglevel==10) cout<<"avant mise à 10000 des state\n";
		for (int no=0;no<this->gt[loc].nnodes;no++) this->gt[loc].nodes[no].state=10000;
        if (debuglevel==10) cout<<"apres mise à 10000 des state\n";
		int anc=this->gt[loc].nnodes-1;
        if (debuglevel==10) cout<<"anc="<<anc<<"\n";
		if (debuglevel==10) cout<<"kmin = "<<this->locuslist[loc].kmin<<"   kmax = "<<this->locuslist[loc].kmax<<"\n";
		if (this->locuslist[loc].type<5) {        //MICROSAT
			this->gt[loc].nodes[anc].state=this->locuslist[loc].kmin + (int)(0.5*(this->locuslist[loc].kmax-this->locuslist[loc].kmin));
		}
		else if (this->locuslist[loc].type<10) {  //SEQUENCE
			this->gt[loc].nodes[anc].state=0;
			this->gt[loc].nodes[anc].dna = init_dnaseq(loc);
			if (debuglevel==10) cout << "locus " << loc  << "\n";
			if (debuglevel==10) cout << "anc-dna = " << this->gt[loc].nodes[anc].dna << "\n";
		}
		else {                                   //SNP
			this->gt[loc].nodes[anc].state=0;
		}
		int br, numut=-1;
        bool trouve;
		int len;
        if (debuglevel==10) cout <<"avant la boucle while ngenes="<<this->gt[loc].ngenes<<"\n";
		while (anc>=this->gt[loc].ngenes) {
			if (debuglevel>10) cout << "locus " << loc << "   ngenes = " << this->gt[loc].ngenes << "   anc = " << anc << "\n";
			br=0;
			while (br<this->gt[loc].nbranches) {
                trouve = (this->gt[loc].branches[br].top == anc);
                if ((trouve)and(this->gt[loc].nodes[this->gt[loc].branches[br].bottom].state == 10000)) {
                    this->gt[loc].nodes[this->gt[loc].branches[br].bottom].state = this->gt[loc].nodes[this->gt[loc].branches[br].top].state;
                    if (this->locuslist[loc].type>4) {
                        this->gt[loc].nodes[this->gt[loc].branches[br].bottom].dna = this->gt[loc].nodes[this->gt[loc].branches[br].top].dna;
                    }
                    if (this->gt[loc].branches[br].nmut>0) {
                        for (int j=0;j<this->gt[loc].branches[br].nmut;j++) {
                            numut++;
                            mute(loc,numut,br);
                        }
                    }
                }
                br++;
            }
            anc--;
        }
        if((debuglevel==9)and(loc==10)) {
            for (int sa=0;sa<this->nsample;sa++) {
                cout<<"sample "<<sa<<"\n";
                for (int ind=0;ind<this->locuslist[loc].ss[sa];ind++) cout<<ind<<"  "<<this->gt[loc].nodes[ind+sa*this->locuslist[loc].ss[sa]].dna<<"\n";
            }
        }
        this->locuslist[loc].sitmut.clear();
        int sa0=0,sa2=this->locuslist[loc].ss[sa0];
        if (debuglevel==10) cout<<"sa2="<<sa2<<"\n";
		int sa,ind;
		if (debuglevel==10) cout<< "tirage au hasard des gènes avant attribution aux individus\n";
		ordre.resize(this->data.nsample);ind=0;
        if (debuglevel==10) cout<<"apres resize de ordre\n";
		for (sa=0;sa<this->data.nsample;sa++) {
			ordre[sa] = melange(this->mw,this->locuslist[loc].ss[sa]);
			if (sa>0) {for (int i=0;i<this->locuslist[loc].ss[sa];i++) ordre[sa][i]+=ind;}
			ind +=this->locuslist[loc].ss[sa];
		}
		if((debuglevel==9)and(loc==10)) {
            for (int sa=0;sa<this->nsample;sa++) {
                for (int ind=0;ind<this->locuslist[loc].ss[sa];ind++) cout<<"ordre["<<sa<<"]["<<ind<<"] = "<<ordre[sa][ind]<<"\n";
            }
        }
		if (debuglevel==10) cout<<"apres ordre\n";
        this->locuslist[loc].samplesize =new int[this->data.nsample];
        for (int sa=0;sa<this->data.nsample;sa++) this->locuslist[loc].samplesize[sa] =this->locuslist[loc].ss[sa];
		if (this->locuslist[loc].type<5) {      //MICROSAT
			this->locuslist[loc].haplomic = new int*[this->data.nsample];
			for (sa=0;sa<this->data.nsample;sa++) this->locuslist[loc].haplomic[sa] = new int [this->locuslist[loc].ss[sa]];
			sa=0;ind=0;
			for (int i=0;i<this->gt[loc].ngenes;i++) {
				if (this->gt[loc].nodes[ordre[sa][ind]].state == 10000) {ordre.clear();return 2;}
				this->locuslist[loc].haplomic[sa][ind] = this->gt[loc].nodes[ordre[sa][ind]].state;
				ind++;if (ind==this->locuslist[loc].ss[sa]) {sa++;ind=0;}
			}
			if (debuglevel==10) cout<<"apres repartition dans le sample 0\n";
		}
		else if (this->locuslist[loc].type<10) {//DNA SEQUENCES
			this->locuslist[loc].haplodna = new string*[this->data.nsample];
			for (sa=0;sa<this->data.nsample;sa++) this->locuslist[loc].haplodna[sa] = new string [this->locuslist[loc].ss[sa]];
			sa=0;ind=0;
			for (int i=0;i<this->gt[loc].ngenes;i++) {
				if (this->gt[loc].nodes[ordre[sa][ind]].state == 10000) {
					for (int br=0;br<this->gt[loc].nbranches;br++)
						{if (debuglevel==10) cout << "branche " << br << "   bottom=" << this->gt[loc].branches[br].bottom ;
						 if (debuglevel==10) cout << "   top=" << this->gt[loc].branches[br].top << "   nmut=" << this->gt[loc].branches[br].nmut;
						 if (debuglevel==10) cout << "   length=" << this->gt[loc].branches[br].length << "\n";
						}
					if (debuglevel==10) cout <<"\n";
					for (int n=0;n<this->gt[loc].nnodes;n++) {
						if (debuglevel==10) cout << "noeud  " << n << "   state = " << this->gt[loc].nodes[n].state << "   dna = " ;
						if (debuglevel==10) for (int j=0;j<len+1;j++) cout << this->gt[loc].nodes[n].dna[j];
						if (debuglevel==10) cout << "\n";
					}
					if (debuglevel==10) cout << "\n";
					ordre.clear();
					return 2;
				}
				this->locuslist[loc].haplodna[sa][ind] = this->gt[loc].nodes[ordre[sa][ind]].dna;
				ind++;if (ind==this->locuslist[loc].ss[sa]) {sa++;ind=0;}
				}
		}
		else {									//SNP
			this->locuslist[loc].haplosnp = new short int*[this->data.nsample];
			for (sa=0;sa<this->data.nsample;sa++) this->locuslist[loc].haplosnp[sa] = new short int [this->locuslist[loc].ss[sa]];
			sa=0;ind=0;
			for (int i=0;i<this->gt[loc].ngenes;i++) {
				if (this->gt[loc].nodes[ordre[sa][ind]].state == 10000) {ordre.clear();return 2;}
				this->locuslist[loc].haplosnp[sa][ind] = (short int)this->gt[loc].nodes[ordre[sa][ind]].state;
				ind++;if (ind==this->locuslist[loc].ss[sa]) {sa++;ind=0;}
			}
			if (debuglevel==10) cout<<"apres repartition dans le sample 0\n";
		}
		ordre.clear();
        if (debuglevel==7)cout<<"Locus "<<loc<<"    nmutotdans cree_haplo = "<<this->gt[loc].nmutot<<"\n";
		return 0;
	}

    string verifytree() {
        bool *popleine;
        popleine = new bool[this->scen.popmax+1];
        for (int p=1;p<this->scen.popmax+1;p++) {popleine[p]=false;}
        for (int iseq=0;iseq<this->nseq;iseq++) {
            this->seqlist[iseq].popfull = new bool[this->scen.popmax+1];
            if (iseq==0) for (int p=1;p<this->scen.popmax+1;p++) this->seqlist[iseq].popfull[p]=false;
            else for (int p=1;p<this->scen.popmax+1;p++) this->seqlist[iseq].popfull[p]=this->seqlist[iseq-1].popfull[p];
            if (this->seqlist[iseq].action == 'C') {
                if ((this->seqlist[iseq].t1>-1)and(this->seqlist[iseq].t1<this->seqlist[iseq].t0)) {
                    this->seqlist[iseq].ecris();
                    return "probleme coal avec t1<t0";
                }
                if ((not popleine[this->seqlist[iseq].pop])and(this->seqlist[iseq].t1>this->seqlist[iseq].t0)) {
                    for (int jseq=0;jseq<=iseq;jseq++) {
                        this->seqlist[jseq].ecris();
                        for (int p=1;p<this->scen.popmax+1;p++) if (this->seqlist[jseq].popfull[p]) cout<<p<<"  ";
                        cout<<"\n";
                    }
                    cout<<"\n";
                    this->seqlist[iseq].ecris();return "probleme coal avec pop vide";
                }
            }
            else if (this->seqlist[iseq].action == 'M') {
                if ((not popleine[this->seqlist[iseq].pop])and (not popleine[this->seqlist[iseq].pop1])) {
                    this->seqlist[iseq].ecris();return "probleme merge de deux pop vides";
                }
                popleine[this->seqlist[iseq].pop]=true;this->seqlist[iseq].popfull[this->seqlist[iseq].pop]=true;
                popleine[this->seqlist[iseq].pop1]=false;this->seqlist[iseq].popfull[this->seqlist[iseq].pop1]=false;
            }
            else if (this->seqlist[iseq].action == 'S') {
                if (not popleine[this->seqlist[iseq].pop]) {this->seqlist[iseq].ecris();return "probleme split avec pop vide";}
                popleine[this->seqlist[iseq].pop]=false;this->seqlist[iseq].popfull[this->seqlist[iseq].pop]=false;
                popleine[this->seqlist[iseq].pop1]=true;this->seqlist[iseq].popfull[this->seqlist[iseq].pop1]=true;
                popleine[this->seqlist[iseq].pop2]=true;this->seqlist[iseq].popfull[this->seqlist[iseq].pop2]=true;
            }
            else if (this->seqlist[iseq].action == 'A') {
                popleine[this->seqlist[iseq].pop]=true;this->seqlist[iseq].popfull[this->seqlist[iseq].pop]=true;
            }
       }
        for (int iseq=0;iseq<this->nseq;iseq++) delete []this->seqlist[iseq].popfull;
        delete []popleine;
        return "";
    }
    
	int dosimulpart(int numscen){
        if (debuglevel==5)        {cout<<"debut de dosimulpart\n";fflush(stdin);}
		vector <int> simulOK;
        string checktree;
		int *emptyPop,loc;
		bool treedone,dnaloc=false,trouve;
        int locus,sa,indiv,nuc;
        //cout<<"this->nloc="<<this->nloc<<"\n";
		simulOK.resize(this->nloc);
		GeneTreeC GeneTreeY, GeneTreeM;
        if (debuglevel==10) cout<<"avant draw scenario\n";fflush(stdin);
		this->drawscenario(numscen);
		if (debuglevel==10) cout <<"avant setHistparamValue\n";fflush(stdin);
		this->setHistParamValue();
		if (debuglevel==10) cout << "apres setHistParamValue\n";fflush(stdin);
		//if (trace) cout<<"scen.nparam = "<<this->scen.nparam<<"\n";
		//if (trace) for (int k=0;k<this->scen.nparam;k++){
		//         	cout << this->scen.histparam[k].value << "   ";fflush(stdin);}
		this->setSequence();
		if (debuglevel==10) cout <<"apres setSequence\n";
		if (debuglevel==5) cout <<"avant checktree\n";
        checktree=this->verifytree();
        if (checktree!="") {
            FILE *flog;
            cout<<checktree<<"\n";
            this->scen.ecris();
            checktree="A gene genealogy failed in scenario "+IntToString(numscen+1)+". Check scenario and prior consistency.\n  ";
            flog=fopen(reftablelogfilename,"w");fprintf(flog,"%s",checktree.c_str());fclose(flog);
            exit(1);
        }
        if (debuglevel==5) cout <<"apres checktree\n";
		bool gtYexist=false, gtMexist=false;
		this->gt = new GeneTreeC[this->nloc];
		emptyPop = new int[this->scen.popmax+1];
		//cout << "particule " << ipart <<"   nparam="<<this->scen.nparam<<"\n";
		//for (int k=0;k<this->scen.nparam;k++){
		//	cout << this->scen.histparam[k].value << "   ";
		//}
		//cout << "\n";
        for (loc=0;loc<this->nloc;loc++) simulOK[loc]=-1;
		setMutParammoyValue();
        if (debuglevel==10) cout<<"nloc="<<this->nloc<<"\n";fflush(stdin);
		for (loc=0;loc<this->nloc;loc++) {
             if (debuglevel==10) cout<<"debut de la boucle du locus "<<loc<<"\n";fflush(stdin);
			if (this->locuslist[loc].groupe>0) { //On se limite aux locus inclus dans un groupe
				if (this->locuslist[loc].type <10) setMutParamValue(loc);
				if ((this->locuslist[loc].type >4)and(this->locuslist[loc].type <10)) {
					comp_matQ(loc);
                    dnaloc=true;
					} else dnaloc=false;
				treedone=false;
				if ((this->locuslist[loc].type % 5) == 3) {
					if (gtYexist) {this->gt[loc] = copytree(GeneTreeY);treedone=true;}
				}
				else if ((locuslist[loc].type % 5) == 4) {
					    if (debuglevel==10) cout << "coucou   gtMexist=" << gtMexist <<"\n";
						if (gtMexist) {this->gt[loc] = copytree(GeneTreeM);treedone=true;}
					}
				if (not treedone) {
					if (debuglevel==10) cout << "avant init_tree \n";
					this->gt[loc] = init_tree(loc);
					if (debuglevel==10){
                        cout << "initialisation de l'arbre du locus " << loc  << "    ngenes="<< this->gt[loc].ngenes<< "   nseq="<< this->nseq <<"\n";
                        cout<< "scenario "<<this->scen.number<<"\n";
                    }
					for (int p=0;p<this->scen.popmax+1;p++) {emptyPop[p]=1;} //True
					for (int iseq=0;iseq<this->nseq;iseq++) {
						if (debuglevel==10) {
                            cout << "traitement de l element de sequence " << iseq << "    action= "<<this->seqlist[iseq].action;

                           if (this->seqlist[iseq].action == 'C') cout <<"   "<<this->seqlist[iseq].t0<<" - "<<this->seqlist[iseq].t1;
                           else  cout <<"   "<<this->seqlist[iseq].t0;
                           cout<<"    pop="<<this->seqlist[iseq].pop;cout<<"    Ne="<<this->seqlist[iseq].N;
                           if ((this->seqlist[iseq].action == 'M')or(this->seqlist[iseq].action == 'S')) cout <<"   pop1="<<this->seqlist[iseq].pop1;
                           if (this->seqlist[iseq].action == 'S') cout <<"   pop2="<<this->seqlist[iseq].pop2;
                           cout<<"\n";
                           fflush(stdin);
                        }
						if (this->seqlist[iseq].action == 'C') {	//COAL
							//for (int k=1;k<4;k++) cout << emptyPop[k] << "   ";
							//cout <<"\n";
							if (((this->seqlist[iseq].t1>this->seqlist[iseq].t0)or(this->seqlist[iseq].t1<0))and(emptyPop[seqlist[iseq].pop]==0)) {
								//cout << "dosimul appel de coal_pop \n";
								coal_pop(loc,iseq);
								//cout << "apres coal_pop\n";
							}
						}
						else if (this->seqlist[iseq].action == 'M') {	//MERGE
							if ((emptyPop[seqlist[iseq].pop]==0)or(emptyPop[seqlist[iseq].pop1]==0)) {
								//cout << "dosimul appel de pool_pop \n";
								pool_pop(loc,iseq);
								emptyPop[seqlist[iseq].pop]  =0;
								emptyPop[seqlist[iseq].pop1] =1;
							}
						}
						else if (this->seqlist[iseq].action == 'S') {  //SPLIT
							if (emptyPop[seqlist[iseq].pop]==0) {
								//cout << "dosimul appel de split_pop \n";
								split_pop(loc,iseq);
								emptyPop[seqlist[iseq].pop]  =1;
								emptyPop[seqlist[iseq].pop1] =0;
								emptyPop[seqlist[iseq].pop2] =0;
							}
						}
						else if (this->seqlist[iseq].action == 'A') {  //ADSAMP
							//cout << "dosimul appel de add_sample \n";
							add_sample(loc,iseq);
							emptyPop[seqlist[iseq].pop]  =0;
							//for (int k=1;k<4;k++) cout << emptyPop[k] << "   ";
							//cout <<"\n";
						}
					}	//LOOP ON iseq
		/* copie de l'arbre si locus sur Y ou mitochondrie */
					if (not gtYexist) {if ((locuslist[loc].type % 5) == 3) {GeneTreeY  = copytree(this->gt[loc]);gtYexist=true;}}

					if (not gtMexist) {if ((locuslist[loc].type % 5) == 4) {GeneTreeM  = copytree(this->gt[loc]);gtMexist=true;}}
				}
	/* mutations */
				if (this->locuslist[loc].type <10) put_mutations(loc);
				else put_one_mutation(loc);
				 if (debuglevel==10) cout << "Locus " <<loc << "  apres put_mutations\n";
				simulOK[loc]=cree_haplo(loc);
				if (debuglevel==10) cout << "Locus " <<loc << "  apres cree_haplo   : simOK[loc] ="<<simulOK[loc]<<"\n";fflush(stdin);

				locus=loc;
                if (simulOK[loc] != 0) {if (debuglevel==10) cout << "avant break interne\n";break;}
				if (debuglevel==10) cout << "fin du locus " << loc << "   "<< simulOK[loc] << "\n";
			}
			//cout<<"   OK\n\n";
			//if (loc==50) 
			//exit(6);
		}		//LOOP ON loc
        if (simulOK[locus]==0) {
                if (debuglevel==10) cout<<this->data.nmisshap<<" donnees manquantes et "<<this->data.nmissnuc<<" nucleotides manquants\n";fflush(stdin);
                if (this->data.nmisshap>0) {
                        for (int i=0;i<this->data.nmisshap;i++) {
                              locus=this->data.misshap[i].locus;
                              if (this->locuslist[locus].groupe>0) {
                                      sa=this->data.misshap[i].sample;indiv=this->data.misshap[i].indiv;
                                      //cout<<"MISSHAP   locus "<<locus<<"  sample "<<sa<<"  indiv "<<indiv<<"\n";
                                      if (this->locuslist[locus].type<5) this->locuslist[locus].haplomic[sa][indiv] = MICMISSING;
                                      else                               this->locuslist[locus].haplodna[sa][indiv] = SEQMISSING;
                                      this->locuslist[locus].samplesize[sa]--;
                              }
                        }

                }
                if (this->data.nmissnuc>0) {
                        for (int i=0;i<this->data.nmissnuc;i++) {
                              locus=this->data.missnuc[i].locus;
                              if ((this->locuslist[locus].groupe>0)and(this->gt[locus].nmutot>0)) {
                                      sa=this->data.missnuc[i].sample;
                                      indiv=this->data.missnuc[i].indiv;
                                      nuc=this->data.missnuc[i].nuc;
                                      if (debuglevel==10) cout<<"MISSNUC "<<i<<"  locus "<<locus<<"  sample "<<sa<<"  indiv "<<indiv<<"  nuc "<<nuc/*<<"\n"*/;
                                      int k=0;trouve=false;
                                      while ((k<this->locuslist[locus].sitmut.size())and(not trouve)) {
                                          trouve = (nuc==this->locuslist[locus].sitmut[k]);
                                          if (not trouve) k++;
                                      }
                                      if (trouve) {
                                          //cout<<"MISSNUC  locus "<<locus<<"  sample "<<sa<<"  indiv "<<indiv<<"  nuc "<<nuc<<"  k="<<k<<"\n";
                                          if (debuglevel==10) cout<</*this->locuslist[locus].haplodna[sa][indiv]<<*/"   k="<<k<<"  ("<<this->locuslist[locus].haplodna[sa][indiv].length()<<")\n";
                                          //this->locuslist[locus].haplodna[sa][indiv].replace(k,1,"N");
										  this->locuslist[locus].haplodna[sa][indiv][k]='N';
                                          //cout<<this->locuslist[locus].haplodna[sa][indiv]<<"\n";
                                      } else {
										  if (debuglevel==10) cout<<"  (non muté)\n";
									  }
                              }

                        }

                }
                if (this->data.nmisssnp>0){
					cout<<"ANORMAL\n";
					for (int i=0;i<this->data.nmisssnp;i++) {
						locus=this->data.misssnp[i].locus;
                        if (this->locuslist[locus].groupe>0) {
							sa=this->data.misssnp[i].sample;indiv=this->data.misssnp[i].indiv;
                            //cout<<"MISSHAP   locus "<<locus<<"  sample "<<sa<<"  indiv "<<indiv<<"\n";
							this->locuslist[locus].haplosnp[sa][indiv] = (short int)SNPMISSING;
							this->locuslist[locus].samplesize[sa]--;
						}
					}
					
				}
        }
        if (debuglevel==10) cout<<"avant le sitmut2.clear()\n";
        for (loc=0;loc<this->nloc;loc++) if ((this->locuslist[loc].type>4)and(simulOK[loc]>-1)) this->locuslist[loc].sitmut2.clear();
        if (debuglevel==10) cout<<"avant les delete\n";fflush(stdin);
        delete [] emptyPop;
        delete [] this->seqlist;
		for (int loc=0;loc<this->nloc;loc++) {
            if ((this->locuslist[loc].groupe>0)and(simulOK[loc]>-1)) {
                if (debuglevel==10) cout<<"deletetree du locus "<<loc<<"\n";
                if(this->locuslist[loc].type>4) deletetree(this->gt[loc],true);
                else deletetree(this->gt[loc],false);
            }
        }
		delete [] this->gt;
		if (gtYexist) deletetree(GeneTreeY,dnaloc);
		if (gtMexist) deletetree(GeneTreeM,dnaloc);
		//if (trace) cout << "Fin de dosimulpart \n";
		int simOK=0;for (int loc=0;loc<this->nloc;loc++) {if (this->locuslist[loc].groupe>0) simOK+=simulOK[loc];}
		if (debuglevel==10) cout<<"fin de dosimulpart   simOK="<<simOK<<"\n";fflush(stdin);
        if (debuglevel==7) {
            for (int loc=0;loc<this->nloc;loc++) if(this->locuslist[loc].type>4) {
                cout<<"Locus "<<loc<<"\n";
                for(int sa=0;sa<this->nsample;sa++) {cout<<this->locuslist[loc].haplodna[sa][0]<<"\n";cout<<this->locuslist[loc].haplodna[sa][1]<<"\n\n";}
            }
        }
        //exit(1);
		return simOK;
	}

/*	int dosimulpartsnp(int numscen){
        if (debuglevel==5)        {cout<<"debut de dosimulpartsnp\n";fflush(stdin);}
		vector <int> simulOK;
        string checktree;
		int *emptyPop,loc;
		bool treedone,dnaloc=false,trouve;
        int locus,sa,indiv,nuc;
        //cout<<"this->nloc="<<this->nloc<<"\n";
		simulOK.resize(this->nloc);
		GeneTreeC GeneTreeY, GeneTreeM;
        if (debuglevel==10) cout<<"avant draw scenario\n";fflush(stdin);
		this->drawscenario(numscen);
		if (debuglevel==10) cout <<"avant setHistparamValue\n";fflush(stdin);
		this->setHistParamValue();
		if (debuglevel==10) cout << "apres setHistParamValue\n";fflush(stdin);
		//if (trace) cout<<"scen.nparam = "<<this->scen.nparam<<"\n";
		//if (trace) for (int k=0;k<this->scen.nparam;k++){
		//         	cout << this->scen.histparam[k].value << "   ";fflush(stdin);}
		this->setSequence();
		if (debuglevel==10) cout <<"apres setSequence\n";
		if (debuglevel==5) cout <<"avant checktree\n";
        checktree=this->verifytree();
        if (checktree!="") {
            FILE *flog;
            cout<<checktree<<"\n";
            this->scen.ecris();
            checktree="A gene genealogy failed in scenario "+IntToString(numscen+1)+". Check scenario and prior consistency.\n  ";
            flog=fopen(reftablelogfilename,"w");fprintf(flog,"%s",checktree.c_str());fclose(flog);
            exit(1);
        }
        if (debuglevel==5) cout <<"apres checktree\n";
		bool gtYexist=false, gtMexist=false;
		this->gt = new GeneTreeC[this->nloc];
		emptyPop = new int[this->scen.popmax+1];
		//cout << "particule " << ipart <<"   nparam="<<this->scen.nparam<<"\n";
		//for (int k=0;k<this->scen.nparam;k++){
		//	cout << this->scen.histparam[k].value << "   ";
		//}
		//cout << "\n";
        for (loc=0;loc<this->nloc;loc++) simulOK[loc]=-1;
		setMutParammoyValue();
        if (debuglevel==10) cout<<"nloc="<<this->nloc<<"\n";fflush(stdin);
		for (loc=0;loc<this->nloc;loc++) {
             if (debuglevel==10) cout<<"debut de la boucle du locus "<<loc<<"\n";fflush(stdin);
			if (this->locuslist[loc].groupe>0) { //On se limite aux locus inclus dans un groupe
				setMutParamValue(loc);
				if (this->locuslist[loc].type >4) {
					comp_matQ(loc);
                    dnaloc=true;
					} else dnaloc=false;
				treedone=false;
				if ((this->locuslist[loc].type % 5) == 3) {
					if (gtYexist) {this->gt[loc] = copytree(GeneTreeY);treedone=true;}
				}
				else if ((locuslist[loc].type % 5) == 4) {
					    if (debuglevel==10) cout << "coucou   gtMexist=" << gtMexist <<"\n";
						if (gtMexist) {this->gt[loc] = copytree(GeneTreeM);treedone=true;}
					}
				if (not treedone) {
					if (debuglevel==10) cout << "avant init_tree \n";
					this->gt[loc] = init_tree(loc);
					if (debuglevel==10){
                        cout << "initialisation de l'arbre du locus " << loc  << "    ngenes="<< this->gt[loc].ngenes<< "   nseq="<< this->nseq <<"\n";
                        cout<< "scenario "<<this->scen.number<<"\n";
                    }
					for (int p=0;p<this->scen.popmax+1;p++) {emptyPop[p]=1;} //True
					for (int iseq=0;iseq<this->nseq;iseq++) {
						if (debuglevel==10) {
                            cout << "traitement de l element de sequence " << iseq << "    action= "<<this->seqlist[iseq].action;

                           if (this->seqlist[iseq].action == 'C') cout <<"   "<<this->seqlist[iseq].t0<<" - "<<this->seqlist[iseq].t1;
                           else  cout <<"   "<<this->seqlist[iseq].t0;
                           cout<<"    pop="<<this->seqlist[iseq].pop;
                           if ((this->seqlist[iseq].action == 'M')or(this->seqlist[iseq].action == 'S')) cout <<"   pop1="<<this->seqlist[iseq].pop1;
                           if (this->seqlist[iseq].action == 'S') cout <<"   pop2="<<this->seqlist[iseq].pop2;
                           cout<<"\n";
                           fflush(stdin);
                        }
						if (this->seqlist[iseq].action == 'C') {	//COAL
							//for (int k=1;k<4;k++) cout << emptyPop[k] << "   ";
							//cout <<"\n";
							if (((this->seqlist[iseq].t1>this->seqlist[iseq].t0)or(this->seqlist[iseq].t1<0))and(emptyPop[seqlist[iseq].pop]==0)) {
								//cout << "dosimul appel de coal_pop \n";
								coal_pop(loc,iseq);
								//cout << "apres coal_pop\n";
							}
						}
						else if (this->seqlist[iseq].action == 'M') {	//MERGE
							if ((emptyPop[seqlist[iseq].pop]==0)or(emptyPop[seqlist[iseq].pop1]==0)) {
								//cout << "dosimul appel de pool_pop \n";
								pool_pop(loc,iseq);
								emptyPop[seqlist[iseq].pop]  =0;
								emptyPop[seqlist[iseq].pop1] =1;
							}
						}
						else if (this->seqlist[iseq].action == 'S') {  //SPLIT
							if (emptyPop[seqlist[iseq].pop]==0) {
								//cout << "dosimul appel de split_pop \n";
								split_pop(loc,iseq);
								emptyPop[seqlist[iseq].pop]  =1;
								emptyPop[seqlist[iseq].pop1] =0;
								emptyPop[seqlist[iseq].pop2] =0;
							}
						}
						else if (this->seqlist[iseq].action == 'A') {  //ADSAMP
							//cout << "dosimul appel de add_sample \n";
							add_sample(loc,iseq);
							emptyPop[seqlist[iseq].pop]  =0;
							//for (int k=1;k<4;k++) cout << emptyPop[k] << "   ";
							//cout <<"\n";
						}
					}	//LOOP ON iseq
		// copie de l'arbre si locus sur Y ou mitochondrie 
					if (not gtYexist) {if ((locuslist[loc].type % 5) == 3) {GeneTreeY  = copytree(this->gt[loc]);gtYexist=true;}}

					if (not gtMexist) {if ((locuslist[loc].type % 5) == 4) {GeneTreeM  = copytree(this->gt[loc]);gtMexist=true;}}
				}
	// mutations 
				put_mutations(loc);
				 if (debuglevel==10) cout << "Locus " <<loc << "  apres put_mutations\n";
				simulOK[loc]=cree_haplo(loc);
				if (debuglevel==10) cout << "Locus " <<loc << "  apres cree_haplo   : simOK[loc] ="<<simulOK[loc]<<"\n";fflush(stdin);

				locus=loc;
                if (simulOK[loc] != 0) {if (debuglevel==10) cout << "avant break interne\n";break;}
				if (debuglevel==10) cout << "fin du locus " << loc << "   "<< simulOK[loc] << "\n";
			}
			//cout<<"   OK\n";
		}		//LOOP ON loc
        if (simulOK[locus]==0) {
                if (debuglevel==10) cout<<this->data.nmisshap<<" donnees manquantes et "<<this->data.nmissnuc<<" nucleotides manquants\n";fflush(stdin);
        }
        if (debuglevel==10) cout<<"avant le sitmut2.clear()\n";
        for (loc=0;loc<this->nloc;loc++) if ((this->locuslist[loc].type>4)and(simulOK[loc]>-1)) this->locuslist[loc].sitmut2.clear();
        if (debuglevel==10) cout<<"avant les delete\n";fflush(stdin);
        delete [] emptyPop;
        delete [] this->seqlist;
		for (int loc=0;loc<this->nloc;loc++) {
            if ((this->locuslist[loc].groupe>0)and(simulOK[loc]>-1)) {
                if (debuglevel==10) cout<<"deletetree du locus "<<loc<<"\n";
                if(this->locuslist[loc].type>4) deletetree(this->gt[loc],true);
                else deletetree(this->gt[loc],false);
            }
        }
		delete [] this->gt;
		if (gtYexist) deletetree(GeneTreeY,dnaloc);
		if (gtMexist) deletetree(GeneTreeM,dnaloc);
		//if (trace) cout << "Fin de dosimulpart \n";
		int simOK=0;for (int loc=0;loc<this->nloc;loc++) {if (this->locuslist[loc].groupe>0) simOK+=simulOK[loc];}
		if (debuglevel==10) cout<<"fin de dosimulpart   simOK="<<simOK<<"\n";fflush(stdin);
        if (debuglevel==7) {
            for (int loc=0;loc<this->nloc;loc++) if(this->locuslist[loc].type>4) {
                cout<<"Locus "<<loc<<"\n";
                for(int sa=0;sa<this->nsample;sa++) {cout<<this->locuslist[loc].haplodna[sa][0]<<"\n";cout<<this->locuslist[loc].haplodna[sa][1]<<"\n\n";}
            }
        }
        //exit(1);
		return simOK;
	}*/
	
/***********************************************************************************************************************/

    string dogenepop(){
        string sgp,sind,snum;
        int iloc,*k,ty;
        k = new int[this->nloc];
        sgp="Simulated genepop file \n";
		//cout<<sgp<<"\n";
		//cout<<"nloc="<<this->nloc<<"\n";
        for (iloc=0;iloc<this->nloc;iloc++) {
            if (this->locuslist[iloc].type==0) sgp +="Locus M_A_"+IntToString(iloc+1)+"  <A>\n";
            if (this->locuslist[iloc].type==1) sgp +="Locus M_H_"+IntToString(iloc+1)+"  <H>\n";
            if (this->locuslist[iloc].type==2) sgp +="Locus M_X_"+IntToString(iloc+1)+"  <X>\n";
            if (this->locuslist[iloc].type==3) sgp +="Locus M_Y_"+IntToString(iloc+1)+"  <Y>\n";
            if (this->locuslist[iloc].type==4) sgp +="Locus M_M_"+IntToString(iloc+1)+"  <M>\n";
            if (this->locuslist[iloc].type==5) sgp +="Locus S_A_"+IntToString(iloc+1)+"  <A>\n";
            if (this->locuslist[iloc].type==6) sgp +="Locus S_H_"+IntToString(iloc+1)+"  <H>\n";
            if (this->locuslist[iloc].type==7) sgp +="Locus S_X_"+IntToString(iloc+1)+"  <X>\n";
            if (this->locuslist[iloc].type==8) sgp +="Locus S_Y_"+IntToString(iloc+1)+"  <Y>\n";
            if (this->locuslist[iloc].type==9) sgp +="Locus S_M_"+IntToString(iloc+1)+"  <M>\n";
        }
        //cout<<sgp<<"\n";
        for (int ech=0;ech<this->data.nsample;ech++) {
            if (ech==0) sgp += "POP"; else {sgp +="\nPOP";/*cout<<sgp<<"\n";*/}
            for (int loc=0;loc<this->nloc;loc++) k[loc] = 0;
            for (int ind=0;ind<this->data.nind[ech];ind++) {
                snum = IntToString(ind+1);
                if (ind<9)  snum ="0"+snum;
                if (ind<99) snum ="0"+snum;
                sgp += "\n  "+IntToString(ech+1)+"-"+snum+"  , ";
                for (iloc=0;iloc<this->nloc;iloc++) {
                    ty = this->locuslist[iloc].type;
				    //if (ech>0) cout<<"iloc="<<iloc<<"    type="<<ty<<"   k[iloc]=]"<<k[iloc]<<"   index="<<k[iloc] <<"\n";
                    if (ty==0) {
                        sgp +="   "+IntToString(this->locuslist[iloc].haplomic[ech][k[iloc]]);k[iloc]++;
                        sgp +=IntToString(this->locuslist[iloc].haplomic[ech][k[iloc]]);k[iloc]++;
                    }
                    if (ty==1) {sgp +="   "+IntToString(this->locuslist[iloc].haplomic[ech][k[iloc]]);k[iloc]++;}
                    if (ty==2) {
                        sgp +="   "+IntToString(this->locuslist[iloc].haplomic[ech][k[iloc]]);k[iloc]++;
                        if (this->data.indivsexe[ech][ind]==2) {sgp +=IntToString(this->locuslist[iloc].haplomic[ech][k[iloc]]);k[iloc]++;}
                        else sgp += "   ";
                    }
                    if (ty==3) {
                        if (this->data.indivsexe[ech][ind]==2) sgp +="   000";
                        else {sgp +="   "+IntToString(this->locuslist[iloc].haplomic[ech][k[iloc]]);k[iloc]++;}
                    }
                    if (ty==4) {sgp +="   "+IntToString(this->locuslist[iloc].haplomic[ech][k[iloc]]);k[iloc]++;}
                    if (ty==5) {
                        sgp +="   <["+this->locuslist[iloc].haplodna[ech][k[iloc]]+"][";k[iloc]++;
                        sgp += this->locuslist[iloc].haplodna[ech][k[iloc]]+"]>";k[iloc]++;
                    }
                    if (ty==6) {sgp +="   <["+this->locuslist[iloc].haplodna[ech][k[iloc]]+"]>";k[iloc]++;}
                    if (ty==7) {

                        sgp +="   <["+this->locuslist[iloc].haplodna[ech][k[iloc]]+"]";k[iloc]++;
                        if (this->data.indivsexe[ech][ind]==2) {sgp += "["+this->locuslist[iloc].haplodna[ech][k[iloc]]+"]>";k[iloc]++;}
                        else sgp += ">";
                    }
                    if (ty==8) {
                        if (this->data.indivsexe[ech][ind]==2) sgp +="   [<>]";
                        else {sgp +="   <["+this->locuslist[iloc].haplodna[ech][k[iloc]]+"]>";k[iloc]++;}
                    }
                    if (ty==9) {sgp +="   <["+this->locuslist[iloc].haplodna[ech][k[iloc]]+"]>";k[iloc]++;}
                }
            }
        }
        return sgp;
    }

/***********************************************************************************************************************/

	void calfreq(int gr) {
		//int n=0;
		//cout <<"debut de calfreq nloc = "<<this->nloc<<"  groupe = "<<gr <<"\n";
        int loc,iloc;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            loc=this->grouplist[gr].loc[iloc];
           // cout <<"\nLocus "<< loc <<"   kmin="<<this->locuslist[loc].kmin<<"   kmax="<<this->locuslist[loc].kmax<<"\n";
            this->locuslist[loc].nal = this->locuslist[loc].kmax-this->locuslist[loc].kmin+1;
            this->locuslist[loc].freq = new long double* [this->data.nsample];
            //cout <<"nal ="<<this->locuslist[loc].nal<<"\n";
            for (int samp=0;samp<this->data.nsample;samp++) {
                this->locuslist[loc].freq[samp] = new long double [this->locuslist[loc].nal];
                for (int i=0;i<this->locuslist[loc].nal;i++) this->locuslist[loc].freq[samp][i]=0.0;
                //cout << this->locuslist[loc].ss[samp] <<"\n";
                //cout <<this->locuslist[loc].samplesize[samp]<< "\n";
                for (int i=0;i<this->locuslist[loc].ss[samp];i++){
                    if (this->locuslist[loc].haplomic[samp][i] != MICMISSING) {
                        //cout <<"  "<<this->locuslist[loc].haplomic[samp][i];
                        this->locuslist[loc].freq[samp][this->locuslist[loc].haplomic[samp][i]-this->locuslist[loc].kmin] +=1.0;
                        //n++;
                    }
                }
                //cout <<" fini\n";
                //if (this->locuslist[loc].samplesize[samp]<1) cout << "samplesize["<<loc<<"]["<<samp<<"]="<<this->locuslist[loc].samplesize[samp]<<"\n";
                for (int k=0;k<this->locuslist[loc].kmax-this->locuslist[loc].kmin+1;k++)if (this->locuslist[loc].samplesize[samp]>0) this->locuslist[loc].freq[samp][k]/=this->locuslist[loc].samplesize[samp];
                /*cout << "loc "<<loc <<"  sa "<<samp;
                for (int k=0;k<this->locuslist[loc].nal;k++) {if (this->locuslist[loc].freq[samp][k]>0.0) cout << "   "<<this->locuslist[loc].freq[samp][k]<<"("<<k+this->locuslist[loc].kmin<<")";}
                cout << "\n";*/
		    }
		}
		//cout <<"fin de calfreq \n";
	}

	void calfreqsnp(int gr) {
        int loc,iloc;
		short int g0=0;
		long double h0,h1,h2,m0=0.0,m1=0.0,m2=0.0;
		//cout << "calfreqsnp début\n";
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            loc=this->grouplist[gr].loc[iloc];
            this->locuslist[loc].freq = new long double* [this->data.nsample];
            for (int samp=0;samp<this->data.nsample;samp++) {
                this->locuslist[loc].freq[samp] = new long double [2];
				this->locuslist[loc].freq[samp][0] = 0.0;this->locuslist[loc].freq[samp][1] = 0.0;
                for (int i=0;i<this->locuslist[loc].ss[samp];i++){
                    if (this->locuslist[loc].haplosnp[samp][i] == g0) this->locuslist[loc].freq[samp][0] +=1.0;
				}
				this->locuslist[loc].freq[samp][1] = this->locuslist[loc].samplesize[samp]-this->locuslist[loc].freq[samp][0];
				this->locuslist[loc].freq[samp][0] /=this->locuslist[loc].samplesize[samp];
				this->locuslist[loc].freq[samp][1] /=this->locuslist[loc].samplesize[samp];
				//printf("loc %3d    sa %d     freq[0]=%10.6Lf\n",loc,samp,this->locuslist[loc].freq[samp][0]);
				
			}
		}
		//cout<<"calfreqsnp fin\n";
	}

    void liberefreq(int gr) {
        int loc,iloc;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            loc=this->grouplist[gr].loc[iloc];
            if (this->locuslist[loc].groupe>0){
                if (this->locuslist[loc].type<5) {
                    for (int samp=0;samp<this->data.nsample;samp++) delete []this->locuslist[loc].freq[samp];
                    delete []this->locuslist[loc].freq;
                    for (int samp=0;samp<this->data.nsample;samp++) delete []this->locuslist[loc].haplomic[samp];
                    delete []this->locuslist[loc].haplomic;
                    delete []this->locuslist[loc].samplesize;
                } else if(this->locuslist[loc].type>9) {
                    for (int samp=0;samp<this->data.nsample;samp++) delete []this->locuslist[loc].freq[samp];
                    delete []this->locuslist[loc].freq;
                    for (int samp=0;samp<this->data.nsample;samp++) delete []this->locuslist[loc].haplosnp[samp];
                    delete []this->locuslist[loc].haplosnp;
                    delete []this->locuslist[loc].samplesize;
				}
            }
        }
    }

    void liberefreqsnp(int gr) {
        int loc,iloc;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            loc=this->grouplist[gr].loc[iloc];
            if (this->locuslist[loc].groupe>0){
                if (this->locuslist[loc].type<5) {
                    for (int samp=0;samp<this->data.nsample;samp++) delete []this->locuslist[loc].freq[samp];
                    delete []this->locuslist[loc].freq;
                    for (int samp=0;samp<this->data.nsample;samp++) delete []this->locuslist[loc].haplomic[samp];
                    delete []this->locuslist[loc].haplomic;
                    delete []this->locuslist[loc].samplesize;
                }
            }
        }
    }

    void libere_freq(int gr) {
        int kloc,iloc;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            for (int samp=0;samp<this->data.nsample;samp++) delete []this->locuslist[kloc].freq[samp];
            delete []this->locuslist[kloc].freq;
            for (int samp=0;samp<this->data.nsample;samp++) delete []this->locuslist[kloc].haplomic[samp];
            delete []this->locuslist[kloc].haplomic;
        }
    }

    void liberednavar(int gr) {
        int kloc,iloc;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            for (int samp=0;samp<this->data.nsample;samp++) {
                for (int ind=0;ind<this->locuslist[kloc].ss[samp];ind++) {
                  //cout<<"avant delete this->locuslist["<<kloc<<"].haplodnavar["<<samp<<"]["<<ind<<"]\n";
                  //delete []this->locuslist[kloc].haplodnavar[samp][ind];
                  this->locuslist[kloc].haplodnavar[samp][ind].clear();
                  this->locuslist[kloc].haplodna[samp][ind].clear();
                }
                delete []this->locuslist[kloc].haplodnavar[samp];
                delete []this->locuslist[kloc].haplodna[samp];
            }
            delete []this->locuslist[kloc].haplodnavar;
            delete []this->locuslist[kloc].haplodna;
            delete []this->locuslist[kloc].samplesize;
        }
    }

	void cal_snhet(int gr,int numsnp) {
		long double *het;
		int iloc,kloc,nl=0;
		int sample=this->grouplist[gr].sumstatsnp[numsnp].samp-1;
		het = new long double[this->grouplist[gr].nloc];
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			kloc=this->grouplist[gr].loc[iloc];
			if (this->locuslist[kloc].samplesize[sample]>1){
				het[nl]=1.0;
				for (int k=0;k<2;k++) het[nl] -= sqr(this->locuslist[kloc].freq[sample][k]);
				het[nl] *= ((long double)this->locuslist[kloc].samplesize[sample]/((long double)this->locuslist[kloc].samplesize[sample]-1));
				nl++;
			}
		}
		this->grouplist[gr].sumstatsnp[numsnp].n = nl;
		this->grouplist[gr].sumstatsnp[numsnp].x =new long double[nl];
		for (int loc=0;loc<nl;loc++) this->grouplist[gr].sumstatsnp[numsnp].x[loc] = het[loc];
		this->grouplist[gr].sumstatsnp[numsnp].defined=true;
		delete []het;
	}

	void cal_snnei(int gr,int numsnp) {
		//cout<<"debu de cal_snnei\n";
		long double *nei,fi,fj,gi,gj;
		int iloc,loc,nl=0,n1,n2;
		int sample=this->grouplist[gr].sumstatsnp[numsnp].samp-1;
		int sample1=this->grouplist[gr].sumstatsnp[numsnp].samp1-1;
		//cout<<"numsnp="<<numsnp<<"sample = "<<sample<<"    sample1 = "<<sample1<<"\n";
		nei = new long double[this->grouplist[gr].nloc];
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			loc=this->grouplist[gr].loc[iloc];
			n1=this->locuslist[loc].samplesize[sample];n2=this->locuslist[loc].samplesize[sample1];
			if ((n1>0)and(n2>0)) {
				fi=this->locuslist[loc].freq[sample][0];fj=this->locuslist[loc].freq[sample1][0];
				gi=this->locuslist[loc].freq[sample][1];gj=this->locuslist[loc].freq[sample1][1];
				nei[nl] = 1.0- (fi*fj + gi*gj)/sqrt(fi*fi + gi*gi)/sqrt(fj*fj + gj*gj);
				//cout<<"nei["<<nl<<"] = "<<nei[nl]<<"\n";
				nl++;
			}
		}
		this->grouplist[gr].sumstatsnp[numsnp].n = nl;
		this->grouplist[gr].sumstatsnp[numsnp].x =new long double[nl];
		for (int loc=0;loc<nl;loc++) this->grouplist[gr].sumstatsnp[numsnp].x[loc] = nei[loc];
		this->grouplist[gr].sumstatsnp[numsnp].defined=true;
		delete []nei;
	}

	void cal_snfst(int gr,int numsnp) {
		//cout<<"debut de cal_snfst\n";
		long double *fst,fi,fj,n1,n2;
		long double sniA,sniAA,sni,sni2,s2A,s1l,s3l,nc,MSI,MSP,s2I,s2P;
		int iloc,loc,nl=0;
		int sample=this->grouplist[gr].sumstatsnp[numsnp].samp-1;
		int sample1=this->grouplist[gr].sumstatsnp[numsnp].samp1-1;
		fst = new long double[this->grouplist[gr].nloc];
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			loc=this->grouplist[gr].loc[iloc];
			n1=(long double)this->locuslist[loc].samplesize[sample];n2=(long double)this->locuslist[loc].samplesize[sample1];
			if ((n1>0)and(n2>0)) {
			    s1l = 0.0;s3l = 0.0;
				for (int all=0;all<2;all++) {
					fi=this->locuslist[loc].freq[sample][all];
					fj=this->locuslist[loc].freq[sample1][all];
					sniAA = fi*n1+fj*n2;
					sniA  = 2.0*sniAA;
					sni = n1+n2;
					sni2 = n1*n1 + n2*n2;
					s2A = 2.0*(n1*fi*fi + n2*fj*fj);
					nc = (sni-sni2/sni);
					MSI = (0.5*sniA+sniAA-s2A)/(sni-2.0);
					MSP = (s2A - 0.5*sniA*sniA/sni);
					s2I = 0.5*MSI;
					s2P = (MSP-MSI)/2.0/nc;
					s1l += s2P;
					s3l += s2P + s2I;
				}
				fst[nl]=0.0;
				if ((s3l>0.0)and(s1l>0.0)) fst[nl]=s1l/s3l;
				if(fst[nl]<0.0) fst[nl]=0.0;
				nl++;
				//printf("s1l=%10.5Lf   s3l=%10.5Lf   fst[%3d] = %10.5Lf\n", s1l,s3l,nl,fst[nl-1]);
			}
		}
		this->grouplist[gr].sumstatsnp[numsnp].n = nl;
		this->grouplist[gr].sumstatsnp[numsnp].x =new long double[nl];
		for (int loc=0;loc<nl;loc++) this->grouplist[gr].sumstatsnp[numsnp].x[loc] = fst[loc];
		this->grouplist[gr].sumstatsnp[numsnp].defined=true;
		delete []fst;
	}

	complex<long double> pente_liksnp(int gr, int st, int i0) {
		long double a,freq1,freq2,li0,delta,*li;
		int ig1,ig2,ind,loc,iloc,nn;
		li = new long double[2];
		StatC stat=this->grouplist[gr].sumstat[st];
		int sample=stat.samp-1;
		int sample1=stat.samp1-1;
		int sample2=stat.samp2-1;
		for (int rep=0;rep<2;rep++) {
			a=0.001*(long double)(i0+rep);li[rep]=0.0;
			for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
				loc=this->grouplist[gr].loc[iloc];
			    ind=0;
			    for (int i=0;i<this->locuslist[loc].ss[sample];) {
			    	nn = calploidy(loc,sample,ind);
				    ind++;
	    			switch (nn)
	    			{ case 1 :  ig1 = this->locuslist[loc].haplosnp[sample][i];i++;
								if (ig1!=MICMISSING) {
									freq1 = a*this->locuslist[loc].freq[sample1][ig1]+(1.0-a)*this->locuslist[loc].freq[sample2][ig1];
									if (freq1>0.0) li[rep] +=log(freq1);
								}
								break;
	    			  case 2 :  ig1 = this->locuslist[loc].haplosnp[sample][i];i++;
								ig2 = this->locuslist[loc].haplomic[sample][i];i++;
				                if ((ig1!=MICMISSING)and(ig2!=MICMISSING)) {
									if (ig1==ig2) {
				                		freq1 = a*this->locuslist[loc].freq[sample1][ig1]+(1.0-a)*this->locuslist[loc].freq[sample2][ig1];
				                		if (freq1>0.0) li[rep] +=log(sqr(freq1));
				                	}
				                	else {
				                		freq1 = a*this->locuslist[loc].freq[sample1][ig1]+(1.0-a)*this->locuslist[loc].freq[sample2][ig1];
										freq2 = a*this->locuslist[loc].freq[sample1][ig2]+(1.0-a)*this->locuslist[loc].freq[sample2][ig2];
				                		if (freq1*freq2>0.0) li[rep] +=log(2.0*freq1*freq2);
				                		else {
				                			if (freq1>0.0) li[rep] +=log(freq1);
				                			if (freq2>0.0) li[rep] +=log(freq2);
				                		}
				                	}
				                }
				                break;
	    			}
    			}
			}
		}
		li0=li[0];
		delta=li[1]-li[0];
		delete []li;
		return complex<long double>(li0,delta);
	}

/*	long double cal_snaml(int gr,int st){
		int i1=1,i2=998,i3;
		long double p1,p2,p3,lik1,lik2,lik3;
		complex<long double> c;
		c=pente_lik(gr,st,i1);lik1=real(c);p1=imag(c);
		c=pente_lik(gr,st,i2);lik2=real(c);p2=imag(c);
		if ((p1<0.0)and(p2<0.0)) return 0.0;
		if ((p1>0.0)and(p2>0.0)) return 1.0;
		do {
			i3 = (i1+i2)/2;
			c=pente_liksnp(gr,st,i3);lik3=real(c);p3=imag(c);
			if (p1*p3<0.0) {i2=i3;p2=p3;lik2=lik3;}
			else		   {i1=i3;p1=p3;lik1=lik3;}
		} while (abs(i2-i1)>1);
		if (lik1>lik2) return 0.001*(long double)i1; else return 0.001*(long double)i2;
	}*/
	
	void cal_snaml(int gr,int numsnp) {
		long double *aml,f1,f2,f3,n1,n2,Dcra,U,V,N,M,*rangx,*rangy;
		int iloc,loc,nl=0;
		
		int sample=this->grouplist[gr].sumstatsnp[numsnp].samp-1;
		int sample1=this->grouplist[gr].sumstatsnp[numsnp].samp1-1;
		int sample2=this->grouplist[gr].sumstatsnp[numsnp].samp2-1;
		aml = new long double[this->grouplist[gr].nloc];
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			loc=this->grouplist[gr].loc[iloc];
			n1=(long double)this->locuslist[loc].samplesize[sample];n2=(long double)this->locuslist[loc].samplesize[sample1];
			if ((n1>0)and(n2>0)) {
				f1=this->locuslist[loc].freq[sample1][0];
				f2=this->locuslist[loc].freq[sample2][0];
				f3=this->locuslist[loc].freq[sample][0];
				if (((f1<f2)and(f3<f2))or((f1>f2)and(f3>f2))) {
					aml[nl] = (f3-f2)/(f1-f2);
					if (aml[nl]>1.0) aml[nl]=1.0;
					nl++;
				}
			}
		}
		this->grouplist[gr].sumstatsnp[numsnp].n = nl;
		this->grouplist[gr].sumstatsnp[numsnp].x =new long double[nl];
		for (int loc=0;loc<nl;loc++) this->grouplist[gr].sumstatsnp[numsnp].x[loc] = aml[loc];
		this->grouplist[gr].sumstatsnp[numsnp].defined=true;
		delete []aml;
	}
	


	long double cal_pid1p(int gr,int st){
        int iloc,kloc,nt=0,ni=0;
        int sample=this->grouplist[gr].sumstat[st].samp-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            if(this->locuslist[kloc].samplesize[sample]>1) {
                for (int i=0;i<this->locuslist[kloc].ss[sample]-1;i++){
                    if (this->locuslist[kloc].haplomic[sample][i] != MICMISSING) {
                        for (int j=i+1;j<this->locuslist[kloc].ss[sample];j++){
                            if (this->locuslist[kloc].haplomic[sample][j] != MICMISSING) {
                                nt++;
                                if (this->locuslist[kloc].haplomic[sample][i]==this->locuslist[kloc].haplomic[sample][j]) ni++;
                            }
                        }
                    }
                }
            }
        }
        //cout<<ni<<" sur "<<nt<<"\n";
        if (nt>0) return (long double)ni/(long double)nt;
		else return 0.0;
    }

	long double cal_nal1p(int gr,int st){

		long double nalm=0.0;
		int iloc,kloc,nl=0;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
		//cout <<"groupe "<<gr<<"  cat "<<this->grouplist[gr].sumstat[st].cat<<"   sample "<<this->grouplist[gr].sumstat[st].samp;//<<"\n";
		//cout << "     nloc = " << this->grouplist[gr].nloc;// <<"\n";
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			kloc=this->grouplist[gr].loc[iloc];
			//cout << this->locuslist[loc].samplesize[stat.samp] <<"\n";
			if(this->locuslist[kloc].samplesize[sample]>0) {
                for (int k=0;k<this->locuslist[kloc].nal;k++) {if (this->locuslist[kloc].freq[sample][k]>0.00001) nalm +=1.0;}
                nl++;
			}
		}
		//cout <<"    naltot="<<nalm<<"    nl="<<nl;//<<"    nmoy="<<  "\n";
		if (nl>0) nalm=nalm/(long double)nl;
		//cout<<"    nmoy="<<nalm<<  "\n";
		return nalm;
	}

	long double cal_nal2p(int gr,int st){
		int k,iloc,kloc,nl=0;
		long double nalm=0.0;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
		int sample1=this->grouplist[gr].sumstat[st].samp1-1;
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			kloc=this->grouplist[gr].loc[iloc];
			if((this->locuslist[kloc].samplesize[sample]>0)and(this->locuslist[kloc].samplesize[sample1]>0)) {
				for (int k=0;k<this->locuslist[kloc].nal;k++) {
					if (this->locuslist[kloc].freq[sample][k]+this->locuslist[kloc].freq[sample1][k]>0.000001) nalm+=1.0;
				}
				nl++;
			}
			else {
				if (this->locuslist[kloc].samplesize[sample]>0) {
					for (k=0;k<this->locuslist[kloc].nal;k++) {
						if (this->locuslist[kloc].freq[sample][k]>0.000001) nalm+=1.0;
					}
					nl++;
				}
				else {
					if (this->locuslist[kloc].samplesize[sample1]>0) {
						for (k=0;k<this->locuslist[kloc].nal;k++) {
							if (this->locuslist[kloc].freq[sample1][k]>0.000001) nalm+=1.0;
						}
						nl++;
					}
				}
			}
		}
		if (nl>0) nalm=nalm/(long double)nl;
		return nalm;
	}

	long double cal_het1p(int gr,int st){
		long double het,hetm=0.0;
		int iloc,kloc,nl=0;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			kloc=this->grouplist[gr].loc[iloc];
			if (this->locuslist[kloc].samplesize[sample]>1){

				het=1.0;
				for (int k=0;k<this->locuslist[kloc].nal;k++) het -= sqr(this->locuslist[kloc].freq[sample][k]);
				het *= ((long double)this->locuslist[kloc].samplesize[sample]/((long double)this->locuslist[kloc].samplesize[sample]-1));
				hetm += het;
				//printf("kloc=%2d   het = %13.10Lf\n",kloc,het);
				nl++;
			}
		}
		if (nl>0) hetm=hetm/(long double)nl;
		//printf("\n heterozygotie = %13.10Lf\n\n",hetm);
		return hetm;
	}

	long double cal_het2p(int gr,int st){
		long double het,hetm=0.0;
		int iloc,loc,n1,n2,nt,nl=0;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
		int sample1=this->grouplist[gr].sumstat[st].samp1-1;
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			loc=this->grouplist[gr].loc[iloc];
			n1=this->locuslist[loc].samplesize[sample];n2=this->locuslist[loc].samplesize[sample1];nt=n1+n2;
			if (nt>1){
				het=1.0;
				for (int k=0;k<this->locuslist[loc].nal;k++) {het -= sqr(((long double)n1*this->locuslist[loc].freq[sample][k]+(long double)n2*this->locuslist[loc].freq[sample1][k])/(long double)nt);}
				het *= (long double)nt/(long double)(nt-1);
				hetm +=het;
				nl++;
			}
		}
		if (nl>0) return hetm/(long double)nl; else return 0.0;
	}

	long double cal_var1p(int gr,int st){
		long double s,v,vm=0.0,m,ms;
		int iloc,loc,n,nl=0;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
			for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
				loc=this->grouplist[gr].loc[iloc];
			  v=0.0;s=0.0;n=0;
			  for (int i=0;i<this->locuslist[loc].ss[sample];i++){
				  if (this->locuslist[loc].haplomic[sample][i] != MICMISSING) {
					  n++;
					  s+=this->locuslist[loc].haplomic[sample][i];
					  v+=sqr(this->locuslist[loc].haplomic[sample][i]);
				  }
			  }
			  m = (long double)n;
			  ms = (long double)this->locuslist[loc].motif_size;
			  if (n>1){
				  vm+=(v-sqr(s)/m)/(m-1.0)/sqr(ms);
				  nl++;
			  }
			  //cout<<"var["<<sample<<"]["<<loc<<"]="<<(v-sqr(s)/m)/(m-1.0)/sqr(ms)<<"   v="<<v<<"   s="<<s<<"   m="<<m<<"\n";
		  }
		  //printf("\nvm=%13.10Lf\n",vm/(long double)nl);
		  if (nl>0) return vm/(long double)nl; else return 0.0;
	}

	long double cal_var2p(int gr,int st){
		long double s,v,vm=0.0,m,ms;
		int iloc,loc,n,nl=0;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
		int sample1=this->grouplist[gr].sumstat[st].samp1-1;
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			loc=this->grouplist[gr].loc[iloc];
			v=0.0;s=0.0;n=0;
			for (int i=0;i<this->locuslist[loc].ss[sample];i++){
				if (this->locuslist[loc].haplomic[sample][i] != MICMISSING) {
					n++;
					s+=this->locuslist[loc].haplomic[sample][i];
					v+=sqr(this->locuslist[loc].haplomic[sample][i]);
				}
			}
			for (int i=0;i<this->locuslist[loc].ss[sample1];i++){
				if (this->locuslist[loc].haplomic[sample1][i] != MICMISSING) {
					n++;
					s+=this->locuslist[loc].haplomic[sample1][i];
					v+=sqr(this->locuslist[loc].haplomic[sample1][i]);
				}
			}
			m = (long double)n;
			ms = (long double)this->locuslist[loc].motif_size;
			if (n>1){
				vm+=(v-sqr(s)/m)/(m-1.0)/sqr(ms);
				nl++;
			}
		}
		if (nl>0) return vm/(long double)nl; else return 0.0;
	}

	long double cal_mgw1p(int gr,int st){
		long double num=0.0,den=0.0;
		int min,max;
		int iloc,loc;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			loc=this->grouplist[gr].loc[iloc];
		    if (this->locuslist[loc].samplesize[sample]>0) {
				for (int k=0;k<this->locuslist[loc].nal;k++) {
					if (this->locuslist[loc].freq[sample][k]>0.00001) num +=1.0;}

				min=0;while (this->locuslist[loc].freq[sample][min]<0.00001) min++;
				max=this->locuslist[loc].kmax-this->locuslist[loc].kmin;while (this->locuslist[loc].freq[sample][max]<0.00001) max--;
				den += (long double)(1+max-min)/(long double)this->locuslist[loc].motif_size;
		    }
		}
		if (den>0) return num/den; else return 0.0;
	}

	long double cal_Fst2p(int gr,int st){
		long double s1=0.0,s3=0.0,s1l,s2l,s3l,sni,sni2,sniA,sniAA;
		int al,pop,kpop,i,ig1,ig2,nA,AA,ni;
		long double s2A,MSG,MSI,MSP,s2G,s2I,s2P,nc;
		int iloc,loc,nn,ind;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
		int sample1=this->grouplist[gr].sumstat[st].samp1-1;
		//cout << "cal_Fst2p\n";
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			loc=this->grouplist[gr].loc[iloc];
			//cout << "Fst loc = "<<loc<<"\n";
		    s1l=0.0;s2l=0.0;s3l=0.0;nc=0;
//		    cout << "\n\nlocus "<< loc << "\n";
		    for (al=this->locuslist[loc].kmin;al<=this->locuslist[loc].kmax;al++) {
//		    	cout << "\n allele "<< al << "\n";
		    	sni=0;sni2=0;sniA=0;sniAA=0;s2A=0.0;
		    	for (kpop=0;kpop<2;kpop++) {
		    		if (kpop==0) pop=sample; else pop=sample1;
		    		nA=0;AA=0;ni=0;ind=0;
//ind est le numéro de l'individu (haploïde ou diploïde ou même sans génotype si Y femelle)
//i est le numéro de la copie du gène
		    		for (i=0;i<this->locuslist[loc].ss[pop];){
		    			nn=calploidy(loc,pop,ind);
						ind++;
		    			switch (nn)
		    			{ case 1 :  ig1 = this->locuslist[loc].haplomic[pop][i];i++;
									if (ig1==al) {AA++;nA+=2;}
									if (ig1!=MICMISSING) ni++;

									break;
		    			  case 2 :  ig1 = this->locuslist[loc].haplomic[pop][i];i++;
									ig2 = this->locuslist[loc].haplomic[pop][i];i++;
					                if ((ig1==al) and (ig2==al)) AA++;
					                if (ig1==al) nA++;
					                if (ig2==al) nA++;
					                if ((ig1!=MICMISSING)and(ig2!=MICMISSING)) ni++;
					                break;
		    			}
		    		}
		            sniA  +=(long double)nA;
		            sniAA +=(long double)AA;
		            sni    +=(long double)ni;

		            sni2  +=sqr((long double)ni);
		            s2A   +=sqr((long double)nA)/(long double)(2*ni);
		    	}
		    	if (sni>0) nc=sni-sni2/sni; else nc=0.0;
//		    	cout << "Allele " << al <<"   sni = " << sni << "   sni2 = "<< sni2 << "   nc = "<< nc;
//		    	cout << "   sniA = "<< sniA << "   sniAA = " << sniAA<< "   s2A = " << s2A << "\n";
		    	if (sni*nc>0.0) {
		            MSG=(0.5*sniA-sniAA)/sni;
		            MSI=(0.5*sniA+sniAA-s2A)/(sni-2.0);
		            MSP=(s2A-0.5*sqr(sniA)/sni);
		            s2G=MSG;s2I=0.5*(MSI-MSG);s2P=(MSP-MSI)/(2.0*nc);
		            s1l=s1l+s2P;         //numerateur de theta
		            s2l=s2l+s2P+s2I;     //numerateur de F
		            s3l=s3l+s2P+s2I+s2G; //denominateur de theta et de F
		    	}
		    }
//            cout << "s1l = "<< s1l<<"   s2l = "<<s2l<<"   s3l = "<< s3l << "\n";
		    s1 += (long double)nc*s1l;
		    s3 += (long double)nc*s3l;
		}
		if (s3>0.0) return s1/s3; else return 0.0;
	}

	long double cal_lik2p(int gr,int st){
  		int pop,nal,k,i,ind,ig1,ig2;
  		long double frt,a,b,num_lik,den_lik,lik;
		int iloc,loc,nn,nl=0;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
		int sample1=this->grouplist[gr].sumstat[st].samp1-1;
		lik = 0.0;
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			loc=this->grouplist[gr].loc[iloc];
		    if (this->locuslist[loc].samplesize[sample]*this->locuslist[loc].samplesize[sample1]>0) {
				nl++;
				nal = 0;
				for (k=0;k<this->locuslist[loc].nal;k++) {
					frt=0.0;
					for (pop=0;pop<this->data.nsample;pop++) frt += this->locuslist[loc].freq[pop][k];
					if (frt>0.000001) nal++;
				}
				b = 1.0/(long double)nal;
				a = 1.0/(long double)this->data.nind[sample];
				ind = 0;
			    for (i=0;i<this->locuslist[loc].ss[sample];i++){
					nn=calploidy(loc,sample,ind);
					ind++;
	    			switch (nn)
	    			{ case 1 :  ig1 = this->locuslist[loc].haplomic[sample][i];
								if (ig1!=MICMISSING) {
									num_lik = round(this->locuslist[loc].freq[sample1][ig1-this->locuslist[loc].kmin]*this->locuslist[loc].samplesize[sample1])+b;
									den_lik = (long double) (this->locuslist[loc].samplesize[sample1]+1);
									lik -= a*log10(num_lik/den_lik);
								}
								break;
	    			  case 2 :  ig1 = this->locuslist[loc].haplomic[sample][i];
								ig2 = this->locuslist[loc].haplomic[sample][++i];
				                if ((ig1!=MICMISSING)and(ig2!=MICMISSING)) {
				                	if (ig1==ig2) {
				                		num_lik = 1.0+b+arrondi(this->locuslist[loc].freq[sample1][ig1-this->locuslist[loc].kmin]*this->locuslist[loc].samplesize[sample1]);
				                		num_lik *=b+arrondi(this->locuslist[loc].freq[sample1][ig2-this->locuslist[loc].kmin]*this->locuslist[loc].samplesize[sample1]);
				                	}
				                	else {
				                		num_lik = 2.0*(b+arrondi(this->locuslist[loc].freq[sample1][ig1-this->locuslist[loc].kmin]*this->locuslist[loc].samplesize[sample1]));
				                		num_lik *=b+arrondi(this->locuslist[loc].freq[sample1][ig2-this->locuslist[loc].kmin]*this->locuslist[loc].samplesize[sample1]);
				                	}
				                	den_lik = (long double) ((this->locuslist[loc].samplesize[sample1]+2)*(this->locuslist[loc].samplesize[sample1]+1));
									lik -= a*log10(num_lik/den_lik);
				                }
				                break;
	    			}
    			}
		    }
		}
		if (nl>0) return lik/(long double)nl; else return 0.0;
	}

	long double cal_dmu2p(int gr,int st){
		long double *moy,dmu2=0.0,s;
		int iloc,loc,pop,nl=0;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
		int sample1=this->grouplist[gr].sumstat[st].samp1-1;
		moy = new long double[2];
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			loc=this->grouplist[gr].loc[iloc];
		    if (this->locuslist[loc].samplesize[sample]*this->locuslist[loc].samplesize[sample1]>0) {
				nl++;
				for (int kpop=0;kpop<2;kpop++) {
					if (kpop==0) pop=sample; else pop=sample1;
					s = 0.0;moy[kpop]=0.0;
					for (int i=0;i<this->locuslist[loc].ss[pop];i++) {
						if (this->locuslist[loc].haplomic[pop][i]!=MICMISSING) s += this->locuslist[loc].haplomic[pop][i];
					}
					moy[kpop]=s/(long double)this->locuslist[loc].samplesize[pop];
				}
		    }
		    dmu2 += sqr((moy[1]-moy[0])/(long double)this->locuslist[loc].motif_size);
		}
		delete []moy;
		if (nl>0) return dmu2/(long double)nl; else return 0.0;
	}

	long double cal_das2p(int gr,int st){
		long double s=0.0;
		int iloc,loc,nl=0;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
		int sample1=this->grouplist[gr].sumstat[st].samp1-1;
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			loc=this->grouplist[gr].loc[iloc];
			for (int i=0;i<this->locuslist[loc].ss[sample];i++) {
				if (this->locuslist[loc].haplomic[sample][i]!=MICMISSING) {
					for (int j=0;j<this->locuslist[loc].ss[sample1];j++) {
						if (this->locuslist[loc].haplomic[sample1][j]!=MICMISSING) {
							nl++;
							if (this->locuslist[loc].haplomic[sample][i] == this->locuslist[loc].haplomic[sample1][j]) s+=1.0;
						}
					}
				}
			}
		}
		if (nl>0) return s/(long double)nl; else return 0.0;
	}

	complex<long double> pente_lik(int gr,int st, int i0) {
		long double a,freq1,freq2,li0,delta,*li;
		int ig1,ig2,ind,loc,iloc,nn;
		li = new long double[2];
		StatC stat=this->grouplist[gr].sumstat[st];
		int sample=stat.samp-1;
		int sample1=stat.samp1-1;
		int sample2=stat.samp2-1;
		//cout<<"sample ="<<sample<<"   sample1="<<sample1<<"   sample2="<<sample2<<"\n";
		for (int rep=0;rep<2;rep++) {
			a=0.001*(long double)(i0+rep);li[rep]=0.0;
			//cout << "rep = " << rep << "   a = "<< a << "  lik = "<< li[rep] << "\n";
			for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
				loc=this->grouplist[gr].loc[iloc];
			    //cout << "\n\n locus "<< loc<< "\n";
			    ind=0;
			    for (int i=0;i<this->locuslist[loc].ss[sample];) {
			    	nn = calploidy(loc,sample,ind);
				    //cout <<"ploidie="<<nn<<"\n";
				    ind++;
	    			switch (nn)
	    			{ case 1 :  ig1 = this->locuslist[loc].haplomic[sample][i];i++;
								if (ig1!=MICMISSING) {
									freq1 = a*this->locuslist[loc].freq[sample1][ig1-this->locuslist[loc].kmin]+(1.0-a)*this->locuslist[loc].freq[sample2][ig1-this->locuslist[loc].kmin];
									if (freq1>0.0) li[rep] +=log(freq1);
								}
								break;
	    			  case 2 :  ig1 = this->locuslist[loc].haplomic[sample][i];i++;
					        ig2 = this->locuslist[loc].haplomic[sample][i];i++;
				                if ((ig1!=MICMISSING)and(ig2!=MICMISSING)) {
				                	 //cout<<"ig1="<<ig1<<"    ig2="<<ig2<<"   kmin="<<this->locuslist[loc].kmin<<"\n";
					 //cout<<"freq[samp1][ig1]="<<this->locuslist[loc].freq[sample1][ig1-this->locuslist[loc].kmin]<<"\n";
					 //cout<<"freq[samp2][ig1]="<<this->locuslist[loc].freq[sample2][ig1-this->locuslist[loc].kmin]<<"\n";
					 //cout<<"freq[samp1][ig2]="<<this->locuslist[loc].freq[sample1][ig2-this->locuslist[loc].kmin]<<"\n";
					 //cout<<"freq[samp2][ig2]="<<this->locuslist[loc].freq[sample2][ig2-this->locuslist[loc].kmin]<<"\n";
									if (ig1==ig2) {
				                		freq1 = a*this->locuslist[loc].freq[sample1][ig1-this->locuslist[loc].kmin]+(1.0-a)*this->locuslist[loc].freq[sample2][ig1-this->locuslist[loc].kmin];
				                		if (freq1>0.0) li[rep] +=log(sqr(freq1));
				                	}
				                	else {
				                		freq1 = a*this->locuslist[loc].freq[sample1][ig1-this->locuslist[loc].kmin]+(1.0-a)*this->locuslist[loc].freq[sample2][ig1-this->locuslist[loc].kmin];
				                		//cout<<"freq1 = "<<freq1<<"\n";
								freq2 = a*this->locuslist[loc].freq[sample1][ig2-this->locuslist[loc].kmin]+(1.0-a)*this->locuslist[loc].freq[sample2][ig2-this->locuslist[loc].kmin];
				                		//cout<<"freq2 = "<<freq2<<"\n";
				                		if (freq1*freq2>0.0) li[rep] +=log(2.0*freq1*freq2);
				                		else {
				                			if (freq1>0.0) li[rep] +=log(freq1);
				                			if (freq2>0.0) li[rep] +=log(freq2);
				                		}
				                	}
				                }
				               // cout<<"coucou\n";
				                break;
	    			}
	    			//cout << "lik = " << li[rep] << "\n";
    			}
			}
			//cout << "rep = " << rep << "   a = "<< a << "lik = "<< li[rep] << "\n";
		}
		//cout << "i0 = " <<i0 << "   li[i0] = "<< li[0] << "\n";
		li0=li[0];
		delta=li[1]-li[0];
		delete []li;
		return complex<long double>(li0,delta);
	}

	long double cal_Aml3p(int gr,int st){
		int i1=1,i2=998,i3;
		long double p1,p2,p3,lik1,lik2,lik3;
		complex<long double> c;
		c=pente_lik(gr,st,i1);lik1=real(c);p1=imag(c);
		c=pente_lik(gr,st,i2);lik2=real(c);p2=imag(c);
		if ((p1<0.0)and(p2<0.0)) return 0.0;
		if ((p1>0.0)and(p2>0.0)) return 1.0;
		do {
			i3 = (i1+i2)/2;
			c=pente_lik(gr,st,i3);lik3=real(c);p3=imag(c);
			if (p1*p3<0.0) {i2=i3;p2=p3;lik2=lik3;}
			else		   {i1=i3;p1=p3;lik1=lik3;}
		} while (abs(i2-i1)>1);
		if (lik1>lik2) return 0.001*(long double)i1; else return 0.001*(long double)i2;
	}

	bool identseq(string seq1, string seq2,string appel,int kloc,int sample,int ind) {
	    bool ident=true;
	    int k=0;
        if (seq1.length()!=seq2.length()) {
            cout<<"Erreur au locus "<<kloc<<" appelé par "<<appel;
            cout<<"     seq1="<<seq1<<"<("<<seq1.length()<<")   seq2="<<seq2<<"<("<<seq2.length()<<")\n";
            cout<<"dnavar = "<<this->locuslist[kloc].dnavar<<"   sample="<<sample<<"   ind="<<ind<<"\n";
            exit(1);
        }
        if (seq1.length()==0) return ident;
        //cout<<seq1<<"    "<<seq2<<"\n";
	    while ((ident)and(k<seq1.length())) {
	        ident = ((seq1[k]=='N')or(seq2[k]=='N')or(seq1[k]==seq2[k]));
	        if (ident) k++;
	    }
	    //if (ident) cout<<"ident\n"; else cout<<"different\n";
	    return ident;
	}

	long double cal_nha1p(int gr,int st){
	    string *haplo;
	    int iloc,kloc,j,nhl=0,nhm=0,nl=0;
	    bool trouve;
		long double res=0.0;
        int sample=this->grouplist[gr].sumstat[st].samp-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            nhl=0;
            kloc=this->grouplist[gr].loc[iloc];
            haplo = new string[this->locuslist[kloc].ss[sample]];
            if(this->locuslist[kloc].samplesize[sample]>0) {
                nl++;
                if (this->locuslist[kloc].dnavar==0) {
                    haplo[nhl]="";
                    nhl++;
                } else {
                    for (int i=0;i<this->locuslist[kloc].ss[sample];i++) {
                        if (this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING) {
                            //cout<<"not seqmissing nhl="<<nhl<<"\n";
                            if (nhl==0) {
                                haplo[nhl] = this->locuslist[kloc].haplodnavar[sample][i];
                                nhl++;
                               //cout<<"nl="<<nl<<"   nhl="<<nhl<<"    "<<haplo[nhl-1]<<"\n";
                            } else {
                                trouve=false;j=0;
                                while ((not trouve)and(j<nhl)) {
                                	trouve=identseq(haplo[j],this->locuslist[kloc].haplodnavar[sample][i],"nha1p",kloc,sample,i);
                                	if (not trouve) j++;
                                }
                                if (trouve) {
                                	for (int k=0;k<this->locuslist[kloc].dnavar;k++) if (haplo[j][k]=='N') haplo[nhl][k] = this->locuslist[kloc].haplodnavar[sample][i][k];
                                } else {
                                haplo[nhl] = this->locuslist[kloc].haplodnavar[sample][i];
                                nhl++;
                                }
                            }
                        }
                    }
                }
            }
            nhm += nhl;
            delete []haplo;
        }
        if (nl>0) res=(long double)nhm/(long double)nl;
        return res;
    }

    int* cal_nsspl(int kloc,int sample, int *nssl,bool *OK) {
        char c0;
        bool ident;
        int j,*ss,nss=0;
        vector <int> nuvar;
        if(this->locuslist[kloc].samplesize[sample]>0) {
            *OK=true;
            if((debuglevel==9)and(kloc==10)) cout<<"OK=true  dnavar="<<this->locuslist[kloc].dnavar<<"\n";
            if (this->locuslist[kloc].dnavar>0) {
                for (j=0;j<this->locuslist[kloc].dnavar;j++) {
                    if((debuglevel==9)and(kloc==10)) cout<<"j="<<j<<"  ss="<<this->locuslist[kloc].ss[sample]<<"\n";
                    c0='z';ident=true;
                    for (int i=0;i<this->locuslist[kloc].ss[sample];i++) {
                        /*if((debuglevel==9)and(kloc==10)) {
                              cout<<this->locuslist[kloc].haplodnavar[sample][i]<<"   "<<this->locuslist[kloc].haplodnavar[sample][i][j]<<"   "<<c0;
                              cout<<"  "<<(this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING);
                              cout<<"  "<<(this->locuslist[kloc].haplodnavar[sample][i][j]!='N');
                              cout<<"  "<<((this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING)and(this->locuslist[kloc].haplodnavar[sample][i][j]!='N'))<<"\n";
                        }*/
                        if ((this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING)and(this->locuslist[kloc].haplodnavar[sample][i][j]!='N')) {
                            if((debuglevel==9)and(kloc==10)) cout<<"c0="<<c0<<"\n";
                            if (c0=='z') c0=this->locuslist[kloc].haplodnavar[sample][i][j];
                            else ident=(c0==this->locuslist[kloc].haplodnavar[sample][i][j]);
                            if((debuglevel==9)and(kloc==10)) {if (not ident) cout<<"nucleotide "<<j<<"   c0="<<c0<<"   c1="<<this->locuslist[kloc].haplodnavar[sample][i][j]<<"   i="<<i<<"\n";}
                        }
                        if (not ident) break;
                    }
                    if (not ident) {nss++;nuvar.push_back(j);}
                }
            }
        } else *OK=false;
        ss = new int[nss];
        for (j=0;j<nss;j++) ss[j]=nuvar[j];
        if (not nuvar.empty()) nuvar.clear();
        *nssl=nss;
        //cout<<"locus "<<kloc<<"   nss="<<nss<<"\n";
        return ss;
    }

	long double cal_nss1p(int gr,int st){
        int iloc,kloc,nssl,nssm=0,nl=0,*ss;
		long double res=0.0;
        bool OK;
        int sample=this->grouplist[gr].sumstat[st].samp-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            ss = this->cal_nsspl(kloc,sample,&nssl,&OK);
            //cout<<"nss1p   nssl="<<nssl<<"\n";
            //for (int k=0;k<nssl;k++) cout<<"  "<<ss[k];if(nssl>0) cout<<"\n";
            if (OK) {nl++;nssm += nssl;}
            delete []ss;
        if((debuglevel==9)and(kloc==10)) {
          cout<<"\ndnavar ="<<this->locuslist[kloc].dnavar<<"\n";
            for (int sa=0;sa<this->nsample;sa++) {
                cout<<" dans cal_nss1p   sample "<<sa<<"\n";
                for (int ind=0;ind<this->locuslist[kloc].ss[sa];ind++) cout<<ind<<"  "<<this->locuslist[kloc].haplodna[sa][ind]<<"\n";
            }
            cout <<"nssl = "<<nssl<<"\n";
        }
        }
        if (nl>0) res=(long double)nssm/(long double)nl;
        if ((debuglevel==9)and(kloc==10)) {cout<<"nombre de sites segregeant = "<<res<<"\n"; if (not this->dnatrue) exit(1);}
 		return res;
	}

    long double cal_mpdpl(int kloc,int sample,int *nd) {
        int npdl=0,di,k,ndd=0;
        long double res=0.0;
        for (int i=0;i<this->locuslist[kloc].ss[sample]-1;i++) {
            for (int j=i+1;j<this->locuslist[kloc].ss[sample];j++) {
                if ((this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING)and(this->locuslist[kloc].haplodna[sample][j]!=SEQMISSING)) {
                    ndd++;
                    di=0;
                    for (k=0;k<this->locuslist[kloc].dnavar;k++) {
                        if ((this->locuslist[kloc].haplodnavar[sample][i][k]!='N')and(this->locuslist[kloc].haplodnavar[sample][j][k]!='N')and(this->locuslist[kloc].haplodnavar[sample][i][k]!=this->locuslist[kloc].haplodnavar[sample][j][k])) di++;
                    }
                    npdl +=di;
                }
            }
        }
        if (ndd>0) res=(long double)npdl/(long double)ndd;
        *nd=ndd;
        return res;
    }

	long double cal_mpd1p(int gr,int st){
        int iloc,kloc,npdl,nd=0,nl=0;
		long double npdm=0.0,res=0.0,mpdp;
        int sample=this->grouplist[gr].sumstat[st].samp-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            npdl=0;
            kloc=this->grouplist[gr].loc[iloc];
            mpdp=this->cal_mpdpl(kloc,sample,&nd);
            if (nd>0){npdm +=mpdp;nl++;}
        }
        if (nl>0) res=(long double)npdm/(long double)nl;
		return res;

	}

	long double cal_vpd1p(int gr,int st){
		int iloc,kloc,k,di,nl=0,nd;
		long double res=0.0,mpd,spd,vpd=0.0;
        int sample=this->grouplist[gr].sumstat[st].samp-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            nd=0;mpd=0.0;spd=0.0;
            kloc=this->grouplist[gr].loc[iloc];
            for (int i=0;i<this->locuslist[kloc].ss[sample]-1;i++) {
                for (int j=i+1;j<this->locuslist[kloc].ss[sample];j++) {
                    if ((this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING)and(this->locuslist[kloc].haplodna[sample][j]!=SEQMISSING)) {
                        nd++;
                        di=0;
                        for (k=0;k<this->locuslist[kloc].dnavar;k++) {
                            if ((this->locuslist[kloc].haplodnavar[sample][i][k]!='N')and(this->locuslist[kloc].haplodnavar[sample][j][k]!='N')and(this->locuslist[kloc].haplodnavar[sample][i][k]!=this->locuslist[kloc].haplodnavar[sample][j][k])) di++;
                        }
                        mpd +=(long double)di;
                        spd +=(long double)di*(long double)di;
                    }
                }
            }
            if (nd>1){spd =(spd -mpd*mpd/(long double)nd)/(long double)(nd-1);nl++;vpd +=spd;}
        }
        if (nl>0) res=vpd/(long double)nl;
		return res;

	}

	long double cal_dta1pl(int kloc,int sample,bool *OKK){
        long double a1,a2,b1,b2,c1,c2,e1,e2,S,pi;
        int nd,n=0,*ss,kS;
        bool OK;
        *OKK=true;
       if (this->locuslist[kloc].dnavar<1) return 0.0;
		long double res=0.0;
        for (int i=0;i<this->locuslist[kloc].ss[sample];i++) if (this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING) n++;
        if (n<2) {*OKK=false;return 0.0;}
        a1=0;for(int i=1;i<n;i++) a1 += 1.0/(long double)i;
        a2=0;for(int i=1;i<n;i++) a2 += 1.0/(long double)(i*i);
        b1=(long double)(n+1)/(long double)(n-1)/3.0;
        b2=2.0*((long double)(n*n+n)+3.0)/9.0/(long double)(n*n-n);
        c1=b1-1.0/a1;
        c2=b2-((long double)(n+2)/a1/(long double)n) + (a2/a1/a1);
        e1=c1/a1;
        e2=c2/(a1*a1+a2);
        pi=cal_mpdpl(kloc,sample,&nd);
        ss =cal_nsspl(kloc,sample,&kS,&OK);
        S = (long double)kS;
        if ((nd>0)and(OK)and(e1*S+e2*S*(S-1.0)>0.0)) res=(pi-S/a1)/sqrt(e1*S+e2*S*(S-1.0));
        //cout<<"a1="<<a1<<"  a2="<<a2<<"b1="<<b1<<"  b2="<<b2<<"c1="<<c1<<"  c2="<<c2<<"e1="<<e1<<"  e2="<<e2<<"\n";
        //cout<<"nd="<<nd<<"   OK="<<OK<<"   pi="<<pi<<"   S="<<S<<"   res="<<res<<"\n";
		return res;
	}

    long double cal_dta1p(int gr,int st) {
        int iloc,kloc,nl=0;
        long double res=0.0,tal;
        bool OK;
        int sample=this->grouplist[gr].sumstat[st].samp-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            tal=this->cal_dta1pl(kloc,sample,&OK);
            if (OK) {nl++;res +=tal;}
        }
        //cout<<"nl="<<nl<<"   res="<<res<<"\n";
        if(nl>0) res /=nl;
        return res;
    }

	long double cal_pss1p(int gr,int st){
        int iloc,kloc,nl=0,nps=0,**ssa,*nssa;
        bool trouve,OK;
		long double res=0.0;
        int sample=this->grouplist[gr].sumstat[st].samp-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            ssa = new int*[this->data.nsample];
            nssa = new int[this->data.nsample];
      //on cherche les sites variables des différents échantillons
            for (int sa=0;sa<this->data.nsample;sa++) ssa[sa]=this->cal_nsspl(kloc,sa,&nssa[sa],&OK);
            //cout<<"\nlocus "<<kloc<<"\n";
            for (int sa=0;sa<this->data.nsample;sa++) {
                //cout<<"sample "<<sa+1<<"   nssa="<<nssa[sa]<<"\n";
                //for (int k=0;k<nssa[sa];k++) cout<<"   "<<ssa[sa][k];
                //cout<<"\n";
            }
      //on compte le nombre de sites variables de l'échantillon cible qui ne sont pas variables dans les autres échantillons
            nl++;
            for (int j=0;j<nssa[sample];j++) {
                trouve=false;
                for (int sa=0;sa<this->data.nsample;sa++) {
                    if ((sa!=sample)and(nssa[sa]>0)) {
                        for (int k=0;k<nssa[sa];k++) {
                            trouve = (ssa[sample][j]==ssa[sa][k]);
                            if (trouve) break;
                        }
                    }
                    if (trouve) break;
              }
              if (not trouve) nps++;
            }
            for (int sa=0;sa<this->data.nsample;sa++) if (nssa[sa]>0) delete [] ssa[sa];
            delete []ssa;delete []nssa;
        }
        if (nl>0) res = (long double)nps/(long double)nl;
        //cout<<"PSS_"<<this->grouplist[gr].sumstat[st].samp<<" = "<<res<<"   (nps="<<nps<<" nl="<<nl<<")\n";
		return res;
	}

    void afs(int sample,int iloc,int kloc) {
        int nf[4],ii,jj;
        this->n_afs[sample][iloc]=0;
        //cout<<"sample "<<sample<<"  locus "<<kloc<<"   dnavar = "<<this->locuslist[kloc].dnavar <<"\n";
        if (this->locuslist[kloc].dnavar==0) this->n_afs[sample][iloc]=0;
        else {
            for (int j=0;j<this->locuslist[kloc].dnavar;j++) {
                //cout <<"site "<<j+1<<" sur "<<this->locuslist[kloc].dnavar<<"\n";
                for (int k=0;k<4;k++) nf[k]=0;
                for (int i=0;i<this->locuslist[kloc].ss[sample];i++) {
                    //if (this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING) {
                    //cout<<this->locuslist[kloc].haplodnavar[sample][i][j]<<"\n";
                        switch(this->locuslist[kloc].haplodnavar[sample][i][j])
                        { case 'A' :nf[0]++;break;
                          case 'C' :nf[1]++;break;
                          case 'G' :nf[2]++;break;
                          case 'T' :nf[3]++;break;
                        }
                    //}
                }
//tri des frequences dans l'ordre croissant
                for (int i=0;i<3;i++) {for (int k=i+1;k<4;k++) if (nf[i]>nf[k]) {ii=nf[i];nf[i]=nf[k];nf[k]=ii;}}
                //for (int i=0;i<4;i++) cout <<nf[i]<<"  ";cout<<"\n";
//recherche de la plus petite fréquence non nulle
                jj=0;while ((nf[jj]==0)and(jj<4)) jj++;
                if (jj<3) {
                    this->n_afs[sample][iloc]++;
                    this->t_afs[sample][iloc].push_back(nf[jj]);
                }
            }
        }
        //cout<<"sample "<<sample<<"  locus "<<iloc<<"   n_afs = "<<this->n_afs[sample][iloc]<<"\n";
        //if (this->n_afs[sample][iloc]>0) {for (int i=0;i<n_afs[sample][iloc];i++) cout<<"  "<<t_afs[sample][iloc][i];cout<<"\n";}
        this->afsdone[sample][iloc]=true;
    }

    long double cal_mns1p(int gr,int st){
        int iloc,kloc,nl=0;
		long double res=0.0;
        int sample=this->grouplist[gr].sumstat[st].samp-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            if (not this->afsdone[sample][iloc]) this->afs(sample,iloc,kloc);
            for (int i=0;i<this->n_afs[sample][iloc];i++) res += (long double)this->t_afs[sample][iloc][i]/(long double)this->n_afs[sample][iloc];
            nl++;
        }
        if (nl>0) res /=(long double)nl;
		return res;
	}

	long double cal_vns1p(int gr,int st){
        int iloc,kloc,nl=0;
		long double res=0.0,sx,sx2,a,v;
        int sample=this->grouplist[gr].sumstat[st].samp-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            //cout<<"Locus "<<kloc<<"\n";
            if (not afsdone[sample][iloc]) afs(sample,iloc,kloc);
            //cout<<"apres afs\n";
            if (n_afs[sample][iloc]>1) {
                sx=0.0;sx2=0.0;
                for (int i=0;i<n_afs[sample][iloc];i++) {
                  a = (long double)t_afs[sample][iloc][i];
                  sx += a;
                  sx2 +=a*a;
                }
                //cout<<"sample "<<sample<<"   locus "<<kloc<<"  sx="<<sx<<"  sx2="<<sx2<<"\n";
                a = (long double)n_afs[sample][iloc];
                v = (sx2-sx*sx/a)/a;
            } else {v=0.0;/*cout<<"vns="<<v<<"\n";*/}
            nl++;//cout<<"nl="<<nl<<"\n";
            //if (v>0.0) cout<<"sample "<<sample<<"   locus "<<kloc<<"  sx="<<sx<<"  sx2="<<sx2<<"  n="<<a<<"   vns="<<v<<"\n";
            if (v>0.0) res += v;
        }
		if (nl>0) res /=(long double)nl;
		//exit(1);
		return res;
	}

	long double cal_nha2p(int gr,int st){
        string *haplo;
        int iloc,kloc,j,nhl=0,nhm=0,nl=0,sample,dmax;
        bool trouve;
        long double res=0.0;
        int samp0=this->grouplist[gr].sumstat[st].samp-1;
        int samp1=this->grouplist[gr].sumstat[st].samp1-1;
        //cout <<"samples "<<samp0<<" & "<<samp1<<"\n";
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            nhl=0;
            kloc=this->grouplist[gr].loc[iloc];
            dmax=this->locuslist[kloc].ss[samp0]+this->locuslist[kloc].ss[samp1];
            haplo = new string[dmax];
            if(this->locuslist[kloc].samplesize[samp0]+this->locuslist[kloc].samplesize[samp1]>0) {
                nl++;
                if (this->locuslist[kloc].dnavar==0) {
                    haplo[nhl]="";
                    nhl++;
                }else {
                    for (int samp=0;samp<2;samp++) {
                        if (samp==0) sample=samp0; else sample=samp1;
                        for (int i=0;i<this->locuslist[kloc].ss[sample];i++) {
                            if (this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING) ; {
                                if (nhl==0) {
                                    haplo[nhl] = this->locuslist[kloc].haplodnavar[sample][i];
                                    nhl++;
                                } else {
                                    trouve=false;j=0;
                                    while ((not trouve)and(j<nhl)) {
                                        trouve=identseq(haplo[j],this->locuslist[kloc].haplodnavar[sample][i],"nah2p",kloc,sample,i);
                                        if (not trouve) j++;
                                    }
                                    if (trouve) {
                                        for (int k=0;k<this->locuslist[kloc].dnavar;k++) if (haplo[j][k]=='N') haplo[j][k] = this->locuslist[kloc].haplodnavar[sample][i][k];
                                    } else {
                                    haplo[nhl] = this->locuslist[kloc].haplodnavar[sample][i];
                                    nhl++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            nhm += nhl;
            delete [] haplo;
        }
        if (nl>0) res=(long double)nhm/(long double)nl;
		return res;
	}

    int* cal_nss2pl(int kloc,int samp0, int samp1, int *nssl,bool *OK) {
        char c0;
        bool ident;
        int j,*ss,nss=0;
        vector <int> nuvar;
        if(this->locuslist[kloc].samplesize[samp0]+this->locuslist[kloc].samplesize[samp1]>0) {
            *OK=true;
            if (not this->locuslist[kloc].dnavar==0) {
                for (j=0;j<this->locuslist[kloc].dnavar;j++) {
                    c0='\0';ident=true;
                    for (int i=0;i<this->locuslist[kloc].ss[samp0];i++) {
                        if ((this->locuslist[kloc].haplodna[samp0][i]!=SEQMISSING)and(this->locuslist[kloc].haplodnavar[samp0][i][j]!='N')) {
                            if (c0=='\0') c0=this->locuslist[kloc].haplodnavar[samp0][i][j];
                            else ident=(c0==this->locuslist[kloc].haplodnavar[samp0][i][j]);
                            //if (not ident) cout<<"nucleotide "<<j<<"   c0="<<c0<<"   c1="<<this->locuslist[kloc].haplodnavar[sample][i][j]<<"   i="<<i<<"\n";
                        }
                        if (not ident) break;
                    }
                    if (ident) {
                        for (int i=0;i<this->locuslist[kloc].ss[samp1];i++) {
                            if ((this->locuslist[kloc].haplodna[samp1][i]!=SEQMISSING)and(this->locuslist[kloc].haplodnavar[samp1][i][j]!='N')) {
                                if (c0=='\0') c0=this->locuslist[kloc].haplodnavar[samp1][i][j];
                                else ident=(c0==this->locuslist[kloc].haplodnavar[samp1][i][j]);
                                //if (not ident) cout<<"nucleotide "<<j<<"   c0="<<c0<<"   c1="<<this->locuslist[kloc].haplodnavar[sample][i][j]<<"   i="<<i<<"\n";
                            }
                            if (not ident) break;
                        }
                    }
                    if (not ident) {nss++;nuvar.push_back(j);}
                }
            }
        } else *OK=false;
        ss = new int[nss];
        for (j=0;j<nss;j++) ss[j]=nuvar[j];
        if (not nuvar.empty()) nuvar.clear();
        *nssl=nss;
        return ss;
    }

	long double cal_nss2p(int gr,int st){
        int iloc,kloc,nssl,nssm=0,nl=0,*ss;
        long double res=0.0;
        bool OK;
        //cout<<"\n";
        int samp0=this->grouplist[gr].sumstat[st].samp-1;
        int samp1=this->grouplist[gr].sumstat[st].samp1-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            ss = this->cal_nss2pl(kloc,samp0,samp1,&nssl,&OK);
            //cout<<"nssl = "<<nssl<<"\n";
            //for (int k=0;k<nssl;k++) cout<<"  "<<ss[k];if(nssl>0) cout<<"\n";
            if (OK) {nl++;nssm += nssl;}
            //cout<<"cumul nl="<<nl<<"   cumul nssm="<<nssm<<"\n";
            if (nssl>0) delete []ss;
        }
        if (nl>0) res=(long double)nssm/(long double)nl;
		return res;

	}

    long double cal_mpw2pl(int kloc,int samp0, int samp1, bool *OK) {
        int isamp,samp,di,mdl=0,nd=0;
        if(this->locuslist[kloc].samplesize[samp0]+this->locuslist[kloc].samplesize[samp1]>0) {
            *OK=true;
            if (not this->locuslist[kloc].dnavar==0) {
                for (isamp=0;isamp<2;isamp++) {
                    if (isamp==0) samp=samp0; else samp=samp1;
                    //cout<<" sample "<<samp<<"   ss="<< this->locuslist[kloc].ss[samp]<<"\n";
                    for (int i=0;i<this->locuslist[kloc].ss[samp]-1;i++) {
                        if (this->locuslist[kloc].haplodna[samp][i]!=SEQMISSING) {
                            //cout <<"coucou i\n";
                            for (int j=i+1;j<this->locuslist[kloc].ss[samp];j++) {
                                if (this->locuslist[kloc].haplodna[samp][j]!=SEQMISSING) {
                                    //cout<<"coucou j\n";
                                    nd++;
                                    di=0;
                                    for (int k=0;k<this->locuslist[kloc].dnavar;k++) {
                                        if ((this->locuslist[kloc].haplodnavar[samp][i][k]!='N')and(this->locuslist[kloc].haplodnavar[samp][j][k]!='N')and(this->locuslist[kloc].haplodnavar[samp][i][k]!=this->locuslist[kloc].haplodnavar[samp][j][k])) di++;
                                    }
                                    mdl +=di;
                                }
                                //cout<<"individus "<<i<<" et "<<j<<"   di="<<di<<"  nd="<<nd<<"   mdl="<<mdl<<"\n";
                            }
                        } //else cout <<"MISSING  ";
                    }
                }
            }
        } else *OK=false;
        if (nd>0) return(long double)mdl/(long double)nd; else return (long double)mdl;

    }


	long double cal_mpw2p(int gr,int st){
        int iloc,kloc,nl=0;
		long double res=0.0,md=0.0,mdl;
        bool OK;
        int samp0=this->grouplist[gr].sumstat[st].samp-1;
        int samp1=this->grouplist[gr].sumstat[st].samp1-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            mdl= cal_mpw2pl(kloc,samp0,samp1,&OK);
            if (OK) {nl++;md+=mdl;}
        }
        if (nl>0) res = md/(long double)nl;
		return res;
	}

    long double cal_mpb2pl(int kloc,int samp0, int samp1, bool *OK) {
        int di,mdl=0,nd=0;
        if(this->locuslist[kloc].samplesize[samp0]+this->locuslist[kloc].samplesize[samp1]>0) {
            *OK=true;
            if (not this->locuslist[kloc].dnavar==0) {
                for (int i=0;i<this->locuslist[kloc].ss[samp0];i++) {
                    if (this->locuslist[kloc].haplodna[samp0][i]!=SEQMISSING) {
                        //cout <<"coucou i\n";
                        for (int j=0;j<this->locuslist[kloc].ss[samp1];j++) {
                            if (this->locuslist[kloc].haplodna[samp1][j]!=SEQMISSING) {
                                //cout<<"coucou j\n";
                                nd++;
                                di=0;
                                for (int k=0;k<this->locuslist[kloc].dnavar;k++) {
                                    if ((this->locuslist[kloc].haplodnavar[samp0][i][k]!='N')and(this->locuslist[kloc].haplodnavar[samp1][j][k]!='N')and(this->locuslist[kloc].haplodnavar[samp0][i][k]!=this->locuslist[kloc].haplodnavar[samp1][j][k])) di++;
                                }
                                mdl +=di;
                            }
                            //cout<<"individus "<<i<<" et "<<j<<"   di="<<di<<"  nd="<<nd<<"   mdl="<<mdl<<"\n";
                        }
                    } //else cout <<"MISSING  ";
                }
            }
        } else *OK=false;
        if (nd>0) return (long double)mdl/(long double)nd; else  return (long double)mdl;
    }

    long double cal_mpb2p(int gr,int st){
        int iloc,kloc,nl=0;
        long double res=0.0,md=0.0,mdl;
        bool OK;
        int samp0=this->grouplist[gr].sumstat[st].samp-1;
        int samp1=this->grouplist[gr].sumstat[st].samp1-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            mdl = cal_mpb2pl(kloc,samp0,samp1,&OK);
            if (OK) {nl++;md+=mdl;}
        }
        if (nl>0) res = md/(long double)nl;
		return res;

	}

	long double cal_fst2p(int gr,int st){
		int iloc,kloc,nl=0;
        long double res=0.0,num=0.0,den=0.0,Hw,Hb;
        bool OKw,OKb;
        int samp0=this->grouplist[gr].sumstat[st].samp-1;
        int samp1=this->grouplist[gr].sumstat[st].samp1-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            Hw = cal_mpw2pl(kloc,samp0,samp1,&OKw);
            Hb = cal_mpb2pl(kloc,samp0,samp1,&OKb);
            if ((OKw)and(OKb)) {nl++;num+=Hb-Hw;den+=Hb;}
        }
        if (den>0) res=num/den;
		return res;

	}

    void cal_freq(int gr,int st) {
        int iloc,kloc,nhaplo,isamp,sample,dmax,j;
        long double d;
        int samp0=this->grouplist[gr].sumstat[st].samp-1;
        int samp1=this->grouplist[gr].sumstat[st].samp1-1;
        int samp2=this->grouplist[gr].sumstat[st].samp2-1;
        string *haplo;
        bool trouve;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            dmax=this->locuslist[kloc].samplesize[samp0]+this->locuslist[kloc].samplesize[samp1]+this->locuslist[kloc].samplesize[samp2];
            //cout<<"Locus "<<kloc<<"   (cal_freq)\n";
            this->locuslist[kloc].freq = new long double*[this->data.nsample];
            this->locuslist[kloc].haplomic = new int*[this->data.nsample];
            this->locuslist[kloc].kmin=100;
            nhaplo=0;
            if (this->locuslist[kloc].dnavar==0) {
                for (isamp=0;isamp<3;isamp++) {
                    if (isamp==0) sample=samp0; else if (isamp==1) sample=samp1; else sample=samp2;
                    this->locuslist[kloc].freq[sample] = new long double[1];
                    this->locuslist[kloc].freq[sample][0]=1.0;
                    this->locuslist[kloc].haplomic[sample] = new int[this->locuslist[kloc].ss[sample]];
                    for (int j=0;j<this->locuslist[kloc].ss[sample];j++) this->locuslist[kloc].haplomic[sample][j]=this->locuslist[kloc].kmin;
                }
                //haplo[nhaplo] = "";
                nhaplo++;
            } else {
                haplo = new string[dmax];
                for (isamp=0;isamp<3;isamp++) {
                    if (isamp==0) sample=samp0; else if (isamp==1) sample=samp1; else sample=samp2;
                    for (int i=0;i<this->locuslist[kloc].ss[sample];i++) {
                        if (this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING) {
                            if (nhaplo==0) {
                                haplo[nhaplo] = this->locuslist[kloc].haplodnavar[sample][i];
                                nhaplo++;
                            } else {
                                trouve=false;j=0;
                                while ((not trouve)and(j<nhaplo)) {
                                    trouve=identseq(haplo[j],this->locuslist[kloc].haplodnavar[sample][i],"cal_freq1",kloc,sample,i);
                                    if (not trouve) j++;
                                }
                                if (trouve) {
                                    for (int k=0;k<this->locuslist[kloc].dnavar;k++) if (haplo[j][k]=='N') haplo[nhaplo][k] = this->locuslist[kloc].haplodnavar[sample][i][k];
                                } else {
                                haplo[nhaplo] = this->locuslist[kloc].haplodnavar[sample][i];
                                nhaplo++;
                                }
                            }
                        }
                    }
                }
                for (isamp=0;isamp<3;isamp++) {
                    if (isamp==0) sample=samp0; else if (isamp==1) sample=samp1; else sample=samp2;
                    this->locuslist[kloc].freq[sample] = new long double[nhaplo];
                    for (j=0;j<nhaplo;j++) this->locuslist[kloc].freq[sample][j] = 0.0;
                    d=1.0/(long double)this->locuslist[kloc].samplesize[sample];
                    this->locuslist[kloc].haplomic[sample] = new int[this->locuslist[kloc].ss[sample]];
                    for (int i=0;i<this->locuslist[kloc].ss[sample];i++) {
                        if (this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING) {
                            trouve=false;j=0;
                            while ((not trouve)and(j<nhaplo)) {
                                trouve=identseq(haplo[j],this->locuslist[kloc].haplodnavar[sample][i],"cal_freq2",kloc,sample,i);
                                if (not trouve) j++;
                            }
                            this->locuslist[kloc].freq[sample][j] +=d;
                            this->locuslist[kloc].haplomic[sample][i]=this->locuslist[kloc].kmin+j;
                        } else this->locuslist[kloc].haplomic[sample][i]=MICMISSING;
                    }
                }
                delete []haplo;
            }
        }
    }

    long double cal_aml3p(int gr,int st){
        int iloc,kloc,nlocutil=0,*ss,nssl;
        long double p1,p2,p3,lik1,lik2,lik3;
        int i1=1,i2=998,i3;
        bool OK;
        long double res=0.0;
        complex<long double> c;
        //cout<<"debut de cal_aml3p\n";
        int samp1=this->grouplist[gr].sumstat[st].samp1-1;
        int samp2=this->grouplist[gr].sumstat[st].samp2-1;


        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            kloc=this->grouplist[gr].loc[iloc];
            ss = this->cal_nss2pl(kloc,samp1,samp2,&nssl,&OK);
            if ((OK)and(nssl>0)) nlocutil++;
        }
        delete []ss;
        if (nlocutil<1) return 0.5;
        //cout<<"avant cal_freq\n";
        cal_freq(gr,st);
        c=pente_lik(gr,st,i1);lik1=real(c);p1=imag(c);
        c=pente_lik(gr,st,i2);lik2=real(c);p2=imag(c);
        if ((p1<0.0)and(p2<0.0)) return 0.0;
        if ((p1>0.0)and(p2>0.0)) return 1.0;
        do {
            i3 = (i1+i2)/2;
            c=pente_lik(gr,st,i3);lik3=real(c);p3=imag(c);
            if (p1*p3<0.0) {i2=i3;p2=p3;lik2=lik3;}
            else           {i1=i3;p1=p3;lik1=lik3;}
        } while (abs(i2-i1)>1);
        if (lik1>lik2) res=0.001*(long double)i1; else res = 0.001*(long double)i2;
        libere_freq(gr);
        return res;
	}

	void cal_numvar(int gr) {
        int i,j,k,ns,pop,j0;
        int locus,nlocs=this->grouplist[gr].nloc;
        string ss;
        char *site;
        bool ident;
        //this->nvar = new int[nlocs];
        //this->numvar = new int*[nlocs];
        vector <int> nuvar;
        //cout <<"debut de cal_numvar dnatrue ="<<this->dnatrue<<"\n";
		if (not this->dnatrue) {
            for (int iloc=0;iloc<nlocs;iloc++) {
                locus= this->grouplist[gr].loc[iloc];
                this->locuslist[locus].dnavar = 0;
                for (pop=0;pop<this->data.nsample;pop++) {
                    for (i=0;i<this->locuslist[locus].ss[pop];i++) {
                        if (this->locuslist[locus].haplodna[pop][i].length()>this->locuslist[locus].dnavar) {
                            this->locuslist[locus].dnavar=this->locuslist[locus].haplodna[pop][i].length();
                            //cout<<"  >"<<this->locuslist[locus].haplodna[pop][i]<<"<  ("<<this->locuslist[locus].haplodna[pop][i].length()<<")\n";
                        }
                    }
                    if (this->locuslist[locus].dnavar<0) {
                        cout<<"Locus "<<locus<<"   sample "<<pop <<"   dnavar="<<this->locuslist[locus].dnavar<<"\n";
                         //this->locuslist[locus].dnavar = 0;
                        exit(1);
                    }
                }
                //cout<<"coucou\n";
                this->locuslist[locus].haplodnavar = new string*[this->data.nsample];
            	for (pop=0;pop<this->data.nsample;pop++) {
               	    this->locuslist[locus].haplodnavar[pop] = new string[this->locuslist[locus].ss[pop]];
                	for (i=0;i<this->locuslist[locus].ss[pop];i++) {
                   	    if (this->locuslist[locus].haplodna[pop][i].length()==this->locuslist[locus].dnavar) {
                            this->locuslist[locus].haplodnavar[pop][i] = this->locuslist[locus].haplodna[pop][i];
                        } else {
                            this->locuslist[locus].haplodnavar[pop][i] = "";
                            for (int j=0;j<this->locuslist[locus].dnavar;j++)this->locuslist[locus].haplodnavar[pop][i] += "N";
                        }
                    }
                }
            }
        } else {
            for (int iloc=0;iloc<nlocs;iloc++) {
                locus= this->grouplist[gr].loc[iloc];
                this->locuslist[locus].haplodnavar = new string*[this->data.nsample];
                ns=0; for (pop=0;pop<this->data.nsample;pop++) ns += this->locuslist[locus].ss[pop];
                site = new char[ns];
                this->locuslist[locus].dnavar=0;
                if (this->locuslist[locus].dnalength>0) {
                    //cout <<"longseq = "<<this->locuslist[locus].dnalength<<"\n";;
                    for (k=0;k<this->locuslist[locus].dnalength;k++) {
                        j=-1;
                        for (pop=0;pop<this->data.nsample;pop++) {
                            for (i=0;i<this->locuslist[locus].ss[pop];i++) {
                                j++;
                                if (this->locuslist[locus].haplodna[pop][i] == SEQMISSING) site[j]='N';
                                else site[j] = this->locuslist[locus].haplodna[pop][i][k];
                                //if ((locus==13)and(j<1000)) cout<<"locus"<<locus<<"   site["<<j<<"] = "<<site[j]<<"\n";
                            }
                        }

                        j0=0;
                        while ((site[j0]=='N')and(j0<ns-1)) j0++; //recherche du premier nucléotide non N
                        j=j0+1;
                        ident=true;
                        while ((ident)and(j<ns)) {
                            ident=((site[j]=='N')or(site[j]==site[j0]));
                            j++;
                        }
                        if (not ident) {
                           this->locuslist[locus].dnavar++;
                           nuvar.push_back(k);
                        }
                    }
                }
                //cout<<"locus "<<locus<<"   dnavar = "<<this->locuslist[locus].dnavar<<"\n";
                for (pop=0;pop<this->data.nsample;pop++) {
                    this->locuslist[locus].haplodnavar[pop] = new string[this->locuslist[locus].ss[pop]];
                    for (i=0;i<this->locuslist[locus].ss[pop];i++) {
                        ss="";
                        for (int k=0;k<this->locuslist[locus].dnavar;k++) {
                            if ( this->locuslist[locus].haplodna[pop][i]!=SEQMISSING) {
                                ss +=this->locuslist[locus].haplodna[pop][i][nuvar[k]];
                            } else {
                                ss +="N";
                            }
                        }
                        this->locuslist[locus].haplodnavar[pop][i] = ss;
                        //for(int k=0;k<this->locuslist[locus].dnavar;k++) cout<<"   " <<nuvar[k]; cout<<"\n";
                    }
                }
		    	if (not nuvar.empty()) nuvar.clear();
		    	delete []site;
                ss.clear();
		    }
		}
    }

	/*  Numérotation des stat
	 *  1 : nal			-1 : nha	    -13 : fst
	 *  2 : het			-2 : nss            -14 : aml
	 *  3 : var			-3 : mpd
	 *  4 : MGW			-4 : vpd
	 *  5 : Fst			-5 : dta
	 *  6 : lik			-6 : pss
	 *  7 : dm2			-7 : mns
	 *  8 : n2P			-8 : vns
	 *  9 : h2P			-9 : nh2
	 * 10 : v2P		       -10 : ns2
	 * 11 : das                    -11 : mp2
	 * 12 : Aml                    -12 : mpb
	 *
	 *
	 */
	long double cal_qu1L(int n, long double *x) {
		if ((n % 4)==0) return 0.5*(x[n/4]+x[(n/4)-1]);
		else if ((n % 2)==0) return x[n/4];
		else return (0.25*x[(n/4)-1])+(0.75*x[n/4]);
	}
	long double cal_qu3L(int n, long double *x) {
		if ((n % 4)==0) return 0.5*(x[3*n/4]+x[(3*n/4)+1]);
		else if ((n % 2)==0) return x[3*n/4];
		else return (0.25*x[(3*n/4)+1])+(0.75*x[3*n/4]);
	}
	
	void docalstat(int gr) {
//		cout << "avant calfreq\n";
		//cout << this->grouplist[gr].type<<"\n";
		if (this->grouplist[gr].type == 0)  this->calfreq(gr);
        else if (this->grouplist[gr].type == 1)  this->cal_numvar(gr);
		else  this->calfreqsnp(gr);
//		cout << "apres calfreq\n";
        for (int st=0;st<this->grouplist[gr].nstat;st++) {
            if ((this->grouplist[gr].sumstat[st].cat==-7)or(this->grouplist[gr].sumstat[st].cat==-8)) {
                this->afsdone.resize(this->data.nsample);
                this->t_afs.resize(this->data.nsample);
                this->n_afs.resize(this->data.nsample);
                for (int sa=0;sa<this->data.nsample;sa++) {
                    this->afsdone[sa].resize(this->grouplist[gr].nloc);
                    this->t_afs[sa].resize(this->grouplist[gr].nloc);
                    this->n_afs[sa].resize(this->grouplist[gr].nloc);
                    for (int loc=0;loc<this->grouplist[gr].nloc;loc++) this->afsdone[sa][loc]=false;
                }
            }
        }
        int numsnp;
		for (int st=0;st<this->grouplist[gr].nstat;st++) {
			/*if (this->grouplist[gr].sumstat[st].cat<5)
			{cout <<" calcul de la stat "<<st<<"   cat="<<this->grouplist[gr].sumstat[st].cat<<"   group="<<gr<<"   samp = "<<this->grouplist[gr].sumstat[st].samp  <<"\n";fflush(stdin);}
			else if (this->grouplist[gr].sumstat[st].cat<12)
			{cout <<" calcul de la stat "<<st<<"   cat="<<this->grouplist[gr].sumstat[st].cat<<"   group="<<gr<<"   samp = "<<this->grouplist[gr].sumstat[st].samp <<"   samp1 = "<<this->grouplist[gr].sumstat[st].samp1 <<"\n";fflush(stdin);}
			else
			{cout <<" calcul de la stat "<<st<<"   cat="<<this->grouplist[gr].sumstat[st].cat<<"   group="<<gr<<"   samp = "<<this->grouplist[gr].sumstat[st].samp <<"   samp1 = "<<this->grouplist[gr].sumstat[st].samp1 <<"   samp2 = "<<this->grouplist[gr].sumstat[st].samp2<<"\n";fflush(stdin);}
			*/
			int categ;
			categ=this->grouplist[gr].sumstat[st].cat;
            //cout<<"avant calcul stat categ="<<categ<<"\n";
			
			
			
			switch (categ)
			{	case     0 : this->grouplist[gr].sumstat[st].val = cal_pid1p(gr,st);break;
                case     1 : this->grouplist[gr].sumstat[st].val = cal_nal1p(gr,st);break;
				case     2 : this->grouplist[gr].sumstat[st].val = cal_het1p(gr,st);break;
				case     3 : this->grouplist[gr].sumstat[st].val = cal_var1p(gr,st);break;
				case     4 : this->grouplist[gr].sumstat[st].val = cal_mgw1p(gr,st);break;
				case     5 : this->grouplist[gr].sumstat[st].val = cal_nal2p(gr,st);break;
				case     6 : this->grouplist[gr].sumstat[st].val = cal_het2p(gr,st);break;
				case     7 : this->grouplist[gr].sumstat[st].val = cal_var2p(gr,st);break;
                case     8 : this->grouplist[gr].sumstat[st].val = cal_Fst2p(gr,st);break;
                case     9 : this->grouplist[gr].sumstat[st].val = cal_lik2p(gr,st);break;
                case    10 : this->grouplist[gr].sumstat[st].val = cal_das2p(gr,st);break;
				case    11 : this->grouplist[gr].sumstat[st].val = cal_dmu2p(gr,st);break;
				case    12 : this->grouplist[gr].sumstat[st].val = cal_Aml3p(gr,st);break;
				case    -1 : this->grouplist[gr].sumstat[st].val = cal_nha1p(gr,st);break;
				case    -2 : this->grouplist[gr].sumstat[st].val = cal_nss1p(gr,st);break;
				case    -3 : this->grouplist[gr].sumstat[st].val = cal_mpd1p(gr,st);break;
				case    -4 : this->grouplist[gr].sumstat[st].val = cal_vpd1p(gr,st);break;
				case    -5 : this->grouplist[gr].sumstat[st].val = cal_dta1p(gr,st);break;
				case    -6 : this->grouplist[gr].sumstat[st].val = cal_pss1p(gr,st);break;
				case    -7 : this->grouplist[gr].sumstat[st].val = cal_mns1p(gr,st);break;
				case    -8 : this->grouplist[gr].sumstat[st].val = cal_vns1p(gr,st);break;
				case    -9 : this->grouplist[gr].sumstat[st].val = cal_nha2p(gr,st);break;
				case   -10 : this->grouplist[gr].sumstat[st].val = cal_nss2p(gr,st);break;
				case   -11 : this->grouplist[gr].sumstat[st].val = cal_mpw2p(gr,st);break;
				case   -12 : this->grouplist[gr].sumstat[st].val = cal_mpb2p(gr,st);break;
				case   -13 : this->grouplist[gr].sumstat[st].val = cal_fst2p(gr,st);break;
				case   -14 : this->grouplist[gr].sumstat[st].val = cal_aml3p(gr,st);break;
				case    21 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snhet(gr,numsnp);
							 this->grouplist[gr].sumstat[st].val = cal_moyL(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;
				case    22 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snhet(gr,numsnp);
							 this->grouplist[gr].sumstat[st].val = cal_varL(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;
				case    23 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snhet(gr,numsnp);
							 if (not this->grouplist[gr].sumstatsnp[numsnp].sorted) {
								 sort(&this->grouplist[gr].sumstatsnp[numsnp].x[0],&this->grouplist[gr].sumstatsnp[numsnp].x[this->grouplist[gr].sumstatsnp[numsnp].n]);
								 this->grouplist[gr].sumstatsnp[numsnp].sorted=true;
							 }
							 this->grouplist[gr].sumstat[st].val = cal_qu1L(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;
				case	24 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snhet(gr,numsnp);
							 if (not this->grouplist[gr].sumstatsnp[numsnp].sorted) {
								 sort(&this->grouplist[gr].sumstatsnp[numsnp].x[0],&this->grouplist[gr].sumstatsnp[numsnp].x[this->grouplist[gr].sumstatsnp[numsnp].n]);
								 this->grouplist[gr].sumstatsnp[numsnp].sorted=true;
							 }
							 this->grouplist[gr].sumstat[st].val = cal_qu3L(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;

				case    25 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snnei(gr,numsnp);
							 this->grouplist[gr].sumstat[st].val = cal_moyL(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;
				case    26 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snnei(gr,numsnp);
							 this->grouplist[gr].sumstat[st].val = cal_varL(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;
				case    27 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snnei(gr,numsnp);
							 if (not this->grouplist[gr].sumstatsnp[numsnp].sorted) {
								 sort(&this->grouplist[gr].sumstatsnp[numsnp].x[0],&this->grouplist[gr].sumstatsnp[numsnp].x[this->grouplist[gr].sumstatsnp[numsnp].n]);
								 this->grouplist[gr].sumstatsnp[numsnp].sorted=true;
							 }
							 this->grouplist[gr].sumstat[st].val = cal_qu1L(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;
				case	28 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snnei(gr,numsnp);
							 if (not this->grouplist[gr].sumstatsnp[numsnp].sorted) {
								 sort(&this->grouplist[gr].sumstatsnp[numsnp].x[0],&this->grouplist[gr].sumstatsnp[numsnp].x[this->grouplist[gr].sumstatsnp[numsnp].n]);
								 this->grouplist[gr].sumstatsnp[numsnp].sorted=true;
							 }
							 this->grouplist[gr].sumstat[st].val = cal_qu3L(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;

				case    29 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snfst(gr,numsnp);
							 this->grouplist[gr].sumstat[st].val = cal_moyL(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;
				case    30 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snfst(gr,numsnp);
							 this->grouplist[gr].sumstat[st].val = cal_varL(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;
				case    31 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snfst(gr,numsnp);
							 if (not this->grouplist[gr].sumstatsnp[numsnp].sorted) {
								 sort(&this->grouplist[gr].sumstatsnp[numsnp].x[0],&this->grouplist[gr].sumstatsnp[numsnp].x[this->grouplist[gr].sumstatsnp[numsnp].n]);
								 this->grouplist[gr].sumstatsnp[numsnp].sorted=true;
							 }
							 this->grouplist[gr].sumstat[st].val = cal_qu1L(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;
				case	32 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snfst(gr,numsnp);
							 if (not this->grouplist[gr].sumstatsnp[numsnp].sorted) {
								 sort(&this->grouplist[gr].sumstatsnp[numsnp].x[0],&this->grouplist[gr].sumstatsnp[numsnp].x[this->grouplist[gr].sumstatsnp[numsnp].n]);
								 this->grouplist[gr].sumstatsnp[numsnp].sorted=true;
							 }
							 this->grouplist[gr].sumstat[st].val = cal_qu3L(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;

				case    33 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snaml(gr,numsnp);
							 this->grouplist[gr].sumstat[st].val = cal_moyL(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;
				case    34 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snaml(gr,numsnp);
							 this->grouplist[gr].sumstat[st].val = cal_varL(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;
				case    35 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snaml(gr,numsnp);
							 if (not this->grouplist[gr].sumstatsnp[numsnp].sorted) {
								 sort(&this->grouplist[gr].sumstatsnp[numsnp].x[0],&this->grouplist[gr].sumstatsnp[numsnp].x[this->grouplist[gr].sumstatsnp[numsnp].n]);
								 this->grouplist[gr].sumstatsnp[numsnp].sorted=true;
							 }
							 this->grouplist[gr].sumstat[st].val = cal_qu1L(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;
				case	36 : numsnp=this->grouplist[gr].sumstat[st].numsnp;
							 if (not this->grouplist[gr].sumstatsnp[numsnp].defined) cal_snaml(gr,numsnp);
							 if (not this->grouplist[gr].sumstatsnp[numsnp].sorted) {
								 sort(&this->grouplist[gr].sumstatsnp[numsnp].x[0],&this->grouplist[gr].sumstatsnp[numsnp].x[this->grouplist[gr].sumstatsnp[numsnp].n]);
								 this->grouplist[gr].sumstatsnp[numsnp].sorted=true;
							 }
							 this->grouplist[gr].sumstat[st].val = cal_qu3L(this->grouplist[gr].sumstatsnp[numsnp].n,this->grouplist[gr].sumstatsnp[numsnp].x);break;
			}
			//cout << "stat["<<st<<"]="<<this->grouplist[gr].sumstat[st].val<<"   ";fflush(stdin);
			//cout<<"\n";
		}
		if ((this->grouplist[gr].type == 0)or(this->grouplist[gr].type>9)) liberefreq(gr);
        else liberednavar(gr);
        if (not this->afsdone.empty()) {
            for (int sa=0;sa<this->data.nsample;sa++) {
                this->afsdone[sa].clear();
                for (int loc=0;loc<this->grouplist[gr].nloc;loc++) if (not this->t_afs[sa][loc].empty()) this->t_afs[sa][loc].clear();
                this->t_afs[sa].clear();
                this->n_afs[sa].clear();
            }
            this->afsdone.clear();
            this->t_afs.clear();
            this->n_afs.clear();
        }
	}

};

