CFLAGS = -fopenmp -O3   -g  #-Wall -Wextra
LIBS =   -L. -ldcmt  #-lstdc++ -lgomp -lm -lc
CPPFILES = general.cpp acploc.cpp bias.cpp comparscen.cpp conf.cpp data.cpp estimparam.cpp header.cpp matrices.cpp mesutils.cpp modchec.cpp particleset.cpp particuleC.cpp randomgenerator.cpp reftable.cpp simfile.cpp testgen.cpp  
HFILES = global.h particuleC.h history.h header.h 
OBJS = acploc.o bias.o comparscen.o conf.o data.o estimparam.o general.o header.o history.o matrices.o mesutils.o particleset.o particuleC.o randomgenerator.o reftable.o simfile.o sumstat.o modchec.o  
CC = g++
#CC = /usr/local/bin/g++

all: general initRNGproject

libdcmt.a:
	cd ../dcmt0.6.1/lib/ && $(MAKE) $(MFLAGS)  

### OK :
acploc.o: acploc.cpp acploc.h
	$(CC) $(CFLAGS) -o acploc.o -c acploc.cpp

bias.o: bias.cpp bias.h
	$(CC) $(CFLAGS) -o bias.o -c bias.cpp

comparscen.o: comparscen.cpp comparscen.h
	$(CC) $(CFLAGS) -o comparscen.o -c comparscen.cpp

conf.o: conf.cpp conf.h
	$(CC) $(CFLAGS) -o conf.o -c conf.cpp

data.o: data.cpp data.h
	$(CC) $(CFLAGS) -o data.o -c data.cpp

estimparam.o: estimparam.cpp estimparam.h
	$(CC) $(CFLAGS) -o estimparam.o -c estimparam.cpp

general.o: general.cpp
	$(CC) $(CFLAGS) -o general.o -c general.cpp

header.o: header.cpp header.h
	$(CC) $(CFLAGS) -o header.o -c header.cpp

history.o: history.cpp history.h
	$(CC) $(CFLAGS) -o history.o -c history.cpp

initRNGproject.o: initRNGproject.cpp
	$(CC) $(CFLAGS) -o initRNGproject.o -c initRNGproject.cpp

matrices.o: matrices.cpp matrices.h
	$(CC) $(CFLAGS) -o matrices.o -c matrices.cpp

mesutils.o: mesutils.cpp mesutils.h
	$(CC) $(CFLAGS) -o mesutils.o -c mesutils.cpp

particleset.o: particleset.cpp particleset.h
	$(CC) $(CFLAGS) -o particleset.o -c particleset.cpp
	
particuleC.o: particuleC.cpp particuleC.h
	$(CC) $(CFLAGS) -o particuleC.o -c particuleC.cpp

randomgenerator.o: randomgenerator.cpp randomgenerator.h
	$(CC) $(CFLAGS) -o randomgenerator.o -c randomgenerator.cpp

reftable.o: reftable.cpp reftable.h
	$(CC) $(CFLAGS) -o reftable.o -c reftable.cpp

simfile.o: simfile.cpp simfile.h
	$(CC) $(CFLAGS) -o simfile.o -c simfile.cpp

sumstat.o: sumstat.cpp particuleC.h
	$(CC) $(CFLAGS) -o sumstat.o -c sumstat.cpp

modchec.o: modchec.cpp modchec.h
	$(CC) $(CFLAGS) -o modchec.o -c modchec.cpp

general:  $(OBJS) libdcmt.a
	$(CC) $(CFLAGS) -o general $(OBJS) $(LIBS)

initRNGproject: initRNGproject.o  randomgenerator.o
	$(CC) $(CFLAGS) -o initRNGproject mesutils.o matrices.o initRNGproject.o randomgenerator.o $(LIBS)



### Pour faire des tests



testRNG.o: testRNG.cpp
	$(CC) $(CFLAGS) -o testRNG.o -c testRNG.cpp

testRNG: testRNG.o randomgenerator.o
	$(CC) $(CFLAGS) -o testRNG testRNG.o randomgenerator.o $(LIBS)


clean: 
	rm *.o
	rm general 