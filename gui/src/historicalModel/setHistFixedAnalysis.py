# -*- coding: utf-8 -*-

import sys, codecs
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
from collections import defaultdict
#from uis.setHistFrame_ui import Ui_Frame
from utils.visualizescenario import *
from utils.history import *
from geneticData.setGenDataAnalysis import SetGeneticDataAnalysis
from output import *
from analysis.setupEstimationBias import SetupEstimationBias
from analysis.setupComparisonConfidence import SetupComparisonConfidence
import variables
from variables import UIPATH
from utils.cbgpUtils import getFsEncoding, Parents

formHistModelFixed,baseHistModelFixed= uic.loadUiType((u"%s/setHistFrame.ui"%UIPATH).encode(getFsEncoding(logLevel=False)))

class HistFixed(formHistModelFixed,baseHistModelFixed):
    """ définition du modèle historique dans le cadre d'une analyse
    les valeurs sont fixées
    """
    def __init__(self,analysis,parent=None):
        super(HistFixed,self).__init__(parent)
        self.parent=parent
        self.parents = Parents(self.parent)
        self.analysis = analysis
        self.sc_to_show = self.analysis.chosenSc
        self.list_selected_evaluate_sc = self.analysis.candidateScList

        self.paramList = []
        self.condList = []
        self.param_info_dico = {}

        self.createWidgets()

        self.addTheSc()

        self.addTheParams()

        self.restoreAnalysisValues()


    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)

        self.ui.clearButton.hide()
        self.ui.frame_3.hide()
        self.ui.frame_2.hide()
        self.ui.scrollArea_3.hide()
        self.ui.defPrButton.hide()
        self.ui.chkScButton.hide()
        self.ui.addScButton.hide()
        self.ui.groupBox_12.hide()
        self.ui.maxLabel.hide()
        self.ui.meanLabel.hide()
        # self.ui.label_11.hide()
        self.ui.stdevLabel.hide()
        self.ui.minLabel.setText("Value")
        self.ui.drawPreviewsCheck.hide()
        self.ui.refreshPreviewsButton.hide()

        self.ui.groupBox_13.setMinimumSize(QtCore.QSize(150, 20))
        self.ui.groupBox_13.setMaximumSize(QtCore.QSize(150,20))

        self.ui.verticalLayout_6.setAlignment(QtCore.Qt.AlignTop)
        self.ui.horizontalLayout_5.setAlignment(QtCore.Qt.AlignLeft)
        self.ui.horizontalLayout_6.setAlignment(QtCore.Qt.AlignLeft)
        self.ui.horizontalLayout_2.setAlignment(QtCore.Qt.AlignLeft)
        self.ui.horizontalLayout_3.setAlignment(QtCore.Qt.AlignLeft)

        self.ui.analysisNameLabel.setText(self.analysis.name)

    def restoreAnalysisValues(self):
        if self.analysis.histParamsFixed != None:
            for p in self.analysis.histParamsFixed.keys():
                for param in self.paramList:
                    pname = str(param.findChild(QLabel,"paramNameLabel").text())
                    if pname == p:
                        param.findChild(QLineEdit,"meanValueParamEdit").setText(self.analysis.histParamsFixed[p][1])

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
        plainTextEdit.setReadOnly(True)

    def getScText(self):
        """ récupère le texte du scenario concerné dans le modèle historique de la reftable
        """
        return self.parents.hist_model_win.scList[self.sc_to_show-1].findChild(QPlainTextEdit,"scplainTextEdit").toPlainText()

    def addTheParams(self):
        """ trouve et ajoute les paramètres du scenario concerné
        """
        try:
            sc = str(self.getScText())
            scChecker = Scenario(number=self.sc_to_show)
            scChecker.checkread(sc.strip().split('\n'),self.parents.data)
            scChecker.checklogic()
            dico_sc_infos = {}
            dico_sc_infos["text"] = sc.strip().split('\n')
            dico_sc_infos["checker"] = scChecker
            #print "nb param du sc ",num," ",scChecker.nparamtot
        except Exception as e:
            output.notify(str(e))
            self.exit()
            return

        visible = False
        dico_param = self.parents.hist_model_win.param_info_dico
        for param in scChecker.parameters:
            pname = param.name
            pcat = param.category
            elem = dico_param[pname]
            val = (float(elem[2]) + float(elem[3])) / 2
            self.addParamGui(pname,str(val),visible)
            self.param_info_dico[pname] = [pcat]

    def addParamGui(self,name,val,visible):
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
        if "darwin" in sys.platform:
        	ff = setCondButton.font()
        	ff.setPointSize(10)
        	setCondButton.setFont(ff)
        else:
            ff = setCondButton.font()
            ff.setPointSize(8)
            setCondButton.setFont(ff)
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

        meanValueParamEdit.setText(val)
        return groupBox_8

    def majParamInfoDico(self):
        """ met a jour les infos des paramètres dans l'attribut param_info_dico
        """
        # recup des valeurs pour les params
        for param in self.paramList:
            pname = str(param.findChild(QLabel,"paramNameLabel").text())
            val =  str(param.findChild(QLineEdit,"meanValueParamEdit").text())
            self.param_info_dico[pname] = [self.param_info_dico[pname][0],val]

    def checkAll(self):
        """ verification de la coherence des valeurs du modèle historique. Vérification des conditions.
        """
        problems = ""
        conditions = defaultdict(set)
        for cond in self.parents.findChildren(QtGui.QLabel, "condLabel") :
            condText = str(cond.text()).strip()
            for b in ['<=','<','>=','>'] :
                if b in condText :
                    a,c = condText.split(b)
                    conditions[a].add((b,c))
                    break
        paramValues = {}
        for param in self.paramList:
            pname = str(param.findChild(QLabel,"paramNameLabel").text())
            try:
                value =  float(param.findChild(QLineEdit,"meanValueParamEdit").text())
                paramValues[pname] = value
                if value < 0:
                    problems += "Values for parameter %s are incoherent.\n"%pname
            except Exception as e:
                problems += "%s.\n"%e

        for pname in paramValues.keys() :
            for condition in conditions[pname] :
                if not eval("{0} {1} {2}".format(paramValues[pname], condition[0], paramValues[condition[1]])) :
                    problems += "Parameter {0} ({1}) must be {2} to parameter {3} ({4}).\n".format(pname, paramValues[pname], condition[0], condition[1], paramValues[condition[1]])
        if problems == "":
            return True
        else:
            QMessageBox.information(self,"Value error","%s"%problems)
            return False

    def getNextWidgetSnp(self):
        """ methode appelée par ping pong
        """
        if self.analysis.category == "bias":
            #next_title = "bias and precision"
            return SetupEstimationBias(self.analysis,self.parent)
        else:
            #next_title = "evaluate confidence"
            return SetupComparisonConfidence(self.analysis,self.parent)

    def getNextWidgetMsatSeq(self):
        """ methode appelée par ping pong
        """
        return SetGeneticDataAnalysis(self.analysis,self.parent.parent)

    def validate(self):
        """ on vérifie ici que les valeurs sont bien des float et qu'elles sont bien supérieures à 0
        """

        if self.checkAll():
            self.majParamInfoDico()
            self.analysis.histParamsFixed = self.param_info_dico
            # appel ping pong
            next_widget = self.parents.getNextWidget(self)

            self.ui.parents.analysisStack.addWidget(next_widget)
            self.ui.parents.analysisStack.removeWidget(self)
            self.ui.parents.analysisStack.setCurrentWidget(next_widget)

            self.parents.updateDoc(next_widget)

    def exit(self):
        ## reactivation des onglets
        #self.parent.parent.setTabEnabled(1,True)
        #self.parent.parent.setTabEnabled(0,True)
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentIndex(1)
        self.ui.parents.analysisStack.removeWidget(self)
        self.ui.parents.analysisStack.setCurrentIndex(0)

    def getNbParam(self):
        return len(self.paramList)

