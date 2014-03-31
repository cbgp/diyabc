all:
	cd dcmt0.6.1/lib/; $(MAKE) $(MFLAGS) 
	cd src-JMC-C++; staticgcc=-static-libgcc debug=-g opti=-O3  $(MAKE) $(MFLAGS) all

fast:
	cd dcmt0.6.1/lib/; $(MAKE) $(MFLAGS)
	cd src-JMC-C++; opti=-O0 $(MAKE) $(MFLAGS) all

valgrind:
	cd dcmt0.6.1/lib/; $(MAKE) $(MFLAGS)
	export GLIBCXX_FORCE_NEW=1; cd src-JMC-C++; opti=-O3 debug=-g $(MAKE) $(MFLAGS) all

dpkg:
	cd dcmt0.6.1/lib/; $(MAKE) $(MFLAGS) 
	cd src-JMC-C++; $(MAKE) -f dpkgMakefile $(MFLAGS) all

clean:
	cd dcmt0.6.1/lib/; $(MAKE) $(MFLAGS) clean
	cd src-JMC-C++; $(MAKE) $(MFLAGS) clean

force_look:
	true
