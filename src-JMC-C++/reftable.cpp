
#ifndef FSTREAM
#include <fstream>
#define FSTREAM
#endif
#ifndef IOSTREAM
#include <iostream>
#define IOSTREAM
#endif
#ifndef STRING
#include <string>
#define STRING
#endif
#ifndef CSTRING
#include <cstring>
#define CSTRING
#endif


class ReftableC 
{
public:
    int nrec,*nrecscen,nscen;
    long posnrec;
    char *datapath, *filename, *filelog, *pch;
    int *nparam,nstat,po;
    float *param,*sumstat;
    fstream fifo;
    
    int readheader(char * fname,char *flogname, char* datafilename) {
        int nb;
        //cout <<"debut de readheader\n";
        fstream f0(fname,ios::in|ios::out|ios::binary);
        //cout<<"apres fstream\n";
        this->filename = new char[ strlen(fname)+1];
        this->filelog  = new char[ strlen(flogname)+1];
        strcpy(this->filename,fname);
        strcpy(this->filelog,flogname);
        //int p=strcspn(this->filelog,".");
        //this->filelog[p]='\0';
        //strcat(this->filelog,".log");
        
        if (!f0) {return 1;}  //fichier non ouvrable e.g. inexistant
        else {
            //cout<<"fichier OK\n";
            f0.seekg(0);
            f0.read((char*)&(this->nrec),sizeof(int));
            f0.read((char*)&(this->nscen),sizeof(int));
            cout <<"readheader.readheader    nscen = "<<this->nscen<<"\n";
            this->nrecscen = new int[this->nscen];
            for (int i=0;i<this->nscen;i++) {f0.read((char*)&(this->nrecscen[i]),sizeof(int));/*cout<<"nrecscen["<<i<<"] = "<<this->nrecscen[i]<<"\n";*/}
            this->nparam = new int[nscen];
            for (int i=0;i<this->nscen;i++) {f0.read((char*)&(this->nparam[i]),sizeof(int));cout<<"nparam["<<i<<"] = "<<this->nparam[i]<<"\n";}
            f0.read((char*)&(this->nstat),sizeof(int));//cout<<"nstat = "<<this->nstat<<"\n";
            f0.close();
            return 0;  //retour normal
        }
    }

    int writeheader() {
        int nb;
        ofstream f1(this->filename,ios::out|ios::binary);
        if (!f1.is_open()) {return 1;}  //fichier impossible à ouvrir
        nb=this->nrec;f1.write((char*)&nb,sizeof(int));
        //cout <<"reftable.writeheader     nscen = "<<this->nscen<<"\n";
        nb=this->nscen;f1.write((char*)&nb,sizeof(int));
        for (int i=0;i<this->nscen;i++) {nb=this->nrecscen[i];f1.write((char*)&nb,sizeof(int));}
        for (int i=0;i<this->nscen;i++) {nb=this->nparam[i];f1.write((char*)&nb,sizeof(int));}
        nb=this->nstat;f1.write((char*)&nb,sizeof(int));
        f1.close();
        return 0;  //retour normal
    }
    
    void readrecord(enregC *enr) {
        this->fifo.read((char*)&(enr->numscen),sizeof(int));
        for (int i=0;i<this->nparam[enr->numscen-1];i++) this->fifo.read((char*)&(enr->param[i]),sizeof(float));
        for (int i=0;i<this->nstat;i++) this->fifo.read((char*)&(enr->stat[i]),sizeof(float));
    }
    
    int writerecords(int nenr, enregC *enr) {
        int bidon;
        for (int i=0;i<nenr;i++){ 
             if (enr[i].message != "OK") {
                 ofstream f1(this->filelog,ios::out);
                 f1<<enr[i].message<<"\n";
                 f1.close();
                 return 1;
             }
        }
        //cout<<"avant ouverture de reftable.log nscen="<< this->nscen <<"\n";fflush(stdin);
        ofstream f1(this->filelog,ios::out);
        f1<<"OK\n";
        int *nrs,nb;
        nrs = new int[this->nscen];
        for (int i=0;i<this->nscen;i++) nrs[i]=0;
        this->fifo.seekp(0,ios::end);
        //cout<<"apres le seekp\n";fflush(stdin);
        for (int i=0;i<nenr;i++) {
            this->fifo.write((char*)&(enr[i].numscen),sizeof(int));
            nrs[enr[i].numscen-1]++;
            for (int j=0;j<this->nparam[enr[i].numscen-1];j++) this->fifo.write((char*)&(enr[i].param[j]),sizeof(float));
            for (int j=0;j<this->nstat;j++) this->fifo.write((char*)&(enr[i].stat[j]),sizeof(float));
        }
        //cout<<"AVANT MODIF      des nombres d'enregistrements\n";fflush(stdin);
        this->fifo.seekp(0,ios::beg);
        this->fifo.read((char*)&(this->nrec),sizeof(int));
        //cout <<"reftable.writerecords     avant lecture nscen = "<<this->nscen<<"\n";
        this->fifo.read((char*)&(bidon),sizeof(int));
        //cout <<"reftable.writerecords     apres lecture nscen = "<<bidon<<"\n";
        for (int i=0;i<this->nscen;i++) this->fifo.read((char*)&(this->nrecscen[i]),sizeof(int));
        this->fifo.seekp(0,ios::beg);
        //cout<< "position avant écriture du nouveau nrec : "<<this->fifo.tellg()<<"\n";
        nb =this->nrec+nenr;this->fifo.write((char*)&nb,sizeof(int));
        //cout<< "position avant écriture de this->nscen: "<<this->fifo.tellg()<<"\n";
        nb=this->nscen;this->fifo.write((char*)&nb,sizeof(int));
        //cout<< "position avant écriture des this->nrecscen: "<<this->fifo.tellg()<<"\n";
        
        for (int i=0;i<this->nscen;i++) {nb=this->nrecscen[i]+nrs[i];this->fifo.write((char*)&nb,sizeof(int));}
        fifo.flush();
        f1<<this->nrec+nenr<<"\n";
        f1.close();
        delete []nrs;
        return 0;
    }
  
    int openfile() {
        this->fifo.open(this->filename,ios::in|ios::out|ios::binary);
        return 0;
    }
    
    int openfile2() {
        this->fifo.open(this->filename,ios::in|ios::binary);
        this->fifo.seekg(0);
        this->fifo.read((char*)&(this->nrec),sizeof(int));
        this->fifo.read((char*)&(this->nscen),sizeof(int));
        //cout <<"nrec = "<<nrec<<"\n";
        this->nrecscen = new int[this->nscen];
        for (int i=0;i<this->nscen;i++) {this->fifo.read((char*)&(this->nrecscen[i]),sizeof(int));/*cout<<"nrecscen["<<i<<"] = "<<this->nrecscen[i]<<"\n";*/}
        this->nparam = new int[nscen];
        for (int i=0;i<this->nscen;i++) {this->fifo.read((char*)&(this->nparam[i]),sizeof(int));/*cout<<"nparam["<<i<<"] = "<<this->nparam[i]<<"\n";*/}
        this->fifo.read((char*)&(this->nstat),sizeof(int));//cout<<"nstat = "<<this->nstat<<"\n";
        return 0;
    }
    int closefile() {
        this->fifo.close();
        return 0;
    }
  
    void concat() {
        cout<<"debut de concat\n";
        enregC *enr;
        char *reftabname,*reftab,*reflogname,*num,nenr,*buffer;
        char extbin[]=".bin\0";
        char extlog[]=".log\0";
        int i,n,nu=1,nrecc,nerct,*nrecscenc,size,ns;
        bool premier=true;
        this->openfile();
        reftabname = new char[ strlen(this->filename)+10];
        reflogname = new char[ strlen(this->filename)+10];
        reftab = new char[ strlen(this->filename)+1];
        strcpy(reftab,this->filename);
        i=strlen(this->filename);
        while (this->filename[i]!='.') i--;
        reftab[i]='\0';
        num = new char[9];
        n=sprintf (num, "_%d",nu++);
        strcpy(reftabname,reftab);
        strcat(reftabname,num);strcpy(reflogname,reftabname);
        strcat(reftabname,extbin);strcat(reflogname,extlog);
        //cout<<reftabname<<"\n";
        //cout<<reflogname<<"\n";
        fstream f0(reftabname,ios::in|ios::binary);
        if(f0) {f0.close();i=rename(reftabname,this->filename);}
        else {cout <<" no file to concatenate\n";exit(1);}
        i=rename(reflogname,this->filelog);
        cout <<reftabname<<"\n";
        cout<<reflogname<<"\n";
        this->openfile2();this->closefile();
        this->openfile();
        for (nu=2;nu>1;nu++) {
            n=sprintf (num, "_%d",nu);
            strcpy(reftabname,reftab);
            strcat(reftabname,num);strcpy(reflogname,reftabname);
            strcat(reftabname,extbin);strcat(reflogname,extlog);
            fstream f0(reftabname,ios::in|ios::binary);
            if(f0) {
                cout<<reftabname<<"\n";
                cout <<reftabname<<"\n";
                f0.seekp(0,ios::end);size=f0.tellp();
                f0.seekp(0,ios::beg);
                //cout<<"position ="<<f0.tellp()<<"   size = "<<size<<"\n";
                buffer = new char[size];
                f0.read((char*)&(nrecc),sizeof(int));this->nrec +=nrecc;size -=4;
                //cout<<"nrecc="<<nrecc<<"\n";
                f0.read((char*)&(ns),sizeof(int));size -=4;
                //cout<<"nscen="<<ns<<"\n";
                if (premier) nrecscenc = new int[ns];
                for (int i=0;i<ns;i++) {f0.read((char*)&(nrecscenc[i]),sizeof(int));size -=4;this->nrecscen[i] +=nrecscenc[i];/*cout<<"scenario "<<i<<"   "<<this->nrecscen[i]<<"\n";*/}
                for (int i=0;i<ns;i++) {f0.read((char*)&(nrecscenc[i]),sizeof(int));size -=4;/*cout<<"scenario "<<i<<"   nparam = "<<nrecscenc[i]<<"\n";*/}
                f0.read((char*)&ns,sizeof(int));size -=4;/*cout<<"nstat = "<<ns<<"\n";*/
                f0.read(buffer,size);
                //cout<<"size final ="<<size<<"\n";
                f0.close();
                remove(reftabname);remove(reflogname);
                this->fifo.seekp(0,ios::end);this->fifo.write(buffer,size);
                this->fifo.seekp(0,ios::beg);
                this->fifo.write((char*)&(this->nrec),sizeof(int));
                this->fifo.seekp(8,ios::beg);
                for (int i=0;i<this->nscen;i++) this->fifo.write((char*)&(this->nrecscen[i]),sizeof(int));
                ofstream f1(this->filelog,ios::out);
                f1<<"OK\n";
                f1<<this->nrec<<"\n";
                f1.close();
            } else break;
        }
        this->fifo.close();
        
    }
  
};
