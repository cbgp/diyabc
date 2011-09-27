# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic

formSetMutationModelMsatFixed,baseSetMutationModelMsatFixed = uic.loadUiType("uis/setMutationModelMsat.ui")

class SetMutationModelMsatSimulation(formSetMutationModelMsatFixed,baseSetMutationModelMsatFixed):
    """ Classe qui utilise le fichier graphique des SetMutationModelMsat.
    On ne remplit ici qu'une valeur fixe pour chaque ligne
    """
    def __init__(self,parent=None,box_group=None):
        super(SetMutationModelMsatSimulation,self).__init__(parent)
        self.parent=parent
        self.box_group = box_group
        self.createWidgets()

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        self.ui.mmrMaxEdit.hide()
        self.ui.mmrMeanEdit.hide()
        self.ui.mmrShapeEdit.hide()
        self.ui.mcpMaxEdit.hide()
        self.ui.mcpMeanEdit.hide()
        self.ui.mcpShapeEdit.hide()
        self.ui.msrMaxEdit.hide()
        self.ui.msrMeanEdit.hide()
        self.ui.msrShapeEdit.hide()
        self.ui.label_19.hide()
        self.ui.label_24.hide()
        self.ui.label_4.hide()
        self.ui.label_20.hide()
        self.ui.label_21.hide()
        self.ui.label_22.hide()
        self.ui.label_5.hide()
        self.ui.label_6.hide()
        self.ui.label_7.hide()
        self.ui.label_25.hide()
        self.ui.label_26.hide()
        self.ui.label_27.hide()
        #self.ui.label_35.hide()
        self.ui.label_34.setText(str(self.ui.label_34.text()).replace("the maximum ",""))
        self.ui.label_33.setText(str(self.ui.label_33.text()).replace("the maximum ",""))
        self.ui.groupBox.hide()
        #self.ui.groupBox_2.hide()
        #self.ui.groupBox_3.hide()
        self.ui.groupBox_4.hide()
        self.ui.groupBox_5.hide()
        #self.ui.groupBox_6.hide()
        self.ui.clearButton.hide()

        self.ui.mmrMinEdit.setText("5.00E-4")
        self.ui.ilmrShapeEdit.setText("2")
        self.ui.mcpMinEdit.setText("0.22")
        self.ui.ilcpShapeEdit.setText("2")
        self.ui.msrMinEdit.setText("1.00E-8")
        self.ui.ilsrShapeEdit.setText("2")

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)

        self.field_names_dico ={self.ui.mmrMinEdit : "Mean mutation rate",

                            self.ui.ilmrMinEdit : "Min of individual locus mutation rate",
                            self.ui.ilmrMaxEdit : "Max of individual locus mutation rate" ,
                            self.ui.ilmrMeanEdit : "Mean of individual locus mutation rate",
                            self.ui.ilmrShapeEdit : "Shape of individual locus mutation rate",

                            self.ui.mcpMinEdit : "Mean coefficient P",
                                             
                            self.ui.ilcpMinEdit :"Min of individual locus coefficient P",
                            self.ui.ilcpMaxEdit :"Max of individual locus coefficient P",
                            self.ui.ilcpMeanEdit:"Mean of individual locus coefficient P", 
                            self.ui.ilcpShapeEdit:"Shape of individual locus coefficient P",
                                             
                            self.ui.msrMinEdit : "Mean SNI rate",
                                             
                            self.ui.ilsrMinEdit :"Min of individual locus SNI rate",
                            self.ui.ilsrMaxEdit :"Max of individual locus SNI rate",
                            self.ui.ilsrMeanEdit :"Mean of individual locus SNI rate",
                            self.ui.ilsrShapeEdit:"Shape of individual locus SNI rate"}

        # [doit_etre_non_vide , doit_être_float, doit être > 0]
        self.constraints_dico = { self.ui.mmrMinEdit : [1,1,1],

                           self.ui.ilmrMinEdit : [1,1,1],  
                           self.ui.ilmrMaxEdit : [1,1,1], 
                           self.ui.ilmrMeanEdit : [1,0,0], 
                           self.ui.ilmrShapeEdit : [1,1,0],

                           self.ui.mcpMinEdit : [1,1,0],   

                           self.ui.ilcpMinEdit : [1,1,0],  
                           self.ui.ilcpMaxEdit : [1,1,0], 
                           self.ui.ilcpMeanEdit : [1,0,0], 
                           self.ui.ilcpShapeEdit : [1,1,0],

                           self.ui.msrMinEdit : [1,1,0],   

                           self.ui.ilsrMinEdit : [1,1,0],  
                           self.ui.ilsrMaxEdit : [1,1,0],  
                           self.ui.ilsrMeanEdit : [1,0,0],
                           self.ui.ilsrShapeEdit : [1,1,0]}

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

        self.ui.frame_5.setMaximumSize(QtCore.QSize(250, 80))
        self.ui.frame_2.setMaximumSize(QtCore.QSize(250, 80))
        #self.ui.frame_3.setMaximumSize(QtCore.QSize(250, 80))
        #self.ui.frame_4.setMaximumSize(QtCore.QSize(250, 80))
        self.ui.frame_7.setMaximumSize(QtCore.QSize(250, 80))
        #self.ui.frame_8.setMaximumSize(QtCore.QSize(250, 80))

    def getMutationConf(self):
        """ renvoie les valeurs actuelles
        """
        s_result = u""
        s_result += u"MEANMU %s\n"%self.ui.mmrMinEdit.text()
        s_result += u"GAMMU GA[%s,"%self.ui.ilmrMinEdit.text()
        s_result += u"%s,"%self.ui.ilmrMaxEdit.text()
        s_result += u"%s,"%self.ui.ilmrMeanEdit.text()
        s_result += u"%s]\n"%self.ui.ilmrShapeEdit.text()
        s_result += u"MEANP %s\n"%self.ui.mcpMinEdit.text()
        s_result += u"GAMP GA[%s,"%self.ui.ilcpMinEdit.text()
        s_result += u"%s,"%self.ui.ilcpMaxEdit.text()
        s_result += u"%s,"%self.ui.ilcpMeanEdit.text()
        s_result += u"%s]\n"%self.ui.ilcpShapeEdit.text()
        s_result += u"MEANSNI %s\n"%self.ui.msrMinEdit.text()
        s_result += u"GAMSNI GA[%s,"%self.ui.ilsrMinEdit.text()
        s_result += u"%s,"%self.ui.ilsrMaxEdit.text()
        s_result += u"%s,"%self.ui.ilsrMeanEdit.text()
        s_result += u"%s]\n"%self.ui.ilsrShapeEdit.text()

        return s_result

    def exit(self):
        self.parent.switchTo(self.parent)

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
                    if self.constraints_dico[field][2] == 1:
                        if val <= 0:
                            raise Exception("%s should be positive"%self.field_names_dico[field])
                    else:
                        if val < 0:
                            raise Exception("%s should not be negative"%self.field_names_dico[field])
        except Exception,e:
            QMessageBox.information(self,"Value error","%s"%e)
            return False

        return True


