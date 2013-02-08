all:
	cd dcmt0.6.1/lib/; $(MAKE) $(MFLAGS) 
	cd src-JMC-C++; $(MAKE) $(MFLAGS) all

dpkg:
	cd dcmt0.6.1/lib/; $(MAKE) $(MFLAGS) 
	cd src-JMC-C++; $(MAKE) -f dpkgMakefile $(MFLAGS) all

clean:
	cd dcmt0.6.1/lib/; $(MAKE) $(MFLAGS) clean
	cd src-JMC-C++; $(MAKE) $(MFLAGS) clean

force_look:
	true
