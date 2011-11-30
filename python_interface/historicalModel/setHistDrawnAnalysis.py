# -*- coding: utf-8 -*-

import sys
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.setHistFrame_ui import Ui_Frame
from utils.visualizescenario import *
from utils.history import *
from setCondition import SetCondition
from geneticData.setGenDataAnalysis import SetGeneticDataAnalysis
from analysis.setupEstimationBias import SetupEstimationBias
from analysis.setupComparisonEvaluation import SetupComparisonEvaluation

formHistModelDrawn,baseHistModelDrawn = uic.loadUiType("uis/setHistFrame.ui")

class HistDrawn(formHistModelDrawn,baseHistModelDrawn):
    """ définition du modèle historique dans le cadre d'une analyse
    les valeurs ne sont pas fixées
    """
    def __init__(self,analysis,parent=None):
        super(HistDrawn,self).__init__(parent)
        self.parent=parent
        self.analysis = analysis
        self.sc_to_show = self.analysis.chosenSc
        self.list_selected_evaluate_sc = self.analysis.candidateScList

        self.paramList = []
        self.condList = []
        self.param_info_dico = {}

        self.createWidgets()

        self.addTheSc()

        self.addTheParams()

        self.addTheConditions()

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)

        self.ui.clearButton.hide()
        self.ui.frame_3.hide()
        self.ui.frame_2.hide()
        #self.ui.scrollArea_3.hide()
        self.ui.defPrButton.hide()
        self.ui.chkScButton.hide()
        self.ui.addScButton.hide()
        self.ui.drawPreviewsCheck.hide()
        self.ui.refreshPreviewsButton.hide()

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
        plainTextEdit.setReadOnly(True)

    def getScText(self):
        """ récupère le texte du scenario concerné dans le modèle historique de la reftable
        """
        return self.parent.parent.hist_model_win.scList[self.sc_to_show-1].findChild(QPlainTextEdit,"scplainTextEdit").toPlainText()

    def addTheParams(self):
        """ trouve et ajoute les paramètres du scenario concerné
        """
        try:
            sc = str(self.getScText())
            scChecker = Scenario(number=self.sc_to_show)
            scChecker.checkread(sc.strip().split('\n'),self.parent.parent.data)
            scChecker.checklogic()
            dico_sc_infos = {}
            dico_sc_infos["text"] = sc.strip().split('\n')
            dico_sc_infos["checker"] = scChecker
            #print "nb param du sc ",num," ",scChecker.nparamtot
        except Exception,e:
            output.notify(str(e))
            self.exit()
            return

        # calcul des infos pour savoir si on peut setCondition sur ce param
        dico_cat = {}
        lprem = []
        for param in scChecker.parameters:
            if dico_cat.has_key(param.category):
                dico_cat[param.category] += 1
            else:
                lprem.append(param.name)
                dico_cat[param.category] = 1

        dico_param = self.parent.parent.hist_model_win.param_info_dico
        for param in scChecker.parameters:
            pname = param.name
            pcat = param.category

            visible = False
            if pname not in lprem and dico_cat[pcat] > 1:
                visible = True

            elem = dico_param[pname]
            self.addParamGui(pname,elem[1],elem[2],elem[3],elem[4],elem[5],visible)
            self.param_info_dico[pname] = [pcat]

    def addTheConditions(self):
        """ récupère les conditions du modèle historique de la reftable contenant
        uniquement des paramètre du scenarion concerné
        """
        self.majParamInfoDico()
        for condbox in self.parent.parent.hist_model_win.condList:
            txt = str(condbox.findChild(QLabel,"condLabel").text())
            if "<=" in txt:
                sep = "<="
            elif ">=" in txt:
                sep = ">="
            elif ">" in txt:
                sep = ">"
            elif "<" in txt:
                sep = "<"
            termes = txt.split(sep)
            if termes[0] in self.param_info_dico.keys() and termes[1] in self.param_info_dico.keys():
                self.addCondition(txt)


    def addParamGui(self,name,law,min,max,mean,stdev,visible):
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
        uniformParamRadio.setMinimumSize(QtCore.QSize(20, 0))
        horizontalLayout_9.addWidget(uniformParamRadio)
        logUniformRadio = QtGui.QRadioButton(groupBox_10)
        logUniformRadio.setObjectName("logUniformRadio")
        logUniformRadio.setMinimumSize(QtCore.QSize(20, 0))
        horizontalLayout_9.addWidget(logUniformRadio)
        normalRadio = QtGui.QRadioButton(groupBox_10)
        normalRadio.setObjectName("normalRadio")
        normalRadio.setMinimumSize(QtCore.QSize(20, 0))
        horizontalLayout_9.addWidget(normalRadio)
        logNormalRadio = QtGui.QRadioButton(groupBox_10)
        logNormalRadio.setObjectName("logNormalRadio")
        logNormalRadio.setMinimumSize(QtCore.QSize(20, 0))
        ## alignement des radio
        #uniformParamRadio.setAlignment(QtCore.Qt.AlignCenter)
        #logUniformRadio.setAlignment(QtCore.Qt.AlignCenter)
        #normalRadio.setAlignment(QtCore.Qt.AlignCenter)
        #logNormalRadio.setAlignment(QtCore.Qt.AlignCenter)
        horizontalLayout_9.addWidget(logNormalRadio)
        horizontalLayout_9.insertStretch(4)
        horizontalLayout_9.insertStretch(3)
        horizontalLayout_9.insertStretch(2)
        horizontalLayout_9.insertStretch(1)
        horizontalLayout_9.insertStretch(0)
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
        horizontalLayout_13.addWidget(groupBox_14)
        self.ui.verticalLayout_6.addWidget(groupBox_8)

        # liste locale des paramètres
        self.paramList.append(groupBox_8)

        minValueParamEdit.setText(min)
        maxValueParamEdit.setText(max)
        meanValueParamEdit.setText(mean)
        stValueParamEdit.setText(stdev)

        if law == "UN":
            uniformParamRadio.setChecked(True)
        elif law == "NO":
            normalRadio.setChecked(True)
        elif law == "LN":
            logNormalRadio.setChecked(True)
        elif law == "LU":
            logUniformRadio.setChecked(True)


        return groupBox_8

    def setConditionAction(self):
        """ methode qui receptionne l'evennement du clic sur "set condition"
        ajout d'une condition sur un paramètre ou une paire de paramètres
        """
        param_src = self.sender().parent().findChild(QLabel,"paramNameLabel").text()
        # construction de la liste des params qui sont dans la même catégorie que le notre
        target_list = []
        for pname in self.param_info_dico.keys():
            if pname != param_src and self.param_info_dico[str(pname)][0] == self.param_info_dico[str(param_src)][0]:
                target_list.append(pname)
        self.setCondition = SetCondition(self.sender().parent().findChild(QLabel,"paramNameLabel").text(),target_list,self)
        self.setCondition.show()

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
            if param.findChild(QRadioButton,'logNormalRadio').isChecked():
                law = "LN"
            elif param.findChild(QRadioButton,'normalRadio').isChecked():
                law = "NO"
            elif param.findChild(QRadioButton,'uniformParamRadio').isChecked():
                law = "UN"
            elif param.findChild(QRadioButton,'logUniformRadio').isChecked():
                law = "LU"
            visible = param.findChild(QPushButton,"setCondButton").isVisible()
            self.param_info_dico[pname] = [self.param_info_dico[pname][0],law,min,max,mean,stdev,visible]

    def addCondition(self,cond_str=""):
        """ ajout d'une condition dans la zone du bas réservée à cet effet
        """
        groupBox_cond = QtGui.QGroupBox(self.ui.scrollAreaWidgetContents_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_cond.sizePolicy().hasHeightForWidth())
        groupBox_cond.setSizePolicy(sizePolicy)
        groupBox_cond.setTitle("")
        groupBox_cond.setObjectName("groupBox_6")
        groupBox_cond.setMaximumSize(QtCore.QSize(100, 44))
        verticalLayout_2 = QtGui.QVBoxLayout(groupBox_cond)
        verticalLayout_2.setObjectName("verticalLayout_2")
        verticalLayout_2.setContentsMargins(-1, 1, -1, 1)
        label_2 = QtGui.QLabel(cond_str,groupBox_cond)
        label_2.setObjectName("condLabel")
        label_2.setAlignment(QtCore.Qt.AlignCenter)
        label_2.setMinimumSize(QtCore.QSize(52, 11))
        label_2.setMaximumSize(QtCore.QSize(52, 11))
        verticalLayout_2.addWidget(label_2)
        pushButton_5 = QtGui.QPushButton("remove",groupBox_cond)
        #sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Fixed)
        #sizePolicy.setHorizontalStretch(0)
        #sizePolicy.setVerticalStretch(0)
        #sizePolicy.setHeightForWidth(pushButton_5.sizePolicy().hasHeightForWidth())
        #pushButton_5.setSizePolicy(sizePolicy)
        pushButton_5.setMinimumSize(QtCore.QSize(52, 17))
        pushButton_5.setMaximumSize(QtCore.QSize(52, 17))
        pushButton_5.setObjectName("rmCondButton")
        verticalLayout_2.addWidget(pushButton_5)
        # ajout dans la GUI
        self.ui.horizontalLayout_2.addWidget(groupBox_cond)

        # evennement de suppression de la condition
        QObject.connect(pushButton_5,SIGNAL("clicked()"),self.rmCond)

        self.condList.append(groupBox_cond)

    def rmCond(self,condBox=None):
        """ Suppression d'une condition sur les variables
        dans l'affichage et dans la liste locale
        """
        if condBox == None:
            condBox = self.sender().parent()
        condBox.hide()
        self.condList.remove(condBox)

    def checkAll(self):
        """ verification de la coherence des valeurs du modèle historique
        """
        # pour tous les params, min<=max, tout>0
        problems = ""
        for param in self.paramList:
            pname = str(param.findChild(QLabel,"paramNameLabel").text())
            try:
                min =   float(param.findChild(QLineEdit,"minValueParamEdit").text())
                max =   float(param.findChild(QLineEdit,"maxValueParamEdit").text())
                mean =  float(param.findChild(QLineEdit,"meanValueParamEdit").text())
                stdev = float(param.findChild(QLineEdit,"stValueParamEdit").text())
                #step =  float(param.findChild(QLineEdit,"stepValueParamEdit").text())
                if min > max or min < 0 or max < 0 or mean < 0 or stdev < 0:
                    problems += "Values for parameter %s are incoherent\n"%pname
            except Exception,e:
                problems += "%s\n"%e

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
            return SetupComparisonEvaluation(self.analysis,self.parent)

    def getNextWidgetMsatSeq(self):
        """ methode appelée par ping pong
        """
        return SetGeneticDataAnalysis(self.analysis,self.parent.parent)

    def validate(self):
        """ validation du historical model, nous n'avons ici que les valeurs à vérifier
        """

        if self.checkAll():
            self.majParamInfoDico()
            self.analysis.histParams = self.param_info_dico
            lCond = []
            for cb in self.condList:
                lCond.append(str(cb.findChild(QLabel,"condLabel").text()))
            self.analysis.condTxtList = lCond
            # appel ping pong
            next_widget = self.parent.parent.getNextWidget(self)

            self.parent.parent.ui.analysisStack.addWidget(next_widget)
            self.parent.parent.ui.analysisStack.removeWidget(self)
            self.parent.parent.ui.analysisStack.setCurrentWidget(next_widget)

    def exit(self):
        ## reactivation des onglets
        #self.parent.parent.setTabEnabled(1,True)
        #self.parent.parent.setTabEnabled(0,True)
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentIndex(1)
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentIndex(0)


    def getNbParam(self):
        return len(self.paramList)

