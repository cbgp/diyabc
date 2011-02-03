# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from defineNewAnalysis_ui import Ui_Frame
from comparison import Comparison
from estimation import Estimation
from genericScenarioSelection import GenericScenarioSelection
from biasScenarioSelection import BiasNEvaluateScenarioSelection

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
            if len(self.parent.hist_model_win.scList) >= 2:
                compFrame = Comparison(self)
                genSel = GenericScenarioSelection(len(self.parent.hist_model_win.scList),"Select the scenarios that you wish to compare",compFrame,"Comparison of scenarios",2,self)
                self.parent.addTab(genSel,"Scenario selection")
                self.parent.removeTab(self.parent.indexOf(self))
                self.parent.setCurrentWidget(genSel)
            else:
                QMessageBox.information(self,"Scenario error","At least 2 scenarios are needed for this analysis")
        elif self.ui.estimateRadio.isChecked():
            if len(self.parent.hist_model_win.scList) >= 1:
                estimateFrame = Estimation(self)
                genSel = GenericScenarioSelection(len(self.parent.hist_model_win.scList),"Parameters will be estimated considering data sets simulated with",estimateFrame,"ABC parameter estimation",1,self)
                self.parent.addTab(genSel,"Scenario selection")
                self.parent.removeTab(self.parent.indexOf(self))
                self.parent.setCurrentWidget(genSel)
            else:
                QMessageBox.information(self,"Scenario error","At least 1 scenario is needed for this analysis")
        elif self.ui.preEvRadio.isChecked():
            paramtxt = ""
            if self.ui.pcaCheck.isChecked():
                paramtxt+= " PCA "
            if self.ui.pcaCheck.isChecked() and self.ui.lossCheck.isChecked():
                paramtxt+="and"
            if self.ui.lossCheck.isChecked():
                paramtxt+= " Locate S.S."
            self.parent.addRow("scenario prior combination",paramtxt,"4","new")
            self.exit()
        elif self.ui.biasRadio.isChecked():
            genSel = BiasNEvaluateScenarioSelection(len(self.parent.hist_model_win.scList),False,self)
            self.parent.addTab(genSel,"Scenario selection")
            self.parent.removeTab(self.parent.indexOf(self))
            self.parent.setCurrentWidget(genSel)

        elif self.ui.evaluateRadio.isChecked():
            genSel = BiasNEvaluateScenarioSelection(len(self.parent.hist_model_win.scList),True,self)
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

