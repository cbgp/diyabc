/*
 * header.h
 *
 *  Created on: 9 déc. 2011
 *      Author: ppudlo
 */

#ifndef HEADER_H_
#define HEADER_H_
#include <string>
#include <fstream>
#include "data.h"
#include "history.h"
#include "particuleC.h"
#include "header.h"


class MutParameterC
{
public:
    std::string name;
    int groupe;
    int category;   //0 pour mutmoy, 1 pour Pmoy, 2 pour snimoy, 3 pour musmoy, 4 pour k1moy et 5 pour k2moy
    double value;
    PriorC prior;

    void ecris();
};


class HeaderC
{
public:
    std::string message,datafilename,entete,entetehist,entetemut,entetemut0,entetestat;
    std::string pathbase;
    DataC dataobs;
    int nparamtot,nstat,nscenarios,nconditions,ngroupes,nparamut,nsimfile;
    std::string *paramname, *statname;
    ScenarioC *scenario,scen;
    HistParameterC *histparam;
    ConditionC *condition;
    LocusGroupC *groupe;
    bool drawuntil;
    ParticleC particuleobs;
    MutParameterC *mutparam;
    float *stat_obs;
	float reffreqmin;

	void libere();
	void assignloc(int gr);

	int readHeaderDebut(std::ifstream & file);
	int readHeaderScenarios(std::ifstream & file);
	int readHeaderHistParam(std::ifstream & file);
	int readHeaderLoci(std::ifstream & file);
	int readHeaderGroupPrior(std::ifstream & file);
	int readHeaderGroupStat(std::ifstream & file);
	int readHeaderEntete(std::ifstream & file);

	int buildSuperScen();
	int buildMutParam();

	int readHeader(std::string headerfilename);
	int readHeadersim(std::string headersimfilename);
	void calstatobs(std::string statobsfilename);
};


#endif /* HEADER_H_ */
