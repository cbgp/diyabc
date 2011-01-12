# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from drawScenario_ui import Ui_MainWindow
from visualizescenario import *
import history 
from history import IOScreenError

class drawScenario(QMainWindow):
    def __init__(self,sc_txt_list,parent=None):
        super(drawScenario,self).__init__(parent)
        self.parent=parent
        self.createWidgets()
        self.sc_txt_list = sc_txt_list
        self.pixList = []    

        for num,sc in enumerate(self.sc_txt_list):
            scChecker = history.Scenario(number=num)
            try:
                scChecker.checkread(sc.split('\n'))
                scChecker.checklogic()
                t = PopTree(scChecker)
                t.do_tree()
                #for ev in scChecker.history.events : print ev
                #for  no in t.node : print no
                #print "  "
                #for  b in t.br : print b
                #print "  "
                for s in t.segments: 
                    print s
                    print type(s)
                self.addDraw(t.segments,scChecker)
            except IOScreenError, e:
                print "Un scenario a une erreur : ", e

    def createWidgets(self):
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        QObject.connect(self.ui.closeButton,SIGNAL("clicked()"),self.close)
        QObject.connect(self.ui.saveButton,SIGNAL("clicked()"),self.saveDraws)

    def addDraw(self,segments,scc):
        """ dessine un scenario et l'ajoute à la fenetre
        """
        
        tab_colors = ["#0000FF","#00FF00","#FF0000","#00FFFF","#FF00FF","#FFFF00","#000000","#808080","#008080","#800080","#808000","#000080","#008000","#800000","#A4A0A0","#A0A4A0","#A0A0A4","#A00000","#00A000","#00A0A0"]
        NN = scc.history.ne0s

        xmax = 450
        ymax = 400
        pix = QPixmap(xmax,ymax)
        self.pixList.append(pix)
        pix.fill(Qt.white)

        painter = QPainter(pix)
        pen = QPen(Qt.black,2)
        painter.setPen(pen)
        
        # dessin de la cartouche
        painter.drawLine(2,2,xmax-2,2)
        painter.drawLine(xmax-2,2,xmax-2,ymax-2)
        painter.drawLine(xmax-2,ymax-2,2,ymax-2)
        painter.drawLine(2,ymax-2,2,2)
        painter.setPen(QPen(Qt.black,6))
        # TODO verifier la validité de NN qui doit avoir une taille de 1 quand N N N
        for i in range(len(NN)):
            pen = QPen(Qt.yellow,6)
            pen.setCapStyle(Qt.RoundCap)
            pen.setColor(QColor(tab_colors[(i%20)]))
            painter.setPen(pen)
            painter.drawLine(20,40+20*i,30,40+20*i)
            pen.setColor(QColor("#000000"))
            painter.setPen(pen)
            painter.drawText( 40,45+20*i, NN[i].name)
        painter.setPen(QPen(Qt.black,20))
        painter.drawText( 10,15, "Scenario %i"%(scc.number+1))

        # echelle de temps
        pen = QPen(Qt.black,1)
        painter.setPen(pen)
        painter.drawLine(xmax-50,50,xmax-50,ymax-50)
        for i in range(len(scc.history.events)):
            y = scc.history.events[i].y
            painter.drawLine(xmax-60,y,xmax-40,y)




        # dessin des segments
        for s in segments:
            if s.ydeb == s.yfin:
                xmed = (s.xdeb+s.xfin)/2
                ymed = s.ydeb
                jj = 0
                while(jj < len(NN)) and (NN[jj].name != s.sNedeb):
                    jj+=1
                pen = QPen(Qt.yellow,10)
                pen.setCapStyle(Qt.RoundCap)
                pen.setColor(QColor(tab_colors[(jj%20)]))
                painter.setPen(pen)
                painter.drawLine(s.xdeb,ymed,xmed,ymed)
                painter.setPen(QPen(Qt.black,8))
                x0 = xmed-(2*len(s.sadm))
                painter.drawText( x0, ymed+16, s.sadm)

                jj = 0
                while(jj < len(NN)) and (NN[jj].name != s.sNefin):
                    jj+=1
                pen = QPen(Qt.yellow,10)
                pen.setCapStyle(Qt.RoundCap)
                pen.setColor(QColor(tab_colors[(jj%20)]))
                painter.setPen(pen)
                painter.drawLine(s.xfin,ymed,xmed,ymed)
            else:
                jj = 0
                while(jj < len(NN)) and (NN[jj].name != s.sNefin):
                    jj+=1
                pen = QPen(Qt.black,10)
                pen.setCapStyle(Qt.RoundCap)
                pen.setColor(QColor(tab_colors[(jj%20)]))
                painter.setPen(pen)
                painter.drawLine(s.xdeb,s.ydeb,s.xfin,s.yfin)

        label = QLabel()
        label.setPixmap(pix)
        self.ui.horizontalLayout_2.addWidget(label)

    def saveDraws(self):
        for ind,pix in enumerate(self.pixList):
            im = pix.toImage()
            im.save("/tmp/im_%i.jpg"%ind)
            print 'sauvé dans /tmp/im_%i.jpg'%ind

    def closeEvent(self, event):
        pass
