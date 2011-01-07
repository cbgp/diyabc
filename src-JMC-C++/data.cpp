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
	str s;
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
		getline(file,)
	}
	return gpdata;
}

