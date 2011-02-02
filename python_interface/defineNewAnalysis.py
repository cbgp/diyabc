# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from defineNewAnalysis_ui import Ui_Frame
from comparison import Comparison
from genericScenarioSelection import GenericScenarioSelection

class DefineNewAnalysis(QFrame):
    def __init__(self,parent=None):
        super(DefineNewAnalysis,self).__init__(parent)
        self.parent=parent
        self.createWidgets()
        self.ui.verticalLayout_2.setAlignment(Qt.AlignHCenter)
        self.ui.verticalLayout_2.setAlignment(Qt.AlignTop)



    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.pcaCheck,SIGNAL("clicked()"),self.preEvaluateCheck)
        QObject.connect(self.ui.lossCheck,SIGNAL("clicked()"),self.preEvaluateCheck)

    def preEvaluateCheck(self):
        if not self.ui.pcaCheck.isChecked() and not self.ui.lossCheck.isChecked():
            if self.sender() == self.ui.pcaCheck:
                self.ui.lossCheck.setChecked(True)
            else:
                self.ui.pcaCheck.setChecked(True)

    def validate(self):
        if self.ui.comparisonRadio.isChecked():
            compFrame = Comparison(self)
            genSel = GenericScenarioSelection(len(self.parent.hist_model_win.scList),"Select the scenarios that you wish to compare",compFrame,"Comparison of scenarios",self)
            self.parent.addTab(genSel,"Scenario selection")
            self.parent.removeTab(self.parent.indexOf(self))
            self.parent.setCurrentWidget(genSel)
        else:
            print "NYI"

    def exit(self):
        # reactivation des onglets
        self.parent.setTabEnabled(1,True)
        self.parent.setTabEnabled(0,True)
        self.parent.removeTab(self.parent.indexOf(self))
        self.parent.setCurrentIndex(1)

