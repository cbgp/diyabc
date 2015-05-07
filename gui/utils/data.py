# -*- coding: utf-8 -*-
'''
Created on 24 sept. 2009

@author: cornuet
'''

import re,os.path, sys, codecs, traceback
from sets import Set
from collections import defaultdict
from cbgpUtils import log,getFsEncoding

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


LOC_TYP    = ('<A>','<H>', '<X>', '<Y>', '<M>')
LOC_TYP1   = ('_A','_H','_X','_Y','_M')
LOCUS_TYPE = ('mic_A','mic_H','mic_X','mic_Y','mic_M','seq_A','seq_H','seq_X','seq_Y','seq_M')
SLOCUS_TYPE= ('Autosomal diploid microsatellites','Autosomal haploid microsatellites','X-linked microsatellites','Y-linked microsatellites','Mitochondrial microsatellites',
              'Autosomal diploid DNA sequences','Autosomal haploid DNA sequences','X-linked DNA sequences','Y-linked DNA sequences','Mitochondrial DNA sequences')


class Locus(object):
    """ doc"""


    def __init__(self, type=None, name=None, group=None,
#                 pi_A=None, pi_C=None, pi_G=None, pi_T=None,
#                 mut_rate=-1,Pgeom=-1, sni_rate=-1, mus_rate=-1, k1=-1, k2=-1,
#                 tabsit=[], mutsit=[], sitmut=[]
                 mini=None, maxi=None, kmin=None, kmax=None, motif_size=None, motif_range=None,
                 ngenes=None, dnalength=None):
        self.name    = name
        self.group   = group
        if type not in (0,1,2,3,4,5,6,7,8,9) :
            raise ValueError, "type must be in %s, %s given" % (LOCUS_TYPE,type)
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
            return "%s (type=%s)"%(self.name, self.type)
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
    __LOCUS_TYPES = ["A", "H", "Y", "X", "M"]
    __ACCEPTED_CHARACTERS = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-<>[], \t.'
    __SEQUENCE_CODE = "ATGCN-atgcn"
    def __init__(self,filename):
        self.filename         = filename  # name of the genepop data file (string)
        self.fsCoding         = getFsEncoding(logLevel=False)
        self.message          = None  # message about the content of the data file if reading is successful or error message if not  (string)
        self.title            = None  # title of the data file  (string)
        self.nloc_mic         = 0     # number of microsatellite loci (integer)
        self.nloc_seq         = 0     # number of sequence loci (integer)
        self.nloc             = 0     # total number of loci (integer)
        self.nsample          = 0     # number of samples (integer)
        self.nind             = []    # number of individuals per [sample] (list of integer)
        self.nindtot          = 0     # total number of individuals
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
            if (i>0)and(line.upper().find("POP")>-1) : break
        line = lines[i+1]
        line2 = line.split(",")
        line3 = line2[1].split()
        gen = line3[iloc]
        if gen.find("<[")>-1 : return "seq_"
        else                 : return "mic_"

    def __test_genepop(self):
        "not so quick but very dirty genepop file format tester"
        try :
            #with open(self.filename,'r') as f: read_data = f.read()
            f = open(str(self.filename).encode(self.fsCoding),'r')
        except :
            raise IOError , "Error when attempting to read data file %s" % self.filename
        read_data = f.read().strip()
        lines = read_data.splitlines()
        f.close()


        if len(lines)<4 :
#data file with not enough lines
            raise NotGenepopFileError("data file with not enough lines")
        npop=0

        for ili,li in enumerate(lines) :
            lim= li.upper()
            if (ili>0)and(lim.find('POP')>-1)and(lim.find(',')<1) : npop +=1
            #print lim,"   npop=%s"%(npop)
        if npop<1 :
#data file with no keyword "POP"
            raise NotGenepopFileError('data file with no keyword "POP"')
        npop=0
        for ili,li in enumerate(lines) :
            lim= li.upper()
            if (ili>0) and (lim.find('POP')>-1)and(lim.find(',')<1) : npop +=1
            if (npop>0) and (lim.find('POP')<0)and (li.find(',')<1) :
#data file with missing comma between individual name and genotype
               #print ili
               raise NotGenepopFileError("either Genepop file with missing comma between individual name and genotype or not a Genepop file")
        nloc=0
        for ili,li in enumerate(lines) :
            lim= li.upper()
            if (lim.find('POP')<0) and (ili>0) : nloc+=1
            if (lim.find('POP')>-1) and (ili>0) : break
        for ili,li in enumerate(lines) :
            lim= li.upper()
            if ili>nloc :
                if (lim.find('POP')<0) :
                    li2=li.expandtabs(1)
                    li3=li2.split(",")
                    li4=li3[1].split()
                    if len(li4)!=nloc :
                        mes="Individual %s has a wrong number of loci"%(li3[0].strip())
#an individual has a wrong number of loci
                        raise NotGenepopFileError(mes)
        # others tests : unknown characters and locus section
        popNum = 0
        for ili,li in enumerate(lines[1:]) :
            error_list = [c in Data.__ACCEPTED_CHARACTERS for c in li]
            if not all(error_list) :
                raise NotGenepopFileError("line : %s, found and unknown character : %s >%s< %s. \n Be careful, this can be a special character which could not be printed on screen !" \
                        % (ili+2, li[:error_list.index(False)], li[error_list.index(False)] , li[error_list.index(False) + 1:]))
            li = li.split()
            if len(li) >= 1 :
                if li[0].upper() == "POP":
                    popNum += 1
    #                 if len(li) > 1 :
    #                     raise NotGenepopFileError("line : %s, key word POP must be alone")
                else :
                    #locus infos
                    if not popNum :
                    # test if locus types are defined
                        if len(li) >= 2 :
                            if '<' in li[-1] or '>' in li[-1] or len(li) >= 3 :
                                try :
                                    locusNotice = "\n Locus type should : \n - start with one and only one '<' \n - be a locus type in %s \n - end with one and only one '>'" % ", ".join(Data.__LOCUS_TYPES)
                                    if li[-1] not in ['<%s>' % k for k in Data.__LOCUS_TYPES] :
                                        if li[-1][0] != '<' or len(li[-1]) == 1 :
                                            raise NotGenepopFileError("""line %s, unknown locus type %s, %s """ % ((ili+2), li[-1], locusNotice))
                                        elif li[-1][1] not in Data.__LOCUS_TYPES or len(li[-1]) == 2 :
                                            raise NotGenepopFileError("""line %s, unknown locus type %s, %s""" % ((ili+2), li[-1], locusNotice))
                                        elif len(li[-1]) != 3 or li[-1][2] != '>' :
                                            raise NotGenepopFileError("""line %s, unknown locus type %s, %s""" % ((ili+2), li[-1], locusNotice))
                                    elif len(li) > 3 :
                                        raise Exception
                                except NotGenepopFileError as e :
                                    raise e
        #                         except :
        #                             raise NotGenepopFileError("line %s, unknown locus definition, expect 1 or 2 words : first is the locus name without any spaces, second (optional) is the locus type" % (ili+2))
                                if len(li) > 3 :
                                    raise NotGenepopFileError("line %s, unknown locus definition, found %s words, only 2 or one expected" % (ili+2, str(len(li))))
                    # pop individual infos
                    else :
                        # kkpz test to stringent. Tests aborded for next release
                        return
                        # if comma is add the end of the name
                        if ',' in li[0] :
                            index = li[0].index(',')
                            if index and index == 0 :
                                li = [',', li[0][index+1:] ] + li[1:]
                            elif index and index == (len(li[0]) - 1) :
                                li = [ li[0][0:index] , ',' ] + li[1:]
                            elif index :
                                li = [ li[0][0:index] , ',', li[0][index+1:] ] + li[1:]
                        # if comma is stick on the first value
                        elif li[1][0] == ',' and len(li[1]) > 1:
                             li = [li[0]] + [','] + [li[1][1:]] + li[2:]


                        if li[1] != ',' :
                            raise NotGenepopFileError("line %s, POP number %s,  missing comma between individual name and genotype" % (ili+2, popNum))
                        # test individual locus
                        for nlocus, locus in enumerate(li[2:]) :
                            numbers = "1234567890"
                            #microsat locus
                            if locus[0] in numbers :
                                error_list = [c in numbers for c in locus]
                                if not all(error_list) :
                                    raise NotGenepopFileError("line : %s, locus %s in individual %s in POP number %s, found and unknown character for a microsat locus : %s >%s< %s. Only integer numbers are expected.\n Be careful, this can be a special character which could not be printed on screen !"\
                                            % (ili+2, nlocus+1, li[0], popNum, locus[:error_list.index(False)], locus[error_list.index(False)] , locus[error_list.index(False) + 1:]))
                                if len(locus) not in [3,6] :
                                    raise NotGenepopFileError("line : %s, locus %s in individual %s in POP number %s, microsat locus are defined by 3 or 6 digits. Found  %s digits : %s  Be careful, this can be a insertion of a special character which could not be printed on screen !"\
                                            % (ili+2, nlocus+1, li[0], popNum, len(locus), locus))

                            # seq locus
                            elif locus[0] == '<' :
                                if locus[0:2] != '<[' :
                                    raise NotGenepopFileError("line : %s, locus %s in individual %s in POP number %s, sequence locus should start with '<[" \
                                        % (ili+2, nlocus+1, li[0], popNum))
                                if locus[-2:] != ']>' :
                                    raise NotGenepopFileError("line : %s, locus %s in individual %s in POP number %s, sequence locus should end with ']>" \
                                        % (ili+2, nlocus+1, li[0], popNum))

                                seqs = locus[2:-2].split('][')
                                for seq in seqs :
                                    error_list = [c in Data.__SEQUENCE_CODE for c in seq]
                                    if not all(error_list) :
                                        raise NotGenepopFileError("line : %s, locus %s in individual %s in POP number %s, found and unknown character for a sequence  locus : %s >%s< %s. Only integer numbers are expected.\n Be careful, this can be a special character which could not be printed on screen !"\
                                                % (ili+2, nlocus+1, li[0], popNum, seq[:error_list.index(False)], seq[error_list.index(False)] , seq[error_list.index(False) + 1:]))
                            # unknown individual locus format
                            else :
                                raise NotGenepopFileError("line : %s, locus %s in individual %s in POP number %s, unknown individual locus format" \
                                        % (ili+2, nlocus+1, li[0], popNum))

    def __test_genepop_ng(self):
        """Other tests on data file

        Rules =========================
            Locus definition :
                No blank lines
                'Locus' key word alone accepted
                locus name alone accepted
                type tag alone accepted
            POPs
                no blank lines
                "POP" key word must be alone in a single line
                multiples commas accepted, the last one is the delimiter
                for each individual, number of loci must a be egual to loci definitions number
                if microsat is coded with 3 digits, locus type must be Y, H, M
                if seqs is coded with only one seq, locus type must be Y, H, M

        """
        loci = {} # loci[number] = {'name' : 'xx', 'type' : '<x>', line : x, 'num' : x }
        pops = {} # pops[number] = [{'name' : 'xx', 'num' : x , line : x , loci : [locus1, locus2, ...]}, {...},  ]
        try :
            #with open(self.filename,'r') as f: read_data = f.read()
            f = open(str(self.filename).encode(self.fsCoding),'r')
        except :
            log(1, traceback.format_exc())
            raise IOError , "Error when attempting to read data file %s : %s" % (self.filename)
        read_data = f.read().strip()
        f.close()
        p = re.compile("^\\s*POP\\s*$", re.MULTILINE|re.IGNORECASE)
        parts = p.split(read_data)#, re.MULTILINE)
        del read_data
        dataDefLine = parts[0].splitlines()[0]
        lociDefLines = parts[0].splitlines()[1:]
        popsLinesPart = [ popPart.splitlines() for popPart in parts[1:]]

        # loci -------------------------------------
        re1 = '(Locus\\s)?'    # Locus key word
        re2 = '(\\s+)?'     # White Spaces
        re3 = '([\\w]+)?'   # locus name
        re4 = '(\\s+)?'     # White Spaces
        re5 = '(<[^>]+>)?'  # locus type 1
        rg  =  re.compile(re1+re2+re3+re4+re5,re.IGNORECASE)
        locusNum = 0
        for locusLineNum, locusDefinition in enumerate(lociDefLines) :
            locus = rg.search(locusDefinition)
            if locus :
                loci[locusNum] = {'name' : locus.group(3),  'type' : locus.group(5), 'line' : locusLineNum + 2}
                locusNum += 1
            if not locus or len(Set(locus.groups())) <= 1 :
                raise NotGenepopFileError("Genepop file : %s\nline : %s, locus line '%s', unknown locus format" \
                                        % (self.filename, locusLineNum+2, locusDefinition))

        #POPs --------------------------------------

        reMicroSat = re.compile('^\\d+$')
        reSeqs = re.compile('^<\[.*\]>$', re.IGNORECASE)
        re0 = ''
        re1 = '(.+)?'  # individual name
        re2 = '(,)'    # comma mandatory delimiter
        re3 = '(.*)'   # loci values
        rg = re.compile(re1+re2+re3,re.IGNORECASE)
        fileLineNum = locusLineNum+2
        for popNum, popLines in enumerate(popsLinesPart) :
            fileLineNum +=1
            pops[popNum] = []
            popLines = popLines[1:] # remove first blank line (due to split by "pop" regexp)
            for indNum, locusInd in enumerate(popLines) :
                fileLineNum +=1
                individual = rg.match(locusInd)
                if individual :
                    indName = individual.group(1).strip()
                    indLoci = individual.group(3).strip()
                    indLociList = indLoci.split()
                    for indLocusNum, indLocus in enumerate(indLociList) :
                        # microsat
                        if reMicroSat.match(indLocus) :
                            if len(indLocus) == 3 and loci[indLocusNum]['type'] not in ['<Y>','<H>','<M>', '<X>'] :
                                raise NotGenepopFileError("Genepop file : %s\nline : %s, pop number : %s, individual number : %s, individual name : %s, loci number : %s, has a single allele : \nThis locus must be defined as '<X>','<M>', <Y> or '<H>' instead of  locus type %s (line %s, name %s)." \
                                        % (self.filename, fileLineNum, popNum+1, indNum+1, indName, indLocusNum+1, loci[indLocusNum]['type'], loci[indLocusNum]['line'], loci[indLocusNum]['name']))
                        # seqs
                        elif reSeqs.match(indLocus) :
                            try :
                                self.__testSeqLocusFormat(indLocus)
                            except NotGenepopFileError as e :
                                raise NotGenepopFileError("Genepop file : %s\nline : %s, pop number : %s, individual number : %s, individual name : %s, loci number : %s.\n\n%s" \
                                        % (self.filename, fileLineNum, popNum+1, indNum+1, indName, indLocusNum+1, loci[indLocusNum]['type'], loci[indLocusNum]['line'], loci[indLocusNum]['name'], e))

                            if indLocus.count('[') < 2 and loci[indLocusNum]['type'] not in ['<Y>','<H>','<M>', '<X>'] :
                                raise NotGenepopFileError("Genepop file : %s\nline : %s, pop number : %s, individual number : %s, individual name : %s, loci number : %s, has a single allele : \nThis locus must be defined as '<X>','<M>', <Y> or '<H>' instead of locus type %s (line %s, name %s)." \
                                        % (self.filename, fileLineNum, popNum+1, indNum+1, indName, indLocusNum+1, loci[indLocusNum]['type'], loci[indLocusNum]['line'], loci[indLocusNum]['name']))
                        else :
                            raise NotGenepopFileError("Genepop file : %s\nline : %s, pop number : %s, individual number : %s, individual name : %s, locus number : %s, locus type : %s, locus name : %s.\n\n Unknown locus format : %s" \
                                        % (self.filename, fileLineNum, popNum+1, indNum+1, indName, indLocusNum+1, loci[indLocusNum]['type'], loci[indLocusNum]['name'], indLocus))

                    # test if number of locus is egual to locus definition
                    if len(indLociList) != len(loci) :
                        raise NotGenepopFileError("Genepop file : %s\nline : %s, pop number : %s, individual number : %s, individual name : %s.\n\nMust have %s loci, found %s" \
                                        % (self.filename, fileLineNum, popNum+1, indNum+1, indName, len(loci), len(indLociList)))

                else :
                    raise NotGenepopFileError("Genepop file : %s\nline : %s, pop number %s, unknown individual locus format : \n%s" \
                                        % (self.filename, fileLineNum, popNum+1, locusInd))
                pops[popNum].append({
                                     'name' : indName,
                                     'num'  : indLocusNum,
                                     'line' : fileLineNum,
                                     'loci' : indLociList
                                     })


            def nestedDefaultdictAndSet():
                def nested2() :
                    return defaultdict(set)

                return defaultdict(nested2)

            pop_locus_valuesSet = nestedDefaultdictAndSet()
            locus_valuesSet = defaultdict(set)
            # test if a locus of a same pop in only made of missing data
            #pops = {} # pops[number] = [{'name' : 'xx', 'num' : x , 'line' : x , 'loci' : [locus1, locus2, ...]}, {...},  ]
            for  popNum in pops.keys() :
                for ind in pops[popNum] :
                    for locusNum, locusValue in enumerate(ind['loci']) :
                        pop_locus_valuesSet[popNum][locusNum].add(locusValue)
            for  popNum in pops.keys() :
                for locusNum in loci.keys() :
                    if True in [pop_locus_valuesSet[popNum][locusNum].issubset(x) for x in [Set(["000"]),  Set(["000000"]), Set(["000","000000"]),\
                                                                          Set(["<[]>"]), Set(["<[][]>"]), Set(["<[]>", "<[][]>"]) ] ] :
                        raise NotGenepopFileError("Genepop file : %s\nLocus %s (name : %s, type : %s) has only missing data in pop %s : intractable configuration" %\
                                        (self.filename, locusNum+1, loci[locusNum]['name'], loci[locusNum]['type'],  popNum+1))


    def __testSeqLocusFormat(self, locus):
        return True

    def __read_genepop(self):
        try :
            self.__test_genepop_ng()
            self.__test_genepop()
            #self.__test_genepop_ng()
        except NotGenepopFileError as e :
            log(1, traceback.format_exc())
            raise e
        except :
            log(1, traceback.format_exc())
            raise Error("Test on genepop format failed. Please check your file format. If all seems normal, send the file or a bug report to support")

        f=open(str(self.filename).encode(self.fsCoding),'r')
        read_data = f.read().strip()
        lines = read_data.splitlines()
        f.close()
        self.title = lines[0]
        nl = 1
        line = lines[nl].upper()
        iloc=-1
        self.nloc_mic = 0
        self.nloc_seq = 0
# loop to read locus names and parentality
        while (line.find('POP')<0) and (nl<len(lines)) :
            iloc+=1
            tp = self.__locus_type_and_ploidy(lines,iloc)
            c1,c2=line.find('<'),line.find('>')
            if (c1>-1) and (c2>-1) :
                t=line[c1+1]
                tt=tp+t
                itt=0
                while (tt!=LOCUS_TYPE[itt]) : itt+=1
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
        #self.indivname.append([])
        self.genotype = []
        nindtot = 0
# loop to read genotypes
        while nl<len(lines) :
            line = lines[nl].upper()
            if (line.find('POP')>-1) and(line.find(',')<1):
                self.nsample +=1
                self.nind.append(0)
                self.indivname.append([])
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
                if len(self.genotype)==1 :
                    for ig,g in enumerate(self.genotype[0]) :
                        if self.locuslist[ig].type==0 :
                            try:
                                a=int(g)
                            except:
                                self.locuslist[ig].type=5

                #print self.genotype[-1]
            nl +=1


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
                        if seqlen==-1 :
                            seqlen = len(g)
                            loc.dnalength = seqlen
                        if len(g) != seqlen :
                            mes="At locus %s, individual %s has a sequence length different from previous sequences at the same locus (%s /= %s)"%(self.locuslist[iloc].name,self.indivname[sample][ind],len(g),seqlen)
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
        for n in self.nind :
            self.nindtot += n

# reading genotypes
        for iloc,loc in enumerate(self.locuslist):
            if loc.type<5 :  self.__domicrosat(loc,iloc)
            if loc.type>4 :  self.__dosequence(loc,iloc)
        self.nloc = self.nloc_seq + self.nloc_mic

class DataSnp():
    __LOCUS_TYPES = set(['A','X','Y','M','H','a','x','y','m','h'])
    __LOCUS_VALUES = set(['0','1','2','9'])
    __IND_SEX_TYPES = set(['9','M','F','m','f'])
    def __init__(self,filename):
        self.filename = filename
        self.fsCoding = getFsEncoding(logLevel=False)
        self.nloc = 0
        self.nloctot = 0
        self.nindtot = 0
        self.nsample = 0
        self.ntypeloc = {}
        # taille des populations
        self.nind = []
        self.nind_hash = {}
        self.commentValuesDict = {'maf' : 'HUDSON'}
                                  #'nm'  : "1.0nf"}
        self.__readData()


    def __readData(self):
        """
        Read SNP data file and infos from it

        code is an ugly mix between ugly legacy code and ugly new code. Enjoy !
        """
        name = self.filename
        nLocFam = 0
        # list ofsexual  locus index
        sexLocus = []
        locus_type = defaultdict()
        pat = re.compile(r'\s+')
        f=open(str(name).encode(self.fsCoding),'r')
        data = f.read().strip()
        f.close()
        datalines = data.split('\n')

        l1 = datalines[0].strip()
        l1compressed = pat.sub(' ',l1)
        l1parts = l1compressed.split(' ')
        prem = 0
        if not (len(l1parts) > 3 and l1parts[0] == "IND" and (l1parts[1].lower() == "sex") and l1parts[2] == "POP"):
            self.__parseCommentWords(l1parts)
            l1 = datalines[1].strip()
            l1compressed = pat.sub(' ',l1)
            l1parts = l1compressed.split(' ')
            prem = 1
            if not (len(l1parts) > 3 and l1parts[0] == "IND" and (l1parts[1].lower() == "sex") and l1parts[2] == "POP"):
                raise Exception("%s\n\nWrong description line format."% (self.filename))
        if len(datalines) < 1:
            raise Exception("%s\n\nNot enough lines in SNP datafile."%self.filename)


        #test NM values
        errorStrNMformat = "NM value bad format :\n NM type format is like : <(NM keyword)=(float number)(NF keyword)>  with no spaces\nNM must be > or = to NF\nFor example :\n   <NM=1.0NF> (for equal sex ratio)\n   <NM=0.3NF>\n"
        if not self.commentValuesDict.has_key('nm') :
            raise Exception("Please specify sex ratio in the headline\n\n"+errorStrNMformat)
        if self.commentValuesDict['nm'][-2:] != "nf" :
                raise Exception("Can not find NF keyword\n\n"+errorStrNMformat)
        try :
            if float(self.commentValuesDict['nm'][:-2]) > 1 :
                raise Exception("Foun\n\n"+errorStrNMformat)
        except ValueError:
            raise Exception("Can not detect a float before NF keyword\n\n"+errorStrNMformat)
        for idx, locty in enumerate(l1parts):
            if idx > 2 :
                if locty not in DataSnp.__LOCUS_TYPES :
                    raise Exception("%s\n\nFound an unknown locus type '%s' at locus %s. Should be one of : %s." % (self.filename,locty, str(idx-2), ", ".join(DataSnp.__LOCUS_TYPES)))
                if self.ntypeloc.has_key(locty):
                    self.ntypeloc[locty] += 1
                else:
                    self.ntypeloc[locty] = 1
                locus_type[idx-3] = locty
                if locty in ['X','Y','x','y'] :
                    sexLocus.append(idx)


        nbX = 0
        nbY = 0
        if self.ntypeloc.has_key('X'):
            nbX = self.ntypeloc['X']
        if self.ntypeloc.has_key('Y'):
            nbY = self.ntypeloc['Y']

        sexPresent = (nbX > 0 or nbY > 0)

        nbInd = len(datalines) - (prem+1)
        l0 = pat.sub(' ',datalines[prem].strip())
        nbLoci = len(l0.split(' ')) - 3

        #-----------------------------

        types = set()
        types_list = []
        nbU = 0
        def nestedDefaultdictAndSet():
            def nested2() :
                return defaultdict(set)

            return defaultdict(nested2)

        pop_locus_valuesSet = nestedDefaultdictAndSet()
        locus_valuesSet = defaultdict(set)
        locus_values = defaultdict(list)
        indivSex = []

        for i in range(len(datalines))[prem+1:]:
            line = pat.sub(' ',datalines[i].strip())
            lineElements = line.split(' ')
            # sex test
            if lineElements[1] not in DataSnp.__IND_SEX_TYPES :
                raise Exception('%s\n\nWrong sex type found : %s at line %s for individual %s' \
                                % (self.filename,lineElements[1],str(i+prem),lineElements[0]) )
            # loci number test
            if len(lineElements) != (nbLoci + 3):
                raise Exception("%s\n\nWrong number of loci at line %s for individual %s. It should have %s loci, got %s."\
                                %(self.filename,str(i+1),lineElements[0],nbLoci,str(len(lineElements)-3) ))
            # loci values test
            for idx, locusValue in enumerate(lineElements[3:]) :
                if locusValue not in DataSnp.__LOCUS_VALUES :
                    raise Exception("%s\n\nIndividual %s has an unknown value (line %s, locus number %s is set to %s=%s). Expected value is one of %s" \
                        % (self.filename,lineElements[0],str(i+prem),str(idx),l1parts[idx+3],locusValue, ", ".join(DataSnp.__LOCUS_VALUES) ))
                pop_locus_valuesSet[lineElements[2]][idx].add(locusValue)
                locus_valuesSet[idx].add(locusValue)
                locus_values[idx].append(locusValue)
            # values for sexual loci test if sex in not set
            indivSex.append(lineElements[1].lower())
            if lineElements[1].lower() not in ['f','m']:
                for index in sexLocus :
                    if lineElements[index] != '9' :
                        #raise Exception("Individual %s has unknown sex while some SNPs are on sex chromosomes"%line.split(' ')[0])
                        raise Exception("%s\n\nIndividual %s has a sex SNP locus (line %s, locus number %s is set to %s=%s but it should be set to '9') set while is sex is set to unknown (%s)" \
                                        % (self.filename,lineElements[0],str(i+prem),str(index-1),l1parts[index],lineElements[index],lineElements[1]))


            ctype = lineElements[2]
            types.add(ctype)

            # gestion du nind version list
            if ctype not in types_list:
                types_list.append(ctype)
                self.nind.append(1)
            else:
                index = types_list.index(ctype)
                self.nind[index] += 1

            # gestion du nind version hashtable
            if not self.nind_hash.has_key(ctype):
                self.nind_hash[ctype] = 1
            else:
                self.nind_hash[ctype] += 1





        for pop in pop_locus_valuesSet.keys() :
            for locus in  pop_locus_valuesSet[pop].keys() :
                if len(pop_locus_valuesSet[pop][locus]) == 1 and '9' in pop_locus_valuesSet[pop][locus] :
                    raise Exception("%s\n\nLocus %s in pop %s has only missing datas (value 9)). This not allowed. Please remove this locus from your data file." \
                        % (self.filename, locus+1, pop))
        nbLociPoly = 0
        for locus in locus_valuesSet.keys() :
            if not (True in [locus_valuesSet[locus].issubset(x) for x in [Set(['9','0']), Set(['9','2']), Set(['9']), Set(['2']), Set(['0'])]]):
                nbLociPoly += 1
            else :
                self.ntypeloc[locus_type[locus]] -= 1
        if nbLociPoly == 0 :
            raise Exception("%s\n\nFound only monomorphic loci !!!"%self.filename)

        if str(self.commentValuesDict['maf']).lower() != "hudson" :
            try :
                float(self.commentValuesDict['maf'])
            except :
                raise Exception("MAF value must be a float or the key word 'hudson', '%s' given." % self.commentValuesDict['maf'])
            if float(self.commentValuesDict['maf']) <= 0.0 or float(self.commentValuesDict['maf']) > 0.5 :
                raise Exception("MAF value must be upper to 0 and lower or egual to 0.5 , '%s' given." % self.commentValuesDict['maf'])
            rejectedLocusMAF = {}
            for locus in locus_type.keys() :
                n0 = 0
                n2 = 0
                for indiv in range(len(indivSex)) :
                    # 1 copy
                    if indivSex[indiv] == "m" and locus_type[locus] in ["X","Y","x","y"]:
                        if locus_values[locus][indiv] == "0" :
                            n0 += 1
                        elif locus_values[locus][indiv] == "1" :
                            n2 += 1
                        else :
                            pass
                    # 2 copies
                    else :
                        if locus_values[locus][indiv] == "0" :
                            n0 += 2
                        elif locus_values[locus][indiv] == "1" :
                            n0 +=1
                            n2 += 1
                        elif locus_values[locus][indiv] == "2" :
                            n2 += 2
                        else :
                            pass
                fam = 0
                if (n0+n2) > 1 :
                    fam = 0
                    #from fractions import Fraction
                    if n0 <= n2 :
                        fam = float(n0)/float(n0+n2)
                    else :
                        fam = float(n2)/float(n0+n2)
                    if fam >= float(self.commentValuesDict['maf']) :
                        nLocFam +=1
                    else :
                        #locus rejected
                        rejectedLocusMAF[locus] = fam
                        self.ntypeloc[locus_type[locus]] = self.ntypeloc[locus_type[locus]] - 1
            rejectedLocusText = "Those locuses have been rejected because there MAF are below the limit (%s) :\n%s\n"%\
                                (self.commentValuesDict['maf'],",".join([str(x) for x in sorted(rejectedLocusMAF.keys())]))
            log(1,rejectedLocusText)
            nbLociPoly = nLocFam

        nbSample = len(types)
        self.nindtot = nbInd
        self.nloc = nbLociPoly
        self.nloctot = nbLoci
        self.nsample = nbSample



    def __parseCommentWords(self,words):
        for word in words :
            if word[0] == '<' and "=" in word and word[-1] == '>' :
                valName, val = word.split("=")
                self.commentValuesDict[valName[1:].lower()] = val[:-1].lower()


def isSNPDatafile(name):
    if os.path.exists(str(name).encode(getFsEncoding(logLevel=False))):
        f=open(str(name).encode(getFsEncoding(logLevel=False)),'rU')
        lines=f.readlines()
        f.close()
        if len(lines) > 0:
            l1 = lines[0].strip()
            pat = re.compile(r'\s+')
            l1compressed = pat.sub(' ',l1)
            l1parts = l1compressed.split(' ')
            if len(l1parts) > 3 and l1parts[0] == "IND" and (l1parts[1].lower() == "sex") and l1parts[2] == "POP":
                return True
            l1 = lines[1].strip()
            pat = re.compile(r'\s+')
            l1compressed = pat.sub(' ',l1)
            l1parts = l1compressed.split(' ')
            if len(l1parts) > 3 and l1parts[0] == "IND" and (l1parts[1].lower() == "sex") and l1parts[2] == "POP":
                return True
    return False


if __name__ == "__main__":
    testMsatSeqsFiles = [
                 #"/media/psf/Home/VMshare/louiT1_2013_5_2-1/loui10_new_ghostnat.dat",
                 #"/media/psf/Home/VMshare/example_1_microsat_data_one_pop_with_bottleneck_multisamples_2013_7_19-1/simu_dataset_microsat_one_pop_bottleneck_multisamples_001.mss",
                 #"/media/psf/Home/VMshare/example_2_microsat_sequence_data_complexe_scenarios_ghost_pop_project_2013_7_9-1/toytest2_micro_seq_complexe_001.mss",
                 #"/tmp//toytest2_micro_seq_complexe_001.mss",

                 ]
    testSNPFiles = [
                "/home/dehneg/P16.snp",
                 #"/media/psf/Home/VMshare/example_3_SNP_data_divergence_and_admixture_project_2013_7_9-1/simu_dataset_test_divergence_admixture_001.snp",
                 ]

    for f in testMsatSeqsFiles :
        print "\n======================="
        print "Start loading %s" % f
        plop = Data(f)
        plop.loadfromfile()
        print "file %s loaded" % f
        print "=======================\n"

    for f in testSNPFiles :
        print "\n======================="
        print "Start loading %s" % f
        plop = DataSnp(f)
        print "isSNPDatafile  = " , isSNPDatafile(f)
        print "nloctot = ", plop.nloctot, ",  nlcc = poly = ", plop.nloc, ", ntypeloc  =   ", plop.ntypeloc
        print "file %s loaded" % f
        print "=======================\n"




