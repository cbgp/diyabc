/*
 * tests.cpp
 *
 *  Created on: 11 april 2011
 *      Author: cornuet
 */




    void dotest1(int seed){
      char paramname[3]={"Ne"};
      
        header.nscenarios=1;
        header.scenario = new ScenarioC[1];
        header.scenario[0].number = 1;
        header.scenario[0].prior_proba = 1.0;
        header.scenario[0].nparam = 1;
        header.scenario[0].nparamvar=1;
        header.scenario[0].nn0=1;
        header.scenario[0].ne0 = new Ne0C[1];
        header.scenario[0].ne0[0].name=paramname;
        header.scenario[0].ne0[0].val=-1;
        header.scenario[0].nevent=1;
        header.scenario[0].event = new EventC[1];
        header.scenario[0].event[0].time=0;
        header.scenario[0].event[0].action='E';
        header.scenario[0].event[0].pop=1;
        header.scenario[0].event[0].sample=1;
        header.scenario[0].nsamp=1;
        header.scenario[0].histparam=new HistParameterC[1]; 
        header.scenario[0].histparam[0].name=paramname;
        header.scenario[0].histparam[0].category='N';
        header.scenario[0].time_sample=0;
        header.nconditions=0;
    }

    void dotest(char* testpar,int seed) {
        int testnum=atoi(testpar);
        switch (testnum) {
          case 1  : dotest1(seed);
                    break;
        
        }
    }