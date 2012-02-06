/*
 * history.cpp
 *
 *  Created on: 8 déc. 2011
 *      Author: ppudlo
 */
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
using namespace std;

#include "randomgenerator.h"
#include "mesutils.h"
#include "history.h"


vector <string> histparname;
vector <int> histparcat;



/* Méthodes de PriorC */

void PriorC::ecris(){
	cout <<"    loi="<<this->loi<<"   min="<<this->mini<<"   max="<<this->maxi<<"   ndec="<<this->ndec;
	if (this->loi=="GA") cout <<"    shape="<<this->sdshape;
	if (this->constant) cout<<"   constant"; else cout<<"   non constant";
	if (this->fixed) cout<<"   fixed\n";else cout<<"   non fixed\n";
}

double PriorC::drawfromprior(MwcGen & mw){
	    double r;
    if (this->mini==this->maxi) return this->mini;
    if (this->loi=="UN") return mw.gunif(this->mini,this->maxi);
    if (this->loi=="LU") return mw.glogunif(this->mini,this->maxi);
    if (this->loi=="NO") {
      do {r = mw.gnorm(this->mean,this->sdshape);}
      while ((r<this->mini)or(r>this->maxi));
      return r;
    }
    if (this->loi=="LN") {
      do {r = mw.glognorm(this->mean,this->sdshape);}
      while ((r<this->mini)or(r>this->maxi));
      return r;
    }
    if (this->loi=="GA") {
      if (this->mean<1E-12) return 0;
      if (this->sdshape<1E-12) return this->mean;
      if (this->maxi<1E-12) return this->maxi;
      do {r = mw.ggamma3(this->mean,this->sdshape);}
      while ((r<this->mini)or(r>this->maxi));
      return r;
    }
    return -1.0;
}

void PriorC::readprior(string ss){
    string s1,*sb;
    int j;
    s1 = ss.substr(3,ss.length()-4);
    sb = splitwords(s1,",",&j);
    this->mini=atof(sb[0].c_str());
    this->maxi=atof(sb[1].c_str());
            this->ndec=ndecimales(this->mini,this->maxi);
            //cout << "ndec="<<this->ndec<<"   (mini="<<this->mini<<"   maxi="<<this->maxi<<"\n";
    if (ss.find("UN[")!=string::npos) {this->loi="UN";}
    else if (ss.find("LU[")!=string::npos) {this->loi="LU";}
    else if (ss.find("NO[")!=string::npos) {this->loi="NO";this->mean=atof(sb[2].c_str());this->sdshape=atof(sb[3].c_str());}
    else if (ss.find("LN[")!=string::npos) {this->loi="LN";this->mean=atof(sb[2].c_str());this->sdshape=atof(sb[3].c_str());}
    else if (ss.find("GA[")!=string::npos) {this->loi="GA";this->mean=atof(sb[2].c_str());this->sdshape=atof(sb[3].c_str());}
    if (this->maxi==0.0) this->constant=true;
    else this->constant = ((this->maxi-this->mini)/this->maxi<0.000001);
	this->fixed=false;
    //cout<<ss<<"   ";
    //if (this->constant) cout<<"constant\n"; else cout<<"variable\n";
    delete []sb;
}

/* Méthodes de ConditionC */

void ConditionC::ecris(){
	cout<<this->param1<<"  "<<this->operateur<<"  "<<this->param2<<"\n";
}
void ConditionC::readcondition(string ss){
		//cout<<"condition : "<<ss<<"\n";
	if (ss.find(">=")!=string::npos){
		this->operateur=">=";this->param1=ss.substr(0,ss.find(">="));this->param2=ss.substr(ss.find(">=")+2,ss.length()-(ss.find(">=")+2));}
	else if (ss.find("<=")!=string::npos){
		this->operateur="<=";this->param1=ss.substr(0,ss.find("<="));this->param2=ss.substr(ss.find("<=")+2,ss.length()-(ss.find("<=")+2));}
	if ((ss.find(">")!=string::npos)and(ss.find(">=")==string::npos)){
		this->operateur=">";this->param1=ss.substr(0,ss.find(">"));this->param2=ss.substr(ss.find(">")+1,ss.length()-(ss.find(">")+1));}
	else if ((ss.find("<")!=string::npos)and(ss.find("<=")==string::npos)){
		this->operateur="<";this->param1=ss.substr(0,ss.find("<"));this->param2=ss.substr(ss.find("<")+1,ss.length()-(ss.find("<")+1));}
	//this->ecris();
}




/* méthodes de EventC  */

bool operator< (const EventC & lhs, const EventC & rhs)
{
  return lhs.time < rhs.time;
}


void EventC::ecris(){
  string sstime;
  if (this->time!=-9999)
    sstime=IntToString(this->time);
  else {
    /*cout<<this->stime<<"\n";*/
    sstime = this->stime;
  }
  //cout <<"event.action"<<this->action<<"\n";
  if (this->action=='V') cout<<"    "<<sstime<<"   VarNe  pop="<<this->pop<<"   "<<this->sNe<<"\n";
  else if (this->action=='M') cout<<"    "<<sstime<<"   merge  pop="<<this->pop<<"   pop1="<<this->pop1<<"\n";
  else if (this->action=='S') cout<<"    "<<sstime<<"   split  pop="<<this->pop<<"   pop1="<<this->pop1<<"   pop2="<<this->pop2<<"   "<<this->sadmixrate<<"\n";
  else if (this->action=='E') cout<<"    "<<sstime<<"   samp   pop="<<this->pop<<"\n";
  else if (this->action=='R') cout<<"    "<<sstime<<"  refsamp pop="<<this->pop<<"    nindref="<<this->nindMref+this->nindFref<<"\n";
}


/* Méthodes de HistParameterC */

void HistParameterC::ecris(){
    cout<<"    name="<<this->name<<"   val="<<this->value<<"   category="<<this->category<<"\n";
    prior.ecris();
  }

/* Méthodes de LocusGroupC */

void LocusGroupC::libere(){
    delete []loc;
    for (int i=0;i<this->nstat;i++) free(&(this->sumstat[i]));
    delete []this->sumstat;

  }



/**
 * Copie du contenu d'une classe ScenarioC
 */
ScenarioC::ScenarioC(ScenarioC const & source) {
  //cout<<"debut de copyscenario\n";
  this->prior_proba = source.prior_proba;
  this->number = source.number;
  this->popmax = source.popmax;
  this->npop = source.npop;
  this->nsamp = source.nsamp;
  this->nparam = source.nparam;
  this->nparamvar = source.nparamvar;
  this->nevent = source.nevent;
  this->nn0 = source.nn0;
  this->nconditions = source.nconditions;
  //cout<<"    fin des copy simples  this->nevent ="<<this->nevent<<"\n";
  this->event = new EventC[this->nevent];
  for (int i=0;i<this->nevent;i++) this->event[i] = source.event[i]; // copyevent supprimé (PP)
  // cout<<"   apres copyevent\n";
  this->ne0 = new Ne0C[this->nn0];
  for (int i=0;i<this->nn0;i++) this->ne0[i] = source.ne0[i];
  //  cout<<"   apres copyne0\n";
  this->time_sample = new int[this->nsamp];
  for (int i=0;i<this->nsamp;i++) this->time_sample[i] = source.time_sample[i];
  // cout<<"   apres copytime_sample\n";
  this->histparam = new HistParameterC[this->nparam];
  for (int i=0;i<this->nparam;i++) {this->histparam[i] = source.histparam[i];/*cout<<this->histparam[i].name<<"\n"<<flush;*/}
  //  cout<<"   apres copyhistparam\n";
  this->paramvar = new double[this->nparamvar];
  for (int i=0;i<this->nparamvar;i++) {this->paramvar[i] = source.paramvar[i];/*cout<<this->histparam[i].name<<"\n"<<flush;*/}
  //  cout<<"   apres copyparamvar\n";
  if (this->nconditions>0) {
    this->condition = new ConditionC[this->nconditions];
    for (int i=0;i<this->nconditions;i++) this->condition[i] = source.condition[i];
  }
}


ScenarioC & ScenarioC::operator= (ScenarioC  const & source) {
  if (this == &source)
    return *this;

  if( paramvar != NULL) delete [] paramvar;
  if( time_sample != NULL) delete [] time_sample;
  if( event != NULL) delete [] event;
  if( ne0 != NULL) delete [] ne0;
  if( histparam != NULL) delete [] histparam;
  if( condition != NULL) delete [] condition;



  this->prior_proba = source.prior_proba;
  this->number = source.number;
  this->popmax = source.popmax;
  this->npop = source.npop;
  this->nsamp = source.nsamp;
  this->nparam = source.nparam;
  this->nparamvar = source.nparamvar;
  this->nevent = source.nevent;
  this->nn0 = source.nn0;
  this->nconditions = source.nconditions;
  //cout<<"    fin des copy simples  this->nevent ="<<this->nevent<<"\n";
  this->event = new EventC[this->nevent];
  for (int i=0;i<this->nevent;i++) this->event[i] = source.event[i]; // copyevent supprimé (PP)
  // cout<<"   apres copyevent\n";
  this->ne0 = new Ne0C[this->nn0];
  for (int i=0;i<this->nn0;i++) this->ne0[i] = source.ne0[i];
  //  cout<<"   apres copyne0\n";
  this->time_sample = new int[this->nsamp];
  for (int i=0;i<this->nsamp;i++) this->time_sample[i] = source.time_sample[i];
  // cout<<"   apres copytime_sample\n";
  this->histparam = new HistParameterC[this->nparam];
  for (int i=0;i<this->nparam;i++) {this->histparam[i] = source.histparam[i];/*cout<<this->histparam[i].name<<"\n"<<flush;*/}
  //  cout<<"   apres copyhistparam\n";
  this->paramvar = new double[this->nparamvar];
  for (int i=0;i<this->nparamvar;i++) {this->paramvar[i] = source.paramvar[i];/*cout<<this->histparam[i].name<<"\n"<<flush;*/}
  //  cout<<"   apres copyparamvar\n";
  if (this->nconditions>0) {
    this->condition = new ConditionC[this->nconditions];
    for (int i=0;i<this->nconditions;i++) this->condition[i] = source.condition[i];
  }
  return *this;
};


void ScenarioC::ecris() {
    cout <<"scenario "<<this->number<<"   ("<<this->prior_proba<<")\n";
    cout <<"    nevent="<<this->nevent<<"   nne0="<<nn0<<"\n";
    cout <<"    nsamp ="<<this->nsamp<<"   npop="<<this->npop<<"   popmax="<<this->popmax<< "\n";
    cout <<"    nparam="<<this->nparam<<"    nparamvar="<<this->nparamvar<<"\n";
    for (int i=0;i<this->nsamp;i++) cout<<"    samp/refsamp "<<i+1<<"    time_sample = "<<this->time_sample[i]<<"\n";
    cout<<"    events:\n";
    for (int i=0;i<this->nevent;i++) this->event[i].ecris();
    cout<<"    histparam:\n";
    for (int i=0;i<this->nparam;i++) this->histparam[i].ecris();
    cout<<"    nparamvar = "<<this->nparamvar<<"\n";cout<<" paramvar : ";
    for (int i=0;i<this->nparamvar;i++) cout<<this->paramvar[i]<<"   ";cout<<"\n";
    cout <<"   nconditions="<<this->nconditions<<"\n";
    if (this->nconditions>0) for (int i=0;i<this->nconditions;i++) this->condition[i].ecris();
  }

string ScenarioC::checklogic() {
	string message;
	int maxpop=0;
	for (int ievent=0;ievent<this->nevent;ievent++) {
		//cout<<"\n";
		//this->event[ievent].ecris();
		//cout<<"\n";
		//cout<<ievent<<"   maxpop="<<maxpop<<"\n";
		//cout<<this->event[ievent].pop;
		if (this->event[ievent].pop>maxpop) maxpop = this->event[ievent].pop;
		if ((this->event[ievent].action=='M')or(this->event[ievent].action=='S')) {
			if (this->event[ievent].pop1>maxpop) maxpop = this->event[ievent].pop1;
			//cout<<"   "<<this->event[ievent].pop1;
		}
		if (this->event[ievent].action=='S') {
			if (this->event[ievent].pop2>maxpop) maxpop = this->event[ievent].pop2;
			//cout<<"   "<<this->event[ievent].pop2;
		}
	}
	cout<<"nsamp="<<this->nsamp<<"    maxpop="<<maxpop<<"\n";
	//if (maxpop != this->nsamp)
	//	return "the number of population sizes in line 1 is different from the number of populations found in subsequent lines";
	bool *popexist;
	popexist = new bool[maxpop+1];
	for (int i=0;i<maxpop+1;i++) popexist[i] = false;
	for (int ievent=0;ievent<this->nevent;ievent++) {
		//this->event[ievent].ecris();
		//for (int i=1;i<=maxpop;i++) cout<<popexist[i]<<"   ";cout<<"\n";
		if ((this->event[ievent].action=='E')or(this->event[ievent].action=='R')) popexist[this->event[ievent].pop] = true;
		if (this->event[ievent].action=='M') {
			if (this->event[ievent].pop == this->event[ievent].pop1){
				delete []popexist;
				return "in line "+IntToString(ievent+2)+" population " +IntToString(this->event[ievent].pop)+" merges with itself";
			}
			if (not popexist[this->event[ievent].pop1]){
				delete []popexist;
				return "in line "+IntToString(ievent+2)+" population " +IntToString(this->event[ievent].pop1)+" does not exist anymore";
			}
			if (not popexist[this->event[ievent].pop]){
				delete []popexist;
				return "in line "+IntToString(ievent+2)+" population " +IntToString(this->event[ievent].pop)+" does not exist anymore";
			}
			popexist[this->event[ievent].pop1] = false;
		}
		if (this->event[ievent].action=='S') {
			if ((this->event[ievent].pop == this->event[ievent].pop1)or(this->event[ievent].pop == this->event[ievent].pop2)or(this->event[ievent].pop1 == this->event[ievent].pop2)){
				delete []popexist;
				return "in line "+IntToString(ievent+2)+" admixture must occur between three different populations";
			}
			if (not popexist[this->event[ievent].pop1]){
				delete []popexist;
				return "in line "+IntToString(ievent+2)+" population " +IntToString(this->event[ievent].pop1)+" does not exist anymore";
			}
			if (not popexist[this->event[ievent].pop2]){
				delete []popexist;
				return "in line "+IntToString(ievent+2)+" population " +IntToString(this->event[ievent].pop2)+" does not exist anymore";
			}
			if (not popexist[this->event[ievent].pop]){
				delete []popexist;
				return "in line "+IntToString(ievent+2)+" population " +IntToString(this->event[ievent].pop)+" does not exist anymore";
			}
			popexist[this->event[ievent].pop] = false;
			popexist[this->event[ievent].pop1] = true;
			popexist[this->event[ievent].pop2] = true;
		}
		//for (int i=1;i<=maxpop;i++) cout<<popexist[i]<<"   ";cout<<"\n\n";
	}
	int np=0;
	vector <int> numpop;
	for (int i=1;i<maxpop+1;i++) if (popexist[i]) {np++;numpop.push_back(i);}
	delete []popexist;
	if (np>1){
		message = "more than one population (";
		for (int i = 0; i < (int)numpop.size(); i++) {
			message = message + IntToString(numpop[i]);
			if (i+1 < (int)numpop.size()) message = message+",";
		}
		message = message + ") are ancestral";
		numpop.clear();
		return message;
	}
	numpop.clear();
	return "";
}

void ScenarioC::detparam(string s,int cat) {
  string s1;
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
  for (i = 0; i < (int)ss.size(); i++) {
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


string ScenarioC::read_events(int nl,string *ls) {
    string *ss,sevent,ligne;
    int n;
	size_t absent=string::npos;
	ligne=majuscules(ls[0]);
	if ((ligne.find("SAMPLE")!=absent)or(ligne.find("REFSAMPLE")!=absent)or(ligne.find("MERGE")!=absent)or(ligne.find("VARNE")!=absent)or(ligne.find("SPLIT")!=absent)or(ligne.find("SEXUAL")!=absent)) {
		return "the first line must provide effective population size(s)";
	}
    ss = splitwords(ls[0]," ",&n);
    this->nn0=n;this->nparam=0;
    this->ne0 = new Ne0C[this->nn0];
    for (int i=0;i<this->nn0;i++) {
      if (atoi(ss[i].c_str())==0) {
	this->ne0[i].name = ss[i];
	this->ne0[i].val=-1;this->detparam(ss[i],0);
      } else  {
	this->ne0[i].name = string("");
	this->ne0[i].val = atoi(ss[i].c_str());
      }
    }
    this->nevent = nl-1;
    this->event = new EventC[nl-1];
    this->nsamp=0;this->npop=this->nn0;this->popmax=this->nn0;
    delete []ss;
    for (int i=0;i<nl-1;i++) {
      ss = splitwords(ls[i+1]," ",&n);
      if (ss[0]=="0") {this->event[i].time=0;}
      else if (atoi(ss[0].c_str())==0) {
	this->event[i].time=-9999;
	this->event[i].stime = ss[0];
	//cout<<"avant this->detparam("<<ss[0]<<",1)\n";
	this->detparam(ss[0],1);
	//cout<<"apres this->detparam("<<ss[0]<<",1)\n";
      }
      else {this->event[i].time=atoi(ss[0].c_str());}
      sevent=majuscules(ss[1]);
	  if ((sevent!="SAMPLE")and(sevent!="REFSAMPLE")and(sevent!="MERGE")and(sevent!="VARNE")and(sevent!="SPLIT")and(sevent!="SEXUAL")) {
		  return "unrecognized keyword at line "+IntToString(i+2);
	}
      //cout<<"apres majuscules(ss[1])";
      if (sevent=="SAMPLE") {
	if (n>5) return "too many words at line "+IntToString(i+2);
	if ((n<3)or(n==4))return "not enough words at line "+IntToString(i+2);
	this->event[i].action='E';
	this->event[i].pop=atoi(ss[2].c_str());
	if ((this->event[i].pop<1)or(this->event[i].pop>this->nn0))
		return "population number must be a positive integer at most equal to "+IntToString(this->nn0)+ " at line "+IntToString(i+2);
	this->nsamp++;
	this->event[i].sample=this->nsamp;
	this->event[i].nindMref=0;this->event[i].nindFref=0;
	if (n==5) {this->event[i].nindMref=atoi(ss[3].c_str());this->event[i].nindFref=atoi(ss[4].c_str());}
	//cout <<this->event[i].time<<"  SAMPLE"<<"   "<<this->event[i].pop<<"\n";
	cout<<"SAMPLE   nindref="<<this->event[i].nindMref+this->event[i].nindFref<<"\n";

      } else if (sevent=="REFSAMPLE") {
	if (n>5) return "too many words at line "+IntToString(i+2);
	if (n<5)return "not enough words at line "+IntToString(i+2);
	this->event[i].action='R';
	this->event[i].pop=atoi(ss[2].c_str());
	if ((this->event[i].pop<1)or(this->event[i].pop>this->nn0))
		return "population number must be a positive integer at most equal to "+IntToString(this->nn0)+ " at line "+IntToString(i+2);
	this->nsamp++;
	this->event[i].sample=this->nsamp;
	this->event[i].nindMref=atoi(ss[3].c_str());
	this->event[i].nindFref=atoi(ss[4].c_str());
	cout<<"REFSAMPLE   nindref="<<this->event[i].nindMref+this->event[i].nindFref<<"\n";

      } else if (sevent=="MERGE") {
	if (n>4) return "too many words at line "+IntToString(i+2);
	if (n<4) return "not enough words at line "+IntToString(i+2);
	this->event[i].action='M';
	this->event[i].pop=atoi(ss[2].c_str());
	if ((this->event[i].pop<1)or(this->event[i].pop>this->nn0))
		return "population number must be a positive integer at most equal to "+IntToString(this->nn0)+ " at line "+IntToString(i+2);
	this->event[i].pop1=atoi(ss[3].c_str());
	if ((this->event[i].pop1<1)or(this->event[i].pop1>this->nn0))
		return "population number must be a positive integer at most equal to "+IntToString(this->nn0)+ " at line "+IntToString(i+2);
	//cout <<this->event[i].stime<<"  MERGE"<<"   "<<this->event[i].pop<<"   "<<this->event[i].pop1<<"\n";

      } else if (sevent=="SPLIT") {
	if (n>6) return "too many words at line "+IntToString(i+2);
	if (n<6) return "not enough words at line "+IntToString(i+2);
	this->event[i].action='S';
	this->event[i].pop=atoi(ss[2].c_str());
	if ((this->event[i].pop<1)or(this->event[i].pop>this->nn0))
		return "population number must be a positive integer at most equal to "+IntToString(this->nn0)+ " at line "+IntToString(i+2);
	this->event[i].pop1=atoi(ss[3].c_str());
	if ((this->event[i].pop1<1)or(this->event[i].pop1>this->nn0))
		return "population number must be a positive integer at most equal to "+IntToString(this->nn0)+ " at line "+IntToString(i+2);
	this->event[i].pop2=atoi(ss[4].c_str());
	if ((this->event[i].pop2<1)or(this->event[i].pop2>this->nn0))
		return "population number must be a positive integer at most equal to "+IntToString(this->nn0)+ " at line "+IntToString(i+2);
	if (atof(ss[5].c_str())!=0.0) this->event[i].admixrate=atof(ss[5].c_str());
	else {
	  this->event[i].admixrate=-1.0;
	  // this->event[i].ladmixrate=ss[5].length()+1;
	  // this->event[i].sadmixrate=new char[this->event[i].ladmixrate];
	  this->event[i].sadmixrate = ss[5];
	  this->detparam(ss[5],2);
	}
	//cout <<this->event[i].stime<<"  SPLIT"<<"   "<<this->event[i].pop<<"   "<<this->event[i].pop1<<"   "<<this->event[i].pop2<<"   "<<this->event[i].sadmixrate<<"\n";
      } else if (sevent=="VARNE") {
	if (n>4) return "too many words at line "+IntToString(i+2);
	if (n<4)return "not enough words at line "+IntToString(i+2);
	this->event[i].action='V';
	//cout<<"this->event[i].action = "<<this->event[i].action<<"\n";
	this->event[i].pop=atoi(ss[2].c_str());
	if ((this->event[i].pop<1)or(this->event[i].pop>this->nn0))
		return "population number must be a positive integer at most equal to "+IntToString(this->nn0)+ " at line "+IntToString(i+2);
	//cout<<"this->event[i].pop = "<<this->event[i].pop<<"\n";
	//cout<<"ss[3] = "<<ss[3]<<"\n";
	//cout<<"atoi(ss[3].c_str()) = "<<atoi(ss[3].c_str())<<"\n";
	if (atoi(ss[3].c_str())!=0) this->event[i].Ne=atoi(ss[3].c_str());
	else {
	  this->event[i].Ne=-1;
	  //cout<<"this->event[i].Ne = "<<this->event[i].Ne<<"\n";
	  // this->event[i].lNe=ss[3].length()+1;
	  //cout<<"this->event[i].lNe="<<this->event[i].lNe<<"\n";
	  // this->event[i].sNe=new char[this->event[i].lNe];
	  //cout<<"avant le strcpy\n";
	  this->event[i].sNe = ss[3];
	  //cout<<"avant this->detparam("<<ss[3]<<",0)\n";
	  this->detparam(ss[3],0);
	}
	//cout <<this->event[i].stime<<"  VARNE"<<"   "<<this->event[i].pop<<"\n";
      }
      delete []ss;
    }
	if (this->nsamp<1)
		return "you forgot to indicate when samples are taken";
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
    for (int i=0;i<this->nevent;i++) {if ((this->event[i].action=='E')or(this->event[i].action=='R')) {n++;this->time_sample[n]=this->event[i].time;}}
	return "";
}


/* Méthodes de SequenceBitC */

void SequenceBitC::ecris(){
    switch (action){
    case 'C' : cout <<"coalescence dans pop "<<this->pop<<"   N="<<this->N<<"   t0="<<this->t0<<"   t1="<<this->t1<<"\n";break;
    case 'M' : cout << "merge pop="<<this->pop<<"  pop1="<<this->pop1<<"  à t0="<<this->t0<<"\n";break;
    case 'S' : cout <<"split de pop="<<this->pop<<"  vers pop1="<<this->pop1<<" et pop2="<<this->pop2<<"  à t0="<<this->t0<<"\n";break;
    case 'A' : cout <<"add sample à pop="<<this->pop<<"  à t0="<<this->t0<<"\n";break;
    }
  }


/* Méthodes de GeneTreeC */

GeneTreeC::GeneTreeC(GeneTreeC const & source) {
  this->nnodes = source.nnodes;
  this->ngenes = source.ngenes;
  this->nbranches = source.nbranches;
  this->nbOK = source.nbOK;
  this->nbOKOK = source.nbOKOK;
  this->branches = new BranchC[this->nbranches];
  this->nodes = new NodeC[this->nnodes];
  for (int b=0;b<this->nbranches;b++) {
    this->branches[b] = source.branches[b];
  }
  for (int n=0;n<this->nnodes;n++) {
    this->nodes[n] = source.nodes[n];
  }
}


GeneTreeC & GeneTreeC::operator=(GeneTreeC const & source) {
  if( this == &source)
    return *this;

  if( nodes != NULL) delete [] nodes;
  if( branches != NULL) delete [] branches;

  this->nnodes = source.nnodes;
  this->ngenes = source.ngenes;
  this->nbranches = source.nbranches;
  this->nbOK = source.nbOK;
  this->nbOKOK = source.nbOKOK;
  this->branches = new BranchC[this->nbranches];
  this->nodes = new NodeC[this->nnodes];
  for (int b=0;b<this->nbranches;b++) {
    this->branches[b] = source.branches[b];
  }
  for (int n=0;n<this->nnodes;n++) {
    this->nodes[n] = source.nodes[n];
  }
  return *this;
}








