# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setMutationModel_ui import Ui_Frame

class SetMutationModel(QFrame):
    def __init__(self,parent=None):
        super(SetMutationModel,self).__init__(parent)
        self.parent=parent
        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)

    def getMutationConf(self):
        """ renvoie les lignes à écrire dans la conf
        """
        result = ""
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
        result += "MEANMU %s[%s,%s,%s,%s]\n"%(law,mmrMin,mmrMax,mmrMean,mmrShape)
        
        ilmrMin =    str(self.ui.ilmrMinEdit.text())
        ilmrMax =    str(self.ui.ilmrMaxEdit.text())
        ilmrMean =   u'%s'%(self.ui.ilmrMeanEdit.text())
        ilmrShape =  str(self.ui.ilmrShapeEdit.text())
        if ilmrMean == "":
            ilmrMean = "-9"
        if ilmrShape == "":
            ilmrShape = "2"
        law = "GA"
        result += "GAMMU %s[%s,%s,%s,%s]\n"%(law,ilmrMin,ilmrMax,ilmrMean,ilmrShape)

        mcpMin =    str(self.ui.mcpMinEdit.text())
        mcpMax =    str(self.ui.mcpMaxEdit.text())
        mcpMean =   str(self.ui.mcpMeanEdit.text())
        mcpShape =  str(self.ui.mcpShapeEdit.text())
        if mcpMean == "":
            mcpMean = "-9"
        if mcpShape == "":
            mcpShape = "2"

        if self.ui.mcpUnifRadio.isChecked():
            law = "UN"
        elif self.ui.mcpLogRadio.isChecked():
            law = "LN"
        else:
            law = "GA"
        result += "MEANP %s[%s,%s,%s,%s]\n"%(law,mcpMin,mcpMax,mcpMean,mcpShape)

        ilcpMin =    str(self.ui.ilcpMinEdit.text())
        ilcpMax =    str(self.ui.ilcpMaxEdit.text())
        ilcpMean =   str(self.ui.ilcpMeanEdit.text())
        ilcpShape =  str(self.ui.ilcpShapeEdit.text())
        if ilcpMean == "":
            ilcpMean = "-9"
        if ilcpShape == "":
            ilcpShape = "2"

        law = "GA"
        result += "GAMP %s[%s,%s,%s,%s]\n"%(law,ilcpMin,ilcpMax,ilcpMean,ilcpShape)

        msrMin =    str(self.ui.msrMinEdit.text())
        msrMax =    str(self.ui.msrMaxEdit.text())
        msrMean =   str(self.ui.msrMeanEdit.text())
        msrShape =  str(self.ui.msrShapeEdit.text())
        if msrMean == "":
            msrMean = "-9"
        if msrShape == "":
            msrShape = "2"

        if self.ui.msrUnifRadio.isChecked():
            law = "UN"
        elif self.ui.msrLogRadio.isChecked():
            law = "LN"
        else:
            law = "GA"
        result += "MEANSNI %s[%s,%s,%s,%s]\n"%(law,msrMin,msrMax,msrMean,msrShape)

        ilsrMin =    str(self.ui.ilsrMinEdit.text())
        ilsrMax =    str(self.ui.ilsrMaxEdit.text())
        ilsrMean =   u'%s'%(self.ui.ilsrMeanEdit.text())
        ilsrShape =  str(self.ui.ilsrShapeEdit.text())
        if ilsrMean == "":
            ilsrMean = "-9"
        if ilsrShape == "":
            ilsrShape = "2"

        law = "GA"
        result += "GAMSNI %s[%s,%s,%s,%s]\n"%(law,ilsrMin,ilsrMax,ilsrMean,ilsrShape)
        
        return result

    def setMutationConf(self,lines):
        """ set les valeurs depuis la conf
        """
        mmrValues = lines[0].split('[')[1].split(']')[0].split(',')
        if mmrValues[2] == "-9":
            mmrValues[2] = ""
        if mmrValues[3] == "2":
            mmrValues[3] = ""
        self.ui.mmrMinEdit.setText(mmrValues[0])
        self.ui.mmrMaxEdit.setText(mmrValues[1])    
        self.ui.mmrMeanEdit.setText(mmrValues[2])   
        self.ui.mmrShapeEdit.setText(mmrValues[3])  
        law = lines[0].split('[')[0].split(' ')[-1]

        print "law:",law
        if law == "UN":
            self.ui.mmrUnifRadio.setChecked(True)
        elif law == "LN": 
            self.ui.mmrLogRadio.setChecked(True)
        else:
            self.ui.mmrGammaRadio.setChecked(True)
        
        ilmrValues = lines[1].split('[')[1].split(']')[0].split(',')
        if ilmrValues[2] == "-9":
            ilmrValues[2] = ""
        if ilmrValues[3] == "2":
            ilmrValues[3] = ""
        self.ui.ilmrMinEdit.setText(ilmrValues[0])
        self.ui.ilmrMaxEdit.setText(ilmrValues[1])
        self.ui.ilmrMeanEdit.setText(ilmrValues[2])
        self.ui.ilmrShapeEdit.setText(ilmrValues[3])

        mcpValues = lines[2].split('[')[1].split(']')[0].split(',')
        if mcpValues[2] == "-9":
            mcpValues[2] = ""
        if mcpValues[3] == "2":
            mcpValues[3] = ""
        self.ui.mcpMinEdit.setText(mcpValues[0])
        self.ui.mcpMaxEdit.setText(mcpValues[1])    
        self.ui.mcpMeanEdit.setText(mcpValues[2])   
        self.ui.mcpShapeEdit.setText(mcpValues[3])  
        law = lines[2].split('[')[0].split(' ')[-1]

        print "law:",law
        if law == "UN":
            self.ui.mcpUnifRadio.setChecked(True)
        elif law == "LN": 
            self.ui.mcpLogRadio.setChecked(True)
        else:
            self.ui.mcpGammaRadio.setChecked(True)

        ilcpValues = lines[3].split('[')[1].split(']')[0].split(',')
        if ilcpValues[2] == "-9":
            ilcpValues[2] = ""
        if ilcpValues[3] == "2":
            ilcpValues[3] = ""
        self.ui.ilcpMinEdit.setText(ilcpValues[0])
        self.ui.ilcpMaxEdit.setText(ilcpValues[1])
        self.ui.ilcpMeanEdit.setText(ilcpValues[2])
        self.ui.ilcpShapeEdit.setText(ilcpValues[3])

        msrValues = lines[4].split('[')[1].split(']')[0].split(',')
        if msrValues[2] == "-9":
            msrValues[2] = ""
        if msrValues[3] == "2":
            msrValues[3] = ""
        self.ui.msrMinEdit.setText(msrValues[0])
        self.ui.msrMaxEdit.setText(msrValues[1])    
        self.ui.msrMeanEdit.setText(msrValues[2])   
        self.ui.msrShapeEdit.setText(msrValues[3])  
        law = lines[4].split('[')[0].split(' ')[-1]

        print "law:",law
        if law == "UN":
            self.ui.msrUnifRadio.setChecked(True)
        elif law == "LN": 
            self.ui.msrLogRadio.setChecked(True)
        else:
            self.ui.msrGammaRadio.setChecked(True)

        ilsrValues = lines[5].split('[')[1].split(']')[0].split(',')
        if ilsrValues[2] == "-9":
            ilsrValues[2] = ""
        if ilsrValues[3] == "2":
            ilsrValues[3] = ""
        self.ui.ilsrMinEdit.setText(ilsrValues[0])
        self.ui.ilsrMaxEdit.setText(ilsrValues[1])
        self.ui.ilsrMeanEdit.setText(ilsrValues[2])
        self.ui.ilsrShapeEdit.setText(ilsrValues[3])


    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))



