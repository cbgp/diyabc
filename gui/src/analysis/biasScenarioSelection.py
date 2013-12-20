# -*- coding: utf-8 -*-

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
from historicalModel.setHistDrawnAnalysis import HistDrawn
from historicalModel.setHistFixedAnalysis import HistFixed
import variables
from variables import UIPATH

formBiasScenarioSelection,baseBiasScenarioSelection = uic.loadUiType("%s/biasScenarioSelection.ui"%UIPATH)

class BiasNConfidenceScenarioSelection(formBiasScenarioSelection,baseBiasScenarioSelection):
    """ Sélection du scenario dans la cadre d'une analyse de type bias ou confidence
    """
    def __init__(self,nb_sc,analysis,parent=None):
        super(BiasNConfidenceScenarioSelection,self).__init__(parent)
        self.parent=parent
        self.analysis = analysis
        self.nb_sc = nb_sc
        self.checklist = []
        self.radiolist = []
        self.createWidgets()

        self.putChoices()

        self.ui.verticalLayout_3.setAlignment(Qt.AlignHCenter)
        self.ui.verticalLayout_3.setAlignment(Qt.AlignTop)
        self.ui.verticalLayout_2.setAlignment(Qt.AlignHCenter)
        self.ui.verticalLayout_2.setAlignment(Qt.AlignTop)
        self.ui.verticalLayout.setAlignment(Qt.AlignHCenter)
        self.ui.verticalLayout.setAlignment(Qt.AlignTop)

        self.restoreAnalysisValues()

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)


        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.fixedRadio,SIGNAL("clicked()"),self.checkParameterValues)
        QObject.connect(self.ui.drawnPriorRadio,SIGNAL("clicked()"),self.checkParameterValues)
        QObject.connect(self.ui.drawnPosteriorRadio,SIGNAL("clicked()"),self.checkParameterValues)

        if self.analysis.category == "bias":
            self.ui.analysisTypeLabel.setText("Bias and mean square error")
        else:
            self.ui.analysisTypeLabel.setText("Confidence in scenario choice")
        self.ui.projectDirEdit.setText(self.parent.parent.dir)

        self.ui.analysisNameLabel.setText(self.analysis.name)

        # hide posterior option
        if self.analysis.name[0:4].upper() != 'CBGP' :
             self.ui.drawnPosteriorRadio.hide()
        self.checkParameterValues()

    def restoreAnalysisValues(self):
        if self.analysis.chosenSc != None:
            self.radiolist[self.analysis.chosenSc - 1].setChecked(True)

        if self.analysis.category == "confidence":
            for i in range(self.nb_sc):
                num = i+1
                check = self.checklist[i]
                check.setChecked(num in self.analysis.candidateScList)
        if self.analysis.drawn != None:
            print "self.analysis.drawn = " , self.analysis.drawn
            if self.analysis.drawn is False :
                self.ui.fixedRadio.setChecked(True)
            # in legacy analysis, drawn can be set to True for prior :
            elif  self.analysis.drawn is True :
                self.ui.drawnPriorRadio.setChecked(True)
            else :
                self.ui.drawnPriorRadio.setChecked('prior' in self.analysis.drawn)
                self.ui.drawnPosteriorRadio.setChecked('posterior' in self.analysis.drawn)

        cp = self.analysis.computationParameters
        posteriorDataSetNumber = ""
        try:
            posteriorDataSetNumber = cp.split('z:')[1].split(';')[0]
        except IndexError:
            pass
        posteriorSimulatedDataSetNumber = ""
        try:
            posteriorSimulatedDataSetNumber = cp.split('a:')[1].split(';')[0]
        except IndexError:
            pass
        self.ui.posteriorDataSetNumberEdit.setText(posteriorDataSetNumber)
        self.ui.posteriorSimulatedDataSetNumberEdit.setText(posteriorSimulatedDataSetNumber)
        self.checkParameterValues(reset=False)

    def checkParameterValues(self, reset=True):
        self.ui.posteriorDataSetNumberFrame.hide()
        if self.analysis.category == "confidence" and self.ui.drawnPosteriorRadio.isChecked():
            nbRecordsSelectedScenario = self.parent.parent.readNbRecordsOfScenario(int(self.getSelectedScenario()))
            self.ui.maxPosteriorDataSetNumberLabel.setText("max = %s"%nbRecordsSelectedScenario)
            if str(self.ui.posteriorSimulatedDataSetNumberEdit.text()) is '' :
                self.ui.posteriorSimulatedDataSetNumberEdit.setText(str(nbRecordsSelectedScenario))
            elif int(str(self.ui.posteriorSimulatedDataSetNumberEdit.text()))  >  nbRecordsSelectedScenario :
                self.ui.posteriorSimulatedDataSetNumberEdit.setText(str(nbRecordsSelectedScenario))
            if reset or str(self.ui.posteriorDataSetNumberEdit.text()) is ''  :
                self.ui.posteriorDataSetNumberEdit.setText(str(nbRecordsSelectedScenario/100))
            self.ui.posteriorDataSetNumberFrame.show()


    def validate(self):
        """ passe à l'étape suivante de la définition de l'analyse
        """
        # pour confidence et bias, on a selectionné un scenario
        self.analysis.chosenSc = self.getSelectedScenario()

        if self.ui.fixedRadio.isChecked():
            self.analysis.drawn = False
        elif self.ui.drawnPriorRadio.isChecked():
            self.analysis.drawn = 'prior'
        elif self.ui.drawnPosteriorRadio.isChecked():
            self.analysis.drawn = 'posterior'
            if self.analysis.category == "confidence":
                try :
                    if int(str(self.ui.posteriorSimulatedDataSetNumberEdit.text())) > self.parent.parent.readNbRecordsOfScenario(int(self.analysis.chosenSc)) \
                      or  int(str(self.ui.posteriorSimulatedDataSetNumberEdit.text())) <= 0 :
                        raise
                except :
                     QMessageBox.information(self,"Number error","Total number of simulated data considered for local regression should be a positive number and inferior to the number of records of the scenario %s : %s" \
                                                %(self.analysis.chosenSc, self.parent.parent.readNbRecordsOfScenario(int(self.analysis.chosenSc)) ))
                     return 0
                try :
                    if int(str(self.ui.posteriorDataSetNumberEdit.text())) > int(str(self.ui.posteriorSimulatedDataSetNumberEdit.text())) \
                      or  int(str(self.ui.posteriorDataSetNumberEdit.text())) <= 0 :
                        raise
                except :
                     QMessageBox.information(self,"Number error","Number of datasets for local linear regression (simulate data closest to observed) should be a positive number and inferior to the number of records of the scenario %s : %s" \
                                                %(self.analysis.chosenSc, str(self.ui.posteriorSimulatedDataSetNumberEdit.text()) ))
                     return 0


        # le cas du confidence, les sc à afficher dans le hist model sont ceux selectionnés
        if self.analysis.category == "confidence":
            if len(self.getListSelectedScenarios()) >= 2:
                # pour confidence on a du selectionner au moins deux scenarios
                self.analysis.candidateScList = self.getListSelectedScenarios()
                if self.ui.fixedRadio.isChecked():
                    next_widget = HistFixed(self.analysis,self.parent)
                elif self.ui.drawnPosteriorRadio.isChecked():
                    #Quick and not so dirty patch to avoid prior config panel when confidence use posteriors
                    tmpWidget = HistDrawn(self.analysis,self.parent)
                    next_widget = tmpWidget.parent.parent.getNextWidget(tmpWidget)
                # prior
                else:
                    next_widget = HistDrawn(self.analysis,self.parent)
            else:
                QMessageBox.information(self,"Selection error","At least %s scenarios have to be selected"%2)
                return 0
        # le cas du bias, un sc à afficher, celui du radio button
        else:
            # en fonction de fixed ou drawn, l'écran suivant présente un objet différent
            if self.ui.fixedRadio.isChecked():
                next_widget = HistFixed(self.analysis,self.parent)
            elif self.ui.drawnPosteriorRadio.isChecked():
                #Quick and not so dirty patch to avoid prior config panel when confidence use posteriors
                tmpWidget = HistDrawn(self.analysis,self.parent)
                next_widget = tmpWidget.parent.parent.getNextWidget(tmpWidget)
            else:
                next_widget = HistDrawn(self.analysis,self.parent)
        self.parent.parent.ui.analysisStack.addWidget(next_widget)
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentWidget(next_widget)
        # skip   group loci selection for microsat and seqs analysis
        if not self.parent.parent.isSnp() :
            next_widget.validate()

        self.parent.parent.parent.updateDoc(next_widget)

    def getSelectedScenario(self):
        """ retourne le numero du scenario choisi
        """
        for i,r in enumerate(self.radiolist):
            if r.isChecked():
                return i+1

    def getListSelectedScenarios(self):
        """ retourne la liste des scenarios choisis
        """
        res = []
        for i,ch in enumerate(self.checklist):
            if ch.isChecked():
                res.append(i+1)
        return res

    def putChoices(self):
        """ met les choix de scenario en place
        """
        for i in range(self.nb_sc):
            num = i+1
            radio = QRadioButton("Scenario %s"%num,self)
            self.radiolist.append(radio)
            self.ui.verticalLayout_4.addWidget(radio)
            QObject.connect(radio,SIGNAL("clicked()"),self.checkParameterValues)
        self.radiolist[0].setChecked(True)

        if self.analysis.category == "confidence":
            for i in range(self.nb_sc):
                num = i+1
                check = QCheckBox("Scenario %s"%num,self)
                check.setChecked(True)
                self.checklist.append(check)
                self.ui.verticalLayout_6.addWidget(check)
        else:
            self.ui.candidateScenariosLabel.hide()
            self.ui.candidateScenariosTipLabel.hide()
            self.ui.frame_3.hide()

    def exit(self):
        ## reactivation des onglets
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentIndex(0)

