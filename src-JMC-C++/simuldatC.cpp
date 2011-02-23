#include <iostream>

//using namespace boost::python;
//using namespace std;
//#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "particuleC.cpp"
//#include "data.cpp"

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
    string message;
};

class HeaderC
{
public:
	string message,datafilename,entete;
	DataC dataobs;
	int nparamtot,nstat,nscenarios,nconditions,ngroupes;
	ScenarioC *scenario;
	HistParameterC *histparam;
	ConditionC *condition;
	LocusGroupC *groupe;
	bool drawuntil;

	PriorC readprior(string ss) {
		PriorC prior;
		string s1,*sb;
		int j;
		s1 = ss.substr(3,ss.length()-4);
		sb = splitwords(s1,",",&j);
		prior.mini=atof(sb[0].c_str());
		prior.maxi=atof(sb[1].c_str());
                prior.ndec=ndecimales(prior.mini,prior.maxi);
                //cout << "ndec="<<prior.ndec<<"   (mini="<<prior.mini<<"   maxi="<<prior.maxi<<"\n";
		if (ss.find("UN[")!=string::npos) {prior.loi="UN";}
		else if (ss.find("LU[")!=string::npos) {prior.loi="LU";}
		else if (ss.find("NO[")!=string::npos) {prior.loi="NO";prior.mean=atof(sb[2].c_str());prior.sdshape=atof(sb[3].c_str());}
		else if (ss.find("LN[")!=string::npos) {prior.loi="LN";prior.mean=atof(sb[2].c_str());prior.sdshape=atof(sb[3].c_str());}
		else if (ss.find("GA[")!=string::npos) {prior.loi="GA";prior.mean=atof(sb[2].c_str());prior.sdshape=atof(sb[3].c_str());}
		prior.constant = ((prior.maxi-prior.mini)/prior.maxi<0.000001);
		return prior;
	}

	PriorC readpriormut(string ss) {
		PriorC prior;
		string s1,*sb;
		int j;
		s1 = ss.substr(3,ss.length()-4);
		sb = splitwords(s1,",",&j);
		prior.mini=atof(sb[0].c_str());
		prior.maxi=atof(sb[1].c_str());
		prior.ndec=ndecimales(prior.mini,prior.maxi);
		if (ss.find("UN[")!=string::npos) {prior.loi="UN";}
		else if (ss.find("LU[")!=string::npos) {prior.loi="LU";}
		else if (ss.find("GA[")!=string::npos) {prior.loi="GA";prior.mean=atof(sb[2].c_str());prior.sdshape=atof(sb[3].c_str());}
		prior.constant = ((prior.maxi-prior.mini)/prior.maxi<0.000001);
		return prior;
	}


	ConditionC readcondition(string ss){
		ConditionC cond;
		if (ss.find(">=")!=string::npos){
			cond.operateur=">=";cond.param1=ss.substr(0,ss.find(">="));cond.param2=ss.substr(ss.find(">=")+2,ss.length()-(ss.find(">=")+2));}
		else if (ss.find("<=")!=string::npos){
			cond.operateur="<=";cond.param1=ss.substr(0,ss.find("<="));cond.param2=ss.substr(ss.find("<=")+2,ss.length()-(ss.find("<=")+2));}
		if (ss.find(">")!=string::npos){
			cond.operateur=">";cond.param1=ss.substr(0,ss.find(">"));cond.param2=ss.substr(ss.find(">")+1,ss.length()-(ss.find(">")+1));}
		else if (ss.find("<")!=string::npos){
			cond.operateur="<";cond.param1=ss.substr(0,ss.find("<"));cond.param2=ss.substr(ss.find("<")+1,ss.length()-(ss.find("<")+1));}
			return cond;
	}

	void assignloc(int gr){
		this->groupe[gr].nloc = 0;
		for (int loc=0;loc<dataobs.nloc;loc++) {
			if (dataobs.locus[loc].groupe==gr) this->groupe[gr].nloc++;
		}
		this->groupe[gr].loc = new int[this->groupe[gr].nloc];
		int iloc=-1;
		for (int i=0;i<dataobs.nloc;i++) {
			if (dataobs.locus[i].groupe==gr) {iloc++;this->groupe[gr].loc[iloc] = i;}
		}
	}

	HeaderC* readHeader(char* headerfilename){
                char reftable[]="header.txt";
                char *path;
		string s1,s2,**sl,*ss,*ss1,*ss2;
		int *nlscen,nss,nss1,j,k,l,gr,grm,k1,k2;
		ifstream file(headerfilename, ios::in);
		if (file == NULL) {
			this->message = "File ReftableHeader.txt not found";
			return this;
		} else this->message="";
		getline(file,this->datafilename);
                path = new char[strlen(headerfilename)];
                strcpy(path,headerfilename);
                k = strpos(headerfilename,reftable);
                path[k]='\0';
                strcat(path,this->datafilename.c_str());
                this->datafilename=string(path);
		this->dataobs.loadfromfile(path);
		getline(file,s1);
		this->nparamtot=getwordint(s1,1);
		this->nstat=getwordint(s1,4);
//Partie Scenarios
		getline(file,s1);		//ligne vide
		getline(file,s1);       // nombre de scenarios
		this->nscenarios=getwordint(s1,1);
		sl = new string*[this->nscenarios];
		nlscen = new int[this->nscenarios];
		this->scenario = new ScenarioC[this->nscenarios];
		for (int i=0;i<this->nscenarios;i++) nlscen[i]=getwordint(s1,3+i);
		for (int i=0;i<this->nscenarios;i++) {
			sl[i] = new string[nlscen[i]];
			getline(file,s1);
			scenario[i].number = getwordint(s1,2);
			scenario[i].prior_proba = getwordfloat(s1,3);
			scenario[i].nparam = 0;scenario[i].nparamvar=0;
			for (int j=0;j<nlscen[i];j++) getline(file,sl[i][j]);
			scenario[i].read_events(nlscen[i],sl[i]);
		}
		for (int i=0;i<this->nscenarios;i++) delete []sl[i];
		delete [] sl;
//Partie historical parameters
		getline(file,s1);		//ligne vide
		getline(file,s1);
		ss=splitwords(s1," ",&nss);
		s2=ss[3].substr(1,ss[3].length()-2);
		ss2=splitwords(s2,",",&nss);
		this->nparamtot = atoi(ss2[0].c_str());
		this->nconditions = atoi(ss2[1].c_str());
		delete [] ss2;
		this->histparam = new HistParameterC[this->nparamtot];
		if (this->nconditions>0) this->condition = new ConditionC[this->nconditions];
		delete [] ss;
		for (int i=0;i<this->nparamtot;i++) {
			getline(file,s1);
			ss=splitwords(s1," ",&nss);
			this->histparam[i].name=ss[0];
			//cout<<this->histparam[i].name<<"\n";
			if (ss[1]=="N") this->histparam[i].category = 0;
			else if  (ss[1]=="T") this->histparam[i].category = 1;
			else if  (ss[1]=="A") this->histparam[i].category = 2;
			this->histparam[i].prior = this->readprior(ss[2]);
		}
		delete [] ss;
		if (this->nconditions>0) {
			this->condition = new ConditionC[this->nconditions];
			for (int i=0;i<this->nconditions;i++) {
				getline(file,s1);
				this->condition[i] = this->readcondition(s1);
			}
			getline(file,s1);
			if (s1 != "DRAW UNTIL") this->drawuntil=false; else  this->drawuntil=true;
		}
//retour sur les parametres spécifiques à chaque scenario;
		for (int i=0;i<this->nscenarios;i++) {
			this->scenario[i].nparamvar=0;
			for (int j=0;j<this->scenario[i].nparam;j++) {
				int k=0;
				while (this->scenario[i].histparam[j].name!=this->histparam[k].name) k++;
				this->scenario[i].histparam[j].prior = copyprior(this->histparam[k].prior);
				if (not this->histparam[k].prior.constant) {
					//this->scenario[i].paramvar[this->scenario[i].nparamvar].prior = copyprior(this->histparam[k].prior);
					this->scenario[i].nparamvar++;
				}	
			}
			//cout<<"scenario "<<i<<"   "<<this->scenario[i].nparam<<" param et "<<this->scenario[i].nparamvar<<" paramvar\n "<<flush;
		}
//retour sur les conditions spécifiques à chaque scenario
        if (this->nconditions>0) {
            for (int i=0;i<this->nscenarios;i++) {
                int nc=0;
                for (j=0;j<this->nconditions;j++) {
                    int np=0;
                    for (k=0;k<this->scenario[i].nparam;k++) {
                        if (this->condition[j].param1 == this->scenario[i].histparam[k].name) np++;
                        if (this->condition[j].param2 == this->scenario[i].histparam[k].name) np++;
                    }
                    if (np==2) nc++;
                }
                this->scenario[i].nconditions=nc;
                this->scenario[i].condition = new ConditionC[nc];
                nc=0;
                while (nc<this->scenario[i].nconditions) {
                    for (j=0;j<this->nconditions;j++) {
                        int np=0;
                        for (k=0;k<this->scenario[i].nparam;k++) {
                            if (this->condition[j].param1 == this->scenario[i].histparam[k].name) np++;
                            if (this->condition[j].param2 == this->scenario[i].histparam[k].name) np++;
                        }
                        if (np==2) {
                            this->scenario[i].condition[nc]=copycondition(this->condition[j]);
                            this->scenario[i].condition[nc].ecris();
                            nc++;   
                        }                   
                    }
                }
                //cout <<"dans readheader  \n";
                
           }
        } else for (int i=0;i<this->nscenarios;i++) this->scenario[i].nconditions=0;
//Partie loci description
		getline(file,s1);		//ligne vide
		getline(file,s1);		//ligne "loci description"
		grm=1;
		for (int loc=0;loc<this->dataobs.nloc;loc++){
			getline(file,s1);
			ss=splitwords(s1," ",&nss);
			k=0;while (ss[k].find("[")==string::npos) k++;
			if (ss[k]=="[M]") {
				s1=ss[k+1].substr(1,ss[k+1].length());gr=atoi(s1.c_str());this->dataobs.locus[loc].groupe=gr;if (gr>grm) grm=gr;
				this->dataobs.locus[loc].motif_size=atoi(ss[k+2].c_str());this->dataobs.locus[loc].motif_range=atoi(ss[k+3].c_str());
			}
			else if (ss[k]=="[S]") {
				s1=ss[k+1].substr(1,ss[k+1].length());gr=atoi(s1.c_str());this->dataobs.locus[loc].groupe=gr;if (gr>grm) grm=gr;
				//cout<<this->dataobs.locus[loc].dnalength<<"\n";  
				//this->dataobs.locus[loc].dnalength=atoi(ss[k+2].c_str());  //inutile variable déjà renseignée
			}
		}
		delete [] ss;
//Partie group priors
		getline(file,s1);		//ligne vide
		getline(file,s1);		//ligne "group prior"
                this->ngroupes=getwordint(s1,3);
		this->groupe = new LocusGroupC[this->ngroupes+1];
		this->assignloc(0);
		//cout<<"on attaque les groupes : analyse des priors nombre de groupes="<<this->ngroupes <<"\n";
		for (gr=1;gr<=this->ngroupes;gr++){
			getline(file,s1);
			ss=splitwords(s1," ",&nss);
			this->assignloc(gr);
			if (ss[2]=="[M]") {
				this->groupe[gr].type=0;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormutmoy = this->readpriormut(ss1[1]);delete [] ss1;
				if (this->groupe[gr].priormutmoy.constant) this->groupe[gr].mutmoy=this->groupe[gr].priormutmoy.mini; 
				else {this->groupe[gr].mutmoy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}
				
				getline(file,s1);ss1=splitwords(s1," ",&nss1);
                                //cout<<ss1[1]<<"\n";
                                
                                this->groupe[gr].priormutloc = this->readpriormut(ss1[1]);delete [] ss1;
				//cout<<"mutloc  ";this->groupe[gr].priormutloc.ecris();
				
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorPmoy   = this->readpriormut(ss1[1]);delete [] ss1;
				if (this->groupe[gr].priorPmoy.constant) this->groupe[gr].Pmoy=this->groupe[gr].priorPmoy.mini; 
				else {this->groupe[gr].Pmoy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}
				
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorPloc   = this->readpriormut(ss1[1]);delete [] ss1;
				//cout<<"Ploc    ";this->groupe[gr].priorPloc.ecris();
				
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorsnimoy = this->readpriormut(ss1[1]);delete [] ss1;
				if (this->groupe[gr].priorsnimoy.constant) this->groupe[gr].snimoy=this->groupe[gr].priorsnimoy.mini; 
				else {this->groupe[gr].snimoy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}
				
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorsniloc = this->readpriormut(ss1[1]);delete [] ss1;
				//cout<<"sniloc  ";this->groupe[gr].priorsniloc.ecris();
				
			} else if (ss[2]=="[S]") {
				this->groupe[gr].type=1;
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormusmoy = this->readpriormut(ss1[1]);delete [] ss1;
				if (this->groupe[gr].priormusmoy.constant) this->groupe[gr].musmoy=this->groupe[gr].priormusmoy.mini; 
				else {this->groupe[gr].musmoy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}
				
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormusloc = this->readpriormut(ss1[1]);delete [] ss1;
				
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork1moy  = this->readpriormut(ss1[1]);delete [] ss1;
				if (this->groupe[gr].priork1moy.constant) this->groupe[gr].k1moy=this->groupe[gr].priork1moy.mini; 
				else {this->groupe[gr].k1moy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}
				
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork1loc  = this->readpriormut(ss1[1]);delete [] ss1;
				
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork2moy  = this->readpriormut(ss1[1]);delete [] ss1;
				if (this->groupe[gr].priork2moy.constant) this->groupe[gr].k2moy=this->groupe[gr].priork2moy.mini; 
				else {this->groupe[gr].k2moy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}
				
				getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork2loc  = this->readpriormut(ss1[1]);delete [] ss1;
				
				getline(file,s1);ss1=splitwords(s1," ",&nss1);
				this->groupe[gr].p_fixe=atof(ss1[2].c_str());this->groupe[gr].gams=atof(ss1[3].c_str());
				if (ss1[1]=="JK") this->groupe[gr].mutmod=0;
				else if (ss1[1]=="K2P") this->groupe[gr].mutmod=1;
				else if (ss1[1]=="HKY") this->groupe[gr].mutmod=2;
				else if (ss1[1]=="TN") this->groupe[gr].mutmod=3;
				cout<<"mutmod = "<<this->groupe[gr].mutmod <<"\n";
			}
		}
		//cout<<"avant la mise à jour des paramvar\n";fflush(stdin);
		delete [] ss;
//Mise à jour des paramvar
		for (int i=0;i<this->nscenarios;i++) {
			this->scenario[i].paramvar = new double[this->scenario[i].nparamvar];
			for (int j=0;j<this->scenario[i].nparamvar;j++)this->scenario[i].paramvar[j]=-1.0;
			//this->scenario[i].ecris();
		}
		
//Partie group statistics
         //cout<<"debut des group stat\n";
                this->nstat=0;
		getline(file,s1);		//ligne vide
		getline(file,s1);		//ligne "group group statistics"
		//cout <<"s1="<<s1<<"\n";
                for (gr=1;gr<=this->ngroupes;gr++) {
			getline(file,s1);
			ss=splitwords(s1," ",&nss);
                        //cout <<"s1="<<s1<<"   ss[3]="<< ss[3] <<"   atoi = "<< atoi(ss[3].c_str()) <<"\n";
			this->groupe[gr].nstat = getwordint(ss[3],0);
                        //cout <<"s1="<<s1<<"   ss[3]="<< ss[3] <<"   atoi = "<< this->groupe[gr].nstat <<"\n";
                        this->nstat +=this->groupe[gr].nstat;
			this->groupe[gr].sumstat = new StatC[this->groupe[gr].nstat];
			delete [] ss;
			k=0;
			while (k<this->groupe[gr].nstat) {
                          //cout <<"stat "<<k<<"    groupe "<<gr<<"\n";
				getline(file,s1);
                                //cout <<"s1="<<s1<<"\n";
				ss=splitwords(s1," ",&nss);
                                //cout << ss[0]<<"\n";
				j=0;while (ss[0]!=stat_type[j]) j++;
                                //cout<<"j="<<j<<"\n";
				if (this->groupe[gr].type==0) {   //MICROSAT
					if (stat_num[j]<5) {
						for (int i=1;i<nss;i++) {
						      this->groupe[gr].sumstat[k].cat=stat_num[j];
						      this->groupe[gr].sumstat[k].samp=atoi(ss[i].c_str());
						      k++;
						}
					} else if (stat_num[j]<12) {
						for (int i=1;i<nss;i++) {
							this->groupe[gr].sumstat[k].cat=stat_num[j];
							ss1=splitwords(ss[i],"&",&nss1);
							this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
							this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
							k++;
						}
						delete [] ss1;
					} else if (stat_num[j]==12) {
						for (int i=1;i<nss;i++) {
							this->groupe[gr].sumstat[k].cat=stat_num[j];
							ss1=splitwords(ss[i],"&",&nss1);
							this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
							this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
							this->groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
							k++;
						}
						delete [] ss1;
					}
				} else if (this->groupe[gr].type==1) {   //DNA SEQUENCE
					if (stat_num[j]>-5) {
						for (int i=1;i<nss;i++) {
						       this->groupe[gr].sumstat[k].cat=stat_num[j];
						       this->groupe[gr].sumstat[k].samp=atoi(ss[i].c_str());
						       k++;
						}
					} else if (stat_num[j]>-14) {
						for (int i=1;i<nss;i++) {
							this->groupe[gr].sumstat[k].cat=stat_num[j];
							ss1=splitwords(ss[i],"&",&nss1);
							this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
							this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
							k++;
						}
						delete [] ss1;
					} else if (stat_num[j]==-14) {
						for (int i=1;i<nss;i++) {
							this->groupe[gr].sumstat[k].cat=stat_num[j];
							ss1=splitwords(ss[i],"&",&nss1);
							this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
							this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
							this->groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
							k++;
						}
						delete [] ss1;
					}
				}
			}
			delete [] ss;
		}
//Entete du fichier reftable
		getline(file,s1);		//ligne vide
		getline(file,this->entete);		//ligne entete
                //cout<<this->entete<<"\n";fflush(stdin);
		return this;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
				this->particule[p].locuslist[kloc].mut_rate = this->header.dataobs.locus[kloc].mut_rate;
				this->particule[p].locuslist[kloc].Pgeom = this->header.dataobs.locus[kloc].Pgeom;
				this->particule[p].locuslist[kloc].sni_rate = this->header.dataobs.locus[kloc].sni_rate;
				}
			else {
				this->particule[p].locuslist[kloc].dnalength =  this->header.dataobs.locus[kloc].dnalength;
				this->particule[p].locuslist[kloc].mus_rate =  this->header.dataobs.locus[kloc].mus_rate;
				this->particule[p].locuslist[kloc].k1 =  this->header.dataobs.locus[kloc].k1;
				this->particule[p].locuslist[kloc].k2 =  this->header.dataobs.locus[kloc].k2;
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
		    this->particule[p].scenario[i] = copyscenario(this->header.scenario[i]);
		    //this->particule[p].scenario[i].ecris();
		}
		
	}

	void cleanParticle(int ipart){
//nettoyage de locuslist
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
//nettoyage de grouplist		
		for (int gr=0;gr<this->particule[ipart].ngr;gr++) {
			delete []this->particule[ipart].grouplist[gr].loc;
			delete []this->particule[ipart].grouplist[gr].sumstat;
		}
		delete []this->particule[ipart].grouplist;
//nettoyage de data
		delete []this->particule[ipart].data.nind;
		for(int sa=0;sa<this->particule[ipart].data.nsample;sa++) delete []this->particule[ipart].data.indivsexe[sa];
		delete []this->particule[ipart].data.indivsexe;
//nettoyage de scenario
		for (int i=0;i<this->particule[ipart].nscenarios;i++) {
			delete []this->particule[ipart].scenario[i].paramvar;
			delete []this->particule[ipart].scenario[i].time_sample;
			delete []this->particule[ipart].scenario[i].event;
			delete []this->particule[ipart].scenario[i].ne0;
			delete []this->particule[ipart].scenario[i].histparam;
		}
		delete []this->particule[ipart].scenario;
//nettoyage de matQ
		for (int i=0;i<4;i++) delete []this->particule[ipart].matQ[i];
		delete []this->particule[ipart].matQ;
//nettoyage des données manquantes		
	        delete []this->particule[ipart].mhap;
	        delete []this->particule[ipart].mnuc;
	}
	
	void cleanParticleSet(){
		for (int i=0;i<this->npart;i++) this->cleanParticle(i);
		delete []this->particule;
		for (int loc=0;loc<this->nloc;loc++) {if (this->locuslist[loc].type>4)   delete []this->locuslist[loc].mutsit;}
		delete []this->locuslist;
		for (int gr=0;gr<this->ngr;gr++) delete []this->grouplist[gr].loc;
		delete []this->grouplist;
		delete []this->nind;
		for (int i=0;i<this->nsample;i++) delete[]this->indivsexe[i];
		delete []this->indivsexe;
	}

	enregC* dosimultabref(HeaderC header,int npart, bool dnatrue)
	{
		int gr;
                this->npart = npart;
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
		}
		int ipart,tid=0,jpart=0,nthreads,base;
		int *sOK;
		sOK = new int[npart];
		enregC *enreg;
		int nstat;
                enreg = new enregC[this->npart];
		//cout << "avant pragma\n";
                base=rand();
                
//#pragma omp parallel for shared(sOK) private(tid,gr)
                for (ipart=0;ipart<this->npart;ipart++){
			//tid = omp_get_thread_num();
			//cout <<"tid = "<<tid <<"\n";
                        this->particule[ipart].mw.randinit(ipart+base ,false);
			sOK[ipart]=this->particule[ipart].dosimulpart(ipart);
			if (sOK[ipart]==0) {
			 	for(gr=1;gr<=this->particule[ipart].ngr;gr++) this->particule[ipart].docalstat(gr);
			} 
		}
//fin du pragma
                //cout << "apres pragma\n";
                for (int ipart=0;ipart<this->npart;ipart++) {
			if (sOK[ipart]==0){
				enreg[ipart].numscen=0;
				if (this->particule[ipart].nscenarios>1) {enreg[ipart].numscen=this->particule[ipart].scen.number;}
				enreg[ipart].param = new float[this->particule[ipart].scen.nparamvar];
				for (int j=0;j<this->particule[ipart].scen.nparamvar;j++) {enreg[ipart].param[j]=this->particule[ipart].scen.paramvar[j];}
				enreg[ipart].stat = new float[header.nstat];
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
                /*FILE * pFile;
                pFile = fopen ("courant.log","w");
                for (int ipart=0;ipart<this->npart;ipart++) {
                        if (sOK[ipart]==0){
                          fprintf(pFile,"scen %d",enreg[ipart].numscen);
                          for (int j=0;j<this->particule[ipart].scen.nparamvar;j++) fprintf(pFile,"  %12.6f",enreg[ipart].param[j]);
                          for (int st=0;st<nstat;st++) fprintf(pFile,"   %6.2f",enreg[ipart].stat[st]);
                          fprintf(pFile,"\n");
                        }
                }
                fclose(pFile);*/
		//cout <<"fin du remplissage \n";
		//cleanParticleSet();
		//cout << "fin de dosimultabref\n";
		return enreg;
	}
};
