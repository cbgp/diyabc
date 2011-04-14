# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from uis.defineNewAnalysis_ui import Ui_Frame
from setupComparisonEvaluation import SetupComparisonEvaluation
from setupEstimationBias import SetupEstimationBias
from genericScenarioSelection import GenericScenarioSelection
from biasScenarioSelection import BiasNEvaluateScenarioSelection
from analysis import Analysis
import output

class DefineNewAnalysis(QFrame):
    """ écran de définition d'une nouvelle analyse. demande
    le type d'analyse souhaitée
    """
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
        QObject.connect(self.ui.modCheckPcaCheck,SIGNAL("clicked()"),self.modCheckCheck)
        QObject.connect(self.ui.modCheckLossCheck,SIGNAL("clicked()"),self.modCheckCheck)

        self.ui.horizontalLayout_3.setAlignment(Qt.AlignHCenter)

        nbsets = str(self.parent.ui.nbSetsDoneEdit.text()).strip()
        nbScenario = len(self.parent.hist_model_win.scList)
        nbStat = self.parent.gen_data_win.getNbSumStats()
        verb = "is"
        plur = ""
        if nbScenario > 1:
            verb = "are"
            plur = 's'
        text = "The reference table contains %s records.\nEach record includes %s parameters and %s summary statistics.\nThere %s %s scenario%s."%(nbsets,self.parent.hist_model_win.getNbVariableParam(),nbStat,verb,nbScenario,plur)
        self.ui.titleLabel.setText(text)

    def preEvaluateCheck(self):
        """ empêche qu'aucune des deux cases ne soit cochée
        """
        if not self.ui.pcaCheck.isChecked() and not self.ui.lossCheck.isChecked():
            if self.sender() == self.ui.pcaCheck:
                self.ui.lossCheck.setChecked(True)
            else:
                self.ui.pcaCheck.setChecked(True)

    def modCheckCheck(self):
        """ empêche qu'aucune des deux cases ne soit cochée
        """
        if not self.ui.modCheckPcaCheck.isChecked() and not self.ui.modCheckLossCheck.isChecked():
            if self.sender() == self.ui.modCheckPcaCheck:
                self.ui.modCheckLossCheck.setChecked(True)
            else:
                self.ui.modCheckPcaCheck.setChecked(True)

    def checkName(self):
        """ vérifie si le nom est valide et libre
        """
        # TODO verifier que l'analyse n'existe pas deja
        # TODO verifier les caractères interdits
        name = str(self.ui.analysisNameEdit.text())
        if name.strip() == "":
            output.notify(self,"name error","Analysis name cannot be empty")
            return False
        for an in self.parent.analysisList:
            n = an.name
            if name.strip() == n.strip():
                output.notify(self,"name error","Name already used")
                return False
        return True

    def validate(self):
        """ on poursuit la définition de l'analyse
        """
        if self.checkName():
            name = str(self.ui.analysisNameEdit.text())
            # pour les cas de comparison et estimate, la selection n'influe pas sur l'écran suivant
            # on instancie donc Comparison et Estimation maintenant
            if self.ui.comparisonRadio.isChecked():
                if len(self.parent.hist_model_win.scList) >= 2:
                    analysis = Analysis(name,"compare")
                    compFrame = SetupComparisonEvaluation(analysis,self)
                    genSel = GenericScenarioSelection(len(self.parent.hist_model_win.scList),"Select the scenarios that you wish to compare",compFrame,"Comparison of scenarios",2,analysis,self)
                    #self.parent.addTab(genSel,"Scenario selection")
                    #self.parent.removeTab(self.parent.indexOf(self))
                    #self.parent.setCurrentWidget(genSel)
                    self.parent.ui.analysisStack.addWidget(genSel)
                    self.parent.ui.analysisStack.removeWidget(self)
                    self.parent.ui.analysisStack.setCurrentWidget(genSel)
                else:
                    QMessageBox.information(self,"Scenario error","At least 2 scenarios are needed for this analysis")
            elif self.ui.estimateRadio.isChecked():
                if len(self.parent.hist_model_win.scList) >= 1:
                    analysis = Analysis(name,"estimate")
                    estimateFrame = SetupEstimationBias(analysis,self)
                    genSel = GenericScenarioSelection(len(self.parent.hist_model_win.scList),"Parameters will be estimated considering data sets simulated with",estimateFrame,"ABC parameter estimation",1,analysis,self)
                    #self.parent.addTab(genSel,"Scenario selection")
                    #self.parent.removeTab(self.parent.indexOf(self))
                    #self.parent.setCurrentWidget(genSel)
                    self.parent.ui.analysisStack.addWidget(genSel)
                    self.parent.ui.analysisStack.removeWidget(self)
                    self.parent.ui.analysisStack.setCurrentWidget(genSel)
                else:
                    QMessageBox.information(self,"Scenario error","At least 1 scenario is needed for this analysis")
            elif self.ui.modCheckRadio.isChecked():
                if len(self.parent.hist_model_win.scList) >= 1:
                    compParamtxt = "a:"
                    if self.ui.modCheckPcaCheck.isChecked():
                        compParamtxt += "p"
                    if self.ui.modCheckLossCheck.isChecked():
                        compParamtxt += "l"
                    analysis = Analysis(name,"modelChecking")
                    analysis.aParams = compParamtxt
                    modCheckFrame = SetupEstimationBias(analysis,self)
                    genSel = GenericScenarioSelection(len(self.parent.hist_model_win.scList),"Parameters will be estimated considering data sets simulated with",modCheckFrame,"Model Checking",1,analysis,self)
                    self.parent.ui.analysisStack.addWidget(genSel)
                    self.parent.ui.analysisStack.removeWidget(self)
                    self.parent.ui.analysisStack.setCurrentWidget(genSel)
                else:
                    QMessageBox.information(self,"Scenario error","At least 1 scenario is needed for this analysis")
            elif self.ui.preEvRadio.isChecked():
                paramtxt = ""
                compParamtxt = "a:"
                if self.ui.pcaCheck.isChecked():
                    paramtxt+= " PCA "
                    compParamtxt += "p"
                if self.ui.pcaCheck.isChecked() and self.ui.lossCheck.isChecked():
                    paramtxt+="and"
                if self.ui.lossCheck.isChecked():
                    paramtxt+= " Locate S.S."
                    compParamtxt += "l"
                analysis = Analysis(name,"pre-ev")
                analysis.params = paramtxt
                analysis.computationParameters = compParamtxt
                self.parent.addAnalysis(analysis)
                self.exit()
            elif self.ui.biasRadio.isChecked():
                analysis = Analysis(name,"bias")
                genSel = BiasNEvaluateScenarioSelection(len(self.parent.hist_model_win.scList),analysis,self)
                #self.parent.addTab(genSel,"Scenario selection")
                #self.parent.removeTab(self.parent.indexOf(self))
                #self.parent.setCurrentWidget(genSel)
                self.parent.ui.analysisStack.addWidget(genSel)
                self.parent.ui.analysisStack.removeWidget(self)
                self.parent.ui.analysisStack.setCurrentWidget(genSel)

            elif self.ui.evaluateRadio.isChecked():
                analysis = Analysis(name,"evaluate")
                genSel = BiasNEvaluateScenarioSelection(len(self.parent.hist_model_win.scList),analysis,self)
                #self.parent.addTab(genSel,"Scenario selection")
                #self.parent.removeTab(self.parent.indexOf(self))
                #self.parent.setCurrentWidget(genSel)
                self.parent.ui.analysisStack.addWidget(genSel)
                self.parent.ui.analysisStack.removeWidget(self)
                self.parent.ui.analysisStack.setCurrentWidget(genSel)

            else:
                print "NYI"

    def exit(self):
        ## reactivation des onglets
        #self.parent.setTabEnabled(1,True)
        #self.parent.setTabEnabled(0,True)
        #self.parent.removeTab(self.parent.indexOf(self))
        #self.parent.setCurrentIndex(1)
        self.parent.ui.analysisStack.removeWidget(self)
        self.parent.ui.analysisStack.setCurrentIndex(0)

