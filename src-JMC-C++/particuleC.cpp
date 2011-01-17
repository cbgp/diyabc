/*
 * particuleC.cpp
 *
 *  Created on: 23 sept. 2010
 *      Author: cornuet
 */

#include "data.cpp"
#include "randomgenerator.cpp"
#include <vector>
#include <iostream>
#include <complex>
#include <string>
using namespace std;
#define MISSING -9999


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
 *
 */

int arrondi(double a) {return (int)(a + 0.5);}

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

struct PriorC
{
	string loi;
	double mini,maxi,mean,sdshape;
	int ndec;
	bool constant;
};

struct ConditionC
{
	string param1,param2,operateur;

};

struct StatC
{
	int cat,samp,samp1,samp2,group;
	double val;
};

struct EventC
{
	int action;
    int pop,pop1,pop2,sample,Ne,time;
    double admixrate;
    int numevent0;
    string stime,sNe,sadmixrate;

};

struct Ne0C
{
	int val;
	string name;
};

struct HistParameterC
{
	string name;
	int category;
	double value;
	PriorC prior;
};
struct LocusGroupC
{
	int *loc,nloc;           //numeros des locus du groupe
	double p_fixe,gams;
	double musmoy,mutmoy,Pmoy,snimoy;
	double k1moy,k2moy;
	int  type,mutmod;
	PriorC priormusmoy,priork1moy,priork2moy,priormusloc,priork1loc,priork2loc;
	PriorC priormutmoy,priorPmoy,priorsnimoy,priormutloc,priorPloc,priorsniloc;
};

// vector<vector<int>> essai[x][];

struct SequenceBitC
{
    /* action = 0 (coal), 1 (merge), 2 (split), 3 (adsamp)
	*/
	int action,pop,pop1,pop2,sample;
    int N,t0,t1;
    double admixrate;

};

struct NodeC
{
	int pop,sample,state;
    double height;
	string dna;
};

struct BranchC
{
	int bottom,top,nmut;
	double length;
};

struct GeneTreeC
{
	NodeC *nodes;
	BranchC *branches;
	int nmutot,nnodes,nbranches,ngenes;
};

struct ParticleC
{
	LocusC *locuslist;
	LocusGroupC *grouplist;
	DataC  data;
	MissingHaplo *mhap;
	MissingNuc   *mnuc;
	EventC *eventlist;
	SequenceBitC *seqlist;
	GeneTreeC *gt;
	StatC *stat;
	MwcGen mw;
	Ne0C *ne0list;
	HistParameterC *parameterlist;
	int *paramvar;
	bool dnatrue;
	int npart,nloc,ngr,nevent,nn0,nparam,nparamvar,nseq,popmax,nstat,nsample,*nind,**indivsexe,scenario;
	int *time_sample;
	double matQ[4][4];

	int calploidy(int loc, int sa,int i) {
		if ((this->locuslist[loc].type == 0)  //AUTOSOMAL DIPLOID
				or((this->locuslist[loc].type == 2)and(this->data.indivsexe[sa][i] == 2))) //X-LINKED + FEMALE
					{return 2;}
		else if ((this->locuslist[loc].type == 3)and(this->data.indivsexe[sa][i] == 2))  //Y-LINKED + FEMALE
					{return 0;}
		else 		{return 1;}   //AUTOSOMAL HAPLOID or X-LINKED MALE or MITOCHONDRIAL

	}

	GeneTreeC copytree(GeneTreeC source) {
		GeneTreeC dest;
		dest.nnodes = source.nnodes;
		dest.ngenes = source.ngenes;
		dest.nbranches = source.nbranches;
		dest.branches = new BranchC[dest.nbranches];
		dest.nodes = new NodeC[dest.nnodes];
		for (int b=0;b<dest.nbranches;b++) {
			dest.branches[b].top = source.branches[b].top;
			dest.branches[b].bottom = source.branches[b].bottom;
			dest.branches[b].length = source.branches[b].length;
		}
		for (int n=0;n<dest.nnodes;n++) {
			dest.nodes[n].pop = source.nodes[n].pop;
			dest.nodes[n].height = source.nodes[n].height;
			dest.nodes[n].sample = source.nodes[n].sample;
		}
		return dest;
	}

	void deletetree(GeneTreeC tree) {
		delete [] tree.nodes;
		delete [] tree.branches;
	}

	double drawfromprior(PriorC prior) {
		double r;
		if (prior.mini==prior.maxi) return prior.mini;
		if (prior.loi=="UN") return this->mw.gunif(prior.mini,prior.maxi);
		if (prior.loi=="LU") return this->mw.glogunif(prior.mini,prior.maxi);
		if (prior.loi=="NO") {
			do {r = this->mw.gnorm(prior.mean,prior.sdshape);}
			while ((r<prior.mini)or(r>prior.maxi));
			return r;
		}
		if (prior.loi=="LN") {
			do {r = this->mw.glognorm(prior.mean,prior.sdshape);}
			while ((r<prior.mini)or(r>prior.maxi));
			return r;
		}
		if (prior.loi=="GA") {
			do {r = this->mw.ggamma3(prior.mean,prior.sdshape);}
			while ((r<prior.mini)or(r>prior.maxi));
			return r;
		}
		return -1.0;
	}

	double param2val(string paramname){
		int ip;
		for (ip=0;ip<this->nparam;ip++) {if (paramname==this->parameterlist[ip].name) break;}
		if (this->parameterlist[ip].value<0.0) this->parameterlist[ip].value = this->drawfromprior(this->parameterlist[ip].prior);
		return this->parameterlist[ip].value;

	}
	double getvalue(string line) {
		double result;
		bool fin=false;
		string operateur,plus="+",minus="-";
		unsigned int posign,posplus,posminus;
		posplus=line.find('+');posminus=line.find('-');
		if ((posplus>line.size())and(posminus>line.size())) {return  this->param2val(line);}
		else {if ((posplus>=0)and(posminus>line.size())){posign=posplus;}
			else {if ((posminus>=0)and(posplus>line.size())){posign=posminus;}
				else {if (posplus<posminus) {posign=posplus;} else {posign=posminus;}}}
		}
		result=this->param2val(line.substr(0,posign));
		//cout << result;
		operateur=line.substr(posign,1);
		//cout <<"   "<<operateur<<"   ";
		line=line.substr(posign+1);
		//cout << line << "\n";
		while (not fin) {
			posplus=line.find('+');posminus=line.find('-');
			if ((posplus>line.size())and(posminus>line.size())) {
				if (operateur==plus) {result +=this->param2val(line);}
				if (operateur==minus) {result -=this->param2val(line);}
				//cout << "result = " << result <<"\n";
				fin=true;break;
			}
			else {if ((posplus>=0)and(posminus>line.size())){posign=posplus;}
			else {if ((posminus>=0)and(posplus>line.size())){posign=posminus;}
				else {if (posplus<posminus) {posign=posplus;} else {posign=posminus;}}}
			}
			if (operateur==plus) {result +=this->param2val(line.substr(0,posign));}
			if (operateur==minus) {result -=this->param2val(line.substr(0,posign));}
			operateur=line.substr(posign,1);
			line=line.substr(posign+1);
		}
		return result;
	}

	void setHistParamValue() {
		for (int ievent=0;ievent<this->nevent;ievent++) {
			if (this->eventlist[ievent].Ne<0) {this->eventlist[ievent].Ne = (int)this->getvalue(this->eventlist[ievent].sNe);}
			if (this->eventlist[ievent].time<0) {this->eventlist[ievent].time = (int)this->getvalue(this->eventlist[ievent].stime);}
			if ((this->eventlist[ievent].admixrate<0)and(this->eventlist[ievent].action==3)) {this->eventlist[ievent].admixrate = this->getvalue(this->eventlist[ievent].sadmixrate);}
		}
		for (int i=0;i<this->nn0;i++) {
			if (this->ne0list[i].val<0) this->ne0list[i].val = (int)this->getvalue(this->ne0list[i].name);
		}
	}

	void setMutParamValue(int loc){
		int gr = this->locuslist[loc].groupe;
		//cout <<"\n SetMutParamValue pour le locus "<<loc<< " (groupe"<< gr <<")\n";
		if (this->locuslist[loc].type<5) {  //MICROSAT
			//cout << "mutmoy = "<<this->grouplist[gr].mutmoy <<" (avant)";
			if (this->grouplist[gr].mutmoy<0) this->grouplist[gr].mutmoy = this->drawfromprior(this->grouplist[gr].priormutmoy);
			this->grouplist[gr].priormutloc.mean = this->grouplist[gr].mutmoy;
			//cout << "   et "<<this->grouplist[gr].mutmoy <<" (apres)\n";

			//cout <<this->locuslist[loc].priormut.loi<<","<< this->locuslist[loc].priormut.mini<<","<< this->locuslist[loc].priormut.maxi<<","<< this->locuslist[loc].priormut.mean<<","<< this->locuslist[loc].priormut.sdshape<<"\n";
			if ((this->grouplist[gr].priormutloc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].mut_rate = this->drawfromprior(this->grouplist[gr].priormutloc);
			else this->locuslist[loc].mut_rate =this->grouplist[gr].mutmoy;
			//cout << "   et "<<this->locuslist[loc].mut_rate <<" (apres)\n";

			//cout << "Pmoy = "<<this->grouplist[gr].Pmoy <<" (avant)";
			if (this->grouplist[gr].Pmoy<0) this->grouplist[gr].Pmoy = this->drawfromprior(this->grouplist[gr].priorPmoy);
			this->grouplist[gr].priorPloc.mean = this->grouplist[gr].Pmoy;
			//cout << "   et "<<this->grouplist[gr].Pmoy <<" (apres)\n";

			if ((this->grouplist[gr].priorPloc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].Pgeom = this->drawfromprior(this->grouplist[gr].priorPloc);
			else this->locuslist[loc].Pgeom =this->grouplist[gr].Pmoy;
			//cout << "   et "<<this->locuslist[loc].Pgeom <<" (apres)\n";

			if (this->grouplist[gr].snimoy<0) this->grouplist[gr].snimoy = this->drawfromprior(this->grouplist[gr].priorsnimoy);
			this->grouplist[gr].priorsniloc.mean = this->grouplist[gr].snimoy;

			if (this->grouplist[gr].priorsniloc.sdshape>0.001 ) this->locuslist[loc].sni_rate = this->drawfromprior(this->grouplist[gr].priorsniloc);
			else this->locuslist[loc].sni_rate =this->grouplist[gr].snimoy;
			//cout <<"mutmoy = "<< this->grouplist[gr].mutmoy << "  Pmoy = "<<this->grouplist[gr].Pmoy <<"  snimoy="<<this->grouplist[gr].snimoy<<"\n";
			//cout <<"locus "<<loc<<"  groupe "<<gr<< "  mut_rate="<<this->locuslist[loc].mut_rate<<"  Pgeom="<<this->locuslist[loc].Pgeom<<"  sni_rate="<<this->locuslist[loc].sni_rate << "\n";
		}
		else		                    //DNA SEQUENCE
		{
			//cout << "musmoy = "<<this->grouplist[gr].musmoy <<" (avant)";
			if (this->grouplist[gr].musmoy<0) this->grouplist[gr].musmoy = this->drawfromprior(this->grouplist[gr].priormusmoy);
			this->grouplist[gr].priormusloc.mean = this->grouplist[gr].musmoy;
			//cout << "   et "<<this->grouplist[gr].musmoy <<" (apres)\n";

			//cout <<this->locuslist[loc].priormut.loi<<","<< this->locuslist[loc].priormut.mini<<","<< this->locuslist[loc].priormut.maxi<<","<< this->locuslist[loc].priormut.mean<<","<< this->locuslist[loc].priormut.sdshape<<"\n";
			if ((this->grouplist[gr].priormusloc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].mus_rate = this->drawfromprior(this->grouplist[gr].priormusloc);
			else this->locuslist[loc].mus_rate =this->grouplist[gr].musmoy;
			//cout << "   et "<<this->locuslist[loc].mut_rate <<" (apres)\n";

			if (this->grouplist [gr].mutmod>0) {
			//cout << "Pmoy = "<<this->grouplist[gr].Pmoy <<" (avant)";
			if (this->grouplist[gr].k1moy<0) this->grouplist[gr].k1moy = this->drawfromprior(this->grouplist[gr].priork1moy);
			this->grouplist[gr].priork1loc.mean = this->grouplist[gr].k1moy;
			//cout << "   et "<<this->grouplist[gr].Pmoy <<" (apres)\n";

			if ((this->grouplist[gr].priork1loc.sdshape>0.001)and(this->grouplist[gr].nloc>1)) this->locuslist[loc].k1 = this->drawfromprior(this->grouplist[gr].priork1loc);
			else this->locuslist[loc].k1 =this->grouplist[gr].k1moy;
			//cout << "   et "<<this->locuslist[loc].Pgeom <<" (apres)\n";
			}

			if (this->grouplist [gr].mutmod>2) {
			if (this->grouplist[gr].k2moy<0) this->grouplist[gr].k2moy = this->drawfromprior(this->grouplist[gr].priork2moy);
			this->grouplist[gr].priork2loc.mean = this->grouplist[gr].k2moy;

			if (this->grouplist[gr].priork2loc.sdshape>0.001 ) this->locuslist[loc].k2 = this->drawfromprior(this->grouplist[gr].priork2loc);
			else this->locuslist[loc].k2 =this->grouplist[gr].k2moy;
			//cout <<"mutmoy = "<< this->grouplist[gr].mutmoy << "  Pmoy = "<<this->grouplist[gr].Pmoy <<"  snimoy="<<this->grouplist[gr].snimoy<<"\n";
			//cout <<"locus "<<loc<<"  groupe "<<gr<< "  mut_rate="<<this->locuslist[loc].mut_rate<<"  Pgeom="<<this->locuslist[loc].Pgeom<<"  sni_rate="<<this->locuslist[loc].sni_rate << "\n";
			}
		}
	}

	bool firstEvent(int ievent) {
		if (ievent == 0) {return 1;} //1 pour TRUE
		bool found;
		int jevent=ievent;
		while (jevent>0) {
			jevent -=1;
			if (this->eventlist[jevent].action == 3) {
				found = (this->eventlist[jevent].pop1 == (this->eventlist[ievent].pop)or(this->eventlist[jevent].pop2 ==this->eventlist[ievent].pop));}
			else {found = this->eventlist[jevent].pop == this->eventlist[ievent].pop;}
			if (found) {break;}
			}
		return (not found);
	}

	int findNe(int ievent, int refpop) {
		if (ievent == 0) {return this->ne0list[refpop].val;}
		bool found;
		while (ievent>0) {
			ievent -=1;
			found = (this->eventlist[ievent].action == 1)and(this->eventlist[ievent].pop == refpop);
			if (found) {break;}
			}
		if (found) {return this->eventlist[ievent].Ne;}
		else       {return this->ne0list[refpop-1].val;}
	}

	int findT0(int ievent, int refpop) {
		int jevent = ievent;
		bool found = 0; //FALSE
		while (jevent>0) {
			jevent -=1;
			if (this->eventlist[jevent].action == 3) {
				found = (this->eventlist[jevent].pop1 == refpop)or(this->eventlist[jevent].pop2 == refpop);}
			else {found = (this->eventlist[jevent].pop == refpop);}
			if (found) {break;}
			}
		if (found) {return this->eventlist[jevent].time;}
		else       {return this->eventlist[ievent].time;}
	}

	void seqCoal(int iseq, int ievent, int refpop) {
		this->seqlist[iseq].action = 0;
		this->seqlist[iseq].N = findNe(ievent,refpop);
		this->seqlist[iseq].pop = refpop;
		this->seqlist[iseq].t0 = findT0(ievent,refpop);
		this->seqlist[iseq].t1 = this->eventlist[ievent].time;
	}

	void seqSamp(int iseq, int ievent) {
		this->seqlist[iseq].action = 3;
		this->seqlist[iseq].pop = this->eventlist[ievent].pop;
		this->seqlist[iseq].t0 = this->eventlist[ievent].time;
		this->seqlist[iseq].sample = this->eventlist[ievent].sample;
	}

	void seqMerge(int iseq, int ievent) {
		this->seqlist[iseq].action = 1;
		this->seqlist[iseq].pop = this->eventlist[ievent].pop;
		this->seqlist[iseq].pop1 = this->eventlist[ievent].pop1;
		this->seqlist[iseq].t0 = this->eventlist[ievent].time;
	}

	void seqSplit(int iseq, int ievent) {
		this->seqlist[iseq].action = 2;
		this->seqlist[iseq].pop = this->eventlist[ievent].pop;
		this->seqlist[iseq].pop1 = this->eventlist[ievent].pop1;
		this->seqlist[iseq].pop2 = this->eventlist[ievent].pop2;
		this->seqlist[iseq].t0 = this->eventlist[ievent].time;
		this->seqlist[iseq].admixrate = this->eventlist[ievent].admixrate;
	}

	int compteseq() {
    	int n = 0;
    	for (int k=0;k<nevent;k++){
    		if (this->eventlist[k].action == 0) {n +=2;}  //SAMPLE
    		else if (this->eventlist[k].action == 1) {n +=1;} //VARNE
    		else if (this->eventlist[k].action == 2) {n +=3;} //MERGE
    		else if (this->eventlist[k].action == 3) {n +=4;} //SPLIT
    	}
    	return n;
    }




	void setSequence() {
		int kseq = compteseq();
		this->seqlist = new SequenceBitC[kseq];
		int iseq = 0;
		for (int ievent=0;ievent<this->nevent;ievent++){
			if (firstEvent(ievent) == 0) {  // If NOT FirstEvent
				if (this->eventlist[ievent].action == 1) {   // if action = VARNE
					seqCoal(iseq,ievent,this->eventlist[ievent].pop);iseq++;
				}
				else if (this->eventlist[ievent].action == 0) {   // if action = SAMPLE
					seqCoal(iseq,ievent,this->eventlist[ievent].pop);iseq++;
					seqSamp(iseq,ievent);iseq++;
				}
			}
			else if (this->eventlist[ievent].action == 0) {   // if action = SAMPLE
				seqSamp(iseq,ievent);iseq++;
			}
			if (this->eventlist[ievent].action == 2) {   // if action = MERGE
				seqCoal(iseq,ievent,this->eventlist[ievent].pop);
				if (this->seqlist[iseq].t0 <  this->seqlist[iseq].t1) {iseq++;}
				seqCoal(iseq,ievent,this->eventlist[ievent].pop1);
				if (this->seqlist[iseq].t0 <  this->seqlist[iseq].t1) {iseq++;}
				seqMerge(iseq,ievent);iseq++;
			}
			if (firstEvent(ievent) == 0) {  // If NOT FirstEvent
				if(this->eventlist[ievent].action == 3) {   // if action = SPLIT
					seqCoal(iseq,ievent,this->eventlist[ievent].pop);
					if (this->seqlist[iseq].t0 <  this->seqlist[iseq].t1) {iseq++;}
					seqCoal(iseq,ievent,this->eventlist[ievent].pop1);
					if (this->seqlist[iseq].t0 <  this->seqlist[iseq].t1) {iseq++;}
					seqCoal(iseq,ievent,this->eventlist[ievent].pop2);
					if (this->seqlist[iseq].t0 <  this->seqlist[iseq].t1) {iseq++;}
					seqSplit(iseq,ievent);iseq++;

				}
			}
		}
		if ((this->eventlist[this->nevent-1].action == 0)or(this->eventlist[this->nevent-1].action == 2)) {
			this->seqlist[iseq].N = findNe(this->nevent-1,eventlist[this->nevent-1].pop);
		}
		else if (this->eventlist[this->nevent-1].action == 1) {
			this->seqlist[iseq].N = this->eventlist[this->nevent-1].Ne;
		}
		this->seqlist[iseq].pop = this->eventlist[this->nevent-1].pop;
		this->seqlist[iseq].t0 = this->eventlist[this->nevent-1].time;
		this->seqlist[iseq].t1 = -1;
		this->seqlist[iseq].action = 0;
		this->nseq=iseq+1;
		/*std::cout << "nombre de SequenceBits alloués = " << kseq << "\n";
		std::cout << "nombre de SequenceBits utilisés = " << iseq << "\n";
		for (int i=0;i<this->nseq;i++){
			if (this->seqlist[i].action == 0) {
				std::cout << seqlist[i].t0 << " à " << seqlist[i].t1 << "   coal  pop=" << seqlist[i].pop;
				std::cout << "  N=" << seqlist[i].N << "\n";
			}
			else if (this->seqlist[i].action == 1) {
				std::cout << seqlist[i].t0 << "   Merge  " << "pop " << seqlist[i].pop << " et " << seqlist[i].pop1 << "\n";
			}
			else if (this->seqlist[i].action == 2) {
				std::cout << seqlist[i].t0 << "   Split  " << "pop " << seqlist[i].pop << " vers " << seqlist[i].pop1 << " et " <<seqlist[i].pop2 << "\n";
			}
			else if (this->seqlist[i].action == 3) {
				std::cout << seqlist[i].t0 << "   Adsamp  " << "à la pop " << seqlist[i].pop << "\n";
			}

		}*/
	}
	void comp_matQ(int loc) {
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
		double s=0.0;
		for (int i=0;i<4;i++) {
			for (int j=0;j<4;j++) {s += this->matQ[i][j];}
		}
		for (int i=0;i<4;i++) {
			for (int j=0;j<4;j++) {this->matQ[i][j] /= s;}
		}
	}


	GeneTreeC init_tree(int loc) {
		//cout << "début de init_tree pour le locus " << loc  <<"\n";
		GeneTreeC gt;
		int nnod=0,nn,n=0;
		//cout << "nsample = " << this->data.nsample << "   samplesize[0][0] = " << samplesize[0][0] << "\n";
		for (int sa=0;sa<this->data.nsample;sa++) {nnod +=this->locuslist[loc].ss[sa];}
		//cout << "nombre initial de noeuds = " << nnod  <<"\n";
		gt.ngenes = nnod;
		gt.nnodes=nnod;
		gt.nbranches = 0;
		gt.nodes = new NodeC[2*nnod+1];
		gt.branches = new BranchC[2*nnod];
		for (int sa=0;sa<this->data.nsample;sa++) {
			for (int ind=0;ind<this->data.nind[sa];ind++) {
				nn=calploidy(loc,sa,ind);
				//cout << "n=" << n << "     nn=" << nn << "\n";
				if (nn>0) {
					for (int i=n;i<n+nn;i++){
						gt.nodes[i].sample=sa+1;
						//cout << gt.nodes[i].sample  << "\n";
						gt.nodes[i].height=this->time_sample[sa];
						//cout << gt.nodes[i].height  << "\n";
					}
				n +=nn;
				}
			}
		}
		/*for (int i=0;i<gt.nnodes;i++){
			cout << "node " << i << "sample = " << gt.nodes[i].sample << "\n";
		}*/
		return gt;

	}

	int evalcriterium(int iseq,int nLineages) {
		if(this->seqlist[iseq].t1<0) {return 1;}
		int nGen;
		double ra;
		int OK=0;
		nGen=this->seqlist[iseq].t1-this->seqlist[iseq].t0;
		ra= (float) nLineages/ (float)this->seqlist[iseq].N;
		if (nGen<=30) {
			if (ra < (0.0031*nGen*nGen - 0.053*nGen + 0.7197)) {OK=1;}
		}
		else if (nGen<=100){
			if (ra < (0.033*nGen + 1.7)) {OK=1;}
		}
		else if (ra<0.5) {OK=1;}
		return OK;
	}

	int draw_node(int loc,int iseq,int nLineages) {
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

	void coal_pop(int loc,int iseq) {
		//cout <<"\n";
		//cout << "debut COAL nbranches=" << this->gt[loc].nbranches << "\n";
		int nLineages=0;
		bool final=false;
		for (int i=0;i<this->gt[loc].nnodes;i++){
			//cout << gt[loc].nodes[i].pop << "  ";
			if (this->gt[loc].nodes[i].pop == this->seqlist[iseq].pop) {nLineages +=1;}
		}
		//cout << "\n";
		if (this->seqlist[iseq].t1<0) {final=true;}
		if (this->seqlist[iseq].N<1) {std::cout << "coal_pop : population size <1 \n" ;exit(100);}

		if (evalcriterium(iseq,nLineages) == 1) {		//CONTINUOUS APPROXIMATION
			//cout << "Approximation continue final="<< final << "   nLineages=" << nLineages << "  pop=" <<this->seqlist[iseq].pop << "\n";
			double start = this->seqlist[iseq].t0;
			//cout << "start initial= " << start << "\n";
			while ((nLineages>1)and((final)or((not final)and(start<this->seqlist[iseq].t1)))) {
				double ra = this->mw.random();
				while (ra == 0.0) {ra = this->mw.random();}
				double lra; lra = log(ra);
				start -= (this->locuslist[loc].coeff*this->seqlist[iseq].N/nLineages/(nLineages-1.0))*log(ra);
				//cout << "coeff = " << this->locuslist[loc].coeff << "   N = " << this->seqlist[iseq].N << "nl*(nl-1) = " << nLineages/(nLineages-1.0) << "\n";
				//cout << "start courant= " << start << "  log(ra)=" << lra << "\n";
				if ((final)or((not final)and(start<this->seqlist[iseq].t1))) {
					this->gt[loc].nodes[this->gt[loc].nnodes].pop=this->seqlist[iseq].pop;
					this->gt[loc].nodes[this->gt[loc].nnodes].height=start;
					this->gt[loc].nnodes++;
					this->gt[loc].branches[this->gt[loc].nbranches].top=this->gt[loc].nnodes-1;
					this->gt[loc].branches[this->gt[loc].nbranches].bottom=draw_node(loc,iseq,nLineages);
					//cout << "retour noeud tiré : " << this->gt[loc].branches[this->gt[loc].nbranches].bottom <<"\n";
					nLineages--;
					this->gt[loc].branches[this->gt[loc].nbranches].length=this->gt[loc].nodes[this->gt[loc].branches[this->gt[loc].nbranches].top].height-this->gt[loc].nodes[this->gt[loc].branches[this->gt[loc].nbranches].bottom].height;
					this->gt[loc].nbranches++;
					this->gt[loc].branches[this->gt[loc].nbranches].top=this->gt[loc].nnodes-1;
					this->gt[loc].branches[this->gt[loc].nbranches].bottom=draw_node(loc,iseq,nLineages);
					//cout << "retour noeud tiré : " << this->gt[loc].branches[this->gt[loc].nbranches].bottom <<"\n";
					this->gt[loc].branches[this->gt[loc].nbranches].length=this->gt[loc].nodes[this->gt[loc].branches[this->gt[loc].nbranches].top].height-this->gt[loc].nodes[this->gt[loc].branches[this->gt[loc].nbranches].bottom].height;
					this->gt[loc].nbranches++;
					//cout << "nbranches = " << this->gt[loc].nbranches << "\n";
				}
			}
		}
		else {											//GENERATION PER GENERATION
			//cout << "Génération par génération pour le locus "<<loc<<"\n";
			//int *numtire,*num,*knum;
			int gstart= (int)(this->seqlist[iseq].t0+0.5);
			int Ne= (int) (0.5*this->locuslist[loc].coeff*this->seqlist[iseq].N+0.5);
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

	void pool_pop(int loc,int iseq) {
		for (int i=0;i<this->gt[loc].nnodes;i++){
			if (this->gt[loc].nodes[i].pop == this->seqlist[iseq].pop1) {
				this->gt[loc].nodes[i].pop = this->seqlist[iseq].pop;
			}
		}
	}

	void split_pop(int loc,int iseq) {
		for (int i=0;i<this->gt[loc].nnodes;i++){
			if (this->gt[loc].nodes[i].pop == this->seqlist[iseq].pop){
			if (this->mw.random()<this->seqlist[iseq].admixrate) {this->gt[loc].nodes[i].pop = this->seqlist[iseq].pop1;}
			else                                      {this->gt[loc].nodes[i].pop = this->seqlist[iseq].pop2;}
			}
		}
	}

	void add_sample(int loc,int iseq) {
		for (int i=0;i<this->gt[loc].nnodes;i++){
			if (this->gt[loc].nodes[i].sample == this->seqlist[iseq].sample) {
				this->gt[loc].nodes[i].pop = this->seqlist[iseq].pop;
			}
		}

	}

	int gpoisson(double lambda) {
		if (lambda<=0) {return 0;}
		int i=0;
		double p=0.0;
		double ra;
		while (p<lambda) {
			i++;
			do {ra = this->mw.random();} while (ra==0.0);
			p -= log(ra);
		}
		return (i-1);
	}

	void put_mutations(int loc) {
		this->gt[loc].nmutot=0;
		double mutrate;
		if (this->locuslist[loc].type <5) mutrate=this->locuslist[loc].mut_rate+this->locuslist[loc].sni_rate;
		else                        mutrate=this->locuslist[loc].mus_rate;
		for (int b=0;b<this->gt[loc].nbranches;b++) {
			this->gt[loc].branches[b].nmut = this->mw.poisson(this->gt[loc].branches[b].length*mutrate);
			this->gt[loc].nmutot += this->gt[loc].branches[b].nmut;
		}
	}

	void mute(int loc, int numut, int b) {
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
		else  {								//DNA SEQUENCE
			string dna=this->gt[loc].nodes[this->gt[loc].branches[b].bottom].dna;
			char dnb;
			double ra=this->mw.random();
			int n =this->locuslist[loc].sitmut[numut];
			dnb = dna[n];
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
			dna[n] = dnb;
			this->gt[loc].nodes[this->gt[loc].branches[b].bottom].dna = dna;
		}
	}

	char draw_nuc(int loc) {
		double ra=this->mw.random();
		if (ra<this->locuslist[loc].pi_A)															return 'A';
		else if (ra<this->locuslist[loc].pi_A+this->locuslist[loc].pi_C)							return 'C';
		else if (ra<this->locuslist[loc].pi_A+this->locuslist[loc].pi_C+this->locuslist[loc].pi_G)	return 'G';
		else																						return 'T';
	}

	string init_dnaseq(int loc) {
		this->locuslist[loc].tabsit = new int[this->locuslist[loc].dnalength];
		int *sitmut2;
		string dna;
		sitmut2 = new int[this->gt[loc].nmutot];
		this->locuslist[loc].sitmut = new int[this->gt[loc].nmutot];
		for (int i=0;i<this->gt[loc].nmutot;i++){
			double ra=this->mw.random();
			double s=0.0;
			int k=-1;
			while (s<ra) {k++;s+=this->locuslist[loc].mutsit[k];}
			sitmut2[i]=k;
		}
		if (this->dnatrue) {			//TRUE DNA SEQUENCE
			for (int i=0;i<this->gt[loc].nmutot;i++) this->locuslist[loc].sitmut[i]=sitmut2[i];
			dna.resize(this->locuslist[loc].dnalength+1);
			for (int i=0;i<this->locuslist[loc].dnalength;i++){
				this->locuslist[loc].tabsit[i] = i;
				dna[i]=draw_nuc(loc);
			}
			dna[this->locuslist[loc].dnalength]='\0';
		}
		else {							//ARTIFICIAL SEQUENCE MADE ONLY OF VARIABLE SITES
			string dna2;
			dna.resize(this->gt[loc].nmutot+1);
			dna2.resize(this->locuslist[loc].dnalength);
			for (int i=0;i<this->locuslist[loc].dnalength;i++){
				dna2[i] = 'z';
				this->locuslist[loc].tabsit[i] = -1;
			}
			int k=0;
			for (int i=0;i<this->gt[loc].nmutot;i++) {
				if (dna2[sitmut2[i]] == 'z') {
					this->locuslist[loc].tabsit[sitmut2[i]] = k;
					this->locuslist[loc].sitmut[i]=k;
					k++;
					dna2[sitmut2[i]] = draw_nuc(loc);
					dna[k-1]=dna2[sitmut2[i]];
				}
			}
			dna.resize(k);
		}
		delete []sitmut2;
		return dna;
	}

	int cree_haplo(int loc) {
		//cout <<"CREE_HAPLO\n";
/*		if (loc==10) {
		cout << "nbranches = " << this->gt[loc].nbranches << "    nnoeuds=" << gt[loc].nnodes <<"\n";
		for (int br=0;br<this->gt[loc].nbranches;br++)
			{cout << "branche " << br << "   bottom=" << this->gt[loc].branches[br].bottom ;
			 cout << "   top=" << this->gt[loc].branches[br].top << "   nmut=" << this->gt[loc].branches[br].nmut;
			 cout << "   length=" << this->gt[loc].branches[br].length << "\n";
			}
		}*/
		vector < vector <int> > ordre;
		for (int no=0;no<this->gt[loc].nnodes;no++) this->gt[loc].nodes[no].state=10000;
		int anc=this->gt[loc].nnodes-1;
		if (this->locuslist[loc].type<5) {
			this->gt[loc].nodes[anc].state=this->locuslist[loc].kmin + (int)(0.5*(this->locuslist[loc].kmax-this->locuslist[loc].kmin));
			//cout << "anc-state = " << this->gt[loc].nodes[anc].state << "\n";
		}
		else {
			this->gt[loc].nodes[anc].state=0;
			this->gt[loc].nodes[anc].dna = init_dnaseq(loc);
/*			cout << "locus " << loc  << "\n";
			cout << "anc-state = " << this->gt[loc].nodes[anc].state << "\n";
			cout << "anc-dna = " << this->gt[loc].nodes[anc].dna << "\n";*/
		}
		anc++;
		int numut=-1;
		int len;
		while (anc>=this->gt[loc].ngenes) {
			anc--;
			//cout << "locus " << loc << "   ngenes = " << this->gt[loc].ngenes << "   anc = " << anc << "\n";
			int i=-1;
			bool trouve=false;
			while ((not trouve)and(i<this->gt[loc].nbranches-1)) {
				i++;
				trouve = (this->gt[loc].branches[i].top == anc);
				/*if (this->locuslist[loc].type>4) {
					cout << "branche " << i << "  top = " << anc << "   state = " << gt[loc].nodes[this->gt[loc].branches[i].bottom].state << "\n";
				}*/
				if ((trouve)and(this->gt[loc].nodes[this->gt[loc].branches[i].bottom].state == 10000)) {
					this->gt[loc].nodes[this->gt[loc].branches[i].bottom].state = this->gt[loc].nodes[this->gt[loc].branches[i].top].state;
					if (this->locuslist[loc].type>4) {
						len=-1;
						do len++; while (this->gt[loc].nodes[this->gt[loc].branches[i].top].dna[len]!='\0');
						//cout << "len = " << len << "\n";
						this->gt[loc].nodes[this->gt[loc].branches[i].bottom].dna = new char[len+1];
						for (int j=0;j<len+1;j++) this->gt[loc].nodes[this->gt[loc].branches[i].bottom].dna[j]=this->gt[loc].nodes[this->gt[loc].branches[i].top].dna[j];
						//cout << "node " << this->gt[loc].branches[i].bottom << "   dna = " << this->gt[loc].nodes[this->gt[loc].branches[i].bottom].dna << "\n";
					}
					if (this->gt[loc].branches[i].nmut>0) {
						for (int j=0;j<this->gt[loc].branches[i].nmut;j++) {
							numut++;
							mute(loc,numut,i);
						}
					}
					trouve=false;
				}
			}
		}
		/*if (this->locuslist[loc].type<5) {    for (int no=0;no<this->gt[loc].nnodes;no++) cout << gt[loc].nodes[no].state << "   ";}
		else {for (int no=0;no<this->gt[loc].nnodes;no++) cout << gt[loc].nodes[no].dna << "   ";}
		cout << "\n";*/
		int sa,ind;
		// tirage au hasard des gènes avant attribution aux individus
		ordre.resize(this->data.nsample);ind=0;
		for (sa=0;sa<this->data.nsample;sa++) {
			ordre[sa] = melange(this->mw,this->locuslist[loc].ss[sa]);
			if (sa>0) {for (int i=0;i<this->locuslist[loc].ss[sa];i++) ordre[sa][i]+=ind;}
			ind +=this->locuslist[loc].ss[sa];
		}
		if (this->locuslist[loc].type<5) {       //MICROSAT
			this->locuslist[loc].haplomic = new int*[this->data.nsample];
			for (sa=0;sa<this->data.nsample;sa++) this->locuslist[loc].haplomic[sa] = new int [this->locuslist[loc].ss[sa]];
			sa=0;ind=0;
			for (int i=0;i<this->gt[loc].ngenes;i++) {
				if (this->gt[loc].nodes[ordre[sa][ind]].state == 10000) return 2;
				this->locuslist[loc].haplomic[sa][ind] = this->gt[loc].nodes[ordre[sa][ind]].state;
				ind++;if (ind==this->locuslist[loc].ss[sa]) {sa++;ind=0;}
			}
		this->locuslist[loc].samplesize =new int[this->data.nsample];
		for (int sa=0;sa<this->data.nsample;sa++) this->locuslist[loc].samplesize[sa] =this->locuslist[loc].ss[sa];
//AJOUTER LE TRAITEMENT DES DONNEES MANQUANTES DES MICROSAT
		//Penser à décrémenter samplesize
		}
		else {									//DNA SEQUENCES
			this->locuslist[loc].haplodna = new char**[this->data.nsample];
			for (sa=0;sa<this->data.nsample;sa++) this->locuslist[loc].haplodna[sa] = new char* [this->locuslist[loc].ss[sa]];
			sa=0;ind=0;
			for (int i=0;i<this->gt[loc].ngenes;i++) {
				if (this->gt[loc].nodes[ordre[sa][ind]].state == 10000) {
					cout << "nbranches = " << this->gt[loc].nbranches << "    nnoeuds=" << gt[loc].nnodes <<"\n";
					for (int br=0;br<this->gt[loc].nbranches;br++)
						{cout << "branche " << br << "   bottom=" << this->gt[loc].branches[br].bottom ;
						 cout << "   top=" << this->gt[loc].branches[br].top << "   nmut=" << this->gt[loc].branches[br].nmut;
						 cout << "   length=" << this->gt[loc].branches[br].length << "\n";
						}
					cout <<"\n";
					for (int n=0;n<this->gt[loc].nnodes;n++) {
						cout << "noeud  " << n << "   state = " << this->gt[loc].nodes[n].state << "   dna = " ;
						for (int j=0;j<len+1;j++) cout << this->gt[loc].nodes[n].dna[j];
						cout << "\n";
					}
					cout << "\n";
					return 2;
				}
				this->locuslist[loc].haplodna[sa][ind] = this->gt[loc].nodes[ordre[sa][ind]].dna;
				ind++;if (ind==this->locuslist[loc].ss[sa]) {sa++;ind=0;}
				}
//AJOUTER LE TRAITEMENT DES DONNEES MANQUANTES DES SEQUENCES
		}
		return 0;
	}

	int dosimulpart(int ipart,int mrseed){
		vector <int> simulOK;
		int simOK;
		int *emptyPop;
		bool treedone;
		this->mw.randinit(mrseed,true);
		simulOK.resize(this->nloc);
		GeneTreeC GeneTreeY, GeneTreeM;
		this->setHistParamValue();
		this->setSequence();
		bool gtYexist=false, gtMexist=false;
		this->gt = new GeneTreeC[this->nloc];
		emptyPop = new int[this->popmax+1];
		//cout << "particule " << ipart <<"   " "\n";
		for (int k=0;k<this->nparam;k++){
			//cout << this->parameterlist[k].value << "   ";
		}
		//cout << "\n";
		for (int loc=0;loc<this->nloc;loc++) {
			setMutParamValue(loc);
			if (this->locuslist[loc].type >4) {
				comp_matQ(loc);
				}
			treedone=false;
			if ((this->locuslist[loc].type % 5) == 3) {
				if (gtYexist) {this->gt[loc] = copytree(GeneTreeY);treedone=true;}
			}
			else if ((locuslist[loc].type % 5) == 4) {
				    //cout << "coucou   gtMexist=" << gtMexist <<"\n";
					if (gtMexist) {this->gt[loc] = copytree(GeneTreeM);treedone=true;}
				 }
			if (not treedone) {
				//cout << "avant init_tree \n";
				this->gt[loc] = init_tree(loc);
				//cout << "initialisation de l'arbre du locus " << loc  << "    ngenes="<< this->gt[loc].ngenes<< "   nseq="<< this->nseq <<"\n";
				for (int p=0;p<this->popmax+1;p++) {emptyPop[p]=1;} //True
				for (int iseq=0;iseq<this->nseq;iseq++) {
					//cout << "traitement de l element de sequence " << iseq << "    action= "<<this->seqlist[iseq].action << "\n";
					if (this->seqlist[iseq].action == 0) {	//COAL
						//for (int k=1;k<4;k++) cout << emptyPop[k] << "   ";
						//cout <<"\n";
						if (((this->seqlist[iseq].t1>this->seqlist[iseq].t0)or(this->seqlist[iseq].t1<0))and(emptyPop[seqlist[iseq].pop]==0)) {
							//cout << "dosimul appel de coal_pop \n";
							coal_pop(loc,iseq);
							//cout << "apres coal_pop\n";
						}
					}
					else if (this->seqlist[iseq].action == 1) {	//MERGE
						if ((emptyPop[seqlist[iseq].pop]==0)or(emptyPop[seqlist[iseq].pop1]==0)) {
							//cout << "dosimul appel de pool_pop \n";
							pool_pop(loc,iseq);
							emptyPop[seqlist[iseq].pop]  =0;
							emptyPop[seqlist[iseq].pop1] =1;
						}
					}
					else if (this->seqlist[iseq].action == 2) {  //SPLIT
						if (emptyPop[seqlist[iseq].pop]==0) {
							//cout << "dosimul appel de split_pop \n";
							split_pop(loc,iseq);
							emptyPop[seqlist[iseq].pop]  =1;
							emptyPop[seqlist[iseq].pop1] =0;
							emptyPop[seqlist[iseq].pop2] =0;
						}
					}
					else if (this->seqlist[iseq].action == 3) {  //ADSAMP
						//cout << "dosimul appel de add_sample \n";
						add_sample(loc,iseq);
						emptyPop[seqlist[iseq].pop]  =0;
						//for (int k=1;k<4;k++) cout << emptyPop[k] << "   ";
						//cout <<"\n";
					}
				}	//LOOP ON iseq
	/* copie de l'arbre si locus sur Y ou mitochondrie */
				if ((locuslist[loc].type % 5) == 3) {GeneTreeY  = copytree(this->gt[loc]);gtYexist=true;}

				if ((locuslist[loc].type % 5) == 4) {
					//cout <<"avant copie de genetreeM\n";
					GeneTreeM  = copytree(this->gt[loc]);gtMexist=true;}
			}
/* mutations */
			put_mutations(loc);
			//cout << "Locus " <<loc << "  apres put_mutations\n";
			simOK=cree_haplo(loc);
			//cout << "Locus " <<loc << "  apres cree_haplo\n";
			if (simOK!=0) cout <<"    locus "<< loc << "   simOK <> 0\n";
			simulOK[loc] = simOK;
			//cout << "fin du locus " << loc << "   "<< simulOK[loc] << "\n";
		}		//LOOP ON loc
		delete [] emptyPop;
		for (int loc=0;loc<this->nloc;loc++) deletetree(this->gt[loc]);
		delete [] this->gt;
		if (gtYexist) deletetree(GeneTreeY);
		if (gtMexist) deletetree(GeneTreeM);
		//for (int loc=0;loc<this->nloc;loc++) cout << extract<int>(simulOK[loc]) << "   ";
		//cout << "\n";
		//cout << "Fin de dosimulpart \n";
		simOK=0;for (int loc=0;loc<this->nloc;loc++) simOK+=simulOK[loc];
		return simOK;
	}


/***********************************************************************************************************************/

	void calfreq() {
/*		double **fre;
		fre = new double*[this->nloc];
		for (int loc=0;loc<this->nloc;loc++) {
			fre[loc] = new double[this->locuslist[loc].kmax-this->locuslist[loc].kmin+1];
			for (int i=0;i<this->locuslist[loc].kmax-this->locuslist[loc].kmin+1;i++) fre[loc][i] = 0.0;
			}*/


		int n=0;
		//cout <<"debut de calfreq nloc = "<<this->nloc<<"  nsample = "<<this->data.nsample <<"\n";
		for (int loc=0;loc<this->nloc;loc++) {
			if (this->locuslist[loc].type<5) {
				//cout <<"Locus "<< loc <<"\n";
				this->locuslist[loc].nal = this->locuslist[loc].kmax-this->locuslist[loc].kmin+1;
				this->locuslist[loc].freq = new double* [this->data.nsample];
				//cout <<"nal ="<<this->locuslist[loc].nal<<"\n";
				for (int samp=0;samp<this->data.nsample;samp++) {
					this->locuslist[loc].freq[samp] = new double [this->locuslist[loc].nal];
					for (int i=0;i<this->locuslist[loc].nal;i++) this->locuslist[loc].freq[samp][i]=0.0;
					//cout << this->locuslist[loc].ss[samp] <<"\n";
					//cout <<this->locuslist[loc].samplesize[samp]<< "\n";
					for (int i=0;i<this->locuslist[loc].ss[samp];i++){
						if (this->locuslist[loc].haplomic[samp][i] != MISSING) {
							//cout <<"  "<<this->locuslist[loc].haplomic[samp][i];
							this->locuslist[loc].freq[samp][this->locuslist[loc].haplomic[samp][i]-this->locuslist[loc].kmin] +=1.0;
							n++;
						}
					}
					//cout <<" fini\n";
					if (this->locuslist[loc].samplesize[samp]<1) cout << "samplesize["<<loc<<"]["<<samp<<"]="<<this->locuslist[loc].samplesize[samp]<<"\n";
					for (int k=0;k<this->locuslist[loc].kmax-this->locuslist[loc].kmin+1;k++) this->locuslist[loc].freq[samp][k]/=this->locuslist[loc].samplesize[samp];
					//cout << "loc "<<loc <<"  sa "<<samp;
					//for (int k=0;k<this->locuslist[loc].nal;k++) {if (this->locuslist[loc].freq[samp][k]>0.0) cout << "   "<<this->locuslist[loc].freq[samp][k]<<"("<<k+this->locuslist[loc].kmin<<")";}
					//cout << "\n";
					}
			}
		}
/*		for (int loc=0;loc<this->nloc;loc++) {
			if (this->locuslist[loc].type<5) {
				for (int samp=0;samp<this->data.nsample;samp++) {
					for (int i=0;i<this->locuslist[loc].nal;i++) {
						fre[loc][i] +=this->locuslist[loc].freq[samp][i];
					}
				}
			}
		}
		for (int loc=0;loc<this->nloc;loc++) {
			if (this->locuslist[loc].type<5) {
				cout << "\nLocus "<<loc+1 << "\n    ";
				for (int i=0;i<this->locuslist[loc].nal;i++) {if (fre[loc][i] > 0.0) cout << "    "<<i+this->locuslist[loc].kmin;}
				cout << "\n";
				for (int samp=0;samp<this->data.nsample;samp++) {
					cout <<"Pop " <<samp+1;
					for (int i=0;i<this->locuslist[loc].nal;i++) {if (fre[loc][i] > 0.0) printf(" %6.3f",this->locuslist[loc].freq[samp][i]);}
					cout <<"\n";
				}
			}
		}
		cout <<"fin de calfreq \n";*/
	}

	double cal_nal1p(StatC stat){
		double nalm=0.0;
		int iloc,loc,nl=0;
		//cout << "nloc = " << stat.nloc <<"\n";
		for (iloc=0;iloc<this->grouplist[stat.group].nloc;iloc++){
			loc=this->grouplist[stat.group].loc[iloc];
			//cout << "cal_n1p  locus = " << loc <<"   stat.group = "<<stat.group<<"\n";
			//cout << this->locuslist[loc].samplesize[stat.samp] <<"\n";
			if(this->locuslist[loc].samplesize[stat.samp]>0) {
			for (int k=0;k<this->locuslist[loc].nal;k++) {if (this->locuslist[loc].freq[stat.samp][k]>0.00001) nalm +=1.0;}
			nl++;
			}
		}
		//cout <<"nalm="<<nalm<<"    nl="<<nl<<"\n";
		if (nl>0) return nalm/(double)nl; else return 0.0;
	}

	double cal_nal2p(StatC stat){
		int k,iloc,loc,nl=0;
		double nalm=0.0;
		for (iloc=0;iloc<this->grouplist[stat.group].nloc;iloc++){
			loc=this->grouplist[stat.group].loc[iloc];
			if((this->locuslist[loc].samplesize[stat.samp]>0)and(this->locuslist[loc].samplesize[stat.samp1]>0)) {
				for (int k=0;k<this->locuslist[loc].nal;k++) {
					if (this->locuslist[loc].freq[stat.samp][k]+this->locuslist[loc].freq[stat.samp1][k]>0.000001) nalm+=1.0;
				}
				nl++;
			}
			else {
				if (this->locuslist[loc].samplesize[stat.samp]>0) {
					for (k=0;k<this->locuslist[loc].nal;k++) {
						if (this->locuslist[loc].freq[stat.samp][k]>0.000001) nalm+=1.0;
					}
					nl++;
				}
				else {
					if (this->locuslist[loc].samplesize[stat.samp1]>0) {
						for (k=0;k<this->locuslist[loc].nal;k++) {
							if (this->locuslist[loc].freq[stat.samp1][k]>0.000001) nalm+=1.0;
						}
						nl++;
					}
				}
			}
		}
		if (nl>0) return nalm/(double)nl; else return 0.0;
	}

	double cal_het1p(StatC stat){
		double het,hetm=0.0;
		int iloc,loc,nl=0;
		for (iloc=0;iloc<this->grouplist[stat.group].nloc;iloc++){
			loc=this->grouplist[stat.group].loc[iloc];
			if (this->locuslist[loc].samplesize[stat.samp]>1){
				het=1.0;
				for (int k=0;k<this->locuslist[loc].nal;k++) het -= sqr(this->locuslist[loc].freq[stat.samp][k]);
				het *= ((double)this->locuslist[loc].samplesize[stat.samp]/((double)this->locuslist[loc].samplesize[stat.samp]-1));
				hetm += het;
				nl++;
			}
		}
		if (nl>0) return hetm/(double)nl; else return 0.0;
	}

	double cal_het2p(StatC stat){
		double het,hetm=0.0;
		int iloc,loc,n1,n2,nt,nl=0;
		for (iloc=0;iloc<this->grouplist[stat.group].nloc;iloc++){
			loc=this->grouplist[stat.group].loc[iloc];
			n1=this->locuslist[loc].samplesize[stat.samp];n2=this->locuslist[loc].samplesize[stat.samp1];nt=n1+n2;
			if (nt>1){
				het=1.0;
				for (int k=0;k<this->locuslist[loc].nal;k++) {het -= sqr(((double)n1*this->locuslist[loc].freq[stat.samp][k]+(double)n2*this->locuslist[loc].freq[stat.samp1][k])/(double)nt);}
				het *= double(nt)/(double)(nt-1);
				hetm +=het;
				nl++;
			}
		}
		if (nl>0) return hetm/(double)nl; else return 0.0;
	}

	double cal_var1p(StatC stat){
		  double s,v,vm=0.0,m,ms;
		  int iloc,loc,n,nl=0;
			for (iloc=0;iloc<this->grouplist[stat.group].nloc;iloc++){
				loc=this->grouplist[stat.group].loc[iloc];
			  v=0.0;s=0.0;n=0;
			  for (int i=0;i<this->locuslist[loc].ss[stat.samp];i++){
				  if (this->locuslist[loc].haplomic[stat.samp][i] != MISSING) {
					  n++;
					  s+=this->locuslist[loc].haplomic[stat.samp][i];
					  v+=sqr(this->locuslist[loc].haplomic[stat.samp][i]);
				  }
			  }
			  m = (double)n;
			  ms = (double)this->locuslist[loc].motif_size;
			  if (n>1){
				  vm+=(v-sqr(s)/m)/(m-1.0)/sqr(ms);
				  nl++;
			  }
		  }
		  if (nl>0) return vm/(double)nl; else return 0.0;
	}

	double cal_var2p(StatC stat){
		double s,v,vm=0.0,m,ms;
		int iloc,loc,n,nl=0;
		for (iloc=0;iloc<this->grouplist[stat.group].nloc;iloc++){
			loc=this->grouplist[stat.group].loc[iloc];
			v=0.0;s=0.0;n=0;
			for (int i=0;i<this->locuslist[loc].ss[stat.samp];i++){
				if (this->locuslist[loc].haplomic[stat.samp][i] != MISSING) {
					n++;
					s+=this->locuslist[loc].haplomic[stat.samp][i];
					v+=sqr(this->locuslist[loc].haplomic[stat.samp][i]);
				}
			}
			for (int i=0;i<this->locuslist[loc].ss[stat.samp1];i++){
				if (this->locuslist[loc].haplomic[stat.samp1][i] != MISSING) {
					n++;
					s+=this->locuslist[loc].haplomic[stat.samp1][i];
					v+=sqr(this->locuslist[loc].haplomic[stat.samp1][i]);
				}
			}
			m = (double)n;
			ms = (double)this->locuslist[loc].motif_size;
			if (n>1){
				vm+=(v-sqr(s)/m)/(m-1.0)/sqr(ms);
				nl++;
			}
		}
		if (nl>0) return vm/(double)nl; else return 0.0;
	}

	double cal_mgw1p(StatC stat){
		double num=0.0,den=0.0;
		int min,max;
		int iloc,loc;
		for (iloc=0;iloc<this->grouplist[stat.group].nloc;iloc++){
			loc=this->grouplist[stat.group].loc[iloc];
		    if (this->locuslist[loc].samplesize[stat.samp]>0) {
				for (int k=0;k<this->locuslist[loc].nal;k++) {
					if (this->locuslist[loc].freq[stat.samp][k]>0.00001) num +=1.0;}

				min=0;while (this->locuslist[loc].freq[stat.samp][min]<0.00001) min++;
				max=this->locuslist[loc].kmax-this->locuslist[loc].kmin;while (this->locuslist[loc].freq[stat.samp][max]<0.00001) max--;
				den += (double)(1+max-min)/double(this->locuslist[loc].motif_size);
		    }
		}
		if (den>0) return num/den; else return 0.0;
	}

	double cal_Fst2p(StatC stat){
		double s1=0.0,s3=0.0,s1l,s2l,s3l,sni,sni2,sniA,sniAA;
		int al,pop,kpop,i,ig1,ig2,nA,AA,ni;
		double s2A,MSG,MSI,MSP,s2G,s2I,s2P,nc;
		int iloc,loc,nn,ind;
		//cout << "cal_Fst2p\n";
		for (iloc=0;iloc<this->grouplist[stat.group].nloc;iloc++){
			loc=this->grouplist[stat.group].loc[iloc];
			//cout << "Fst loc = "<<loc<<"\n";
		    s1l=0.0;s2l=0.0;s3l=0.0;nc=0;
//		    cout << "\n\nlocus "<< loc << "\n";
		    for (al=this->locuslist[loc].kmin;al<=this->locuslist[loc].kmax;al++) {
//		    	cout << "\n allele "<< al << "\n";
		    	sni=0;sni2=0;sniA=0;sniAA=0;s2A=0.0;
		    	for (kpop=0;kpop<2;kpop++) {
		    		if (kpop==0) pop=stat.samp; else pop=stat.samp1;
		    		nA=0;AA=0;ni=0;ind=0;
//ind est le numéro de l'individu (haploïde ou diploïde ou même sans génotype si Y femelle)
//i est le numéro de la copie du gène
		    		for (i=0;i<this->locuslist[loc].ss[pop];){
		    			nn=calploidy(loc,pop,ind);
						ind++;
		    			switch (nn)
		    			{ case 1 :  ig1 = this->locuslist[loc].haplomic[pop][i];i++;
									if (ig1==al) {AA++;nA+=2;}
									if (ig1!=MISSING) ni++;
	
									break;
		    			  case 2 :  ig1 = this->locuslist[loc].haplomic[pop][i];i++;
									ig2 = this->locuslist[loc].haplomic[pop][i];i++;
					                if ((ig1==al) and (ig2==al)) AA++;
					                if (ig1==al) nA++;
					                if (ig2==al) nA++;
					                if ((ig1!=MISSING)and(ig2!=MISSING)) ni++;
					                break;
		    			}
		    		}
		            sniA  +=(double)nA;
		            sniAA +=(double)AA;
		            sni    +=(double)ni;
		            sni2  +=sqr((double)ni);
		            s2A   +=sqr((double)nA)/(double)(2*ni);
		    	}
		    	if (sni>0) nc=sni-sni2/sni; else nc=0.0;
//		    	cout << "Allele " << al <<"   sni = " << sni << "   sni2 = "<< sni2 << "   nc = "<< nc;
//		    	cout << "   sniA = "<< sniA << "   sniAA = " << sniAA<< "   s2A = " << s2A << "\n";
		    	if (sni*nc>0.0) {
		            MSG=(0.5*sniA-sniAA)/sni;
		            MSI=(0.5*sniA+sniAA-s2A)/(sni-2.0);
		            MSP=(s2A-0.5*sqr(sniA)/sni);
		            s2G=MSG;s2I=0.5*(MSI-MSG);s2P=(MSP-MSI)/(2.0*nc);
		            s1l=s1l+s2P;         //numerateur de theta
		            s2l=s2l+s2P+s2I;     //numerateur de F
		            s3l=s3l+s2P+s2I+s2G; //denominateur de theta et de F
		    	}
		    }
//            cout << "s1l = "<< s1l<<"   s2l = "<<s2l<<"   s3l = "<< s3l << "\n";
		    s1 += (double)nc*s1l;
		    s3 += (double)nc*s3l;
		}
		if (s3>0.0) return s1/s3; else return 0.0;
	}

	double cal_lik2p(StatC stat){
  		int pop,nal,k,i,ind,ig1,ig2;
  		double frt,a,b,num_lik,den_lik,lik;
		int iloc,loc,nn,nl=0;
		lik = 0.0;
		for (iloc=0;iloc<this->grouplist[stat.group].nloc;iloc++){
			loc=this->grouplist[stat.group].loc[iloc];
		    if (this->locuslist[loc].samplesize[stat.samp]*this->locuslist[loc].samplesize[stat.samp1]>0) {
				nl++;
				nal = 0;
				for (k=0;k<this->locuslist[loc].nal;k++) {
					frt=0.0;
					for (pop=0;pop<this->data.nsample;pop++) frt += this->locuslist[loc].freq[pop][k];
					if (frt>0.000001) nal++;
				}
				b = 1.0/(double)nal;
				a = 1.0/(double)this->data.nind[stat.samp];
				ind = 0;
			    for (i=0;i<this->locuslist[loc].ss[stat.samp];i++){
					nn=calploidy(loc,stat.samp,ind);
					ind++;
	    			switch (nn)
	    			{ case 1 :  ig1 = this->locuslist[loc].haplomic[stat.samp][i];
								if (ig1!=MISSING) {
									num_lik = round(this->locuslist[loc].freq[stat.samp1][ig1-this->locuslist[loc].kmin]*this->locuslist[loc].samplesize[stat.samp1])+b;
									den_lik = (double) (this->locuslist[loc].samplesize[stat.samp1]+1);
									lik -= a*log10(num_lik/den_lik);
								}
								break;
	    			  case 2 :  ig1 = this->locuslist[loc].haplomic[stat.samp][i];
								ig2 = this->locuslist[loc].haplomic[stat.samp][++i];
				                if ((ig1!=MISSING)and(ig2!=MISSING)) {
				                	if (ig1==ig2) {
				                		num_lik = 1.0+b+arrondi(this->locuslist[loc].freq[stat.samp1][ig1-this->locuslist[loc].kmin]*this->locuslist[loc].samplesize[stat.samp1]);
				                		num_lik *=b+arrondi(this->locuslist[loc].freq[stat.samp1][ig2-this->locuslist[loc].kmin]*this->locuslist[loc].samplesize[stat.samp1]);
				                	}
				                	else {
				                		num_lik = 2.0*(b+arrondi(this->locuslist[loc].freq[stat.samp1][ig1-this->locuslist[loc].kmin]*this->locuslist[loc].samplesize[stat.samp1]));
				                		num_lik *=b+arrondi(this->locuslist[loc].freq[stat.samp1][ig2-this->locuslist[loc].kmin]*this->locuslist[loc].samplesize[stat.samp1]);
				                	}
				                	den_lik = (double) ((this->locuslist[loc].samplesize[stat.samp1]+2)*(this->locuslist[loc].samplesize[stat.samp1]+1));
									lik -= a*log10(num_lik/den_lik);
				                }	
				                break;
	    			}
    			}
		    }
		}
		if (nl>0) return lik/(double)nl; else return 0.0;
	}

	double cal_dmu2p(StatC stat){
		double *moy,dmu2=0.0,s;
		int iloc,loc,pop,nl=0;
		moy = new double[2];
		for (iloc=0;iloc<this->grouplist[stat.group].nloc;iloc++){
			loc=this->grouplist[stat.group].loc[iloc];
		    if (this->locuslist[loc].samplesize[stat.samp]*this->locuslist[loc].samplesize[stat.samp1]>0) {
				nl++;
				for (int kpop=0;kpop<2;kpop++) {
					if (kpop==0) pop=stat.samp;	else pop=stat.samp1;
					s = 0.0;moy[kpop]=0.0;
					for (int i=0;i<this->locuslist[loc].ss[pop];i++) {
						if (this->locuslist[loc].haplomic[pop][i]!=MISSING) s += this->locuslist[loc].haplomic[pop][i];
					}
					moy[kpop]=s/(double)this->locuslist[loc].samplesize[pop];
				}
		    }
		    dmu2 += sqr((moy[1]-moy[0])/(double)this->locuslist[loc].motif_size);
		}
		delete []moy;
		if (nl>0) return dmu2/(double)nl; else return 0.0;
	}

	double cal_das2p(StatC stat){
		double s=0.0;
		int iloc,loc,nl=0;
		for (iloc=0;iloc<this->grouplist[stat.group].nloc;iloc++){
			loc=this->grouplist[stat.group].loc[iloc];
			for (int i=0;i<this->locuslist[loc].ss[stat.samp];i++) {
				if (this->locuslist[loc].haplomic[stat.samp][i]!=MISSING) {
					for (int j=0;j<this->locuslist[loc].ss[stat.samp1];j++) {
						if (this->locuslist[loc].haplomic[stat.samp1][j]!=MISSING) {
							nl++;
							if (this->locuslist[loc].haplomic[stat.samp][i] == this->locuslist[loc].haplomic[stat.samp1][j]) s+=1.0;
						}
					}
				}
			}
		}
		if (nl>0) return s/(double)nl; else return 0.0;
	}

	complex<double> pente_lik(StatC stat, int i0) {
		double a,freq1,freq2,li0,delta,*li;
		int ig1,ig2,ind,loc,iloc,nn;
		li = new double[2];
		for (int rep=0;rep<2;rep++) {
			a=0.001*(double)(i0+rep);li[rep]=0.0;
//			cout << "rep = " << rep << "   a = "<< a << "lik = "<< li[rep] << "\n";
			for (iloc=0;iloc<this->grouplist[stat.group].nloc;iloc++){
				loc=this->grouplist[stat.group].loc[iloc];
//			    cout << "\n\n locus "<< loc<< "\n";
			    ind=0;
			    for (int i=0;i<this->locuslist[loc].ss[stat.samp];) {
			    	nn = calploidy(loc,stat.samp,ind);
				ind++;
	    			switch (nn)
	    			{ case 1 :  ig1 = this->locuslist[loc].haplomic[stat.samp][i];i++;
								if (ig1!=MISSING) {
									freq1 = a*this->locuslist[loc].freq[stat.samp1][ig1-this->locuslist[loc].kmin]+(1.0-a)*this->locuslist[loc].freq[stat.samp2][ig1-this->locuslist[loc].kmin];
									if (freq1>0.0) li[rep] +=log(freq1);
								}
								break;
	    			  case 2 :  ig1 = this->locuslist[loc].haplomic[stat.samp][i];i++;
					        ig2 = this->locuslist[loc].haplomic[stat.samp][i];i++;
				                if ((ig1!=MISSING)and(ig2!=MISSING)) {
				                	if (ig1==ig2) {
				                		freq1 = a*this->locuslist[loc].freq[stat.samp1][ig1-this->locuslist[loc].kmin]+(1.0-a)*this->locuslist[loc].freq[stat.samp2][ig1-this->locuslist[loc].kmin];
				                		if (freq1>0.0) li[rep] +=log(sqr(freq1));
				                	}
				                	else {
				                		freq1 = a*this->locuslist[loc].freq[stat.samp1][ig1-this->locuslist[loc].kmin]+(1.0-a)*this->locuslist[loc].freq[stat.samp2][ig1-this->locuslist[loc].kmin];
				                		freq2 = a*this->locuslist[loc].freq[stat.samp1][ig2-this->locuslist[loc].kmin]+(1.0-a)*this->locuslist[loc].freq[stat.samp2][ig2-this->locuslist[loc].kmin];
				                		if (freq1*freq2>0.0) li[rep] +=log(2.0*freq1*freq2);
				                		else {
				                			if (freq1>0.0) li[rep] +=log(freq1);
				                			if (freq2>0.0) li[rep] +=log(freq2);
				                		}
				                	}
				                }	
				                break;
	    			}
//	    			cout << "lik = " << li[rep] << "\n";
    			}
			}
//			cout << "rep = " << rep << "   a = "<< a << "lik = "<< li[rep] << "\n";
		}
//		cout << "i0 = " <<i0 << "   li[i0] = "<< li[0] << "\n";
		li0=li[0];
		delta=li[1]-li[0];
		delete []li;
		return complex<double>(li0,delta);
	}		
		
	double cal_Aml3p(StatC stat){
		int i1=1,i2=998,i3;
		double p1,p2,p3,lik1,lik2,lik3;
		complex<double> c;
		c=pente_lik(stat,i1);lik1=real(c);p1=imag(c);
		c=pente_lik(stat,i2);lik2=real(c);p2=imag(c);
		if ((p1<0.0)and(p2<0.0)) return 0.0;
		if ((p1>0.0)and(p2>0.0)) return 1.0;
		do {
			i3 = (i1+i2)/2;
			c=pente_lik(stat,i3);lik3=real(c);p3=imag(c);
			if (p1*p3<0.0) {i2=i3;p2=p3;lik2=lik3;}
			else		   {i1=i3;p1=p3;lik1=lik3;}
		} while (abs(i2-i1)>1);	
		if (lik1>lik2) return 0.001*(double)i1; else return 0.001*(double)i2;
	}

	double cal_nha1p(StatC stat){
		double res=0.0;
		return res;

	}

	double cal_nss1p(StatC stat){
		double res=0.0;
		return res;

	}

	double cal_mpd1p(StatC stat){
		double res=0.0;
		return res;

	}

	double cal_vpd1p(StatC stat){
		double res=0.0;
		return res;

	}

	double cal_dta1p(StatC stat){
		double res=0.0;
		return res;

	}

	double cal_pss1p(StatC stat){
		double res=0.0;
		return res;

	}

	double cal_mns1p(StatC stat){
		double res=0.0;
		return res;

	}

	double cal_vns1p(StatC stat){
		double res=0.0;
		return res;

	}

	double cal_nha2p(StatC stat){
		double res=0.0;
		return res;

	}

	double cal_nss2p(StatC stat){
		double res=0.0;
		return res;

	}

	double cal_mpw2p(StatC stat){
		double res=0.0;
		return res;

	}

	double cal_mpb2p(StatC stat){
		double res=0.0;
		return res;

	}

	double cal_fst2p(StatC stat){
		double res=0.0;
		return res;

	}

	double cal_aml3p(StatC stat){
		double res=0.0;
		return res;

	}

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
	 *
	 */
	void docalstat() {
//		cout << "avant calfreq\n";
		calfreq();
//		cout << "apres calfreq\n";
		for (int st=0;st<this->nstat;st++) {
//			cout <<" calcul de la stat "<<st<<"   cat="<<this->stat[st].cat<<"   group="<<stat[st].group<<"\n";
			switch (this->stat[st].cat)
			{	case     1 : this->stat[st].val = cal_nal1p(this->stat[st]);break;
				case     2 : this->stat[st].val = cal_het1p(this->stat[st]);break;
				case     3 : this->stat[st].val = cal_var1p(this->stat[st]);break;
				case     4 : this->stat[st].val = cal_mgw1p(this->stat[st]);break;
				case     5 : this->stat[st].val = cal_Fst2p(this->stat[st]);break;
				case     6 : this->stat[st].val = cal_lik2p(this->stat[st]);break;
				case     7 : this->stat[st].val = cal_dmu2p(this->stat[st]);break;
				case     8 : this->stat[st].val = cal_nal2p(this->stat[st]);break;
				case     9 : this->stat[st].val = cal_het2p(this->stat[st]);break;
				case    10 : this->stat[st].val = cal_var2p(this->stat[st]);break;
				case    11 : this->stat[st].val = cal_das2p(this->stat[st]);break;
				case    12 : this->stat[st].val = cal_Aml3p(this->stat[st]);break;
				case    -1 : this->stat[st].val = cal_nha1p(this->stat[st]);break;
				case    -2 : this->stat[st].val = cal_nss1p(this->stat[st]);break;
				case    -3 : this->stat[st].val = cal_mpd1p(this->stat[st]);break;
				case    -4 : this->stat[st].val = cal_vpd1p(this->stat[st]);break;
				case    -5 : this->stat[st].val = cal_dta1p(this->stat[st]);break;
				case    -6 : this->stat[st].val = cal_pss1p(this->stat[st]);break;
				case    -7 : this->stat[st].val = cal_mns1p(this->stat[st]);break;
				case    -8 : this->stat[st].val = cal_vns1p(this->stat[st]);break;
				case    -9 : this->stat[st].val = cal_nha2p(this->stat[st]);break;
				case   -10 : this->stat[st].val = cal_nss2p(this->stat[st]);break;
				case   -11 : this->stat[st].val = cal_mpw2p(this->stat[st]);break;
				case   -12 : this->stat[st].val = cal_mpb2p(this->stat[st]);break;
				case   -13 : this->stat[st].val = cal_fst2p(this->stat[st]);break;
				case   -14 : this->stat[st].val = cal_aml3p(this->stat[st]);break;
			}
			//cout << "stat["<<st<<"]="<<this->stat[st].val<<"\n";
		}
	}

};

