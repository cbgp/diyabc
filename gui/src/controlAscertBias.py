# -*- coding: utf-8 -*-


import sys, codecs, traceback
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
from utils.visualizescenario import *
from utils.data import *
from utils.cbgpUtils import log,getFsEncoding
import output
import variables
from variables import UIPATH

formControl,baseControl = uic.loadUiType((u"%s/controlAscertBias.ui"%UIPATH).encode(getFsEncoding(logLevel=False)))

class ControlAscertBias(formControl,baseControl):
    def __init__(self,parent=None):
        super(ControlAscertBias,self).__init__(parent)
        self.parent=parent
        self.fsCoding = getFsEncoding(logLevel=False)
        self.createWidgets()

    def createWidgets(self):
        #self.ui = Ui_Frame()
        self.ui = self
        self.ui.setupUi(self)

        self.ui.verticalLayout_3.setAlignment(QtCore.Qt.AlignCenter)

        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.ascYesRadio,SIGNAL("toggled(bool)"),self.activateAsc)
        QObject.connect(self.ui.ascNoRadio,SIGNAL("toggled(bool)"),self.activateAsc)

        self.ui.ascYesRadio.setChecked(True)
        self.ui.ascNoRadio.setChecked(True)

    def activateAsc(self,state):
        if state:
            if self.sender() == self.ui.ascYesRadio:
                self.ui.valuesFrame.show()
            else:
                self.ui.valuesFrame.hide()

    def loadAscertConf(self):
        if os.path.exists((u"%s/%s"%(self.parent.dir,self.parent.parent.ascertainment_conf_name)).encode(self.fsCoding)):
            f = codecs.open((u"%s/%s"%(self.parent.dir,self.parent.parent.ascertainment_conf_name)).encode(self.fsCoding),'rU',"utf-8")
            lines = f.readlines()
            f.close()
            if len(lines) >= 3:
                self.ui.asc1ValEdit.setText(lines[0].strip())
                self.ui.asc2ValEdit.setText(lines[1].strip())
                self.ui.asc3ValEdit.setText(lines[2].strip())
                self.ui.ascYesRadio.setChecked(True)
            else:
                self.ui.ascNoRadio.setChecked(True)


    def validate(self,silent=False):
        if self.ui.ascYesRadio.isChecked():
            problem = ""
            try:
                freq = float(self.ui.asc1ValEdit.text())
                if freq < 0 or freq > 0.5 :
                    raise Exception("Frequency must be positive and lower than 0.5")
                poly = float(self.ui.asc2ValEdit.text())
                if poly < 0 or poly > 1 :
                    raise Exception("Proportion of polymorphic SNPs must be in [0,1]")
                elim = float(self.ui.asc3ValEdit.text())
                if elim < 0 or elim > 1 :
                    raise Exception("Threshold to eliminate parameter values must be in [0,1]")
            except Exception as e:
                problem += "%s\n"%(e)
            if problem != "":
                if not silent:
                    output.notify(self,"Value error",problem)
                self.parent.setAscertValid(False)
            else:
                self.parent.setAscertValid(True)
                #sortie
                self.exit()
        else:
            self.parent.setAscertValid(True)
            self.exit()

    def exit(self):
        self.parent.ui.refTableStack.removeWidget(self)
        self.parent.ui.refTableStack.setCurrentIndex(0)

