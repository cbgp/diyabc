# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from estimation_ui import Ui_Frame

class Estimation(QFrame):
    def __init__(self,parent=None):
        super(Estimation,self).__init__(parent)
        self.parent=parent
        self.createWidgets()
        self.ui.verticalLayout_3.setAlignment(Qt.AlignHCenter)
        self.ui.verticalLayout_3.setAlignment(Qt.AlignTop)



    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)


    def validate(self):
        self.parent.parent.addRow("parameter estimation","params","5","new")
        self.exit()

    def setScenarios(self,scList):
        print "je set %s scenarios"%scList

    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(1,True)
        self.parent.parent.setTabEnabled(0,True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(1)

