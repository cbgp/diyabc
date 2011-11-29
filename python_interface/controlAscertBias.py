# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
from utils.visualizescenario import *
from utils.data import *
import output
from utils.cbgpUtils import log
import codecs

formControl,baseControl = uic.loadUiType("uis/controlAscertBias.ui")

class ControlAscertBias(formControl,baseControl):
    def __init__(self,parent=None):
        super(ControlAscertBias,self).__init__(parent)
        self.parent=parent

        self.createWidgets()

    def createWidgets(self):
        #self.ui = Ui_Frame()
        self.ui = self
        self.ui.setupUi(self)

        self.ui.verticalLayout_3.setAlignment(QtCore.Qt.AlignCenter)

        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)

    def loadAscertConf(self):
        if os.path.exists(self.parent.dir+"/%s"%self.parent.parent.ascertainment_conf_name):
            f = codecs.open(self.parent.dir+"/%s"%self.parent.parent.ascertainment_conf_name,'r',"utf-8")
            lines = f.readlines()
            f.close()
            if len(lines) >= 1:
                self.ui.freqMinEdit.setText(lines[0].strip())

    def validate(self,silent=False):
        # verif
        problem = ""
        try:
            freq = float(self.ui.freqMinEdit.text())
            if freq < 0 or freq > 0.5 :
                raise Exception("Frequency must be positive and lower than 0.5")
        except Exception,e:
            problem += "Frequency : %s\n"%(e)
        if problem != "":
            if not silent:
                output.notify(self,"Value error",problem)
            self.parent.setAscertValid(False)
        else:
            self.parent.setAscertValid(True)
            #sortie
            self.exit()

    def exit(self):
        self.parent.ui.refTableStack.removeWidget(self)
        self.parent.ui.refTableStack.setCurrentIndex(0)

