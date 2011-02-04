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
    def __init__(self,parent=None,box_group=None):
        super(SetMutationModel,self).__init__(parent)
        self.parent=parent
        self.box_group = box_group
        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.clearButton,SIGNAL("clicked()"),self.clear)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)

        self.field_names_dico ={self.ui.mmrMinEdit : "Min of mean mutation rate", self.ui.mmrMaxEdit : "Max of mean mutation rate", self.ui.mmrMeanEdit : "Mean of mean mutation rate",self.ui.mmrShapeEdit : "Shape of mean mutation rate",
                            self.ui.ilmrMinEdit : "Min of individual locus mutation rate", self.ui.ilmrMaxEdit : "Max of individual locus mutation rate" , self.ui.ilmrMeanEdit : "Mean of individual locus mutation rate",
                            self.ui.ilmrShapeEdit : "Shape of individual locus mutation rate",
                            self.ui.mcpMinEdit : "Min of mean coefficient P",
                            self.ui.mcpMaxEdit : "Max of mean coefficient P",
                            self.ui.mcpMeanEdit :"Mean of mean coefficient P",
                            self.ui.mcpShapeEdit:"Shape of mean coefficient P", 
                                             
                            self.ui.ilcpMinEdit :"Min of individual locus coefficient P",
                            self.ui.ilcpMaxEdit :"Max of individual locus coefficient P",
                            self.ui.ilcpMeanEdit:"Mean of individual locus coefficient P", 
                            self.ui.ilcpShapeEdit:"Shape of individual locus coefficient P",
                                             
                            self.ui.msrMinEdit : "Min of mean SNI rate",
                            self.ui.msrMaxEdit : "Max of mean SNI rate",
                            self.ui.msrMeanEdit :"Mean of mean SNI rate",
                            self.ui.msrShapeEdit :"Shape of mean SNI rate",
                                             
                            self.ui.ilsrMinEdit :"Min of individual locus SNI rate",
                            self.ui.ilsrMaxEdit :"Max of individual locus SNI rate",
                            self.ui.ilsrMeanEdit :"Mean of individual locus SNI rate",
                            self.ui.ilsrShapeEdit:"Shape of individual locus SNI rate"}




        # [doit_etre_non_vide , doit_être_float]
        self.constraints_dico = { self.ui.mmrMinEdit : [1,1],
                           self.ui.mmrMaxEdit : [1,1],
                           self.ui.mmrMeanEdit : [1,0],
                           self.ui.mmrShapeEdit : [1,1],

                           self.ui.ilmrMinEdit : [1,1],  
                           self.ui.ilmrMaxEdit : [1,1], 
                           self.ui.ilmrMeanEdit : [1,0], 
                           self.ui.ilmrShapeEdit : [1,1],

                           self.ui.mcpMinEdit : [1,1],   
                           self.ui.mcpMaxEdit : [1,1],   
                           self.ui.mcpMeanEdit : [1,0],  
                           self.ui.mcpShapeEdit : [1,1], 

                           self.ui.ilcpMinEdit : [1,1],  
                           self.ui.ilcpMaxEdit : [1,1], 
                           self.ui.ilcpMeanEdit : [1,0], 
                           self.ui.ilcpShapeEdit : [1,1],

                           self.ui.msrMinEdit : [1,1],   
                           self.ui.msrMaxEdit : [1,1],   
                           self.ui.msrMeanEdit : [1,0],  
                           self.ui.msrShapeEdit : [1,1], 

                           self.ui.ilsrMinEdit : [1,1],  
                           self.ui.ilsrMaxEdit : [1,1],  
                           self.ui.ilsrMeanEdit : [1,0],
                           self.ui.ilsrShapeEdit : [1,1]}

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


    # TODO fille
    #def exit(self):
    #    # reactivation des onglets
    #    self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
    #    self.parent.parent.removeTab(self.parent.parent.indexOf(self))
    #    self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))

    # TODO fille
    #def clear(self):
    #    self.parent.clearMutationModel(self.box_group)

    # TODO fille
    #def validate(self):
    #    """ vérifie la validité des informations entrées dans le mutation model
    #    retourne au setGen tab et set la validité du mutation model du groupe
    #    """
    #    if self.allValid():
    #        self.exit()
    #        self.parent.setMutationValid_dico[self.box_group] = True
    #        self.parent.writeGeneticConfFromGui()

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
        except Exception,e:
            QMessageBox.information(self,"Value error","%s"%e)
            return False

        return True

    def getNbParam(self):
        """ retourne le nombre de paramètres (MEAN dont le min<max) pour ce mutation model
        """
        nb_param = 0
        if float(str(self.ui.mmrMinEdit.text())) < float(str(self.ui.mmrMaxEdit.text())):
            nb_param += 1
        if float(str(self.ui.mcpMinEdit.text())) < float(str(self.ui.mcpMaxEdit.text())):
            nb_param += 1
        if float(str(self.ui.msrMinEdit.text())) < float(str(self.ui.msrMaxEdit.text())):
            nb_param += 1
        return nb_param

    # TODO fille
    #def getParamTableHeader(self):
    #    result = u""
    #    gnumber = self.parent.groupList.index(self.box_group)+1
    #    if float(str(self.ui.mmrMinEdit.text())) < float(str(self.ui.mmrMaxEdit.text())):
    #        pname = u"µmic_%s"%(gnumber)
    #        result += pname
    #        for i in range(14-len(pname)):
    #            result += " "
    #    if float(str(self.ui.mcpMinEdit.text())) < float(str(self.ui.mcpMaxEdit.text())):
    #        pname = u"pmic_%s"%(gnumber)
    #        result += pname
    #        for i in range(14-len(pname)):
    #            result += " "
    #    if float(str(self.ui.msrMinEdit.text())) < float(str(self.ui.msrMaxEdit.text())):
    #        pname = u"snimic_%s"%(gnumber)
    #        result += pname
    #        for i in range(14-len(pname)):
    #            result += " "
    #    #print "result %s : %s"%(gnumber,result)
    #    return result



