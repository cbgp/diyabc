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
        #TODO maj ces infos
        self.group_info_dico = {}
        self.locus_info_list = []

        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        self.ui.verticalLayout_3.setAlignment(QtCore.Qt.AlignTop)
        self.ui.tableWidget.setColumnWidth(0,230)
        self.ui.tableWidget.setColumnWidth(1,60)
        self.ui.tableWidget.setColumnWidth(2,60)
        self.ui.tableWidget.setColumnWidth(3,60)

        t = True
        for i in range(100):
            if t:
                self.addRow(type="S")
            else:
                self.addRow()
            t = not t

        QObject.connect(self.ui.addGroupButton,SIGNAL("clicked()"),self.addGroup)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.clearButton,SIGNAL("clicked()"),self.clear)

    def addRow(self,name="locus",type="M"):
        self.ui.tableWidget.insertRow(self.ui.tableWidget.rowCount())
        #self.ui.tableWidget.setCellWidget(self.ui.tableWidget.rowCount()-1,i,QPushButton("View"))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,0,QTableWidgetItem("%s %i"%(name,self.ui.tableWidget.rowCount())))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,1,QTableWidgetItem("%s"%type))
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
        verticalLayout_3 = QtGui.QVBoxLayout()
        verticalLayout_3.setObjectName("verticalLayout_3")
        rmButton = QtGui.QPushButton("Remove group",groupBox)
        rmButton.setObjectName("rmButton")
        verticalLayout_3.addWidget(rmButton)
        listWidget = QtGui.QListWidget(groupBox)
        listWidget.setObjectName("listWidget")
        verticalLayout_3.addWidget(listWidget)
        horizontalLayout.addLayout(verticalLayout_3)
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
        QObject.connect(rmButton,SIGNAL("clicked()"),self.rmGroup)

        self.groupList.append(groupBox)

    def rmGroup(self):
        """ Enlève les loci présents dans le groupe et supprime le groupe
        """
        box = self.sender().parent()
        lw = box.findChild(QListWidget,"listWidget")
        lw.selectAll()
        self.rmFromGroup(box)
        # suppression de la box
        box.hide()
        self.groupList.remove(box)
        # renommage des groupes
        for i,box in enumerate(self.groupList):
            old_title = str(box.title())
            box.setTitle("Group %i %s"%(i+1,' '.join(old_title.split(' ')[2:])))

    def addToGroup(self):
        """ ajoute les loci selectionnés au groupe du bouton pressé
        """
        all_similar = True
        first_type_found = ""
        box = self.sender().parent()
        listwidget = box.findChild(QListWidget,"listWidget")
        row_list = []
        selection = self.ui.tableWidget.selectedIndexes()
        selection.sort()
        for it in selection:
            if it.row() not in row_list:
                row_list.append(it.row())
                # verif que tous les loci selectionnés ont le meme type
                if first_type_found == "":
                    first_type_found = str(self.ui.tableWidget.item(it.row(),1).text())
                else:
                    if first_type_found != str(self.ui.tableWidget.item(it.row(),1).text()):
                        all_similar = False
        # verif que les loci deja dans le groupe sont du même type que ceux sélectionnés
        # on compare le premier type trouvé dans la selection avec le type du premier
        # élément du groupe
        if listwidget.count() > 0:
            if all_similar:
                name = str(listwidget.item(0).text())
                num = int(name.split(' ')[1])
                # info sur le premier locus du groupe
                type = str(self.ui.tableWidget.item(num-1,1).text())
                print "\ntype deja present : %s"%type
                if type != first_type_found:
                    all_similar = False
        else:
            # le groupe est vide, on set le nom avec le type
            old_title = str(box.title())
            tt = ''
            if first_type_found == 'M':
                tt = "Microsatellites"
            elif first_type_found == 'S':
                tt = "Sequences"
            if tt != '':
                box.setTitle("%s %s : %s"%(old_title.split(' ')[0],old_title.split(' ')[1],tt))

        if all_similar:
            # déselection des présents
            for ind_to_unselect in range(listwidget.count()):
                listwidget.item(ind_to_unselect).setSelected(False)
            # ajout des nouveaux
            for row in row_list:
                name = self.ui.tableWidget.item(row,0).text()
                # ajout trié dans le groupe
                i = 0
                num_to_insert = int(name.split(' ')[1])
                while i < listwidget.count() and int(listwidget.item(i).text().split(' ')[1]) < num_to_insert :
                    i+=1
                #box.findChild(QListWidget,"listWidget").addItem(name)
                box.findChild(QListWidget,"listWidget").insertItem(i,name)
                box.findChild(QListWidget,"listWidget").item(i).setSelected(True)
                # on cache la ligne
                self.ui.tableWidget.setRowHidden(row,True)
        else:
            QMessageBox.information(self,"Group error","In a group, all the loci must have the same type")

    def rmFromGroup(self,box=None):
        """ retire les loci selectionnés du groupe du bouton pressé
        """
        if box == None:
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

        # si le groupe devient vide, on enleve le type dans le nom
        if listw_of_group.count() == 0:
            old_title = str(box.title())
            box.setTitle("%s %s"%(old_title.split(' ')[0],old_title.split(' ')[1]))


    def exit(self):
        # reactivation des onglets
        self.parent.setTabEnabled(0,True)
        self.parent.setTabEnabled(1,True)
        self.parent.setTabEnabled(3,False)
        self.parent.setCurrentIndex(0)
    def validate(self):
        pass
    def clear(self):
        #for i in range(len(self.groupList)):
        #    self.groupList[i].hide()
        for box in self.groupList:
            box.hide()
            lw = box.findChild(QListWidget,"listWidget")
            for row in range(lw.count()):
                name = str(lw.item(row).text())
                num = int(name.split(' ')[1])
                # on decouvre la ligne
                self.ui.tableWidget.setRowHidden(num-1,False)
        self.groupList = []
