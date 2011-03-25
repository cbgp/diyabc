

//using namespace boost::python;
//using namespace std;
//#include <vector>

#ifdef _OPENMP
#ifndef OMPH
#include <omp.h>
#define OMPH
#endif
#endif

#ifndef HEADER
#include "header.cpp"
#define HEADER
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

struct enregC {
    int numscen;
    float *param,*stat;
    double dist;
    string message;
};

enregC *enreg;


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
				this->particule[p].grouplist[gr].priormutloc = copyprior(this->header.groupe[gr].priormutloc);
				this->particule[p].grouplist[gr].Pmoy = this->header.groupe[gr].Pmoy;
				if (this->header.groupe[gr].Pmoy<0.0) this->particule[p].grouplist[gr].priorPmoy = copyprior(this->header.groupe[gr].priorPmoy);
				else this->particule[p].grouplist[gr].priorPmoy.constant=true;
				this->particule[p].grouplist[gr].priorPloc = copyprior(this->header.groupe[gr].priorPloc);
				this->particule[p].grouplist[gr].snimoy = this->header.groupe[gr].snimoy;
				if (this->header.groupe[gr].snimoy<0.0) this->particule[p].grouplist[gr].priorsnimoy = copyprior(this->header.groupe[gr].priorsnimoy);
				else this->particule[p].grouplist[gr].priorsnimoy.constant=true;
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
				this->particule[p].grouplist[gr].priormusloc = copyprior(this->header.groupe[gr].priormusloc);
				if (this->header.groupe[gr].mutmod>0){
					this->particule[p].grouplist[gr].k1moy = this->header.groupe[gr].k1moy ;	//k1moy
					if (header.groupe[gr].k1moy<0) this->particule[p].grouplist[gr].priork1moy = copyprior(this->header.groupe[gr].priork1moy);
					else this->particule[p].grouplist[gr].priork1moy.constant=true;
					this->particule[p].grouplist[gr].priork1loc = copyprior(this->header.groupe[gr].priork1loc);
				}
				if (this->header.groupe[gr].mutmod>2){
					this->particule[p].grouplist[gr].k2moy = this->header.groupe[gr].k2moy ;	//k2moy
					if (header.groupe[gr].k2moy<0) this->particule[p].grouplist[gr].priork2moy = copyprior(this->header.groupe[gr].priork2moy);
					else this->particule[p].grouplist[gr].priork2moy.constant=true;
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

			}
		}
	}

	void setloci(int p) {
	        int kmoy;
		this->particule[p].nloc = this->header.dataobs.nloc;
		this->particule[p].locuslist = new LocusC[this->header.dataobs.nloc];
		//cout<<"avant la boucle\n";
		for (int kloc=0;kloc<this->header.dataobs.nloc;kloc++){
			this->particule[p].locuslist[kloc].type = this->header.dataobs.locus[kloc].type;
			this->particule[p].locuslist[kloc].groupe = this->header.dataobs.locus[kloc].groupe;
			//cout<<"locus "<<kloc<<"   groupe "<<this->particule[p].locuslist[kloc].groupe<<"\n";
			if (this->header.dataobs.locus[kloc].type < 5) {
			      	kmoy=(this->header.dataobs.locus[kloc].maxi+this->header.dataobs.locus[kloc].mini)/2;
				this->particule[p].locuslist[kloc].kmin=kmoy-((this->header.dataobs.locus[kloc].motif_range/2)-1)*this->header.dataobs.locus[kloc].motif_size;
				this->particule[p].locuslist[kloc].kmax=this->particule[p].locuslist[kloc].kmin+(this->header.dataobs.locus[kloc].motif_range-1)*this->header.dataobs.locus[kloc].motif_size;
				//cout<<"kmin="<<this->particule[p].locuslist[kloc].kmin<<"     kmax="<<this->particule[p].locuslist[kloc].kmax<<"\n";
				this->particule[p].locuslist[kloc].motif_size = this->header.dataobs.locus[kloc].motif_size;
				this->particule[p].locuslist[kloc].motif_range = this->header.dataobs.locus[kloc].motif_range;
				//this->particule[p].locuslist[kloc].mut_rate = this->header.dataobs.locus[kloc].mut_rate;
				//this->particule[p].locuslist[kloc].Pgeom = this->header.dataobs.locus[kloc].Pgeom;
				//this->particule[p].locuslist[kloc].sni_rate = this->header.dataobs.locus[kloc].sni_rate;
				}
			else {
				this->particule[p].locuslist[kloc].dnalength =  this->header.dataobs.locus[kloc].dnalength;
				//this->particule[p].locuslist[kloc].mus_rate =  this->header.dataobs.locus[kloc].mus_rate;
				//this->particule[p].locuslist[kloc].k1 =  this->header.dataobs.locus[kloc].k1;
				//this->particule[p].locuslist[kloc].k2 =  this->header.dataobs.locus[kloc].k2;
				this->particule[p].locuslist[kloc].pi_A = this->header.dataobs.locus[kloc].pi_A ;
				this->particule[p].locuslist[kloc].pi_C =  this->header.dataobs.locus[kloc].pi_C;
				this->particule[p].locuslist[kloc].pi_G =  this->header.dataobs.locus[kloc].pi_G;
				this->particule[p].locuslist[kloc].pi_T =  this->header.dataobs.locus[kloc].pi_T;
				this->particule[p].locuslist[kloc].mutsit = new double[ this->header.dataobs.locus[kloc].dnalength];
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
                    //this->header.scenario[i].ecris();
		    this->particule[p].scenario[i] = copyscenario(this->header.scenario[i]);
		    //this->particule[p].scenario[i].ecris();
		}
		this->particule[p].scen=copyscenario(this->header.scen);
                //cout<<"\n\n";
                //this->particule[p].scen.ecris();
		
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
                         
                
                }
        }

/*	void cleanParticle(int ipart){
//nettoyage de locuslist
//cout <<"debut du cleanParticle\n";
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
                //cout<<"apres cleanlocuslist\n";
//nettoyage de grouplist		
		for (int gr=0;gr<this->particule[ipart].ngr;gr++) {
			delete []this->particule[ipart].grouplist[gr].loc;
			delete []this->particule[ipart].grouplist[gr].sumstat;
		}
		delete []this->particule[ipart].grouplist;
                //cout<<"apres cleangroup\n";
//nettoyage de data
		this->particule[ipart].data.libere();
                //cout<<"apres cleandata\n";
//nettoyage de scenario
		for (int i=0;i<this->particule[ipart].nscenarios;i++) {
			delete []this->particule[ipart].scenario[i].paramvar;
			delete []this->particule[ipart].scenario[i].time_sample;
			delete []this->particule[ipart].scenario[i].event;
			delete []this->particule[ipart].scenario[i].ne0;
			delete []this->particule[ipart].scenario[i].histparam;
		}
                        delete []this->particule[ipart].scen.paramvar;
                        delete []this->particule[ipart].scen.time_sample;
                        delete []this->particule[ipart].scen.event;
                        delete []this->particule[ipart].scen.ne0;
                        delete []this->particule[ipart].scen.histparam;
		
		delete []this->particule[ipart].scenario;
                //cout<<"apres cleanscenario\n";
//nettoyage de matQ
		/*for (int i=0;i<4;i++) delete []this->particule[ipart].matQ[i];
		cout<<"apres clean matQ[i]\n";
		delete []this->particule[ipart].matQ;
                cout<<"apres clean matQ\n";
	}*/
                
              
	
	/*void cleanParticleSet(){
		for (int i=0;i<this->npart;i++) {
                        delete []this->particule[i].scen.paramvar;
                        delete []this->particule[i].scen.time_sample;
                        delete []this->particule[i].scen.event;
                        delete []this->particule[i].scen.ne0;
                        delete []this->particule[i].scen.histparam;
                        if (this->particule[i].scen.nconditions>0) delete []this->particule[i].scen.condition;
                }
	}*/

	void dosimultabref(HeaderC header,int npart, bool dnatrue,bool multithread,bool firsttime, int numscen,int seed)
	{
               int ipart,jpart=0,nthreads;
	       int gr,nstat,pa,ip,iscen;
               bool trouve,trace;
               this->npart = npart;
               int *sOK;
               trace=true;
               //if (trace) cout <<"debut de dosimultabref\n";fflush(stdin);
               sOK = new int[npart];
               //if (this->defined) cout <<"particleSet defined\n";
               //else cout<<"particleSet UNdefined\n";
               //this->header = header;
               if (firsttime) {                
                    this->particule = new ParticleC[this->npart];
                    this->header = header;
                    for (int p=0;p<this->npart;p++) {
                        //cout <<"avant set particule "<<p<<"\n";
                        this->particule[p].dnatrue = dnatrue;
                        //cout <<"dnatrue\n";
                        this->setdata(p);
                        //cout <<"setdata\n";
                        this->setgroup(p);
                        //cout<<"setgroup\n";
                        this->setloci(p);
                        //cout<<"setloci\n";
                        this->setscenarios(p);
                        //cout << "                    apres set particule\n";
                        this->particule[p].mw.randinit(p,seed);
                    }
                }
                else {
                     for (int p=0;p<this->npart;p++) {
                        this->resetparticle(p);
                     }
                }
                //if (trace) cout << "avant pragma npart = "<<npart<<"\n";
	       #pragma omp parallel for shared(sOK) private(gr) if(multithread)
                for (ipart=0;ipart<this->npart;ipart++){
                        //if (trace) cout <<"avant dosimulpart de la particule "<<ipart<<"\n";
			sOK[ipart]=this->particule[ipart].dosimulpart(trace,numscen);
                        //if (trace) cout<<"apres dosimulpart de la particule "<<ipart<<"\n";
			if (sOK[ipart]==0) {
			 	for(gr=1;gr<=this->particule[ipart].ngr;gr++) this->particule[ipart].docalstat(gr);
			}
			//if (trace) cout<<"apres docalstat de la particule "<<ipart<<"\n";
		}
//fin du pragma
                //if (trace) cout << "apres pragma\n";
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
		//if (trace) cout<<"apres remplissage des enreg\n";fflush(stdin);
                if (firsttime){
                      FILE * pFile;
                      char  *curfile;
                      //cout<<"avant curfile\n";fflush(stdin);
                      curfile = new char[strlen(this->header.pathbase)+13];
                      strcpy(curfile,this->header.pathbase);
                      strcat(curfile,"courant.log");
                      //cout<<curfile<<"\n";
                      pFile = fopen (curfile,"w");
                      fprintf(pFile,"%s\n",this->header.entete.c_str());
                      for (int ipart=0;ipart<this->npart;ipart++) {
                              if (sOK[ipart]==0){
                                fprintf(pFile,"%3d  ",enreg[ipart].numscen);
                                iscen=enreg[ipart].numscen-1;
                                //cout<<"scenario "<<enreg[ipart].numscen<<"\n";
                                pa=0;
                                //cout<<header.nparamtot<<"   "<<this->particule[ipart].scen.nparamvar<<"\n";
                                for (int j=0;j<header.nparamtot;j++) {
                                    trouve=false;ip=-1;
                                    while ((not trouve)and(ip<header.scenario[iscen].nparam-1)) {
                                        ip++;
                                        trouve=(header.histparam[j].name == header.scenario[iscen].histparam[ip].name);
                                        //cout<<"->"<<header.histparam[j].name<<"<-   ?   ->"<< header.scenario[iscen].histparam[ip].name<<"<-"<<trouve<<"\n";
                                    }
                                    if (trouve) {fprintf(pFile,"  %12.6f",enreg[ipart].param[ip]);pa++;}
                                    else fprintf(pFile,"              ");
                                }
                                for (int j=pa;j<this->particule[ipart].scen.nparamvar;j++) fprintf(pFile,"  %12.6f",enreg[ipart].param[j]);
                                for (int st=0;st<nstat;st++) fprintf(pFile,"  %12.6f",enreg[ipart].stat[st]);
                                fprintf(pFile,"\n");
                              }
                      }
                      fclose(pFile);
                }
		//cout <<"fin de l'ecriture du fichier courant.log' \n";
		//cleanParticleSet();
		//cout << "fin de dosimultabref\n";
                delete [] sOK;
                //if (trace) cout <<"fin de dosimultabref\n";
	}
};
