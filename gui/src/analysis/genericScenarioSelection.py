# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
from utils.cbgpUtils import log
import variables
from variables import UIPATH

formGenericScenarioSelection,baseGenericScenarioSelection = uic.loadUiType("%s/genericScenarioSelection.ui"%UIPATH)

class GenericScenarioSelection(formGenericScenarioSelection,baseGenericScenarioSelection):
    """ Sélection du scenario dans la cadre d'une analyse de type comparison ou estimate
    """
    def __init__(self,nb_sc,label,next_widget,next_title,nb_min_sel,analysis,parent):
        super(GenericScenarioSelection,self).__init__(parent)
        self.parent=parent
        self.analysis = analysis
        self.nb_sc = nb_sc
        self.nb_min_sel = nb_min_sel
        self.next_widget = next_widget
        self.next_title = next_title
        self.checklist = []
        self.createWidgets()
        self.ui.selectionLabel.setText(label)

        self.ui.analysisTypeLabel.setText(self.next_title)
        self.ui.projectDirEdit.setText(self.parent.parent.dir)

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

        self.ui.analysisNameLabel.setText(self.analysis.name)

    def restoreAnalysisValues(self):
        """ Si on est en edition d'analyse, elle possède déjà des valeurs que 
        l'on doit afficher
        """
        if self.analysis.candidateScList != []:
            for i,ch in enumerate(self.checklist):
                ch.setChecked((i+1) in self.analysis.candidateScList)

    def validate(self):
        """ passe à l'étape suivante de définition de l'analyse
        """
        if len(self.getListSelectedScenarios()) >= self.nb_min_sel:
            self.analysis.candidateScList = self.getListSelectedScenarios()
            self.next_widget.setScenarios(self.analysis.candidateScList)
            analysis_in_edition = (self.analysis.computationParameters != "")
            if self.analysis.category == "compare" and not analysis_in_edition:
                self.next_widget.setRecordValues(self.analysis.candidateScList)
            self.parent.parent.ui.analysisStack.addWidget(self.next_widget)
            self.parent.parent.ui.analysisStack.removeWidget(self)
            self.parent.parent.ui.analysisStack.setCurrentWidget(self.next_widget)
            self.parent.parent.parent.updateDoc(self.next_widget)
        else:
            QMessageBox.information(self,"Selection error","At least %s scenarios have to be selected"%self.nb_min_sel)

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
        bg = QButtonGroup(self)
        left = True
        for i in range(self.nb_sc):
            num = i+1
            if self.analysis.category == "modelChecking":
                check = QRadioButton("Scenario %s"%num,self)
                bg.addButton(check)
            else:
                check = QCheckBox("Scenario %s"%num,self)
            if self.analysis.category == "compare":
                check.setChecked(True)
            self.checklist.append(check)
            if left:
                self.ui.verticalLayout.addWidget(check)
            else:
                self.ui.verticalLayout_2.addWidget(check)
            left = not left
        # les deux cas où on a pas le choix des scenarios parce qu'ils sont trop peu
        if self.nb_sc == 1:
            self.checklist[0].setChecked(True)
            self.checklist[0].setDisabled(True)
            self.next_widget.ui.redefButton.hide()
        log(4,"type of next widget : %s"%type(self.next_widget))
        if self.nb_sc == 2 and "Comparison" in str(type(self.next_widget)):
            self.checklist[0].setChecked(True)
            self.checklist[0].setDisabled(True)
            self.checklist[1].setChecked(True)
            self.checklist[1].setDisabled(True)
            self.next_widget.ui.redefButton.hide()

    def exit(self):
        # reactivation des onglets
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentIndex(0)

