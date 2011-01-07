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
//Proprietes des locus sequences
	double pi_A,pi_C,pi_G,pi_T;
	double *mutsit;   //array of dnalength elements giving the relative probability of a mutation to a given site of the sequence
	int *sitmut;   //array of dna sites that are changed through a mutation
	int dnalength;
	int *tabsit;
//	int *haplostate; //array[gene copy] tous les "gene copies" sont mises à la suite les unes des autres (groupées par sample)
	int **haplomic; //array[sample][gene copy]
//Proprietes des locus microsatellites
	int mini,maxi,kmin,kmax,motif_size,motif_range,nal;
	double mut_rate,Pgeom,sni_rate,mus_rate,k1,k2;
	string **haplodna;  //array[sample][gene copy][nucleotide] tous les nucleotides de chaque individu sont mis à la suite les uns des autres
	int *ssmic;
	int *samplesize;
	double **freq;
};

struct MissingHaplo
{
	int locus,sample,indiv;
};

struct MissingNuc
{
	int locus,sample,indiv,nuc;
};

struct DataC
{
	string filename,message,title,**indivname;
	int nsample,nloc;
	int *nind;
	int **indivsexe;
	double sexratio;
	MissingHaplo *misshap;
	MissingNuc   *missnuc;
	LocusC *locus;
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

DataC read_genepop(string filename) {
	bool fin=false;
	string s,s1,s2,locusname;
	char c;
	size_t j;
	DataC gpdata;
	stringstream out;
	gpdata.filename = filename;
	cout << gpdata.filename<<"\n";
	ifstream file(filename.c_str(), ios::in);
	if (file == NULL) {
		gpdata.message = "File not found";
		return gpdata;
	}
	getline(file,gpdata.title);
	cout <<gpdata.title<<"\n";
	gpdata.nloc=0;
	while (not fin) {
		getline(file,s);
		s1=majuscules(s);
		cout << s << "    " << s1 <<"\n";
		if (s1.find("POP")==string::npos){	//il s'agit d'un nom de locus
			gpdata.nloc +=1;
			cout <<"nloc="<<gpdata.nloc<<"\n";
			if (gpdata.nloc==1) gpdata.locus = (LocusC*) malloc(sizeof(LocusC));
			else		        gpdata.locus = (LocusC*) realloc(gpdata.locus,gpdata.nloc*sizeof(LocusC));
			cout <<"s = "<<s<< "\n";
			j=s.find("<");
			//cout <<"j="<<j<<"   npos="<<string::npos<<"\n";
			if (j!=string::npos) {         //il y a un type de locus noté après le nom
				s2=majuscules(s.substr(j+1,1));
				if (s2=="A") gpdata.locus[gpdata.nloc-1].type=0; else
					if (s2=="H") gpdata.locus[gpdata.nloc-1].type=1; else
						if (s2=="X") gpdata.locus[gpdata.nloc-1].type=2; else
							if (s2=="Y") gpdata.locus[gpdata.nloc-1].type=3; else
								if (s2=="M") gpdata.locus[gpdata.nloc-1].type=4; else
									{out << gpdata.nloc;
									 gpdata.message="unrecoknized type at locus "+out.str();
									 return gpdata;
									}
				s=s.substr(0,j-1);
			}
			gpdata.locus[gpdata.nloc-1].name = new char[s.length()+1];
			strncpy(gpdata.locus[gpdata.nloc-1].name, s.c_str(), s.length()+1);
			cout << gpdata.locus[gpdata.nloc-1].name<<"\n";
		} else fin=true;
	}
	gpdata.nsample=0;

	return gpdata;
}


int main(){
	DataC data;
	data = read_genepop("datatest1.txt");
}
