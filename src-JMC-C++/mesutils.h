// mesutils.h
//

#ifndef MESUTILS_H_
#define MESUTILS_H_

using namespace std;

struct matC
{
  long double v;
  int num;
  int ind;
};
/**
* définit l'opérateur de comparaison de deux enregistrements de type matC
* pour l'utilisation de la fonction sort du module algorithm
*/
struct compC
{
  bool operator () (matC const & lhs, matC const & rhs) const;
};

double walltime (double * t0);
int getwordint (string s, int num);
double getwordfloat (string s, int num);
string * splitwords (string s, string sep, int * k);
string * splitwordsR (string s, string sep, int m, int * k);
string centre (string const & s, int k);
int strpos (char * st1, char * st2);
int ndecimales (double mini, double maxi);
string IntToString (int number);
string FloatToString (float number);
string DoubleToString (double number);
string LongDoubleToString (long double number);
string majuscules (string s);
string char2string (char * c);
string trim (string s);
int arrondi (double a);
string TimeToStr (double ti);
double cal_moy (int n, double * x);
long double cal_moyL (int n, long double * x);
double cal_mode (int n, double * x);
long double cal_modeL (int n, long double * x);
double cal_med (int n, double * x);
long double cal_medL (int n, long double * x);
double cal_sd (int n, double * x);
long double cal_sdL (int n, long double * x);
long double cal_varL (int n, long double * x);
double lnormal_dens (double x, double m, double s);
double do_del (double temp, double x);
double pnorm5 (double x, double mu, double sigma);
struct rescov
{
  int n;
  long double * mu;
  long double * * cov;
};
rescov covarianceponderee (int nl, int nc, long double * * A, long double * w);
struct resAFD
{
  int nlambda;
  long double proportion;
  long double * lambda;
  long double slambda;
  long double * * vectprop;
  long double * * princomp;
  long double * moy;
};
resAFD AFD (int nl, int nc, int * pop, long double * omega, long double * * X, long double prop);
int PGCD (int a, int b);
int PPCM (int a, int b);
long double DCVM (int n, int m, long double * x, long double * y);
void combrank2 (int n, int m, long double * x, long double * y, long double * rangx, long double * rangy);
inline string char2string (char * c)
{
	return string(c);
}

#endif
