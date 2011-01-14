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
from setHistoricalModel import setHistoricalModel
from data import Data
from datetime import datetime 
import os.path

class Project(QTabWidget):
    def __init__(self,ui,name,parent=None):
        self.mainUi = ui
        self.name=name
        self.hist_state = 0
        self.data = None

        # utile seulement si on derive de QTabWidget
        super(Project,self).__init__(parent)
        # on peut aussi instancier un tabwidget
        #self.tab = QTabWidget()

        # instanciation du widget project_ui
        self.ui = Ui_TabWidget()
        self.ui.setupUi(self)

        # desactivation des boutons
        self.ui.setHistoricalButton.setDisabled(True)
        self.ui.setGeneticButton.setDisabled(True)
        self.ui.setSummaryButton.setDisabled(True)
        self.ui.runButton.setDisabled(True)
        self.ui.browseDirButton.setDisabled(True)
        self.ui.dataFileEdit.setReadOnly(True)
        self.ui.dirEdit.setReadOnly(True)
        self.ui.groupBox.hide()

        # creation des 3 onglets "set ..."
        self.hist_model_win = setHistoricalModel(self)
        self.addTab(self.hist_model_win,"Set historical model")
        self.setTabEnabled(2,False)

        self.gen_data_win = None
        self.addTab(self.gen_data_win,"Set genetic data")
        self.setTabEnabled(3,False)

        self.sum_stats_win = None
        self.addTab(self.sum_stats_win,"Set summary statistics")
        self.setTabEnabled(4,False)

        # manual alignment set
        self.ui.verticalLayout_2.setAlignment(self.ui.newAnButton,Qt.AlignCenter)
        self.ui.verticalLayout_3.setAlignment(self.ui.progressBar,Qt.AlignCenter)
        self.ui.projNameLabelValue.setText(name)
        self.ui.tableWidget.setColumnWidth(1,150)
        self.ui.tableWidget.setColumnWidth(2,300)
        self.ui.tableWidget.setColumnWidth(3,70)

        # ajout au tabwidget de l'ui principale
        # ajout de l'onglet
        self.mainUi.tabWidget.addTab(self,name)
        self.mainUi.tabWidget.setCurrentWidget(self)

        QObject.connect(self.ui.newAnButton,SIGNAL("clicked()"),self.addRow)
        QObject.connect(self.ui.tableWidget,SIGNAL("cellClicked(int,int)"),self.clcl)
        QObject.connect(self.ui.setHistoricalButton,SIGNAL("clicked()"),self.setHistorical)
        QObject.connect(self.ui.setGeneticButton,SIGNAL("clicked()"),self.changeIcon)
        QObject.connect(self.ui.setSummaryButton,SIGNAL("clicked()"),self.changeIcon)
        QObject.connect(self.ui.browseDataFileButton,SIGNAL("clicked()"),self.dataFileSelection)
        QObject.connect(self.ui.browseDirButton,SIGNAL("clicked()"),self.dirSelection)

        # inserer image
        self.ui.setHistoricalButton.setIcon(QIcon("docs/redcross.png"))
        self.ui.setSummaryButton.setIcon(QIcon("docs/redcross.png"))
        self.ui.setGeneticButton.setIcon(QIcon("docs/redcross.png"))

        self.setTabIcon(0,QIcon("docs/redcross.png"))
        self.setTabIcon(1,QIcon("/usr/share/pixmaps/baobab.xpm"))


        #for i in range(self.ui.tableWidget.columnCount()):
        #    for j in range(self.ui.tableWidget.rowCount()):
        #        it = QTableWidgetItem("%i,%i"%(j,i))
        #        self.ui.tableWidget.setItem(j,i,it)

    def dataFileSelection(self):
        qfd = QFileDialog()
        qfd.setDirectory(self.ui.dirEdit.text())
        name = qfd.getOpenFileName()
        self.data = Data(name)
        try:
            self.data.loadfromfile()
            microsat = ""
            sequences = ""
            et = ""
            if self.data.nloc_mic > 0:
                microsat = "%s microsat"%self.data.nloc_mic
            if self.data.nloc_seq > 0:
                sequences = "%s sequences"%self.data.nloc_seq
            if self.data.nloc_mic > 0 and self.data.nloc_seq > 0:
                et = "and "
            self.ui.dataFileInfoLabel.setText("%s loci (%s%s%s)\n%s individuals in %s samples" % (self.data.nloc,microsat,et,sequences,self.data.nindtot,self.data.nsample))
            self.ui.dataFileEdit.setText(name)
            self.ui.browseDirButton.setDisabled(False)
        except Exception,e:
            keep = ""
            if self.ui.dataFileEdit.text() != "":
                keep = "\n\nKeeping previous selected file"
            QMessageBox.information(self,"Data file error","%s%s"%(e,keep))


    def dirSelection(self):
        qfd = QFileDialog()
        #qfd.setDirectory("~/")
        name = qfd.getExistingDirectory()
        # name_YYYY_MM_DD_num le plus elevé
        dd = datetime.now()
        num = 36
        cd = 100
        while cd > 0 and not os.path.exists(name+"/%s_%i_%i_%i_%i"%(self.name,dd.year,dd.month,dd.day,cd)):
            cd -= 1
        if cd == 100:
            QMessageBox.information(self,"Error","With this version, you cannot have more than 100 project directories")
        else:
            self.ui.dirEdit.setText(name+"/%s_%i_%i_%i-%i"%(self.name,dd.year,dd.month,dd.day,(cd+1)))
        self.ui.groupBox.show()
        self.ui.setHistoricalButton.setDisabled(False)

    def changeIcon(self):
        l=["ok.png","redcross.png"]
        self.hist_state = (self.hist_state+1)% len(l)
        self.ui.setHistoricalButton.setIcon(QIcon("docs/"+l[self.hist_state%len(l)]))
        self.ui.setGeneticButton.setIcon(QIcon("docs/"+l[self.hist_state%len(l)]))
        self.ui.setSummaryButton.setIcon(QIcon("docs/"+l[self.hist_state%len(l)]))
        self.setTabIcon(0,QIcon("docs/"+l[self.hist_state%len(l)]))

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

    def setHistorical(self):
        """ passe sur l'onglet correspondant
        """
        self.setTabEnabled(0,False)
        self.setTabEnabled(1,False)
        self.setCurrentWidget(self.hist_model_win)

    def setNbScenarios(self,nb):
        self.ui.nbScLabel.setText(nb)
    def setNbParams(self,nb):
        self.ui.nbParamLabel.setText(nb)
    def clearHistoricalModel(self):
        self.removeTab(self.indexOf(self.hist_model_win))
        self.hist_model_win = setHistoricalModel(self)
        self.insertTab(2,self.hist_model_win,"Set historical model")
        #self.setTabEnabled(2,False)
        self.setCurrentWidget(self.hist_model_win)

