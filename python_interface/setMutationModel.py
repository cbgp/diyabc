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
        result += "MEANMU   %s[%s,%s,%s,%s]\n"%(law,mmrMin,mmrMax,mmrMean,mmrShape)
        
        ilmrMin =    str(self.ui.ilmrMinEdit.text())
        ilmrMax =    str(self.ui.ilmrMaxEdit.text())
        ilmrMean =   u'%s'%(self.ui.ilmrMeanEdit.text())
        ilmrShape =  str(self.ui.ilmrShapeEdit.text())
        if ilmrMean == "":
            ilmrMean = "-9"
        if ilmrShape == "":
            ilmrShape = "2"
        law = "GA"
        result += "GAMMU   %s[%s,%s,%s,%s]\n"%(law,ilmrMin,ilmrMax,ilmrMean,ilmrShape)

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
        result += "MEANP   %s[%s,%s,%s,%s]\n"%(law,mcpMin,mcpMax,mcpMean,mcpShape)

        ilcpMin =    str(self.ui.ilcpMinEdit.text())
        ilcpMax =    str(self.ui.ilcpMaxEdit.text())
        ilcpMean =   str(self.ui.ilcpMeanEdit.text())
        ilcpShape =  str(self.ui.ilcpShapeEdit.text())
        if ilcpMean == "":
            ilcpMean = "-9"
        if ilcpShape == "":
            ilcpShape = "2"

        law = "GA"
        result += "GAMP   %s[%s,%s,%s,%s]\n"%(law,ilcpMin,ilcpMax,ilcpMean,ilcpShape)

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
        result += "MEANSNI   %s[%s,%s,%s,%s]\n"%(law,msrMin,msrMax,msrMean,msrShape)

        ilsrMin =    str(self.ui.ilsrMinEdit.text())
        ilsrMax =    str(self.ui.ilsrMaxEdit.text())
        ilsrMean =   u'%s'%(self.ui.ilsrMeanEdit.text())
        ilsrShape =  str(self.ui.ilsrShapeEdit.text())
        if ilsrMean == "":
            ilsrMean = "-9"
        if ilsrShape == "":
            ilsrShape = "2"

        law = "GA"
        result += "GAMSNI   %s[%s,%s,%s,%s]\n"%(law,ilsrMin,ilsrMax,ilsrMean,ilsrShape)
        
        return result

    def setMutationConf(self,lines):
        """ set les valeurs depuis la conf
        """
        print lines

    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))



