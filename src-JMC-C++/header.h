/*
 * header.h
 *
 *  Created on: 9 déc. 2011
 *      Author: ppudlo
 */

#ifndef HEADER_H_
#define HEADER_H_

class MutParameterC
{
public:
    string name;
    int groupe;
    int category;   //0 pour mutmoy, 1 pour Pmoy, 2 pour snimoy, 3 pour musmoy, 4 pour k1moy et 5 pour k2moy
    double value;
    PriorC prior;

    void ecris();
};


class HeaderC
{
public:
    string message,datafilename,entete,entetehist,entetemut,entetemut0,entetestat;
    string pathbase;
    DataC dataobs;
    int nparamtot,nstat,nscenarios,nconditions,ngroupes,nparamut,nsimfile;
    string *paramname, *statname;
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
	ConditionC readcondition(string ss);
	void assignloc(int gr);
	int readHeader(string headerfilename);
	int readHeadersim(string headersimfilename);
	void calstatobs(string statobsfilename);
};


#endif /* HEADER_H_ */
