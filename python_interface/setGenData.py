# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setGenData_ui import Ui_Frame
from drawScenario import drawScenario
from visualizescenario import *
import history 
from history import IOScreenError

class setGeneticData(QFrame):
    def __init__(self,parent=None):
        super(setGeneticData,self).__init__(parent)
        self.parent=parent
        self.groupList = []
        self.group_info_dico = {}

        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        self.ui.verticalLayout_3.setAlignment(QtCore.Qt.AlignTop)
        self.ui.tableWidget.setColumnWidth(0,250)
        self.ui.tableWidget.setColumnWidth(1,60)
        self.ui.tableWidget.setColumnWidth(2,60)
        self.ui.tableWidget.setColumnWidth(3,60)

        QObject.connect(self.ui.addGroupButton,SIGNAL("clicked()"),self.addGroup)

    def addRow(self):
    #    print self.ui.tableWidget.rowCount()
        self.ui.tableWidget.insertRow(self.ui.tableWidget.rowCount())
        print self.ui.tableWidget.rowCount()
        for i in range(self.ui.tableWidget.columnCount()):
            if i == 5:
                self.ui.tableWidget.setCellWidget(self.ui.tableWidget.rowCount()-1,i,QPushButton("View"))
            else:
                self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,i,QTableWidgetItem("new"+str(i)))
                print str(self.ui.tableWidget.rowCount()-1)+","+str(i)
            #self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,i).setText("new")
        #self.ui.tableWidget.insertRow(2)


    def addGroup(self):
        groupBox = QtGui.QGroupBox(self.ui.scrollAreaWidgetContents)
        groupBox.setMinimumSize(QtCore.QSize(0, 150))
        groupBox.setMaximumSize(QtCore.QSize(16777215, 150))
        groupBox.setObjectName("groupBox")
        horizontalLayout = QtGui.QHBoxLayout(groupBox)
        horizontalLayout.setObjectName("horizontalLayout")
        verticalLayout = QtGui.QVBoxLayout()
        verticalLayout.setObjectName("verticalLayout")
        addToButton = QtGui.QPushButton(groupBox)
        addToButton.setObjectName("addToButton")
        verticalLayout.addWidget(addToButton)
        rmFromButton = QtGui.QPushButton(groupBox)
        rmFromButton.setObjectName("rmFromButton")
        verticalLayout.addWidget(rmFromButton)
        horizontalLayout.addLayout(verticalLayout)
        listWidget = QtGui.QListWidget(groupBox)
        listWidget.setObjectName("listWidget")
        horizontalLayout.addWidget(listWidget)
        verticalLayout_2 = QtGui.QVBoxLayout()
        verticalLayout_2.setObjectName("verticalLayout_2")
        setMutationButton = QtGui.QPushButton(groupBox)
        setMutationButton.setObjectName("setMutationButton")
        verticalLayout_2.addWidget(setMutationButton)
        setSumButton = QtGui.QPushButton(groupBox)
        setSumButton.setObjectName("setSumButton")
        verticalLayout_2.addWidget(setSumButton)
        horizontalLayout.addLayout(verticalLayout_2)
        self.ui.verticalLayout_3.addWidget(groupBox)


        self.groupList.append(groupBox)

