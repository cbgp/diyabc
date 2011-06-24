#ifndef STRING
#include <string>
#define STRING
#endif

#ifndef ALGORITHM
#include <algorithm>
#define ALGORITHM
#endif
#ifndef SYS_TIMEH
#include <sys/time.h>
#define SYS_TIMEH
#endif
#ifndef MATHH
#include <math.h>
#define MATHH
#endif

#ifndef MATRICES
#include "matrices.cpp"
#define MATRICES
#endif

using namespace std;

#define PI 3.141592653589793

double walltime( double *t0 )
{
  double mic, time;
  double mega = 0.000001;
  struct timeval tp;
  struct timezone tzp;
  static long base_sec = 0;
  static long base_usec = 0;

  (void) gettimeofday(&tp,&tzp);
  if (base_sec == 0)
    {
      base_sec = tp.tv_sec;
      base_usec = tp.tv_usec;
    }

  time = (double) (tp.tv_sec - base_sec);
  mic = (double) (tp.tv_usec - base_usec);
  time = (time + mic * mega) - *t0;
  return(time);
}

int getwordint(string s,int num){
	s.append(" ");
	while (s.find(" ")== 0) s=s.substr(1);
	int i=1,j;
	while ((i<num)and(s.length()>1)) {
		j=s.find(" ");
		s=s.substr(j);
		while (s.find(" ")== 0) s=s.substr(1);
		i++;
	}
	s=s.substr(0,s.find(" "));
	if ((s.find("(")==0)or(s.find("[")==0)) s=s.substr(1,s.length()-2);
	return atoi(s.c_str());
}

double getwordfloat(string s,int num){
	s.append(" ");
	while (s.find(" ")== 0) s=s.substr(1);
	int i=1,j;
	while ((i<num)and(s.length()>1)) {
		j=s.find(" ");
		s=s.substr(j);
		while (s.find(" ")== 0) s=s.substr(1);
		i++;
	}
	s=s.substr(0,s.find(" "));
	if ((s.find("(")==0)or(s.find("[")==0)) s=s.substr(1,s.length()-2);
	return atof(s.c_str());
}

string* splitwords(string s,string sep,int *k){
	int j=0,j0,j1;
	while (s.find(sep)== 0) s=s.substr(1);
	*k=0;
	s.append(sep);
	string *sb,s0,s1;
	s1=string();
	for (int i=0;i<s.length();i++) {
		s0=s.substr(i,1);
		if (s0==sep){
			j++;
			if (j==1) {
				s1.append(s0);
				if (j==1) (*k)++;
				//cout <<" j=1  k="<<*k<<"\n";
			}
		} else {
			s1.append(s0);
			j=0;
		}
	}
	sb = new string[*k];
	for (int i=0;i<*k;i++) {
		j0=s1.find(sep);
		sb[i]=s1.substr(0,j0);
		s1=s1.substr(j0+1,s.length());

	}
	//cout <<"k="<<*k<<"\n";
	return sb;
}

string centre(string s,int k){
        int l=s.length();
        string sb=s;
        int av,ap;
        av=(k-l)/2;
        ap=k-l-av;
        if ((av<=0)and(ap<=0)) return s;
        if (av<=0) return s+" ";
        sb=s;
        for (int i=0;i<av;i++) sb=" "+sb;
        if (ap==0) return sb;
        for (int i=0;i<ap;i++) sb=sb+" ";
        return sb;
}

int strpos(char* st1,char* st2) {
    int i,p=0,n1,n2;
    char *s;
    n1 = strlen(st1);
    n2 = strlen(st2);
    if (n2<n1) {
      s = new char[n2+1];
      s[strlen(st2)]='\0';
      for (p=0;p<n1-n2;p++) {
           for (i=0;i<n2;i++) s[i]=st1[p+i];
           if (strcmp(s,st2)==0) break;
      }
    }
    return p;

}


int ndecimales(double mini,double maxi){
  double p;
  int k;
  if (mini>=1) return 0; 
  p = mini/100; 
  k = 0;
  while (abs((long int)(p-round(p)))>1E-12) {k++;p=10*p;}
  return k;
}

string IntToString ( int number )
{
  std::ostringstream oss;
  oss<< number;
  return oss.str();
}
string FloatToString ( float number )
{
  std::ostringstream oss;
  oss<< number;
  return oss.str();
}


string DoubleToString ( double number )
{
  std::ostringstream oss;
  oss<< number;
  return oss.str();
}

string majuscules(string s) {
    string s2;
    char *c;
    c = new char[s.length()+1];
    for (int i=0;i<s.length();i++) {
        c[i]=s[i];
        c[i]=toupper(c[i]);
    }
    s2 = string(c,s.length());
    delete [] c;
    return s2;
}

string char2string(char *c) {
    stringstream s;
    s<<c;
    return s.str();
}

char* str2char(string x)
{
    size_t size = x.size() + 1;
    char* conv = new char[ size ];
    strncpy( conv, x.c_str(), size );
    return(conv);
}

int arrondi(double a) {return (int)(a + 0.5);}

string TimeToStr(double ti) {
    string stime="";
    int m_s,sec,min,hou,day;
    day = int(ti)/86400; ti=ti-(double)day*86400;
    hou = int(ti)/3600;  ti=ti-(double)hou*3600;
    min = int(ti)/60;    ti=ti-(double)min*60;
    sec = int(ti)   ;    ti=ti-(double)sec; 
    m_s = int(1000*ti);
    if (day>0) { 
        stime=IntToString(day)+" day";
        if (day>1) stime+="s "; else stime+=" ";
    }
    if (hou>0) stime += IntToString(hou)+" h ";
    if (min>0) stime += IntToString(min)+" min ";
    if ((day>0)or(hou>0)) return stime;
    if (sec>0) stime += IntToString(sec)+" s ";
    if (min>0) return stime; 
    if (m_s>0) stime += IntToString(m_s)+" ms ";
    return stime;
}

double cal_moy(int n, double *x) {
    double sx=0;
    for (int i=0;i<n;i++) sx +=x[i];
    if (n>0) return sx/(double)n;
    else return 0.0;
}

double cal_mode(int n, double *x) {
    int l0=1,l1,l2=n,dl,lmin;
    double min;
    dl=(l2-l0)/2;
    while (x[l0-1] != x[l2-1]) {
        min=x[l0+dl-1] - x[l0-1];lmin=l0;
        for (int l=l0;l<l2-dl;l++) {
            if(x[l+dl-1]-x[l-1]<min) {
                min=x[l+dl-1]-x[l-1];
                lmin=l;
            }
        }
        l0=lmin;l2=lmin+dl;dl=(l2-l0)/2;
    }
    return x[l0-1];
}

double cal_med(int n,double *x) {
    sort(&x[0],&x[n]);
    if ((n%2)==0) return 0.5*(x[n/2-1]+x[n/2]);
    else return x[n/2];
}

double cal_sd(int n,double *x) {
    double sx,sx2,a=x[0];
    bool ident=true;
    int i=1;
    while ((ident) and (i<n)) {ident=(x[i]==a);i++;}
    if (ident) return 0.0;
    sx=0.0;sx2=0.0;
    for (int i=0;i<n;i++) {
        sx +=x[i];
        sx2 +=x[i]*x[i];
    }
    if (n<2) return 0.0;
    else return  sqrt((sx2-(sx/(double)n*sx))/(double)(n-1));
}

double lnormal_dens(double x, double m, double s) {
   return -0.5*((x-m)/s)*((x-m)/s)-log(s)-0.5*log(2*PI);
}

double do_del(double temp, double x) {  //calcule cum avec ccum=1.0-cum
    double xsq,del;
    xsq= floor(x*16.0)/16.0;
    del= (x-xsq)*(x+xsq);
    return exp(-0.5*xsq*xsq)*exp(-0.5*del)*temp;
}
    
double pnorm5(double x,double mu, double sigma){
    double a[5] = {2.2352520354606839287,161.02823106855587881,1067.6894854603709582,18154.981253343561249,0.065682337918207449113};
    double b[4] = {47.20258190468824187,976.09855173777669322,10260.932208618978205,45507.789335026729956};
    double c[9] = {0.39894151208813466764,8.8831497943883759412,93.506656132177855979,597.27027639480026226,2494.5375852903726711,6848.1904505362823326,11602.651437647350124,9842.7148383839780218,1.0765576773720192317e-8};
    double d[8] = {22.266688044328115691,235.38790178262499861,1519.377599407554805,6485.558298266760755,18615.571640885098091,34900.952721145977266,38912.003286093271411,19685.429676859990727};
    double p[6] = {0.21589853405795699,0.1274011611602473639,0.022235277870649807,0.001421619193227893466,2.9112874951168792e-5,0.02307344176494017303};
    double q[5] = {1.28426009614491121,0.468238212480865118,0.0659881378689285515,0.00378239633202758244,7.29751555083966205e-5};
    double DBL_EPSILON=2.2204460492503131E-16;
    double sqrt2PI=2.506628274631000502415765284811;
    double pp,xden,xnum,temp,eps,xsq,y,cum,ccum;
    int i;
    
    if (sigma<=0.0) {if (x<mu) return 0.0; else return 1.0;}
    pp = (x-mu)/sigma;
    if (pp<0) y=-pp; else y=pp;
    eps = 0.5*DBL_EPSILON;
    if (y<=0.67448975) {
        if (y>eps) {
            xsq=pp*pp;
            xnum=a[4]*xsq;
            xden=xsq;
            for (int i=0;i<3;i++) {xnum=(xnum + a[i])*xsq;xden=(xden + b[i])*xsq;}
        } else {xnum=0.0;xden=0.0;}
        temp=pp*(xnum + a[3])/(xden + b[3]);
        cum=0.5+temp;
    } else {
        if (y<=sqrt(32.0)) {
            xnum=c[8]*y;
            xden=y;
            for (int i=0;i<7;i++) {xnum=(xnum+c[i])*y;xden=(xden+d[i])*y;}
            temp=(xnum+c[7])/(xden+d[7]);
            cum=do_del(temp,y);
            if (pp>0.0) cum=1.0-cum;
        } else {
            xsq=1.0/(pp*pp);
            xnum=p[5]*xsq;
            xden=xsq;
            for (int i=0;i<4;i++) {xnum=(xnum+p[i])*xsq;xden=(xden+q[i])*xsq;}
            temp=xsq*(xnum+p[4])/(xden+q[4]);
            temp=(1.0/sqrt2PI-temp)/y;
            cum=do_del(temp,pp);
            if (pp>0.0) cum=1.0-cum;
        }
    }
    return cum;
}

struct rescov {
    int n;
    double *mu,**cov;
};

rescov covarianceponderee(int nl, int nc, double **A, double *w) {
    rescov mcv;
    mcv.n=nc;
    mcv.mu = new double[nc];
    mcv.cov = new double*[nc];for (int i=0;i<nc;i++) mcv.cov[i] = new double[nc];
    double *wn,sw2,sw;
    //sx = new double[nc];
    wn = new double[nl];
    sw=0.0;for (int k=0;k<nl;k++) sw +=w[k];
    for (int k=0;k<nl;k++) wn[k] =w[k]/sw;
    sw2=0.0;for (int k=0;k<nl;k++) sw2 +=wn[k]*wn[k];
    for (int i=0;i<nc;i++) {
        mcv.mu[i]=0.0;for (int k=0;k<nl;k++) mcv.mu[i] +=wn[k]*A[k][i];
        //mu[i]=sx[i];
        for (int j=0;j<=i;j++){
            mcv.cov[i][j]=0.0;
            for (int k=0;k<nl;k++) mcv.cov[i][j] +=wn[k]*A[k][i]*A[k][j];
            mcv.cov[j][i]=mcv.cov[i][j];
        }
    }
    for (int i=0;i<nc;i++) {
        for (int j=0;j<nc;j++) mcv.cov[i][j] = (mcv.cov[i][j]-mcv.mu[i]*mcv.mu[j])/(1.0-sw2);
    }
    delete []wn;
    return mcv;
}

struct resAFD{
   int nlambda;      
   double proportion,*lambda,slambda,**vectprop,**princomp,*moy;
};

resAFD AFD(int nl, int nc, int *pop,double *omega, double **X, double prop) {
    resAFD res;
    rescov mcv;
    res.proportion = prop;
    vector <int> nk,numpop;   //npop est remplace par numpop.size() ou nk.size()
    bool trouve;
    double **matC,**matCT,**matTI,**matM,*valprop,**mk,anl,piv,sl,co,sy,sy2,somega,*w,*wk;
    int snk,j;
    //cout<<"debut AFD  nl="<<nl<<"\n";
    if (not numpop.empty()) numpop.clear();numpop.push_back(pop[0]);
    if (not nk.empty()) nk.clear();nk.push_back(1);
    for (int i=1;i<nl;i++) {
        trouve=false;
        for (j=0;j<numpop.size();j++) {
            trouve=(numpop[j]==pop[i]);
            if (trouve) break;
        }
        if (trouve) {nk[j]++;}
        else {numpop.push_back(pop[i]);nk.push_back(1);}
    }
    snk=0;for (int i=0;i<nk.size();i++) snk += nk[i];
    //cout<<"apres calcul snk\n";
    if (nl!=snk) {cout<< "dans AFD nl!=snk\n";exit(1);}
    matC = new double*[nc];for (int i=0;i<nc;i++) matC[i] = new double[numpop.size()];
    matCT = new double*[numpop.size()];for (int i=0;i<numpop.size();i++) matCT[i] = new double[nc];
    //matM = new *double[nc];for (int i=0;i<nc;i++) matM[i] = new double[nc];
    valprop = new double[nc];
    res.vectprop = new double*[nc];for (int i=0;i<nc;i++) res.vectprop[i] = new double[nc];
    //anl=1.0/(double)nl;
    somega=0.0;for (int i=0;i<nl;i++) somega +=omega[i];
    //cout<<"somega="<<somega<<"\n";
    w = new double[nl];for (int i=0;i<nl;i++) w[i]=omega[i]/somega;
    //cout<<"avant covarianceponderee\n";
    mcv=covarianceponderee(nl,nc,X,w);
    //cout<<"apres covarianceponderee\n";
    res.moy=new double[mcv.n];for (j=0;j<mcv.n;j++) res.moy[j]=mcv.mu[j];
    mk = new double*[numpop.size()];for (int i=0;i<numpop.size();i++) mk[i] = new double[nc];
    wk = new double[numpop.size()];
    //cout<<"numpop.size="<<numpop.size()<<"\n";
    for (int k=0;k<numpop.size();k++) {
        wk[k]=0.0;for (int i=0;i<nl;i++) {if (pop[i]==numpop[k]) {wk[k] +=w[i];}}
        //cout<<"wk["<<k<<"]="<<wk[k]<<"\n";
        for (j=0;j<nc;j++) {
            mk[k][j]=0.0;
            for (int i=0;i<nl;i++) if (pop[i]==numpop[k]) mk[k][j] += w[i]*X[i][j]/wk[k];
        }
    }
    //cout<<"apres la premiere boucle\n";
    for (int k=0;k<numpop.size();k++) {
        //cout<<"k="<<k<<"\n";
        //cout<<"   w[k]="<<wk[k]<<"\n";
        co=sqrt(wk[k]);
        for (j=0;j<nc;j++) {
            matC[j][k] = co*(mk[k][j]-res.moy[j]);
            matCT[k][j] = matC[j][k];
            //cout<<"k="<<k<<"   j="<<j<<"   co="<<co<<"   mk[k][j]="<<mk[k][j]<<"   res.moy[j]="<<res.moy[j]<<"\n";
        }
    }
    //cout<<"avant invM\n";
    matTI = invM(mcv.n,mcv.cov);
    matM = prodM(numpop.size(),nc,numpop.size(),prodM(numpop.size(),nc,nc,matCT,matTI),matC);
    //cout<<"avant jacobi\n";
    jacobi(numpop.size(),matM,valprop,res.vectprop);
    res.vectprop = prodM(nc,numpop.size(),numpop.size(),prodM(nc,nc,numpop.size(),matTI,matC),res.vectprop);
//tri des valeurs et vecteurs propres par ordre décroissant des valeurs propres
    for (int i=0;i<numpop.size()-1;i++) {
        for (j=i+1;j<numpop.size();j++) if (valprop[i]<valprop[j]) {
            piv=valprop[i];valprop[i]=valprop[j];valprop[j]=piv;
            for (int k=0;k<nc;k++) {
                piv=res.vectprop[k][i];res.vectprop[k][i]=res.vectprop[k][j];res.vectprop[k][j]=piv;
            }
        }
    }
    res.slambda=0.0;for (int i=0;i<numpop.size();i++) res.slambda +=valprop[i];
    res.nlambda=1;sl=valprop[0];
    while ((res.nlambda<numpop.size())and(sl/res.slambda<prop)) {
        res.slambda +=valprop[res.nlambda];res.nlambda++;
    }
    res.lambda = new double[res.nlambda];
    for (int i=0;i<res.nlambda;i++) res.lambda[i] = valprop[i];
    res.princomp = new double*[nl];for (int i=0;i<nl;i++) res.princomp[i] = new double[res.nlambda];
    for (int i=0;i<nl;i++) {
        for (j=0;j<res.nlambda;j++) {
            res.princomp[i][j]=0.0;
            for (int k=0;k<nc;k++) res.princomp[i][j] += (X[i][k]-res.moy[k])*res.vectprop[k][j];
        }
    }
    for (j=0;j<mcv.n;j++) delete []mcv.cov[j];delete []mcv.cov;
    delete []mcv.mu;
    for (int i=0;i<nc;i++) delete []matC[i];delete []matC;
    for (int i=0;i<numpop.size();i++) delete []matCT[i];delete []matCT;
    for (int i=0;i<nc;i++) delete []matTI[i];delete []matTI;
    for (int i=0;i<numpop.size();i++) delete []matM[i];delete []matM;
    for (int i=0;i<numpop.size();i++) delete []mk[i];delete []mk;
    delete []valprop;
    delete []w;
    delete []wk;
    return res;
}