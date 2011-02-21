#include <fstream>
#include <cstring>
#include <iostream>
#include <string>


class ReftableC 
{
public:
    int nrec,*nrecscen,nscen;
    long posnrec;
    char *datapath, *filename;
    int *nparam,nstat;
    float *param,*sumstat;
    fstream fifo;
    
    int readheader(char * fname,int nscen,char* datafilename) {
        int nb;
        fstream f0(fname,ios::in|ios::out|ios::binary);
        strcpy(this->filename,fname);
        if (!f0) {return 1;}  //fichier non ouvrable e.g. inexistant
        else {
            strcpy(this->filename,fname);
            f0.seekg(0);
            f0.read((char*)&nb,sizeof(int));
            this->datapath = new char[nb];
            f0.read(this->datapath,nb);
            if (strcmp(this->datapath,datafilename) !=0) {return 2;} // fichier avec un nom du fichier data différent
            this->posnrec=f0.tellp();
            f0.read((char*)&(this->nrec),sizeof(int));
            this->nrecscen = new int[nscen];
            for (int i=0;i<nscen;i++) f0.read((char*)&(this->nrecscen[i]),sizeof(int));
            nparam = new int[nscen];
            for (int i=0;i<nscen;i++) f0.read((char*)&(this->nparam[i]),sizeof(int));
            f0.read((char*)&(this->nstat),sizeof(int));
            f0.close();
            return 0;  //retour normal
        }
    }

    int writeheader() {
        int nb;
        ofstream f1(this->filename,ios::out|ios::binary);
        if (!f1.is_open()) {return 1;}  //fichier impossible à ouvrir
        nb=strlen(this->datapath)+1;
        f1.write((char*)&nb,sizeof(int));
        f1.write((char*)&(this->datapath),nb);
        nb=this->nrec;f1.write((char*)&nb,sizeof(int));
        for (int i=0;i<nscen;i++) {nb=this->nrecscen[i];f1.write((char*)&nb,sizeof(int));}
        for (int i=0;i<nscen;i++) {nb=this->nparam[i];f1.write((char*)&nb,sizeof(int));}
        nb=this->nstat;f1.write((char*)&nb,sizeof(int));
        f1.close();
        return 0;  //retour normal
    }
    
    enregC readrecord() {
        enregC enr; 
        this->fifo.read((char*)&(enr.numscen),sizeof(int));
        for (int i=0;i<this->nparam[enr.numscen-1];i++) this->fifo.read((char*)&(enr.param[i]),sizeof(float));
        for (int i=0;i<this->nstat;i++) this->fifo.read((char*)&(enr.stat[i]),sizeof(float));
        return enr;
    }
    
    int writerecords(int nenr, enregC *enr) {
        int *nrs,nb;
        nrs = new int[this->nscen];
        for (int i=0;i<this->nscen;i++) nrs[i]=0;
        this->fifo.seekp(0,ios::end);
        for (int i=0;i<nenr;i++) {
            this->fifo.write((char*)&(enr[i].numscen),sizeof(int));
            nrs[enr[i].numscen-1]++;
            for (int j=0;j<this->nparam[enr[i].numscen-1];j++) this->fifo.write((char*)&(enr[i].param[j]),sizeof(float));
            for (int j=0;j<this->nstat;j++) this->fifo.write((char*)&(enr[i].stat[j]),sizeof(float));
        }
        this->fifo.seekp(this->posnrec);
        this->fifo.read((char*)&(this->nrec),sizeof(int));
        for (int i=0;i<nscen;i++) this->fifo.read((char*)&(this->nrecscen[i]),sizeof(int));
        this->fifo.seekp(this->posnrec);
        nb =this->nrec+nenr;this->fifo.write((char*)&nb,sizeof(int));
        for (int i=0;i<nscen;i++) {nb=this->nrecscen[i]+nrs[i];this->fifo.write((char*)&nb,sizeof(int));}
        fifo.flush();
    }
  
    int openfile() {
        this->fifo.open(this->filename,ios::in|ios::out|ios::binary);
        return 0;
    }
    
    int closefile() {
        this->fifo.close();
        return 0;
    }
  
};