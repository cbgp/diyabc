

//using namespace boost::python;
//using namespace std;
//#include <vector>

//#ifdef _OPENMP
#ifndef OMPH
#include <omp.h>
#define OMPH
#endif
//#endif

#ifndef HEADER
#include "header.cpp"
#define HEADER
#endif

#ifndef REFTABLE
#include "reftable.cpp"
#define REFTABLE
#endif

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
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*struct enregC {
    int numscen;
    float *param,*stat;
    double dist;
    string message;
};*/

enregC *enreg;
extern long double **phistar;
extern int debuglevel;
extern ReftableC rt;

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
        MwcGen *mw;

	void setdata(int p) {
		this->particule[p].data.nsample = this->header.dataobs.nsample;
		this->particule[p].data.nind = new int[this->header.dataobs.nsample];
		this->particule[p].data.indivsexe = new int*[this->header.dataobs.nsample];
		for (int i=0;i<this->header.dataobs.nsample;i++) {
			this->particule[p].data.nind[i] = this->header.dataobs.nind[i];
			this->particule[p].data.indivsexe[i] = new int[this->header.dataobs.nind[i]];
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
                this->particule[p].data.nmissnuc = this->header.dataobs.nmissnuc;
                if (this->particule[p].data.nmisshap>0) {
                          this->particule[p].data.missnuc = new MissingNuc[this->particule[p].data.nmissnuc];
                          for (int i=0;i<this->header.dataobs.nmissnuc;i++) {
                                  this->particule[p].data.missnuc[i].locus  = this->header.dataobs.missnuc[i].locus;
                                  this->particule[p].data.missnuc[i].sample = this->header.dataobs.missnuc[i].sample;
                                  this->particule[p].data.missnuc[i].indiv  = this->header.dataobs.missnuc[i].indiv;
                                  this->particule[p].data.missnuc[i].nuc    = this->header.dataobs.missnuc[i].nuc;
                          }
                }
	}

void setgroup(int p) {
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
				if (this->header.groupe[gr].mutmoy<0.0) this->particule[p].grouplist[gr].priormutmoy = copyprior(this->header.groupe[gr].priormutmoy);
				else this->particule[p].grouplist[gr].priormutmoy.constant=true;
				this->particule[p].grouplist[gr].priormutmoy.fixed=this->header.groupe[gr].priormutmoy.fixed;
				/*if ((this->particule[p].grouplist[gr].priormutmoy.constant)and(this->header.groupe[gr].priormutmoy.fixed)) {
                    this->particule[p].grouplist[gr].priormutmoy.constant=false;
				}*/
				this->particule[p].grouplist[gr].priormutloc = copyprior(this->header.groupe[gr].priormutloc);

				this->particule[p].grouplist[gr].Pmoy = this->header.groupe[gr].Pmoy;
				if (this->header.groupe[gr].Pmoy<0.0) this->particule[p].grouplist[gr].priorPmoy = copyprior(this->header.groupe[gr].priorPmoy);
				else this->particule[p].grouplist[gr].priorPmoy.constant=true;
				this->particule[p].grouplist[gr].priorPmoy.fixed=this->header.groupe[gr].priorPmoy.fixed;
				/*if ((this->particule[p].grouplist[gr].priorPmoy.constant)and(this->header.groupe[gr].priorPmoy.fixed)) {
                    this->particule[p].grouplist[gr].priorPmoy.constant=false;
				}*/
				this->particule[p].grouplist[gr].priorPloc = copyprior(this->header.groupe[gr].priorPloc);

				this->particule[p].grouplist[gr].snimoy = this->header.groupe[gr].snimoy;
				if (this->header.groupe[gr].snimoy<0.0) this->particule[p].grouplist[gr].priorsnimoy = copyprior(this->header.groupe[gr].priorsnimoy);
				else this->particule[p].grouplist[gr].priorsnimoy.constant=true;
				this->particule[p].grouplist[gr].priorsnimoy.fixed=this->header.groupe[gr].priorsnimoy.fixed;
				/*if ((this->particule[p].grouplist[gr].priorsnimoy.constant)and(this->header.groupe[gr].priorsnimoy.fixed)) {
                     this->particule[p].grouplist[gr].priorsnimoy.constant=false;
				}*/
				this->particule[p].grouplist[gr].priorsniloc = copyprior(this->header.groupe[gr].priorsniloc);
			}
			else {							//SEQUENCES
			        //cout<<"SEQUENCE\n";
				this->particule[p].grouplist[gr].mutmod = this->header.groupe[gr].mutmod;	//mutmod
				this->particule[p].grouplist[gr].p_fixe = this->header.groupe[gr].p_fixe;	//p_fixe
				this->particule[p].grouplist[gr].gams   = this->header.groupe[gr].gams;	//gams

				this->particule[p].grouplist[gr].musmoy = this->header.groupe[gr].musmoy;	//musmoy
				if (this->header.groupe[gr].musmoy<0.0) this->particule[p].grouplist[gr].priormusmoy = copyprior(this->header.groupe[gr].priormusmoy);
				else this->particule[p].grouplist[gr].priormusmoy.constant=true;
				this->particule[p].grouplist[gr].priormusmoy.fixed=this->header.groupe[gr].priormusmoy.fixed;
				/*if ((this->particule[p].grouplist[gr].priormusmoy.constant)and(this->header.groupe[gr].priormusmoy.fixed)) {
                    this->particule[p].grouplist[gr].priormusmoy.constant=false;
				}*/
				this->particule[p].grouplist[gr].priormusloc = copyprior(this->header.groupe[gr].priormusloc);

				if (this->header.groupe[gr].mutmod>0){
					this->particule[p].grouplist[gr].k1moy = this->header.groupe[gr].k1moy ;	//k1moy
					if (header.groupe[gr].k1moy<0) this->particule[p].grouplist[gr].priork1moy = copyprior(this->header.groupe[gr].priork1moy);
					else this->particule[p].grouplist[gr].priork1moy.constant=true;
                    this->particule[p].grouplist[gr].priork1moy.fixed=this->header.groupe[gr].priork1moy.fixed;
                    /*if ((this->particule[p].grouplist[gr].priork1moy.constant)and(this->header.groupe[gr].priork1moy.fixed)) {
                        this->particule[p].grouplist[gr].priork1moy.constant=false;
                    }*/
					this->particule[p].grouplist[gr].priork1loc = copyprior(this->header.groupe[gr].priork1loc);
				}
				if (this->header.groupe[gr].mutmod>2){
					this->particule[p].grouplist[gr].k2moy = this->header.groupe[gr].k2moy ;	//k2moy
					if (header.groupe[gr].k2moy<0) this->particule[p].grouplist[gr].priork2moy = copyprior(this->header.groupe[gr].priork2moy);
					else this->particule[p].grouplist[gr].priork2moy.constant=true;
                    this->particule[p].grouplist[gr].priork2moy.fixed=this->header.groupe[gr].priork2moy.fixed;
                    /*if ((this->particule[p].grouplist[gr].priork2moy.constant)and(this->header.groupe[gr].priork2moy.fixed)) {
                        this->particule[p].grouplist[gr].priork2moy.constant=false;
                    }*/
					this->particule[p].grouplist[gr].priork2loc = copyprior(this->header.groupe[gr].priork2loc);
				}
			}
			this->particule[p].grouplist[gr].nstat=this->header.groupe[gr].nstat;
			this->particule[p].grouplist[gr].sumstat = new StatC[header.groupe[gr].nstat];
			for (int i=0;i<this->header.groupe[gr].nstat;i++){
				this->particule[p].grouplist[gr].sumstat[i].cat   = header.groupe[gr].sumstat[i].cat;
				this->particule[p].grouplist[gr].sumstat[i].samp  = header.groupe[gr].sumstat[i].samp;
				this->particule[p].grouplist[gr].sumstat[i].samp1 = header.groupe[gr].sumstat[i].samp1;
				this->particule[p].grouplist[gr].sumstat[i].samp2 = header.groupe[gr].sumstat[i].samp2;
				if (this->particule[p].grouplist[gr].sumstat[i].cat>20) {
					this->particule[p].grouplist[gr].sumstat[i].vals = new long double[this->particule[p].grouplist[gr].nloc];
					for (int loc=0;loc<this->particule[p].grouplist[gr].nloc;loc++) 
						this->particule[p].grouplist[gr].sumstat[i].vals[loc] = header.particuleobs.grouplist[gr].sumstat[i].vals[loc];
				}
			}
		}
	}

	void setloci(int p) {
	        int kmoy;
		this->particule[p].nloc = this->header.dataobs.nloc;
		this->particule[p].locuslist = new LocusC[this->header.dataobs.nloc];
		for (int kloc=0;kloc<this->header.dataobs.nloc;kloc++){
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
				}
			else {
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
				}
			this->particule[p].nsample = this->header.dataobs.nsample;  
			this->particule[p].locuslist[kloc].coeff =  this->header.dataobs.locus[kloc].coeff;  
			this->particule[p].locuslist[kloc].ss = new int[ this->header.dataobs.nsample];      
			for (int sa=0;sa<this->particule[p].nsample;sa++) this->particule[p].locuslist[kloc].ss[sa] =  this->header.dataobs.locus[kloc].ss[sa];
			this->particule[p].locuslist[kloc].samplesize = new int[ this->header.dataobs.nsample];
			for (int sa=0;sa<this->particule[p].nsample;sa++) this->particule[p].locuslist[kloc].samplesize[sa] =  this->header.dataobs.locus[kloc].samplesize[sa];
			//cout << "samplesize[0]="<<this->particule[p].locuslist[kloc].samplesize[0]<<"\n";
		}
	}

	void setscenarios (int p) {
		this->particule[p].nscenarios=this->header.nscenarios;
		this->particule[p].drawuntil = this->header.drawuntil;
		this->particule[p].scenario = new ScenarioC[this->header.nscenarios];
		for (int i=0;i<this->header.nscenarios;i++) {
            //cout<<"avant la copie du scenario "<<i<<" dans la particule "<<p<<"\n";
            //cout << "\nscenario source\n";
            //if (p==0) cout<<"header nconditions="<<this->header.scenario[i].nconditions<<"\n";
		    this->particule[p].scenario[i] = copyscenario(this->header.scenario[i]);
            //cout<<"apres la copie du scenario "<<i<<" dans la particule "<<p<<"\n";
		    //if (p==0) this->particule[p].scenario[i].ecris();
            //if (p==0) cout<<"dans particule[0] scenario["<<i<<"] nconditions="<<this->particule[p].scenario[i].nconditions<<"\n";
		}
        //cout<<"\navant la copie du superscenario\n";
        //this->header.scen.ecris();
		this->particule[p].scen=copyscenario(this->header.scen);
        //cout<<"\napres la copie du superscenario\n";
        //this->particule[p].scen.ecris();
        //cout<<"setscenarios nloc="<<this->particule[p].nloc<<"\n";
        //exit(1);

	}

	void resetparticle (int p) {
                for (int gr=1;gr<=this->particule[p].ngr;gr++) {
                        if (this->header.groupe[gr].type==0) {  //MICROSAT
                                //cout <<"MICROSAT "<<p<<"\n";
                                this->particule[p].grouplist[gr].mutmoy = this->header.groupe[gr].mutmoy;
                                this->particule[p].grouplist[gr].Pmoy = this->header.groupe[gr].Pmoy;
                                this->particule[p].grouplist[gr].snimoy = this->header.groupe[gr].snimoy;
                        }
                        else {                                                  //SEQUENCES
                                //cout<<"SEQUENCE\n";
                                this->particule[p].grouplist[gr].musmoy = this->header.groupe[gr].musmoy;       //musmoy
                                if (this->header.groupe[gr].mutmod>0){
                                        this->particule[p].grouplist[gr].k1moy = this->header.groupe[gr].k1moy ;        //k1moy
                                }
                                if (this->header.groupe[gr].mutmod>2){
                                        this->particule[p].grouplist[gr].k2moy = this->header.groupe[gr].k2moy ;        //k2moy
                                }
                        }
                 //for (int loc=0;loc<this->nloc;loc++) this->particule[p].libere(false);

                }
        }

    void dosimulphistar(HeaderC header, int npart, bool dnatrue,bool multithread,bool firsttime, int numscen,int seed,int nsel) {
        int scen=numscen-1;
        int ii,ip1,ip2,ipart,gr,nstat,k,nparam=header.scenario[scen].nparam;
        bool phistarOK;
        this->npart = npart;
        int *sOK;
        sOK = new int[npart];
        if (debuglevel==5) cout<<"avant firsttime\n";
        if (firsttime) {
            this->particule = new ParticleC[this->npart];
            this->header = header;
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
            do {
                phistarOK=true;
                k=this->particule[0].mw.rand0(nsel);
                if (header.scenario[scen].nconditions>0) {
                    for (int j=0;j<header.scenario[scen].nconditions;j++) {
                        ip1=0;while (header.scenario[scen].condition [j].param1!=header.scenario[scen].histparam[ip1].name) ip1++;
                        ip2=0;while (header.scenario[scen].condition [j].param2!=header.scenario[scen].histparam[ip2].name) ip2++;
                        if (header.scenario[scen].condition[j].operateur==">")       phistarOK=(phistar[k][ip1] >  phistar[k][ip2]);
                        else if (header.scenario[scen].condition[j].operateur=="<")  phistarOK=(phistar[k][ip1] <  phistar[k][ip2]);
                        else if (header.scenario[scen].condition[j].operateur==">=") phistarOK=(phistar[k][ip1] >= phistar[k][ip2]);
                        else if (header.scenario[scen].condition[j].operateur=="<=") phistarOK=(phistar[k][ip1] <= phistar[k][ip2]);
                        if (not phistarOK) break;
                    }
                }
            } while(not phistarOK);
            ii=0;
            for (int i=0;i<nparam;i++) {
                if (not header.scenario[scen].histparam[i].prior.constant) {
                  if (debuglevel==5) cout<<"phistar["<<k<<"]["<<ii<<"]="<<phistar[k][ii]<<"\n";
                    this->particule[p].scenario[scen].histparam[i].value=phistar[k][ii];
                    ii++;
                } else this->particule[p].scenario[scen].histparam[i].value=header.scenario[scen].histparam[i].prior.mini;
				if (p==0) cout<<header.scenario[scen].histparam[i].name<<"="<<this->particule[p].scenario[scen].histparam[i].value<<"\n";
            }
            //cout<<"apres la copie des paramètres historiques\n";
            for (gr=1;gr<=header.ngroupes;gr++)
                if (header.groupe[gr].type==0) {  //MICROSAT
                        //cout <<"MICROSAT "<<p<<"\n";
                        //cout<<"phistar["<<k<<"]["<<ii<<"]="<<phistar[k][ii]<<"    \n";
                        if (not header.groupe[gr].priormutmoy.constant) {this->particule[p].grouplist[gr].mutmoy = phistar[k][ii];ii++;}
                        //cout<<this->particule[p].grouplist[gr].mutmoy<<"\n";
                        //cout<<"phistar["<<k<<"]["<<ii<<"]="<<phistar[k][ii]<<"    ";
                        if (not header.groupe[gr].priorPmoy.constant)   {this->particule[p].grouplist[gr].Pmoy  =  phistar[k][ii];ii++;}
                        //cout<<this->particule[p].grouplist[gr].Pmoy<<"\n";
                        //cout<<"phistar["<<k<<"]["<<ii<<"]="<<phistarOK[k][ii]<<"    ";
                        if (not header.groupe[gr].priorsnimoy.constant) {this->particule[p].grouplist[gr].snimoy = phistar[k][ii];ii++;}
                        //cout<<this->particule[p].grouplist[gr].snimoy<<"\n";
                }
                else {                                                  //SEQUENCES
                        //cout<<"SEQUENCE\n";
                        if (not header.groupe[gr].priormutmoy.constant) {this->particule[p].grouplist[gr].musmoy = phistar[k][ii];ii++;}   //musmoy
                        if (header.groupe[gr].mutmod>0){
                                if (not header.groupe[gr].priormutmoy.constant){this->particule[p].grouplist[gr].k1moy =phistar[k][ii];ii++;}        //k1moy
                        }
                        if (header.groupe[gr].mutmod>2){
                                if (not header.groupe[gr].priormutmoy.constant){this->particule[p].grouplist[gr].k2moy =phistar[k][ii];ii++;}        //k2moy
                        }
                }
                //cout<<"\n";
        }
        cout<<"apres le remplissage des particules\n";
        //if (num_threads>0) omp_set_num_threads(num_threads);_
    #pragma omp parallel for shared(sOK) private(gr) if(multithread)
        for (ipart=0;ipart<this->npart;ipart++){
                //if (trace) cout <<"avant dosimulpart de la particule "<<ipart<<"\n";
            sOK[ipart]=this->particule[ipart].dosimulpart(numscen);
            if (sOK[ipart]==0) {
                for(gr=1;gr<=this->particule[ipart].ngr;gr++) {this->particule[ipart].docalstat(gr);}
            }
    //if (trace) cout<<"apres docalstat de la particule "<<ipart<<"\n";
        }
//fin du pragma
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
                        //cout<<this->particule[ipart].grouplist[gr].sumstat[st].val<<"  ";
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


    }

	void dosimultabref(HeaderC header, int npart, bool dnatrue,bool multithread,bool firsttime, int numscen,int seed)
	{
               int ipart,gr,nstat,pa,ip,iscen,np,ns;
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
	       #pragma omp parallel for shared(sOK) private(gr) if(multithread)
                for (ipart=0;ipart<this->npart;ipart++){
                        if (debuglevel==5) cout <<"avant dosimulpart de la particule "<<ipart<<"\n";
                         //cout <<"   nloc="<<this->particule[ipart].nloc<<"\n";
			        sOK[ipart]=this->particule[ipart].dosimulpart(numscen);
                        if (debuglevel==5) cout<<"apres dosimulpart de la particule "<<ipart<<"\n";
			        if (sOK[ipart]==0) {
			 	        for(gr=1;gr<=this->particule[ipart].ngr;gr++) this->particule[ipart].docalstat(gr);
			        }
			        if (debuglevel==5) cout<<"apres docalstat de la particule "<<ipart<<"\n";
		        }
//fin du pragma
                if (debuglevel==5) cout << "apres pragma\n";
                //exit(1);
                for (int ipart=0;ipart<this->npart;ipart++) {
			if (sOK[ipart]==0){
				enreg[ipart].numscen=1;
                if (this->particule[ipart].nscenarios>1) {enreg[ipart].numscen=this->particule[ipart].scen.number;}

                cout<<"dans particleset nparamvar="<<this->particule[ipart].scen.nparamvar<<"\n";
				for (int j=0;j<this->particule[ipart].scen.nparamvar;j++) {
                    enreg[ipart].param[j]=this->particule[ipart].scen.paramvar[j];
                    cout<<this->particule[ipart].scen.paramvar[j]<<"  ("<<enreg[ipart].param[j]<<")     ";
                }
                cout <<"\n";
                nstat=0;
				for(int gr=1;gr<=this->particule[ipart].ngr;gr++){
					for (int st=0;st<this->particule[ipart].grouplist[gr].nstat;st++){enreg[ipart].stat[nstat]=this->particule[ipart].grouplist[gr].sumstat[st].val;nstat++;}
				}
				enreg[ipart].message="OK";
			}
			else {
                                enreg[ipart].message  = "A gene genealogy failed in scenario ";
                                enreg[ipart].message += IntToString(this->particule[ipart].scen.number);
                                enreg[ipart].message += ". Check consistency of the scenario over possible historical parameter ranges.";
                        }
		}
		if (debuglevel==5) cout<<"apres remplissage des enreg\n";fflush(stdin);
        if (firsttime){
              if (debuglevel==5) cout<<"FIRSTTIME\n";
              FILE * pFile;
              char  *curfile;
              int categ,iq;
              bool trouve2;
              curfile = new char[strlen(this->header.pathbase)+13];
              strcpy(curfile,this->header.pathbase);
              strcat(curfile,"courant.log");
              if (debuglevel==5) cout<<curfile<<"\n";
              pFile = fopen (curfile,"w");
              fprintf(pFile,"%s\n",this->header.entete.c_str());
			  //cout<<"FISTTIME\n";
			  //cout<<header.entete<<"\n";
			  int nph,npm;
			  ss=splitwords(header.entetehist," ",&nph);
			  ss=splitwords(header.entetemut," ",&npm);
              ss=splitwords(header.entete," ",&ns);
              np=ns-header.nstat-1;
              //cout<<"ns="<<ns<<"  nparam="<<np<<"   nparamut="<<rt.nparamut<<"   nstat="<<header.nstat<<"\n";
			  //cout<<"nph="<<nph<<"    npm="<<npm<<"\n";
              for (int ipart=0;ipart<this->npart;ipart++) {
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
						  cout<<"\nstat de la particule "<<ipart<<"\n";
						  for (int st=0;st<header.nstat;st++) cout<<enreg[ipart].stat[st]<<"   ";
						  cout<<"\n";
					  }
                  }
              }
              fclose(pFile);
              if (debuglevel==5) cout<<"apres fclose\n";
        }
        //cout<<"apres remise en ordre des enreg.param\n";
        if (debuglevel==5) cout<<"avant delete sOK\n";
        delete [] sOK;
        if (debuglevel==5) cout<<"apres delete sOK\n";
	}

	string* simulgenepop(HeaderC header, int npart, bool multithread, int seed) {
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
};
