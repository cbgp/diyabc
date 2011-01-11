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
//Proprietes des locus sequences
	double pi_A,pi_C,pi_G,pi_T;
	double *mutsit;   //array of dnalength elements giving the relative probability of a mutation to a given site of the sequence
	int *sitmut;   //array of dna sites that are changed through a mutation
	int dnalength;
	int *tabsit;
	string **haplodna;  //array[sample][gene copy][nucleotide] tous les nucleotides de chaque individu sont mis à la suite les uns des autres
//	int *haplostate; //array[gene copy] tous les "gene copies" sont mises à la suite les unes des autres (groupées par sample)
//Proprietes des locus microsatellites
	int mini,maxi,kmin,kmax,motif_size,motif_range,nal;
	double mut_rate,Pgeom,sni_rate,mus_rate,k1,k2;
	int **haplomic; //array[sample][gene copy]
	int *ssmic;
	int *samplesize;
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
		free(this->misshap);
//		free(this->missnuc);
		for (int loc=0;loc<this->nloc;loc++){
//			delete [] this->locus[loc].name;
			if (this->locus[loc].type<5) {
				for (int  ech=0;ech<this->nsample;ech++) free(this->locus[loc].haplomic[ech]);
				free(this->locus[loc].haplomic);
			} else {
				for (int  ech=0;ech<this->nsample;ech++)free(this->locus[loc].haplodna[ech]);
				free(this->locus[loc].haplodna);
				delete [] this->locus[loc].tabsit;
				delete [] this->locus[loc].sitmut;
			}
			delete [] this->locus[loc].samplesize;
			delete [] this->locus[loc].ssmic;
		}
		free(this->locus);
	}


	DataC* readfile(string filename){
		bool fin=false;
		string s,s1,s2,locusname;
		int ech,ind,nech,*nind;
		char c;
		size_t j;
		stringstream out;
		ifstream file(filename.c_str(), ios::in);
		if (file == NULL) {
			this->message = "File not found";
			return this;
		}
		getline(file,this->title);
		this->nloc=0;
		while (not fin) {
			getline(file,s);
			s1=majuscules(s);
			if (s1.find("POP")==string::npos){	//il s'agit d'un nom de locus
				this->nloc +=1;
				if (this->nloc==1) this->locus = (LocusC*) malloc(sizeof(LocusC));
				else		        this->locus = (LocusC*) realloc(this->locus,this->nloc*sizeof(LocusC));
				j=s.find("<");
				if (j!=string::npos) {         //il y a un type de locus noté après le nom
					s2=majuscules(s.substr(j+1,1));
					if (s2=="A") this->locus[this->nloc-1].type=0; else
						if (s2=="H") this->locus[this->nloc-1].type=1; else
							if (s2=="X") this->locus[this->nloc-1].type=2; else
								if (s2=="Y") this->locus[this->nloc-1].type=3; else
									if (s2=="M") this->locus[this->nloc-1].type=4; else
										{out << this->nloc;
										 this->message="unrecoknized type at locus "+out.str();
										 return this;
										}
					s=s.substr(0,j-1);
				} else this->locus[this->nloc-1].type=0;
				this->locus[this->nloc-1].name = new char[s.length()+1];
				strncpy(this->locus[this->nloc-1].name, s.c_str(), s.length()+1);
			} else fin=true;
		}
		nech=1;
		nind = (int*) malloc(sizeof(int));
		nind[0]=0;
	    	getline(file,s);
	    while (not file.eof()) {
	    	s1=majuscules(s);
	    	if (s1.find("POP")==string::npos){nind[nech-1] +=1;}
	    	else {nech +=1;nind = (int*)realloc(nind,nech*sizeof(int));nind[nech-1]=0;}
	    	getline(file,s);
		}
		file.close();
		this->nsample=nech;
		this->nind = new int[nech];
		for (int i=0;i<nech;i++) {this->nind[i]=nind[i];}
		this->indivname = new string*[nech];
		this->indivsexe = new int*[nech];
		this->genotype = new string**[nech];
		for (int i=0;i<nech;i++) {
			this->indivname[i]= new string[nind[i]];
			this->indivsexe[i] = new int[nind[i]];
			this->genotype[i] = new string*[nind[i]];
			for(j=0;j<this->nind[i];j++) this->genotype[i][j] = new string[this->nloc];
		}
		ifstream file2(filename.c_str(), ios::in);
		for (int i=0;i<this->nloc+1;i++) {getline(file2,s);}
		for (ech=0;ech<this->nsample;ech++) {
			getline(file2,s);  //ligne "POP"
			for (ind=0;ind<this->nind[ech];ind++) {
				getline(file2,s);
				j=s.find(",");
				this->indivname[ech][ind] = s.substr(0,j);
				s = s.substr(j+1,s.length());
			    istringstream iss(s);
				for (int i=0;i<this->nloc;i++) {
					iss >> this->genotype[ech][ind][i];
					if ((this->genotype[ech][ind][i].find("[")!=string::npos)and(this->locus[i].type<5)) this->locus[i].type +=5;
				}
			}
		}
		file2.close();
		free(nind);
	}

    void do_microsat(int loc){
    	cout << "je regarde le locus 2  "<<this->genotype[0][2][2]<<"\n";
    	cout <<"locus "<<loc<<"\n";
    	string geno,*gen;
    	int l,ll,n,gg;
    	gen = new string[2];
    	this->locus[loc].mini=1000;this->locus[loc].maxi=0;
    	this->locus[loc].haplomic =(int**)malloc(this->nsample*sizeof(int*));
    	this->locus[loc].ssmic = new int[this->nsample];
    	this->locus[loc].samplesize = new int[this->nsample];
    	for (int ech=0;ech<this->nsample;ech++){
//    		cout <<"sample "<<ech<<"\n";
    		this->locus[loc].ssmic[ech] = 0;
    		this->locus[loc].samplesize[ech] = 0;
    		for (int ind=0;ind<this->nind[ech];ind++){
    			cout <<"avant geno"<< this->genotype[ech][ind][loc];
    			geno=string(this->genotype[ech][ind][loc]);
    			cout <<"   geno = "<<geno<<"\n";
    			l=geno.length();
    			if ((loc==2)and(ech==0)and(ind<5)) cout << ind <<"   "<<this->genotype[ech][ind][loc]<<"\n";
    			if (l>3){n=2;} else {n=1;}
    			if (n==2) {
    				ll=l/2;
    				gen[0]=geno.substr(0,ll);
    				gen[1]=geno.substr(ll,ll);
    				this->locus[loc].ssmic +=2;
    			}
    			else {
    				gen[0]=geno;
    				if (this->locus[loc].type==2) this->indivsexe[ech][ind]=1;
    			    if ((this->locus[loc].type==2)and(geno!="000")) this->indivsexe[ech][ind]=1;
    			    this->locus[loc].ssmic +=1;
    			}
    			for (int i=0;i<n;i++) {
    				if (gen[i]!="000") {
    					this->locus[loc].samplesize[ech] +=1;
    					gg = atoi(gen[i].c_str());
    					if (this->locus[loc].samplesize[ech] ==1) this->locus[loc].haplomic[ech]=(int*)malloc(sizeof(int));
    					else                                      this->locus[loc].haplomic[ech]=(int*)realloc(this->locus[loc].haplomic[ech],this->locus[loc].samplesize[ech]*sizeof(int));
    					this->locus[loc].haplomic[ech][this->locus[loc].samplesize[ech]-1] = gg;
    					if (gg>this->locus[loc].maxi) this->locus[loc].maxi=gg;
    					if (gg<this->locus[loc].mini) this->locus[loc].mini=gg;
    					if (gg==0) {cout <<"sample "<<ech<<"   ind "<<ind<< "   i="<<i<<"   geno = "<<geno<<"  gen[0]="<<gen[0]<<"   gen[1]="<<gen[1]<<"   l="<<l<<"   n="<<n<<"\n";}

    				} else {
    					this->nmisshap +=1;
    					if (this->nmisshap==1) this->misshap = (MissingHaplo*)malloc(sizeof(MissingHaplo));
    					else                   this->misshap = (MissingHaplo*)realloc(this->misshap,this->nmisshap*sizeof(MissingHaplo));
    					this->misshap[this->nmisshap-1].sample=ech;
    					this->misshap[this->nmisshap-1].locus=loc;
    					this->misshap[this->nmisshap-1].indiv=ind;
    				}
    			}
    			//if (ind==0)cout<<this->locus[loc].haplomic[ech][0]<<this->locus[loc].haplomic[ech][1]<<"\n";
    		}

    	}
    }


    void do_sequence(int loc){

    }

	DataC * loadfromfile(string filename) {
		this->readfile(filename);
		for (int ech=0;ech<this->nsample;ech++) {
			for (int ind=0;ind<this->nind[ech];ind++) {
				cout << this->indivname[ech][ind]<<"   ";
				for (int loc=0;loc<this->nloc;loc++) cout <<this->genotype[ech][ind][loc]<<"  ";
				cout <<"\n";
			}
		}
//		this->nmisshap=0;
//		this->nmissnuc=0;
		for (int loc=0;loc<this->nloc;loc++) {
			if (this->locus[loc].type<5) this->do_microsat(loc);
			else                         this->do_sequence(loc);
		}
		cout <<"\n\n";
		for (int ech=0;ech<this->nsample;ech++) {
			for (int ind=0;ind<this->nind[ech];ind++) {
				cout << this->indivname[ech][ind]<<"   ";
				for (int loc=0;loc<this->nloc;loc++) cout <<this->genotype[ech][ind][loc]<<"  ";
				cout <<"\n";
			}
		}
		for (int  ech=0;ech<this->nsample;ech++) {
			for (int ind=0;ind<this->nind[ech];ind++) {
				delete [] genotype[ech][ind];
			}
			delete [] genotype[ech];
		}
		delete [] genotype;
	}
};

int main(){
	DataC data;
	data.loadfromfile("datatest1.txt");
//	data.readfile("datatest1.txt");
	cout << data.title << "\nnloc = "<<data.nloc<<"\n";
	for (int i=0;i<data.nloc;i++) cout << data.locus[i].name <<"\n";
	for (int i=0;i<data.nsample;i++) cout << data.nind[i] <<"\n";
	for (int loc=0;loc<data.nloc;loc++) cout << data.locus[loc].name << "   mini = " <<data.locus[loc].mini<<"   maxi = "<<data.locus[loc].maxi<<"\n";
//	data.libere();
}
