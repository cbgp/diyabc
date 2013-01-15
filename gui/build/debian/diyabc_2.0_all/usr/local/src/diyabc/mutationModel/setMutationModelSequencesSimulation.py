# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.setMutationModelSequences_ui import Ui_Frame

formSetMutationModelSequencesFixed,baseSetMutationModelSequencesFixed = uic.loadUiType("uis/setMutationModelSequences.ui")

class SetMutationModelSequencesSimulation(formSetMutationModelSequencesFixed,baseSetMutationModelSequencesFixed):
    """ Classe qui utilise le fichier graphique des SetMutationModelSequences.
    On ne remplit ici qu'une valeur fixe pour chaque ligne
    """
    def __init__(self,parent=None,box_group=None):
        super(SetMutationModelSequencesSimulation,self).__init__(parent)
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

        self.ui.mmrMinEdit.setText("1.00E-9")
        #self.ui.ilmrMinEdit.setText("2")
        self.ui.mc1MinEdit.setText("10")
        #self.ui.ilc1MinEdit.setText("2")
        self.ui.mc2MinEdit.setText("10")
        #self.ui.ilc2MinEdit.setText("2")

        self.field_names_dico = {
        self.ui.mmrMinEdit:"Min of mean mutation rate",

        self.ui.ilmrMinEdit:"Min of individual locus mutation rate",
        self.ui.ilmrMaxEdit:"Max of individual locus mutation rate",
        self.ui.ilmrMeanEdit:"Mean of individual locus mutation rate",
        self.ui.ilmrShapeEdit:"Shape of individual locus mutation rate",

        self.ui.mc1MinEdit:" Min of mean coefficient k_C/T",

        self.ui.ilc1MinEdit:"Min of individual locus coefficient k_C/T",
        self.ui.ilc1MaxEdit:"Max of individual locus coefficient k_C/T",
        self.ui.ilc1MeanEdit:"Mean of individual locus coefficient k_C/T",
        self.ui.ilc1ShapeEdit:"Shape of individual locus coefficient k_C/T",

        self.ui.mc2MinEdit:"Min of mean coefficient k_A/G",

        self.ui.ilc2MinEdit:"Min of individual locus coefficient k_A/G",
        self.ui.ilc2MaxEdit:"Max of individual locus coefficient k_A/G",
        self.ui.ilc2MeanEdit:"Mean of individual locus coefficient k_A/G",
        self.ui.ilc2ShapeEdit:"Shape of individual locus coefficient k_A/G"
        }

        self.constraints_dico = {
        self.ui.mmrMinEdit:[1,1,1],

        self.ui.ilmrMinEdit:[1,1,1],
        self.ui.ilmrMaxEdit:[1,1,1],
        self.ui.ilmrMeanEdit:[1,0,0],
        self.ui.ilmrShapeEdit:[1,1,0],

        self.ui.mc1MinEdit:[1,1,1],

        self.ui.ilc1MinEdit:[1,1,1],
        self.ui.ilc1MaxEdit:[1,1,1],
        self.ui.ilc1MeanEdit:[1,0,0],
        self.ui.ilc1ShapeEdit:[1,1,0],

        self.ui.mc2MinEdit:[1,1,1],

        self.ui.ilc2MinEdit:[1,1,1],
        self.ui.ilc2MaxEdit:[1,1,1],
        self.ui.ilc2MeanEdit:[1,0,0],
        self.ui.ilc2ShapeEdit:[1,1,0]
        }

        self.ui.mmrMaxEdit.hide()
        self.ui.mmrMeanEdit.hide()
        self.ui.mmrShapeEdit.hide()
        self.ui.mc1MaxEdit.hide()
        self.ui.mc1MeanEdit.hide()
        self.ui.mc1ShapeEdit.hide()
        self.ui.mc2MaxEdit.hide()
        self.ui.mc2MeanEdit.hide()
        self.ui.mc2ShapeEdit.hide()
        self.ui.groupBox.hide()
        #self.ui.groupBox_2.hide()
        #self.ui.groupBox_3.hide()
        self.ui.groupBox_4.hide()
        self.ui.groupBox_5.hide()
        #self.ui.groupBox_6.hide()
        self.ui.label_20.hide()
        self.ui.label_21.hide()
        self.ui.label_22.hide()
        #self.ui.label_15.hide()
        #self.ui.label_16.hide()
        #self.ui.label_17.hide()
        self.ui.label_5.hide()
        self.ui.label_6.hide()
        self.ui.label_7.hide()
        #self.ui.label_10.hide()
        #self.ui.label_11.hide()
        #self.ui.label_12.hide()
        self.ui.label_25.hide()
        self.ui.label_26.hide()
        self.ui.label_27.hide()
        #self.ui.label_30.hide()
        #self.ui.label_31.hide()
        #self.ui.label_32.hide()
        #self.ui.label_14.hide()
        self.ui.label_4.hide()
        self.ui.label_24.hide()
        self.ui.label_19.hide()
        #self.ui.label_9.hide()
        #self.ui.label_29.hide()
        #self.ui.label_35.hide()
        self.ui.clearButton.hide()

        self.ui.ilmrLabel.setText("%s\n(shape of the gamma)\n(1)"%self.ui.ilmrLabel.text())
        self.ui.ilc1Label.setText("%s\n(shape of the gamma)\n(1)"%self.ui.ilc1Label.text())
        self.ui.ilc2Label.setText("%s\n(shape of the gamma)\n(1)"%self.ui.ilc2Label.text())
        self.ui.ilmrLabel.setMinimumSize(QtCore.QSize(150,0)) 
        self.ui.ilc1Label.setMinimumSize(QtCore.QSize(150,0))
        self.ui.ilc2Label.setMinimumSize(QtCore.QSize(150,0))

        self.ui.groupBox_2.setMaximumSize(QtCore.QSize(120,9999))
        self.ui.groupBox_3.setMaximumSize(QtCore.QSize(120,9999))
        self.ui.groupBox_6.setMaximumSize(QtCore.QSize(120,9999))
        self.ui.groupBox_2.setMinimumSize(QtCore.QSize(110,0))
        self.ui.groupBox_3.setMinimumSize(QtCore.QSize(110,0))
        self.ui.groupBox_6.setMinimumSize(QtCore.QSize(110,0))

        for field in self.field_names_dico.keys():
            field.setMaximumSize(QtCore.QSize(100, 25))

        self.ui.mmrFrame.setMaximumSize(QtCore.QSize(250,100))
        #self.ui.secondFrame.setMaximumSize(QtCore.QSize(250,100))
        self.ui.mc1Frame.setMaximumSize(QtCore.QSize(250,100))
        #self.ui.fourthFrame.setMaximumSize(QtCore.QSize(250,100))
        self.ui.mc2Frame.setMaximumSize(QtCore.QSize(250,100))
        #self.ui.sixthFrame.setMaximumSize(QtCore.QSize(250,100))

    def exit(self):
        self.parent.switchTo(self.parent)

    def validate(self):
        """ vérifie la validité des informations entrées dans le mutation model
        retourne au setGen tab et set la validité du mutation model du groupe
        """
        if self.allValid():
            self.exit()
            self.parent.setMutationSeqValid_dico[self.box_group] = True
            set_mut_button = self.box_group.findChild(QPushButton,"setMutationButton")
            fontt = set_mut_button.font()
            fontt.setBold(False)
            set_mut_button.setStyleSheet("border-color: #000000")
            set_mut_button.setFont(fontt)

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
        s_result = u""
        s_result += u"MEANMU %s\n"%self.ui.mmrMinEdit.text()
        s_result += u"GAMMU GA[%s,"%self.ui.ilmrMinEdit.text()
        s_result += u"%s,"%self.ui.ilmrMaxEdit.text()
        s_result += u"%s,"%self.ui.ilmrMeanEdit.text()
        s_result += u"%s]\n"%self.ui.ilmrShapeEdit.text()
        if self.hasToBeVerified(self.ui.mc1MinEdit):
            s_result += u"MEANK1 %s\n"%self.ui.mc1MinEdit.text()
            s_result += u"GAMK1 GA[%s,"%self.ui.ilc1MinEdit.text()
            s_result += u"%s,"%self.ui.ilc1MaxEdit.text()
            s_result += u"%s,"%self.ui.ilc1MeanEdit.text()
            s_result += u"%s]\n"%self.ui.ilc1ShapeEdit.text()
            if self.hasToBeVerified(self.ui.mc2MinEdit):
                s_result += u"MEANK2 %s\n"%self.ui.mc2MinEdit.text()
                s_result += u"GAMK2 GA[%s,"%self.ui.ilc2MinEdit.text()
                s_result += u"%s,"%self.ui.ilc2MaxEdit.text()
                s_result += u"%s,"%self.ui.ilC2MeanEdit.text()
                s_result += u"%s]\n"%self.ui.ilc2ShapeEdit.text()

        if self.ui.jukesRadio.isChecked():
            model = "JK"
        elif self.ui.kimuraRadio.isChecked():
            model = "K2P"
        elif self.ui.haseRadio.isChecked():
            model = "HKY"
        elif self.ui.tamuraRadio.isChecked():
            model = "TN"
        s_result += "MODEL %s %s %s\n"%(model,str(self.ui.isEdit.text()).strip(),str(self.ui.sotgEdit.text()).strip())

        return s_result


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

        model = lines[6].split(' ')[1]
        iss = lines[6].split(' ')[2]
        sotg = lines[6].split(' ')[3]
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
            for field in self.constraints_dico.keys():
                #print self.field_names_dico[field]
                if self.hasToBeVerified(field):
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

