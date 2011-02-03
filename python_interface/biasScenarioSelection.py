# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from biasScenarioSelection_ui import Ui_Frame

class BiasNEvaluateScenarioSelection(QFrame):
    # TODO refaire nextwidget
    def __init__(self,nb_sc,evaluate,parent=None):
        super(BiasNEvaluateScenarioSelection,self).__init__(parent)
        self.parent=parent
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

    def validate(self):
        if evaluate:
            if len(self.getListSelectedScenarios()) >= 2:
                self.next_widget.setScenarios(self.getListSelectedScenarios())
                self.parent.parent.addTab(self.next_widget,self.next_title)
                self.parent.parent.removeTab(self.parent.parent.indexOf(self))
                self.parent.parent.setCurrentWidget(self.next_widget)
            else:
                QMessageBox.information(self,"Selection error","At least %s scenarios have to be selected"%self.nb_min_sel)


    def getListSelectedScenarios(self):
        res = []
        for i,ch in enumerate(self.checklist):
            if ch.isChecked():
                res.append(i+1)
        return res

    def putChoices(self):
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
                self.checklist.append(check)
                self.ui.verticalLayout_6.addWidget(check)
        else:
            self.ui.label.hide()
            self.ui.frame_3.hide()


        ## les deux cas où on a pas le choix des scenarios parce qu'ils sont trop peu
        #if self.nb_sc == 1:
        #    self.checklist[0].setChecked(True)
        #    self.checklist[0].setDisabled(True)
        #    self.next_widget.ui.redefButton.hide()
        #print type(self.next_widget)
        #if self.nb_sc == 2 and "Comparison" in str(type(self.next_widget)):
        #    self.checklist[0].setChecked(True)
        #    self.checklist[0].setDisabled(True)
        #    self.checklist[1].setChecked(True)
        #    self.checklist[1].setDisabled(True)
        #    self.next_widget.ui.redefButton.hide()


            

    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(1,True)
        self.parent.parent.setTabEnabled(0,True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(1)

