#ifndef STRING
#include <string>
#define STRING
#endif
using namespace std;

#define PI 3.141592653589793

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
  while (abs(p-round(p))>1E-12) {k++;p=10*p;}
  return k;
}

string IntToString ( int number )
{
  std::ostringstream oss;

  // Works just like cout
  oss<< number;

  // Return the underlying string
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
