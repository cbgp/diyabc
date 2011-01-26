# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setMutationModelSequences_ui import Ui_Frame

class SetMutationModelSequences(QFrame):
    def __init__(self,parent=None):
        super(SetMutationModelSequences,self).__init__(parent)
        self.parent=parent
        self.createWidgets()

        if self.ui.jukesRadio.isChecked():
            self.showJukes()
        elif self.ui.haseRadio.isChecked() or self.ui.kimuraRadio.isChecked():
            self.showKimuraHase()
        elif self.ui.tamuraRadio.isChecked():
            self.showTamura()

        self.ui.verticalLayout_27.setAlignment(Qt.AlignTop)
        self.ui.verticalLayout_26.setAlignment(Qt.AlignTop)

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.tamuraRadio,SIGNAL("clicked()"),self.showTamura)
        QObject.connect(self.ui.jukesRadio,SIGNAL("clicked()"),self.showJukes)
        QObject.connect(self.ui.haseRadio,SIGNAL("clicked()"),self.showKimuraHase)
        QObject.connect(self.ui.kimuraRadio,SIGNAL("clicked()"),self.showKimuraHase)

    def showJukes(self):
        self.ui.firstFrame.show()
        self.ui.secondFrame.show()
        self.ui.thirdFrame.hide()
        self.ui.fourthFrame.hide()
        self.ui.fifthFrame.hide()
        self.ui.sixthFrame.hide()
    def showKimuraHase(self):
        self.ui.firstFrame.show()
        self.ui.secondFrame.show()
        self.ui.thirdFrame.show()
        self.ui.fourthFrame.show()
        self.ui.fifthFrame.hide()
        self.ui.sixthFrame.hide()
    def showTamura(self):
        self.ui.firstFrame.show()
        self.ui.secondFrame.show()
        self.ui.thirdFrame.show()
        self.ui.fourthFrame.show()
        self.ui.fifthFrame.show()
        self.ui.sixthFrame.show()

    def getMutationConf(self):
        """ renvoie les lignes à écrire dans la conf
        """
        result = ""
        if self.ui.jukesRadio.isChecked():
            model = "JK"
        elif self.ui.kimuraRadio.isChecked():
            model = "K2P"
        elif self.ui.haseRadio.isChecked():
            model = "HKY"
        elif self.ui.tamuraRadio.isChecked():
            model = "TN"

        mmrMin =    str(self.ui.mmrMinEdit.text())
        mmrMax =    str(self.ui.mmrMaxEdit.text())
        mmrMean =   str(self.ui.mmrMeanEdit.text())
        mmrShape =  str(self.ui.mmrShapeEdit.text())
        if mmrMean == "":
            mmrMean = "-9"
        if mmrShape == "":
            mmrShape = "2"

        if self.ui.mmrUnifRadio.isChecked():
            law = "UN"
        elif self.ui.mmrLogRadio.isChecked():
            law = "LN"
        else:
            law = "GA"
        result += "MEANMU   %s[%s,%s,%s,%s]\n"%(law,mmrMin,mmrMax,mmrMean,mmrShape)
        
        ilmrMin =    str(self.ui.ilmrMinEdit.text())
        ilmrMax =    str(self.ui.ilmrMaxEdit.text())
        ilmrMean =   str(self.ui.ilmrMeanEdit.text())
        ilmrShape =  str(self.ui.ilmrShapeEdit.text())
        if ilmrMean == "":
            ilmrMean = "-9"
        if ilmrShape == "":
            ilmrShape = "2"
        law = "GA"
        result += "GAMMU   %s[%s,%s,%s,%s]\n"%(law,ilmrMin,ilmrMax,ilmrMean,ilmrShape)

        if model != "JK":

            mc1Min =    str(self.ui.mc1MinEdit.text())
            mc1Max =    str(self.ui.mc1MaxEdit.text())
            mc1Mean =   str(self.ui.mc1MeanEdit.text())
            mc1Shape =  str(self.ui.mc1ShapeEdit.text())
            if mc1Mean == "":
                mc1Mean = "-9"
            if mc1Shape == "":
                mc1Shape = "2"

            if self.ui.mc1UnifRadio.isChecked():
                law = "UN"
            elif self.ui.mc1LogRadio.isChecked():
                law = "LN"
            else:
                law = "GA"
            result += "MEANK1   %s[%s,%s,%s,%s]\n"%(law,mc1Min,mc1Max,mc1Mean,mc1Shape)

            ilc1Min =    str(self.ui.ilc1MinEdit.text())
            ilc1Max =    str(self.ui.ilc1MaxEdit.text())
            ilc1Mean =   str(self.ui.ilc1MeanEdit.text())
            ilc1Shape =  str(self.ui.ilc1ShapeEdit.text())
            if ilc1Mean == "":
                ilc1Mean = "-9"
            if ilc1Shape == "":
                ilc1Shape = "2"

            law = "GA"
            result += "GAMK1   %s[%s,%s,%s,%s]\n"%(law,ilc1Min,ilc1Max,ilc1Mean,ilc1Shape)

            if model == "TN":

                mc2Min =    str(self.ui.mc2MinEdit.text())
                mc2Max =    str(self.ui.mc2MaxEdit.text())
                mc2Mean =   str(self.ui.mc2MeanEdit.text())
                mc2Shape =  str(self.ui.mc2ShapeEdit.text())
                if mc2Mean == "":
                    mc2Mean = "-9"
                if mc2Shape == "":
                    mc2Shape = "2"

                if self.ui.mc2UnifRadio.isChecked():
                    law = "UN"
                elif self.ui.mc2LogRadio.isChecked():
                    law = "LN"
                else:
                    law = "GA"
                result += "MEANK2   %s[%s,%s,%s,%s]\n"%(law,mc2Min,mc2Max,mc2Mean,mc2Shape)

                ilc2Min =    str(self.ui.ilc2MinEdit.text())
                ilc2Max =    str(self.ui.ilc2MaxEdit.text())
                ilc2Mean =   str(self.ui.ilc2MeanEdit.text())
                ilc2Shape =  str(self.ui.ilc2ShapeEdit.text())
                if ilc2Mean == "":
                    ilc2Mean = "-9"
                if ilc2Shape == "":
                    ilc2Shape = "2"

                law = "GA"
                result += "GAMSK2   %s[%s,%s,%s,%s]\n"%(law,ilc2Min,ilc2Max,ilc2Mean,ilc2Shape)
        
        return result

    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))



