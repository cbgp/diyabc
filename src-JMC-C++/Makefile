CFLAGS = -fopenmp -msse4 -funroll-loops -DNDEBUG=1   -I/usr/include  
LIBS =  -L/usr/local/lib64 -L/usr/lib64

all: general

general: global.h general.cpp acploc.cpp bias.cpp comparscen.cpp conf.cpp data.cpp estimparam.cpp header.cpp matrices.cpp mesutils.cpp modchec.cpp particleset.cpp particuleC.cpp randomgenerator.cpp reftable.cpp simfile.cpp testgen.cpp  
	g++ $(CFLAGS) -o general general.cpp $(LIBS)

clean: 
	rm general 