/*
 * data.cpp
 *
 * Created on : 07 january 2010
 *     Author : cornuet
 *
 */


#include <iostream>
#include <string>

using namespace std;

struct LocusC
{
	string name;
	int type;  //0 à 9
	int groupe;    //numero du groupe auquel appartient le locus
	double coeff;  // coefficient pour la coalescence (dépend du type de locus et du sexratio)
//Proprietes des locus sequences
	double pi_A,pi_C,pi_G,pi_T;
	double *mutsit;   //array of dnalength elements giving the relative probability of a mutation to a given site of the sequence
	int *sitmut;   //array of dna sites that are changed through a mutation
	int dnalength;
	int *tabsit;
	int *haplostate; //array[gene copy] tous les "gene copies" sont mises à la suite les unes des autres (groupées par sample)
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

DataC read_genepop(string filename) {
	bool fin=false;
	str s,s2;
	int j;

	gpdata = new DataC;
	gpdata.filename = filename;
	ifstream file(filename, ios::in|ios::nocreate);
	if (file == NULL) {
		gpdata.message = "File not found";
		return gpdata;
	}
	getline(file,gpdata.title);
	gpdata.nloc=0;
	while (not fin) {
		getline(file,s);
		for (int i=0;i<s.length;i++) s[i] =toupper(s[i]);
		if (s.find("POP")==string::npos){	//il s'agit d'un nom de locus
			gpdata.nloc +=1;
			if (nloc==1) gpdata.locus = (LocusC*) malloc(sizeof(LocusC));
			else		 gpdata.locus = (LocusC*) realloc(gpdata.locus,nloc*sizeof(LocusC));
			j=s.find("<");
			if (j!=string::npos) {         //il y a un type de locus noté après le nom
				s2=toupper(s[j+1]);
				if (s2=="A") gpdata.locus[nloc-1].type=0; else
					if (s2=="H") gpdata.locus[nloc-1].type=1; else
						if (s2=="X") gpdata.locus[nloc-1].type=2; else
							if (s2=="Y") gpdata.locus[nloc-1].type=3; else
								if (s2=="M") gpdata.locus[nloc-1].type=4; else
									{sprintf(gpdata.message,"unrecoknized type at locus %d",nloc);
									 return gpdata;
									}
				s=s.substr(0,j-1);
			}
			gpdata.locus.name=s;
		} else fin=true;
	}
	return gpdata;
}

