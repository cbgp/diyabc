# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setMutationModel_ui import Ui_Frame

class SetMutationModelFixed(QFrame):
    def __init__(self,parent=None,box_group=None):
        super(SetMutationModelFixed,self).__init__(parent)
        self.parent=parent
        self.box_group = box_group
        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_Frame()
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
        self.ui.label_35.hide()
        self.ui.label_34.hide()
        self.ui.label_33.hide()
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

        for field in self.field_names_dico.keys():
            field.setMaximumSize(QtCore.QSize(100, 25))

        self.ui.frame_5.setMaximumSize(QtCore.QSize(250, 50))
        self.ui.frame_2.setMaximumSize(QtCore.QSize(250, 50))
        self.ui.frame_3.setMaximumSize(QtCore.QSize(250, 50))
        self.ui.frame_4.setMaximumSize(QtCore.QSize(250, 50))
        self.ui.frame_7.setMaximumSize(QtCore.QSize(250, 50))
        self.ui.frame_8.setMaximumSize(QtCore.QSize(250, 50))

    def getMutationConf(self):
        """ renvoie les lignes à écrire dans la conf
        """
        l_result = []
        l_result.append(str(self.ui.mmrMinEdit.text()))
        l_result.append(str(self.ui.ilmrMinEdit.text()))
        l_result.append(str(self.ui.mcpMinEdit.text()))
        l_result.append(str(self.ui.ilcpMinEdit.text()))
        l_result.append(str(self.ui.msrMinEdit.text()))
        l_result.append(str(self.ui.ilsrMinEdit.text()))

        return l_result

    def setMutationConf(self,lines):
        """ set les valeurs depuis la conf
        """
        print "lines[0] : %s"%lines[0]
        mmrValues = lines[0].split('[')[1].split(']')[0].split(',')
        self.ui.mmrMinEdit.setText(mmrValues[0])
        self.ui.mmrMaxEdit.setText(mmrValues[1])    
        valmmr = (float(mmrValues[0]) + float(mmrValues[1]) )/2
        self.ui.mmrMinEdit.setText("%s"%valmmr)

        ilmrValues = lines[1].split('[')[1].split(']')[0].split(',')
        self.ui.ilmrMinEdit.setText(ilmrValues[0])
        self.ui.ilmrMaxEdit.setText(ilmrValues[1])    
        valilmr = (float(ilmrValues[0]) + float(ilmrValues[1]) )/2
        self.ui.ilmrMinEdit.setText("%s"%valilmr)

        ilcpValues = lines[3].split('[')[1].split(']')[0].split(',')
        self.ui.ilcpMinEdit.setText(ilcpValues[0])
        self.ui.ilcpMaxEdit.setText(ilcpValues[1])    
        valilcp = (float(ilcpValues[0]) + float(ilcpValues[1]) )/2
        self.ui.ilcpMinEdit.setText("%s"%valilcp)

        mcpValues = lines[2].split('[')[1].split(']')[0].split(',')
        self.ui.mcpMinEdit.setText(mcpValues[0])
        self.ui.mcpMaxEdit.setText(mcpValues[1])    
        valmcp = (float(mcpValues[0]) + float(mcpValues[1]) )/2
        self.ui.mcpMinEdit.setText("%s"%valmcp)

        msrValues = lines[4].split('[')[1].split(']')[0].split(',')
        self.ui.msrMinEdit.setText(msrValues[0])
        self.ui.msrMaxEdit.setText(msrValues[1])    
        valmsr = (float(msrValues[0]) + float(msrValues[1]) )/2
        self.ui.msrMinEdit.setText("%s"%valmsr)

        ilsrValues = lines[5].split('[')[1].split(']')[0].split(',')
        self.ui.ilsrMinEdit.setText(ilsrValues[0])
        self.ui.ilsrMaxEdit.setText(ilsrValues[1])    
        valilsr = (float(ilsrValues[0]) + float(ilsrValues[1]) )/2
        self.ui.ilsrMinEdit.setText("%s"%valilsr)



    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))

    def validate(self):
        """ vérifie la validité des informations entrées dans le mutation model
        retourne au setGen tab et set la validité du mutation model du groupe
        """
        if self.allValid():
            self.exit()
            self.parent.setMutationValid_dico[self.box_group] = True

    def allValid(self):
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
            if float(self.ui.ilsrMinEdit.text()) >= 0:
                raise Exception("Individuals locus SNI rate should be positive")
            if float(self.ui.msrMinEdit.text()) >= 0:
                raise Exception("Mean SNI rate should be positive")
            if float(self.ui.ilcpMinEdit.text()) >= 0:
                raise Exception("Individuals locus coefficient P should be positive")
            if float(self.ui.mcpMinEdit.text()) >= 0:
                raise Exception("Mean coefficient P should be positive")
            if float(self.ui.ilmrMinEdit.text()) >= 0:
                raise Exception("Individuals locus mutation rate should be positive")
            if float(self.ui.mmrMinEdit.text()) >= 0:
                raise Exception("Mean mutation rate should be positive")
        except Exception,e:
            QMessageBox.information(self,"Value error","%s"%e)
            return False

        return True


