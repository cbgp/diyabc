# -*- coding: utf-8 -*-

import os,sys
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.setHistFrame_ui import Ui_Frame
from setHistoricalModel import SetHistoricalModel
from drawScenario import DrawScenario
from utils.visualizescenario import *
from utils.history import *
from set_condition import SetCondition
import output
from output import log

class SetHistoricalModelSimulation(SetHistoricalModel):
    """ Classe pour la définition du modèle historique dans le cadre 
    de la génération de la table de référence
    """
    def __init__(self,parent=None):
        super(SetHistoricalModelSimulation,self).__init__(parent)

        self.sampleSizeList = []

        self.ui.addScButton.hide()
        self.ui.frame_3.hide()
        self.ui.frame_2.hide()

        self.ui.label_7.hide()
        self.ui.label_8.hide()
        #self.ui.label_11.hide()
        self.ui.label_10.hide()
        self.ui.label_9.setText("Value")
        self.ui.defPrButton.setText("Set parameter\nvalues")
        self.ui.groupBox_12.hide()

        self.ui.groupBox_13.setMinimumSize(QtCore.QSize(150, 20))
        self.ui.groupBox_13.setMaximumSize(QtCore.QSize(150,20))

        self.ui.verticalLayout_6.setAlignment(QtCore.Qt.AlignTop)
        self.ui.horizontalLayout_5.setAlignment(QtCore.Qt.AlignLeft)
        self.ui.horizontalLayout_6.setAlignment(QtCore.Qt.AlignLeft)
        self.ui.horizontalLayout_2.setAlignment(QtCore.Qt.AlignLeft)
        self.ui.horizontalLayout_3.setAlignment(QtCore.Qt.AlignLeft)

        self.ui.scrollArea_3.setMaximumSize(9999,90)
        self.ui.scrollArea_3.setMinimumSize(0,90)

        defSampleSizeButton = QPushButton("Set sample size")
        self.ui.verticalLayout_3.addWidget(defSampleSizeButton)
        QObject.connect(defSampleSizeButton,SIGNAL("clicked()"),self.defineSampleSize)

        #self.ui.horizontalLayout_2.addWidget(QLabel("Define number of \nfemale individuals :\nmale individuals:"))
        groupBoxSampleSize = QtGui.QGroupBox(self.ui.scrollAreaWidgetContents_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBoxSampleSize.sizePolicy().hasHeightForWidth())
        groupBoxSampleSize.setSizePolicy(sizePolicy)
        groupBoxSampleSize.setTitle("")
        groupBoxSampleSize.setObjectName("groupBox_6")
        groupBoxSampleSize.setMaximumSize(QtCore.QSize(130, 80))
        verticalLayout_2 = QtGui.QVBoxLayout(groupBoxSampleSize)
        verticalLayout_2.setObjectName("verticalLayout_2")
        verticalLayout_2.setContentsMargins(-1, 1, -1, 1)
        label_2 = QtGui.QLabel("Define number for")
        label_2.setObjectName("sampleLabel")
        label_2.setAlignment(QtCore.Qt.AlignCenter)
        verticalLayout_2.addWidget(label_2)
        labelm = QtGui.QLabel("of male")
        labelm.setAlignment(QtCore.Qt.AlignRight)
        labelf = QtGui.QLabel("of female")
        labelf.setAlignment(QtCore.Qt.AlignRight)
        verticalLayout_2.addWidget(labelf)
        verticalLayout_2.addWidget(labelm)
        # ajout dans la GUI
        self.ui.horizontalLayout_2.addWidget(groupBoxSampleSize)

    def defineSampleSize(self,silent=False):
        # nettoyage
        rmList = []
        for ssizeBox in self.sampleSizeList:
            ssizeBox.hide()
            rmList.append(ssizeBox)
        for ss in rmList:
            self.sampleSizeList.remove(ss)

        # remplissage
        scTxt = str(self.scList[0].findChild(QPlainTextEdit,"scplainTextEdit").toPlainText())

        scChecker = Scenario(number=1,prior_proba="1")
        try:
            log(3,"sc text %s"%scTxt)
            scChecker.checkread(scTxt.strip().split('\n'),None)
            scChecker.checklogic()
            dico_sc_infos = {}
            dico_sc_infos["text"] = scTxt.strip().split('\n')
            dico_sc_infos["checker"] = scChecker
        except IOScreenError, e:
            if not silent:
                output.notify(self,"Scenario error","%s"%e)

        for i in range(scChecker.nsamp):
            self.addSampleSizeBox(i+1)

    def addSampleSizeBox(self,num):
        groupBoxSampleSize = QtGui.QGroupBox(self.ui.scrollAreaWidgetContents_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBoxSampleSize.sizePolicy().hasHeightForWidth())
        groupBoxSampleSize.setSizePolicy(sizePolicy)
        groupBoxSampleSize.setTitle("")
        groupBoxSampleSize.setObjectName("groupBox_6")
        groupBoxSampleSize.setMaximumSize(QtCore.QSize(100, 80))
        verticalLayout_2 = QtGui.QVBoxLayout(groupBoxSampleSize)
        verticalLayout_2.setObjectName("verticalLayout_2")
        verticalLayout_2.setContentsMargins(-1, 1, -1, 1)
        label_2 = QtGui.QLabel("sample %s"%num,groupBoxSampleSize)
        label_2.setObjectName("sampleLabel")
        label_2.setAlignment(QtCore.Qt.AlignCenter)
        verticalLayout_2.addWidget(label_2)
        fsizeEdit = QtGui.QLineEdit("25",groupBoxSampleSize)
        fsizeEdit.setObjectName("fsizeEdit")
        fsizeEdit.setMaximumSize(30,20)
        fsizeEdit.setMinimumSize(30,20)
        fsizeEdit.setAlignment(QtCore.Qt.AlignRight)
        sizeEdit = QtGui.QLineEdit("25",groupBoxSampleSize)
        sizeEdit.setObjectName("msizeEdit")
        sizeEdit.setMaximumSize(30,20)
        sizeEdit.setMinimumSize(30,20)
        sizeEdit.setAlignment(QtCore.Qt.AlignRight)
        verticalLayout_2.addWidget(fsizeEdit)
        verticalLayout_2.addWidget(sizeEdit)
        # ajout dans la GUI
        self.ui.horizontalLayout_2.addWidget(groupBoxSampleSize)

        self.sampleSizeList.append(groupBoxSampleSize)

    def hideRemoveScButtons(self):
        for e in self.findChildren(QPushButton,"rmScButton"):
            e.hide()

    def addParamGui(self,name,type_param,code_type_param):
        """ ajoute un paramètre à la GUI et à la liste locale de paramètres
        et retourne la groupBox créée
        """
        visible = False

        groupBox_8 = QtGui.QFrame(self.ui.scrollArea)
        #groupBox_8.setFrameShape(QtGui.QFrame.StyledPanel)
        #groupBox_8.setFrameShadow(QtGui.QFrame.Raised)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_8.sizePolicy().hasHeightForWidth())
        groupBox_8.setSizePolicy(sizePolicy)
        groupBox_8.setMinimumSize(QtCore.QSize(0, 26))
        groupBox_8.setMaximumSize(QtCore.QSize(16777215, 26))
        groupBox_8.setObjectName("sampleSizeBox")
        horizontalLayout_13 = QtGui.QHBoxLayout(groupBox_8)
        horizontalLayout_13.setAlignment(QtCore.Qt.AlignLeft)
        horizontalLayout_13.setObjectName("horizontalLayout_13")
        horizontalLayout_13.setContentsMargins(-1, 1, -1, 1)
        groupBox_9 = QtGui.QGroupBox(groupBox_8)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_9.sizePolicy().hasHeightForWidth())
        groupBox_9.setSizePolicy(sizePolicy)
        groupBox_9.setMinimumSize(QtCore.QSize(140, 25))
        groupBox_9.setMaximumSize(QtCore.QSize(140, 25))
        groupBox_9.setObjectName("groupBox_9")
        horizontalLayout_8 = QtGui.QHBoxLayout(groupBox_9)
        horizontalLayout_8.setObjectName("horizontalLayout_8")
        horizontalLayout_8.setContentsMargins(-1, 1, -1, 1)
        paramNameLabel = QtGui.QLabel(name,groupBox_9)
        paramNameLabel.setObjectName("paramNameLabel")
        horizontalLayout_8.addWidget(paramNameLabel)
        setCondButton = QtGui.QPushButton("set condition",groupBox_9)
        setCondButton.setObjectName("setCondButton")
        setCondButton.setMinimumSize(QtCore.QSize(80, 20))
        setCondButton.setMaximumSize(QtCore.QSize(80, 20))
        if not visible:
            setCondButton.hide()
        else:
            QObject.connect(setCondButton,SIGNAL("clicked()"),self.setConditionAction)
        horizontalLayout_8.addWidget(setCondButton)
        horizontalLayout_13.addWidget(groupBox_9)
        groupBox_14 = QtGui.QGroupBox(groupBox_8)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_14.sizePolicy().hasHeightForWidth())
        groupBox_14.setSizePolicy(sizePolicy)
        groupBox_14.setMinimumSize(QtCore.QSize(150, 25))
        groupBox_14.setMaximumSize(QtCore.QSize(150,25))
        groupBox_14.setObjectName("groupBox_14")
        horizontalLayout_10 = QtGui.QHBoxLayout(groupBox_14)
        horizontalLayout_10.setObjectName("horizontalLayout_10")
        horizontalLayout_10.setContentsMargins(-1, 1, -1, 1)
        meanValueParamEdit = QtGui.QLineEdit(groupBox_14)
        meanValueParamEdit.setMinimumSize(QtCore.QSize(60, 0))
        meanValueParamEdit.setMaximumSize(QtCore.QSize(60, 16777215))
        meanValueParamEdit.setObjectName("meanValueParamEdit")
        meanValueParamEdit.setAlignment(QtCore.Qt.AlignRight)
        horizontalLayout_10.addWidget(meanValueParamEdit)
        horizontalLayout_13.addWidget(groupBox_14)
        self.ui.verticalLayout_6.addWidget(groupBox_8)

        # liste locale des paramètres
        self.paramList.append(groupBox_8)

        if code_type_param == "N":
            meanValueParamEdit.setText("1000")
        if code_type_param == "T":
            meanValueParamEdit.setText("1000")
        elif code_type_param == "A":
            meanValueParamEdit.setText("0.5")
        return groupBox_8

    def majParamInfoDico(self):
        """ met a jour les infos des paramètres dans l'attribut param_info_dico
        """
        # recup des valeurs pour les params
        for param in self.paramList:
            pname = str(param.findChild(QLabel,"paramNameLabel").text())
            val =  str(param.findChild(QLineEdit,"meanValueParamEdit").text())
            self.param_info_dico[pname] = [self.param_info_dico[pname][0],val,val,val,val]

    def validate(self):
        """ vérifie la validité du modèle historique et quitte
        """
        # VERIFS, si c'est bon, on change d'onglet, sinon on reste
        if self.checkAll() and self.checkSampleSize():
            self.parent.setHistValid(True)

            self.majProjectGui()
            self.majParamInfoDico()

            self.parent.ui.setGeneticButton.setDisabled(False)
            self.parent.ui.setHistoricalButton.setDisabled(True)

            self.returnToProject()

    def checkSampleSize(self):
        # on verifie qu'on a le bon nombre de sample
        nsamp = 0
        scTxt = str(self.scList[0].findChild(QPlainTextEdit,"scplainTextEdit").toPlainText())
        scChecker = Scenario(number=1,prior_proba="1")
        try:
            scChecker.checkread(scTxt.strip().split('\n'),None)
            scChecker.checklogic()
            dico_sc_infos = {}
            dico_sc_infos["text"] = scTxt.strip().split('\n')
            dico_sc_infos["checker"] = scChecker
        except Exception, e:
            pass
        if scChecker.nsamp != len(self.sampleSizeList):
            output.notify(self,"Sample size error","Sample number differs between the scenario and the size definition")
            return False
        try:
            for ssBox in self.sampleSizeList:
                msize = int(ssBox.findChild(QLineEdit,"msizeEdit").text())
                fsize = int(ssBox.findChild(QLineEdit,"fsizeEdit").text())
                if fsize+msize < 1:
                    output.notify(self,"Sample size values error","Total sample size must be positive")
                    return False
        except Exception,e:
            output.notify(self,"Sample size values error","%s"%e)
            return False
        return True

    def getConf(self):
        result = "%s samples\n"%len(self.sampleSizeList)
        for box in self.sampleSizeList:
            nbf = box.findChild(QLineEdit,"fsizeEdit").text()
            nbm = box.findChild(QLineEdit,"msizeEdit").text()
            result += "%s %s\n"%(nbf,nbm)

        scTxt = str(self.scList[0].findChild(QPlainTextEdit,"scplainTextEdit").toPlainText()).strip()
        result += "\nscenario (%s)\n"%(len(scTxt.split('\n')))
        result += "%s\n"%scTxt

        result += "\nhistorical parameters (%s)"%len(self.paramList)

        for pbox in self.paramList:
            pname = str(pbox.findChild(QLabel,"paramNameLabel").text())
            info = self.param_info_dico[pname]
            # recherche de la box du param
            name = ""
            i = 0
            while i<len(self.paramList) and name != pname:
                paramBox = self.paramList[i]
                name = paramBox.findChild(QLabel,"paramNameLabel").text()                    
                i+=1
            result+= "\n%s %s %s"%(pname,info[0],info[1])
        return result




