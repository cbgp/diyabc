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
 * Classe PriorC :éléments de définition d'un prior de paramètre historique
 */
class PriorC
{
public:
  std::string loi;
  double mini,maxi,mean,sdshape;
  int ndec;
  bool constant,fixed;

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

  void ecris();
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
  EventC *event;
  Ne0C *ne0;
  HistParameterC *histparam;
  ConditionC *condition;


  /* action = 0 (varne), 1 (merge), 2 (split), 3 (adsamp)
   * category=0 (Ne)   , 1 (time),  3 (admixrate)
   */

  /* Déclaration des méthodes */
  ScenarioC(){
    paramvar = NULL;
    time_sample = NULL;
    event = NULL;
    ne0 = NULL;
    histparam = NULL;
    condition = NULL;
  };
  ScenarioC(ScenarioC const & source);
  ScenarioC & operator= (ScenarioC  const & source);

  ~ScenarioC(){
    if( paramvar != NULL) delete [] paramvar;
    if( time_sample != NULL) delete [] time_sample;
    if( event != NULL) delete [] event;
    if( ne0 != NULL) delete [] ne0;
    if( histparam != NULL) delete [] histparam;
    if( condition != NULL) delete [] condition;
  };

  /* détermination du ou des paramètres contenus dans la std::string s */
  void detparam(std::string s,int cat);
  /* lecture/interprétation des lignes d'un scénario */
  std::string read_events(int nl,std::string *ls);
  /* verification d'un scénario*/
  std::string checklogic();
  void ecris();
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

  void ecris();
};



/**
 * Struct NodeC : éléments de définition d'un noeud de l'arbre de coalescence
 */
struct NodeC
{
  int pop,sample,state,brhaut,ndat;
  double height;
  std::string dna;
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
 * Class GeneTreeC : éléments de définition d'un arbre de coalescence
 */
class GeneTreeC
{
public:
  NodeC *nodes;
  BranchC *branches;
  int nmutot,nnodes,nbranches,ngenes,nbOK;

  /* Déclaration des méthodes */
  GeneTreeC(){
    nodes = NULL;
    branches = NULL;
  };
  ~GeneTreeC(){
    if( nodes != NULL) delete [] nodes;
    if( branches != NULL) delete [] branches;
  };

  GeneTreeC(GeneTreeC const & source);
  GeneTreeC & operator=(GeneTreeC const & source);

};



#endif /* HISTORY_H_ */
