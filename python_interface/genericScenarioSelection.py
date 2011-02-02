# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from genericScenarioSelection_ui import Ui_Frame

class GenericScenarioSelection(QFrame):
    def __init__(self,nb_sc,label,next_widget,next_title,parent=None):
        super(GenericScenarioSelection,self).__init__(parent)
        self.parent=parent
        self.nb_sc = nb_sc
        self.next_widget = next_widget
        self.next_title = next_title
        self.checklist = []
        self.createWidgets()
        self.ui.selectionLabel.setText(label)

        if self.nb_sc > 2:
            self.putChoices()

        self.ui.verticalLayout.setAlignment(Qt.AlignHCenter)
        self.ui.verticalLayout.setAlignment(Qt.AlignTop)

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        # il y en a au minimum deux, ils sont donc déjà présents
        self.checklist.append(self.ui.sc1Check)
        self.checklist.append(self.ui.sc2Check)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)

    def validate(self):
        self.next_widget.setScenarios(self.getListSelectedScenarios())
        self.parent.parent.addTab(self.next_widget,self.next_title)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentWidget(self.next_widget)


    def getListSelectedScenarios(self):
        res = []
        for i,ch in enumerate(self.checklist):
            if ch.isChecked():
                res.append(i+1)
        return res

    def putChoices(self):
        for i in range(self.nb_sc-2):
            num = i+3
            check = QCheckBox("Scenario %s"%num,self)
            self.checklist.append(check)
            self.ui.verticalLayout.addWidget(check)

            

    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(1,True)
        self.parent.parent.setTabEnabled(0,True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(1)

