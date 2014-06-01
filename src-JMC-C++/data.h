/*
 * data.h
 *
 *  Created on: 9 déc. 2011
 *      Author: ppudlo
 */

#ifndef DATA_H_
#define DATA_H_

#include <string>
#include <vector>

class MissingSnp
{
public:
	int sample,indiv;
	MissingSnp & operator= (MissingSnp const & source);
};

/**
*  Structure LocusC : définition de la structure LocusC
*/
class LocusC
{
public:
  std::string name;
  int type;  //0 à 14
  int groupe;    //numero du groupe auquel appartient le locus
  double coeffcoal;  // coefficient pour la coalescence (dépend du type de locus et du sexratio)
  long double **freq;
  //Proprietes des locus sequences
  long double pi_A,pi_C,pi_G,pi_T;
  std::vector <long double> mutsit;   //array of dnalength elements giving the relative probability of a mutation to a given site of the sequence
  std::vector <int> sitmut;   //array of nsitmut dna sites that are changed through a mutation
  std::vector <int> sitmut2;  //
  int dnalength,dnavar;
  std::vector <int> tabsit;   //array of dnalength elements giving the number of a dna site;
  std::string **haplodna; //array[sample][gene copy][nucleotide] tous les nucleotides de chaque individu sont mis à la suite les uns des autres
  std::string **haplodnavar; //seulement les sites variab
  //Proprietes des locus microsatellites
  int mini,maxi,kmin,kmax,motif_size,motif_range,nal;
  double mut_rate,Pgeom,sni_rate,mus_rate,k1,k2;
  int **haplomic; //array[sample][gene copy]
  //Propriétés des locus SNP
  bool firstime;
  short int **haplosnp; //array[sample][gene copy] 0,1,9
  bool mono;  //mono=true si un seul allèle dans l'échantillon global
  double weight; //poids du locus=1, sauf quand biais de recrutement
  int nsample,*samplesize,nmisssnp;
  MissingSnp *misssnp;
  
  
	LocusC() {
		freq = NULL;
		mutsit.clear();
		sitmut.clear();
		sitmut2.clear();
		tabsit.clear();
		haplodna = NULL;
		haplodnavar = NULL;
		haplomic = NULL;
		haplosnp = NULL;
		samplesize = NULL;
		freq = NULL;
		name="";
		type=-1;
		groupe=-1;
		pi_A=pi_C=pi_G=pi_T=-1.0;
		dnalength=dnavar=-1;
		mini=maxi=kmin=kmax=-1;
		motif_range=motif_size=-1;
		mut_rate=Pgeom=sni_rate=mus_rate=k1=k2=0.0;
		firstime=mono=true;
		weight=1.0;
		nsample=0;
		misssnp = NULL;
	};
	~LocusC() {
		/*if (freq != NULL) delete [] freq;
		if (haplodna != NULL){ 
			for (int i=0;i<this->nsample;i++) delete [] haplodna[i];
			delete [] haplodna;
		}
		if (haplodnavar != NULL) { 
			for (int i=0;i<this->nsample;i++) delete [] haplodnavar[i];
			delete [] haplodnavar;
		}
		if (haplomic != NULL) { 
			for (int i=0;i<this->nsample;i++) delete [] haplomic[i];
			delete [] haplomic;
		}
		if (haplosnp != NULL) { 
			for (int i=0;i<this->nsample;i++) delete [] haplosnp[i];
			delete [] haplosnp;
		}*/
		if (not mutsit.empty()) mutsit.clear();
		if (not sitmut.empty()) sitmut.clear();
		if (not sitmut2.empty()) sitmut2.clear();
		if (not tabsit.empty()) tabsit.clear();
	} ;
	
	LocusC & operator= (LocusC const & source);

  void libere(bool obs, int nsample);
};

class MissingHaplo
{
public:
	int locus,sample,indiv;
	MissingHaplo & operator= (MissingHaplo const & source);
};

class MissingNuc
{
public:
	int locus,sample,indiv,nuc;
	MissingNuc & operator= (MissingNuc const & source);
};


class DataC
{
public:
	std::string message,title,**indivname;
	int nsample,nsample0,nloc,nmisshap,nmissnuc,nmisssnp,filetype;
	//int *nind;
	//int **indivsexe;
	double sexratio,maf;
	MissingHaplo *misshap, *misssnp;
	MissingNuc   *missnuc;
	LocusC *locus;
    bool Aindivname,Anind,Aindivsexe,Alocus;
	//int **ss;  //nombre de copies de gènes (manquantes incluses) par [locustype][sample], locustype variant de 0 à 4.
	bool *catexist;
	std::vector < std::vector <int> > ssize;//nombre de copies de gènes (manquantes incluses) par [locustype][sample], locustype variant de 0 à 4.
	std::vector <int> nind;
	std::vector < std::vector <int> > indivsexe;
	/*vector < vector <int> > ss;//nombre de copies de gènes (manquantes incluses) par [locustype][sample], locustype variant de 0 à 4.
	vector <int> nind;
	vector < vector <int> > indivsexe;*/

	/* Méthodes */
	DataC(){
		indivname = NULL;
		misshap = NULL;
		misssnp = NULL;
		missnuc = NULL;
		locus = NULL;
		catexist = NULL;
		ssize.clear();
		indivsexe.clear();
	};
	~DataC(){
		/*if( indivname != NULL) delete [] indivname;
		if( genotype != NULL) delete [] genotype;
		if( misshap != NULL) delete [] misshap;
		if( misssnp != NULL) delete [] misssnp;
		if( missnuc != NULL) delete [] missnuc;
		if( locus != NULL) delete [] locus;
		if( catexist != NULL) delete [] catexist;*/
	};
	DataC & operator= (DataC const & source);
	
	void libere();
	/**
	 * détermination du type de fichier de donnée
	 * return=-1
	 * return=0 si genepop
	 * return=1 si snp
	 */
	int testfile(std::string filename);
	/**
	 * lecture d'un fichier de donnée SNP et stockage des informations dans une structure DataC
	 */
	int  readfilesnp(std::string filename);
	/**
	 * supprime les locus monomorphes
	 */
	void purgelocmonomorphes();
	void missingdata();
	/**
	 * traitement des locus snp
	 */
	void do_snp(int loc);


	/**
	 * ecriture en binaire d'un fichier snp
	 */
	void ecribin(std::string filenamebin);
	/**
	 * lecture en binaire d'un fichier snp
	 */
	void libin(std::string filenamebin);
	/**
	 * lecture d'un fichier de donnée GENEPOP et stockage des informations dans une structure DataC
	 */
	int readfile(std::string filename);
	/**
	 * traitement des génotypes microsat au locus loc
	 */
	void do_microsat(int loc);
	/**
	 * traitement des génotypes DNA sequence au locus loc
	 */
	void do_sequence(int loc);
	/**
	 * calcul du coefficient dans la formule de coalescence en fonction du type de locus
	 * 0:autosomal diploide, 1:autosomal haploïde, 2:X-linked, 3:Y-linked, 4:mitochondrial
	 */
	void cal_coeffcoal(int loc);

	void calcule_ss();
	/**
	 * chargement des données dans une structure DataC
	 */
	int loadfromfile(std::string filename);
};



#endif /* DATA_H_ */
