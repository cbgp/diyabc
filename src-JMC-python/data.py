'''
Created on 24 sept. 2009

@author: cornuet
'''

MICMISSING=-9999
SEQMISSING="[]"

class Error(Exception):
    """Base class for exceptions in this module"""
    pass

class NotsamelengthsequencesError(Error):
    """At least one sequence has a different length"""
    pass

class NotGenepopFileError(Error):
    """Data file is not in Genepop format"""

class MissingData(object):
    def __init__(self,sample=None,locus=None,indiv=None):
        self.sample = sample
        self.locus  = locus
        self.indiv  = indiv
    def __str__(self):
        return "Missing data at (sample=%s, locus=%s, indiv=%s)" % (self.sample, self.locus, self.indiv)
    def __repr__(self):
        return "MissingData(sample=%s, locus=%s, indiv=%s)" % (self.sample, self.locus, self.indiv)
        
class MissingNucleotide(object):
    def __init__(self,sample=None,locus=None,indiv=None,nucleotide=None):
        self.sample = sample
        self.locus  = locus
        self.indiv  = indiv
        self.nucleotide = nucleotide 
    def __str__(self):
        return "Missing nucleotide at (sample=%s, locus=%s, indiv=%s, nucleotide=%s)" % (self.sample, self.locus, self.indiv, self.nucleotide)
    def __repr__(self):
        return "MissingNucleotide(sample=%s, locus=%s, indiv=%s, nucleotide=%s)" % (self.sample, self.locus, self.indiv, self.nucleotide)

class LocusType(object):
    def __init__(self, type=None):
        self.type = type
        
class Locus(object):
    """ doc"""
    
    LOC_TYP    = ('<A>','<H>', '<X>', '<Y>', '<M>')
    LOC_TYP1   = ('_A','_H','_X','_Y','_M')
    LOCUS_TYPE = ('mic_A','mic_H','mic_X','mic_Y','mic_M','seq_A','seq_H','seq_X','seq_Y','seq_M')
    SLOCUS_TYPE= ('Autosomal diploid microsatellites','Autosomal haploid microsatellites','X-linked microsatellites','Y-linked microsatellites','Mitochondrial microsatellites',
                  'Autosomal diploid DNA sequences','Autosomal haploid DNA sequences','X-linked DNA sequences','Y-linked DNA sequences','Mitochondrial DNA sequences')
    
    def __init__(self, type=None, name=None, group=None,
#                 pi_A=None, pi_C=None, pi_G=None, pi_T=None,
#                 mut_rate=-1,Pgeom=-1, sni_rate=-1, mus_rate=-1, k1=-1, k2=-1, 
#                 tabsit=[], mutsit=[], sitmut=[]
                 mini=None, maxi=None, kmin=None, kmax=None, motif_size=None, motif_range=None,
                 ngenes=None, dnalength=None):
        self.name    = name
        self.group   = group
        if type not in (0,1,2,3,4,5,6,7,8,9) :
            raise ValueError, "type must be in %s, %s given" % (Locus.LOCUS_TYPE,type)
        self.type    = type
#Proprietes des locus sequences
#        self.pi_A   = pi_A
#        self.pi_C   = pi_C
#        self.pi_G   = pi_G
#        self.pi_T   = pi_T
#        self.mutsit      = mutsit   #list of dnalength elements giving the relative probability of a mutation to a given site of the sequence 
#        self.sitmut      = sitmut   #list of dna sites that are changed through a mutation  
        self.dnalength   = dnalength
#        self.tabsit      = tabsit
#Proprietes des locus microsatellites
        self.mini   = mini
        self.maxi   = maxi
        self.kmin   = kmin
        self.kmax   = kmax
        self.motif_size  = motif_size
        self.motif_range = motif_range
        self.ngenes      = ngenes
        
    def __str__(self):
        if self.type<5 :
            return "{0} (type={1})".format(self.name, self.type)
        else :
            return "%s(type=%s, length=%s)" % (self.name, self.type, self.dnalength)
    def __repr__(self):
        if self.type<5 :
            return "Locus(name=%s, type=%s)" % (self.name, self.type)
        else :
            return "Locus(name=%s, type=%s, length=%s)" % (self.name, self.type,  self.length)
    
        
class LocusList(list):
    def append(self,value):
        if isinstance(value,Locus) :
            super(LocusList,self).append(value)
        else :
            raise ValueError , "This is not a locus"
        
class LocusGroup(object):
                    
    def __init__(self,type=None,nloc=None,loclist=None,p_fixe=None,gams=None,mutmoy=None,Pmoy=None,snimoy=None,musmoy=None,k1moy=None,k2moy=None,mutmod=None,
                 priormusmoy=None,priormusloc=None,priork1moy=None,priork1loc=None,priork2moy=None,priork2loc=None,
                 priormutmoy=None,priormutloc=None,priorPmoy=None,priorPloc=None,priorsnimoy=None,priorsniloc=None):
        self.type = type
        self.nloc = nloc
        self.loclist = loclist
        self.p_fixe = p_fixe
        self.gams = gams
        self.mutmoy = mutmoy
        self.Pmoy = Pmoy
        self.snimoy = snimoy
        self.musmoy = musmoy
        self.k1moy = k1moy
        self.k2moy = k2moy
        self.mutmod = mutmod
        self.priormutmoy = priormutmoy
        self.priorPmoy = priorPmoy
        self.priorsnimoy = priorsnimoy
        self.priormusmoy = priormusmoy
        self.priork1moy = priork1moy
        self.priork2moy = priork2moy
        self.priormutloc =priormutloc
        self.priorPloc   =priorPloc
        self.priorsniloc =priorsniloc
        self.priormusloc =priormusloc
        self.priork1loc  =priork1loc
        self.priork2loc  =priork2loc

class LocusGroupList(list):
    def append(self,value):
        if isinstance(value,LocusGroup) :
            super(LocusList,self).append(value)
        else :
            raise ValueError , "This is not a locus group"

class Data(object):
    ''' this class describes an extended genepop format data set and read it from file'''
    def __init__(self,filename):
        self.filename         = filename  # name of the genepop data file (string)
        self.message          = None  # message about the content of the data file if reading is successful or error message if not  (string)
        self.title            = None  # title of the data file  (string)
        self.nloc_mic         = 0     # number of microsatellite loci (integer)
        self.nloc_seq         = 0     # number of sequence loci (integer)
        self.nloc             = 0     # total number of loci (integer)
        self.nsample          = 0     # number of samples (integer)
        self.nind             = []    # number of individuals per [sample] (list of integer)
        self.samplesize       = []    # number of gene copies per [locus][sample] EXcluding missing data (list of list of integer)
        self.locuslist        = LocusList()    # list of loci (list of Locus)
        self.locusgrouplist   = LocusGroupList()    # list of groups of loci (list of LocusGroup)

        self.indivname        = []    # list of names of individuals (list of list of string) [sample][individual]
        self.indivsexe        = []    # list of sexes of individuals (list of list of integer) (male=1,female=2) [sample][individual]
        self.haplo            = []    # values of gene copies per [locus][sample][gene copy] : list of list of list of (integer,string)
        self.nhaplo           = []    # number of gene copies per [locus][sample] including missing data (list of list of integer)
        self.missdat          = []    # list of missing data in microsatellite loci (list of Missingdata)
        self.missnuc          = []    # list of missing nucleotide in sequence loci (list of MissingNucleotide)
        self.sexratio         = 0.5   # NM/(NF+NM)

    def __locus_type_and_ploidy(self,lines,iloc):
        for i,line in enumerate(lines):
            if line.upper().find("POP")>-1 : break
        line = lines[i+1]
        line2 = line.split(",")
        line3 = line2[1].split()
        gen = line3[iloc]
        if gen.find("<[")>-1 : return "seq_"
        else                 : return "mic_"
        
    def __test_genepop(self):
        try :    
            with open(self.filename,'r') as f: read_data = f.read()
        except :
            raise IOError , "Error when attempting to read data file"
        f=open(self.filename,'r')
        read_data = f.read() 
        lines = read_data.splitlines()
        f.close()
        if len(lines)<4 :
#data file with not enough lines
            raise NotGenepopFileError("data file with not enough lines")
        npop=0
        for li in lines :
            lim= li.upper()
            if (lim.find('POP')>-1) : npop +=1
        if npop<1 :
#data file with no keyword "POP"
            raise NotGenepopFileError('data file with no keyword "POP"')
        npop=0
        for li in lines :
            lim= li.upper()
            if (lim.find('POP')>-1) : npop +=1
            if (npop>0) and (lim.find('POP')<0)and (li.find(',')<1) :
#data file with missing comma between individual name and genotype
               raise NotGenepopFileError("either Genepop file with missing comma between individual name and genotype or not a Genepop file")
        nloc=0
        for ili,li in enumerate(lines) :
            lim= li.upper()
            if (lim.find('POP')<0)and(ili>0) : nloc+=1
            if lim.find('POP')>-1 : break
        for ili,li in enumerate(lines) :
            lim= li.upper()
            if ili>nloc :
                if (lim.find('POP')<0) :
                    li2=li.expandtabs(1)
                    li3=li2.split(",")
                    li4=li3[1].split()
                    if len(li4)!=nloc :
                        mes="Individual {0} has a wrong number of loci".format(li3[0].strip())
#an individual has a wrong number of loci
                        raise NotGenepopFileError(mes)

    def __read_genepop(self):
        self.__test_genepop()
        f=open(self.filename,'r')
        read_data = f.read() 
        lines = read_data.splitlines()
        f.close()
        self.title = lines[0]
        nl = 1
        line = lines[nl].upper()
        iloc=-1
#        self.nloc_mic = 0
#        self.nloc_seq = 0
# loop to read locus names and parentality
        while (line.find('POP')<0) and (nl<len(lines)) :
            iloc+=1
            tp = self.__locus_type_and_ploidy(lines,iloc)
            c1,c2=line.find('<'),line.find('>')
            if (c1>-1) and (c2>-1) :
                t=line[c1+1]
                tt=tp+t
                itt=0
                while (tt!=Locus.LOCUS_TYPE[itt]) : itt+=1
                locusc = Locus(type=itt)
                mots = lines[nl].split() 
                locusc.name=lines[nl][0:c1-1]
            else : 
                locusc =  Locus(type=0)
                locusc.name = lines[nl].strip()
            nl+=1
            line = lines[nl].upper()
            #print locusc.name,"   ",locusc.type
            self.locuslist.append(locusc)
        self.nloc = len(self.locuslist)
        self.nsample = 0
        self.indivname.append([])
        self.genotype = []
        nindtot = 0
# loop to read genotypes
        while nl<len(lines) :
            line = lines[nl].upper()
            if line.find('POP')>-1 :
                self.nsample +=1
                self.nind.append(0)
                self.indivname[-1].append([])
            else :    
# now parsing individual's data lines as 
#   1-001 ,    208208   000000   204206   220220 
                line=lines[nl].expandtabs(1)  #remplace les tabulations par des espaces
                nindtot +=1
                self.nind[-1] +=1
                line2 = line.split(",")
                line3 = line2[1].split()
                self.indivname[-1].append(line2[0].strip())
                self.genotype.append(line3)
                #print self.genotype[-1]
            nl +=1
        self.nloc_tot = self.nloc_seq + self.nloc_mic
        

    def __domicrosat(self,loc,iloc):
# dealing with a microsatellite locus
#        self.ss_mic.append([])
        self.samplesize.append([])
        self.nhaplo.append([])
        self.haplo.append([])
        indtot=-1
        min=1000
        max=0
        for sample in range(self.nsample) :
#            self.ss_mic[-1].append(0)
            self.samplesize[-1].append(0)
            self.nhaplo[-1].append(0)
            self.haplo[-1].append([])
            for ind in range(self.nind[sample]) :
                indtot+=1
#                self.ss_mic[-1][-1] += 1
                geno = self.genotype[indtot][iloc]
                if len(geno)>3    : 
                    gen =[geno[:len(geno)/2],geno[len(geno)/2:]]
                    self.nhaplo[-1][-1] +=2
                else              : 
                    gen = [geno]
                    self.nhaplo[-1][-1] +=1
                if (len(geno)<4)  :
                    if (loc.type==2) : self.indivsexe[sample][ind]=1
                    if (loc.type==3) and(gen!="000") :self.indivsexe[sample][ind]=2
                for g in gen :
                    if g == "000" :
                        if not ((self.indivsexe[sample][ind]==2)and(loc.type==3)) : 
#                            print "g='000' pour sexe=",self.indivsexe[sample][ind],"   type=",loc.type
                            self.haplo[-1][-1].append((MICMISSING,str(MICMISSING)))
                            m = MissingData(locus=iloc,sample=sample,indiv=len(self.haplo[-1][-1])-1)
                            self.missdat.append(m)
                    else : 
                        gg=int(g)
                        self.haplo[-1][-1].append((gg,g))
                        self.samplesize[-1][-1] += 1
                        if gg>max : max=gg
                        if gg<min : min=gg
        loc.mini, loc.maxi = min, max
        self.nloc_mic +=1

    def __dosequence(self,loc,iloc):
#dealing with a seqsuence locus
        seqlen=-1
#        self.ss_seq.append([])
        self.samplesize.append([])
        self.nhaplo.append([])
        self.haplo.append([])
        pi_A = pi_C = pi_G = pi_T = nt = 0
        indtot=-1
        for sample in range(self.nsample) :
#            self.ss_seq[-1].append(0)
            self.samplesize[-1].append(0)
            self.nhaplo[-1].append(0)
            self.haplo[-1].append([])
            for ind in range(self.nind[sample]) :
                indtot+=1
#                self.ss_seq[-1][-1] += 1
                geno = self.genotype[indtot][iloc]
                if geno.find('][')<0 :  #une seule copie
                    gen = [geno[geno.find("[")+1:geno.find("]")]]
                    if (loc.type==7) : self.indivsexe[sample][ind]=1
                    if (loc.type==8) :
                        if (len(gen[0])!=0) :self.indivsexe[sample][ind]=1
                        else             :self.indivsexe[sample][ind]=2
                    self.nhaplo[-1][-1] +=1
                else :                #deux copies
                    g1 = geno[geno.find("[")+1:geno.find("]")]
                    g2 = geno[geno.find("]")+2:]           
                    g2=g2[g2.find("[")+1:g2.find("]")]
                    gen = [g1,g2]
                    self.nhaplo[-1][-1] +=2
                for g in gen:
                    if len(g)==0 :
                        if  not ((self.indivsexe[sample][ind]==2)and(loc.type==8)) :
                            self.haplo[-1][-1].append((0,g))
                            m = MissingData(locus=iloc,sample=sample,indiv=len(self.haplo[-1][-1])-1)
                            self.missdat.append(m)
                    else :
                        if seqlen==-1 : seqlen = len(g)
                        if len(g) != seqlen :
                            mes="At locus {0}, individual {1} has a sequence length different from previous sequences at the same locus".format(self.locuslist[iloc].name,self.indivname[sample][ind])
                            raise NotsamelengthsequencesError(mes)
                        self.haplo[-1][-1].append((0,g))
                        self.samplesize[-1][-1] += 1
                        for i in range(len(g)):
                            c = g[i].upper()
                            if   c == "A" : pi_A,nt = pi_A+1.0,nt+1
                            elif c == "C" : pi_C,nt = pi_C+1.0,nt+1
                            elif c == "G" : pi_G,nt = pi_G+1.0,nt+1
                            elif c == "T" : pi_T,nt = pi_T+1.0,nt+1
                            elif c != "-" :
                                m = MissingNucleotide(locus=iloc,sample=sample,indiv=len(self.haplo[-1][-1])-1,nucleotide=i)
                                self.missnuc.append(m) 
        loc.pi_A, loc.pi_C, loc.pi_G, loc.pi_T = pi_A/nt, pi_C/nt, pi_G/nt, pi_T/nt
        loc.length = seqlen
        self.nloc_seq +=1                                
    
    def loadfromfile(self):
# raw reading of the data file
        self.__read_genepop()
#initialize indivsexe
        for sa in range(self.nsample) :
            self.indivsexe.append([])
            for ind in range(self.nind[sa]) :
                self.indivsexe[-1].append(2)  #DEFAULT = FEMALE  
            
# reading genotypes
        for iloc,loc in enumerate(self.locuslist):
            if loc.type<5 :  self.__domicrosat(loc,iloc)
            if loc.type>4 :  self.__dosequence(loc,iloc)

                                
                                                                   
                        
                
                
        











 
data=Data(filename='tout.txt') 
data.loadfromfile()
nindtot=0
for n in data.nind :
    nindtot += n
print "%s locus (%s microsat et %s sequences), %s individus au total" % (data.nloc,data.nloc_mic,data.nloc_seq,nindtot)
for loc in data.locuslist :
    print loc.name,"   type=",loc.type
for m in data.missdat: print m
for m in data.missnuc: print m
for m in data.haplo:  print m




#print "toto"
#print data.samplesize
#print data.haplo[0][0][0]
#print data.haplo
#print "=============================================="
#for loc in range(len(data.locuslist)):
#    if data.locuslist[loc].type=="mic" :
#        for sam in range(data.nsample) :
#            for ind in range(data.samplesize[loc][sam]) :
#                print loc,sam,ind,data.haplo[loc][sam][ind]
#                
#print data.nloc_mic,"locus microsatellites"
#for loc in data.locuslist: print loc

#print data.nloc_seq,"locus sequences"
#for loc in data.locuslist: print loc

#iloc=-1
#for m in data.indivname: print m
#for loc in data.locuslist :
#    if loc.type=="seq" :
#        iloc+=1
#        print loc.ploidy
#        for sam in range(data.nsample):
#            for ind in range(len(data.haplo_seq[iloc][sam])):
#                print iloc,sam,ind,data.haplo_seq[iloc][sam][ind]

#print data.haplo_seq[0][0]
#for m in data.missdat_mic:print m
#for m in data.missdat: print m
#for m in data.missnuc: print m
#for m in data.samplesize_mic : print m
#print
#for m in data.samplesize_seq : print m

