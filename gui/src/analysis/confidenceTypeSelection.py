# -*- coding: utf-8 -*-

import sys, codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
from biasScenarioSelection import BiasNConfidenceScenarioSelection
from historicalModel.setHistDrawnAnalysis import HistDrawn
from historicalModel.setHistFixedAnalysis import HistFixed
from setupComparisonConfidence import SetupComparisonConfidence
import variables
from variables import UIPATH
from utils.cbgpUtils import getFsEncoding, Parents

formConfidenceTypeSelection,baseConfidenceTypeSelection = uic.loadUiType((u"%s/confidenceTypeSelection.ui"%UIPATH).encode(getFsEncoding(logLevel=False)))

class ConfidenceTypeSelection(formConfidenceTypeSelection,baseConfidenceTypeSelection):
    """ Selection du scenario dans la cadre d'une analyse de type bias ou confidence
    """
    def __init__(self,nb_sc,analysis,parent=None):
        super(ConfidenceTypeSelection,self).__init__(parent)
        self.parent=parent
        self.parents = Parents(self.parent)
        self.analysis = analysis
        self.nb_sc = nb_sc
        self.createWidgets()


        self.restoreAnalysisValues()

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)


        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.posteriorConfidenceButton,SIGNAL("clicked()"),self.checkParameterValues)
        QObject.connect(self.ui.priorConfidenceButton,SIGNAL("clicked()"),self.checkParameterValues)
        QObject.connect(self.ui.priorGlobalConfidenceButton,SIGNAL("clicked()"),self.checkParameterValues)
        QObject.connect(self.ui.priorSpecificConfidenceButton,SIGNAL("clicked()"),self.checkParameterValues)
        if "confidence" in self.analysis.category :
            pass
            #self.ui.analysisTypeLabel.setText("Type based confidence")
        else :
            log(1,"Should found a confidence scenario but found : " % self.analysis.category)
            raise Exception("Should found a confidence scenario but found : " % self.analysis.category)

        self.ui.projectDirEdit.setText(self.parents.dir)

        self.ui.analysisNameLabel.setText(self.analysis.name)

        self.checkParameterValues()

    def restoreAnalysisValues(self):
        if self.analysis.category == "confidence_posterior_global":
            self.ui.posteriorConfidenceButton.setChecked(True)
        else :
            if self.analysis.category == "confidence_prior_global":
                self.ui.priorConfidenceButton.setChecked(True)
                self.ui.priorGlobalConfidenceButton.setChecked(True)
            elif self.analysis.category  == "confidence_prior_specific"  or \
                (self.analysis.category  == "confidence" and len(self.analysis.computationParameters) > 1) :
                self.ui.priorConfidenceButton.setChecked(True)
                self.ui.priorSpecificConfidenceButton.setChecked(True)


        self.checkParameterValues(reset=False)

    def checkParameterValues(self, reset=True):
        self.ui.priorTypeFrame.hide()
        if self.ui.posteriorConfidenceButton.isChecked() :
            self.ui.priorGlobalConfidenceButton.setChecked(False)
            self.ui.priorSpecificConfidenceButton.setChecked(False)
        if self.ui.priorConfidenceButton.isChecked() :
            self.ui.priorTypeFrame.show()


    def validate(self):
        """ passe à l'étape suivante de la définition de l'analyse
        """
        next_widget = False
        if self.ui.posteriorConfidenceButton.isChecked() :
            self.analysis.category = "confidence_posterior_global"
            next_widget = SetupComparisonConfidence(analysis=self.analysis, parent=self)
        elif self.ui.priorConfidenceButton.isChecked() :
            if self.ui.priorGlobalConfidenceButton.isChecked() :
                self.analysis.category = "confidence_prior_global"
                next_widget = SetupComparisonConfidence(analysis=self.analysis, parent=self)
            elif self.ui.priorSpecificConfidenceButton.isChecked() :
                self.analysis.category = "confidence_prior_specific"
                next_widget = BiasNConfidenceScenarioSelection(len(self.parents.hist_model_win.scList),self.analysis,self)
            else :
                QMessageBox.information(self,"No prior selection error","You must choose a prior type : GLOBAL or SCENARIO SPECIFIC.")
                return 0

        else :
            QMessageBox.information(self,"No type based selection error","You must choose a type based confidence : Posterior or Prior.")
            return 0
        self.ui.parents.analysisStack.addWidget(next_widget)
        self.ui.parents.analysisStack.removeWidget(self)
        self.ui.parents.analysisStack.setCurrentWidget(next_widget)
        # skip   group loci selection for microsat and seqs analysis


        self.parents.updateDoc(next_widget)



    def exit(self):
        ## reactivation des onglets
        self.ui.parents.analysisStack.removeWidget(self)
        self.ui.parents.analysisStack.setCurrentIndex(0)

