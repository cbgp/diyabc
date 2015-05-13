/*
 * randforest.h
 *
 *  Created on: 28 février 2014
 *      Author: jmcornuet
 */

#ifndef RANDFOREST_H_
#define RANDFOREST_H_

#include "randomgenerator.h"
#include <string>
#include <iostream>
#include <vector>
#include "reftable.h"
#include "particleset.h"

struct VMC {
	int ind;
	double x;
};

struct VMD {
	std::string name;
	double x;
};

class NodeRC
 {
public:
	int pere,filsG,filsD,nvar,nsets,npassages,nsetG,nsetD,model,imax;
	double cutval,disval,disval2,delta;
	bool terminal;
	vector <int>indvar;
	vector <int>numset;
	vector <int>numsetG;
	vector <int>numsetD;
	bool getdisval(MwcGen& mw);
	double caldisval(int nscen, int nsets, const vector<VMC>&vm, double val, vector <int>& nn);	
	~NodeRC() {
		if (not indvar.empty()) indvar.clear();
		if (not numset.empty()) numset.clear();
		if (not numsetG.empty()) numsetG.clear();
		if (not numsetD.empty()) numsetD.clear();
	}
		NodeRC & operator= (NodeRC  const & source);
};

class TreeC 
{
public:
	int nnodes,nsets,nvar,fmax;
	MwcGen mw;
	bool fin;
	
	vector <int> numset;
	vector <int> indsel;
	vector <int> score;
	vector <int> index;
	vector <NodeRC> node;
	vector <double> wind;
	vector <double> wstat;
	vector <bool> varused;
	
	int infermodel(const vector <double>& stat);
	void ecris(int num);
	int calprofondeur(const vector <double>& stat);
	int calprox(int n0, const vector <double>& stat, const vector <double>& statobs);
	
	~TreeC() {
		if (not numset.empty()) numset.clear();
		if (not indsel.empty()) indsel.clear();
		if (not score.empty()) score.clear();
		if (not index.empty()) index.clear();
		if (not node.empty()) node.clear();
		if (not wind.empty()) wind.clear();
		if (not wstat.empty()) wstat.clear();
		if (not varused.empty()) varused.clear();
	}
	TreeC & operator= (TreeC  const & source);
};

class RFC
{
public:
	int ntrees,ntot,nsets,nstat,nmodel,nvar,nsel,nstatclass,nbootsamp;
	
	vector <int>model;
	vector <int>indsel;
	vector <vector <int> >nimportance;
	vector <vector <double> > vote;
	vector <double>varimp;
	vector <vector <double> > stat;
	vector <vector <double> > importance;
	vector <TreeC> tree;
	vector <double> statobs;
	vector <int> bootsamp;
	vector <string> statname;
	
	void initrees(int seed);
	void growtrees(int rep);
	double training_accuracy();
	void infermodel();
	int bestmodel(int nmod,vector <double>& vote, const vector <double>& stat);	
	int bestmodel2(int k,int nmod,const vector <double>& stat);	
	void var_importance();
	void var_importance2();
	void var_importance3(int rep);
    void dimimportance();
	void readstat(bool LD);
	void calposterior(int seed); 
	
	~RFC(){
		if (not model.empty()) model.clear();
		if (not indsel.empty()) indsel.clear();
		if (not nimportance.empty()) nimportance.clear();
		if (not vote.empty()) vote.clear();
		if (not varimp.empty()) varimp.clear();
		if (not stat.empty()) stat.clear();
		if (not importance.empty()) importance.clear();
		if (not tree.empty()) tree.clear();
		if (not statobs.empty()) statobs.clear();
		if (not bootsamp.empty()) bootsamp.clear();
		if (not statname.empty()) statname.clear();
	}
	RFC & operator= (RFC  const & source);
};
void dorandfor(std::string opt,  int seed);


#endif /* RANDFOREST_H_ */