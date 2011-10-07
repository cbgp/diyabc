/*
 * data.cpp
 *
 * Created on : 07 january 2010
 *     Author : cornuet
 *
 */

#ifndef FSTREAM
#include <fstream>
#define FSTREAM
#endif
#ifndef IOSTREAM
#include <iostream>
#define IOSTREAM
#endif
#ifndef STRING
#include <string>
#define STRING
#endif
#ifndef CSTRING
#include <cstring>
#define CSTRING
#endif
#ifndef SSTREAM
#include <sstream>
#define SSTREAM
#endif
#ifndef STDLIBH
#include <stdlib.h>
#define STDLIBH
#endif
#ifndef CTYPEH
#include <ctype.h>
#define CTYPEH
#endif
#ifndef MESUTILS
#include "mesutils.cpp"
#define MESUTILS
#endif
#ifndef VECTOR
#include <vector>
#define VECTOR
#endif


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
	vector <double> mutsit;   //array of dnalength elements giving the relative probability of a mutation to a given site of the sequence
	vector <int> sitmut;   //array of nsitmut dna sites that are changed through a mutation
	vector <int> sitmut2;  //
	int dnalength,dnavar;
	vector <int> tabsit;   //array of dnalength elements giving the number of a dna site;
	//char ***haplodna;  //array[sample][gene copy][nucleotide] tous les nucleotides de chaque individu sont mis à la suite les uns des autres
	//char ***haplodnavar; //seulement les sites variables
    string **haplodna;
    string **haplodnavar;
//	int *haplostate; //array[gene copy] tous les "gene copies" sont mises à la suite les unes des autres (groupées par sample)
//Proprietes des locus microsatellites
	int mini,maxi,kmin,kmax,motif_size,motif_range,nal;
	double mut_rate,Pgeom,sni_rate,mus_rate,k1,k2;
	int **haplomic; //array[sample][gene copy]
	
	void libere(bool obs, int nsample) {
      //cout<<"debut  nsample="<<nsample<<"\n";
       delete []this->name;
       //cout<<"apres delete name\n";
       delete []this->ss;
       //cout<<"apres delete ss\n";
       //delete []this->samplesize;
       //cout<<"apres delete samplesize\n";
       if (this->type<5) {
          /*for (int i=0;i<nsample;i++) delete []this->haplomic[i];
          delete []this->haplomic;*/
       } else {
           if (not obs) {
              if (not mutsit.empty()) mutsit.clear();
              if (not tabsit.empty())tabsit.clear();
           }
       }
       //cout<<"fin\n";
    }
};

struct MissingHaplo
{
	int locus,sample,indiv;
};

struct MissingNuc
{
	int locus,sample,indiv,nuc;
};

class DataC
{
public:
	string message,title,**indivname,***genotype;
	int nsample,nloc,nmisshap,nmissnuc,filetype;
	int *nind;
	int **indivsexe;
	double sexratio;
	MissingHaplo *misshap;
	MissingNuc   *missnuc;
	LocusC *locus;
    bool Aindivname,Agenotype,Anind,Aindivsexe,Alocus;
        
/**
* liberation de la mémoire occupée par la classe DataC
*/
	void libere(){
                if (Aindivname) {
		    for (int  ech=0;ech<this->nsample;ech++) delete [] this->indivname[ech];
		    delete [] this->indivname;
                    //cout<<"delete indivname\n";
                }
                if (Aindivsexe) {
		    for (int  ech=0;ech<this->nsample;ech++) delete [] this->indivsexe[ech];
		    delete [] this->indivsexe;
                    //cout<<"delete indivsexe\n";
                }
                if (Agenotype) {
                    for (int  ech=0;ech<this->nsample;ech++) {
                            for (int ind=0;ind<this->nind[ech];ind++) {
                                    delete [] this->genotype[ech][ind];
                                    //cout <<"delete genotype[ech][ind]\n";
                            }
                            delete [] this->genotype[ech];
                            //cout <<"delete genotype[ech]\n";
                    }
                    delete [] this->genotype;
                    //cout<<"delete genotype\n";
                }
                //cout<<"apres delete genotype\n";
		if (this->nmisshap>0) delete [] this->misshap;
		if (this->nmissnuc>0) delete [] this->missnuc;
                //cout<<"apres delete misshap et missnuc\n";
                if (Alocus) {
                    for (int loc=0;loc<this->nloc;loc++) this->locus[loc].libere(true,this->nsample);
                    delete [] this->locus;
                }
		if (Anind) delete [] this->nind;
	}

/**
* détermination du type de fichier de donnée 
* return=-1 
* return=0 si genepop
* return=1 si snp
*/ 
	int testfile(string filename){
	    int nss;
	    string ligne,*ss;
		ifstream file(filename.c_str(), ios::in);
		if (file == NULL) return -1;
		getline(file,ligne);
        ss=splitwords(ligne," ",&nss);
		if ((ss[0]=="IND")and(ss[1]=="POP")and (nss>100)) {
		  cout<<"Fichier SNP\n";
		  return 1;
		}
		return 0;
	}

/**
* lecture d'un fichier de donnée SNP et stockage des informations dans une structure DataC
*/
	DataC* readfilesnp(string filename){
		int ech,ind,nech,*nindi,nss;
		bool deja;
		string s1,*ss;
		vector <string> popname;
		ifstream file(filename.c_str(), ios::in);
		getline(file,s1);
        ss=splitwords(s1," ",&nss);
		this->nloc=nss-2;
		this->locus = new LocusC[this->nloc];
		for (int loc=0;loc<this->nloc;loc++) this->locus[loc].name  = strdup(ss[loc+2].c_str());
//recherche du nombre d'échantillons
		nech=1;popname.resize(nech);
		getline(file,s1);
		delete[]ss;
		ss=splitwords(s1," ",&nss);
		popname[nech-1]=ss[1];
		while (not file.eof()) {
			getline(file,s1);
			delete[]ss;
			ss=splitwords(s1," ",&nss);
			deja=false;
			for (int n=0;n<nech;n++) {deja=(ss[1]==popname[n]);if (deja) break;}
			if (not deja) {
			    nech++;
				popname.resize(nech);
				popname[nech-1]=ss[1];
			}
		}
		this->nsample = nech;
		this->nind = new int[nech];
//recherche du nombre d'individus par échantillon
		nindi = new int[nech];
		for (ech=0;ech<nech;ech++) nindi[ech]=0;
		file.seekg (0, ios::beg);
		getline(file,s1);
 		while (not file.eof()) {
			getline(file,s1);
			delete[]ss;
			ss=splitwords(s1," ",&nss);
			ech=0;while (ss[1]!=popname[ech]) ech++;
			nindi[ech]++;
		}
		for (ech=0;ech<nech;ech++) this->nind[ech] = nindi[ech];
//remplissage des noms et des génotypes des individus
		this->indivname = new string*[nech];
		for (ech=0;ech<nech;ech++) this->indivname[ech] = new string[nindi[ech]];
		this->genotype = new string**[nech];
		for (ech=0;ech<nech;ech++) {
			this->genotype[ech] = new string*[nindi[ech]];
			for (ind=0;ind<nindi[ech];ind++) this->genotype[ech][ind] = new string[nloc];
		}
		file.seekg (0, ios::beg);
		getline(file,s1);
		for (ech=0;ech<nech;ech++) nindi[ech]=0;
 		while (not file.eof()) {
			getline(file,s1);
			delete[]ss;
			ss=splitwords(s1," ",&nss);
			ech=0;while (ss[1]!=popname[ech]) ech++;
			this->indivname[ech][nindi[ech]]=ss[1];
			for (int loc=0;loc<this->nloc;loc++) this->genotype[ech][nindi[ech]][loc]= ss[loc+2];
			nindi[ech]++;
		}
		file.close();
		delete []nindi;delete []ss;
	}
/**
* supprime les locus monomorphes
*/
	void purgelocmonomorphes(){
		bool *mono;
		int ind,ech,kloc=0,nloc=0;
		string ***ge;
		string premier;
		mono = new bool[this->nloc];
		for (int loc=0;loc<this->nloc;loc++) {
			premier=this->genotype[0][0][loc];
			if (premier=="1") mono[loc]=false;
			else {
				mono[loc]=true;
				for (int ech=0;ech<this->nsample;ech++) {
					for (int ind=0;ind<this->nind[ech];ind++) {
						mono[loc]=(this->genotype[ech][ind][loc]==premier);
						if (not mono[loc]) break;
					}
					if (not mono[loc]) break;
				}
			}
			if (not mono[loc]) nloc++;
		}
		if (nloc<this->nloc){
			ge = new string**[this->nsample];
			for (ech=0;ech<this->nsample;ech++) {
				ge[ech] = new string*[this->nind[ech]];
				for (ind=0;ind<this->nind[ech];ind++) ge[ech][ind] = new string[nloc];
			}
			for (int loc=0;loc<this->nloc;loc++) {
				if (not mono[loc]) {
					for (ech=0;ech<this->nsample;ech++) {
						for (ind=0;ind<this->nind[ech];ind++) ge[ech][ind][kloc] = this->genotype[ech][ind][loc];
					}
					kloc++;
				} 
			}
			for (ech=0;ech<this->nsample;ech++) {
				for (ind=0;ind<this->nind[ech];ind++){ 
					delete[]this->genotype[ech][ind];
					this->genotype[ech][ind] = new string[kloc];
					for (int loc=0;loc<kloc;loc++) this->genotype[ech][ind][loc] = ge[ech][ind][loc];
				}
			}
			for (ech=0;ech<this->nsample;ech++) {
				for (ind=0;ind<this->nind[ech];ind++) delete[]ge[ech][ind];
			    delete[]ge[ech];
			}
			delete[]ge;
		}
	}
	
	
/**
* traitement des locus snp
*/
  void do_snp(int loc) {
	
	
  }


/**
* lecture d'un fichier de donnée GENEPOP et stockage des informations dans une structure DataC
*/
	DataC* readfile(string filename){
		bool fin;
		string s,s1,s2,locusname;
		int ech,ind,nech,*nindi;
		char c;
		size_t j,j0,j1;
		Aindivname=false;Agenotype=false;Anind=false;Aindivsexe=false;Alocus=false;
                stringstream out;
		ifstream file(filename.c_str(), ios::in);
		if (file == NULL) {
			this->message = "Data.cpp File "+filename+" not found";
			return this;
		} else this->message="";
		getline(file,this->title);
		j0=title.find("<NM=");
		if (j0!=string::npos) {
                        //cout<<"j0="<<j0<<"\n";
			j1=title.find("NF>",j0+3);
			s=title.substr(j0+4,j1-(j0+4));
			this->sexratio=atof(s.c_str())/(1.0+atof(s.c_str()));
		} else this->sexratio=0.5;
		//cout<<"dans data readfile this->sexratio="<<this->sexratio<<"\n";
		this->nloc=0;fin=false;
		while (not fin) {
			getline(file,s);
			s1=majuscules(s);
			if (s1.find("POP")==string::npos) this->nloc +=1;	//il s'agit d'un nom de locus
			else fin=true;
		}
		this->locus = new LocusC [this->nloc];this->Alocus=true;
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
				if ((s1.find("POP")!=string::npos)and(s1.find(",")==string::npos)) {nech +=1;nindi[nech-1]=0;}
                else {
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
	    	}
	    	getline(file,s);
		}
		file.close();
		this->nsample=nech;
		this->nind = new int[nech];this->Anind=true;
		for (int i=0;i<nech;i++) {this->nind[i]=nindi[i];}
		if (this->nmisshap>0) this->misshap = new MissingHaplo[this->nmisshap];
		if (this->nmissnuc>0) this->missnuc = new MissingNuc[this->nmissnuc];
	        this->nmisshap=0;
		this->nmissnuc=0;
		this->indivname = new string*[nech];this->Aindivname=true;
		this->indivsexe = new int*[nech];this->Aindivsexe=true;
		this->genotype = new string**[nech];this->Agenotype=true;
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
                //cout<<"dans data nsample = "<<this->nsample<<"\n";
	}

/**
* traitement des génotypes microsat au locus loc
*/
    void do_microsat(int loc){
    	string geno,*gen;
    	int l,ll,n,gg,ng;
    	gen = new string[2];
        vector <int> haplo;
    	this->locus[loc].mini=1000;this->locus[loc].maxi=0;
    	this->locus[loc].haplomic = new int*[this->nsample];
    	this->locus[loc].ss = new int[this->nsample];
    	this->locus[loc].samplesize = new int[this->nsample];
        //cout<<"data Locus="<<loc<<"\n";
    	for (int ech=0;ech<this->nsample;ech++){
    		this->locus[loc].ss[ech] = 0;
    		this->locus[loc].samplesize[ech] = 0;
            ng=0;
    		for (int ind=0;ind<this->nind[ech];ind++){
    			geno=string(this->genotype[ech][ind][loc]);
    			l=geno.length();
    			if (l>3){n=2;} else {n=1;}
    			if (n==2) {
    				ll=l/2;
    				gen[0]=geno.substr(0,ll);
    				gen[1]=geno.substr(ll,ll);
    			}
    			else {
    				gen[0]=geno;
    				if (this->locus[loc].type==2) this->indivsexe[ech][ind]=1;
    			    if ((this->locus[loc].type==3)and(geno!="000")) this->indivsexe[ech][ind]=1;
    			}
    			for (int i=0;i<n;i++) {
    				ng++;
                    if (gen[i]!="000") {
    					this->locus[loc].samplesize[ech] +=1;
    					gg = atoi(gen[i].c_str());
    					haplo.push_back(gg);
    					if (gg>this->locus[loc].maxi) this->locus[loc].maxi=gg;
    					if (gg<this->locus[loc].mini) this->locus[loc].mini=gg;

    				} else {
    					//if (not ((this->indivsexe[ech][ind]==2)and(this->locus[loc].type==3))) { // on ne prend pas en compte les chromosomes Y des femelles
                        this->nmisshap +=1;
                        this->misshap[this->nmisshap-1].sample=ech;
                        this->misshap[this->nmisshap-1].locus=loc;
                        this->misshap[this->nmisshap-1].indiv=ind;
                        haplo.push_back(-9999);
    					//}
    				}
    			}
    		}
    		this->locus[loc].ss[ech] = ng;
    		this->locus[loc].haplomic[ech] = new int[ng];
            for (int i=0;i<ng;i++) this->locus[loc].haplomic[ech][i]=haplo[i];
            if (not haplo.empty()) haplo.clear();
            //cout<<"sample "<<ech<<"\n";
            //for (int i=0;i<this->locus[loc].ss[ech];i++) cout<<"  "<<this->locus[loc].haplomic[ech][i];
            //cout<<"\n";
    	}
    	delete [] gen;
        
    }


/**
* traitement des génotypes DNA sequence au locus loc
*/
    void do_sequence(int loc){
        //cout<<"do_sequence locus"<<loc<<"\n";
    	string geno,*gen;
    	int l,ll,n,gg,j0,j1,j2,ng;
    	gen = new string[2];
    	string sbase,seq;
    	char base[] ="ACGT";
        vector <string> haplo;
    	this->locus[loc].haplodna = new string*[this->nsample];
    	this->locus[loc].ss = new int[this->nsample];
    	this->locus[loc].samplesize = new int[this->nsample];
    	this->locus[loc].dnalength = -1;
    	for (int ech=0;ech<this->nsample;ech++){
    		this->locus[loc].ss[ech] = 0;
    		this->locus[loc].samplesize[ech] = 0;
            ng=0;
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
    				if (this->locus[loc].type==2) this->indivsexe[ech][ind]=1;
    			    if ((this->locus[loc].type==3)and(geno!="[]")) this->indivsexe[ech][ind]=1;

    			}
    			for (int i=0;i<n;i++) {
                    ng++;
                    haplo.push_back(gen[i]);
    				if (gen[i]!="") {
    					this->locus[loc].samplesize[ech] +=1;
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
                        //cout <<indivname[ech][ind]<<"donnée manquante"<<"   nmisshap="<<this->nmisshap<<  "\n";
                        this->nmisshap +=1;
                        this->misshap[this->nmisshap-1].sample=ech;
                        this->misshap[this->nmisshap-1].locus=loc;
                        this->misshap[this->nmisshap-1].indiv=ind;
    				}
    			}
    		}
    		//cout <<"ng="<<ng<<"\n";
            this->locus[loc].ss[ech] = ng;
            this->locus[loc].haplodna[ech] = new string[ng];
            for (int i=0;i<ng;i++) {
                this->locus[loc].haplodna[ech][i]=haplo[i];
                haplo[i].clear();
                //cout<<"  "<<this->locus[loc].haplodna[ech][i]<<"("<<this->locus[loc].haplodna[ech][i].length()<<")";
            }
            //cout<<"\n\n";
            if (not haplo.empty()) haplo.clear();
            //cout<<"coucou\n";
    	}
		this->locus[loc].pi_A=0.0;this->locus[loc].pi_C=0.0;this->locus[loc].pi_G=0.0;this->locus[loc].pi_T=0.0;
		double nn=0.0;
    	for (int ech=0;ech<this->nsample;ech++){
    		for (int i=0;i<this->locus[loc].ss[ech];i++){
                if (not this->locus[loc].haplodna[ech][i].empty()) {
                    for (int j=0;j<this->locus[loc].dnalength;j++) {
                        if (this->locus[loc].haplodna[ech][i][j]==base[0]) {this->locus[loc].pi_A+=1.0;nn+=1.0;}
                        else if (this->locus[loc].haplodna[ech][i][j]==base[1]) {this->locus[loc].pi_C+=1.0;nn+=1.0;}
                        else if (this->locus[loc].haplodna[ech][i][j]==base[2]) {this->locus[loc].pi_G+=1.0;nn+=1.0;}
                        else if (this->locus[loc].haplodna[ech][i][j]==base[3]) {this->locus[loc].pi_T+=1.0;nn+=1.0;}
                    }
                }
    		}
    	}
    	if (nn>0.0) {this->locus[loc].pi_A /=nn;this->locus[loc].pi_C /=nn;this->locus[loc].pi_G /=nn;this->locus[loc].pi_T /=nn;}
    	this->locus[loc].tabsit.resize(this->locus[loc].dnalength);
    	this->locus[loc].mutsit.resize(this->locus[loc].dnalength);
    	delete [] gen;
        //if (loc==12) for (int i=0;i<this->locus[loc].ss[0];i++) cout<<i<<"   "<<this->locus[loc].haplodna[0][i]<<"\n";
    }

/**
* calcul du coefficient dans la formule de coalescence en fonction du type de locus 
* 0:autosomal diploide, 1:autosomal haploïde, 2:X-linked, 3:Y-linked, 4:mitochondrial
*/
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
                //if (loc==0) cout<<"sexratio="<<this->sexratio<<"    coefficient="<<this->locus[loc].coeff<<"\n";
    }


/**
* chargement des données dans une structure DataC 
*/
    DataC * loadfromfile(string filename) {
		int loc,kloc;
		this->filetype = this->testfile(filename);
		if (this->filetype==-1) {
			cout<<"Unreckognized file format"<<"\n";
			exit(1);
		}
		if (this->filetype==0) {
			this->readfile(filename);
					cout <<this->message<<   "\n";
					if (this->message != "") exit(1);
			kloc=this->nloc;
			for (loc=0;loc<kloc;loc++) {
				if (this->locus[loc].type<5) this->do_microsat(loc);
				else                         this->do_sequence(loc);
				this->cal_coeff(loc);
			}
		}
		if (this->filetype==1) {
			this->readfilesnp(filename);
			this->purgelocmonomorphes();
			for (loc=0;loc<this->nloc;loc++) this->do_snp(loc);
		}
	}
        
};
