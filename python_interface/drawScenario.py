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
                self.addDraw(t.segments)
            except IOScreenError, e:
                print "Un scenario a une erreur : ", e

    def createWidgets(self):
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        QObject.connect(self.ui.closeButton,SIGNAL("clicked()"),self.close)
        QObject.connect(self.ui.saveButton,SIGNAL("clicked()"),self.saveDraws)

    def addDraw(self,segments):
        pix = QPixmap(400,400)
        self.pixList.append(pix)
        pix.fill(Qt.white)

        painter = QPainter(pix)
        painter.setPen(QPen(Qt.blue,10))
        #painter.drawLine(0, 0, 100, 100)
        #painter.drawText( 100, 100, "plop")
        for s in segments:
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
