'''
Created on 24 sept. 2009

@author: cornuet
'''

from math import log as _log, exp as _exp

def isaninteger(s): 
    try: 
        a=int(s) 
        return True 
    except: 
        return False 
 
def isafloat(s): 
    try: 
        a=float(s) 
        return True 
    except: 
        return False 

class Particle(object):
    def __init__(self,scenario=None,histparam=None,locuslist=None,locusgrouplist=None,statlist=None,dnatrue=None):
        self.scenario  = scenario
        self.locuslist = locuslist
        self.locusgrouplist = locusgrouplist
        self.statlist  = statlist
        self.dnatrue   = dnatrue
     
    def param2val(self,param):
        param = param.strip()
#        for p in self.histparam :
        for p in self.scenario.parameters :
            if p.name == param :break
        return p.value
    
    def getvalue(self,line,category):
        if line == None : return None
        if (category != "A") and isaninteger(line) :return int(line)
        if (category == "A") and isafloat(line)    :return float(line)
        line = line.strip()
        posplus,posminus = line.find("+"),line.find("-")
        if (posplus<0)and(posminus<0) :     #no sign + or - : line = single parameter name
                return self.param2val(line)
        elif (posplus>=0)and(posminus<0) :  #one sign + and no sign -
            posign=posplus
        elif (posplus<0)and(posminus>=0) :  #one sign - and no sign +
            posign=posminus
        elif (posplus<posminus) :           #sign + encountered first
            posign=posplus
        elif (posplus>posminus) :           #sign - encountered first
            posign=posminus
        result = self.param2val(line[:posign])
        operator = line[posign]
        line = line[posign+1:]
        while len(line)>0 :
            posplus,posminus = line.find("+"),line.find("-")
            if (posplus<0)and(posminus<0) :  
                if operator=="+" : return result+self.param2val(line)
                if operator=="-" : return result-self.param2val(line)
            elif (posplus>=0)and(posminus<0) :  #one sign + and no sign -
                posign=posplus
            elif (posplus<0)and(posminus>=0) :  #one sign - and no sign +
                posign=posminus
            elif (posplus<posminus) :           #sign + encountered first
                posign=posplus
            elif (posplus>posminus) :           #sign - encountered first
                posign=posminus
            if operator=="+" : result += self.param2val(line[:posign])
            if operator=="-" : result -= self.param2val(line[:posign])
            operator = line[posign]
            line = line[posign+1:]
            
    def loadhistparam(self):
        for ev in self.scenario.history.events:
            ev.time      = self.getvalue(ev.stime,"T")
            ev.Ne        = self.getvalue(ev.sNe,"N")
            ev.admixrate = self.getvalue(ev.sadmixrate,"A")
        for ne in self.scenario.history.ne0s:
            ne.val       = self.getvalue(ne.name,"N")
        
    def loadlocparam(self):
        pass
            

class Prior(object):
    """each prior instance must belong to a single parameter"""
    def __init__(self, loi=None, mini=None, maxi=None, mean=None, sdshape=None, ndec=None, constant=None, type=None):
        self.loi      = loi
        self.mini     = mini
        self.maxi     = maxi
        self.mean     = mean
        self.sdshape  = sdshape
        self.ndec     = ndec
        self.constant = constant
        self.type     = type
        
    def __str__(self):
        if   self.loi == "uniforme"     : 
            ss = "UN["+str(self.mini)+","+str(self.maxi)+","+str(0)+","+str(0)+"]"
        elif self.loi == "loguniforme"  : 
            ss = "LU["+str(self.mini)+","+str(self.maxi)+","+str(0)+","+str(0)+"]"
        elif self.loi == "normale"      : 
            ss = "NO["+str(self.mini)+","+str(self.maxi)+","+str(self.mean)+","+str(self.sdshape)+"]"
        elif self.loi == "lognormale"   : 
            ss = "LN["+str(self.mini)+","+str(self.maxi)+","+str(self.mean)+","+str(self.sdshape)+"]"
        elif self.loi == "gamma"        : 
            ss = "GA["+str(self.mini)+","+str(self.maxi)+","+str(self.mean)+","+str(self.sdshape)+"]"
        else                            : 
            raise ValueError , "distribution "+self.loi+" not implemented"
        return ss
    
    def __repr__(self):
        return self.__str__()


#    def simul(self,mw):
#        self.mw = mw
#        if self.constant:
#            return self.min
#        if self.loi == 'UN':    #uniforme
#            return self.min + self.mw.rand()*(self.max - self.min)
#        elif self.loi == 'NO':    #normale
#            x = Normal(self.mw,self.moy,self.sd).rand()
#            while x<self.min or x>self.max : 
#                x = Normal(self.mw,self.moy,self.sd).rand()
#            return x
#        elif self.loi == 'LN':    #lognormale
#            x = Lognorm(self.mw,_log(self.moy),_log(self.sd)).rand()
#            while x<self.min or x>self.max : 
#                x = Lognorm(self.mw,_log(self.moy),_log(self.sd)).rand()
#            return x
#        elif self.loi == 'LU':    #loguniforme
#            x = Logunif(self.mw,_log(self.min),_log(self.max)).rand()
#            while x<self.min or x>self.max : 
#                x = Logunif(self.mw,_log(self.min),_log(self.max)).rand()
#            return x
#        elif self.loi == 'GA':    #gamma
#            x = Gamma3(self.mw,_self.moy,_self.shape).rand()
#            while x<self.min or x>self.max : 
#                x = Gamma3(self.mw,_self.moy,_self.shape).rand()
#            return x
    
       
class HistParameter(object):
    
    categorydict = {"N":"effective population size","T":"time","A":"admixture"}
    PARAM_TYPE = ("N","T","A")
    def __init__(self, name=None, sign=None, category=None, value=None, constant=None, prior=None):
        self.name     = name
        self.sign     = sign
        self.category = category  # ="N" for an effective size, "T" for an event time or "A" for an admixture rate
        self.value    = value
        self.constant = constant
        if prior == None : self.prior = Prior()
        else             : self.prior = prior
        
    def __repr__(self):
        return ("HistParameter(name='%s', category='%s', value=%s, constant=%s, prior=%s)" % (self.name,self.category,self.value,self.constant,self.prior))
        
class HistParameters(list):
    def append(self,value):
        if isinstance(value,HistParameter) :
            super(HistParameters,self).append(value)
        else :
            raise ValueError , "Value is not a from class HistParameter, %s class given" % value.__class__.__name__

class SumStat(object):
    
    def __init__(self,category=None,sample=None,sample1=None,sample2=None,nloc=None,loc=None):
        self.category  = category
        self.sample    = sample
        self.sample1   = sample1
        self.sample2   = sample2
        self.nloc      = nloc
        self.loc       = loc    #list of concerned loci        
    def __repr__(self):
        return ("SumStat(category='%s', sample=%s, sample1=%s, sample2=%s, nloc=%s)" % (self.category,self.sample,self.sample1,self.sample2,self.nloc))
        
class SumStats(list):
    def append(self,value):
        if isinstance(value,SumStat) :
            super(SumStats,self).append(value)
        else :
            raise ValueError , "Value is not a from class SumStat, %s class given" % value.__class__.__name__
        
        
        
        
        
        
        
        
        
                 
