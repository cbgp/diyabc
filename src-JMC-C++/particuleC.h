/*
 * particuleC.h
 *
 *  Created on: 8 déc. 2011
 *      Author: ppudlo
 */

#ifndef PARTICULEC_H_
#define PARTICULEC_H_
#include <string>
#include "history.h"
#include "data.h"
#include "randomgenerator.h"
#include "mesutils.h"

#define MICMISSING -9999
#define SEQMISSING ""
#define SNPMISSING 9
#define NUCMISSING 'N'

/**
 * Struct ParticleC : éléments de définition d'une particule
 */
class ParticleC
 {
 public:
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
	  bool firstime;
	  double sexratio;
	  int npart,nloc,ngr,nparam,nseq,nstat,nsample,*nind,**indivsexe,nscenarios,nconditions,**numvar,*nvar,refnindtot;
	  float reffreqmin;
	  bool ***dat,***ref;  //appartenance d'un gène aux data (dat) ou aux individus de référence (ref) par [locustype][sample][gene]
	  bool *catexist;
	  double matQ[4][4];
	  long int naccept,ntentes;
	  double weight,locpol,sumweight,threshold;

   /* Déclaration des méthodes */
	  void libereobs(bool obs);
	  void libere();
   void ecris();

   // calcule le nombre de copies de gènes de l'individu i de l'échantillon sa au locus loc
   int calploidy(int loc, int sa,int i);

   // recopie le scénario numscen dans this->scen
   // Si numscen<1, tirage au sort préalable du scenario dans le prior
   void drawscenario(int *numscen);



   // retourne la valeur d'un paramètre à partir de son nom (propriété name)
   double param2val(std::string paramname);

   // retourne la valeur d'une expression du genre param1 + param2 - param3
   double getvalue(std::string line); // FIXME: cela n'a rien à faire là ???

   // indique si toutes les conditions sur les paramètres sont vérifiées
   bool conditionsOK();

   // vérifie l'ordre des événements quand il a y en a plusieurs à la même génération et effectue les corrections
   void checkorder();

   // génère les valeurs des paramètres historiques d'un scénario donné (renvoie false si pb)
   bool setHistParamValue();

   // génère les valeurs des paramètres mutationnels moyens des différents groupes de locus
   void setMutParammoyValue();

   // génère les valeurs des paramètres mutationnels du locus loc
   void setMutParamValue(int loc);


   /////// Création de la séquence d'(événements)

   // retourne vrai si et seulement si l'événement ievent est basal (??)
   bool firstEvent(int ievent);

   // retourne la valeur du Ne pour l'événement ievent et la pop refpop
   int findNe(int ievent, int refpop);

   // retourne la valeur du time pour l'événement ievent et la pop refpop
   int findT0(int ievent, int refpop);

   // établit les paramètres d'un segment de l'arbre de coalescence
   void seqCoal(int iseq, int ievent, int refpop);

   // établit les paramètres d'un événement "ajout d'un échantllon" à l'arbre de coalescence
   void seqSamp(int iseq, int ievent);

   // établit les paramètres d'un événement "merge" à l'arbre de coalescence
   void seqMerge(int iseq, int ievent);

   //  établit les paramètres d'un événement "split" à l'arbre de coalescence
   void seqSplit(int iseq, int ievent);

   // compte le nombre nécessaires d'éléments de la séquence définissant l'arbre de coalescence
   int compteseq();

   // établit la séquence d'événements populationnels définissant l'arbre de coalescence
   void setSequence();

   // calcule les éléments de la matrice de transition Q qui fournit
   // la probabilité de mutation d'un nucléotide à un autre en fonction
   // du modèle mutationnel choisi
   void comp_matQ(int loc);

   // initialise l'arbre de coalescence, i.e. alloue la mémoire pour les noeuds et les branches,
   // initialise les propriétés "sample" et "height" des noeuds terminaux
   // initialise à 0 la propriété "pop" de tous les noeuds et à 0 la propriété "sample" des noeuds non-terminaux
   void init_tree(GeneTreeC & gt,int loc, bool gtexist, bool reference);

   // évalue la pertinence de l'approximation continue pour le traitement de la coalescence
   // (1= approximation continue, 0 = generation par generation)
   int evalcriterium(int iseq,int nLineages);

   // tire au hasard (uniformement) une lignée ancestrale parmi celles de la population requise
   int draw_node(int loc,int iseq,int nLineages);

   // coalesce les lignées ancestrales de la population requise
   void coal_pop(int loc,int iseq, bool reference, int *refmrca);

   // FIXME: sens ???
   // debut modifications de l'arbre
   void pool_pop(int loc,int iseq);
   void split_pop(int loc,int iseq);
   void add_sample(int loc,int iseq);
   // fin modification de l'arbre

   // SNP: cherche sous-arbre depuis echantillon de ref jusqu'a MRCA
   void cherche_branchesOK(int loc);
   // ADN et microsat: mettre les mutations dans l'arbre
   void put_mutations(int loc);
   // SNP: mettre la mutation dans l'arbre
   void put_one_mutation(int loc);
   // effectue une mutation sur le noeud inferieur de la branche b (en position site[numut] si ADN)
   void mute(int loc, int numut, int b);
   char draw_nuc(int loc);
   // initialise la séquence ancestrale (ADN)
   std::string init_dnaseq(int loc);
   // part de MRCA et crée tous les haplotypes en bas de l'arbre
   int cree_haplo(int loc);
   // vérification de la topologie de l'arbre de coalescence
   std::string verifytree();
   // SNP: test si le locus est polymorphe chez les individus de reference
   bool polymref(int loc);

   // tire le scenario, simule les paramatres et les haplotypes
   // (renvoie 0 si tout s'est bien passé, d'autres nombres si erreur (voir code))
   int dosimulpart(int numscen);
   // idem, mais pour la simulation de fichier
   std::string dogenepop();
   std::string dodataSNP();
   
   /* Partie calcul des summary stat */
   int samplesize(int loc, int sample);
   void calfreq(int gr);
   void calfreqsnp(int gr);
   void liberefreq(int gr);
   void liberefreqsnp(int gr);
   void libere_freq(int gr);
   void liberednavar(int gr);
   // SNP
   void cal_snhet(int gr,int numsnp);
   void cal_snnei(int gr,int numsnp);
   void cal_snfst(int gr,int numsnp);
   void cal_snaml(int gr,int numsnp);
   // MICROSAT
   long double cal_pid1p(int gr,int st);
   long double cal_nal1p(int gr,int st);
   long double cal_nal2p(int gr,int st);
   long double cal_het1p(int gr,int st);
   long double cal_het2p(int gr,int st);
   long double cal_var1p(int gr,int st);
   long double cal_var2p(int gr,int st);
   long double cal_mgw1p(int gr,int st);
   long double cal_Fst2p(int gr,int st);
   long double cal_lik2p(int gr,int st);
   long double cal_dmu2p(int gr,int st);
   long double cal_das2p(int gr,int st);
   pair<long double, long double> pente_lik(int gr,int st, int i0);
   long double cal_Aml3p(int gr,int st);
   // DNA
   bool identseq(std::string seq1, std::string seq2,std::string appel,int kloc,int sample,int ind);
   long double cal_nha1p(int gr,int st);
   int* cal_nsspl(int kloc,int sample, int *nssl,bool *OK);
   long double cal_nss1p(int gr,int st);
   long double cal_mpdpl(int kloc,int sample,int *nd);
   long double cal_mpd1p(int gr,int st);
   long double cal_vpd1p(int gr,int st);
   long double cal_dta1pl(int kloc,int sample,bool *OKK);
   long double cal_dta1p(int gr,int st);
   long double cal_pss1p(int gr,int st);
   void afs(int sample,int iloc,int kloc);
   long double cal_mns1p(int gr,int st);
   long double cal_vns1p(int gr,int st);
   long double cal_nha2p(int gr,int st);
   int* cal_nss2pl(int kloc,int samp0, int samp1, int *nssl,bool *OK);
   long double cal_nss2p(int gr,int st);
   long double cal_mpw2pl(int kloc,int samp0, int samp1, bool *OK);
   long double cal_mpw2p(int gr,int st);
   long double cal_mpb2pl(int kloc,int samp0, int samp1, bool *OK);
   long double cal_mpb2p(int gr,int st);
   long double cal_fst2p(int gr,int st);
   void cal_freq(int gr,int st); // FIXME: différence avec calfreq ????
   long double cal_aml3p(int gr,int st);
   void cal_numvar(int gr);
   long double cal_p0L(StatsnpC stsnp);
   long double mQ(int n, int n0, int n1,long double *x);
   long double cal_medL(int n, long double *x);
   long double cal_qu1L(int n, long double *x);
   long double cal_qu3L(int n, long double *x);
   long double cal_moyL0(StatsnpC stsnp);
   long double cal_moyL(StatsnpC stsnp);
   long double cal_varL0(StatsnpC stsnp);
   void docalstat(int gr,double weight);

 };
#endif /* PARTICULEC_H_ */
