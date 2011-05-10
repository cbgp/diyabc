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
#define SEQMISSING '\0'
#define NUCMISSING 'N'
#define NSTAT 27

string stat_type[NSTAT] = {"PID","NAL","HET","VAR","MGW","N2P","H2P","V2P","FST","LIK","DAS","DM2","AML","NHA","NSS","MPD","VPD","DTA","PSS","MNS","VNS","NH2","NS2","MP2","MPB","HST","SML"};
int stat_num[NSTAT]     = {  0  ,  1  ,  2  ,  3  ,  4  ,  5  ,  6  ,  7  ,  8  ,  9  ,  10 ,  11 ,  12 , -1  , -2  , -3  , -4  , -5  , -6  , -7  , -8  , -9  , -10 , -11 , -12 , -13 , -14 };
/*  Numérotation des stat
 * 	1 : nal			-1 : nha			-13 : fst
 *  2 : het			-2 : nss            -14 : aml
 *  3 : var			-3 : mpd
 *  4 : MGW			-4 : vpd
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
    vector <string> histparname;
    vector <int> histparcat;

extern int debuglevel;

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
	bool constant;
	
	void ecris(){
		cout <<"    loi="<<this->loi<<"   min="<<this->mini<<"   max="<<this->maxi<<"   ndec="<<this->ndec;
		if (this->loi=="GA") cout <<"    shape="<<this->sdshape;
		if (this->constant) cout<<"   constant\n"; else cout<<"   variable\n";
	}
	  
};

/**
* Classe ConditionC :éléments de définition d'une condition sur un couple de paramètres historiques
*/

struct ConditionC
{
	string param1,param2,operateur;
	
	void ecris(){cout<<this->param1<<this->operateur<<this->param2<<"\n";}

};

/**
* struct StatC :éléments de définition d'une summary statistic
*/
struct StatC
{
	int cat,samp,samp1,samp2,group;
	double val;
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
		cout <<"ltime="<<this->ltime<<"\n";
      //cout<<"    "<<this->stime<<"   "<<this->action<<"   "<<this->sadmixrate<<"\n";
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
	int *loc,nloc,nstat;           // *loc=numeros des locus du groupe
	int type;                      //O= microsat, 1=sequence
	double p_fixe,gams;
	double musmoy,mutmoy,Pmoy,snimoy;
	double k1moy,k2moy;
	int  mutmod;
	PriorC priormusmoy,priork1moy,priork2moy,priormusloc,priork1loc,priork2loc;
	PriorC priormutmoy,priorPmoy,priorsnimoy,priormutloc,priorPloc,priorsniloc;
	StatC *sumstat;
    
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
        //cout<<"s1="<<s1<<"\n";
        while (s1.length()>0) {
                plus=s1.find("+");
                minus=s1.find("-");
                if ((plus == string::npos)and(minus == string::npos)) {ss.push_back(s1);s1="";}
                else {
                        if (plus>=0) posigne=plus;
                        else		 posigne=minus;
                        ss.push_back(s1.substr(0,posigne));
                        s1=s1.substr(posigne+1);	
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
    	string *ss;
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
                     this->event[i].stime=new char[this->event[i].ltime];
                     strcpy(this->event[i].stime,ss[0].c_str());this->detparam(ss[0],1);}
    			 else {this->event[i].time=atoi(ss[0].c_str());}
    		if (majuscules(ss[1])=="SAMPLE") {
    			this->event[i].action='E';
    			this->event[i].pop=atoi(ss[2].c_str());
    			this->nsamp++;
			this->event[i].sample=this->nsamp;
    			//cout <<this->event[i].time<<"  SAMPLE"<<"   "<<this->event[i].pop<<"\n";
    		} else if (majuscules(ss[1])=="MERGE") {
    			this->event[i].action='M';
    			this->event[i].pop=atoi(ss[2].c_str());
    			this->event[i].pop1=atoi(ss[3].c_str());
    			//cout <<this->event[i].stime<<"  MERGE"<<"   "<<this->event[i].pop<<"   "<<this->event[i].pop1<<"\n";
    			
    		} else if (majuscules(ss[1])=="SPLIT") {
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
    		} else if (majuscules(ss[1])=="VARNE") {
    			this->event[i].action='V';
    			this->event[i].pop=atoi(ss[2].c_str());
    			if (atof(ss[3].c_str())!=0.0) this->event[i].Ne=atoi(ss[3].c_str());
    			else {
                              this->event[i].Ne=-1;
                              this->event[i].lNe=ss[3].length()+1;
                              this->event[i].sNe=new char[this->event[i].lNe];
                              strcpy(this->event[i].sNe,ss[3].c_str());
                              this->detparam(ss[3],0);
                        }
    		}
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
    	cout<<"    paramvar:\n";
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
        //cout<<"   apres copyevent\n";
	dest.ne0 = new Ne0C[dest.nn0];
	for (int i=0;i<dest.nn0;i++) dest.ne0[i] = copyne0(source.ne0[i]);
        //cout<<"   apres copyne0\n";
	dest.time_sample = new int[dest.nsamp];
	for (int i=0;i<dest.nsamp;i++) dest.time_sample[i] = source.time_sample[i];
        //cout<<"   apres copytime_sample\n";
	dest.histparam = new HistParameterC[dest.nparam];
	for (int i=0;i<dest.nparam;i++) {dest.histparam[i] = copyhistparameter(source.histparam[i]);/*cout<<dest.histparam[i].name<<"\n"<<flush;*/}
        //cout<<"   apres copyhistparam\n";
	dest.paramvar = new double[dest.nparamvar];
	for (int i=0;i<dest.nparamvar;i++) {dest.paramvar[i] = source.paramvar[i];/*cout<<dest.histparam[i].name<<"\n"<<flush;*/}
        //cout<<"   apres copyparamvar\n";
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

};

/**
* Struct NodeC : éléments de définition d'un noeud de l'arbre de coalescence 
*/
struct NodeC
{
	int pop,sample,state;
    double height;
	string dna;
};

/**
* Struct BranchC : éléments de définition d'une branche de l'arbre de coalescence 
*/
struct BranchC
{
	int bottom,top,nmut;
	double length;
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
					{return 0;}
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
	void deletetree(GeneTreeC tree) {
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
	    while ((OK)and(i<this->scen.nconditions)){
	        //this->scen.condition[i].ecris();
	        //cout<<this->scen.condition[i].param1<<this->scen.condition[i].operateur<<this->scen.condition[i].param2<<"\n";
	        ip1=0;while (this->scen.condition [i].param1!=this->scen.histparam[ip1].name) ip1++;
	        ip2=0;while (this->scen.condition [i].param2!=this->scen.histparam[ip2].name) ip2++;
	        if (this->scen.condition[i].operateur==">")       OK=(this->scen.histparam[ip1].value > this->scen.histparam[ip2].value);
	        else if (this->scen.condition[i].operateur=="<")  OK=(this->scen.histparam[ip1].value < this->scen.histparam[ip2].value);
	        else if (this->scen.condition[i].operateur==">=") OK=(this->scen.histparam[ip1].value >= this->scen.histparam[ip2].value);
	        else if (this->scen.condition[i].operateur=="<=") OK=(this->scen.histparam[ip1].value <= this->scen.histparam[ip2].value);
	        i++;
	    }
	    return OK;
	}

/**
* Struct ParticleC : génère les valeurs des paramètres historiques d'un scénario donné' 
*/
	bool setHistParamValue(bool usepriorhist) {
		bool OK=true;
        if (usepriorhist) {
            if (this->scen.nconditions>0) {
                if (drawuntil) {
                    //cout <<"drawuntil\n";
                    OK=false;
                    while (not OK) {
                        for (int p=0;p<this->scen.nparam;p++) {
                            this->scen.histparam[p].value = drawfromprior(this->scen.histparam[p].prior);
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
                        this->scen.histparam[p].value = drawfromprior(this->scen.histparam[p].prior);
                        if (this->scen.histparam[p].category<2) this->scen.histparam[p].value = floor(0.5+this->scen.histparam[p].value);
                    }  
                    OK = conditionsOK();
                }
            }else {
                for (int p=0;p<this->scen.nparam;p++) {
                    this->scen.histparam[p].value = drawfromprior(this->scen.histparam[p].prior);
                    if (this->scen.histparam[p].category<2) this->scen.histparam[p].value = floor(0.5+this->scen.histparam[p].value);
                    //cout << this->scen.histparam[p].name<<" = "<<this->scen.histparam[p].value<<"\n";
                }
                OK=true;
            }
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
		//for (int p=0;p<this->scen.nparam;p++) cout<<this->scen.histparam[p].name<<" = " <<this->scen.histparam[p].value<<"\n";
		//cout <<"\n";
		if (OK) {
			for (int ievent=0;ievent<this->scen.nevent;ievent++) {
				if (this->scen.event[ievent].action=='V') { if (this->scen.event[ievent].Ne<0) this->scen.event[ievent].Ne= (int)(0.5+this->getvalue(this->scen.event[ievent].sNe));}
				if (this->scen.event[ievent].time==-9999) {this->scen.event[ievent].time = (int)(0.5+this->getvalue(this->scen.event[ievent].stime));}
				if (this->scen.event[ievent].action=='S') {if (this->scen.event[ievent].admixrate<0) this->scen.event[ievent].admixrate = this->getvalue(this->scen.event[ievent].sadmixrate);}
			}
//TRI SUR LES TEMPS DES EVENEMENTS
            sort(&this->scen.event[0],&this->scen.event[this->scen.nevent],compevent());
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
	void setMutParammoyValue(bool usepriormut){
		int gr;
		for (gr=1;gr<=this->ngr;gr++) {
		    //cout<<"groupe "<<gr<<"   type="<<this->grouplist[gr].type<<"\n";
		    if (this->grouplist[gr].type==0) {  //microsat
		        if (this->grouplist[gr].mutmoy<0) {
			    this->grouplist[gr].mutmoy = this->drawfromprior(this->grouplist[gr].priormutmoy);
                            //this->grouplist[gr].priormutmoy.ecris();
			    if (not this->grouplist[gr].priormutmoy.constant) {
			        this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].mutmoy;
			        this->scen.ipv++;
			    }
			}
			//cout<<"mutmoy="<<this->grouplist[gr].mutmoy<<"\n";fflush(stdin);
			if (this->grouplist[gr].Pmoy<0) {
			    this->grouplist[gr].Pmoy = this->drawfromprior(this->grouplist[gr].priorPmoy);
			    if (not this->grouplist[gr].priorPmoy.constant) {
			      //cout<<"prior pmoy non constant ?\n";  
                              this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].Pmoy;
			        this->scen.ipv++;
			    }
			}
			//cout<<"Pmoy="<<this->grouplist[gr].Pmoy<<"\n";fflush(stdin);
			if (this->grouplist[gr].snimoy<0) {
			    this->grouplist[gr].snimoy = this->drawfromprior(this->grouplist[gr].priorsnimoy);
			    if (not this->grouplist[gr].priorsnimoy.constant) {
			        this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].snimoy;
			        this->scen.ipv++;
			    }
			}
			//cout<<"snimoy="<<this->grouplist[gr].snimoy<<"\n";fflush(stdin);
		    }
		    if (this->grouplist[gr].type==1) {  //sequence
		        if (this->grouplist[gr].musmoy<0) {
			    this->grouplist[gr].musmoy = this->drawfromprior(this->grouplist[gr].priormusmoy);
			    if (not this->grouplist[gr].priormusmoy.constant) {
			        this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].musmoy;
			        this->scen.ipv++;
			    }
			}
			//cout<<"musmoy="<<this->grouplist[gr].musmoy<<"\n";fflush(stdin);
			if (this->grouplist [gr].mutmod>0){
			    if (this->grouplist[gr].k1moy<0) { 
			        this->grouplist[gr].k1moy = this->drawfromprior(this->grouplist[gr].priork1moy);
			        if (not this->grouplist[gr].priork1moy.constant) {
			            this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].k1moy;
			            this->scen.ipv++;
				}
			    }
			//    cout<<"k1moy="<<this->grouplist[gr].k1moy<<"\n";fflush(stdin);
			}
			if (this->grouplist [gr].mutmod>2){
			    if (this->grouplist[gr].k2moy<0) {
			        this->grouplist[gr].k2moy = this->drawfromprior(this->grouplist[gr].priork2moy);
			        if (not this->grouplist[gr].priork2moy.constant) {
			            this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].k2moy;
			            this->scen.ipv++;
				}
			    }
			}
		    }
		
		}
		this->scen.nparamvar=this->scen.ipv;
	}
	
/**
* Struct ParticleC : génère les valeurs des paramètres mutationnels du locus loc 
*/
	void setMutParamValue(int loc){
		int gr = this->locuslist[loc].groupe;
		//cout <<"\n SetMutParamValue pour le locus "<<loc<< " (groupe "<< gr <<")\n";
		if (this->locuslist[loc].type<5) {  //MICROSAT
			this->grouplist[gr].priormutloc.mean = this->grouplist[gr].mutmoy;
			if ((this->grouplist[gr].priormutloc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].mut_rate = this->drawfromprior(this->grouplist[gr].priormutloc);
			else this->locuslist[loc].mut_rate =this->grouplist[gr].mutmoy;
			//cout << "mutloc["<<loc<<"]="<<this->locuslist[loc].mut_rate <<"\n";

			this->grouplist[gr].priorPloc.mean = this->grouplist[gr].Pmoy;
			if ((this->grouplist[gr].priorPloc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].Pgeom = this->drawfromprior(this->grouplist[gr].priorPloc);
			else this->locuslist[loc].Pgeom =this->grouplist[gr].Pmoy;
			//cout << "Ploc["<<loc<<"]="<<this->locuslist[loc].Pgeom <<"\n";

			this->grouplist[gr].priorsniloc.mean = this->grouplist[gr].snimoy;
			//cout <<"coucou\n";fflush(stdin);
			if ((this->grouplist[gr].priorsniloc.sdshape>0.001 )and(this->grouplist[gr].nloc>1)) this->locuslist[loc].sni_rate = this->drawfromprior(this->grouplist[gr].priorsniloc);
			else this->locuslist[loc].sni_rate =this->grouplist[gr].snimoy;
			//cout << "sniloc["<<loc<<"]="<<this->locuslist[loc].sni_rate <<"\n";
			
	
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
	
	void comp_matQ(int loc) {
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
		double s=0.0;
		for (int i=0;i<4;i++) {
			for (int j=0;j<4;j++) {s += this->matQ[i][j];}
		}
		for (int i=0;i<4;i++) {
			for (int j=0;j<4;j++) {this->matQ[i][j] /= s;}
		}
	}


	GeneTreeC init_tree(int loc) {
		//cout << "début de init_tree pour le locus " << loc  <<"\n";
		GeneTreeC gt;
		int nnod=0,nn,n=0;
		//cout << "nsample = " << this->data.nsample << "   samplesize[0] = " << this->locuslist[loc].samplesize[0] << "\n";
		for (int sa=0;sa<this->data.nsample;sa++) {nnod +=this->locuslist[loc].ss[sa];}
		//cout << "nombre initial de noeuds = " << nnod  <<"\n";
		gt.ngenes = nnod;
		gt.nnodes=nnod;
		gt.nbranches = 0;
		gt.nodes = new NodeC[2*nnod+1];
		gt.branches = new BranchC[2*nnod];
		for (int sa=0;sa<this->data.nsample;sa++) {
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

	void coal_pop(int loc,int iseq) {
		//cout <<"\n";
		//cout << "debut COAL nbranches=" << this->gt[loc].nbranches <<"   nnodes="<<this->gt[loc].nnodes <<"\n";
		int nLineages=0;
		bool final=false;
        bool trace = ((loc==12)/*and(iseq==5)*/);
        //cout << "coucou 1 \n";
		for (int i=0;i<this->gt[loc].nnodes;i++){
			//cout << gt[loc].nodes[i].pop << "  ";
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
				double lra; lra = log(ra);
				start -= (this->locuslist[loc].coeff*this->seqlist[iseq].N/nLineages/(nLineages-1.0))*log(ra);
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
			else                                      {this->gt[loc].nodes[i].pop = this->seqlist[iseq].pop2;}
			}
		}
	}

	void add_sample(int loc,int iseq) {
		//cout <<"add sample "<<this->seqlist[iseq].sample<<"   nnodes="<<this->gt[loc].nnodes<<"\n";
        //int nn=0,j=0;
		for (int i=0;i<this->gt[loc].nnodes;i++){
            //cout<<"\n a "<<j;;fflush(stdin);
            //j++;
            //cout<<"  "<<this->seqlist[iseq].sample;fflush(stdin);
            //cout<<"  "<<this->gt[loc].nodes[i].sample;;fflush(stdin);
			if (this->gt[loc].nodes[i].sample == this->seqlist[iseq].sample) {
				this->gt[loc].nodes[i].pop = this->seqlist[iseq].pop;
                //nn++;
                //cout<<"  "<<i;
			}
		}
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
		else                        mutrate=this->locuslist[loc].mus_rate;
		for (int b=0;b<this->gt[loc].nbranches;b++) {
			this->gt[loc].branches[b].nmut = this->mw.poisson(this->gt[loc].branches[b].length*mutrate);
			this->gt[loc].nmutot += this->gt[loc].branches[b].nmut;
		}
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
		else  {								//DNA SEQUENCE
			string dna=this->gt[loc].nodes[this->gt[loc].branches[b].bottom].dna;
			char dnb;
			double ra=this->mw.random();
			int n =this->locuslist[loc].sitmut[numut];
            //cout<<"numut="<<numut<<"   n="<<n<<"\n";
			dnb = dna[n];
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
			dna[n] = dnb;
			this->gt[loc].nodes[this->gt[loc].branches[b].bottom].dna = dna;
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
      if (debuglevel==10) cout<<"debut de init_dnaseq  dnatrue = "<<this->dnatrue<<"\n";
		this->locuslist[loc].tabsit = new int[this->locuslist[loc].dnalength];
		int *sitmut2;
		string dna;
		sitmut2 = new int[this->gt[loc].nmutot];
		this->locuslist[loc].sitmut = new int[this->gt[loc].nmutot];
		for (int i=0;i<this->gt[loc].nmutot;i++){
			double ra=this->mw.random();
			double s=0.0;
			int k=-1;
			while (s<ra) {k++;s+=this->locuslist[loc].mutsit[k];}
			sitmut2[i]=k;
		}
		if (this->dnatrue) {			//TRUE DNA SEQUENCE
			for (int i=0;i<this->gt[loc].nmutot;i++) this->locuslist[loc].sitmut[i]=sitmut2[i];
			dna.resize(this->locuslist[loc].dnalength+1);
			for (int i=0;i<this->locuslist[loc].dnalength;i++){
				this->locuslist[loc].tabsit[i] = i;
				dna[i]=draw_nuc(loc);
			}
			dna[this->locuslist[loc].dnalength]='\0';
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
				if (dna2[sitmut2[i]] == 'z') {
					this->locuslist[loc].tabsit[sitmut2[i]] = k;
					//this->locuslist[loc].sitmut[i]=k;
					k++;
					dna2[sitmut2[i]] = draw_nuc(loc);
					dna[k-1]=dna2[sitmut2[i]];
				}
			}
			for (int i=0;i<this->gt[loc].nmutot;i++) this->locuslist[loc].sitmut[i]=this->locuslist[loc].tabsit[sitmut2[i]];
			dna.resize(k+1);
            dna[k]='\0';
		}
		if (debuglevel==10) cout <<dna<<"\n";
		if (debuglevel==9) {            
            cout<<"sitmut2\n";
            for (int i=0;i<this->gt[loc].nmutot;i++) cout<<sitmut2[i]<<"  ";cout<<"\n";

            cout<<"sitmut\n";
            for (int i=0;i<this->gt[loc].nmutot;i++) cout<<this->locuslist[loc].sitmut[i]<<"  ";cout<<"\n";
        }
		delete []sitmut2;
		return dna;
	}

	int cree_haplo(int loc) {
		if (debuglevel==10) cout <<"CREE_HAPLO\n";
		/*if (debuglevel==10) {
		cout << "nbranches = " << this->gt[loc].nbranches << "    nnoeuds=" << gt[loc].nnodes <<"\n";
        
        for (int br=0;br<this->gt[loc].nbranches;br++)
			{cout << "branche " << br << "   bottom=" << this->gt[loc].branches[br].bottom ;
			 cout << "   top=" << this->gt[loc].branches[br].top << "   nmut=" << this->gt[loc].branches[br].nmut;
			 cout << "   length=" << this->gt[loc].branches[br].length << "\n";
			}
		}*/
		vector < vector <int> > ordre;
        if (debuglevel==10) cout<<"avant mise à 10000 des state\n";
		for (int no=0;no<this->gt[loc].nnodes;no++) this->gt[loc].nodes[no].state=10000;
        if (debuglevel==10) cout<<"apres mise à 10000 des state\n";
		int anc=this->gt[loc].nnodes-1;
        if (debuglevel==10) cout<<"anc="<<anc<<"\n";
		if (debuglevel==10) cout<<"kmin = "<<this->locuslist[loc].kmin<<"   kmax = "<<this->locuslist[loc].kmax<<"\n";
		if (this->locuslist[loc].type<5) {
			this->gt[loc].nodes[anc].state=this->locuslist[loc].kmin + (int)(0.5*(this->locuslist[loc].kmax-this->locuslist[loc].kmin));
			//cout << "anc-state = " << this->gt[loc].nodes[anc].state << "\n";
		}
		else {
			this->gt[loc].nodes[anc].state=0;
			this->gt[loc].nodes[anc].dna = init_dnaseq(loc);
			if (debuglevel==10) cout << "locus " << loc  << "\n";
			if (debuglevel==10) cout << "anc-dna = " << this->gt[loc].nodes[anc].dna << "\n";
			//if (debuglevel==10) cout << "anc-dna = " << this->gt[loc].nodes[anc].dna << "\n";
		}
		//anc++;
		int br, numut=-1;
        bool trouve;
		int len;
        if (debuglevel==10) cout <<"avant la boucle while ngenes="<<this->gt[loc].ngenes<<"\n";
		while (anc>=this->gt[loc].ngenes) {
			//anc--;
			if (debuglevel==10) cout << "locus " << loc << "   ngenes = " << this->gt[loc].ngenes << "   anc = " << anc << "\n";
			br=0;
			while (br<this->gt[loc].nbranches) {
                trouve = (this->gt[loc].branches[br].top == anc);
                if ((trouve)and(this->gt[loc].nodes[this->gt[loc].branches[br].bottom].state == 10000)) {
                    this->gt[loc].nodes[this->gt[loc].branches[br].bottom].state = this->gt[loc].nodes[this->gt[loc].branches[br].top].state;
                    if (this->locuslist[loc].type>4) {
                        len=-1;
                        do len++; while (this->gt[loc].nodes[this->gt[loc].branches[br].top].dna[len]!='\0');
                        //cout << "len = " << len << "\n";
                        this->gt[loc].nodes[this->gt[loc].branches[br].bottom].dna.resize(len+1);
                        for (int j=0;j<len+1;j++) this->gt[loc].nodes[this->gt[loc].branches[br].bottom].dna[j]=this->gt[loc].nodes[this->gt[loc].branches[br].top].dna[j];
                        //cout << "node " << this->gt[loc].branches[i].bottom << "   dna = " << this->gt[loc].nodes[this->gt[loc].branches[i].bottom].dna << "\n";
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
        int ind2=0,sa0=0,sa2=this->locuslist[loc].ss[sa0];
		if (this->locuslist[loc].type<5) {    
              for (int no=0;no<this->gt[loc].nnodes;no++) {
                    //if (debuglevel==10) {cout << gt[loc].nodes[no].state << "   ";if ((ind2+1)%20 == 0) cout <<"\n";}
                    ind2++;
                    if (ind2==sa2) {
                        ind2=0;sa0++;sa2=this->locuslist[loc].ss[sa0];
                    }
              }
        }
		else {for (int no=0;no<this->gt[loc].nnodes;no++) if (debuglevel==10) cout << gt[loc].nodes[no].dna << "   ";}
		if (debuglevel==10) cout << "\n";
		int sa,ind;
		if (debuglevel==10) cout<< "tirage au hasard des gènes avant attribution aux individus\n";
		ordre.resize(this->data.nsample);ind=0;
        if (debuglevel==10) cout<<"apres resize de ordre\n";
		for (sa=0;sa<this->data.nsample;sa++) {
			ordre[sa] = melange(this->mw,this->locuslist[loc].ss[sa]);
			if (sa>0) {for (int i=0;i<this->locuslist[loc].ss[sa];i++) ordre[sa][i]+=ind;}
			ind +=this->locuslist[loc].ss[sa];
		}
		if (debuglevel==10) cout<<"apres ordre\n";
        this->locuslist[loc].samplesize =new int[this->data.nsample];
        for (int sa=0;sa<this->data.nsample;sa++) this->locuslist[loc].samplesize[sa] =this->locuslist[loc].ss[sa];
		if (this->locuslist[loc].type<5) {       //MICROSAT
			this->locuslist[loc].haplomic = new int*[this->data.nsample];
			for (sa=0;sa<this->data.nsample;sa++) this->locuslist[loc].haplomic[sa] = new int [this->locuslist[loc].ss[sa]];
			sa=0;ind=0;
			for (int i=0;i<this->gt[loc].ngenes;i++) {
				if (this->gt[loc].nodes[ordre[sa][ind]].state == 10000) return 2;
				this->locuslist[loc].haplomic[sa][ind] = this->gt[loc].nodes[ordre[sa][ind]].state;
				ind++;if (ind==this->locuslist[loc].ss[sa]) {sa++;ind=0;}
			}
			if (debuglevel==10) cout<<"apres repartition dans le sample 0\n";
			//for (int i=0;i<this->locuslist[loc].ss[0];i++) {cout <<this->locuslist[loc].haplomic[0][i]<<"  ";if ((i+1)%20 == 0) cout<<"\n";}
		}
		else {									//DNA SEQUENCES
			this->locuslist[loc].haplodna = new char**[this->data.nsample];
			for (sa=0;sa<this->data.nsample;sa++) this->locuslist[loc].haplodna[sa] = new char* [this->locuslist[loc].ss[sa]];
			sa=0;ind=0;
			for (int i=0;i<this->gt[loc].ngenes;i++) {
				if (this->gt[loc].nodes[ordre[sa][ind]].state == 10000) {
					//cout << "nbranches = " << this->gt[loc].nbranches << "    nnoeuds=" << gt[loc].nnodes <<"\n";
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
					return 2;
				}
				this->locuslist[loc].haplodna[sa][ind] = str2char(this->gt[loc].nodes[ordre[sa][ind]].dna);
				ind++;if (ind==this->locuslist[loc].ss[sa]) {sa++;ind=0;}
				}
		}
		return 0;
	}

	int dosimulpart(int numscen,bool usepriorhist, bool usepriormut){
                //if (trace) cout<<"debut de dosimulpart\n";fflush(stdin);
		vector <int> simulOK;
		int *emptyPop,loc;
		bool treedone;
                int locus,sa,indiv,nuc;
		simulOK.resize(this->nloc);
		GeneTreeC GeneTreeY, GeneTreeM;
        if (debuglevel==10) cout<<"avant draw scenario\n";fflush(stdin);
		this->drawscenario(numscen);
		if (debuglevel==10) cout <<"avant setHistparamValue\n";fflush(stdin);
		this->setHistParamValue(usepriorhist);
		if (debuglevel==10) cout << "apres setHistParamValue\n";fflush(stdin);
		//if (trace) cout<<"scen.nparam = "<<this->scen.nparam<<"\n";
		//if (trace) for (int k=0;k<this->scen.nparam;k++){
		//         	cout << this->scen.histparam[k].value << "   ";fflush(stdin);}
		this->setSequence();
		if (debuglevel==10) cout <<"apres setSequence\n";
		bool gtYexist=false, gtMexist=false;
		this->gt = new GeneTreeC[this->nloc];
		emptyPop = new int[this->scen.popmax+1];
		//cout << "particule " << ipart <<"   nparam="<<this->scen.nparam<<"\n";
		//for (int k=0;k<this->scen.nparam;k++){
		//	cout << this->scen.histparam[k].value << "   ";
		//}
		//cout << "\n";
		setMutParammoyValue(usepriormut);
        if (debuglevel==10) cout<<"nloc="<<this->nloc<<"\n";fflush(stdin);
		for (loc=0;loc<this->nloc;loc++) {
                        //if (trace) cout<<"debut de la boucle du locus "<<loc<<"\n";fflush(stdin);
			if (this->locuslist[loc].groupe>0) {
				setMutParamValue(loc);
				if (this->locuslist[loc].type >4) {
					comp_matQ(loc);
					}
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
					if (debuglevel==10) cout << "initialisation de l'arbre du locus " << loc  << "    ngenes="<< this->gt[loc].ngenes<< "   nseq="<< this->nseq <<"\n";
					for (int p=0;p<this->scen.popmax+1;p++) {emptyPop[p]=1;} //True
					for (int iseq=0;iseq<this->nseq;iseq++) {
						if (debuglevel==10) {
                            cout << "traitement de l element de sequence " << iseq << "    action= "<<this->seqlist[iseq].action;
                            
                           if (this->seqlist[iseq].action == 'C') cout <<"   "<<this->seqlist[iseq].t0<<" - "<<this->seqlist[iseq].t1;
                           else  cout <<"   "<<this->seqlist[iseq].t0;
                           cout<<"    pop="<<this->seqlist[iseq].pop;
                           if (this->seqlist[iseq].action == 'M') cout <<"   pop1="<<this->seqlist[iseq].pop1;
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
					if ((locuslist[loc].type % 5) == 3) {GeneTreeY  = copytree(this->gt[loc]);gtYexist=true;}

					if ((locuslist[loc].type % 5) == 4) {
						//cout <<"avant copie de genetreeM\n";
						GeneTreeM  = copytree(this->gt[loc]);gtMexist=true;}
				}
	/* mutations */
				put_mutations(loc);
				 if (debuglevel==10) cout << "Locus " <<loc << "  apres put_mutations\n";
				simulOK[loc]=cree_haplo(loc);
				if (debuglevel==10) cout << "Locus " <<loc << "  apres cree_haplo   : simOK[loc] ="<<simulOK[loc]<<"\n";fflush(stdin);
                
				locus=loc;
                if (simulOK[loc] != 0) {if (debuglevel==10) cout << "avant break interne\n";break;}
				//cout << "fin du locus " << loc << "   "<< simulOK[loc] << "\n";
			}
		}		//LOOP ON loc
        if (simulOK[locus]==0) {
                //cout<<this->data.nmisshap<<" donnees manquantes et "<<this->data.nmissnuc<<"nucleotides manquants\n";fflush(stdin);
                if (this->data.nmisshap>0) {
                        for (int i=0;i<this->data.nmisshap;i++) {
                              locus=this->data.misshap[i].locus;
                              if (this->locuslist[locus].groupe>0) {
                                      sa=this->data.misshap[i].sample;indiv=this->data.misshap[i].indiv;
                                      if (this->locuslist[locus].type<5) this->locuslist[locus].haplomic[sa][indiv] = MICMISSING;
                                      else                               this->locuslist[locus].haplodna[sa][indiv] = SEQMISSING;
                                      this->locuslist[locus].samplesize[sa]--;
                              }  
                        }
                
                }
                  if (this->data.nmissnuc>0) {
                        for (int i=0;i<this->data.nmissnuc;i++) {
                              locus=this->data.missnuc[i].locus;
                              if (this->locuslist[locus].groupe>0) {
                                      sa=this->data.missnuc[i].sample;indiv=this->data.missnuc[i].indiv;nuc=this->data.missnuc[i].nuc;
                                      this->locuslist[locus].haplodna[sa][indiv][nuc] = NUCMISSING;
                              }  
                        }
                
                }
        }
        //cout<<"avant les delete\n";fflush(stdin);
        delete [] emptyPop;
        delete [] this->seqlist;
		for (int loc=0;loc<this->nloc;loc++) {if (this->locuslist[loc].groupe>0) deletetree(this->gt[loc]);}
		delete [] this->gt;
		if (gtYexist) deletetree(GeneTreeY);
		if (gtMexist) deletetree(GeneTreeM);
		//if (trace) cout << "Fin de dosimulpart \n";
		int simOK=0;for (int loc=0;loc<this->nloc;loc++) {if (this->locuslist[loc].groupe>0) simOK+=simulOK[loc];}
		//cout<<"fin de dosimulpart   simOK="<<simOK<<"\n";fflush(stdin);
        //exit(1);
		return simOK;
	}


/***********************************************************************************************************************/

	void calfreq() {
/*		double **fre;
		fre = new double*[this->nloc];
		for (int loc=0;loc<this->nloc;loc++) {
			fre[loc] = new double[this->locuslist[loc].kmax-this->locuslist[loc].kmin+1];
			for (int i=0;i<this->locuslist[loc].kmax-this->locuslist[loc].kmin+1;i++) fre[loc][i] = 0.0;
			}*/


		int n=0;
		//cout <<"debut de calfreq nloc = "<<this->nloc<<"  nsample = "<<this->data.nsample <<"\n";
		for (int loc=0;loc<this->nloc;loc++) {
		    if (this->locuslist[loc].groupe>0){
			if (this->locuslist[loc].type<5) {
				//cout <<"Locus "<< loc <<"\n";
				this->locuslist[loc].nal = this->locuslist[loc].kmax-this->locuslist[loc].kmin+1;
				this->locuslist[loc].freq = new double* [this->data.nsample];
				//cout <<"nal ="<<this->locuslist[loc].nal<<"\n";
				for (int samp=0;samp<this->data.nsample;samp++) {
					this->locuslist[loc].freq[samp] = new double [this->locuslist[loc].nal];
					for (int i=0;i<this->locuslist[loc].nal;i++) this->locuslist[loc].freq[samp][i]=0.0;
					//cout << this->locuslist[loc].ss[samp] <<"\n";
					//cout <<this->locuslist[loc].samplesize[samp]<< "\n";
					for (int i=0;i<this->locuslist[loc].ss[samp];i++){
						if (this->locuslist[loc].haplomic[samp][i] != MICMISSING) {
							//cout <<"  "<<this->locuslist[loc].haplomic[samp][i];
							this->locuslist[loc].freq[samp][this->locuslist[loc].haplomic[samp][i]-this->locuslist[loc].kmin] +=1.0;
							n++;
						}
					}
					//cout <<" fini\n";
					if (this->locuslist[loc].samplesize[samp]<1) cout << "samplesize["<<loc<<"]["<<samp<<"]="<<this->locuslist[loc].samplesize[samp]<<"\n";
					for (int k=0;k<this->locuslist[loc].kmax-this->locuslist[loc].kmin+1;k++) this->locuslist[loc].freq[samp][k]/=this->locuslist[loc].samplesize[samp];
					//cout << "loc "<<loc <<"  sa "<<samp;
					//for (int k=0;k<this->locuslist[loc].nal;k++) {if (this->locuslist[loc].freq[samp][k]>0.0) cout << "   "<<this->locuslist[loc].freq[samp][k]<<"("<<k+this->locuslist[loc].kmin<<")";}
					//cout << "\n";
					}
			}
		    }
		}
/*		for (int loc=0;loc<this->nloc;loc++) {
			if (this->locuslist[loc].type<5) {
				for (int samp=0;samp<this->data.nsample;samp++) {
					for (int i=0;i<this->locuslist[loc].nal;i++) {
						fre[loc][i] +=this->locuslist[loc].freq[samp][i];
					}
				}
			}
		}
		for (int loc=0;loc<this->nloc;loc++) {
			if (this->locuslist[loc].type<5) {
				cout << "\nLocus "<<loc+1 << "\n    ";
				for (int i=0;i<this->locuslist[loc].nal;i++) {if (fre[loc][i] > 0.0) cout << "    "<<i+this->locuslist[loc].kmin;}
				cout << "\n";
				for (int samp=0;samp<this->data.nsample;samp++) {
					cout <<"Pop " <<samp+1;
					for (int i=0;i<this->locuslist[loc].nal;i++) {if (fre[loc][i] > 0.0) printf(" %6.3f",this->locuslist[loc].freq[samp][i]);}
					cout <<"\n";
				}
			}
		}
		cout <<"fin de calfreq \n";*/
	}

    void liberefreq() {
        for (int loc=0;loc<this->nloc;loc++) {
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

	double cal_pid1p(int gr,int st){
        double pidm=0.0;
        int iloc,kloc,nl=0,nt=0,ni=0;
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
        if (nt>0) return (double)ni/(double)nt;
    }
	
	double cal_nal1p(int gr,int st){
	  
		double nalm=0.0;
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
		if (nl>0) nalm=nalm/(double)nl;
		//cout<<"    nmoy="<<nalm<<  "\n";
		return nalm;
	}

	double cal_nal2p(int gr,int st){
		int k,iloc,kloc,nl=0;
		double nalm=0.0;
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
		if (nl>0) nalm=nalm/(double)nl; 
		return nalm;
	}

	double cal_het1p(int gr,int st){
		double het,hetm=0.0;
		int iloc,kloc,nl=0;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			kloc=this->grouplist[gr].loc[iloc];
			if (this->locuslist[kloc].samplesize[sample]>1){
				het=1.0;
				for (int k=0;k<this->locuslist[kloc].nal;k++) het -= sqr(this->locuslist[kloc].freq[sample][k]);
				het *= ((double)this->locuslist[kloc].samplesize[sample]/((double)this->locuslist[kloc].samplesize[sample]-1));
				hetm += het;
				nl++;
			}
		}
		if (nl>0) hetm=hetm/(double)nl;
		return hetm;
	}

	double cal_het2p(int gr,int st){
		double het,hetm=0.0;
		int iloc,loc,n1,n2,nt,nl=0;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
		int sample1=this->grouplist[gr].sumstat[st].samp1-1;
		for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
			loc=this->grouplist[gr].loc[iloc];
			n1=this->locuslist[loc].samplesize[sample];n2=this->locuslist[loc].samplesize[sample1];nt=n1+n2;
			if (nt>1){
				het=1.0;
				for (int k=0;k<this->locuslist[loc].nal;k++) {het -= sqr(((double)n1*this->locuslist[loc].freq[sample][k]+(double)n2*this->locuslist[loc].freq[sample1][k])/(double)nt);}
				het *= double(nt)/(double)(nt-1);
				hetm +=het;
				nl++;
			}
		}
		if (nl>0) return hetm/(double)nl; else return 0.0;
	}

	double cal_var1p(int gr,int st){
		double s,v,vm=0.0,m,ms;
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
			  m = (double)n;
			  ms = (double)this->locuslist[loc].motif_size;
			  if (n>1){
				  vm+=(v-sqr(s)/m)/(m-1.0)/sqr(ms);
				  nl++;
			  }
		  }
		  if (nl>0) return vm/(double)nl; else return 0.0;
	}

	double cal_var2p(int gr,int st){
		double s,v,vm=0.0,m,ms;
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
			m = (double)n;
			ms = (double)this->locuslist[loc].motif_size;
			if (n>1){
				vm+=(v-sqr(s)/m)/(m-1.0)/sqr(ms);
				nl++;
			}
		}
		if (nl>0) return vm/(double)nl; else return 0.0;
	}

	double cal_mgw1p(int gr,int st){
		double num=0.0,den=0.0;
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
				den += (double)(1+max-min)/double(this->locuslist[loc].motif_size);
		    }
		}
		if (den>0) return num/den; else return 0.0;
	}

	double cal_Fst2p(int gr,int st){
		double s1=0.0,s3=0.0,s1l,s2l,s3l,sni,sni2,sniA,sniAA;
		int al,pop,kpop,i,ig1,ig2,nA,AA,ni;
		double s2A,MSG,MSI,MSP,s2G,s2I,s2P,nc;
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
		            sniA  +=(double)nA;
		            sniAA +=(double)AA;
		            sni    +=(double)ni;
		            sni2  +=sqr((double)ni);
		            s2A   +=sqr((double)nA)/(double)(2*ni);
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
		    s1 += (double)nc*s1l;
		    s3 += (double)nc*s3l;
		}
		if (s3>0.0) return s1/s3; else return 0.0;
	}

	double cal_lik2p(int gr,int st){
  		int pop,nal,k,i,ind,ig1,ig2;
  		double frt,a,b,num_lik,den_lik,lik;
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
				b = 1.0/(double)nal;
				a = 1.0/(double)this->data.nind[sample];
				ind = 0;
			    for (i=0;i<this->locuslist[loc].ss[sample];i++){
					nn=calploidy(loc,sample,ind);
					ind++;
	    			switch (nn)
	    			{ case 1 :  ig1 = this->locuslist[loc].haplomic[sample][i];
								if (ig1!=MICMISSING) {
									num_lik = round(this->locuslist[loc].freq[sample1][ig1-this->locuslist[loc].kmin]*this->locuslist[loc].samplesize[sample1])+b;
									den_lik = (double) (this->locuslist[loc].samplesize[sample1]+1);
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
				                	den_lik = (double) ((this->locuslist[loc].samplesize[sample1]+2)*(this->locuslist[loc].samplesize[sample1]+1));
									lik -= a*log10(num_lik/den_lik);
				                }	
				                break;
	    			}
    			}
		    }
		}
		if (nl>0) return lik/(double)nl; else return 0.0;
	}

	double cal_dmu2p(int gr,int st){
		double *moy,dmu2=0.0,s;
		int iloc,loc,pop,nl=0;
		int sample=this->grouplist[gr].sumstat[st].samp-1;
		int sample1=this->grouplist[gr].sumstat[st].samp1-1;
		moy = new double[2];
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
					moy[kpop]=s/(double)this->locuslist[loc].samplesize[pop];
				}
		    }
		    dmu2 += sqr((moy[1]-moy[0])/(double)this->locuslist[loc].motif_size);
		}
		delete []moy;
		if (nl>0) return dmu2/(double)nl; else return 0.0;
	}

	double cal_das2p(int gr,int st){
		double s=0.0;
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
		if (nl>0) return s/(double)nl; else return 0.0;
	}

	complex<double> pente_lik(int gr,int st, int i0) {
		double a,freq1,freq2,li0,delta,*li;
		int ig1,ig2,ind,loc,iloc,nn;
		li = new double[2];
		StatC stat=this->grouplist[gr].sumstat[st];
		int sample=stat.samp-1;
		int sample1=stat.samp1-1;
		int sample2=stat.samp2-1;
		//cout<<"sample ="<<sample<<"   sample1="<<sample1<<"   sample2="<<sample2<<"\n";
		for (int rep=0;rep<2;rep++) {
			a=0.001*(double)(i0+rep);li[rep]=0.0;
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
		return complex<double>(li0,delta);
	}		
		
	double cal_Aml3p(int gr,int st){
		int i1=1,i2=998,i3;
		double p1,p2,p3,lik1,lik2,lik3;
		complex<double> c;
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
		if (lik1>lik2) return 0.001*(double)i1; else return 0.001*(double)i2;
	}

	bool identseq(int loc, char* seq1, char* seq2) {
	    bool ident=true;
	    int k=0;
	    while ((ident)and(k<this->locuslist[loc].dnavar)) {
	        ident = ((seq1[k]=='N')or(seq2[k]=='N')or(seq1[k]==seq2[k]));
	        if (ident) k++;
	    }
	    return ident;
	}

	double cal_nha1p(int gr,int st){
	    char **haplo;
	    int iloc,kloc,k,j,nhl=0,nhm=0,nl=0;
	    bool trouve,ident;
		double res=0.0;
        int sample=this->grouplist[gr].sumstat[st].samp-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            nhl=0; 
            kloc=this->grouplist[gr].loc[iloc]; 
            if(this->locuslist[kloc].samplesize[sample]>0) {
                nl++;
                if (this->locuslist[kloc].dnavar==0) nhl=1;
                else {
                    haplo = new char*[this->locuslist[kloc].samplesize[sample]]; 
                    for (int i=0;i<this->locuslist[kloc].ss[sample];i++) {
                        if (this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING) ; {
                            if (nhl==0) { 
                                haplo[nhl] = new char[this->locuslist[kloc].dnavar+1];
                                for (int k=0;k<this->locuslist[kloc].dnavar;k++) haplo[nhl][k] = this->locuslist[kloc].haplodnavar[sample][i][k];
                                haplo[nhl][this->locuslist[kloc].dnavar]='\0';
                                nhl++;
                                //cout<<"nl="<<nl<<"   nhl="<<nhl<<"    "<<haplo[nhl-1]<<"\n";
                            } else {
                                trouve=false;j=0;
                                while ((not trouve)and(j<nhl)) {
                                	trouve=identseq(kloc,haplo[j],this->locuslist[kloc].haplodnavar[sample][i]);
                                	if (not trouve) j++;
                                }
                                if (trouve) {
                                	for (int k=0;k<this->locuslist[kloc].dnavar;k++) if (haplo[j][k]=='N') haplo[nhl][k] = this->locuslist[kloc].haplodnavar[sample][i][k];
                                } else {
                                haplo[nhl] = new char[this->locuslist[kloc].dnavar+1];
                                for (int k=0;k<this->locuslist[kloc].dnavar;k++) haplo[nhl][k] = this->locuslist[kloc].haplodnavar[sample][i][k]; 
                                haplo[nhl][this->locuslist[kloc].dnavar]='\0';
                                nhl++;
                                //cout<<"nl="<<nl<<"   nhl="<<nhl<<"    "<<haplo[nhl-1]<<"\n";                                    
                                }
                            }
                        }    
                    }
                }
            }            
            nhm += nhl;
        }
        if (nl>0) res=(double)nhm/(double)nl;
        return res;
    }
                

	double cal_nss1p(int gr,int st){
        int iloc,kloc,k,j,j0,nssl,nssm=0,nl=0;
        bool trouve,ident;
		double res=0.0;
        char c0;
        int sample=this->grouplist[gr].sumstat[st].samp-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            nssl=0; 
            kloc=this->grouplist[gr].loc[iloc]; 
            if(this->locuslist[kloc].samplesize[sample]>0) {
                nl++;
                if (not this->locuslist[kloc].dnavar==0) {
                    for (j=0;j<this->locuslist[kloc].dnavar;j++) {
                        c0='\0';ident=true;
                        for (int i=0;i<this->locuslist[kloc].ss[sample];i++) {
                            if ((this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING)and(this->locuslist[kloc].haplodnavar[sample][i][j]!='N')) {
                                if (c0=='\0') c0=this->locuslist[kloc].haplodnavar[sample][i][j];
                                else ident=(c0==this->locuslist[kloc].haplodnavar[sample][i][j]);
                                //if (not ident) cout<<"nucleotide "<<j<<"   c0="<<c0<<"   c1="<<this->locuslist[kloc].haplodnavar[sample][i][j]<<"   i="<<i<<"\n";
                            }
                            if (not ident) break;
                       }
                       if (not ident) nssl++;
                    }
                }
            }
            //cout<<"   locus "<<kloc<< "   nssl = "<<nssl<<"\n";
            nssm += nssl;
        }
        if (nl>0) res=(double)nssm/(double)nl;
 		return res;
	}

	double cal_mpd1p(int gr,int st){
        int iloc,kloc,k,j,npdl,nd=0,di,nl=0;;
		double npdm=0.0,res=0.0;
        int sample=this->grouplist[gr].sumstat[st].samp-1;
        for (iloc=0;iloc<this->grouplist[gr].nloc;iloc++){
            npdl=0;nd=0;
            kloc=this->grouplist[gr].loc[iloc]; 
            for (int i=0;i<this->locuslist[kloc].ss[sample]-1;i++) {
                for (int j=i+1;j<this->locuslist[kloc].ss[sample];j++) {
                    if ((this->locuslist[kloc].haplodna[sample][i]!=SEQMISSING)and(this->locuslist[kloc].haplodna[sample][j]!=SEQMISSING)) {
                        nd++;
                        di=0;
                        for (k=0;k<this->locuslist[kloc].dnavar;k++) {
                            if ((this->locuslist[kloc].haplodnavar[sample][i][k]!='N')and(this->locuslist[kloc].haplodnavar[sample][j][k]!='N')and(this->locuslist[kloc].haplodnavar[sample][i][k]!=this->locuslist[kloc].haplodnavar[sample][j][k])) di++;
                        }
                        npdl +=di;
                    }
                }
            }
            if (nd>0){npdm +=(double)npdl/(double)nd;nl++;}
        }
        if (nl>0) res=(double)npdm/(double)nl;
		return res;

	}

	double cal_vpd1p(int gr,int st){
		double res=0.0;
		return res;

	}

	double cal_dta1p(int gr,int st){
		double res=0.0;
		return res;

	}

	double cal_pss1p(int gr,int st){
		double res=0.0;
		return res;

	}

	double cal_mns1p(int gr,int st){
		double res=0.0;
		return res;

	}

	double cal_vns1p(int gr,int st){
		double res=0.0;
		return res;

	}

	double cal_nha2p(int gr,int st){
		double res=0.0;
		return res;

	}

	double cal_nss2p(int gr,int st){
		double res=0.0;
		return res;

	}

	double cal_mpw2p(int gr,int st){
		double res=0.0;
		return res;

	}

	double cal_mpb2p(int gr,int st){
		double res=0.0;
		return res;

	}

	double cal_fst2p(int gr,int st){
		double res=0.0;
		return res;

	}

	double cal_aml3p(int gr,int st){
		double res=0.0;
		return res;

	}

	void cal_numvar(int gr) {
        int i,j,k,ns,pop,j0,lonseq;
        int locus,nlocs=this->grouplist[gr].nloc;
        char *site;
        bool ident;
        //this->nvar = new int[nlocs];
        //this->numvar = new int*[nlocs];
        vector <int> nuvar;
        //cout <<"debut de cal_numvar dnatrue ="<<this->dnatrue<<"\n";
		if (not this->dnatrue) {
            for (int iloc=0;iloc<nlocs;iloc++) {
                locus= this->grouplist[gr].loc[iloc];
                this->locuslist[locus].haplodnavar = new char**[data.nsample];
            	for (pop=0;pop<this->data.nsample;pop++) {
               	    this->locuslist[locus].haplodnavar[pop] = new char*[this->locuslist[locus].ss[pop]];
                	for (i=0;i<this->locuslist[locus].ss[pop];i++) {
                   	    this->locuslist[locus].dnavar = strlen(this->locuslist[locus].haplodna[pop][i]);
                   	    this->locuslist[locus].haplodnavar[pop][i] = new char[this->locuslist[locus].dnavar];
                    	for (int k=0;k<this->locuslist[locus].dnavar;k++) this->locuslist[locus].haplodnavar[pop][i][k] = this->locuslist[locus].haplodna[pop][i][k];
                    }
                }
            //cout<<"locus "<<locus<<"   dnavar = "<<this->locuslist[locus].dnavar<<"\n";
            }
		} else {
            for (int iloc=0;iloc<nlocs;iloc++) {
                locus= this->grouplist[gr].loc[iloc];
                this->locuslist[locus].haplodnavar = new char**[data.nsample];
                ns=0; for (pop=0;pop<this->data.nsample;pop++) ns += this->locuslist[locus].ss[pop];
                site = new char[ns];
                this->locuslist[locus].dnavar=0;
                /*lonseq=0;
                for (pop=0;pop<this->data.nsample;pop++) {
                    for (i=0;i<this->locuslist[locus].ss[pop];i++) { 
                        lonseq = strlen(this->locuslist[locus].dnalength);
                        if (lonseq>0) break;
                    }
                    if (lonseq>0) break;
                }*/
                if (this->locuslist[locus].dnalength>0) {
                    //cout <<"longseq = "<<this->locuslist[locus].dnalength<<"\n";;
                    for (k=0;k<this->locuslist[locus].dnalength;k++) {
                        j=-1;
                        for (pop=0;pop<this->data.nsample;pop++) {
                            for (i=0;i<this->locuslist[locus].ss[pop];i++) {
                                j++;
                                if (this->locuslist[locus].haplodna[pop][i] == '\0') site[j]='N';
                                else site[j] = this->locuslist[locus].haplodna[pop][i][k];               
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
                    this->locuslist[locus].haplodnavar[pop] = new char*[this->locuslist[locus].ss[pop]];
                    for (i=0;i<this->locuslist[locus].ss[pop];i++) {
                        this->locuslist[locus].haplodnavar[pop][i] = new char[this->locuslist[locus].dnavar+1];
                        for (int k=0;k<this->locuslist[locus].dnavar;k++) this->locuslist[locus].haplodnavar[pop][i][k] = this->locuslist[locus].haplodna[pop][i][nuvar[k]];
                        this->locuslist[locus].haplodnavar[pop][i][this->locuslist[locus].dnavar]='\0';
                        //cout<<"pop="<<pop<<"   ind="<<i<<"   "<<this->locuslist[locus].haplodnavar[pop][i]<<"\n";
                        //for(int k=0;k<this->locuslist[locus].dnavar;k++) cout<<"   " <<nuvar[k]; cout<<"\n";
                    }
                }
		    	if (not nuvar.empty()) nuvar.clear();
		    	delete []site;
		    }
		}
		//cout <<"fin de cal_numvar\n";
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
	void docalstat(int gr) {
//		cout << "avant calfreq\n";
		if (this->grouplist[gr].type == 0)  calfreq();
                else this->cal_numvar(gr);
//		cout << "apres calfreq\n";
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
			}
			//cout << "stat["<<st<<"]="<<this->grouplist[gr].sumstat[st].val<<"\n";fflush(stdin);
		}
		if (this->grouplist[gr].type == 0) liberefreq();
	}

};

