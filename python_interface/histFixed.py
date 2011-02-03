# -*- coding: utf-8 -*-

import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from setHistFrame_ui import Ui_Frame
from drawScenario import DrawScenario
from visualizescenario import *
import history 
from history import IOScreenError
from set_condition import SetCondition

class HistFixed(QFrame):
    def __init__(self,sc_to_show,list_selected_evaluate_sc=None,parent=None):
        super(HistFixed,self).__init__(parent)
        self.parent=parent
        self.sc_to_show = sc_to_show
        self.list_selected_evaluate_sc = list_selected_evaluate_sc

        self.paramList = []

        self.createWidgets()

        self.addTheSc()

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.close)

        self.ui.clearButton.hide()
        self.ui.frame_3.hide()
        self.ui.scrollArea_3.hide()
        self.ui.defPrButton.hide()
        self.ui.chkScButton.hide()
        self.ui.addScButton.hide()

        self.ui.verticalLayout_6.setAlignment(QtCore.Qt.AlignTop)
        self.ui.horizontalLayout_6.setAlignment(QtCore.Qt.AlignLeft)
        self.ui.horizontalLayout_2.setAlignment(QtCore.Qt.AlignLeft)
        self.ui.horizontalLayout_3.setAlignment(QtCore.Qt.AlignLeft)

    def addTheSc(self):
        
        
        # creation de la groupbox a ajouter
        groupBox = QtGui.QGroupBox(self.ui.scScrollContent)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox.sizePolicy().hasHeightForWidth())
        groupBox.setSizePolicy(sizePolicy)
        groupBox.setObjectName("groupBox_"+str(self.sc_to_show))
        verticalLayout_6 = QtGui.QVBoxLayout(groupBox)
        verticalLayout_6.setObjectName("verticalLayout_6")
        horizontalLayout_6 = QtGui.QHBoxLayout()
        horizontalLayout_6.setObjectName("horizontalLayout_6")
        label_3 = QtGui.QLabel("scenario %i"%self.sc_to_show,groupBox)
        # ne mettre qu'un seul label du nom scLabel pour pouvoir le retrouver avec findChild()
        label_3.setObjectName("scLabel")
        horizontalLayout_6.addWidget(label_3)
        verticalLayout_6.addLayout(horizontalLayout_6)
        plainTextEdit = QtGui.QPlainTextEdit(groupBox)
        plainTextEdit.setLineWrapMode(QtGui.QPlainTextEdit.NoWrap)
        plainTextEdit.setObjectName("scplainTextEdit")
        verticalLayout_6.addWidget(plainTextEdit)
        # ajout de la groupbox
        self.ui.horizontalLayout_3.addWidget(groupBox)

        # recup du texte du scenario
        plainTextEdit.setPlainText(self.getScText())

    def getScText(self):
        return self.parent.parent.hist_model_win.scList[self.sc_to_show-1].findChild(QPlainTextEdit,"scplainTextEdit").toPlainText()

    def addParamGui(self,name,type_param,code_type_param):
        """ ajoute un paramètre à la GUI et à la liste locale de paramètres
        et retourne la groupBox créée
        """
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
        horizontalLayout_8.addWidget(setCondButton)
        horizontalLayout_13.addWidget(groupBox_9)
        groupBox_10 = QtGui.QGroupBox(groupBox_8)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_10.sizePolicy().hasHeightForWidth())
        groupBox_10.setSizePolicy(sizePolicy)
        groupBox_10.setMinimumSize(QtCore.QSize(290, 25))
        groupBox_10.setMaximumSize(QtCore.QSize(290, 25))
        groupBox_10.setObjectName("groupBox_10")
        horizontalLayout_9 = QtGui.QHBoxLayout(groupBox_10)
        horizontalLayout_9.setObjectName("horizontalLayout_9")
        horizontalLayout_9.setContentsMargins(-1, 1, -1, 1)
        uniformParamRadio = QtGui.QRadioButton(groupBox_10)
        uniformParamRadio.setChecked(True)
        uniformParamRadio.setObjectName("uniformParamRadio")
        horizontalLayout_9.addWidget(uniformParamRadio)
        logUniformRadio = QtGui.QRadioButton(groupBox_10)
        logUniformRadio.setObjectName("logUniformRadio")
        horizontalLayout_9.addWidget(logUniformRadio)
        normalRadio = QtGui.QRadioButton(groupBox_10)
        normalRadio.setObjectName("normalRadio")
        horizontalLayout_9.addWidget(normalRadio)
        logNormalRadio = QtGui.QRadioButton(groupBox_10)
        logNormalRadio.setObjectName("logNormalRadio")
        ## alignement des radio
        #uniformParamRadio.setAlignment(QtCore.Qt.AlignCenter)
        #logUniformRadio.setAlignment(QtCore.Qt.AlignCenter)
        #normalRadio.setAlignment(QtCore.Qt.AlignCenter)
        #logNormalRadio.setAlignment(QtCore.Qt.AlignCenter)
        horizontalLayout_9.addWidget(logNormalRadio)
        horizontalLayout_13.addWidget(groupBox_10)
        groupBox_14 = QtGui.QGroupBox(groupBox_8)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_14.sizePolicy().hasHeightForWidth())
        groupBox_14.setSizePolicy(sizePolicy)
        groupBox_14.setMinimumSize(QtCore.QSize(0, 25))
        groupBox_14.setMaximumSize(QtCore.QSize(16777215,25))
        groupBox_14.setObjectName("groupBox_14")
        horizontalLayout_10 = QtGui.QHBoxLayout(groupBox_14)
        horizontalLayout_10.setObjectName("horizontalLayout_10")
        horizontalLayout_10.setContentsMargins(-1, 1, -1, 1)
        minValueParamEdit = QtGui.QLineEdit(groupBox_14)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(minValueParamEdit.sizePolicy().hasHeightForWidth())
        minValueParamEdit.setSizePolicy(sizePolicy)
        minValueParamEdit.setMinimumSize(QtCore.QSize(60, 0))
        minValueParamEdit.setMaximumSize(QtCore.QSize(60, 16777215))
        minValueParamEdit.setObjectName("minValueParamEdit")
        horizontalLayout_10.addWidget(minValueParamEdit)
        maxValueParamEdit = QtGui.QLineEdit(groupBox_14)
        maxValueParamEdit.setMinimumSize(QtCore.QSize(60, 0))
        maxValueParamEdit.setMaximumSize(QtCore.QSize(60, 16777215))
        maxValueParamEdit.setObjectName("maxValueParamEdit")
        horizontalLayout_10.addWidget(maxValueParamEdit)
        meanValueParamEdit = QtGui.QLineEdit(groupBox_14)
        meanValueParamEdit.setMinimumSize(QtCore.QSize(60, 0))
        meanValueParamEdit.setMaximumSize(QtCore.QSize(60, 16777215))
        meanValueParamEdit.setObjectName("meanValueParamEdit")
        horizontalLayout_10.addWidget(meanValueParamEdit)
        stValueParamEdit = QtGui.QLineEdit(groupBox_14)
        stValueParamEdit.setMinimumSize(QtCore.QSize(60, 0))
        stValueParamEdit.setMaximumSize(QtCore.QSize(60, 16777215))
        stValueParamEdit.setObjectName("stValueParamEdit")
        horizontalLayout_10.addWidget(stValueParamEdit)
        stepValueParamEdit = QtGui.QLineEdit(groupBox_14)
        stepValueParamEdit.setMinimumSize(QtCore.QSize(60, 0))
        stepValueParamEdit.setMaximumSize(QtCore.QSize(60, 16777215))
        stepValueParamEdit.setObjectName("stepValueParamEdit")
        horizontalLayout_10.addWidget(stepValueParamEdit)
        horizontalLayout_13.addWidget(groupBox_14)
        self.ui.verticalLayout_6.addWidget(groupBox_8)

        # liste locale des paramètres
        self.paramList.append(groupBox_8)

        # TODO modifier
        if code_type_param == "N" or code_type_param == "T":
            minValueParamEdit.setText("10")
            maxValueParamEdit.setText("10000")
            stepValueParamEdit.setText("1")
        elif code_type_param == "A":
            minValueParamEdit.setText("0.001")
            maxValueParamEdit.setText("0.999")
            stepValueParamEdit.setText("0.001")
        meanValueParamEdit.setText("0")
        stValueParamEdit.setText("0")



        return groupBox_8


    def majParamInfoDico(self):
        """ met a jour les infos des paramètres dans l'attribut param_info_dico
        """
        # recup des valeurs pour les params
        for param in self.paramList:
            pname = str(param.findChild(QLabel,"paramNameLabel").text())
            min =   str(param.findChild(QLineEdit,"minValueParamEdit").text())
            max =   str(param.findChild(QLineEdit,"maxValueParamEdit").text())
            mean =  str(param.findChild(QLineEdit,"meanValueParamEdit").text())
            stdev = str(param.findChild(QLineEdit,"stValueParamEdit").text())
            step =  str(param.findChild(QLineEdit,"stepValueParamEdit").text())
            if param.findChild(QRadioButton,'logNormalRadio').isChecked():
                law = "LN"
            elif param.findChild(QRadioButton,'normalRadio').isChecked():
                law = "NO"
            elif param.findChild(QRadioButton,'uniformParamRadio').isChecked():
                law = "UN"
            elif param.findChild(QRadioButton,'logUniformRadio').isChecked():
                law = "LU"
            visible = param.findChild(QPushButton,"setCondButton").isVisible()
            self.param_info_dico[pname] = [self.param_info_dico[pname][0],law,min,max,mean,stdev,step,visible]


    def validate(self):
        #self.majParamInfoDico()
        # creation et ecriture du fichier dans le rep choisi
        self.writeHistoricalConfFromGui()
        # VERIFS, si c'est bon, on change d'onglet, sinon on reste
        if self.checkAll():
            self.parent.setHistValid(True)

            self.majProjectGui()

            self.returnToProject()


    def closeEvent(self, event):
        # le cancel ne vérifie rien
        if self.sender() == self.ui.clearButton:
            self.parent.clearHistoricalModel()
        elif self.sender() == self.ui.exitButton:
            #self.majParamInfoDico()
            self.writeHistoricalConfFromGui()
            self.majProjectGui()
            self.returnToProject()


    def getNbParam(self):
        return len(self.paramList)

