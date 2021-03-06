# DIYABC V2.0

COMPILE SOURCES 
---------------

DIYABC dependencies :
   * Graphical User Interface : pyqt4 (pyqt4-dev-tools), matplotlib (python-matplotlib)
   * DIYABC core : gcc (with open-mp)
   * Documentation : latex



How to compile :
```
   tar -xzf diyabc-v2.xx-source.tgz
   cd diyabc-v2.xx
   make clean
   MYFLAGS="-static" make
```

DIYABC binary, named "general", should be in the src-JMC-C++ directory. 

For HPC cluster, you must compile DIYABC on your cluster and set the binary in DIYABC 
interface preferences>>cluster panel :
 * First choose "cluster" in  preferences>>cluster panel and can give the full path of 
   the "general" binary file on your cluster. 
or
 * Upload the "general" file onto your own personal computer. Choose "local" in  
   preferences>>cluster panel and browse your own personal computer to find the "general" 
   binary file uploaded (the one you just compiled on your cluster).



RUN
---

First, you must compile DIYABC sources to in order to obtain the "general" binary.
Next, Use the following commands:
```
   cd diyabc-v2.xx 
   bash diyabc.sh
```
or 
```
   cd diyabc-v2.xx/gui/src
   python diyabc.py
```

Finally, uncheck "use default executable check" and browse "Path to executable file" in the
preferences>>various panel. This will set the "general" binary file you just compile on 
your personal computer. 
