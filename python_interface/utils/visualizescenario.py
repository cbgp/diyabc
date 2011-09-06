# -*- coding: utf-8 -*-
'''
Created on 10 juin 2010

@author: cornuet
'''

from __future__ import division
import history
import random


class Error(Exception):
    """Base class for exceptions in this module"""
    pass

class PopTreeError(Error):
    """ Error drawing pop tree"""  
    pass

class PopTreeBranch(object):
    def __init__(self,father=None,child=None,nvisit=None,sadm=None,style=""):
        self.father = father
        self.child  = child
        self.nvisit = nvisit
        self.sadm   = sadm
        self.style  = style
        
    def __repr__(self):
        return "PopTreeBranch(father=%s, child=%s, nvisit=%s, sadm=%s, style=%s)"%(self.father, self.child, self.nvisit, self.sadm, self.style) 
    
class PopTreeNode(object):
    def __init__(self,x=0,y=None,nvisit=None,pop=None,ev=None,NP=None,category=None,style=""):
        self.x         = x
        self.y         = y
        self.nvisit    = nvisit
        self.pop       = pop
        self.ev        = ev
        self.NP        = NP
        self.category  = category
        self.style  = style
        
    def __repr__(self):
        return "PopTreeNode(x=%s, y=%s, nvisit=%s, pop=%s, ev=%s, NP=%s, category=%s, style=%s)"%(self.x, self.y, self.nvisit, self.pop, self.ev, self.NP, self.category, self.style) 
       
class PopTreeSegment(object):    
    def __init__(self,xdeb=None,ydeb=None,xfin=None,yfin=None,popdeb=None,popfin=None,evdeb=None,evfin=None,sNedeb=None,sNefin=None,sadm=None,style=""):
        self.xdeb    = xdeb
        self.ydeb    = ydeb
        self.xfin    = xfin
        self.yfin    = yfin
        self.popdeb  = popdeb
        self.popfin  = popfin
        self.evdeb   = evdeb
        self.evfin   = evfin
        self.sNedeb  = sNedeb
        self.sNefin  = sNefin
        self.sadm    = sadm
        self.style   = style

    def __repr__(self):
        return "PopTreeSegment(xdeb=%s, ydeb=%s, xfin=%s, yfin=%s, popdeb=%s, popfin=%s, evdeb=%s, evfin=%s, sNedeb=%s, sNefin=%s, sadm=%s, style=%s)"%(self.xdeb, self.ydeb, self.xfin, self.yfin, self.popdeb, self.popfin, self.evdeb, self.evfin, self.sNedeb, self.sNefin, self.sadm, self.style) 

class PopTreePopu(object):
    def __init__(self,X=None,Y=None,sampled=None,time=None,sNe=None,col=None):
        self.X       = X
        self.Y       = Y
        self.sampled = sampled
        self.time    = time
        self.sNe     = sNe
        self.col     = col

    def __repr__(self):
        return "PopTreePopu(X=%s, Y=%s, sampled=%s, time=%s, sNe=%s, col=%s)"%(self.X, self.Y, self.sampled, self.time, self.sNe, self.col) 



class PopTree(object):
    XMAX=500
    YMAX=380
    YBAS=370
    YHAUT=25
    def __init__(self,scenario):
        if not isinstance(scenario, history.Scenario) :
            raise ValueError, "Bad type for scenario. history.Scenario expected, %s type given" % scenario.__class__.__name__
        self.scenario = scenario
        self.NN       = [] #liste des noms d'effective sample sizes (sans doublon) utilises dans le scenario
        self.NM       = [] #liste des noms d'effective sample sizes (doublons possibles) des populations (=1ere ligne)
        self.br       = [] #liste des branches
        self.node     = [] #liste des noeuds
        self.deltaY   = 100
        self.nanc     = 0
        self.segments = []    
    
    def initNN(self):
        for nn in self.scenario.history.ne0s :
            if nn.name<>None : 
                self.NM.append(nn.name)
                self.NN.append(nn.name) 
            else             : 
                self.NM.append(str(nn.val))
                self.NN.append(str(nn.val))
        for ev in self.scenario.history.events :
            if ev.action == "VARNE" :
                if ev.Ne == None :self.NN.append(ev.sNe)
                if ev.sNe == None : self.NN.append(str(ev.Ne))
        print "avant"
        print self.NN
        self.NN = list(set(self.NN))
        print "apres"
        print self.NN
        self.scenario.history.NN = self.NN
                
    def reset_tree(self):
        self.br = []
        self.node = []
        
    def echangebr(self,i,j):
        bb = PopTreeBranch()
        bb.father, bb.child, bb.nvisit = self.br[i].father, self.br[i].child, self.br[i].nvisit
        self.br[i].father, self.br[i].child, self.br[i].nvisit = self.br[j].father, self.br[j].child, self.br[j].nvisit
        self.br[j].father, self.br[j].child, self.br[j].nvisit = bb.father, bb.child, bb.nvisit
    
    
    def cree_node(self,ev,iev,numpop,t0):
        cnode = PopTreeNode()
        cnode.ev = iev
        if   numpop == 0 :cnode.pop = ev.pop
        elif numpop == 1 :cnode.pop = ev.pop1
        elif numpop == 2 :cnode.pop = ev.pop2
        if   ev.action == "SAMPLE" :cnode.category = "sa"
        elif ev.action == "VARNE"  :cnode.category = "vn"
        elif ev.action == "MERGE"  :cnode.category = "me"
        elif ev.action == "SEXUAL" :cnode.category = "se"
        elif ev.action == "SPLIT"  :
            if numpop == 0 :cnode.category = "sp0"
            else           :cnode.category = "sp1"
        cnode.y = (PopTree.YBAS-25) - (ev.graphtime-t0)*self.deltaY
        return cnode
    
    def cree_branch(self,ev,numpop):
        if   numpop == 0 : pop = ev.pop
        elif numpop == 1 : pop = ev.pop1
        elif numpop == 2 : pop = ev.pop2
        i = len(self.node)-1
        done = False
        while (i>0) and (not done):
            i -=1
            if self.node[i].pop == pop :
                cbr = PopTreeBranch()
                cbr.father, cbr.child, cbr.nvisit = (len(self.node)-1), i, 0
                done = True
                self.br.append(cbr)
        return done
    
    def build_tree(self):
        t0, t1 = self.scenario.history.time0, self.scenario.history.time1
        if t1 != t0 : self.deltaY = (PopTree.YBAS-PopTree.YHAUT-75)//(t1-t0)
        else        : self.deltaY = 100
        self.reset_tree()
        for iev,ev in enumerate(self.scenario.history.events):
            if ev.action == "SAMPLE" :
                cnode = self.cree_node(ev,iev,0,t0)
                deja = False
                if iev>0 :
                    jev = iev-1
                    while (jev >= 0)and (not deja) : 
                        deja = (ev.pop == self.scenario.history.events[jev].pop)
                        jev -=1
                    if deja :
                        cnode.category = "sa2"
                        cnode.NP = ev.sNe
                        if cnode.NP == None : cnode.NP = self.NM[ev.pop-1]
                if not deja : cnode.NP = self.NM[cnode.pop-1]
                if ev.sNe == None : ev.sNe = cnode.NP
                ev.y = cnode.y
                self.node.append(cnode)
                self.cree_branch(ev,0)
                
            if ev.action == "VARNE" :
                cnode = self.cree_node(ev,iev,0,t0)
                cnode.NP = ev.sNe
                ev.y = cnode.y
                self.node.append(cnode)
                self.cree_branch(ev,0)

            if ev.action == "SEXUAL" :
                cnode = self.cree_node(ev,iev,0,t0)
                cnode.NP = ev.sNe
                cnode.style = "dot"
                ev.y = cnode.y
                self.node.append(cnode)
                self.cree_branch(ev,0)

            if ev.action == "MERGE" :
                cnode = self.cree_node(ev,iev,0,t0)
                deja = False
                jev = iev-1
                while (jev >= 0)and (not deja) : 
                    deja = (ev.pop == self.scenario.history.events[jev].pop)
                    jev -=1
                if deja : cnode.NP = ev.sNe
                if cnode.NP == None : cnode.NP = self.NM[cnode.pop-1]
                if ev.sNe == None : ev.sNe = cnode.NP
                ev.y = cnode.y
                self.node.append(cnode)
                nchild = 0
                if self.cree_branch(ev,0) : nchild +=1
                if self.cree_branch(ev,1) : nchild +=1
                if nchild == 2 :
                    y1,y2 = self.node[self.br[len(self.br)-1].child].y, self.node[self.br[len(self.br)-2].child].y
                    if (y1 < y2) or ((y1 == y2) and (random.random()<0.5)) : self.echangebr(len(self.br)-1,len(self.br)-2)

            if ev.action == "SPLIT" :
                cnode = self.cree_node(ev,iev,0,t0)
                deja = False
                jev = iev
                while (jev > 0)and (not deja) : 
                    jev -=1
                    deja = (ev.pop == self.scenario.history.events[jev].pop)
                if deja : cnode.NP = self.scenario.history.events[jev].sNe
                if cnode.NP == None : cnode.NP = self.NM[cnode.pop-1]
                if ev.sNe == None : ev.sNe = cnode.NP
                ev.y = cnode.y
                self.node.append(cnode)
                self.cree_branch(ev,0)
                cnode = self.cree_node(ev,iev,1,t0)
                deja = False
                jev = iev
                while (jev > 0)and (not deja) : 
                    jev -=1
                    deja = (ev.pop1 == self.scenario.history.events[jev].pop)
                if deja : cnode.NP = self.scenario.history.events[jev].sNe
                if cnode.NP == None : cnode.NP = self.NM[cnode.pop-1]
                if ev.sNe == None : ev.sNe = cnode.NP
#                ev.y = cnode.y
                self.node.append(cnode)
                cbr = PopTreeBranch()
                cbr.father, cbr.child, cbr.nvisit = len(self.node)-1, len(self.node)-2, 0
                self.br.append(cbr)
                self.cree_branch(ev,1)
                cnode = self.cree_node(ev,iev,2,t0)
                deja = False
                jev = iev
                while (jev > 0)and (not deja) : 
                    jev -=1
                    deja = (ev.pop2 == self.scenario.history.events[jev].pop)
                if deja : cnode.NP = self.scenario.history.events[jev].sNe
                if cnode.NP == None : cnode.NP = self.NM[cnode.pop-1]
                if ev.sNe == None : ev.sNe = cnode.NP
#                ev.y = cnode.y
                self.node.append(cnode)
                cbr = PopTreeBranch()
                cbr.father, cbr.child, cbr.nvisit = len(self.node)-1, len(self.node)-3, 0
                self.br.append(cbr)
                self.cree_branch(ev,2)
        cnode = PopTreeNode()
        cnode.category = "vn"
        cnode.pop = self.node[len(self.node)-1].pop
        cnode.y = self.node[len(self.node)-1].y - 20
        self.node.append(cnode)
        cbr = PopTreeBranch()
        cbr.father, cbr.child, cbr.nvisit = len(self.node)-1, len(self.node)-2, 0
        self.br.append(cbr)
        i = len(self.node)
        done = False
        while (i>0) and (not done) :
            i -=1
            if self.node[i].category == "me" :
                self.nanc = i
                done = True
        if (not done)  : self.nanc = 0          

    def findchildren(self,fa):
        b = len(self.br)
        trouve = False
        while (not trouve)and(b>0) :
            b -=1
            trouve = (self.br[b].father == fa)
        if trouve  : print "b=%s  ch1=%s"%(b,self.br[b].child)
        if trouve : ch1 = self.br[b].child
        else      : return -1, -1
        trouve = False
        if b>0 :
            while (not trouve)and(b>0) :
                b -=1
                trouve = (self.br[b].father == fa)
            if trouve  : print "b=%s  ch2=%s"%(b,self.br[b].child)
            if trouve : ch2 = self.br[b].child
            else      : ch2 = -1
        else   : ch2 = -1
        return ch1, ch2
            
    def findfathers(self,ch):
        b = len(self.br)
        trouve = False
        while (not trouve)and(b>0) :
            b -=1
            trouve = (self.br[b].child == ch)
        if trouve : fa1 = self.br[b].father
        else      : return -1, -1
        trouve = False
        if b>0 :
            while (not trouve)and(b>0) :
                b -=1
                trouve = (self.br[b].child == ch)
            if trouve : fa2 = self.br[b].father
            else      : fa2 = -1
        else   : fa2 = -1
        return fa1, fa2
            
    def findchildren2(self,fa):
        c1,c2 = self.findchildren(fa)
        print "fa=%s  c1=%s  c2=%s"%(fa,c1,c2)
        if c1<0  : ch1 = c1
        else     : 
            while (self.node[c1].category == "vn")or(self.node[c1].category == "se") :
                fa1 = c1
                c1,c3 = self.findchildren(fa1)
                if c1<0 : c1 = c3
            ch1 = c1      
        if c2<0  : ch2 = c2
        else     : 
            while (self.node[c2].category == "vn")or(self.node[c2].category == "se") :
                fa1 = c2
                c3,c2 = self.findchildren(fa1)
                if c2<0 : c2 = c3
            ch2 = c2
        return ch1,ch2
    
    def findfather2(self,ch):
        f1,f2 = self.findfathers(ch)
        if f1<0 : f1 = f2
        while (f1>=0)and((self.node[f1].category == "vn"or(self.node[f1].category == "se"))) :
            c = f1
            f1,f2 = self.findfathers(c)
            if f1<0 : f1 = f2
        return f1
    
    def findbranchfather(self,fa,countvisit):
        b = len(self.br)
        trouve = False
        while (not trouve)and(b>0) :
            b -=1
            if countvisit  : trouve = (self.br[b].father == fa) and (self.br[b].nvisit<2)
            else           : trouve =  self.br[b].father == fa
        if trouve : return b
        else      : return -1
        
    def findbranchchild(self,ch,countvisit):
        b = len(self.br)
        trouve = False
        while (not trouve)and(b>0) :
            b -=1
            if countvisit  : trouve = (self.br[b].child == ch) and (self.br[b].nvisit<2)
            else           : trouve =  self.br[b].child == ch
        if trouve : return b
        else      : return -1
        
    def findline(self,n):
        k = n
        pente = (self.node[self.nanc].x - self.node[n].x)/(self.node[self.nanc].y - self.node[n].y)
        while k != self.nanc :
            b = self.findbranchchild(k,False)
            k = self.br[b].father
            if self.node[k].x == 0 :
                self.node[k].x = self.node[n].x+ int(round(pente*(self.node[k].y-self.node[n].y)))
        return pente
    
    def findgoodchild(self,n):
        j1,j2 = self.findchildren(n)
        if j1 == -1 : return j2
        else        : return j1
        
    def findgoodfather(self,n):
        k1,k2 = self.findfathers(n)
        if k1 == -1 : return k2
        else        : return k1
                    
    def draw_tree(self):
        necht = 0
        for no in self.node : 
            if no.category == "sa" : necht +=1
        pasx = self.XMAX//(necht+1)
        xx = pasx
        n = len(self.node)-1
        nv = 0
        while nv<2*len(self.br) :
            i = self.findbranchfather(n,True)
            if i>-1 :
                self.br[i].nvisit +=1
                nv +=1
                n = self.br[i].child
            else    :
                if self.node[n].category == "sa" : 
                    self.node[n].x = xx
                    xx +=pasx
                i = self.findbranchchild(n,True)
                if i>-1 :
                    self.br[i].nvisit +=1
                    nv +=1
                    n = self.br[i].father
        if self.nanc<len(self.node)-1 :
            for i in range(self.nanc,len(self.node)) : self.node[i].x = self.XMAX//2
        """print "\n draw_tree etape 1\n"
        for i,no in enumerate(self.node) :
            print no 
        print "\n"""
        if self.nanc == 0 : return
        n = 0
        while self.node[n].x == 0 :
            n +=1
        nmin,nmax = n,n
        for i in range(n+1,self.nanc+1) :
            if (self.node[i].x>0) and (self.node[i].x<self.node[nmin].x) : nmin = i
        slope1 = self.findline(nmin)
        for i in range(n+1,self.nanc+1) :
            if (self.node[i].x>0) and (self.node[i].x>self.node[nmax].x) : nmax = i
        slope2 = self.findline(nmax)
        if slope1<slope2 : 
            slope=slope1
            slope1=slope2
            slope2=slope
        n=-1
        found = False
        while (n<self.nanc-1) and (not found):
            n +=1
            found = self.node[n].category == "sp0"
        while found :
            j = self.findbranchfather(n,False)
            if self.node[self.br[j].child].x>0 : self.node[n].x = self.node[self.br[j].child].x
            found2 = False
            while (n<self.nanc-1) and (not found2) :
                n +=1
                found2 = self.node[n].category == "sp0" 
            found = found2    
        nrest0 = 0
        #print "\n draw_tree etape 2\n"
        for i in range(0,len(self.node)) : 
            if self.node[i].x == 0 : nrest0 +=1
            #print self.node[i]
        nforce = 0
        while nrest0>0 :
            nrest = nrest0
            n = 0
            while n<len(self.node)-1 :
                found = False
                while (n<len(self.node)-1) and (not found) :
                    n +=1
                    found = (self.node[n].x == 0)
                if found :
                    if (self.node[n].category == "vn")or(self.node[n].category == "sa2")or(self.node[n].category == "se") :
                        j = self.findgoodchild(n)
                        k = self.findgoodfather(n)
                        if (self.node[j].x>0) and (self.node[k].x>0) :
                            self.node[n].x = self.node[j].x+int(round((self.node[n].y-self.node[j].y)/(self.node[k].y-self.node[j].y)*(self.node[k].x-self.node[j].x)))
                            nrest -=1
                        if nforce>1 :
                            j = self.findgoodchild(n)
                            k = self.findgoodfather(n)
                            self.node[n].x = self.node[j].x+int(round((self.node[n].y-self.node[j].y)/(self.node[k].y-self.node[j].y)*(self.node[k].x-self.node[j].x)))
                            nrest -=1
                    elif (self.node[n].category == "me") :
                        j1, j2 = self.findchildren2(n)
                        if (self.node[j1].x>0) and (self.node[j2].x>0) :
                            if self.node[j1].y == self.node[j2].y : self.node[n].x = (self.node[j1].x+self.node[j2].x)//2
                            else :
                                dy1, dy2 = self.node[n].y-self.node[j1].y, self.node[n].y-self.node[j2].y
                                self.node[n].x = int(round((self.node[j1].x*dy2+self.node[j2].x*dy1)/(dy1+dy2)))
                            nrest -=1
                        else :
                            if nforce == 1 :
                                j = self.findbranchchild(n,False)
                                if self.node[self.br[j].father].x > 0 :
                                    j1,j2 = self.findchildren(self.br[j].father)
                                    if (j1<0) or (j2<0) :
                                        self.node[n].x = self.node[self.br[j].father].x
                                        nrest -=1
                                    else :
                                        if (n == j1) and (self.node[j2].x>0) :
                                            if self.node[self.br[j].father].x > self.node[j2].x : 
                                                self.node[n].x = self.node[self.br[j].father].x+int(round(slope1*(self.node[n].y-self.node[self.br[j].father].y)))
                                            else :
                                                self.node[n].x = self.node[self.br[j].father].x+int(round(slope2*(self.node[n].y-self.node[self.br[j].father].y)))
                                            nrest -=1
                                        if (n == j2) and (self.node[j1].x>0) :
                                            if self.node[self.br[j].father].x > self.node[j1].x : 
                                                self.node[n].x = self.node[self.br[j].father].x+int(round(slope1*(self.node[n].y-self.node[self.br[j].father].y)))
                                            else :
                                                self.node[n].x = self.node[self.br[j].father].x+int(round(slope2*(self.node[n].y-self.node[self.br[j].father].y)))
                                            nrest -=1
                    elif (self.node[n].category == "sp0") :
                        j1,j2 = self.findchildren(n)
                        if self.node[j1].x > 0 :  self.node[n].x = self.node[j1].x
                        else  :
                            j1,j2 = self.findfathers(n)
                            if (self.node[j1].x > 0) and (self.node[j2].x > 0) : self.node[n].x = (self.node[j1].x+self.node[j2].x)//2
                        if self.node[n].x > 0 : 
                            nrest -=1
                    elif (self.node[n].category == "sp1") :
                        j = self.findbranchchild(n,False)
                        if self.node[self.br[j].father].x > 0 :
                            j1, j2 = self.findchildren(self.br[j].father)
                            if (j1<0) or (j2<0) :
                                self.node[n].x = self.node[self.br[j].father].x
                                nrest -=1
                            else :
                                if (n == j1) and (self.node[j2].x>0) :
                                    if self.node[self.br[j].father].x > self.node[j2].x : 
                                        self.node[n].x = self.node[self.br[j].father].x+int(round(slope1*(self.node[n].y-self.node[self.br[j].father].y)))
                                    else :
                                        self.node[n].x = self.node[self.br[j].father].x+int(round(slope2*(self.node[n].y-self.node[self.br[j].father].y)))
                                    nrest -=1
                                if (n == j2) and (self.node[j1].x>0) :
                                    if self.node[self.br[j].father].x > self.node[j1].x : 
                                        self.node[n].x = self.node[self.br[j].father].x+int(round(slope1*(self.node[n].y-self.node[self.br[j].father].y)))
                                    else :
                                        self.node[n].x = self.node[self.br[j].father].x+int(round(slope2*(self.node[n].y-self.node[self.br[j].father].y)))                                        
                                    nrest -=1
                        if nforce == 1 :
                            j1,j2 = self.findchildren(n)
                            if self.node[j1].category == "sp0" : j,k = j2,j1
                            else                               : j,k = j1,j2
                            if j>-1 :
                                self.node[n].x = self.node[j].x
                                nrest -=1
                            else :
                                j1 = self.br[self.findbranchchild(n,False)].father
                                if (self.node[j1].x>0) and (self.node[k].x>0) :
                                    self.node[n].x = (self.node[j1].x + self.node[k].x)//2
                                    nrest -=1
            if nrest == nrest0 : nforce +=1
            else               : nforce = 0
            nrest0 = nrest
        """print "\nDraw_tree etape 2"
        for i,no in enumerate(self.node) :
            print no """       
        n = -1
        while n < (len(self.node)-1) :
            found = False
            while (not found)and(n<len(self.node)-1) :
                n +=1
                found = self.node[n].category == "sp0"       
                if found :
                    j1,j2 = self.findfathers(n)                    
                    self.node[n].x = (self.node[j1].x+self.node[j2].x)//2    
        for i in range(0,len(self.node)) : self.node[i].x = self.XMAX - 30 - self.node[i].x        
        """print "\nDraw_tree etape 3"
        for i,no in enumerate(self.node) :
            print no"""        
                
    def improve_draw(self):
        for i,no in enumerate(self.node) :
            if no.category == "sp0" :
                j =0
                while self.br[j].child != i : j +=1
                i1, k1 = self.br[j].father, j
                j +=1
                while self.br[j].child != i : j +=1
                i2, k2 = self.br[j].father, j
                if abs(self.node[i1].x - self.node[i2].x) < 40 :
                    if random.random()<0.5 : 
                        self.node[i1].x +=20
                        self.node[i2].x -=20 
                    else :
                        self.node[i1].x -=20
                        self.node[i2].x +=20
                if self.node[i1].pop == self.scenario.history.events[self.node[i1].ev].pop1 :
                    self.br[k1].sadm = self.scenario.history.events[self.node[i1].ev].sadmixrate 
                    self.br[k2].sadm = '1-'+self.br[k1].sadm
                else :
                    self.br[k2].sadm = self.scenario.history.events[self.node[i1].ev].sadmixrate 
                    self.br[k1].sadm = '1-'+self.br[k2].sadm
        nr=1
        while nr != 0 :
            nr=0
            for i,no in enumerate(self.node) :
                if no.category == "me" :
                    i1 = self.findfather2(i)
                    if i1 >-1 :
                        if (no.y == self.node[i1].y)and(no.x != self.node[i1].x) :
                            no.x = self.node[i1].x
                            nr +=1
        """print "\nImprove_tree etape 1"
        for i,no in enumerate(self.node) :
            print no"""        
        for i,no in enumerate(self.node) :
            if no.category == "me" :
                i1 = self.findfather2(i)
                k1,k2 = self.findchildren2(i)
                #print "i=%s  i1=%s  k1=%s  k2=%s" %(i,i1,k1,k2) 
                if (i1>0)and(k1>0)and(k2>0) :
                    if (self.node[k1].y != self.node[i1].y) and (self.node[k2].y != self.node[i1].y) :
                        x1 = self.node[i1].x + (no.y-self.node[i1].y)*(self.node[k1].x-self.node[i1].x)/(self.node[k1].y-self.node[i1].y)
                        x2 = self.node[i1].x + (no.y-self.node[i1].y)*(self.node[k2].x-self.node[i1].x)/(self.node[k2].y-self.node[i1].y)
                        if x1>x2 : piv=x1;x1=x2;x2=piv
                        #print "x1=%s  x2=%s" %(x1,x2)
                        if (no.x<int(round(x1))-5) or (no.x>int(round(x2))+5) :
                            no.x = int(round(0.5*(x1+x2)))
                            i2=i
                            while i2!=i1 :
                                j=0
                                while self.br[j].child != i2 : j +=1
                                i2 = self.br[j].father
                                if ((self.node[i2].category == "vn")or(self.node[i2].category == "se"))and(self.node[i1].y != no.y) :
                                    self.node[i2].x = no.x + int(round((self.node[i2].y-no.y)*(self.node[i1].x-no.x)/(self.node[i1].y-no.y)))
                                    #print "premier while node[i2].x=%s"%(self.node[i2].x)
                            i2=i
                            while i2!=k1 :
                                jj=len(self.br)-1
                                while self.br[j].father != i2 : j -=1
                                i2 = self.br[j].child
                                if ((self.node[i2].category == "vn")or(self.node[i2].category == "se"))and(self.node[k1].y != no.y) :
                                    self.node[i2].x = no.x + int(round((self.node[i2].y-no.y)*(self.node[k1].x-no.x)/(self.node[k1].y-no.y)))
                                    #print "deuxieme while node[i2].x=%s"%(self.node[i2].x)
                            i2=i
                            while i2!=k2 :
                                while (self.br[j].father != i2)and(j>0) : 
                                    j -=1
                                if i2 == self.br[j].father :
                                    i2 = self.br[j].child;
                                    if ((self.node[i2].category == "vn")or(self.node[i2].category == "se"))and(self.node[i1].y != no.y) :
                                        self.node[i2].x = no.x + int(round((self.node[i2].y-no.y)*(self.node[k2].x-no.x)/(self.node[k2].y-no.y)))
                                        #print "troisieme while node[i2].x=%s"%(self.node[i2].x)
                                    else: break
        """print "\nImprove_tree etape 2"
        for i,no in enumerate(self.node) :
            print no """       
    def setbranchstyle(self):
        fin = False;
        while (not fin) :
            fin = True
            for b in self.br :
                print self.node[b.father].style,self.node[b.child].style
                if (self.node[b.father].style == "dot")and(self.node[b.child].style != "dot") :
                    self.node[b.child].style = "dot"
                    fin = False
    
    
    def signe(self,a):
        if a>0   :return 1
        elif a<0 :return -1
        else     :return 0
    
    def creesegment(self):
        segment = []
        for i in range(0,len(self.br)) :
            seg = PopTreeSegment()
            b = self.br[i]
            seg.xdeb, seg.ydeb, seg.xfin, seg.yfin = self.node[b.father].x, self.node[b.father].y, self.node[b.child].x, self.node[b.child].y
            seg.popdeb, seg.popfin, seg.evdeb, seg.evfin = self.node[b.father].pop, self.node[b.child].pop, self.node[b.father].ev, self.node[b.child].ev
            seg.sNedeb, seg.sNefin, seg.sadm = self.node[b.father].NP, self.node[b.child].NP, b.sadm
            seg.style = self.node[b.father].style
            segment.append(seg)
        return segment
    
    def secant(self,seg1,seg2):
        xe = 0.0
        if seg1.ydeb == seg1.yfin :
            ye = seg1.ydeb
            if seg2.ydeb == seg2.yfin :
                if seg1.ydeb == seg2.ydeb :
                    if min(seg2.xdeb, seg2.xfin) > max(seg1.xdeb, seg1.xfin) : return False
                    if min(seg1.xdeb, seg1.xfin) > max(seg2.xdeb, seg2.xfin) : return False
                else : return False
            else :
                a2 = (seg2.xfin-seg2.xdeb)/(seg2.yfin-seg2.ydeb)
                xe = seg2.xdeb + a2*(ye-seg2.ydeb)
        if seg2.ydeb == seg2.yfin :
            ye = seg2.ydeb
            if seg1.yfin == seg1.ydeb : a2 = self.signe(seg1.xfin-seg1.xdeb)*(1e+100)     
            else                      : a2 = (seg1.xfin-seg1.xdeb)/(seg1.yfin-seg1.ydeb)
            xe = seg1.xdeb + a2*(ye-seg1.ydeb)
        if (seg1.ydeb != seg1.yfin) and (seg2.ydeb != seg2.yfin) :
            a1 = (seg1.xfin - seg1.xdeb)/(seg1.yfin - seg1.ydeb)
            a2 = (seg2.xfin - seg2.xdeb)/(seg2.yfin - seg2.ydeb)
            if abs(a1-a2) > 1e-30 :
                ye = (a1*seg1.ydeb - a2*seg2.ydeb + seg2.xdeb -seg1.xdeb)/(a1-a2)
                xe = seg1.xdeb + a1*(ye-seg1.ydeb)
            else :
                if abs(seg2.yfin - seg1.ydeb) < 1 : a3 = self.signe(seg2.yfin-seg1.ydeb)*(1e+100)
                else                              : a3 = (seg2.xfin-seg1.xdeb)/(seg2.yfin-seg1.ydeb)
                if abs(a1-a3) > 1e-30 : return False
                if min(seg2.xdeb, seg2.xfin) > max(seg1.xdeb, seg1.xfin) : return False
                if min(seg1.xdeb, seg1.xfin) > max(seg2.xdeb, seg2.xfin) : return False
        xx = int(round(xe))
        amin = min(seg1.xdeb, seg1.xfin)
        amax = max(seg1.xdeb, seg1.xfin)
        bmin = min(seg2.xdeb, seg2.xfin)
        bmax = max(seg2.xdeb, seg2.xfin)
        return (xx>amin)and(xx<amax)and(xx>bmin)and(xx<bmax)
    
    def countpb(self,segment):
        npb = 0
        for i in range(0,len(segment)-1) :
            for j in range(i+1,len(segment)) :
                if self.secant(segment[i],segment[j]) :
                    npb +=1
        return npb
    
    def do_tree(self):
        self.initNN()
        ntrial = 0
        npb   = 2
        while (ntrial<100) and (npb>0) :
            self.build_tree()
            print "apres build"
            self.draw_tree()
            print "apres draw"
            self.improve_draw()
            print "apres improve"
            self.setbranchstyle()
            print "apres setbranchstyle"
            self.segments = self.creesegment()
            npb= self.countpb(self.segments)
            ntrial +=1
            print "%s : %s problemes\n"%(ntrial,npb)
        if npb<1 :
            if len(self.br) == 1 : self.segments[0].ydeb = self.YHAUT
        else :
            raise PopTreeError, "Scenario %s seems OK. But the current version of DIYABC is unable to provide a valid graphic representation."%(self.scenario.number)    
            
def testCheckScenario():
#    ts = ['N1 N2 N3', '0 sample 1', '0 sample 2', '0 sample 3', '50 split 3 1 2 lambda', "100 merge 1 2"]
    ts = ['N1 N2 N3 N4 N5','0 sample 1', '0 sample 2', '0 sample 3','0 sample 4','t1 split 2 5 3 r1','t2 merge 1 5','t3 split 3 1 4 r2','t4 merge 1 4']
    print ts
    s = history.Scenario()
    s.checkread(ts)
    print "apres checkread"
    s.checklogic()
    print "apres checklogic"
    t = PopTree(s)
    t.do_tree()
    print "time0=",s.history.time0
    print "time1=",s.history.time1
    for ev in s.history.events : print ev
    for  no in t.node : print no
    print "  "
    for  b in t.br : print b
    print "  "
    for s in t.segments : print s
        
if __name__ == "__main__" :
    testCheckScenario()
