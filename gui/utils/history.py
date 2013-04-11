# -*- coding: utf-8 -*-
'''
Created on 8 oct. 2009

Set of classes usefull in historical model definition of DIYABC

@author: cornuet
'''
import copy
import param
from param import isaninteger, isafloat
import re

class Error(Exception):
    """Base class for exceptions in this module"""
    pass

class IOScreenError(Error):
    """ Error editing texts"""  
    pass

class Event(object):

    EVENT_TYPE    = ('SAMPLE','REFSAMPLE','VARNE', 'MERGE', 'SPLIT', 'SEXUAL')

    
    def __init__(self, action=None, pop=None, pop1=None, pop2=None, sample=None, Ne=None, N=None, time=None, graphtime =None, admixrate=None, numevent0=None,
                 sNe=None, stime=None, sadmixrate=None,xg=None,xc=None,xd=None,y=None, nindMref=0, nindFref=0):
        self.action     = action
        self.pop        = pop
        self.pop1       = pop1
        self.pop2       = pop2
        self.sample     = sample
        self.Ne         = Ne
        self.time       = time
        self.graphtime  = graphtime
        self.admixrate  = admixrate
        self.numevent0  = numevent0
        self.sNe        = sNe
        self.stime      = stime
        self.sadmixrate = sadmixrate
        self.xg         = xg
        self.xc         = xc
        self.xd         = xd
        self.y          = y      
        self.nindMref   = nindMref
        self.nindFref   = nindFref
        
    def __repr__(self):
        return "Event (time=%s, graphtime=%s, stime=%s, action=%s, pop=%s, pop1=%s, pop2=%s, sample=%s, Ne=%s, sNe=%s, admixrate=%s, sadmixrate=%s, numevent0=%s, xg=%s, xc=%s, xd=%s, y=%s" % (self.time,self.graphtime,self.stime,self.action,self.pop,self.pop1,self.pop2,self.sample,self.Ne,self.sNe,self.admixrate,self.sadmixrate,self.numevent0,self.xg,self.xc,self.xd,self.y)
    def __gt__(self,other):
        return self.time>other.time
    
    def __lt__(self,other):        
        return self.time<other.time
    
    def __eq__(self,other):
        return self.time==other.time
    
    def __ne__(self,other):
        return self.time != other.time
    
class History(object):
    def __init__(self,events=None,ne0s=None,time0=None,time1=None):
        if events == None : self.events = []
        else              : self.events = events
        if ne0s == None   : self.ne0s = []
        else              : self.ne0s   = ne0s
        self.time0 = time0
        self.time1 = time1
        self.NN = []
        
    def sortEvents(self):
        self.events.sort()
        n=len(self.events)
        for i in range(0,n-1) :
            for j in range(i+1,n):
                if self.events[i].numevent0>self.events[j].numevent0 :
                    a=self.events[i]
                    self.events[i]=self.events[j]
                    self.events[j]=a
                    
class SequenceBit(object):
    def __init__(self, action=None, N=None, pop=None, pop1=None, pop2=None, sample=None, admixrate=None, t0=None, t1=None):                
        self.action     = action
        self.pop        = pop
        self.pop1       = pop1
        self.pop2       = pop2
        self.sample     = sample
        self.N          = N
        self.t0         = t0
        self.t1         = t1
        self.admixrate  = admixrate

    def __repr__(self):
        return "SequenceBit (action=%s, pop=%s, pop1=%s, pop2=%s, sample=%s,N=%s, t0=%s, t1=%s, admixrate=%s" % (self.action,self.pop,self.pop1,self.pop2,self.sample,self.N,self.t0,self.t1,self.admixrate)

    def __gt__(self,other):
        if self.t0 == other.t0 :
            if (other.action=="ADSAMP")and(self.action!="ADSAMP"): return True
            elif (self.action=="ADSAMP")and(other.action!="ADSAMP"): return False
        return self.t0>other.t0
    
    def __lt__(self,other):        
        if self.t0 == other.t0 :
            if (other.action=="ADSAMP")and(self.action!="ADSAMP"): return False
            elif (self.action=="ADSAMP")and(other.action!="ADSAMP"): return True
        return self.t0<other.t0
    
    def __eq__(self,other):
        if self.t0 == other.t0 :
            if (other.action=="ADSAMP")and(self.action!="ADSAMP"): return False
            elif (self.action=="ADSAMP")and(other.action!="ADSAMP"): return False
            else : return True
        return False
    
    def __ne__(self,other):
        if self.t0 == other.t0 :
            if (other.action=="ADSAMP")and(self.action!="ADSAMP"): return False
            elif (self.action=="ADSAMP")and(other.action!="ADSAMP"): return False
        return True
        
class Sequence(list) :
    def append(self,value):
        if isinstance(value,SequenceBit) :
            super(Sequence,self).append(value)
        else :
            raise ValueError , "This is not a SequenceBit"
       
    
    
class Scenario(object):
    def __init__(self,parameters=None,history=None,refhistory=None, number=None, time_sample=None, 
					prior_proba=None, nparamtot=None, popmax=None, npop=None, nsamp=None, nrefsamp=None):
        if parameters == None :  self.parameters = []
        else                  :  self.parameters = parameters
        self.history     = history
        self.refhistory  = refhistory
        self.number      = number
        self.prior_proba = prior_proba
        self.nparamtot   = nparamtot
        self.popmax      = popmax
        self.npop        = npop
        self.nsamp       = nsamp
        self.nrefsamp    = nrefsamp
        self.parametersbackup = []
        self.dicoPopRefNindRef = {}
        if time_sample == None : self.time_sample =[]
        else                   : self.time_sample= time_sample
        
          
    def setgraphtime(self):
        for i in range(len(self.history.events)-1) :
            for j in range(i+1,len(self.history.events)) :
                iev,jev = self.history.events[i],self.history.events[j]
                if (iev.time != None)and(jev.time != None) :
                    if iev.time>jev.time :
                        self.history.events[i],self.history.events[j] = jev,iev
                        
        if self.history.events[0].time == None  : self.history.time0,self.history.events[0].graphtime = 0,0
        else                                    : self.history.time0,self.history.events[0].graphtime = self.history.events[0].time,self.history.events[0].time
        self.history.time1 = self.history.time0
        for iev,ev in enumerate(self.history.events) :
            if iev>0 :
                if ev.time != None :
                    if self.history.events[iev-1].time != None :
                        if self.history.events[iev-1].time != ev.time : ev.graphtime = self.history.events[iev-1].graphtime+1
                        else                                          : ev.graphtime = self.history.events[iev-1].graphtime
                    else :
                        trouve = False
                        k = -1
                        while (not trouve)and (k<iev-1) :
                            k +=1
                            kev = self.history.events[k]
                            trouve = (kev.time == None)and(kev.stime == ev.stime)
                        if trouve : ev.graphtime = self.history.events[k].graphtime
                        else      : ev.graphtime = self.history.events[iev-1].graphtime+1
                else :
                    trouve = False
                    k = -1
                    while (not trouve)and (k<iev-1) :
                        k +=1
                        kev = self.history.events[k]
                        trouve = (kev.time == None)and(kev.stime == ev.stime)
                    if trouve : ev.graphtime = self.history.events[k].graphtime
                    else      : ev.graphtime = self.history.events[iev-1].graphtime+1
            if ev.graphtime>self.history.time1 : self.history.time1=ev.graphtime
                    
         
    def detparam(self,s,category):
        s1 = s
        ss = []
        while len(s1)>0 :
            plus = s1.find("+")
            minus= s1.find("-")
            if (plus == -1)and(minus == -1) :
                ss.append(s1)
                s1 = ""
            else :
                if plus>=0 : posigne = plus;
                else      : posigne = minus;
                ss.append(s1[:posigne])
                s1 = s1[posigne+1:]
        for j in range(len(ss)) :
			if not isaninteger(ss[j]):
				pr = param.HistParameter()
				pr.category,pr.prior = category, None
				pr.name, pr.value = ss[j], None
				if len(self.parameters) >0 :
					trouve = False
					i = -1
					while (not trouve) and (i<len(self.parameters)-1) :
						i +=1
						trouve = pr.name == self.parameters[i].name
						if trouve and (self.parameters[i].category != category):
							raise IOScreenError, "%s is used as a %s parameter and %s parameter"%(pr.name,param.HistParameter.categorydict[category],param.HistParameter.categorydict[self.parameters[i].category] )
					if not trouve : self.parameters.append(pr)
				else :
					self.parameters.append(pr)
                     

    
    def checkread(self,textarray,data=None):
        if len(self.parameters)>0 :
            self.parametersbackup = copy.deepcopy(self.parameters) 
            self.parameters = []
        else : self.parametersbackup = []
        ligne0 = textarray[0]   #take the first line of the scenario 
        ligne  =ligne0.upper()  #put it in uppercase
        if ligne.find('SAMPLE')+ligne.find('REFSAMPLE')+ligne.find('MERGE')+ligne.find('VARNE')+ligne.find('SPLIT')+ligne.find('SEXUAL')>-4: 
            raise IOScreenError, "The first line must provide effective population sizes"
        ls = ligne0.split()
        self.npop = len(ls)
        self.history = History()
        Ncur=[]
        for l in ls :
            NN = Ne0()
            NNc = Ne0()
            if isaninteger(l): NN.name, NN.val, NNc.name, NNc.val = str(l),int(l),str(l),int(l)
            else                          : NN.name, NN.val, NNc.name, NNc.val = l, None, l, None
            self.history.ne0s.append(NN)
            Ncur.append(NNc)
            if NN.val == None : self.detparam(NN.name,"N")   
        self.nsamp = 0
        self.nrefsamp = 0
        nevent = 0
        pat = re.compile(r'\s+')
        for i,li_n_c in enumerate(textarray[1:]):
            li = pat.sub(' ',li_n_c)
            if li.upper().find('SAMPLE')+li.upper().find('REFSAMPLE') > -1 : 
                self.nsamp+=1
                if li.upper().find('REFSAMPLE') > -1 :
                    self.dicoPopRefNindRef[int(li.strip().split(' ')[-3])] = int(li.strip().split(' ')[-1]) + int(li.strip().split(' ')[-2])
                    self.nrefsamp+=1
                if (len(li.strip().split(' ')) < 3)or(len(li.strip().split(' ')) == 4):
                    raise IOScreenError, "At line %i, the number of words is incorrect"%(i+2)
                # verif que le nombre apres "sample" est bien inférieur ou egal au nombre de pop (len(Ncur))
                if (li.upper().find(' SAMPLE')>-1) and (int(li.strip().split(' ')[2]) > len(Ncur)): 
                    raise IOScreenError, "At line %i, the population number (%s) is higher than the number of population (%s) defined at line 1"%((i+2),li.split(' ')[-1],len(Ncur))
                
                if (li.upper().find('REFSAMPLE')>-1) and (int(li.strip().split(' ')[2]) > len(Ncur)):
                    raise IOScreenError, "At line %i, the population number (%s) is higher than the number of population (%s) defined at line 1"%((i+2),li.split(' ')[-1],len(Ncur))
        if self.nsamp<1 :
            raise IOScreenError, "You must indicate when samples are taken"
        if data!=None:
            #print self.nsamp,"========", data.nsample
            if self.nsamp != data.nsample+self.nrefsamp :
                raise IOScreenError, "The number of samples in scenario %s does not match the data file"%(self.number)
        if len(textarray)>1 :
            for ili0 in range(0,len(textarray)-1) :
                for ili1 in range(ili0+1,len(textarray)) :
                    li0tem = textarray[ili0].split()
                    li1tem = textarray[ili1].split()
                    idem = False  
                    if len(li0tem) == len(li1tem) :
                        idem = True
                        j = 0
                        while idem and (j<len(li0tem)) :
                            idem = (li0tem[j] == li1tem[j])
                            j +=1
                    if idem :
                        raise IOScreenError,"Lines %s and %s of scenario %s are identical"%(ili0+1,ili1+1,self.number)                 
            j,ns = 0, 0
            for jli0 in range(1,len(textarray)):
                if len(textarray[jli0])>0:
                    j +=1
                    li = textarray[jli0]
                    Li = li.upper()
                    litem = li.split()
                    nitems = len(litem)
                    
                    if Li.find(" SAMPLE")+ Li.find("REFSAMPLE")>-1:
                        if (nitems<3)or(nitems==4):
                            raise IOScreenError, "Line %s of scenario %s is incomplete"%(jli0+1,self.number)
                        self.cevent = Event()
                        self.cevent.numevent0 = nevent
                        nevent +=1
                        if Li.find(" SAMPLE")>-1 : 
							self.cevent.action = "SAMPLE"
                        if Li.find("REFSAMPLE")>-1 : 
							self.cevent.action = "REFSAMPLE"
                        self.cevent.stime = litem[0]
                        if isaninteger(litem[0]) : self.cevent.time = int(litem[0])
                        else : self.detparam(litem[0],"T")
                        if isaninteger(litem[2]):
                            self.cevent.pop = int(litem[2]) 
                        else :
                            raise IOScreenError, "Unable to read population number on line %s of scenario %s"%(jli0+1,self.number)
                        self.cevent.Ne=Ncur[self.cevent.pop-1].val
                        self.cevent.sNe=Ncur[self.cevent.pop-1].name
                        ns +=1
                        self.cevent.sample = ns
                        self.time_sample.append(self.cevent.time)
                        if (nitems==5):
							if isaninteger(litem[3]):
								self.cevent.nindMref = int(litem[3])
							else :
								raise IOScreenError, "Unable to read number of males on line %s of scenario %s"%(jli0+1,self.number)
							if isaninteger(litem[4]):
								self.cevent.nindFref = int(litem[4])
							else :
								raise IOScreenError, "Unable to read number of females on line %s of scenario %s"%(jli0+1,self.number)
							#print self.cevent.nindMref,"   ",self.cevent.nindFref
							#print "                 ",data.nind[0]
							if Li.find(" SAMPLE") != -1 and (data!=None):
								if self.cevent.nindMref+self.cevent.nindFref > data.nind[self.cevent.pop-1] :
									raise IOScreenError, "The total number of reference individuals (%s) is larger than that of the data set (%s) on line %s of scenario %s"%(self.cevent.nindMref+self.cevent.nindFref,data.nind[self.cevent.pop-1],jli0+1,self.number)
									
								
                    elif Li.find("VARNE")>-1:
                        if nitems<4:
                            raise IOScreenError, "Line %s of scenario %s is incomplete"%(jli0+1,self.number)
                        self.cevent = Event()
                        self.cevent.numevent0 = nevent
                        nevent +=1
                        self.cevent.action = "VARNE"
                        self.cevent.stime = litem[0]
                        if isaninteger(litem[0]) : self.cevent.time = int(litem[0])
                        else : self.detparam(litem[0],"T")
                        if isaninteger(litem[2]):
                            self.cevent.pop = int(litem[2]) 
                        else :
                            raise IOScreenError, "Unable to read population number on line %s of scenario %s"%(jli0+1,self.number)
                        if isaninteger(litem[3]):
                            self.cevent.Ne = int(litem[3]);self.cevent.sNe = None
                        else :
                            self.cevent.sNe = litem[3];self.cevent.Ne = None
                            self.detparam(litem[3],"N")
                        Ncur[self.cevent.pop-1].val=self.cevent.Ne
                        Ncur[self.cevent.pop-1].name=self.cevent.sNe
                    elif Li.find("MERGE")>-1:
                        if nitems<4:
                            raise IOScreenError, "Line %s of scenario %s is incomplete"%(jli0+1,self.number)
                        if nitems>4:
                            raise IOScreenError, "At line %i, the number of words is incorrect"%(i+2)
                        self.cevent = Event()
                        self.cevent.numevent0 = nevent
                        nevent +=1
                        self.cevent.action = "MERGE"
                        self.cevent.stime = litem[0]
                        if isaninteger(litem[0]) : self.cevent.time = int(litem[0])
                        else : self.detparam(litem[0],"T")
                        if isaninteger(litem[2]):
                            self.cevent.pop = int(litem[2]) 
                        else :
                            raise IOScreenError, "Unable to read the first population number on line %s of scenario %s"%(jli0+1,self.number)
                        if isaninteger(litem[3]):
                            self.cevent.pop1 = int(litem[3]) 
                        else :
                            raise IOScreenError, "Unable to read the second population number on line %s of scenario %s"%(jli0+1,self.number)
                        self.cevent.Ne=Ncur[self.cevent.pop-1].val
                        self.cevent.sNe=Ncur[self.cevent.pop-1].name
                    elif Li.find("SPLIT")>-1:
                        if nitems<6:
                            raise IOScreenError, "Line %s of scenario %s is incomplete"%(jli0+1,self.number)
                        self.cevent = Event()
                        self.cevent.numevent0 = nevent
                        nevent +=1
                        self.cevent.action = "SPLIT"
                        self.cevent.stime = litem[0]
                        if isaninteger(litem[0]) : self.cevent.time = int(litem[0])
                        else : self.detparam(litem[0],"T")
                        if isaninteger(litem[2]):
                            self.cevent.pop = int(litem[2]) 
                        else :
                            raise IOScreenError, "Unable to read the first population number on line %s of scenario %s"%(jli0+1,self.number)
                        if isaninteger(litem[3]):
                            self.cevent.pop1 = int(litem[3]) 
                        else :
                            raise IOScreenError, "Unable to read the second population number on line %s of scenario %s"%(jli0+1,self.number)
                        if isaninteger(litem[4]):
                            self.cevent.pop2 = int(litem[4]) 
                        else :
                            raise IOScreenError, "Unable to read the third population number on line %s of scenario %s"%(jli0+1,self.number)
                        if isafloat(litem[5]):
                            self.cevent.admixrate = float(litem[5]) 
                        else :
                            self.cevent.sadmixrate = litem[5]
                            self.detparam(litem[5],"A")    
                        self.cevent.Ne=Ncur[self.cevent.pop-1].val
                        self.cevent.sNe=Ncur[self.cevent.pop-1].name
                    elif Li.find("SEXUAL")>-1:
                        if nitems<3:
                            raise IOScreenError, "Line %s of scenario %s is incomplete"%(jli0+1,self.number)
                        self.cevent = Event()
                        self.cevent.numevent0 = nevent
                        nevent +=1
                        self.cevent.action = "SEXUAL"
                        self.cevent.stime = litem[0]
                        if isaninteger(litem[0]) : self.cevent.time = int(litem[0])
                        else : self.detparam(litem[0],"T")
                        if isaninteger(litem[2]):
                            self.cevent.pop = int(litem[2]) 
                        else :
                            raise IOScreenError, "Unable to read population number on line %s of scenario %s"%(jli0+1,self.number)
                        self.cevent.Ne=Ncur[self.cevent.pop-1].val
                        self.cevent.sNe=Ncur[self.cevent.pop-1].name
                        ns +=1
                        self.cevent.sample = ns
                        self.time_sample.append(self.cevent.time)
                    else :
                        raise IOScreenError,"Uninterpretable line %s in scenario %s"%(jli0+1,self.number)
                    self.history.events.append(self.cevent)
                    if self.cevent.pop>self.popmax : self.popmax=self.cevent.pop
            self.setgraphtime()
            if len(self.parametersbackup)>0 :
                for pb in self.parametersbackup :
                    trouve = False
                    i = -1
                    while (not trouve)and(i<len(self.parameters)-1) :
                        i +=1
                        p = self.parameters[i]
                        trouve = (pb.name == p.name) and (pb.category == p.category)
                    if trouve : self.parameters[i].value = pb.value
#                    self.smoothgraphtime()
            
    def checklogic(self):
        maxpop = 0
        for ev in self.history.events :
            if ev.pop>maxpop :  maxpop =ev.pop 
            if ev.pop1>maxpop :  maxpop =ev.pop1 
            if ev.pop2>maxpop :  maxpop =ev.pop2
        if self.npop != maxpop :
            raise IOScreenError, "In scenario %s, the number of population sizes in line 1 is different from the number of populations"%(self.number)
        popexist = []
        for j in range(0,maxpop+1) : popexist.append(True)
        for iev,ev in enumerate(self.history.events):
            if (ev.action == "SAMPLE") and (not popexist[ev.pop]) :
                raise IOScreenError, "In scenario %s line %s sample is taken from population %s which does not exist anymore"%(self.number,iev+1,ev.pop) 
            elif ev.action == "MERGE" :
                if ev.pop == ev.pop1 :
                    raise IOScreenError, "In scenario %s line %s, population %s merges with itself"%(self.number,iev+1,ev.pop)
                if popexist[ev.pop] and popexist[ev.pop1] :
                    popexist[ev.pop1] = False
                else :
                    if not popexist[ev.pop] :
                        raise IOScreenError, "In scenario %s line %s, population %s does not exist anymore"%(self.number,iev+1,ev.pop) 
                    if not popexist[ev.pop1] :
                        raise IOScreenError, "In scenario %s line %s, population %s does not exist anymore"%(self.number,iev+1,ev.pop1)
            elif ev.action == "SPLIT" :
                if (ev.pop == ev.pop1) or (ev.pop == ev.pop2) or (ev.pop1 == ev.pop2) :
                    raise IOScreenError, "In scenario %s line %s, admixture must occur between three different populations"%(self.number,iev+1)
                if popexist[ev.pop] and popexist[ev.pop1] and popexist[ev.pop2] :
                    popexist[ev.pop] = False
                else :
                    if not popexist[ev.pop] :
                        raise IOScreenError, "In scenario %s line %s, population %s does not exist anymore"%(self.number,iev+1,ev.pop) 
                    if not popexist[ev.pop1] :
                        raise IOScreenError, "In scenario %s line %s, population %s does not exist anymore"%(self.number,iev+1,ev.pop1)
                    if not popexist[ev.pop2] :
                        raise IOScreenError, "In scenario %s line %s, population %s does not exist anymore"%(self.number,iev+1,ev.pop2)
        np =0
        popexist[0] = False
        s =""
        for ip,p in enumerate(popexist) :
            if p :
                np +=1
                s = s + " %s"%(ip)
        if np>1 :
            raise IOScreenError, "In scenario %s, more than one population (%s) are ancestral"%(self.number,s)
        self.refhistory = copy.deepcopy(self.history)        
                                           
class Scenarios(list):
    def append(self,value):
        if isinstance(value,Scenario) :
            super(Scenarios,self).append(value)
        else :
            raise ValueError , "This is not a Scenario"
        
class Ne0(object):
    def __init__(self, val=None, name=None):
        self.val    = val
        self.name   = name
        
    def __str__ (self) :
        return "val=%s, name=%s" % (self.val,self.name)

    def __repr__ (self) :
        return "Ne0(val=%s, name='%s')" % (self.val,self.name)



    
    
def testCheckScenario():
    goodScenario = [
                     ['N N', '0 sample 1', '0 sample 2', "t1-db merge 1 2","t1 VarNe 1 Nb"],
                     ['N N N', '0 sample 1', '0 sample 2', '0 sample 3','6 merge 1 2','4 merge 2 3'],
                     ['N N N', '0 sample 1', '0 sample 2', '0 sample 3','+t1 merge 1 2','2 merge 2 3']
                   ]
    badScenario = [
                   ['N N N', '0 sample 1', '0 sample 2', "1 sample 3", "3 merge 1 2"],
                   ['N N N', '0 sample ', '0 sample 2', "1 sample 3", "3 merge 1 2"]
                   ]
    
    # tests good scenario
    print "Good scenarios"
    for it,t in enumerate(goodScenario) :
        print"Scenario ",it+1
        print t
        try :
            s = Scenario()
            s.number = it+1
            s.checkread(t)
            s.checklogic()
        except IOScreenError, e :
            print "Un bon scenario a une erreur : ", e
            print s.history.events
        for ev in s.history.events :
            print ev
        print " "
        for p in s.parameters : print p
        print " "
            
    # tests bad scenario
    print "Bad scenarios"
    for it,t in enumerate(badScenario) :
        print"Scenario ",it+1
        print t
        try :
            s = Scenario()
            s.number = it+1
            s.checkread(t)
            s.checklogic()
        except IOScreenError, e :
            print "ok erreur pour %s : %s" % (t,e)
        for ev in s.history.events :
            print ev
        print"   "
        for p in s.parameters : print p
        print " "
    
        
if __name__ == "__main__" :
    testCheckScenario()
    
