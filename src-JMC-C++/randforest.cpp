/*
 * randforest.cpp
 * 
 * Created on : 24 november 2014
 * 
 */

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include "randforest.h"
#include "mesutils.h"
#include "matrices.h"
#include <stdlib.h>
#include "randomgenerator.h"
#include <algorithm>
#include <vector>
#include <math.h>
#include <iomanip>
#include "particleset.h"
#include <time.h>

#define LOG2 0.69314718055994530942;


using namespace std;

extern string ident, headerfilename;
extern bool multithread;
extern string progressfilename, path, ident;
extern string scurfile;
extern HeaderC header;
extern ReftableC rt;
extern int debuglevel;
extern enregC *enreg;
extern long double **phistarOK;
extern ParticleSetC ps;

RFC rf,rfmin;
ofstream fout;
int nvoisins = 500, nj = 20;


bool operator < (const VMC & lhs,const VMC & rhs) {
	return lhs.x < rhs.x;
}

bool operator < (const VMD & lhs,const VMD & rhs) {
	return lhs.x > rhs.x;
}

/**
 * Definition de l'operateur = pour une instance de la classe NodeC
 */
  NodeRC & NodeRC::operator= (NodeRC const & source) {
	if (this== &source) return *this;
	this->pere = source.pere;
	this->filsG = source.filsG;
	this->filsD = source.filsD;
	this->nvar = source.nvar;
	this->nsets = source.nsets;
	this->npassages = source.npassages;
	this->nsetG = source.nsetG;
	this->nsetD = source.nsetD;
	this->model = source.model;
	this->imax = source.imax;
	this->cutval = source.cutval;
	this->disval = source.disval;
	this->delta = source.delta;
	this->terminal = source.terminal;
	if (not this->indvar.empty()) this->indvar.clear();
	if (not source.indvar.empty()) {
		this->indvar = vector<int>(source.indvar.size());
		for (int i=0;i<source.indvar.size();i++) this->indvar[i] = source.indvar[i];
	}
	if (not this->numset.empty()) this->numset.clear();
	if (not source.numset.empty()) {
		this->numset = vector<int>(source.numset.size());
		for (int i=0;i<source.numset.size();i++) this->numset[i] = source.numset[i];
	}
	if (not this->numsetG.empty()) this->numsetG.clear();
	if (not source.numsetG.empty()) {
		this->numsetG = vector<int>(source.numsetG.size());
		for (int i=0;i<source.numsetG.size();i++) this->numsetG[i] = source.numsetG[i];
	}
	if (not this->numsetD.empty()) this->numsetD.clear();
	if (not source.numsetD.empty()) {
		this->numsetD = vector<int>(source.numsetD.size());
		for (int i=0;i<source.numsetD.size();i++) this->numsetD[i] = source.numsetD[i];
	}
	return *this;
}


/**
 * Definition de l'operateur = pour une instance de la classe TreeC
 */
  TreeC & TreeC::operator= (TreeC const & source) {
	if (this== &source) return *this;
	this->nnodes = source.nnodes;
	this->nsets = source.nsets;
	this->nvar = source.nvar;
	this->fmax = source.fmax;
	this->fin = source.fin;
	/// ON NE COPIE PAS LE GENERATEUR DE NOMBRES ALEATOIRES 
	if (not this->numset.empty()) this->numset.clear();
	if (not source.numset.empty()) {
		this->numset = vector<int>(source.numset.size());
		for (int i=0;i<source.numset.size();i++) this->numset[i] = source.numset[i];
	}
	if (not this->indsel.empty()) this->indsel.clear();
	if (not source.indsel.empty()) {
		this->indsel = vector<int>(source.indsel.size());
		for (int i=0;i<source.indsel.size();i++) this->indsel[i] = source.indsel[i];
	}
	if (not this->score.empty()) this->score.clear();
	if (not source.score.empty()) {
		this->score = vector<int>(source.score.size());
		for (int i=0;i<source.score.size();i++) this->score[i] = source.score[i];
	}
	if (not this->index.empty()) this->index.clear();
	if (not source.index.empty()) {
		this->index = vector<int>(source.index.size());
		for (int i=0;i<source.index.size();i++) this->index[i] = source.index[i];
	}
	if (not this->node.empty()) this->node.clear();
	if (not source.node.empty()) {
		this->node = vector<NodeRC>(source.node.size());
		for (int i=0;i<source.node.size();i++) this->node[i] = source.node[i];
	}
	if (not this->wind.empty()) this->wind.clear();
	if (not source.wind.empty()) {
		this->wind = vector<double>(source.wind.size());
		for (int i=0;i<source.wind.size();i++) this->wind[i] = source.wind[i];
	}
	if (not this->wstat.empty()) this->wstat.clear();
	if (not source.wstat.empty()) {
		this->wstat = vector<double>(source.wstat.size());
		for (int i=0;i<source.wstat.size();i++) this->wstat[i] = source.wstat[i];
	}
	if (not this->varused.empty()) this->varused.clear();
	if (not source.varused.empty()) {
		this->varused = vector<bool>(source.varused.size());
		for (int i=0;i<source.varused.size();i++) this->varused[i] = source.varused[i];
	}
	return *this;
}

/**
 * Definition de l'operateur = pour une instance de la classe RFC
 */
  RFC & RFC::operator= (RFC const & source) {
	if (this== &source) return *this;
	this->ntrees = source.ntrees;
	this->ntot = source.ntot;
	this->nsets = source.nsets;
	this->nstat = source.nstat;
	this->nmodel = source.nmodel;
	this->nvar = source.nvar;
	this->nsel = source.nsel;
	this->nstatclass = source.nstatclass;
	this->nbootsamp = source.nbootsamp;
	if (not this->model.empty()) this->model.clear();
	if (not source.model.empty()) {
		this->model = vector<int>(source.model.size());
		for (int i=0;i<source.model.size();i++) this->model[i] = source.model[i];
	}
	if (not this->indsel.empty()) this->indsel.clear();
	if (not source.indsel.empty()) {
		this->indsel = vector<int>(source.indsel.size());
		for (int i=0;i<source.indsel.size();i++) this->indsel[i] = source.indsel[i];
	}
	if (not this->nimportance.empty()) {
		for (int i=0;i<this->nimportance.size();i++) {
			if (not this->nimportance[i].empty()) this->nimportance[i].clear();
		}
		this->nimportance.clear();
	}
	if (not source.nimportance.empty()) {
		this->nimportance = vector < vector  <int> >(source.nimportance.size());
		for (int i=0;i<source.nimportance.size();i++) {
			if (not source.nimportance[i].empty()) {
				this->nimportance[i] = vector<int>(source.nimportance[i].size());
				for (int j=0;j<source.nimportance[i].size();j++) this->nimportance[i][j] = source.nimportance[i][j];
			}
		}
	}
	if (not this->vote.empty()) {
		for (int i=0;i<this->vote.size();i++) {
			if (not this->vote[i].empty()) this->vote[i].clear();
		}
		this->vote.clear();
	}
	if (not source.vote.empty()) {
		this->vote = vector < vector <double> >(source.vote.size());
		for (int i=0;i<source.vote.size();i++) {
			if (not source.vote[i].empty()) {
				this->vote[i] = vector<double>(source.vote[i].size());
				for (int j=0;j<source.vote[i].size();j++) this->vote[i][j] = source.vote[i][j];
			}
		}
	}
	if (not this->stat.empty()) {
		for (int i=0;i<this->stat.size();i++) {
			if (not this->stat[i].empty()) this->stat[i].clear();
		}
		this->stat.clear();
	}
	if (not source.stat.empty()) {
		this->stat = vector < vector <double> >(source.stat.size());
		for (int i=0;i<source.stat.size();i++) {
			if (not source.stat[i].empty()) {
				this->stat[i] = vector<double>(source.stat[i].size());
				for (int j=0;j<source.stat[i].size();j++) this->stat[i][j] = source.stat[i][j];
			}
		}
	}
	if (not this->importance.empty()) {
		for (int i=0;i<this->importance.size();i++) {
			if (not this->importance[i].empty()) this->importance[i].clear();
		}
		this->importance.clear();
	}
	if (not source.importance.empty()) {
		this->importance = vector < vector <double> >(source.importance.size());
		for (int i=0;i<source.importance.size();i++) {
			if (not source.importance[i].empty()) {
				this->importance[i] = vector<double>(source.importance[i].size());
				for (int j=0;j<source.importance[i].size();j++) this->importance[i][j] = source.importance[i][j];
			}
		}
	}
	if (not this->bootsamp.empty()) this->bootsamp.clear();
	if (not source.bootsamp.empty()) {
		this->bootsamp = vector<int>(source.bootsamp.size());
		for (int i=0;i<source.bootsamp.size();i++) this->bootsamp[i] = source.bootsamp[i];
	}
	if (not this->varimp.empty()) this->varimp.clear();
	if (not source.varimp.empty()) {
		this->varimp = vector<double>(source.varimp.size());
		for (int i=0;i<source.varimp.size();i++) this->varimp[i] = source.varimp[i];
	}
	if (not this->tree.empty()) this->tree.clear();
	if (not source.tree.empty()) {
		this->tree = vector<TreeC>(source.tree.size());
		for (int i=0;i<source.tree.size();i++) this->tree[i] = source.tree[i];
	}
	if (not this->statobs.empty()) this->statobs.clear();
	if (not source.statobs.empty()) {
		this->statobs = vector<double>(source.statobs.size());
		for (int i=0;i<source.statobs.size();i++) this->statobs[i] = source.statobs[i];
	}
	if (not this->statname.empty()) this->statname.clear();
	if (not source.statname.empty()) {
		this->statname = vector<string>(source.statname.size());
		for (int i=0;i<source.statname.size();i++) this->statname[i] = source.statname[i];
	}
	return *this;
}
	

	double NodeRC::caldisval(int nscen, int nsets, const vector<VMC>&vm, double val, vector <int>& nn) {
		double res,cg,cd,ggg,ddd,gd;
		vector <int>ga,dr;
		int gg,dd;
		ga = vector <int>(nscen);
		dr = vector <int>(nscen);
		for (int m=0;m<nscen;m++) {ga[m]=0;dr[m]=0;}
		gg=0;dd=0;
		for (int k=0;k<nsets;k++) {
			if (vm[k].x<val) {ga[vm[k].ind]++;gg++;}
			if (vm[k].x==val) {
				if (gg>dd) {dr[vm[k].ind]++;dd++;}
				else {ga[vm[k].ind]++;gg++;}
			}
			if (vm[k].x>val){dr[vm[k].ind]++;dd++;}
		}
		res=0.0;
		if (gg*dd==0) return 100.0;
		ggg=(double)gg;ddd=(double)dd;gd=ggg+ddd;
		for (int m=0;m<nscen;m++) {
			if (gg>0) cg= (double)ga[m]/ggg;
			if (dd>0) cd= (double)dr[m]/ddd;
			if (debuglevel==41) cout<<"caldisval nsets="<<nsets<<"   val="<<val<<"   gg="<<gg<<"    dd="<<dd<<"   ga["<<m<<"]="<<ga[m]<<"     dr["<<m<<"]="<<dr[m]<<"\n";
			res +=cg*(1.0-cg)*ggg+cd*(1.0-cd)*ddd; 
		}
		res=res/gd;
		nn[0]=gg;nn[1]=dd;
		if (res != res) {
			cout<<"val="<<val<<"\n";
			for (int k=0;k<nsets;k++) cout<<"("<<vm[k].ind<<")  "<<vm[k].x<<"   ";cout<<"\n";
			for (int m=0;m<nscen;m++) cout<<"ga["<<m<<"]="<<ga[m]<<"     dr["<<m<<"]="<<dr[m]<<"\n";
			cout<<"gg="<<gg<<"    dd="<<dd<<"\n";
			exit(1);
		}
		ga.clear();dr.clear();
		return res;
	}
	

	// calcul la règle au noeud courant
	bool NodeRC::getdisval(MwcGen& mw) {
		//double *dis,*cut,
		double c,d,da,db,dfa,epsilon,disG,disD,deltamax,xmin,xmax,xmed,fxmin,fxmax,fxmed,r=0.618,aa,bb;
		vector <VMC> vm;
		vector <int> nn,nnm;
		vector <double> f,b;
		vector <int> a;
		int k,kk,n,n1,m;
		bool ecr=true;
		//dis=new double[nvar];
		//cut=new double[nvar];
		disval=100.0;
		if (debuglevel==41) cout<<"rf.nmodel = "<<rf.nmodel<<"   rf.nsets="<<rf.nsets<<"   nsets="<<nsets<<"\n";
		vm = vector <VMC>(rf.nsets);
		nn = vector <int>(2);
		nnm= vector <int>(2);
		f = vector <double>(rf.nmodel);
		a = vector <int>(rf.nmodel,0);
		b = vector <double>(rf.nmodel);
		if (debuglevel==41) cout<<"getdisval nsets="<<rf.nsets<<"   nvar="<<rf.nvar<<"   rf.nmodel="<<rf.nmodel<<"\n";
		if (debuglevel==41) for (int m=0;m<rf.nmodel;m++) a[m]=0;
		if (debuglevel==41) cout<<"111111111111\n";
		if (debuglevel==41) {for (int j=0;j<10;j++) cout<<this->numset[j]<<"   ";cout<<"\n";}
		for (int j=0;j<nsets;j++) vm[j].ind=rf.model[this->numset[j]];
		if (debuglevel==41) {for (int j=0;j<10;j++) cout<<vm[j].ind<<"   ";cout<<"\n";}
		for (int j=0;j<nsets;j++) a[vm[j].ind]++;
		if (debuglevel==41) {for (int m=0;m<rf.nmodel;m++) cout<<"a["<<m<<"]="<<a[m]<<"   ";cout<<"\n";}
		if (debuglevel==41) cout<<"22222\n";
		n=0;for (int m=0;m<rf.nmodel;m++) if (a[m]>0) n++;
		//dd=0.0;for (int m=0;m<rf.nmodel;m++) if ((a[m]>0)and(a[m]<nsets)) {c=(double)a[m]/(double)nsets;dd-=c*log(c)/LOG2;}
		dfa=0.0;for (int m=0;m<rf.nmodel;m++) {c=(double)a[m]/(double)nsets;dfa +=c*(1.0-c);}
		if (debuglevel==41) cout<<"dfa="<<dfa<<"\n";
		for (int m=0;m<rf.nmodel;m++) if (a[m]>0) b[m]=1.0/(double)a[m]; else b[m]=0.0;
		sort(&a[0],&a[rf.nmodel]);
		n1=(int)a[rf.nmodel-1];
		if (debuglevel==41) {for (int m=0;m<rf.nmodel;m++) cout<<"b["<<m<<"]="<<b[m]<<"  ";cout<<"    n1="<<n1<<"\n";}
		if (nsets==n1) {a.clear();b.clear();f.clear();vm.clear();return true;}//cout<<"nsets="<<nsets<<"     n1="<<n1<<"\n";ecr=false;
		if (n1>1){ 
			for (int i=0;i<nvar;i++) {
				for (int j=0;j<nsets;j++) vm[j].ind=rf.model[numset[j]];
				vm[0].x=rf.stat[numset[0]][indvar[i]];xmax=xmin=vm[0].x;
				for (int j=1;j<nsets;j++){
					vm[j].x=rf.stat[numset[j]][indvar[i]];
					if (vm[j].x>xmax) xmax=vm[j].x;
					if (vm[j].x<xmin) xmin=vm[j].x;
				}
				aa=xmin;bb=xmax;epsilon=0.5*(aa+bb)/10000.0;
				//cout<<"\naa="<<aa<<"   bb="<<bb<<"\n";
				xmin=r*aa+(1.0-r)*bb;
				xmax=aa+bb-xmin;
				fxmin=caldisval(rf.nmodel, nsets, vm, xmin,nn);
				fxmax=caldisval(rf.nmodel, nsets, vm, xmax,nn);
				if (debuglevel==41) cout<<"aa="<<aa<<"   bb="<<bb<<"   xmin="<<xmin<<"   xmax="<<xmax<<"   fxmin="<<fxmin<<"   fxmax="<<fxmax<<"\n";
				m=0;
				while ((fabs(bb-aa)>fabs(epsilon))and(m<100)) {
					//cout<<"avant  "<<fabs(bb-aa)<<"   "<<epsilon<<"\n";
					if (fxmin<=fxmax) {
						bb=xmax;
						xmax=xmin;
						xmin=r*aa+(1-r)*bb;
						fxmax=fxmin;
						fxmin=caldisval(rf.nmodel, nsets, vm, xmin,nn);
					} else {
						aa=xmin;
						xmin=xmax;
						xmax=(1-r)*aa+r*bb;
						fxmin=fxmax;
						fxmax=caldisval(rf.nmodel, nsets, vm, xmax,nn);
					}
					//cout<<"aa="<<aa<<"   bb="<<bb<<"   xmin="<<xmin<<"   xmax="<<xmax<<"   fxmin="<<fxmin<<"   fxmax="<<fxmax<<"\n";
					//cout<<"apres  "<<fabs(bb-aa)<<"   "<<epsilon<<"\n";
					m++;
				}
				if (m>99) {
					cout<<"m="<<m<<"\n";
					cout<<"aa="<<aa<<"   bb="<<bb<<"   xmin="<<xmin<<"   xmax="<<xmax<<"   fxmin="<<fxmin<<"   fxmax="<<fxmax<<"\n";
					cout<<"apres  "<<fabs(bb-aa)<<"   "<<epsilon<<"\n";
					
				}
				c=0.5*(aa+bb);
				d=caldisval(rf.nmodel, nsets, vm,c,nn);
				da=caldisval(rf.nmodel, nsets, vm,aa,nn);
				db=caldisval(rf.nmodel, nsets, vm,bb,nn);
				if ((da<d)and(da<db)) {d=da;c=aa;}
				else if ((db<d)and(db<da)){d=db;c=bb;}
				d=caldisval(rf.nmodel, nsets, vm,c,nn);
				if (debuglevel==41) cout<<"aa="<<aa<<"   bb="<<bb<<"   c="<<c<<"   d="<<d<<"    epsilon="<<epsilon<<"\n";
				//cout<<"\nvar="<<indvar[i]<<"   c="<<c<<"   d="<<d<<"    disval="<<disval<<"  nn[0]="<<nn[0]<<"  nn[1]="<<nn[1]<<"\n";
				if (disval>d) {
					disval=d;
					cutval=c;
					imax=indvar[i];
					delta = nsets*(dfa - d);
					nnm[0]=nn[0];
					nnm[1]=nn[1];
				}
				//cout<<"var="<<indvar[i]<<"   c="<<c<<"   d="<<d<<"    disval="<<disval<<"\n";
				if (debuglevel==41) {cout<<"var "<<indvar[i]<<"     ";for (int m=0;m<rf.nmodel;m++) cout<<"f["<<m<<"]="<<f[m]<<"   ";}
				if (debuglevel==41) {cout<<"    d="<<d<<"   dis="<<disval<<"   cut="<<cutval<<"\n";}
			}
		} else {
			//cout<<"     ATTENTION n1=1\n";
			disval=0.0;
			for (int i=0;i<nvar;i++) {
				for (int j=0;j<nsets;j++){vm[j].x=rf.stat[numset[j]][indvar[i]];}
				sort(&vm[0],&vm[nsets]);
				if (i==0) {
					deltamax=vm[nsets-1].x-vm[0].x;
					imax=indvar[i];
					cutval=0.5*(vm[nsets-1].x+vm[0].x);
				}
				else {
					if (deltamax<vm[nsets-1].x-vm[0].x) {
						deltamax=vm[nsets-1].x-vm[0].x;
						imax=indvar[i];
						cutval=0.5*(vm[nsets-1].x+vm[0].x);
					}
				}
			}
		}
		if (debuglevel==42) cout<<"imax="<<imax<<"   disval="<<disval<<"   cutval="<<cutval<<"    nvar="<<nvar<<"    nsets="<<nsets<<"\n-----------------------------------------------------\n";
		nsetG=0;nsetD=0;
		for (int j=0;j<nsets;j++) {
			if (rf.stat[numset[j]][imax]<cutval) nsetG++;
			if (rf.stat[numset[j]][imax]==cutval) {
				if (nsetD>=nsetG) nsetG++; else nsetD++;
			}
			if (rf.stat[numset[j]][imax]>cutval) nsetD++;
		}
		if ((nsetG<1)or(nsetD<1)){ 
			cout<<"nsets="<<nsets<<"    nsetG="<<nsetG<<"    nsetD="<<nsetD<<"   disval="<<disval<<"   cutval="<<cutval<<"   imax="<<imax<<"\n";
			//for (int j=0;j<nsets;j++) cout<<stat[numset[j]][imax]<<"   ";cout<<"\n";
		}
		numsetG = vector <int>(nsetG);
		numsetD = vector <int>(nsetD);
		nsetG=0;nsetD=0;
		//for (int j=0;j<nsets;j++) cout<<"("<<rt.model[numset[j]]<<"):"<<stat[numset[j]][imax]<<"  ";cout<<"\n";
		for (int j=0;j<nsets;j++) {
			//cout<<"indiv "<<numset[j]<<"   x="<<stat[numset[j]][imax]<<"  scen "<<rt.model[numset[j]];
			if (rf.stat[numset[j]][imax]<cutval) {
				numsetG[nsetG] = numset[j];//cout<<" -> à gauche\n";
				nsetG++;
			}  
			if (rf.stat[numset[j]][imax]==cutval) {
				if (nsetD>=nsetG){
					numsetG[nsetG] = numset[j];//cout<<" -> à gauche\n";
					nsetG++;
				} else {
					numsetD[nsetD] = numset[j];//cout<<" -> à droite\n";
					nsetD++;
				}
			} 
			if (rf.stat[numset[j]][imax]>cutval) {
					numsetD[nsetD] = numset[j];//cout<<" -> à droite\n";
					nsetD++;
			}
		}
		if (n1>1){
			if ((nnm[0]!=nsetG)or(nnm[1]!=nsetD)) {
				cout<<"imax="<<imax<<"\n";
				cout<<"nsetG="<<nsetG<<"   nnm[0]="<<nnm[0]<<"\n";
				cout<<"nsetD="<<nsetD<<"   nnm[1]="<<nnm[1]<<"\n";
				for (int j=0;j<nsets;j++) cout<<rf.stat[numset[j]][imax]<<" ("<< rf.model[numset[j]]<<")  ";cout<<" \n";
				exit(1);
			}
		}
		a.clear();b.clear();f.clear();vm.clear();
		return false;
	}

	void TreeC::ecris(int num) {
		cout<<"\nEcriture de l'arbre "<<num<<"         "<<nnodes<<" noeuds\n";
		for (int i=0;i<nnodes;i++) {
			cout<<"Noeud "<<i<<"   pere "<<node[i].pere;
			if (not node[i].terminal )	{cout<<"   ("<<header.statname[node[i].imax]<<"   filsG "<<node[i].filsG<<"   filsD "<<node[i].filsD<<"   nsets="<<node[i].nsets;
				cout<<"   disval="<<node[i].disval<<"\n";
			}
			else cout<<"   TERMINAL   scenario "<<node[i].model<<"\n";
			for (int j=0;j<node[i].nsets;j++)cout<<"("<<rf.model[node[i].numset[j]]<<"):"<<node[i].numset[j]<<"  ";cout<<"\n";
		}
		cout<<"Fin de l'arbre "<<num<<"\n";
	}
	
	int TreeC::infermodel(const vector <double>& stat) {
		int k=0;
		do {
			if (stat[node[k].imax]<node[k].cutval) k=node[k].filsG; else k=node[k].filsD;
		} while (not node[k].terminal);
		return node[k].model;
	}
	
	int TreeC::calprofondeur(const vector <double>& stat) {
		int k=0,n=0;
		do {
			if (stat[node[k].imax]<node[k].cutval) k=node[k].filsG; else k=node[k].filsD;
			n++;
		} while (not node[k].terminal);
		return n;
	}
	
	
	int TreeC::calprox(int n0, const vector <double>& stat,const vector <double>& statobs) {
	//cout<<"calprox  n0="<<n0<<"\n";
		int k=0,n=0;
		bool fin=false;
		while ((not fin)and(n<n0)) {
			if ((statobs[node[k].imax]<node[k].cutval) and (stat[node[k].imax]<node[k].cutval)) k=node[k].filsG;
			else if ((statobs[node[k].imax]>=node[k].cutval) and (stat[node[k].imax]>=node[k].cutval)) k=node[k].filsD;
			else fin=true;
			if (not fin) n++;//cout<<"n="<<n<<"\n";
		}
		return (n0-n);
	}
	
	void RFC::initrees(int seed) {
		int fmax;
		tree = vector <TreeC>(this->ntrees);
		for (int i=0;i<this->ntrees;i++) {
			cout<<"initialisation de l'arbre "<<i+1<<"\r";fflush(stdout);
			tree[i].wind = vector <double>(this->nsets);
			for (int j=0;j<this->nsets;j++) tree[i].wind[j]=1.0;
			tree[i].wstat = vector <double>(this->nstat);
			for (int j=0;j<this->nstat;j++) tree[i].wstat[j]=1.0;
			tree[i].nnodes = 2*this->nsets-1;
			tree[i].nsets = this->nsel;
			tree[i].nvar = this->nvar;
			tree[i].node = vector <NodeRC>(2*this->nsel);
			tree[i].mw.randinit(seed+i,3*(i+seed));
			tree[i].numset = vector <int>(this->nsel);
			tree[i].indsel = vector <int>(this->nsel);
			tree[i].score = vector <int>(this->nstat);
			//fmax = tree[i].mw.LiuChenSample(nsets,tree[i].wind,nsel, tree[i].indsel);
			//if (fmax>1)	cout<<"fmax="<<fmax<<"\n";
			tree[i].mw.samplewith(this->nsets,this->nsel,tree[i].indsel);
			//if (i==0) {for (int j=0;j<10;j++) cout<<tree[i].indsel[j]<<"   ";cout<<"\n";}
			for (int j=0;j<this->nsel;j++) {
				tree[i].numset[j]=tree[i].indsel[j];
			}
			//if (i==0) {for (int j=0;j<10;j++) cout<<tree[i].numset[j]<<"   ";cout<<"\n";}
			tree[i].varused = vector <bool>(this->nstat);
			for (int j=0;j<this->nstat;j++) tree[i].varused[j]=false;
		}
		//cout<<"fin de l'initialisation des arbres\n";
		cout<<"\n";
	}

	void RFC::dimimportance() {
		this->importance = vector< vector <double> > (this->nbootsamp);
		this->nimportance = vector< vector <int> > (this->nbootsamp);
	}
	/*void RFC::writetree(string filename, const ReftableC& rt) {
		
		
	}*/
	
	void RFC::growtrees(int rep) {
		//cout<<"debut de growtrees("<<rep<<")\n";
		int k,kk,ndone=0;
		importance[rep] = vector <double>(this->nstat);
		nimportance[rep] = vector <int>(this->nstat);
		for (int i=0;i<this->nstat;i++) {importance[rep][i]=0.0;nimportance[rep][i]=0;}
		//cout<<ntrees<<" arbres à construire dans growtrees\n";
#pragma omp parallel for shared(ndone) private(k,kk) if(multithread)
		for (int i=0;i<this->ntrees;i++) {
			//cout<<"debut de tree "<<i<<"\n";fflush(stdout);
			tree[i].node[0].nsets=tree[i].nsets;
			tree[i].node[0].nvar=tree[i].nvar;
			tree[i].node[0].npassages = 0;
			tree[i].node[0].pere = 0;
			tree[i].node[0].indvar = vector <int> (tree[i].nvar);
			//tree[i].fmax = tree[i].mw.LiuChenSample(rt.nstat,tree[i].wstat,tree[i].nvar,tree[i].node[0].indvar);
			tree[i].mw.resample(this->nstat,tree[i].nvar,tree[i].node[0].indvar);
			tree[i].node[0].numset = vector <int>(tree[i].nsets);
			for (int j=0;j<tree[i].nsets;j++) tree[i].node[0].numset[j]=tree[i].numset[j];
			//for (int m=0;m<10;m++) cout<<tree[i].node[0].numset[m]<<"  ";cout<<"\n";
			k=0;
			tree[i].fin=false;//cout<<"avant la boucle while\n";
			while (not tree[i].fin) {
				//cout<<k<<"\r";fflush(stdout);
				//cout<<" \nAVANT LE NOEUD "<<k<<"\n";
				//cout<<"Noeud k="<<k<<"   ";if (tree[i].node[k].terminal) cout<<"terminal\n";else cout<<"not terminal\n";
				tree[i].node[k].terminal=tree[i].node[k].getdisval(tree[i].mw);
				if (not tree[i].node[k].terminal){ //cout<<"noeud non terminal\n";
					tree[i].varused[tree[i].node[k].imax] = true;//cout<<"Noeud k="<<k<<"   imax="<<tree[i].node[k].imax<<"\n";
					importance[rep][tree[i].node[k].imax] +=tree[i].node[k].delta;
					nimportance[rep][tree[i].node[k].imax]++;
					tree[i].node[k].filsG=k+1;
					if (k==2*nsel) {
						cout<<"dépassement du nombre de noeuds 1 dans l'arbre "<<i<<"   kk="<<kk<<"\n";
						cout<<"k="<<k<<"\n";
						for (kk=0;kk<=30;kk++) cout<<"node="<<kk<<"  pere="<<tree[i].node[kk].pere<<"  filsG="<<tree[i].node[kk].filsG<<"  filsD="<<tree[i].node[kk].filsD<<"  n="<<tree[i].node[kk].npassages<<"\n";
						exit(1);
					}
					k++;//cout<<"création du noeud "<<k<<" dans la descente sur "<<2*nsel-1<<"\n";
					tree[i].node[k].npassages=0;
					tree[i].node[k].pere=k-1;
					tree[i].node[k].nsets=tree[i].node[tree[i].node[k].pere].nsetG;
					tree[i].node[k].nvar=tree[i].nvar;
					tree[i].node[k].numset = vector <int>(tree[i].node[k].nsets);
					for (int j=0;j<tree[i].node[k].nsets;j++) tree[i].node[k].numset[j]=tree[i].node[tree[i].node[k].pere].numsetG[j];
					tree[i].node[k].indvar = vector <int>(tree[i].nvar);
					//tree[i].fmax= tree[i].mw.LiuChenSample(rt.nstat,tree[i].wstat,tree[i].nvar,tree[i].node[k].indvar);
					tree[i].mw.resample(this->nstat,tree[i].nvar,tree[i].node[k].indvar);
					/*k++;
					tree[i].node[k].pere=k-2;//
					tree[i].node[k].nsets=tree[i].node[[tree[i].node[k].pere].nsetD;
					tree[i].node[k].numset = new int[tree[i].node[k].nsets];
					for (int j=0;j<tree[i].node[k].nsets;j++) tree[i].node[k].numset[i]=tree[i].node[tree[i].node[k].pere].numsetD[j];
					tree[i].node[k].indvar = new int[tree[i].nvar];
					tree[i].fmax = tree[i].mw.LiuChenSample(rt.nstat,tree[i].wstat,tree[i].nvar,tree[i].node[k].indvar);
					*/
					
					tree[i].node[k].npassages++;
					//cout<<"noeud k="<<k<<"   npassages="<<tree[i].node[k].npassages<<"\n";
					
				} else {//cout<<"noeud terminal\n";
					tree[i].node[k].model = rf.model[tree[i].node[k].numset[0]];
					kk=k;
					do {
						kk=tree[i].node[kk].pere;
						//cout<<"on remonte d'un cran  kk="<<kk<<"   npassages="<<tree[i].node[kk].npassages<<"\n";
					} while ((tree[i].node[kk].npassages==2)and(kk!=0));
					tree[i].fin= ((kk==0)and(tree[i].node[kk].npassages==1));
					if (not tree[i].fin){
						if (k==2*nsel){
							cout<<"dépassement du nombre de noeuds 2 dans l'arbre "<<i<<"   kk="<<kk<<"\n";
							cout<<"k="<<k<<"\n";
							for (kk=0;kk<=k;kk++) { 
								cout<<"node="<<kk<<"  pere="<<tree[i].node[kk].pere;
								if (not tree[i].node[kk].terminal){ 
									cout<<"  filsG="<<tree[i].node[kk].filsG<<" ("<<tree[i].node[kk].nsetG<<")";
									cout<<"  filsD="<<tree[i].node[kk].filsD<<" ("<<tree[i].node[kk].nsetD<<")  n="<<tree[i].node[kk].npassages<<"\n";
								}else cout<<"  terminal\n";
							}
							exit(1);
						}
						k++;//cout<<"création du noeud "<<k<<" apres remontée sur "<<2*nsel-1<<"\n";
						tree[i].node[k].npassages=0;
						tree[i].node[k].pere=kk;
						tree[i].node[kk].filsD=k;
						tree[i].node[k].nsets=tree[i].node[tree[i].node[k].pere].nsetD;
						tree[i].node[k].nvar=tree[i].nvar;
						tree[i].node[k].numset = vector <int>(tree[i].node[k].nsets);
						for (int j=0;j<tree[i].node[k].nsets;j++) tree[i].node[k].numset[j]=tree[i].node[tree[i].node[k].pere].numsetD[j];
						tree[i].node[k].indvar = vector <int>(tree[i].nvar);
						//tree[i].fmax = tree[i].mw.LiuChenSample(this->nstat,tree[i].wstat,tree[i].nvar,tree[i].node[k].indvar);
						tree[i].mw.resample(this->nstat,tree[i].nvar,tree[i].node[k].indvar);
						tree[i].node[kk].npassages++;
						tree[i].node[k].npassages++;
					}
				}
			}
			tree[i].nnodes=k+1;
			//tree[i].ecris(rt,i);
			for (int m=0;m<=k;m++) {
				if (not tree[i].node[m].terminal) {
					tree[i].node[m].numsetD.clear();
					tree[i].node[m].numsetG.clear();
				}
			}
			ndone++;
			cout<<"construction de l'arbre "<<ndone<<"\r";fflush(stdout);
			//cout<<"fin de tree "<<i+1<<"   "<<tree[i].nnodes<<" noeuds\n";fflush(stdout);
		}
		cout<<"\n";
	}
	
	int RFC::bestmodel(int nscen,vector <double>& vote,const vector <double>& stat) {
		double sum;
		int infmodel,best,i;
		for (i=0;i<nscen;i++) vote[i]=0.0;
		for (i=0;i<ntrees;i++) {
			infmodel=tree[i].infermodel(stat);
			vote[infmodel]+=1.0;
		}
		best=0;
		for (i=1;i<nscen;i++) if (vote[i]>vote[best]) best=i;
		return best;
	}
	
	int RFC::bestmodel2(int k, int nscen,const vector <double>& stat) {
		int infmodel,best,i;
		vector <int>vote;
		bool participe;
		vote = vector <int>(nscen);
		for (i=0;i<nscen;i++) vote[i]=0;
		for (i=0;i<ntrees;i++) {
			participe=false;
			for (int j=0;j<tree[i].nsets;j++) {
				participe=(k==tree[i].numset[j]);
				if (participe) break;
			}
			if (not participe){
				infmodel=tree[i].infermodel(stat);
				vote[infmodel]++;
			}
		}
		best=0;
		for (i=1;i<nscen;i++) if (vote[i]>vote[best]) best=i;
		vote.clear();
		return best;
	}
	
	void RFC::infermodel() {
		int best;
		vector <double> vote;
		vector <double> statobs;
		statobs = vector <double>(header.nstat);
		for (int i=0;i<header.nstat;i++) statobs[i] = (double)header.stat_obs[i];
		vote = vector <double>(rt.nscen);
		best=bestmodel(rt.nscen,vote,statobs);
		cout<<"Estimation du modèle pour les données observées\nScénario            ";
		for (int i=0;i<rt.nscen;i++) cout<<"  "<<i+1<<"    ";cout<<"\n"<<"Proportion de votes ";
		for (int i=0;i<rt.nscen;i++) cout<< std::fixed<<setw(4)<<setprecision(3)<<vote[i]<<"  "; cout<<"\n";
		cout<<"Le scénario "<<best+1<<" l'emporte\n";
		fout<<"\nInferred scenario for observed data\nScenario              ";
		for (int i=0;i<rt.nscen;i++) fout<<"  "<<i+1<<"   ";fout<<"\n"<<"Proportion of votes ";
		for (int i=0;i<rt.nscen;i++) fout<< std::fixed<<setw(4)<<setprecision(3)<<vote[i]<<"  "; fout<<"\n";
		fout<<"Scenario "<<best+1<<" is the winner\n";
		fout<<"\nScenario  number of trees\n";
		for (int i=0;i<rt.nscen;i++) fout<<"   "<<i+1<<"        "<<(int)vote[i]<<"\n";fout<<"\n";
		vote.clear();
		statobs.clear();
	}
	
	double RFC::training_accuracy() {
		cout<<"début de training accuracy\n";
		double sfv,ppe;
		int j,k,n,sum,sumt=0,correct=0,ndone=0;
		bool deja;
		vector <vector <int> >vote;
		vector <vector <double> >fvote;
		vector <int>truemodel;
		vector <int>infmodel;
		vote  = vector <vector<int> >(this->nmodel);
		fvote = vector <vector<double> >(this->nmodel);
		truemodel = vector <int>(this->nsets);
		infmodel = vector <int>(this->nsets);
		for (int i=0;i<rt.nscen;i++) {
			vote[i] = vector <int>(this->nmodel);
			fvote[i] = vector <double>(this->nmodel);
			for (int j=0;j<rt.nscen;j++) vote[i][j]=0;
		}
		//cout<<"avant la boucle multithread\n";
#pragma omp parallel for shared(truemodel,infmodel,ndone) if(multithread)
		for (int i=0;i<nsets;i++) {
			truemodel[i] = this->model[i];
			infmodel[i] = bestmodel2(i,this->nmodel,this->stat[i]);
			ndone++;
			if (((ndone+1) % 100)==0) cout<<"test de l'individu "<<ndone+1<<" / "<<nsets<<"                      \r";fflush(stdout);
			//cout<<"trumodel["<<i<<"] = "<<truemodel[i]<<"     infmodel["<<i<<"] = "<<infmodel[i]<<"\n";
		}
		cout<<"\n";
		//cout<<"après la boucle multithread\n";
		for (int i=0;i<nsets;i++) vote[truemodel[i]][infmodel[i]]++;
		cout<<"\nTraining accuracy\n";
		//cout<<"En données brutes\n True\\Estim.\n";
		//cout<<"    ";for (int i=0;i<rt.nscen;i++) cout<<"    "<<i+1<<"   ";cout<<"\n";
		fout<<"\nTraining accuracy\n";
		fout<<"Raw data\n True\\Estim.\n";
		fout<<"    ";for (int i=0;i<this->nmodel;i++) fout<<"    "<<i+1<<"   ";fout<<"\n";
		/*for (int i=0;i<rt.nscen;i++) {
			cout<< std::fixed<<setw(3)<<i+1<<" ";
			for (int j=0;j<rt.nscen;j++) cout<<setw(6)<<setprecision(0)<<vote[i][j]<<"  ";cout<<"\n";
		}*/
		for (int i=0;i<this->nmodel;i++) {
			fout<<std::fixed<<setw(3)<<i+1<<" "<< std::fixed;
			for (int j=0;j<rt.nscen;j++) fout<<setw(6)<<setprecision(0)<<vote[i][j]<<"  ";fout<<"\n";
		}
		for (int i=0;i<this->nmodel;i++) {
			sum=0;for (int j=0;j<this->nmodel;j++) sum +=vote[i][j];
			sumt +=sum;
			correct +=vote[i][i];
			for (int j=0;j<this->nmodel;j++) fvote[i][j] =(double)vote[i][j]/(double)sum;
		}
		//cout<<"Somme totale = "<<sumt<<"   (attendus : "<<nsets<<")\n";
		//cout<<"\nEn données normalisées (somme de chaque ligne=1)\n True\\Estim.\n";
		//cout<<"   ";for (int i=0;i<rt.nscen;i++) cout<<"    "<<i+1<<"  ";
		//cout<<" class.error\n";
		for (int i=0;i<this->nmodel;i++) {
			//cout<<std::fixed<<setw(3)<<i+1<<"  "<< std::fixed;
			//for (int j=0;j<rt.nscen;j++) cout<<setw(5)<<setprecision(3)<<fvote[i][j]<<"  ";
			sfv=0.0;for (int j=0;j<rt.nscen;j++) if (j!=i) sfv+=fvote[i][j];
			//cout<<setw(8)<<setprecision(4)<<sfv<<"\n";
		}
		fout<<"Sum = "<<sumt<<"   (expected : "<<nsets<<")\n";
		fout<<"\nNormalized data (sum of each line = 1)\n True\\Estim.\n";
		fout<<"   ";for (int i=0;i<this->nmodel;i++) fout<<"    "<<i+1<<"  ";fout<<" class.error\n";
		for (int i=0;i<this->nmodel;i++) {
			fout<<std::fixed<<setw(3)<<i+1<<"  "<< std::fixed;
			for (int j=0;j<rt.nscen;j++) fout<<setw(5)<<setprecision(3)<<fvote[i][j]<<"  ";
			sfv=0.0;for (int j=0;j<this->nmodel;j++) if (j!=i) sfv+=fvote[i][j];
			fout<<setw(8)<<setprecision(4)<<sfv<<"\n";
		}
		ppe = (double)(sumt-correct)/(double)sumt;
		cout<<"Prior predictive error : "<<fixed<<setw(6)<<setprecision(3)<<ppe<<"\n";
		fout<<"\nPrior predictive error : "<<fixed<<setw(6)<<setprecision(3)<<ppe<<"\n";
		for (int i=0;i<rt.nscen;i++) vote[i].clear();
		for (int i=0;i<rt.nscen;i++) fvote[i].clear();
		vote.clear();
		fvote.clear();
		truemodel.clear();
		infmodel.clear();
		return ppe;
	}
	
	void RFC::var_importance3(int rep) {
		//cout<<"\nRecherche des stats les plus informatives\n";
		int ns;
		ns=nstatclass;if (ns>this->nstat) ns=this->nstat;
		vector <VMD> vd;
		vd =  vector <VMD>(this->nstat);
		for (int i=0;i<this->nstat;i++) {
			if (nimportance[rep][i]>0) vd[i].x=importance[rep][i]/ntrees /*/(double)nimportance[i]*/;else vd[i].x=0.0;
			vd[i].name = this->statname[i];//cout<<"vd["<<i<<"].name = "<<vd[i].name<<"\n";
			for (int j=vd[i].name.length();j<12;j++) vd[i].name +=" ";
		}
		//cout<<"avant le sort\n";
		sort(&vd[0],&vd[this->nstat]);
		if (ns==nstatclass) cout<<" Classement des "<<nstatclass<<" stats les plus informatives:\n";
		else cout<<"\n\n Classement des stats selon leur informativité:\n";
		for (int i=0;i<ns;i++) {
			cout<<fixed<<setw(4)<<i+1<<"  "<<vd[i].name<<"   "<<fixed<<setw(10)<<setprecision(2)<<vd[i].x;
			cout<<"   ("<<fixed<<setw(6)<<setprecision(2)<<vd[i].x/vd[0].x*100.0<<")\n";
		}
		fout<<"\nMost informative summary statistics\n";
		for (int i=0;i<ns;i++) {
			fout<<fixed<<setw(4)<<i+1<<"  "<<vd[i].name<<"   "<<fixed<<setw(10)<<setprecision(2)<<vd[i].x;
			fout<<"   ("<<fixed<<setw(6)<<setprecision(2)<<vd[i].x/vd[0].x*100.0<<")\n";
		}
	}

	void RFC::readstat(bool LD) {
		cout<<"\nLecture des données\n";
		int nscenOK=0,nparamax=0,iscen,bidon;
        enregC enr;
		bool scenOK;
		this->nmodel=rt.nscenchoisi;cout<<"nmodel="<<this->nmodel<<"\n";
		this->model = vector <int>(this->nsets);
		//for (int i=0;i<this->nmodel;i++) this->model[i]=rt.scenchoisi[i];
		this->nstat=rt.nstat;cout<<"nstat = "<<this->nstat<<"   nsets="<<this->nsets<<"\n";
        for (int i=0;i<rt.nscen;i++) if (rt.nparam[i]>nparamax) nparamax=rt.nparam[i];
        enr.param = vector <float>(nparamax);
        enr.stat  = vector <float>(this->nstat);
		this->stat = vector <vector <double> >(this->nsets);
//LECTURE DE LA TABLE DE REFERENCE
		rt.filename=path+"reftableRF.bin";
        rt.openfile2();bidon=0;cout<<"ouverture de "<<rt.filename<<"\n";
		while ((nscenOK<this->nsets)and(bidon==0)) {
			bidon=rt.readrecord(&enr);//cout<<"nscenOK="<<nscenOK<<"   bidon="<<bidon<<"\n";
			if (bidon==0){
				scenOK=false;iscen=0;//cout<<"numscen="<<enr.numscen<<"    iscen="<<iscen<<"\n";
				while((not scenOK)and(iscen<this->nmodel)) {
					scenOK=(enr.numscen==rt.scenchoisi[iscen]);
					iscen++;
				}
				if (scenOK) {
					this->stat[nscenOK] = vector <double>(this->nstat);
					for (int j=0;j<this->nstat;j++) this->stat[nscenOK][j] = (double)enr.stat[j]; 
					this->model[nscenOK] = enr.numscen-1;
					//if (nscenOK<10) cout<<"scenOK=true   enr.numscen="<<this->model[nscenOK]<<"   nscenOK="<<nscenOK<<"\n";
					nscenOK++;	
				}
			}
		}
		rt.closefile();
		this->nsets=nscenOK;
		cout<<"nsets = "<<this->nsets<<"\n";
//LECTURE DES STATS OBSERVEES
		this->statobs = vector <double>(this->nstat);
		for (int j=0;j<this->nstat;j++) this->statobs[j] = (double)header.stat_obs[j];

		this->statname = vector <string>(this->nstat);
		for (int i=0;i<this->nstat;i++) this->statname[i]=header.statname[i];
		
//AJOUT CONDITIONNEL DES LD
		if (LD) {
			cout<<"\nAnalyse Discriminante pour l'ajout des LD...\n";
			long double *w,**X;
			int *mod;
			mod = new int[this->nsets];for (int i=0;i<this->nsets;i++) mod[i]=this->model[i];
			vector <long double>statpiv;
			resAFD afd;
			w = new long double[this->nsets];
			for (int i=0;i<this->nsets;i++) w[i] = 1.0;
			X = new long double*[this->nsets];
			for (int i=0;i<this->nsets;i++) {
				X[i] = new long double[this->nstat];
				for (int j=0;j<this->nstat;j++) X[i][j] = (long double) this->stat[i][j];
			}
			afd = AFD(this->nsets,this->nstat,mod,w,X,1.0);
			//cout<<"afd.nlambda = "<<afd.nlambda<<"\n";
			cout<<"Fin de l'analyse discriminante\n";
			for (int i=0;i<this->nsets;i++){this->stat[i].resize(this->nstat+afd.nlambda-1);}
		//calcul des LD sur les jeux simulés
			statpiv = vector <long double>(afd.nlambda);
			for (int i=0;i<this->nsets;i++) {
				for (int j=0;j<afd.nlambda-1;j++){
					statpiv[j] =0.0;
					for (int k=0;k<this->nstat;k++) statpiv[j] += (X[i][k] - afd.moy[k])*afd.vectprop[k][j];
					this->stat[i][nstat+j] = (double)statpiv[j];
				}
			}
		//delete des vecteurs et tableaux
			for (int i=0;i<this->nsets;i++) delete []X[i];delete []X;
			delete []w;delete []mod;
		//calcul des LD sur le jeu observé
			this->statobs.resize(this->nstat+afd.nlambda-1);
			for (int j=0;j<afd.nlambda-1;j++){
				statpiv[j] =0.0;
				for (int k=0;k<this->nstat;k++) statpiv[j] += ((long double)this->statobs[k] - afd.moy[k])*afd.vectprop[k][j];
				this->statobs[nstat+j] = (double)statpiv[j];
			}
			statpiv.clear();
		//ajout des noms de LD
			this->statname = vector <string>(this->nstat);
			for (int i=0;i<this->nstat;i++) this->statname[i]=header.statname[i];
			this->statname.resize(this->nstat+afd.nlambda-1);
			for (int i=0;i<afd.nlambda-1;i++) this->statname[this->nstat+i]="LD"+IntToString(i+1);
			this->nstat+=afd.nlambda-1;
			cout<<"Fin de l'ajout des composantes LD\n";
		} else cout<<"Pas d'ajout des LD";
	}
	
	void RFC::calposterior(int seed) {
		cout<<"\nCalcul de la Posterior Predictive Error\n";
	//Calcul de la profondeur de l'observation dans chaque arbre
		vector <int> profobs;
		profobs = vector<int>(this->ntrees);
		for (int i=0;i<this->ntrees;i++) {
			profobs[i]=this->tree[i].calprofondeur(this->statobs);
			//cout<<"Profondeur de l'observé dans l'arbre "<<i+1<<" = "<<profobs[i]<<"\n";
		}
		//cout<<"Calcul de la proximité des jeux simulés\n";
	//Calcul de la proximité des jeux simulés
		VMC *prox;
		prox = new VMC[this->nsel];
		for (int i=0;i<this->nsel;i++) {
			prox[i].ind = i;
			prox[i].x=0;
			for (int j=0;j<this->ntrees;j++) prox[i].x += (double)this->tree[j].calprox(profobs[j],this->stat[i],this->statobs);
			//cout<<"prox["<<i<<"].ind = "<<prox[i].ind<<"     x = "<<prox[i].x<<"\n";
		}
		//cout<<"Classement des plus proches voisins\n";
		sort(&prox[0],&prox[this->nsel]);
		int ntot=nvoisins*nj;
		enreg = new enregC[this->nsel];
		rt.openfile2();
		int ii,ns,nphistarOK;
		double **stata;
		vector <vector <double> > statb; 
		for (int p=0;p<this->nsel;p++) {
			//cout<<"lecture de l'enregistrement "<<p;
			enreg[p].stat = vector <float>(this->nstat);
			enreg[p].param = vector <float>(rt.nparamax);
			enreg[p].numscen = rt.readparam(enreg[p].param);
			//cout<<"   scénario "<<enreg[p].numscen<<"\n";
		}
		rt.closefile(); 
		//cout<<"fin de la lecture des "<<nsel<<" enregistrements de la reftable\n";
		statb = vector < vector <double> >(ntot);
		for (int p=0;p<nvoisins*nj;p++) {
			statb[p] = vector<double>(this->nstat);
		}
		int kk=0;
		for (int iscen=0;iscen<rt.nscen;iscen++) {
			//cout<<rt.nparam[iscen]<<"\n";
			nphistarOK=0;
			for (int p=0;p<nvoisins;p++) if (enreg[prox[p].ind].numscen-1==iscen) nphistarOK++;
			
			if (nphistarOK>0){
				//cout<<"scenario "<<iscen+1<<"   nphistarOK = "<<nphistarOK<<"\n";
				phistarOK = new long double*[nphistarOK*nj];
				stata = new double*[nphistarOK*nj];
				ns=0;
				for (int p=0;p<nvoisins;p++) if (enreg[prox[p].ind].numscen-1==iscen) {
					for (int i=0;i<nj;i++) {
						ii=ns*nj+i;
						phistarOK[ii] = new long double[rt.nparam[iscen]];
						stata[ii] = new double[rt.nstat];
						for (int j=0;j<rt.nparam[iscen];j++) phistarOK[ii][j]=enreg[prox[p].ind].param[j];
					}
					ns++;
				}
				//for (int j=0;j<rt.nparam[iscen];j++) cout<<phistarOK[0][j]<<"\n";
				//cout<<"pperror ns="<<ns*nj<<" sur "<<nvoisins*nj<<" pour le scenario "<<iscen+1<<"\n";
				//cout<<"avant le dosimulstat\n";
				ps.dosimulstat(0,ns*nj,false,multithread,iscen+1,seed,stata);
				//cout<<"apres le dosimulstat\n";
				ns=0;
				for (int p=0;p<nvoisins;p++) if (enreg[prox[p].ind].numscen-1==iscen) {
					for (int i=0;i<nj;i++) {
						ii=ns*nj+i;					
						for (int j=0;j<rt.nstat;j++) statb[kk][j] = stata[ii][j];
						delete [] phistarOK[ii];delete [] stata[ii];
						//cout<<"p="<<p<<"   ii="<<ii<<"   kk="<<kk<<"\n";
						kk++;
					}
					ns++;
				}
				delete [] phistarOK;delete [] stata;
			}
		}
		int nerror=0;
		for (int p=0;p<nvoisins;p++) {
			if (bestmodel2(p/nj,rt.nscen,statb[p]) != enreg[prox[p].ind].numscen-1) nerror++;
		}
		double pperror;
		pperror = (double)nerror/(double)(nvoisins*nj);
		cout<<"\nPosterior predictive error = "<<pperror<<"\n";
		fout<<"\nPosterior predictive error = "<<pperror<<"\n";
	}
	
	void dorandfor(string opt,int seed) {
		cout<<"\nDébut de dorandfor\n";
		double clock_zero=0.0,debut,duree;
		debut=walltime(&clock_zero);
        time_t rawtime;
        time ( &rawtime );
        struct tm * timeinfo;
        timeinfo = localtime ( &rawtime );
		int nrecpos,repmin;
		double ppe,ppemin;
		bool LD=false,flagk=false,flago=false;
		string nomfiresult,s0,s1;
		nomfiresult = path + ident + "_randomforest.txt";
		progressfilename = path + ident + "_progress.txt";
		cout<<"options : "<<opt<<"\n";
		vector<string> ss;
		vector<string> ss1;  
		splitwords(opt,";",ss);
		rf.ntrees = 500;
		for (int i=0;i<ss.size();i++) {
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);
			if (s0=="s:"){
				splitwords(s1,",",ss1);rt.nscenchoisi=ss1.size();
                rt.scenchoisi = new int[rt.nscenchoisi];
                for (int j=0;j<rt.nscenchoisi;j++) rt.scenchoisi[j] = atoi(ss1[j].c_str());
                nrecpos=0;for (int j=0;j<rt.nscenchoisi;j++) nrecpos +=rt.nrecscen[rt.scenchoisi[j]-1];
				cout <<"scenarios à tester : ";
                for (int j=0;j<rt.nscenchoisi;j++) {cout<<rt.scenchoisi[j]; if (j<rt.nscenchoisi-1) cout <<",";}cout<<"\n";
				ss1.clear();
			} else if (s0=="n:") {
                rf.nsets=atoi(s1.c_str());
                cout<<"nombre total de jeux de données considérés (tous scénarios confondus)= "<<rf.nsets<<"\n";
            } else if (s0=="d:") {
                LD=true;
                cout<<"ajout des linear discriminant scores\n";
            } else if (s0=="t:") {
                rf.ntrees =atoi(s1.c_str());
                cout<<"nombre d'arbres ' = "<<rf.ntrees<<"\n";
            } else if (s0=="b:") {
				splitwords(s1,",",ss1);rf.nbootsamp=ss1.size();
                rf.bootsamp = vector <int>(rf.nbootsamp);
                for (int j=0;j<rf.nbootsamp;j++) rf.bootsamp[j] = atoi(ss1[j].c_str());
				cout <<"taille des échantillons pour bootstrap : ";
                for (int j=0;j<rf.nbootsamp;j++) {cout<<rf.bootsamp[j]; if (j<rf.nbootsamp-1) cout <<",";}cout<<"\n";
				ss1.clear();
            } else if (s0=="k:") {
				rf.nvar=atoi(s1.c_str());
				flagk=true;
            } else if (s0=="o:") {
				rf.nstatclass=atoi(s1.c_str());
				cout<<"affichage des "<<rf.nstatclass<<" statistiques les plus efficaces\n";
				flago=true;
			}
				
		}
		cout<<"fin de l'analyse des options\n\n";
		rf.readstat(LD);
		if (not flagk)rf.nvar=(int)sqrt(rf.nstat);
		if (not flago)rf.nstatclass=30;
		fout.open(nomfiresult.c_str());
        fout<<"DIYABC :                 Search of the best scenario through Random Forest                         "<<asctime(timeinfo)<<"\n";
        fout<<"Data file       : "<<header.datafilename<<"\n";
        fout<<"Reference table : "<<rt.filename<<"\n";
        fout<<"Total number of simulated data sets : "<<rf.nsets<<"\n";
        if (LD) fout<<"Linear discriminant scores have been added to summary statistics\n";
        fout<<"The forest includes "<<rf.ntrees<<" trees\n";
        fout<<"Calibration has been performed with trees containing ";
        for (int j=0;j<rf.nbootsamp;j++) {fout<<rf.bootsamp[j]; if (j<rf.nbootsamp-1) fout <<",";} fout<<" data sets\n";
        fout<<"Total number of summary statistics : "<<rf.nstat<<"    Number drawn at random at each node : "<<rf.nvar<<"\n\n";
		fout<<"*************************Calibration**************************\n\n";
		ppemin = 1.0;
		rf.dimimportance();
		for (int rep=0;rep<rf.nbootsamp;rep++){
			rf.nsel = rf.bootsamp[rep];
			cout<<"\nrep="<<rep+1<<"    nsel="<<rf.nsel<<"\n";
			cout<<rf.nstat<<" statistiques résumées en tout. Tirage de "<<rf.nvar<<" stat à chaque noeud\n";
			cout<<rf.ntrees<<" arbres à construire avec "<<rf.nsel<<" enregistrements par arbre\n";
			//fout<<rf.nstat<<" statistiques résumées en tout. Tirage de "<<rf.nvar<<" stat à chaque noeud\n";
			//fout<<rf.ntrees<<" arbres à construire avec "<<rf.nsel<<" enregistrements par arbre\n";
			fout<<"-----------Trees with "<<rf.bootsamp[rep]<<" data sets-------------\n";
			rf.initrees(seed);//cout<<"apres initrees\n";
			rf.growtrees(rep);cout<<"......c'est fait.\n";
			rf.infermodel();
			ppe=rf.training_accuracy();
			if (ppe<=ppemin-0.001) {ppemin=ppe;repmin=rep;rfmin=rf;}
			rf.tree.clear();
			fout<<"-----------------------------------------------------------\n";
		}
		cout<<"\nRésultat de la calibration avec "<<rf.nsets<<" : nsel="<<rf.bootsamp[repmin]<<"  (prior predictive error = "<<ppemin<<")\n";
		fout<<"\nResults of calibration with  "<<rf.nsets<<" overall data sets : \n";
		fout<<"Minimum prior predictive error = "<<ppemin<<" obtained with trees of "<<rf.bootsamp[repmin]<<" data sets\n";
		rfmin.calposterior(seed);
		rf.var_importance3(repmin);
		
		duree=walltime(&debut);
		cout<<"\n\ndurée totale ="<<TimeToStr(duree)<<"\n";
		fout<<"\n\nTotal duration ="<<TimeToStr(duree)<<"\n";
		fout.close();
	}
