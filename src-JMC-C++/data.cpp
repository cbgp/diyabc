/*
 * data.cpp
 *
 * Created on : 07 january 2010
 *     Author : cornuet
 *
 */


#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
//#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
using namespace std;

struct LocusC
{
	char* name;
	int type;  //0 à 9
	int groupe;    //numero du groupe auquel appartient le locus
	double coeff;  // coefficient pour la coalescence (dépend du type de locus et du sexratio)
	double **freq;
	int *ss;          //comptabilise toutes les "gene copies" données manquantes inclues
	int *samplesize;  //comptabilise les "gene copies" non-manquantes
//Proprietes des locus sequences
	double pi_A,pi_C,pi_G,pi_T;
	double *mutsit;   //array of dnalength elements giving the relative probability of a mutation to a given site of the sequence
	int nsitmut;   // length of array *sitmut;
	int *sitmut;   //array of nsitmut dna sites that are changed through a mutation
	int dnalength;
	int *tabsit;   //array of dnalength elements giving the number of a dna site;
	char ***haplodna;  //array[sample][gene copy][nucleotide] tous les nucleotides de chaque individu sont mis à la suite les uns des autres
//	int *haplostate; //array[gene copy] tous les "gene copies" sont mises à la suite les unes des autres (groupées par sample)
//Proprietes des locus microsatellites
	int mini,maxi,kmin,kmax,motif_size,motif_range,nal;
	double mut_rate,Pgeom,sni_rate,mus_rate,k1,k2;
	int **haplomic; //array[sample][gene copy]
};

struct MissingHaplo
{
	int locus,sample,indiv;
};

struct MissingNuc
{
	int locus,sample,indiv,nuc;
};

string majuscules(string s) {
	string s2;
	char *c;
	c = new char[s.length()+1];
	for (int i=0;i<s.length();i++) {
		c[i]=s[i];
		c[i]=toupper(c[i]);
	}
	s2 = string(c,s.length());
	delete [] c;
	return s2;
}

class DataC
{
public:
	string message,title,**indivname,***genotype;
	int nsample,nloc,nmisshap,nmissnuc;
	int *nind;
	int **indivsexe;
	double sexratio;
	MissingHaplo *misshap;
	MissingNuc   *missnuc;
	LocusC *locus;

	void libere(){
		for (int  ech=0;ech<this->nsample;ech++) delete [] this->indivname[ech];
		delete [] this->indivname;
		for (int  ech=0;ech<this->nsample;ech++) delete [] this->indivsexe[ech];
		delete [] this->indivsexe;
		for (int  ech=0;ech<this->nsample;ech++) {
			for (int ind=0;ind<this->nind[ech];ind++) {
				delete [] genotype[ech][ind];
			}
			delete [] genotype[ech];
		}
		delete [] genotype;
		if (this->nmisshap>0) delete [] this->misshap;
		if (this->nmissnuc>0) delete [] this->missnuc;

		for (int loc=0;loc<this->nloc;loc++){
			delete [] this->locus[loc].name;
			if (this->locus[loc].type<5) {
				for (int  ech=0;ech<this->nsample;ech++) delete [] this->locus[loc].haplomic[ech];
				delete [] this->locus[loc].haplomic;
			} else {
				for (int  ech=0;ech<this->nsample;ech++) {
					for (int ind=0;ind<this->nind[ech];ind++) delete [] this->locus[loc].haplodna[ech][ind];
					delete [] this->locus[loc].haplodna[ech];
				}
				delete [] this->locus[loc].haplodna;
				delete [] this->locus[loc].tabsit;
				delete [] this->locus[loc].mutsit;
				if (this->locus[loc].nsitmut>0) delete [] this->locus[loc].sitmut;
			}
			delete [] this->locus[loc].samplesize;
			delete [] this->locus[loc].ss;
		}
		delete [] this->locus;
		delete [] this->nind;
	}


	DataC* readfile(string filename){
		bool fin;
		string s,s1,s2,locusname;
		int ech,ind,nech,*nindi;
		char c;
		size_t j,j0,j1;
		stringstream out;
		ifstream file(filename.c_str(), ios::in);
		if (file == NULL) {
			this->message = "File "+filename+" not found";
			return this;
		} else this->message="";
		getline(file,this->title);
		j0=title.find("<NM=");
		if (j0>0) {
			j1=title.find("NF>",j0+3);
			s=title.substr(j0+4,j1-(j0+4));
			this->sexratio=atof(s.c_str())/(1.0+atof(s.c_str()));
		} else this->sexratio=0.5;
		this->nloc=0;fin=false;
		while (not fin) {
			getline(file,s);
			s1=majuscules(s);
			if (s1.find("POP")==string::npos) this->nloc +=1;	//il s'agit d'un nom de locus
			else fin=true;
		}
		this->locus = new LocusC [this->nloc];
		this->nsample=1;
		nindi = new int[1000];
		nindi[0]=0;
	    getline(file,s);
	    nech=1;
	    this->nmisshap=0;
		this->nmissnuc=0;
	    while (not file.eof()) {
	    	if (s!="") {
				s1=majuscules(s);
//				cout << s1<<"\n";
				if (s1.find("POP")==string::npos) {
					nindi[nech-1] +=1;
					s2=s1.substr(s1.find(","),s1.length());
	//COMPTAGE DES DONNEES MANQUANTES
					j=s2.find("000");
					while (j!=string::npos) {
						this->nmisshap +=1;
						j=s2.find("000",j+3);
					}
					j=s2.find("[]");
					while (j!=string::npos) {
						this->nmisshap +=1;
						j=s2.find("[]",j+2);
					}
					j=s2.find("N");
					while (j!=string::npos) {
						this->nmissnuc +=1;
						j=s2.find("N",j+1);
					}
					j=s2.find("-");
					while (j!=string::npos) {
						this->nmissnuc +=1;
						j=s2.find("-",j+1);
					}
				}
	    	    else {nech +=1;nindi[nech-1]=0;}
	    	}
	    	getline(file,s);
		}
		file.close();
		this->nsample=nech;
		this->nind = new int[nech];
		for (int i=0;i<nech;i++) {this->nind[i]=nindi[i];}
		if (this->nmisshap>0) this->misshap = new MissingHaplo[this->nmisshap];
		if (this->nmissnuc>0) this->missnuc = new MissingNuc[this->nmissnuc];
	    this->nmisshap=0;
		this->nmissnuc=0;
		this->indivname = new string*[nech];
		this->indivsexe = new int*[nech];
		this->genotype = new string**[nech];
		for (int i=0;i<nech;i++) {
			this->indivname[i]= new string[nind[i]];
			this->indivsexe[i] = new int[nind[i]];
			for(j=0;j<this->nind[i];j++) this->indivsexe[i][j] = 2;
			this->genotype[i] = new string*[nind[i]];
			for(j=0;j<this->nind[i];j++) this->genotype[i][j] = new string[this->nloc];
		}
///////////
		ifstream file2(filename.c_str(), ios::in);
		getline(file2,s);
		for (int loc=0;loc<this->nloc;loc++) {
			getline(file2,s);
			j=s.find("<");
			if (j!=string::npos) {         //il y a un type de locus noté après le nom
			s2=majuscules(s.substr(j+1,1));
//			cout << "s2="<<s2<<"\n";
			if (s2=="A") this->locus[loc].type=0; else
				if (s2=="H") this->locus[loc].type=1; else
					if (s2=="X") this->locus[loc].type=2; else
						if (s2=="Y") this->locus[loc].type=3; else
							if (s2=="M") this->locus[loc].type=4; else
								{out <<loc+1;
								 this->message="unrecognized type at locus "+out.str();
								 return this;
								}
				s=s.substr(0,j-1);
			} else this->locus[loc].type=0;
			this->locus[loc].name = new char[s.length()+1];
			strncpy(this->locus[loc].name, s.c_str(), s.length());
			this->locus[loc].name[s.length()]='\0';
		}
		for (ech=0;ech<this->nsample;ech++) {
			getline(file2,s);  //ligne "POP"
//			cout << s <<"\n";
			for (ind=0;ind<this->nind[ech];ind++) {
				getline(file2,s);
				j=s.find(",");
				this->indivname[ech][ind] = s.substr(0,j);
				s = s.substr(j+1,s.length());
			    	istringstream iss(s);
				for (int i=0;i<this->nloc;i++) {
					iss >> this->genotype[ech][ind][i];
					if ((this->genotype[ech][ind][i].find("[")!=string::npos)and(this->locus[i].type<5)) this->locus[i].type +=5;
//					cout << this->genotype[ech][ind][i] << "   "<<this->locus[i].type<<"\n";
				}
			}
		}
		file2.close();
		delete [] nindi;
	}

    void do_microsat(int loc){
    	string geno,*gen;
    	int l,ll,n,gg;
    	gen = new string[2];
    	this->locus[loc].mini=1000;this->locus[loc].maxi=0;
    	this->locus[loc].haplomic = new int*[this->nsample];
    	for (int ech=0;ech<this->nsample;ech++) {
    		this->locus[loc].haplomic[ech] = new int[2*this->nind[ech]];
    	}
    	this->locus[loc].ss = new int[this->nsample];
    	this->locus[loc].samplesize = new int[this->nsample];
    	for (int ech=0;ech<this->nsample;ech++){
    		this->locus[loc].ss[ech] = 0;
    		this->locus[loc].samplesize[ech] = 0;
    		for (int ind=0;ind<this->nind[ech];ind++){
    			geno=string(this->genotype[ech][ind][loc]);
    			l=geno.length();
    			if (l>3){n=2;} else {n=1;}
    			if (n==2) {
    				ll=l/2;
    				gen[0]=geno.substr(0,ll);
    				gen[1]=geno.substr(ll,ll);
    				this->locus[loc].ss[ech] +=2;
    			}
    			else {
    				gen[0]=geno;
    				if (this->locus[loc].type==2) this->indivsexe[ech][ind]=1;
    			    if ((this->locus[loc].type==3)and(geno!="000")) this->indivsexe[ech][ind]=1;
    			    this->locus[loc].ss[ech] +=1;
    			}
    			for (int i=0;i<n;i++) {
    				if (gen[i]!="000") {
    					this->locus[loc].samplesize[ech] +=1;
    					gg = atoi(gen[i].c_str());
    					this->locus[loc].haplomic[ech][this->locus[loc].samplesize[ech]-1] = gg;
    					if (gg>this->locus[loc].maxi) this->locus[loc].maxi=gg;
    					if (gg<this->locus[loc].mini) this->locus[loc].mini=gg;

    				} else {
    					if (not ((this->indivsexe[ech][ind]==2)and(this->locus[loc].type==3))) { // on ne prend pas en compte les chromosomes Y des femelles
							this->nmisshap +=1;
							this->misshap[this->nmisshap-1].sample=ech;
							this->misshap[this->nmisshap-1].locus=loc;
							this->misshap[this->nmisshap-1].indiv=ind;
    					}
    				}
    			}
    		}
    	}
    	delete [] gen;
    }


    void do_sequence(int loc){
    	string geno,*gen;
    	int l,ll,n,gg,j0,j1,j2;
    	gen = new string[2];
    	string sbase;
    	char base[] ="ACGT";
    	this->locus[loc].haplodna = new char**[this->nsample];
    	for (int ech=0;ech<this->nsample;ech++) {
    		this->locus[loc].haplodna[ech] = new char*[2*this->nind[ech]];
    	}
    	this->locus[loc].ss = new int[this->nsample];
    	this->locus[loc].samplesize = new int[this->nsample];
    	this->locus[loc].dnalength = -1;
    	for (int ech=0;ech<this->nsample;ech++){
    		this->locus[loc].ss[ech] = 0;
    		this->locus[loc].samplesize[ech] = 0;
    		for (int ind=0;ind<this->nind[ech];ind++){
    			geno=this->genotype[ech][ind][loc];
    			if (geno.find("][")==string::npos) n=1; else n=2;
    			if (n==2) {
    				j0=geno.find("[")+1;
    				j1=geno.find("][");
    				j2=geno.find("]",j1+2);
    				if ((this->locus[loc].dnalength<0)and(j1-j0>0)) this->locus[loc].dnalength=j1-j0;
    				if ((this->locus[loc].dnalength>0)and ((j1-j0!=this->locus[loc].dnalength)or(j2-(j1+2)!=this->locus[loc].dnalength))) {
    					std::stringstream ss;
    					ss <<"ERROR : At locus "<<loc+1<<" indivividual "<<this->indivname[ech][ind]<<", the sequence length has changed. Please, give it the same length as before.";
    					this->message=ss.str();
    					return;
    				}
    				gen[0]=geno.substr(j0,j1-j0);
    				gen[1]=geno.substr(j1+2,j2-(j1+2));
    				//cout <<indivname[ech][ind]<<"  n=2     "<<gen[0]<<"   "<<gen[1]<<"\n";
    				this->locus[loc].ss[ech] +=2;
    			}else {
    				j0=geno.find("[")+1;
    				j1=geno.find("]");
    				if ((this->locus[loc].dnalength<0)and(j1-j0>0)) this->locus[loc].dnalength=j1-j0;
    				if ((this->locus[loc].dnalength>0)and (j1-j0!=this->locus[loc].dnalength)and(j1>j0)) {
    					std::stringstream ss;
    					ss <<"ERROR : At locus "<<loc+1<<" indivividual "<<this->indivname[ech][ind]<<", the sequence length has changed. Please, give it the same length as before.";
    					this->message=ss.str();
    					return;
    				}
    				gen[0]=geno.substr(j0,j1-j0);
    				this->locus[loc].ss[ech] +=1;
    				if (this->locus[loc].type==2) this->indivsexe[ech][ind]=1;
    			    if ((this->locus[loc].type==3)and(geno!="[]")) this->indivsexe[ech][ind]=1;

    			}
    			for (int i=0;i<n;i++) {
    				if (gen[i]!="") {
    					this->locus[loc].samplesize[ech] +=1;
    					this->locus[loc].haplodna[ech][this->locus[loc].samplesize[ech]-1] = new char[gen[i].length()+1];
    					strncpy(this->locus[loc].haplodna[ech][this->locus[loc].samplesize[ech]-1], gen[i].c_str(), gen[i].length());
    					j0=min(gen[i].find("-"),gen[i].find("N"));
    					while (j0!=string::npos) {
    						this->nmissnuc +=1;
    						this->missnuc[this->nmissnuc-1].sample=ech;
    						this->missnuc[this->nmissnuc-1].locus=loc;
   						    this->missnuc[this->nmissnuc-1].indiv=ind;
    						this->missnuc[this->nmissnuc-1].nuc=j0;
    						j0=min(gen[i].find("-",j0+1),gen[i].find("N",j0+1));
    					}
    				} else {
    					if (not ((this->indivsexe[ech][ind]==2)and(this->locus[loc].type==8))) { // on ne prend pas en compte les chromosomes Y des femelles
            				cout <<indivname[ech][ind]<<"donnée manquante"<<"   nmisshap="<<this->nmisshap<<  "\n";
							this->nmisshap +=1;
							this->misshap[this->nmisshap-1].sample=ech;
							this->misshap[this->nmisshap-1].locus=loc;
							this->misshap[this->nmisshap-1].indiv=ind;
    					}
    				}
    			}
    			for (int i=this->locus[loc].samplesize[ech];i<this->locus[loc].ss[ech];i++) this->locus[loc].haplodna[ech][i] = new char[1];
    		}
    	}
		this->locus[loc].pi_A=0.0;this->locus[loc].pi_C=0.0;this->locus[loc].pi_G=0.0;this->locus[loc].pi_T=0.0;
		double nn=0.0;
    	for (int ech=0;ech<this->nsample;ech++){
    		for (int i=0;i<this->locus[loc].samplesize[ech];i++){
    			for (int j=0;j<this->locus[loc].dnalength;j++) {
    				if (this->locus[loc].haplodna[ech][i][j]==base[0]) {this->locus[loc].pi_A+=1.0;nn+=1.0;}
    				else if (this->locus[loc].haplodna[ech][i][j]==base[1]) {this->locus[loc].pi_C+=1.0;nn+=1.0;}
    				else if (this->locus[loc].haplodna[ech][i][j]==base[2]) {this->locus[loc].pi_G+=1.0;nn+=1.0;}
    				else if (this->locus[loc].haplodna[ech][i][j]==base[3]) {this->locus[loc].pi_T+=1.0;nn+=1.0;}
    			}
    		}
    	}
    	if (n>0.0) {this->locus[loc].pi_A /=nn;this->locus[loc].pi_C /=nn;this->locus[loc].pi_G /=nn;this->locus[loc].pi_T /=nn;}
    	this->locus[loc].nsitmut=0;
    	this->locus[loc].tabsit = new int[this->locus[loc].dnalength];
    	this->locus[loc].mutsit = new double[this->locus[loc].dnalength];
    	delete [] gen;
    }

    void cal_coeff(int loc){
		double coeff=0.0;
		switch (this->locus[loc].type % 5)
		{	case 0 :  coeff = 16.0*this->sexratio*(1.0-this->sexratio);break;
			case 1 :  coeff = 2.0;break;
			case 2 :  coeff = 18.0*this->sexratio*(1.0-this->sexratio)/(1.0+this->sexratio);break;
			case 3 :  coeff = 2.0*this->sexratio;break;
			case 4 :  coeff = 2.0*(1.0-this->sexratio);break;
		}
		this->locus[loc].coeff=coeff;
    }

    DataC * loadfromfile(string filename) {
		int loc,kloc;
		this->readfile(filename);
		kloc=this->nloc;
		for (loc=0;loc<kloc;loc++) {
			//cout << "locus "<<loc<<"   type = "<<this->locus[loc].type<<"\n";
			if (this->locus[loc].type<5) this->do_microsat(loc);
			else                         this->do_sequence(loc);
			this->cal_coeff(loc);
		}
	}
};

/*int main(){
	DataC data;
	data.loadfromfile("tout.txt");
	cout <<"\n\n";
	for (int ech=0;ech<data.nsample;ech++) {
		for (int ind=0;ind<data.nind[ech];ind++) {
			cout << data.indivname[ech][ind]<<"   ";
			for (int loc=0;loc<data.nloc;loc++) cout <<data.genotype[ech][ind][loc]<<"  ";
			cout <<"\n";
		}
	}
	cout << data.title << "\n nloc = "<<data.nloc<<"   nsample = "<<data.nsample<<"\n";
	for (int i=0;i<data.nloc;i++) cout << data.locus[i].name <<"\n";
	for (int ech=0;ech<data.nsample;ech++) {
		cout << data.nind[ech] <<"\n";
		for (int ind=0;ind<data.nind[ech];ind++) cout << data.indivname[ech][ind]<<"   sexe = "<<data.indivsexe[ech][ind]<<"\n";
	}
	for (int loc=0;loc<data.nloc;loc++) {
		if (data.locus[loc].type<5)
		cout << data.locus[loc].name << "   mini = " <<data.locus[loc].mini<<"   maxi = "<<data.locus[loc].maxi<<"\n";
	}
	for (int i=0;i<data.nmisshap;i++) cout <<" missing data sample "<<data.misshap[i].sample<<"  ind "<<data.misshap[i].indiv<<"  locus "<<data.misshap[i].locus<<"\n";
	for (int i=0;i<data.nmissnuc;i++) cout <<" missing nucl sample "<<data.missnuc[i].sample<<"  ind "<<data.missnuc[i].indiv<<"  locus "<<data.missnuc[i].locus<<"  nuc "<<data.missnuc[i].nuc<<"\n";
	data.libere();
}*/
