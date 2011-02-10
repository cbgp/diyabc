# -*- coding: utf-8 -*-

import os
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from preferences_ui import Ui_MainWindow

class Preferences(QMainWindow):
    """ Classe principale qui est aussi la fenêtre principale de la GUI
    """
    def __init__(self,parent=None):
        super(Preferences,self).__init__(parent)
        self.createWidgets()

        self.ui.tabWidget.setTabText(0,"Connexion")
        self.ui.tabWidget.setTabText(1,"def mut mod M")
        self.ui.tabWidget.setTabText(2,"def mut mod S")


    def createWidgets(self):
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        self.ui.gridLayout.setAlignment(Qt.AlignTop)
        self.ui.verticalLayout_57.setAlignment(Qt.AlignTop)
        self.ui.verticalLayout_54.setAlignment(Qt.AlignTop)
        self.ui.verticalLayout_29.setAlignment(Qt.AlignTop)
        self.ui.verticalLayout_26.setAlignment(Qt.AlignTop)

        QObject.connect(self.ui.saveButton,SIGNAL("clicked()"),self.savePreferences)
        QObject.connect(self.ui.cancelButton,SIGNAL("clicked()"),self.close)

        #QObject.connect(self.ui.saveMMMButton,SIGNAL("clicked()"),self.saveMMM)
        #QObject.connect(self.ui.saveMMSButton,SIGNAL("clicked()"),self.saveMMS)

    def savePreferences(self):
        self.saveMMM()
        self.saveMMS()
        self.saveConnexion()
        self.saveHM()
        self.close()

    # TODO
    def saveHM(self):
        pass

    def saveMMM(self):
        if not os.path.exists(os.path.expanduser("~/.diyabc/")):
            os.mkdir(os.path.expanduser("~/.diyabc/"))
        else:
            if os.path.exists(os.path.expanduser("~/.diyabc/mutationM_default_values")):
                os.remove(os.path.expanduser("~/.diyabc/mutationM_default_values"))

        lines = self.getMutationConf()
        f = codecs.open(os.path.expanduser("~/.diyabc/mutationM_default_values"),"w","utf-8")
        f.write(lines)
        f.close()



    def saveMMS(self):
        pass

    def saveConnexion(self):
        pass

    def loadPreferences(self):
        if os.path.exists(os.path.expanduser("~/.diyabc/")):
            self.loadMMM()
            self.loadMMS()
            self.loadConnexion()
            self.loadHM()

    def loadMMM(self):
        if os.path.exists(os.path.expanduser("~/.diyabc/mutationM_default_values")):
            f = codecs.open(os.path.expanduser("~/.diyabc/mutationM_default_values"),"r","utf-8")
            lines = f.readlines()
            f.close()
            self.setMutationConf(lines)
        else:
            print "no MMM conf found"

    def loadMMS(self):
        pass
    def loadConnexion(self):
        pass
    def loadHM(self):
        pass
        

    def getMutationConf(self):
        """ renvoie les lignes à écrire dans la conf MMM
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
        """ set les valeurs depuis la conf MMM
        """
        print "lines[0] : %s"%lines[0]
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

        #print "law:",law
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

        #print "law:",law
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

        #print "law:",law
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
