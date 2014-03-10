# -*- coding: utf-8 -*-

import sys, codecs
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.setMutationModelSequences_ui import Ui_Frame
import variables
from variables import UIPATH

formSetMutationModelSequencesFixed,baseSetMutationModelSequencesFixed = uic.loadUiType((u"%s/setMutationModelSequences.ui"%UIPATH).encode(sys.getfilesystemencoding()))

class SetMutationModelSequencesFixed(formSetMutationModelSequencesFixed,baseSetMutationModelSequencesFixed):
    """ Classe qui utilise le fichier graphique des SetMutationModelSequences.
    On ne remplit ici qu'une valeur fixe pour chaque ligne
    """
    def __init__(self,parent=None,box_group=None):
        super(SetMutationModelSequencesFixed,self).__init__(parent)
        self.parent=parent
        self.box_group = box_group
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
        self.ui=self
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.tamuraRadio,SIGNAL("clicked()"),self.showTamura)
        QObject.connect(self.ui.jukesRadio,SIGNAL("clicked()"),self.showJukes)
        QObject.connect(self.ui.haseRadio,SIGNAL("clicked()"),self.showKimuraHase)
        QObject.connect(self.ui.kimuraRadio,SIGNAL("clicked()"),self.showKimuraHase)

        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)

        self.field_names_dico = {
        self.ui.mmrMinEdit:"mean mutation rate",
        self.ui.ilmrMinEdit:"individual locus mutation rate",
        self.ui.mc1MinEdit:" mean coefficient k_C/T",
        self.ui.ilc1MinEdit:"individual locus coefficient k_C/T",
        self.ui.mc2MinEdit:"mean coefficient k_A/G",
        self.ui.ilc2MinEdit:"individual locus coefficient k_A/G"
        }

        self.constraints_dico = {
        self.ui.mmrMinEdit:[1,1],
        self.ui.ilmrMinEdit:[1,1],
        self.ui.mc1MinEdit:[1,1],
        self.ui.ilc1MinEdit:[1,1],
        self.ui.mc2MinEdit:[1,1],
        self.ui.ilc2MinEdit:[1,1]
        }

        self.ui.mmrMaxEdit.hide()
        self.ui.mmrMeanEdit.hide()
        self.ui.mmrShapeEdit.hide()
        self.ui.ilmrMaxEdit.hide()
        self.ui.ilmrMeanEdit.hide()
        self.ui.ilmrShapeEdit.hide()
        self.ui.mc1MaxEdit.hide()
        self.ui.mc1MeanEdit.hide()
        self.ui.mc1ShapeEdit.hide()
        self.ui.ilc1MaxEdit.hide()
        self.ui.ilc1MeanEdit.hide()
        self.ui.ilc1ShapeEdit.hide()
        self.ui.mc2MaxEdit.hide()
        self.ui.mc2MeanEdit.hide()
        self.ui.mc2ShapeEdit.hide()
        self.ui.ilc2MaxEdit.hide()
        self.ui.ilc2MeanEdit.hide()
        self.ui.ilc2ShapeEdit.hide()
        self.ui.groupBox.hide()
        self.ui.groupBox_2.hide()
        self.ui.groupBox_3.hide()
        self.ui.groupBox_4.hide()
        self.ui.groupBox_5.hide()
        self.ui.groupBox_6.hide()
        self.ui.label_20.hide()
        self.ui.label_21.hide()
        self.ui.label_22.hide()
        self.ui.label_15.hide()
        self.ui.label_16.hide()
        self.ui.label_17.hide()
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
        self.ui.label_14.hide()
        self.ui.label_4.hide()
        self.ui.label_24.hide()
        self.ui.label_19.hide()
        self.ui.label_9.hide()
        self.ui.label_29.hide()
        #self.ui.label_35.hide()
        self.ui.clearButton.hide()

        self.ui.ilmrLabel.setText("%s\n(shape of the gamma)\n(1)"%self.ui.ilmrLabel.text())
        self.ui.ilc1Label.setText("%s\n(shape of the gamma)\n(1)"%self.ui.ilc1Label.text())
        self.ui.ilc2Label.setText("%s\n(shape of the gamma)\n(1)"%self.ui.ilc2Label.text())
        self.ui.ilmrLabel.setMinimumSize(QtCore.QSize(150,0))
        self.ui.ilc1Label.setMinimumSize(QtCore.QSize(150,0))
        self.ui.ilc2Label.setMinimumSize(QtCore.QSize(150,0))

        for field in self.field_names_dico.keys():
            field.setMaximumSize(QtCore.QSize(100, 25))

        self.ui.mmrFrame.setMaximumSize(QtCore.QSize(250,100))
        self.ui.ilmrFrame.setMaximumSize(QtCore.QSize(250,100))
        self.ui.mc1Frame.setMaximumSize(QtCore.QSize(250,100))
        self.ui.ilc1Frame.setMaximumSize(QtCore.QSize(250,100))
        self.ui.mc2Frame.setMaximumSize(QtCore.QSize(250,100))
        self.ui.ilc2Frame.setMaximumSize(QtCore.QSize(250,100))


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
            self.parent.setMutationSeqValid_dico[self.box_group] = True


    def showJukes(self):
        """ clic sur le modèle jukes kantor, montre les deux premières lignes
        """
        for prefix in ["mmr","ilmr"]:
            exec("self.ui.%sFrame.show()"%prefix)
        for prefix in ["mc1","ilc1","mc2","ilc2"]:
            exec("self.ui.%sFrame.hide()"%prefix)
    def showKimuraHase(self):
        """ clic sur le modèle kimura ou hase, montre les quatres premières lignes
        """
        for prefix in ["mmr","ilmr","mc1","ilc1"]:
            exec("self.ui.%sFrame.show()"%prefix)
        for prefix in ["mc2","ilc2"]:
            exec("self.ui.%sFrame.hide()"%prefix)
    def showTamura(self):
        """ clic sur le modèle tamura, montre toutes les lignes
        """
        for prefix in ["mmr","ilmr","mc1","ilc1","mc2","ilc2"]:
            exec("self.ui.%sFrame.show()"%prefix)

    def getMutationConf(self):
        """ renvoie les valeurs actuelles
        """
        l_result = []
        l_result.append(str(self.ui.mmrMinEdit.text()))
        l_result.append(str(self.ui.ilmrMinEdit.text()))
        if self.hasToBeVerified(self.ui.mc1MinEdit):
            l_result.append(str(self.ui.mc1MinEdit.text()))
            l_result.append(str(self.ui.ilc1MinEdit.text()))
            if self.hasToBeVerified(self.ui.mc2MinEdit):
                l_result.append(str(self.ui.mc2MinEdit.text()))
                l_result.append(str(self.ui.ilc2MinEdit.text()))

        return l_result

    def setMutationConfFromAnalysisValues(self,v):
        self.ui.mmrMinEdit.setText("%s"%v[0])
        self.ui.ilmrMinEdit.setText("%s"%v[1])
        if len(v) > 2:
            self.ui.mc1MinEdit.setText("%s"%v[2])
            self.ui.ilc1MinEdit.setText("%s"%v[3])
            if len(v) > 4:
                self.ui.mc2MinEdit.setText("%s"%v[4])
                self.ui.ilc2MinEdit.setText("%s"%v[5])

    def setMutationConf(self,lines):
        """ set les valeurs depuis la conf
        """

        mmrValues = lines[0].split('[')[1].split(']')[0].split(',')
        valmmr = (float(mmrValues[0]) + float(mmrValues[1]) )/2
        self.ui.mmrMinEdit.setText("%s"%valmmr)

        ilmrValues = lines[1].split('[')[1].split(']')[0].split(',')
        #valilmr = (float(ilmrValues[0]) + float(ilmrValues[1]) )/2
        #self.ui.ilmrMinEdit.setText("%s"%valilmr)
        self.ui.ilmrMinEdit.setText("2")

        mc2Values = lines[4].split('[')[1].split(']')[0].split(',')
        valmc2 = (float(mc2Values[0]) + float(mc2Values[1]) )/2
        self.ui.mc2MinEdit.setText("%s"%valmc2)

        mc1Values = lines[2].split('[')[1].split(']')[0].split(',')
        valmc1 = (float(mc1Values[0]) + float(mc1Values[1]) )/2
        self.ui.mc1MinEdit.setText("%s"%valmc1)

        ilc1Values = lines[3].split('[')[1].split(']')[0].split(',')
        #valilc1 = (float(ilc1Values[0]) + float(ilc1Values[1]) )/2
        #self.ui.ilc1MinEdit.setText("%s"%valilc1)
        self.ui.ilc1MinEdit.setText("2")

        ilc2Values = lines[5].split('[')[1].split(']')[0].split(',')
        #valilc2 = (float(ilc2Values[0]) + float(ilc2Values[1]) )/2
        #self.ui.ilc2MinEdit.setText("%s"%valilc2)
        self.ui.ilc2MinEdit.setText("2")

        model = lines[6].split()[1]
        iss = lines[6].split()[2]
        sotg = lines[6].split()[3]
        self.ui.isEdit.setText(iss)
        self.ui.sotgEdit.setText(sotg)
        if model == "K2P":
            self.ui.kimuraRadio.setChecked(True)
            self.showKimuraHase()
        elif model == "TN":
            self.ui.tamuraRadio.setChecked(True)
            self.showTamura()
        elif model == "JK":
            self.ui.jukesRadio.setChecked(True)
            self.showJukes()
        elif model == "HKY":
            self.ui.haseRadio.setChecked(True)
            self.showKimuraHase()



    def allValid(self,silent=False):
        """ vérifie chaque zone de saisie, si un probleme est présent, affiche un message pointant l'erreur
        et retourne False
        """
        try:
            if self.hasToBeVerified(self.ui.mmrMinEdit) and float(self.ui.mmrMinEdit.text()) <= 0:
                raise Exception("Mean mutation rate should be positive")
            elif self.hasToBeVerified(self.ui.ilmrMinEdit) and float(self.ui.ilmrMinEdit.text()) < 0:
                raise Exception("Individual locus mutation rate should not be negative")
            elif self.hasToBeVerified(self.ui.mc1MinEdit) and float(self.ui.mc1MinEdit.text()) <= 0:
                raise Exception("Mean coefficient k_C/T should be positive")
            elif self.hasToBeVerified(self.ui.ilc1MinEdit) and float(self.ui.ilc1MinEdit.text()) < 0:
                raise Exception("Individual locus coefficient k_C/T should not be negative")
            elif self.hasToBeVerified(self.ui.mc2MinEdit) and float(self.ui.mc2MinEdit.text()) <= 0:
                raise Exception("Mean coefficient k_A/G should be positive")
            elif self.hasToBeVerified(self.ui.ilc2MinEdit) and float(self.ui.ilc2MinEdit.text()) < 0:
                raise Exception("Individual locus coefficient k_A/G should not be negative")
            for field in self.constraints_dico.keys():
                #print self.field_names_dico[field]
                if self.hasToBeVerified(field):
                    valtxt = (u"%s"%(field.text())).strip()
                    if self.constraints_dico[field][0] == 1:
                        if valtxt == "":
                            raise Exception("%s should not be empty"%self.field_names_dico[field])
                    if self.constraints_dico[field][1] == 1:
                        val = float(valtxt)
        except Exception as e:
            QMessageBox.information(self,"Value error","%s"%e)
            return False

        return True

    def hasToBeVerified(self,field):
        """ détermine si field doit être vérifié en fonction du modèle choisi
        """
        if self.ui.jukesRadio.isChecked() and field not in [self.ui.mmrMinEdit,self.ui.mmrMaxEdit,
                self.ui.mmrMeanEdit,self.ui.mmrShapeEdit,self.ui.ilmrMinEdit,self.ui.ilmrMaxEdit,self.ui.ilmrMeanEdit,self.ui.ilmrShapeEdit]:
            return False
        elif not self.ui.tamuraRadio.isChecked() and field in [self.ui.mc2MinEdit,self.ui.mc2MaxEdit,
                self.ui.mc2MeanEdit,self.ui.mc2ShapeEdit,self.ui.ilc2MinEdit,self.ui.ilc2MaxEdit,self.ui.ilc2MeanEdit,self.ui.ilc2ShapeEdit]:
            return False
        else:
            return True

    def getNbParam(self):
        """ retourne le nombre de paramètres (MEAN dont le min<max) pour ce mutation model
        """
        nb_param = 0
        if float(str(self.ui.mmrMinEdit.text())) < float(str(self.ui.mmrMaxEdit.text())):
            nb_param += 1
        if not self.ui.jukesRadio.isChecked():
            if float(str(self.ui.mc1MinEdit.text())) < float(str(self.ui.mc1MaxEdit.text())):
                nb_param += 1
            if self.ui.tamuraRadio.isChecked():
                if float(str(self.ui.mc2MinEdit.text())) < float(str(self.ui.mc2MaxEdit.text())):
                    nb_param += 1
        return nb_param




