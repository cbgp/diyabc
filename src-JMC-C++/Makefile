CFLAGS = -fopenmp -O3   -g
LIBS =   -L. -ldcmt
CPPFILES = general.cpp acploc.cpp bias.cpp comparscen.cpp conf.cpp data.cpp estimparam.cpp header.cpp matrices.cpp mesutils.cpp modchec.cpp particleset.cpp particuleC.cpp randomgenerator.cpp reftable.cpp simfile.cpp testgen.cpp  
HFILES = global.h particuleC.h history.h header.h 

all: general

matrices.o: matrices.cpp matrices.h
	g++ $(CFLAGS) -o matrices.o -c matrices.cpp

mesutils.o: mesutils.cpp mesutils.h
	g++ $(CFLAGS) -o mesutils.o -c mesutils.cpp

randomgenerator.o: randomgenerator.cpp randomgenerator.h
	g++ $(CFLAGS) -o randomgenerator.o -c randomgenerator.cpp

data.o: data.cpp data.h
	g++ $(CFLAGS) -o data.o -c data.cpp

history.o: history.cpp history.h
	g++ $(CFLAGS) -o history.o -c history.cpp

sumstat.o: sumstat.cpp particuleC.h
	g++ $(CFLAGS) -o sumstat.o -c sumstat.cpp

particuleC.o: particuleC.cpp particuleC.h
	g++ $(CFLAGS) -o particuleC.o -c particuleC.cpp

header.o: header.cpp header.h
	g++ $(CFLAGS) -o header.o -c header.cpp

reftable.o: reftable.cpp reftable.h
	g++ $(CFLAGS) -o reftable.o -c reftable.cpp

testRNG.o: testRNG.cpp
	g++ $(CFLAGS) -o testRNG.o -c testRNG.cpp

testRNG: testRNG.o randomgenerator.o
	g++ $(CFLAGS) -o testRNG testRNG.o randomgenerator.o $(LIBS)

general:  $(CPPFILES) $(HFILES)
	g++ $(CFLAGS) -o general general.cpp $(LIBS)

clean: 
	rm *.o
	rm general 