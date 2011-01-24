# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setSummaryStatistics_ui import Ui_Frame

class SetSummaryStatistics(QFrame):
    def __init__(self,parent=None):
        super(SetSummaryStatistics,self).__init__(parent)
        self.parent=parent
        self.createWidgets()

        #self.ui.verticalLayout_27.setAlignment(Qt.AlignTop)
        #self.ui.verticalLayout_26.setAlignment(Qt.AlignTop)

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)

    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))



