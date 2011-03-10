/*
 * estimparam.cpp
 *
 *  Created on: 9 march 2011
 *      Author: cornuet
 */
#include "simuldatC.cpp"

    int cal_varstat(char* reftablefilename,double *var_stat) {
      
        
        return 0;
    }












    void doestim(char *headerfilename,char *reftablefilename,char *statobsfilename,char *options) {
        char *datafilename;
        headerC header;
        ReftableC rt;
        double *var_stat;
        int rtOK,nstatOK;
        
        header.readHeader(headerfilename);
        datafilename=strdup(header.datafilename.c_str());
        rtOK=rt.readheader(reftablefilename,header.nscenarios,datafilename);)
                  
        
        
        nstatOK = cal_varstat(reftablefilename,var_stat);
    
    
    
    }