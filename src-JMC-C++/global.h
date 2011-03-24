    ReftableC rt;
    HeaderC header;
    char *headerfilename, *reftablefilename,*datafilename,*statobsfilename, *reftablelogfilename,*path,*ident;
    double **matX0, *vecW, **alpsimrat,**parsim;    
    int nstatOKsel;
    
    
    
    
                                                     
                  
/*                                                                                                                                                                                                                                                                  
    
-e "s:1;n:3000000;m:10000;t:3;p:oc" -m
-c "s:1,2,3;n:3000000;d:500;l:30000;m=10"
-b "s:1;d:100;n:1000000;m:10000;t:3;p:oc;h:N=3000 t1=10000 t2=20000;u:G1(0.0005 2.0 0.22 2.0 0.0 0.0)-G2(0.0005 2.0 0.22 2.0 0.0 0.0)"
-b "s:1;d:100;n:1000000;m:10000;t:3;p:oc;h:N=UN[100,10000,0,0] t1=UN[1000,50000,0,0] t2=UN[1000,50000,0,0];u:G1(UN[0.0001,0.001,0,0] GA[0.00001,0.01,0.0,2.0] UN[0.1,0.3,0.0,0.0] GA[0.05,0.5,0.0,2.0] UN[0.0,0.0,0.0,0.0] GA[0.0,0.0,0.0,0.0])"
*/