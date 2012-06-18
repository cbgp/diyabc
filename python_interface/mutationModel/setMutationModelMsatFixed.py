# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.setMutationModelMsat_ui import Ui_Frame

formSetMutationModelMsatFixed,baseSetMutationModelMsatFixed = uic.loadUiType("uis/setMutationModelMsat.ui")

class SetMutationModelMsatFixed(formSetMutationModelMsatFixed,baseSetMutationModelMsatFixed):
    """ Classe qui utilise le fichier graphique des SetMutationModelMsat.
    On ne remplit ici qu'une valeur fixe pour chaque ligne
    """
    def __init__(self,parent=None,box_group=None):
        super(SetMutationModelMsatFixed,self).__init__(parent)
        self.parent=parent
        self.box_group = box_group
        self.createWidgets()

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        self.ui.mmrMaxEdit.hide()
        self.ui.mmrMeanEdit.hide()
        self.ui.mmrShapeEdit.hide()
        self.ui.ilmrMaxEdit.hide()
        self.ui.ilmrMeanEdit.hide()
        self.ui.ilmrShapeEdit.hide()
        self.ui.mcpMaxEdit.hide()
        self.ui.mcpMeanEdit.hide()
        self.ui.mcpShapeEdit.hide()
        self.ui.ilcpMaxEdit.hide()
        self.ui.ilcpMeanEdit.hide()
        self.ui.ilcpShapeEdit.hide()
        self.ui.msrMaxEdit.hide()
        self.ui.msrMeanEdit.hide()
        self.ui.msrShapeEdit.hide()
        self.ui.ilsrMaxEdit.hide()
        self.ui.ilsrMeanEdit.hide()
        self.ui.ilsrShapeEdit.hide()
        self.ui.label_19.hide()
        self.ui.label_14.hide()
        self.ui.label_29.hide()
        self.ui.label_24.hide()
        self.ui.label_9.hide()
        self.ui.label_4.hide()
        self.ui.label_20.hide()
        self.ui.label_21.hide()
        self.ui.label_22.hide()
        self.ui.label_17.hide()
        self.ui.label_16.hide()
        self.ui.label_15.hide()
        self.ui.label_5.hide()
        self.ui.label_6.hide()
        self.ui.label_7.hide()
        self.ui.label_10.hide()
        self.ui.label_11.hide()
        self.ui.label_12.hide()
        self.ui.label_25.hide()
        self.ui.label_26.hide()
        self.ui.label_27.hide()
        self.ui.label_30.hide()
        self.ui.label_31.hide()
        self.ui.label_32.hide()
        #self.ui.label_35.hide()
        self.ui.label_34.setText(str(self.ui.label_34.text()).replace("the maximum ",""))
        self.ui.label_33.setText(str(self.ui.label_33.text()).replace("the maximum ",""))
        self.ui.groupBox.hide()
        self.ui.groupBox_2.hide()
        self.ui.groupBox_3.hide()
        self.ui.groupBox_4.hide()
        self.ui.groupBox_5.hide()
        self.ui.groupBox_6.hide()
        self.ui.clearButton.hide()

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)

        self.field_names_dico ={self.ui.mmrMinEdit : "mean mutation rate",
                            self.ui.ilmrMinEdit : "individual locus mutation rate",
                            self.ui.mcpMinEdit : "mean coefficient P",                                         
                            self.ui.ilcpMinEdit :"individual locus coefficient P",
                            self.ui.msrMinEdit : "mean SNI rate",
                            self.ui.ilsrMinEdit :"individual locus SNI rate"}

        # [doit_etre_non_vide , doit_être_float]
        self.constraints_dico = { self.ui.mmrMinEdit : [1,1],
                           self.ui.ilmrMinEdit : [1,1],  
                           self.ui.mcpMinEdit : [1,1],   
                           self.ui.ilcpMinEdit : [1,1],  
                           self.ui.msrMinEdit : [1,1],   
                           self.ui.ilsrMinEdit : [1,1]}

        self.ui.ilmrLabel.setText("%s\n(shape of the gamma)\n(1)"%self.ui.ilmrLabel.text())
        self.ui.mcpLabel.setText("%s\n(2)"%self.ui.mcpLabel.text())
        self.ui.msrLabel.setText("%s (3)"%self.ui.msrLabel.text())
        self.ui.ilcpLabel.setText("%s\n(shape of the gamma)\n(1)"%self.ui.ilcpLabel.text())
        self.ui.ilsrLabel.setText("%s\n(shape of the gamma)\n(1)"%self.ui.ilsrLabel.text())
        self.ui.ilmrLabel.setMinimumSize(QtCore.QSize(150,0)) 
        self.ui.ilcpLabel.setMinimumSize(QtCore.QSize(150,0))
        self.ui.ilsrLabel.setMinimumSize(QtCore.QSize(150,0))

        for field in self.field_names_dico.keys():
            field.setMaximumSize(QtCore.QSize(100, 25))

        self.ui.mmrFrame.setMaximumSize(QtCore.QSize(250, 80))
        self.ui.ilmrFrame.setMaximumSize(QtCore.QSize(250, 80))
        self.ui.mcpFrame.setMaximumSize(QtCore.QSize(250, 80))
        self.ui.ilcpFrame.setMaximumSize(QtCore.QSize(250, 80))
        self.ui.msrFrame.setMaximumSize(QtCore.QSize(250, 80))
        self.ui.ilsrFrame.setMaximumSize(QtCore.QSize(250, 80))

    def getMutationConf(self):
        """ renvoie les valeurs actuelles
        """
        l_result = []
        l_result.append(str(self.ui.mmrMinEdit.text()))
        l_result.append(str(self.ui.ilmrMinEdit.text()))
        l_result.append(str(self.ui.mcpMinEdit.text()))
        l_result.append(str(self.ui.ilcpMinEdit.text()))
        l_result.append(str(self.ui.msrMinEdit.text()))
        l_result.append(str(self.ui.ilsrMinEdit.text()))

        return l_result

    def setMutationConfFromAnalysisValues(self,v):
        self.ui.mmrMinEdit.setText("%s"%v[0])
        self.ui.ilmrMinEdit.setText("%s"%v[1])
        self.ui.ilcpMinEdit.setText("%s"%v[2])
        self.ui.mcpMinEdit.setText("%s"%v[3])
        self.ui.msrMinEdit.setText("%s"%v[4])
        self.ui.ilsrMinEdit.setText("%s"%v[5])

    def setMutationConf(self,lines):
        """ set les valeurs depuis la conf
        """
        #print "lines[0] : %s"%lines[0]
        mmrValues = lines[0].split('[')[1].split(']')[0].split(',')
        valmmr = (float(mmrValues[0]) + float(mmrValues[1]) )/2
        self.ui.mmrMinEdit.setText("%s"%valmmr)

        ilmrValues = lines[1].split('[')[1].split(']')[0].split(',')
        #valilmr = (float(ilmrValues[0]) + float(ilmrValues[1]) )/2
        #self.ui.ilmrMinEdit.setText("%s"%valilmr)
        self.ui.ilmrMinEdit.setText("2")

        ilcpValues = lines[3].split('[')[1].split(']')[0].split(',')
        #valilcp = (float(ilcpValues[0]) + float(ilcpValues[1]) )/2
        #self.ui.ilcpMinEdit.setText("%s"%valilcp)
        self.ui.ilcpMinEdit.setText("2")

        mcpValues = lines[2].split('[')[1].split(']')[0].split(',')
        valmcp = (float(mcpValues[0]) + float(mcpValues[1]) )/2
        self.ui.mcpMinEdit.setText("%s"%valmcp)

        msrValues = lines[4].split('[')[1].split(']')[0].split(',')
        valmsr = (float(msrValues[0]) + float(msrValues[1]) )/2
        self.ui.msrMinEdit.setText("%s"%valmsr)

        ilsrValues = lines[5].split('[')[1].split(']')[0].split(',')
        #valilsr = (float(ilsrValues[0]) + float(ilsrValues[1]) )/2
        #self.ui.ilsrMinEdit.setText("%s"%valilsr)
        self.ui.ilsrMinEdit.setText("2")

    def exit(self):
        ## reactivation des onglets
        #self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))
        self.parent.parent.ui.analysisStack.removeWidget(self)
        #self.parent.parent.ui.analysisStack.setCurrentIndex(0)
        self.parent.parent.ui.analysisStack.setCurrentWidget(self.parent)

    def validate(self):
        """ vérifie la validité des informations entrées dans le mutation model
        retourne au setGen tab et set la validité du mutation model du groupe
        """
        if self.allValid():
            self.exit()
            self.parent.setMutationValid_dico[self.box_group] = True

    def allValid(self,silent=False):
        """ vérifie chaque zone de saisie, si un probleme est présent, affiche un message pointant l'erreur
        et retourne False
        """
        try:
            for field in self.constraints_dico.keys():
                #print self.field_names_dico[field]
                valtxt = (u"%s"%(field.text())).strip()
                if self.constraints_dico[field][0] == 1:
                    if valtxt == "":
                        raise Exception("%s should not be empty"%self.field_names_dico[field])
                if self.constraints_dico[field][1] == 1:
                    val = float(valtxt)
            # verifs des min et max
            if float(self.ui.ilsrMinEdit.text()) < 0:
                raise Exception("Individual locus SNI rate should not be negative")
            if float(self.ui.msrMinEdit.text()) < 0:
                raise Exception("Mean SNI rate should be negative")
            if float(self.ui.ilcpMinEdit.text()) < 0:
                raise Exception("Individual locus coefficient P should not be negative")
            if float(self.ui.mcpMinEdit.text()) < 0:
                raise Exception("Mean coefficient P not should be negative")
            if float(self.ui.ilmrMinEdit.text()) <= 0:
                raise Exception("Individual locus mutation rate should be positive")
            if float(self.ui.mmrMinEdit.text()) < 0:
                raise Exception("Mean mutation rate should not be negative")
        except Exception as e:
            QMessageBox.information(self,"Value error","%s"%e)
            return False

        return True


