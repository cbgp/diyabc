CFLAGS = -fopenmp -O3   -g
LIBS =   -L. -ldcmt
CPPFILES = general.cpp acploc.cpp bias.cpp comparscen.cpp conf.cpp data.cpp estimparam.cpp header.cpp matrices.cpp mesutils.cpp modchec.cpp particleset.cpp particuleC.cpp randomgenerator.cpp reftable.cpp simfile.cpp testgen.cpp  
HFILES = global.h particuleC.h history.h header.h 
OBJS = acploc.o bias.o comparscen.o conf.o data.o estimparam.o general.o header.o history.o matrices.o mesutils.o particleset.o particuleC.o randomgenerator.o reftable.o simfile.o sumstat.o modchec.o  

all: general initRNGproject

libdcmt.a:
	cd ../dcmt0.6.1/lib/ && $(MAKE) $(MFLAGS)  

### OK :
acploc.o: acploc.cpp acploc.h
	g++ $(CFLAGS) -o acploc.o -c acploc.cpp

bias.o: bias.cpp bias.h
	g++ $(CFLAGS) -o bias.o -c bias.cpp

comparscen.o: comparscen.cpp comparscen.h
	g++ $(CFLAGS) -o comparscen.o -c comparscen.cpp

conf.o: conf.cpp conf.h
	g++ $(CFLAGS) -o conf.o -c conf.cpp

data.o: data.cpp data.h
	g++ $(CFLAGS) -o data.o -c data.cpp

estimparam.o: estimparam.cpp estimparam.h
	g++ $(CFLAGS) -o estimparam.o -c estimparam.cpp

general.o: general.cpp
	g++ $(CFLAGS) -o general.o -c general.cpp

header.o: header.cpp header.h
	g++ $(CFLAGS) -o header.o -c header.cpp

history.o: history.cpp history.h
	g++ $(CFLAGS) -o history.o -c history.cpp

initRNGproject.o: initRNGproject.cpp
	g++ $(CFLAGS) -o initRNGproject.o -c initRNGproject.cpp

matrices.o: matrices.cpp matrices.h
	g++ $(CFLAGS) -o matrices.o -c matrices.cpp

mesutils.o: mesutils.cpp mesutils.h
	g++ $(CFLAGS) -o mesutils.o -c mesutils.cpp

particleset.o: particleset.cpp particleset.h
	g++ $(CFLAGS) -o particleset.o -c particleset.cpp
	
particuleC.o: particuleC.cpp particuleC.h
	g++ $(CFLAGS) -o particuleC.o -c particuleC.cpp

randomgenerator.o: randomgenerator.cpp randomgenerator.h
	g++ $(CFLAGS) -o randomgenerator.o -c randomgenerator.cpp

reftable.o: reftable.cpp reftable.h
	g++ $(CFLAGS) -o reftable.o -c reftable.cpp

simfile.o: simfile.cpp simfile.h
	g++ $(CFLAGS) -o simfile.o -c simfile.cpp

sumstat.o: sumstat.cpp particuleC.h
	g++ $(CFLAGS) -o sumstat.o -c sumstat.cpp

modchec.o: modchec.cpp modchec.h
	g++ $(CFLAGS) -o modchec.o -c modchec.cpp

general:  $(OBJS) libdcmt.a
	g++ $(CFLAGS) -o general $(OBJS) $(LIBS)

initRNGproject: initRNGproject.o  randomgenerator.o
	g++ $(CFLAGS) -o initRNGproject initRNGproject.o randomgenerator.o $(LIBS)



### Pour faire des tests



testRNG.o: testRNG.cpp
	g++ $(CFLAGS) -o testRNG.o -c testRNG.cpp

testRNG: testRNG.o randomgenerator.o
	g++ $(CFLAGS) -o testRNG testRNG.o randomgenerator.o $(LIBS)


clean: 
	rm *.o
	rm general 