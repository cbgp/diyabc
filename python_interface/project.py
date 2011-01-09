# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from diyabc_ui import Ui_MainWindow
from project import *
from project_ui import *

class Project():
    def __init__(self,ui,name):
        self.mainUi = ui
        self.tab = QTabWidget()
        self.hist_state = 0

        # utile seulement si on derive de QTabWidget
        #super(Project,self).__init__(None)

        # instanciation du widget project_ui
        self.ui = Ui_TabWidget()
        self.ui.setupUi(self.tab)

        # manual alignment set
        self.ui.verticalLayout_2.setAlignment(self.ui.newAnButton,Qt.AlignCenter)
        self.ui.projNameLabelValue.setText(name)
        self.ui.tableWidget.setColumnWidth(1,150)
        self.ui.tableWidget.setColumnWidth(2,300)
        self.ui.tableWidget.setColumnWidth(3,70)

        # ajout au tabwidget de l'ui principale
        # ajout de l'onglet
        self.mainUi.tabWidget.addTab(self.tab,name)
        self.mainUi.tabWidget.setCurrentWidget(self.tab)

        QObject.connect(self.ui.newAnButton,SIGNAL("clicked()"),self.addRow)
        QObject.connect(self.ui.tableWidget,SIGNAL("cellClicked(int,int)"),self.clcl)
        QObject.connect(self.ui.setHistoricalButton,SIGNAL("clicked()"),self.changeIcon)
        QObject.connect(self.ui.setGeneticButton,SIGNAL("clicked()"),self.changeIcon)
        QObject.connect(self.ui.setSummaryButton,SIGNAL("clicked()"),self.changeIcon)
        QObject.connect(self.ui.browseDataFileButton,SIGNAL("clicked()"),self.dataFileSelection)
        QObject.connect(self.ui.browseDirButton,SIGNAL("clicked()"),self.dirSelection)

        # inserer image
        self.ui.setHistoricalButton.setIcon(QIcon("/home/julien/vcs/git/diyabc/python_interface/docs/ok.png"))
        self.ui.setSummaryButton.setIcon(QIcon("/home/julien/vcs/git/diyabc/python_interface/docs/ok.png"))
        self.ui.setGeneticButton.setIcon(QIcon("/home/julien/vcs/git/diyabc/python_interface/docs/ok.png"))

        self.tab.setTabIcon(0,QIcon("/home/julien/vcs/git/diyabc/python_interface/docs/ok.png"))
        self.tab.setTabIcon(1,QIcon("/usr/share/pixmaps/baobab.xpm"))

        # suppression du premier onglet obligatoire
        #if self.mainUi.tabWidget.count() == 2:

        #for i in range(self.ui.tableWidget.columnCount()):
        #    for j in range(self.ui.tableWidget.rowCount()):
        #        it = QTableWidgetItem("%i,%i"%(j,i))
        #        self.ui.tableWidget.setItem(j,i,it)


    def dataFileSelection(self):
        qfd = QFileDialog()
        qfd.setDirectory(self.ui.dirEdit.text())
        name = qfd.getOpenFileName()
        self.ui.dataFileEdit.setText(name)

    def dirSelection(self):
        qfd = QFileDialog()
        #qfd.setDirectory("~/")
        name = qfd.getExistingDirectory()
        self.ui.dirEdit.setText(name)

    def changeIcon(self):
        l=["ok.png","redcross.png"]
        self.hist_state = (self.hist_state+1)% len(l)
        self.ui.setHistoricalButton.setIcon(QIcon("/home/julien/vcs/git/diyabc/python_interface/docs/"+l[self.hist_state%len(l)]))
        self.ui.setGeneticButton.setIcon(QIcon("/home/julien/vcs/git/diyabc/python_interface/docs/"+l[self.hist_state%len(l)]))
        self.ui.setSummaryButton.setIcon(QIcon("/home/julien/vcs/git/diyabc/python_interface/docs/"+l[self.hist_state%len(l)]))
        self.tab.setTabIcon(0,QIcon("/home/julien/vcs/git/diyabc/python_interface/docs/"+l[self.hist_state%len(l)]))

        self.ui.progressBar.setValue((self.ui.progressBar.value()+10)%self.ui.progressBar.maximum())

    def clcl(self,i,j):
        print str(i)+","+str(j)
        self.ui.tableWidget.item(i,j).setFlags(Qt.ItemIsEditable)
    def addRow(self):
        print self.ui.tableWidget.rowCount()
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


