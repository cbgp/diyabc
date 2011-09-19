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

class SetHistoricalModelSimulation(SetHistoricalModel):
    """ Classe pour la définition du modèle historique dans le cadre 
    de la génération de la table de référence
    """
    def __init__(self,parent=None):
        super(SetHistoricalModelSimulation,self).__init__(parent)
        self.ui.addScButton.hide()
        self.ui.frame_3.hide()

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
        groupBox_8.setObjectName("groupBox_8")
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

