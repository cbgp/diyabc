



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



