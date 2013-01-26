# -*- coding: utf-8 -*-

import os
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from PyQt4 import uic
#from uis.viewAnalysisParameters_ui import Ui_Frame
import output
import variables
from variables import UIPATH

formViewAnalysisParameters,baseViewAnalysisParameters = uic.loadUiType("%s/viewAnalysisParameters.ui"%UIPATH)

class ViewAnalysisParameters(formViewAnalysisParameters,baseViewAnalysisParameters):
    def __init__(self,parent,analysis):
        super(ViewAnalysisParameters,self).__init__(parent)
        self.parent = parent
        self.analysis = analysis
        self.createWidgets()

    def createWidgets(self):
        #self.ui = Ui_Frame()
        self.ui = self
        self.setupUi(self)

        self.ui.tableWidget.setColumnWidth(0,250)
        self.ui.tableWidget.setColumnWidth(1,600)

        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.exit)

        self.addRow("name",self.analysis.name,editable=True)
        tab={"pre-ev":"Pre-evaluate scenario-prior combinations",
                "estimate":"Estimate posterior distributions of parameters",
                "bias":"Compute bias and precision on parameter estimations",
                "compare":"Compute posterior probabilities of scenarios",
                "confidence":"Evaluate confidence in scenario choice",
                "modelChecking":"Perform model checking"}
        self.addRow("type",tab[self.analysis.category])
        #if self.analysis.chosenSc != None:
        #    self.addRow("Selected scenario",self.analysis.chosenSc)
        #if self.analysis.candidateScList != [] and self.analysis.candidateScList != None:
        #    self.addRow("Selected scenarios",self.analysis.candidateScList)

        tabCompParams = self.analysis.computationParameters.split(';')
        transDico = {'1':"no",'2':"log",'3':"logit",'4':"logtg"}
        execRowSize = 75
        if self.analysis.category == "estimate" or self.analysis.category == "modelChecking" or self.analysis.category == "bias":
            if self.analysis.category == "bias":
                execRowSize = 200
            for e in tabCompParams:
                if e.split(':')[0] == 'n':
                    self.addRow("Chosen number of simulated data",e.split(':')[1])
                elif e.split(':')[0] == 'm':
                    self.addRow("Number of selected data",e.split(':')[1])
                elif e.split(':')[0] == 'd':
                    self.addRow("Number of test data sets",e.split(':')[1])
                elif e.split(':')[0] == 't':
                    self.addRow("Transformation applied to parameters",transDico[e.split(':')[1]])
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
        elif self.analysis.category == "confidence":
            execRowSize = 200
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

        executablePath = self.parent.parent.preferences_win.getExecutablePath()
        nbMaxThread = self.parent.parent.preferences_win.getMaxThreadNumber()
        particleLoopSize = str(self.parent.parent.preferences_win.particleLoopSizeEdit.text())
        params = self.analysis.computationParameters

        #self.addRow("computation parameters",self.analysis.computationParameters)
        c = self.analysis.category
        if c == "estimate":
            option = "-e"
        elif c == "modelChecking":
            option = "-j"
        elif c == "bias":
            option = "-b"
            particleLoopSize = int(params.split('d:')[1].split(';')[0])
        elif c == "compare":
            option = "-c"
        elif c == "confidence":
            option = "-f"
            particleLoopSize = int(params.split('t:')[1].split(';')[0])
        elif c == "pre-ev":
            option = "-d"

        #cmd_args_list = [executablePath,"-p", "%s/"%self.parent.dir, option, '"%s"'%params, "-i", '%s'%self.analysis.name, "-m", "-t", "%s"%nbMaxThread]
        cmd_args_list = ['"'+executablePath+'"',"-p", '"%s/"'%self.parent.dir, "%s"%option, '"%s"'%params, "-i", '%s'%self.analysis.name,"-g" ,"%s"%particleLoopSize , "-m", "-t", "%s"%nbMaxThread]
        command_line = " ".join(cmd_args_list)
        self.addRow("executed command line",command_line,editable=True)
        self.ui.tableWidget.setRowHeight(self.ui.tableWidget.rowCount()-1,execRowSize)

        QObject.connect(self.ui.tableWidget,SIGNAL("cellChanged(int,int)"),self.cellChanged)

    def cellChanged(self,row,col):
        if row == 0 and col == 1:
            val = str(self.ui.tableWidget.item(0,1).text())
            self.parent.changeAnalysisName(self.analysis,val)
        
    def addRow(self,name,value,editable=False):
        self.ui.tableWidget.insertRow(self.ui.tableWidget.rowCount())
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,0,QTableWidgetItem("%s"%name))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,1,QTableWidgetItem("%s"%value))
        self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,0).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,0).flags() & ~Qt.ItemIsEditable & ~Qt.ItemIsSelectable)
        if not editable:
            self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,1).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,1).flags() & ~Qt.ItemIsEditable & ~Qt.ItemIsSelectable)

    def exit(self):
        self.parent.ui.analysisStack.removeWidget(self)
        self.parent.ui.analysisStack.setCurrentIndex(0)


