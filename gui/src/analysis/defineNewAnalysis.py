# -*- coding: utf-8 -*-

import sys, codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
from setupComparisonConfidence import SetupComparisonConfidence
from setupEstimationBias import SetupEstimationBias
from genericScenarioSelection import GenericScenarioSelection
from biasScenarioSelection import BiasNConfidenceScenarioSelection
from analysis import Analysis
import output
import variables
from variables import UIPATH

formDefineNewAnalysis,baseDefineNewAnalysis = uic.loadUiType((u"%s/defineAnalysis.ui"%UIPATH).encode(sys.getfilesystemencoding()))

class DefineNewAnalysis(formDefineNewAnalysis,baseDefineNewAnalysis):
    """ écran de définition d'une nouvelle analyse. demande
    le type d'analyse souhaitée
    Si une analyse est fournie au constructeur, il passe directement à l'étape suivante : edition d'analyse
    """
    def __init__(self,parent=None,nbScenario=0,analysis_to_edit=None):
        super(DefineNewAnalysis,self).__init__(parent)
        self.parent=parent
        self.nbScenario = nbScenario
        self.analysis_to_edit = analysis_to_edit
        self.createWidgets()
        self.ui.verticalLayout_2.setAlignment(Qt.AlignHCenter)
        self.ui.verticalLayout_2.setAlignment(Qt.AlignTop)

        self.parent.parent.updateDoc(self)

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.confPcaCheck,SIGNAL("clicked()"),self.preEvaluateCheck)
        QObject.connect(self.ui.lossCheck,SIGNAL("clicked()"),self.preEvaluateCheck)
        QObject.connect(self.ui.modCheckPcaCheck,SIGNAL("clicked()"),self.modCheckCheck)
        QObject.connect(self.ui.modCheckLossCheck,SIGNAL("clicked()"),self.modCheckCheck)

        if self.nbScenario < 2:
            self.ui.comparisonRadio.setDisabled(True)
            self.ui.confidenceRadio.setDisabled(True)

        self.ui.horizontalLayout_3.setAlignment(Qt.AlignHCenter)

        nbsets = str(self.parent.ui.nbSetsDoneEdit.text()).strip()
        nbScenario = len(self.parent.hist_model_win.scList)
        nbStat = self.parent.getNbSumStats()
        verb = "is"
        plur = ""
        if nbScenario > 1:
            verb = "are"
            plur = 's'
        text = "The reference table contains %s records.\nEach record includes %s parameters and %s summary statistics.\nThere %s %s scenario%s."%(nbsets,self.parent.hist_model_win.getNbVariableParam(),nbStat,verb,nbScenario,plur)
        self.ui.titleInfoLabel.setText(text)

        self.analysisNameEdit.setStyleSheet("background-color: #EFB1B3")
        #self.analysisNameEdit.setText("Fill me")
        #self.analysisNameEdit.grabKeyboard()
        #self.analysisNameEdit.setFocus()
        #self.analysisNameEdit.focusInEvent = self.analysisNameEdition
        QObject.connect(self.analysisNameEdit,SIGNAL("textChanged(QString)"),self.analysisNameEdition)

        if self.analysis_to_edit != None:
            # TODO compléter
            for e in [
                    self.ui.preEvRadio,
                    self.ui.estimateRadio,
                    self.ui.biasRadio,
                    self.ui.comparisonRadio,
                    self.ui.confidenceRadio,
                    self.ui.modCheckRadio,

                    self.ui.confPcaCheck,
                    self.ui.lossCheck,
                    self.ui.confFdaCompareCheck,
                    self.ui.confFdaCheck,
                    self.ui.modCheckPcaCheck,
                    self.ui.modCheckLossCheck
                    ]:
                e.setDisabled(True)
            if self.analysis_to_edit.category == "pre-ev":
                self.ui.preEvRadio.setDisabled(False)
                self.ui.preEvRadio.setChecked(True)
                self.ui.lossCheck.setDisabled(False)
                self.ui.confPcaCheck.setDisabled(False)
                self.ui.lossCheck.setChecked(True)
                self.ui.confPcaCheck.setChecked(True)
                self.ui.lossCheck.setChecked("l" in self.analysis_to_edit.computationParameters)
                self.ui.confPcaCheck.setChecked("p" in self.analysis_to_edit.computationParameters)
            elif self.analysis_to_edit.category == "estimate":
                self.ui.estimateRadio.setDisabled(False)
                self.ui.estimateRadio.setChecked(True)
            elif self.analysis_to_edit.category == "compare":
                self.ui.confFdaCompareCheck.setDisabled(False)
                self.ui.confFdaCompareCheck.setChecked(self.analysis_to_edit.fda == "1")
                self.ui.comparisonRadio.setDisabled(False)
                self.ui.comparisonRadio.setChecked(True)
            elif self.analysis_to_edit.category == "modelChecking":
                self.ui.modCheckPcaCheck.setDisabled(False)
                self.ui.modCheckLossCheck.setDisabled(False)
                self.ui.modCheckRadio.setDisabled(False)
                self.ui.modCheckRadio.setChecked(True)
                self.ui.modCheckLossCheck.setChecked(True)
                self.ui.modCheckPcaCheck.setChecked(True)
                self.ui.modCheckLossCheck.setChecked("l" in self.analysis_to_edit.aParams)
                self.ui.modCheckPcaCheck.setChecked("p" in self.analysis_to_edit.aParams)
            elif self.analysis_to_edit.category == "bias":
                self.ui.biasRadio.setDisabled(False)
                self.ui.biasRadio.setChecked(True)
            elif self.analysis_to_edit.category == "confidence":
                self.ui.confFdaCheck.setDisabled(False)
                self.ui.confFdaCheck.setChecked(self.analysis_to_edit.fda == "1")
                self.ui.confidenceRadio.setChecked(True)
                self.ui.confidenceRadio.setDisabled(False)

    def analysisNameEdition(self,string):
        self.analysisNameEdit.setStyleSheet("background-color: ")

    def preEvaluateCheck(self):
        """ empêche qu'aucune des deux cases ne soit cochée
        """
        if not self.ui.confPcaCheck.isChecked() and not self.ui.lossCheck.isChecked():
            if self.sender() == self.ui.confPcaCheck:
                self.ui.lossCheck.setChecked(True)
            else:
                self.ui.confPcaCheck.setChecked(True)

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
        name = str(self.ui.analysisNameEdit.text())
        if name.strip() == "":
            output.notify(self,"name error","Analysis name cannot be empty")
            return False
        for c in self.parent.parent.illegalAnalysisNameCharacters:
            if c in name:
                output.notify(self,"name error","Analysis name contains illegal characters")
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
        # on passe à la suite seulement si le nom est valide ou
        # que l'on a une analyse à éditer. Ne marche que si les 'OU' sont
        # des 'OU SINON' et les 'ET' des 'ET ALORS'
        if self.analysis_to_edit != None or self.checkName():
            name = str(self.ui.analysisNameEdit.text())
            name = name.replace(' ','_')
            genSel = None
            # pour les cas de comparison et estimate, la selection n'influe pas sur l'écran suivant
            # on instancie donc Comparison et Estimation maintenant
            if (self.analysis_to_edit != None and self.analysis_to_edit.category == "compare") or self.ui.comparisonRadio.isChecked():
                if len(self.parent.hist_model_win.scList) >= 2:
                    if self.analysis_to_edit != None:
                        analysis = self.analysis_to_edit
                        self.parent.changeAnalysisName(analysis,name)
                    else:
                        analysis = Analysis(name,"compare")
                    if self.ui.confFdaCompareCheck.isChecked():
                        paramtxt="1"
                    else:
                        paramtxt="0"
                    analysis.fda = paramtxt
                    compFrame = SetupComparisonConfidence(analysis,self)
                    genSel = GenericScenarioSelection(len(self.parent.hist_model_win.scList),"Select the scenarios that you wish to compare",compFrame,"Comparison of scenarios",2,analysis,self)
                else:
                    QMessageBox.information(self,"Scenario error","At least 2 scenarios are needed for this analysis")
            elif (self.analysis_to_edit != None and self.analysis_to_edit.category == "estimate") or self.ui.estimateRadio.isChecked():
                if len(self.parent.hist_model_win.scList) >= 1:
                    if self.analysis_to_edit != None:
                        analysis = self.analysis_to_edit
                        self.parent.changeAnalysisName(analysis,name)
                    else:
                        analysis = Analysis(name,"estimate")
                    estimateFrame = SetupEstimationBias(analysis,self)
                    genSel = GenericScenarioSelection(len(self.parent.hist_model_win.scList),"Parameters will be estimated considering data sets simulated with",estimateFrame,"ABC parameter estimation",1,analysis,self)
                else:
                    QMessageBox.information(self,"Scenario error","At least 1 scenario is needed for this analysis")
            elif (self.analysis_to_edit != None and self.analysis_to_edit.category == "modelChecking") or self.ui.modCheckRadio.isChecked():
                if len(self.parent.hist_model_win.scList) >= 1:
                    compParamtxt = "a:"
                    if self.ui.modCheckPcaCheck.isChecked():
                        compParamtxt += "p"
                    if self.ui.modCheckLossCheck.isChecked():
                        compParamtxt += "l"

                    if self.analysis_to_edit != None:
                        analysis = self.analysis_to_edit
                        self.parent.changeAnalysisName(analysis,name)
                    else:
                        analysis = Analysis(name,"modelChecking")
                    analysis.aParams = compParamtxt
                    modCheckFrame = SetupEstimationBias(analysis,self)
                    genSel = GenericScenarioSelection(len(self.parent.hist_model_win.scList),"Parameters will be estimated considering data sets simulated with",modCheckFrame,"Model Checking",1,analysis,self)
                else:
                    QMessageBox.information(self,"Scenario error","At least 1 scenario is needed for this analysis")
            elif (self.analysis_to_edit != None and self.analysis_to_edit.category == "pre-ev") or self.ui.preEvRadio.isChecked():
                paramtxt = ""
                compParamtxt = "a:"
                if self.ui.confPcaCheck.isChecked():
                    paramtxt+= " PCA "
                    compParamtxt += "p"
                if self.ui.confPcaCheck.isChecked() and self.ui.lossCheck.isChecked():
                    paramtxt+="and"
                if self.ui.lossCheck.isChecked():
                    paramtxt+= " Locate S.S."
                    compParamtxt += "l"

                if self.analysis_to_edit == None:
                    analysis = Analysis(name,"pre-ev")
                    analysis.params = paramtxt
                    analysis.computationParameters = compParamtxt
                    self.parent.addAnalysis(analysis)
                else:
                    self.analysis_to_edit.params = paramtxt
                    self.analysis_to_edit.computationParameters = compParamtxt
                self.exit()
            elif (self.analysis_to_edit != None and self.analysis_to_edit.category == "bias") or self.ui.biasRadio.isChecked():
                if self.analysis_to_edit != None:
                    analysis = self.analysis_to_edit
                    self.parent.changeAnalysisName(analysis,name)
                else:
                    analysis = Analysis(name,"bias")
                genSel = BiasNConfidenceScenarioSelection(len(self.parent.hist_model_win.scList),analysis,self)

            elif (self.analysis_to_edit != None and self.analysis_to_edit.category == "confidence") or self.ui.confidenceRadio.isChecked():
                if self.analysis_to_edit != None:
                    analysis = self.analysis_to_edit
                    self.parent.changeAnalysisName(analysis,name)
                else:
                    analysis = Analysis(name,"confidence")
                if self.ui.confFdaCheck.isChecked():
                    paramtxt="1"
                else:
                    paramtxt="0"
                analysis.fda = paramtxt
                genSel = BiasNConfidenceScenarioSelection(len(self.parent.hist_model_win.scList),analysis,self)
            else:
                output.notify(self,"category error","Choose an analysis type")
            if genSel != None:
                self.parent.ui.analysisStack.addWidget(genSel)
                self.parent.ui.analysisStack.removeWidget(self)
                self.parent.ui.analysisStack.setCurrentWidget(genSel)
                self.parent.parent.updateDoc(genSel)


    def exit(self):
        # reactivation des onglets
        self.parent.ui.analysisStack.removeWidget(self)
        self.parent.ui.analysisStack.setCurrentIndex(0)

