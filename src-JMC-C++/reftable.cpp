
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
#ifndef ALGORITHM
#include <algorithm>
#define ALGORITHM
#endif
#ifndef MESUTILS
#include "mesutils.cpp"
#define MESUTILS
#endif

using namespace std;


extern int nrecneeded;
extern double remtime,debutr;
extern char *progressfilename;

struct enregC {
    int numscen;
    float *param,*stat;
    long double dist;
    string message;
};
/**
* définit l'opérateur de comparaison de deux enregistrements de type enregC
* pour l'utilisation de la fonction sort du module algorithm
*/
struct compenreg
{
   bool operator() (const enregC & lhs, const enregC & rhs) const
   {
      return lhs.dist < rhs.dist;
   }
};


class ReftableC
{
public:
    int nrec,*nrecscen,nscen,nreclus,nrec0;
    long posnrec;
    char *datapath, *filename, *filelog, *filename0;
    int *nparam,nstat,po,nparamax,nscenchoisi,*scenchoisi,scenteste,nparamut,*nhistparam;
    float *param,*sumstat;
    HistParameterC **histparam;
    MutParameterC  *mutparam;
    fstream fifo;
    int nstatOK,nsel,nenr;
    enregC* enrsel;
    float *stat_obs;
    long double *var_stat;

    void sethistparamname(HeaderC header) {
      //cout<<"debut de sethistparamname\n";
        int nparamvar=0,pp;
        this->nparamut = header.nparamut;
        this->nhistparam = new int[header.nscenarios];
        this->histparam = new HistParameterC*[header.nscenarios];
        this->mutparam = new MutParameterC[header.nparamut];
        //cout<<"avant la boucle des scenarios\n";
        for (int i=0;i<header.nscenarios;i++) {
            nparamvar=0;
            for (int p=0;p<header.scenario[i].nparam;p++) if (not header.scenario[i].histparam[p].prior.constant) nparamvar++;
            //cout<<"scenario "<<i<<"   header.scenario[i].nparam="<<header.scenario[i].nparam <<"  nparamvar="<<nparamvar<<"\n";
            this->histparam[i] = new HistParameterC[nparamvar];
            this->nhistparam[i] =nparamvar;
            pp=-1;
            for (int p=0;p<header.scenario[i].nparam;p++) if (not header.scenario[i].histparam[p].prior.constant) {
                pp++;//cout<<"pp="<<pp;
                this->histparam[i][pp].name = header.scenario[i].histparam[p].name;
                this->histparam[i][pp].prior = copyprior( header.scenario[i].histparam[p].prior);
                this->histparam[i][pp].category = header.scenario[i].histparam[p].category;
                //cout<<"  OK\n";
            }
            //cout<<"coucou this->nparam[i] = "<<this->nparam[i]<<"   nparamvar="<<nparamvar<<"   header.nparamut="<<header.nparamut<<"\n";
            if (this->nparam[i]!=nparamvar+header.nparamut) {
                cout<<"PROBLEME scenario "<<i<<"  nparam="<<this->nparam[i]<<"  nparamvar="<<nparamvar<<"   nmutparam="<<nparamut<<"\n";
                exit(1);
            }
            //cout<<"couicou2\n";
            for (int p=0;p<this->nparamut;p++) {
                this->mutparam[p].name = header.mutparam[p].name;
                this->mutparam[p].prior = copyprior(header.mutparam[p].prior);
            }
        }
        //cout<<"fin de sethisparamname\n";
    }

    int readheader(char * fname,char *flogname, char* datafilename) {
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
        //cout<<"dans readheader this->filelog = '"<<this->filelog<<"\n";exit(1);
        if (!f0) {return 1;}  //fichier non ouvrable e.g. inexistant
        else {
            //cout<<"fichier OK\n";
            f0.seekg(0);
            f0.read((char*)&(this->nrec),sizeof(int));
            f0.read((char*)&(this->nscen),sizeof(int));
            //cout <<"readheader.readheader    nscen = "<<this->nscen<<"   nrec="<<this->nrec<<"\n";
            this->nrecscen = new int[this->nscen];
            for (int i=0;i<this->nscen;i++) {f0.read((char*)&(this->nrecscen[i]),sizeof(int));/*cout<<"nrecscen["<<i<<"] = "<<this->nrecscen[i]<<"\n";*/}
            this->nparam = new int[nscen];
            for (int i=0;i<this->nscen;i++) {f0.read((char*)&(this->nparam[i]),sizeof(int));/*cout<<"nparam["<<i<<"] = "<<this->nparam[i]<<"\n";*/}
            f0.read((char*)&(this->nstat),sizeof(int));//cout<<"nstat = "<<this->nstat<<"\n";
            f0.close();
            this->nrec0=this->nrec;
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
        for (int i=0;i<this->nscen;i++) {nb=this->nparam[i];f1.write((char*)&nb,sizeof(int));cout<<"nparam="<<this->nparam[i]<<"\n";}
        nb=this->nstat;f1.write((char*)&nb,sizeof(int));
        f1.close();
        return 0;  //retour normal
    }

    int readrecord(enregC *enr) {
        int bidon=0;
        //cout<<"debut de readrecord\n";
		//try {
			this->fifo.read((char*)&(enr->numscen),sizeof(int));
			if (enr->numscen>this->nscen) {
			FILE *flog;
            flog=fopen(progressfilename,"w");fprintf(flog,"%s","The reftable.bin file is corrupted. Clone the project and simulate a new file.\n");fclose(flog);
			cout<<"\nThe reftable.bin file is corrupted.\n";
			exit(1);
			}
			//throw(1);
			//cout<<"numscen = "<<enr->numscen<<"\n";
			for (int i=0;i<this->nparam[enr->numscen-1];i++) {this->fifo.read((char*)&(enr->param[i]),sizeof(float));/*cout<<enr->param[i]<<"  ";*/}
			//throw(2);
			//cout <<"\n";
			for (int i=0;i<this->nstat;i++) {this->fifo.read((char*)&(enr->stat[i]),sizeof(float));/*cout<<enr->stat[i]<<"  ";if ((i%10)==9) cout<<"\n";*/}
			//throw(3);
		/*}
		catch(int e) {
			cout<<"reftable.bin file is corrupted\n";
		    if (e==1) cout<<"Impossible to read the scenario number\n";
		    if (e==2) cout<<"Impossible to read one parameter value\n";
		    if (e==3) cout<<"Impossible to read one summary statistics value\n";
			bidon=e;
		}*/
		
        //cout <<"\n";
        //cout<<"fin de readrecord\n";
        //cin>>bidon;
        return bidon;
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
		//cout<<"avant le seekp\n";fflush(stdin);
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
        remtime=walltime(&debutr)*(nrecneeded-this->nrec-nenr)/(this->nrec+nenr-this->nrec0);
        f1<<this->nrec+nenr<<"\n";
        f1<<TimeToStr(remtime)<<"\n";
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
    
    int testfile(char* reftablefilename, int npart) {
		bool corrompu=false;
		cout<<"\nverification de l'integrite de la table de reference \nfichier"<<reftablefilename<<"\n";
		char str[10000];
        this->fifo.open(reftablefilename,ios::in|ios::out|ios::binary);
        this->fifo.seekg(0);
        this->fifo.read((char*)&(this->nrec),sizeof(int));
        this->fifo.read((char*)&(this->nscen),sizeof(int));
        cout <<"nrec = "<<nrec<<"\n";
        this->nrecscen = new int[this->nscen];
        for (int i=0;i<this->nscen;i++) {this->fifo.read((char*)&(this->nrecscen[i]),sizeof(int));/*cout<<"nrecscen["<<i<<"] = "<<this->nrecscen[i]<<"\n";*/}
        this->nparam = new int[this->nscen];
        for (int i=0;i<this->nscen;i++) {this->fifo.read((char*)&(this->nparam[i]),sizeof(int));/*cout<<"nparam["<<i<<"] = "<<this->nparam[i]<<"\n";*/}
        this->fifo.read((char*)&(this->nstat),sizeof(int));//cout<<"nstat = "<<this->nstat<<"\n";

		for (int i=0;i<this->nscen;i++) this->nrecscen[i]=0;
		int numscen,k,npmax;
		float x;
		cout<<this->nscen<<" scenario(s)\n";
		for(k=0;k<this->nrec;k++) {
			if ((k%npart)==0) cout<<k<<"\r";
			this->fifo.read((char*)&(numscen),sizeof(int));
			if (numscen<=this->nscen) {
				this->nrecscen[numscen-1]++;
				for (int i=0;i<this->nparam[numscen-1];i++) this->fifo.read((char*)&x,sizeof(float));
				for (int i=0;i<this->nstat;i++) this->fifo.read((char*)&x,sizeof(float));
			} 
			else {
				corrompu=true;
				break;
			}
		}
		cout<<"\n";
		if (not corrompu) {cout<<"fichier reftable OK\n\n";return 0;}
		enregC e;
		npmax=0;for (int i=0;i<this->nscen;i++) if(npmax<this->nparam[i]) npmax=this->nparam[i];
		e.param=new float[npmax];
		e.stat =new float[this->nstat];
		int nb;
		if (k>0) this->nrec=npart*(k/npart);
		cout<<"\n\nfichier corrompu à partir de l'enregistrement "<<k<<"\n";
		cout<<"on va récupérer les "<<this->nrec<<" premiers enregistrements\n";
        this->fifo.seekg(0);
        this->fifo.read((char*)&(k),sizeof(int));
        this->fifo.read((char*)&(this->nscen),sizeof(int));
        for (int i=0;i<this->nscen;i++) {this->fifo.read((char*)&(this->nrecscen[i]),sizeof(int));/*cout<<"nrecscen["<<i<<"] = "<<this->nrecscen[i]<<"\n";*/}
        this->nparam = new int[this->nscen];
        for (int i=0;i<this->nscen;i++) {this->fifo.read((char*)&(this->nparam[i]),sizeof(int));/*cout<<"nparam["<<i<<"] = "<<this->nparam[i]<<"\n";*/}
        this->fifo.read((char*)&(this->nstat),sizeof(int));//cout<<"nstat = "<<this->nstat<<"\n";
		this->filename0=strdup(reftablefilename);
		cout<<this->filename0<<"\n";
		this->filename0[strlen(reftablefilename)-1]='s';
		cout<<this->filename0<<"\n";
		
		ofstream f1(this->filename0,ios::out|ios::binary);
        if (!f1.is_open()) {cout<<"impossible d'ouvrir le fichier\n";exit(1);}  //fichier impossible à ouvrir
        nb=this->nrec;f1.write((char*)&nb,sizeof(int));
        //cout <<"reftable.writeheader     nscen = "<<this->nscen<<"\n";
        nb=this->nscen;f1.write((char*)&nb,sizeof(int));
        for (int i=0;i<this->nscen;i++) {nb=this->nrecscen[i];f1.write((char*)&nb,sizeof(int));}
        for (int i=0;i<this->nscen;i++) {nb=this->nparam[i];f1.write((char*)&nb,sizeof(int));}
        nb=this->nstat;f1.write((char*)&nb,sizeof(int));
		cout<<"fin d'ecriture de l'entete   nrec="<<this->nrec<<"\n";
		for (int h=0;h<this->nrec;h++) {
			this->fifo.read((char*)&(numscen),sizeof(int));
			for (int i=0;i<this->nparam[numscen-1];i++) {this->fifo.read((char*)&(e.param[i]),sizeof(float));}
			for (int i=0;i<this->nstat;i++) {this->fifo.read((char*)&(e.stat[i]),sizeof(float));}
			f1.write((char*)&(numscen),sizeof(int));
            for (int i=0;i<this->nparam[numscen-1];i++) f1.write((char*)&(e.param[i]),sizeof(float));
            for (int i=0;i<this->nstat;i++) f1.write((char*)&(e.stat[i]),sizeof(float));
			if ((((h+1)%npart)==0)or(h==this->nrec-1)) cout<<h+1<<"\r";
		}
		f1.close();
		this->fifo.close();
		remove(reftablefilename);
		rename(this->filename0,reftablefilename);
		cout<<"\nfin d'ecriture des enregistrements\n";
		return 1;
	}
    
    
    int closefile() {
        this->fifo.close();
        return 0;
    }

    void concat() {
        cout<<"debut de concat\n";
        char *reftabname,*reftab,*reflogname,*num,*buffer;
        char extbin[]=".bin\0";
        char extlog[]=".log\0";
        int i,n,nu=1,nrecc,*nrecscenc,size,ns;
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
        strcat(reftabname,num);
        cout<<reftabname<<"\n";

        strcpy(reflogname,reftabname);
        strcat(reftabname,extbin);strcat(reflogname,extlog);
        cout<<reftabname<<"\n";
        //cout<<reflogname<<"\n";
        fstream f0(reftabname,ios::in|ios::binary);
        if(f0) {f0.close();i=rename(reftabname,this->filename);}
        else {cout <<" no file to concatenate\n";exit(1);}
        i=rename(reflogname,this->filelog);
        cout <<reftabname<<"\n";
        cout<<reflogname<<"\n";
        cout<<this->filename<<"\n";
        this->openfile2();this->closefile();
        this->openfile();
        for (nu=2;nu<50000;nu++) {
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
            }
        }
        this->fifo.close();
    }

/**
* calcule les variances des statistiques résumées
* sur les 100000 premiers enregistrements de la table de référence
*/
    int cal_varstat() {
        int nrecutil,iscen,nsOK,bidon,i,step;
        long double *sx,*sx2,x,an,nr,*min,*max;
        bool scenOK;
        //cout <<"debut de cal_varstat\n";
        enregC enr;
        nrecutil=100000;if (nrecutil>this->nrec) nrecutil=this->nrec;
        nr=0;for (int i=0;i<nscenchoisi;i++) nr+=nrecscen[this->scenchoisi[i]-1];
        if (nrecutil>nr) nrecutil=nr;
        sx  = new long double[this->nstat];
        sx2 = new long double[this->nstat];
        min = new long double[this->nstat];
        max = new long double[this->nstat];
        var_stat = new long double[this->nstat];
        for (int j=0;j<this->nstat;j++) {sx[j]=0.0;sx2[j]=0.0;min[j]=10.0;max[j]=-10.0;}
        enr.stat = new float[this->nstat];
        this->nparamax = 0;for (int i=0;i<this->nscen;i++) if (this->nparam[i]>this->nparamax) this->nparamax=this->nparam[i];
        enr.param = new float[this->nparamax];
        this->openfile2();
        i=0;step=nrecutil/100;
        while (i<nrecutil) {
            bidon=this->readrecord(&enr);
            //cout<<"coucou\n";
            scenOK=false;iscen=0;
            while((not scenOK)and(iscen<this->nscenchoisi)) {
                scenOK=(enr.numscen==this->scenchoisi[iscen]);
                iscen++;
            }
            if (scenOK) {
                i++;
                for (int j=0;j<this->nstat;j++) {
                    x = (long double)enr.stat[j];
                    sx[j] += x;
                    sx2[j] += x*x;
					if (min[j]>x) min[j]=x;
					if (max[j]<x) max[j]=x;
                }
            }
            //if ((i % step)==0) {cout<<"\rcal_varstat : "<<i/step<<"%";fflush(stdout);}
        }
        //    cout<<i<<"   "<<nrecutil<<"\n";
        if (i<nrecutil) nrecutil=i;
        //    cout<<i<<"   "<<nrecutil<<"\n";
        this->closefile();
        nsOK=0;
        an=1.0*(long double)nrecutil;
        for (int j=0;j<this->nstat;j++) {
            this->var_stat[j]=(sx2[j] -sx[j]*sx[j]/an)/(an-1.0);
            if (this->var_stat[j]>0) nsOK++;
			printf("var_stat[%3d] = %12.8Lf   min=%12.8Lf   max=%12.8Lf\n",j,this->var_stat[j],min[j],max[j]);
        }
        delete []sx;delete []sx2;
        cout<<"\nnstatOK = "<<nsOK<<"\n";
        return nsOK;
    }

/**
* alloue la mémoire pour enrsel
*/
    void alloue_enrsel(int nsel) {
        int nparamax=0;
        for (int i=0;i<this->nscen;i++) if (this->nparam[i]>nparamax) nparamax=this->nparam[i];
        this->enrsel = new enregC[2*nsel];
        for (int i=0;i<2*nsel;i++) {
           this->enrsel[i].param = new float[nparamax];
           this->enrsel[i].stat  = new float[this->nstat];
        }
    }

/**
* desalloue la mémoire de enrsel
*/
    void desalloue_enrsel(int nsel) {
        for (int i=0;i<2*nsel;i++) {
          delete []this->enrsel[i].param;
          delete []this->enrsel[i].stat;
        }
        delete []this->enrsel;
    }

/**
* calcule la distance de chaque jeu de données simulé au jeu observé
* et sélectionne les nsel enregistrements les plus proches (copiés dans enregC *enrsel)
*/
    void cal_dist(int nrec, int nsel, float *stat_obs) {
        int nn,nparamax,nrecOK=0,iscen,bidon,step;
        bool firstloop=true,scenOK;
        long double diff;
        this->nreclus=0;step=nrec/100;
        nn=nsel;
        nparamax = 0;for (int i=0;i<this->nscen;i++) if (this->nparam[i]>nparamax) nparamax=this->nparam[i];
        //cout<<"cal_dist nsel="<<nsel<<"   nparamax="<<nparamax<<"   nrec="<<nrec<<"   nreclus="<<this->nreclus<<"   nstat="<<this->nstat<<"   2*nn="<<2*nn<<"\n";
        //cout<<" apres allocation de enrsel\n";
        this->openfile2();
        while (this->nreclus<nrec) {
            if (firstloop) {nrecOK=0;firstloop=false;}
            else nrecOK=nn;
            while ((nrecOK<2*nn)and(this->nreclus<nrec)) {
                do {bidon=this->readrecord(&(this->enrsel[nrecOK]));} while (bidon!=0);
                scenOK=false;iscen=0;
                while((not scenOK)and(iscen<this->nscenchoisi)) {
                    scenOK=(this->enrsel[nrecOK].numscen==this->scenchoisi[iscen]);
                    iscen++;
                }
                if (scenOK) {
                   this->nreclus++;
                   this->enrsel[nrecOK].dist=0.0;
                    for (int j=0;j<this->nstat;j++) if (this->var_stat[j]>0.0) {
                        diff =(long double)(this->enrsel[nrecOK].stat[j] - stat_obs[j]);
                      this->enrsel[nrecOK].dist += diff*diff/this->var_stat[j];
					  //if (nreclus==1) printf("  %12.6f   %12.6f   %12.6Lf   %12.8Lf\n",this->enrsel[nrecOK].stat[j],stat_obs[j],diff*diff,this->enrsel[nrecOK].dist);
                    }
                    this->enrsel[nrecOK].dist =sqrt(this->enrsel[nrecOK].dist);
                    nrecOK++;
                if (this->nreclus==nrec) break;
                }
                if ((this->nreclus % step)==0) {cout<<"\rcal_dist : "<<this->nreclus/step<<"%";fflush(stdout);}
            }
            sort(&this->enrsel[0],&this->enrsel[2*nn],compenreg());
        }
        cout<<"\rcal_dist : fini   nreclus="<<nreclus<<"\n";
        this->closefile();
        cout<<"\nnrec_lus = "<<this->nreclus<<"   nrecOK = "<<nrecOK<<"\n";
        cout<<"    distmin = "<<this->enrsel[0].dist<<"    distmax = "<<this->enrsel[nsel-1].dist<<"\n";
        printf("    distmin = %12.8Lf   distmax = %12.8Lf\n",this->enrsel[0].dist/(long double)this->nstat,this->enrsel[nsel-1].dist/(long double)this->nstat);
		/*for (int i=0;i<nsel;i++){
			cout<<this->enrsel[i].numscen<<"  ";
			for (int j=0;j<this->nparam[this->enrsel[i].numscen-1];j++) printf("  %6.0f",this->enrsel[i].param[j]);
			for (int j=0;j<this->nstat;j++) printf("  %8.5f",this->enrsel[i].stat[j]);
			cout<<"\n";
		}*/
        //exit(1);
    }

};



