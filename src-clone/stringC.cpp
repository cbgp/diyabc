/*
 * stringC.cpp
 *
 * Created on : 17 january 2010
 *     Author : cornuet
 *
 */

#include <string>
#include <stdlib.h>

class stringC : public string {

	int getwordint(stringC s,int num){
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
		if ((s.find("(")==0)or(s.find("[")==0)) s=s.substr(1,s.length()-1);
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
		if ((s.find("(")==0)or(s.find("[")==0)) s=s.substr(1,s.length()-1);
		return atof(s.c_str());
	}

	string getwordstring(string s,int num){
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
		return s;
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
					cout <<" j=1  k="<<*k<<"\n";
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
		cout <<"k="<<*k<<"\n";
		return sb;
	}

};

int main(int argc, char *argv[]) {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	stringC s,*sb;
	s="   13    parameters  and (75) summary statistics (0.33333333)";
	cout << getwordint(s,4)<<"\n";
	cout << getwordint(s,1)<<"\n";
	cout <<getwordfloat(s,7)<<"\n";
	int a;
	sb=splitwords(s," ",&a);
	cout <<"a="<<a<<"\n";
	for (int i=0;i<a;i++) cout <<sb[i]<<"\n";
	return 0;
}
