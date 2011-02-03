# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from genericScenarioSelection_ui import Ui_Frame

class GenericScenarioSelection(QFrame):
    def __init__(self,nb_sc,label,next_widget,next_title,nb_min_sel,parent=None):
        super(GenericScenarioSelection,self).__init__(parent)
        self.parent=parent
        self.nb_sc = nb_sc
        self.nb_min_sel = nb_min_sel
        self.next_widget = next_widget
        self.next_title = next_title
        self.checklist = []
        self.createWidgets()
        self.ui.selectionLabel.setText(label)

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
        if len(self.getListSelectedScenarios()) >= self.nb_min_sel:
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
        left = True
        for i in range(self.nb_sc):
            num = i+1
            check = QCheckBox("Scenario %s"%num,self)
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
        print type(self.next_widget)
        if self.nb_sc == 2 and "Comparison" in str(type(self.next_widget)):
            self.checklist[0].setChecked(True)
            self.checklist[0].setDisabled(True)
            self.checklist[1].setChecked(True)
            self.checklist[1].setDisabled(True)
            self.next_widget.ui.redefButton.hide()


            

    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(1,True)
        self.parent.parent.setTabEnabled(0,True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(1)

