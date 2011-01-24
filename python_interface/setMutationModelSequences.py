# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setMutationModelSequences_ui import Ui_Frame

class SetMutationModelSequences(QFrame):
    def __init__(self,parent=None):
        super(SetMutationModelSequences,self).__init__(parent)
        self.parent=parent
        self.createWidgets()

        if self.ui.jukesRadio.isChecked():
            self.showJukes()
        elif self.ui.haseRadio.isChecked() or self.ui.kimuraRadio.isChecked():
            self.showKimuraHase()
        elif self.ui.tamuraRadio.isChecked():
            self.showTamura()

        self.ui.verticalLayout_27.setAlignment(Qt.AlignTop)
        self.ui.verticalLayout_26.setAlignment(Qt.AlignTop)

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.tamuraRadio,SIGNAL("clicked()"),self.showTamura)
        QObject.connect(self.ui.jukesRadio,SIGNAL("clicked()"),self.showJukes)
        QObject.connect(self.ui.haseRadio,SIGNAL("clicked()"),self.showKimuraHase)
        QObject.connect(self.ui.kimuraRadio,SIGNAL("clicked()"),self.showKimuraHase)

    def showJukes(self):
        self.ui.firstFrame.show()
        self.ui.secondFrame.show()
        self.ui.thirdFrame.hide()
        self.ui.fourthFrame.hide()
        self.ui.fifthFrame.hide()
        self.ui.sixthFrame.hide()
    def showKimuraHase(self):
        self.ui.firstFrame.show()
        self.ui.secondFrame.show()
        self.ui.thirdFrame.show()
        self.ui.fourthFrame.show()
        self.ui.fifthFrame.hide()
        self.ui.sixthFrame.hide()
    def showTamura(self):
        self.ui.firstFrame.show()
        self.ui.secondFrame.show()
        self.ui.thirdFrame.show()
        self.ui.fourthFrame.show()
        self.ui.fifthFrame.show()
        self.ui.sixthFrame.show()

    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))



