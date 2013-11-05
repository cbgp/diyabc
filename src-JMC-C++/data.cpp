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
#include <stdlib.h>
#include <ctype.h>
#include <vector>
#include <time.h>

#include "mesutils.h"
#include "randomgenerator.h"
/*
#ifndef MESUTILS
#include "mesutils.cpp"
#define MESUTILS
#endif

#ifndef RANDOMGENERATOR
#include "randomgenerator.cpp"
#define RANDOMGENERATOR
#endif
*/
#include "data.h"

using namespace std;

string ***genotype;
bool Agenotype;

	MissingHaplo & MissingHaplo::operator=(MissingHaplo const & source) {
		if (this== &source) return *this;
		this->locus = source.locus;
		this->sample = source.sample;
		this->indiv = source.indiv;
		return *this;
	}

	MissingNuc & MissingNuc::operator=(MissingNuc const & source) {
		if (this== &source) return *this;
		this->locus = source.locus;
		this->sample = source.sample;
		this->indiv = source.indiv;
		this->nuc = source.nuc;
		return *this;
	}
	
/**
 *  Structure LocusC : libération de la mémoire occupée par la structure LocusC
 */
void LocusC::libere(bool obs, int nsample) {
	if ((this->type>4)and(this->type<10)) {
		if (not obs) {
			if (not mutsit.empty()) mutsit.clear();
			if (not tabsit.empty())tabsit.clear();
		}
	}
	//cout<<"fin\n";
}


	LocusC & LocusC::operator=(LocusC const & source) {
		if (this== &source) return *this;
		if (this->freq != NULL) {
			for (int i=0;i<source.nsample;i++) delete [] this->freq[i];
		}
		if (this->haplodna != NULL) {
			for (int i=0;i<source.nsample;i++) delete [] this->haplodna[i];
		}
		if (this->haplodnavar != NULL) {
			for (int i=0;i<source.nsample;i++) delete [] this->haplodnavar[i];
		}
		if (this->haplomic != NULL) {
			for (int i=0;i<source.nsample;i++) delete [] this->haplomic[i];
		}
		if (this->haplosnp != NULL) {
			for (int i=0;i<source.nsample;i++) delete [] this->haplosnp[i];
		}
		if (this->samplesize !=NULL) delete [] this->samplesize;
		if (not this->mutsit.empty()) this->mutsit.clear();
		if (not this->sitmut.empty()) this->sitmut.clear();
		if (not this->sitmut2.empty()) this->sitmut2.clear();
		if (not this->tabsit.empty()) this->tabsit.clear();
		this->name = source.name;
		this->type = source.type;
		this->groupe = source.groupe;
		this->coeffcoal = source.coeffcoal;
		this->pi_A = source.pi_A;
		this->pi_C = source.pi_C;
		this->pi_G = source.pi_G;
		this->pi_T = source.pi_T;
		this->dnalength = source.dnalength;
		this->dnavar = source.dnavar;
		this->mini = source.mini;
		this->maxi = source.maxi;		
		this->kmin = source.kmin;		
		this->kmax = source.kmax;		
		this->motif_size = source.motif_size;		
		this->motif_range = source.motif_range;		
		this->nal = source.nal;		
		this->mut_rate = source.mut_rate;		
		this->Pgeom = source.Pgeom;		
		this->sni_rate = source.sni_rate;		
		this->mus_rate = source.mus_rate;
		this->k1 = source.k1;		
		this->k2 = source.k2;		
		this->firstime = source.firstime;		
		this->mono	 = source.mono;	
		this->weight = source.weight;		
		this->nsample = source.nsample;
		if (source.freq != NULL){
			this->freq = new long double*[this->nsample];
			for (int i=0;i<this->nsample;i++) {
				this->freq[i] = new long double[this->nal];
				for (int j=0;j<this->nal;j++) this->freq[i][j] = source.freq[i][j];
			}
		}
		if (source.samplesize != NULL){
			this->samplesize = new int[this->nsample];
			for (int i=0;i<this->nsample;i++) this->samplesize[i] = source.samplesize[i];
		}
		if (source.haplodna != NULL) {
			this->haplodna = new string*[this->nsample];
			for (int i=0;i<this->nsample;i++) {
				this->haplodna[i] = new string[source.samplesize[i]];
				for (int j=0;j<source.samplesize[i];j++) this->haplodna[i][j] = source.haplodna[i][j];
			}
		}
		if (source.haplodnavar != NULL) {
			this->haplodnavar = new string*[this->nsample];
			for (int i=0;i<this->nsample;i++) {
				this->haplodnavar[i] = new string[source.samplesize[i]];
				for (int j=0;j<source.samplesize[i];j++) this->haplodnavar[i][j] = source.haplodnavar[i][j];
			}
		}
		if (source.haplomic != NULL) {
			this->haplomic = new int*[this->nsample];
			for (int i=0;i<this->nsample;i++) {
				this->haplomic[i] = new int[source.samplesize[i]];
				for (int j=0;j<source.samplesize[i];j++) this->haplomic[i][j] = source.haplomic[i][j];
			}
		}
		if (source.haplosnp != NULL) {
			this->haplosnp = new short int*[this->nsample];
			for (int i=0;i<this->nsample;i++) {
				this->haplosnp[i] = new short int[source.samplesize[i]];
				for (int j=0;j<source.samplesize[i];j++) this->haplosnp[i][j] = source.haplosnp[i][j];
			}
		}
		if (not source.mutsit.empty()) {
			for (int i=0;i<source.mutsit.size();i++) this->mutsit.push_back(source.mutsit[i]);
		}
		if (not source.sitmut.empty()) {
			for (int i=0;i<source.sitmut.size();i++) this->sitmut.push_back(source.sitmut[i]);
		}
		if (not source.sitmut2.empty()) {
			for (int i=0;i<source.sitmut2.size();i++) this->sitmut2.push_back(source.sitmut2[i]);
		}
		if (not source.tabsit.empty()) {
			for (int i=0;i<source.tabsit.size();i++) this->tabsit.push_back(source.tabsit[i]);
		}
		return *this;
	}

	DataC & DataC::operator=(DataC const & source) {
		if (this== &source) return *this;
		if(this->indivname != NULL) delete [] this->indivname;
		if(this->misshap != NULL) delete [] this->misshap;
		if(this->misssnp != NULL) delete [] this->misssnp;
		if(this->missnuc != NULL) delete [] this->missnuc;
		if(this->locus != NULL) delete [] this->locus;
		if(this->catexist != NULL) delete [] this->catexist;
		if(not this->nind.empty()) this->nind.clear();
		if(not this->ssize.empty()) {
			for (int i=0;i<ssize.size();i++)this->ssize[i].clear();
			this->ssize.clear();
		}
		if(not this->indivsexe.empty()) {
			for (int i=0;i<indivsexe.size();i++)this->indivsexe[i].clear();
			this->indivsexe.clear();
		}
		this->message = source.message;
		this->title = source.title;
		this->nsample = source.nsample;
		this->nsample0 = source.nsample0;
		this->nloc = source.nloc;
		this->nmisshap = source.nmisshap;
		this->nmissnuc = source.nmissnuc;
		this->nmisssnp = source.nmisssnp;
		this->filetype = source.filetype;
		this->sexratio = source.sexratio;
		this->Aindivname = source.Aindivname;
		this->Anind = source.Anind;
		this->Aindivsexe = source.Aindivsexe;
		this->Alocus = source.Alocus;
		for (int i=0;i<source.nind.size();i++) this->nind.push_back(source.nind[i]);
		if (source.indivname != NULL){
			this->indivname = new string*[this->nsample0];
			for (int i=0;i<this->nsample0;i++) {
				this->indivname[i] = new string[this->nind[i]];
				for (int j=0;j<this->nind[i];j++) this->indivname[i][j] = source.indivname[i][j];
			}
		}
		if (source.misshap != NULL) {
			this->misshap = new MissingHaplo[this->nmisshap];
			for (int i=0;i<this->nmisshap;i++) this->misshap[i] = source.misshap[i];
		}
		if (source.misssnp != NULL) {
			this->misssnp = new MissingHaplo[this->nmisssnp];
			for (int i=0;i<this->nmisssnp;i++) this->misssnp[i] = source.misssnp[i];
		}
		if (source.missnuc != NULL) {
			this->missnuc = new MissingNuc[this->nmissnuc];
			for (int i=0;i<this->nmissnuc;i++) this->missnuc[i] = source.missnuc[i];
		}
		if (source.locus != NULL) {
			this->locus = new LocusC[this->nloc];
			for (int i=0;i<this->nloc;i++) this->locus[i] = source.locus[i];
		}
		if (source.catexist != NULL) {
			this->catexist = new bool[5];
			for (int i=0;i<5;i++) this->catexist[i] = source.catexist[i];
		}
		this->ssize.resize(source.ssize.size());
		for (int i=0;i<source.ssize.size();i++) {
			for (int j=0;j<source.ssize[i].size();j++) this->ssize[i].push_back(source.ssize[i][j]);
		}
		this->indivsexe.resize(source.indivsexe.size());
		for (int i=0;i<source.indivsexe.size();i++) {
			for (int j=0;j<source.indivsexe[i].size();j++) this->indivsexe[i].push_back(source.indivsexe[i][j]);
		}
		return *this;
	}

/**
* liberation de la mémoire occupée par la classe DataC
*/
	void DataC::libere(){
		//cout<<"Aindivname="<<Aindivname<<"   Aindivsexe="<<Aindivsexe<<"   Agenotype="<<Agenotype<<"   Anind="<<Anind<<"\n";
		if (Aindivname) {
			for (int  ech=0;ech<this->nsample0;ech++) delete [] this->indivname[ech];
			delete [] this->indivname;
			//cout<<"delete indivname\n";
		}
		if (Aindivsexe) {
			for (int  ech=0;ech<this->nsample0;ech++) this->indivsexe[ech].clear();
			this->indivsexe.clear();
                    //cout<<"delete indivsexe\n";
		}
		if (Agenotype) {
			for (int  ech=0;ech<this->nsample0;ech++) {
					for (int ind=0;ind<this->nind[ech];ind++) {
							delete [] genotype[ech][ind];
							//cout <<"delete genotype[ech][ind]\n";
					}
					delete [] genotype[ech];
					//cout <<"delete genotype[ech]\n";
			}
			delete [] genotype;
			//cout<<"delete genotype\n";
		}
		//cout<<"apres delete genotype\n";
		if (this->nmisshap>0) delete [] this->misshap;
		if (this->nmissnuc>0) delete [] this->missnuc;
			//cout<<"apres delete misshap et missnuc\n";
		if (Alocus) {
			for (int loc=0;loc<this->nloc;loc++) {
				if (this->locus[loc].type<5) {for (int ech=0;ech<nsample;ech++) delete [] this->locus[loc].haplomic[ech];}
				else if (this->locus[loc].type<10) {for (int ech=0;ech<nsample;ech++) delete [] this->locus[loc].haplodna[ech];}
				else {for (int ech=0;ech<nsample;ech++) delete [] this->locus[loc].haplosnp[ech];}
			this->locus[loc].libere(true,this->nsample);
			}
			delete [] this->locus;
		}
		if (Anind) this->nind.clear();
	}

string  getligne(ifstream file) {
	string s="";
	char ch;
	bool term;
	do {
		ch=file.get();
		term=((ch=='\r')or(ch=='\n'));
		if (not term) s.push_back(ch);
	} while (not term);
	if (ch=='\r') ch=file.get();
	return s;
}
	
/**
* détermination du type de fichier de donnée
* return=-1
* return=0 si genepop
* return=1 si snp
*/
	int DataC::testfile(string filename){
		//cout<<"debut de testfile fichier "<<filename<<"\n";
	    int nss,prem;
	    string ligne,*ss=NULL;
		ifstream file0(filename.c_str(), ios::in);
		if (file0 == NULL) {cout<<"file0=NULL\n";return -1;}
		getline(file0,ligne);
		cout<<ligne<<"\n";
		ligne=purgetab(ligne);
        ss=splitwords(ligne," ",&nss);
		if (nss>2) {
			cout<<ss[0]<<" "<<ss[1]<<" "<<ss[2]<<"\n";
			if ((ss[0]=="IND")and(ss[1]=="SEX")and(ss[2]=="POP")) {
			cout<<"Fichier "<<filename<<" : SNP\n";
			prem=0;
			return 1;
			}
		}
		//cout<<"avant le delete[]ss\n";
		delete []ss; ss = NULL;
		//cout<<"avant getline\n";
		getline(file0,ligne);
		//cout<<"ligne 2 =<"<<ligne<<">\n";
        ss=splitwords(ligne," ",&nss);
		//cout<<"apres le deuxième splitwords\n";
		if ((ss[0]=="IND")and(ss[1]=="SEX")and(ss[2]=="POP")) {
		  cout<<"Fichier "<<filename<<" : SNP\n";
		  prem=1;
		  delete [] ss;
		  return 1;
		}
		delete [] ss; ss = NULL;
		file0.close();
		ifstream file(filename.c_str(), ios::in);
		getline(file,ligne);
		int nloc=0;
		bool trouvepop=false;
		while (not trouvepop) {
			getline(file, ligne);
			ligne=majuscules(ligne);
			//cout<<ligne<<"\n";
			trouvepop=(ligne.find("POP")!=string::npos);
			if (not trouvepop) nloc++;
		}
		cout<<"nloc="<<nloc<<"\n";
		if (not trouvepop) return -2;
		while (not file.eof()){
			getline(file, ligne);
			//cout<<"longueur de la ligne = "<<ligne.length()<<"\n";
			//cout<<ligne<<"\n";
			if(ligne.length()>2) {if(ligne.at(ligne.length()-1)=='\r') ligne=ligne.substr(ligne.length()-1);}
			//cout<<"apres le test DOS\n";
			if( (int)ligne.length() >= 2 + 3*nloc){
				ligne=purgetab(ligne);
				ligne=majuscules(ligne);
				if ((ligne.find(",")==string::npos)and(ligne.find("POP")==string::npos)) return -2;
				if(ligne.find(",")!=string::npos) {
					ligne = ligne.substr(ligne.find(",") + 1);
					ss=splitwords(ligne," ",&nss);
					cout<<"nss="<<nss<<"\n";
					cout<<ss[nss-1]<<"\n";
					if (nss!=nloc) {delete [] ss;ss=NULL;return -2;}
					cout<<"avant exit(1)\n";
					//exit(1);
				}
			}
		}
		cout<<"avant le delete\n";
		if (ss!=NULL) {cout<<"ss != NULL\n";} else cout<<"ss = NULL\n";
		if (ss!=NULL) {delete [] ss;ss=NULL;}
		cout<<"avant return 0 dans testfile\n";
		return 0;
	}

/**
* lecture d'un fichier de donnée SNP et stockage des informations dans une structure DataC
*/
	int  DataC::readfilesnp(string filename){
		int ech,ind,nech,*nindi,nss,j0,j1,prem;
		bool deja;
		string s1,*ss,s,sss;
		vector <string> popname;
		Aindivname=false;Agenotype=false;Anind=false;Aindivsexe=false;Alocus=false;
		ifstream file(filename.c_str(), ios::in);
		if (file == NULL) {
			this->message = "Data.cpp File "+filename+" not found";
			return 1;
		} else this->message="";
		getline(file,s1);s1=purgetab(s1);
		j0=s1.find("<NM=");
		if (j0!=string::npos) {
                        //cout<<"j0="<<j0<<"\n";
			j1=s1.find("NF>",j0+3);
			s=s1.substr(j0+4,j1-(j0+4));
			this->sexratio=atof(s.c_str())/(1.0+atof(s.c_str()));
			getline(file,s1);s1=purgetab(s1);
			prem=1;
		} else {this->sexratio=0.5;prem=0;}
        ss=splitwords(s1," ",&nss);
		this->nloc=nss-3;
		this->locus = new LocusC[this->nloc];this->Alocus=true;
		cout<<"this->nloc = "<<this->nloc<<"\n";
		for (int loc=0;loc<this->nloc;loc++) {
			sss=ss[loc+3].substr(0,1);
			if (sss=="A") this->locus[loc].type=10;
			else if (sss=="H")this->locus[loc].type=11;
			else if (sss=="X")this->locus[loc].type=12;
			else if (sss=="Y")this->locus[loc].type=13;
			else if (sss=="M")this->locus[loc].type=14;
			this->locus[loc].name = sss+IntToString(loc);
		}
		cout<<"recherche du nombre d'échantillons\n";
//recherche du nombre d'échantillons
		nech=1;popname.resize(nech);
		getline(file,s1);s1=purgetab(s1);
		delete[]ss;
		ss=splitwordsR(s1," ",3,&nss);
		popname[nech-1]=ss[2];
		while (not file.eof()) {
			getline(file,s1);s1=purgetab(s1);
			if (s1.length()>10) {
				delete[]ss;
				ss=splitwordsR(s1," ",3,&nss);
				//cout<<ss[0]<<"  "<<ss[1]<<"  "<<ss[2]<<"   nech="<<nech<<"\n";
				deja=false;
				for (int n=0;n<nech;n++) {deja=(ss[2]==popname[n]);if (deja) break;}
				if (not deja) {
					nech++;
					popname.resize(nech);
					popname[nech-1]=ss[2];
				}
			}
		}
		this->nsample = nech;  cout<<nech<<" échantillons : ";
		for (int loc=0;loc<this->nloc;loc++) this->locus[loc].nsample = nech;
		for (ech=0;ech<nech;ech++) cout<<popname[ech]<<"  ";cout<<"\n";
		this->nind.resize(nech);
		file.close();
//recherche du nombre d'individus par échantillon
		nindi = new int[nech];
		for (ech=0;ech<nech;ech++) nindi[ech]=0;
		file.open(filename.c_str(), ios::in);
		getline(file,s1);if (prem==1) getline(file,s1);s1=purgetab(s1);
 		while (not file.eof()) {
			getline(file,s1);s1=purgetab(s1);
			int s1l=s1.length();
			if (s1l>10) {
				delete[]ss;
				ss=splitwordsR(s1," ",3,&nss);
				//cout<<ss[0]<<"  "<<ss[1]<<"  "<<ss[2]<<"\n";
				ech=0;while (ss[2]!=popname[ech]) ech++;
				nindi[ech]++;
			}
		}
		for (ech=0;ech<nech;ech++) this->nind[ech] = nindi[ech];Anind=true;
		for (ech=0;ech<nech;ech++) cout <<"échantillon "<<ech+1<<" : "<<this->nind[ech]<<" individus\n";
//remplissage des noms et des génotypes des individus
		this->indivname = new string*[nech];Aindivname=true;
		this->indivsexe.resize(nech);this->Aindivsexe=true;
		for (ech=0;ech<nech;ech++) this->indivname[ech] = new string[nindi[ech]];
		for (ech=0;ech<nech;ech++) this->indivsexe[ech].resize(nindi[ech]);
		genotype = new string**[nech];Agenotype=true;
		for (ech=0;ech<nech;ech++) {
			genotype[ech] = new string*[nindi[ech]];
			for (ind=0;ind<nindi[ech];ind++) genotype[ech][ind] = new string[nloc];
		}
		file.close();
		file.open(filename.c_str(), ios::in);
		getline(file,s1);if (prem==1) getline(file,s1);s1=purgetab(s1);
		for (ech=0;ech<nech;ech++) nindi[ech]=0;
 		while (not file.eof()) {
			getline(file,s1);s1=purgetab(s1);
			int s1l=s1.length();
			if (s1l>10) {
				delete[]ss;
				ss=splitwords(s1," ",&nss);
				ech=0;while (ss[2]!=popname[ech]) ech++;
				this->indivsexe[ech][nindi[ech]]=0;
				if (ss[1]=="M")      this->indivsexe[ech][nindi[ech]]=1;
				else if (ss[1]=="F") this->indivsexe[ech][nindi[ech]]=2;
				this->indivname[ech][nindi[ech]]=ss[0];
				for (int loc=0;loc<this->nloc;loc++) genotype[ech][nindi[ech]][loc]= ss[loc+3];
				cout<<"individu "<<nindi[ech]+1<<" de l'échantillon "<<ech+1<<"   "<<this->indivname[ech][nindi[ech]];
				cout<<"  "<<this->indivsexe[ech][nindi[ech]]<<"  "<<popname[ech]<<"   "<<genotype[ech][nindi[ech]][0]<<"\r";
				nindi[ech]++;
				//for (int ec=0;ec<nech;ec++) cout<<nindi[ec]<<"   ";cout<<"\n";
			}
		}
		cout<<"\n";
		file.close();
		cout<<"avant les delete\n";
		delete []nindi;delete []ss;
		cout<<"fin de la lecture du fichier\n";
		return 0;
	}
/**
* supprime les locus monomorphes
*/
	void DataC::purgelocmonomorphes(){
		int ind,ech,ind0 = 0,ech0,kloc=0,nloc=0,*typ;
		string ***ge,misval="9";
		string premier="";
		for (int loc=0;loc<this->nloc;loc++) {
			for (ech0=0;ech0<this->nsample;ech0++){
				for (ind0=0;ind0<this->nind[ech0];ind0++) {
					if (genotype[ech0][ind0][loc]!=misval) premier=genotype[ech0][ind0][loc];
					if (premier!="") break;
				}
				if (premier!="") break;
			}
			if (premier=="") {
				this->locus[loc].mono=true; //le locus n'a que des données manquantes
				cout<<"premier="<<premier<<" au locus"<<loc<<"\n";
			} else {
				this->locus[loc].mono=true;
				for (ech=ech0;ech<this->nsample;ech++) {
					for (ind=ind0+1;ind<this->nind[ech];ind++) {
					    if ((this->indivsexe[ech][ind]==2)and(genotype[ech][ind][loc]=="1")) this->locus[loc].mono=false;
						if (not this->locus[loc].mono) break;
						this->locus[loc].mono=((genotype[ech][ind][loc]==premier)or(genotype[ech][ind][loc]==misval));
						if (not this->locus[loc].mono) break;
					}
					ind0=-1;
					if (not this->locus[loc].mono) break;
				}
			}
			if (not this->locus[loc].mono) {
				nloc++;
			} else {
				//cout<<"le locus "<<loc<<" est monomorphe"<<"   tous les génotypes sont égaux à "<<premier<<"\n";
				/*for (ech=0;ech<this->nsample;ech++){
					for (ind=0;ind<this->nind[ech];ind++) {
						cout<<genotype[ech][ind][loc]<<" ";
					}
					cout<<"\n";
				}*/
			}
			
		}
	if (nloc<this->nloc){
			cout<<"purge de "<<this->nloc-nloc<<" locus monomorphes\n";
			ge = new string**[this->nsample];
			typ = new int[nloc];
			for (ech=0;ech<this->nsample;ech++) {
				ge[ech] = new string*[this->nind[ech]];
				for (ind=0;ind<this->nind[ech];ind++) ge[ech][ind] = new string[nloc];
			}
			for (int loc=0;loc<this->nloc;loc++) {
				if (not this->locus[loc].mono) {
					for (ech=0;ech<this->nsample;ech++) {
						for (ind=0;ind<this->nind[ech];ind++) ge[ech][ind][kloc] = genotype[ech][ind][loc];
					}
					typ[kloc] = this->locus[loc].type;
					kloc++;
				}
			}
			delete[]this->locus;
			this->locus = new LocusC[kloc];
			for (int loc=0;loc<kloc;loc++) {this->locus[loc].type = typ[loc];this->locus[loc].nsample=this->nsample;}
			delete[]typ;
			for (ech=0;ech<this->nsample;ech++) {
				for (ind=0;ind<this->nind[ech];ind++){
					delete[]genotype[ech][ind];
					genotype[ech][ind] = new string[kloc];
					for (int loc=0;loc<kloc;loc++) genotype[ech][ind][loc] = ge[ech][ind][loc];
				}
			}
			for (ech=0;ech<this->nsample;ech++) {
				for (ind=0;ind<this->nind[ech];ind++) delete[]ge[ech][ind];
			    delete[]ge[ech];
			}
			delete[]ge;
			cout<<"PPL observé = "<<100.0*(double)kloc/(double)this->nloc<<" % \n";
			this->nloc=kloc;
		} else cout<<"tous les locus sont polymorphes\n";
		//exit(1);
	}

	void DataC::missingdata(){
		int ind,ech,ng,typ,plo;
		short int misval=9;
		this->nmisssnp=0;
		/*for (ech=0;ech<this->nsample;ech++) {
			for (int loc=0;loc<this->nloc;loc++) {
				typ=this->locus[loc].type % 5;
				ng=0;
				for (ind=0;ind<this->nind[ech];ind++){
					plo=1;
					if ((typ == 0)or((typ == 2)and(this->indivsexe[ech][ind] == 2))) plo=2;
					cout<<"ech="<<ech<<"  ind="<<ind<<"  loc="<<loc<<"  plo="<<plo<<"   "<<genotype[ech][ind][loc]<<"\n";
					if (genotype[ech][ind][loc]==misval) {
						if (plo==2){
							this->misssnp[this->nmisssnp].locus = loc;
							this->misssnp[this->nmisssnp].sample = ech;
							this->misssnp[this->nmisssnp].indiv = ng;
							this->nmisssnp++;
							this->misssnp[this->nmisssnp].locus = loc;
							this->misssnp[this->nmisssnp].sample = ech;
							this->misssnp[this->nmisssnp].indiv = ng+1;
							this->nmisssnp++;
						}
						if (plo==1){
							this->misssnp[this->nmisssnp].locus = loc;
							this->misssnp[this->nmisssnp].sample = ech;
							this->misssnp[this->nmisssnp].indiv = ng;
							this->nmisssnp++;
						}
					}
					ng +=plo;
					cout<<"ng="<<ng<<"\n";
				}
			}
		}*/
		for (ech=0;ech<this->nsample;ech++) {
			for (int loc=0;loc<this->nloc;loc++) {
				typ=this->locus[loc].type % 5;
				for (int i=0;i<this->ssize[typ][ech];i++) {
					if (this->locus[loc].haplosnp[ech][i]==misval) this->nmisssnp++;
				}
			}
		}
		this->misssnp = new MissingHaplo[nmisssnp];
		this->nmisssnp=0;
		for (ech=0;ech<this->nsample;ech++) {
			for (int loc=0;loc<this->nloc;loc++) {
				typ=this->locus[loc].type % 5;
				for (int i=0;i<this->ssize[typ][ech];i++) {
					if (this->locus[loc].haplosnp[ech][i]==misval) {
							this->misssnp[this->nmisssnp].locus = loc;
							this->misssnp[this->nmisssnp].sample = ech;
							this->misssnp[this->nmisssnp].indiv = i;
							this->nmisssnp++;
					}
				}
			}
		}
	}
/**
* traitement des locus snp
*/
	void DataC::do_snp(int loc) {
		vector <short int> haplo;
		int ech,ind,ss;
		string misval="9";
		short int g0=0,g1=1,g9=9;
		this->locus[loc].haplosnp = new short int*[this->nsample];
		this->locus[loc].samplesize = new int[this->nsample];
		cout<<"\ndo_snp locus = "<<loc+1<<"\n";
		for (ech=0;ech<this->nsample;ech++) {
			ss=0;
			for (ind=0;ind<this->nind[ech];ind++){
				//cout<<"ech="<<ech<<"   ind="<<ind<<"    "<<genotype[ech][ind][loc]<<"\n";
				//cout<<"locustype="<<this->locus[loc].type<<"    indivsexe="<<this->indivsexe[ech][ind]<<"\n";
				if ((this->locus[loc].type==10)or((this->locus[loc].type==12)and(this->indivsexe[ech][ind]==2))) {
					ss +=2;
					if (genotype[ech][ind][loc].substr(0,1)!=misval) {
						if (genotype[ech][ind][loc].substr(0,1)=="0") {haplo.push_back(g0);haplo.push_back(g0);}
						if (genotype[ech][ind][loc].substr(0,1)=="1") {haplo.push_back(g0);haplo.push_back(g1);}
						if (genotype[ech][ind][loc].substr(0,1)=="2") {haplo.push_back(g1);haplo.push_back(g1);}
					} else {haplo.push_back(g9);haplo.push_back(g9);}
				} else {
					ss +=1;
					if (genotype[ech][ind][loc].substr(0,1)!=misval) {
						if (genotype[ech][ind][loc].substr(0,1)=="0") {haplo.push_back(g0);}
						if (genotype[ech][ind][loc].substr(0,1)=="1") {haplo.push_back(g1);}
					} else {haplo.push_back(g9);}
				}
			}
			//cout<<"avant transfert  ss="<<ss<<"   haplo.size = "<<haplo.size()<<"\n";
			this->locus[loc].samplesize[ech] = ss;
			cout<<"Dans do_snp("<<loc<<")  samplesize[";
			cout<<ech<<"]="<<this->locus[loc].samplesize[ech]<<"\n";
			this->locus[loc].haplosnp[ech] = new short int[ss];
			for (int i=0;i<ss;i++) this->locus[loc].haplosnp[ech][i] = haplo[i];
			if (not haplo.empty()) haplo.clear();
		}
		cout<<"sortie de do_snp\n";
	}

	void DataC::calcule_ss() {
		this->catexist = new bool[5];
		this->ssize.resize(5);
		for (int i=0;i<5;i++) this->catexist[i]=false;
		for (int locustype=0;locustype<5;locustype++) {
			if (not this->catexist[locustype]) {
				for (int loc=0;loc<this->nloc;loc++) {
					if ((this->locus[loc].type % 5) == locustype) {
						this->ssize[locustype].resize(this->nsample);
						for (int sa=0;sa<this->nsample;sa++) {
							this->ssize[locustype][sa]=0;
							for (int ind=0;ind<this->nind[sa];ind++) {
								//if ((this->locus[loc].type==10)or((this->locus[loc].type==12)and(this->indivsexe[sa][ind]==2))) this->ssize[locustype][sa] +=2;
								//else if (not((this->locus[loc].type==13)and(this->indivsexe[sa][ind]==2))) this->ssize[locustype][sa] +=1;
								if( (locustype == 0) or ( (locustype == 2)and(this->indivsexe[sa][ind]==2) ) ) {
									this->ssize[locustype][sa] +=2;
								} else {
									if(this->indivsexe[sa][ind]>0) this->ssize[locustype][sa] +=1;
								}
							}
							//cout<<"data.ss["<<locustype<<"]["<<sa<<"]="<<this->ssize[locustype][sa]<<"\n";
						}
						this->catexist[locustype] = true;
					}
				}
			}
		}
	}

/**
* ecriture en binaire d'un fichier snp 
*/
	void DataC::ecribin(string filenamebin) {
        MwcGen mwc;        mwc.randinit(990,time(NULL));
		int *index,lon,kloc,categ;
		//index=mwc.randperm(this->nloc);
		// a virer ?
		index = new int[this->nloc];
		for(int i=0; i< this->nloc; i++)
			index[i] = i;
		// fin a virer ?
		fstream f1;		
        f1.open(filenamebin.c_str(),ios::out|ios::binary);
		f1.write((char*)&(this->sexratio),sizeof(double));											//sex-ratio
		f1.write((char*)&(this->nloc),sizeof(int));													//nombre de locus
		f1.write((char*)&(this->nsample),sizeof(int));												//nombre d'échantillons
		for(int i=0;i<this->nsample;i++) f1.write((char*)&(this->nind[i]),sizeof(int));				//nombre d'individus par échantillons
		for(int i=0;i<this->nsample;i++){															//noms des individus
			for (int j=0;j<this->nind[i];j++) {
				lon=this->indivname[i][j].length()+1;
				f1.write((char*)&(lon),sizeof(int));
				f1.write(this->indivname[i][j].c_str(),lon);
			}
		}
		for(int i=0;i<this->nsample;i++){															//sexes des individus
			for (int j=0;j<this->nind[i];j++) f1.write((char*)&(this->indivsexe[i][j]),sizeof(int));
		}
		for (int loc=0;loc<this->nloc;loc++) {kloc = index[loc];f1.write((char*)&(this->locus[kloc].type),sizeof(int));} //types des locus																		
		for (int locustype=0;locustype<5;locustype++) f1.write((char*)&(this->catexist[locustype]),sizeof(bool));
		for (int locustype=0;locustype<5;locustype++) {					//nombre total de gènes par catégorie de locus
			if (this->catexist[locustype]){
				for (int sa=0;sa<this->nsample;sa++) {
					f1.write((char*)&(this->ssize[locustype][sa]),sizeof(int));
					cout<<"ecribin   this->ssize["<<locustype<<"]["<<sa<<"]="<<this->ssize[locustype][sa]<<"\n";
				}
			}
			
		} 
		for (int loc=0;loc<this->nloc;loc++) {
			kloc = index[loc];
			categ = this->locus[kloc].type % 5;
			for (int ech=0;ech<this->nsample;ech++) {
				for (int i=0;i<this->ssize[categ][ech];i++) f1.write((char*)&(this->locus[kloc].haplosnp[ech][i]),sizeof(short int));
			}
		}
		f1.close();
cout<<"fin de ecribin\n";
	}

/**
* lecture en binaire d'un fichier snp 
*/
	void DataC::libin(string filenamebin) {
		fstream f0;
		char* buffer;
		buffer = new char[1000];
		int lon,categ;
        f0.open(filenamebin.c_str(),ios::in|ios::binary);
		f0.read((char*)&(this->sexratio),sizeof(double)); cout<<"sexratio="<<this->sexratio<<"\n";											//sex-ratio
		f0.read((char*)&(this->nloc),sizeof(int));	cout<<"nloc="<<this->nloc<<"\n";				//nombre de locus
		f0.read((char*)&(this->nsample),sizeof(int));	cout<<"nsample="<<this->nsample<<"\n";		//nombre d'échantillons
		this->nind.resize(this->nsample);
		for(int i=0;i<this->nsample;i++) f0.read((char*)&(this->nind[i]),sizeof(int));				//nombre d'individus par échantillons
		this->indivname = new string*[this->nsample];
		for(int i=0;i<this->nsample;i++) this->indivname[i] = new string[this->nind[i]];
		for(int i=0;i<this->nsample;i++){															//noms des individus
			for (int j=0;j<this->nind[i];j++) {
				f0.read((char*)&(lon),sizeof(int)); 
				f0.read(buffer,lon);
				this->indivname[i][j] = char2string(buffer);
			}
		}
		//cout<<"libin 1\n";
		this->indivsexe.resize(this->nsample);
		for(int i=0;i<this->nsample;i++) this->indivsexe[i].resize(this->nind[i]);
		for(int i=0;i<this->nsample;i++){															//sexes des individus
			for (int j=0;j<this->nind[i];j++) f0.read((char*)&(this->indivsexe[i][j]),sizeof(int));
		}
		this->locus = new LocusC[this->nloc];
		for (int loc=0;loc<this->nloc;loc++) this->locus[loc].nsample = this->nsample;
		for (int loc=0;loc<this->nloc;loc++) f0.read((char*)&(this->locus[loc].type),sizeof(int)); //types des locus
		this->catexist = new bool[5];																		
		this->ssize.resize(5);																		
		for (int locustype=0;locustype<5;locustype++) f0.read((char*)&(this->catexist[locustype]),sizeof(bool));
		for (int locustype=0;locustype<5;locustype++) cout<<" type "<<locustype<<"   catexist ="<<this->catexist[locustype]<<"\n";
		for (int locustype=0;locustype<5;locustype++) {											//nombre total de gènes par catégorie de locus
			if (this->catexist[locustype]) {
				this->ssize[locustype].resize(this->nsample);
				for (int sa=0;sa<this->nsample;sa++) f0.read((char*)&(this->ssize[locustype][sa]),sizeof(int));
				cout<<"type "<<locustype<<"\n";
				for (int sa=0;sa<this->nsample;sa++) cout <<this->ssize[locustype][sa]<<"   "; cout<<"\n"; 
			}
		}
		//cout<<"libin 2\n";
		for (int loc=0;loc<this->nloc;loc++) {
			this->locus[loc].haplosnp = new short int*[this->nsample];
			this->locus[loc].samplesize =new int[this->nsample];
			categ = this->locus[loc].type % 5;
			for (int ech=0;ech<this->nsample;ech++) {
				this->locus[loc].samplesize[ech] = this->ssize[categ][ech];
				this->locus[loc].haplosnp[ech] = new short int[this->ssize[categ][ech]];
				for (int i=0;i<this->ssize[categ][ech];i++) f0.read((char*)&(this->locus[loc].haplosnp[ech][i]),sizeof(short int));
			}
		}
		f0.close();
		delete []buffer;
		cout<<"dans libin     ";
		cout<<"this->locus[0].haplosnp[0][0]="<<this->locus[0].haplosnp[0][0]<<"\n";
	}

/**
* lecture d'un fichier de donnée GENEPOP et stockage des informations dans une structure DataC
*/
	int DataC::readfile(string filename){
		bool fin;
		string s,s1,s2,locusname;
		int ech,ind,nech,*nindi;
		size_t j,j0,j1;
		Aindivname=false;Agenotype=false;Anind=false;Aindivsexe=false;Alocus=false;
        stringstream out;
		ifstream file(filename.c_str(), ios::in);
		if (file == NULL) {
			this->message = "Data.cpp File "+filename+" not found";
			return 1;
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
	    	if (trim(s) != "") {
				s1=majuscules(s);
				s1=purgetab(s1);
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
		cout<<"  nmisshap="<<nmisshap<<"\n";
		this->nsample=nech;
		this->nind.resize(nech);this->Anind=true;
		for (int i=0;i<nech;i++) {this->nind[i]=nindi[i];}
		if (this->nmisshap>0) this->misshap = new MissingHaplo[this->nmisshap];
		if (this->nmissnuc>0) this->missnuc = new MissingNuc[this->nmissnuc];
	    this->nmisshap=0;
		this->nmissnuc=0;
		this->indivname = new string*[nech];this->Aindivname=true;
		this->indivsexe.resize(nech);this->Aindivsexe=true;
		genotype = new string**[nech];Agenotype=true;
		for (int i=0;i<nech;i++) {
			this->indivname[i]= new string[nind[i]];
			this->indivsexe[i].resize(nind[i]);
			for(j=0; (int)j < this->nind[i];j++) this->indivsexe[i][j] = 2;
			genotype[i] = new string*[nind[i]];
			for(j=0; (int)j < this->nind[i];j++) genotype[i][j] = new string[this->nloc];
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
								 return 1;
								}
				s=s.substr(0,j-1);
			} else this->locus[loc].type=0;
			//this->locus[loc].name = new char[s.length()+1];
			//strncpy(this->locus[loc].name, s.c_str(), s.length());
			//this->locus[loc].name[s.length()]='\0';
			this->locus[loc].name = s;
		}
		for (int loc=0;loc<this->nloc;loc++) this->locus[loc].nsample = this->nsample;
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
					iss >> genotype[ech][ind][i];
					if ((genotype[ech][ind][i].find("[")!=string::npos)and(this->locus[i].type<5)) this->locus[i].type +=5;
//					cout << genotype[ech][ind][i] << "   "<<this->locus[i].type<<"\n";
				}
			}
		}
		file2.close();
		delete [] nindi;
                //cout<<"dans data nsample = "<<this->nsample<<"\n";
		return 0;
	}

/**
* traitement des génotypes microsat au locus loc
*/
    void DataC::do_microsat(int loc){
    	string geno,*gen;
    	int l,ll,n,gg,ng;
    	gen = new string[2];
        vector <int> haplo;
    	this->locus[loc].mini=1000;this->locus[loc].maxi=0;
    	this->locus[loc].haplomic = new int*[this->nsample];
     	this->locus[loc].samplesize = new int[this->nsample];
       //cout<<"data Locus="<<loc<<"\n";
    	for (int ech=0;ech<this->nsample;ech++){
            ng=0;
    		for (int ind=0;ind<this->nind[ech];ind++){
    			geno=string(genotype[ech][ind][loc]);
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
                    if (gen[i]!="000") {
    					gg = atoi(gen[i].c_str());
    					haplo.push_back(gg);
    					if (gg>this->locus[loc].maxi) this->locus[loc].maxi=gg;
    					if (gg<this->locus[loc].mini) this->locus[loc].mini=gg;
    				} else {
    					//if (not ((this->indivsexe[ech][ind]==2)and(this->locus[loc].type==3))) { // on ne prend pas en compte les chromosomes Y des femelles
                        this->nmisshap +=1;
                        this->misshap[this->nmisshap-1].sample=ech;
                        this->misshap[this->nmisshap-1].locus=loc;
                        this->misshap[this->nmisshap-1].indiv=ng;
                        haplo.push_back(-9999);
    					//cout <<"misshap["<<this->nmisshap-1 <<"]  sample="<<ech<<"  loc="<<loc<<"  indiv="<<ng<<"\n";
    				}
    				ng++;
    			}
    		}
    		this->locus[loc].samplesize[ech] = ng;
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
    void DataC::do_sequence(int loc){
        //cout<<"do_sequence locus"<<loc<<"\n";
		cout<<"loc="<<loc<<"\n";
    	string geno,*gen;
    	int n,j0,j1,j2,ng,*ss;
    	gen = new string[2];
    	string sbase,seq;
    	char base[] ="ACGT";
        vector <string> haplo;
    	this->locus[loc].haplodna = new string*[this->nsample];
    	this->locus[loc].samplesize = new int[this->nsample];
		ss = new int[this->nsample];
    	this->locus[loc].dnalength = -1;
    	for (int ech=0;ech<this->nsample;ech++){
            ng=0;
    		for (int ind=0;ind<this->nind[ech];ind++){
    			geno=genotype[ech][ind][loc];
    			if (geno.find("][")==string::npos) n=1; else n=2;
    			if (n==2) {
    				j0=geno.find("[")+1;
    				j1=geno.find("][");
    				j2=geno.find("]",j1+2);
    				if ((this->locus[loc].dnalength<0)and(j1-j0>0)) this->locus[loc].dnalength=j1-j0;
    				if ((this->locus[loc].dnalength>0)and ((j1-j0!=this->locus[loc].dnalength)or(j2-(j1+2)!=this->locus[loc].dnalength))and(j1>j0)) {
    					std::stringstream ss;
    					ss <<"ERROR : At locus "<<loc+1<<" individual "<<this->indivname[ech][ind]<<", the sequence length has changed. Please, give it the same length as before.";
    					this->message=ss.str();
						cout<<ss.str()<<"\n";
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
    					ss <<"ERROR : At locus "<<loc+1<<" individual "<<this->indivname[ech][ind]<<", the sequence length has changed. Please, give it the same length as before.";
    					this->message=ss.str();
						cout<<ss.str()<<"\n";
    					return;
    				}
    				gen[0]=geno.substr(j0,j1-j0);
    				if (this->locus[loc].type==2) this->indivsexe[ech][ind]=1;
    			    if ((this->locus[loc].type==3)and(geno!="[]")) this->indivsexe[ech][ind]=1;

    			}
    			for (int i=0;i<n;i++) {
                    haplo.push_back(gen[i]);
    				if (gen[i]!="") {
    					j0=min(gen[i].find("-"),gen[i].find("N"));
    					while (j0!=string::npos) {
    						this->nmissnuc +=1;
    						this->missnuc[this->nmissnuc-1].sample=ech;
    						this->missnuc[this->nmissnuc-1].locus=loc;
   						    this->missnuc[this->nmissnuc-1].indiv=ng;
    						this->missnuc[this->nmissnuc-1].nuc=j0;
    						j0=min(gen[i].find("-",j0+1),gen[i].find("N",j0+1));
    					}
    				} else {
                        //cout <<indivname[ech][ind]<<"donnée manquante"<<"   nmisshap="<<this->nmisshap<<  "\n";
                        this->nmisshap +=1;
                        this->misshap[this->nmisshap-1].sample=ech;
                        this->misshap[this->nmisshap-1].locus=loc;
                        this->misshap[this->nmisshap-1].indiv=ng;
    				}
                    ng++;
    			}
    		}
    		ss[ech] = ng;
    		//cout <<"ng="<<ng<<"\n";
			this->locus[loc].samplesize[ech] = ng;
            this->locus[loc].haplodna[ech] = new string[ng];
            for (int i=0;i<ng;i++) {
                this->locus[loc].haplodna[ech][i]=haplo[i];
                haplo[i].clear();
                if (loc==20) cout<<"ech_"<<ech<<"  ind_"<<i<<"  locus_"<<loc<<"  "<<this->locus[loc].haplodna[ech][i]<<"("<<this->locus[loc].haplodna[ech][i].length()<<")\n";
            }
            //cout<<"\n\n";
            if (not haplo.empty()) haplo.clear();
            //cout<<"coucou\n";
    	}
		this->locus[loc].pi_A=0.0;this->locus[loc].pi_C=0.0;this->locus[loc].pi_G=0.0;this->locus[loc].pi_T=0.0;
		double nn=0.0;
    	for (int ech=0;ech<this->nsample;ech++){
    		for (int i=0;i<ss[ech];i++){
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
		cout<<"loc="<<loc<<"\n";
        if (loc==20) {
			for (int ech=0;ech<this->nsample;ech++){
				for (int i=0;i<ss[ech];i++) {
					cout<<"ech_"<<ech<<"  ind_"<<i<<"   "<<this->locus[loc].haplodna[ech][i]<<"\n";
				}
			}
		}
    }

/**
* calcul du coefficient dans la formule de coalescence en fonction du type de locus
* 0:autosomal diploide, 1:autosomal haploïde, 2:X-linked, 3:Y-linked, 4:mitochondrial
*/
    void DataC::cal_coeffcoal(int loc){
		double coeff=0.0;
		switch (this->locus[loc].type % 5)
		{	case 0 :  coeff = 16.0*this->sexratio*(1.0-this->sexratio);break;
			case 1 :  coeff = 2.0;break;
			case 2 :  coeff = 18.0*this->sexratio*(1.0-this->sexratio)/(1.0+this->sexratio);break;
			case 3 :  coeff = 2.0*this->sexratio;break;
			case 4 :  coeff = 2.0*(1.0-this->sexratio);break;
		}
		this->locus[loc].coeffcoal=coeff;
        //cout<<"locus "<<loc<<"    sexratio="<<this->sexratio<<"    coefficient="<<this->locus[loc].coeffcoal<<"\n";
    }


/**
* chargement des données dans une structure DataC
*/
    int DataC::loadfromfile(string filename) {
    	int loc,kloc;
    	string filenamebin;
    	fstream fs;
    	int error =0;
    	//size_t k=filename.find_last_of(".");
    	filenamebin=filename+".bin";
    	cout<<filenamebin<<"\n";
    	this->filetype = this->testfile(filename);
		cout<<"this->filetype = "<<this->filetype<<"\n";
    	if (this->filetype==-2) {
    		this->message = "Unreckognized file format";
    		error =1; return 1;
    	} else if (this->filetype==-1) {
    		this->message = "data file not found";
    			error =1; return error;
    	}
    	if (this->filetype==0) {
    		error = this->readfile(filename);
    		if (error != 0) return error;
    		kloc=this->nloc;
    		for (loc=0;loc<kloc;loc++) {
    			if (this->locus[loc].type<5) this->do_microsat(loc);
    			else                         this->do_sequence(loc);
    			this->cal_coeffcoal(loc);
    		}
    		this->calcule_ss();
    	}
    	if (this->filetype==1) {
    		fs.open(filenamebin.c_str(),ios::in|ios::binary);
    		if (fs) {
				cout<<"lecture du fichier binaire\n";
    			fs.close();
    			this->libin(filenamebin);
				cout<<"sexratio="<<this->sexratio<<"\n";
    			this->sexratio=0.5;
    			for (loc=0;loc<this->nloc;loc++) this->cal_coeffcoal(loc);
    			cout<<"fin de la lecture du fichier binaire\n";
    		} else {
    			error = this->readfilesnp(filename);
    			if (error != 0) return error;
    			cout<<"fin de la lecture\n";
    			this->purgelocmonomorphes();cout<<"fin de la purge des monomorphes\n";
    			cout<<"sexratio="<<this->sexratio<<"\n";
    			for (loc=0;loc<this->nloc;loc++) {this->do_snp(loc);this->cal_coeffcoal(loc);}
    			cout<<"apres le' traitement' des snp\n";
    			this->calcule_ss();
    			cout<<"reecriture dans le fichier binaire "<<filenamebin<<"\n";
    			this->ecribin(filenamebin);
    			cout<<"relecture du fichier binaire\n";
    			this->libin(filenamebin);
    			cout<<"fin de la lecture du fichier binaire\n";
    			for (loc=0;loc<this->nloc;loc++) this->cal_coeffcoal(loc);
				cout<<"apres le calcul des coefficients de coalescence\n\n";
    		}
    		this->calcule_ss();
    		cout<<"avant missingdata\n";
    		this->missingdata();
			cout<<"apres missingdata\n";
    	}
    	this->nsample0 = this->nsample;
    	return 0;
    }


