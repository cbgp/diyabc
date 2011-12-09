/*
 * randomgenerator.cpp
 *
 *  Created on: 23 sept. 2010
 *      Author: cornuet
 */

#include <math.h>
#include <stdlib.h>
extern"C"{
#include "../dcmt0.6.1/include/dc.h"
}
#include "randomgenerator.h"
//#include <time.h>

//#define MODULO 16777216

double sqr(double x) {return x*x;}

extern mt_struct* r;
#pragma omp threadprivate(r)

	void MwcGen::randinit(unsigned int indice, unsigned int indice2) {
//		this->mult = multipli[indice % 10000];
//        srand(indice2);
//        this->mwcx = rand() % MODULO;
//		srand(indice2*indice);
//        this->carry= rand() % MODULO;
//        //std::cout << "mwcx = " << this->mwcx << "   carry = " << this->carry << "\n";
	}
	
	void MwcGen::ecris() {
//		std::cout<<"mwcx ="<<this->mwcx<<"    mult="<<this->mult<<"   carry="<<this->carry<<"\n";
//
	}

    double MwcGen::random() {
//		unsigned long int y ;
//		double r;
//		y = (this->mult)*(this->mwcx) + (this->carry);
//		this->mwcx = y % MODULO;
//		this->carry = y / MODULO;
//		r = (double) this->mwcx /(double)MODULO;
//		return r;
    	return genrand_mt(r)/ 4294967296.0 ;
	}

	int MwcGen::rand0(int max) {
		double r;
		r = floor(max * this->random());
		return (int) r;
	}

	int MwcGen::rand1(int max) {
		double r;
		r = floor(max * this->random()) + 1.0;
		return (int) r;
	}

	int* MwcGen::randperm(int n) {
		int *index,j;
		index = new int[n];
		index[0] = 0;
		for (int i=1;i<n;i++) {
			j = this->rand0(i+1);
			index[i] = index[j];
			index[j] = i;
		}
		return index;
	}

	int MwcGen::poisson (double lambda) {
		if (lambda<=0) {return 0;}
		int i=0;
		double p=0.0;
		double ra;
		while (p<lambda) {
			i++;
			do {ra = this->random();} while (ra==0.0);
			p -= log(ra);
		}
		return (i-1);
	}

	int MwcGen::binom(int n,double p) { // version light valable pour n<=30
	  int k=0;
	  if (n<1) return 0;
	  for (int i=0;i<n;i++) if(this->random() < p) k++;
	  return k;
	}

	double MwcGen::gunif(double min,double max) {
		double u;
		u = min + (max-min)*this->random();
		return u;
	}

	double MwcGen::glogunif(double min,double max) {
		double u,logmin,logmax;
		logmin=log(min);logmax=log(max);
		u = logmin + (logmax-logmin)*this->random();
                //cout <<logmin<<"   "<<logmax<<"   "<<u<<"   "<<exp(u)<<"\n";
		return exp(u);
	}

	double MwcGen::gnormal() {
		const double TWOPI = 6.283185307; //8.0*atan(1.0);
		double u1,u2,c,d;
		do {u1 = this->random();} while (u1<0.0000001);
		u2 = this->random();
		c = sqrt(-2.0*log(u1));
		d = TWOPI*u2;
		if (this->random()<0.5) return c*cos(d);
		else 					return c*sin(d);
	}

	double MwcGen::gnorm(double mean,double sd) {return mean + sd*this->gnormal();}

	double MwcGen::glognorm(double mean,double sd) {
		double u,logmean,logsd;
		logmean=log(mean);logsd=log(sd);
		u = gnorm(logmean,logsd);
		return exp(u);
	}

	double MwcGen::ggamma (double shape, double rate) {
		const double log4=1.3862943611198906188344642429164,sg_magic=2.5040773967762740733732583523869,e=2.7182818284590452353602874713527;
		double alpha,beta,ainv,bbb,ccc,u1,u2,v,x,z,r,res;
		bool fin=false;;
		alpha=shape;
		beta=1.0/rate;
		if (alpha>1.0) {
			ainv = sqrt(2.0*alpha-1.0);
			bbb = alpha - log4;
			ccc = alpha+ainv;
			while (not fin) {
				do {u1 = this->random();} while ((u1<0.0000001)or(u1>0.9999999));
				u2 = 1.0 - this->random();
				v = log(u1/(1.0-u1))/ainv;
				x = alpha*exp(v);
				z = u1*u1*u2;
				r = bbb + ccc*v - x;
				fin = ((r + sg_magic -4.5*z >= 0.0)or(r >= log(z)));
			}
			res = x*beta;
		}
		else {
			if (alpha==1.0) {
				do {u1 = this->random();} while (u1<0.0000001);
				res = -log(u1)*beta;
			}
			if (alpha<1.0) {
				while (not fin) {
					u2 = this->random();
					bbb = (e+alpha)/e;
					v = bbb*u2;
					if (v<=1.0) x = exp((1.0/alpha)*log(v));
					else        x = -log((bbb-v)/alpha);
					u1 = this->random();
					if (v>1.0)  fin = (u1 <= exp((alpha-1.0)*log(x)));
					else         fin = (u1 <= exp(x));
				}
				res = x*beta;
			}
		}
		return res;
	}

	double MwcGen::ggamma2(double mean, double sd) {
		double shape,rate;
		if (sd<=0.0) return -1.0;
		shape = sqr(mean/sd);
		rate = mean/sqr(sd);
		return this->ggamma(shape,rate);
	}

	double MwcGen::ggamma3(double mean, double shape) {
		double sd;
		if (shape==0.0) return 1.0;
		sd = mean/sqrt(shape);
		return this->ggamma2(mean,sd);
	}


