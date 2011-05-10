/*



*/
#ifndef PARTICULEC
#include "particuleC.cpp"
#define PARTICULEC
#endif

#ifndef DATA
#include "data.cpp"
#define DATA
#endif

class MutParameterC
{
public:
    string name;
    int groupe;
    int category;   //0 pour mutmoy, 1 pour Pmoy, 2 pour snimoy, 3 pour musmoy, 4 pour k1moy et 5 pour k2moy
    double value;
    PriorC prior;
        
    void ecris() {
        cout<<"    groupe="<<this->groupe<<"   category="<<this->category<<"\n";
        //prior.ecris();
    }
};


class HeaderC
{
public:
    string message,datafilename,entete;
    char * pathbase;
    DataC dataobs;
    int nparamtot,nstat,nscenarios,nconditions,ngroupes,nparamut;
    string *paramname, *statname;
    ScenarioC *scenario,scen;
    HistParameterC *histparam;
    ConditionC *condition;
    LocusGroupC *groupe;
    bool drawuntil;
    ParticleC particuleobs;
    MutParameterC *mutparam;
    double *stat_obs;
        
    void libere() {
        this->dataobs.libere();
        //delete []this->pathbase;
        //delete []this->paramname;
        if(this->nconditions>0) delete []this->condition;
        for(int i=0;i<this->nscenarios;i++) this->scenario[i].libere();
        delete []this->scenario;
        this->scen.libere();
        //for(int i=1;i<ngroupes+1;i++) this->groupe[i].libere();
        delete []this->groupe;
    }

    PriorC readprior(string ss) {
        PriorC prior;
        string s1,*sb;
        int j;
        s1 = ss.substr(3,ss.length()-4);
        sb = splitwords(s1,",",&j);
        prior.mini=atof(sb[0].c_str());
        prior.maxi=atof(sb[1].c_str());
                prior.ndec=ndecimales(prior.mini,prior.maxi);
                //cout << "ndec="<<prior.ndec<<"   (mini="<<prior.mini<<"   maxi="<<prior.maxi<<"\n";
        if (ss.find("UN[")!=string::npos) {prior.loi="UN";}
        else if (ss.find("LU[")!=string::npos) {prior.loi="LU";}
        else if (ss.find("NO[")!=string::npos) {prior.loi="NO";prior.mean=atof(sb[2].c_str());prior.sdshape=atof(sb[3].c_str());}
        else if (ss.find("LN[")!=string::npos) {prior.loi="LN";prior.mean=atof(sb[2].c_str());prior.sdshape=atof(sb[3].c_str());}
        else if (ss.find("GA[")!=string::npos) {prior.loi="GA";prior.mean=atof(sb[2].c_str());prior.sdshape=atof(sb[3].c_str());}
        if (prior.maxi==0.0) prior.constant=true;
        else prior.constant = ((prior.maxi-prior.mini)/prior.maxi<0.000001);
        //cout<<ss<<"   ";
        //if (prior.constant) cout<<"constant\n"; else cout<<"variable\n";
        delete []sb;
        return prior;
    }

    PriorC readpriormut(string ss) {
        PriorC prior;
        string s1,*sb;
        int j;
        s1 = ss.substr(3,ss.length()-4);
        sb = splitwords(s1,",",&j);
        prior.mini=atof(sb[0].c_str());
        prior.maxi=atof(sb[1].c_str());
        prior.ndec=ndecimales(prior.mini,prior.maxi);
        if (ss.find("UN[")!=string::npos) {prior.loi="UN";}
        else if (ss.find("LU[")!=string::npos) {prior.loi="LU";}
        else if (ss.find("GA[")!=string::npos) {prior.loi="GA";prior.mean=atof(sb[2].c_str());prior.sdshape=atof(sb[3].c_str());}
                if (prior.maxi==0.0) prior.constant=true;
                else if ((prior.maxi-prior.mini)/prior.maxi<0.000001) prior.constant=true;
                else prior.constant=false;
        //cout<<ss<<"   ";
        //if (prior.constant) cout<<"constant\n"; else cout<<"variable\n";
        delete []sb;
        return prior;
    }


    ConditionC readcondition(string ss){
        ConditionC cond;
        //cout<<"condition : "<<ss<<"\n";
        if (ss.find(">=")!=string::npos){
            cond.operateur=">=";cond.param1=ss.substr(0,ss.find(">="));cond.param2=ss.substr(ss.find(">=")+2,ss.length()-(ss.find(">=")+2));}
        else if (ss.find("<=")!=string::npos){
            cond.operateur="<=";cond.param1=ss.substr(0,ss.find("<="));cond.param2=ss.substr(ss.find("<=")+2,ss.length()-(ss.find("<=")+2));}
        if (ss.find(">")!=string::npos){
            cond.operateur=">";cond.param1=ss.substr(0,ss.find(">"));cond.param2=ss.substr(ss.find(">")+1,ss.length()-(ss.find(">")+1));}
        else if (ss.find("<")!=string::npos){
            cond.operateur="<";cond.param1=ss.substr(0,ss.find("<"));cond.param2=ss.substr(ss.find("<")+1,ss.length()-(ss.find("<")+1));}
            return cond;
    }

    void assignloc(int gr){
        this->groupe[gr].nloc = 0;
        for (int loc=0;loc<dataobs.nloc;loc++) {
            if (dataobs.locus[loc].groupe==gr) this->groupe[gr].nloc++;
        }
        this->groupe[gr].loc = new int[this->groupe[gr].nloc];
        int iloc=-1;
        for (int i=0;i<dataobs.nloc;i++) {
            if (dataobs.locus[i].groupe==gr) {iloc++;this->groupe[gr].loc[iloc] = i;}
        }
    }

        /*ScenarioC superscen() {
                int neventm,nn0m,nsampm,nparamm,nparamvarm,nconditionm;
                neventm=0;    for (int i=0;i<this->nscenarios;i++) {if (neventm<this->scenario[i].nevent)          neventm=this->scenario[i].nevent;}
                nn0m=0;       for (int i=0;i<this->nscenarios;i++) {if (nn0m<this->scenario[i].nn0)                nn0m=this->scenario[i].nn0;}
                nsampm=0;     for (int i=0;i<this->nscenarios;i++) {if (nsampm<this->scenario[i].nsamp)            nsampm=this->scenario[i].nsamp;}
                nparamm=0;    for (int i=0;i<this->nscenarios;i++) {if (nparamm<this->scenario[i].nparam)         nparamm=this->scenario[i].nparam;}
                nparamvarm=0; for (int i=0;i<this->nscenarios;i++) {if (nparamvarm<this->scenario[i].nparamvar)    nparamvarm=this->scenario[i].nparamvar;}
                nconditionm=0;for (int i=0;i<this->nscenarios;i++) {if (nconditionm<this->scenario[i].nconditions) nconditionm=this->scenario[i].nconditions;}
                cout<<"neventm="<<neventm<<"   nn0m="<<nn0m<<"   nsampm="<<nsampm<<"   nparamm="<<nparamm<<"nparamvarm="<<nparamvarm<<"   ncondm="<<nconditionm<<"\n";
                this->scen.event = new EventC[neventm];
                this->scen.ne0   = new Ne0C[nn0m];
                
                for (int i=0;i<nn0m;i++) this->scen.ne0[i].name="blabla";
                this->scen.time_sample = new int[nsampm];
                this->scen.histparam = new HistParameterC[nparamm];
                this->scen.paramvar = new double[nparamvarm+3];
                if (nconditionm>0) this->scen.condition = new ConditionC[nconditionm];
                
        }*/

    HeaderC* readHeader(char* headerfilename){
                char reftable[]="header.txt";
                char *path;
        string s1,s2,**sl,*ss,*ss1,*ss2;
        int *nlscen,nss,nss1,j,k,l,gr,grm,k1,k2;
        //cout<<"debut de readheader\n";
        //cout<<"readHeader headerfilename = "<<headerfilename<<"\n";
        ifstream file(headerfilename, ios::in);
        if (file == NULL) {
            this->message = "Header  File "+string(headerfilename)+" not found";
            cout<<this->message<<"\n";
            return this;
        } else this->message="";
        getline(file,this->datafilename);
                //cout<<this->datafilename<<"\n";fflush(stdin);
                path = new char[strlen(headerfilename)+this->datafilename.length()];
                strcpy(path,headerfilename);
                //cout<<path<<"\n";fflush(stdin);
                k = strpos(headerfilename,reftable);
                //cout<<"k="<<k<<"\n";fflush(stdin);
                path[k]='\0';
                this->pathbase=strdup(path);
                strcat(path,this->datafilename.c_str());
                //cout<<"Path = "<<path<<"\n";fflush(stdin);
                this->datafilename=string(path);
                //cout<<"Data = "<<this->datafilename<<"\n";fflush(stdin);
        this->dataobs.loadfromfile(path);
        getline(file,s1);
        this->nparamtot=getwordint(s1,1);
        this->nstat=getwordint(s1,4);
                //cout<<"avant scenarios\n";fflush(stdin);
//Partie Scenarios
        getline(file,s1);       //ligne vide
        getline(file,s1);       // nombre de scenarios
        this->nscenarios=getwordint(s1,1);
        sl = new string*[this->nscenarios];
        nlscen = new int[this->nscenarios];
        this->scenario = new ScenarioC[this->nscenarios];
        for (int i=0;i<this->nscenarios;i++) nlscen[i]=getwordint(s1,3+i);
        for (int i=0;i<this->nscenarios;i++) {
            sl[i] = new string[nlscen[i]];
            getline(file,s1);
            this->scenario[i].number = getwordint(s1,2);
            this->scenario[i].prior_proba = getwordfloat(s1,3);
            this->scenario[i].nparam = 0;
            this->scenario[i].nparamvar=0;
            for (int j=0;j<nlscen[i];j++) getline(file,sl[i][j]);
            this->scenario[i].read_events(nlscen[i],sl[i]);
        }
        for (int i=0;i<this->nscenarios;i++) delete []sl[i];
        delete [] sl;
//Partie historical parameters
                //cout <<"avant histparam\n";fflush(stdin);
        getline(file,s1);       //ligne vide
        getline(file,s1);
        ss=splitwords(s1," ",&nss);
        s2=ss[3].substr(1,ss[3].length()-2);
        ss2=splitwords(s2,",",&nss);
        this->nparamtot = atoi(ss2[0].c_str());
        this->nconditions = atoi(ss2[1].c_str());
        delete [] ss2;
        this->histparam = new HistParameterC[this->nparamtot];
        if (this->nconditions>0) this->condition = new ConditionC[this->nconditions];
        delete [] ss;
        for (int i=0;i<this->nparamtot;i++) {
            getline(file,s1);
            //cout<<s1<<"\n";
            ss=splitwords(s1," ",&nss);
            this->histparam[i].name=ss[0];
            //cout<<this->histparam[i].name<<"\n";
            if (ss[1]=="N") this->histparam[i].category = 0;
            else if  (ss[1]=="T") this->histparam[i].category = 1;
            else if  (ss[1]=="A") this->histparam[i].category = 2;
            this->histparam[i].prior = this->readprior(ss[2]);
            //    cout<<"readHeader "<<this->histparam[i].name;
            //    if (this->histparam[i].prior.constant) cout<<"   constant\n"; else cout<<"   variable\n";
        }
        delete [] ss;
        if (this->nconditions>0) {
            this->condition = new ConditionC[this->nconditions];
            for (int i=0;i<this->nconditions;i++) {
                getline(file,s1);
                this->condition[i] = this->readcondition(s1);
            }
            getline(file,s1);
            if (s1 != "DRAW UNTIL") this->drawuntil=false; else  this->drawuntil=true;
        }
//retour sur les parametres spécifiques à chaque scenario;
                //cout <<"avant retour sur histparam/scenario\n";fflush(stdin);
        for (int i=0;i<this->nscenarios;i++) {
            this->scenario[i].nparamvar=0;
            for (int j=0;j<this->scenario[i].nparam;j++) {
                int k=0;
                while (this->scenario[i].histparam[j].name!=this->histparam[k].name) {k++;}
                this->scenario[i].histparam[j].prior = copyprior(this->histparam[k].prior);
                if (not this->histparam[k].prior.constant) {
                    //this->scenario[i].paramvar[this->scenario[i].nparamvar].prior = copyprior(this->histparam[k].prior);
                    this->scenario[i].nparamvar++;
                }
                //this->scenario[i].histparam[j].ecris();
            }
            //cout<<"scenario "<<i<<"   "<<this->scenario[i].nparam<<" param et "<<this->scenario[i].nparamvar<<" paramvar\n "<<flush;
        }
//retour sur les conditions spécifiques à chaque scenario
                //cout <<"avant retour sur conditions\n";fflush(stdin);
                if (this->nconditions>0) {
                    for (int i=0;i<this->nscenarios;i++) {
                        int nc=0;
                        for (j=0;j<this->nconditions;j++) {
                            int np=0;
                            for (k=0;k<this->scenario[i].nparam;k++) {
                                if (this->condition[j].param1 == this->scenario[i].histparam[k].name) np++;
                                if (this->condition[j].param2 == this->scenario[i].histparam[k].name) np++;
                            }
                            if (np==2) nc++;
                        }
                        this->scenario[i].nconditions=nc;
                        this->scenario[i].condition = new ConditionC[nc];
                        nc=0;
                        while (nc<this->scenario[i].nconditions) {
                            for (j=0;j<this->nconditions;j++) {
                                int np=0;
                                for (k=0;k<this->scenario[i].nparam;k++) {
                                    if (this->condition[j].param1 == this->scenario[i].histparam[k].name) np++;
                                    if (this->condition[j].param2 == this->scenario[i].histparam[k].name) np++;
                                }
                                if (np==2) {
                                    this->scenario[i].condition[nc]=copycondition(this->condition[j]);
                                    //this->scenario[i].condition[nc].ecris();
                                    nc++;   
                                }                   
                            }
                        }
                        //cout <<"dans readheader  \n";
                        
                  }
                } else for (int i=0;i<this->nscenarios;i++) this->scenario[i].nconditions=0;
//Partie loci description
                //cout <<"avant partie loci\n";fflush(stdin);
        getline(file,s1);       //ligne vide
        getline(file,s1);       //ligne "loci description"
        grm=1;
        for (int loc=0;loc<this->dataobs.nloc;loc++){
            getline(file,s1);
            ss=splitwords(s1," ",&nss);
            k=0;while (ss[k].find("[")==string::npos) k++;
            if (ss[k]=="[M]") {
                s1=ss[k+1].substr(1,ss[k+1].length());gr=atoi(s1.c_str());this->dataobs.locus[loc].groupe=gr;if (gr>grm) grm=gr;
                this->dataobs.locus[loc].motif_size=atoi(ss[k+2].c_str());this->dataobs.locus[loc].motif_range=atoi(ss[k+3].c_str());
            }
            else if (ss[k]=="[S]") {
                s1=ss[k+1].substr(1,ss[k+1].length());gr=atoi(s1.c_str());this->dataobs.locus[loc].groupe=gr;if (gr>grm) grm=gr;
                this->dataobs.locus[loc].mutsit = new double[this->dataobs.locus[loc].dnalength];
                //cout<<this->dataobs.locus[loc].dnalength<<"\n";  
                //this->dataobs.locus[loc].dnalength=atoi(ss[k+2].c_str());  //inutile variable déjà renseignée
            }
        }
        delete [] ss;
//Partie group priors
                //cout <<"avant partie group priors\n";fflush(stdin);
        getline(file,s1);       //ligne vide
        getline(file,s1);       //ligne "group prior"
                this->ngroupes=getwordint(s1,3);
        //cout<<"header.ngroupes="<<this->ngroupes;        
        this->groupe = new LocusGroupC[this->ngroupes+1];
        this->assignloc(0);
        //cout<<"on attaque les groupes : analyse des priors nombre de groupes="<<this->ngroupes <<"\n";
        for (gr=1;gr<=this->ngroupes;gr++){
            getline(file,s1);
            ss=splitwords(s1," ",&nss);
            this->assignloc(gr);
            if (ss[2]=="[M]") {
                this->groupe[gr].type=0;
                getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormutmoy = this->readpriormut(ss1[1]);delete [] ss1;
                if (this->groupe[gr].priormutmoy.constant) this->groupe[gr].mutmoy=this->groupe[gr].priormutmoy.mini; 
                else {this->groupe[gr].mutmoy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}
                getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormutloc = this->readpriormut(ss1[1]);delete [] ss1;
                getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorPmoy   = this->readpriormut(ss1[1]);delete [] ss1;
                if (this->groupe[gr].priorPmoy.constant) this->groupe[gr].Pmoy=this->groupe[gr].priorPmoy.mini; 
                else {this->groupe[gr].Pmoy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}
                
                getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorPloc   = this->readpriormut(ss1[1]);delete [] ss1;
                //cout<<"Ploc    ";this->groupe[gr].priorPloc.ecris();
                
                getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorsnimoy = this->readpriormut(ss1[1]);delete [] ss1;
                if (this->groupe[gr].priorsnimoy.constant) this->groupe[gr].snimoy=this->groupe[gr].priorsnimoy.mini; 
                else {this->groupe[gr].snimoy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}
                
                getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priorsniloc = this->readpriormut(ss1[1]);delete [] ss1;
                //cout<<"sniloc  ";this->groupe[gr].priorsniloc.ecris();
                
            } else if (ss[2]=="[S]") {
                this->groupe[gr].type=1;
                getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormusmoy = this->readpriormut(ss1[1]);delete [] ss1;
                if (this->groupe[gr].priormusmoy.constant) this->groupe[gr].musmoy=this->groupe[gr].priormusmoy.mini; 
                else {this->groupe[gr].musmoy=-1.0;for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}}
                
                getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priormusloc = this->readpriormut(ss1[1]);delete [] ss1;
                
                getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork1moy  = this->readpriormut(ss1[1]);delete [] ss1;
                if (this->groupe[gr].priork1moy.constant) this->groupe[gr].k1moy=this->groupe[gr].priork1moy.mini; 
                else this->groupe[gr].k1moy=-1.0;
                
                getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork1loc  = this->readpriormut(ss1[1]);delete [] ss1;
                
                getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork2moy  = this->readpriormut(ss1[1]);delete [] ss1;
                if (this->groupe[gr].priork2moy.constant) this->groupe[gr].k2moy=this->groupe[gr].priork2moy.mini; 
                else this->groupe[gr].k2moy=-1.0;
                
                getline(file,s1);ss1=splitwords(s1," ",&nss1);this->groupe[gr].priork2loc  = this->readpriormut(ss1[1]);delete [] ss1;
                
                getline(file,s1);ss1=splitwords(s1," ",&nss1);
                this->groupe[gr].p_fixe=atof(ss1[2].c_str());this->groupe[gr].gams=atof(ss1[3].c_str());
                if (ss1[1]=="JK") this->groupe[gr].mutmod=0;
                else if (ss1[1]=="K2P") this->groupe[gr].mutmod=1;
                else if (ss1[1]=="HKY") this->groupe[gr].mutmod=2;
                else if (ss1[1]=="TN") this->groupe[gr].mutmod=3;
                //cout<<"mutmod = "<<this->groupe[gr].mutmod <<"\n";
                if (this->groupe[gr].mutmod>0) {
                    if (not this->groupe[gr].priork1moy.constant)for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}
                }
                if (this->groupe[gr].mutmod==3) {
                    if (not this->groupe[gr].priork2moy.constant)for (int i=0;i<this->nscenarios;i++) {this->scenario[i].nparamvar++;}
                }
            }
        }
//Mise à jour des locus séquences
        MwcGen mwc;
        mwc.randinit(999,time(NULL));
        int nsv;
        bool nouveau;
        for (int loc=0;loc<this->dataobs.nloc;loc++){
            gr=this->dataobs.locus[loc].groupe;
            if ((this->dataobs.locus[loc].type>4)and(gr>0)){
                nsv = floor(this->dataobs.locus[loc].dnalength*(1.0-0.01*this->groupe[gr].p_fixe)+0.5);
                for (int i=0;i<this->dataobs.locus[loc].dnalength;i++) this->dataobs.locus[loc].mutsit[i] = mwc.ggamma3(1.0,this->groupe[gr].gams);
                int *sitefix;
                sitefix=new int[this->dataobs.locus[loc].dnalength-nsv];
                for (int i=0;i<this->dataobs.locus[loc].dnalength-nsv;i++) {
                    if (i==0) sitefix[i]=mwc.rand0(this->dataobs.locus[loc].dnalength);
                    else {
                        do {
                            sitefix[i]=mwc.rand0(this->dataobs.locus[loc].dnalength);
                            nouveau=true;j=0;
                            while((nouveau)and(j<i)) {nouveau=(sitefix[i]!=sitefix[j]);j++;}
                        } while (not nouveau);
                    }
                    this->dataobs.locus[loc].mutsit[i] = 0.0;
                }
                delete [] sitefix;
                double s=0.0;
                for (int i=0;i<this->dataobs.locus[loc].dnalength;i++) s += this->dataobs.locus[loc].mutsit[i];
                for (int i=0;i<this->dataobs.locus[loc].dnalength;i++) this->dataobs.locus[loc].mutsit[i] /=s;
            }    
}
        //cout<<"avant la mise à jour des paramvar\n";fflush(stdin);
        delete [] ss;
//Mise à jour des paramvar
        for (int i=0;i<this->nscenarios;i++) {
            this->scenario[i].paramvar = new double[this->scenario[i].nparamvar];
            for (int j=0;j<this->scenario[i].nparamvar;j++)this->scenario[i].paramvar[j]=-1.0;
            //this->scenario[i].ecris();
        }
        
                //cout<<"avant superscen\n";
                this->scen.nevent=0;    
                for (int i=0;i<this->nscenarios;i++) {if (this->scen.nevent<this->scenario[i].nevent) this->scen.nevent=this->scenario[i].nevent;}
                this->scen.nn0=0;       
                for (int i=0;i<this->nscenarios;i++) {if (this->scen.nn0<this->scenario[i].nn0)       this->scen.nn0 =this->scenario[i].nn0;}
                this->scen.nsamp=0;     
                for (int i=0;i<this->nscenarios;i++) {if (this->scen.nsamp<this->scenario[i].nsamp)   this->scen.nsamp=this->scenario[i].nsamp;}
                this->scen.nparam=0;    
                for (int i=0;i<this->nscenarios;i++) {if (this->scen.nparam<this->scenario[i].nparam)  this->scen.nparam=this->scenario[i].nparam;}
                this->scen.nparamvar=0; 
                for (int i=0;i<this->nscenarios;i++) {if (this->scen.nparamvar<this->scenario[i].nparamvar) this->scen.nparamvar=this->scenario[i].nparamvar;}
                this->scen.nconditions=0;
                for (int i=0;i<this->nscenarios;i++) {if (this->scen.nconditions<this->scenario[i].nconditions) this->scen.nconditions=this->scenario[i].nconditions;}
                this->scen.event = new EventC[this->scen.nevent];
                int lonmax=0;
                for (int i=0;i<this->nscenarios;i++) {
                    for (int j=0;j<this->scenario[i].nevent;j++) {if (lonmax<this->scenario[i].event[j].ltime) lonmax=this->scenario[i].event[j].ltime;}
                }
                for (int i=0;i<this->scen.nevent;i++) {
                    this->scen.event[i].stime = new char[lonmax];
                    this->scen.event[i].ltime=0;
                    for (int j=0;j<lonmax;j++) this->scen.event[i].stime[j]='\0';
                }
                lonmax=0;
                for (int i=0;i<this->nscenarios;i++) {
                    for (int j=0;j<this->scenario[i].nevent;j++) {if (lonmax<this->scenario[i].event[j].lNe) lonmax=this->scenario[i].event[j].lNe;}
                }
                for (int i=0;i<this->scen.nevent;i++) {
                    this->scen.event[i].sNe = new char[lonmax];
                    this->scen.event[i].lNe=0;
                    for (int j=0;j<lonmax;j++) this->scen.event[i].sNe[j]='\0';
                }
                 lonmax=0;
                for (int i=0;i<this->nscenarios;i++) {
                    for (int j=0;j<this->scenario[i].nevent;j++) {if (lonmax<this->scenario[i].event[j].ladmixrate) lonmax=this->scenario[i].event[j].ladmixrate;}
                }
                for (int i=0;i<this->scen.nevent;i++) {
                  this->scen.event[i].sadmixrate = new char[lonmax];
                  this->scen.event[i].ladmixrate=0;
                  for (int j=0;j<lonmax;j++) this->scen.event[i].sadmixrate[j]='\0';
               }
               
                this->scen.ne0   = new Ne0C[this->scen.nn0];
                lonmax=0;
                for (int i=0;i<this->nscenarios;i++) {
                    for (int j=0;j<this->scenario[i].nn0;j++) {if (lonmax<this->scenario[i].ne0[j].lon) lonmax=this->scenario[i].ne0[j].lon;};
                }
                for (int i=0;i<this->scen.nn0;i++) {
                    this->scen.ne0[i].lon=lonmax;
                    this->scen.ne0[i].name = new char[lonmax];
                }
                this->scen.time_sample = new int[this->scen.nsamp];
                this->scen.histparam = new HistParameterC[this->scen.nparam];
                this->scen.paramvar = new double[this->scen.nparamvar+3];
                if (this->scen.nconditions>0) this->scen.condition = new ConditionC[this->scen.nconditions];
                //this->scen.ecris();
                //cout<<"apres superscen\n";
                //exit(1);
//Partie group statistics
                //cout<<"debut des group stat\n";
                this->nstat=0;
        getline(file,s1);       //ligne vide
        getline(file,s1);       //ligne "group group statistics"
        //cout <<"s1="<<s1<<"\n";
        for (gr=1;gr<=this->ngroupes;gr++) {
            getline(file,s1);
            ss=splitwords(s1," ",&nss);
                        //cout <<"s1="<<s1<<"   ss[3]="<< ss[3] <<"   atoi = "<< atoi(ss[3].c_str()) <<"\n";
            this->groupe[gr].nstat = getwordint(ss[3],0);
                        //cout <<"s1="<<s1<<"   ss[3]="<< ss[3] <<"   atoi = "<< this->groupe[gr].nstat <<"\n";
                        this->nstat +=this->groupe[gr].nstat;
            this->groupe[gr].sumstat = new StatC[this->groupe[gr].nstat];
            delete [] ss;
            k=0;
                        //cout<<"nstat="<<this->groupe[gr].nstat<<"\n";
            while (k<this->groupe[gr].nstat) {
                          //cout <<"stat "<<k<<"    groupe "<<gr<<"\n";
                getline(file,s1);
                                //cout <<"s1="<<s1<<"\n";
                ss=splitwords(s1," ",&nss);
                                //cout << ss[0]<<"\n";
                j=0;while (ss[0]!=stat_type[j]) j++;
                                //cout<<"j="<<j<<"\n";
                if (this->groupe[gr].type==0) {   //MICROSAT
                    if (stat_num[j]<5) {
                        for (int i=1;i<nss;i++) {
                              this->groupe[gr].sumstat[k].cat=stat_num[j];
                              this->groupe[gr].sumstat[k].samp=atoi(ss[i].c_str());
                              k++;
                        }
                    } else if (stat_num[j]<12) {
                        for (int i=1;i<nss;i++) {
                            this->groupe[gr].sumstat[k].cat=stat_num[j];
                            ss1=splitwords(ss[i],"&",&nss1);
                            this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                            this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                            k++;
                            delete [] ss1;
                        }
                    } else if (stat_num[j]==12) {
                        for (int i=1;i<nss;i++) {
                            this->groupe[gr].sumstat[k].cat=stat_num[j];
                            ss1=splitwords(ss[i],"&",&nss1);
                            this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                            this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                            this->groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
                            k++;
                            delete [] ss1;
                        }
                    }
                } else if (this->groupe[gr].type==1) {   //DNA SEQUENCE
                    if (stat_num[j]>-5) {
                        for (int i=1;i<nss;i++) {
                               this->groupe[gr].sumstat[k].cat=stat_num[j];
                               this->groupe[gr].sumstat[k].samp=atoi(ss[i].c_str());
                               k++;
                        }
                    } else if (stat_num[j]>-14) {
                        for (int i=1;i<nss;i++) {
                            this->groupe[gr].sumstat[k].cat=stat_num[j];
                            ss1=splitwords(ss[i],"&",&nss1);
                            this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                            this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                            k++;
                            delete [] ss1;
                        }
                    } else if (stat_num[j]==-14) {
                        for (int i=1;i<nss;i++) {
                            this->groupe[gr].sumstat[k].cat=stat_num[j];
                            ss1=splitwords(ss[i],"&",&nss1);
                            this->groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                            this->groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                            this->groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
                            k++;
                            delete [] ss1;
                        }
                    }
                }
                    //cout<<"fin de la stat "<<k<<"\n";
                        }
            delete [] ss;
        }
        this->nparamut=0;
        for (gr=1;gr<=this->ngroupes;gr++) {
            if (this->groupe[gr].type==0) {
                if (not this->groupe[gr].priormutmoy.constant) this->nparamut++;
                if (not this->groupe[gr].priorPmoy.constant) this->nparamut++;
                if (not this->groupe[gr].priorsnimoy.constant) this->nparamut++;
            } else {
                if (not this->groupe[gr].priormusmoy.constant) this->nparamut++;
                //cout<<"calcul de nparamut\n";
                if (this->groupe[gr].mutmod>0) {if (not this->groupe[gr].priork1moy.constant) this->nparamut++;}
                if (this->groupe[gr].mutmod==3){if (not this->groupe[gr].priork2moy.constant) this->nparamut++;}
                //cout<<"fin du calcul de nparamut = "<<this->nparamut<<"\n";
            }  
        }
        this->mutparam = new MutParameterC[nparamut];
        this->nparamut=0;
        for (gr=1;gr<=this->ngroupes;gr++) {
            if (this->groupe[gr].type==0) {
                if (not this->groupe[gr].priormutmoy.constant) {
                    this->mutparam[nparamut].name="umic_"+IntToString(gr);
                    this->mutparam[nparamut].groupe=gr;
                    this->mutparam[nparamut].category=0;
                    this->mutparam[nparamut].prior = copyprior(this->groupe[gr].priormutmoy);
                    this->nparamut++;
                }
                if (not this->groupe[gr].priorPmoy.constant) {
                    this->mutparam[nparamut].name="Pmic_"+IntToString(gr);
                    this->mutparam[nparamut].groupe=gr;
                    this->mutparam[nparamut].category=1;
                    this->mutparam[nparamut].prior = copyprior(this->groupe[gr].priorPmoy);
                    this->nparamut++;
                }
                if (not this->groupe[gr].priorsnimoy.constant) {
                    this->mutparam[nparamut].name="snimic_"+IntToString(gr);
                    this->mutparam[nparamut].groupe=gr;
                    this->mutparam[nparamut].category=2;
                    this->mutparam[nparamut].prior = copyprior(this->groupe[gr].priorsnimoy);
                    this->nparamut++;
              }
            } else {
                if (not this->groupe[gr].priormusmoy.constant){
                    this->mutparam[nparamut].name="useq_"+IntToString(gr);
                    this->mutparam[nparamut].groupe=gr;
                    this->mutparam[nparamut].category=3;
                    this->mutparam[nparamut].prior = copyprior(this->groupe[gr].priormusmoy);
                    this->nparamut++;
               }
                if ((this->groupe[gr].mutmod>0)and(not this->groupe[gr].priork1moy.constant)) {
                    this->mutparam[nparamut].name="k1seq_"+IntToString(gr);
                    this->mutparam[nparamut].groupe=gr;
                    this->mutparam[nparamut].category=4;
                    this->mutparam[nparamut].prior = copyprior(this->groupe[gr].priork1moy);
                    this->nparamut++;
                }
                if ((this->groupe[gr].mutmod==3)and(not this->groupe[gr].priork2moy.constant)) {
                    this->mutparam[nparamut].name="k2seq_"+IntToString(gr);
                    this->mutparam[nparamut].groupe=gr;
                    this->mutparam[nparamut].category=5;
                    this->mutparam[nparamut].prior = copyprior(this->groupe[gr].priork2moy);
                    this->nparamut++;
               }
            }  
        }
        
  //Entete du fichier reftable
        getline(file,s1);       //ligne vide
        getline(file,this->entete);     //ligne entete
        this->statname =new string[this->nstat];
        ss=splitwords(entete," ",&nss);
        for (int i=nstat-1;i>=0;i--) this->statname[i]=ss[--nss];
        //for (int i=0;i<this->nstat;i++) cout<<this->statname[i]<<"   ";cout<<"\n";
        delete []ss;
                //cout<<"scenarios à la fin de readheader\n";
                //exit(1);
                //for (int i=0;i<this->nscenarios;i++) this->scenario[i].ecris();
                //this->scen.ecris();
                //cout<<"fin de readheader\n\n\n";
        return this;
    }
    
    void calstatobs(char* statobsfilename) {
//partie DATA
                //cout<<"debut de calstatobs\n";
                this->particuleobs.dnatrue = true;
                this->particuleobs.nsample = this->dataobs.nsample;
                //cout<<this->dataobs.nsample<<"\n";
                this->particuleobs.data.nsample = this->dataobs.nsample;
                this->particuleobs.data.nind = new int[this->dataobs.nsample];
                this->particuleobs.data.indivsexe = new int*[this->dataobs.nsample];
                for (int i=0;i<this->dataobs.nsample;i++) {
                        this->particuleobs.data.nind[i] = this->dataobs.nind[i];
                        this->particuleobs.data.indivsexe[i] = new int[this->dataobs.nind[i]];
                        for (int j=0;j<this->dataobs.nind[i];j++) this->particuleobs.data.indivsexe[i][j] = this->dataobs.indivsexe[i][j];
                }
                //cout<<"apres DATA\n";
//partie GROUPES
                int ngr = this->ngroupes;
                //cout<<"ngr="<<ngr<<"\n";
                this->particuleobs.ngr = ngr;
                this->particuleobs.grouplist = new LocusGroupC[ngr+1];
                this->particuleobs.grouplist[0].nloc = this->groupe[0].nloc;
                this->particuleobs.grouplist[0].loc  = new int[this->groupe[0].nloc];
                for (int i=0;i<this->groupe[0].nloc;i++) this->particuleobs.grouplist[0].loc[i] = this->groupe[0].loc[i];
                for (int gr=1;gr<=ngr;gr++) {
                        //cout <<"groupe "<<gr<<"\n";
                        this->particuleobs.grouplist[gr].type =this->groupe[gr].type;
                        this->particuleobs.grouplist[gr].nloc = this->groupe[gr].nloc;
                        this->particuleobs.grouplist[gr].loc  = new int[this->groupe[gr].nloc];
                        for (int i=0;i<this->groupe[gr].nloc;i++) this->particuleobs.grouplist[gr].loc[i] = this->groupe[gr].loc[i];
                        this->particuleobs.grouplist[gr].nstat=this->groupe[gr].nstat;
                        this->particuleobs.grouplist[gr].sumstat = new StatC[this->groupe[gr].nstat];
                        //cout<<"calstatobs nstat["<<gr<<"]="<<this->groupe[gr].nstat<<"\n";
                        for (int i=0;i<this->groupe[gr].nstat;i++){
                                this->particuleobs.grouplist[gr].sumstat[i].cat   = this->groupe[gr].sumstat[i].cat;
                                this->particuleobs.grouplist[gr].sumstat[i].samp  = this->groupe[gr].sumstat[i].samp;
                                this->particuleobs.grouplist[gr].sumstat[i].samp1 = this->groupe[gr].sumstat[i].samp1;
                                this->particuleobs.grouplist[gr].sumstat[i].samp2 = this->groupe[gr].sumstat[i].samp2;

                        }
                }
                //cout<<"apres GROUPS\n";
//partie LOCUSLIST
                int kmoy;
                this->particuleobs.nloc = this->dataobs.nloc;
                this->particuleobs.locuslist = new LocusC[this->dataobs.nloc];
                //cout<<"avant la boucle\n";
                for (int kloc=0;kloc<this->dataobs.nloc;kloc++){
                        this->particuleobs.locuslist[kloc].type = this->dataobs.locus[kloc].type;
                        this->particuleobs.locuslist[kloc].groupe = this->dataobs.locus[kloc].groupe;
                        this->particuleobs.locuslist[kloc].coeff =  this->dataobs.locus[kloc].coeff;
                        this->particuleobs.locuslist[kloc].name =  new char[strlen(this->dataobs.locus[kloc].name)+1];
                        strcpy(this->particuleobs.locuslist[kloc].name,this->dataobs.locus[kloc].name);
                        this->particuleobs.locuslist[kloc].ss = new int[ this->dataobs.nsample];
                        for (int sa=0;sa<this->particuleobs.nsample;sa++) this->particuleobs.locuslist[kloc].ss[sa] =  this->dataobs.locus[kloc].ss[sa];
                        this->particuleobs.locuslist[kloc].samplesize = new int[ this->dataobs.nsample];
                        
                        for (int sa=0;sa<this->particuleobs.nsample;sa++) this->particuleobs.locuslist[kloc].samplesize[sa] =  this->dataobs.locus[kloc].samplesize[sa];
                        //cout<<"locus "<<kloc<<"   groupe "<<this->particuleobs.locuslist[kloc].groupe<<"\n";
                        if (this->dataobs.locus[kloc].type < 5) {
                                kmoy=(this->dataobs.locus[kloc].maxi+this->dataobs.locus[kloc].mini)/2;
                                this->particuleobs.locuslist[kloc].kmin=kmoy-((this->dataobs.locus[kloc].motif_range/2)-1)*this->dataobs.locus[kloc].motif_size;
                                this->particuleobs.locuslist[kloc].kmax=this->particuleobs.locuslist[kloc].kmin+(this->dataobs.locus[kloc].motif_range-1)*this->dataobs.locus[kloc].motif_size;
                                //cout<<"kmin="<<this->particuleobs.locuslist[kloc].kmin<<"     kmax="<<this->particuleobs.locuslist[kloc].kmax<<"\n";
                                this->particuleobs.locuslist[kloc].motif_size = this->dataobs.locus[kloc].motif_size;
                                this->particuleobs.locuslist[kloc].motif_range = this->dataobs.locus[kloc].motif_range;
                                this->particuleobs.locuslist[kloc].haplomic = new int*[this->particuleobs.data.nsample];
                                for (int sa=0;sa<this->particuleobs.data.nsample;sa++){
                                      this->particuleobs.locuslist[kloc].haplomic[sa] = new int[this->particuleobs.locuslist[kloc].ss[sa]]; 
                                      for (int i=0;i<this->particuleobs.locuslist[kloc].ss[sa];i++)this->particuleobs.locuslist[kloc].haplomic[sa][i]=this->dataobs.locus[kloc].haplomic[sa][i];
                                }
                        }else {
                                this->particuleobs.locuslist[kloc].dnalength =  this->dataobs.locus[kloc].dnalength;
                                this->particuleobs.locuslist[kloc].pi_A = this->dataobs.locus[kloc].pi_A ;
                                this->particuleobs.locuslist[kloc].pi_C =  this->dataobs.locus[kloc].pi_C;
                                this->particuleobs.locuslist[kloc].pi_G =  this->dataobs.locus[kloc].pi_G;
                                this->particuleobs.locuslist[kloc].pi_T =  this->dataobs.locus[kloc].pi_T;
                                this->particuleobs.locuslist[kloc].haplodna = new char**[this->particuleobs.data.nsample];
                                for (int sa=0;sa<this->particuleobs.data.nsample;sa++){
                                      this->particuleobs.locuslist[kloc].haplodna[sa] = new char*[this->particuleobs.locuslist[kloc].ss[sa]];
                                      for (int i=0;i<this->particuleobs.locuslist[kloc].ss[sa];i++){
                                           this->particuleobs.locuslist[kloc].haplodna[sa][i] = new char[this->dataobs.locus[kloc].dnalength+1];
                                           for (int jj=0;jj<this->dataobs.locus[kloc].dnalength+1;jj++) this->particuleobs.locuslist[kloc].haplodna[sa][i][jj] = this->dataobs.locus[kloc].haplodna[sa][i][jj];
                                      }
                                 }
                        }
                      // cout << "samplesize[0]="<<this->particuleobs.locuslist[kloc].samplesize[0]<<"\n";
                }
               //cout<<"avant entete\n"; 
               string *sb,ent;
               int j;
               sb = splitwords(this->entete," ",&j);
               ent="";
               for (int k=j-this->nstat;k<j;k++) ent=ent+centre(sb[k],14);
               ent=ent+"\n";
               cout<<"entete : "<<entete<<"\n";
               delete []sb;
               FILE *fobs;
               fobs=fopen(statobsfilename,"w");
               fputs(ent.c_str(),fobs);
               for(int gr=1;gr<=this->particuleobs.ngr;gr++) {
                     cout<<"avant calcul des statobs du groupe "<<gr<<"\n";
                     this->particuleobs.docalstat(gr);
                     cout<<"apres calcul des statobs du groupe "<<gr<<"\n";
                     for (int j=0;j<this->particuleobs.grouplist[gr].nstat;j++) fprintf(fobs," %8.4f     ",this->particuleobs.grouplist[gr].sumstat[j].val);
               }
               fprintf(fobs,"\n");
               fclose(fobs);
               this->particuleobs.libere(true);
               //exit(1);
        }
        /** 
* lit le fichier des statistiques observées (placées dans double *stat_obs)
*/
    double* read_statobs(char *statobsfilename) {
       string entete,ligne,*ss;
       int ns;
       ifstream file(statobsfilename, ios::in);
       getline(file,entete);
       getline(file,ligne);
       file.close();
       ss=splitwords(ligne," ",&ns);
       //cout<<"statobs ns="<<ns<<"\n";
       if (ns!=this->nstat) exit(1);
       this->stat_obs = new double[ns];
       for (int i=0;i<ns;i++) this->stat_obs[i]=atof(ss[i].c_str());
       delete []ss;
       return this->stat_obs;
       //for (int i=0;i<ns;i++) cout<<stat_obs[i]<<"   ";
       //cout<<"\n";
    }


};
