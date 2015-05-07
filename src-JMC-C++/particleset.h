/*
 * particleset.h
 *
 *  Created on: 9 déc. 2011
 *      Author: ppudlo
 */

#ifndef PARTICLESET_H_
#define PARTICLESET_H_

#include "particuleC.h"


/**
* Structure ParticleSet : définition d'un ensemble de particules
*/
struct ParticleSetC
{
	HeaderC header;
	std::vector<ParticleC> particule;
	std::vector<LocusC> locuslist;
	std::vector<LocusGroupC> grouplist;
	int npart,nloc,ngr;
	int nsample,nscenarios;
	std::vector<int> nind;
	std::vector<std::vector<int> > indivsexe;
	std::vector<ScenarioC> scenario;
	double sexratio;
    MwcGen *mw;

    ParticleSetC():particule(0), locuslist(0), grouplist(0),
    		nind(0), indivsexe(0), scenario(0), mw(0) {};

    // remplissage de la partie data de la particule p
    void setdata(int p);
    // remplissage de la partie groupe de la particule p
    void setgroup(int p);
    // remplissage de la partie locus de la particule p
    void setloci(int p);
    // remplissage de la partie scenarios de la particule p
    void setscenarios (int p, bool simulfile);
    // réinitialisation de la particule p
    void resetparticle (int p);
	// liberation des particules
	void libere(int npart);
	// simulation de particules de paramètres fixées
	void dosimulstat(HeaderC const & header, int debut, int npart, bool dnatrue, bool multithread, int numscen, int seed, double **stat);    
	// simulation des particules utilisées pour le model checking
    void dosimulphistar(HeaderC const & header, int npart, bool dnatrue,
    		            bool multithread,bool firsttime, int numscen,int seed,int nsel);
    // simulation des particules utilisées pour la table de référence, le biais et la confiance
    void dosimultabref(HeaderC const & header, int npart, bool dnatrue,bool multithread,
    		           bool firsttime, int numscen,int seed, int depuis);
    // simulation des particules utilisées pour la création de fichiers genepop
    string* simulgenepop(HeaderC const & header, int npart, bool multithread, int seed);
    // simulation des particules utilisées pour la création de fichiers SNP
    string* simuldataSNP(HeaderC const & header, int npart, bool multithread, int seed);
};


#endif /* PARTICLESET_H_ */
