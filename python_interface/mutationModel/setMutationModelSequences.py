# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.setMutationModelSequences_ui import Ui_Frame

formSetMutationModelSequences,baseSetMutationModelSequences = uic.loadUiType("uis/setMutationModelSequences.ui")

class SetMutationModelSequences(formSetMutationModelSequences,baseSetMutationModelSequences):
    """ définition du modèle mutationnel pour les séquences
    """
    def __init__(self,parent=None,box_group=None):
        super(SetMutationModelSequences,self).__init__(parent)
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

        QObject.connect(self.ui.clearButton,SIGNAL("clicked()"),self.clear)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)

        QObject.connect(self.ui.mmrGammaRadio,SIGNAL("toggled(bool)"),self.mmrGamma)
        QObject.connect(self.ui.mmrLogRadio,SIGNAL("toggled(bool)"),self.mmrNoGamma)
        QObject.connect(self.ui.mmrUnifRadio,SIGNAL("toggled(bool)"),self.mmrNoGamma)
        QObject.connect(self.ui.mc2GammaRadio,SIGNAL("toggled(bool)"),self.mc2Gamma)
        QObject.connect(self.ui.mc2LogRadio,SIGNAL("toggled(bool)"),self.mc2NoGamma)
        QObject.connect(self.ui.mc2UnifRadio,SIGNAL("toggled(bool)"),self.mc2NoGamma)
        QObject.connect(self.ui.mc1GammaRadio,SIGNAL("toggled(bool)"),self.mc1Gamma)
        QObject.connect(self.ui.mc1LogRadio,SIGNAL("toggled(bool)"),self.mc1NoGamma)
        QObject.connect(self.ui.mc1UnifRadio,SIGNAL("toggled(bool)"),self.mc1NoGamma)
        if self.ui.mc1GammaRadio.isChecked():
            self.mc1Gamma(True)
        else:
            self.mc1NoGamma(True)
        if self.ui.mmrGammaRadio.isChecked():
            self.mmrGamma(True)
        else:
            self.mmrNoGamma(True)
        if self.ui.mc2GammaRadio.isChecked():
            self.mc2Gamma(True)
        else:
            self.mc2NoGamma(True)

        self.field_names_dico = {
        self.ui.mmrMinEdit:"Min of mean mutation rate",
        self.ui.mmrMaxEdit:"Max of mean mutation rate",
        self.ui.mmrMeanEdit:"Mean of mean mutation rate",
        self.ui.mmrShapeEdit:"Shape of mean mutation rate",

        self.ui.ilmrMinEdit:"Min of individuals locus mutation rate",
        self.ui.ilmrMaxEdit:"Max of individuals locus mutation rate",
        self.ui.ilmrMeanEdit:"Mean of individuals locus mutation rate",
        self.ui.ilmrShapeEdit:"Shape of individuals locus mutation rate",

        self.ui.mc1MinEdit:" Min of mean coefficient k_C/T",
        self.ui.mc1MaxEdit:"Max of mean coefficient k_C/T",
        self.ui.mc1MeanEdit:"Mean of mean coefficient k_C/T",
        self.ui.mc1ShapeEdit:"Shape of mean coefficient k_C/T",

        self.ui.ilc1MinEdit:"Min of individuals locus coefficient k_C/T",
        self.ui.ilc1MaxEdit:"Max of individuals locus coefficient k_C/T",
        self.ui.ilc1MeanEdit:"Mean of individuals locus coefficient k_C/T",
        self.ui.ilc1ShapeEdit:"Shape of individuals locus coefficient k_C/T",

        self.ui.mc2MinEdit:"Min of mean coefficient k_A/G",
        self.ui.mc2MaxEdit:"Max of mean coefficient k_A/G",
        self.ui.mc2MeanEdit:"Mean of mean coefficient k_A/G",
        self.ui.mc2ShapeEdit:"Shape of mean coefficient k_A/G",

        self.ui.ilc2MinEdit:"Min of individuals locus coefficient k_A/G",
        self.ui.ilc2MaxEdit:"Max of individuals locus coefficient k_A/G",
        self.ui.ilc2MeanEdit:"Mean of individuals locus coefficient k_A/G",
        self.ui.ilc2ShapeEdit:"Shape of individuals locus coefficient k_A/G"
        }

        self.constraints_dico = {
        self.ui.mmrMinEdit:[1,1],
        self.ui.mmrMaxEdit:[1,1],
        self.ui.mmrMeanEdit:[1,0],
        self.ui.mmrShapeEdit:[1,1],

        self.ui.ilmrMinEdit:[1,1],
        self.ui.ilmrMaxEdit:[1,1],
        self.ui.ilmrMeanEdit:[1,0],
        self.ui.ilmrShapeEdit:[1,1],

        self.ui.mc1MinEdit:[1,1],
        self.ui.mc1MaxEdit:[1,1],
        self.ui.mc1MeanEdit:[1,0],
        self.ui.mc1ShapeEdit:[1,1],

        self.ui.ilc1MinEdit:[1,1],
        self.ui.ilc1MaxEdit:[1,1],
        self.ui.ilc1MeanEdit:[1,0],
        self.ui.ilc1ShapeEdit:[1,1],

        self.ui.mc2MinEdit:[1,1],
        self.ui.mc2MaxEdit:[1,1],
        self.ui.mc2MeanEdit:[1,0],
        self.ui.mc2ShapeEdit:[1,1],

        self.ui.ilc2MinEdit:[1,1],
        self.ui.ilc2MaxEdit:[1,1],
        self.ui.ilc2MeanEdit:[1,0],
        self.ui.ilc2ShapeEdit:[1,1]
        }

    def showJukes(self):
        for prefix in ["mmr","ilmr"]:
            exec("self.ui.%sFrame.show()"%prefix)
        for prefix in ["mc1","ilc1","mc2","ilc2"]:
            exec("self.ui.%sFrame.hide()"%prefix)
    def showKimuraHase(self):
        for prefix in ["mmr","ilmr","mc1","ilc1"]:
            exec("self.ui.%sFrame.show()"%prefix)
        for prefix in ["mc2","ilc2"]:
            exec("self.ui.%sFrame.hide()"%prefix)
    def showTamura(self):
        for prefix in ["mmr","ilmr","mc1","ilc1","mc2","ilc2"]:
            exec("self.ui.%sFrame.show()"%prefix)

    def mmrGamma(self,act):
        if act:
            self.ui.mmrMeanEdit.setDisabled(False)
            self.ui.mmrShapeEdit.setDisabled(False)
    def mmrNoGamma(self,act):
        if act:
            self.ui.mmrMeanEdit.setDisabled(True)
            self.ui.mmrShapeEdit.setDisabled(True)
    def mc1Gamma(self,act):
        if act:
            self.ui.mc1MeanEdit.setDisabled(False)
            self.ui.mc1ShapeEdit.setDisabled(False)
    def mc1NoGamma(self,act):
        if act:
            self.ui.mc1MeanEdit.setDisabled(True)
            self.ui.mc1ShapeEdit.setDisabled(True)
    def mc2Gamma(self,act):
        if act:
            self.ui.mc2MeanEdit.setDisabled(False)
            self.ui.mc2ShapeEdit.setDisabled(False)
    def mc2NoGamma(self,act):
        if act:
            self.ui.mc2MeanEdit.setDisabled(True)
            self.ui.mc2ShapeEdit.setDisabled(True)

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
            law = "LU"
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
            law = "LU"
        else:
            law = "GA"
        result += "MEANK1 %s[%s,%s,%s,%s]\n"%(law,mc1Min,mc1Max,mc1Mean,mc1Shape)

        ilc1Min =    str(self.ui.ilc1MinEdit.text())
        ilc1Max =    str(self.ui.ilc1MaxEdit.text())
        ilc1Mean =   str(self.ui.ilc1MeanEdit.text())
        ilc1Shape =  str(self.ui.ilc1ShapeEdit.text())
        if ilc1Mean == "":
            ilc1Mean = "-9"
        if ilc1Shape == "":
            ilc1Shape = "2"

        law = "GA"
        result += "GAMK1 %s[%s,%s,%s,%s]\n"%(law,ilc1Min,ilc1Max,ilc1Mean,ilc1Shape)


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
            law = "LU"
        else:
            law = "GA"
        result += "MEANK2 %s[%s,%s,%s,%s]\n"%(law,mc2Min,mc2Max,mc2Mean,mc2Shape)

        ilc2Min =    str(self.ui.ilc2MinEdit.text())
        ilc2Max =    str(self.ui.ilc2MaxEdit.text())
        ilc2Mean =   str(self.ui.ilc2MeanEdit.text())
        ilc2Shape =  str(self.ui.ilc2ShapeEdit.text())
        if ilc2Mean == "":
            ilc2Mean = "-9"
        if ilc2Shape == "":
            ilc2Shape = "2"

        law = "GA"
        result += "GAMK2 %s[%s,%s,%s,%s]\n"%(law,ilc2Min,ilc2Max,ilc2Mean,ilc2Shape)
        result += "MODEL %s %s %s\n"%(model,str(self.ui.isEdit.text()).strip(),str(self.ui.sotgEdit.text()).strip())
        
        return result

    def setMutationConf(self,lines):
        """ set les valeurs depuis la conf
        """
        mmrValues = lines[0].split('[')[1].split(']')[0].split(',')
        if mmrValues[2] == "-9":
            mmrValues[2] = ""
        #if mmrValues[3] == "2":
        #    mmrValues[3] = ""
        self.ui.mmrMinEdit.setText(mmrValues[0])
        self.ui.mmrMaxEdit.setText(mmrValues[1])    
        self.ui.mmrMeanEdit.setText(mmrValues[2])   
        self.ui.mmrShapeEdit.setText(mmrValues[3])  
        law = lines[0].split('[')[0].split(' ')[-1]

        #print "law:",law
        if law == "UN":
            self.ui.mmrUnifRadio.setChecked(True)
        elif law == "LU": 
            self.ui.mmrLogRadio.setChecked(True)
        else:
            self.ui.mmrGammaRadio.setChecked(True)
        
        ilmrValues = lines[1].split('[')[1].split(']')[0].split(',')
        if ilmrValues[2] == "-9":
            ilmrValues[2] = ""
        #if ilmrValues[3] == "2":
        #    ilmrValues[3] = ""
        self.ui.ilmrMinEdit.setText(ilmrValues[0])
        self.ui.ilmrMaxEdit.setText(ilmrValues[1])
        self.ui.ilmrMeanEdit.setText(ilmrValues[2])
        self.ui.ilmrShapeEdit.setText(ilmrValues[3])

        mc1Values = lines[2].split('[')[1].split(']')[0].split(',')
        if mc1Values[2] == "-9":
            mc1Values[2] = ""
        #if mc1Values[3] == "2":
        #    mc1Values[3] = ""
        self.ui.mc1MinEdit.setText(mc1Values[0])
        self.ui.mc1MaxEdit.setText(mc1Values[1])    
        self.ui.mc1MeanEdit.setText(mc1Values[2])   
        self.ui.mc1ShapeEdit.setText(mc1Values[3])  
        law = lines[2].split('[')[0].split(' ')[-1]

        #print "law:",law
        if law == "UN":
            self.ui.mc1UnifRadio.setChecked(True)
        elif law == "LU": 
            self.ui.mc1LogRadio.setChecked(True)
        else:
            self.ui.mc1GammaRadio.setChecked(True)

        ilc1Values = lines[3].split('[')[1].split(']')[0].split(',')
        if ilc1Values[2] == "-9":
            ilc1Values[2] = ""
        #if ilc1Values[3] == "2":
        #    ilc1Values[3] = ""
        self.ui.ilc1MinEdit.setText(ilc1Values[0])
        self.ui.ilc1MaxEdit.setText(ilc1Values[1])
        self.ui.ilc1MeanEdit.setText(ilc1Values[2])
        self.ui.ilc1ShapeEdit.setText(ilc1Values[3])

        mc2Values = lines[4].split('[')[1].split(']')[0].split(',')
        if mc2Values[2] == "-9":
            mc2Values[2] = ""
        #if mc2Values[3] == "2":
        #    mc2Values[3] = ""
        self.ui.mc2MinEdit.setText(mc2Values[0])
        self.ui.mc2MaxEdit.setText(mc2Values[1])    
        self.ui.mc2MeanEdit.setText(mc2Values[2])   
        self.ui.mc2ShapeEdit.setText(mc2Values[3])  
        law = lines[4].split('[')[0].split(' ')[-1]

        #print "law:",law
        if law == "UN":
            self.ui.mc2UnifRadio.setChecked(True)
        elif law == "LU": 
            self.ui.mc2LogRadio.setChecked(True)
        else:
            self.ui.mc2GammaRadio.setChecked(True)

        ilc2Values = lines[5].split('[')[1].split(']')[0].split(',')
        if ilc2Values[2] == "-9":
            ilc2Values[2] = ""
        #if ilc2Values[3] == "2":
        #    ilc2Values[3] = ""
        self.ui.ilc2MinEdit.setText(ilc2Values[0])
        self.ui.ilc2MaxEdit.setText(ilc2Values[1])
        self.ui.ilc2MeanEdit.setText(ilc2Values[2])
        self.ui.ilc2ShapeEdit.setText(ilc2Values[3])

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
            if self.hasToBeVerified(self.ui.mmrMinEdit) and float(self.ui.mmrMinEdit.text()) > float(self.ui.mmrMaxEdit.text()):
                raise Exception("Mean mutation rate has incoherent min and max values")
            elif self.hasToBeVerified(self.ui.ilmrMinEdit) and float(self.ui.ilmrMinEdit.text()) > float(self.ui.ilmrMaxEdit.text()):
                raise Exception("Individuals locus mutation rate has incoherent min and max values")
            elif self.hasToBeVerified(self.ui.mc1MinEdit) and float(self.ui.mc1MinEdit.text()) > float(self.ui.mc1MaxEdit.text()):
                raise Exception("Mean coefficient k_C/T has incoherent min and max values")
            elif self.hasToBeVerified(self.ui.ilc1MinEdit) and float(self.ui.ilc1MinEdit.text()) > float(self.ui.ilc1MaxEdit.text()):
                raise Exception("Individuals locus coefficient k_C/T has incoherent min and max values")
            elif self.hasToBeVerified(self.ui.mc2MinEdit) and float(self.ui.mc2MinEdit.text()) > float(self.ui.mc2MaxEdit.text()):
                raise Exception("Mean coefficient k_A/G has incoherent min and max values")
            elif self.hasToBeVerified(self.ui.ilc2MinEdit) and float(self.ui.ilc2MinEdit.text()) > float(self.ui.ilc2MaxEdit.text()):
                raise Exception("Individuals locus coefficient k_A/G has incoherent min and max values")
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
            if not silent:
                QMessageBox.information(self,"Value error","%s"%e)
            return False

        return True

    def hasToBeVerified(self,field):
        """ détermine si field doit être vérifié en fonction du modèle choisi et de la loi sélectionnée
        """
        if (field == self.ui.mmrMeanEdit or field == self.ui.mmrShapeEdit) and not self.ui.mmrGammaRadio.isChecked():
            return False
        elif (field == self.ui.mc1MeanEdit or field == self.ui.mc1ShapeEdit) and not self.ui.mc1GammaRadio.isChecked():
            return False
        elif (field == self.ui.mc2MeanEdit or field == self.ui.mc2ShapeEdit) and not self.ui.mc2GammaRadio.isChecked():
            return False
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




    def exit(self):
        pass
    def clear(self):
        pass
    def validate(self):
        pass

