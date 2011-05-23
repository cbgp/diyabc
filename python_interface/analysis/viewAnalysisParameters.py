# -*- coding: utf-8 -*-

import os
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from uis.viewAnalysisParameters_ui import Ui_Frame
import output

class ViewAnalysisParameters(QFrame):
    def __init__(self,parent,analysis):
        super(ViewAnalysisParameters,self).__init__(parent)
        self.parent = parent
        self.analysis = analysis
        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)
        self.ui.tableWidget.setColumnWidth(0,250)
        self.ui.tableWidget.setColumnWidth(1,600)

        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.exit)

        self.addRow("name",self.analysis.name)
        self.addRow("type",self.analysis.category)
        #if self.analysis.chosenSc != None:
        #    self.addRow("Selected scenario",self.analysis.chosenSc)
        #if self.analysis.candidateScList != [] and self.analysis.candidateScList != None:
        #    self.addRow("Selected scenarios",self.analysis.candidateScList)

        tabCompParams = self.analysis.computationParameters.split(';')
        transDico = {'1':"no",'2':"log",'3':"logit",'4':"logtg"}
        if self.analysis.category == "estimate" or self.analysis.category == "modelChecking" or self.analysis.category == "bias":
            for e in tabCompParams:
                if e.split(':')[0] == 'n':
                    self.addRow("Chosen number of simulated data",e.split(':')[1])
                elif e.split(':')[0] == 'm':
                    self.addRow("Number of selected data",e.split(':')[1])
                elif e.split(':')[0] == 'd':
                    self.addRow("Number of test data sets",e.split(':')[1])
                elif e.split(':')[0] == 't':
                    self.addRow("Number of selected data",transDico[e.split(':')[1]])
                elif e.split(':')[0] == 'p':
                    self.addRow("Choice of parameters",e.split(':')[1].replace('c',"COMPOSITE ").replace('o',"ORIGINAL ").lower())
        elif self.analysis.category == "compare":
            for e in tabCompParams:
                if e.split(':')[0] == 's':
                    self.addRow("Chosen scenarios",e.split(':')[1])
                elif e.split(':')[0] == 'd':
                    self.addRow("Direct estimate",e.split(':')[1])
                elif e.split(':')[0] == 'n':
                    self.addRow("Chosen number of simulated data",e.split(':')[1])
                elif e.split(':')[0] == 'l':
                    self.addRow("Logistic regression",e.split(':')[1])
                elif e.split(':')[0] == 'm':
                    self.addRow("Regression number",e.split(':')[1])
        elif self.analysis.category == "evaluate":
            for e in tabCompParams:
                if e.split(':')[0] == 's':
                    self.addRow("Candidate scenarios",e.split(':')[1])
                elif e.split(':')[0] == 'r':
                    self.addRow("Chosen scenario",e.split(':')[1])
                elif e.split(':')[0] == 'd':
                    self.addRow("Direct estimate",e.split(':')[1])
                elif e.split(':')[0] == 'n':
                    self.addRow("Chosen number of simulated data",e.split(':')[1])
                elif e.split(':')[0] == 'l':
                    self.addRow("Logistic regression",e.split(':')[1])
                elif e.split(':')[0] == 'm':
                    self.addRow("Regression number",e.split(':')[1])
                elif e.split(':')[0] == 't':
                    self.addRow("Number of requested test data sets",e.split(':')[1])

        self.addRow("computation parameters",self.analysis.computationParameters)

        executablePath = self.parent.parent.preferences_win.getExecutablePath()
        nbMaxThread = self.parent.parent.preferences_win.getMaxThreadNumber()
        params = self.analysis.computationParameters
        cmd_args_list = [executablePath,"-p", "%s/"%self.parent.dir, "-d", '%s'%params, "-i", '%s'%self.analysis.name, "-m", "-t", "%s"%nbMaxThread]
        command_line = " ".join(cmd_args_list)
        self.addRow("executed command line",command_line)

        
    def addRow(self,name,value):
        self.ui.tableWidget.insertRow(self.ui.tableWidget.rowCount())
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,0,QTableWidgetItem("%s"%name))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,1,QTableWidgetItem("%s"%value))

    def exit(self):
        self.parent.ui.analysisStack.removeWidget(self)
        self.parent.ui.analysisStack.setCurrentIndex(0)


