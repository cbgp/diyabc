# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setHistoricalModel_ui import Ui_MainWindow

class setHistoricalModel(QMainWindow):
    def __init__(self,parent=None):
        super(setHistoricalModel,self).__init__(parent)
        self.parent=parent
        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        QObject.connect(self.ui.addScButton,SIGNAL("clicked()"),self.addSc)

        # liste des scenarios non effacés
        self.scList = []
        self.rpList = []

    def addSc(self):
        
        # le numero du nouveau scenario est la taille du tableau actuel de scenarios
        num_scenario = len(self.scList)
        
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
        label_3 = QtGui.QLabel("scenario "+str(len(self.scList)),groupBox)
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
        plainTextEdit.setObjectName("plainTextEdit")
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
        groupBox_r.setMaximumSize(QtCore.QSize(16777215, 50))
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
        lineEdit.setObjectName("lineEdit")
        verticalLayout_6.addWidget(lineEdit)
        self.ui.horizontalLayout_6.addWidget(groupBox_r)

        # ajout des rp dans la liste locale (plus facile à manipuler)
        self.rpList.append(groupBox_r)

    def rmSc(self):
        """ Suppression d'un scenario dans l'affichage et dans la liste locale
        """
        self.sender().parent().hide()
        num_scenar = self.scList.index(self.sender().parent())
        self.scList.remove(self.sender().parent())
        ## mise a jour des index dans le label
        for i in range(len(self.scList)):
            self.scList[i].findChild(QLabel,"scLabel").setText("scenario %i"% i)
            # manière moins sure mais tout de même intéressante dans le principe
            #self.scList[i].layout().itemAt(0).layout().itemAt(0).widget().setText("scenario %i"% i)

        # suppression et disparition de l'indice de répartition pour le scenario à supprimer
        self.rpList.pop(num_scenar).hide()
        for i in range(len(self.rpList)):
            self.rpList[i].findChild(QLabel,"rpLabel").setText("scenario %i"% i)

    def closeEvent(self, event):
        self.parent.setDisabled(False)
