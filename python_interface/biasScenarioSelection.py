# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from uis.biasScenarioSelection_ui import Ui_Frame
from histDrawn import HistDrawn
from histFixed import HistFixed

class BiasNEvaluateScenarioSelection(QFrame):
    """ Sélection du scenario dans la cadre d'une analyse de type bias ou evaluate
    """
    def __init__(self,nb_sc,evaluate,analysis,parent=None):
        super(BiasNEvaluateScenarioSelection,self).__init__(parent)
        self.parent=parent
        self.analysis = analysis
        self.nb_sc = nb_sc
        # on affiche ou pas la partie droite "select candidate scenarios"
        self.evaluate = evaluate
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

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)


        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)

        if self.analysis[0] == "bias":
            self.ui.analysisTypeLabel.setText("Bias and mean square error")
        else:
            self.ui.analysisTypeLabel.setText("Confidence in scenario choice")
        self.ui.projectDirEdit.setText(self.parent.parent.dir)

    def validate(self):
        """ passe à l'étape suivante de la définition de l'analyse
        """
        if self.ui.fixedRadio.isChecked():
            self.analysis.append("fixed")
        else:
            self.analysis.append("drawn")
        # pour evaluate et bias, on a selectionné un scenario
        self.analysis.append(self.getSelectedScenario())
        # le cas du evaluate, les sc à afficher dans le hist model sont ceux selectionnés
        if self.evaluate:
            if len(self.getListSelectedScenarios()) >= 2:
                # pour evaluate on a du selectionner au moins deux scenarios 
                self.analysis.append(self.getListSelectedScenarios())
                if self.ui.fixedRadio.isChecked():
                    next_widget = HistFixed(self.getSelectedScenario(),self.getListSelectedScenarios(),self.analysis,self.parent)
                else:
                    next_widget = HistDrawn(self.getSelectedScenario(),self.getListSelectedScenarios(),self.analysis,self.parent)
            else:
                QMessageBox.information(self,"Selection error","At least %s scenarios have to be selected"%self.nb_min_sel)
                return 0
        # le cas du bias, un sc à afficher, celui du radio button
        else:
            # en fonction de fixed ou drawn, l'écran suivant présente un objet différent
            if self.ui.fixedRadio.isChecked():
                next_widget = HistFixed(self.getSelectedScenario(),None,self.analysis,self.parent)
            else:
                next_widget = HistDrawn(self.getSelectedScenario(),None,self.analysis,self.parent)
        #self.parent.parent.addTab(next_widget,"Historical model")
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentWidget(next_widget)
        self.parent.parent.ui.analysisStack.addWidget(next_widget)
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentWidget(next_widget)
        


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
        self.radiolist[0].setChecked(True)

        if self.evaluate:
            for i in range(self.nb_sc):
                num = i+1
                check = QCheckBox("Scenario %s"%num,self)
                check.setChecked(True)
                self.checklist.append(check)
                self.ui.verticalLayout_6.addWidget(check)
        else:
            self.ui.label.hide()
            self.ui.frame_3.hide()

    def exit(self):
        ## reactivation des onglets
        #self.parent.parent.setTabEnabled(1,True)
        #self.parent.parent.setTabEnabled(0,True)
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentIndex(1)
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentIndex(0)

