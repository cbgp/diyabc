#include <iostream>
#include <omp.h>
#include <string.h>

using namespace std;

int strpos(char* st1,char* st2) {
    int i,p=0,n1,n2;
    char *s;
    n1 = strlen(st1);
    n2 = strlen(st2);
    cout<<"n1="<<n1<<"   n2="<<n2<<"\n";
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



main() {

int nthreads,tid,npart=100,k;
float x;

char filename[]="~/workspace/diyabc/src-JMC-C++/apis/reftable.bin";
char extension[]="reftable.bin";
char datafile[]="SBCN.txt";
/*#pragma omp parallel private(tid)
   {
       tid=omp_get_thread_num();
       cout <<"Hello word from thread "<<tid<<"\n";
       if (tid==0)  cout<<"number of threads = "<<omp_get_num_threads()<<"\n";
   
   }*/
//debut du pragma
/*#pragma omp parallel for  private(tid,x) schedule(static)
                for (int ipart=0;ipart<npart;ipart++){
                        tid = omp_get_thread_num();
                        for(int i=0;i<10000;i++){
                            for (int j=0;j<10000;j++) x=((double)j+1.0+(double(tid)))/((double)i+2.0);
                        }
                        cout <<tid <<"   x="<<x<<  "\n";
                }
//fin du pragma*/


   k=strpos(filename,extension);
   cout << "k="<<k<<"\n";
   filename[k]='\0';
   //cout<<datafile<<"\n";
   strcat(filename,datafile);
   cout <<filename<<"\n";
}