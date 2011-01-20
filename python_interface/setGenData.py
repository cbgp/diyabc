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
        self.locus_info_list = []

        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        self.ui.verticalLayout_3.setAlignment(QtCore.Qt.AlignTop)
        self.ui.tableWidget.setColumnWidth(0,250)
        self.ui.tableWidget.setColumnWidth(1,60)
        self.ui.tableWidget.setColumnWidth(2,60)
        self.ui.tableWidget.setColumnWidth(3,60)

        for i in range(100):
            self.addRow()

        QObject.connect(self.ui.addGroupButton,SIGNAL("clicked()"),self.addGroup)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.clearButton,SIGNAL("clicked()"),self.clear)

    def addRow(self,name="locus",type="M"):
        self.ui.tableWidget.insertRow(self.ui.tableWidget.rowCount())
        #self.ui.tableWidget.setCellWidget(self.ui.tableWidget.rowCount()-1,i,QPushButton("View"))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,0,QTableWidgetItem("%s %i"%(name,self.ui.tableWidget.rowCount())))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,1,QTableWidgetItem("M"))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,2,QTableWidgetItem("2"))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,3,QTableWidgetItem("40"))
        self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,0).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,0).flags() & ~Qt.ItemIsEditable)
        self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,1).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,1).flags() & ~Qt.ItemIsEditable)


    def addGroup(self):
        groupBox = QtGui.QGroupBox("Group %i"%(len(self.groupList)+1),self.ui.scrollAreaWidgetContents)
        groupBox.setMinimumSize(QtCore.QSize(0, 150))
        groupBox.setMaximumSize(QtCore.QSize(16777215, 150))
        groupBox.setObjectName("groupBox")
        horizontalLayout = QtGui.QHBoxLayout(groupBox)
        horizontalLayout.setObjectName("horizontalLayout")
        verticalLayout = QtGui.QVBoxLayout()
        verticalLayout.setObjectName("verticalLayout")
        addToButton = QtGui.QPushButton(">>",groupBox)
        addToButton.setObjectName("addToButton")
        verticalLayout.addWidget(addToButton)
        rmFromButton = QtGui.QPushButton("<<",groupBox)
        rmFromButton.setObjectName("rmFromButton")
        verticalLayout.addWidget(rmFromButton)
        horizontalLayout.addLayout(verticalLayout)
        listWidget = QtGui.QListWidget(groupBox)
        listWidget.setObjectName("listWidget")
        horizontalLayout.addWidget(listWidget)
        listWidget.setSelectionMode(QtGui.QAbstractItemView.MultiSelection)
        verticalLayout_2 = QtGui.QVBoxLayout()
        verticalLayout_2.setObjectName("verticalLayout_2")
        setMutationButton = QtGui.QPushButton("Set mutation\nModel",groupBox)
        setMutationButton.setObjectName("setMutationButton")
        verticalLayout_2.addWidget(setMutationButton)
        setSumButton = QtGui.QPushButton("Set Summary\nStatistics",groupBox)
        setSumButton.setObjectName("setSumButton")
        verticalLayout_2.addWidget(setSumButton)
        horizontalLayout.addLayout(verticalLayout_2)
        self.ui.verticalLayout_3.addWidget(groupBox)

        QObject.connect(addToButton,SIGNAL("clicked()"),self.addToGroup)
        QObject.connect(rmFromButton,SIGNAL("clicked()"),self.rmFromGroup)

        self.groupList.append(groupBox)

    def addToGroup(self):
        """ ajoute les loci selectionnés au groupe du bouton pressé
        """
        box = self.sender().parent()
        row_list = []
        selection = self.ui.tableWidget.selectedIndexes()
        selection.sort()
        for it in selection:
            if it.row() not in row_list:
                row_list.append(it.row())

        # déselection des présents
        for ind_to_unselect in range(box.findChild(QListWidget,"listWidget").count()):
            box.findChild(QListWidget,"listWidget").item(ind_to_unselect).setSelected(False)
        # ajout des nouveaux
        for row in row_list:
            name = self.ui.tableWidget.item(row,0).text()
            # ajout dans le groupe
            box.findChild(QListWidget,"listWidget").addItem(name)
            box.findChild(QListWidget,"listWidget").item(box.findChild(QListWidget,"listWidget").count()-1).setSelected(True)
            # on cache la ligne
            self.ui.tableWidget.setRowHidden(row,True)

    def rmFromGroup(self):
        """ retire les loci selectionnés du groupe du bouton pressé
        """
        box = self.sender().parent()
        listw_of_group = box.findChild(QListWidget,"listWidget")
        row_list = []
        selection = listw_of_group.selectedIndexes()
        for it in selection:
            if it.row() not in row_list:
                row_list.append(int(it.row()))
        row_list.sort()
        row_list.reverse()

        for row in row_list:
            name = str(listw_of_group.item(row).text())
            num = int(name.split(' ')[1])
            # retrait du groupe
            item = listw_of_group.takeItem(row)
            # on decouvre la ligne
            self.ui.tableWidget.setRowHidden(num-1,False)


    def exit(self):
        # reactivation des onglets
        self.parent.setTabEnabled(0,True)
        self.parent.setTabEnabled(1,True)
        self.parent.setTabEnabled(3,False)
        self.parent.setCurrentIndex(0)
    def validate(self):
        pass
    def clear(self):
        pass
