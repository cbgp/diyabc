/*
 * history.h
 *
 *  Created on: 9 déc. 2011
 *      Author: ppudlo
 */

#ifndef HISTORY_H_
#define HISTORY_H_

#include <string>
#include "randomgenerator.h"


/**
 * struct StatC :éléments de définition d'une summary statistic
 */
struct StatC
{
  int cat,samp,samp1,samp2,group,numsnp;
  long double val;
//  StatC(StatC const & source);
  StatC & operator= (StatC const & source);
};

/**
 * struct StatsnpC :éléments de définition d'une summary statistic pour les snp
 */
struct StatsnpC
{
  int cat,samp,samp1,samp2,group,n;
  long double *x,sw,*w;
  bool defined;
  StatsnpC(StatsnpC const & source);
  StatsnpC & operator= (StatsnpC const & source);
  StatsnpC(){
	  x = NULL;
	  w = NULL;
}
  ~StatsnpC(){
    if( x != NULL) delete [] x;
    if( w != NULL) delete [] w;
  };
};





/**
 * Classe PriorC :éléments de définition d'un prior de paramètre historique
 */
class PriorC
{
public:
  std::string loi;
  double mini,maxi,mean,sdshape;
  int ndec;
  bool constant,fixed;
  PriorC() {
	  loi = "";
}
//  PriorC(PriorC const & source);
  PriorC & operator= (PriorC const & source);
  void ecris();
  double drawfromprior(MwcGen & mw);
  void readprior(std::string ss);
};

/**
 * Classe ConditionC :éléments de définition d'une condition sur un couple de paramètres historiques
 */

class ConditionC
{
public:
  std::string param1,param2,operateur;
	ConditionC(){
		param1 = "";
		param2 = "";
		operateur = "";
	};
	~ConditionC(){
		//std::cout<<"passage dans le destructeur de ConditionC\n";
	}
//  ConditionC(ConditionC const & source);
  ConditionC & operator= (ConditionC const & source);
  void ecris();
  void readcondition(std::string ss);

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
  int numevent0,nindMref,nindFref;
  std::string stime, sNe, sadmixrate;
  // char *stime,*sNe,*sadmixrate;
  // int ltime,lNe,ladmixrate;


  /**
   * Déclarations des méthodes (PP)
   */

  // Définition de l'opérateur < pour le tri des événements populationnels (PP)
  friend bool operator< (const EventC & lhs, const EventC & rhs);

  // Pour écrire l'événement dans la sortie standard
  void ecris();
};

/**
 * struct Ne0C : éléments de définition d'un effectif efficace à la ligne 1 d'un scénario
 */
class Ne0C
{
public:
  int val;
  std::string name;
};

/**
 * Classe HistparameterC :éléments de définition d'un paramètre historique
 */
class HistParameterC
{
public:
  std::string name;
  int category;   //0 pour N, 1 pour T et 2 pour A
  double value;
  PriorC prior;
  HistParameterC(){
	  name = "";
}
//  HistParameterC(HistParameterC const & source);
  HistParameterC & operator= (HistParameterC const & source);
  void ecris(bool simulfile);
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
  /* Déclaration des méthodes */
  LocusGroupC(){
    loc = NULL;
    sumstat = NULL;
    sumstatsnp = NULL;
	nstat = 0;
	nstatsnp = 0;
  };
//  LocusGroupC(LocusGroupC const & source);
  LocusGroupC & operator= (LocusGroupC  const & source);

  ~LocusGroupC(){
	  //std::cout<<"passage dans le destructeur de LocusGroupC\n";
    if( loc != NULL) delete [] loc;
    if( sumstat != NULL) delete [] sumstat;
    if( sumstatsnp != NULL) delete [] sumstatsnp;
	//std::cout<<" sortie du destructeur de LocusGroupC\n";
  };

  void libere();

};

/**
 * Classe ScenarioC :éléments de définition d'un scénario
 */
class ScenarioC
{
public:
  double *paramvar, prior_proba;
  int number,popmax,npop,nsamp,*time_sample,nparam,nevent,nn0,nparamvar,nconditions,ipv;
  std::string *stime_sample;
  EventC *event;
  Ne0C *ne0;
  HistParameterC *histparam;
  ConditionC *condition;
  /* Déclaration des méthodes */
  ScenarioC(){
    paramvar = NULL;
    time_sample = NULL;
    stime_sample = NULL;
    event = NULL;
    ne0 = NULL;
    histparam = NULL;
    condition = NULL;
  };
  ScenarioC(ScenarioC const & source);
  ScenarioC & operator= (ScenarioC  const & source);

  ~ScenarioC(){
	  //std::cout<<"passage dans le destructeur de ScenarioC\n";
    /*if( paramvar != NULL) delete [] paramvar;
    if( time_sample != NULL) delete [] time_sample;
    if( stime_sample != NULL) delete [] stime_sample;
    if( event != NULL) delete [] event;
    if( ne0 != NULL) delete [] ne0;
    if( histparam != NULL) delete [] histparam;
    if( condition != NULL) delete [] condition;*/
  };

  /* détermination du ou des paramètres contenus dans la std::string s */
  void detparam(std::string s,int cat);
  /* lecture/interprétation des lignes d'un scénario */
  std::string read_events(int nl,std::string *ls);
  /* verification d'un scénario*/
  std::string checklogic();
  void ecris(bool simulfile);
  void libere();
}; /* fin classe ScenarioC */



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
  SequenceBitC(){
	  popfull = NULL;
}
  ~SequenceBitC(){
	  //std::cout<<"passage dans le destructeur de SequenceBitC\n";
	  if (popfull != NULL) delete [] popfull;
	  //std::cout<<"   sortie du destructeur de SequenceBitC\n";
}
  void ecris();
};



/**
 * Struct NodeC : éléments de définition d'un noeud de l'arbre de coalescence
 */
struct NodeC
{
  int pop,sample,state,brhaut,ndat,nref;
  double height;
  std::string dna;
  bool OK;
  ~NodeC(){}
};

/**
 * Struct BranchC : éléments de définition d'une branche de l'arbre de coalescence
 */
struct BranchC
{
  int bottom,top,nmut;
  double length;
  bool OK,OKOK;
  ~BranchC(){}
};

/**
 * Class GeneTreeC : éléments de définition d'un arbre de coalescence
 */
class GeneTreeC
{
public:
  NodeC *nodes;
  BranchC *branches;
  int nmutot,nnodes,nbranches,ngenes,nbOK,nbOKOK;

  /* Déclaration des méthodes */
  GeneTreeC(){
    nodes = NULL;
    branches = NULL;
  };
  ~GeneTreeC(){
	//std::cout<<"passage dans le destructeur de GeneTreeC\n";
    if( nodes != NULL) {delete [] nodes;}
    if( branches != NULL) delete [] branches;
  };

  GeneTreeC(GeneTreeC const & source);
  GeneTreeC & operator=(GeneTreeC const & source);

};



#endif /* HISTORY_H_ */
