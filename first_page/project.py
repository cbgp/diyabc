# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from first_page_ui import Ui_MainWindow
from project import *
from project_ui import *

class Project(QTabWidget):
    def __init__(self,ui,name):
        self.mainUi = ui

        super(Project,self).__init__(None)
        # instanciation du widget project_ui
        self.ui = Ui_TabWidget()
        self.ui.setupUi(self)

        # manual alignment set
        self.ui.verticalLayout_2.setAlignment(self.ui.newAnButton,Qt.AlignCenter)
        self.ui.projNameLabel.setText(name)
        self.ui.tableWidget.setColumnWidth(1,150)
        self.ui.tableWidget.setColumnWidth(2,300)
        self.ui.tableWidget.setColumnWidth(3,70)


        #it = QTableWidgetItem("%i,%i"%(j,i))
        self.ui.tableWidget.setCellWidget(0,5,QPushButton("View"))


        # ajout au tabwidget de l'ui principale
        # ajout de l'onglet
        self.mainUi.tabWidget.addTab(self,name)
        self.mainUi.tabWidget.setCurrentWidget(self)


