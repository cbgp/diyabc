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
        self.ui.tableWidget.setColumnWidth(0,170)
        self.ui.tableWidget.setColumnWidth(1,600)

        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.exit)

        self.addRow("name",self.analysis.name)
        self.addRow("type",self.analysis.category)
        self.addRow("Selected scenario",self.analysis.chosenSc)
        self.addRow("Selected scenarios",self.analysis.candidateScList)
        self.addRow("computation parameters",self.analysis.computationParameters)

    def addRow(self,name,value):
        self.ui.tableWidget.insertRow(self.ui.tableWidget.rowCount())
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,0,QTableWidgetItem("%s"%name))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,1,QTableWidgetItem("%s"%value))

    def exit(self):
        self.parent.ui.analysisStack.removeWidget(self)
        self.parent.ui.analysisStack.setCurrentIndex(0)


