/*
 * reftable.h
 *
 *  Created on: 9 déc. 2011
 *      Author: ppudlo
 */

#ifndef REFTABLE_H_
#define REFTABLE_H_
#include <string>
#include <fstream>
#include "history.h"
#include "header.h"

class enregC {
public:
    int numscen;
    float *param,*paramvv,*stat;
    long double dist;
    std::string message;
    friend bool operator<(const enregC & lhs, const enregC & rhs);
};



class ReftableC
{
public:
    int nrec,*nrecscen,nscen,nreclus,nrec0;
    long posnrec;
    std::string datapath, filename, filelog, filename0, filerefscen;
    int *nparam,nstat,po,nparamax,nscenchoisi,*scenchoisi,scenteste,nparamut,*nhistparam;
    float *param,*sumstat;
    HistParameterC **histparam;
    MutParameterC  *mutparam;
    std::fstream fifo;
    int nstatOK,nsel,nenr;
    enregC* enrsel;
    float *stat_obs;
    long double *var_stat;

    void sethistparamname(HeaderC const & header);
    int readheader(std::string fname, std::string flogname, std::string reftabscen);
    int writeheader();
    int readrecord(enregC *enr);
    int writerecords(int nenr, enregC *enr);
    int openfile();
    int openfile2();
    int testfile(std::string reftablefilename, int npart);
    int closefile();
	void bintotxt();
    void concat();
    // calcule les variances des statistiques résumées
    // sur les 100000 premiers enregistrements de la table de référence
    int cal_varstat();
    // alloue / desalloue la mémoire pour enrsel
    void alloue_enrsel(int nsel);
    void desalloue_enrsel(int nsel);
    // calcule la distance de chaque jeu de données simulé au jeu observé
    // et sélectionne les nsel enregistrements les plus proches (copiés dans enregC *enrsel)
    void cal_dist(int nrec, int nsel, float *stat_obs);

};


#endif /* REFTABLE_H_ */
