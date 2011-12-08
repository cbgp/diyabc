CFLAGS = -fopenmp -O3   
LIBS =   -L. -ldcmt
CPPFILES = general.cpp acploc.cpp bias.cpp comparscen.cpp conf.cpp data.cpp estimparam.cpp header.cpp matrices.cpp mesutils.cpp modchec.cpp particleset.cpp particuleC.cpp randomgenerator.cpp reftable.cpp simfile.cpp testgen.cpp  
HFILES = global.h particuleC.h

all: general

general:  $(CPPFILES) $(HFILES)
	g++ $(CFLAGS) -o general general.cpp $(LIBS)

clean: 
	rm general 