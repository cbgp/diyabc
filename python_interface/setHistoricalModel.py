# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setHistoricalModel_ui import Ui_MainWindow
from drawScenario import drawScenario
from visualizescenario import *
import history 
from history import IOScreenError

class setHistoricalModel(QMainWindow):
    def __init__(self,parent=None):
        super(setHistoricalModel,self).__init__(parent)
        self.parent=parent
        # liste des scenarios, pourcentages et conditions non effacés
        self.scList = []
        self.rpList = []
        self.condList = []
        self.paramList = []

        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        QObject.connect(self.ui.addScButton,SIGNAL("clicked()"),self.addSc)
        QObject.connect(self.ui.uniformRadio,SIGNAL("clicked()"),self.setUniformRp)
        QObject.connect(self.ui.otherRadio,SIGNAL("clicked()"),self.setOtherRp)
        #TODO
        #QObject.connect(self.ui.chkScButton,SIGNAL("clicked()"),self.addCondition)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.close)
        QObject.connect(self.ui.chkScButton,SIGNAL("clicked()"),self.checkToDraw)
        QObject.connect(self.ui.defPrButton,SIGNAL("clicked()"),self.definePriors)

    def addSc(self):
        
        # le numero du nouveau scenario est la taille du tableau actuel de scenarios
        num_scenario = len(self.scList)+1
        
        # creation de la groupbox a ajouter
        groupBox = QtGui.QGroupBox(self.ui.scScrollContent)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox.sizePolicy().hasHeightForWidth())
        groupBox.setSizePolicy(sizePolicy)
        groupBox.setObjectName("groupBox_"+str(num_scenario))
        verticalLayout_6 = QtGui.QVBoxLayout(groupBox)
        verticalLayout_6.setObjectName("verticalLayout_6")
        horizontalLayout_6 = QtGui.QHBoxLayout()
        horizontalLayout_6.setObjectName("horizontalLayout_6")
        label_3 = QtGui.QLabel("scenario %i"%num_scenario,groupBox)
        # ne mettre qu'un seul label du nom scLabel pour pouvoir le retrouver avec findChild()
        label_3.setObjectName("scLabel")
        horizontalLayout_6.addWidget(label_3)
        pushButton_6 = QtGui.QPushButton("remove",groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(pushButton_6.sizePolicy().hasHeightForWidth())
        pushButton_6.setSizePolicy(sizePolicy)
        pushButton_6.setMaximumSize(QtCore.QSize(80, 16777215))
        pushButton_6.setObjectName("pushButton_"+str(len(self.scList)))
        horizontalLayout_6.addWidget(pushButton_6)
        verticalLayout_6.addLayout(horizontalLayout_6)
        plainTextEdit = QtGui.QPlainTextEdit(groupBox)
        plainTextEdit.setLineWrapMode(QtGui.QPlainTextEdit.NoWrap)
        plainTextEdit.setObjectName("scplainTextEdit")
        verticalLayout_6.addWidget(plainTextEdit)
        # ajout de la groupbox
        self.ui.horizontalLayout_3.addWidget(groupBox)

        # evennement de suppression du scenario
        QObject.connect(pushButton_6,SIGNAL("clicked()"),self.rmSc)

        # ajout du scenario dans la liste locale (plus facile à manipuler)
        self.scList.append(groupBox)

        # ajout de la groupbox de repartition
        groupBox_r = QtGui.QGroupBox(self.ui.repScrollContent)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_r.sizePolicy().hasHeightForWidth())
        groupBox_r.setSizePolicy(sizePolicy)
        groupBox_r.setMaximumSize(QtCore.QSize(16777215, 60))
        groupBox_r.setObjectName("groupBox_r")
        verticalLayout_6 = QtGui.QVBoxLayout(groupBox_r)
        verticalLayout_6.setObjectName("verticalLayout_6")
        label_3 = QtGui.QLabel("scenario "+str(num_scenario),groupBox_r)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(label_3.sizePolicy().hasHeightForWidth())
        label_3.setSizePolicy(sizePolicy)
        label_3.setObjectName("rpLabel")
        verticalLayout_6.addWidget(label_3)
        lineEdit = QtGui.QLineEdit(groupBox_r)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(lineEdit.sizePolicy().hasHeightForWidth())
        lineEdit.setSizePolicy(sizePolicy)
        lineEdit.setMinimumSize(QtCore.QSize(0, 26))
        lineEdit.setMaximumSize(QtCore.QSize(40, 26))
        lineEdit.setObjectName("rpEdit")
        verticalLayout_6.addWidget(lineEdit)
        self.ui.horizontalLayout_6.addWidget(groupBox_r)

        # ajout des rp dans la liste locale (plus facile à manipuler)
        self.rpList.append(groupBox_r)

        if self.ui.uniformRadio.isChecked():
            self.setUniformRp()

    def rmSc(self):
        """ Suppression d'un scenario dans l'affichage et dans la liste locale
        """
        self.sender().parent().hide()
        num_scenar = self.scList.index(self.sender().parent())
        self.scList.remove(self.sender().parent())
        ## mise a jour des index dans le label
        for i in range(len(self.scList)):
            self.scList[i].findChild(QLabel,"scLabel").setText("scenario %i"% (i+1))
            # manière moins sure mais tout de même intéressante dans le principe
            #self.scList[i].layout().itemAt(0).layout().itemAt(0).widget().setText("scenario %i"% i)

        # suppression et disparition de l'indice de répartition pour le scenario à supprimer
        self.rpList.pop(num_scenar).hide()
        for i in range(len(self.rpList)):
            self.rpList[i].findChild(QLabel,"rpLabel").setText("scenario %i"% (i+1))

        if self.ui.uniformRadio.isChecked():
            self.setUniformRp()

    def setUniformRp(self):
        """ met les pourcentages à une valeur identique 
        """
        if len(self.scList) != 0:
            val = 1.0/float(len(self.scList))
            # pour chaque pourcentage
            for rp in self.rpList:
                lineEdit = rp.findChild(QLineEdit,"rpEdit")
                lineEdit.setText(str(round(val,2)))
                lineEdit.setDisabled(True)

            

    def setOtherRp(self):
        """ redonne la possibilité de modifier à sa guise les pourcentages
        """
        for rp in self.rpList:
            lineEdit = rp.findChild(QLineEdit,"rpEdit")
            lineEdit.setDisabled(False)

    def addCondition(self,cond_str=""):
        groupBox_cond = QtGui.QGroupBox(self.ui.scrollAreaWidgetContents_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_cond.sizePolicy().hasHeightForWidth())
        groupBox_cond.setSizePolicy(sizePolicy)
        groupBox_cond.setTitle("")
        groupBox_cond.setObjectName("groupBox_6")
        verticalLayout_2 = QtGui.QVBoxLayout(groupBox_cond)
        verticalLayout_2.setObjectName("verticalLayout_2")
        label_2 = QtGui.QLabel(cond_str,groupBox_cond)
        label_2.setObjectName("condLabel")
        label_2.setAlignment(QtCore.Qt.AlignCenter)
        verticalLayout_2.addWidget(label_2)
        pushButton_5 = QtGui.QPushButton("remove",groupBox_cond)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(pushButton_5.sizePolicy().hasHeightForWidth())
        pushButton_5.setSizePolicy(sizePolicy)
        pushButton_5.setMaximumSize(QtCore.QSize(80, 16777215))
        pushButton_5.setObjectName("rmCondButton")
        verticalLayout_2.addWidget(pushButton_5)
        # ajout dans la GUI
        self.ui.horizontalLayout_2.addWidget(groupBox_cond)

        # evennement de suppression de la condition
        QObject.connect(pushButton_5,SIGNAL("clicked()"),self.rmCond)

        self.condList.append(groupBox_cond)

    def rmCond(self):
        """ Suppression d'une condition sur les variables
        dans l'affichage et dans la liste locale
        """
        self.sender().parent().hide()
        self.condList.remove(self.sender().parent())

    def checkToDraw(self):
        """ clic sur le bouton check scenario pour dessiner les scenarios
        """
        chk_list = self.checkScenarios()
        if chk_list != None:
            self.drawScenarios(chk_list)
        else:
            QMessageBox.information(self,"Scenario error","Correct your scenarios to be able to draw them.")

    def definePriors(self):
        """ clic sur le bouton de définition des priors
        """
        chk_list = self.checkScenarios()
        if chk_list != None:
            self.putParameters(chk_list)
        else:
            QMessageBox.information(self,"Scenario error","Correct your scenarios to be able to extract the parameters.")
    def putParameters(self,chk_list):
        """ A partir de la liste des scenarios (vérifiés donc valides), on ajoute les paramètres dans la GUI
        La liste de paramètres est vidée avant cette opération
        """
        # on enleve les groupbox de la GUI
        for paramBox in self.paramList:
            paramBox.hide()
            self.ui.verticalLayout_2.removeWidget(paramBox)
        # on vide la liste locale de paramètres
        self.paramList = []
        dico_parameters = {}
        dico_count_per_category = {}
        for sc in chk_list:
            for param in sc["checker"].parameters:
                dico_parameters[param.name] = param.category
                # on compte le nombre de param dans chaque categorie
                if dico_count_per_category.has_key(param.category):
                    dico_count_per_category[param.category] += 1
                else:
                    dico_count_per_category[param.category] = 1
        for pname in dico_parameters.keys():
            if dico_count_per_category[dico_parameters[pname]] > 1:
                self.addParamGui(pname,"multiple")
            else:
                self.addParamGui(pname,"unique")



    def checkScenarios(self):
        """ action de verification des scenarios
        """
        # construction de la liste de scenarios
        sc_txt_list = []
        for sc in self.scList:
            sc_txt_list.append(str(sc.findChild(QPlainTextEdit,"scplainTextEdit").toPlainText()))
            #print sc_txt_list
        nb_scenarios_invalides = 0
        scenarios_info_list = []
        for num,sc in enumerate(sc_txt_list):
            scChecker = history.Scenario(number=num+1)
            try:
                scChecker.checkread(sc.split('\n'))
                scChecker.checklogic()
                t = PopTree(scChecker)
                t.do_tree()
                #for ev in scChecker.history.events : print ev
                #for  no in t.node : print no
                #print "  "
                #for  b in t.br : print b
                #print "  "
                for s in t.segments: 
                    print s
                    print type(s)
                dico_sc_infos = {}
                dico_sc_infos["text"] = sc.split('\n')
                dico_sc_infos["checker"] = scChecker
                dico_sc_infos["tree"] = t
                scenarios_info_list.append(dico_sc_infos)
            except IOScreenError, e:
                print "Un scenario a une erreur : ", e
                nb_scenarios_invalides += 1
                QMessageBox.information(self,"Scenario error","%s"%e)
        # si tous les scenarios sont bons, on renvoie les données utiles, sinon on renvoie None
        if nb_scenarios_invalides == 0:
            return scenarios_info_list
        else:
            return None

    def drawScenarios(self,scenarios_info_list):
        """ lance la fenetre ou se trouveront les graphes des scenarios
        """
        # creation de la fenêtre
        self.draw_sc_win = drawScenario(scenarios_info_list,self)
        self.draw_sc_win.show()
        self.draw_sc_win.drawAll()

    def addParamGui(self,name,type_param):
        groupBox_8 = QtGui.QGroupBox(self.ui.groupBox_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_8.sizePolicy().hasHeightForWidth())
        groupBox_8.setSizePolicy(sizePolicy)
        groupBox_8.setMinimumSize(QtCore.QSize(0, 70))
        groupBox_8.setMaximumSize(QtCore.QSize(16777215, 70))
        groupBox_8.setObjectName("groupBox_8")
        horizontalLayout_13 = QtGui.QHBoxLayout(groupBox_8)
        horizontalLayout_13.setObjectName("horizontalLayout_13")
        groupBox_9 = QtGui.QGroupBox(groupBox_8)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_9.sizePolicy().hasHeightForWidth())
        groupBox_9.setSizePolicy(sizePolicy)
        groupBox_9.setMinimumSize(QtCore.QSize(140, 50))
        groupBox_9.setMaximumSize(QtCore.QSize(140, 50))
        groupBox_9.setObjectName("groupBox_9")
        horizontalLayout_8 = QtGui.QHBoxLayout(groupBox_9)
        horizontalLayout_8.setObjectName("horizontalLayout_8")
        paramNameLabel = QtGui.QLabel(name,groupBox_9)
        paramNameLabel.setObjectName("paramNameLabel")
        horizontalLayout_8.addWidget(paramNameLabel)
        setCondButton = QtGui.QPushButton("set\ncondition",groupBox_9)
        setCondButton.setObjectName("setCondButton")
        horizontalLayout_8.addWidget(setCondButton)
        horizontalLayout_13.addWidget(groupBox_9)
        groupBox_10 = QtGui.QGroupBox(groupBox_8)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_10.sizePolicy().hasHeightForWidth())
        groupBox_10.setSizePolicy(sizePolicy)
        groupBox_10.setMinimumSize(QtCore.QSize(290, 50))
        groupBox_10.setMaximumSize(QtCore.QSize(290, 50))
        groupBox_10.setObjectName("groupBox_10")
        horizontalLayout_9 = QtGui.QHBoxLayout(groupBox_10)
        horizontalLayout_9.setObjectName("horizontalLayout_9")
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
        horizontalLayout_9.addWidget(logNormalRadio)
        horizontalLayout_13.addWidget(groupBox_10)
        groupBox_14 = QtGui.QGroupBox(groupBox_8)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_14.sizePolicy().hasHeightForWidth())
        groupBox_14.setSizePolicy(sizePolicy)
        groupBox_14.setMinimumSize(QtCore.QSize(0, 50))
        groupBox_14.setMaximumSize(QtCore.QSize(16777215, 50))
        groupBox_14.setObjectName("groupBox_14")
        horizontalLayout_10 = QtGui.QHBoxLayout(groupBox_14)
        horizontalLayout_10.setObjectName("horizontalLayout_10")
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
        self.ui.verticalLayout_2.addWidget(groupBox_8)

        # liste locale des paramètres
        self.paramList.append(groupBox_8)

        if type_param == "unique":
            setCondButton.hide()

        # evennement d'ajout d'une condition sur un paramètre
        QObject.connect(setCondButton,SIGNAL("clicked()"),self.setCondition)

    def setCondition(self):
        """ methode qui receptionne l'evennement du clic sur "set condition"
        ajout d'une condition sur un paramètre ou une paire de paramètres
        """
        self.addCondition(self.sender().parent().findChild(QLabel,"paramNameLabel").text())

    def closeEvent(self, event):
        # gestion des valeurs
        nb_sc = len(self.scList)
        pluriel = ""
        if nb_sc > 1:
            pluriel = "s"
        self.parent.setNbScenarios("%i scenario%s"%(nb_sc,pluriel))
        nb_params = len(self.paramList)
        pluriel = ""
        if nb_params > 1:
            pluriel = "s"
        self.parent.setNbParams("%i parameter%s"%(nb_params,pluriel))
