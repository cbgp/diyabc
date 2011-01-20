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

        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)
        self.ui.verticalLayout_3.setAlignment(QtCore.Qt.AlignTop)

        #QObject.connect(self.ui.addScButton,SIGNAL("clicked()"),self.addSc)
