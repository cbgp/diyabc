/*
 * randomgenerator.h
 *
 *  Created on: 8 déc. 2011
 *      Author: ppudlo
 */

#ifndef RANDOMGENERATOR_H_
#define RANDOMGENERATOR_H_

double sqr(double);

class MwcGen  // FIXME: attention, n'est plus un mwc.
{
public:
	// FIXME: attention, ne sert plus a rien !!!!
	void randinit(unsigned int indice, unsigned int indice2);

	// FIXME: attention, ne sert plus a rien !!!!
	void ecris();

	// renvoie un nombre entre 0 (inclus) et 1 (exclu)
    double random();
    // renvoie un entier entre 0 (inclus) et max (exclu)
	int rand0(int max);
	// renvoie un entier entre 1 (inclus) et max (inclus)
	int rand1(int max);
	// renvoie une permutation aleatoire de [0, n-1]
	int* randperm(int n);


	int poisson (double lambda);
	int binom(int n,double p); // version light valable pour n<=30
	double gunif(double min,double max);
	double glogunif(double min,double max);
	double gnormal();
	double gnorm(double mean,double sd);
	double glognorm(double mean,double sd);
	double ggamma (double shape, double rate);
	double ggamma2(double mean, double sd);
	double ggamma3(double mean, double shape);
};

#endif /* RANDOMGENERATOR_H_ */
