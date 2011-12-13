/*
 * modchec.h
 *
 *  Created on: 13 déc. 2011
 *      Author: ppudlo
 */

#ifndef MODCHEC_H_
#define MODCHEC_H_
#include <string>

bool resetstats(std::string s);
std::string pseudoprior2(long double x);
int detphistarOK(int nsel,long double **phistar,long double **phistarOK);
void call_loc(int npart,int nstat, int nrec,int nsel,long double **ss, float *stat_obs);
void call_acp(int nr, int ns, int nstat, int *numscen,long double **ssref,
		long double **ssphistar, float *stat_obs);
void domodchec(std::string opt,int seed);


#endif /* MODCHEC_H_ */
