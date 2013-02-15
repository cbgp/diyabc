/*
 * particleset.cpp
 *
 *  Created on: 23 sept. 2010
 *      Author: cornuet
 */
#include <string>
#include <omp.h>
#include <cstdlib>

/*
#ifndef HEADER
#include "header.cpp"
#define HEADER
#endif

#ifndef REFTABLE
#include "reftable.cpp"
#define REFTABLE
#endif
*/
#include "header.h"
#include "reftable.h"
#include "particuleC.h"
#include "particleset.h"

using namespace std;

extern enregC *enreg;
extern long double **phistar;
extern long double **phistarOK;
extern int debuglevel;
extern ReftableC rt;
extern string scurfile;

long int ntentes,naccept;


/**
 * Structure ParticleSet : remplissage de la partie data de la particule p
 */
void ParticleSetC::setdata(int p) {
	this->particule[p].threshold = this->header.threshold;
	this->particule[p].sexratio = this->header.dataobs.sexratio;
	this->particule[p].data.nsample = this->header.dataobs.nsample;
	this->particule[p].nsample = this->header.dataobs.nsample;
	this->particule[p].data.nind.resize(this->header.dataobs.nsample);
	this->particule[p].data.indivsexe.resize(this->header.dataobs.nsample);
	this->particule[p].data.catexist = new bool[5];
	for (int cat=0;cat<5;cat++) this->particule[p].data.catexist[cat] =  this->header.dataobs.catexist[cat];
	this->particule[p].data.ss.resize(5);
	for (int cat=0;cat<5;cat++) {
		if (this->particule[p].data.catexist[cat]) {
			this->particule[p].data.ss[cat].resize(this->header.dataobs.nsample);
			for (int sa=0;sa<this->header.dataobs.nsample;sa++) this->particule[p].data.ss[cat][sa] = this->header.dataobs.ss[cat][sa];
			/*if (p==0) {
					cout<<"dans particule[0]\n";
					for (int sa=0;sa<this->header.dataobs.nsample;sa++) cout <<this->particule[p].data.ss[cat][sa]<<"   ";cout<<"\n";
				}*/
		}
	}
	for (int i=0;i<this->header.dataobs.nsample;i++) {
		this->particule[p].data.nind[i] = this->header.dataobs.nind[i];
		this->particule[p].data.indivsexe[i].resize(this->header.dataobs.nind[i]);
		for (int j=0;j<header.dataobs.nind[i];j++) this->particule[p].data.indivsexe[i][j] = this->header.dataobs.indivsexe[i][j];
	}
	this->particule[p].data.nmisshap = this->header.dataobs.nmisshap;
	if (this->particule[p].data.nmisshap>0) {
		this->particule[p].data.misshap = new MissingHaplo[this->particule[p].data.nmisshap];
		for (int i=0;i<this->header.dataobs.nmisshap;i++) {
			this->particule[p].data.misshap[i].locus  = this->header.dataobs.misshap[i].locus;
			this->particule[p].data.misshap[i].sample = this->header.dataobs.misshap[i].sample;
			this->particule[p].data.misshap[i].indiv  = this->header.dataobs.misshap[i].indiv;
		}
	}
	this->particule[p].data.nmisssnp = this->header.dataobs.nmisssnp;
	if (this->particule[p].data.nmisssnp>0) {
		this->particule[p].data.misssnp = new MissingHaplo[this->particule[p].data.nmisssnp];
		for (int i=0;i<this->header.dataobs.nmisssnp;i++) {
			this->particule[p].data.misssnp[i].locus  = this->header.dataobs.misssnp[i].locus;
			this->particule[p].data.misssnp[i].sample = this->header.dataobs.misssnp[i].sample;
			this->particule[p].data.misssnp[i].indiv  = this->header.dataobs.misssnp[i].indiv;
		}
	}
	this->particule[p].data.nmissnuc = this->header.dataobs.nmissnuc;
	if (this->particule[p].data.nmissnuc>0) {
		this->particule[p].data.missnuc = new MissingNuc[this->particule[p].data.nmissnuc];
		for (int i=0;i<this->header.dataobs.nmissnuc;i++) {
			this->particule[p].data.missnuc[i].locus  = this->header.dataobs.missnuc[i].locus;
			this->particule[p].data.missnuc[i].sample = this->header.dataobs.missnuc[i].sample;
			this->particule[p].data.missnuc[i].indiv  = this->header.dataobs.missnuc[i].indiv;
			this->particule[p].data.missnuc[i].nuc    = this->header.dataobs.missnuc[i].nuc;
		}
	}
	this->particule[p].reffreqmin = this->header.reffreqmin;
}

/**
 * Structure ParticleSet : remplissage de la partie groupe de la particule p
 */
void ParticleSetC::setgroup(int p) {
	int ngr = this->header.ngroupes;
	//cout<<"ngr="<<ngr<<"\n";
	this->particule[p].ngr = ngr;
	this->particule[p].grouplist = new LocusGroupC[ngr+1];
	this->particule[p].grouplist[0].nloc = this->header.groupe[0].nloc;
	this->particule[p].grouplist[0].loc  = new int[this->header.groupe[0].nloc];
	for (int i=0;i<this->header.groupe[0].nloc;i++) this->particule[p].grouplist[0].loc[i] = this->header.groupe[0].loc[i];
	for (int gr=1;gr<=ngr;gr++) {
		//cout <<"groupe "<<gr<<"\n";
		this->particule[p].grouplist[gr].type =this->header.groupe[gr].type;
		this->particule[p].grouplist[gr].nloc = this->header.groupe[gr].nloc;
		this->particule[p].grouplist[gr].loc  = new int[this->header.groupe[gr].nloc];
		for (int i=0;i<this->header.groupe[gr].nloc;i++) this->particule[p].grouplist[gr].loc[i] = this->header.groupe[gr].loc[i];
		if (this->header.groupe[gr].type==0) {	//MICROSAT
			//cout <<"MICROSAT\n";
			this->particule[p].grouplist[gr].mutmoy = this->header.groupe[gr].mutmoy;
			this->particule[p].grouplist[gr].priormutmoy = this->header.groupe[gr].priormutmoy;
			this->particule[p].grouplist[gr].priormutloc = this->header.groupe[gr].priormutloc;

			this->particule[p].grouplist[gr].Pmoy = this->header.groupe[gr].Pmoy;
			this->particule[p].grouplist[gr].priorPmoy = this->header.groupe[gr].priorPmoy;
			this->particule[p].grouplist[gr].priorPloc = this->header.groupe[gr].priorPloc;

			this->particule[p].grouplist[gr].snimoy = this->header.groupe[gr].snimoy;
			this->particule[p].grouplist[gr].priorsnimoy = this->header.groupe[gr].priorsnimoy;
			this->particule[p].grouplist[gr].priorsniloc = this->header.groupe[gr].priorsniloc;
		}
		else if (this->header.groupe[gr].type==1) {							//SEQUENCES
			//cout<<"SEQUENCE\n";
			this->particule[p].grouplist[gr].mutmod = this->header.groupe[gr].mutmod;	//mutmod
			this->particule[p].grouplist[gr].p_fixe = this->header.groupe[gr].p_fixe;	//p_fixe
			this->particule[p].grouplist[gr].gams   = this->header.groupe[gr].gams;	//gams

			this->particule[p].grouplist[gr].musmoy = this->header.groupe[gr].musmoy;	//musmoy
			this->particule[p].grouplist[gr].priormusmoy = this->header.groupe[gr].priormusmoy;
			this->particule[p].grouplist[gr].priormusloc = this->header.groupe[gr].priormusloc;

			if (this->header.groupe[gr].mutmod>0){
				this->particule[p].grouplist[gr].k1moy = this->header.groupe[gr].k1moy ;	//k1moy
				this->particule[p].grouplist[gr].priork1moy = this->header.groupe[gr].priork1moy;
				this->particule[p].grouplist[gr].priork1loc = this->header.groupe[gr].priork1loc;
			}
			
			if (this->header.groupe[gr].mutmod>2){
				this->particule[p].grouplist[gr].k2moy = this->header.groupe[gr].k2moy ;	//k2moy
				this->particule[p].grouplist[gr].priork2moy = this->header.groupe[gr].priork2moy;
				this->particule[p].grouplist[gr].priork2loc = this->header.groupe[gr].priork2loc;
			}
		}
		//cout<<"nstat = "<<this->header.groupe[gr].nstat<<"\n";
		this->particule[p].grouplist[gr].nstat=this->header.groupe[gr].nstat;
		this->particule[p].grouplist[gr].sumstat = new StatC[header.groupe[gr].nstat];
		for (int i=0;i<this->header.groupe[gr].nstat;i++){
			this->particule[p].grouplist[gr].sumstat[i].cat   = header.groupe[gr].sumstat[i].cat;
			this->particule[p].grouplist[gr].sumstat[i].samp  = header.groupe[gr].sumstat[i].samp;
			this->particule[p].grouplist[gr].sumstat[i].samp1 = header.groupe[gr].sumstat[i].samp1;
			this->particule[p].grouplist[gr].sumstat[i].samp2 = header.groupe[gr].sumstat[i].samp2;
			this->particule[p].grouplist[gr].sumstat[i].numsnp = header.groupe[gr].sumstat[i].numsnp;
			//cout<<"this->particule["<<p<<"].grouplist["<<gr<<"].sumstat["<<i<<"].cat="<<this->particule[p].grouplist[gr].sumstat[i].cat<<"\n";
		}
		//cout<<"dans particleset  setgroup  nstatsnp = "<<this->header.groupe[gr].nstatsnp<<"\n";
		this->particule[p].grouplist[gr].nstatsnp=this->header.groupe[gr].nstatsnp;
		if (this->header.groupe[gr].nstatsnp>0) {
			this->particule[p].grouplist[gr].sumstatsnp = new StatsnpC[header.groupe[gr].nstatsnp];
			for (int i=0;i<this->header.groupe[gr].nstatsnp;i++){
				this->particule[p].grouplist[gr].sumstatsnp[i].cat   = header.groupe[gr].sumstatsnp[i].cat;
				this->particule[p].grouplist[gr].sumstatsnp[i].samp  = header.groupe[gr].sumstatsnp[i].samp;
				this->particule[p].grouplist[gr].sumstatsnp[i].samp1 = header.groupe[gr].sumstatsnp[i].samp1;
				this->particule[p].grouplist[gr].sumstatsnp[i].samp2 = header.groupe[gr].sumstatsnp[i].samp2;
				this->particule[p].grouplist[gr].sumstatsnp[i].defined = header.groupe[gr].sumstatsnp[i].defined;
				this->particule[p].grouplist[gr].sumstatsnp[i].sorted = header.groupe[gr].sumstatsnp[i].sorted;
				this->particule[p].grouplist[gr].sumstatsnp[i].x = new long double[header.groupe[gr].nloc];
				this->particule[p].grouplist[gr].sumstatsnp[i].w = new long double[header.groupe[gr].nloc];
			}
		}
	}
}

/**
 * Structure ParticleSet : remplissage de la partie locus de la particule p
 */
void ParticleSetC::setloci(int p) {
	int kmoy,cat;
	this->particule[p].nloc = this->header.dataobs.nloc;
	this->particule[p].locuslist = new LocusC[this->header.dataobs.nloc];
	for (int kloc=0;kloc<this->header.dataobs.nloc;kloc++){
		this->particule[p].nsample = this->header.dataobs.nsample;
		this->particule[p].locuslist[kloc].coeffcoal =  this->header.dataobs.locus[kloc].coeffcoal;
		this->particule[p].locuslist[kloc].type = this->header.dataobs.locus[kloc].type;
		this->particule[p].locuslist[kloc].groupe = this->header.dataobs.locus[kloc].groupe;
		if (this->header.dataobs.locus[kloc].type < 5) {
			kmoy=(this->header.dataobs.locus[kloc].maxi+this->header.dataobs.locus[kloc].mini)/2;
			this->particule[p].locuslist[kloc].kmin=kmoy-((this->header.dataobs.locus[kloc].motif_range/2)-1)*this->header.dataobs.locus[kloc].motif_size;
			this->particule[p].locuslist[kloc].kmax=this->particule[p].locuslist[kloc].kmin+(this->header.dataobs.locus[kloc].motif_range-1)*this->header.dataobs.locus[kloc].motif_size;
			this->particule[p].locuslist[kloc].motif_size = this->header.dataobs.locus[kloc].motif_size;
			this->particule[p].locuslist[kloc].motif_range = this->header.dataobs.locus[kloc].motif_range;
			//this->particule[p].locuslist[kloc].mut_rate = this->header.dataobs.locus[kloc].mut_rate;
			//this->particule[p].locuslist[kloc].Pgeom = this->header.dataobs.locus[kloc].Pgeom;
			//this->particule[p].locuslist[kloc].sni_rate = this->header.dataobs.locus[kloc].sni_rate;
		} else if (this->header.dataobs.locus[kloc].type < 10) {
			this->particule[p].locuslist[kloc].dnalength =  this->header.dataobs.locus[kloc].dnalength;
			this->particule[p].locuslist[kloc].kmin =  0;
			this->particule[p].locuslist[kloc].kmax =  0;
			//this->particule[p].locuslist[kloc].k2 =  this->header.dataobs.locus[kloc].k2;
			this->particule[p].locuslist[kloc].pi_A = this->header.dataobs.locus[kloc].pi_A ;
			this->particule[p].locuslist[kloc].pi_C =  this->header.dataobs.locus[kloc].pi_C;
			this->particule[p].locuslist[kloc].pi_G =  this->header.dataobs.locus[kloc].pi_G;
			this->particule[p].locuslist[kloc].pi_T =  this->header.dataobs.locus[kloc].pi_T;
			this->particule[p].locuslist[kloc].mutsit.resize(this->header.dataobs.locus[kloc].dnalength);
			for (int i=0;i<this->particule[p].locuslist[kloc].dnalength;i++) this->particule[p].locuslist[kloc].mutsit[i] = this->header.dataobs.locus[kloc].mutsit[i];
			//std::cout <<"\n";
		} else  if (this->header.dataobs.locus[kloc].type >= 10) {
		}
		//cout << "samplesize[0]="<<this->particule[p].locuslist[kloc].samplesize[0]<<"\n";
	}
}

/**
 * Structure ParticleSet : remplissage de la partie scenarios de la particule p
 */
void ParticleSetC::setscenarios (int p) {
	this->particule[p].nscenarios=this->header.nscenarios;
	this->particule[p].drawuntil = this->header.drawuntil;
	this->particule[p].scenario = new ScenarioC[this->header.nscenarios];
	for (int i=0;i<this->header.nscenarios;i++) {
		//cout<<"avant la copie du scenario "<<i<<" dans la particule "<<p<<"\n";
		//cout << "\nscenario source\n";
		//if (p==0) cout<<"header nconditions="<<this->header.scenario[i].nconditions<<"\n";
		this->particule[p].scenario[i] = this->header.scenario[i];
		//cout<<"this->particule[p].scenario["<<i<<"].nconditions = "<<this->particule[p].scenario[i].nconditions<<"\n";
		//cout<<"apres la copie du scenario "<<i<<" dans la particule "<<p<<"\n";
		//if (p==0) this->header.scenario[i].ecris();
		//if (p==0) cout<<"dans particule[0] scenario["<<i<<"] nconditions="<<this->particule[p].scenario[i].nconditions<<"\n";
	}
	//cout<<"\navant la copie du superscenario\n";
	//this->header.scen.ecris();
	this->particule[p].scen = this->header.scen;
	//cout<<"\napres la copie du superscenario\n";
	//this->particule[p].scen.ecris();
	//cout<<"setscenarios nloc="<<this->particule[p].nloc<<"\n";
	//exit(1);

}

/**
 * Structure ParticleSet : réinitialisation de la particule p
 */
void ParticleSetC::resetparticle (int p) {
	for (int gr=1;gr<=this->particule[p].ngr;gr++) {
		if (this->header.groupe[gr].type==0) {  //MICROSAT
			//cout <<"MICROSAT "<<p<<"\n";
			this->particule[p].grouplist[gr].mutmoy = this->header.groupe[gr].mutmoy;
			this->particule[p].grouplist[gr].Pmoy = this->header.groupe[gr].Pmoy;
			this->particule[p].grouplist[gr].snimoy = this->header.groupe[gr].snimoy;
		}
		else if (this->header.groupe[gr].type==1) {        //SEQUENCES
			//cout<<"SEQUENCE\n";
			this->particule[p].grouplist[gr].musmoy = this->header.groupe[gr].musmoy;       //musmoy
			if (this->header.groupe[gr].mutmod>0){
				this->particule[p].grouplist[gr].k1moy = this->header.groupe[gr].k1moy ;        //k1moy
			}
			if (this->header.groupe[gr].mutmod>2){
				this->particule[p].grouplist[gr].k2moy = this->header.groupe[gr].k2moy ;        //k2moy
			}
		} else if (this->header.groupe[gr].type>9) {		//SNP
			for (int i=0;i<this->particule[p].grouplist[gr].nstatsnp;i++){
				this->particule[p].grouplist[gr].sumstatsnp[i].defined=false;
				this->particule[p].grouplist[gr].sumstatsnp[i].sorted=false;
			}
		}
		//for (int loc=0;loc<this->nloc;loc++) this->particule[p].libere(false);
	}
}

/**
 * Structure ParticleSet : simulation des particules utilisées pour le model checking
 */
void ParticleSetC::dosimulphistar(HeaderC const & header, int npart, bool dnatrue,bool multithread,bool firsttime, int numscen,int seed,int nsel) {
	int scen=numscen-1;
	int ii,ip1,ip2,ipart,gr,nstat,k,nparam=header.scenario[scen].nparam;
	//bool phistarOK;
	this->npart = npart;
	int *sOK;
	sOK = new int[npart];
	if (debuglevel==5) cout<<"avant firsttime="<<firsttime<<"\n";
	if (firsttime) {
		this->particule = new ParticleC[this->npart];
		if (debuglevel==5) cout<<"avant this->header=header   this->npart="<<this->npart<<"\n";
		this->header = header;
		if (debuglevel==5) cout<<"apres this->header=header   this->npart="<<this->npart<<"\n";
		for (int p=0;p<this->npart;p++) {
			if (debuglevel==5) cout <<"avant set particule "<<p<<"\n";
			this->particule[p].dnatrue = dnatrue;
			if (debuglevel==5) cout <<"dnatrue\n";
			this->setdata(p);
			if (debuglevel==5) cout <<"setdata\n";
			this->setgroup(p);
			if (debuglevel==5) cout<<"setgroup\n";
			this->setloci(p);
			if (debuglevel==5) cout<<"setloci\n";
			this->setscenarios(p);
			if (debuglevel==5) cout<< "                    apres set particule\n";
			this->particule[p].mw.randinit(p,seed);
		}
	}
	if (debuglevel==5) cout<<"apres firsttime\n";
	for (int p=0;p<this->npart;p++) {
		k=this->particule[0].mw.rand0(nsel);
		if (debuglevel==5) cout<<"p="<<p<<"    k="<<k<<"\n";
		ii=0;
		for (int i=0;i<nparam;i++) {
			if (not header.scenario[scen].histparam[i].prior.constant) {
				if (debuglevel==5) cout<<"phistar["<<k<<"]["<<ii<<"]="<<phistarOK[k][ii]<<"\n";
				this->particule[p].scenario[scen].histparam[i].value=phistarOK[k][ii];
				this->particule[p].scenario[scen].histparam[i].prior.fixed=true;
				ii++;
			} else this->particule[p].scenario[scen].histparam[i].value=header.scenario[scen].histparam[i].prior.mini;
			if (debuglevel==5) cout<<header.scenario[scen].histparam[i].name<<"="<<this->particule[p].scenario[scen].histparam[i].value<<"   ";
		}
		if (debuglevel==5) cout<<"\n";
		//cout<<"apres la copie des paramètres historiques\n";
		for (gr=1;gr<=header.ngroupes;gr++)
			if (header.groupe[gr].type==0) {  //MICROSAT
				//cout <<"MICROSAT "<<p<<"\n";
				//cout<<"phistar["<<k<<"]["<<ii<<"]="<<phistar[k][ii]<<"    \n";
				if (not header.groupe[gr].priormutmoy.constant) {
					this->particule[p].grouplist[gr].mutmoy = phistarOK[k][ii];ii++;
					this->particule[p].grouplist[gr].priormutmoy.fixed=true;
				}
				//cout<<this->particule[p].grouplist[gr].mutmoy<<"\n";
				//cout<<"phistar["<<k<<"]["<<ii<<"]="<<phistar[k][ii]<<"    ";
				if (not header.groupe[gr].priorPmoy.constant)   {
					this->particule[p].grouplist[gr].Pmoy  =  phistarOK[k][ii];ii++;
					this->particule[p].grouplist[gr].priorPmoy.fixed=true;
				}
				//cout<<this->particule[p].grouplist[gr].Pmoy<<"\n";
				//cout<<"phistar["<<k<<"]["<<ii<<"]="<<phistarOK[k][ii]<<"    ";
				if (not header.groupe[gr].priorsnimoy.constant) {
					this->particule[p].grouplist[gr].snimoy = phistarOK[k][ii];ii++;
					this->particule[p].grouplist[gr].priorsnimoy.fixed=true;
				}
				//cout<<this->particule[p].grouplist[gr].snimoy<<"\n";
			}
			else if (header.groupe[gr].type==1) {       //SEQUENCES
				//cout<<"SEQUENCE\n";
				if (not header.groupe[gr].priormusmoy.constant) {         //musmoy
					this->particule[p].grouplist[gr].musmoy = phistarOK[k][ii];ii++;
					this->particule[p].grouplist[gr].priormusmoy.fixed=true;
				}   
				if (header.groupe[gr].mutmod>0){
					if (not header.groupe[gr].priork1moy.constant){        //k1moy
						this->particule[p].grouplist[gr].k1moy =phistarOK[k][ii];ii++;
						this->particule[p].grouplist[gr].priork1moy.fixed=true;
					}
				}
				if (header.groupe[gr].mutmod>2){
					if (not header.groupe[gr].priork2moy.constant){        //k2moy
						this->particule[p].grouplist[gr].k2moy =phistarOK[k][ii];ii++;
						this->particule[p].grouplist[gr].priork2moy.fixed=true;
					}
				}
			}
		//cout<<"\n";
	}
	cout<<"apres le remplissage de "<<npart<<" particules\n";
	//if (num_threads>0) omp_set_num_threads(num_threads);_
#pragma omp parallel for shared(sOK) private(gr) if(multithread)
	for (ipart=0;ipart<this->npart;ipart++){
		//cout <<"avant dosimulpart de la particule "<<ipart<<"\n";
		sOK[ipart]=this->particule[ipart].dosimulpart(numscen);
		//cout<<sOK[ipart]<<"apres dosimulpart de la particule "<<ipart<<"\n";
		if (sOK[ipart]==0) {
			for(gr=1;gr<=this->particule[ipart].ngr;gr++) {this->particule[ipart].docalstat(gr,this->particule[ipart].weight);}
		}
		 //cout<<"apres docalstat de la particule "<<ipart<<"\n";
	}
	cout<<"apres la simulation de "<<npart<<" particules\n";
///////////////fin du pragma
	for (int ipart=0;ipart<this->npart;ipart++) {
		if (sOK[ipart]==0){
			enreg[ipart].numscen=1;
			if (this->particule[ipart].nscenarios>1) {enreg[ipart].numscen=this->particule[ipart].scen.number;}

			//cout<<"dans particleset\n";
			for (int j=0;j<this->particule[ipart].scen.nparamvar;j++) {
				enreg[ipart].param[j]=this->particule[ipart].scen.paramvar[j];
				//cout<<this->particule[ipart].scen.paramvar[j]<<"  ("<<enreg[ipart].param[j]<<")     ";
			}
			//cout <<"\n";
			nstat=0;
			for(int gr=1;gr<=this->particule[ipart].ngr;gr++){
				for (int st=0;st<this->particule[ipart].grouplist[gr].nstat;st++){
					enreg[ipart].stat[nstat]=this->particule[ipart].grouplist[gr].sumstat[st].val;
					//cout<<this->particule[ipart].grouplist[gr].sumstat[st].val<<" ("<<enreg[ipart].stat[nstat]<<")  ";
					nstat++;
				}
				//cout<<"\n";
			}
			enreg[ipart].message="OK";
		}
		else {
			enreg[ipart].message  = "A gene genealogy failed in scenario ";
			enreg[ipart].message += IntToString(this->particule[ipart].scen.number);
			enreg[ipart].message += ". Check consistency of the scenario over possible historical parameter ranges.";
		}
	}
	cout<<"fin de dosimulphistar\n";
}

/**
 * Structure ParticleSet : tirage des paramètres dans les priors (calcul du biais sans info)
*/
/*void ParticleSetC::drawparamfromprior(HeaderC const & header, int npart, bool dnatrue,bool multithread, int numscen,int seed)
{
	bool simuOK;
	this->npart = npart;
	this->particule = new ParticleC[this->npart];
	this->header = header;
	for (int p=0;p<this->npart;p++) {
		if (debuglevel==5) cout <<"avant set particule "<<p<<"\n";
		this->particule[p].dnatrue = dnatrue;
		//if (debuglevel==5) cout <<"dnatrue\n";
		this->setdata(p);
		if (debuglevel==5) cout <<"setdata\n";
		this->setgroup(p);
		if (debuglevel==5) cout<<"setgroup\n";
		this->setloci(p);
		if (debuglevel==5) cout<<"setloci\n";
		this->setscenarios(p);
		if (debuglevel==5) cout << "                    apres set particule "<<p<<"\n";
		this->particule[p].mw.randinit(p,seed);
	}
#pragma omp parallel for private(gr) if(multithread)
	for (int ipart=0;ipart<this->npart;ipart++) {
		this->particule[ipart].drawscenario(&numscen);
		simuOK = this->setHistParamValue();
		setMutParammoyValue();

		
	}
///fin du pragma
}*/

void ParticleSetC::libere(int npart) {
	for (int p=0;p<npart;p++) {
		if (this->particule[p].data.nmisshap>0) delete [] this->particule[p].data.misshap;
		if (this->particule[p].data.nmisssnp>0) delete [] this->particule[p].data.misssnp; 		
		if (this->particule[p].data.nmissnuc>0) delete [] this->particule[p].data.missnuc;
		delete [] this->particule[p].locuslist;
		delete [] this->particule[p].grouplist[0].loc;
		for (int gr=1;gr<=this->particule[p].ngr;gr++) {
			delete [] this->particule[p].grouplist[gr].sumstat;
			delete [] this->particule[p].grouplist[gr].loc;
		}
		delete [] this->particule[p].grouplist;
		delete [] this->particule[p].scenario;
		delete [] this->particule[p].data.catexist;
	}
	delete [] this->particule;
}


/**
 * Structure ParticleSet : simulation des particules utilisées pour la table de référence, le biais et la confiance
 */
void ParticleSetC::dosimultabref(HeaderC const & header, int npart, bool dnatrue,bool multithread,bool firsttime, int numscen,int seed,int depuis)
{
	int ipart,gr,nstat,pa,ip,iscen,np,ns,ccc;
	bool trouve;
	this->npart = npart;
	int *sOK;
	string *ss;
	if (debuglevel==5)  cout <<"debut de dosimultabref\n";fflush(stdin);
	sOK = new int[npart];
	//if (this->defined) cout <<"particleSet defined\n";
	//else cout<<"particleSet UNdefined\n";
	//this->header = header;
	if (debuglevel==5) cout <<"dosimultabref npart = "<<npart<<"\n";
	if (firsttime) {
		this->particule = new ParticleC[this->npart];
		this->header = header;
		for (int p=0;p<this->npart;p++) {
			if (debuglevel==5) cout <<"avant set particule "<<p<<"\n";
			this->particule[p].dnatrue = dnatrue;
			//if (debuglevel==5) cout <<"dnatrue\n";
			this->setdata(p);
			if (debuglevel==5) cout <<"setdata\n";
			this->setgroup(p);
			if (debuglevel==5) cout<<"setgroup\n";
			this->setloci(p);
			if (debuglevel==5) cout<<"setloci\n";
			this->setscenarios(p);
			if (debuglevel==5) cout << "                    apres set particule "<<p<<"\n";
			this->particule[p].mw.randinit(p,seed);
		}
	}
	else {
		for (int p=0;p<this->npart;p++) {
			this->resetparticle(p);
		}
	}
	if (debuglevel==5) cout << "avant pragma npart = "<<this->npart<<"\n";
	//if (num_threads>0) omp_set_num_threads(num_threads);_
	//ntentes=0;naccept=0;
#pragma omp parallel for shared(sOK) private(gr) if(multithread)
	for (ipart=0;ipart<this->npart;ipart++){
		//cout<<"this->particule["<<ipart<<"].grouplist[1].mutmoy = "<<this->particule[ipart].grouplist[1].mutmoy<<"\n";
		//cout<<"this->particule["<<ipart<<"].grouplist[1].priormutmoy.constant = "<<this->particule[ipart].grouplist[1].priormutmoy.constant<<"\n";
		if (debuglevel==5) cout <<"avant dosimulpart de la particule "<<ipart<<"\n";
		//cout <<"   nloc="<<this->particule[ipart].nloc<<"\n";
		sOK[ipart]=this->particule[ipart].dosimulpart(numscen);
		if (debuglevel==14) cout<<"particule "<<ipart<<"   taux de polymorphisme = "<<this->particule[ipart].locpol<<"   poids = "<<this->particule[ipart].weight<<"\n";
		//ntentes += this->particule[ipart].ntentes;
		//naccept += this->particule[ipart].naccept;
		if (debuglevel==5) cout<<"apres dosimulpart de la particule "<<ipart<<"\n";
		
		if (sOK[ipart]==0) {
			if (debuglevel==5) cout<<"avant docalstat de la particule "<<ipart<<"\n";
			for(gr=1;gr<=this->particule[ipart].ngr;gr++) this->particule[ipart].docalstat(gr,this->particule[ipart].weight);
		}
		if (debuglevel==5) cout<<"apres docalstat de la particule "<<ipart<<"\n";
	}
///fin du pragma
	if (debuglevel==5) cout << "apres pragma\n";
	//cout<<"acceptation ="<<(double)(100*naccept)/(double)ntentes<<"   ("<<naccept<<" sur "<<ntentes<<")\n";
	//exit(1);
	int nph,npm,nn=0;
	ss=splitwords(header.entetehist," ",&nph);
	for (int m=0;m<nph;m++) {
		ccc=ss[m][0];
		if (not isalnum(ccc))  nn++;
	}
	delete []ss;
	nph-=nn;
	//nph--;
	if (debuglevel==5) cout<<header.entetehist<<"\n";
	if (header.entetemut.length()>10) {ss=splitwords(header.entetemut," ",&npm);delete []ss;} else npm=0;
	//cout<<"nph="<<nph<<"   npm="<<npm<<"\n";
	for (int ipart=0;ipart<this->npart;ipart++) {
		enreg[ipart].numscen=1;
		if (this->particule[ipart].nscenarios>1) {enreg[ipart].numscen=this->particule[ipart].scen.number;}
		if (sOK[ipart]==0){
			if (debuglevel==5) cout<<"dans particleset ipart="<<ipart<<"     nparamvar="<<this->particule[ipart].scen.nparamvar<<"\n";
			for (int j=0;j<this->particule[ipart].scen.nparamvar;j++) {
			//for (int j=0;j<nph+npm;j++) {	
				//cout<<"this->particule["<<ipart<<"].scen.paramvar["<<j<<"] = "<<this->particule[ipart].scen.paramvar[j]<<"\n";
				enreg[ipart].param[j]=this->particule[ipart].scen.paramvar[j];
				//cout<<"enreg["<<ipart<<"].param["<<j<<"] = "<<enreg[ipart].param[j]<<"\n";
				if (debuglevel==5) cout<<this->particule[ipart].scen.paramvar[j]<<"  ("<<enreg[ipart].param[j]<<")     ";
			}
			if (debuglevel==5) cout <<"\n";
			nstat=0;
			for(int gr=1;gr<=this->particule[ipart].ngr;gr++){
				/*cout<<"\ngroupe "<<gr<<"\n";
				for (int st=0;st<this->particule[ipart].grouplist[gr].nstat;st++) cout<<this->particule[ipart].grouplist[gr].sumstat[st].val<<"   \n";
				cout<<"\n";*/
				for (int st=0;st<this->particule[ipart].grouplist[gr].nstat;st++){enreg[ipart].stat[nstat]=this->particule[ipart].grouplist[gr].sumstat[st].val;nstat++;/*cout<<nstat<<"\n";*/}
			}
			//for (int st=0;st<nstat;st++) cout<<enreg[ipart].stat[st]<<"   ";cout<<"\n\n";
			enreg[ipart].message="OK";
		}
		else {
			cout<<"A gene genealogy failed in scenario "<<this->particule[ipart].scen.number<<"\n";
			enreg[ipart].message  = "A gene genealogy failed in scenario ";
			enreg[ipart].message += IntToString(this->particule[ipart].scen.number);
			enreg[ipart].message += ". Check consistency of the scenario over possible historical parameter ranges.";
		}
	}
	if (debuglevel==5) cout<<"apres remplissage des enreg\n";fflush(stdin);
	if (firsttime){
		if (debuglevel==5) cout<<"FIRSTTIME\n";
		FILE * pFile;
		int categ,iq,icur=0;
		bool trouve2;
		if (depuis==0){
			scurfile = this->header.pathbase + "first_records_of_the_reference_table_"+ IntToString(icur) +".txt";
			while (ifstream(scurfile.c_str())) {
				icur++;
				//scurfile=this->header.pathbase+"courant_"+IntToString(icur)+".log";
				scurfile = this->header.pathbase + "first_records_of_the_reference_table_"+ IntToString(icur) +".txt";
				}
		}
		cout<<"ecriture du fichier courant = "<<scurfile<<"\n";
		pFile = fopen (scurfile.c_str(),"w");
		fprintf(pFile,"%s\n",this->header.entete.c_str());
		cout<<"FISTTIME\n";
		cout<<header.entete<<"\n\n";
		cout<<header.entetehist<<"\n\n";
		cout<<header.entetemut<<"\n\n";
		/*int nph,npm;
		ss=splitwords(header.entetehist," ",&nph);
		if (debuglevel==5) cout<<header.entetehist<<"\n";
		if (header.entetemut.length()>10) ss=splitwords(header.entetemut," ",&npm); else npm=0;*/
		//if (ss!=NULL) {delete []ss;ss=NULL;}
		ss=splitwords(header.entete," ",&ns);
		cout<<"nph="<<nph<<"   npm="<<npm<<"   ns="<<ns<<"\n";
		np=ns-header.nstat-1;
		//cout<<"ns="<<ns<<"  nparam="<<np<<"   nparamut="<<rt.nparamut<<"   nstat="<<header.nstat<<"\n";
		//cout<<"nph="<<nph<<"    npm="<<npm<<"\n";
		//cout<<"npart="<<this->npart<<"\n";
		for (int ipart=0;ipart<this->npart;ipart++) {
			//cout<<"ipart="<<ipart<<"    sOK[ipart]="<<sOK[ipart]<<"\n";
			if (sOK[ipart]==0){
				//cout<<enreg[ipart].numscen<<"\n";
				fprintf(pFile,"%3d  ",enreg[ipart].numscen);
				iscen=enreg[ipart].numscen-1;
				//cout<<"scenario "<<enreg[ipart].numscen<<"\n";
				if (debuglevel==5) cout<<header.nparamtot<<"   "<<rt.nhistparam[iscen]<<"   "<<np<<"\n";
				pa=0;
				for (int j=1;j<nph;j++) {
					trouve=false;ip=-1;
					while ((not trouve)and(ip<rt.nhistparam[iscen]-1)) {
						ip++;
						trouve=(ss[j] == rt.histparam[iscen][ip].name);
						//cout<<"->"<<ss[j]<<"<-   ?   ->"<< header.scenario[iscen].histparam[ip].name<<"<-"<<trouve<<"\n";
					}
					if (trouve) {
						trouve2=false;iq=-1;
						while ((not trouve2)and(iq<header.nparamtot-1)) {
							iq++;
							trouve2=(ss[j] == header.histparam[iq].name);
						}
						if (trouve2) categ=header.histparam[iq].category; else categ=0;
						if (categ<2) fprintf(pFile,"  %12.0f",enreg[ipart].param[ip]);
						else fprintf(pFile,"  %12.3f",enreg[ipart].param[ip]);
						pa++;
					}
					else fprintf(pFile,"              ");
				}
				if (debuglevel==5) cout<<"pa="<<pa<<"   rt.nparam[iscen]="<<rt.nparam[iscen]<<"\n";
				for (int j=0;j<npm;j++) fprintf(pFile,"  %12.3e",enreg[ipart].param[pa+j]);
				for (int st=0;st<header.nstat;st++) fprintf(pFile,"  %12.6f",enreg[ipart].stat[st]);
				fprintf(pFile,"\n");
				if (debuglevel==5){
					cout<<"parametres de la particule"<<ipart<<"\n";
					for (int j=0;j<npm+pa;j++) cout<<enreg[ipart].param[j]<<"   ";
					cout<<"\n";
					cout<<"\nstat de la particule "<<ipart<<"\n";
					for (int st=0;st<header.nstat;st++) cout<<enreg[ipart].stat[st]<<"   ";
					cout<<"\n";
				}
			}
		}
		delete [] ss;
		fclose(pFile);
		if (debuglevel==5) cout<<"apres fclose\n";
	}
	//cout<<"apres remise en ordre des enreg.param\n";
	if (debuglevel==5) cout<<"avant delete sOK\n";
	delete [] sOK;
	if (debuglevel==5) cout<<"apres delete sOK\n";
	//exit(1);
}

/**
 * Structure ParticleSet : simulation des particules utilisées pour la création de fichiers genepop
 */
string* ParticleSetC::simulgenepop(HeaderC const & header, int npart, bool multithread, int seed) {
	bool dnatrue=true;
	int numscen=1;
	this->npart=npart;
	int *sOK;
	string *ss;
	sOK = new int[npart];
	this->particule = new ParticleC[this->npart];
	ss = new string[npart];
	this->header = header;
	cout<<"avant le remplissage des "<<this->npart<<" particules\n";
	for (int p=0;p<this->npart;p++) {
		this->particule[p].dnatrue = dnatrue;
		this->setdata(p);//cout<<"apres setdata\n";
		this->setgroup(p);//cout<<"apres setgroup\n";
		this->setloci(p);//cout<<"apres setloci\n";
		this->setscenarios(p);//cout<<"apres setscenario\n";
		this->particule[p].mw.randinit(p,seed);
	}
	cout<<"avant omp\n";
#pragma omp parallel for shared(sOK) if(multithread)
	for (int ipart=0;ipart<this->npart;ipart++){
		sOK[ipart] = this->particule[ipart].dosimulpart(numscen);
		//cout<<"sOK["<<ipart<<"]="<<sOK[ipart]<<"\n";
		if (sOK[ipart]==0) ss[ipart] = this->particule[ipart].dogenepop();
		else ss[ipart] = "";
		//cout<<ss[ipart]<<"\n";
	}
	return ss;
}

/**
 * Structure ParticleSet : simulation des particules utilisées pour la création de fichiers SNP
 */
string* ParticleSetC::simuldataSNP(HeaderC const & header, int npart, bool multithread, int seed) {
	bool dnatrue=true;
	int numscen=1;
	this->npart=npart;
	int *sOK;
	string *ss;
	sOK = new int[npart];
	this->particule = new ParticleC[this->npart];
	ss = new string[npart];
	this->header = header;
	cout<<"avant le remplissage des "<<this->npart<<" particules\n";
	for (int p=0;p<this->npart;p++) {
		//this->particule[p].dnatrue = dnatrue;
		this->setdata(p);cout<<"apres setdata\n";
		// this->setgroup(p);cout<<"apres setgroup\n"; // Je supprime cette ligne car pas de groupe de locus dans les parties simul de jeux de donnees. Pierre
		this->setloci(p);cout<<"apres setloci\n";
		this->setscenarios(p);cout<<"apres setscenario\n";
		this->particule[p].mw.randinit(p,seed);
	}
	cout<<"avant omp\n";
#pragma omp parallel for shared(sOK) if(multithread)
	for (int ipart=0;ipart<this->npart;ipart++){
		sOK[ipart] = this->particule[ipart].dosimulpart(numscen);
		cout<<"sOK["<<ipart<<"]="<<sOK[ipart]<<"\n";
		if (sOK[ipart]==0) ss[ipart] = this->particule[ipart].dodataSNP();
		else ss[ipart] = "";
		//cout<<ss[ipart]<<"\n";
	}
	return ss;
}

