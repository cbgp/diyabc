/*
 * particuleC.cpp
 *
 *  Created on: 23 sept. 2010
 *      Author: cornuet
 */
#include <cstdio>
#include <cmath>
#include <vector>
#include <iostream>
//#include <complex>
#include <utility>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <omp.h>



#include "randomgenerator.h"
#include "mesutils.h"
#include "data.h"
#include "history.h"

/*
#ifndef RANDOMGENERATOR
#include "randomgenerator.cpp"
#define RANDOMGENERATOR
#endif
#ifndef MESUTILS
#include "mesutils.cpp"
#define MESUTILS
#endif
#ifndef DATA
#include "data.cpp"
#define DATA
#endif
#ifndef HISTORY
#include "history.cpp"
#define HISTORY
#endif

#ifndef SUMSTAT
#include "sumstat.cpp"
#define SUMSTAT
#endif
*/
#include "particuleC.h"


using namespace std;

extern string * stat_type;
extern int* stat_num;

extern int debuglevel;
extern string reftablelogfilename;

//long int naccept,ntentes; à faire par particule et à sommer à la fin dans particleset

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
 * retourne un ordre aléatoire pour les éléments d'un vecteur
 * à partir du k-ième
 */
vector <int> melange2(MwcGen mw, int k, int n) {
  vector <double> ra;
  vector <int> ord;
  ord.resize(n);ra.resize(n);
  for (int i=0;i<k;i++) ord[i]=i;
  for (int i=k;i<n;i++) ra[i] = mw.random();
  for (int i=k;i<n;i++) {
    ord[i] = k;
    for (int j=k;j<n;j++) {if (ra[i]>ra[j]) ord[i]++;}
  }
  return ord;
}






  void ParticleC::libere(bool obs) {
    for (int i=0;i<this->nloc;i++) locuslist[i].libere(obs, this->nsample);
    delete []locuslist;
    for (int cat=0;cat<5;cat++) {
      if (this->catexist[cat]) {
	for (int sa=0;sa<this->nsample;sa++){
	  delete []this->dat[cat][sa];
	  delete []this->ref[cat][sa];
	}
	delete []this->dat[cat];
	delete []this->ref[cat];
      }
    }
    delete []this->dat;
    delete []this->ref;
    delete []this->catexist;
  }


  void ParticleC::ecris(){
    for (int i=0;i<this->nscenarios;i++) this->scenario[i].ecris();
  }

  /**
   * Struct ParticleC : calcule le nombre de copies de gènes de l'individu i de l'échantillon sa au locus loc
   */
  int ParticleC::calploidy(int loc, int sa,int i) {
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
  void ParticleC::drawscenario(int numscen) {
	  double ra,sp=0.0;
	  int iscen,ngref;
	  if (numscen<1) {
		  ra = this->mw.random();
		  iscen=-1;
		  while ((ra>sp)and(iscen<this->nscenarios-1)) {
			  iscen++;sp +=this->scenario[iscen].prior_proba;
		  }
	  } else iscen=numscen-1;
	  
	  // copie scenario[iscen] dans scen 
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
		  this->scen.ne0[i].name = this->scenario[iscen].ne0[i].name;
	  }
	  //for (int i=0;i<this->scen.nn0;i++) this->scen.ne0[i] = copyne0(this->scenario[iscen].ne0[i]);
	  for (int i=0;i<this->scen.nevent;i++) {
		  // this->scen.event[i].libere();
		  this->scen.event[i] = this->scenario[iscen].event[i]; // copyevent supprimé (PP)
	  }
	  for (int i=0;i<this->scen.nparam;i++) {this->scen.histparam[i] = this->scenario[iscen].histparam[i];/*cout<<this->scen.histparam[i].name<<"\n"<<flush;*/}
	  for (int i=0;i<this->scen.nparamvar;i++) {this->scen.paramvar[i] = this->scenario[iscen].paramvar[i];/*cout<<this->scen.histparam[i].name<<"\n"<<flush;*/}
	  if (this->scen.nconditions>0) {
		  for (int i=0;i<this->scen.nconditions;i++) this->scen.condition[i] = this->scenario[iscen].condition[i];
	  }
	  
	  // fin copie
	  
	  //cout<<"drawscenario nparamvar="<<this->scen.nparamvar<<"\n";
	  //this->scen.ecris();
	  //cout<<"dans drawscenario    nsample="<<this->scen.nsamp<<"\n";
	  //for (int sa=0;sa<this->scen.nsamp;sa++) cout<<this->data.ss[0][sa]<<"   ";cout<<"\n";
	  this->refnindtot=0;
	  this->ref = new bool**[5];
	  this->dat = new bool**[5];
	  for (int cat=0;cat<5;cat++) {
		  if (this->data.catexist[cat]) {
			  this->ref[cat] = new bool*[this->scen.nsamp];
			  this->dat[cat] = new bool*[this->scen.nsamp];
			  for (int sa=0;sa<this->scen.nsamp;sa++) {this->ref[cat][sa] = new bool[this->data.ss[cat][sa]];this->dat[cat][sa] = new bool[this->data.ss[cat][sa]];}
			  for (int sa=0;sa<this->scen.nsamp;sa++) {
				  for (int ievent=0;ievent<this->scen.nevent;ievent++) {
					  if ((this->scen.event[ievent].action=='E')and(this->scen.event[ievent].sample==sa+1)) {
						switch (cat) {
							case 0 : ngref=2*(this->scen.event[ievent].nindMref + this->scen.event[ievent].nindFref);break;
							case 1 : ngref=this->scen.event[ievent].nindMref + this->scen.event[ievent].nindFref;break;
							case 2 : ngref=this->scen.event[ievent].nindMref + 2*this->scen.event[ievent].nindFref;break;
							case 3 : ngref=this->scen.event[ievent].nindMref;break;
							case 4 : ngref=this->scen.event[ievent].nindMref + this->scen.event[ievent].nindFref;break;
						}
						  for (int i=0;i<this->data.ss[cat][sa];i++) {
							  this->dat[cat][sa][i] = true;
							  this->ref[cat][sa][i] = (i<ngref);
							  if (this->ref[cat][sa][i]) this->refnindtot++;
						  }
						  //cout<<"drawscenario : this->scen.event[ievent].action=='E'"<<"    refnindtot="<<this->refnindtot<<"\n";
					  }
					  if ((this->scen.event[ievent].action=='R')and(this->scen.event[ievent].sample==sa+1)) {
						  this->refnindtot +=this->data.ss[cat][sa];
						  for (int i=0;i<this->data.ss[cat][sa];i++) {
							  this->dat[cat][sa][i] = false;
							  this->ref[cat][sa][i] = true;
						  }
						  //cout<<"drawscenario : this->scen.event[ievent].action=='R'"<<"    refnindtot="<<this->refnindtot<<"   ss="<<this->data.ss[cat][sa]<<"\n";
					  }
					  
				  }
			  }
		  }
	  }
	  //cout<<"drawscenario : refnindtot="<<this->refnindtot<<"\n";
	  
	  //home/cornuet/workspace/diyabchg/src-JMC-C++/general -p /home/cornuet/diyabc/AVEC_BIAIS/S1F_2012_1_6-1/ -j s:1;n:99409;m:1000;t:3;v:;q:1000;a:pl -i mc1 -g 500 -m -t 8
	  }



  /**
   * Struct ParticleC : retourne la valeur d'un paramètre à partir de son nom (propriété name) ou de sa valeur
   */
  double ParticleC::param2val(string paramname){
    int ip;
	ip = atoi(paramname.c_str());
	if (ip!=0) return (double)ip;
    //cout<<"paramname = "<<paramname<<"\n";
    for (ip=0;ip<this->scen.nparam;ip++) {if (paramname==this->scen.histparam[ip].name) break;}
    //cout<<"param "<<ip<<"    value="<<this->scen.histparam[ip].value<<"\n";fflush(stdin);
    return this->scen.histparam[ip].value;

  }

  /**
   * Struct ParticleC : retourne la valeur d'une expression du genre param1 +param2-param3
   */
  double ParticleC::getvalue(string line) {
    double result;
    bool fin=false;
	int ls;
    cout<<"line="<<line<<"\n";
    string operateur,plus="+",minus="-";
    size_t posign,posplus,posminus;
    posplus=line.find('+'); posminus=line.find('-');
    if ((posplus==string::npos)and(posminus==string::npos)) {
    	return  this->param2val(line);
    } else {
		ls = line.size();
    	if ((posplus<ls)and(posminus>ls)){posign=posplus;}
    	else {
    		if ((posminus<ls)and(posplus>ls)){posign=posminus;} 
    		else {
    			if (posplus<posminus) {posign=posplus;}
    			else {posign=posminus;}
    		}
    	}
    }
    result=this->param2val(line.substr(0,posign));
    cout << result;
    operateur=line.substr(posign,1);
    cout <<"   "<<operateur<<"   ";
    line=line.substr(posign+1);
    cout << line << "\n";
    while (not fin) {
		ls = line.size();
		posplus=line.find('+');posminus=line.find('-');
		if ((posplus>ls)and(posminus>ls)) {
			if (operateur==plus) {result +=this->param2val(line);}
			if (operateur==minus) {result -=this->param2val(line);}
			cout << "result = " << result <<"\n";
			fin=true;break;
		} else {
			if ((posplus<ls)and(posminus>ls)){posign=posplus;} 
			else {
				if ((posminus<ls)and(posplus>ls)){posign=posminus;} 
				else {
					if (posplus<posminus) {posign=posplus;} 
					else {posign=posminus;}
				}
			}
      }
      if (operateur==plus) {result +=this->param2val(line.substr(0,posign));}
      if (operateur==minus) {result -=this->param2val(line.substr(0,posign));}
      operateur=line.substr(posign,1);
      line=line.substr(posign+1);
    }
    cout<<"result final="<<result<<"\n";
    return result;
  }

  /**
   * Struct ParticleC : indique si toutes les conditions sur les paramètres sont vérifiées
   */
  bool ParticleC::conditionsOK() {
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

  void ParticleC::checkorder() {
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
	      for (int k=0;k<(int)exaequo[g].size();k++)
	    	  if (exaequo[g][k]==iev) {deja=true;gg=g;kk=k;break;}
	    }
	    //cout<<"deja="<<deja<<"   g="<<gg<<"   k="<<kk<<"\n";
	    if (deja) {
	      trouve=false;
	      //cout<<"exaequo[g].size()="<<exaequo[gg].size()<<"\n";
	      for (int k=0;k<(int)exaequo[gg].size();k++)
	    	  if (exaequo[gg][k]==jev){trouve=true;break;}
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
      for (int iev=0;iev<(int)exaequo[g].size();iev++) {
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
      for (int iev=0;iev<(int)exaequo[g].size()-1;iev++) {
	for (int jev=iev+1;jev<(int)exaequo[g].size();jev++) {
	  if (bonordre[iev]>bonordre[jev]) {
	    //cout<<"echange entre "<<iev<<" et "<<jev<<"\n";
	    piv = this->scen.event[exaequo[g][iev]];  // copyevent supprimé (PP)
	    this->scen.event[exaequo[g][iev]] = this->scen.event[exaequo[g][jev]]; // copyevent supprimé (PP)
	    this->scen.event[exaequo[g][jev]] = piv; // copyevent supprimé (PP)
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
  bool ParticleC::setHistParamValue() {
    //cout<<"dans sethistparamvalue nconditions="<<this->scen.nconditions<<"   drawuntil="<<drawuntil<<"\n";
    bool OK=true;
    if (this->scen.nconditions>0) {
		if (drawuntil) {
			//cout <<"drawuntil\n";
			OK=false;
			while (not OK) {
				for (int p=0;p<this->scen.nparam;p++) {
					if (not this->scen.histparam[p].prior.fixed) this->scen.histparam[p].value = this->scen.histparam[p].prior.drawfromprior(this->mw);
					if (this->scen.histparam[p].category<2) this->scen.histparam[p].value = floor(0.5+this->scen.histparam[p].value);
					//cout<<this->scen.histparam[p].name <<" = "<<this->scen.histparam[p].value<<"\n";
					//cout<<"    "<<this->scen.histparam[p].prior.loi <<"  ["<<this->scen.histparam[p].prior.mini<<","<<this->scen.histparam[p].prior.maxi <<"]\n";
				}
				//cout <<"avant test conditions\n";
				OK = conditionsOK();
				//if (OK) cout <<"condition OK\n";
			}
		} else {
			for (int p=0;p<this->scen.nparam;p++) {
				if (not this->scen.histparam[p].prior.fixed) this->scen.histparam[p].value = this->scen.histparam[p].prior.drawfromprior(this->mw);
				if (this->scen.histparam[p].category<2) this->scen.histparam[p].value = floor(0.5+this->scen.histparam[p].value);
			}
			OK = conditionsOK();
		}
    }else {
		for (int p=0;p<this->scen.nparam;p++) {
			if (not this->scen.histparam[p].prior.fixed) this->scen.histparam[p].value = this->scen.histparam[p].prior.drawfromprior(this->mw);
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
      sort(&this->scen.event[0],&this->scen.event[this->scen.nevent]); 
      // compevent inutile: operator< est surchargé correctement (PP)
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
  void ParticleC::setMutParammoyValue(){
    int gr;
    //cout<<"\nthis->scen.ipv="<<this->scen.ipv<<"\n";
    for (gr=1;gr<=this->ngr;gr++) {
      //cout<<"groupe "<<gr<<"\n";
	  //cout<<"   type="<<this->grouplist[gr].type<<"\n";
      if (this->grouplist[gr].type==0) {  //microsat
//cout<<"microsat  groupe[gr].priormutmoy.constant="<<this->grouplist[gr].priormutmoy.constant <<"\n";

	if (not this->grouplist[gr].priormutmoy.fixed) this->grouplist[gr].mutmoy = this->grouplist[gr].priormutmoy.drawfromprior(this->mw);
	if (not this->grouplist[gr].priormutmoy.constant) {
	  this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].mutmoy;
	  this->scen.ipv++;
	  //cout<<"mutmoy ipv++\n";
	}
	//cout<<"mutmoy="<<this->grouplist[gr].mutmoy<<"\n";
	if (not this->grouplist[gr].priorPmoy.fixed) this->grouplist[gr].Pmoy = this->grouplist[gr].priorPmoy.drawfromprior(this->mw);
	if (not this->grouplist[gr].priorPmoy.constant) {
	  this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].Pmoy;
	  this->scen.ipv++;
	  //cout<<"Pmoy ipv++\n";
	}
	//cout<<"Pmoy="<<this->grouplist[gr].Pmoy<<"\n";
	//cout<<"avant ecriture de priorsnimoy\n";
	//this->grouplist[gr].priorsnimoy.ecris();	
	if (not this->grouplist[gr].priorsnimoy.fixed) this->grouplist[gr].snimoy = this->grouplist[gr].priorsnimoy.drawfromprior(this->mw);
	if (not this->grouplist[gr].priorsnimoy.constant) {
	  this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].snimoy;
	  this->scen.ipv++;
	  //cout<<"snimoy ipv++\n";
	} else this->grouplist[gr].snimoy = this->grouplist[gr].priorsnimoy.mini;
	//cout<<"snimoy="<<this->grouplist[gr].snimoy<<"\n";
      }
      if (this->grouplist[gr].type==1) {  //sequence
	if (not this->grouplist[gr].priormusmoy.fixed) this->grouplist[gr].musmoy = this->grouplist[gr].priormusmoy.drawfromprior(this->mw);
	if (not this->grouplist[gr].priormusmoy.constant) {		
	  this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].musmoy;
	  //cout<<"musmoy ipv++\n";
	  this->scen.ipv++;
	}
	if (this->grouplist [gr].mutmod>0){
	  if (not this->grouplist[gr].priork1moy.fixed) this->grouplist[gr].k1moy = this->grouplist[gr].priork1moy.drawfromprior(this->mw);
	  if (not this->grouplist[gr].priork1moy.constant) {
	    this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].k1moy;
	    //cout<<"k1moy ipv++\n";
	    this->scen.ipv++;
	  }
	}
	if (this->grouplist [gr].mutmod>2){
	  if (not this->grouplist[gr].priork2moy.fixed) this->grouplist[gr].k2moy = this->grouplist[gr].priork2moy.drawfromprior(this->mw);
	  if (not this->grouplist[gr].priork2moy.constant) {
	    this->scen.paramvar[this->scen.ipv]=this->grouplist[gr].k2moy;
	    this->scen.ipv++;
	    //cout<<"k2moy ipv++\n";
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
  void ParticleC::setMutParamValue(int loc){
    int gr = this->locuslist[loc].groupe;
    //cout <<"\n SetMutParamValue pour le locus "<<loc<< " (groupe "<< gr <<")\n";
    if (this->locuslist[loc].type<5) {  //MICROSAT
      this->grouplist[gr].priormutloc.mean = this->grouplist[gr].mutmoy;
      if ((this->grouplist[gr].priormutloc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].mut_rate = this->grouplist[gr].priormutloc.drawfromprior(this->mw);
      else this->locuslist[loc].mut_rate =this->grouplist[gr].mutmoy;
      //cout << "mutloc["<<loc<<"]="<<this->locuslist[loc].mut_rate <<"\n";

      this->grouplist[gr].priorPloc.mean = this->grouplist[gr].Pmoy;
      if ((this->grouplist[gr].priorPloc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].Pgeom = this->grouplist[gr].priorPloc.drawfromprior(this->mw);
      else this->locuslist[loc].Pgeom =this->grouplist[gr].Pmoy;
      //cout << "Ploc["<<loc<<"]="<<this->locuslist[loc].Pgeom <<"\n";

      this->grouplist[gr].priorsniloc.mean = this->grouplist[gr].snimoy;
      //cout <<"coucou\n";fflush(stdin);
      if ((this->grouplist[gr].priorsniloc.sdshape>0.001 )and(this->grouplist[gr].nloc>1)) this->locuslist[loc].sni_rate = this->grouplist[gr].priorsniloc.drawfromprior(this->mw);
      else this->locuslist[loc].sni_rate =this->grouplist[gr].snimoy;
      //cout << "sniloc["<<loc<<"]="<<this->locuslist[loc].sni_rate <<"\n";


    }
    else		                    //DNA SEQUENCE
      {
	//cout << "musmoy = "<<this->grouplist[gr].musmoy <<" (avant)";
	if (this->grouplist[gr].musmoy<0) this->grouplist[gr].musmoy = this->grouplist[gr].priormusmoy.drawfromprior(this->mw);
	this->grouplist[gr].priormusloc.mean = this->grouplist[gr].musmoy;
	//cout << "   et "<<this->grouplist[gr].musmoy <<" (apres)\n";

	if ((this->grouplist[gr].priormusloc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].mus_rate = this->grouplist[gr].priormusloc.drawfromprior(this->mw);
	else this->locuslist[loc].mus_rate =this->grouplist[gr].musmoy;
	//cout << "   et "<<this->locuslist[loc].mut_rate <<" (apres)\n";

	if (this->grouplist [gr].mutmod>0) {
	  //cout << "Pmoy = "<<this->grouplist[gr].Pmoy <<" (avant)";
	  if (this->grouplist[gr].k1moy<0) this->grouplist[gr].k1moy = this->grouplist[gr].priork1moy.drawfromprior(this->mw);
	  this->grouplist[gr].priork1loc.mean = this->grouplist[gr].k1moy;
	  //cout << "   et "<<this->grouplist[gr].Pmoy <<" (apres)\n";

	  if ((this->grouplist[gr].priork1loc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].k1 = this->grouplist[gr].priork1loc.drawfromprior(this->mw);
	  else this->locuslist[loc].k1 =this->grouplist[gr].k1moy;
	  //cout << "   et "<<this->locuslist[loc].Pgeom <<" (apres)\n";
	}

	if (this->grouplist [gr].mutmod>2) {
	  if (this->grouplist[gr].k2moy<0) this->grouplist[gr].k2moy = this->grouplist[gr].priork2moy.drawfromprior(this->mw);
	  this->grouplist[gr].priork2loc.mean = this->grouplist[gr].k2moy;

	  if (this->grouplist[gr].priork2loc.sdshape>0.001 ) this->locuslist[loc].k2 = this->grouplist[gr].priork2loc.drawfromprior(this->mw);
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
  bool ParticleC::firstEvent(int ievent) {
	  if (ievent == 0) {return 1;} //1 pour TRUE
	  bool found = false;
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
  int ParticleC::findNe(int ievent, int refpop) {
    if (ievent == 0) {return this->scen.ne0[refpop-1].val;}
    bool found = false;
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
  int ParticleC::findT0(int ievent, int refpop) {
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
  void ParticleC::seqCoal(int iseq, int ievent, int refpop) {
    this->seqlist[iseq].action = 'C';
    this->seqlist[iseq].N = findNe(ievent,refpop);
    this->seqlist[iseq].pop = refpop;
    this->seqlist[iseq].t0 = findT0(ievent,refpop);
    this->seqlist[iseq].t1 = this->scen.event[ievent].time;
  }

  /**
   * Struct ParticleC : établit les paramètres d'un événement "ajout d'un échantllon" à l'arbre de coalescence
   */
  void ParticleC::seqSamp(int iseq, int ievent) {
    this->seqlist[iseq].action = 'A';
    this->seqlist[iseq].pop = this->scen.event[ievent].pop;
    this->seqlist[iseq].t0 = this->scen.event[ievent].time;
    this->seqlist[iseq].sample = this->scen.event[ievent].sample;
    //cout << " seqlist["<<iseq<<"].sample = "<<this->seqlist[iseq].sample<<"\n";
  }

  /**
   * Struct ParticleC : établit les paramètres d'un événement "merge" à l'arbre de coalescence
   */
  void ParticleC::seqMerge(int iseq, int ievent) {
    this->seqlist[iseq].action = 'M';
    this->seqlist[iseq].pop = this->scen.event[ievent].pop;
    this->seqlist[iseq].pop1 = this->scen.event[ievent].pop1;
    this->seqlist[iseq].t0 = this->scen.event[ievent].time;
  }

  /**
   * Struct ParticleC : établit les paramètres d'un événement "split" à l'arbre de coalescence
   */
  void ParticleC::seqSplit(int iseq, int ievent) {
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
  int ParticleC::compteseq() {
    int n = 0;
    for (int k=0;k<this->scen.nevent;k++){
      if ((this->scen.event[k].action == 'E')or(this->scen.event[k].action == 'R')) {n +=2;}  //SAMPLE
      else if (this->scen.event[k].action == 'V') {n +=1;} //VARNE
      else if (this->scen.event[k].action == 'M') {n +=3;} //MERGE
      else if (this->scen.event[k].action == 'S') {n +=4;} //SPLIT
    }
    return n;
  }

  /**
   * Struct ParticleC : établit la séquence d'événements populationnels définissant l'arbre de coalescence
   */
  void ParticleC::setSequence() {
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
	else if ((this->scen.event[ievent].action == 'E')or(this->scen.event[ievent].action == 'R')) {   // if action = SAMPLE
	  seqCoal(iseq,ievent,this->scen.event[ievent].pop);iseq++;
	  seqSamp(iseq,ievent);iseq++;
	  //cout<<this->seqlist[iseq-1].action<<"\n";
	}
      }
      else if ((this->scen.event[ievent].action == 'E')or(this->scen.event[ievent].action == 'R')) {   // if action = SAMPLE
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
    if ((this->scen.event[this->scen.nevent-1].action == 'M')or(this->scen.event[this->scen.nevent-1].action == 'E')or(this->scen.event[this->scen.nevent-1].action == 'R')) {
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
  void ParticleC::comp_matQ(int loc) {
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
  void ParticleC::init_tree(GeneTreeC & gt, int loc, bool gtexist) {
    //cout << "début de init_tree pour le locus " << loc  <<"\n";


    int nnod=0,nn,n=0,cat=(this->locuslist[loc].type % 5);
    //cout << "nsample = " << this->nsample << "   samplesize[0] = " << this->locuslist[loc].samplesize[0] << "\n";
    for (int sa=0;sa<this->data.nsample;sa++) {nnod +=this->data.ss[cat][sa];}
    //cout << "nombre initial de noeuds = " << nnod  <<"\n";exit(6);
    gt.ngenes = nnod;
    gt.nnodes=nnod;
    gt.nbranches = 0;
	if (not gtexist){
		gt.nodes = new NodeC[2*nnod+1];
		gt.branches = new BranchC[2*nnod];
	}
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

  }

  /**
   * Struct ParticleC : évalue la pertinence de l'approximation continue pour le traitement de la coalescence
   */
  int ParticleC::evalcriterium(int iseq,int nLineages) {
    if(this->seqlist[iseq].t1<0) {return 1;}
    int nGen;
    double ra;
    int OK=0;
    nGen=this->seqlist[iseq].t1-this->seqlist[iseq].t0;
    ra= (double) nLineages/ (double)this->seqlist[iseq].N;
    if (nGen<=30) {
      if (ra < (0.0031*nGen*nGen - 0.053*nGen + 0.7197)) {OK=1;}
    }
    else if (nGen<=100){
      if (ra < (0.033*nGen + 1.7)) {OK=1;}
    }
    else if (ra<0.5) {OK=1;}
	//cout <<"nGen="<<nGen<<"   nl="<<nLineages<<"   Ne="<<this->seqlist[iseq].N<<"    nl/Ne="<<ra<<"    ";
    return OK;
  }

  /**
   * Struct ParticleC : tire au hasard (uniformément) une lignée ancestrale parmi celles de la population requise
   */
  int ParticleC::draw_node(int loc,int iseq,int nLineages) {
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
  void ParticleC::coal_pop(int loc,int iseq) {
    bool trace = false;
    if (trace){
      cout <<"\n";
      cout << "debut COAL pop="<<this->seqlist[iseq].pop<<"  nbranches=" << this->gt[loc].nbranches <<"   nnodes="<<this->gt[loc].nnodes ;
      cout<<"   Ne="<<this->seqlist[iseq].N<<"   t0="<<this->seqlist[iseq].t0<<"   t1="<<this->seqlist[iseq].t1;
      cout<<"    coeffcoal="<<this->locuslist[loc].coeffcoal <<"\n";}
    int nLineages=0;
    bool final=false;
    double lra;
    for (int i=0;i<this->gt[loc].nnodes;i++){
      if (this->gt[loc].nodes[i].pop == this->seqlist[iseq].pop) {nLineages +=1;}
    }
    //cout<<"debut coal_pop nbranches="<< this->gt[loc].nbranches << "  nLineages=" << nLineages << "\n";
    if (this->seqlist[iseq].t1<0) {final=true;}
    if (this->seqlist[iseq].N<1) {std::cout << "coal_pop : population size <1 ("<<this->seqlist[iseq].N<<") \n" ;exit(100);}
    if (trace) cout<<"pop size>=1\n";
    if (evalcriterium(iseq,nLineages) == 1) {		//CONTINUOUS APPROXIMATION
		if (trace) cout << "Approximation continue final="<< final << "   nLineages=" << nLineages << "  pop=" <<this->seqlist[iseq].pop << "\n";
		double start = this->seqlist[iseq].t0;
		if (trace) cout << "start initial= " << start << "\n";
		while ((nLineages>1)and((final)or((not final)and(start<this->seqlist[iseq].t1)))) {
			double ra = this->mw.random();
			while (ra == 0.0) {ra = this->mw.random();}
			lra = log(ra);
			start -= (this->locuslist[loc].coeffcoal*this->seqlist[iseq].N/nLineages/(nLineages-1.0))*lra;
			if (trace)  cout << "coeffcoal = " << this->locuslist[loc].coeffcoal << "   N = " << this->seqlist[iseq].N << "   nl/(nl-1) = " << nLineages/(nLineages-1.0) << "\n";
			if (trace) cout << "start courant= " << start << "  log(ra)=" << lra << "\n";
			if ((final)or((not final)and(start<this->seqlist[iseq].t1))) {
				this->gt[loc].nodes[this->gt[loc].nnodes].pop = this->seqlist[iseq].pop;
				this->gt[loc].nodes[this->gt[loc].nnodes].height=start;
				this->gt[loc].nnodes++;
				if (trace) cout <<"nouveau noeud = "<<this->gt[loc].nnodes-1<<"    nLineages = "<<nLineages<<"\n";
				this->gt[loc].branches[this->gt[loc].nbranches].top=this->gt[loc].nnodes-1;
				this->gt[loc].branches[this->gt[loc].nbranches].bottom=draw_node(loc,iseq,nLineages);
				if (trace) cout << "retour premier noeud tiré : " << this->gt[loc].branches[this->gt[loc].nbranches].bottom <<"\n";
				nLineages--;
				this->gt[loc].branches[this->gt[loc].nbranches].length=this->gt[loc].nodes[this->gt[loc].branches[this->gt[loc].nbranches].top].height-this->gt[loc].nodes[this->gt[loc].branches[this->gt[loc].nbranches].bottom].height;
				this->gt[loc].nbranches++;
				this->gt[loc].branches[this->gt[loc].nbranches].top=this->gt[loc].nnodes-1;
				this->gt[loc].branches[this->gt[loc].nbranches].bottom=draw_node(loc,iseq,nLineages);
				if (trace) cout << "retour second noeud tiré : " << this->gt[loc].branches[this->gt[loc].nbranches].bottom <<"\n";
				this->gt[loc].branches[this->gt[loc].nbranches].length=this->gt[loc].nodes[this->gt[loc].branches[this->gt[loc].nbranches].top].height-this->gt[loc].nodes[this->gt[loc].branches[this->gt[loc].nbranches].bottom].height;
				this->gt[loc].nbranches++;
				if (trace) cout << "nbranches = " << this->gt[loc].nbranches << "\n";
			}
		}
    }
    else {											//GENERATION PER GENERATION
      //cout << "Génération par génération pour le locus "<<loc<<"\n";
      //int *numtire,*num,*knum;
      int gstart= (int)(this->seqlist[iseq].t0+0.5);
      int Ne= (int) (0.5*this->locuslist[loc].coeffcoal*this->seqlist[iseq].N+0.5);
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

  void ParticleC::pool_pop(int loc,int iseq) {
    for (int i=0;i<this->gt[loc].nnodes;i++){
      if (this->gt[loc].nodes[i].pop == this->seqlist[iseq].pop1) {
	this->gt[loc].nodes[i].pop = this->seqlist[iseq].pop;
      }
    }
  }

  void ParticleC::split_pop(int loc,int iseq) {
    for (int i=0;i<this->gt[loc].nnodes;i++){
      if (this->gt[loc].nodes[i].pop == this->seqlist[iseq].pop){
	if (this->mw.random()<this->seqlist[iseq].admixrate) {this->gt[loc].nodes[i].pop = this->seqlist[iseq].pop1;}
	else                                                 {this->gt[loc].nodes[i].pop = this->seqlist[iseq].pop2;}
      }
    }
  }

  void ParticleC::add_sample(int loc,int iseq) {
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



  void ParticleC::put_mutations(int loc) {
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

void ParticleC::cherche_branchesOK(int loc) {
	for (int b=0;b<this->gt[loc].nbranches;b++) this->gt[loc].branches[b].OK = true;
	for (int b=0;b<this->gt[loc].nbranches;b++) this->gt[loc].branches[b].OKOK = true;
	this->gt[loc].nbOK  =this->gt[loc].nbranches;
	this->gt[loc].nbOKOK=this->gt[loc].nbranches;
	if (this->refnindtot<1) return;
	int j,nodemrca=0,f1,f2,b;
	bool mrca;
	//mise à true de nodes[i].OK pour les gènes des populations de référence
	//printf("npopref = %3d     popref[0]=%3d\n",this->data.npopref,this->data.popref[0]);
	int cat = this->locuslist[loc].type % 5;
	int sa=0,ind=0;
	for (int i=0;i<this->gt[loc].ngenes;i++) {
		this->gt[loc].nodes[i].OK = this->ref[cat][sa][ind];
		ind++;
		if (ind==this->data.ss[cat][sa]) {ind=0;sa++;}
	}
	//calcul du nombre de descendants échantillonnés par noeud
	sa=0,ind=0;
	for (int i=0;i<this->gt[loc].nnodes;i++) {
		this->gt[loc].nodes[i].ndat=0;
		if (i<this->gt[loc].ngenes){        
			if (this->dat[cat][sa][ind]) this->gt[loc].nodes[i].ndat=1;
			ind++;
			if (ind==this->data.ss[cat][sa]) {ind=0;sa++;}
		} else {
			b=0;
			while (i!=this->gt[loc].branches[b].top) b++;
			f1=this->gt[loc].branches[b].bottom;b++;
			while (i!=this->gt[loc].branches[b].top) b++;
			f2=this->gt[loc].branches[b].bottom;
			this->gt[loc].nodes[i].ndat=this->gt[loc].nodes[f1].ndat+this->gt[loc].nodes[f2].ndat;
		}
	}
	//mise à false des branches de l'arbre
	for (b=0;b<this->gt[loc].nbranches;b++) this->gt[loc].branches[b].OK = false;
	for (b=0;b<this->gt[loc].nbranches;b++) this->gt[loc].branches[b].OKOK = false;
	//remplissage des brhaut des nodes
	for (b=0;b<this->gt[loc].nbranches;b++) {
		this->gt[loc].nodes[this->gt[loc].branches[b].bottom].brhaut = b;
	}
	//recherche du mrca des gènes des pop de référence et mise à true des branches.OK y conduisant
	//et dont les noeuds bas ont au moins un descendant échantillonné
	this->gt[loc].nbOK=0;this->gt[loc].nbOKOK=0;
	for (int i=0;i<this->gt[loc].ngenes-1;i++) {
		if (this->gt[loc].nodes[i].OK) {
			for (int j=i+1;j<this->gt[loc].ngenes;j++) {
				if (this->gt[loc].nodes[j].OK) {
					f1=i;f2=j;b=0;
					do {
						if (this->gt[loc].branches[b].bottom == f1) {
							this->gt[loc].branches[b].OK=true;this->gt[loc].nbOK++;
							if ((f1!=i)and(this->gt[loc].nodes[f1].ndat>0)) {this->gt[loc].branches[b].OKOK=true;this->gt[loc].nbOKOK++;}
							f1 = this->gt[loc].branches[b].top;
						}
						if (this->gt[loc].branches[b].bottom == f2) {
							this->gt[loc].branches[b].OK=true;this->gt[loc].nbOK++;
							if ((f2!=j)and(this->gt[loc].nodes[f2].ndat>0)) { this->gt[loc].branches[b].OKOK=true;this->gt[loc].nbOKOK++;}
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
	/*for (int i=0;i<this->gt[loc].ngenes;i++) {
		f1=i;
		do {
			b = this->gt[loc].nodes[f1].brhaut;
			f1 = this->gt[loc].branches[b].top;
		} while (f1<nodemrca);
		if (f1==nodemrca) {  // le gène i descend du mrcalengthtotOK
			f1=i;
			do {
				b = this->gt[loc].nodes[f1].brhaut;
				f1 = this->gt[loc].branches[b].top;
				this->gt[loc].branches[b].OK = true;
			} while (f1<nodemrca);
		}
	}*/
	if (debuglevel==20){
		cout<<"\nLOCUS "<<loc<<"\n";
		for (b=0;b<this->gt[loc].nbranches;b++){
			printf("branche %3d   bas %3d   haut %3d   longueur %8.3e  (nodemrca %3d)",b,this->gt[loc].branches[b].bottom,this->gt[loc].branches[b].top,this->gt[loc].branches[b].length,nodemrca);
			if (this->gt[loc].branches[b].OK) printf("   OK\n"); else printf("\n");
		}
	}
}

void ParticleC::put_one_mutation(int loc) {
    this->gt[loc].nmutot=1;
    double ra,r,s=0.0,lengthtotOK=0.0,lengthtotOKOK=0.0;
    int b;
    for (b=0;b<this->gt[loc].nbranches;b++) {
      if (this->gt[loc].branches[b].OK) lengthtotOK +=this->gt[loc].branches[b].length;
      if (this->gt[loc].branches[b].OKOK) lengthtotOKOK +=this->gt[loc].branches[b].length;
      this->gt[loc].branches[b].nmut = 0;
      //if (loc==1) cout<<"longueur de la branche "<<b<<" = "<<this->gt[loc].branches[b].length<<"\n";
    }
    //cout<<"   lOKOK="<<lengthtotOKOK<<"   lOK="<<lengthtotOK<<"\n";
	//cout<<lengthtotOKOK/lengthtotOK<<"\n";
    this->locuslist[loc].weight=lengthtotOKOK/lengthtotOK;
	if (debuglevel==11) cout<<"weight="<<this->locuslist[loc].weight<<"   lOKOK="<<lengthtotOKOK<<"   lOK="<<lengthtotOK<<"    ";
    if (this->locuslist[loc].weight==0.0) return;
	ra=this->mw.random();
    r=ra*lengthtotOKOK;
    //if (loc<1) cout<<"lengthtot="<<lengthtot<<"     r="<<r<<"\n";
	
	b=0;while (not this->gt[loc].branches[b].OKOK) b++;
    s=this->gt[loc].branches[b].length;
    while (s<r) {b++; if (this->gt[loc].branches[b].OKOK) s +=this->gt[loc].branches[b].length;};
    this->gt[loc].branches[b].nmut = 1;
	if ((b==0)and(not this->gt[loc].branches[b].OKOK)) {
		cout<<"lengthtot="<<lengthtotOKOK<<"     r="<<r<<"    s="<<s<<"\n";
		exit(2);
	}
    //if (loc<10) cout<<"mutation dans la branche "<<b<<" sur "<<this->gt[loc].nbranches<<"\n";
    //if (loc<10) cout<<nOK<<" branches mutables sur "<<this->gt[loc].nbranches<<"\n";
    //if (debuglevel==20) cout<<"locus "<<loc<<"   numero de la branche mutée : "<<b<<" ("<<this->gt[loc].nbranches<<")"<<"   longueur = "<<this->gt[loc].branches[b].length<<" sur "<<lengthtot<<"\n";
  } 


  void ParticleC::mute(int loc, int numut, int b) {
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
		  //cout<<"la branche "<<b<<" a muté ------------------------------\n";
	  }
  }

  char ParticleC::draw_nuc(int loc) {
    double ra=this->mw.random();
    if (ra<this->locuslist[loc].pi_A)															return 'A';
    else if (ra<this->locuslist[loc].pi_A+this->locuslist[loc].pi_C)							return 'C';
    else if (ra<this->locuslist[loc].pi_A+this->locuslist[loc].pi_C+this->locuslist[loc].pi_G)	return 'G';
    else																						return 'T';
  }

  string ParticleC::init_dnaseq(int loc) {
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

  int ParticleC::cree_haplo(int loc) {
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
    int len = 0, cat = (this->locuslist[loc].type % 5);
    if (debuglevel==10) cout <<"avant la boucle while ngenes="<<this->gt[loc].ngenes<<"\n";
    while (anc>=this->gt[loc].ngenes) {
      if (debuglevel==10) cout << "locus " << loc << "   ngenes = " << this->gt[loc].ngenes << "   anc = " << anc << "\n";
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
	for (int ind=0;ind<this->data.ss[cat][sa];ind++) cout<<ind<<"  "<<this->gt[loc].nodes[ind+sa*this->data.ss[cat][sa]].dna<<"\n";
      }
    }
    this->locuslist[loc].sitmut.clear();
    int sa0=0,sa2=this->data.ss[cat][sa0];
    if (debuglevel==10) cout<<"sa2="<<sa2<<"\n";
    int sa,ind,ngref;
    if (debuglevel==10) cout<< "tirage au hasard des gènes avant attribution aux individus\n";
    ordre.resize(this->data.nsample);
    if (debuglevel==10) cout<<"apres resize de ordre\n";
	ind=0;
    for (sa=0;sa<this->data.nsample;sa++) {
		if (this->locuslist[loc].type>=10){
			ngref=0;for (int i=0;i<this->data.ss[cat][sa];i++) if (this->ref[cat][sa][i]) ngref++;
			ordre[sa] = melange2(this->mw,ngref,this->data.ss[cat][sa]);
			//for (int i=0;i<this->data.ss[cat][sa];i++) ordre[sa][i] = i;
		} else {
			ordre[sa] = melange(this->mw,this->data.ss[cat][sa]);
		}
      if (sa>0) {for (int i=0;i<this->data.ss[cat][sa];i++) ordre[sa][i]+=ind;}
      ind +=this->data.ss[cat][sa];
    }
    if((debuglevel==9)and(loc==10)) {
      for (int sa=0;sa<this->nsample;sa++) {
	for (int ind=0;ind<this->data.ss[cat][sa];ind++) cout<<"ordre["<<sa<<"]["<<ind<<"] = "<<ordre[sa][ind]<<"\n";
      }
    }
    if (debuglevel==10) cout<<"apres ordre\n";
    if (this->locuslist[loc].type<5) {      //MICROSAT
      
      this->locuslist[loc].haplomic = new int*[this->data.nsample];
      for (sa=0;sa<this->data.nsample;sa++) this->locuslist[loc].haplomic[sa] = new int [this->data.ss[cat][sa]];
      sa=0;ind=0;
      for (int i=0;i<this->gt[loc].ngenes;i++) {
    	  if (this->gt[loc].nodes[ordre[sa][ind]].state == 10000) {ordre.clear();return 2;}
    	  this->locuslist[loc].haplomic[sa][ind] = this->gt[loc].nodes[ordre[sa][ind]].state;
    	  ind++;
    	  if (ind==this->data.ss[cat][sa]) {sa++;ind=0;}  // TODO: bug ici ?
      }
      if (debuglevel==10) cout<<"apres repartition dans le sample 0\n";
    }
    else if (this->locuslist[loc].type<10) {//DNA SEQUENCES
      this->locuslist[loc].haplodna = new string*[this->data.nsample];
      for (sa=0;sa<this->data.nsample;sa++) this->locuslist[loc].haplodna[sa] = new string [this->data.ss[cat][sa]];
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
	ind++;if (ind==this->data.ss[cat][sa]) {sa++;ind=0;}
      }
    }
    else {									//SNP
		if (debuglevel==10) cout << " firstime = " << firstime << endl;
		if (this->locuslist[loc].firstime){
			this->locuslist[loc].haplosnp = new short int*[this->data.nsample];
			for (sa=0;sa<this->data.nsample;sa++) this->locuslist[loc].haplosnp[sa] = new short int [this->data.ss[cat][sa]];
			this->locuslist[loc].firstime=false;
		}
		sa=0;ind=0;
		for (int i=0;i<this->gt[loc].ngenes;i++) {
			if (this->gt[loc].nodes[ordre[sa][ind]].state == 10000) {ordre.clear();return 2;}
			this->locuslist[loc].haplosnp[sa][ind] = (short int)this->gt[loc].nodes[ordre[sa][ind]].state;
			// if (loc<1) cout<<this->locuslist[loc].haplosnp[sa][ind]<<"   ";
			ind++;if (ind==this->data.ss[cat][sa]) {sa++;ind=0;}
			// if ((loc<1)and(ind==0))cout<<"\n";
		}
		if (debuglevel==10) cout<<"apres repartition dans le sample 0\n";
    }
    ordre.clear();
    if (debuglevel==7)cout<<"Locus "<<loc<<"    nmutotdans cree_haplo = "<<this->gt[loc].nmutot<<"\n";
    return 0;
  }

  string ParticleC::verifytree() {
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
    
  bool ParticleC::polymref(int loc) {
    int nr=0,n1r=0,nd=0,n1d=0;
    double p;
    bool polyref=false, polydat=false,poly;
    //cout<<"dans polymref debut\n";
	if (this->refnindtot<1) return true;
    int cat = (this->locuslist[loc].type % 5);
    for (int sa=0;sa<this->nsample;sa++) {
		for (int i=0;i<this->data.ss[cat][sa];i++) {
			if (this->ref[cat][sa][i]) {
				nr +=1;
				n1r +=(int)this->locuslist[loc].haplosnp[sa][i];
			}
			if (this->dat[cat][sa][i]) {
				nd +=1;
				n1d +=(int)this->locuslist[loc].haplosnp[sa][i];
			}
		}
    } 
    //cout<<"dans polymref fin   n1="<<n1<<"   n="<<n<<"\n";
    if (nr==0) exit(1);
    p=(double)n1r/(double)nr;
    if (p > 0.5) polyref=(p <= 1.0-this->reffreqmin);
    else         polyref=(p >= this->reffreqmin);
	polydat = (n1d>0)and(n1d<nd);
	poly = ((polyref) and (polydat));
    if (loc==0) cout<<"polymref   refnindtot="<<this->refnindtot<<"   reffreqmin="<<this->reffreqmin<<"   p="<<p<<"   poly="<<poly<<"\n";
	if (debuglevel==20) {
		if (poly) cout<<"LOCUS ACCEPTE\n"; else cout<<"LOCUS REFUSE\n";
	}  
	return poly;
  }
	
  int ParticleC::dosimulpart(int numscen){
	  if (debuglevel==5)        {cout<<"debut de dosimulpart  nloc="<<this->nloc<<"\n";fflush(stdin);}
	  vector <int> simulOK;
	  string checktree;
	  int *emptyPop,loc,nlocutil=0,loca=-1,simOK;
	  bool treedone,dnaloc=false,trouve,snpOK;
	  int locus = 0, sa, indiv, nuc;
	  //cout<<"this->nloc="<<this->nloc<<"\n";
	  this->ntentes = 0;this->naccept = 0;
	  simulOK.resize(this->nloc);
	  GeneTreeC GeneTreeY, GeneTreeM;
	  if (debuglevel==10) cout<<"avant draw scenario\n";fflush(stdin);
	  this->drawscenario(numscen);
	  if (debuglevel==10) cout <<"avant setHistparamValue\n";fflush(stdin);
	  this->setHistParamValue();
	  if (debuglevel==10) cout << "apres setHistParamValue\n";fflush(stdin);
	   if (debuglevel==-1) cout<<"scen.nparam = "<<this->scen.nparam<<"\n";
	  if (debuglevel==-1) for (int k=0;k<this->scen.nparam;k++){
	           	cout << this->scen.histparam[k].value << "   ";fflush(stdin);}
	  this->setSequence();
	  if (debuglevel==10) cout <<"apres setSequence\n";
	  if (debuglevel==5) cout <<"avant checktree\n";
	  checktree=this->verifytree();
	  if (checktree!="") {
		  FILE *flog;
		  cout<<checktree<<"\n";
		  this->scen.ecris();
		  checktree="A gene genealogy failed in scenario "+IntToString(numscen+1)+". Check scenario and prior consistency.\n  ";
		  flog=fopen(reftablelogfilename.c_str(),"w");fprintf(flog,"%s",checktree.c_str());fclose(flog);
		  exit(1);
	  }
	  if (debuglevel==5) cout <<"apres checktree\n";
	  bool gtYexist=false, gtMexist=false,*gtexist;
	  this->gt = new GeneTreeC[this->nloc];
	  emptyPop = new int[this->scen.popmax+1];
	  //cout << "particule " << ipart <<"   nparam="<<this->scen.nparam<<"\n";
	  //for (int k=0;k<this->scen.nparam;k++){
	  //	cout << this->scen.histparam[k].value << "   ";
	  //}
	  //cout << "\n";
	  if (debuglevel==5) cout <<"avant simulOK=-1\n";
	  for (loc=0;loc<this->nloc;loc++) simulOK[loc]=-1;
	  if (debuglevel==5) cout <<"avant setMutParammoyValue \n";
	  setMutParammoyValue();
	  if (debuglevel==5) cout <<"apres setMutParammoyValue \n";
	  if (debuglevel==10) cout<<"nloc="<<this->nloc<<"\n";fflush(stdin);
	  gtexist = new bool[this->nloc];
	  for (loc=0;loc<this->nloc;loc++) {gtexist[loc] = false;simulOK[loc]= 0;}
	  for (int gr=1;gr<this->ngr+1;gr++) nlocutil +=this->grouplist[gr].nloc;
	  this->sumweight = 0.0;
	for (loc=0;loc<this->nloc;loc++) {
		if (this->locuslist[loc].groupe>0) { //On se limite aux locus inclus dans un groupe
			if (debuglevel==10) cout<<"debut de la boucle du locus "<<loc<<"\n";fflush(stdin);
				this->ntentes++;
				if (this->locuslist[loc].type <10) setMutParamValue(loc);
				if ((this->locuslist[loc].type >4)and(this->locuslist[loc].type <10)) {
					comp_matQ(loc);
					dnaloc=true;
				} else dnaloc=false;
				treedone=false;
				if ((this->locuslist[loc].type % 5) == 3) {
					if (gtYexist) {this->gt[loc] = GeneTreeY; treedone=true;}
				}
				else if ((locuslist[loc].type % 5) == 4) {
					if (debuglevel==10) cout << "coucou   gtMexist=" << gtMexist <<"\n";
					if (gtMexist) {this->gt[loc] = GeneTreeM; treedone=true;}
				}
				if (not treedone) {
					if (debuglevel==10) cout << "avant init_tree \n";
					init_tree(this->gt[loc], loc, gtexist[loc]);
					if (debuglevel==10){
						cout << "initialisation de l'arbre du locus " << loc  << "    ngenes="<< this->gt[loc].ngenes<< "   nseq="<< this->nseq <<"\n";
						cout<< "scenario "<<this->scen.number<<"\n";
					}
					for (int p=0;p<this->scen.popmax+1;p++) {emptyPop[p]=1;} //True
					//if (not gtexist[loc]) for (int p=0;p<this->scen.popmax+1;p++) {emptyPop[p]=1;} //True
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
					if (not gtYexist) {if ((locuslist[loc].type % 5) == 3) {GeneTreeY  = this->gt[loc]; gtYexist=true;}}
					if (not gtMexist) {if ((locuslist[loc].type % 5) == 4) {GeneTreeM  = this->gt[loc]; gtMexist=true;}}
				}
				/* mutations */
				//cout<<"avant le traitement des mutations locus "<<loc<<"\n";
				snpOK = true;
				if (this->locuslist[loc].type <10){
					put_mutations(loc);
					if (debuglevel==10) cout << "Locus " <<loc << "  apres put_mutations\n";
					simulOK[loc]=cree_haplo(loc);
					if (debuglevel==10) cout << "Locus " <<loc << "  apres cree_haplo   : simOK[loc] ="<<simulOK[loc]<<"\n";fflush(stdin);
					this->naccept++;
					this->sumweight +=1.0;
				} else {
				  //if (loc==0) cout<<"this->refnindtot="<<this->refnindtot<<"\n";
				this->locuslist[loc].firstime = true;
				cherche_branchesOK(loc);
				if (debuglevel==11) cout<<"locus "<<loc<<"   nbOK = "<<this->gt[loc].nbOK<<"   nbOKOK = "<<this->gt[loc].nbOKOK<<"\n";
				put_one_mutation(loc);
				if (this->locuslist[loc].weight>0.0) {
					simulOK[loc]=cree_haplo(loc);
					if (debuglevel==11) cout<<"apres cree_haplo  weight="<<this->locuslist[loc].weight<<"\n";
					if (polymref(loc)) {
						this->naccept++;
						if (debuglevel==11) cout<<"apres polymref  weight="<<this->locuslist[loc].weight<<"\n";
					}else {
						this->locuslist[loc].weight=0.0;
						simulOK[loc]=0;
						gtexist[loc]=true; 
						loc--;
					} 
				} else {
					simulOK[loc]=0;
					gtexist[loc]=true; 
					loc--;
				}
				this->sumweight +=this->locuslist[loc].weight;
				//if (debuglevel==11) cout<<"weight = "<<this->sumweight<<"\n";
				this->locpol = this->sumweight/(double)this->ntentes;
				if ((this->ntentes>=nlocutil)and(this->locpol<0.2)) this->sumweight=-1.0;
			}
			if (this->sumweight<0.0) break;
		}
		if (this->sumweight<0.0) break;
	}	//LOOP ON loc
	if (this->sumweight>=0.0) this->weight = this->sumweight/(double)nlocutil;
	else this->weight = 0.0;
	if (debuglevel==11) cout<<"poids de la particule = "<<this->weight<<"   taux de polymorphisme = "<<this->locpol;
	if (debuglevel==11) cout<<"    "<<this->naccept<<" sur = "<<this->ntentes<<"\n";
	if (this->weight>0.0){  
		if (debuglevel==10) cout<<this->data.nmisshap<<" donnees manquantes et "<<this->data.nmissnuc<<" nucleotides manquants\n";fflush(stdin);
		if (this->data.nmisshap>0) {
			for (int i=0;i<this->data.nmisshap;i++) {
				locus=this->data.misshap[i].locus;
				if (this->locuslist[locus].groupe>0) {
					sa=this->data.misshap[i].sample;indiv=this->data.misshap[i].indiv;
					//cout<<"MISSHAP   locus "<<locus<<"  sample "<<sa<<"  indiv "<<indiv<<"\n";
					if (this->locuslist[locus].type<5) this->locuslist[locus].haplomic[sa][indiv] = MICMISSING;
					else                               this->locuslist[locus].haplodna[sa][indiv] = SEQMISSING;
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
					while (   (k < (int)this->locuslist[locus].sitmut.size()) and (not trouve)   ) {
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
				}
			}
		}
	}
	if (debuglevel==11) cout<<"avant le sitmut2.clear()\n";
	for (loc=0;loc<this->nloc;loc++) {
		if (this->locuslist[loc].groupe>0) {
			if ((this->locuslist[loc].type>4)and(simulOK[loc]>-1)) this->locuslist[loc].sitmut2.clear();
		}
	}
	  if (debuglevel==11) cout<<"avant les delete\n";fflush(stdin);
	  delete [] emptyPop;
	  delete [] this->seqlist;
	  for (int loc=0;loc<this->nloc;loc++) {
		  if ((this->locuslist[loc].groupe>0)and(simulOK[loc]>-1)) {
			  if (debuglevel==10) cout<<"deletetree du locus "<<loc<<"\n";
			  // if(this->locuslist[loc].type>4) deletetree(this->gt[loc],true);
			  // else deletetree(this->gt[loc],false);
		  }
	  }
	  delete [] this->gt;
	  // if (gtYexist) deletetree(GeneTreeY,dnaloc);
	  // if (gtMexist) deletetree(GeneTreeM,dnaloc);
	  //if (trace) cout << "Fin de dosimulpart \n";
	  if (this->weight>0.0){
		simOK=0;for (int loc=0;loc<this->nloc;loc++) {if (this->locuslist[loc].groupe>0) simOK+=simulOK[loc];}
	}
	  if (debuglevel==11) cout<<"fin de dosimulpart   simOK="<<simOK<<"\n";fflush(stdin);
	  if (debuglevel==7) {
		  for (int loc=0;loc<this->nloc;loc++) if((this->locuslist[loc].type>4)and(this->locuslist[loc].type<10)) {
			  cout<<"Locus "<<loc<<"\n";
			  for(int sa=0;sa<this->nsample;sa++) {cout<<this->locuslist[loc].haplodna[sa][0]<<"\n";cout<<this->locuslist[loc].haplodna[sa][1]<<"\n\n";}
		  }
	  }
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

  string ParticleC::dogenepop(){
	  string sgp,sind,snum;
	  int iloc,*k,ty;
	  k = new int[this->nloc];
//	  cout << "*** ss=" << endl;
//	  for(int sa = 0; sa < data.nsample; sa++)
//		  cout << "ss[0][" << sa << "] = " <<this->data.ss[0][sa] << endl;
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
					  try{
					  sgp +="   "+IntToString3(this->locuslist[iloc].haplomic[ech][k[iloc]]);k[iloc]++;
					  sgp +=IntToString3(this->locuslist[iloc].haplomic[ech][k[iloc]]);k[iloc]++;
					  } catch (std::exception &e) {
						  cout << e.what() << endl;
						  cout << " ech, iloc, k[iloc] =" << ech << ", " << iloc << ", " << k[iloc] << endl;
						  cout << "locuslist[iloc].haplomic[ech][k[iloc]] = " << locuslist[iloc].haplomic[ech][k[iloc]] << endl;
						  //throw e;
					  }
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

	string ParticleC::dodataSNP(){
		string sgp,sind,spop,sex,ligne;
		short int g;		
		int *ig;
		ig = new int[this->nloc];
		sgp="IND   SEX   POP   ";
		for (int loc=0;loc<this->nloc;loc++) {
			switch(this->locuslist[loc].type) {
				case 10 : sgp +=" A";break;
				case 11 : sgp +=" H";break;
				case 12 : sgp +=" X";break;
				case 13 : sgp +=" Y";break;
				case 14 : sgp +=" M";break;
			}
		}
		sgp +="\n";
		for (int ech=0;ech<this->data.nsample;ech++) {
			spop="P"+IntToString(ech+1);while (spop.length()<6) spop +=" ";
			for (int loc=0;loc<this->nloc;loc++) ig[loc] = 0;
			for (int ind=0;ind<this->data.nind[ech];ind++) {
				sind="P"+IntToString(ech+1)+"_"+IntToString(ind+1);while (sind.length()<6) sind +=" ";
				if (this->data.indivsexe[ech][ind] == 1) sex = " M    ";
				else sex = " F    ";
				ligne = sind+sex+spop;
				for (int loc=0;loc<this->nloc;loc++){
					switch(this->locuslist[loc].type) {
						case 10 : 
							ligne +=" "+ShortIntToString(this->locuslist[loc].haplosnp[ech][ig[loc]]+this->locuslist[loc].haplosnp[ech][ig[loc]+1]);
							ig[loc] +=2;
							break;
						case 11 : 
							ligne +=" "+ShortIntToString(this->locuslist[loc].haplosnp[ech][ig[loc]]);
							ig[loc] +=1;
							break;
						case 12 : 
							if (this->data.indivsexe[ech][ind] == 1){
								ligne +=" "+ShortIntToString(this->locuslist[loc].haplosnp[ech][ig[loc]]);
								ig[loc] +=1;
							} else {
								ligne +=" "+ShortIntToString(this->locuslist[loc].haplosnp[ech][ig[loc]]+this->locuslist[loc].haplosnp[ech][ig[loc]+1]);
								ig[loc] +=2;
							}
							break;
						case 13 : 
							if (this->data.indivsexe[ech][ind] == 1){
								ligne +=" "+ShortIntToString(this->locuslist[loc].haplosnp[ech][ig[loc]]);
								ig[loc] +=1;
							} else {
								ligne +=" 9";
							}
							break;
						case 14 : 
							ligne +=" "+ShortIntToString(this->locuslist[loc].haplosnp[ech][ig[loc]]);
							ig[loc] +=1;
							break;
					}
				}
				//cout<<ligne<<"\n";
				sgp +=ligne+"\n";
			}
		}
		return sgp;
	}
