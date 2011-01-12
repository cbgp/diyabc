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



    def createWidgets(self):
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        QObject.connect(self.ui.closeButton,SIGNAL("clicked()"),self.close)
        QObject.connect(self.ui.saveButton,SIGNAL("clicked()"),self.saveDraws)

    def drawAll(self):
        for num,sc in enumerate(self.sc_txt_list):
            scChecker = history.Scenario(number=num+1)
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
                self.addDraw(t.segments,scChecker,t)
            except IOScreenError, e:
                print "Un scenario a une erreur : ", e
                QMessageBox.information(self,"Scenario error","%s"%e)
                self.close()


    def addDraw(self,segments,scc,t):
        """ dessine un scenario et l'ajoute à la fenetre
        """
        
        tab_colors = ["#0000FF","#00FF00","#FF0000","#00FFFF","#FF00FF","#FFFF00","#000000","#808080","#008080","#800080","#808000","#000080","#008000","#800000","#A4A0A0","#A0A4A0","#A0A0A4","#A00000","#00A000","#00A0A0"]
        NN = scc.history.ne0s

        xmax = 500
        ymax = 450
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
        font = QFont()
        font.setItalic(False)
        font.setPixelSize(16)
        painter.setFont(font)
        painter.drawText( 10,20, "Scenario %i"%(scc.number+1))

        # echelle de temps
        pen = QPen(Qt.black,1)
        painter.setPen(pen)
        painter.drawLine(xmax-50,50,xmax-50,ymax-50)
        pen = QPen(Qt.black,4)
        painter.setPen(pen)
        for i in range(len(scc.history.events)):
            y = scc.history.events[i].y
            painter.drawLine(xmax-55,y,xmax-45,y)
            font = QFont()
            font.setItalic(False)
            font.setPixelSize(10)
            painter.setFont(font)
            painter.drawText(xmax-30,y+5,scc.history.events[i].stime)
        font = QFont()
        font.setItalic(False)
        font.setPixelSize(10)
        painter.setFont(font)
        painter.drawText(xmax-170,14,"(Warning ! Time is not to scale.)");

        # echantillons
        pen = QPen(Qt.black,1)
        painter.setPen(pen)
        n=0
        for i in range(0,len(t.node)):
            if (t.node[i].category == "sa") or (t.node[i].category == "sa2"):
                for j in range(0,len(t.node)):
                    if ((t.node[i].category == "sa") or (t.node[i].category == "sa2")) and (t.node[i].category != t.node[j].category):
                        if t.node[i].y > t.node[j].y:
                            t.node[i].category = "sa";
                            t.node[j].category = "sa2";
                        else:
                            t.node[j].category = "sa";
                            t.node[i].category = "sa2";
                    for i in range(0,len(t.node)):
                        if (t.node[i].category == "sa") or (t.node[i].category == "sa2"):
                            x = t.node[i].x
                            y = t.node[i].y
                            n+=1
                            pen = QPen(Qt.black,1)
                            painter.setPen(pen)
                            painter.drawRoundRect(x-15,y+10,35,20)
                            font = QFont()
                            font.setItalic(True)
                            font.setPixelSize(10)
                            painter.setFont(font)
                            painter.drawText(x-10,y+25,'Sa %i'%t.node[i].pop)
                            font = QFont()
                            font.setItalic(False)
                            font.setPixelSize(12)
                            painter.setFont(font)
                            if t.node[i].category == "sa":
                                painter.drawText(x-19,y+50,"Pop %i"%t.node[i].pop)


        # dessin des segments
        for s in segments:
            print "seg",s
            if s.ydeb == s.yfin and s.xdeb != s.xfin:
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
                font = QFont()
                font.setItalic(False)
                font.setPixelSize(10)
                painter.setFont(font)
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
