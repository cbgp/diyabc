/*
 * modchec.cpp
 *
 *  Created on: 4 april 2011
 *      Author: cornuet
 */
#ifndef HEADER
#include "header.cpp"
#define HEADER
#endif

#ifndef VECTOR
#include <vector>
#define VECTOR
#endif

#ifndef MATRICES
#include "matrices.cpp"
#define MATRICES
#endif

#ifndef MESUTILS
#include "mesutils.cpp"
#define MESUTILS
#endif


using namespace std;

    bool resetstats(string s) {
        int j,ns,nq,nss1,gr,k=0;
        string *ss,*qq,*ss1;
//comptage des stat        
        for (gr=1;gr<=header.ngroupes;gr++) {header.groupe[gr].nstat=0;} header.nstat=0;
        ss =splitwords(s,",",&ns);
        for (int i=0;i<ns;i++) { 
            qq=splitwords(ss[j],"_",&nq);
            if (nq!=3) return false;
            gr=atoi(qq[1].c_str());
            header.groupe[gr].nstat++;header.nstat++;
        }    
        for (gr=1;gr<=header.ngroupes;gr++) header.groupe[gr].sumstat = new StatC[header.groupe[gr].nstat];     
        ss =splitwords(s,",",&ns);
        for (int i=0;i<ns;i++) { 
            qq=splitwords(ss[j],"_",&nq);
            gr=atoi(qq[1].c_str());
            j=0;while (qq[0]!=stat_type[j]) j++;
            if (header.groupe[gr].type==0) {   //MICROSAT
                if (stat_num[j]<5) {
                      header.groupe[gr].sumstat[k].cat=stat_num[j];
                      header.groupe[gr].sumstat[k].samp=atoi(qq[2].c_str());
                      k++;
                } else if (stat_num[j]<12) {
                      header.groupe[gr].sumstat[k].cat=stat_num[j];
                      ss1=splitwords(qq[2],"&",&nss1);
                      header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                      header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                      k++;
                      delete [] ss1;
                } else if (stat_num[j]==12) {
                      header.groupe[gr].sumstat[k].cat=stat_num[j];
                      ss1=splitwords(qq[2],"&",&nss1);
                      header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                      header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                      header.groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
                      k++;
                      delete [] ss1;
                }
            } else if (header.groupe[gr].type==1) {   //DNA SEQUENCE
                if (stat_num[j]>-5) {
                      header.groupe[gr].sumstat[k].cat=stat_num[j];
                      header.groupe[gr].sumstat[k].samp=atoi(qq[2].c_str());
                      k++;
                } else if (stat_num[j]>-14) {
                        header.groupe[gr].sumstat[k].cat=stat_num[j];
                        ss1=splitwords(qq[2],"&",&nss1);
                        header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                        header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                        k++;
                        delete [] ss1;
                } else if (stat_num[j]==-14) {
                        header.groupe[gr].sumstat[k].cat=stat_num[j];
                        ss1=splitwords(qq[2],"&",&nss1);
                        header.groupe[gr].sumstat[k].samp=atoi(ss1[0].c_str());
                        header.groupe[gr].sumstat[k].samp1=atoi(ss1[1].c_str());
                        header.groupe[gr].sumstat[k].samp2=atoi(ss1[2].c_str());
                        k++;
                        delete [] ss1;
                }
            }            
        }
        return true;
    }



    void domodchec(char *options){
        char  *progressfilename;
        int nstatOK, iprog,nprog;
        int nrec,nsel,ns,ns1,nrecpos;
        string opt,*ss,s,*ss1,s0,s1;
        double  *stat_obs;
        bool usestats;
        
        FILE *flog;
        
        progressfilename = new char[strlen(path)+strlen(ident)+20];
        strcpy(progressfilename,path);
        strcat(progressfilename,ident);
        strcat(progressfilename,"_progress.txt");
        cout<<"debut domodchec  options : "<<options<<"\n";
        opt=char2string(options);
        ss = splitwords(opt,";",&ns);
        for (int i=0;i<ns;i++) { //cout<<ss[i]<<"\n";
            s0=ss[i].substr(0,2);
            s1=ss[i].substr(2);
            if (s0=="s:") {
                ss1 = splitwords(s1,",",&rt.nscenchoisi);
                rt.scenchoisi = new int[rt.nscenchoisi];
                for (int j=0;j<rt.nscenchoisi;j++) rt.scenchoisi[j] = atoi(ss1[j].c_str());
                nrecpos=0;for (int j=0;j<rt.nscenchoisi;j++) nrecpos +=rt.nrecscen[rt.scenchoisi[j]-1];
                cout <<"scenario choisi : "<<rt.scenchoisi[0]<<"\n";
            } else if (s0=="n:") {
                nrec=atoi(s1.c_str());
                if(nrec>nrecpos) nrec=nrecpos;
                cout<<"nombre total de jeux de données considérés (pour le scénario choisi )= "<<nrec<<"\n";
            } else if (s0=="m:") {
                nsel=atoi(s1.c_str());    
                cout<<"nombre de jeux de données considérés pour la régression locale = "<<nsel<<"\n";
            } else if (s0=="t:") {
                numtransf=atoi(s1.c_str()); 
                switch (numtransf) {
                  case 1 : cout <<" pas de transformation des paramètres\n";break;
                  case 2 : cout <<" transformation log des paramètres\n";break;
                  case 3 : cout <<" transformation logit des paramètres\n";break;
                  case 4 : cout <<" transformation log(tg) des paramètres\n";break;
                }
            } else if (s0=="v:") {
                cout<< "\n";
                usestats = resetstats(s1);
            }            
        }
        
        nstatOK = rt.cal_varstat();                       cout<<"apres cal_varstat\n";
        stat_obs = header.read_statobs(statobsfilename);  cout<<"apres read_statobs\n";
        nprog=100;iprog=1;
        flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        rt.alloue_enrsel(nsel);
        rt.cal_dist(nrec,nsel,stat_obs);                  cout<<"apres cal_dist\n";
        iprog+=8;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        det_numpar();                                     cout<<"apres det_numpar\n";
        nprog=nparamcom*10+14;
        recalparam(nsel);                                 cout<<"apres recalparam\n";
        rempli_mat(nsel,stat_obs);                        cout<<"apres rempli_mat\n";
        local_regression(nsel);               cout<<"apres local_regression\n";
        iprog+=1;flog=fopen(progressfilename,"w");fprintf(flog,"%d %d",iprog,nprog);fclose(flog);
        phistar = calphistar(nsel);                                 cout<<"apres calphistar\n";
        
    
    }